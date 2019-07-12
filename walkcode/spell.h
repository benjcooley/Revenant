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

#include "object.h"

// *************************************************************
// * TSpellData/Variant - Describes the static info for spells *
// *************************************************************

// Spell variant flags
#define SVF_NONE 0

#define MAXTALISMANLEN 16

_STRUCTDEF(SSpellVariant)
struct SSpellVariant
{
	int flags;						// Spell variant flags
	char name[RESNAMELEN];			// Name of spell variant
	char talismans[MAXTALISMANLEN];	// Talismans for this variant
	char effect[RESNAMELEN];		// Number of effect id for this variant (0-whatever)
	int mana;						// Amount of mana for this variant
	int nextspellwait;				// Amount of time to wait before next spell
	int mindamage,maxdamage;		// Damage values for this variant
	int skilllevel;					// What skill you must be to cast this spell
	int type;						// the type of variant
};

typedef TVirtualArray<SSpellVariant, 16, 0> TSpellVariantArray;

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

	TSpellVariantArray variants;		// Spell variants

	int flags;							// Spell flags
	char name[NAMELEN];					// Name of spell type (not individual name)
	char objname[RESNAMELEN];			// Name of spell object to build when spell is cast
	char *desc;							// Spell description for spell book
//	PTBitmap icon;						// Spell book icon for spell
	int damagetype;						// Type of damage spell does
	char invoke[RESNAMELEN];			// Invoke animation
	int effectstart;					// Frames after starting invoke to start effect
};

typedef TPointerArray<SSpellData, 16, 0> TSpellDataArray;

// ***************************************************
// * TSpellList - Contains a list of all game spells *
// ***************************************************

_CLASSDEF(TSpellList)
class TSpellList
{
  private:
	BOOL initialized;				// Are we ready
	TSpellDataArray spelldata;		// Data array

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
};

// ***************************************************
// * TSpell - Represents an active spell in the game *
// ***************************************************

_CLASSDEF(TSpell)
class TSpell
{
  public:
    // The constructor for the tspell object
	TSpell(PTObjectInstance invoke, PTObjectInstance targ, int numtargs, PSSpellData dat, PSSpellVariant var, PTSpell mtr = NULL);
	TSpell()
	{ invoker = NULL; target = NULL; targetnum = 0; timer = 0; master = NULL; effect = NULL; spell = NULL; variant = NULL; }


	// Functions to return current spell values
	PTObjectInstance GetInvoker() { return invoker; }
    PTObjectInstance GetTarget(int numtarg = 0) { if(numtarg >= targetnum) numtarg = 0; return &target[numtarg]; }
	int GetTargetNum() { return targetnum; }
	PSSpellData SpellData() { return spell; }
	PSSpellVariant VariantData() { return variant; }

	void Timer(int value) { timer = value; }

	void SetByName(char* name);
	void SetByTalismans(char* talismans);

	// initialize the spell

	virtual void Pulse() {}
	  // pulse through the spell, this is standard

	virtual BOOL Timer();
	  // returns true if spell is done

	virtual void Kill() { timer = 0; }
	  // Kills this spell    

  protected:
	PTObjectInstance invoker;				// Object that invoked the spell
	PTObjectInstance target;				// Object spell is targeted at
	int targetnum;							// number of targets

	int timer;								// Nice timer value for time based spells
	PTSpell master;							// Master spell object (if slave)
	PTObjectInstance effect;				// the effect used for this spell
	
	PSSpellData spell;						// Data for spell
	PSSpellVariant variant;					// Variant
};

typedef TVirtualArray<TSpell, 32, 0> TSpellArray;

// *********************************************
// * TSpellManager - Contains a list of spells *
// *********************************************

_CLASSDEF(TSpellManager)
class TSpellManager
{
  protected:
	TSpellArray spells;						// Currently active spells
  public:
	TSpellManager() { spells.Clear(); }
	  // default constructor
	~TSpellManager() { spells.Clear(); }
	  // default destructor

	void Pulse() { int size = spells.NumItems(); for(int i = 0; i < size; ++i) { spells[i].Pulse(); if(spells[i].Timer()) spells.Remove(i); } }
	  // pulse through all the spells

	BOOL CastByName(char* name, PTObjectInstance invoker, PTObjectInstance target, int numtargs, PTSpell mst = NULL);
	  // cast a spell by its name, returns success or failure
	BOOL CastByTalismans(char* talismans, PTObjectInstance invoker, PTObjectInstance target, int numtargs, PTSpell mst = NULL);
	  // cast a spell by its talismans, returns success or failure
};

#endif
