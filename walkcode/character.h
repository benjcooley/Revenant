// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   character.h - TCharacter module                     *
// *************************************************************************

#ifndef _CHARACTER_H
#define _CHARACTER_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _COMPLEXOBJ_H
#include "complexobj.h"
#endif

#ifndef _RULES_H
#include "rules.h"
#endif

#ifndef _CHARSTATS_H
#include "charstats.h"
#endif

// Wait types
#define WAIT_NOTHING        0
#define WAIT_RESPONSE       1
#define WAIT_CHAR_DONE      2
#define WAIT_TICKS          3

// FindChar flags
#define FINDCHAR_ENEMY     1    // Find only enemies
#define FINDCHAR_HEAR      2    // Find only characters we can hear
#define FINDCHAR_SEE       4    // Find only characters we can see

_CLASSDEF(TCharacter)
class TCharacter : public TComplexObject
{
  public:
    void ClearChar();       // Clear out working vars of char

    TCharacter(PTObjectImagery newim) : TComplexObject(newim) { ClearChar(); }
    TCharacter(PSObjectDef def, PTObjectImagery newim) : TComplexObject(def, newim) { ClearChar(); }

    virtual int CursorType(PTObjectInstance inst = NULL);
        // Talk icon if they are friendly, attack icon if aggressive, hand if dead
    virtual BOOL Use(PTObjectInstance user, int with = -1);
        // Talk to or attack character
    virtual BOOL DrawShadow() { return TRUE; }
        // Characters get a little shadow that follows them
    virtual void Pulse();
      // Calls ExecuteAction
    virtual void Animate(BOOL draw);
      // Draw character
    virtual DWORD Move();
      // Move character
    virtual void Notify(int notify, void *ptr);
        // Notify Action (check if objects we depend on are killed)
    virtual int CalculateDamage(int damage, int damagetype, int modifier);
        // Calculate the total damage for the character based on a base damage
        // value (i.e. from the weapon), the damage type (i.e. the value returned from
        // GetDamageType()), and a percentage modifier such as +10%(10) or -30%(-30).  Damage
        // is calculated as damage * (100% + modifier) * (100% + chardmgmodifier).
    virtual void Damage(int damage, int damagetype = DT_NONE, int modifier = 0,
        PTActionBlock impact = NULL, PTActionBlock death = NULL);
        // Apply damage to the character.  If character hit, use 'impact' action block
        // instead of default "impact" state, or use 'death' block if he dies instead
        // of default "dead" state.  If impact and death are NULL, uses default "impact"
        // and "dead".  Note that damage is modified based on monsters resistance to 
        // 'damagetype' damage unless damagetype is DT_NONE, in which case the exact
        // damage value in 'damage' is used without ANY modification.
    void RestoreHealth();
        // Cure them of all ailments and set health to max

    virtual void AI();
        // Causes the object to perform its A.I. routines

    virtual char *DefaultRootState() { return (Sleeping() ? "sleep" : Aggressive() ? "combat" : IsDead() ? "dead" : "walk"); }
      // Returns the default root state for this char

  // Action response functions to trigger character AI
    virtual void SignalMovement(PTObjectInstance actor);
        // Actor is moving
    virtual void SignalHostility(PTObjectInstance actor, PTObjectInstance target);
        // Actor is hostile to target
    virtual void SignalAttack(PTObjectInstance actor, PTObjectInstance target);
        // Actor is attacking target

