#ifndef WRAMMAPEDITOR_H
#define WRAMMAPEDITOR_H


#include <QStandardItemModel>
#include <QTimer>
#include <QWidget>
#include "wrammap.h"
#include <emunwaccessclient.h>


namespace Ui {
class WRamMapEditor;
}

class WRamMapEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WRamMapEditor(QWidget *parent = nullptr);
    void    setNWClient(EmuNWAccessClient* client);

    ~WRamMapEditor();

private slots:

    void on_savePushButton_clicked();

    void on_loadPushButton_clicked();

    void on_mapEntryTableView_clicked(const QModelIndex &index);

    void on_deletePushButton_clicked();

private:
    struct  ListEntry {
        //QString             text;
        bool                real;
        WRAMMap::MapEntry*  entry;
        unsigned int        address;
    };
    void                onEntryChanged();
    QList<ListEntry>    listEntry;
    int                 m_currentRow;
    Ui::WRamMapEditor   *ui;
    WRAMMap             wrammap;
    QString             m_currentFile;
    QStandardItemModel* entryListModel;
    EmuNWAccessClient*  NWAClient;
    QTimer              NWATimer;
    QByteArray          WRAMDatas;

    WRamMapEditor::ListEntry    createFakeEntry(unsigned int address, unsigned int size);
    void                        loadFile(QString file);
    QString                     UiAddressItemText(WRAMMap::MapEntry *entry);
    void                        addUiEntryItem(WRAMMap::MapEntry *entry, int row = -1);
    QString                     fakeEntryText(unsigned int address, unsigned int size);
    void                        refreshedData();
};

#endif // WRAMMAPEDITOR_H
