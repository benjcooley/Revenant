// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     Object.h - TObject module                         *
// *************************************************************************

#ifndef _OBJECT_H
#define _OBJECT_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _IMAGERY_H
#include "imagery.h"
#endif

#ifndef _STREAM_H
#include "stream.h"
#endif

#ifndef _LIGHTDEF_H
#include "lightdef.h"
#endif

/*
#define MAKEINDEX(level, sx, sy, item)  ((level<<24) | (sx<<18) | (sy<<12) | (item & 0xFFF))
#define GETLEVEL(index)                 (((unsigned)index) >> 24)
#define GETSECTORX(index)               ((index >> 18) & 0x3F)
#define GETSECTORY(index)               ((index >> 12) & 0x3F)
#define GETITEM(index)                  (index & 0xFFF)
*/

// hack alert - z vals are going nuts, so here's a quick fix (map pos to 3D pos)
#define FIX_Z_VALUE(zmap)                  ((float)zmap / ((float)1.46/* - ((float)0.01 * ((float)zmap / (float)300.0))*/))

// This fix goes from a 3D pos to a map pos
#define REV_FIX_Z_VALUE(z3d)                ((float)z3d * (float)1.46)

// Coordinate converters (between world/map and screen/pixel)
void WorldToScreen(RS3DPoint pos, int &x, int &y);
void WorldToScreen(RS3DPoint pos, RS3DPoint spos);
void WorldToScreenZ(RS3DPoint pos, int &x);
void ScreenToWorld(int x, int y, RS3DPoint pos, int zheight = 0);
void ConvertToVector(int angle, int speed, RS3DPoint vect, int zangle = 0);
int ConvertToFacing(RS3DPoint pos, RS3DPoint target);
int ConvertToFacing(RS3DPoint target);
int ConvertZToFacing(RS3DPoint pos, RS3DPoint target);
int ConvertZToFacing(RS3DPoint target);
int Distance(RS3DPoint pos);
int Distance(RS3DPoint pos, RS3DPoint target);
int AngleDiff(int angle1, int angle2);

// Create new unique id
DWORD GenerateUniqueID();

_CLASSDEF(TObjectAnimator)

// *************************************************************
// * TObjectBuilder - Creates objects for a given object class *
// *************************************************************

#define MAXOBJECTTYPES 256

_CLASSDEF(SObjectDef)
_CLASSDEF(TObjectInstance)

_CLASSDEF(TObjectBuilder)
class TObjectBuilder
{
  public:
    TObjectBuilder(char *name);
      // Sets name and adds builder to builder array
    virtual PTObjectInstance Build(PTObjectImagery img) = 0;
      // Creates a new object from the given 'def' structure
    virtual PTObjectInstance Build(PSObjectDef def, PTObjectImagery img) = 0;
      // Creates a new object from the given 'def' structure

    static PTObjectBuilder GetBuilder(int objtype);
      // Gets a pointer to a builder in the builder array
    static PTObjectBuilder GetBuilder(char *name);
      // Gets a pointer to a builder in the builder array

    char *GetTypeName() {return objtypename;}

  private:
    static int numobjtypes;                             // Number of object types
    static PTObjectBuilder builders[MAXOBJECTTYPES];    // Object builder for each type

    char *objtypename;
};

#define DEFINE_BUILDER(name, obj)                                               \
class obj##Builder : public TObjectBuilder                                      \
{                                                                               \
  public:                                                                       \
    obj##Builder() : TObjectBuilder(name) {}                                    \
    virtual PTObjectInstance Build(PTObjectImagery img)                         \
        { return new obj(img); }                                                \
    virtual PTObjectInstance Build(PSObjectDef def, PTObjectImagery img)        \
        { return new obj(def, img); }                                           \
};

#define REGISTER_BUILDER(obj) obj##Builder obj##BuilderInstance;

// **********************
// * TInventoryIterator *
// **********************

_CLASSDEF(TInventoryIterator)
class TInventoryIterator
{
  public:
    TInventoryIterator(PTObjectInstance own)
        { owner = own; invindex = 0; item = NULL; NextItem(); }

    PTObjectInstance Item() { return item; }
      // Returns current item.
    int InvIndex() { return invindex - 1; }
      // Returns current inventory index
    PTObjectInstance NextItem();
      // Advance to the next item, and return it (NULL if end of list)
    BOOL operator ++ (int) { return (NextItem() != NULL); }
      // Allows moving forwards through array in sequential order
    //BOOL operator -- (int) { return PrevItem(); }
      // Allows moving backwards through array in sequential order
//  operator BOOL () { return (item != NULL); }
//    // Returns FALSE if at end of item list
    operator PTObjectInstance () { return item; }
      // Typecast operator for object
    PTObjectInstance operator -> () { return item; }
      // Allows iterator to be used as pointer
    RTObjectInstance operator * () { return *item; }
      // Dereferencing operator

  protected:
    PTObjectInstance owner;     // inventory parent
    int invindex;               // index into inventory
    PTObjectInstance item;      // current object
};

// ************************************************
// * TObjectClass - Represents a class of objects *
// ************************************************

// The TObjectClass contains the functions to create and manage a paticular class
// of object.  Each class object represents a class of object in the system like
// inventory items, wall objects, characters, the player character, spell effects
// groups, etc.

// Classes for FORSAKEN
enum
{
    OBJCLASS_ITEM,
    OBJCLASS_WEAPON,
    OBJCLASS_ARMOR,
    OBJCLASS_TALISMAN,
    OBJCLASS_FOOD,
    OBJCLASS_CONTAINER,
    OBJCLASS_LIGHTSOURCE,
    OBJCLASS_TOOL,
    OBJCLASS_MONEY,
    OBJCLASS_TILE,
    OBJCLASS_EXIT,
    OBJCLASS_PLAYER,
    OBJCLASS_CHARACTER,
    OBJCLASS_TRAP,
    OBJCLASS_SHADOW,
    OBJCLASS_HELPER,
    OBJCLASS_KEY,
    OBJCLASS_UNUSED1,
    OBJCLASS_UNUSED2,
    OBJCLASS_UNUSED3,
    OBJCLASS_UNUSED4,
    OBJCLASS_AMMO,
    OBJCLASS_SCROLL,
    OBJCLASS_RANGEDWEAPON,
    OBJCLASS_UNUSED5,
    OBJCLASS_EFFECT,
};

#define MAXSTATSTRINGLEN    64
#define MAXNAMELEN          64

_STRUCTDEF(SStatisticDef)
struct SStatisticDef
{
    char name[MAXNAMELEN];
    int def, min, max;
    DWORD uniqueid;
};

class TStatisticDefList
{
  private:
    TSizableArray<SStatisticDef, 0, 4> statdefs;

