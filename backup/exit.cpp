// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                       Exit.cpp - TExit object                         *
// *************************************************************************

#include <windows.h>

#include "revenant.h"
#include "exit.h"
#include "savegame.h"
#include "mappane.h"
#include "playscreen.h"
#include "player.h"
#include "file.h"
#include "dls.h"
#include "textbar.h"
#include "3dimage.h"
#include "parse.h"
#include "sound.h"
#include "script.h"

REGISTER_BUILDER(TExit)
TObjectClass ExitClass("EXIT", OBJCLASS_EXIT, 0);

// Hard coded class stats
DEFSTAT(Exit, Openable,             OPEN, 0, 0, 0, 1)
DEFSTAT(Exit, Facing,               FACE, 1, 0, 0, 255)
DEFSTAT(Exit, UseCenter,            USE,  2, 0, 0, 2)
DEFSTAT(Exit, StopMoving,           STMV, 3, 0, 1, 1)
DEFSTAT(Exit, Delay,                DLY,  4, 0, 0, 1000)

// Hard coded object stats
DEFOBJSTAT(Exit, Locked,            LOCK, 0, 0, 0, 1)
DEFOBJSTAT(Exit, KeyId,             KEY,  1, 0, 0, 100000)
DEFOBJSTAT(Exit, PickDifficulty,    PICK, 2, 0, 0, 100000)


// these should be static members of TExit, but it doesn't seem to recognize
// their existance when I do it that way, so here they are
PSExitRef exitlist;     // master list of exits
BOOL exitlistdirty;     // if we need to save out the exit list

BOOL TExit::Initialize()
{
    exitlistdirty = FALSE;
    return ReadExitList();
}

void TExit::Close()
{
    WriteExitList();
    DestroyExitList();
}

BOOL TExit::ReadExitList(BOOL reload)
{
    if (!reload)
        exitlist = NULL;

    char fname[MAX_PATH];
    sprintf(fname, "%sexit.def", ClassDefPath);
    
    FILE *fp = TryOpen(fname, "rb");
    if (!fp)
        return FALSE;

    TFileParseStream s(fp, fname);
    TToken t(s);

    t.Get();

    PSExitRef ref;
    char name[128];

    do
    {
        if (t.Type() == TKN_RETURN || t.Type() == TKN_WHITESPACE)
            t.LineGet();

        if (t.Type() == TKN_EOF)
            break;

        ref = new SExitRef;

        if (!Parse(t, "%t (%d, %d, %d) level %d mapindex %d ambient %d (%d, %d, %d)",
                    name, &ref->target.x, &ref->target.y, &ref->target.z, &ref->level, &ref->mapindex,
                    &ref->ambient, &ref->ambcolor.red, &ref->ambcolor.green, &ref->ambcolor.blue))
            return FALSE;

        if (reload)
        {
            // don't overwrite any that already exist in the loaded version
            if (FindExit(name))
            {
                delete ref;
                ref = NULL;
            }
        }

        if (ref)
        {
            ref->name = strdup(name);
            ref->next = exitlist;
            exitlist = ref;
        }

        t.SkipLine();       // skip past any other garbage on the line, including the newline

    } while (t.Type() != TKN_EOF);

    fclose(fp);

    exitlistdirty = FALSE;
    return TRUE;
}

BOOL TExit::WriteExitList()
{
    if (!exitlistdirty)
        return TRUE;        // nothing to do, no changes have been made since last load/save

    if (!ReadExitList(TRUE))    // get any exits that have been added since last load
        return FALSE;

    char fname[MAX_PATH];
    sprintf(fname, "%sexit.def", ClassDefPath);
    
    FILE *fp = TryOpen(fname, "wb");
    if (!fp)
        return FALSE;

    for (PSExitRef ref = exitlist; ref; ref = ref->next)
        if (!fprintf(fp, "%s (%d, %d, %d) level %d mapindex 0x%x ambient %d (%d, %d, %d)\r\n",
                        ref->name, ref->target.x, ref->target.y, ref->target.z, ref->level, ref->mapindex,
                        ref->ambient, ref->ambcolor.red, ref->ambcolor.green, ref->ambcolor.blue))
        {
            fclose(fp);
            return FALSE;
        }

    fclose(fp);
    exitlistdirty = FALSE;
    return TRUE;
}

void TExit::DestroyExitList()
{
    PSExitRef next;

    for (PSExitRef ref = exitlist; ref; ref = next)
    {
        next = ref->next;

        delete ref->name;
        delete ref;
    }

    exitlist = NULL;
    exitlistdirty = FALSE;
}

