#ifndef WRAMENTRYEDITOR_H
#define WRAMENTRYEDITOR_H

#include <QWidget>
#include <wrammap.h>

namespace Ui {
class WRamEntryEditor;
}

class WRamEntryEditor : public QWidget
{
    Q_OBJECT

public:
    explicit WRamEntryEditor(QWidget *parent = nullptr);
    void     setWRamEntry(WRAMMap::MapEntry* entry);
    ~WRamEntryEditor();

private slots:

    void on_commentPlainTextEdit_textChanged();

    void on_typeComboBox_currentIndexChanged(int index);

    void on_addressLineEdit_editingFinished();

    void on_nameLineEdit_editingFinished();

signals:
    void    entryChanged();
private:
    Ui::WRamEntryEditor *ui;
    WRAMMap::MapEntry*  m_entry;
    SNESType::DataType  m_currentType;
    void                setUiSize();
};

#endif // WRAMENTRYEDITOR_H
