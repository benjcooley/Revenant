// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     script.h - Script functions                       *
// *************************************************************************

#ifndef _SCRIPT_H
#define _SCRIPT_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _PARSE_H
#include "parse.h"
#endif

#ifndef _COMMAND_H
#include "command.h"
#endif

#define TRIGGER_NONE      0
#define TRIGGER_ALWAYS    1     // Always running script
#define TRIGGER_TRIGGER   2     // Manual trigger (triggered by the 'trigger' command in script)
#define TRIGGER_DIALOG    3     // Dialog trigger (triggered when char clicked)
#define TRIGGER_PROXIMITY 4     // Triggered a character gets within a certain range 
#define TRIGGER_CUBE      5     // Trigged when character or object enters given cube
#define TRIGGER_ACTIVATE  6     // Triggered when object is activated
#define TRIGGER_USE       7     // Triggered when character uses something
#define TRIGGER_GIVE      8     // Triggered when character gives something
#define TRIGGER_GET       9     // Triggered when character gets something
#define TRIGGER_COMBAT    10    // Triggered when character goes into combat mode
#define TRIGGER_DEAD      11    // Triggered when character dies

// ****************
// * TScriptProto *
// ****************

// Prototype class for scripts.

#define MAXSCRIPTNAME 20
#define MAXTRIGGERS 32

_STRUCTDEF(SScriptTrigger)
struct SScriptTrigger
{
    int type;
    DWORD pos;
    char name[MAXSCRIPTNAME];
    S3DRect cube;
    int dist;
    int priority;
};

typedef TVirtualArray<SScriptTrigger, 0, 4> TTriggerArray;

_CLASSDEF(TScriptProto)
class TScriptProto
{
  public:
    TScriptProto();// { name = NULL; text = NULL; next = NULL; }
    TScriptProto(PTScriptProto pparent, void *powner, char *pfilename, char *pbuffer);
    ~TScriptProto();// { if (name) delete name; if (text) delete text; }
    BOOL ParseCriteria(TToken &t);
        // Parse criteria for the proto
    int ParseScript(TToken &t);
        // Parse out the next block of text as the script
    BOOL WriteScript(FILE *fp);
        // Write script proto to file
    void SetBuffer(char *buffer);
        // Copy buffer into script's text
    void GetBuffer(char *buffer, int buflen);
        // Umm...I guess it get's the buffer
    int Length(){return len;}
        // Returns the lenghth of the script in bytes
    char *Text(){return text;}
        // Returns a pointer to the text
    PTScriptProto ParentProto(){return parent;}
        // Returns a pointer to the parent prototype

    BOOL FitsCriteria(PTObjectInstance inst);
        // Check to see if given object instance will use this script

    int NumTriggers(){return numtriggers;}

    char *name;                             // Text for criteria
    char *text;                             // Text of script
//  PTScriptProto next;                     // Next in list
    PTScriptProto parent;                   // The parent in list
    TTriggerArray triggers;                 // Trigger array
    void *owner;                            // Pointer to an owner for script
    char *filename;
    int len;
    int numtriggers;                        // Number of triggers for this prototype
};

typedef TPointerArray<TScriptProto, 64, 64> TScriptProtoArray;

// ***********
// * TScript *
// ***********

// Script object which contains pseudo-code directing the attached object's actions.

#define SCRIPT_PAUSED       (1 << 16)       // indicates script is currently on hold

#define MAXDEPTH        10

#define COND_UNDEF      0xDEAF      // arbitrary, as long as it is not TRUE or FALSE

_STRUCTDEF(SScriptBlock)
struct SScriptBlock
{
    DWORD loopstart;                // Location to loop back to
    BOOL conditional;               // State of conditional for block
};

class TScript
{
  public:
    TScript();// { proto = NULL; ip = NULL; priority = 0; depth = 0; newtrigger = 0; lastpriority = 0;
                //block[depth].conditional = COND_UNDEF; block[depth].loopstart = NULL; }
        // Init script without data
    TScript(PTScriptProto prototype);
        // Init script from the given buffer
    ~TScript();
        // Destory the script without saving

    BOOL Load(char *filename);
        // Read the script from the given file
    BOOL Save(char *filename);
        // Outputs script to given file

    void SetText(char *buf);
        // Copy contents of buf as script's new text
    char *Text() { if(curproto) return curproto->text; else return NULL; }
        // Get a pointer to the script's text