void TExit::GetExitStrip(int &regx, int &regy, int &regz, int &width, int &length, int &height)
{
    // get all the bounding box data
    GetFacingBoundBox(regx, regy, width, length);

    int dummy;
    GetImagery()->GetWorldBoundBox(state, dummy, dummy, height);

    height = max(1, height);

    if (UseCenter())
    {
        // special case for objects that use the center as the activation area
        if (UseCenter() == 2)
        {
            // rotating walls
            regx = (regx / 4) * 3;
            regy = (regy + 1) / 2;
            width = (width / 4) * 3;
            length = (length + 1) / 2;
        }
        else
        {
            // elevators and teleporters
            regx = (regx / 2);
            regy = (regy / 2);
            width /= 2;
            length /= 2;
        }

        return;
    }

    // get the facing data
    int dir = Facing();
    if (dir < 0)
        dir = GetFace();

    // find the strip based on the direction the exit is facing
    if (dir >= 0xE0 || dir < 0x20)
    {
        // north-facing exit
        //regy += 1;
        length = 1;
    }
    else if (dir < 0x60)
    {
        // east-facing exit
        regx -= width - 1;
        width = 1;
    }
    else if (dir < 0xA0)
    {
        // south-facing exit
        regy -= length - 1;
        length = 1;
    }
    else if (dir < 0xE0)
    {
        // west-facing exit
        //regx += 1;
        width = 1;
    }
}

BOOL TExit::AddExit(char *name, PTObjectInstance inst, BOOL getamb)
{
    if (!name || !*name || !inst)
        return FALSE;

    PSExitRef ref = FindExit(name);

    // if it already exists we can skip some stuff
    if (!ref)
    {
        ref = new SExitRef;
        ref->name = strdup(name);

        ref->next = exitlist;
        exitlist = ref;
    }

    if (inst->ObjClass() != OBJCLASS_EXIT)
    {
        inst->GetPos(ref->target);
        ref->mapindex = -1;
    }
    else
    {
        if (inst->GetImagery() == NULL)
            memset(&ref->target, 0, sizeof(S3DPoint));
        else
        {
            int regx, regy, regz, width, length, height;
            ((PTExit)inst)->GetExitStrip(regx, regy, regz, width, length, height);
            S3DPoint start(0, 0, 0);
            start.x -= regx * GRIDSIZE;
            start.y -= regy * GRIDSIZE;
            S3DPoint end = start;
            end.x += width * GRIDSIZE;
            end.y += length * GRIDSIZE;

            ref->target.x = (start.x + end.x) / 2;
            ref->target.y = (start.y + end.y) / 2;
            ref->target.z = (start.z + end.z) / 2;
        }

        S3DPoint pos;
        inst->GetPos(pos);
        ref->target += pos;

        // remember to close the door on the way out...
        ref->mapindex = inst->GetMapIndex();

        if (getamb)
        {
            ref->ambient = MapPane.GetAmbientLight();
            GetAmbientColor(ref->ambcolor);
        }
        else
        {
            ref->ambcolor.red = ref->ambcolor.green = ref->ambcolor.blue = 255;
            ref->ambient = -1;
        }
    }

    ref->level = MapPane.GetMapLevel();     // hrm...no level on objects...is that bad?

    exitlistdirty = TRUE;

    return TRUE;
}

PSExitRef TExit::FindExit(char *exitname)
{
    for (PSExitRef ref = exitlist; ref; ref = ref->next)
        if (stricmp(ref->name, exitname) == 0)
            return ref;

    return NULL;
}

BOOL TExit::Use(PTObjectInstance user, int with)
{
    TObjectInstance::Use(user, with);

    if (Openable())
    {
        if (CheckKeyUse(user, MapPane.GetInstance(with)))
            return TRUE;

        if (Locked())
        {
            TextBar.Print("It seems to be locked.");
            return FALSE;
        }

        if (state == EXIT_OPEN || state == EXIT_OPENING)
            SetExitState(EXIT_CLOSING);
        else if (state == EXIT_CLOSED || state == EXIT_CLOSING)
            SetExitState(EXIT_OPENING);

        return TRUE;
    }

    return FALSE;
}

int TExit::CursorType(PTObjectInstance with)
{
    if (Openable())
    {
        if (with)
            return CURSOR_HAND;

        return CURSOR_DOOR;
    }

    return CURSOR_NONE;
}

