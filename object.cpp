// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     Object.cpp - TObject module                       *
// *************************************************************************

#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "revenant.h"

#include "3dscene.h"
#include "display.h"
#include "dls.h"
#include "mappane.h"
#include "object.h"
#include "stream.h"
#include "parse.h"
#include "playscreen.h"
#include "resource.h"
#include "script.h"
#include "inventory.h"
#include "player.h"
#include "file.h"
#include "command.h"
#include "sound.h"

// Declarations of global arrays for object classes, imagery, and builders

int TObjectClass::numclasses = 0;
PTObjectClass TObjectClass::classes[MAXOBJECTCLASSES];

// TRUE if classes need be be resaved
BOOL TObjectClass::classesdirty = FALSE;

int TObjectBuilder::numobjtypes = 0;
PTObjectBuilder TObjectBuilder::builders[MAXOBJECTTYPES];


// Tables for ConvertToFacing, Move, and other angle/distance related functions
extern BYTE AngleTable[256][256];
extern BYTE DistTable[256][256];
extern short DistX[256];
extern short DistY[256];

extern DWORD ImageryMemUsage;

WORD UniqueTypeID;

#define WORLDZOFFSET 0x3900 + 0x7FFF

#define MYSTERYVAL1     (867)
#define MYSTERYVAL2     (1000)

// *******************************
// * Map/Screen Conversion Utils *
// *******************************

void WorldToScreen(RS3DPoint pos, int &x, int &y)
{
    // 2:1
    x = pos.x - pos.y;
    y = ((pos.x + pos.y) / 2) - (int)((pos.z * MYSTERYVAL1) / MYSTERYVAL2);
        // the screen z is actually pos.z * cos(30), cos(30) = .866
}

void WorldToScreen(RS3DPoint pos, RS3DPoint spos)
{
    // 2:1
    spos.x = pos.x - pos.y;
    spos.y = ((pos.x + pos.y) / 2) - (int)((pos.z * MYSTERYVAL1) / MYSTERYVAL2);
    spos.z = (WORD)(WORLDZOFFSET - (int)(pos.z / 2) - (int)((pos.x + pos.y) * MYSTERYVAL1 / MYSTERYVAL2));
    //spos.z = (WORD)(WORLDZOFFSET - (int)(pos.z * 2560 / 5120) - (int)((pos.x + pos.y) * 4434 / 5120));
        // Z must be figured out based on the plates being tilted at a 30 degree angle
        // zratio = 512.0 * sin(30) = 256.0, yratio = 512.0 * cos(30) = 443.4
}

void WorldToScreenZ(RS3DPoint pos, int &z)
{
    z = (WORD)(WORLDZOFFSET - (int)(pos.z / 2) - (int)((pos.x + pos.y) * MYSTERYVAL1 / MYSTERYVAL2));
}

void ScreenToWorld(RS3DPoint pos, RS3DPoint spos)
{
    pos.z = (-3464 * spos.y / 1000 - 2 * spos.z) / 4;
    pos.y = - spos.x / 2 + MYSTERYVAL1 * pos.z / MYSTERYVAL2 + spos.y;
    pos.x = spos.x + pos.y;
}

void ScreenToWorld(int x, int y, RS3DPoint pos, int zheight)
{
    if (zheight)
        y += (int)((zheight * MYSTERYVAL1) / MYSTERYVAL2);

    // 2:1
    pos.x = (x / 2) + y;
    pos.y = y - (x / 2);
    pos.z = zheight;
}

void ConvertToVector(int angle, int speed, RS3DPoint vect, int zangle)
{
    if (zangle == 0)
    {
        vect.x = (DistX[angle] * speed) / 256;
        vect.y = (DistY[angle > 128 ? angle - 128 : 128 - angle] * speed) / 256;
        vect.z = 0;
    }
    else
    {
        ConvertToVector(angle, (DistX[zangle] * speed) / 256, vect, 0);
        vect.z = (DistY[zangle] * speed) / 256;
    }
}

int ConvertToFacing(RS3DPoint target)
{
    int absx = absval(target.x);
    int absy = absval(target.y);

    while (absx > 255 || absy > 255)
    {
        absx >>= 1;
        absy >>= 1;
    }

    int angle = 64 - AngleTable[absx][absy];

    // adjust for sign (which of the quadrants does the angle fall into?)
    if (target.x < 0 && target.y > 0)
        angle += 128;
    else if (target.x < 0)
        angle = 256 - angle;
    else if (target.y > 0)
        angle = 128 - angle;

    return angle;
}

int ConvertToFacing(RS3DPoint pos, RS3DPoint target)
{
    S3DPoint p;

    p.x = target.x - pos.x;
    p.y = target.y - pos.y;
    p.z = 0;

    return ConvertToFacing(p);
}

int ConvertZToFacing(RS3DPoint target)
{
    int absdist = abs(Distance(target));
    int absz = abs(target.z);

    while (absdist > 255 || absz > 255)
    {
        absdist >>= 1;
        absz >>= 1;
    }

    int angle = AngleTable[absdist][absz];

    // adjust for sign (which of the quadrants does the angle fall into?)
    if (target.z < 0 && target.y < 0)
        angle += 128;
    else if (target.z < 0)
        angle = 256 - angle;
    else if (target.y < 0)
        angle = 128 - angle;

    return angle;
}

int ConvertZToFacing(RS3DPoint pos, RS3DPoint target)
{
    S3DPoint p;

    p.x = target.x - pos.x;
    p.y = target.y - pos.y;
    p.z = target.z - pos.z;

    return ConvertZToFacing(p);
}

int Distance(RS3DPoint pos)
{
    int numshifts = 0;
    int x = abs(pos.x);
    int y = abs(pos.y);

    while (x > 255 || y > 255)
    {
        x >>= 1;
        y >>= 1;
        numshifts++;
    }

    int d = (int)DistTable[x][y];
    d <<= numshifts;

    return d;
}

int Distance(RS3DPoint pos, RS3DPoint target)
{
    S3DPoint p;

    p.x = abs(target.x - pos.x);
    p.y = abs(target.y - pos.y);
    p.z = 0;

    return Distance(p);
}

int AngleDiff(int angle1, int angle2)
{
    int diff = (angle1 & 255) - (angle2 & 255);
    if (diff > 128)
        diff -= 255;
    else if (diff <= -128)
        diff += 255;
    return diff;
}

// ******************
// * TObjectBuilder *
// ******************

TObjectBuilder::TObjectBuilder(char *name)
{
    if (numobjtypes < MAXOBJECTTYPES)
        builders[numobjtypes++] = this;

    objtypename = _strdup(name);
}

PTObjectBuilder TObjectBuilder::GetBuilder(int objtype)
{
    if (objtype < numobjtypes)
        return builders[objtype];

    return NULL;
}

PTObjectBuilder TObjectBuilder::GetBuilder(char *name)
{
    for (int i = 0; i < numobjtypes; i++)
        if (stricmp(name, builders[i]->objtypename) == 0)
            return builders[i];

    return NULL;
}

// **********************
// * TInventoryIterator *
// **********************

PTObjectInstance TInventoryIterator::NextItem()
{
    // Currently this DOES NOT recurse into other object's inventories,
    // because nothing uses it that way.  Copying some code from TMapIterator
    // would make it possible to do so if it is ever needed.
    item = NULL;

    if (owner)
    {
        do
        {
            if (invindex >= owner->NumInventoryItems())
                break;

            item = owner->GetInventory(invindex++);

        } while (!item);
    }

    return item;
}

// *******************
// * TObjectInstance *
// *******************

void TObjectInstance::ClearObject()
{
    SetNotify(N_SCRIPTADDED);

    animator            = NULL;
    lightdef.lightindex = -1;
    lightdef.lightid    = -1;
    lightdef.multiplier = -1;
    SetCommandDone(FALSE);
    script              = NULL;

    objclass = objtype  = -1;
    flags               = 0;
    notifyflags         = 0;
    sector              = NULL;
    state               = 0;
    group               = 0;

    frame               = 0;
    framerate           = 1;

    level = 0;
    pos.x = pos.y = pos.z = 0;
    vel.x = vel.y = vel.z = 0;
    rotatex = rotatey = rotatez = 0;
    accum.x = accum.y = accum.z = 0;
    invindex = inventnum = -1;
    shadow = -1;

    owner = NULL;

    moveangle = movevert = movedist = 0;
    movebits = 0;

    stats.Clear();

    // make oldpos DIFFERENT from pos here
    oldpos.x = pos.x + 1; oldpos.y = pos.y + 1; oldpos.z = pos.z + 1;
    screenx = screeny = screenz = 0;
}

TObjectInstance::TObjectInstance(PTObjectImagery img)
{
    ClearObject();
    imagery = img;

    if (imagery->NeedsAnimator(this))
        flags |= OF_ANIMATE;

    if (ObjClass() != OBJCLASS_TILE && ObjClass() != OBJCLASS_SHADOW)
        flags |= OF_PULSE;
}

TObjectInstance::TObjectInstance(PSObjectDef def, PTObjectImagery img)
{
    ClearObject();

    memcpy(&objclass, def, sizeof(SObjectDef));

  // Set imagery
    imagery = img;

  // Set ANIMATE and PULSE flags
    if (imagery->NeedsAnimator(this))
        flags |= OF_ANIMATE;

    if (ObjClass() != OBJCLASS_TILE)
        flags |= OF_PULSE;

    if (flags & OF_LIGHT)
        flags = flags | OF_ANIMATE | OF_PULSE;
    
  // Set Object info pointer
    cl = TObjectClass::GetClass(objclass);
    if (!cl)
        FatalError("Bad object class!"); // This should never happen!!
    inf = cl->GetObjType(objtype);
    name = inf->name;

  // Setup stat array
    if (cl->NumObjStats() > 0)
    {
        stats.SetNumItems(cl->NumObjStats());
        for (int c = 0; c < cl->NumObjStats(); c++)
            ResetObjStat(c);
    }

    if (objclass != OBJCLASS_TILE) // No scripts for regular tiles (for efficiency sake)
        InitScript(ScriptManager.ObjectScript(this));
}

