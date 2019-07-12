// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                         key.h - TKey module                           *
// *************************************************************************

#ifndef _KEY_H
#define _KEY_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TKey)
class TKey : public TObjectInstance
{
  public:
    TKey(PTObjectImagery newim) : TObjectInstance(newim) {}
    TKey(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

    // Key stats
    STATFUNC(Value)
    OBJSTATFUNC(KeyId)
};

DEFINE_BUILDER("KEY", TKey)

#endif