BOOL TExit::Activate()
{
    if (GetScript())
        GetScript()->Trigger(TRIGGER_ACTIVATE);

    if (exitflags & EX_FROMEXIT)        // if we just came from an exit, don't reflect back
        return FALSE;

    PSExitRef ref = FindExit(name);     // find this exit in the master list
    if (!ref)
        return FALSE;

    S3DPoint targ = ref->target;

    // minor hack, for now
    if (stricmp(GetTypeName(), "Door") == 0 && Player)
    {
        S3DPoint vect;
        ConvertToVector(Player->GetFace(), 24, vect);
        targ += vect;
    }

    // Set new position
    Player->SetPos(targ, ref->level);

    return TRUE;
}

void TExit::Unactivate()
{
    if (Openable())
        SetExitState(EXIT_CLOSING);
}

BOOL TExit::SetExitState(int es)
{
    int st;

    if (es == EXIT_OPEN)
        st = FindState("open");
    else if (es == EXIT_CLOSED)
        st = FindState("closed");
    else if (es == EXIT_OPENING)
    {
        st = FindState("opening");
        if (st < 0)
            st = FindState("closed to open");
    }
    else if (es == EXIT_CLOSING)
    {
        st = FindState("closing");
        if (st < 0)
            st = FindState("open to closed");
    }

    if (st < 0)
        st = es;

    return SetState(st);
}

void TExit::Pulse()
{
    TContainer::Pulse();

    if (!Editor && CommandDone() && Openable())
    {
        if (state == EXIT_CLOSING)
            SetExitState(EXIT_CLOSED);
        else if (state == EXIT_OPENING)
            SetExitState(EXIT_OPEN);
    }

    if (Player && !Editor && GetImagery())
    {
        int regx, regy, regz, width, length, height;
        GetExitStrip(regx, regy, regz, width, length, height);

        // get the player's relative position to the exit
        S3DPoint delta;
        Player->GetPos(delta);
        delta -= pos;
        delta.x = (delta.x + (regx * GRIDSIZE)) / GRIDSIZE;
        delta.y = (delta.y + (regy * GRIDSIZE)) / GRIDSIZE;
        delta.z = (delta.z + (GetImagery()->GetWorldRegZ(state) * GRIDSIZE)) / GRIDSIZE;

        BOOL activate = TRUE;
//      if (StopMoving())
//      {
//          // for usecenter exits, wait until the character stops moving
//          S3DPoint lnextmove;
//          Player->GetNextMove(lnextmove);
//          if (lnextmove.x != 0 || lnextmove.y != 0 || lnextmove.z != 0)
//              activate = FALSE;
//      }

        // check if the player is over the strip of walkmap immediately past
        // the bounding box in the given direction
        if (delta.x >= 0 && delta.y >= 0 && /*delta.z >= 0 &&*/
            delta.x < width && delta.y < length/* && delta.z < height*/)
        {
            if (Player->IsOnExit() && !(exitflags & EX_ON))
                exitflags |= EX_FROMEXIT; // Looks like we just poped here from another exit

            Player->SetOnExit(); // Indicate we're on an exit
            exitflags |= EX_ON;

            if (activate &&                     // Activation enabled
                !(exitflags & EX_ACTIVATED))    // Hasn't already been activated
            {
                if (wait++ > Delay())
                {
                    wait = 0;
                    if (Activate())
                        exitflags |= EX_ACTIVATED;
                }
            }
        }
        else
        {
            if (exitflags & EX_ACTIVATED)
                Unactivate();

            exitflags &= ~(EX_ON | EX_ACTIVATED | EX_FROMEXIT);
        }
    }
}

void TExit::UseRange(int &mindist, int &maxdist, int &minang, int &maxang)
{
}

void TExit::Load(RTInputStream is, int version, int objversion)
{
    TContainer::Load(is, version, objversion);
    is >> exitflags;
}

void TExit::Save(RTOutputStream os)
{
    TContainer::Save(os);
    os << exitflags;
}

// *************
// * TElevator *
// *************

#if 0

#define ELEV_INACTIVE   0
#define ELEV_ACTIVE     1
#define ELEV_GLOW       2
#define ELEV_FLASH      3

#define TOP             1102
#define BOTTOM          222
#define MIDDLE          (((TOP - BOTTOM) / 2) + BOTTOM)

#define SPEED           8

_CLASSDEF(TElevator)
class TElevator : public TExit
{
  public:
    TElevator(PTObjectImagery newim) : TExit(newim) { speed = 0; glow = NULL; }
    TElevator(PSObjectDef def, PTObjectImagery newim) : TExit(def, newim) { speed = 0; glow = NULL; }

    virtual BOOL Activate();
        // Take the player through the exit

    virtual void Pulse();

  protected:
    int speed;                  // speed and direction (negative or positive)
    int accel;                  // level of acceleration

