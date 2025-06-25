#include "wrammapeditor.h"
#include "ui_mapentrieseditor.h"

#include <QFile>
#include <QFileDialog>
#include <QJsonDocument>

WRamMapEditor::WRamMapEditor(QWidget *parent) :
    MapEntriesEditor(parent)
{
    listEntry.append(createFakeEntry(snesAddress(0), 0x10000));
        //qDebug() << "First unknow type is " <<listEntry.at(0).entry->type;
    listEntry.append(createFakeEntry(snesAddress(0x10000), 0x10000));

    //setNWClient(new EmuNWAccessClient(this));
    for (auto& entry : listEntry)
    {
        addUiEntryItem(entry.entry);
    }
    //loadFile("D:/Project/sneshacking/SoE/rammap.json");
    //WRAMDatas = QByteArray(0x20000, '\0');
    //ui->mapEntryDisplay->setWramData(&WRAMDatas);
    postConstructor();
}

void WRamMapEditor::refreshedData()
{
    /*ui->wramEntryDisplay->setWramData(&WRAMDatas);
    ListEntry &entry = listEntry[m_currentRow];
    ui->wramEntryDisplay->setWRamMapEntry(entry.entry);*/
    ;
}



WRamMapEditor::~WRamMapEditor()
{
    ;
}

void WRamMapEditor::onEntryChanged()
{
    ListEntry& currentEntry = listEntry[m_currentRow];
    unsigned int csize = currentEntry.entry->calculatedSize();
    Dictionnary* currentWRAMEntry = currentEntry.entry;
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
            Dictionnary* prevEntry = listEntry.at(m_currentRow - 1).entry;
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
            auto nEntry = createFakeEntry(snesAddress(currentWRAMEntry->address + csize), next_address - currentWRAMEntry->address + csize);
            listEntry.insert(m_currentRow + 1, nEntry);
            addUiEntryItem(nEntry.entry, m_currentRow + 1);
        }
        //if (currentEntry.entry->caculatedSize())
    }
    if (listEntry[m_currentRow].real == true)
    {
        dh->updateWramEntry(*listEntry.at(m_currentRow).entry);
        entryListModel->item(m_currentRow, 0)->setText(UiAddressItemText(currentWRAMEntry));
    }
    entryListModel->item(m_currentRow, 1)->setText(currentWRAMEntry->name);
}


void WRamMapEditor::saveFile()
{
    dh->saveWRamMap();
}

void    WRamMapEditor::myLoadFile()
{
    dh->loadWRAMMap(m_currentFile);
    unsigned int i = 0;
    for (auto &entry : dh->wramMap().entries)
    {
        ListEntry e;
        e.real = true;
        //e.text = entry.name;
        e.entry = new Dictionnary();
        e.address = entry.address;
        *e.entry = entry;
        if (listEntry.size() > 2)
        {
            auto& prevEntry = dh->wramMap().entries.at(i - 1);
            if (prevEntry.address + prevEntry.calculatedSize() - entry.address > 0)
            {
                listEntry.append(createFakeEntry(snesAddress(prevEntry.address + 1), entry.address - prevEntry.address - 1));
            }
        }
        if (i == 0 && entry.address > 0)
        {
            listEntry.append(createFakeEntry(snesAddress(0), e.entry->address));
        }
        listEntry.append(e);
        i++;
    }
    if (listEntry.last().address + listEntry.last().entry->calculatedSize() < 0x1FFFF)
    {
        listEntry.append(createFakeEntry(snesAddress(listEntry.last().address + listEntry.last().entry->calculatedSize()), 0x20000 - listEntry.last().entry->address - listEntry.last().entry->calculatedSize()));
    }
/*    for (auto& entry :  listEntry)
    {
        addUiEntryItem(entry.entry);
    }
    ui->mapEntryTableView->horizontalHeader()->setStretchLastSection(true);
    ui->mapEntryTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);*/
}


WRamMapEditor::ListEntry WRamMapEditor::createFakeEntry(snesAddress address, unsigned int size)
{
    ListEntry toret;
    toret.real = false;
    toret.address = address;
    toret.entry = new Dictionnary();
    toret.entry->address = snesAddress(address);
    toret.entry->size = size;
    toret.entry->type = DataType::Unknow;
    toret.entry->name = fakeEntryText(address, size);
    return toret;
}

QString    WRamMapEditor::fakeEntryText(snesAddress address, unsigned int size)
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

