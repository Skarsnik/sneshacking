#include "fieldstablewidget.h"

#include <QComboBox>
#include <QDropEvent>
#include <QDebug>

static const QMap<QString, SNESType::FieldType> mappingType = {
    {"None", SNESType::FieldType::None},
    {"Byte", SNESType::FieldType::Byte},
    {"Word", SNESType::FieldType::Word},
    {"Long Pointer", SNESType::FieldType::LongPtr},
    {"Word Pointer", SNESType::FieldType::WordPtr}
};


FieldsTableWidget::FieldsTableWidget(QWidget *parent) : QTableWidget(parent)
{
    setColumnCount(4);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setDragEnabled(true);
    setAcceptDrops(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
    setDragDropOverwriteMode(false);
    setDropIndicatorShown(true);
    QStringList l;
    l << "Name" << "Type" << "Size" << "Comment";
    setHorizontalHeaderLabels(l);
    connect(this, &QTableWidget::cellChanged, this, &FieldsTableWidget::onCellChanged);
}

void FieldsTableWidget::setFields(QVector<SNESType::SNESField> *fields)
{
    m_fields = fields;
}

void FieldsTableWidget::addField(SNESType::SNESField field)
{
    //m_fields->append(field);
    int row = rowCount();
    setRowCount(row + 1);
    qDebug() << "Row count : " << row;
    auto nameItem = new QTableWidgetItem(field.name);
    //nameItem->setFlags(Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    setItem(row, 0, nameItem);
    //f->setItem(row, 1, new QTableWidgetItem(FieldTypeToString(field.type)));
    setItem(row, 2, new QTableWidgetItem(QString::number(field.size)));
    setItem(row, 3, new QTableWidgetItem(field.comment));
    addComboBox(row, field.type);
}

void    FieldsTableWidget::addComboBox(int row, SNESType::FieldType type)
{
    QComboBox* typeCombo = new QComboBox();
    typeCombo->addItem("None", 0);
    typeCombo->addItem("Byte", 1);
    typeCombo->addItem("Word", 2);
    typeCombo->addItem("Long Pointer", 3);
    typeCombo->addItem("Word Pointer", 2);
    setCellWidget(row, 1, typeCombo);
    typeCombo->setCurrentText(mappingType.key(type));
    connect(typeCombo, &QComboBox::currentIndexChanged, this, [=]
    {
        cellChanged(row, 1);
    });
    setCellWidget(row, 1, typeCombo);
}

void FieldsTableWidget::onCellChanged(int row, int column)
{
    qDebug() << "Cell changed" << row << column;
    switch (column)
    {
    case 0:
    {
        (*m_fields)[row].name = item(row, column)->text();
        break;
    }
    case 1:
    {
        QComboBox* box = qobject_cast<QComboBox*>(cellWidget(row, column));
        qDebug() << box;
        (*m_fields)[row].type = mappingType[box->currentText()];
        bool ok;
        (*m_fields)[row].size = box->itemData(box->currentIndex(), Qt::UserRole).toInt(&ok);
        item(row, column + 1)->setText(QString::number((*m_fields)[row].size));
        break;
    }
    case 2:
    {
        (*m_fields)[row].size = item(row, column)->text().toInt();
        break;
    }
    case 3:
    {
        (*m_fields)[row].comment = item(row, column)->text();
        break;
    }
    }
    emit fieldsChanged();
}

void FieldsTableWidget::dropEvent(QDropEvent *event)
{
    if (event->dropAction() != Qt::DropAction::MoveAction)
        return;
    int sourceRow = currentRow();
    int destRow = itemAt(event->position().toPoint())->row();
    qDebug() << "Moving " << sourceRow << " to : " << destRow;
    SNESType::SNESField field = m_fields->at(sourceRow);
    m_fields->remove(sourceRow);
    m_fields->insert(destRow, field);
    //item(sourceRow, 0);
    blockSignals(true);
    //QTableWidget::dropEvent(event);
    auto swap_row = [=](int rowA, int rowB) {
        for (unsigned int i = 0; i < 4; i++)
        {
            if (i == 1)
            {
                continue;
            }
            QTableWidgetItem* src = takeItem(rowA, i);
            QTableWidgetItem* dst = takeItem(rowB, i);
            setItem(rowA, i, dst);
            setItem(rowB, i, src);
        }
        QComboBox* wSrc = qobject_cast<QComboBox*>(cellWidget(rowA, 1));
        QComboBox* wDst = qobject_cast<QComboBox*>(cellWidget(rowB, 1));
        addComboBox(rowA, mappingType[wDst->currentText()]);
        addComboBox(rowB, mappingType[wSrc->currentText()]);
        wSrc->deleteLater();
        wDst->deleteLater();
    };
    if (sourceRow > destRow)
    {
        for (unsigned int i = 0; i < sourceRow - destRow; i++)
        {
            qDebug() << "Swap " << sourceRow - i << " with " << sourceRow - i - 1;
            swap_row(sourceRow - i, sourceRow - i - 1);
        }
    }
    if (sourceRow < destRow)
    {
        for (unsigned int i = 0; i < destRow - sourceRow; i++)
        {
            qDebug() << "Swap " << sourceRow + i << " with " << sourceRow + i + 1;
            swap_row(sourceRow + i, sourceRow + i + 1);
        }
    }
    blockSignals(false);
    emit fieldsChanged();
}
