#include "registereditor.h"
#include "ui_MapEntriesEditor.h"

RegisterEditor::RegisterEditor(QWidget* parent) : MapEntriesEditor(parent)
{
    ui->mapEntryDisplay->setVisible(false);
    setWindowTitle("SNES registers definition editor");
    ui->mapEntryEditor->setToRegisterEditor();
    postConstructor();
}

void RegisterEditor::myLoadFile()
{
    dh->loadRegisters();
    const RegisterDefinitions& regDef = dh->registers();
    for (const auto& entry : regDef.registers)
    {
        ListEntry e;
        e.entry = new Register();
        *(Register*)e.entry = entry;
        e.real = true;
        e.address = e.entry->address;
        listEntry.append(e);
    }
}

void RegisterEditor::saveFile()
{
    dh->saveRegisterDefinition();
}

void RegisterEditor::onEntryChanged()
{
    MapEntriesEditor::onEntryChanged();
    ListEntry& currentEntry = listEntry[m_currentRow];
    Register* currentRegEntry = (Register*)currentEntry.entry;
    dh->updateRegisterDefinition(*currentRegEntry);
}

Dictionnary *RegisterEditor::newEntryPointer()
{
    return new Register();
}
