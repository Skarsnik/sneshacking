#include "romviewer.h"
#include "structeditor.h"
#include "wramentryeditor.h"
#include "wrammapeditor.h"
#include "asm/asmtextview.h"
#include "asm/disasm.h"
#include <QDebug>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Rom rom = Rom::openRomFile("D:/Project/QUsb2snes/tests/usb2snes-tests/custom rom/usb2snes-testlorom.sfc");
    /*RomViewer w;
    w.show();
    w.setRom(Rom::openRomFile("D:/Emulation/Secret of Evermore/Secret of Evermore.smc"));*/

    /*StructEditor e;
    e.show();
    SNESType::SNESStruct sstruct;
    sstruct.name = "Unamed";
    sstruct.size = 8;
    SNESType::SNESField f1;
    f1.size = 1;
    f1.type = SNESType::FieldType::Byte;
    f1.name = "field 1";
    SNESType::SNESField f2;
    f2.size = 2;
    f2.type = SNESType::FieldType::Word;
    f2.name = "field 2";
    SNESType::SNESField f3;
    f3.size = 3;
    f3.type = SNESType::FieldType::LongPtr;
    f3.name = "field 3";

    sstruct.fields.append(f1);
    sstruct.fields.append(f2);
    sstruct.fields.append(f3);
    e.setStruct(sstruct);*/

    /*WRamEntryEditor editor;
    editor.show();*/
    /*WRamMapEditor mEditor;
    mEditor.show();*/

    quint32 pc = rom.headerInfos.emulationVectorReset;
    qDebug() << "NMI is " << pc;
    auto list_instruction = disassemble_str((const uint8_t*)rom.banks.at(0).data.mid(pc - 0x8000, 0x300).constData(), 0x300, &pc, true);
    QList<DisasmInstruction> instructions;
    while (list_instruction != NULL)
    {
        instructions.append(DisasmInstruction(list_instruction->instruction));
        list_instruction = list_instruction->next;
    }
    ASMTextView aView;

    aView.setAsm(instructions);
    aView.show();
    return a.exec();
}
