// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   ctrlmap.h - Game Control Mapper                     *
// *************************************************************************

#include "revenant.h"
#include "ctrlmap.h"
#include "directinput.h"

_STRUCTDEF(SKeyName)
struct SKeyName
{
    int key;
    char *name;
};

static SKeyName keynames[] =
{
    {VK_LBUTTON, "LBUTTON"},
    {VK_RBUTTON, "RBUTTON"},
    {VK_CANCEL, "CANCEL"},
    {VK_MBUTTON, "MBUTTON"},
    {VK_BACK, "BS"},
    {VK_TAB, "TAB"},
    {VK_CLEAR, "CLEAR"},
    {VK_RETURN, "RETURN"},
    {VK_SHIFT, "SHIFT"},
    {VK_CONTROL, "CTRL"},
    {VK_MENU, "MENU"},
    {VK_PAUSE, "PAUSE"},
    {VK_CAPITAL, "CAPS"},
    {VK_ESCAPE, "ESCAPE"},
    {VK_SPACE, "SPACE"},
    {VK_PRIOR, "PGUP"},
    {VK_NEXT, "PGDN"},
    {VK_END, "END"},
    {VK_HOME, "HOME"},
    {VK_LEFT, "LEFT"},
    {VK_UP, "UP"},
    {VK_RIGHT, "RIGHT"},
    {VK_DOWN, "DOWN"},
    {VK_SELECT, "SELECT"},
    {VK_EXECUTE, "EXECUTE"},
    {VK_SNAPSHOT, "SNAPSHOT"},
    {VK_INSERT, "INS"},
    {VK_DELETE, "DEL"},
    {VK_HELP, "HELP"},
    {VK_LWIN, "LWIN"},
    {VK_RWIN, "RWIN"},
    {VK_APPS, "APPS"},
    {VK_NUMPAD0, "NUM0"},
    {VK_NUMPAD1, "NUM1"},
    {VK_NUMPAD2, "NUM2"},
    {VK_NUMPAD3, "NUM3"},
    {VK_NUMPAD4, "NUM4"},
    {VK_NUMPAD5, "NUM5"},
    {VK_NUMPAD6, "NUM6"},
    {VK_NUMPAD7, "NUM7"},
    {VK_NUMPAD8, "NUM8"},
    {VK_NUMPAD9, "NUM9"},
    {VK_MULTIPLY, "NUMMULT"},
    {VK_ADD, "NUMADD"},
    {VK_SEPARATOR, "SEPARATOR"},
    {VK_SUBTRACT, "NUMSUB"},
    {VK_DECIMAL, "NUMDEC"},
    {VK_DIVIDE, "NUMDIV"},
    {VK_F1, "F1"},
    {VK_F2, "F2"},
    {VK_F3, "F3"},
    {VK_F4, "F4"},
    {VK_F5, "F5"},
    {VK_F6, "F6"},
    {VK_F7, "F7"},
    {VK_F8, "F8"},
    {VK_F9, "F9"},
    {VK_F10, "F10"},
    {VK_F11, "F11"},
    {VK_F12, "F12"},
    {VK_F13, "F13"},
    {VK_F14, "F14"},
    {VK_F15, "F15"},
    {VK_F16, "F16"},
    {VK_F17, "F17"},
    {VK_F18, "F18"},
    {VK_F19, "F19"},
    {VK_F20, "F20"},
    {VK_F21, "F21"},
    {VK_F22, "F22"},
    {VK_F23, "F23"},
    {VK_F24, "F24"},
    {VK_NUMLOCK, "NUMLOCK"},
    {VK_SCROLL, "SCROLL"},
    {VK_ATTN, "ATTN"},
    {VK_CRSEL, "CRSEL"},
    {VK_EXSEL, "EXSEL"},
    {VK_EREOF, "EREOF"},
    {VK_PLAY, "PLAY"},
    {VK_ZOOM, "ZOOM"},
    {VK_NONAME, "NONAME"},
    {VK_JOYUPLEFT, "JOYUPLF"},
    {VK_JOYUP, "JOYUP"},
    {VK_JOYUPRIGHT, "JOYUPRT"},
    {VK_JOYLEFT, "JOYLF"},
    {VK_JOYRIGHT, "JOYRT"},
    {VK_JOYDOWNLEFT, "JOYDNLF"},
    {VK_JOYDOWN, "JOYDN"},
    {VK_JOYDOWNRIGHT, "JOYDNRT"},
    {VK_JOYBUTTON1, "JOY1"},
    {VK_JOYBUTTON2, "JOY2"},
    {VK_JOYBUTTON3, "JOY3"},
    {VK_JOYBUTTON4, "JOY4"},
    {VK_JOYBUTTON5, "JOY5"},
    {VK_JOYBUTTON6, "JOY6"},
    {VK_JOYBUTTON7, "JOY7"},
    {VK_JOYBUTTON8, "JOY8"},
    {VK_JOYBUTTON9, "JOY9"},
    {VK_JOYBUTTON10, "JOY10"},
    {VK_JOYBUTTON11, "JOY11"},
    {VK_JOYBUTTON12, "JOY12"},
    {VK_JOYBUTTON13, "JOY13"},
    {VK_JOYBUTTON14, "JOY14"},
    {VK_JOYBUTTON15, "JOY15"},
    {VK_JOYBUTTON16, "JOY16"},
    {VK_JOYBUTTON17, "JOY17"},
    {VK_JOYBUTTON18, "JOY18"},
    {VK_JOYBUTTON19, "JOY19"},
    {VK_JOYBUTTON20, "JOY20"},
    {VK_JOYBUTTON21, "JOY21"},
    {VK_JOYBUTTON22, "JOY22"},
    {VK_JOYBUTTON23, "JOY23"},
    {VK_JOYBUTTON24, "JOY24"},
    {VK_JOYUPLEFT | VK_DBLTAP, "DJOYUPLF"},
    {VK_JOYUP | VK_DBLTAP, "DJOYUP"},
    {VK_JOYUPRIGHT | VK_DBLTAP, "DJOYUPRT"},
    {VK_JOYLEFT | VK_DBLTAP, "DJOYLF"},
    {VK_JOYRIGHT | VK_DBLTAP, "DJOYRT"},
    {VK_JOYDOWNLEFT | VK_DBLTAP, "DJOYDNLF"},
    {VK_JOYDOWN | VK_DBLTAP, "DJOYDN"},
    {VK_JOYDOWNRIGHT | VK_DBLTAP, "DJOYDNRT"},
    {VK_JOYBUTTON1 | VK_DBLTAP, "DJOY1"},
    {VK_JOYBUTTON2 | VK_DBLTAP, "DJOY2"},
    {VK_JOYBUTTON3 | VK_DBLTAP, "DJOY3"},
    {VK_JOYBUTTON4 | VK_DBLTAP, "DJOY4"},
    {VK_JOYBUTTON5 | VK_DBLTAP, "DJOY5"},
    {VK_JOYBUTTON6 | VK_DBLTAP, "DJOY6"},
    {VK_JOYBUTTON7 | VK_DBLTAP, "DJOY7"},
    {VK_JOYBUTTON8 | VK_DBLTAP, "DJOY8"},
    {VK_JOYBUTTON9 | VK_DBLTAP, "DJOY9"},
    {VK_JOYBUTTON10 | VK_DBLTAP, "DJOY10"},
    {VK_JOYBUTTON11 | VK_DBLTAP, "DJOY11"},
    {VK_JOYBUTTON12 | VK_DBLTAP, "DJOY12"},
    {VK_JOYBUTTON13 | VK_DBLTAP, "DJOY13"},
    {VK_JOYBUTTON14 | VK_DBLTAP, "DJOY14"},
    {VK_JOYBUTTON15 | VK_DBLTAP, "DJOY15"},
    {VK_JOYBUTTON16 | VK_DBLTAP, "DJOY16"},
    {VK_JOYBUTTON17 | VK_DBLTAP, "DJOY17"},
    {VK_JOYBUTTON18 | VK_DBLTAP, "DJOY18"},
    {VK_JOYBUTTON19 | VK_DBLTAP, "DJOY19"},
    {VK_JOYBUTTON20 | VK_DBLTAP, "DJOY20"},
    {VK_JOYBUTTON21 | VK_DBLTAP, "DJOY21"},
    {VK_JOYBUTTON22 | VK_DBLTAP, "DJOY22"},
    {VK_JOYBUTTON23 | VK_DBLTAP, "DJOY23"},
    {VK_JOYBUTTON24 | VK_DBLTAP, "DJOY24"}
};
#define NUMKEYNAMES sizearray(keynames)

