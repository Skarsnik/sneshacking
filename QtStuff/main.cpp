#include "asm/fancystringyfier.h"
#include "registereditor.h"
#include "romviewer.h"
#include "structeditor.h"
#include "mapentryeditor.h"
#include "wrammapeditor.h"
#include "asm/asmtextview.h"
#include "asm/disasm.h"
#include <QDebug>
#include <QApplication>
extern "C" const snes_asm_instruction_definition snes_asm_instuction_list[0x100];

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Rom rom = Rom::openRomFile("D:/Project/QUsb2snes/tests/usb2snes-tests/custom rom/usb2snes-testlorom.sfc");
    RomViewer w;
    w.show();
    w.setRom(rom);

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
    DefinitionHandler defh;
    defh.loadRegisters();
    RegisterEditor rEditor;
    rEditor.setDefinitionHandler(&defh);
    rEditor.loadFile("");
    rEditor.show();

    DisasmHandler dh;
    dh.rom = &rom;
    FancyStringyfier fs(defh);
    auto r = dh.disassembleRoutine(snesAddress(rom.headerInfos.emulationVectorReset));
    for (auto intr : r.instructions)
    {
        fs.fancier(*intr);
    }
    ASMTextView aView;
    QObject::connect(&defh, &DefinitionHandler::somethingAdded, [&] (AddressLocation loc, snesAddress addr) {
        qDebug() << "Something added " << addr;
        QString name;
        if (loc == AddressLocation::REGISTER)
        {
            name = defh.snesRegister(addr).name;
        }
        for (const auto &thingaccess : dh.addressAccess[addr])
        {
            auto instruction = thingaccess.instruction;
            qDebug() << "Entry used by " << instruction->offset << " " << instruction->basicStrinfigy;
            instruction->advancedStrinfigy = QString() + snes_asm_instuction_list[instruction->opcode].opcode + " " + name;
            qDebug() << instruction->advancedStrinfigy;
            aView.instructionChanged(instruction);
        }
    });
    QObject::connect(&defh, &DefinitionHandler::somethingUpdated, [&] (AddressLocation loc, snesAddress addr) {
        qDebug() << "Something updated " << addr;
        QString name;
        if (loc == AddressLocation::REGISTER)
        {
            name = defh.snesRegister(addr).name;
        }
        for (const auto &thingaccess : dh.addressAccess[addr])
        {
            auto instruction = thingaccess.instruction;
            qDebug() << "Entry used by " << instruction->offset << " " << instruction->basicStrinfigy;
            fs.fancier(*instruction);
            qDebug() << instruction->advancedStrinfigy;
            aView.instructionChanged(instruction);
        }
    });
    aView.setAsm(r);
    aView.setFixedSize(500, 600);
    aView.show();
    return a.exec();
}