    void Start(PTScriptProto proto = NULL, int pos = 0, int newpriority = 0);
        // Begin script execution at the given location and priority
    void StartTrigger(PTScriptProto proto, PSScriptTrigger st);
        // Begin triger 
    void Continue(PTObjectInstance context);
        // Continue script exectuion
    void Jump(PTObjectInstance context, char *label);
        // Jump to the label
    void Break();
        // Temporarily interrupt script execution
    void Resume();
        // Restart an interrupted script
    BOOL IsPaused() { return (priority & SCRIPT_PAUSED); }
        // Check execution status of script
    void End();
        // Terminate script execution
    BOOL Running() { return priority > 0; }
        // Returns TRUE if the script is already running
    void Trigger(int newtrig, char *triggerstr = NULL)
      { newtrigger = newtrig; if (triggerstr) strcpy(newtriggerstr, triggerstr); }
        // Manually triggers the given script handler
    int GetTrigger() { return trigger; }
        // Returns current trigger type executing
    int GetPriority() { return priority; }
        // Returns the priority of the script executing (also is id of specific script block)
    PTScriptProto GetScriptProto() { return proto; }
        // Returns the prototype for this script

    static void PauseAllScripts(){ pauseall = TRUE; }
        // Causes all scripts to pause
    static void ResumeAllScripts(){ pauseall = FALSE; }
        // Causes all scripts to resume playing

  private:
    BOOL Triggered(PSScriptTrigger st, int priority, PTObjectInstance context);
        // Returns TRUE if the current block was triggered

    static BOOL pauseall;                   // True if all scripts paused

    PTScriptProto proto;                    // Pointer to script prototype
    PTScriptProto topproto, curproto;       // Pointer's to the top and current prototype
    int newtrigger;                         // Next trigger type to execute
    int trigger;                            // Current trigger type executing 
    char newtriggerstr[MAXSCRIPTNAME];      // Name of what is triggering

    char *ip;                               // Next line to execute
    int priority;                           // Priority of current ip (is also the id of trigger block)
    int lastpriority;                       // Last trigger executed (is also the id of trigger block)

    SScriptBlock block[MAXDEPTH];           // For conditionals, loops etc
    int depth;                              // Number of blocks deep

    PSScriptTrigger curtrigger;             // The Current Trigger??
};

// **************
// * TGameState *
// **************

#define MAXGAMESTATES       4096

#define STATE_INVALID       -2000000000

class TGameState
{
  public:
    TGameState() { numstates = 0; }
    ~TGameState()
        { for (int i = 0; i < numstates; i++) if (statename[i]) delete statename[i]; }

    // Load and save gamestates to master definition file
    BOOL Load(char *filename);
    BOOL Save(char *filename);

    // Access functions
    int NumStates() { return numstates; }
    int State(int index)
        { if ((DWORD)index < (DWORD)numstates) return state[index]; return STATE_INVALID; }
    void SetState(int index, int newval)
        { if ((DWORD)index < (DWORD)numstates) state[index] = newval; }
    char *StateName(int index)
        { if ((DWORD)index < (DWORD)numstates) return statename[index]; return NULL; }

    // Search functions
    int FindStateIndex(char *name)
        { for (int i = 0; i < numstates; i++)
            if (!stricmp(name, statename[i])) return i; return -1; }

    int State(char *name) { return State(FindStateIndex(name)); }
    void SetState(char *name, int newval) { SetState(FindStateIndex(name), newval); }

  private:
    int numstates;
    int state[MAXGAMESTATES];
    char *statename[MAXGAMESTATES];
};

// ******************
// * TScriptManager *
// ******************

// Manages all the scripts in the game.

class TScriptManager
{
  public:
    TScriptManager() { scriptsdirty = FALSE; }

    BOOL Initialize();
    void Close();

    void ParseScripts(char *buffer, char *filename, void *owner);
        // Parse a buffer and chunk it into scripts

    BOOL Load(char *filename, void *owner = NULL);
        // Read the script from the given file
    BOOL Save(char *filename, void *owner = NULL);
        // Outputs script to given file
    void Clear(void *owner);
        // Clears all scripts for the given owner

    BOOL ReloadStates();
        // Reloads initial values for game states

    PTScript ObjectScript(PTObjectInstance inst);
        // Find script for the given instance

    int GameState(char *name);
        // Find the gamestate's value (if context, get context state val)
    void SetGameState(char *name, int newval) { gamestate.SetState(name, newval); }
        // Find the gamestate's value

    int FindLocalVal(char *name);
        // Find a value local to the current trigger
    void SetLocalVal(int index, int value);
    void SetLocalVal(char *name, int value) { SetLocalVal(FindLocalVal(name), value); }
        // Set a local value for use by the scripts
    int GetLocalVal(int index);
    int GetLocalVal(char *name) { return GetLocalVal(FindLocalVal(name)); }
        // Get a local value

    void SetScriptsDirty() { scriptsdirty = TRUE; }
        // For manual changes to the script

    PTScriptProto FindScriptProto(char *name);
        // Used to find the parent prototype

  private:
    TGameState gamestate;                   // Game states for scripts
    TScriptProtoArray scripts;              // Pointer to head of linked list

    BOOL scriptsdirty;                      // Whether scripts have been saved to disk
};

#endif
