// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   talisman.cpp - TTalisman module                     *
// *************************************************************************

#include "revenant.h"
#include "talisman.h"
#include "spellpane.h"

REGISTER_BUILDER(TTalisman)

TObjectClass TalismanClass("TALISMAN", OBJCLASS_TALISMAN, 0);

// Hard coded class stats
DEFSTAT(Talisman, Code,     CODE, 0, 0, 0, 255)  // Character code for spells

void TTalisman::RemoveFromInventory()
{
    if (stricmp(owner->GetTypeName(), "Spell Pouch") == 0 ||
        stricmp(owner->GetTypeName(), "SpellPouch") == 0)
        SpellPane.Update();

    TObjectInstance::RemoveFromInventory();
}