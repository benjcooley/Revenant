// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       food.cpp - TFood module                         *
// *************************************************************************

#include "revenant.h"
#include "food.h"

REGISTER_BUILDER(TFood)

TObjectClass FoodClass("FOOD", OBJCLASS_FOOD, 0);

// Food hard coded stats
DEFSTAT(Food, Value, VAL, 0, 0, 0, 1000000)

BOOL TFood::Use(PTObjectInstance user, int with)
{
    if (GetState() == 0)
    {
        SetState(1);
        return TRUE;
    }

    return FALSE;
}