TObjectInstance::~TObjectInstance()
{
    if (objclass == -1)
        return;

    if (Inventory.GetContainer() == this)
        Inventory.SetContainer(NULL);

    if (animator)
        delete animator;

    TObjectImagery::FreeImagery(imagery);

    if (lightdef.lightindex != -1)
        Scene3D.DeleteLight(lightdef.lightindex);

    if (lightdef.lightid != -1)
        FreeLightIndex(lightdef.lightid);

    // recursively delete all the objects in its inventory
    for (TInventoryIterator i(this); i; i++)
    {
        i.Item()->RemoveFromInventory();
        delete i.Item();
    }

    // take itself out of owner's inventory
    RemoveFromInventory();

    // If in map, remove from map
    if (GetSector() != NULL)
        MapPane.RemoveObject(this);

    // Delete the name
    if (name && name != inf->name)
    {
        free(name);
        name = NULL;
    }

    // If we're being centered on, cancel that
    if (MapPane.GetCenterOnObj() == this)
        MapPane.CenterOnObj(NULL, FALSE); // Don't center on anything

    // Kill the script
    if (script)
        delete script;

}

void TObjectInstance::SetName(char *newname)
{
    if (name && name != inf->name)  // If name is not set to point to objinfo name string
    {
        free(name);                 // delete it
        name = NULL;
    }

    if (newname && *newname)
        name = _strdup(newname);    // Name set to a specific name
    else
        name = inf->name;           // Name cleared, set to objinfo name instead

  // Reset script when name changes (could have a new script)   
    if (script)
    {
        delete script;
        script = NULL;
    }

    if (objclass != OBJCLASS_TILE) // No scripts for regular tiles (for efficiency sake)
        InitScript(ScriptManager.ObjectScript(this));
}

PTObjectImagery TObjectInstance::GetImagery()
{
    return  imagery;
}

BOOL TObjectInstance::CreateAnimator()
{
    animator = imagery->NewObjectAnimator(this);

    if (animator)
        animator->Initialize();      // Call the animator's initialize function

    return (animator != NULL);
}

void TObjectInstance::FreeAnimator()
{
    if (animator)
    {
        delete animator;
        animator = NULL;
    }
}

BOOL TObjectInstance::NeedsAnimator()
{
    if (!animator && imagery)
        return imagery->NeedsAnimator(this);

    return FALSE;
}

void TObjectInstance::Damage(int damage, int type)
{
// this is probably a serious hack
    if(IsIced() && type != DAMAGE_ICE)
        SetIced(FALSE);
// end of serious hack
    if (damage < Health())
        SetHealth(Health() - damage);
    else
        SetHealth(0);
}

void TObjectInstance::RepaintObject()
{
}

int TObjectInstance::Distance(PTObjectInstance inst)
{
    return ::Distance(pos, inst->pos);
}

// Returns the angle to the other instance
int TObjectInstance::AngleTo(PTObjectInstance inst)
{
    S3DPoint tpos;
    inst->GetPos(tpos);
    return ConvertToFacing(pos, tpos);
}

// Returns the + or - difference between this objects facing and the dest obj
int TObjectInstance::FaceAngleTo(PTObjectInstance inst)
{
    int angle = AngleTo(inst) - GetFace();
    if (angle >= 128)
        angle = angle - 256;
    else if (angle <= -128)
        angle = angle + 256;
    return angle;
}

void TObjectInstance::GetScreenPos(int &x, int &y)
{
    if (oldpos == pos)
    {
        x = screenx;
        y = screeny;
    }
    else
    {
        WorldToScreen(pos, x, y);
        WorldToScreenZ(pos, screenz);

        screenx = x;
        screeny = y;

        oldpos = pos;
    }
}

void TObjectInstance::GetScreenPos(S3DPoint &s)
{
    if (oldpos == pos)
    {
        s.x = screenx;
        s.y = screeny;
        s.z = screenz;
    }
    else
    {
        WorldToScreen(pos, screenx, screeny);
        WorldToScreenZ(pos, screenz);

        s.x = screenx;
        s.y = screeny;
        s.z = screenz;

        oldpos = pos;
    }
}

// Sets the current object position
int TObjectInstance::SetPos(RS3DPoint newpos, int newlevel, BOOL override)
{
    int index = GetMapIndex();

  // Note: can't change level of regular map objects, only non map objects
    if (newlevel < 0 || !(flags & OF_NONMAP))
        newlevel = MapPane.GetMapLevel();

  // Didn't move
    if (newpos == pos && newlevel == level)
        return index;                           // no movement, so bail out

  // Isn't in map
    if (override || !sector || index < 0)
    {
        pos = newpos;
        level = newlevel;
        return index;
    }

  // Handle shadow, if any
    if (shadow >= 0)
    {
        PTObjectInstance s = MapPane.GetInstance(shadow);
        if (s)
        {
            S3DPoint delta = newpos;
            delta -= pos;

            S3DPoint spos;
            s->GetPos(spos);
            spos += delta;
            s->SetPos(spos);
        }
    }

  // Make sure we're still in currently loaded map after we move
    index = MapPane.CheckPos(this, newpos, newlevel);

  // Get original screen rectangle
    SRect oldrect;
    GetScreenRect(oldrect);

    MapPane.ExtractWalkmap(this);

  // Sets new position
    pos = newpos;
    level = newlevel;

    MapPane.TransferWalkmap(this);

  // Update 3D system light
    if (lightdef.lightindex != -1)
    {
        S3DPoint lpos = pos;
        lpos += lightdef.pos;
        Scene3D.SetLightPosition(lightdef.lightindex, lpos);
    }

  // Get new screen rectangle
    SRect newrect;
    GetScreenRect(newrect);

  // Update background
    if (BgDrawMode() != BGDRAW_NONE)
    {
        int numrects = 0;
        SRect rects[4];
        if (SubtractRect(oldrect, newrect, rects, numrects))
        {
            for (int c = 0; c < numrects; c++)
                MapPane.AddBgUpdateRect(rects[c], BgDrawMode());
        }
        RedrawBackground();
    }

    return index;
}

// Gets new snap position
void TObjectInstance::GetSnapPos(PTObjectInstance oi, int dist, S3DPoint &p)
{
    ConvertToVector(oi->GetFace(), dist, p);
    p += oi->Pos();
}

// Moves this object so that it is the given distance 'dist' from 'oi'
void TObjectInstance::SnapDist(PTObjectInstance oi, int dist)
{
    if (dist >= 0)
    {
        int curdist = Distance(oi);
        if (dist != curdist)
        {
            S3DPoint p;
            ConvertToVector(oi->AngleTo(this), dist, p);
            p += oi->Pos();
            SetPos(p);
        }
    }
}

void TObjectInstance::ResetState()
{
    int stateflags = (imagery)?imagery->GetAniFlags(state):0;
    int statesize = (imagery)?imagery->GetAniLength(state):0;
    if (stateflags & AF_REVERSE)
    {
        frame = statesize - 1;
        framerate = -1;
    }
    else
    {
        frame = 0;
        framerate = 1;
    }
    if (animator)
        animator->ResetState();
}

// Sets the current animation state for the object
BOOL TObjectInstance::SetState(int newstate)
{
    if (GetImagery())
    {
        if ((DWORD)newstate >= (DWORD)GetImagery()->NumStates())
        {
            SetCommandDone(TRUE);
            return FALSE;
        }
    }

    RedrawBackground();
    MapPane.ExtractWalkmap(this);

    LOCKOBJECTS;   // Prevent update system from drawing objects while we change state

  // Set prevoious values for interpolation system
    prevstate = state; // Previous state
    prevframe = frame; // Previous state's final frame (NOT THIS STATES PREVIOUS FRAME!!!)

  // Set new state now
    state = newstate;
    SetCommandDone(FALSE);
    ResetState();

    UNLOCKOBJECTS; // Ok, go ahead and draw objects again

    MapPane.TransferWalkmap(this);

    if (GetImagery()->NeedsAnimator(this))
        flags |= OF_ANIMATE;            // Cause Animate() function to be called
    else
    {
        FreeAnimator();
        flags &= ~(DWORD)OF_ANIMATE;    // Prevents Animate() function from being called
    }

    RedrawBackground();

    return TRUE;
}

BOOL TObjectInstance::AddToInventory(PTObjectInstance inst, int slot)
{
    if (slot < 0)
        slot = FindFreeInventorySlot();

    if ((DWORD)slot >= MAXINVITEMS)
        return FALSE;

    inst->OffScreen();

    int index = inventory.Add(inst);
    if (index < 0)
        return FALSE;

    if (inst->GetMapIndex() <= 0)
        inst->SetMapIndex(MapPane.MakeIndex());

    inst->invindex = index;
    inst->inventnum = slot;
    inst->owner = this;

    inst->pos.x = inst->pos.y = inst->pos.z = 0;
    inst->level = 0;
    inst->sector = NULL;

    inst->SignalAddedToInventory();

    if (this == Inventory.GetContainer())
        Inventory.Update();

    return TRUE;
}

BOOL TObjectInstance::AddToInventory(char *name, int number, int slot)
{
    PTObjectClass cl;
    int ot;
    for (int i = 0; i < MAXOBJECTCLASSES; i++)
    {
        cl = TObjectClass::GetClass(i);
        if (cl && (ot = cl->FindObjType(name)) >= 0)
            break;
    }

    if (ot < 0)
        return FALSE;

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));

    def.objclass = cl->ClassId();
    def.objtype = ot;

    PTObjectInstance inst = cl->NewObject(&def);
    if (!inst)
        return FALSE;

    if (number != 1)
        inst->SetAmount(number);

    return AddToInventory(inst, slot);
}

void TObjectInstance::RemoveFromInventory()
{
    if (owner)
    {
        if (owner == Inventory.GetContainer())
            Inventory.Update();

        owner->inventory.Remove(invindex);
        owner = NULL;
    }

    invindex = -1;
    inventnum = -1;
}

int TObjectInstance::GiveInventoryTo(PTObjectInstance to, char *name, int number)
{
    int total = 0;
    if (number < 1)
        return 0;

    do
    {
        PTObjectInstance inst = FindObjInventory(name);

        if (!inst)
            return total;

        int amt = max(inst->Amount(), 1);

        if (number >= amt)
        {
            total += amt;
            number -= amt;
            inst->RemoveFromInventory();
            if (to)
                to->AddToInventory(inst);
            else
                delete inst;
        }
        else
        {
            inst->SetAmount(amt - number);
            if (to)
            {
                if (!to->AddToInventory(name, number))
                    return total;
            }
            total += number;
            number = 0;
            
            if (inst->owner == Inventory.GetContainer())
                Inventory.Update();
        }

    } while (number > 0);

    return total;
}

int TObjectInstance::GetInventoryAmount(char *name)
{
    int total = 0;

    for (TInventoryIterator i(this); i; i++)
        if (stricmp(i.Item()->GetName(), name) == 0)
            total += max(i.Item()->Amount(), 1);

    return total;
}

