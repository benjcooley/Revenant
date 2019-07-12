// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     weapon.cpp - TWeapon module                       *
// *************************************************************************

#include "revenant.h"
#include "weapon.h"
#include "mappane.h"
#include "textbar.h"

REGISTER_BUILDER(TWeapon)
TObjectClass WeaponClass("WEAPON", OBJCLASS_WEAPON, 0);

DEFSTAT(Weapon, EqSlot,             EQSL, 0, 3, 0, 9)
DEFSTAT(Weapon, Type,               TYPE, 1, WT_HAND, WT_HAND, WT_LAST)
DEFSTAT(Weapon, Damage,             DMG,  2, 0, 0, 10000)
DEFSTAT(Weapon, Combining,          COMB, 3, 0, 0, 16)
DEFSTAT(Weapon, Poison,             PSN,  4, 0, 0, 32)
DEFSTAT(Weapon, Value,              VAL,  5, 0, 0, 1000000)

#define VIAL_STATE_FULL     0
#define VIAL_STATE_EMPTY    1

void TWeapon::ClearWeapon()
{
}

BOOL TWeapon::Use(PTObjectInstance user, int with)
{
    PTObjectInstance inst = MapPane.GetInstance(with);

    if (inst && strcmp(inst->GetName(), "Poison Vial") == 0 &&
        inst->GetState() == VIAL_STATE_FULL && Type() == WT_KNIFE)
    {
        inst->SetState(VIAL_STATE_EMPTY);
        TextBar.Print("%s envenomed.", GetName());

        return TRUE;
    }

    return FALSE;
}

int TWeapon::CursorType(PTObjectInstance with)
{
    if (with && strcmp(with->GetName(), "Poison Vial") == 0 &&
        with->GetState() == VIAL_STATE_FULL && Type() == WT_KNIFE)
        return CURSOR_HAND;

    return CURSOR_NONE;
}

void TWeapon::Load(RTInputStream is, int version, int objversion)
{
    TObjectInstance::Load(is, version, objversion);

    is >> poison;
}

void TWeapon::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);

    os << poison;
}

