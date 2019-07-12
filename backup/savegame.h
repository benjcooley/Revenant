// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 SaveGame.h - SaveGame header file                     *
// *************************************************************************

#ifndef _SAVEGAME_H
#define _SAVEGAME_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

#ifndef _GRAPHICS_H
#include "graphics.h"
#endif

_CLASSDEF(TSaveGame)
class TSaveGame
{
  public:
    TSaveGame() { saved = NULL; }

    BOOL WriteGame(char *name = NULL);
    BOOL WriteGame(int);
        // Write savegame to disk
    BOOL ReadGame(char *name = NULL);
    BOOL ReadGame(int);
        // Read savegame from disk
    BOOL IsLoading() {return loading;}
        // Checks to see if the game is loading

  protected:
    PTObjectInstance saved;             // Saved object (Locke)
    int gametime;                       // Game time when saved
    int version;                        // Version of savegame
    int pane;                           // Which pane was up
    BOOL loading;
};

#endif