BOOL TObjectInstance::HasEmptySlot()
{
    DWORD slot = FindFreeInventorySlot();

    if ((DWORD)slot >= MAXINVITEMS)
        return FALSE;

    return TRUE;
}

void TObjectInstance::SignalAddedToInventory()
{
}

BOOL TObjectInstance::AddToMap()
{
    return (MapPane.AddObject(this) >= 0);

}

void TObjectInstance::RemoveFromMap()
{
    for (TMapIterator i; i; i++)
        if (i.Item() == this)
            break;

    if (i.Item())
        MapPane.RemoveFromSector(this, i.SectorX(), i.SectorY(), i.SectorIndex());
}

int TObjectInstance::FindFreeInventorySlot()
{
    int lowest = 0;
    BOOL done = FALSE;

    while (!done)
    {
        done = TRUE;

        for (TInventoryIterator i(this); i; i++)
            if (i.Item()->InventNum() == lowest)
            {
                done = FALSE;
                lowest++;
                break;
            }
    }

    return lowest;
}

int TObjectInstance::RealNumInventoryItems()
{
    // count up the real number of items in the inventory
    int num = 0;
    for (TInventoryIterator i(this); i; i++)
        num++;

    return num;
}

PTObjectInstance TObjectInstance::GetInventory(int index)
{
    return inventory[index];
}

PTObjectInstance TObjectInstance::GetInventorySlot(int slot)
{
    for (TInventoryIterator i(this); i; i++)
        if (i.Item()->InventNum() == slot)
            return i.Item();

    return NULL;
}

PTObjectInstance TObjectInstance::FindObjInventory(char *name)
{
    for (TInventoryIterator i(this); i; i++)
        if (stricmp(i.Item()->GetName(), name) == 0)
            return i.Item();

    return NULL;
}

PTObjectInstance TObjectInstance::FindObjInventory(int objclass, int type)
{
    for (TInventoryIterator i(this); i; i++)
        if (i.Item()->ObjClass() == objclass && (type < 0 || i.Item()->GetStat("Type") == type))
            return i.Item();

    return NULL;
}

BOOL TObjectInstance::Use(PTObjectInstance user, int with)
{
    if (with >= 0)  // With object.. use with name as key
    {
        PTObjectInstance inst = MapPane.GetInstance(with);
        if (GetScript())
            GetScript()->Trigger(TRIGGER_USE, inst->GetName());
    }
    else        // No with object.. use 'use' object as key
    {
        if (GetScript())
            GetScript()->Trigger(TRIGGER_USE);
        if (user && user->GetScript())
            user->GetScript()->Trigger(TRIGGER_USE, this->GetName());
    }

    /*
    if (!inst || inst->GetOwner() != GetOwner() || inst->ObjClass() != objclass)
        return FALSE;

    PTObjectClass cl = TObjectClass::GetClass(ObjClass());

    if (GetStatistic("Combining") && inst->GetStatistic("Combining") &&
        absval(ObjType() - inst->ObjType()) == 1)
    {
        int slot = inst->InventNum();
        PTObjectInstance own = GetOwner();
        if (own)
        {
            MapPane.RemoveFromInventory(own, InventNum());
            MapPane.RemoveFromInventory(own, inst->InventNum());
        }

        SetFlags(OF_KILL);
        inst->SetFlags(OF_KILL);

        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));

        def.objclass = ObjClass();
        def.objtype = ObjType() > inst->ObjType() ? (ObjType() + 1) : (inst->ObjType() + 1);
        Player->GetPos(def.pos);

        int index = MapPane.NewObject(&def);
        if (index >= 0 && own)
            MapPane.AddToInventory(own, index, slot);

        return TRUE;
    }
    */
    return FALSE;
}

void TObjectInstance::Pulse()
{
/*  static int i = 230;
    static int d = 1;

    if (flags & OF_LIGHT) // TEST
    {
        SetLightIntensity(i);
        i += d;
        if (i >= 255)
        {
            i = 255;
            d = -1;
        }
        else if (i < 230)
        {
            i = 230;
            d = 1;
        }
    }
*/
  // Pulse the animator
    if (animator)
        animator->Pulse();

  // Check if script is done
    if (CommandDone())
        ContinueScript();
}

DWORD TObjectInstance::Move()
{
    if (flags & OF_IMMOBILE || IsInInventory() || objclass == OBJCLASS_TILE || objclass == OBJCLASS_EXIT || objclass == OBJCLASS_SHADOW)
        return MOVE_NOTHING;

  // Now do this frame's movement
    S3DPoint newpos = pos;
    DWORD retval = 0;

    int h = (MapPane.GetWalkHeight(pos) + 1);

    if ((pos.z < h || h == 1) && !(flags & OF_NOCOLLISION))
        return MOVE_BLOCKED;                // very basic collision detection

    if (h < pos.z && vel.z > -TERMINAL_VELOCITY && !(flags & OF_WEIGHTLESS))
    {
        vel.z -= GRAVITY;
        retval |= MOVE_FALLING;

        // $$$ KES $$$ -- check to see that we aren't falling too far...
        if ((pos.z + vel.z) < (h + 1)) vel.z = ((h + 1) - pos.z);
    }


  // ADD IN THE MOVEMENT VALUE HERE!
    S3DPoint nextmove;
    GetNextMove(nextmove);
    accum += nextmove;

  // ADD IN THE VELOCITY VALUE HERE!
    accum += vel;

    rollover(accum.x, newpos.x);
    rollover(accum.y, newpos.y);
    rollover(accum.z, newpos.z);

    if (newpos.z < h)
    {
        if (absval(vel.z) > GRAVITY)
            vel.z = -(vel.z / 4);               // bounce bounce
        else
            vel.z = 0;

        newpos.z = h;
        accum.z = 0;
    }

    if ((objclass == OBJCLASS_CHARACTER) && (newpos.z < h))
        newpos.z = h;

    if (newpos == pos && vel.x == 0 && vel.y == 0 && vel.z == 0)
        return MOVE_NOTHING;        // can only check this _after_ gravity

    if (newpos == pos)
        return MOVE_MOVED | retval;         // they moved, but not a whole unit

    //if (!MapPane.LineOfSight(pos, newpos))
    //  return MOVE_BLOCKED;

    SetPos(newpos);

    return MOVE_MOVED | retval;
}

void TObjectInstance::SetObjectMotion()
{
  // Get next movement values from imagery object
    if (imagery)
        imagery->SetObjectMotion(this);
}

// Set next frame's movement.
void TObjectInstance::SetNextMove(RS3DPoint p)
{
    moveangle = ::ConvertToFacing(p);
    movedist = ::Distance(p);
    movevert = p.z;
}

// Get next frame's movement.
void TObjectInstance::GetNextMove(RS3DPoint p)
{
    ConvertToVector(moveangle, movedist, p);
    p.z = movevert;
}

void TObjectInstance::Animate(BOOL draw)
{
    if (animator)
        animator->Animate(draw);
}

// Sets next frame for object
void TObjectInstance::NextFrame()
{
    if (flags & OF_PARALIZE)
        return;

    if (!imagery)
        return;

    int stateflags = 0;
    int statesize = 0;

    if (IsInInventory())
    {
        statesize = imagery->GetInvAniLength(state);
        stateflags = imagery->GetInvAniFlags(state);
    }
    else
    {
        statesize = imagery->GetAniLength(state);
        stateflags = imagery->GetAniFlags(state);
    }

    frame += framerate;

    if (framerate < 0)
    {
        if (frame < 0)
        {
            SetCommandDone(TRUE);

            if (frame < 0)
            {
                if (stateflags & AF_LOOPING)
                    frame = statesize - 1;
                else
                {
                    if (stateflags & AF_PINGPONG)
                    {
                        frame = 1;
                        framerate = 1;
                    }
                    else
                    {
                        if (animator)
                            animator->SetComplete(TRUE);
                        frame = 0;
                    }
                }
            }
        }
        else if (CommandDone() == TRUE)
            SetCommandDone(FALSE);
    }
    else
    {
        if (frame > (statesize - 1))
        {
            SetCommandDone(TRUE);

            if (frame >= statesize)
            {
                if (stateflags & AF_LOOPING)
                    frame = 0;
                else
                {
                    if (stateflags & AF_PINGPONG)
                    {
                        frame = statesize - 1;
                        framerate = -1;
                    }
                    else
                    {
                        if (animator)
                            animator->SetComplete(TRUE);
                        frame = statesize - 1;
                    }
                }
            }
        }
        else if (CommandDone() == TRUE)
            SetCommandDone(FALSE);
    }

    if (animator)
        animator->SetNewState(FALSE);
}

void TObjectInstance::OnScreen()
{
    if (NeedsAnimator())
        CreateAnimator();
    else
        FreeAnimator();

    if (flags & OF_LIGHT)
    {
        if (lightdef.lightindex == -1)
        {
            S3DPoint lpos = pos;
            lpos += lightdef.pos;
            lightdef.lightindex = Scene3D.AddLight(lpos, lightdef.color, lightdef.intensity, lightdef.multiplier);
        }
    }
}

void TObjectInstance::OffScreen()
{
    if (animator)
        FreeAnimator();

    if (lightdef.lightindex != -1)
    {
        Scene3D.DeleteLight(lightdef.lightindex);
        lightdef.lightindex = -1;
    }
    if (lightdef.lightid != -1)
    {
        FreeLightIndex(lightdef.lightid);
        lightdef.lightid = -1;
    }
}

void TObjectInstance::GetFacingBoundBox(int &nx, int &ny, int &nsx, int &nsy)
{
    int width, length, height;
    GetImagery()->GetWorldBoundBox(state, width, length, height);

    int regx = GetImagery()->GetWorldRegX(state);
    int regy = GetImagery()->GetWorldRegY(state);

    nx = regx;
    ny = regy;
    nsx = width;
    nsy = length;

    if (facing >= 0xE0 || facing < 0x20)
    {
        // north, which is default
        return;
    }
    else if (facing < 0x60)
    {
        // east
        nx = regy;
        ny = regx;
        nsx = length;
        nsy = width;
    }
    else if (facing < 0xA0)
    {
        // south
        nx = width - regx - 1;
    }
    else if (facing < 0xE0)
    {
        // west
        ny = width - regx - 1;
        nsx = length;
        nsy = width;
    }
}

// ********* Background Redrawing Function **********

