// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                complexobj.h - TComplexObject module                   *
// *************************************************************************

#ifndef _COMPLEXOBJ_H
#define _COMPLEXOBJ_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

// States of completion for actions
#define COM_PENDING         0           // Can't start command yet
#define COM_EXECUTING       1           // Command is being executed
#define COM_COMPLETED       2           // Command is complete
#define COM_IMPOSSIBLE      3           // Can't get to this state from here

// Animate action (the default)
typedef enum {
    ACTION_NONE,
    ACTION_ANIMATE,
    ACTION_MOVE,
    ACTION_COMBAT,
    ACTION_COMBATMOVE,
    ACTION_COMBATLEAP,
    ACTION_COLLAPSE,
    ACTION_ATTACK,
    ACTION_BLOCK,
    ACTION_DODGE,
    ACTION_MISS,
    ACTION_INVOKE,
    ACTION_IMPACT,
    ACTION_STUN,
    ACTION_KNOCKDOWN,
    ACTION_FLYBACK,
    ACTION_SAY,
    ACTION_PIVOT,
    ACTION_PULL,
    ACTION_DEAD,
    ACTION_PULP,
    ACTION_BURN,
    ACTION_FLAIL,
    ACTION_SLEEP,
    ACTION_LEAP,
    ACTION_BOW,
    ACTION_BOWMOVE,
    ACTION_BOWAIM,
    ACTION_BOWSHOOT,
} ACTION;

// Note: Please define derived class actions in increments of 100 for each 
// derived class (i.e. CHARACTER 100-199, PLAYER 200-299

// Action block - data about a desired or executing object action (state)
_CLASSDEF(TActionBlock)
class TActionBlock
{
    // !!!!***** WARNING *****!!!! DO NOT PUT VIRTUAL FUNCTIONS IN THIS OBJECT!!!
    // !!!! --- THIS MEANS YOU!!  !!!!

  public:
    TActionBlock() { ClearBlock(); }
    TActionBlock(char *n, ACTION a = ACTION_ANIMATE);               // Animation/action
    TActionBlock(char *n, char *str, ACTION a = ACTION_ANIMATE);    // Animation+string/action
    TActionBlock(TActionBlock &ab, char *str = NULL, ACTION = ACTION_NONE); // Copies another action block
    
    ~TActionBlock() { if (data) free(data); }

    BOOL Is(char *s);           // Multipurpose match (?=any one char, *=0 or more chars, #=any num, [xx]=any one char in braces)
    BOOL Is(ACTION a) { return action == a; } // Is an action
    BOOL IsRight(char *state);  // Is left step for movement
    BOOL IsLeft(char *state);   // Is right step for movement
    BOOL IsStep(char *state);   // Is step for movement
    BOOL IsOneOf(char *state);  // TRUE if state is one of the master state (i.e. "attack1" is one of "attack")
    BOOL IsPartOf(char *prefix, char *state = NULL); // TRUE if state has the given prefix and state name, (doesn't care about suffix)
    void SwapRoot(char *root, char *newroot);  // Swaps the root part of the name with a new root

    int StateNum();             // Returns number of state (i.e. 1 for "attack1")
    void ClearBlock();          // Clears the action block

    ACTION action;              // Action id (what we're doing)
    char name[RESNAMELEN];      // Name of state
    int frame;                  // Frame number to start on when command begins
    int wait;                   // Delay for current command
    int angle, moveangle, turnrate; // Angle of movement and turn rate
    S3DPoint target;            // Target location for movement
    PTObjectInstance obj;       // Target object for action
    PSCharAttackData attack;    // Attack info (if is attack/impact/death/stun/knockdown action)
    PSCharAttackImpact impact;  // Impact info (if is attack/impact/death/stun/knockdown action)
    int damage;                 // Damage attack will do (if hits)
    void *data;                 // Data field (such as text for say)
    union
    {
      DWORD flags;
      struct
      {
        unsigned int firsttime : 1;     // If this is the first time through
        unsigned int transition : 1;    // If currently transitioning to this state
        unsigned int terminating : 1;   // Action block is shutting down
        unsigned int priority : 1;      // Action can't be changed by calling SetDesired() until played
        unsigned int interrupt : 1;     // Interrupts current doing animation unless doing has priority
        unsigned int nowaitdone : 1;    // Allows desired to interrupt this action
        unsigned int dontforce : 1;     // Don't force the animation if transition doesn't exist
        unsigned int stop : 1;          // Abort this action
        unsigned int waitpivot : 1;     // For movenent actions, wait until pivot done before moving
        unsigned int noroot : 1;        // Don't use this as a root state (even if playing a root animation)
        unsigned int loop : 1;          // Loop this command
      };
    };
};