// Initialize the control mapper from a static list of controls
BOOL TControlMap::Initialize(int numcontrols, PSControlEntry controlarray)
{
    if (initialized)
        return TRUE;

    controls.Clear();
    for (int c = 0; c < numcontrols; c++)
    {
        controls.Add(controlarray[c]); // Note virtual array stores copy of structure only
    }

    cmdflagstate = cmdflagchanged = 0;

    initialized = TRUE;

    return TRUE;
}

// closes the control mapper
void TControlMap::Close()
{
    if (!initialized)
        return;

    controls.Clear();

    initialized = FALSE;
}

// Returns the control entry for the given index
void TControlMap::GetControlEntry(int index, PSControlEntry ce)
{
    if (!initialized)
        return;

    if ((DWORD)index >= (DWORD)controls.NumItems())
        return;

    memcpy(ce, &(controls[index]), sizeof(SControlEntry));
}

// Returns the control entry for the given index
void TControlMap::SetControlEntry(int index, PSControlEntry ce)
{
    if (!initialized)
        return;

    if ((DWORD)index >= (DWORD)controls.NumItems())
        return;

    memcpy(&(controls[index]), ce, sizeof(SControlEntry));
}

// Returns the game command given the 'keycode'
int TControlMap::GetCommand(int keycode, BOOL down, DWORD modeflags)
{
    int c;

    if (!initialized)
        return 0;

  // Clear command flags set by modes that are no longer active
    for (c = 0; c < controls.NumItems(); c++)
    {
        if (!controls.Used(c))
            continue;

        PSControlEntry ce = &(controls[c]);

      // Has a flag, but mode no longer active?
        if (!(ce->mode & modeflags) && ce->cmdflag)
        {
            cmdflagchanged |= ce->cmdflag;
            cmdflagstate &= ~(ce->cmdflag);
        }
    }

  // Get new commands
    for (c = 0; c < controls.NumItems(); c++)
    {
        if (!controls.Used(c))
            continue;

        PSControlEntry ce = &(controls[c]);

      // Do key codes one by one
        for (int code = 0; code < CODESPERCOMMAND; code++)
        {
            PSControlKey ck = &ce->codes[code];

            if (ck->keys[0] <= 0) // This code not used
                continue;

          // Start at last key, and go backwards
            int key = KEYSPERCODE - 1;
            int kdown = 1 << (KEYSPERCODE - 1);
            int kmask = 0xFFFFFFFF >> (32 - (KEYSPERCODE - 1));

          // Find last key
            while (key > 0 && ck->keys[key] <= 0)
            {
                key--;
                kdown = kdown >> 1;
                kmask = kmask >> 1;
            }
                
          // If we are last key, and other keys are down, return a command
            if ((ck->keys[key] == keycode) &&       // Key matches last key of command
                ((ce->down && !down) |              // Currently down and key is up OR...
                ((ck->flags & kmask) == kmask)))    // Prefix keys for command (if any) are down
            {
                if (down)
                    ck->flags |= kdown;
                else
                    ck->flags &= ~kdown;

                if (ce->mode & modeflags &&         // Command is part of this mode 
                    ce->down != down)               // Not a key repeat (already down)
                {
                    cmdflagchanged |= ce->cmdflag;

                    ce->down = down;                // Indicate that command is down or not
                    if (down)
                    {
                        cmdflagstate |= ce->cmdflag;
                        return ce->downcmd;
                    }
                    else
                    {
                        cmdflagstate &= ~(ce->cmdflag);
                        return ce->upcmd;
                    }
                }
            }
                    
          // Set or clear down flags for prefix keys
            key--;
            kdown = kdown >> 1;
            while (key >= 0)
            {
                if (ck->keys[key] == keycode)
                {
                    if (down)
                        ck->flags |= kdown;
                    else
                        ck->flags &= ~kdown;
                }
                key--;
                kdown = kdown >> 1;
            }

        } // End of code loop
    
    } // End of command entry loop

    return 0;
}

