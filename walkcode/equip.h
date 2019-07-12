// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  equip.h  - FORSALEN Equipment pane                   *
// *************************************************************************

#ifndef _EQUIP_H
#define _EQUIP_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#ifndef _MULTI_H
#include "multi.h"
#endif

#define SECT_TOP    0
#define SECT_BOTTOM 1

#define NUM_SECTS   2

#include "button.h"

// ***********************************
// * TEquipPane - The Equipment pane *
// ***********************************

// Displays a portrait of the main character with the various weapons, armor, and adornments
// he is currently wearing, and allows user to interactively change these around.

_CLASSDEF(TEquipPane)
class TEquipPane : public TButtonPane
{

  // Function Members

  public:

  // Constructor & Destructor
    TEquipPane() : TButtonPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT) { equipdata = NULL; }
    ~TEquipPane() { if (equipdata) delete equipdata; }

  // Init, close, input handling
    virtual BOOL Initialize();
        // Initializes pane and prepares to load map resources
    virtual void MouseClick(int button, int x, int y);

  // Background drawing functions
    virtual void DrawBackground();
        // Draws the tile information to the background
    virtual void Animate(BOOL draw);
        // Animate the pane
    void DrawAnim(PTObjectInstance inst, PTBitmap bm);

    int GetHeldSlot() { return heldslot; }
        // For moving objects between panes

    void Scroll(int amount);

  private:
    int OnSlot(int x, int y);
        // Returns the eq slot number that pixel position x, y is located in

    PTMulti equipdata;

    PTBitmap top;                           // background for pane
    PTBitmap bottom;                        // bottom section
    int section;                            // which section they are in

    int grabslot;                           // dragging items with the mouse
    int heldslot;
};

#endif

