// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       armor.h - TArmor module                         *
// *************************************************************************

#ifndef _ARMOR_H
#define _ARMOR_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "object.h"

_CLASSDEF(TArmor)
class TArmor : public TObjectInstance
{
  public:
    TArmor(PTObjectImagery newim) : TObjectInstance(newim) {}
    TArmor(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

    // Armor stats
    STATFUNC(EqSlot)
    STATFUNC(Value)
    STATFUNC(Protection)
    STATFUNC(Combining)
    STATFUNC(ResistPoison)
    STATFUNC(Stealth)
};

DEFINE_BUILDER("ARMOR", TArmor)

#endif