// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      player.h - TPlayer module                        *
// *************************************************************************

#ifndef _PLAYER_H
#define _PLAYER_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _CHARACTER_H
#include "character.h"
#endif

#ifndef _EQUIP_H
#include "equip.h"
#endif

#ifndef _WEAPON_H
#include "weapon.h"
#endif

#ifndef _MULTI_H
#include "multi.h"
#endif

#ifndef _CHARSTATS_H
#include "charstats.h"
#endif

#ifndef _RULES_H
#include "rules.h"
#endif

// Equipment slot defines
#define NUM_EQ_SLOTS	9

#define EQ_NECK			0
#define EQ_BODY			1
#define EQ_OFFHAND		2
#define EQ_PRIMEHAND	3
#define EQ_R_ACCESSORY	4
#define EQ_L_ACCESSORY	5
#define EQ_RANGEDWEAPON	6
#define EQ_AMMO			7
#define EQ_FEET			8

#define PLYRSTATMOD_NORMAL			0
#define PLYRSTATMOD_HALFROUNDDOWN   1
#define PLYRSTATMOD_HALFROUNDUP     2

// **************
// * Skill Tree *
// **************

_STRUCTDEF(SSkill)
struct SSkill
{
	char *name;				// Name of the skill
	BOOL children;			// Whether or not it has children
	BOOL lastchild;			// End of child list
	int ancestor;			// Parent skill
	int difficulty;			// Difficulty of learning
};

extern SSkill SkillTree[NUM_SKILLS];	// Master skill tree

// ***********
// * TPlayer *
// ***********

_CLASSDEF(TPlayer)
class TPlayer : public TCharacter
{
  public:
	void ClearPlayer();

	TPlayer(PTObjectImagery newim);
	TPlayer(PSObjectDef def, PTObjectImagery newim);
	~TPlayer();

	virtual BOOL GetZ(PTSurface surface) { if (!Editor) return FALSE; return TCharacter::GetZ(surface); }
	virtual BOOL AlwaysOnTop() { if (!Editor) return FALSE; return TCharacter::AlwaysOnTop(); }
	virtual BOOL Use(PTObjectInstance user, int with = -1) { return FALSE; }
	virtual int CursorType(PTObjectInstance inst = NULL) { return CURSOR_NONE; }
		// These functions make sure the player never clicks on themselves

	virtual void Pulse();
	virtual DWORD Move();

	virtual void AI() { }

	virtual void Damage(int damage, int type = DAMAGE_UNDEFINED);
		// Apply damage to the player

//	virtual int SwingRange();
//	virtual int ThrustRange();
		// These are computed from the currently wielded weapon

	PTObjectInstance PrimeHand() { return equipment[EQ_PRIMEHAND]; }
	PTObjectInstance OffHand() { return equipment[EQ_OFFHAND]; }
	PTObjectInstance Body() { return equipment[EQ_BODY]; }
	PTObjectInstance Neck() { return equipment[EQ_NECK]; }
	PTObjectInstance RAccessory() { return equipment[EQ_R_ACCESSORY]; }
	PTObjectInstance LAccessory() { return equipment[EQ_L_ACCESSORY]; }
	PTObjectInstance Feet() { return equipment[EQ_FEET]; }

	void RefreshEquip();
	    // Sets up all equipment pointers, and refreshes equipment pane if necessary
	void Equip(PTObjectInstance oi, int slot);
		// Set up equipment pointers from objects in player's inventory
	PTObjectInstance GetEquip(int slot)	{ return equipment[slot]; }
		// Returns object pointer to equipment in the given slot

	// Player stat access
	int PlyrStat(int plyrstat) { return GetObjStat(plyrstat + PLRSTAT_FIRST); }
	  // Returns the value of a given physical attribute for player
	int PlyrStatMod(int plyrstat, int flags = PLYRSTATMOD_NORMAL);
	  // Returns the modifier value for the given player stat
	int PlyrStatPcnt(int plyrstat, int inc = 10) { return PlyrStatMod(plyrstat) * inc; }
	  // Returns a percentage modifier value for a physical attribute
	int PlyrStatSkillMod(int skill);
	  // Returns a percentage modifier value for a physical attribute

	// Skill access
	int Skill(int skillnum) { return GetObjStat(skillnum + SK_FIRST); }
	  // Get skill
	int SkillExp(int skillnum) { return GetObjStat(skillnum + SKE_FIRST); }
	  // Get skill experience
	int SkillMod(int skillnum)
		{ return chardata->classdata->skillmods[skillnum] + PlyrStatSkillMod(skillnum); }
	int SkillPcnt(int skillnum, int pcnt = 5) 
		{ return (Skill(skillnum) + 
				  chardata->classdata->skillmods[skillnum] +
				  PlyrStatSkillMod(skillnum)) * pcnt; }
	  // Returns total of character skill level plus class skill modifier plus stat skill
	  // modifierUses exponential skill value to get linear power.  Min is minimum power,
	  // base is the base skill value which must double for each increase 'inc' in 
	  // power.  (i.e. min=5, base=5, inc=5... skill 0=5, 5=10, 10=15, 20=20, 40=25, 80=30, etc.)
	void SetSkill(int skillnum, int v) { SetObjStat(skillnum, v); }
	  // Sets the skill value
	int WeaponSkill(int weapontype) { return Skill(SK_WEAPONSKILLS + weapontype); }

	// Info functions
	virtual int GetResistance(int type);
	  // Get character's resistance to the given damage type

	// Resolve functions
	virtual int ResolveCombat(PTActionBlock ab, int bits);

	// Multi-player system

	// Cheat - ride that hog
	void GetOnYerHog();