    PTObjectInstance glow;      // attached glow effect
};

DEFINE_BUILDER("ELEVATOR", TElevator)
REGISTER_BUILDER(TElevator)

int OldElevScrollLock;

BOOL TElevator::Activate()
{
    TExit::Activate();

    if (state == ELEV_INACTIVE)
    {
        SetExitState(ELEV_ACTIVE);
        speed = 1;
        OldElevScrollLock = ScrollLock;
        ScrollLock = TRUE;
        return TRUE;
    }

    return FALSE;
}

void TElevator::Pulse()
{
    extern TObjectClass EffectClass;

    TExit::Pulse();

    if (Editor)
        return;

    if (speed == 0 && !(exitflags & EX_ON))
    {
        // make sure elevator is at the same level as the player
        if (Player)
        {
            S3DPoint lpos;
            Player->GetPos(lpos);
            if (lpos.z < (MIDDLE - 32) && pos.z >= (MIDDLE + 32))
                speed--;
            else if (lpos.z >= (MIDDLE + 32) && pos.z < (MIDDLE - 32))
                speed++;

            if (speed && state == ELEV_INACTIVE)
                SetExitState(ELEV_ACTIVE);
        }
    }

    if (speed != 0)
    {
        S3DPoint newpos = pos;
        newpos.z += speed;
        SetPos(newpos);

        if (glow)
        {
            glow->GetPos(newpos);
            newpos.z += speed;
            glow->SetPos(newpos);
        }

        if (state == ELEV_ACTIVE)
        {
            if (!(exitflags & EX_ON))
            {
                // make sure elevator is at the same level as the player
                if (Player)
                {
                    S3DPoint lpos;
                    Player->GetPos(lpos);
                    if (lpos.z < MIDDLE && speed >= 0)
                        speed--;
                    else if (lpos.z >= MIDDLE && speed <= 0)
                        speed++;
                }
            }

            BOOL addglow = FALSE;

            if (pos.z == (TOP + 24))
            {
                speed = -1;
                addglow = TRUE;
            }

            if (pos.z == (BOTTOM + 24))
            {
                speed = 1;
                addglow = TRUE;
            }

            if (addglow)
            {
                PTObjectInstance inst = MapPane.FindClosestObject("ElevGlow", pos, FALSE);
                if (inst)
                {
                    glow = inst;
                    glow->ResetFlags(glow->GetFlags() & ~(OF_INVISIBLE));
                    newpos = pos;
                    newpos.x -= 24;
                    newpos.y -= 24;
                    newpos.z -= 10;
                    glow->SetPos(newpos);
                }

                SetExitState(ELEV_GLOW);
                accel = 0;
            }
        }
        else if (state == ELEV_GLOW)
        {
            if (speed < 0 && pos.z <= (BOTTOM + 64))
            {
                if (accel++ >= 2)
                {
                    accel = 0;
                    speed++;
                    if (speed > -1)
                        speed = -1;
                }
            }
            else if (speed > 0 && pos.z >= (TOP - 64))
            {
                if (accel++ >= 2)
                {
                    accel = 0;
                    speed--;
                    if (speed < 1)
                        speed = 1;
                }
            }
            else if (abs(speed) < SPEED)
            {
                if (accel++ >= 2)
                {
                    accel = 0;

                    if (speed > 0)
                        speed++;
                    else
                        speed--;
                }
            }
            else
                accel = 0;

            if ((speed < 0 && pos.z <= BOTTOM) || (speed > 0 && pos.z >= TOP))
            {
                newpos = pos;

                if (speed < 0)
                    newpos.z = BOTTOM;
                else
                    newpos.z = TOP;

                SetPos(newpos);

                speed = 0;
                SetExitState(ELEV_INACTIVE);
                ScrollLock = OldElevScrollLock;

                if (glow)
                {
                    glow->SetFlags(OF_INVISIBLE);
                    glow = NULL;
                }
            }
        }
    }
}

#endif

// ***************
// * TPressPlate *
// ***************

#define PLATE_UP        0
#define PLATE_DOWN      1

_CLASSDEF(TPressPlate)
class TPressPlate : public TExit
{
  public:
    TPressPlate(PTObjectImagery newim) : TExit(newim) { }
    TPressPlate(PSObjectDef def, PTObjectImagery newim) : TExit(def, newim) { }

    virtual BOOL Use(PTObjectInstance user, int with = -1) { return FALSE; }
    virtual int CursorType(PTObjectInstance with = NULL) { return CURSOR_NONE; }

    virtual BOOL Activate();
    virtual void Unactivate();
};