// Returns the default point in bgdraw pipeline at which to redraw this object.  The
// bg draw pipeline draws the UNLIT data first (raw tile graphics/colors), then applies
// the lighting, then draws any lit graphics to the lit buffer, then copies the whole thing
// to the screen.  Objects like lights only need to refresh from the lighting stage on up,
// thus saving scads of time.
//
// This function doesn't currently check the imagery for what
int TObjectInstance::BgDrawMode()
{
    if (flags & OF_LIGHT)
        return BGDRAW_LIGHTS; 
    else if (flags & (OF_MOVING | OF_INVISIBLE | OF_SELDRAW))
        return BGDRAW_NONE;
    else return BGDRAW_UNLIT;
}

void TObjectInstance::RedrawBackground(int bgdraw)
{
  // If object is tile or light and drawn background buffers add
  // an update rectangle for the object. (moving objects not included)

    if (!(flags & OF_MOVING) &&                       // No moving objs
        (!(flags & OF_SELDRAW) || (flags & OF_LIGHT)) // No sel objs (except lights)
         && InventNum() < 0)                          // No objects inside another object
    {
        SRect r;
        GetScreenRect(r);
        if (bgdraw == -1)               // Use default draw pipeline flag for object
        {
            if (flags & OF_LIGHT)
                bgdraw = BGDRAW_LIGHTS; // Default to redraw from lights on up
            else
                bgdraw = BGDRAW_UNLIT;  // Default to draw from unlit on up (all)
        }
        MapPane.AddBgUpdateRect(r, bgdraw);
    }
}

// ********** Script Functions ************

void TObjectInstance::InitScript(PTScript newscr)
{
    if (objclass == OBJCLASS_TILE)
        return; // Tiles can't do scripts!!

    script = newscr;
    if (script)
    {
        flags |= OF_PULSE;          // Pulse me so script will run
        SetNotify(N_SCRIPTDELETED); // Tell us if script gets hacked
    }

    ResetScript();
}

void TObjectInstance::ResetScript()
{
    if (script)
        script->Start();
}

void TObjectInstance::ContinueScript()
{
    if (script && !(flags & OF_PAUSE))
        script->Continue(this);
}

void TObjectInstance::ScriptJump(char *label)
{
    if (script)
        script->Jump(this, label);
}

// ****************************** Parse Command ********************************

int TObjectInstance::ParseCommand(TToken &t)
{
    return CMD_BADCOMMAND;
}

// *************************** Lighting Functions *****************************

void TObjectInstance::RedrawLight()
{
    if (flags & OF_LIGHT && lightdef.intensity != 0)
    {
        SRect r;
        GetLightRect(r);
        MapPane.AddBgUpdateRect(r, BGDRAW_LIGHTS);
    }
}

void TObjectInstance::GetLightRect(RSRect r)
{
    S3DPoint wlpos = pos;
    wlpos += lightdef.pos;

    S3DPoint slpos;
    WorldToScreen(wlpos, slpos);

    r.left = slpos.x - lightdef.intensity;
    r.top = slpos.y - lightdef.intensity;
    r.right = r.left + lightdef.intensity + lightdef.intensity;
    r.bottom = r.top + lightdef.intensity + lightdef.intensity;
}

void TObjectInstance::SetLightIntensity(BYTE newintensity)
{
    if (!(flags & OF_LIGHT))
        return;

    LOCKOBJECTS;    // Prevent update system from drawing any lights right now

    BOOL bigger = newintensity > lightdef.intensity;

    if (!bigger)
        RedrawLight();

    lightdef.intensity = newintensity;

    if (lightdef.lightindex != -1)
        Scene3D.SetLightIntensity(lightdef.lightindex, lightdef.intensity);

    if (bigger)
        RedrawLight();

    UNLOCKOBJECTS; // Allow update system to draw lights now
}

void TObjectInstance::SetLightMultiplier(int mult)
{
    if (!(flags & OF_LIGHT))
        return;

    LOCKOBJECTS;    // Prevent update system from drawing any lights right now

    lightdef.multiplier = mult;

//  if (lightdef.lightindex != -1)
//      Scene3D.SetLightIntensity(lightdef.lightindex, lightdef.intensity);

    if (lightdef.lightid != -1)
    {
        FreeLightIndex(lightdef.lightid);
        lightdef.lightid = NewLightIndex(lightdef.color, lightdef.multiplier);
    }

    RedrawLight();

    UNLOCKOBJECTS; // Allow update system to draw lights now
}

void TObjectInstance::SetLightPos(RS3DPoint newpos)
{
    if (!(flags & OF_LIGHT))
        return;

    LOCKOBJECTS;     // Prevent update system from drawing any lights right now

    RedrawLight();

    lightdef.pos = newpos;

    if (lightdef.lightindex != -1)
    {
        S3DPoint lpos = pos;
        lpos += lightdef.pos;
        Scene3D.SetLightPosition(lightdef.lightindex, lpos);
        RedrawLight();
    }

    UNLOCKOBJECTS;   // Allow update system to draw lights now
}

void TObjectInstance::SetLightColor(SColor color)
{
    if (!(flags & OF_LIGHT) || (color.red == lightdef.color.red &&
        color.green == lightdef.color.green && color.blue == lightdef.color.blue))
        return;

    LOCKOBJECTS;     // Prevent update system from drawing any lights right now

    lightdef.color = color;

    if (lightdef.lightindex != -1)
        Scene3D.SetLightColor(lightdef.lightindex, color);

    if (lightdef.lightid != -1)
    {
        FreeLightIndex(lightdef.lightid);
        lightdef.lightid = NewLightIndex(color, lightdef.multiplier);
    }

    RedrawLight();

    UNLOCKOBJECTS;   // Allow update system to draw lights now
}

void TObjectInstance::DrawLight(PTSurface surface, BOOL resetid)
{
    if ((flags & OF_LIGHT) && (lightdef.intensity > 0))
    {
        if (imagery)
            imagery->DrawLight(this, surface);

        S3DPoint wlpos = pos;
        wlpos += lightdef.pos;

        S3DPoint slpos;
        WorldToScreen(wlpos, slpos);

        if (resetid || lightdef.lightid == -1)
            lightdef.lightid = NewLightIndex(lightdef.color, lightdef.multiplier);

//      if (NoNormals)
            DrawStaticLightNoNormals(slpos, lightdef.color, lightdef.intensity, surface, lightdef.lightid);
//      else if (lightdef.flags & LIGHT_DIR)
//          DrawStaticDirLight(slpos, lightdef.color, lightdef.intensity, surface, lightdef.lightid);
//      else
//          DrawStaticLight(slpos, lightdef.color, lightdef.intensity, surface, lightdef.lightid);
    }
}

// DLS brightness routine (gives brightness given distance)
extern double GetLightBrightness(int dist, int intensity, int multiplier);

// Get the amount of illumination from this light to this particular object
int TObjectInstance::GetIllumination(PTObjectInstance oi)
{
  // Not a light
    if (!(flags & OF_LIGHT) || (lightdef.intensity <= 0))
        return 0;

  // Dist from object to light (assumes char, and uses CHARHEIGHT value)
    S3DPoint p;
    oi->GetPos(p);
    p.z += LIGHTINGCHARHEIGHT;
    p -= pos;

  // Out of range
    if (abs(p.x) > lightdef.intensity || abs(p.y) > lightdef.intensity)
        return 0;

  // Get distance from light to object
    float d = (float)sqrt((double)(p.x * p.x + p.y * p.y));
    d = (float)sqrt((double)(d * d + p.z * p.z));

  // Get total brighness from dls brightness tables
    return (int)(
        min(GetLightBrightness((int)d, lightdef.intensity, lightdef.multiplier), 1.0) * 255.0);
}

// ********************** End of Light Functions ************************

void TObjectInstance::SetCommandDone(BOOL newcmd)
{
    commanddone = newcmd;
}

void TObjectInstance::SetNotify(DWORD newflags)
{
    if (newflags == 0)
        ResetFlags(flags & (~OF_NOTIFY));
    else
    {
        MapPane.SetNotify(newflags);    // Tells mappane there's at least one object that wants these flags
        ResetFlags(flags | OF_NOTIFY);  // Tell map that we want to be notified of stuff
    }
}

void TObjectInstance::Notify(int notify, void *ptr)
{
    if (notify == N_SCRIPTDELETED) // Check if we're using this script and delete it if so
    {
        if (script && script->GetScriptProto() == (PTScriptProto)ptr)
        {
            delete script;
            script = NULL;
        }
    }
    else if (notify == N_SCRIPTADDED)   // Check to see if new script matches us
    {
        if (!script && objclass != OBJCLASS_TILE) // No scripts for regular tiles
            InitScript(ScriptManager.ObjectScript(this));
    }
}