	// Overloaded Health, Fatigue, and Mana stats to update screen bars
	virtual void SetHealth(int v);
	virtual void SetFatigue(int v);
	virtual void SetMana(int v);

	// Miscellaneous player values
	OBJSTATFUNC(Level)
	OBJSTATFUNC(Exp)
	
	// Player stats
	OBJSTATFUNC(Strn)
	OBJSTATFUNC(Cons)
	OBJSTATFUNC(Agil)
	OBJSTATFUNC(Rflx)
	OBJSTATFUNC(Mind)
	OBJSTATFUNC(Luck)

    // Player skills
	OBJSTAT(Attack)
	OBJSTAT(Defense)
	OBJSTAT(Invoke)
	OBJSTAT(Hands)
	OBJSTAT(ShortBlades)
	OBJSTAT(LongBlades)
	OBJSTAT(Bludgeons)
	OBJSTAT(Axes)
	OBJSTAT(Bows)
	OBJSTAT(Stealth)
	OBJSTAT(LockPick)

	// Player skill experience
	OBJSTAT(AttackExp)
	OBJSTAT(DefenseExp)
	OBJSTAT(InvokeExp)
	OBJSTAT(HandsExp)
	OBJSTAT(ShortBladesExp)
	OBJSTAT(LongBladesExp)
	OBJSTAT(BludgeonsExp)
	OBJSTAT(AxesExp)
	OBJSTAT(BowsExp)
	OBJSTAT(StealthExp)
	OBJSTAT(LockPickExp)

   // Calculated stats
    virtual int MaxHealth() 
		{ return Rules.healthperlevel * Level() * 
			(100 + chardata->classdata->healthmod + PlyrStatPcnt(PLRSTAT_CONS)) / 100; }
	  // Calculates maximum health as 10 * level * (100% + classmod% + constitutionmod%)
    virtual int MaxFatigue() 
		{ return Rules.fatigueperlevel * Level() * 
			(100 + chardata->classdata->fatiguemod + PlyrStatPcnt(PLRSTAT_CONS)) / 100; }
	  // Calculates maximum health as 10 * level * (100% + classmod% + constitutionmod%)
    virtual int MaxMana() 
		{ return Rules.manaperlevel * Level() * 
			(100 + chardata->classdata->manamod + PlyrStatPcnt(PLRSTAT_MIND)) / 100; }
	  // Calculates maximum health as 10 * level * (100% + classmod% + constitutionmod%)
//	virtual int BlockPcnt() { return SkillPcnt(SK_DEFENSE) + 10; }
	  // Returns percentage of time character will block an attack
	virtual int ArmorValue() { if (Body()) return Body()->GetStat("Protection"); else return 0; }
	  // Return armor value
	virtual int WeaponType() { return WT_LONGBLADE; } //if (PrimeHand()) return PrimeHand()->GetStat("Type"); else return WT_HAND; }
	  // Returns the type of weapon being used
	virtual int WeaponDamage() { return 5; } //if (PrimeHand()) return PrimeHand()->GetStat("Damage"); else return chardata->handdamage; }
	  // Returns the current weapon's damage value
	virtual int StealthMod() { 
		return SkillPcnt(SK_STEALTH, 10) + 
			(Body()?Body()->GetStat("Stealth"):0) + 
			(Feet()?Feet()->GetStat("Stealth"):0);  }
	  // Get stealth by combining clothing stealth value and stealth stat

  private:
	PTObjectInstance equipment[NUM_EQ_SLOTS];			// player's weapons and armor

	BOOL OnTheHog;					// hog cheat
};

DEFINE_BUILDER("Player", TPlayer)

typedef TPointerArray<TPlayer, 4, 4> TPlayerArray;

// ******************
// * TPlayerManager *
// ******************

// The player manager holds the current game player list.  This list can be initialized
// before the beginning of PlayScreen by setting up the players in the network player
// setup screen, or by loading a saved game, etc.  Players are not automatically added
// to the manager when they are created, but are automatically removed when they are
// deleted. 
//
// Net games may use the player manager id# for the player id in the game.
//
// Also, since players are marked as NONMAP objects in the game, they will continue to
// exist after all maps have been deleted.  This means that, to delete all players, you
// must Close(), or Clear() the PlayerManager for the players to be deallocated.

_CLASSDEF(TPlayerManager)
class TPlayerManager
{
  public:
	BOOL Initialize();
	  // Initialize the player manager
	void Close();
	  // Closes the player manager
	void Clear();
	  // Deletes all players
	int NumPlayers() { return players.NumItems(); }
	  // Returns number of areas
	PTPlayer GetPlayer(int playernum) { return players[playernum]; }
	  // Returns the control entry for the given index
	void SetMainPlayer(int playernum);
	  // Sets the main player for the game
	void SetMainPlayer(PTPlayer player);
	  // Sets the main player for the game
	int GetMainPlayerNum() { return mainplayernum; }
	  // Returns the main player number
	PTPlayer GetMainPlayer() { return Player; }
	  // Returns the game's main player (same as just accessing the Player global)
	int AddPlayer(PTPlayer player);
	  // Adds another player and returns index into player array
	void RemovePlayer(int removenum, BOOL collapse = TRUE);
	  // Remove a player, if collapse is true, collapses player list so no empty slot is left
	  // Use collapse when setting up a game, and no collapse when in game so that player id
	  // nums remain the same for network messages.
	void RemovePlayer(PTPlayer removeplayer, BOOL collapse = TRUE);
	  // Remove a player (given player pointer)

  private:
	TPlayerArray players;
	int mainplayernum;
};

#endif