BOOL TControlMap::CommandOn(int command, DWORD modeflags)
{
    if (!initialized)
        return FALSE;

    for (int c = 0; c < controls.NumItems(); c++)
    {
        if (!controls.Used(c))
            continue;

        PSControlEntry ce = &(controls[c]);

        if ((ce->downcmd == command) &&         // Command matches
             ce->down &&                        // Key or keys are currently down
            (ce->mode & modeflags))             // We're in this mode
                return TRUE;
        else if (ce->upcmd == command)          // Matches up command and we weren't down
            return TRUE;
    }

    return FALSE;
}

// Clears command settings
void TControlMap::Clear()
{
    if (!initialized)
        return;

    for (int c = 0; c < controls.NumItems(); c++)
    {
        if (!controls.Used(c))
            continue;

        PSControlEntry ce = &(controls[c]);

        ce->down = FALSE;

      // Do key codes one by one
        for (int code = 0; code < CODESPERCOMMAND; code++)
        {
            PSControlKey ck = &ce->codes[code];

            for (int key = 0;  key < KEYSPERCODE; key++)
                ck->keys[key] = 0;

            ck->flags = 0;
        }
    }
}

// Gets a keycode id given the ascii string name
int TControlMap::GetKeyCode(char *name)
{
    if (name[1] == NULL && (name[0] >= 'A' && name[0] <= 'Z'))
        return name[0];
    if (name[1] == NULL && (name[0] >= 'a' && name[0] <= 'z'))
        return name[0] - 'a' + 'A';
    for (int c = 0; c < NUMKEYNAMES; c++)
    {
        if (!stricmp(name, keynames[c].name))
            return keynames[c].key;
    }

    return 0;
}

