// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       food.cpp - TFood module                         *
// *************************************************************************

#include "revenant.h"
#include "character.h"
#include "statusbar.h"
#include "typeinfo.h"
#include "food.h"

REGISTER_BUILDER(TFood)

TObjectClass FoodClass("FOOD", OBJCLASS_FOOD, 0);

// Food hard coded stats
DEFSTAT(Food, Value, VAL, 0, 0, 0, 1000000)
DEFSTAT(Food, Health, HLTH, 1, 0, 0, 1000000)
DEFSTAT(Food, Mana, MANA, 2, 0, 0, 1000000)
DEFSTAT(Food, Fatigue, FATG, 3, 0, 0, 1000000)
DEFSTAT(Food, Poison, PSN, 4, 0, 0, 1000000)
DEFSTAT(Food, Cure, CURE, 5, 0, 0, 1000000)
DEFSTAT(Food, Fill, FILL, 5, 0, 0, 1000000)


BOOL TFood::Use(PTObjectInstance user, int with)
{
    if (GetState() == 0)
    {
        if(((PTCharacter)user)->IsCharacter())
        {
        // make the user healthier
            ((PTCharacter)user)->SetHealth(user->Health() + Health());

            if(((PTCharacter)user)->Health() > ((PTCharacter)user)->MaxHealth())
                ((PTCharacter)user)->SetHealth(((PTCharacter)user)->MaxHealth());

            if(((PTCharacter)user) == ((PTCharacter)Player))
                HealthBar.ChangeLevel(((PTCharacter)user)->Health() * 1000 / ((PTCharacter)user)->MaxHealth());

        // give the user more mana
            ((PTCharacter)user)->SetMana(user->Mana() + Mana());

            if(((PTCharacter)user)->Mana() > ((PTCharacter)user)->MaxMana())
                ((PTCharacter)user)->SetMana(((PTCharacter)user)->MaxMana());

            if(((PTCharacter)user) == ((PTCharacter)Player))
                StaminaBar.ChangeLevel(((PTCharacter)user)->Mana() * 1000 / ((PTCharacter)user)->MaxMana());
        }
        SetState(1);
        return TRUE;
    }

    return FALSE;
}
