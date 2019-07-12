// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                           Spell.h - Spell                             *
// *************************************************************************

#ifndef _SPELL_H
#define _SPELL_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

// *************************************************************
// * TSpellData/Variant - Describes the static info for spells *
// *************************************************************

// Spell variant flags
#define SVF_NONE 0

#define SPELLSIZE       (6)
#define MAXTALISMANLEN  (SPELLSIZE + 1)

_STRUCTDEF(SSpellVariant)
struct SSpellVariant
{
    int flags;                      // Spell variant flags
    char name[RESNAMELEN];          // Name of spell variant
    char talismans[MAXTALISMANLEN]; // Talismans for this variant
    char effect[RESNAMELEN];        // Number of effect id for this variant (0-whatever)
    int mana;                       // Amount of mana for this variant
    int nextspellwait;              // Amount of time to wait before next spell
    int mindamage,maxdamage;        // Damage values for this variant
    int skilllevel;                 // What skill you must be to cast this spell
    int type;                       // the type of variant
    int height;                     // the height of the spell
    int facing;                     // the facing of the spell
    int ani_delay;                  // animation delay
};

typedef TVirtualArray<SSpellVariant, 16, 16> TSpellVariantArray;

// Spell flags
#define SF_NONE 0

_STRUCTDEF(SSpellData)
struct SSpellData
{
    SSpellData();
      // Sets default values
    ~SSpellData();
    BOOL Load(char *aname, TToken &t);
      // Loads the spell from the "SPELL.DEF" file

    TSpellVariantArray variants;        // Spell variants

    int flags;                          // Spell flags
    char name[NAMELEN];                 // Name of spell type (not individual name)
    char objname[RESNAMELEN];           // Name of spell object to build when spell is cast
    char *desc;                         // Spell description for spell book
//  PTBitmap icon;                      // Spell book icon for spell
    int damagetype;                     // Type of damage spell does
    char invoke[RESNAMELEN];            // Invoke animation
    int effectstart;                    // Frames after starting invoke to start effect
};

typedef TPointerArray<SSpellData, 16, 16> TSpellDataArray;

// ***************************************************
// * TSpellList - Contains a list of all game spells *
// ***************************************************

_CLASSDEF(TSpellList)
class TSpellList
{
  private:
    BOOL initialized;               // Are we ready
    TSpellDataArray spelldata;      // Data array

  public:
    TSpellList() { initialized = FALSE; }
    ~TSpellList() { Close(); }

    BOOL Initialize();
      // Initializes spell data stuff
    void Close();
      // Kills all spell data stuff
    int NumSpells() { return spelldata.NumItems(); }
      // Returns number of spells
    PSSpellData GetSpellData(int num) { return spelldata[num]; }
      // Gets pointer to spell data based on talisman list
    PSSpellData GetSpellDataByTalismans(char *talismans);
      // Gets pointer to spell data based on talisman list
    PSSpellData GetSpellDataByName(char *name);
      // Gets pointer to spell data based on spell name
    PSSpellVariant GetVariantDataByName(char *name);
      // Gets pointer to a variant data based on spell name
    PSSpellVariant GetVariantDataByTalismans(char *talismans);
      // Gets pointer to a variant data based on talismans name
    BOOL Load();
      // Loads spell data from SPELL.DEF file

    // return an array of MAXTALISMANLEN filled with appropriate stuff
    static void GetTalList(char* string, int* tal);
    
    // compare talisman lists, return TRUE for same, FALSE otherwise
    static BOOL CompareTalList(int* tal1, int* tal2);
};

// ***************************************************
// * TSpell - Represents an active spell in the game *
// ***************************************************

#define MAXSPELLTARGETS 64

_CLASSDEF(TSpell)
class TSpell
{
  public:
    // The constructor for the tspell object
    TSpell(PTObjectInstance invoke, PTObjectInstance *targ, int numtargs, PS3DPoint sourcepos, PSSpellData dat, PSSpellVariant var, PTSpell mtr = NULL);
    TSpell()
    { invoker = NULL; targets[0] = NULL; targetnum = 0;
      timer = 0; master = NULL; spell = NULL; variant = NULL; frame = 0; 
      magic_offense = 0; magic_defense = 0;
      source.x = source.y = source.z = -1; }

    // Functions to return current spell values
    PTObjectInstance GetInvoker() { return invoker; }
      // Returns the character that invoked the spell
    PTObjectInstance GetTarget(int numtarg = 0) 
        { if (numtarg >= targetnum) return NULL; else return targets[numtarg]; }
      // Returns the target for the spell
    int GetTargetNum() { return targetnum; }
      // Returns the number of targets
    BOOL GetSourcePos(S3DPoint &pos);
      // Gets the source position of the spell.  Either the position passed by 'sourcepos'
      // in the contstructor, or the current 'right hand' position of the character if
      // 'sourcepos' was set to -1,-1,-1.

    PSSpellData SpellData() { return spell; }
      // The spell data structure from the SPELL.DEF file
    PSSpellVariant VariantData() { return variant; }
      // The variant structure from the SPELL.DEF file

    void Timer(int value) { timer = value; }
    void SetByName(char* name);
    void SetByTalismans(char* talismans);

    void Damage(PTObjectInstance ch);

    virtual void Pulse() {}
      // pulse through the spell, this is standard

    virtual BOOL Timer();
      // returns true if spell is done

    virtual void Kill() { timer = 0; }
      // Kills this spell    

    int GetDefense()        { return magic_defense; }
    void SetDefense(int i)  { magic_defense = i; }

    int GetOffense()        { return magic_offense; }
    void SetOffense(int i)  { magic_offense = i; }

    // drain off the character's mana
    void ManaDrain();

  protected:
    PTObjectInstance invoker;               // Object that invoked the spell
    PTObjectInstance effect;                // The effect for this spell

    int targetnum;                             // Number of targets in target list
    PTObjectInstance targets[MAXSPELLTARGETS]; // Spell's target list

    int timer;                              // Nice timer value for time based spells
    int frame;                              // Frame number for spells
    PTSpell master;                         // Master spell object (if slave)
    
    PSSpellData spell;                      // Data for spell
    PSSpellVariant variant;                 // Variant
    int wait;                               // Wait before the spell starts
    S3DPoint source;                        // Source of spell relative to char pos (-1,-1,-1 if not used)

    int magic_defense;
    int magic_offense;
};

typedef TPointerArray<TSpell, 32, 16> TSpellArray;

// *********************************************
// * TSpellManager - Contains a list of spells *
// *********************************************

_CLASSDEF(TSpellManager)
class TSpellManager
{
  protected:
    TSpellArray spells;                     // Currently active spells
    int wait;                               // wait for spells to be cast
  public:
    TSpellManager() { spells.Clear(); wait = 0; }
      // default constructor
    ~TSpellManager() { spells.Clear(); }
      // default destructor

    void Pulse();
      // pulse through all the spells

    BOOL CastByName(char* name, PTObjectInstance invoker, PTObjectInstance *targets, int numtargs, PS3DPoint sourcepos = NULL, PTSpell mst = NULL);
      // cast a spell by its name, returns success or failure
    BOOL CastByTalismans(char* talismans, PTObjectInstance invoker, PTObjectInstance *targets, int numtargs, PS3DPoint sourcepos = NULL, PTSpell mst = NULL);
      // cast a spell by its talismans, returns success or failure

    int GetDefense();
    int GetOffense();
    int GetSpellCount(char* spell);
};

#endif
