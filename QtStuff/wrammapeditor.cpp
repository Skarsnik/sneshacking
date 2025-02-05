#include "wrammapeditor.h"
#include "ui_wrammapeditor.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>

WRamMapEditor::WRamMapEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WRamMapEditor)
{
    ui->setupUi(this);
    m_currentRow = 0;
    entryListModel = new QStandardItemModel();
    ui->mapEntryTableView->setModel(entryListModel);
    listEntry.append(createFakeEntry(0, 0x10000));
    listEntry.append(createFakeEntry(0x10000, 0x10000));
    setNWClient(new EmuNWAccessClient(this));
    for (auto& entry : listEntry)
    {
        addUiEntryItem(entry.entry);
    }
    connect(ui->wramEntryEditor, &WRamEntryEditor::entryChanged, this, &WRamMapEditor::onEntryChanged);
    ui->mapEntryTableView->horizontalHeader()->setStretchLastSection(true);
    ui->mapEntryTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    loadFile("D:/Project/sneshacking/SoE/rammap.json");
    NWATimer.setInterval(500);
    WRAMDatas = QByteArray(0x20000, '\0');
    ui->wramEntryDisplay->setWramData(&WRAMDatas);
}

void WRamMapEditor::setNWClient(EmuNWAccessClient *client)
{
    NWAClient = client;
    connect(&NWATimer, &QTimer::timeout, this, [=] {
        if (NWAClient->isConnected())
        {
            qDebug() << "Read data";
            NWAClient->cmdCoreReadMemory("WRAM", 0, 0x20000);
        }
    });
    connect(NWAClient, &EmuNWAccessClient::readyRead, this, [=] {
        auto reply = NWAClient->readReply();
        qDebug() << reply.cmd;
        if (reply.cmd == "EMULATION_STATUS")
        {
            auto map = reply.toMap();
            if (map["state"] == "running" || map["state"] == "paused")
            {
                NWATimer.start();
            }
        }
        if (reply.cmd == "CORE_READ")
        {
            WRAMDatas = reply.binary;
            refreshedData();
        }
    });
    if (client->isConnected())
    {
        NWAClient->cmdEmulationStatus();
    }
    else
    {
        client->connectToHost("localhost", 0xBEEF);
        connect(client, &EmuNWAccessClient::connected, this, [=] {
            qDebug() << "NWA connected";
            NWAClient->cmdEmulationStatus();
        });
    }
}

void WRamMapEditor::refreshedData()
{
    ui->wramEntryDisplay->setWramData(&WRAMDatas);
    ListEntry &entry = listEntry[m_currentRow];
    ui->wramEntryDisplay->setWRamMapEntry(entry.entry);
}



WRamMapEditor::~WRamMapEditor()
{
    delete ui;
}

void WRamMapEditor::on_mapEntryTableView_clicked(const QModelIndex &index)
{
    qDebug() << "Current row changed" << index.row();
    if (index.isValid() == false)
        return ;
    ListEntry &entry = listEntry[index.row()];
    m_currentRow = index.row();
    ui->wramEntryEditor->setWRamEntry(entry.entry);
    ui->wramEntryEditor->setEnabled(true);
    ui->wramEntryDisplay->setWRamMapEntry(entry.entry);
}

void WRamMapEditor::onEntryChanged()
{
    ListEntry& currentEntry = listEntry[m_currentRow];
    unsigned int csize = currentEntry.entry->calculatedSize();
    WRAMMap::MapEntry* currentWRAMEntry = currentEntry.entry;
    qDebug() << "Entry changed";
    if (currentEntry.real == false && currentWRAMEntry->type != SNESType::DataType::Unknow)
    {
        qDebug() << "Entry is now not fake : " << currentWRAMEntry->address;
        currentEntry.real = true;
        bool newNextEntry = true;
        //bool newPrevEntry = false;
        int next_address = 0x20000;
        //int prev_address = 0;
        if (m_currentRow != 0)
        {
            WRAMMap::MapEntry* prevEntry = listEntry.at(m_currentRow - 1).entry;
            if (prevEntry->type == SNESType::DataType::Unknow)
                ;
        }
        // This update or create the next unknow
        if (listEntry.size() != m_currentRow + 1)
        {
            if (currentWRAMEntry->address + csize < listEntry.at(m_currentRow + 1).entry->address)
            {
                next_address = listEntry.at(m_currentRow + 1).entry->address;
            } else {
                newNextEntry = false;
            }
        }
        if (currentEntry.address < currentEntry.entry->address)
        {
            auto nEntry = createFakeEntry(currentEntry.address, currentEntry.entry->address - currentEntry.address);
            currentEntry.address = currentEntry.entry->address;
            listEntry.insert(m_currentRow, nEntry);
            addUiEntryItem(nEntry.entry, m_currentRow);
            currentEntry.address = currentEntry.entry->address;
            m_currentRow += 1;
        }
        if (newNextEntry)
        {
            auto nEntry = createFakeEntry(currentWRAMEntry->address + csize, next_address - currentWRAMEntry->address + csize);
            listEntry.insert(m_currentRow + 1, nEntry);
            addUiEntryItem(nEntry.entry, m_currentRow + 1);
        }
        //if (currentEntry.entry->caculatedSize())
    }
    if (listEntry[m_currentRow].real == true)
    {
        entryListModel->item(m_currentRow, 0)->setText(UiAddressItemText(currentWRAMEntry));
    }
    entryListModel->item(m_currentRow, 1)->setText(currentWRAMEntry->name);
}


