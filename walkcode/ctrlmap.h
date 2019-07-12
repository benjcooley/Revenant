// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   ctrlmap.h - Game Control Mapper                     *
// *************************************************************************

#ifndef _CTRLMAP_H
#define _CTRLMAP_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#define KEYSPERCODE 3

#define CKEY_KEY1DOWN 1
#define CKEY_KEY2DOWN 2
#define CKEY_KEY3DOWN 3

_STRUCTDEF(SControlKey)
struct SControlKey
{
    int keys[KEYSPERCODE];
    int flags;
};

#define CODESPERCOMMAND 3
#define ALLMODES 0xFFFFFFFF

_STRUCTDEF(SControlEntry)
struct SControlEntry
{
    char *name;                         // Nice name of keycode (with spaces, etc.)
    char *id;                           // Id for keycode (INI file key)
    DWORD mode;                         // Mode bits this entry is active in
    SControlKey codes[CODESPERCOMMAND]; // Keycodes which activate a command
    int downcmd, upcmd;                 // The command they activate
    DWORD cmdflag;                      // Command flag (for state commands)
    BOOL down;                          // Is command down
};
typedef TVirtualArray<SControlEntry, 16, 16> TControlArray;

_CLASSDEF(TControlMap)
class TControlMap
{
  private:
    BOOL initialized;
    TControlArray controls;
    DWORD cmdflagstate, cmdflagchanged;

  public:
    BOOL Initialize(int numcontrols, PSControlEntry controlarray);
      // Initialize the control mapper from a static list of controls
    void Close();
      // closes the control mapper
    int NumControls() { return controls.NumItems(); }
      // Returns number of controls
    void GetControlEntry(int index, PSControlEntry ce);
      // Returns the control entry for the given index
    void SetControlEntry(int index, PSControlEntry ce);
      // Returns the control entry for the given index
    int GetCommand(int keycode, BOOL down, DWORD modemask = ALLMODES);
      // Returns the game command given the 'keycode','down', and the 'modemask' 
    void GetCommandFlags(DWORD &state, DWORD &changed)
      { state = cmdflagstate; changed = cmdflagchanged; }
      // Returns the current command flags (for 'hold down' style controls such as cursor
      // keys, run mode keys, where the OFF state is all controls up.)
    BOOL CommandOn(int command, DWORD modemask = ALLMODES);
      // Returns TRUE if the command is currently active (down command and buttons are down,
      // or up command and buttons are up)
    int GetKeyCode(char *name);
      // Gets a keycode id given the ascii string name
    char *GetKeyName(int keycode);
      // Gets a string name given the keycode id
    char *MakeKeyString(int numkeys, int *keys, char *buf, int buflen);
      // Takes an array of keycodes and returns a keystring with the format "CTRL-SHIFT-A"
    int ParseKeyString(char *str, int numkeys, int *keys);
      // Takes a string of the format "CTRL-A"
    char *MakeCommandString(PSControlEntry ce, char *buf, int buflen);
      // Takes a command entry structure, and creates a string of the format "CTRL-A,SHIFT-F1"
    int ParseCommandString(char *str, PSControlEntry ce);
      // Takes a command string and parses it into keys for command structure
    void Clear();
      // Clears command settings
    void Load(char *section);
      // Loads settings from the INI file
    void Save(char *section);
      // Saves settings to the INI file
};

#endif