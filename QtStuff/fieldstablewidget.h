#ifndef FIELDSTABLEWIDGET_H
#define FIELDSTABLEWIDGET_H

#include <QTableWidget>
#include <QWidget>
#include <snestype.h>

class FieldsTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    FieldsTableWidget(QWidget* parent = nullptr);
    void setFields(QVector<SNESType::SNESField>* fields);
    void    addField(SNESType::SNESField field);

signals:
    void    fieldsChanged();

private:
    QVector<SNESType::SNESField>*   m_fields;
    void    onCellChanged(int row, int column);


    // QWidget interface
    void addComboBox(int row, SNESType::FieldType type);
protected:
    void dropEvent(QDropEvent *event);
};

#endif // FIELDSTABLEWIDGET_H
