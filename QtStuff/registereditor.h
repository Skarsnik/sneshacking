#ifndef REGISTEREDITOR_H
#define REGISTEREDITOR_H

#include <QObject>
#include "mapentrieseditor.h"

class RegisterEditor : public MapEntriesEditor
{
public:
    RegisterEditor(QWidget* parent = nullptr);

private:
    virtual void    myLoadFile() override;
    virtual void    saveFile() override;
    virtual void    onEntryChanged() override;
    virtual Dictionnary* newEntryPointer() override;
};

#endif // REGISTEREDITOR_H
