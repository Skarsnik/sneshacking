#include "wramentryeditor.h"
#include "ui_wramentryeditor.h"

WRamEntryEditor::WRamEntryEditor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WRamEntryEditor)
{
    m_entry = nullptr;
    ui->setupUi(this);
    // Auto connected fail for that Oo
    connect(ui->nameLineEdit, &QLineEdit::editingFinished, this, &WRamEntryEditor::on_nameLineEdit_editingFinished);
    blockSignals(true);
    ui->arrayTypeComboBox->setEnabled(false);
    ui->arrayTypeLabel->setEnabled(false);
    ui->typeComboBox->addItem("Data", QVariant::fromValue(SNESType::DataType::Data));
    ui->typeComboBox->addItem("Byte", QVariant::fromValue(SNESType::DataType::Byte));
    ui->typeComboBox->addItem("Word", QVariant::fromValue(SNESType::DataType::Word));
    ui->typeComboBox->addItem("Word Pointer", QVariant::fromValue(SNESType::DataType::WordPtr));
    ui->typeComboBox->addItem("Long", QVariant::fromValue(SNESType::DataType::Long));
    ui->typeComboBox->addItem("Long Pointer", QVariant::fromValue(SNESType::DataType::LongPtr));
    ui->typeComboBox->addItem("Array", QVariant::fromValue(SNESType::DataType::Array));
    ui->typeComboBox->addItem("Struct", QVariant::fromValue(SNESType::DataType::Struct));
    ui->typeComboBox->addItem("Linked List", QVariant::fromValue(SNESType::DataType::LinkedList));
    ui->typeComboBox->addItem("Raw GFX", QVariant::fromValue(SNESType::DataType::RawGFX));
    ui->typeComboBox->addItem("Compressed GFX", QVariant::fromValue(SNESType::DataType::CompressedGFX));
    ui->typeComboBox->addItem("SPC Data", QVariant::fromValue(SNESType::DataType::SPCData));

    ui->arrayTypeComboBox->addItem("Data", QVariant::fromValue(SNESType::DataType::Data));
    ui->arrayTypeComboBox->addItem("Struct", QVariant::fromValue(SNESType::DataType::Struct));
    ui->arrayTypeComboBox->addItem("Byte", QVariant::fromValue(SNESType::DataType::Byte));
    ui->arrayTypeComboBox->addItem("Word", QVariant::fromValue(SNESType::DataType::Word));
    ui->arrayTypeComboBox->addItem("Word Pointer", QVariant::fromValue(SNESType::DataType::WordPtr));
    ui->arrayTypeComboBox->addItem("Long", QVariant::fromValue(SNESType::DataType::Long));
    ui->arrayTypeComboBox->addItem("Long Pointer", QVariant::fromValue(SNESType::DataType::LongPtr));
    ui->arrayTypeComboBox->addItem("Word Pointer", QVariant::fromValue(SNESType::DataType::WordPtr));
    blockSignals(false);

    static const QRegularExpression hexInput("^\\$?([0-9]|[a-f]|[A-F]){1,5}");
    QRegularExpressionValidator* regexValidator = new QRegularExpressionValidator(hexInput);
    ui->addressLineEdit->setValidator(regexValidator);
    connect(ui->addressLineEdit, &QLineEdit::textEdited, this, [=](QString text){
        ui->addressLineEdit->setText(text.toUpper());
    });
}

void WRamEntryEditor::setWRamEntry(WRAMMap::MapEntry *entry)
{
    blockSignals(true);
    m_entry = entry;
    int index = ui->typeComboBox->findData(QVariant::fromValue(m_entry->type));
    ui->addressLineEdit->setText(QString::number(entry->address, 16).toUpper());
    ui->typeComboBox->setCurrentIndex(index);
    ui->nameLineEdit->setText(m_entry->name);
    if (entry->type == DataType::Unknow)
        ui->sizeSpinBox->setEnabled(false);
    else
        ui->sizeSpinBox->setEnabled(true);
    setUiSize();
    ui->commentPlainTextEdit->setPlainText(m_entry->comment);
    blockSignals(false);
}

WRamEntryEditor::~WRamEntryEditor()
{
    delete ui;
}



void WRamEntryEditor::on_commentPlainTextEdit_textChanged()
{
    if (m_entry == nullptr)
        return;
    m_entry->comment = ui->commentPlainTextEdit->toPlainText();
    emit entryChanged();
}


void WRamEntryEditor::on_typeComboBox_currentIndexChanged(int index)
{
    if (m_entry == nullptr)
        return;
    SNESType::DataType type = ui->typeComboBox->itemData(index).value<SNESType::DataType>();
    m_entry->type = type;
    m_currentType = type;
    ui->structEditor->setEnabled(false);
    ui->sizeSpinBox->setEnabled(true);
    ui->arrayTypeComboBox->setEnabled(false);
    ui->arrayTypeLabel->setEnabled(false);
    switch (type)
    {
    case SNESType::DataType::Byte:
    case SNESType::DataType::Word:
    case SNESType::DataType::WordPtr:
    case SNESType::DataType::Long:
    case SNESType::DataType::LongPtr:
    {
        ui->sizeSpinBox->setEnabled(false);
        break;
    }
    case SNESType::DataType::Array:
    {
        ui->arrayTypeComboBox->setEnabled(true);
        ui->arrayTypeLabel->setEnabled(true);
        break;
    }
    case SNESType::DataType::Struct:
    {
        ui->structEditor->setEnabled(true);
        ui->sizeSpinBox->setEnabled(false);
    }
    default:
    {
        ui->sizeSpinBox->setEnabled(true);
        ui->arrayTypeComboBox->setEnabled(false);
    }

    }
    setUiSize();
    emit entryChanged();
}


void WRamEntryEditor::on_addressLineEdit_editingFinished()
{
    qDebug() << "Address changed";
    if (m_entry == nullptr)
        return;
    bool    ok;
    m_entry->address = ui->addressLineEdit->text().toInt(&ok, 16);
    emit entryChanged();
}


void WRamEntryEditor::on_nameLineEdit_editingFinished()
{
    qDebug() << "Name edited";
    if (m_entry == nullptr)
        return;
    m_entry->name = ui->nameLineEdit->text();
    emit entryChanged();
}

void WRamEntryEditor::setUiSize()
{
    ui->sizeSpinBox->setValue(m_entry->calculatedSize());
    ui->hexSizeLabel->setText(QString("0x%1").arg(m_entry->calculatedSize(), 0, 16, QChar(' ')));
}

