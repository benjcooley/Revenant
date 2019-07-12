// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        tool.h - TTool module                          *
// *************************************************************************

#ifndef _TOOL_H
#define _TOOL_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TTool)
class TTool : public TObjectInstance
{
  public:
    TTool(PTObjectImagery newim) : TObjectInstance(newim) {}
    TTool(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}

    virtual BOOL Use(PTObjectInstance user, int with = -1);
    virtual int CursorType(PTObjectInstance with = NULL);

    // Tool stats
    STATFUNC(Value)
    STATFUNC(Pick)
};

DEFINE_BUILDER("TOOL", TTool)

#endif