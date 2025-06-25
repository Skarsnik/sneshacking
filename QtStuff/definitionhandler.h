#ifndef DEFINITIONHANDLER_H
#define DEFINITIONHANDLER_H

#include "registerdefinitions.h"
#include "wrammap.h"

#include <QStringLiteral>

class DefinitionHandler : public QObject
{
    Q_OBJECT
public:
    DefinitionHandler();
    void    loadWRAMMap(QString path);
    const WRAMMap&              wramMap() const;
    const RegisterDefinitions&  registers() const;
    void                        loadRegisters();
    AddressLocation             whereIs(snesAddress addr) const;
    const Register&             snesRegister(snesAddress addr) const;
    const Dictionnary&          wramMapEntry(snesAddress addr) const;

    void    updateRegisterDefinition(const Register reg);
    void    updateWramEntry(const Dictionnary dic);
    void    removeWramEntry(const snesAddress addr);
    void    saveWRamMap(QString path = "");
    void    saveRegisterDefinition();
    static const QString registerFilePath;

signals:
    void    somethingUpdated(AddressLocation where, snesAddress address);
    void    somethingAdded(AddressLocation where, snesAddress address);
    void    somethingRemoved(AddressLocation where, snesAddress);
private:
    RegisterDefinitions m_snesRegisters;
    WRAMMap             m_wramMap;
    QString             m_wrammapPath;
};

#endif // DEFINITIONHANDLER_H
