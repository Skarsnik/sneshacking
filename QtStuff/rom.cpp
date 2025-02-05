#include <QDebug>

#include "rom.h"
#include "rominfo.h"
#include "rommapping.h"


QByteArray Rom::datas(snesAddress address, quint32 length)
{
    quint32 romOffset = snesToPc(address);
    qFile->seek(romOffset);
    return qFile->read(length);
}

snesAddress Rom::pcToSnes(quint32 address)
{
    enum rom_type cType = (enum rom_type) mapping;
    int result = rommapping_pc_to_snes(address, cType, hasHeader);
    if (fastRom) // FIXME, this is for hirom
    {
        return snesAddress(result + 0x800000);
    }
    return snesAddress(result);
}

quint32 Rom::snesToPc(snesAddress address)
{
    return rommapping_snes_to_pc(address, (enum rom_type) mapping, hasHeader);
}

Rom Rom::openRomFile(const QString& path)
{
    Rom rom;
    qDebug() << path;
    rom.qFile = new QFile(path);
    QFile* f = rom.qFile;
    if (f->open(QIODevice::ReadOnly))
    {
        if (f->size() & 0x200)
        {
            qDebug() << "ROM has header";
            rom.hasHeader = true;
        } else {
            rom.hasHeader = false;
            qDebug() << "ROM has no header";
        }
        unsigned int header_offset = rom.hasHeader ? 0x200 : 0;
        f->seek(0x7FD5 + header_offset);
        QByteArray romMakeUp = f->read(1);
        QByteArray data;
        f->seek(0x7FC0 + header_offset);
        data = f->read(80);
        if (romMakeUp[0] & 0x1) // bit for HiROM set
        {
            //This is unconsistent, let's try HiROM
            f->seek(0xFFD5 + header_offset);
            romMakeUp = f->read(1);
            if (romMakeUp[0] & 0x1)
            {
                qDebug() << "ROM is HiROM";
                f->seek(0xFFC0 + header_offset);
                data = f->read(80);
            }
        }
        qDebug() << data.toHex(' ');
        const uint8_t* c_data = (const uint8_t*) data.constData();
        rom_infos* romInfo = get_rom_info(c_data);
        rom.rawTitle = romInfo->title;
        qDebug() << "ROM raw title" << rom.rawTitle;
        rom.name = QString(romInfo->title).trimmed();
        rom.mapping = RomMapping::HiROM;
        if (romInfo->type == LoROM)
            rom.mapping = RomMapping::LoROM;
        rom.fastRom = romInfo->fastrom;
        qDebug() << "ROM Size : " << romInfo->size;
        qDebug() << "SRAM Size : " << romInfo->sram_size;
        qDebug() << "Version : " << romInfo->version;
        qDebug() << "CheckSUM  : " << romInfo->checksum << " CheckSUM comp : " << romInfo->checksum_comp;
        qDebug() << "ncop : " << romInfo->native_cop << "nbrk : " << romInfo->native_brk;
        rom.headerInfos.nativeVectorCop = romInfo->native_cop;
        rom.headerInfos.nativeVectorBrk = romInfo->native_brk;
        rom.headerInfos.nativeVectorAbort = romInfo->native_abort;
        rom.headerInfos.nativeVectorNMI = romInfo->native_nmi;
        rom.headerInfos.nativeVectorReset = romInfo->native_reset;
        rom.headerInfos.nativeVectorIRQ = romInfo->native_irq;
        rom.headerInfos.emulationVectorCop = romInfo->emulation_cop;
        rom.headerInfos.emulationVectorAbort = romInfo->emulation_abort;
        rom.headerInfos.emulationVectorNMI = romInfo->emulation_nmi;
        rom.headerInfos.emulationVectorReset = romInfo->emulation_reset;
        rom.headerInfos.emulationVectorIRQ = romInfo->emulation_irq;

        f->seek(header_offset);
        unsigned int numberOfBanks = (f->size() - header_offset) / SNESBankSize;
        for (unsigned int i = 0; i < numberOfBanks; i++)
        {
            Bank bank;
            f->seek(i * SNESBankSize + header_offset);
            bank.data = f->read(SNESBankSize);
            bank.name = QString("Bank $%1").arg(i, 2, 16, QChar('0'));
            rom.banks.append(bank);
        }
    }
    return rom;
}
