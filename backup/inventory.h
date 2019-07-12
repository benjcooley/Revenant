// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   Inventory.h - TInventory Object                     *
// *************************************************************************

#ifndef _INVENTORY_H
#define _INVENTORY_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

// *******************************
// * TInventory - Inventory pane *
// *******************************

// Inventory for Our Hero which allows him to fool around with his possessions.

_CLASSDEF(TInventory)
class TInventory : public TPane
{
  public:
    TInventory() : TPane(INVENTORYPANEX, INVENTORYPANEY, INVENTORYPANEWIDTH, INVENTORYPANEHEIGHT) { container = NULL; }
    ~TInventory() {}

    virtual BOOL Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void Animate(BOOL draw);
    virtual void Show() { TPane::Show(); Update(); }
    virtual void MouseClick(int button, int x, int y);
    virtual void MouseMove(int button, int x, int y);

    PTObjectInstance GetContainer() { return container; }
        // Container whose inventory contents are currently displayed
    PTObjectInstance GetTopContainer()
        { PTObjectInstance inst = container; while (inst->GetOwner()) inst = inst->GetOwner(); return inst; }
        // Master of inventory (usually Player)
    void SetContainer(PTObjectInstance cont)
        { if (container != cont) { container = cont; Update(); } }
        // Sets the object whose inventory the pane displays
    int GetHeldSlot() { return heldslot; }
        // Function for transfering objects to other panes (namely, the equipment pane)

    void DrawAnim(PTObjectInstance inst, PTBitmap bm);
        // Draw an animation for an inventory object

  private:
    int OnSlot(int x, int y);
        // Return slotnumber that pixel position x, y is on
    void SwapSlots(int oldslot, int newslot);
        // Swap the inventory objects in the slots specified

    PTObjectInstance container;         // Object inventory is shown for

    int grabslot;                       // slot number currently dragging with mouse
    int heldslot;                       // for passing objects to other panes
    BOOL isdragging;                    // whether object is in motion
    int startposx, startposy;           // dragging
};

#endif

