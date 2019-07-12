// *************************************************************************
// *                           Cinematix EXILE                             *
// *                    Copyright (C) 1996 Cinematix                       *
// *                rangedweapon.h - TRangedWeapon module                  *
// *************************************************************************

#ifndef _RANGEDWEAPON_H
#define _RANGEDWEAPON_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TRangedWeapon)
class TRangedWeapon : public TObjectInstance
{
  public:
    TRangedWeapon(PTObjectImagery newim) : TObjectInstance(newim) {}
    TRangedWeapon(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

    // Ranged weapon stats
    STATFUNC(EqSlot)
    STATFUNC(Value)

};

DEFINE_BUILDER("RANGEDWEAPON", TRangedWeapon)

#endif
