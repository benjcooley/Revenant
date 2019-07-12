// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 statusbar.h - Status bars (health, stamina)           *
// *************************************************************************

#ifndef _STATUSBAR_H
#define _STATUSBAR_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _MULTI_H
#include "multi.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

_CLASSDEF(TStatusBar)
class TStatusBar : public TPane
{
  public:
    TStatusBar(int x, int y, int w, int h) : TPane(x, y, w, h) { }

    virtual BOOL Initialize();
    virtual void Close();

    virtual void DrawBackground();

    virtual int GetHue() { return 0; }
        // Redefine this for any bar to return the color at the current level

    int GetLevel() { return level; }
    void SetLevel(int lev) { level = targetlevel = max(min(lev, 1000), 0); }
    void ChangeLevel(int tlev) { targetlevel = max(min(tlev, 1000), 0); }

  protected:
    PTMulti tubedata;

    int level;              // level of fluid (0-1000)
    int targetlevel;        // target level (animating towards)
    BOOL animating;         // whether it was drawn to the background last frame
};

_CLASSDEF(THealthBar)
class THealthBar : public TStatusBar
{
  public:
    THealthBar() : TStatusBar(HEALTHBARX, HEALTHBARY, HEALTHBARWIDTH, HEALTHBARHEIGHT) { }

    virtual BOOL Initialize();
    virtual int GetHue();
};

_CLASSDEF(TStaminaBar)
class TStaminaBar : public TStatusBar
{
  public:
    TStaminaBar() : TStatusBar(STAMINABARX, STAMINABARY, STAMINABARWIDTH, STAMINABARHEIGHT) { }

    virtual BOOL Initialize();
    virtual int GetHue();
};

#endif
