// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   talisman.cpp - TTalisman module                     *
// *************************************************************************

#include "revenant.h"
#include "talisman.h"
#include "spell.h"

REGISTER_BUILDER(TTalisman)

TObjectClass TalismanClass("TALISMAN", OBJCLASS_TALISMAN, 0);

DEFSTAT(Talisman, Value,        VAL,  0, 0, 0, 1000000)
DEFSTAT(Talisman, Order,        ORDR, 1, 0, -10, 10)
DEFSTAT(Talisman, Power,        POWR, 2, 0, -10, 10)
DEFSTAT(Talisman, Earth,        ERTH, 3, 0, -10, 10)
DEFSTAT(Talisman, Fire,         FIRE, 4, 0, -10, 10)
DEFSTAT(Talisman, Lightning,    LITN, 5, 0, -10, 10)
DEFSTAT(Talisman, Ice,          ICE,  6, 0, -10, 10)
DEFSTAT(Talisman, Destruction,  DEST, 7, 0, -10, 10)
DEFSTAT(Talisman, Healing,      HEAL, 8, 0, 0, 1)

void TTalisman::AddToInventory(PTObjectInstance oi)
{
    TObjectInstance::AddToInventory(oi);

    if (strcmp(oi->GetTypeName(), "Spell Pouch") == 0)
        SpellPane.Update();
}