// Gets a string name given the keycode id
char *TControlMap::GetKeyName(int keycode)
{
    static char buf[2];
    if (keycode >= 'A' && keycode <= 'Z')
    {
        buf[0] = (char)keycode;
        buf[1] = NULL;
        return buf;
    }

    for (int c = 0; c < NUMKEYNAMES; c++)
    {
        if (keynames[c].key == keycode)
            return keynames[c].name;
    }

    return NULL;
}

// Takes an array of keycodes and returns a keystring with the format "CTRL-SHIFT-A"
char *TControlMap::MakeKeyString(int numkeys, int *keys, char *buf, int buflen)
{
    buf[0] = NULL;

    if (numkeys < 1 || keys[0] <= 0)
        return buf;

    for (int c = 0; c < numkeys; c++)
    {
        if (keys[c] <= 0)
            break;

        char *keyname = GetKeyName(keys[c]);
        if (!keyname)
            break;

        if (c > 0)
            strncatz(buf, "-", buflen);

        strncatz(buf, keyname, buflen);
    }

    return buf;
}

// Takes a string of the format "CTRL-A"
int TControlMap::ParseKeyString(char *str, int numkeys, int *keys)
{
    char buf[64];
    strncpyz(buf, str, 64);

    char *t = buf;
    char *p = strchr(buf, '-');
    if (p)
        *p = NULL;

    int key = 0;
    int num = 0;
    while (t && *t && key < numkeys)
    {
        keys[key] = GetKeyCode(t);

        if (p)
        {
            t = p + 1;
            p = strchr(t, '-');
            if (p)
                *p = NULL;
        }
        else
            t = NULL;

        key++;
        num++;
    }

    while (key < numkeys)
    {
        keys[key] = 0;
        key++;
    }

    return num;
}

// Takes a command entry structure, and creates a string of the format "CTRL-A,SHIFT-F1"
char *TControlMap::MakeCommandString(PSControlEntry ce, char *buf, int buflen)
{
    buf[0] = NULL;

    for (int code = 0; code < CODESPERCOMMAND; code++)
    {
        PSControlKey ck = &ce->codes[code];
        if (ck->keys[0] > 0)
        {
            if (code > 0)
                strncatz(buf, ",", buflen);
            char keybuf[64];
            MakeKeyString(KEYSPERCODE, ck->keys, keybuf, 64);
            strncatz(buf, keybuf, buflen);
        }
    }

    return buf;
}

// Takes a command string and parses it into keys for command structure
int TControlMap::ParseCommandString(char *str, PSControlEntry ce)
{
    char buf[80];
    strncpyz(buf, str, 80);

    char *t = buf;
    char *p = strchr(buf, ',');
    if (p)
        *p = NULL;

    int code = 0;
    int num = 0;
    while (t && *t && code < CODESPERCOMMAND)
    {
        ParseKeyString(t, KEYSPERCODE, ce->codes[code].keys);
        ce->codes[code].flags = 0;

        if (p)
        {
            t = p + 1;
            p = strchr(t, ',');
            if (p)
                *p = NULL;
        }
        else
            t = NULL;

        code++;
        num++;
    }

    while (code < CODESPERCOMMAND)
    {
        for (int key = 0; key < KEYSPERCODE; key++)
            ce->codes[code].keys[key] = 0;
        ce->codes[code].flags = 0;
        code++;
    }

    ce->down = FALSE;

    return num;
}

// Loads settings from the INI file
void TControlMap::Load(char *section)
{
    INISetSection("Controls");

    for (int c = 0; c < controls.NumItems(); c++)
    {
        if (!controls.Used(c))
            continue;
        
        PSControlEntry ce = &(controls[c]);

        char defbuf[64];
        MakeCommandString(ce, defbuf, 64);

        char buf[64];
        INIGetStr(ce->id, defbuf, buf, 64);

        ParseCommandString(buf, ce);
    }
}

// Saves settings to the INI file
void TControlMap::Save(char *section)
{
    INISetSection("Controls");

    for (int c = 0; c < controls.NumItems(); c++)
    {
        if (!controls.Used(c))
            continue;
        
        PSControlEntry ce = &(controls[c]);

        char buf[64];
        MakeCommandString(ce, buf, 64);

        INISetStr(ce->id, buf);
    }
}