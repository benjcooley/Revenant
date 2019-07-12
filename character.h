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

#ifndef _SPELL_H
#include "spell.h"
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


// Maximum number of characters a character can remember seeing
#define MAXHASSEEN 8
_STRUCTDEF(SHasSeen)
struct SHasSeen
{
    PTCharacter chr;            // Has seen this character
    int time;                   // Game ticks when last seen
    BOOL noautocombat;          // Prevents player from toggling autocombat
};

// the default invoke animation delay
#define INVOKE_DELAY    20

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
    virtual BOOL DrawShadow() { return FALSE; }
        // no shadow // Characters get a little shadow that follows them
    virtual void Pulse();
      // Calls ExecuteAction
    virtual void Animate(BOOL draw);
      // Draw character
    virtual DWORD Move();
      // Called by system when character moves
    virtual void Notify(int notify, void *ptr);
        // Notify Action (check if objects we depend on are killed)
    virtual int CalculateDamage(int damage, int damagetype, int modifier);
        // Calculate the total damage for the character based on a base damage
        // value (i.e. from the weapon), the damage type (i.e. the value returned from
        // GetDamageType()), and a percentage modifier such as +10%(10) or -30%(-30).  Damage
        // is calculated as damage * (100% + modifier) * (100% + chardmgmodifier).
    virtual void Damage(int damage, int damagetype = DT_NONE, int modifier = 0,
        PTActionBlock action = NULL, PTCharacter attacker = NULL);
        // Apply damage to the character.  If character hit, use 'action' action block
        // instead of default "impact" state, or use 'action' block if he dies instead
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
    BOOL Goto(int x, int y);
      // Causes character to go to x,y.
    BOOL Stop(char *name = NULL);
      // Stops specified action, or any action if name is NULL
    BOOL Disable();
      // Disables character's AI (for freezing, etc.)
    BOOL Pulp(S3DPoint vel, int piece_count, int blood_count);
      // Causes a character to explode into body parts, blood, & guts
    BOOL Burn();
      // Causes a character to combust into a seething sweltering mass of flames
    void ClearBurn() { burning = NULL; }
      // Sets burning pointer to NULL
    BOOL Flail();
      // Causes a character to act a fool
    BOOL KnockBack(S3DPoint frompos);
      // Causes a character to react with a heavy imapct animation, facing towards frompos
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
    BOOL Say(char *string, int wait = -1, char *anim = NULL, char *sound = NULL);
      // Causes character to blather incessantly about something irrelevant
      // (anim is override for animation to play when saying, NULL is "say")
      // Tag indicates that the say command is a index tag into the DialogList
      // list of dialog lines.  The tag will also be used to play the dialog wave file.
    BOOL SayTag(int tagid, int wait = -1, char *anim = NULL);
      // Says something given a dialog tag id number
    BOOL SayTag(char *tag, int wait = -1, char *anim = NULL);
      // Says something given a dialog tag
    BOOL CastByName(char* name, PTObjectInstance *target = NULL, int numtargs = 0, PS3DPoint sourcepos = NULL);
      // Cast a spell by usings its name
    BOOL CastByTalismans(char* talismans, PTObjectInstance *target = NULL, int numtargs = 0, PS3DPoint sourcepos = NULL);
      // Cast a spell by using a talisman list
    BOOL SetCast(char* ani, PTObjectInstance target, int invoke_delay = INVOKE_DELAY);
      // Set the character to the cast animation
    BOOL Cast(char* talismans, PS3DPoint sourcepos = NULL);
      // quick cast a spell
    BOOL BeginFighting(PTCharacter target = NULL, ACTION action = ACTION_COMBAT);
      // Engage character in combat
    BOOL EndFighting();
      // Leave combat mode
    BOOL BeginCombat(PTCharacter target = NULL)
      { return BeginFighting(target, ACTION_COMBAT); }
      // Enter combat mode
    BOOL EndCombat()
      { return EndFighting(); }
      // Leave combat mode
    BOOL BeginBowMode(PTCharacter target = NULL)
      { return BeginFighting(target, ACTION_BOW); }
      // Enter bow combat mode
    BOOL EndBowMode()
      { return EndFighting(); }
      // Leave bow combat mode
    BOOL DrawBow();
      // Begins drawing bow or crossbow
    BOOL AimBow(int angle);
      // Causes character to aim at given angle before shooting bow
    BOOL AimBowLeft();
      // Causes character to pivot to left when aiming bow
    BOOL AimBowRight();
      // Causes character to pivot to right when aiming bow
    BOOL ShootBow(int angle);
      // Shoots bow or crossbow
    BOOL ButtonAttack(int buttonid);
      // Find attack based on the button the player pressed
    BOOL ButtonAction(int buttonid);
      // Use attack list in RULES.DEF to do an action (usually a PLAYANIM or MAGIC tag)
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
    BOOL Block(int frames = -1);
      // Character blocks an attack
    BOOL StopBlock();
      // Character stops blocking
    BOOL Dodge();
      // Character dodges an attack
    BOOL Leap(int angle);
      // Character leaps in the given direction (combat mode only)
    BOOL PlayAnim(char *string);
      // Causes character to play animation name.

  // Info functions specific to characters
    BOOL IsFighting() { return IsCombat() || IsBowMode(); }
      // Returns whether the character is in a fighting mode
    BOOL IsCombat() { return IsRoot(ACTION_COMBAT); }
      // Returns whether the character is in combat or not
    BOOL IsHandCombat() { return root->Is("comhand"); }
      // Returns wether we are in hand to hand combat mode
    BOOL IsBowMode() { return IsRoot(ACTION_BOW); }
      // Returns whether the character is in combat or not
    BOOL IsBowDrawn();
      // Returns true if bow is drawn and we are in aim mode
    BOOL IsAttack() { return IsDoing(ACTION_ATTACK); }
      // Returns whether the character is attacking or not
    PTCharacter Fighting() { if (IsFighting()) return (PTCharacter)root->obj; return NULL; }
      // Return the current target if they are in combat, NULL if not fighting anyone
    BOOL SetFighting(PTCharacter newtarget);
      // Sets the current fighting target
    BOOL IsTalking() { if (doing && doing->Is("say")) return TRUE; return FALSE; }
      // Returns whether character is talking or not
    BOOL IsWalkMode() { if (root && 
        ((IsCombat() && (root->Is("combat") || root->Is("comhand"))) ||
        (IsBowMode() && root->Is("bow")) ||
        root->Is("walk")) ) return TRUE; return FALSE; }
      // Returns whether character is in walk mode
    BOOL IsRunMode() { if (root && 
        ((IsCombat() && (root->Is("combatrun") || root->Is("comhandrun"))) ||
        (IsBowMode() && root->Is("bowrun")) ||
        root->Is("run")) ) return TRUE; return FALSE; }
      // Returns whether character is in run mode
    BOOL IsSneakMode() { if (root && root->Is("sneak")) return TRUE; return FALSE; }
      // Returns whether character is in sneak mode
    BOOL IsMoving()
        { return doing && 
                 doing->action == ACTION_MOVE || 
                 doing->action == ACTION_COMBATMOVE ||
                 doing->action == ACTION_BOWMOVE; }
      // Returns TRUE if character is doing a moving action (result of calling Go())
    BOOL IsGoto() 
        { return IsMoving() && (doing->target.x != 0 || doing->target.y != 0 || doing->target.z != 0); }
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
    PSCharData GetCharData() { return chardata; }
      // Returns character data structure for this char
    ACTION GetMoveAction(ACTION action);
      // Returns the move action for the given root action
    ACTION GetLeapAction(ACTION action);
      // Returns the move action for the given root action
    BOOL IsFlailing() 
        { return doing && doing->action == ACTION_FLAIL; }
      // Returns TRUE if character is acting like a fool (result of calling Go())

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
    PTCharacter CharBlocking() { return CharBlocking(this, Pos(), Radius()); }
        // Calls static function above with this chars parameters
    BOOL Blocked(S3DPoint &pos, S3DPoint &newpos, DWORD bits = 0, int *height = NULL, PTCharacter *bychar = NULL);
      // Returns TRUE if character would be blocked when going to new position
    
  // Miscellaneous functions
    virtual void MoveTo(RS3DPoint newpos) { movepos = newpos; movetopos = TRUE; }
        // Moves object to new position (does walk checking for characters).
        // Use this function instead of SetPos() to avoid moving objects through or onto
        // barriers.
    void SetOnExit();
      // Flags that character is on an exit
    void EffectBurst(char *name, int height = 50);
      // Create a burst effect of the given name
    int GetCombatFlashTicks() { return combatflashticks; }
      // Returns 0 if no flash, or positive number of ticks left if flash being drawn
    void MakeInvisible();
      // Makes character invisible
    void MakeVisible();
      // Makes character visible
    PTSpellManager GetSpellManager() { return &SpellManager; }
      // Get spell manager object

  // Streaming functions
    virtual int ObjVersion() { return 4; }
        // Returns the version id for this object for loading/saving
    virtual void Load(RTInputStream is, int version, int objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector

  // invisibilty functions
    void SetFade(int amt, int amt2 = 5, int amt3 = -1);
    int GetFade(void);
    void UpdateFade(void);

  // Invisible Spell Functions
    BOOL IsInvisibleSpell(){return invisible_spell;}
    void SetInvisibleSpell(BOOL new_val){invisible_spell = new_val;}

  // Teleport functions
    void SetTeleportLevel(int new_level){teleport_level = new_level;}
    void SetTeleportPosition(S3DPoint new_pos)
        {teleport_position.x = new_pos.x; teleport_position.y = new_pos.y; teleport_position.z = new_pos.z;}
    int GetTeleportLevel(void){return teleport_level;}
    S3DPoint GetTeleportPosition(void){return teleport_position;}

  // Functions to remember if characters are seen or not
    BOOL HasSeenMe(PTCharacter me);
      // Have I been seen by this character?
    void SetHasSeen(PTCharacter me);
      // Add me to the HASSEEN list
    BOOL HasSeenAutoCombat(PTCharacter me);
      // Should I go into combat automatically against this guy I've just seen?
    void SetHasSeenAutoCombat(BOOL on);
      // Sets AUTOCOMBAT mode for all recently seen characters.  If set to FALSE, prevents
      // player from automatically entering combat when he moves towards any of the 
      // current batch of enemies.  This is called when EndCombat() is called.

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
    virtual int DefenseModifier() { return chardata->defensemod + SpellManager.GetDefense(); }
      // Return defense modifier
    virtual int AttackModifier() { return chardata->attackmod + SpellManager.GetOffense(); }
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
    virtual int Transparency();
      // Controls the transparency of character's imagery (character animator).  Things
      // such as invisibility spells, visibility and stealth can affect this value.
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
    virtual char *BodyType() { return "na"; }
      // Character's don't use the equipment replacement system
    virtual char *GetCombatRoot(PTObjectInstance oi = NULL) { return "combat"; }
      // Returns combat root given current weapon or weapon type
    virtual char *GetBowRoot(PTObjectInstance oi = NULL) { return "bow"; } 
      // Returns bow root given current bow weapon type
    BOOL CanSeeCharacter(PTCharacter chr, int angle = -1);
      // Returns TRUE if this character can 'see' the last glimpse of 'chr'
    BOOL IsMagicResistant() { return (magic_resistance > 0.0f); }
      // gets the level of magic resistance
    void SetMagicResistance(float pct) { magic_resistance = pct; }
      // sets the level of magic resistance
    float GetMagicResistance() { return magic_resistance; }
      // gets the level of magic resistance
    BOOL GetAutoCombat() { return autocombat; }
      // returns the state of this characters autocombat setting
    void SetAutoCombat(BOOL ac_state) { autocombat = ac_state; }
      // sets the state of this characters autocombat setting

  protected:
   
    virtual void UpdateAction(int bits = 0);
      // Called by Pulse() to update the action blocks

    BOOL ResolveHit(PTCharacter targ, 
        PSCharAttackData attack, PSCharAttackImpact attackimpact, int attackdamage);
    // This function is called by the ResolveAttack() function to resolve hits for
    // multiple characters.  The characters are usually found by calling the FindCharacters()
    // function, then calling this function for each character found.

    // Resolve functions - redefine these in derived classes for different functionality
    virtual int ResolveAction(int bits = 0);
      // Call the resolve functions, below
    virtual int ResolveMove(PTActionBlock ab, int bits);
    virtual int ResolveAttack(PTActionBlock ab, int bits);
    virtual int ResolveCombatMove(PTActionBlock ab, int bits);
    virtual int ResolveCombat(PTActionBlock ab, int bits);
    virtual int ResolveBowAim(PTActionBlock ab, int bits);
    virtual int ResolveBowShoot(PTActionBlock ab, int bits);
    virtual int ResolveBlock(PTActionBlock ab, int bits);
    virtual int ResolveImpact(PTActionBlock ab, int bits);
    virtual int ResolveDead(PTActionBlock ab, int bits);
    virtual int ResolveSay(PTActionBlock ab, int bits);
    virtual int ResolvePivot(PTActionBlock ab, int bits);
    virtual int ResolveLeap(PTActionBlock ab, int bits);
    virtual int ResolvePull(PTActionBlock ab, int bits);

    char *GetAngleMoveAnim(int movedir, int facedir, char *root, char *animname, int buflen);
      // Returns the correct angle movce animation given the current movedir, facedir, and root name
    void AdvanceAngles(int faceang, int moveang, int maxturn);
      // Advance the character's facing and moving angle
    int UpdateAngle(int angle);
      // Check path along current angle and adjuct accordingly
    PTObjectInstance FindObjAhead();
      // Find object in front of character
    BOOL CanHearCharacter(PTCharacter chr);
      // Returns TRUE if this character can hear the last noise made by 'chr'
    int FindCharacters(PTCharacter chars[], int maxchars, 
        int range = 128, int angle = -1, int anglerange = 32, int flags = 0);
      // Finds characters given the above parameters.  Will find all chars in range from
      // direction 'angle' if not -1 with angle range of 32.  Puts the closest character
      // at the beginning of the list, all other characters are in random order.  Returns
      // the number of characters found.
    PTCharacter FindCharacter(int range = 128, int angle = -1, int anglerange = 32, int flags = 0);
      // Calls the FindCharacters function above with only 1 character
      // Finds characters given the above parameters.  Will find all chars in range from
      // direction 'angle' if not -1 with angle range of 32.
    PTCharacter FindCharacterAhead(int angle, int anglerange = 32);
      // Find closest character in this direction
    PTCharacter FindClosestEnemy(int angle = -1, int anglerange = 32);
      // Finds the closest character attacking this character
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
    BOOL FindButtonAttack(int id, int dmgpcnt, int &attacknum, int &impactnum, int &damage, BOOL isaction = FALSE);
      // Finds a valid attack given an attack id (i.e. controller button), and the given random damage
      // percentage 1-100.
      // Returns the correct attacknum, impactnum, and damage value for the found attack.
      // If 'isaction' is set, will find an attack entry with a CA_ACTION flag set instead (called by ButtonAction())
    BOOL FindPcntAttack(int id, int dmgpcnt, int &attacknum, int &impactnum, int &damage);
      // Finds a valid attack given a randomly generated percentage (0-100) number and a given damage percentage.
      // Returns the correct attacknum, impactnum, and damage value for the found attack.
    BOOL DoAttack(int attacknum, int impactnum, int damage);
      // Executes a particular attack (using index into SCharData's attack array)

    // -- Data members --
    BOOL autocombat;            // this mimics the global, but works as a way to allow locke to flee in
                                // the face of too many enemies.

    BOOL movetopos;             // Set to TRUE if character needs to move to movepos
    S3DPoint movepos;           // The position character needs to move to (while walk checking)

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

  // Poison damage stuff
    int lastpoisondamage;

  // Attack stuff
    PSCharAttackData lastattack; // Last attack
    int lastattackticks;         // Game ticks when last attack occured
    int chainhits;               // Number of hits in a chain attack

  // spells stuff
    TSpellManager SpellManager;  // handles the spells the character casts
    int invokedelay;
    PTActionBlock oldab;

    float magic_resistance;     // between 0.0 and 1.0... percentage of magic resistance

    // Visibility
    int fade;
    int fade_step;
    int fade_limit;

  // Invisible Spell Addition
    BOOL invisible_spell;

  // Teleport Coordinates
    int teleport_level;
    S3DPoint teleport_position;

  // burn stuff
    PTObjectInstance burning;

  // Has seen list  
    SHasSeen hasseen[MAXHASSEEN]; // List of characters seen recently

  // Snap stuff
    int snapticks;                // Total number of frames left in snap move

  // combatflash delay
    int combatflashticks;

  // Last bow shot ticks (so we don't shoot bow too fast)
    int lastbowshot;

  // AI fix, this variable keeps track of the last position we saw the enemy at
    S3DPoint target_last_position;
  // AI fix, this variable keeps track of when to save the last position of the enemy
    int last_position_count;
    BOOL target_out_of_sight;
    int last_position_distance;
    S3DPoint last_position_start_point;
    int target_last_angle;
};

DEFINE_BUILDER("Character", TCharacter)

#endif
