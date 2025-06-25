#ifndef MAPENTRYEDITOR_H
#define MAPENTRYEDITOR_H

#include <QWidget>
#include <wrammap.h>

namespace Ui {
class MapEntryEditor;
}

class MapEntryEditor : public QWidget
{
    Q_OBJECT

public:
    explicit MapEntryEditor(QWidget *parent = nullptr);
    void     setMapEntry(Dictionnary* entry);
    void     setToRegisterEditor();
    ~MapEntryEditor();

private slots:

    void on_commentPlainTextEdit_textChanged();

    void on_typeComboBox_currentIndexChanged(int index);

    void on_addressLineEdit_editingFinished();

    void on_nameLineEdit_editingFinished();

    void on_canonLineEdit_editingFinished();

signals:
    void    entryChanged();

private:
    Ui::MapEntryEditor *ui;
    Dictionnary*        m_entry;
    SNESType::DataType  m_currentType;
    void                setUiSize();
    bool                isRegister = false;
};

#endif // MAPENTRYEDITOR_H
