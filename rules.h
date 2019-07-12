// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   rules.h - TRules object module                      *
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
#define CA_DEATH         0x0008 // Used when damage value is high enough to kill enemy
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
#define CA_CHAIN         0x4000 // Will automatically play next attack when next button pressed
#define CA_AUTOCOMBO     0x8000 // Will play this attack, then the next attack after waiting
#define CA_ATTACKDOWN    0x40000 // Attack for when opponent is on ground only!
#define CA_ATTACKSTUN    0x80000 // Attack for when opponent is stunned only!
#define CA_MOVING        0x100000 // This attack is only available when character is moving
#define CA_ONETARGET     0x200000 // This attack is applied to only one character
#define CA_NOPUSH        0x400000 // This attack will not push the other character
#define CA_MAGICATTACK   0x800000 // This is a magical attack (uses the MAGICATTACK tag)
#define CA_PLAYANIM      0x1000000 // Just plays the given animation
#define CA_INTERACTIVE   0x2000000 // This is an interactive attack (syncs impacts, nopush, nocharblock)
#define CA_SNEAKMODE     0x4000000 // This attack works in sneak mode
#define CA_WALKMODE      0x8000000 // This attack works in walk mode
#define CA_BOWMODE       0x10000000 // This attack works in bow mode
#define CA_RUNNING       0x20000000 // This attack is a running attack
#define CA_ACTION        0x40000000 // This is an action, not an attack

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

#define CAI_STUN        0x0001  // Results in a looping stun using 'loopname' and 'loopwait'
#define CAI_KNOCKDOWN   0x0002  // Results in a looping knockdown using 'loopname' and 'loopwait'
#define CAI_DEATH       0x0004  // This describes a death impact (other than default "death" state)
#define CAI_WHENSTUNNED 0x0008  // This impact will only be used when the character is stunned
#define CAI_WHENDOWN    0x0010  // This impact will only be used when the character is knocked down
#define CAI_FLYBACK     0x0020  // This impact causes character to fly back (use with CAI_KNOCKDOWN!)
#define CAI_BLOOD       0x0040  // This impact spouts blood

#define MAXATTACKIMPACTS 6
#define MAXCHARIMPACTS   6

_STRUCTDEF(SCharAttackImpact)
struct SCharAttackImpact
{
    char impactname[MAXANIMNAME];   // Impact name for this impact
    int flags;
    char loopname[MAXANIMNAME];     // Animation played when opponent stunned, knocked down, killed
    int looptime;                   // Amount of time to play loop animation
    int damagemin, damagemax;       // Damage range for this impact
    int snapdist, snaptime;         // Dist from chr to snapto (or pushto) in snaptime frames
};

_STRUCTDEF(SCharAttackData)
struct SCharAttackData
{
    char attackname[MAXANIMNAME];   // Name of attack animation to use for attack
    int flags;                      // Defines attack flags
    int button;                     // Attack button id (for joystick/keyboard button)
    int attackpcnt;                 // MONSTER: Percentage 0-100 monster will attempt this attack
    int mindist, maxdist;           // Minimum and maximum distance this attack can be used for
    union
    {
      struct    // Information for ordinary attack
      {
        char responsename[MAXANIMNAME]; // Is response combo/counter when opponent in this state
        char blockname[MAXANIMNAME];    // Block opponent may use to block shot
        char missname[MAXANIMNAME];     // Miss animation to use if character is blocked or misses
        char chainname[MAXANIMNAME];    // This attack follows this previous attack in attack chain
        int blocktime;                  // Frames after attack in which opponent can block/counterattack
        int impacttime;                 // Frames to wait to start impact
        int chainexptime;               // Next attack in chain must be done before this time expires
        int nextwait;                   // Frames to wait before doing next attack
        int hitminrange, hitmaxrange;   // Hits character only when within this range
        int hitangle;                   // Hits character only when he is within this angle
        int damagemod;                  // Damage modifier for attack (10=+10%,-30=-30%)
        int fatigue;                    // Base fatigue value for attack
        int attackskill;                // PLAYER: Min offensive skill level to enable attack
        int weaponmask;                 // PLAYER: Weapon mask for this attack
        int weaponskill;                // PLAYER: Min weapon skill level to enable attack
        int numimpacts;                 // Number of impacts for attack
        SCharAttackImpact impacts[MAXATTACKIMPACTS];  // Impacts for attack
     };
     struct     // Information for magical attack
     {
        char spellname[RESNAMELEN];     // Name of spell to cast
        S3DPoint spellsource;           // Offset of spell source for this character
     };
    };
};

typedef TVirtualArray<SCharAttackData, 16, 16> TCharAttackArray;

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

typedef TPointerArray<SClassData, 16, 16> TClassDataArray;

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
    int combatrangemin, combatrangemax; // Range at which monster or player will go into combat
    int maxattackrange;                 // Range at which monster will stop chasing, and start attacking
    SColor swipecolor;                  // Sword swipe color
    char bodytype[MAXANIMNAME];         // PLAYER: Type of body (for selecting equipment geometry)
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
    int walkspeed, runspeed, sneakspeed, combatwalkspeed;
                                        // Speeds for character's movement
    S3DPoint arrowpos;                  // Where arrow starts relative to character
    int arrowspeed;                     // How fast arrow goes
    int bowwait;                        // How long to wait before character can shoot next arrow
    int bowaimspeed;                    // How fast do we pivot when aiming bow
    int numimpacts;
    SCharAttackImpact impacts[MAXCHARIMPACTS]; // Default impacts for characters
};

typedef TPointerArray<SCharData, 16, 16> TCharDataArray;

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

  // Poison damage values
    int poisondamageval;
    int poisondamagerate;


  // Stealth mode values
    int maxstealth, sneakstealth, minstealth;
};

#endif
