// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      armor.cpp - TArmor module                        *
// *************************************************************************

#include "revenant.h"
#include "armor.h"
#include "object.h"
#include "mappane.h"
#include "player.h"

REGISTER_BUILDER(TArmor)

TObjectClass ArmorClass("ARMOR", OBJCLASS_ARMOR, 0);

DEFSTAT(Armor, EqSlot,          EQSL, 0, 4, 0, 8)
DEFSTAT(Armor, Value,           VAL,  1, 0, 0, 1000000)
DEFSTAT(Armor, Protection,      PROT, 2, 1, 1, 10)
DEFSTAT(Armor, Combining,       COMB, 3, 0, 0, 16)
DEFSTAT(Armor, ResistPoison,    RPSN, 4, 0, -100, 100)
DEFSTAT(Armor, Stealth,         STLH, 5, 0, -100, 100)
