// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      shadow.h - TShadow module                        *
// *************************************************************************

#ifndef _SHADOW_H
#define _SHADOW_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TShadow)
class TShadow : public TObjectInstance
{
  public:
    TShadow(PTObjectImagery newim) : TObjectInstance(newim) {}
    TShadow(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}
};

DEFINE_BUILDER("SHADOW", TShadow)

#endif