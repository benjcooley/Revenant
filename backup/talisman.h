// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   talisman.h - TTalisman module                       *
// *************************************************************************

#ifndef _TALISMAN_H
#define _TALISMAN_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TTalisman)
class TTalisman : public TObjectInstance
{
  public:
    TTalisman(PTObjectImagery newim) : TObjectInstance(newim) {}
    TTalisman(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

    virtual void RemoveFromInventory();
        // Remove this object from whatever inventory it is in

    // Talisman stats
    STATFUNC(Code)
};

DEFINE_BUILDER("TALISMAN", TTalisman)

#endif