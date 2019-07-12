// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  container.cpp - TContainer module                    *
// *************************************************************************

#include "revenant.h"
#include "container.h"
#include "mappane.h"
#include "inventory.h"
#include "textbar.h"
#include "tool.h"
#include "player.h"
#include "key.h"
#include "sound.h"
#include "spellpane.h"

REGISTER_BUILDER(TContainer)
TObjectClass ContainerClass("CONTAINER", OBJCLASS_CONTAINER, 0);

// Hard coded class stats
DEFSTAT(Container, Openable,    OPEN, 0, 0, 0, 1)
DEFSTAT(Container, Value,       VAL,  1, 0, 0, 1000000)

// Hard coded object stats
DEFOBJSTAT(Container, Locked,           LOCK, 0, 0, 0, 0)
DEFOBJSTAT(Container, KeyId,            KEY,  1, 0, 0, 0)
DEFOBJSTAT(Container, PickDifficulty,   PICK, 2, 0, 0, 0)

extern PTObjectInstance TakenObject;
extern PTObjectInstance DroppedObject;

// Container states
#define CLOSED      0
#define OPEN        1

BOOL TContainer::Use(PTObjectInstance user, int with)
{
    TObjectInstance::Use(user, with);

    PTObjectInstance inst = MapPane.GetInstance(with);

    if (state == CLOSED && CheckKeyUse(user, inst))
        return TRUE;

    if (Locked())
    {
        TextBar.Print("It seems to be locked.");
        return FALSE;
    }

    char buf[80];

    if (Openable() && state == CLOSED)
    {
        if (inst)
        {
            sprintf(buf, "The %s is closed.", GetName());
            TextBar.Print(buf);
            return FALSE;
        }

        SetState(OPEN);
        
        sprintf(buf, "%s opened.", GetName());
        TextBar.Print(buf);
        return TRUE;
    }

    if (GetTopOwner() == Inventory.GetTopContainer())
    {
        // it's in inventory
        if (!inst)
            Inventory.SetContainer(this);
        else if (NumObjects() < (MAXINVITEMS - 1))
        {
            if ((DWORD)FindFreeInventorySlot() < MAXINVITEMS)
            {
                inst->RemoveFromInventory();
                AddToInventory(inst);
            }
        }
    }
    else
    {
        // it's on the ground
        if (!inst)
        {
            // get from
            TInventoryIterator i(this);
            PTObjectInstance oi = i.Item();

            if (oi)
            {
                if (Inventory.GetContainer() && (DWORD)Inventory.GetContainer()->FindFreeInventorySlot() < MAXINVITEMS)
                {
                    oi->RemoveFromInventory();
                    Inventory.GetContainer()->AddToInventory(oi);

                    sprintf(buf, "%s taken from %s.", oi->GetName(), GetName());
                    TextBar.Print(buf);

                    TakenObject = oi;
                }
                else
                    TextBar.Print("Can't carry any more.");
            }
            else
            {
                if (!Openable())
                    return FALSE;

                SetState(CLOSED);
                sprintf(buf, "%s closed.", GetName());
                TextBar.Print(buf);
            }
        }
        else
        {
            // add to
            inst->RemoveFromInventory();
            AddToInventory(inst);

            sprintf(buf, "%s put in %s.", inst->GetName(), GetName());
            TextBar.Print(buf);

            DroppedObject = inst;
        }

    }

    return TRUE;
}

int TContainer::CursorType(PTObjectInstance inst)
{
    if (Openable() || NumObjects() > 0)
        return CURSOR_HAND;

    return CURSOR_NONE;
}

int TContainer::NumObjects()
{
    return RealNumInventoryItems();
}

void TContainer::Load(RTInputStream is, int version, int objversion)
{
    TObjectInstance::Load(is, version, objversion);

    if (version >= 2 && version < 5) // We're using object stats now
    {
        int contflags, pickdifficulty;
        is >> contflags >> pickdifficulty;
        SetStat("Locked", contflags != 0);
        SetStat("PickDifficulty", pickdifficulty);
    }

}

void TContainer::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
}

