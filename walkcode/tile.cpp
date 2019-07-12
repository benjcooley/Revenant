// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       tile.cpp - TTile module                         *
// *************************************************************************

#include "revenant.h"
#include "tile.h"

REGISTER_BUILDER(TTile)

TObjectClass TileClass("TILE", OBJCLASS_TILE, 0);

DEFSTAT(Tile, Code,         CODE, 0, -1, 0, -1)
DEFSTAT(Tile, Extra,        EXTR, 1, -1, 0, -1)
DEFSTAT(Tile, Supertile,    SUPR, 2, 0, 0, 1)
DEFSTAT(Tile, Width,        WDTH, 3, 0, 0, 16)
DEFSTAT(Tile, Height,       HGHT, 4, 0, 0, 16)
