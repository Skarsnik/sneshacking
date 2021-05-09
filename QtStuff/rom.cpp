#include <QFile>
#include <QDebug>

#include "rom.h"
#include "rominfo.h"


Rom Rom::openRomFile(const QString& path)
{
    Rom rom;
    qDebug() << path;
    QFile f(path);
    if (f.open(QIODevice::ReadOnly))
    {
        if (f.size() & 0x200)
        {
            qDebug() << "ROM has header";
            rom.hasHeader = true;
        } else {
            rom.hasHeader = false;
            qDebug() << "ROM has no header";
        }
        unsigned int header_offset = rom.hasHeader ? 0x200 : 0;
        f.seek(0x7FD5 + header_offset);
        QByteArray romMakeUp = f.read(1);
        QByteArray data;
        f.seek(0x7FC0 + header_offset);
        data = f.read(32);
        if (romMakeUp[0] & 0x1) // bit for HiROM set
        {
            //This is unconsistent, let's try HiROM
            f.seek(0xFFD5 + header_offset);
            romMakeUp = f.read(1);
            if (romMakeUp[0] & 0x1)
            {
                qDebug() << "ROM is HiROM";
                f.seek(0xFFC0 + header_offset);
                data = f.read(32);
            }
        }
        rom_infos* romInfo = get_rom_info(data.data());
        rom.rawTitle = romInfo->title;
        rom.name = QString(romInfo->title).trimmed();
        rom.type = RomType::HiROM;
        if (romInfo->type == LoROM)
            rom.type = RomType::LoROM;
        f.seek(header_offset);
        unsigned int numberOfBanks = (f.size() - header_offset) / SNESBankSize;
        for (unsigned int i = 0; i < numberOfBanks; i++)
        {
            Bank bank;
            f.seek(i * SNESBankSize + header_offset);
            bank.data = f.read(SNESBankSize);
            bank.name = QString("Bank $%1").arg(i, 2, 16, QChar('0'));
            rom.banks.append(bank);
        }

    }
    return rom;
}