  public:           
    int NumStats() { return statdefs.NumItems(); }
        // Number of statistics for this class
    int AddStat(SStatisticDef &newstatdef, int id = -1);
        // Add a new statistic
    void DeleteStat(int statid) { if ((DWORD)statid < (DWORD)statdefs.NumItems()) statdefs.Collapse(statid); }
        // Deletes a statistic
    PSStatisticDef GetStatisticDef(int statid)
        { if ((DWORD)statid < (DWORD)statdefs.NumItems()) return &(statdefs[statid]); else return NULL; }
        // Get StatisticDef structure
    BOOL ParseStat(SStatisticDef &stat, TToken &t);
        // Adds a new stat by parsing stream (reload causes stat def to be parsed but ignored)
    char *GetStatDefString(int statid, char *buf);
        // Returns a parsable def string
    char *StatName(int statid) { if ((DWORD)statid < (DWORD)statdefs.NumItems()) return statdefs[statid].name; else return ""; }
        // Returns statistic name
    int StatDef(int statid) { if ((DWORD)statid < (DWORD)statdefs.NumItems()) return statdefs[statid].def; else return 0; }
        // Returns statistic default value
    int StatMin(int statid) { if ((DWORD)statid < (DWORD)statdefs.NumItems()) return statdefs[statid].min; else return 0; }
        // Returns statistic minimum value
    int StatMax(int statid) {if ((DWORD)statid < (DWORD)statdefs.NumItems()) return statdefs[statid].max; else return 0; }
        // Returns statistic maximum value
    DWORD StatUniqueId(int statid) { if ((DWORD)statid < (DWORD)statdefs.NumItems()) return statdefs[statid].uniqueid; else return 0; }
        // Returns statistic maximum value
    int FindStat(char *statname);
        // Locates stat by name and returns its statid
    void Clear() { statdefs.Clear(); }
};

_STRUCTDEF(SStatEntry)
struct SStatEntry 
{
    SStatEntry(PTObjectClass cl, char *statname, char *uniqueid, int statid, 
        int def, int min, int max, BOOL objstat);
    int id;
};

// Define a stat entry (put these immediatly after the class registration in you CPP file)
#define DEFSTAT(c, n, u, i, df, mn, mx)\
  SStatEntry T##c::se_##n(&c##Class, #n, #u, i, df, mn, mx, FALSE);

#define DEFOBJSTAT(c, n, u, i, df, mn, mx)\
  SStatEntry T##c::se_##n(&c##Class, #n, #u, i, df, mn, mx, TRUE);
 
// Pre Define a Stat (put this in the base class (usually TObjectInstance)
#define PSTATFUNC(n)\
    virtual int n(){return 0;}\
    virtual void Set##n(int v) {}

#define PSTATFUNCVAL(n, r)\
    virtual int n(){return r;}\
    virtual void Set##n(int v) {}

#define POBJSTATFUNC(n)\
    virtual int n(){return 0;}\
    virtual void Set##n(int v) {}

#define POBJSTATFUNCVAL(n, r)\
    virtual int n(){return r;}\
    virtual void Set##n(int v) {}