PTObjectInstance TObjectInstance::LoadObject(RTInputStream is, int version, BOOL ismap)
{
    DWORD uniqueid;
    short objversion = 0;
    short objclass;
    short objtype;
    short blocksize;
    SObjectDef def;
    BOOL forcesimple = FALSE;
    BOOL corrupted = FALSE;

    // ****** Load object block header ******

    // Get object version
    if (version >= 8)
        is >> objversion;

    if (objversion < 0) // Objversion is the placeholder in map version 8 or above
        return NULL;

    is >> objclass;
    if (objclass < 0)   // Placeholder for empty object slot
        return NULL;

    // Check the sector map version before we read the type info
    if (version < 1)
    {
        // Version 0 - No Unique ID's, so just read the objtype directly
        is >> objtype;
        DWORD uniqueid = 0;
        blocksize = -1;     
    }
    else if (version < 4)
    {
        // Version 1 and above - Unique ID's used instead of objtype, so find
        //             the objtype given the Unique ID

        objtype = -1;
        is >> uniqueid;
        blocksize = -1;
    }
    else
    {
        // Version 4 has block size, so we can just skip over objects 
        //              we don't recognize
        objtype = -1;
        is >> uniqueid;
        is >> blocksize;
    }

    // ****** Is this object any good? ******
    
    PTObjectClass cl = TObjectClass::GetClass(objclass);
    if (!cl || !cl->ClassName())
    {
        if (Debug)
            FatalError("Object in map file has invalid class - possible file corruption");
        else if (blocksize >= 0)
        {
            is.MovePos(blocksize);  // Just quietly skip this object
            return NULL;
        }
        else                        // Try to fix it by assuming its a tile
        {
            objclass = OBJCLASS_TILE;
            cl = TObjectClass::GetClass(objclass);
            corrupted = TRUE;
        }
    }

    if (objtype < 0)
    {           
        objtype = cl->FindObjType(uniqueid);

        if (objtype < 0)
        {
            // not found in this class, so check all of them
            int newobjtype, newobjclass;
            PTObjectClass newcl;
            for (newobjclass = 0; newobjclass < MAXOBJECTCLASSES; newobjclass++)
            {
                newcl = TObjectClass::GetClass(newobjclass);
                if (newcl && (newobjtype = newcl->FindObjType(uniqueid)) >= 0)
                {
                    objclass = newobjclass;
                    objtype = newobjtype;
                    cl = newcl;
                    forcesimple = TRUE;
                    break;
                }
            }
        }

        if (objtype < 0)  // Still can't find type
        {
            if (Debug)
            {
                // give a more descriptive error
                char buf[80];
                sprintf(buf, "Object unique id 0x%x not found in class.def", uniqueid);
                FatalError(buf);
            }
            else if (blocksize >= 0)    // Just skip over this object
            {
                is.MovePos(blocksize);  
                return NULL;
            }
            else      // If attempting to fix, assume type is type 0 - first type in list
            {   
                
                objtype = 0;
                corrupted = TRUE;
            }
        }
    }

    // ****** Create the object ******

    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = objclass;
    def.objtype  = objtype;

    PTObjectInstance inst = cl->NewObject(&def);
    if (!inst)
        FatalError("Trouble creating loaded object (corrupted sector file?)");

    // ****** Load the object ******

  // Get start of object
    DWORD start = is.GetPos();

  // Load object and its inventory
    if (forcesimple)
        inst->TObjectInstance::Load(is, version, objversion); // Used if object changed class (for some reason)
    else
        inst->Load(is, version, objversion);                  // This should normally be used
    inst->LoadInventory(is, version);

  // Reset position to start of next object (in case object load is bad or forcesimple is true)
    if (blocksize >= 0)
        is.SetPos(start + blocksize);

  // If this object is corrupted in some way, delete it after doing load
    if (corrupted || (ismap && (inst->Flags() & OF_NONMAP)))
    {
        delete inst;
        return NULL;
    }

    return inst;
}

void TObjectInstance::SaveObject(PTObjectInstance inst, RTOutputStream os, BOOL ismap)
{

    os.MakeFreeSpace(1024); // Check for enough free space for object

  // Note: Main game sector files will not load or save players....
    if (!inst || (ismap && (inst->Flags() & OF_NONMAP)))
    {
        os << (short const)-1; // Keep empty spaces in object array
        return;
    }

    os << (short)inst->ObjVersion();// Object version id
    os << (short)inst->ObjClass();  // Class id
    os << (DWORD)inst->ObjId();     // Object id
    os << (short)0;                 // Block size

    DWORD start = os.GetPos();

    inst->Save(os);
    inst->SaveInventory(os);

    DWORD end = os.GetPos();

    os.SetPos(start - 2);
    os << (short)(end - start);
    os.SetPos(end);
}

void TObjectInstance::LoadInventory(RTInputStream is, int version)
{
    if (version < 3)
        return;

    int num;
    is >> num;

    if (num > 2048) // Maddness!!  Maddness!!
    {
        _RPT1(_CRT_WARN, "Invalid inventory size for obj %s\n", this->GetName());
        return;
    }

    for (int i = 0; i < num; i++)
    {
        PTObjectInstance inst = LoadObject(is, version);
        if (inst)
        {
            inventory.Add(inst);
            inst->SetOwner(this);
        }
        else
        {
            _RPT1(_CRT_WARN, "Invalid inventory object for obj %s", this->GetName());
        }
    }
}

void TObjectInstance::SaveInventory(RTOutputStream os)
{
    int num = RealNumInventoryItems();

    os << num;

    if (num > 0)
    {
        // save them out recursively
        for (TInventoryIterator i(this); i; i++)
        {
            TObjectInstance::SaveObject(i.Item(), os);
        }
    }

}

void TObjectInstance::Load(RTInputStream is, int version, int objversion)
{
    BYTE len;
    is >> len;
    if (len > 0)
    {
        name = (char *)malloc(len + 1);
        for (int i = 0; i < len; i++)
            is >> name[i];
        name[i] = 0;
    }

    DWORD newflags;

    is >> newflags >> pos.x >> pos.y >> pos.z;

  // Make sure fixed flags remain the way they were set in the constructor
    flags = flags & OF_FIXEDFLAGS | (newflags & ~(OF_FIXEDFLAGS));

  // Don't load velocity vectors if not mobile
    if (version < 6 || !(flags & OF_IMMOBILE))
        is >> vel.x >> vel.y >> vel.z;
        
    if (version < 9)
    {
        BYTE statebyte;
        is >> statebyte;
        state = statebyte;
    }
    else
        is >> state; // WORD

  // Non map objects (i.e. players) store their level
  // Players are stored in the save game file, and not in the map, so we need to know what
  // level to put them in when we load them
    if (version >= 6 && (flags & OF_NONMAP))
    {
        if (version < 9)
        {
            BYTE levelbyte;
            is >> levelbyte;
            level = levelbyte;
        }
        else
            is >> level; // WORD
    }
    else
        level = 0;
        
    BYTE health;
    if (version < 5)
        is >> health; // This is now an objstat
    
    if (version < 3)
    {
        BYTE dummy8;
        short dummy16;
        is >> facing >> dummy16 >> inventnum >> dummy16 >> shadow >> dummy8;

        // ignore inventories in old version
        inventnum = -1;

        // sector will set old-style indexes after object is loaded
        mapindex = -1;
    }
    else
    {
        is >> inventnum >> invindex >> shadow >> rotatex >> rotatey >> rotatez >> mapindex;
    }
    moveangle = rotatez;    // Set movement angle

    if (version < 5)  // Set up empty stat array and stick health in it
    {
        frame = 0;
        framerate = 1;
        group = 0;

        if (cl->NumObjStats() > 0)
        {
            stats.SetNumItems(cl->NumObjStats());
            for (int c = 0; c < cl->NumObjStats(); c++)
                ResetObjStat(c);
        }

        SetHealth(health);
    }
    else
    {
        if (version >= 6)
        {
            if (flags & OF_ANIMATE)
                is >> frame >> framerate;   // Save framerate and frame if animating
        }
        else
            is >> frame >> framerate;       // Version 5, always save

        is >> group;

      // Sure stat storage is inefficient, but it's only for a few objects in the game
        if (cl->NumObjStats() > 0)
            stats.SetNumItems(cl->NumObjStats());

        BYTE numstats;
        is >> numstats;
        if (numstats > 0)   // Note: oridnary tiles can't have stats..
        {
            int statid = 0;
            for (int st = 0; st < numstats && statid < stats.NumItems(); st++)
            {
                int stat;
                DWORD uniqueid;
                is >> stat >> uniqueid;

              // Note: to allow us to change the stats for characters,
              // we check the unique id of the stat and match it to our object stat array
                if (uniqueid == cl->ObjStatUniqueId(statid))
                {
                    stats[statid] = stat;   // Quick case, id's all match
                    statid++;
                }
                else                
                {                           // Slow case.. search for id for stat
                    for (int c = 0; c < cl->NumObjStats(); c++)
                    {
                        if (uniqueid == cl->ObjStatUniqueId(statid))
                            stats[c] = stat;
                    }
                }
            }
        }

    }

    if (flags & OF_LIGHT)
    {
        is >> lightdef.flags >> lightdef.pos.x >> lightdef.pos.y >> lightdef.pos.z >>
            lightdef.color.red >> lightdef.color.green >> lightdef.color.blue >>
            lightdef.intensity >> lightdef.multiplier;
        flags |= OF_LIGHT | OF_ANIMATE;
    }

  // Set Object info pointer
    cl = TObjectClass::GetClass(objclass);
    if (!cl)
        FatalError("Bad object class!"); // This should never happen!!
    inf = cl->GetObjType(objtype);

  // Initialize the script (no tiles for efficiency)
    if (objclass != OBJCLASS_TILE)
        InitScript(ScriptManager.ObjectScript(this));

  // Reset inventory
    if (this == Inventory.GetContainer())
        Inventory.Update();
}

void TObjectInstance::Save(RTOutputStream os)
{
    if (flags & OF_LIGHT)
        flags |= OF_PULSE | OF_ANIMATE;

    BYTE len = strlen(name);
    os << len;
    for (int i = 0; i < len; i++)
        os << name[i];
  
  // Save general object data
    os << flags << pos.x << pos.y << pos.z;
    
    if (!(flags & OF_IMMOBILE))
        os << vel.x << vel.y << vel.z;
    
    os << state;

  // Non map objects (i.e. players) store their level
  // Players are stored in the save game file, and not in the map, so we need to know what
  // level to put them in when we load them
    if (flags & OF_NONMAP)
        os << level;
        
    os << inventnum << invindex << shadow << 
        rotatex << rotatey << rotatez << mapindex;
        
    if (flags & OF_ANIMATE)
        os << frame << framerate;
        
    os << group;

  // Save object specific stats (unique id is <=4 char code like "AMT", "TYPE", "AC")
  // Now I've got to say that the unique id thing is just pretty damn cool.  That's one
  // super tricky bit of coding there boy... wow.. what an idea.  Super groovy and all
  // that.  
    os << (BYTE)(cl->NumObjStats());
    for (int c = 0; c < cl->NumObjStats(); c++)
        os << stats[c] << cl->ObjStatUniqueId(c);

  // Save light data
    if (flags & OF_LIGHT)
        os << lightdef.flags << lightdef.pos.x << lightdef.pos.y << lightdef.pos.z <<
            lightdef.color.red << lightdef.color.green << lightdef.color.blue <<
            lightdef.intensity << lightdef.multiplier;
}

// ----------------- Flag Functions --------------

static char *flagnames[] = OBJFLAGNAMES;
#define NUMFLAGS sizearray(flagnames)

int TObjectInstance::GetNumFlags()
{
    return NUMFLAGS;
}

char *TObjectInstance::GetFlagName(int flagnum)
{
    return flagnames[flagnum];
}

int TObjectInstance::GetFlagNum(char *flagname)
{
    for (int c = 0; c < NUMFLAGS; c++)
    {
        if (!stricmp(flagnames[c], flagname))
            return c;
    }
    return -1;
}

void TObjectInstance::SetFlag(char *flagname, BOOL on)
{
    int flagnum = GetFlagNum(flagname);
    if (flagnum < 0)
        return;

    if (on)
        ResetFlags(flags | (1 << flagnum));
    else
        ResetFlags(flags & (~(1 << flagnum)));
}

