#pragma once
#include "disasmhandler.h"
#include "definitionhandler.h"

class FancyStringyfier
{
public:
    FancyStringyfier(DefinitionHandler& defh);
    void fancier(DisasmInstruction& instr);
private:
    DefinitionHandler& definitions;
};

