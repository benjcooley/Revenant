// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   statpane.h - FORSAKEN Stat Pane                     *
// *************************************************************************

#ifndef _STATPANE_H
#define _STATPANE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "button.h"
#include "player.h"

// *************
// * TStatPane *
// *************

// The stat pane is a display of the main characters abilities/skills/stats.

_CLASSDEF(TStatPane)
class TStatPane : public TButtonPane
{
  public:
    TStatPane() : TButtonPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT) {}
    ~TStatPane() {}

    virtual BOOL Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void MouseClick(int button, int x, int y);
    virtual void MouseMove(int button, int x, int y);

    void Scroll(int numlines);
    void ExpandAll() { SetAllExpanded(TRUE); }
    void ContractAll() { SetAllExpanded(FALSE); }

    PTButton ScrollUpButton() { return Button(2); }
    PTButton ScrollDownButton() { return Button(3); }

  protected:
    void SetAllExpanded(BOOL value);
    int OnSlot(int x, int y);

    int startline;                      // for scrolling
    BOOL expanded[NUM_SKILLS];          // whether each level is expanded
};

#endif