BOOL TObjectInstance::IsFlagSet(char *flagname)
{
    int flagnum = GetFlagNum(flagname);
    if (flagnum < 0)
        return FALSE;

    return (flags & (1 << flagnum)) != 0;
}

void TObjectInstance::ResetFlags(DWORD newflags)
{
    DWORD oldflags = flags;
    flags = newflags;

  // Call map pane to allow object to be placed on new lists if.. say...
  // the OF_ANIMATE or OF_PULSE flags change.
    if (sector)
        MapPane.ObjectFlagsChanged(this, oldflags, newflags);

  // Update map if object flags change...
    if (sector != NULL) // In map
    {
      // Did walk status of tile change?
        if (!(oldflags & OF_NOWALK) && (flags & OF_NOWALK))
            MapPane.ExtractWalkmap(this);   // This doesn't seem to be working right now
        else if ((oldflags & OF_NOWALK) && !(flags & OF_NOWALK))
            MapPane.TransferWalkmap(this);  // NOWALK turned off, put tile back on map
    }
}

// ----------------- Object Instance Statistic Functions --------------

int TObjectInstance::GetStat(char *statname)
{
    int statid = cl->FindObjStat(statname);
    if (statid >= 0)
        return stats[statid];
    statid = cl->FindStat(statname);
    if (statid >= 0)
        return cl->GetStat(objtype, statid);
    return 0;
}

void TObjectInstance::SetStat(char *statname, int value)
{
    int statid = cl->FindObjStat(statname);
    if (statid >= 0)
    {
        stats[statid] = value;
        return;
    }
    statid = cl->FindStat(statname);
    if (statid >= 0)
        cl->SetStat(objtype, statid, value);
}

int TObjectInstance::GetStat(char *statname, char *str, int id)
{
    char buf[MAXNAMELEN];
    if (str && id >= 0)
    {
        sprintf(buf, "%s%d%s", statname, id, str);
        statname = buf;
    }
    else if (str)
    {
        sprintf(buf, "%s%s", statname, str);
        statname = buf;
    } 
        
    return GetStat(statname);
}

// Plays a sound at the given object position
BOOL TObjectInstance::PlayWave(char *soundname, int nr, int volume, int freq)
{
    S3DPoint p, mp;
    GetPos(p);
    MapPane.GetMapPos(mp);
    p -= mp;

    int id = SoundPlayer.FindSound(soundname, nr);
    if (id < 0)
        return FALSE;
    if (!SoundPlayer.Mount(id))
        return FALSE;
    SoundPlayer.Play(id, volume, freq, &p);
    SoundPlayer.Unmount(id);

    return TRUE;
}

// ***********************
// * Statistic Functions *
// ***********************

// Statistic entry constructor (adds the given stat to the class)
SStatEntry::SStatEntry(PTObjectClass cl, char *statname, char *uniqueid, int statid, 
    int def, int min, int max, BOOL objstat)
{
    id = statid;
    SStatisticDef statdef;
    strncpyz(statdef.name, statname, MAXNAMELEN);
    char uid[5];
    uid[0] = uid[1] = uid[2] = uid[3] = uid[4] = 0;
    strncpyz(uid, uniqueid, 5);
    statdef.uniqueid = *(DWORD *)uid;
    statdef.def = def;
    statdef.min = min;
    statdef.max = max;
    if (objstat)
        cl->AddObjStat(statdef, id);
    else
        cl->AddStat(statdef, id);
}

// Add a new statistic
int TStatisticDefList::AddStat(SStatisticDef &newstatdef, int newid)
{
  // Statistic already exists.. update it
    for (int c = 0; c < statdefs.NumItems(); c++)
    {
        if (!stricmp(statdefs[c].name, newstatdef.name))
        {
            memcpy(&(statdefs[c]), &newstatdef, sizeof(SStatisticDef));
            return c;
        }
    }

    if (newid >= 0)
        return statdefs.Set(newstatdef, newid);

    return statdefs.Add(newstatdef);
}

BOOL TStatisticDefList::ParseStat(SStatisticDef &stat, TToken &t)
{
    char uniqueidstr[5];
    uniqueidstr[0] = uniqueidstr[1] = uniqueidstr[2] = uniqueidstr[3] = 0; 


    memset(&stat, 0, sizeof(SStatisticDef));

    if (t.Type() != TKN_TEXT && t.Type() != TKN_IDENT)
        return FALSE;
    strncpyz(stat.name, t.Text(), MAXNAMELEN);

    t.WhiteGet();
    if (t.Type() != TKN_IDENT) // Old style def
    {
        if (!Parse(t, "%d %d %d", &stat.def, &stat.min, &stat.max))
            return FALSE;

        stat.name[0] = NULL; // Eliminate all old style stats!
        return TRUE;
    }
    else
    {
        if (!Parse(t, "%5t %d %d %d", uniqueidstr, &stat.def, &stat.min, &stat.max))
            return FALSE;

        strupr(uniqueidstr);
        stat.uniqueid = *(DWORD *)uniqueidstr; // Get groovy 4 char unique id string for stats
    }

    return TRUE;
}

char *TStatisticDefList::GetStatDefString(int statid, char *buf)
{
    if ((DWORD)statid >= (DWORD)statdefs.NumItems())
        return "";

    PSStatisticDef stat = &(statdefs[statid]);

    char uniqueidstr[5];
    uniqueidstr[4] = 0;
    *(DWORD *)uniqueidstr = stat->uniqueid;

    sprintf(buf, "%s %s %d %d %d", 
        stat->name, uniqueidstr, stat->def, stat->min, stat->max);

    return buf;
}

int TStatisticDefList::FindStat(char *statname)
{
    for (TSizableIterator<SStatisticDef> i(&statdefs); i; i++)
    {
        if (stricmp(i.Item()->name, statname) == 0)
            return i.ItemNum();
    }

    return -1;
}

// ****************
// * TObjectClass *
// ****************

TObjectClass::TObjectClass(char *classname, int classid, WORD flags, PTObjectClass base)
{
    name     = classname;
    id       = classid;
    objflags = flags;
    basedon  = base;

    classes[id] = this;

    if (id >= numclasses)
        numclasses = id + 1;

    Clear();
}

TObjectClass::~TObjectClass()
{
    Clear();
}

void TObjectClass::Clear()
{
    for (int c = 0; c < objinfo.NumItems(); c++)  // Remove all object types
        RemoveType(c);

    statdefs.Clear();
    objstatdefs.Clear();
    objinfo.Clear();
}

int TObjectClass::AddType(char *name, char *imgfilename, DWORD uniqueid)
{
    PTObjectBuilder objbuilder = TObjectBuilder::GetBuilder(name);
    if (objbuilder == NULL)
        objbuilder = TObjectBuilder::GetBuilder(ClassName());
    if (uniqueid == 0)
        uniqueid = GenerateUniqueID();
    else
        uniqueid = uniqueid;
    int imageryid = TObjectImagery::RegisterImagery(imgfilename);
    if (imageryid < 0)
        return -1;

    PSObjectInfo inf = new SObjectInfo(name, objbuilder, imageryid, uniqueid);
    int objtype = objinfo.AddPtr(inf);

    inf->stats.SetNumItems(statdefs.NumStats());
    for (int i = 0; i < statdefs.NumStats(); i++)
        ResetStat(objtype, i);

    inf->objstats.SetNumItems(objstatdefs.NumStats());
    for (i = 0; i < objstatdefs.NumStats(); i++)
        ResetObjStat(objtype, i);

    classesdirty = TRUE;

    return objtype;
}

BOOL TObjectClass::RemoveType(int objtype)
{
    if ((objtype < 0) || (objtype >= objinfo.NumItems()))
        return FALSE;

    TObjectImagery::FreeImageryEntry(objinfo[objtype].imageryid);

    objinfo.Remove(objtype);

    classesdirty = TRUE;

    return TRUE;
}

PTObjectInstance TObjectClass::NewObject(PSObjectDef objectdef)
{
    if (objectdef->pos.x < 0)
        objectdef->pos.x = 0;
    if (objectdef->pos.y < 0)
        objectdef->pos.y = 0;
    if (objectdef->pos.z < 0)
        objectdef->pos.z = 0;

    if ((DWORD)((int)((short)objectdef->objtype)) >= (DWORD)objinfo.NumItems())
        return NULL;

    if (!objinfo[objectdef->objtype].objbuilder)
        return NULL;

    objinfo[objectdef->objtype].imagery = TObjectImagery::LoadImagery(objinfo[objectdef->objtype].imageryid);

    if (!objinfo[objectdef->objtype].imagery)
        return NULL;

    PTObjectInstance inst = objinfo[objectdef->objtype].objbuilder->Build(objectdef,
        objinfo[objectdef->objtype].imagery);

    if (objectdef->objclass == OBJCLASS_TILE || objectdef->objclass == OBJCLASS_EXIT)
        inst->SetFlags(OF_IMMOBILE);

    if (inst->GetMapIndex() <= 0)
        inst->SetMapIndex(MapPane.MakeIndex());

    return inst;
}

int TObjectClass::FindClass(char *name)
{
    for (int i = 0; i < numclasses; i++)
        if (classes[i] && classes[i]->name && stricmp(classes[i]->name, name) == 0)
            return i;

    return NULL;
}

int TObjectClass::FindObjType(char *objtypename, BOOL partial)
{
#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(NULL);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif
    for (TVirtualIterator<SObjectInfo> i(&objinfo); i; i++)
    {
        if (!i.Used())
            continue;

        if (partial)
        {
            if (abbrevcmp(objtypename, i.Item()->name) > 0)
                return i.ItemNum();
        }
        else
        {
            if (!stricmp(i.Item()->name, objtypename))
                return i.ItemNum();
        }
    }

    return -1;
}

int TObjectClass::FindObjType(DWORD uniqueid)
{
    for (TVirtualIterator<SObjectInfo> i(&objinfo); i; i++)
    {
        if (!i.Used())
            continue;

        if (i.Item()->uniqueid == uniqueid)
            return i.ItemNum();
    }

    return -1;
}

void TObjectClass::SetClassesDirty()
{
    classesdirty = TRUE;
}

// -------------------- Statistic Functions ----------------------

int TObjectClass::AddStat(SStatisticDef &newstatdef, int newid)
{
    int statid = FindStat(newstatdef.name);
    if (statid >= 0)
        return statid;
        
    statid = statdefs.AddStat(newstatdef, newid);
    if (statid < 0)
        return -1;

    for (int c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].stats.New();
        objinfo[c].stats.Set(statid, newstatdef.def);
    }

    classesdirty = TRUE;
    
    return statid;
}

