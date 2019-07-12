// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      helper.h - THelper module                        *
// *************************************************************************

#ifndef _HELPER_H
#define _HELPER_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(THelper)
class THelper : public TObjectInstance
{
  public:
    THelper(PTObjectImagery newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE; }
    THelper(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE; }
};

DEFINE_BUILDER("HELPER", THelper)

#endif
