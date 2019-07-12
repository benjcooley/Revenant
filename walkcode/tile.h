// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        tile.h - TTile module                          *
// *************************************************************************

#ifndef _TILE_H
#define _TILE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

_CLASSDEF(TTile)
class TTile : public TObjectInstance
{
  public:
    TTile(PTObjectImagery newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE; }
    TTile(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE; }

    // Tile stats
    STATFUNC(Code)
    STATFUNC(Extra)
    STATFUNC(Supertile)
    STATFUNC(Width)
    STATFUNC(Height)
};

DEFINE_BUILDER("TILE", TTile)

#endif