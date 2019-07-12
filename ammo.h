// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        ammo.h - TAmmo module                          *
// *************************************************************************

#ifndef _AMMO_H
#define _AMMO_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "object.h"
#include "imagery.h"

#define AT_MISC     0
#define AT_ARROW    1
#define AT_BOLT     2
#define AT_HAND     3

#define MAXAMMOTYPES    4
#define MAXAMMOIMAGE    32

_CLASSDEF(TAmmo)
class TAmmo : public TObjectInstance
{
  public:
    TAmmo(PTObjectImagery newim) : TObjectInstance(newim) {}
    TAmmo(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) {}
    virtual ~TAmmo();

    static BOOL Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    static void AllocInvItem(PTObjectImagery img, int state, int type, int count);
        // Allocate a new inventory item for the given count and return it
    static void FreeInvItem(int type, int count);
        // Free up use of an instance of this count
    static void AllocGroundItem(PTObjectImagery img, int state, int type, int count);
        // Allocate a new ground item for the given count and return it
    static void FreeGroundItem(int type, int count);
        // Free up use of an instance of this count

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

    // Ammo stats
    STATFUNC(EqSlot)
    STATFUNC(Value)
    STATFUNC(Type)
    OBJSTAT(Amount)
    virtual int Amount() { return GetObjStat(se_Amount.id); }
    virtual void SetAmount(int amt);  // We redefine this in Ammo.cpp

};

DEFINE_BUILDER("AMMO", TAmmo)

#endif