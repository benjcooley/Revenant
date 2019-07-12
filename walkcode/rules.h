// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                rules.h - TRules object module                  *
// *************************************************************************

// This file contains the TRules object, which stores the special global
// character type information for the various characters in the game, class 
// information, and any other user defined rules that are stored in the
// RULES.DEF file.  
//
// See the RULES.DEF file for the definitions of the data stored there

#ifndef _RULES_H
#define _RULES_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _CHARSTATS_H
#include "charstats.h"
#endif

#define MAXCHARATTACKS 32

// Character flags

#define CF_UNDEAD       0x0001  // This character can only be damaged by magical/silver weapons
#define CF_MAGICAL      0x0002  // This character can only be damaged by magical weapons
#define CF_INFRAVISION  0x0004  // This character can see in the dark
#define CF_LIGHTBLIND   0x0008  // This character is blinded by light (reverse dark/light sight)

// Note, searches for attack by searching for 'combo' attacks first, then 'special'
// attacks, then ordinary attacks.  This allows the same controller key to be used for
// different attacks based on the opponent, and for extra attacks

#define CA_SPECIAL       0x0001 // Special attack, set when this attack may override a normal attack
#define CA_RESPONSE      0x0002 // Set when 'response' non-empty (response enabled by opponent state)
#define CA_SPLATTER      0x0004 // Causes opponent to splatter if successful
#define CA_DEATH         0x0008 // Causes opponent to die if successful
#define CA_HAND          0x0010 // Is a thrust type attack (can't do with axe)
#define CA_THRUST        0x0020 // Is a thrust type attack (can't do with axe)
#define CA_SLASH         0x0040 // This is a slashing attack (if axe, uses CHOP damage type)
#define CA_CHOP          0x0080 // Is a chop type attack
#define CA_SPARKS        0x0100 // Show sparks on block
#define CA_BLOOD         0x0200 // Show blood on impact
#define CA_SNAPIMPACT    0x0400 // Snap the impact
#define CA_SNAPBLOCK     0x0800 // Snap the block
#define CA_NOMISS        0x1000 // Will play entire attack ani even if misses
#define CA_SNEAK         0x2000 // This attack only available in sneak mode
#define CA_COMBO         0x4000 // Will automatically play next attack when next button pressed
#define CA_AUTOCOMBO     0x8000 // Will play this attack, then the next attack after waiting

// Damage types

#define NUMDAMAGETYPES 10

#define DT_NONE     -1      // Does no damage whatsoever
#define DT_MISC     0       // Just damage (no specific type)
#define DT_HAND     1       // Damage from hand to hand combat
#define DT_PUNCTURE 2       // Puncture damage (arrows, knives and swords thrusts)
#define DT_CUT      3       // Cutting or slashing damage (knive and sword swings)
#define DT_CHOP     4       // Chopping damage (knive, sword, or axe chops)
#define DT_BLUDGEON 5       // Bludgeoning damage (warhammers, clubs, maces)
#define DT_MAGICAL  6       // Magical damage (other than any that fit below)
#define DT_BURN     7       // Burn damage (i.e. lava, fileball, etc.)
#define DT_FREEZE   8       // Freeze damage (i.e. frost, etc.)
#define DT_POISON   9       // Poison damage

#define ANIMLISTLEN 48
#define CHARGROUPLEN 48

_STRUCTDEF(SCharAttackData)
struct SCharAttackData
{
    int flags;                      // Defines attack flags
    int button;                     // Attack button id (for joystick/keyboard button)
    char attackname[MAXANIMNAME];   // Name of attack animation to use for attack
    char responsename[MAXANIMNAME]; // Is response combo/counter when opponent in this state
    char impactname[MAXANIMNAME];   // Impact to use if hit
    char blockname[MAXANIMNAME];    // Block opponent may use to block shot
    char missname[MAXANIMNAME];     // Miss animation to use if character is blocked or misses
    char deathname[MAXANIMNAME];    // Death animation to use if character is toasted by attack
    int blocktime;                  // Frames after attack in which opponent can block/counterattack
    int impacttime;                 // Frames to wait to start impact
    int waittime;                   // Frames from attack beginning until char can do new move
    int stunwait;                   // Frames to wait during impact to show stun
    int combowait;                  // Amount of time before doing next combo
    int mindist, maxdist;           // Minimum and maximum distance this attack can be used for
    int snapdist;                   // Snap to distance
    int hitminrange, hitmaxrange;   // Hits character only when within this range
    int hitangle;                   // Hits character only when he is within this angle
    int damagemod;                  // Damage modifier for attack (10=+10%,-30=-30%)
    int fatigue;                    // Base fatigue value for attack
    int attackskill;                // PLAYER: Min offensive skill level to enable attack
    int weaponmask;                 // PLAYER: Weapon mask for this attack
    int weaponskill;                // PLAYER: Min weapon skill level to enable attack
    int attackpcnt;                 // MONSTER: Percentage 0-100 monster will attempt this attack
};

