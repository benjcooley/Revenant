// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        trap.h - TTrap module                          *
// *************************************************************************

#ifndef _TRAP_H
#define _TRAP_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TTrap)
class TTrap : public TObjectInstance
{
  public:
    TTrap(PTObjectImagery newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE; }
    TTrap(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE; }
};

DEFINE_BUILDER("TRAP", TTrap)

#endif