void TObjectClass::DeleteStat(int statid)
{
    if ((DWORD)statid >= (DWORD)statdefs.NumStats())
        return;

    statdefs.DeleteStat(statid);

    for (int c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].stats.Collapse(statid);
    }

    classesdirty = TRUE;
}

BOOL TObjectClass::ParseNewStat(TToken &t, BOOL reload)
{
    SStatisticDef stat;

    if (!statdefs.ParseStat(stat, t))
        return FALSE;

  // Stat was eliminated
    if (stat.name[0] == NULL)
        return TRUE;

  // Make sure it doesn't already exist
    if (FindStat(stat.name) >= 0 || FindObjStat(stat.name) >= 0)
        return TRUE;

    // Only Add stats if we're NOT reloading the Class.Def
    if (!reload)
        AddStat(stat);

    return TRUE;
}

int TObjectClass::FindStatVal(int statid, int searchvalue)
{
    for (TVirtualIterator<SObjectInfo> i(&objinfo); i; i++)
    {
        if (!i.Used())
            continue;

        if (i.Item()->stats[statid] == searchvalue)
            return i.ItemNum();
    }

    return -1;
}

int TObjectClass::FindRandStatVal(int statid, int searchvalue, int *heightflux)
// This thing is designed exclusively for the use of the editor's map generator.
{
    int foundlist[64];
    int numfound = 0;
    int closest = 0, thisone;
    DWORD mask;

    if (heightflux)
        *heightflux = 0;

    for (TVirtualIterator<SObjectInfo> i(&objinfo); i; i++)
    {
        if (!i.Used())
            continue;

        DWORD value = i.Item()->stats[statid];

        // build a mask based on the wildcards in the stat value
        mask = 0;
        thisone = 0;
        if (value & 0xF0000000)
        {
            mask |= 0xFF000000;
            thisone++;
        }
        if (value & 0xF00000)
        {
            mask |= 0xFF0000;
            thisone++;
        }
        if (value & 0xF000)
        {
            mask |= 0xFF00;
            thisone++;
        }
        if (value & 0xF0)
        {
            mask |= 0xFF;
            thisone++;
        }

        if (thisone >= closest)
        {
            if (value == (searchvalue & mask))
            {
                if (thisone == closest)
                    foundlist[numfound++] = i.ItemNum();
                else
                {
                    foundlist[0] = i.ItemNum();
                    numfound = 1;
                }

                closest = thisone;
                if (heightflux)
                    *heightflux = 0;
            }
            else if (mask != 0xFFFFFFFF && heightflux)
            {
                int base = 1000;
                for (int j = 0; j < 4; j++)
                {
                    if (value & (0xF0 << (j * 8)))
                    {
                        int v = (value >> (j * 8)) & 0x0F;

                        if (v < base)
                            base = v;
                    }
                }

                if (base < 0xF)
                {
                    for (DWORD b = 1; b <= 1/*base*/; b++)
                    {
                        DWORD v = value;
                        for (int j = 0; j < 4; j++)
                            if (v & (0xF0 << (j * 8)) && ((v >> (j * 8)) & 0x0F) >= b)
                                v -= b << (j * 8);

                        if (v == (searchvalue & mask))
                        {
                            if (thisone == closest)
                                foundlist[numfound++] = i.ItemNum();
                            else
                            {
                                foundlist[0] = i.ItemNum();
                                numfound = 1;
                            }

                            closest = thisone;
                            if (heightflux)
                                *heightflux = b;
                        }
                    }
                }
            }
        }
    }

    if (numfound > 0)
        return foundlist[random(0, numfound - 1)];

    return -1;
}

int TObjectClass::AddObjStat(SStatisticDef &newstatdef, int newid)
{
  // MAKE SURE WE DON'T ABUSE THE STAT SYSTEM BY ADDING STATS TO TILES
    if (id == OBJCLASS_TILE)
        FatalError("Tile objects can't have object statistics (DUMMY)!");

    int statid = FindObjStat(newstatdef.name);
    if (statid >= 0)
        return statid;
        
    statid = objstatdefs.AddStat(newstatdef, newid);
    if (statid < 0)
        return -1;

    for (int c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].objstats.New();
        objinfo[c].objstats.Set(statid, newstatdef.def);
    }

    if (MapPane.IsOpen()) // Reset id's only if map is currently active
    {
        for (TMapIterator i; i; i++)
        {
            if (i.Item()->ObjClass() == id)
                i.Item()->SetObjStat(statid, newstatdef.def);
        }
    }

    classesdirty = TRUE;
    
    return statid;
}

void TObjectClass::DeleteObjStat(int statid)
{
    if ((DWORD)statid >= (DWORD)objstatdefs.NumStats())
        return;

    objstatdefs.DeleteStat(statid);

    for (int c = 0; c < objinfo.NumItems(); c++)
    {
        if (!objinfo.Used(c))
            continue;
        
        objinfo[c].objstats.Collapse(statid);
    }

    if (MapPane.IsOpen()) // Reset id's only if map is currently active
    {
        for (TMapIterator i; i; i++)
        {
            if (i.Item()->ObjClass() == id)
                i.Item()->DelStat(statid);
        }
    }

    classesdirty = TRUE;
}

BOOL TObjectClass::ParseNewObjStat(TToken &t, BOOL reload)
{
    SStatisticDef stat;

    if (!objstatdefs.ParseStat(stat, t))
        return FALSE;

  // Stat was eliminated
    if (stat.name[0] == NULL)
        return TRUE;

  // Make sure it doesn't already exist
    if (FindStat(stat.name) >= 0 || FindObjStat(stat.name) >= 0)
        return TRUE;

    // Only Add stats if we're NOT reloading the Class.Def
    if (!reload)
        AddObjStat(stat);

    return TRUE;
}

void TObjectClass::CopyStats(PTObjectClass from)
{
    int c;
    for (c = 0; c < basedon->NumStats(); c++)
        AddStat(*from->GetStatisticDef(c), c);
    for (c = 0; c < basedon->NumObjStats(); c++)
        AddObjStat(*from->GetObjStatisticDef(c), c);
}

// -------------------- Statistic Functions ----------------------

BOOL TObjectClass::LoadClasses(BOOL lock, BOOL reload)
{
    char fname[MAX_PATH];
    FILE *classfp;
    struct _stat st;

    sprintf(fname, "%sclass.def", ClassDefPath);

    classfp = TryOpen(fname, lock ? "w+" : "r");
    if (classfp == NULL)
        return FALSE;

    TFileParseStream s(classfp, fname);
    TToken t(s);

  // Attempt to quickload headers (if IMAGERY.DAT file is older than CLASS.DEF)
  // Note: might be nice to quickly do a date/time stamp search of the IMAGERY directory
  // and use the lastest time stamp from that.
    if (!NoQuickLoad && !reload)
    {
        _fstat(fileno(classfp), &st);
        TObjectImagery::QuickLoadHeaders(st.st_mtime);
    }

  // Now get first token... 
    t.LineGet();

    // See if they are reloading the Class.Def
    if (reload)
    {
        WORD tmp;

        // Read in the Unique Type ID into a tmp variable
        if (!Parse(t, "Unique Type ID = %d", &tmp))
            FatalError("Reading Unique Type ID from class.def");

        // Compare it against the UniqueTypeID that the program has already been
        // using and use whichever is larger
        if ((tmp > UniqueTypeID) || ((tmp < 32000) && (UniqueTypeID > 32000)))
            UniqueTypeID = tmp;
    }
    else
    {
        // Read in the Unique Type ID
        if (!Parse(t, "Unique Type ID = %d", &UniqueTypeID))
            FatalError("Reading Unique Type ID from class.def");
    }
    t.LineGet();


    do
    {
        if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
            t.LineGet();

        if (t.Is("CLASS"))
        {
            if (!ParseClass(t, reload))
                return FALSE;
        }
        else
        {
            Error("Script identifier expected in class.def.");
            return FALSE;
        }

    } while (t.Type() != TKN_EOF);

    if (!lock)
        fclose(classfp);

    classesdirty = FALSE;

    return TRUE;
}

BOOL TObjectClass::SaveClasses(BOOL lock)
{
    if (!classesdirty)
        return TRUE;

    char fname[MAX_PATH];
    char bakname[MAX_PATH];
    char newname[MAX_PATH];
    FILE *classfp;

    // Reload the Class.Def to read any types that have been added since we
    // loaded it
    if (LoadClasses(FALSE, TRUE) == FALSE)
        return FALSE;

    sprintf(fname, "%sClass.Def", ClassDefPath);
    sprintf(bakname, "%sClass.Bak", ClassDefPath);
    sprintf(newname, "%sClass.New", ClassDefPath);

    // Try to open the file
    classfp = TryOpen(newname, "w+");

    // If the file didn't open, then return with failure
    if (classfp == NULL)
        return FALSE;

    fprintf(classfp, "// ********* Revenant Class Def Save File ********\n"
                "// -----------------------------------------------\n\n"
                "// Revenant - Copyright 1998 Cinematix Studios, Inc.\n\n");

    fprintf(classfp, "Unique Type ID = 0x%04x\n\n", UniqueTypeID);

    for (int i = 0; i < numclasses; i++)
    {
        PTObjectClass cl = classes[i];
        if (cl)
            if (!cl->WriteClass(classfp))
                return FALSE;
    }

    if (!lock)
    {
        fclose(classfp);

        // Delete the Class.Bak file (if it exists)
        if (TryDelete(bakname) == FALSE)
            return FALSE;

        // Rename Class.Def to Class.Bak
        if (TryRename(fname, bakname) == FALSE)
            return FALSE;

        // Rename Class.New to Class.Def
        if (TryRename(newname, fname) == FALSE)
            return FALSE;
    }

    classesdirty = FALSE;

    return TRUE;
}

void TObjectClass::FreeClasses()
{
    for (int c = 0; c < numclasses; c++)
    {
        if (classes[c])
            classes[c]->Clear();
    }
}