BOOL TContainer::CheckKeyUse(PTObjectInstance user, PTObjectInstance inst)
{
    if (!inst || !Locked())
        return FALSE;

    // check for the key unlock
    if (inst->ObjClass() == OBJCLASS_KEY)
    {
        if (((PTKey)inst)->KeyId() == KeyId())
        {   
            PLAY("unlock succeed");
            TextBar.Print("Unlocked.");
            SetLocked(FALSE);
            return TRUE;
        }
        else
        {
            PLAY("unlock failed");
            TextBar.Print("This is the wrong key.");
            return TRUE;
        }
    }

    // check for a lockpick attempt
    if (inst->ObjClass() == OBJCLASS_TOOL)
    {
        int abil = ((PTTool)inst)->Pick();
        if (abil > 0)
        {
            if (user->ObjClass() == OBJCLASS_PLAYER)
                abil += ((PTPlayer)user)->Agil() + ((PTPlayer)user)->Skill(SK_LOCKPICK);

            if (abil < PickDifficulty())
            {
                PLAY("unlock fail");
                TextBar.Print("The lock is too difficult to pick.");
            }
            else if (random(0, abil) < PickDifficulty())
            {
                PLAY("unlock fail");
                TextBar.Print("You fail to pick the lock.");
            }
            else
            {
                PLAY("unlock succeed");
                TextBar.Print("The lock quietly yields to your skills.");
                SetLocked(FALSE);
            }

            return TRUE;
        }
    }

    return FALSE;
}


// *************
// * TVialRack *
// *************

_CLASSDEF(TVialRack)
class TVialRack : public TContainer
{
  public:
    TVialRack(PTObjectImagery newim) : TContainer(newim) {}
    TVialRack(PSObjectDef def, PTObjectImagery newim) : TContainer(def, newim) {}

    virtual BOOL Use(PTObjectInstance user, int with = -1);
    virtual int CursorType(PTObjectInstance inst = NULL);
    virtual void Save(RTOutputStream os);
};

DEFINE_BUILDER("VIAL RACK", TVialRack)
REGISTER_BUILDER(TVialRack)

BOOL TVialRack::Use(PTObjectInstance user, int with)
{
    PTObjectInstance inst = MapPane.GetInstance(with);

    if (!inst)
    {
        if (state > 0)
        {
            if ((DWORD)Inventory.GetContainer()->FindFreeInventorySlot() >= MAXINVITEMS)
                TextBar.Print("Can't carry any more.");
            else
            {
                // generate a new vial
                SObjectDef def;
                memset(&def, 0, sizeof(SObjectDef));
                def.objclass = OBJCLASS_CONTAINER;
                def.objtype = ContainerClass.FindObjType("Empty Vial");
                GetPos(def.pos);
                int index = MapPane.NewObject(&def);

                TakenObject = MapPane.GetInstance(index);
                Inventory.GetContainer()->AddToInventory(TakenObject);

                TextBar.Print("Poison Vial taken from Vial Rack.");
                SetState(state - 1);

                return TRUE;
            }
        }
    }
    else
    {
        if (strcmp(inst->GetName(), "Poison Empty Vial") == 0 && state < 4)
        {
            // Originally I just deleted the vial here, but this caused problems
            // in shop scripts.  Now I just stash it into the rack, which clears itself
            // out when it saves to disk.

            //MapPane.RemoveObject(with);
            //delete inst;

            inst->RemoveFromInventory();
            AddToInventory(inst);

            TextBar.Print("Poison Vial put in Vial Rack.");
            SetState(state + 1);

            DroppedObject = inst;

            return TRUE;
        }
    }

    return FALSE;
}

int TVialRack::CursorType(PTObjectInstance inst)
{
    if (state > 0 && (!inst || (state < 4 && strcmp(inst->GetName(), "Poison Vial") == 0)))
        return CURSOR_HAND;

    return CURSOR_NONE;
}

void TVialRack::Save(RTOutputStream os)
{
    inventory.DeleteAll();

    TContainer::Save(os);
}


BOOL TContainer::AddToInventory(PTObjectInstance inst, int slot)
{
    BOOL ret = TObjectInstance::AddToInventory(inst, slot);
    if (stricmp(GetTypeName(), "Spell Pouch") == 0 || stricmp(GetTypeName(), "SpellPouch") == 0)
        SpellPane.Update();

    return ret;
}