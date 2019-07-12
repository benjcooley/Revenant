// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    death.h - TDeathPane module                        *
// *************************************************************************

#ifndef _DEATH_H
#define _DEATH_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _BUTTON_H
#include "button.h"
#endif

_CLASSDEF(TDeathPane)
class TDeathPane : public TButtonPane
{
  public:
    TDeathPane() : TButtonPane(FRAMEMAPPANEX, FRAMEMAPPANEY, FRAMEMAPPANEWIDTH, FRAMEMAPPANEHEIGHT) { }
    
    virtual BOOL Initialize();
    virtual void Close();
    virtual void DrawBackground();

  private:
    PTMulti deathdata;              // Background and buttons
};

#endif