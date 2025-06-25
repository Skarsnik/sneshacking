#include "definitionhandler.h"

#include <QFile>
#include <QJsonDocument>

const QString DefinitionHandler::registerFilePath = "D:/Project/sneshacking/QtStuff/definitions/snesregisters.json";

DefinitionHandler::DefinitionHandler() {}



void DefinitionHandler::loadWRAMMap(QString map)
{
    QFile f(map);
    f.open(QIODevice::ReadOnly);
    QJsonDocument jDoc = QJsonDocument::fromJson(f.readAll());
    f.close();
    m_wramMap = WRAMMap::fromJSON(jDoc.object());
    m_wrammapPath = map;
}

const WRAMMap &DefinitionHandler::wramMap() const
{
    return m_wramMap;
}

const RegisterDefinitions &DefinitionHandler::registers() const
{
    return m_snesRegisters;
}

void DefinitionHandler::loadRegisters()
{
    QFile f(registerFilePath);
    f.open(QIODevice::ReadOnly);
    QJsonDocument jDoc = QJsonDocument::fromJson(f.readAll());
    f.close();
    m_snesRegisters = RegisterDefinitions::fromJSON(jDoc.object());
}

AddressLocation DefinitionHandler::whereIs(snesAddress addr) const
{
    if (0x2100 <= addr && addr <= 0x44FF)
        return AddressLocation::REGISTER;
    return AddressLocation::WRAM;
}

const Register &DefinitionHandler::snesRegister(snesAddress addr) const
{
    for (auto& entry : m_snesRegisters.registers)
    {
        if (entry.address == addr)
        {
            return entry;
        }
    }
    return Register();
}

const Dictionnary &DefinitionHandler::wramMapEntry(snesAddress addr) const
{
    for (auto& entry : m_wramMap.entries)
    {
        if (entry.address == addr)
        {
            return entry;
        }
    }
    return Dictionnary();
}

void DefinitionHandler::updateRegisterDefinition(const Register reg)
{
    for (auto& entry : m_snesRegisters.registers)
    {
        if (entry.address == reg.address)
        {
            qDebug() << "Snes register updated" << entry.address;
            entry = reg;
            emit somethingUpdated(AddressLocation::REGISTER, entry.address);
            return ;
        }
    }
    qDebug() << "Adding new register";
    m_snesRegisters.registers.append(reg);
    emit somethingAdded(AddressLocation::REGISTER, reg.address);
}

void DefinitionHandler::updateWramEntry(const Dictionnary dic)
{
    for (auto& entry : m_wramMap.entries)
    {
        if (entry.address == dic.address)
        {
            qDebug() << "Wram entry updated" << entry.address;
            entry = dic;
            emit somethingUpdated(AddressLocation::WRAM, entry.address);
            return ;
        }
    }
    m_snesRegisters.registers.append(dic);
    emit somethingAdded(AddressLocation::WRAM, dic.address);
}

void DefinitionHandler::removeWramEntry(const snesAddress addr)
{
    QMutableVectorIterator<Dictionnary> it(m_wramMap.entries);
    while (it.hasNext())
    {
        if (it.next().address == addr)
        {
            it.remove();
            emit somethingRemoved(AddressLocation::WRAM, addr);
            return ;
        }
    }
}

void DefinitionHandler::saveWRamMap(QString path)
{

}

void DefinitionHandler::saveRegisterDefinition()
{
    QFile f(registerFilePath);
    f.open(QIODevice::WriteOnly);
    auto jObj = m_snesRegisters.toJSON();
    QJsonDocument jDoc(jObj);
    f.write(jDoc.toJson());
    f.close();
}
