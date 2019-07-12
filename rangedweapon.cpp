// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               rangedweapon.cpp - TRangedWeapon module                 *
// *************************************************************************

#include "revenant.h"
#include "rangedweapon.h"

REGISTER_BUILDER(TRangedWeapon)
TObjectClass RangedWeaponClass("RANGEDWEAPON", OBJCLASS_RANGEDWEAPON, 0);

// Hard coded class stats
DEFSTAT(RangedWeapon, EqSlot,   EQSL, 0, 6, 0, 8)
DEFSTAT(RangedWeapon, Value,    VAL,  1, 0, 0, 1000000)


