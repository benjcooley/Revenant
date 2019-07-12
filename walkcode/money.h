// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       money.h - TMoney module                         *
// *************************************************************************

#ifndef _MONEY_H
#define _MONEY_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

#define MAXMONEYTYPES   1
#define MAXMONEYIMAGE   64

_CLASSDEF(TMoney)
class TMoney : public TObjectInstance
{
  public:
    TMoney(PTObjectImagery newim) : TObjectInstance(newim) { }
    TMoney(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}
    virtual ~TMoney();

    static BOOL Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    static void AllocInvItem(PTBitmap inv, int type, int count);
        // Allocate a new inventory item for the given count and return it
    static void FreeInvItem(int type, int count);
        // Free up use of an instance of this count
    static void AllocGroundItem(PTBitmap ground, int type, int count);
        // Allocate a new ground item for the given count and return it
    static void FreeGroundItem(int type, int count);
        // Free up use of an instance of this count

    virtual BOOL Use(PTObjectInstance user, int with = -1);
        // Combine money
    virtual int CursorType(PTObjectInstance with = NULL);
        // Returns type of cursor that should appear when mouse arrow is over the object

    virtual void Load(RTInputStream is, int version, int objversion);
        // Loads data from the sector
    virtual void Save(RTOutputStream os);
        // Saves data to the sector

    virtual void SignalAddedToInventory();
        // Called to signal object that it was added to a new inventory
    virtual void RemoveFromInventory();
        // Remove this object from whatever inventory it is in

    virtual void DrawInvItem(int x, int y);
        // Returns bitmap for the inventory image
    virtual PTBitmap InventoryImage();
        // Returns bitmap for the inventory image

    virtual void GetScreenRect(SRect &r);
        // Get screen bounding rectangle for object (in world coordinates)
    virtual void DrawUnlit(PTSurface surface);
        // Returns bitmap for the inventory image
    virtual PTBitmap GetStillImage(int ostate = -1);
        // Returns still image

    // Money stats
    STATFUNC(Value)
    OBJSTAT(Amount)
    virtual int Amount() { return GetObjStat(se_Amount.id); }
    virtual void SetAmount(int amt);  // We redefine this in Money.cpp

};

DEFINE_BUILDER("MONEY", TMoney)

#endif