void WRamMapEditor::on_savePushButton_clicked()
{
    if (m_currentFile.isEmpty())
    {
        m_currentFile = QFileDialog::getSaveFileName(this);
        if (m_currentFile.isEmpty())
            return ;
    }
    WRAMMap map;
    for (auto& entry : listEntry)
    {
        if (entry.real)
        {
            map.entries.append(*entry.entry);
        }
    }
    QJsonObject jObj = map.toJSON();
    QJsonDocument jdoc(jObj);
    QFile f(m_currentFile);
    f.open(QIODevice::WriteOnly);
    f.write(jdoc.toJson());
    f.close();
}


void WRamMapEditor::on_loadPushButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this);
    if (file.isEmpty())
        return ;
    loadFile(file);
}

void    WRamMapEditor::loadFile(QString file)
{
    entryListModel->clear();
    listEntry.clear(); // FIXME, need to clear memory
    m_currentFile = file;
    QFile f(file);
    f.open(QIODevice::ReadOnly);
    QJsonDocument jDoc = QJsonDocument::fromJson(f.readAll());
    f.close();
    WRAMMap map = WRAMMap::fromJSON(jDoc.object());
    unsigned int i = 0;
    for (auto &entry : map.entries)
    {
        ListEntry e;
        e.real = true;
        //e.text = entry.name;
        e.entry = new WRAMMap::MapEntry();
        e.address = entry.address;
        *e.entry = entry;
        if (listEntry.size() > 2)
        {
            auto& prevEntry = map.entries.at(i - 1);
            if (prevEntry.address + prevEntry.calculatedSize() - entry.address > 0)
            {
                listEntry.append(createFakeEntry(prevEntry.address + 1, entry.address - prevEntry.address - 1));
            }
        }
        if (i == 0 && entry.address > 0)
        {
            listEntry.append(createFakeEntry(0, e.entry->address));
        }
        listEntry.append(e);
        i++;
    }
    if (listEntry.last().address + listEntry.last().entry->calculatedSize() < 0x1FFFF)
    {
        listEntry.append(createFakeEntry(listEntry.last().address + listEntry.last().entry->calculatedSize(), 0x20000 - listEntry.last().entry->address - listEntry.last().entry->calculatedSize()));
    }
    for (auto& entry :  listEntry)
    {
        addUiEntryItem(entry.entry);
    }
    ui->mapEntryTableView->horizontalHeader()->setStretchLastSection(true);
    ui->mapEntryTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
}

void WRamMapEditor::addUiEntryItem(WRAMMap::MapEntry *entry, int row)
{
    auto newAddItem = new QStandardItem(UiAddressItemText(entry));
    auto newNameItem = new QStandardItem(entry->name);
    QList<QStandardItem*> li;
    li << newAddItem << newNameItem;
    if (row == -1)
    {
        entryListModel->appendRow(li);
    } else {
        entryListModel->insertRow(row, li);
    }
}

QString WRamMapEditor::UiAddressItemText(WRAMMap::MapEntry *entry)
{
    return QString("$%1").arg(entry->address, 0, 16, QChar(' ')).toUpper();
}

WRamMapEditor::ListEntry WRamMapEditor::createFakeEntry(unsigned int address, unsigned int size)
{
    ListEntry toret;
    toret.real = false;
    toret.address = address;
    toret.entry = new WRAMMap::MapEntry();
    toret.entry->address = address;
    toret.entry->size = size;
    toret.entry->type = DataType::Unknow;
    toret.entry->name = fakeEntryText(address, size);
    return toret;
}

QString    WRamMapEditor::fakeEntryText(unsigned int address, unsigned int size)
{
    return "Unknow " + QString("$%1-$%2").arg(address, 5, 16, QChar('0')).arg(address + size - 1, 5, 16, QChar('0')).toUpper();
}

void WRamMapEditor::on_deletePushButton_clicked()
{
    const auto& entry = listEntry.at(m_currentRow);
    if (entry.real == false)
        return ;
    auto& prevEntry = listEntry[m_currentRow - 1];
    if (prevEntry.real == false)
    {
        prevEntry.entry->size += entry.entry->calculatedSize();
        prevEntry.entry->name = fakeEntryText(prevEntry.entry->address, prevEntry.entry->size);
        entryListModel->item(m_currentRow - 1, 1)->setText(prevEntry.entry->name);
    }
    if (listEntry.size() != m_currentRow + 1)
    {
        auto& nextEntry = listEntry[m_currentRow + 1];
        if (prevEntry.real)
        {
            nextEntry.address = entry.address;
            nextEntry.entry->address = entry.address;
            nextEntry.entry->size += entry.entry->calculatedSize();
            nextEntry.entry->name = fakeEntryText(nextEntry.entry->address, nextEntry.entry->size);
            entryListModel->item(m_currentRow + 1, 1)->setText(nextEntry.entry->name);
            entryListModel->item(m_currentRow + 1, 0)->setText(UiAddressItemText(nextEntry.entry));
        }
    }
    entryListModel->removeRows(m_currentRow, 1);
    listEntry.remove(m_currentRow);
}

