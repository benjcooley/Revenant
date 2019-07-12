// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                        Exit.h - TExit object                          *
// *************************************************************************

#ifndef _EXIT_H
#define _EXIT_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

#ifndef _CONTAINER_H
#include "container.h"
#endif

enum
{
    EXIT_CLOSED,
    EXIT_OPEN,
    EXIT_CLOSING,
    EXIT_OPENING,
};

_STRUCTDEF(SExitRef)
struct SExitRef
{
    char *name;             // name of exit
    S3DPoint target;        // position on level
    int level;              // level to change to
    int mapindex;           // object character is transfered to (usually another exit)
    int ambient;            // level of ambient light
    SColor ambcolor;        // color of ambient light

    PSExitRef next;         // next in list
};

// Exit flags
#define EX_ON           (1 << 0)        // player is on exit strip
#define EX_ACTIVATED    (1 << 1)        // exit has been activated
#define EX_FROMEXIT     (1 << 2)        // player just came from another exit.. don't do anything

// Exit is derived from container in order to get the lock functionality
_CLASSDEF(TExit)
class TExit : public TContainer
{
  public:
    TExit(PTObjectImagery newim) : TContainer(newim) { flags |= OF_PULSE; exitflags = 0;  wait = 0; }
    TExit(PSObjectDef def, PTObjectImagery newim) : TContainer(def, newim) { flags |= OF_PULSE; exitflags = 0; wait = 0; }

    static BOOL Initialize();
        // Set up static vars
    static void Close();
        // Clear static vars

    virtual void Load(RTInputStream is, int version, int objversion);
        // Loads data from the sector
    virtual void Save(RTOutputStream os);
        // Saves data to the sector

    static BOOL AddExit(char *name, PTObjectInstance inst, BOOL getamb = TRUE);
        // Add an exit from name to inst, closing inst if it is an exit, and getting ambient color and level
    static BOOL WriteExitList();
        // Save exit list

    virtual BOOL Activate();
        // Take the player through the exit
    virtual void Unactivate();
        // After they leave, do whatever

    virtual void Pulse();

    virtual BOOL Use(PTObjectInstance user, int with = -1);
        // Open or close the exit
    virtual int CursorType(PTObjectInstance with = NULL);
        // Show that they can enter this object
    virtual void UseRange(int &mindist, int &maxdist, int &minang, int &maxang);
        // Character needs to be standing in front of the door

    BOOL SetExitState(int es);
        // Sets the exit state given the EXIT_XXX macro defines

  // Functions to prevent reflecting exits
    void SetOn() { exitflags |= EX_ON | EX_ACTIVATED; }
        // Use this immediately after teleports to avoid screen flipping
    BOOL IsOn() { return (exitflags & EX_ON); }
        // Returns whether character is on the exit

    // Exit stats
    STATFUNC(Openable)
    STATFUNC(Facing)
    STATFUNC(UseCenter)
    STATFUNC(StopMoving)
    STATFUNC(Delay)
    OBJSTATFUNC(Locked)
    OBJSTATFUNC(KeyId)
    OBJSTATFUNC(PickDifficulty)

  protected:
    static BOOL ReadExitList(BOOL reload = FALSE);
        // Read master list of exits
    static void DestroyExitList();
        // Delete exit list
    static PSExitRef FindExit(char *exitname);
        // Find a given exit
    virtual void GetExitStrip(int &regx, int &regy, int &regz, int &width, int &length, int &height);
        // Get the strip of walkmap that Locke steps on the activate the exit

    DWORD exitflags;        // Exit flags
    int wait;               // Wait until exit activates
};

DEFINE_BUILDER("EXIT", TExit)

enum
{
    LEVER_PULLSOUTH,
    LEVER_PUSHNORTH,
    LEVER_PULLNORTH,
    LEVER_PUSHSOUTH,
};

// ***************
// * TLever      *
// ***************

_CLASSDEF(TLever)
class TLever : public TExit
{
  public:
    TLever(PTObjectImagery newim) : TExit(newim) { usedir = 3; }    // 0 = NE, SE, SW, NW
    TLever(PSObjectDef def, PTObjectImagery newim) : TExit(def, newim) { usedir = 3; }

    virtual BOOL Use(PTObjectInstance user, int with = -1);

    virtual void Pulse();

    S3DPoint targetpos;
    int     usedir;
};

DEFINE_BUILDER("LEVER", TLever)

#endif
