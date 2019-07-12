// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       ammo.cpp - TAmmo module                         *
// *************************************************************************

#include "revenant.h"
#include "ammo.h"
#include "bitmap.h"
#include "imagery.h"
#include "mappane.h"
#include "inventory.h"
#include "character.h"
#include "sound.h"

REGISTER_BUILDER(TAmmo)
TObjectClass AmmoClass("AMMO", OBJCLASS_AMMO, 0);

// Hard coded class stats
DEFSTAT(Ammo, EqSlot,       EQSL, 0, 8, 0, 10)
DEFSTAT(Ammo, Value,        VAL,  1, 0, 0, 1000000)
DEFSTAT(Ammo, Type,         TYPE, 2, 1, 0, 4)

// Hard coded object stats
DEFOBJSTAT(Ammo, Amount,    AMT,  0, 0, 0, 1000)

// These were originally static members of TAmmo but the compiler wasn't
// very hip on that, so they are now here.
PTBitmap ammoinvitem[MAXAMMOTYPES][MAXAMMOIMAGE];   // Bitmaps built on the fly
int ammoinvusecount[MAXAMMOTYPES][MAXAMMOIMAGE];        // Use count for each image

PTBitmap ammogrounditem[MAXAMMOTYPES][MAXAMMOIMAGE];    // Ground images
int ammogroundusecount[MAXAMMOTYPES][MAXAMMOIMAGE]; // Use count for ground

#define IMGAMOUNT(x)    ((x) < MAXAMMOIMAGE ? (x) : ((x) - (MAXAMMOIMAGE/2)) % (MAXAMMOIMAGE/2) + (MAXAMMOIMAGE/2))

struct { int num, x, y; } AmmoPos[MAXAMMOIMAGE] =
{ { 0, 4, 4 }, { 0, 5, 7 }, { 0, 1, 5 },
  { 0, 4, 6 }, { 0, 3, 2 }, { 0, 8, 5 },
  { 0, 3, 0 }, { 0, 8, 8 }, { 0, 5, 2 },
  { 0, 7, 8 }, { 0, 6, 4 }, { 0, 2, 6 },
  { 0, 6, 3 }, { 0, 2, 0 }, { 0, 1, 5 },
  { 0, 5, 1 },
  { 0, 4, 4 }, { 0, 5, 7 }, { 0, 1, 5 },
  { 0, 4, 6 }, { 0, 3, 2 }, { 0, 8, 5 },
  { 0, 3, 0 }, { 0, 8, 8 }, { 0, 5, 2 },
  { 0, 7, 8 }, { 0, 6, 4 }, { 0, 2, 6 },
  { 0, 6, 3 }, { 0, 2, 0 }, { 0, 1, 5 },
  { 0, 5, 1 }
};

BOOL TAmmo::Initialize()
{
    for (int t = 0; t < MAXAMMOTYPES; t++)
        for (int i = 0; i < MAXAMMOIMAGE; i++)
        {
            ammoinvitem[t][i] = ammogrounditem[t][i] = NULL;
            ammoinvusecount[t][i] = ammogroundusecount[t][i] = 0;
        }

    return TRUE;
}

void TAmmo::Close()
{
    for (int t = 0; t < MAXAMMOTYPES; t++)
        for (int i = 0; i < MAXAMMOIMAGE; i++)
        {
            if (ammoinvusecount[t][i])
            {
                ammoinvusecount[t][i] = 1;
                FreeInvItem(t, i);
            }

            if (ammogroundusecount[t][i])
            {
                ammogroundusecount[t][i] = 1;
                FreeGroundItem(t, i);
            }
        }
}

TAmmo::~TAmmo()
{
    int count = IMGAMOUNT(Amount());

    if (ammoinvusecount[objtype][count] && GetOwner() == Inventory.GetContainer())
        FreeInvItem(objtype, count);

    if (ammogroundusecount[objtype][count])
        FreeGroundItem(objtype, count);
}

void TAmmo::SignalAddedToInventory()
{
    TObjectInstance::SignalAddedToInventory();

    if (GetOwner() == Inventory.GetContainer() && imagery)
        AllocInvItem(imagery, state, objtype, Amount());
}

void TAmmo::RemoveFromInventory()
{
    TObjectInstance::RemoveFromInventory();

    if (GetOwner() == Inventory.GetContainer())
        FreeInvItem(objtype, Amount());
}

void TAmmo::SetAmount(int amt)
{
    if (Amount() == amt || amt < 1)
        return;

    if (GetOwner() == Inventory.GetContainer())
    {
        FreeInvItem(objtype, Amount());
        if (imagery)
            AllocInvItem(imagery, state, objtype, amt);
    }

    if (!GetOwner())
    {
        FreeGroundItem(objtype, Amount());
        if (imagery)
            AllocGroundItem(imagery, state, objtype, amt);
    }

    SetObjStat(se_Amount.id, amt);
}


void TAmmo::Load(RTInputStream is, int version, int objversion)
{
    TObjectInstance::Load(is, version, objversion);

    if (version < 5)
    {
        int amount;
        is >> amount;
        amount++;
        SetStat("Amount", amount);
    }
}

void TAmmo::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
}

