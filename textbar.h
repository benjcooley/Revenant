// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      textbar.h - Text Bar Pane                        *
// *************************************************************************

#ifndef _TEXTBAR_H
#define _TEXTBAR_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

// This is the lame little text bar that goes just under the map
// map and tells the player completely useless information like,
// "The door is now open" or "Got the Hammer of Wounding" in an
// attempt to make them feel like complete idiots.
// It also now does something kind of useful - displaying the name
// and health of the creature Locke is fighting when there is nothing
// else to print.

_CLASSDEF(TTextBar)
class TTextBar : public TPane
{
  public:
    TTextBar() : TPane(TEXTBARX, TEXTBARY, TEXTBARWIDTH, TEXTBARHEIGHT) { }

    virtual BOOL Initialize();
    virtual void Close();

    virtual void DrawBackground();

    void Print(char *txt, ...); // printf style output!!!
    void Clear() { text[0] = 0; SetDirty(TRUE); }

    // Opponent health display functions
    void SetHealthDisplay(char *n, int l);
    void ClearHealthDisplay();

    // Sets the target and current levels
    void SetLevels(int newlevel, int newtargetlevel);

  protected:
    char text[80];                  // text to display

    char name[80];                  // name of monster's health to display
    int level;                      // current health level
    int targetlevel;                // target health level
    BOOL animating;                 // whether last frame was drawn to background
    BOOL pulsecheck;                // must be set every tick to keep health bar up
};

#endif
