#ifndef WRAMMAPEDITOR_H
#define WRAMMAPEDITOR_H


#include <QStandardItemModel>
#include <QTimer>
#include <QWidget>
#include "mapentrieseditor.h"


class WRamMapEditor : public MapEntriesEditor
{
    Q_OBJECT

public:
    explicit WRamMapEditor(QWidget *parent = nullptr);

    ~WRamMapEditor();

private slots:
    void on_deletePushButton_clicked();

private:
    void                onEntryChanged();
    //QStandardItemModel* entryListModel;

    MapEntriesEditor::ListEntry    createFakeEntry(snesAddress address, unsigned int size);
    void                        myLoadFile();
    void                        saveFile();
    QString                     fakeEntryText(snesAddress address, unsigned int size);
    void                        refreshedData();
};

#endif // WRAMMAPEDITOR_H