  // ActionBlock generic function callers
    BOOL SetWalkMode();
      // Sets walk mode
    BOOL SetSneakMode();
      // Sets sneak mode
    BOOL SetRunMode();
      // Sets run mode
    BOOL Go(int angle = -1);
      // Start a character moving in the given angle and speed (entry point)
    BOOL Go(S3DPoint vect);
      // Start a character moving in the given movement vector
    BOOL Goto(int x, int y, BOOL exact = FALSE, int endfacing = -1);
      // Causes character to go to x,y.
    BOOL Stop(char *name = NULL);
      // Stops specified action, or any action if name is NULL
    BOOL Disable();
      // Disables character's AI (for freezing, etc.)
    BOOL Pulp(S3DPoint vel, int piece_count, int blood_count);
      // Causes a character to explode into body parts, blood, & guts
    BOOL Burn();
      // Causes a character to combust into a seething sweltering mass of flames
    BOOL Jump();
      // Causes character to jump (in normal mode, use Leap in Combat mode)
    BOOL Pivot(int angle);
      // Pivots character to given direction (in 32 increments)
    BOOL FollowChar(PTObjectInstance inst);
      // Causes character to follow another character.
    BOOL Pickup(PTObjectInstance inst);
      // Causes character to move to and pickup object.
    BOOL Pull(PTObjectInstance inst);
      // Causes character to pull lever
    BOOL TryUse();
      // Attempts to use something in the direction character is facing
    BOOL TryGet();
      // Attempts to get something in the direction character is facing
    BOOL Say(char *string, int wait = -1, char *anim = NULL);
      // Causes character to blather incessantly about something irrelevant
      // (anim is override for animation to play when saying, NULL is "say")
    BOOL Invoke(int *spell, int len);
      // Invoke a spell with talisman string in spell and length of string in len
    BOOL BeginCombat(PTCharacter target = NULL);
      // Engage character in combat
    BOOL EndCombat();
      // Leave combat mode
    BOOL ButtonAttack(int buttonid);
      // Find attack based on the button the player pressed
    BOOL RandomAttack(int pcnt);
      // Find a random attack (for a monster) based on a value from 1-100
    BOOL SpecificAttack(int attacknum);
      // Do a specific attack given the attacknum
    BOOL Swing() { return ButtonAttack(1); }
      // Character swings their weapon at their current target
    BOOL Thrust() { return ButtonAttack(2); }
      // Character thrusts their weapon at their current target
    BOOL Chop() { return ButtonAttack(3); }
      // Character chops with weapon at current target
    BOOL Combo(int num) { return ButtonAttack(3 + num); }
      // Character does combo number num
    BOOL Block();
      // Character blocks an attack
    BOOL Dodge();
      // Character dodges an attack
    BOOL Leap(int angle);
      // Character leaps in the given direction (combat mode only)
    BOOL PlayAnim(char *string);
      // Causes character to play animation name.

  // Info functions specific to characters
    BOOL IsFighting();
      // Returns whether the character is in combat or not
    PTCharacter Fighting() { if (IsFighting()) return (PTCharacter)root->obj; return NULL; }
      // Return the current target if they are in combat, NULL if not fighting anyone
    BOOL SetFighting(PTCharacter newtarget);
      // Sets the current fighting target
    BOOL IsTalking() { if (doing && doing->Is("say")) return TRUE; return FALSE; }
      // Returns whether character is talking or not
    BOOL IsWalkMode() { if (root && root->Is("walk")) return TRUE; return FALSE; }
      // Returns whether character is in walk mode
    BOOL IsRunMode() { if (root && root->Is("run")) return TRUE; return FALSE; }
      // Returns whether character is in run mode
    BOOL IsSneakMode() { if (root && root->Is("sneak")) return TRUE; return FALSE; }
      // Returns whether character is in sneak mode
    BOOL IsMoving() 
        { return doing && doing->action == ACTION_MOVE || doing->action == ACTION_COMBATMOVE; }
      // Returns TRUE if character is doing a moving action (result of calling Go())
    BOOL IsDead() { return (Health() <= 0); }
      // Dammit Jim, I'm a corpse not a doctor..
    BOOL IsEnemy(PTCharacter chr);
      // Returns TRUE if the character is an enemy
    BOOL IsFinalState();
      // Returns whether character is in their last days
    int SqrRadius() { int s = Radius(); s *= s; return s; }
      // Util function
    BOOL ExecutingQueued();
      // Find out whether character is executing a queued set of actions

  // Wait functions
    void Wait(int waitlen);
      // Wait for specified number of frames to elapse
    void WaitChar(PTObjectInstance inst) { if (doing) doing->obj = inst; waittype = WAIT_CHAR_DONE; }
      // Waits for another character to finish his current action
    void WaitResponse() { waittype = WAIT_RESPONSE; }
      // Waits for the player to pick a response in the response panel
    void ForceCommandDone() { forcecommanddone = TRUE; }
      // Forces the current command to be done

