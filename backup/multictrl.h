// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                multictrl.h - FORSAKEN Multipane Control Panel         *
// *************************************************************************

#ifndef _MULTICTRL_H
#define _MULTICTRL_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _BUTTON_H
#include "button.h"
#endif

#ifndef _MULTI_H
#include "multi.h"
#endif

// Defines for multi control panel
#define NUMMCBUTTONS    4
#define BUTTONRADIUS    16

// **************
// * TMultiCtrl *
// **************

// The pane which contains the buttons which flip through the various multifunction panels.

_CLASSDEF(TMultiCtrlPane)
class TMultiCtrlPane : public TButtonPane
{

  // Function Members

  public:

  // Constructor & Destructor
    TMultiCtrlPane() : TButtonPane(MULTICTRLPANEX, MULTICTRLPANEY, MULTICTRLPANEWIDTH, MULTICTRLPANEHEIGHT) {}
    ~TMultiCtrlPane() {}

    virtual BOOL Initialize();
    virtual void Close();

    // These hide and show all the multifunction panes
    virtual void Hide();
    virtual void Show();

    int GetActivePane() { return curpane; }
        // Return the currently active pane
    void ActivatePane(int pane);
        // Activate one of the multipanes and hide the others
    void RedrawCurPane();
        // Redraw current pane
    void RedrawOverhangButtons() { Button(0)->SetDirty(); Button(3)->SetDirty(); }

// Data Members
  private:
    int curpane;                // pane currently showing
    PTMulti buttondata;         // bitmaps for buttons
};

#endif