DEFINE_BUILDER("PressPlate", TPressPlate)
REGISTER_BUILDER(TPressPlate)

BOOL TPressPlate::Activate()
{
    TExit::Activate();

    SetState(PLATE_DOWN);

    return TRUE;
}

void TPressPlate::Unactivate()
{
    SetState(PLATE_UP);
}

// ************
// * TUpBlock *
// ************

_CLASSDEF(TUpBlock)
class TUpBlock : public TExit
{
  public:
    TUpBlock(PTObjectImagery newim) : TExit(newim) { }
    TUpBlock(PSObjectDef def, PTObjectImagery newim) : TExit(def, newim) { }

    virtual BOOL Use(PTObjectInstance user, int with = -1);
    virtual int CursorType(PTObjectInstance with = NULL) { return CURSOR_NONE; }

    virtual void Pulse();
};

DEFINE_BUILDER("UpBlock", TUpBlock)
REGISTER_BUILDER(TUpBlock)

void TUpBlock::Pulse()
{
    if (!Editor && CommandDone())
    {
        if (state == EXIT_CLOSING)
            SetState(EXIT_CLOSED);
        else if (state == EXIT_OPENING)
            SetState(EXIT_OPEN);
    }

    TExit::Pulse();
}

BOOL TUpBlock::Use(PTObjectInstance user, int with)
{
    if (with == -1)
    {
        if (state == EXIT_CLOSING || state == EXIT_CLOSED)
            SetState(EXIT_OPENING);
        else if (state == EXIT_OPENING || state == EXIT_OPEN)
            SetState(EXIT_CLOSING);

        PLAY("grind rock");

        return TRUE;
    }

    return FALSE;
}

// **********************
// * TDragonEntAnimator *
// **********************

_CLASSDEF(TDragonEntAnimator)
class TDragonEntAnimator : public T3DAnimator
{
  public:
    TDragonEntAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    virtual ~TDragonEntAnimator() { Close(); }

    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void GetExitStrip(int &regx, int &regy, int &regz, int &width, int &length, int &height);
};

REGISTER_3DANIMATOR("DragonEnt", TDragonEntAnimator)

void TDragonEntAnimator::GetExitStrip(int &regx, int &regy, int &regz, int &width, int &length, int &height)
{
    regx = 2;
    regy = 2;
    width = 4;
    length = 4;
}

void TDragonEntAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
}

BOOL TDragonEntAnimator::Render()
{
    /*
    DWORD savedcull;
    TRY_D3D(Device2->GetRenderState(D3DRENDERSTATE_CULLMODE, &savedcull));

    TRY_D3D(Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE));

    TRY_D3D(Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, savedcull));
    */

    T3DAnimator::Render();

    return TRUE;
}

// **************
// * TSpikeWall *
// **************

_CLASSDEF(TSpikeWall)
class TSpikeWall : public TExit
{
  public:
    TSpikeWall(PTObjectImagery newim) : TExit(newim) { }
    TSpikeWall(PSObjectDef def, PTObjectImagery newim) : TExit(def, newim) { }

    virtual BOOL Use(PTObjectInstance user, int with = -1) { return FALSE; }
    virtual int CursorType(PTObjectInstance with = NULL) { return CURSOR_NONE; }

    virtual BOOL Activate();
    virtual void Unactivate();
};

DEFINE_BUILDER("SpikeWall", TSpikeWall)
REGISTER_BUILDER(TSpikeWall)

BOOL TSpikeWall::Activate()
{
    TExit::Activate();
    
    if (Player)
    {
        Player->Force("impale");
        Player->Damage(10000, DAMAGE_PIERCING);     // make sure he's good n' dead
    }

    SetState(EXIT_OPENING);

    PLAY("spike");

    return TRUE;
}

void TSpikeWall::Unactivate()
{
    SetState(EXIT_CLOSING);

    PLAY("spike");
}

// ***************
// * TLever      *
// ***************

REGISTER_BUILDER(TLever)

BOOL TLever::Use(PTObjectInstance user, int with)
{
    TExit::Use(user, with);

    return TRUE;
}                                              

void TLever::Pulse()
{
    TExit::Pulse();

    if (Editor)
        return;

/*  if (AtActivatePos() == ACTIVATE_IMTHERE)
    {
        S3DPoint newpos;

        Player->Face( (usedir * 64));   // 0 = NE, SE, SW, NW
        Player->GetPos( newpos);
        newpos.x = targetpos.x;
        newpos.y = targetpos.y;
        Player->SetPos( newpos);
        ((PTCharacter)Player)->Pull( this);
        return;
    }
*/
}

