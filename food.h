// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        food.h - TFood module                          *
// *************************************************************************

#ifndef _FOOD_H
#define _FOOD_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TFood)
class TFood : public TObjectInstance
{
  public:
    TFood(PTObjectImagery newim) : TObjectInstance(newim) {}
    TFood(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

    virtual BOOL Use(PTObjectInstance user, int with = -1);
        // Munch munch munch
    virtual int CursorType(PTObjectInstance inst = NULL) { if (inst) return CURSOR_NONE; return CURSOR_MOUTH; }
        // Yummy

    // Food statistics
    STATFUNC(Value)
    STATFUNC(Health)
    STATFUNC(Mana)
    STATFUNC(Fatigue)
    STATFUNC(Poison)
    STATFUNC(Cure)
    STATFUNC(Fill)
};

DEFINE_BUILDER("FOOD", TFood)

extern TObjectClass FoodClass;

#endif