// Define an actual stat (put this in the actual object i.e. TCharacter or TPlayer)
#define STATFUNC(n)\
  private:\
    static SStatEntry se_##n;\
  public:\
    virtual int n() { return GetStat(se_##n.id); }\
    virtual void Set##n(int v) { SetStat(se_##n.id, v); }

#define OBJSTATFUNC(n)\
  private:\
    static SStatEntry se_##n;\
  public:\
    virtual int n() { return GetObjStat(se_##n.id); }\
    virtual void Set##n(int v) { SetObjStat(se_##n.id, v); }

// Define an actual stat (put this in the actual object i.e. TCharacter or TPlayer)
#define STAT(n)\
  private:\
    static SStatEntry se_##n;\
  public:
  
#define OBJSTAT(n)\
  private:\
    static SStatEntry se_##n;\
  public:

typedef TSizableArray<int, 0, 1> TStatisticList;

#define MAXOBJECTCLASSES    64
#define MAXRESNAMELEN       64

// THIS STRUCTURE IS MULTIPLIED BY THE NUMBER OF OBJECT USED IN THE SYSTEM (WHICH IS IN THE
// THOUSANDS.  THE LARGER THIS STRUCTURE IS, THE MORE MEMORY THE SYSTEM USES.  KEEP THIS
// STRUCTURE AS *****SMALL***** AS POSSIBLE!!  BEN

_STRUCTDEF(SObjectInfo)
struct SObjectInfo
{   // HEY!  KEEP ME SMALL!
    SObjectInfo()
      { name = NULL; objbuilder = NULL; imageryid = 0; imagery = NULL; uniqueid = 0; stats.Clear(); objstats.Clear(); }
    SObjectInfo(char *n, PTObjectBuilder b, int id, DWORD uid)
      { name = _strdup(n); objbuilder = b; imageryid = id; imagery = NULL; uniqueid = uid; stats.Clear(); objstats.Clear(); }
    ~SObjectInfo() { delete [] name; stats.Clear(); objstats.Clear(); }

    char *name;                             // Name of object (allocated with _strdup()
    PTObjectBuilder objbuilder;             // Object builder for this object
    int  imageryid;                         // Imagery object for this object
    PTObjectImagery imagery;                // Imagery object for this object
    TStatisticList stats, objstats;         // Statistics (class and object)
    DWORD uniqueid;                         // Unique ID for this object
};

_CLASSDEF(TObjectClass)
class TObjectClass
{
  public:
    TObjectClass(char *classname, int classid, WORD flags, PTObjectClass base = NULL);
        // Creates object class and adds it to class array
    ~TObjectClass();
        // Destroys the object class and all object types and stats inside it
    void Clear();
        // Called by constructor and destructor to clear all internal arrays/buffers, etc.

  // Load and save class data
    static BOOL LoadClasses(BOOL lock = FALSE, BOOL reload = FALSE);
      // Loads classes (keeps file locked for updating if lock TRUE)
    static BOOL SaveClasses(BOOL lock = FALSE);
      // Saves classes (leaves file locked if lock is TRUE)
    static void FreeClasses();
      // Deallocate class arrays

  // Class functions
    static PTObjectClass GetClass(int objclass)
        { return ((DWORD)objclass >= (DWORD)numclasses) ? NULL : classes[objclass]; }
      // Returns the given class from the class array
    static int FindClass(char *name);
      // Returns the given class from the class array
    static int NumClasses() { return numclasses; }
      // Number of classes
    char *ClassName() { return name; }
        // Name of class
    int ClassId() { return id; }
        // Id number for class
    virtual DWORD ObjectFlags() { return objflags; }
        // Object flag for class
    int AddType(char *name, char *imgfilename, DWORD uniqueid = 0);
        // Add a new object type (if uniqueid=0, a new unique id will be generated)
    BOOL RemoveType(int objtype);
        // Removes an object type

  // Object functions
    PTObjectInstance NewObject(PSObjectDef objectdef);
        // Create a new object based on the given object definition

  // Type functions
    int NumTypes() { return objinfo.NumItems(); }
        // Returns number of object types in the class
    PSObjectInfo GetObjType(int index)  { if (!objinfo.Used(index)) return NULL; else return &(objinfo[index]); }
        // Return objinfo pointer for given index
    int FindObjType(char *objtypename, BOOL partial = FALSE);
        // Return type of object given name
    int FindObjType(DWORD uniqueid);
        // Return type of object given its unique ID

  // Class statistic functions
    int AddStat(SStatisticDef &stat, int id = -1);
        // Add a new statistic
    void DeleteStat(int statid);
        // Deletes a statistic
    PSStatisticDef GetStatisticDef(int statid) { return statdefs.GetStatisticDef(statid); }
        // Get StatisticDef structure
    BOOL ParseNewStat(TToken &t, BOOL reload = FALSE);
        // Adds a new stat by parsing stream (reload causes stat def to be parsed but ignored)
    int NumStats() { return statdefs.NumStats(); }
        // Number of statistics for this class
    char *StatName(int statid) { return statdefs.StatName(statid); }
        // Returns statistic name
    int StatDef(int statid) { return statdefs.StatDef(statid); }
        // Returns statistic default value
    int StatMin(int statid) { return statdefs.StatMin(statid); }
        // Returns statistic minimum value
    int StatMax(int statid) { return statdefs.StatMax(statid); }
        // Returns statistic maximum value
    DWORD StatUniqueId(int statid)  { return statdefs.StatUniqueId(statid); }
        // Returns statistic maximum value
    int FindStat(char *statname) { return statdefs.FindStat(statname); }
        // Locates stat by name and returns its statid
    char *GetStatDefString(int statid, char *buf) { return statdefs.GetStatDefString(statid, buf); }
        // Gets a parsable string that defines the stat

        // Stuff for map generator

    int FindStatVal(int statid, int searchvalue);
        // Find first object in class with searchvalue for given stat (for map generator)
    int FindRandStatVal(int statid, int searchvalue, int *heightflux = NULL);
        // Find random object in class with searchvalue for given stat (for map generator

        // Individual stat access stuff

    int GetStat(int objtype, int statid) { return objinfo[objtype].stats[statid]; }
        // Returns a statistic for an object
    int GetStat(int objtype, char *statname)
        { int statid = statdefs.FindStat(statname); 
          if (statid >= 0) return objinfo[objtype].stats[statid]; else return 0; }
        // Returns a statistic for an object
    void SetStat(int objtype, int statid, int newvalue) { objinfo[objtype].stats[statid] = newvalue; }
        // Sets a statistic for an object (Note: converted to a string if stat is string stat)
    void SetStat(int objtype, char *statname, int newvalue)
        { int statid = statdefs.FindStat(statname); 
          if (statid >= 0) objinfo[objtype].stats[statid] = newvalue; }
        // Sets a statistic for an object (Note: converted to a string if stat is a string stat)
    void ResetStat(int objtype, int statid)
        { objinfo[objtype].stats[statid] = StatDef(statid); }
        // Reset Statistic to default value for class

  // Object statistic functions
    int AddObjStat(SStatisticDef &stat, int id = -1);
        // Add a new statistic
    void DeleteObjStat(int statid);
        // Deletes a statistic
    PSStatisticDef GetObjStatisticDef(int statid) { return objstatdefs.GetStatisticDef(statid); }
        // Get StatisticDef structure
    BOOL ParseNewObjStat(TToken &t, BOOL reload = FALSE);
        // Adds a new stat by parsing stream (reload causes stat def to be parsed but ignored)
    int NumObjStats() { return objstatdefs.NumStats(); }
        // Number of statistics for this class
    char *ObjStatName(int statid) { return objstatdefs.StatName(statid); }
        // Returns statistic name
    int ObjStatDef(int statid)  { return objstatdefs.StatDef(statid); }
        // Returns statistic default value
    int ObjStatMin(int statid)  { return objstatdefs.StatMin(statid); }
        // Returns statistic minimum value
    int ObjStatMax(int statid)  { return objstatdefs.StatMax(statid); }
        // Returns statistic maximum value
    DWORD ObjStatUniqueId(int statid)   { return objstatdefs.StatUniqueId(statid); }
        // Returns statistic maximum value
    int FindObjStat(char *statname) { return objstatdefs.FindStat(statname); }
        // Locates stat by name and returns its statid
    char *GetObjStatDefString(int statid, char *buf) { return objstatdefs.GetStatDefString(statid, buf); }
        // Gets a parsable string that defines the stat

        // Individual obj stat access stuff

    int GetObjStat(int objtype, int statid) { return objinfo[objtype].objstats[statid]; }
        // Returns a statistic for an object
    int GetObjStat(int objtype, char *statname)
        { int statid = objstatdefs.FindStat(statname); 
          if (statid >= 0) return objinfo[objtype].objstats[statid]; else return 0; }
        // Returns a statistic for an object
    void SetObjStat(int objtype, int statid, int newvalue) { objinfo[objtype].objstats[statid] = newvalue; }
        // Sets a statistic for an object (Note: converted to a string if stat is string stat)
    void SetObjStat(int objtype, char *statname, int newvalue)
        { int statid = objstatdefs.FindStat(statname); 
          if (statid >= 0) objinfo[objtype].objstats[statid] = newvalue; }
        // Sets a statistic for an object (Note: converted to a string if stat is a string stat)
    void ResetObjStat(int objtype, int statid)
        { objinfo[objtype].objstats[statid] = ObjStatDef(statid); }
        // Reset Statistic to default value for class

    void CopyStats(PTObjectClass from);
        // Copies stats from base class TObjectClass to this class 
        // (copies character class stats to player class, etc.)

    static void SetClassesDirty();
        // For manual changes to the classes

  private:
    static BOOL ParseClass(TToken &t, BOOL reload = FALSE);
        // Called by LoadClasses() to parse a class
    BOOL WriteClass(FILE *fp);
        // Called by SaveClasses() to write out a class

    static int numclasses;
    static PTObjectClass classes[MAXOBJECTCLASSES];

    static FILE *classfp;                               // Stream file pointer when reading/writing
    static BOOL classesdirty;                           // Set to true when CLASS.DEF needs to be resaved

    char *name;
    int   id;
    WORD  objflags;
    PTObjectClass basedon;

    TStatisticDefList statdefs, objstatdefs;// Statistics for objects
    TVirtualArray<SObjectInfo> objinfo;     // There are no limits to these arrays!!
};

// ****************************************************
// * TObjectInstance - Map object instance base class *
// ****************************************************

// The TObjectInstance class represents an instance of an object in a sector.
// Objects have a fixed sector position, flags indicating thier current settings,
// a state for their current imagery characteristics, etc.

#define OF_IMMOBILE      (1<<0)     // Not affected by gravity etc
#define OF_EDITORLOCK    (1<<1)     // Object is locked down (can't move in editor)
#define OF_LIGHT         (1<<2)     // Object generates light (a light is on for object)
#define OF_MOVING        (1<<3)     // Object is a moving object (characters, exits, players, missiles, etc.)
#define OF_ANIMATING     (1<<4)     // Has animating imagery (animator pointer is set)
#define OF_AI            (1<<5)     // Object has A.I.
#define OF_DISABLED      (1<<6)     // Object A.I. is disabled
#define OF_INVISIBLE     (1<<7)     // Not visible in map pane during normal play
#define OF_EDITOR        (1<<8)     // Is editor only object
#define OF_DRAWFLIP      (1<<9)     // Reverse on the horizontal
#define OF_SELDRAW       (1<<10)    // Editor is manipulating object
#define OF_REVEAL        (1<<11)    // Player needs to see behind object (shutter draw)
#define OF_KILL          (1<<12)    // Suicidal (tells system to kill object next frame)
#define OF_GENERATED     (1<<13)    // Created by map generator
#define OF_ANIMATE       (1<<14)    // Call the objects Animate() func AND create object animators
#define OF_PULSE         (1<<15)    // Call the object Pulse() function
#define OF_WEIGHTLESS    (1<<16)    // Object can move, but is not affected by gravity
#define OF_COMPLEX       (1<<17)    // Object is a complex object
#define OF_NOTIFY        (1<<18)    // Notify object of a system change (see notify codes below)
#define OF_NONMAP        (1<<19)    // Not created, deleted, saved, or loaded by map (see below)
#define OF_ONEXIT        (1<<20)    // Object is currently on an exit (used to prevent exit loops)
#define OF_PAUSE         (1<<21)    // Script is paused      
#define OF_NOWALK        (1<<22)    // Don't use walk map for this tile
#define OF_PARALIZE      (1<<23)    // Freeze the object in mid-animation
#define OF_NOCOLLISION   (1<<24)    // Let the object go through boundries
#define OF_ICED          (1<<25)    // Used to know when to end the iced effect

// !!! Remember to put your flags in arrays below too !!!


// These flags are set by the object constructor and shouldn't ever be changed
// Note: Put any flags in here that you ALWAYS want set for objects of a given class
#define OF_FIXEDFLAGS (OF_COMPLEX | OF_NOTIFY | OF_NONMAP | OF_AI | OF_MOVING)

#define OBJFLAGNAMES { "IMMOBILE", "EDITORLOCK", "LIGHT", "MOVING", "ANIMATING", "AI", "DISABLED",\
            "INVISIBLE", "EDITOR", "DRAWFLIP", "SELDRAW", "REVEAL", "KILL", "GENERATED",\
            "ANIMATE", "PULSE", "WEIGHTLESS", "COMPLEX", "NOTIFY", "NONMAP", "ONEXIT",\
            "PAUSE", "NOWALK", "PARALIZE", "NOCOLLISION", "ICED"}

// NOTE: OF_NONMAP
// ----------------
//
// OF_NONMAP tells the map system that this object is managed outside of the regular map
// system.  This object will not be LOADED, SAVED, CREATED, or DELETED by the map or
// sector system.  Any object with this flag can be inserted into the map and assume that
// it won't be deleted by the map systemThis flag is intended for players, but can be used for
// other objects. 



// Notify System:
// -------------
//
// To allow objects to have references (i.e. pointers) to other objects, which may be 
// deleted at any time, we have included a notify system.  This means that...
//
// !!!! WHENEVER YOU USE A POINTER TO ANOTHER OBJECT IN YOUR OBJECT, YOU MUST CHECK  !!!!
// !!!! SEE IF IT HAS BEEN DELETED AND RESPOND ACCORDINGLY IN YOUR NOTIFY() FUNCTION !!!!
//
// Neglecting to do this will cause the system to HANG!
//
// Other notify codes are useful for system events that are infrequent, and need to be
// handled immediately (not handled by checking global values in an objects Pulse()
// function on the next frame).

// Notify Flags
#define N_DELETINGOBJECT    0x00001     // Objects should check if their object pointers are valid
#define N_DELETINGSECTOR    0x00002     // Objects should check if their object pointers are valid
#define N_DELETING          (N_DELETINGOBJECT | N_DELETINGSECTOR) // Is my object pointer still valid?
#define N_CANCELCONTROL     0x00004     // Object scripts should break if this is recieved
#define N_SCRIPTDELETED     0x00005     // Objects should check if their script is still valid
#define N_SCRIPTADDED       0x00006     // Objects should check if they have a new script

// Useful Notify Macros
#define NOTIFY_DELETED(ptr, obj)\
    ( (notify == N_DELETINGOBJECT && obj == (PTObjectInstance)ptr) ||\
      (notify == N_DELETINGSECTOR && obj->GetSector() == (PTSector)ptr) )

#define NOTIFY_CONTROLCANCELLED(ptr, obj)\
  (notify == N_CANCELCONTROL && (PTObjectInstance)ptr == obj)

#define NOTIFY_MOVED(ptr, obj)\
  (notify == N_MOVED && (PTObjectInstance)ptr == obj)

#define NOTIFY_STATECHANGED(ptr, obj)\
  (notify == N_STATECHANGED && (PTObjectInstance)ptr == obj)

// Move() return value flags
#define MOVE_NOTHING    0           // Nothing happened; no movement whatsoever
#define MOVE_MOVED      (1 << 0)    // The object changed position
#define MOVE_BLOCKED    (1 << 1)    // Tried to move but hit an obstacle
#define MOVE_FALLING    (1 << 2)    // Object is falling due to gravity
#define MOVE_NOTMOVING  (1 << 3)    // This object has no current velocity

// This structure is passed to the NewObject function in MapPane()
// Items placed in this structure should describe the initial state of the object
// and not values that will be automatically set by the object class once the object
// has been created.

_CLASSDEF(SObjectDef)
class SObjectDef
{
  public:
    short    objclass;  // Class of item (-1 if no object)
    short    objtype;   // Type of item in class (-1 if no object)
    DWORD    flags;     // Flags for object

    WORD     state;     // State of object
    WORD     level;     // Level of object

    S3DPoint pos;       // Position of object relative in the map (pos is the exact center of the object)
    S3DPoint vel;       // Movement velocity vector (divide by 0x100 to get world position units)
    S3DPoint accum;     // Accumulator for movement
    
    BYTE     rotatex;   // Rotation around the x axis (0 - 255)
    BYTE     rotatey;   // Rotation around the y axis (0 - 255)
  union
  {
    BYTE     rotatez;   // Rotation around the z axis (0 - 255)
    BYTE     facing;    // Direction object is facing (0 to 255 on x-y plane)
  };

    BYTE     group;     // Group of object
};

// Instance-specific names - keep this number smallish
#define MAXCONTEXTNAME  20

// Damage types
#define DAMAGE_UNDEFINED    0       // unknown type, is just applied as-is (avoid using this)
    // weapon damage
#define DAMAGE_BLUNT        1       // getting hit on the head with a brick
#define DAMAGE_SLASHING     2       // getting slashed by a GinsuKnife(tm)
#define DAMAGE_PIERCING     3       // getting poked with a sticting needle
    // elemental damage
#define DAMAGE_EARTH        10      // between a rock and a hard place
#define DAMAGE_FIRE         11      // getting burnt
#define DAMAGE_LIGHTNING    12      // getting eletricuted
#define DAMAGE_ICE          13      // brr...chilly
    // special damage
#define DAMAGE_POISON       20      // taking poison damage

#define LOAD_BASE(n) { BYTE n##ver; is >> n##ver; n::Load(is, version, n##ver); }
#define SAVE_BASE(n) { os << (BYTE)n::ObjVersion(); n::Save(os); }

_CLASSDEF(TObjectInstance)
class TObjectInstance : protected SObjectDef
{
  // Function Members

  public:

  // Clearing function
    void ClearObject();

  // Constructors/Destructors
    TObjectInstance() { ClearObject(); }
    TObjectInstance(PTObjectImagery img);
        // Constructor for new objects
    TObjectInstance(PSObjectDef def, PTObjectImagery img);
        // Constructor for new objects
    virtual ~TObjectInstance();
        // Destructor - call class to decrease usage count

  // Object status functions
    PSObjectInfo GetInfo() { return inf; }
        // Get the object's info struct
    char *GetClassName() { return cl->ClassName(); }
        // Get the object's info struct
    char *GetTypeName() { return inf->name; }
        // Get type name
    void SetTypeName(char *newname) { strcpy(inf->name, newname); }
        // Set type name
    char *GetName() { return name; }
        // Get instance-specific name
    void SetName(char *newname);
        // Set context name
    DWORD ObjId() { return inf->uniqueid; }
        // Returns object's unique id
    int ObjClass() { return objclass; }
        // Returns object class
    int ObjType() { return objtype; }
        // Returns object class
    int GetGroup() { return group; }
        // Returns group number of object
    void SetGroup(int newgroup) { group = newgroup; }
        // Sets the group number of the object
    void GetPos(RS3DPoint getpos) { getpos = pos; }
        // Gets current object position
    S3DPoint &Pos() { return pos; }
        // Returns a reference to the object pos
    int Distance(PTObjectInstance inst);
        // Returns the distance on the x-y plane between this and inst
    int SqrDist(PTObjectInstance inst) { return SQRDIST(pos, inst->pos); }
        // Returns the square of the distance between this and inst
    int AngleTo(PTObjectInstance inst);
        // Returns the angle to the other instance
    int FaceAngleTo(PTObjectInstance inst);
        // Returns the + or - difference between this objects facing and the dest obj
    void GetScreenPos(int &x, int &y);
        // Gets x and y pixel position of object
    void GetScreenPos(S3DPoint &s);
        // Converts tile pos z to screen zbuffer z
    virtual int SetPos(RS3DPoint newpos, int newlevel = -1, BOOL override = FALSE);
        // Sets current object position, returns object's sector number (override to ignore bounds checking)
    void ForcePos(RS3DPoint newpos) { pos = newpos; }
        // Forces current position to new postion with NO validity checking, sector transfer, etc.
    virtual void MoveTo(RS3DPoint newpos) { SetPos(newpos, -1, FALSE); }
        // Moves object to new position (does walk checking for characters).
        // Use this function instead of SetPos() to avoid moving objects through or onto
        // barriers.
    void GetSnapPos(PTObjectInstance oi, int dist, S3DPoint &p);
        // Return the snap position from this character given this distance
    void SnapDist(PTObjectInstance oi, int dist);
        // Moves this object so that it is exactly 'dist' distance from the object 'oi', while
        // keeping the angle between them the same.
    int GetLevel() { return level; }
        // Returns level of object
    int SetLevel(int newlevel) { return SetPos(pos, newlevel, FALSE); }
        // Sets the level of the object
        // Note: Only OF_NONMAP objects can move between levels!
    void ForceLevel(int newlevel) { level = newlevel; }
        // Sets the level variable of an object directly (useful for various load/save functions) 
    void ClearAccum() { memset(&accum, 0, sizeof(S3DPoint)); }
        // Clear out object's movement accumulator
    DWORD GetNotify() { return notifyflags; }
        // Gets object notify flags
    void SetNotify(DWORD newflags);
        // Sets object notify flags
    virtual void Notify(int notify, void *ptr);
        // Notify Action
    void ForceSector(PTSector newsector) { sector = newsector; }
        // Directly sets the sector pointer for this object
    PTSector GetSector() { return sector; }
        // Get the object's current sector
    int GetState() { return state; }
        // Gets current object state
    int GetPrevState() { return prevstate; }
        // Returns the state we were in last time SetState() was called
    char *GetStateName() { return GetAniName(); }
        // Returns the name of the current state
    virtual BOOL SetState(int state);
        // Sets current object state
    virtual BOOL SetState(char *statename) { return SetState(FindState(statename)); }
        // Sets current object state
    int NumStates() { if (imagery) return imagery->NumStates(); return 1; }
        // Return number of states for object
    int GetFrame() { return frame; }
        // Returns frame number
    int GetPrevFrame() { return prevframe; }
        // Gets the previous frame we were in last time we called SetFrame()
    void SetFrame(int newframe) { frame = newframe; }
        // Sets frame number
    int GetFrameRate() { return framerate; }
        // Gets the current framerate
    void SetFrameRate(int newframerate) { framerate = newframerate; }
        // Sets the current framerate
    PTObjectImagery GetImagery();
        // Returns object's imagery
    virtual BOOL HasAnimator() { return animator != NULL; }
        // Returns TRUE if object is an animating object.
    virtual PTObjectAnimator GetAnimator() { return animator; }
        // Returns object's animator
    virtual BOOL CreateAnimator();
        // Creates Animator
    virtual void FreeAnimator();
        // Releases Animator
    virtual BOOL NeedsAnimator();
        // Check to see if an animator is necessary for object at the moment
    virtual BOOL DrawShadow() { return FALSE; }
        // Returns true of the object wants a simple alpha-channel circle shadow to follow it

  // Object flag functions  
    int GetNumFlags();
        // Gets number of object flags
    char *GetFlagName(int flagnum);
        // Returns the name of a flag
    int GetFlagNum(char *flagname);
        // Returns the number of the flag (use (1 << flagnum) to get flag)
    DWORD Flags() { return flags; }
    DWORD GetFlags() { return flags; }
        // Gets object flags
    void SetFlags(DWORD newflags, DWORD newmask = 0xFFFFFFFF)
        { ResetFlags((flags | newflags) & newmask); }
        // Sets object flags
    void SetFlag(DWORD newflag, BOOL on = TRUE)
        { if (on) ResetFlags(flags | newflag); else ResetFlags(flags & ~newflag); }
        // Sets a particular flag
    void ClearFlag(DWORD newflag)
        { ResetFlags(flags & ~newflag); }
    void SetFlag(char *flag, BOOL on);
        // Sets a particular flag given the flag name
    void ResetFlags(DWORD newflags = 0);
        // Resets flags

    BOOL IsFlagSet(char *flag);
        // Returns TRUE if flag is set
    BOOL IsComplex() { return flags & OF_COMPLEX; }
        // Is this a complex object
    BOOL IsOnExit() { return flags & OF_ONEXIT; }
        // Is character on an exit?
    BOOL IsMoving() { return flags & OF_MOVING; }
        // This object is a moving object (can move, doesn't mean it's moving right now)
    BOOL IsLight() { return flags & OF_LIGHT; }
        // This object is a moving object (can move, doesn't mean it's moving right now)
    BOOL IsPulsed() { return flags & OF_PULSE; }
        // True if this object is pulsed by the system Pulse() function (for ai, control, etc.)
    BOOL IsAnimated() { return flags & OF_ANIMATE; }
        // True if this object is animated by the system Animate() function
        // for drawing on screen, etc.
    BOOL IsCharacter() { return ObjClass() == OBJCLASS_CHARACTER || ObjClass() == OBJCLASS_PLAYER; }
        // True if object is a character or player object
    BOOL IsNotify() { return flags & OF_NOTIFY; }
        // Does this object want to be notified of system events (Notify() function called)

    BOOL IsParalized() { return flags & OF_PARALIZE; }
        // True if this object is paralized - froze in time
    void SetParalize(BOOL on){SetFlag(OF_PARALIZE, on);}
        // Paralize the object, added by Pepper, if this is screwy, hunt down Hersh Reddy
    BOOL IsNoCollision() { return flags & OF_NOCOLLISION; }
        // True if this object can pass through thingies
    void SetNoCollision(BOOL on){SetFlag(OF_NOCOLLISION, on);}
        // Let this object pass through thingies, added by Pepper

    BOOL IsIced() { return flags & OF_ICED; }
        // True if this object is frozen in ice
    void SetIced(BOOL on){SetFlag(OF_ICED, on);}
        // Used to set wether this object is frozen, added by Pepper


  // Object manipulation functions
    virtual void Damage(int damage, int type = DAMAGE_UNDEFINED);
        // Apply damage to an object
    virtual void RepaintObject();
        // Cause object to repaint itself on the screen
    virtual BOOL AddToInventory(PTObjectInstance inst, int slot = -1);
        // Add inst to this object's inventory, in the given slot (first free slot if none specified)
    virtual BOOL AddToInventory(char *name, int number = 1, int slot = -1);
        // Add object of type 'name', amount of 'number' to this objects inventory at slot 'slot'
    virtual void RemoveFromInventory();
        // Remove this object from whatever inventory it is in
    virtual int GiveInventoryTo(PTObjectInstance to, char *name, int number = 1);
        // Gives the object 'name' to another object.  Will move multiple objects, 
        // or objects with varying amounts if 'number' > 1.
    virtual int DeleteFromInventory(char *name, int number = 1)
      { return GiveInventoryTo(NULL, name, number); }
        // Uses the GiveInventoryTo function with a null destination to delete inventory objects
        // from an object.  
    virtual int GetInventoryAmount(char *name);
        // Returns how many 'name' objects are in inventory
    virtual BOOL HasEmptySlot();
        // Returns TRUE if there is an empty slot available
    virtual BOOL AddToMap();
        // Add this object to the map pane
    virtual void RemoveFromMap();
        // Remove this object from the map pane
    virtual int FindFreeInventorySlot();
        // Find the first free inventory slot in the object's inventory
    virtual void SignalAddedToInventory();
        // Called to signal object that it was added to a new inventory
    virtual PTObjectInstance FindObjInventory(char *name);
        // Find an object by name in inventory
    virtual PTObjectInstance FindObjInventory(int objclass, int type = -1);
        // Find an object by class and type in inventory
    virtual BOOL IsInInventory() { return (inventnum >= 0); }
      // Returns true if the object is in another object's inventory and should not be drawn
    virtual BOOL Use(PTObjectInstance user, int with = -1);
        // Uses object (user is the person using it, with is the object to use with this)
        // Returns TRUE if the object was actually used, FALSE if nothing could be done with it
    virtual int CursorType(PTObjectInstance with = NULL) { return CURSOR_NONE; }
        // Returns type of cursor that should appear when mouse arrow is over the object
    virtual void UseRange(int &mindist, int &maxdist, int &minang, int &maxang)
        { mindist = 20; maxdist = 50; minang = 0; maxang = 255; }
        // Valid locations character can be standing in order to use the object

  // Rotation and velocity and scale functions
    void SetRotateX(int ang) { rotatex = ang; }
    int GetRotateX() { return rotatex; }
    void SetRotateY(int ang) { rotatey = ang; }
    int GetRotateY() { return rotatey; }
    void SetRotateZ(int ang) { rotatez = ang; }
    int GetRotateZ() { return rotatez; }
        // All of the above set or get rotations about the given axis
    void Face(int newfacing) { SetRotateZ(newfacing); SetMoveAngle(newfacing); }
        // Faces object towards the given direction and sets motion to go in that direction
    void FaceOnly(int newfacing) { SetRotateZ(newfacing); }
        // Faces object towards the given direction, but does not set movement angle to that dir
    int GetFace() { return GetRotateZ(); }
      // Returns current facing
    void SetVel(S3DPoint &v) { vel = v; }
        // Set object's movement speed
    void GetVel(S3DPoint &v) { v = vel; }
        // Get object's current movement speed

  // Statistic functions (shortcuts to the object's class)
    char *StatName(int statid) { return cl->StatName(statid); }
        // Returns a statistic for an class
    int GetStat(int statid) { return cl->GetStat(objtype, statid); }
        // Returns a statistic for an class
    int FindStat(char *statname) { return cl->FindStat(statname); }
        // Finds a stat and returns its stat id or -1 if not found
    char *ObjStatName(int statid) { return cl->ObjStatName(statid); }
        // Returns a statistic for an class
    int GetObjStat(int statid) { if ((DWORD)statid < (DWORD)stats.NumItems()) return stats[statid]; else return 0; }
        // Returns a statistic for an object
    int FindObjStat(char *statname) { return cl->FindObjStat(statname); }
        // Finds a stat and returns its stat id or -1 if not found
    void SetStat(int statid, int value) { cl->SetStat(objtype, statid, value); }
        // Sets a class statistic
    void SetObjStat(int statid, int value) { if ((DWORD)statid < (DWORD)stats.NumItems()) stats[statid] = value; }
        // Sets an object statistic
    void ResetStat(int statid) { cl->ResetStat(objtype, statid); }
        // Resets class stat to default value
    void ResetObjStat(int statid) { stats[statid] = cl->GetObjStat(objtype, statid); }
        // Resets object stat to default value
    int GetStat(char *statname);
        // Returns a statistic given the statistic name (stat can be object or class stat)
    int GetStat(char *statname, char *str, int id = -1);
        // Returns a statistic via sprintf format 'StatnameId.Str' (stat can be object or class stat)
    void SetStat(char *statname, int value);
        // Sets a statistic given the stat name (stat can be object or class stat)
    void DelStat(int statid) { stats.Collapse(statid); }
        // Deletes a stat in stat array (Used by classes DeleteStat(), don't call directly)

  // Object drawing functions (Called by MapPane only.. don't call directly)
    virtual BOOL OnObject(SPoint &p)
      { SRect r; 
        if (flags & OF_LIGHT) imagery->GetScreenRect(this, r); // Get just the light icon
            else GetScreenRect(r);
        return r.In(p); }
        // Returns true if map point is on object
    virtual void GetScreenRect(SRect &r)
      { if (flags & OF_LIGHT) GetLightRect(r); else if (imagery) imagery->GetScreenRect(this, r); }
        // Get screen bounding rectangle for object (in world coordinates)
    virtual void GetAnimRect(SRect &r) { if (imagery) imagery->GetAnimRect(this, r); }
        // Get animation bounding rect for the object, if different from screen rect
    virtual int BgDrawMode();
        // Determines the bgdraw pipeline position this object should start at when drawing to bg
    virtual void DrawUnlit(PTSurface surface) { if (imagery) imagery->DrawUnlit(this, surface); }
        // Causes the object to draw itself to the unlit background
    void DrawLit(PTSurface surface) { if (imagery) imagery->DrawLit(this, surface); }
        // Causes the object to draw itself to the lit background
    virtual BOOL GetZ(PTSurface surface) { return (imagery) ? imagery->GetZ(this, surface) : FALSE; }
        // Get first uncliped zbuffer point by simulating drawing to the surface
    void DrawSelected(PTSurface surface)
        { if (imagery) imagery->DrawSelected(this, surface); }
        // Causes image to draw selection (hilighting) around itself
    virtual void DrawInvItem(int x, int y) { if (imagery) imagery->DrawInvItem(this, x, y); }
        // Draws inventory icon for object
    void DrawLight(PTSurface surface, BOOL resetid = FALSE);
        // Causes the object to draw light for object
    void RedrawBackground(int bgdraw = -1);
        // Forces the background for the current object rect to be redrawn
        // Allows moving objects to erase and redraw themselves, and redraws lighting
    virtual BOOL AlwaysOnTop() { if (imagery) return imagery->AlwaysOnTop(this); return FALSE; }

  // More spiffy virtuals
    virtual void Pulse();
        // Master function called every frame (if OF_PULSE is set)
    virtual DWORD Move();
        // Cause object to make next time unit of movement based on facing and speed
        // if OF_PULSE and OF_MOVING (no drawing done here)
    virtual void SetObjectMotion();
        // Called ONLY by MapPane MoveObjects() function to set
        // face,nextmove,moveangle for next frame
    virtual void Animate(BOOL draw);
        // Causes the object to prepare the next frame of its animation (if OF_ANIMATE is set)
    virtual void NextFrame();
        // Called to advance objects animation to the next frame (if OF_ANIMATE is set)
    virtual void OnScreen();
        // Called by pane when object comes on screen.
    virtual void OffScreen();
        // Called by pane when object goes off screen.

  // Imagery pass through functions
    virtual PTBitmap GetStillImage(int ostate = -1) { return imagery ? imagery->GetStillImage(ostate < 0 ? GetState() : ostate) : NULL; }
        // Get still bitmap, if any
    virtual PTBitmap InventoryImage() { return imagery ? imagery->GetInvImage(GetState()) : NULL; }
        // Returns bitmap for the inventory image
    virtual BOOL IsInventoryItem() { return (imagery != NULL && imagery->GetInvImage(GetState()) != NULL); }
        // Returns whether or not this item can go into an inventory
    virtual int FindState(char *name) { return imagery ? imagery->FindState(name) : -1; }
        // Find a state in the object's imagery
    virtual int FindTransitionState(char *from, char *to) { return imagery ? imagery->FindTransitionState(from, to) : -1; }
        // Find a state in the object's imagery
    char *GetAniName() { return imagery ? imagery->GetAniName(GetState()) : NULL; }
        // Gets current animation name
    DWORD GetAniFlags() { return imagery ? imagery->GetAniFlags(GetState()) : 0; }
        // Gets current animation flags
    void SaveHeader() { if (imagery) imagery->SaveHeader(); }
        // Causes the header info for the imagery to be saved
    virtual void ResetState();
        // Resets the state back to the first frame again and restarts animation
    void GetFacingBoundBox(int &nx, int &ny, int &nsx, int &nsy);
        // Get the walkmap bounding box with the object's facing calculated in

  // Script functions
    void InitScript(PTScript newscr);
      // Set script to newscr and initialize
    void ResetScript();
      // Reset the current script
    void ContinueScript();
      // Continue script execution (call every frame)
    void ScriptJump(char *label);
      // Jump to a given label in the script
    PTScript GetScript() { return script; }
      // Returns object's script

  // Command parsing function
    virtual int ParseCommand(TToken &t);
      // Takes a script/editor command line and parses it

  // Sound playing functions
    BOOL PlayWave(char *soundname, int nr = -1, int volume = -1, int freq = -1);
      // Plays the given sound at this objects current 3D x,y,z pos
      // If 'nr' is not -1, the number in nr is added to end of soundname (i.e. "sound1")
      // Volume and freq are DirectSound volume and frequency values (-1 is default)

  // Light functions
    void RedrawLight();
      // Causes the objects light to be redrawn
    void GetLightRect(RSRect r);
      // Gets the extent of the objects lighting rectangle
    void SetLightIntensity(BYTE newintensity);
      // Sets the light intensity
    BYTE GetLightIntensity() { return lightdef.intensity; }
      // Returns objects light intensity
    void SetLightMultiplier(int mult);
      // Sets the light intensity
    short GetLightMultiplier() { return lightdef.multiplier; }
      // Returns objects light intensity
    void SetLightPos(RS3DPoint newpos);
      // Sets light relative position
    void GetLightPos(RS3DPoint getpos) { getpos = lightdef.pos; }
      // Gets the light relative position
    void SetLightColor(SColor color);
      // Sets the light color
    void GetLightColor(SColor &color) { color = lightdef.color; }
      // Returns objects light color
    void SetLightFlags(BYTE newflags) { lightdef.flags |= newflags; }
      // Sets light flags
    void ResetLightFlags(BYTE newflags) { lightdef.flags = newflags; }
      // Resets light flags
    BYTE GetLightFlags() { return (BYTE)lightdef.flags; }
      // Gets the light flags
    PSLightDef GetLightDef() { return &lightdef; }
      // Returns light data for object
    int GetIllumination(PTObjectInstance oi);
      // Get the amount of illumination from this light to this particular object
    int GetShadow() { return shadow; }
      // Returns shadow index
    void SetShadow(int index) { shadow = index; }
      // Sets shadow index

  // State handling functions
    virtual BOOL CommandDone() { if (!animator) return TRUE; return commanddone; }
    virtual void SetCommandDone(BOOL newcmd);

  // Streaming functions
    static PTObjectInstance LoadObject(RTInputStream is, int version, BOOL ismap = FALSE);
        // Loads and creates a new object from stream 
        // (objs with OF_NOSAVEMAP i.e. players will be ignored if ismap is TRUE)
    static void SaveObject(PTObjectInstance inst, RTOutputStream os, BOOL ismap = FALSE);
        // Saves object to stream (includes header and block information)
        // (objs with OF_NOSAVEMAP i.e. players will be ignored if ismap is TRUE)
    virtual int ObjVersion() { return 0; }
        // The version id of the object for the Load()/Save() functions.
        // This version number allows objects to change what they stream to the map
        // while still being able to load previous versions of the object.
        // !!!!! IMPORTANT !!!!! When you override the ObjVersion() function, you
        // MUST use the LOAD_BASE() and SAVE_BASE() macros to load and save the base
        // class or the object version for your base class will be WRONG!
    virtual void Load(RTInputStream is, int version, int objversion);
        // Loads object data from the sector 
        // NOTE: When you need to call the base class load function, make sure you 
        // use LOAD_BASE(TBaseClass) instead of TBaseClass:Load(is, version, objversion)
        // or the base class version number will be WRONG!!
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector
        // NOTE: When you need to call the base class load function, make sure you 
        // use SAVE_BASE(TBaseClass) instead of TBaseClass::Save(os)
        // or the base class version number will be WRONG!!
    virtual void LoadInventory(RTInputStream is, int version);
        // Load inventory recursively
    virtual void SaveInventory(RTOutputStream os);
        // Save inventory recursively

  // Inventory access functions
    PTObjectInstance GetInventory(int index);
        // Gets inventory object, index is NOT the same as inventory slot
    PTObjectInstance GetInventorySlot(int slot);
        // Searches for inventory object occupying the given slot
    int NumInventoryItems() { return inventory.NumItems(); }
        // Number of objects in the array
    int RealNumInventoryItems();
        // Number of *used* objects in the array
    int InventNum() { return inventnum; }
    int InvIndex() { return invindex; }
    void SetInventNum(short i) { inventnum = i; }

  // Owner functions
    PTObjectInstance GetOwner() { return owner; }
    PTObjectInstance GetTopOwner()
        { PTObjectInstance inst = this;
          while (inst->owner) inst = inst->owner;
          return inst == this ? NULL : inst; }
        // Recurse through the owner list and get the topmost owner
    void SetOwner(PTObjectInstance newown) { owner = newown; }

  // Mapindex functions
    void SetMapIndex(int newindex) { mapindex = newindex; }
    int GetMapIndex() { return mapindex; }

  // Nextmove functions
    void SetNextMove(RS3DPoint p);
      // Set next frame's movement.
    void GetNextMove(RS3DPoint p);
      // Get next frame's movement.
    void SetMoveAngle(int ang) { moveangle = ang; }
    int GetMoveAngle() { return moveangle; }
      // Angle of motion - does NOT have to match facing, though it usually does
    void SetMoveDist(int dist) { movedist = dist; }
    int GetMoveDist() { return movedist; }
      // Angle of motion - does NOT have to match facing, though it usually does
    void SetMoveVert(int vert) { movevert = vert; }
    int GetMoveVert() { return movevert; }
      // Angle of motion - does NOT have to match facing, though it usually does
    void Halt() { movedist = 0; }
      // Halt forward character motion.
    void SetMoveBits(DWORD newbits) { movebits = newbits; }
      // Called only by the MoveObjects() function in MapPane when all Move() funcs called
    DWORD GetMoveBits() { return movebits; }
      // Returns result flags from latest move

  // Basic object/character stats
  // Note: The macro's below pre-define empty virtual XXXX() and SetXXXX(v) functions
  // You only need to predefine stats here that you want to be able to use from
  // any object.  Some stats, (i.e. player strength, dexterity) that are only accessed
  // in the local object should not be defined here, but only in that object.

      // Money, inventory, etc.
    PSTATFUNC(EqSlot)
    PSTATFUNC(Value)
    PSTATFUNCVAL(Amount, 1)

      // Character, player, etc.
    POBJSTATFUNC(Aggressive)
    POBJSTATFUNC(Poisoned)
    POBJSTATFUNC(Sleeping)
    POBJSTATFUNC(Health)
    POBJSTATFUNC(Fatigue)
    POBJSTATFUNC(Mana)
    POBJSTATFUNC(MaxHealth)
    POBJSTATFUNC(MaxFatigue)
    POBJSTATFUNC(MaxMana)

  protected:

    // ****************************************************************************
    // **** !!!WARNING!!! *** !!!WARNING!!! *** !!!WARNING!!! *** !!!WARNING!!! ***
    // ****************************************************************************

    // Every data member you add to this structure is repeated THOUSANDS of 
    // times (once for each object in the map).  PLEASE use dynamically 
    // allocated sub structures.. put derived class specific data in derived 
    // classes, and just try to keep things small!

    // ****************************************************************************
    // **** !!!WARNING!!! *** !!!WARNING!!! *** !!!WARNING!!! *** !!!WARNING!!! ***
    // ****************************************************************************

  // Identifier
    char *name;                 // What is my name
    DWORD notifyflags;          // Notify Objects of changes
    int mapindex;               // Unique instance id
    PTSector sector;            // Which sector am I in
    PTObjectClass cl;           // Pointer to object's class
    PSObjectInfo inf;           // Pointer to object type info
    int shadow;                 // Attached shadow (-1 if none)

  //Animation/drawing
    PTObjectImagery imagery;    // Pointer to current imagery object
    PTObjectAnimator animator;  // Pointer to animatior
    short frame, framerate;     // Frame number and framerate for object
    WORD prevstate;             // Previous state
    short prevframe;            // Previous state's last frame (not previous frame for this state)

  // Inventory
    PTObjectInstance owner;     // What container it is in
    TPointerArray<TObjectInstance, 0, 4> inventory;
    short inventnum;            // Inventory slot number
    short invindex;             // Inventory index (NOT necessarily equal to slot number)

  // Script
    BOOL  commanddone;          // Indicates if command is complete
    PTScript script;            // Pointer to object's script elements

  // Light
    SLightDef lightdef;         // Current light definition

  // Statistics                 
    TStatisticList stats;       // Stats for object

  // Working members
    int                 moveangle;  // Angle to move next move
    int                 movedist;   // Distance to move next move
    int                 movevert;   // Vertical distance to move next move
    DWORD               movebits;   // Result of last move

    // members to optimize function calls
    S3DPoint oldpos;                // Where it was last time function was called
    int screenx, screeny, screenz;  // Pixel/zbuf coords as of oldpos
};

inline void rollover(int &i, int &j)
{
    if (absval(i) >= ROLLOVER)
    {
        int k = i / ROLLOVER;
        j += k;
        i -= k * ROLLOVER;
    }
}

#endif