BOOL TObjectClass::WriteClass(FILE *fp)
{
    // header
    fprintf(fp, "CLASS \"%s\"\nBEGIN\n\n", name);
    char buf[80];

    // stats
    if (statdefs.NumStats() > 0)
    {
        fprintf(fp, "\n\t// Stat Defs: <StatName> <FourCharId> <Default> <Min> <Max>\n\tSTATS\n\tBEGIN\n");

        for (int c = 0; c < statdefs.NumStats(); c++)
            fprintf(fp, "\t\t%s\n", statdefs.GetStatDefString(c, buf));

        fprintf(fp, "\tEND\n\n");
    }

    // object stats
    if (objstatdefs.NumStats() > 0)
    {
        fprintf(fp, "\n\t// Stat Defs: <StatName> <FourCharId> <Default> <Min> <Max>\n\tOBJSTATS\n\tBEGIN\n");

        for (int c = 0; c < objstatdefs.NumStats(); c++)
            fprintf(fp, "\t\t%s\n", objstatdefs.GetStatDefString(c, buf));

        fprintf(fp, "\tEND\n\n");
    }

    // types
    if (objinfo.NumItems() > 0)
    {
        fprintf(fp, "\n\tTYPES\n\tBEGIN\n");

        int objtype = 0;
        for (TVirtualIterator<SObjectInfo> o(&objinfo); o; o++, objtype++)
        {
            if (!o.Item() || !o.Used())
                continue;

            PSImageryEntry ie = TObjectImagery::GetImageryEntry(o.Item()->imageryid);
            if (!ie)
                continue;

            char *filename = ie->filename;

            if (strncmp(filename, "IMAGERY", 7) == 0)
            {
                int id = atoi(filename + 8);
                fprintf(fp, "\t\t\"%s\" %03d 0x%08x", o.Item()->name, id, o.Item()->uniqueid);
            }
            else
                fprintf(fp, "\t\t\"%s\" \"%s\" 0x%08x", o.Item()->name,
                    filename, o.Item()->uniqueid);

            if (statdefs.NumStats() > 0)
            {
                int c;
                BOOL firsttime;

                fprintf(fp, " {");
                firsttime = TRUE;
                for (c = 0; c < statdefs.NumStats(); c++)
                {
                    if (!firsttime)
                        fprintf(fp, ",");

                    fprintf(fp, "%d", (int)objinfo[objtype].stats[c]);
                    firsttime = FALSE;
                }
                fprintf(fp, "}");

                fprintf(fp, " {");
                firsttime = TRUE;
                for (c = 0; c < objstatdefs.NumStats(); c++)
                {
                    if (!firsttime)
                        fprintf(fp, ",");

                    fprintf(fp, "%d", (int)objinfo[objtype].objstats[c]);
                    firsttime = FALSE;
                }
                fprintf(fp, "}");
            }

            fprintf(fp, "\n");
        }

        fprintf(fp, "\tEND\n\n");
    }

    if (fprintf(fp, "END\n\n") < 1)
        return FALSE;

    return TRUE;
}

BOOL TObjectClass::ParseClass(TToken &t, BOOL reload)
{
#ifdef _DEBUG
    _CrtMemState memstate;
#endif

    t.WhiteGet();
    if (t.Type() != TKN_TEXT)
    {
        Error("Expecting class name");
        return FALSE;
    }

    PTObjectClass cl = TObjectClass::GetClass(TObjectClass::FindClass(t.Text()));
    if (!cl)
    {
        _RPT1(_CRT_ASSERT, "Class %s not found", t.Text());
        t.LineGet();
        if (!t.SkipBlock())
            Error("Unexpected EOF");
        return TRUE;
    }

    t.LineGet();
    t.DoBegin();

  // Copy the base class's hard coded stats
    if (cl->basedon)
        cl->CopyStats(cl->basedon);

    while (!t.IsEnd())
    {
#ifdef _DEBUG
        _CrtMemCheckpoint(&memstate);
#endif
        if (t.Type() == TKN_EOF)
            Error("Unexpected EOF");

        if (t.Type() != TKN_IDENT)
        {
            t.WhiteGet();
            continue;
        }

      // Get class stats
        if (t.Is("STATS"))
        {
            char *staterr = "Parsing class.def: STATS";

            t.LineGet();
            t.DoBegin();
            while (!(t.Type() == TKN_KEYWORD && t.Code() == KEY_END))
            {
                if (!cl->ParseNewStat(t, reload))
                    t.Error(staterr);
                t.LineGet();
            }
            t.DoEnd();

#ifdef _DEBUG
            if (!_CrtCheckMemory())
            {
                _CrtMemDumpAllObjectsSince(&memstate);
                _RPT0(_CRT_ERROR, "Memory Error");
            }
#endif
        }

      // Get object stats
        else if (t.Is("OBJSTATS"))
        {
            char *staterr = "Parsing class.def: OBJSTATS";

            t.LineGet();
            t.DoBegin();
            while (!(t.Type() == TKN_KEYWORD && t.Code() == KEY_END))
            {
                if (!cl->ParseNewObjStat(t, reload))
                    t.Error(staterr);
                t.LineGet();
            }
            t.DoEnd();

#ifdef _DEBUG
            if (!_CrtCheckMemory())
            {
                _CrtMemDumpAllObjectsSince(&memstate);
                _RPT0(_CRT_ERROR, "Memory Error");
            }
#endif

        }
        else if (t.Is("TYPES"))
        {
#ifdef _DEBUG
            _CrtMemCheckpoint(&memstate);
#endif

            t.LineGet();
            t.DoBegin();
            while (!(t.Type() == TKN_KEYWORD && t.Code() == KEY_END))
            {
                char filename[MAXIMFNAMELEN];
                char name[MAXNAMELEN];
                int objtype;
                BOOL newtype = FALSE;
                DWORD uniqueid;

                strcpy(name, t.Text());

                if (t.Type() != TKN_TEXT)
                    t.Error("Parsing class.def: TYPES (header)");

                t.WhiteGet();
                if (t.Type() == TKN_NUMBER)
                {
                    sprintf(filename, "IMAGERY.%03d", t.Index());
                }
                else if (t.Type() == TKN_TEXT)
                    strcpy(filename, t.Text());
                else
                    t.Error("Parsing class.def: TYPES (imagery)");

                t.WhiteGet();

                // Get the Unique ID for this type
                if (t.Type() != TKN_NUMBER)
                    t.Error("Missing Unique ID in class.def: TYPES (imagery)");
                else
                {
                    if (!Parse(t, "%d", &uniqueid))
                        t.Error("Parsing Unique ID in class.def: TYPES (imagery)");
                }

                // See if we are reloading the Class.Def
                if (reload)
                {
                    // See if the type already exists
                    objtype = cl->FindObjType(name);

                    if (objtype == -1)
                    {
                        // It doesn't exist, so add it
                        newtype = TRUE;

                        objtype = cl->AddType(name, filename, uniqueid);
                    }
                }
                else
                {
                    objtype = cl->AddType(name, filename, uniqueid);
                }

                if (objtype < 0)
                {
                    while (t.Type() != TKN_RETURN && t.Type() != TKN_EOF)
                        t.Get();
                    t.LineGet();
                    continue;
                }
                
                int statid, value;

                char *typestaterr = "Parsing class.def: TYPES (stats)";

             // Parse old style statistic list
                while (t.Type() != TKN_RETURN && !t.Is("{"))
                {
                    char statname[80];

                    if (!Parse(t, "%t = %d", statname, &value))
                        t.Error(typestaterr);

                    if ((!reload) || (newtype))
                    {
                        statid = cl->FindStat(statname);
                        if (statid >= 0)
                            cl->SetStat(objtype, statid, value);
                        else
                        {
                            statid = cl->FindObjStat(statname);
                            if (statid >= 0)
                                cl->SetObjStat(objtype, statid, value);
                        }
//                      if (statid < 0)
//                          Error("Invalid stat '%s'", statname);
                    }

                }

             // Parse class statistics
                if (t.Is("{")) // Use bracket style class def stat list
                {
                    int statid = 0;
                    t.WhiteGet();
                
                    while (!t.Is("}") && statid < cl->NumStats())
                    {
                        int value;
                        
                        if (!Parse(t, "%i", &value))
                            t.Error(typestaterr);
                        
                        if (!reload)
                        {
                            cl->objinfo[objtype].stats.New();
                            cl->objinfo[objtype].stats[statid] = value;
                        }

                        statid++;
        
                        if (t.Is(","))
                            t.WhiteGet();

                    }
                    
                    while (!t.Is("}") && !(t.Type() == TKN_RETURN || t.Type() == TKN_EOF))
                    {
                        t.WhiteGet(); // Skip any extra stats on list without erroring (just in case)

                    }

                    if (!t.Is("}"))
                        t.Error(typestaterr);

                    t.WhiteGet();
                }

              // Parse object statistics
                if (t.Is("{")) // Do object Use bracket style class def stat list
                {
                    int statid = 0;
                    t.WhiteGet();
                
                    while (!t.Is("}") && statid < cl->NumObjStats())
                    {
                        int value;
                        
                        if (!Parse(t, "%i", &value))
                            t.Error(typestaterr);
                        
                        if (!reload)
                        {
                            cl->objinfo[objtype].objstats.New();
                            cl->objinfo[objtype].objstats[statid] = value;
                        }

                        statid++;
        
                        if (t.Is(","))
                            t.WhiteGet();

                    }
                    
                    while (!t.Is("}") && !(t.Type() == TKN_RETURN || t.Type() == TKN_EOF))
                    {
                        t.WhiteGet(); // Skip any extra stats on list without erroring (just in case)

                    }

                    if (!t.Is("}"))
                        t.Error(typestaterr);

                    t.WhiteGet();

                }

                t.LineGet();
            }
            t.DoEnd();

#ifdef _DEBUG
            if (!_CrtCheckMemory())
            {
                _CrtMemDumpAllObjectsSince(&memstate);
                _RPT0(_CRT_ERROR, "Memory Error");
            }
#endif
        }
        else
        {
            Error("Expecting STATS or TYPES for class %s", cl->ClassName());
            return FALSE;
        }
    }

    t.DoEnd();

    return TRUE;
}

// GenerateUniqueID.
//------------------------------------------------------------------------------
// This will return a Unique ID used for specifying unique object
// types.  The number is generated as follows:
//
// (high word) (high byte) = Random #
// (high word) (low  byte) = Low byte of Current Time (tick count)
// (low  word)             = UniqueTypeID stored in Class.Def
//
// UniqueTypeID is incremented on every call.

DWORD GenerateUniqueID()
{
    DWORD UniqueID;
    DWORD r = (DWORD)random(0, 255);
    DWORD t = GetTickCount() & 255;

    UniqueID = (r << 24) | (t << 16) | UniqueTypeID;

    UniqueTypeID++;

    return UniqueID;
}