    // Static access functions
    static PTCharacter CharBlocking(PTObjectInstance inst, RS3DPoint pos, int radius = 0);
        // Find if a character is blocking movement to this position
    
  // Miscellaneous functions
    void SetOnExit();
        // Flags that character is on an exit
    void EffectBurst(char *name, int height = 50);
        // Create a burst effect of the given name

  // Streaming functions
    virtual int ObjVersion() { return 1; }
        // Returns the version id for this object for loading/saving
    virtual void Load(RTInputStream is, int version, int objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector

  // invisibilty functions
    void MakeInvisible();
    void MakeVisible();

  // Character class stats
    STATFUNC(Radius)

  // Character object stats
    OBJSTATFUNC(Aggressive)
    OBJSTATFUNC(Poisoned)
    OBJSTATFUNC(Sleeping)
    OBJSTATFUNC(Health)
    OBJSTATFUNC(Fatigue)
    OBJSTATFUNC(Mana)

   // Calculated stats
    virtual int MaxHealth() { return chardata->health; }
      // Returns monster max health value
    virtual int MaxFatigue() { return chardata->fatigue; }
      // Returns monster max fatigue value
    virtual int MaxMana() { return chardata->mana; }
      // Returns monster max mana value
    virtual int BlockPcnt() { return chardata->blockfreq; }
      // Returns percentage of time character will block an attack
    virtual int ArmorValue() { return chardata->armorvalue; }
      // Return armor value
    virtual int DefenseModifier() { return chardata->defensemod; }
      // Return defense modifier
    virtual int AttackModifier() { return chardata->attackmod; }
      // Return offense modifier
    virtual int FatigueModifier() { return (Fatigue() * 4 / MaxFatigue()) - 4; }
      // Return fatigue modifier
    virtual int DamageModifier(int damagetype) { return chardata->damagemods[damagetype]; }
      // Returns percentage of damage
    virtual int WeaponType() { return chardata->weapontype; }
      // Returns the type of weapon being used
    virtual int WeaponDamage() { return chardata->weapondamage; }
      // Returns current weapon damage
    int GetDamageType(int weapontype, int attackflags);
      // Returns the DT_XXX damage type flags for the given weapon type and attack flags
    virtual int Visibility();
      // Returns the total visibility 1-100 for character (based on lights, ambient, and fog, etc.)
    virtual int Hearing(int dist);
      // Returns a 1-100 hearing value which indicates how the average noise will be heard
      // by a monster.  If the monster is sleeping, the listening value is 20% of normal.
    virtual int Sight(int dist);
      // Returns a 1-100 sight value which indicates how the average char will be seen
      // by a monster in the darkness.  If the monster is sleeping, the sight value
      // is always 0.
    virtual int StealthMod() { return 0; }
      // Ordinary characters dont have stealth
    virtual int LastGlimpse() { return glimpse; }
      // What was the last visibility glimpse for the character.  1 is complete
      // and utter invisibility, 100 is plain as day visibility
    virtual int LastNoise() { return noise; }
      // What was the last noise value (1-100) for the character. 1 is pindrop,
      // 100 is pots and pans crashing.

  protected:
   
    virtual void UpdateAction(int bits = 0);
      // Called by pulse() to update the current action block

    // Resolve functions - redefine these in derived classes for different functionality
    virtual int ResolveAction(int bits = 0);
      // Call the resolve functions, below
    virtual int ResolveMove(PTActionBlock ab, int bits);
    virtual int ResolveAttack(PTActionBlock ab, int bits);
    virtual int ResolveCombat(PTActionBlock ab, int bits);
    virtual int ResolveCombatMove(PTActionBlock ab, int bits);
    virtual int ResolveBlock(PTActionBlock ab, int bits);
    virtual int ResolveImpact(PTActionBlock ab, int bits);
    virtual int ResolveDead(PTActionBlock ab, int bits);
    virtual int ResolveSay(PTActionBlock ab, int bits);
    virtual int ResolvePivot(PTActionBlock ab, int bits);
    virtual int ResolveInvoke(PTActionBlock ab, int bits);
    virtual int ResolveLeap(PTActionBlock ab, int bits);
    virtual int ResolvePull(PTActionBlock ab, int bits);

    void AdvanceAngles(int faceang, int moveang, int maxturn);
      // Advance the character's facing and moving angle
    int UpdateAngle(int angle);
      // Check path along current angle and adjuct accordingly
    PTObjectInstance FindObjAhead();
      // Find object in front of character
    BOOL CanHearCharacter(PTCharacter chr);
      // Returns TRUE if this character can hear the last noise made by 'chr'
    BOOL CanSeeCharacter(PTCharacter chr, int angle = -1);
      // Returns TRUE if this character can 'see' the last glimpse of 'chr'
    PTCharacter FindCharacter(int range = 128, int angle = -1, int anglerange = 32, int flags = 0);
      // Finds characters given the above parameters.  Will find all chars in range from
      // direction 'angle' if not -1 with angle range of 32.
    PTCharacter FindCharacterAhead(int angle, int anglerange = 32);
      // Find closest character in this direction
    PTCharacter FindClosestEnemy(int angle = -1, int anglerange = 32);
      // Finds the closest character attacking this character
    BOOL Blocked(S3DPoint &pos, S3DPoint &newpos, DWORD bits = 0, int *height = NULL, PTCharacter *bychar = NULL);
      // Gets blocked status for char
    void ResetStealthValues();
      // Based on character position, lights, and stealth, sets noise and glimpse

  // Attack functions   
    BOOL IsValidAttack(int attacknum, int &impactnum, int &damage,
        int tdist, int id, int pcnt, int dmgpcnt, int flagmask, int flags);
      // Checks attack to see if attack is valid or not, returns TRUE if valid, and the correct impact and
      // damage value for the attack in 'impactnum' and 'damage'.  Must give function target distance in 'tdist',
      // button id in 'id' or -1 if no button, random attack pcnt in 'pcnt' or -1 if no random attack pcnt, 
      // the random damage percentage 1-100 in 'dmgpcnt', and the attack flagmask and flags to specify what
      // kinds of attacks we're looking for.
    BOOL FindButtonAttack(int id, int dmgpcnt, int &attacknum, int &impactnum, int &damage);
      // Finds a valid attack given an attack id (i.e. controller button), and the given random damage
      // percentage 1-100.
      // Returns the correct attacknum, impactnum, and damage value for the found attack.
    BOOL FindPcntAttack(int id, int dmgpcnt, int &attacknum, int &impactnum, int &damage);
      // Finds a valid attack given a randomly generated percentage (0-100) number and a given damage percentage.
      // Returns the correct attacknum, impactnum, and damage value for the found attack.
    BOOL DoAttack(int attacknum, int impactnum, int damage);
      // Executes a particular attack (using index into SCharData's attack array)

    // -- Data members --
    PSCharData chardata;        // Pointer to global character settings for this type of char

    int waittype;               // Wait for this before continuing script execution
    int waitticks;              // Number of ticks to wait for no action block wait

    BOOL forcecommanddone;      // For skipping past animations
    BOOL forcenomove;           // For forcing end movement

    DWORD charflags;            // Character flags

    int exittimestamp;          // When timestamp is +2 frames from current frame, OF_ONEXIT is cleared
    BOOL is_invisible;          // is our character affected by invisibility

  // Move stuff
    int shovedir;               // Last choice (left/right) for going around an obstacle
    
  // Stealth Stuff
    int nextattack;             // Ticks till next attack
    int glimpse;                // Value from 1-100 indicating how visible last move was
    int noise;                  // Value from 1-100 indicating how quiet last move was

  // Stat recovery stuff
    int lasthealthrecov;        // Game time values for last
    int lastfatiguerecov;       // health, fatigue, and mana recovery
    int lastmanarecov;      

  // Attack stuff
    int lastattacknum;           // Last attack num
    PSCharAttackData lastattack; // Last attack
    int lastattackticks;         // Game ticks when last attack occured
};

DEFINE_BUILDER("Character", TCharacter)

#endif