void TAmmo::AllocInvItem(PTObjectImagery img, int state, int type, int count)
{
    if (!img)
        return;

    count = min(count, MAXAMMOIMAGE-1);

    if (ammoinvusecount[type][count] < 1)
    {
        ammoinvitem[type][count] = TBitmap::NewBitmap(INVITEMREALWIDTH, INVITEMREALHEIGHT, BM_15BIT);

        memset(ammoinvitem[type][count]->data16, 0, INVITEMREALWIDTH*INVITEMREALHEIGHT*2);

        for (int i = 0; i < count; i++)
            ammoinvitem[type][count]->Put(AmmoPos[i].x, AmmoPos[i].y, img->GetInvImage(state, AmmoPos[i].num), DM_TRANSPARENT);

        ammoinvusecount[type][count] = 1;
    }
    else
        ammoinvusecount[type][count]++;
}

void TAmmo::FreeInvItem(int type, int count)
{
    count = min(count, MAXAMMOIMAGE-1);

    if (ammoinvusecount[type][count] == 0)
        return;

    if (--(ammoinvusecount[type][count]) < 1 && ammoinvitem[count])
    {
        delete ammoinvitem[type][count];
        ammoinvitem[type][count] = NULL;
    }
}

void TAmmo::AllocGroundItem(PTObjectImagery img, int state, int type, int count)
{
    if (!img)
        return;

    count = min(count, MAXAMMOIMAGE-1);

    if (ammogroundusecount[type][count] < 1)
    {
        ammogrounditem[type][count] = TBitmap::NewBitmap(INVITEMREALWIDTH/2, INVITEMREALHEIGHT/2, BM_8BIT | BM_PALETTE);

        memset(ammogrounditem[type][count]->data16, 0, (INVITEMREALWIDTH/2)*(INVITEMREALHEIGHT/2));
        memcpy(ammogrounditem[type][count]->palette.ptr(), img->GetStillImage(state)->palette.ptr(), img->GetStillImage(state)->palettesize);

        for (int i = 0; i < count+1; i++)
            ammogrounditem[type][count]->Put(AmmoPos[i].x >> 1, AmmoPos[i].y >> 1, img->GetStillImage(state, AmmoPos[i].num), DM_TRANSPARENT);

        ammogroundusecount[type][count] = 1;
    }
    else
        ammogroundusecount[type][count]++;
}

void TAmmo::FreeGroundItem(int type, int count)
{
    count = min(count, MAXAMMOIMAGE-1);

    if (ammogroundusecount[type][count] == 0)
        return;

    if (--(ammogroundusecount[type][count]) < 1 && ammogrounditem[count])
    {
        delete ammogrounditem[type][count];
        ammogrounditem[type][count] = NULL;
    }
}

void TAmmo::DrawInvItem(int x, int y)
{
    int count = IMGAMOUNT(Amount());

    if (ammoinvusecount[objtype][count] < 1 && imagery)
        AllocInvItem(imagery, state, objtype, count);

    imagery->DrawInvItem(this, x, y);
}

PTBitmap TAmmo::InventoryImage()
{
    return ammoinvitem[objtype][IMGAMOUNT(Amount())];
}

void TAmmo::GetScreenRect(SRect &r)
{
    if (imagery)
    {
        PSImageryStateHeader st = imagery->GetState(GetState());

        int x, y;
        WorldToScreen(pos, x, y);

        r.left   = x - st->regx;
        r.right  = r.left + (INVITEMREALWIDTH / 2) - 1;
        r.top    = y - st->regy;
        r.bottom = r.top + (INVITEMREALHEIGHT / 2) - 1;
    }
}

void TAmmo::DrawUnlit(PTSurface surface)
{
    int count = IMGAMOUNT(Amount());

    if (ammogroundusecount[objtype][count] < 1 && imagery)
        AllocGroundItem(imagery, state, objtype, count);

    imagery->DrawUnlit(this, surface);
}

PTBitmap TAmmo::GetStillImage(int ostate)
{
    return ammogrounditem[objtype][IMGAMOUNT(Amount())];
}


// ************
// * TArrow3D *
// ************

_CLASSDEF(TArrow3D)
class TArrow3D : public TObjectInstance
{
  public:
    TArrow3D(PTObjectImagery newim) : TObjectInstance(newim) { killwait = -1; }
    TArrow3D(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { killwait = -1; }

    virtual DWORD Move();
    virtual void Pulse();

    int killwait;
};

DEFINE_BUILDER("Arrow3D", TArrow3D)
REGISTER_BUILDER(TArrow3D)

DWORD TArrow3D::Move()
{
    DWORD bits = TObjectInstance::Move();

    PTObjectInstance inst = (PTObjectInstance)TCharacter::CharBlocking(this, pos);

    if (killwait < 0 && ((bits & MOVE_BLOCKED) || inst))
    {
        // collision!

        PLAY("arrow impact");       // play sound

        if (inst)
        {
            // hit character
            inst->Damage(random(20, 50), DAMAGE_PIERCING);
        }

        killwait = FRAMERATE * 3;
    }

    return bits;
}

void TArrow3D::Pulse()
{
    if (killwait > 0)
        killwait--;
    
    if (killwait == 0)
        SetFlags(OF_KILL);
}