// Helper functions to make it easy to make state names
char *StName(char *name, int num);
char *StName(char *name, char *str);

// TComplexObject adds the state transition code to TObjectInstance.
// It is not intended for use as a seperate object class any more than
// TObjectInstance, but rather as a parent class in order to give certain
// object classes advanced state functionality.

_CLASSDEF(TComplexObject)
class TComplexObject : public TObjectInstance
{
  public:
    void ClearComplexObj();

    TComplexObject(PTObjectImagery newim) : TObjectInstance(newim) { ClearComplexObj(); }
    TComplexObject(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { ClearComplexObj(); }

    virtual void Pulse();
      // Main pulse (done before frame is drawn)

    BOOL IsInRoot() { return (doing == root); }
        // Returns whether the object is in their root state or not

    void Try(char *state)
        { PTActionBlock ab = new TActionBlock(state); ab->angle = GetFace(); SetDesired(ab); }
        // Trys to change to the given state
    int Force(char *state)
        { PTActionBlock ab = new TActionBlock(state); ab->angle = GetFace(); return ForceCommand(ab); }
        // Forces the given state
    // this is a hack               ---v
    BOOL IsDoing(char *state) { if (!this) return FALSE; else return doing ? doing->Is(state) : FALSE; }
        // Returns true if currently doing the given state
    BOOL IsDesired(char *state) { return desired ? desired->Is(state) : FALSE; }
        // Returns true if the desired state matches the given state
    BOOL IsRoot(char *state) { return root ? root->Is(state) : FALSE; }
        // Returns true if the desired state matches the given state
    // this is a hack               ---v
    BOOL IsDoing(ACTION action) { if (!this) return FALSE; else return doing ? doing->action == action : FALSE; }
        // Returns true if currently doing the given state
    BOOL IsDesired(ACTION action) { return desired ? desired->action == action : FALSE; }
        // Returns true if the desired state matches the given state
    BOOL IsRoot(ACTION action) { return root ? root->action == action : FALSE; }
        // Returns true if the desired state matches the given state
    virtual BOOL HasActionAni(char *name, char *from = NULL)
       { if (!*name) return FALSE; 
           else if (FindState(name) >= 0) return TRUE; 
           else return FindTransitionState(from?from:root->name, name) >= 0; }
        // Returns TRUE if the object supports the given action with either a state
        // which matches the action, or a transition to that action from the
        // 'from' state or the root state if 'from' is NULL.  If this function returns
        // TRUE, the action is garanteed to have an animation to play.
    const char* GetState() { return (const char*) doing->name; }
        // return the state name that the object is doing
    virtual void Notify(int notify, void *ptr);
        // Notify Action (check root,desired, and doing for deleted target obj)

  // Streaming functions
    virtual int ObjVersion() { return 1; }
        // Returns the object version for this object
    virtual void Load(RTInputStream is, int version, int objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector


  protected:
    virtual void UpdateAction(int bits = 0);
      // Called by pulse() to update the current action block
    virtual int TryCommand(PTActionBlock ab, int bits = 0);
      // Main command function - attempt to go to desired state
    virtual int ForceCommand(PTActionBlock ab, int bits = 0);
      // Called in some special cases to force a new state - be careful with this one
    virtual PTActionBlock GetRoot() { return root; }
      // Gets the current root action block
    virtual void SetRoot(PTActionBlock ab);
      // Set root to ab and and update pointers
    virtual PTActionBlock GetDoing() { return doing; }
      // Gets the current doing action block
    virtual void SetDoing(PTActionBlock ab);
      // Set doing to ab and update pointers
    virtual PTActionBlock GetDesired() { return desired; }
      // Gets the current desired action block
    virtual void SetDesired(PTActionBlock ab);
      // Set desired pointer and update pointers
    virtual BOOL IsFinalState() { return FALSE; }
      // Returns whether character is in their last days

    // Resolve functions - redefine in derived classes for new or different functionality
    virtual int ResolveAction(int bits = 0);
      // Calls various resolve functions (very object-specific)

    virtual char *DefaultRootState() { return "still"; }
      // Returns the default root state for this object - redefine as necessary

    // Pointers for the state changes
    PTActionBlock doing;        // Pointer to what they are currently doing
    PTActionBlock desired;      // Pointer to what they *want* to be doing
    PTActionBlock root;         // Root state (return here by default)
};

#endif