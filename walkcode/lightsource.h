// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 lightsource.h - TLightSource module                   *
// *************************************************************************

#ifndef _LIGHTSOURCE_H
#define _LIGHTSOURCE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "object.h"

_CLASSDEF(TLightSource)
class TLightSource : public TObjectInstance
{
  public:
    TLightSource(PTObjectImagery newim) : TObjectInstance(newim) {}
    TLightSource(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

    // Lightsource stats
    STATFUNC(EqSlot)
    STATFUNC(Value)
};

DEFINE_BUILDER("LIGHTSOURCE", TLightSource)

#endif