typedef TVirtualArray<SCharAttackData, 16, 0> TCharAttackArray;

_STRUCTDEF(SClassData)
struct SClassData
{
    BOOL Load(char *aname, TToken &t);
      // Loads the class info from the "CHAR.DEF" file

    char name[RESNAMELEN];              // Name of character type (not individual name)
    int statreqs[NUM_PLRSTATS];         // Stat requirements (-x=below,+x=above,0=don't care)
    int skillmods[NUM_SKILLS];          // Plus or minus modifiers for skills
    int healthmod, fatiguemod, manamod; // Class based health fatigue and mana percent mods
};

typedef TPointerArray<SClassData, 16, 0> TClassDataArray;

_STRUCTDEF(SCharData)
struct SCharData
{
    SCharData();
      // Sets default values
    ~SCharData();
      // Kills attack array
    BOOL Load(char *aname, TToken &t);
      // Loads the character from the "CHAR.DEF" file

    char name[RESNAMELEN];              // Name of character type (not individual name)
    char groups[CHARGROUPLEN];          // Groups this character belongs to (not including char type)
    char enemies[CHARGROUPLEN];         // Groups this character attacks (can include char types as well as groups)
    int objtype;                        // Type data is associated with
    int objclass;                       // Class data is associated with
    int flags;                          // Character flags
    TCharAttackArray attacks;           // Attack info
    int damagemods[NUMDAMAGETYPES];     // Damage modifiers for character (100=100% of damage value)
    char blocksounds[SOUNDLISTLEN];     // Sound played when char attack is blocked
    char misssounds[SOUNDLISTLEN];      // Sound played when char attack missed
    int playerblockmin, playerblockstep, playerblockinc; // PLAYER: min pcnt, SK_DEFENSE step, and inc
    int combatrange;                    // Range at which monster or player will go into combat
    PSClassData classdata;              // PLAYER: Class of character
    int blockfreq, blockmin, blockmax;  // MONSTER: block freq, min block time, max block time
    int sightmin, sightmax, sightrange, sightangle; // MONSTER: range at which monster sees or hears
    int hearingmin, hearingmax, hearingrange;       // MONSTER: hearing ability and hearing range
    int weapontype;                     // MONSTER: weapon type
    int weapondamage;                   // MONSTER: damage for monster's weapon
    int armorvalue;                     // MONSTER: Armor value
    int defensemod;                     // MONSTER: Defense modifier
    int attackmod;                      // MONSTER: Offsense modifier
    int minattackfreq, maxattackfreq;   // MONSTER: Min and max attack freq in 100ths of second
    int mana, fatigue, health;          // MONSTER: Max values for monsters
};

typedef TPointerArray<SCharData, 16, 0> TCharDataArray;

_CLASSDEF(TRules)
class TRules
{
  private:
    BOOL initialized;               // Are we ready
    TClassDataArray classdata;      // Class data
    TCharDataArray chardata;        // Data array
    PSCharData def;                 // Default data object (used for ordinary chars)

  public:
    TRules() { initialized = FALSE; }
    ~TRules() { Close(); }

    BOOL Initialize();
      // Initializes character data stuff
    void Close();
      // Kills all character data stuff
    int GetNumClasses() { return classdata.NumItems(); }
      // Returns number of character classes in data manager
    PSClassData GetClass(char *classname);
      // Gets a character class based on the class name
    PSClassData GetClass(int num) { return classdata[num]; }
      // Gets class data given class num
    int GetNumCharData() { return chardata.NumItems(); }
      // Returns number of character data items in manager
    PSCharData GetCharData(int num) { return chardata[num]; }
      // Returns a particular character data item
    PSCharData GetCharData(int objtype, int objclass);
      // Gets pointer to character data structure
    BOOL Load();
      // Loads character data and class data from CHAR.DEF file

  // Daytime...
    int daylength;                  // Length of day in 100ths of a second
    int twilight, twilightsteps;    // Length of twilight period (morning/evening) in 100ths of a second

  // Health recovery values
    int healthperlevel, fatigueperlevel, manaperlevel;
    int healthrecovval, fatiguerecovval, manarecovval;
    int healthrecovrate, fatiguerecovrate, manarecovrate;

  // Stealth mode values
    int maxstealth, sneakstealth, minstealth;
};

#endif
