// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  RevMain.cpp - System main module                     *
// *************************************************************************

#include <windows.h>
#include <io.h>
#include <stdarg.h>

// For multimonitor support
#define COMPILE_MULTIMON_STUBS
#include "multimon.h"

#include <direct.h>
#include <dos.h>
#include <dplay.h>
#include <fcntl.h>
#include <io.h>
#include <math.h>
#include <mmsystem.h>
#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "revenant.h"
#include "3dscene.h"
#include "bitmap.h"
#include "display.h"
#include "directinput.h"
#include "directdraw.h"
#include "graphics.h"
#include "playscreen.h"
#include "mappane.h"
#include "automap.h"
#include "inventory.h"
#include "mainwnd.h"
#include "savegame.h"
#include "screen.h"
#include "font.h"
#include "chunkcache.h"
#include "timer.h"
#include "equip.h"
#include "parse.h"
#include "player.h"
#include "multictrl.h"
#include "spell.h"
#include "spellpane.h"
#include "statpane.h"
#include "script.h"
#include "textbar.h"
#include "statusbar.h"
#include "sound.h"
#include "editor.h"
#include "dls.h"
#include "ctrlmap.h"
#include "videocap.h"
#include "area.h"
#include "rules.h"
#include "dialog.h"

// Global Variables
HINSTANCE hInstance;
HANDLE PauseMutex;
MEMORYSTATUS StartMemory;

// Game directories
char RunPath[MAX_PATH];
char SavePath[MAX_PATH];

// Define the Editor paths
char ClassDefPath[MAX_PATH];        // Where to load / save Class.Def
char ExileRCPath[MAX_PATH];         // Where to run ExileRC from & where
                                    // the graphics for the resources are
char ResourcePath[MAX_PATH];        // Where to read / write the resources
char BaseMapPath[MAX_PATH];         // Where the untouched version of the game map is stored
char CurMapPath[MAX_PATH];          // Where the current map is stored

// Current language
char Language[NAMELEN];             // Where the current map is stored

// Global Structure Defines
PTScreen        CurrentScreen;      // Currently displayed screen object
PTScreen        NextScreen;         // Next Screen to be display object
TDisplay        display;            // Display object
PTDisplay       Display = &display; // Display pointer
T3DScene        Scene3D;            // Display pointer
TPlayScreen     PlayScreen;         // PlayScreen Object
TMapPane        MapPane;            // Map pane for PlayScreen
TInventory      Inventory;          // Inventory of objects
THealthBar      HealthBar;          // Character's health
TStaminaBar     StaminaBar;         // Character's fatigue
TTextBar        TextBar;            // Info line for player
TMultiCtrlPane  MultiCtrl;          // Multipane control panel (buttons)
TEquipPane      EquipPane;          // Equipment pane
TAutoMap        AutoMap;            // Automapper
TSpellPane      SpellPane;          // Talisman stuff
TQuickSpellPane QuickSpells;        // Quick spell buttons
TStatPane       StatPane;           // Display of char's abilities
TScriptManager  ScriptManager;      // Manages all scripts in the game
TMainWindow     MainWindow;         // Windows Object
TSaveGame       SaveGame;           // SaveGame Object
TChunkCache     ChunkCache;         // Tile Cache
TTimer          Timer;              // Timer Object
TVideoCapture   VideoCapture;       // Video capture object
PTFont          SystemFont;         // Basic utility font for game
PTFont          DialogFont;         // Dialog font
PTFont          DialogFontShadow;   // Dialog font shadow
PTFont          SmallFont;          // Small game font
PTFont          GameFont;           // Medium game font
PTFont          GoldFont;           // Medium gold font
PTFont          MetalFont;          // Small gold/metal font
PTFont          MenuFont;           // Menu font
PTPlayer        Player;             // Main player for the game
PTMulti         GameData;           // Multiresource for in game data
TSoundPlayer    SoundPlayer;        // Sound effects player
TControlMap     ControlMap;         // Contains the key/joystick mappings for game control
TAreaManager    AreaManager;        // Manages the game area system
TPlayerManager  PlayerManager;      // Manages the game player list
TRules  Rules;      // Manages global rules data (classes, chars, stats for attacks, etc.)
CRITICAL_SECTION CriticalSection;   // Controls enter critical section functions;
TSpellList      SpellList;          // a list of spells in the game
TDialogList     DialogList;         // List of dialog and other game messages for current language

// Game speed variable
int GameSpeed = 3;          // Value 1-5 which determines how fast the game is running
                            // Used to switch on/off processor intensive effects
// Violence Level
int ViolenceLevel = 5;      // Value 0-5 which determines how much blood and gore, etc. to use
                            // 0 = none

// Total size of preload area (in sectors) if PreloadSectors is on
int PreloadSectorSize = -1;

// Total size of chunk cache to allocate (in MB's)
int ChunkCacheSize = -1;    // -1 is calculate based on available physical memory

// Clear before drawing update buffers?
BOOL ClearBeforeDraw = TRUE;        // Clear before drawing update buffers?
BOOL NoFrameSkip     = FALSE;       // Disables frame skipping on slowdown
BOOL PauseFrameSkip  = FALSE;       // Set during disk loading to avoid skipping afterwards
BOOL NoScrollZBuffer = TRUE;        // Disables updating to video memory ZBuffer
BOOL WaitVertRetrace = TRUE;        // Wait vert retrace (not used)
BOOL ShowDrawing     = FALSE;       // Flips front and back buffer so you can see drawing
BOOL NoNormals       = TRUE;        // Changes to no normals lighting mode
BOOL SmoothScroll    = TRUE;        // Do smooth scrolling
BOOL ScrollLock      = TRUE;        // Lock scrolling to the character
BOOL DrawRealtimeShadows = TRUE;    // Draw alpha shadows for 3D characters
BOOL Show3D          = TRUE;        // Turn on the 3D system
BOOL Interpolate     = TRUE;        // Causes 3D animations to interpolate when state changes
BOOL DoPageFlip      = TRUE;        // Allows the PageFlip function to flip pages
BOOL PauseWhenNotActive = TRUE;     // Causes game to pause when not active
BOOL NoWideBuffers   = FALSE;       // Doesn't allow video buffers with stides wider than their widths
BOOL UseDirect3D2    = TRUE;        // True if you want to be able to use drawprimive stuff
BOOL UseSoftware3D   = FALSE;       // Use software 3D (False assumes we want hardware if available)
BOOL SoftRampMode    = TRUE;        // Use RAMP mode for software 3D (default)
BOOL SoftRGBMode     = FALSE;       // Use RGB mode for software 3D
BOOL UseBlue         = TRUE;        // Use BLUE software renderer instead of DirectX
BOOL UseDrawPrimitive = TRUE;       // If FALSE, 3D system does everything with Execute Buffers (always FALSE if UseDirect3D2 is FALSE)
BOOL CacheExBufs     = TRUE;        // Attempt to cache execute buffers for chars and other animation
BOOL PreloadSectors  = TRUE;        // If true, will load a large sector cache for faster scrolling
BOOL NoPulseObjs     = FALSE;       // Prevents the game objects from being pulsed
BOOL NoAnimateObjs   = FALSE;       // Prevents the game objects from being animated
BOOL NoVidZBufLock   = FALSE;       // Prevents simultaneous locking of video and zbuffer (for voodoo)
BOOL NoBlitZBuffer   = FALSE;       // Card is incapable of blitting to the display ZBuffer (voodoo)
BOOL UseClearZBuffer = FALSE;       // Use a clear z buffer for z restores (i.e. Viewport->Clear())
BOOL IsVooDoo        = FALSE;       // True if using a voodoo card
BOOL IsMMX           = FALSE;       // Has MMX extensions
BOOL NoAI            = FALSE;       // Turns off monster AI
BOOL AutoBeginCombat = TRUE;        // Automatically begins combat if enemy in range and facing him
BOOL PlaySpeech      = TRUE;        // Play speech wave files
BOOL ShowDialog      = FALSE;       // Show dialog lines (always shows if no speech file found)

// Multi-Monitor Variables
int MonitorNum = 0;         // Monitor game will run on (0=default (primary), 1=monitor 1, 2=monitor 2, etc.)
int MonitorX = 0;           // Relative position of monitor in desktop coordinates  
int MonitorY = 0;       
int MonitorW = 640;         // Relative position of monitor in desktop coordinates  
int MonitorH = 480;     
HMONITOR Monitor = NULL;    // Windows monitor handle
MONITORINFOEX MonitorInfo;  // Windows monitor info structure

// Game flags
BOOL Windowed = FALSE;      // Do we run the game in a window in NORMAL mode (instead of EXLUSIVE)
BOOL SingleBuffer = FALSE;  // True if single buffering (TRUE if Windowed=TRUE, or UsingHardware=FALSE)
BOOL Borderless = FALSE;    // When windowed, cut out the border (menu, other windows garabage)?
BOOL Debug = FALSE;         // Should program be run in debug compatible mode?
BOOL Ignore3D = FALSE;      // Whether to disallow 3D imagery (to make it run on systmes without Direct3D)
BOOL Editor = FALSE;        // TRUE when in editor mode
BOOL StartInEditor = FALSE; // Whether to start the program in editor mode
BOOL NoQuickLoad = FALSE;   // TRUE if program should not try to use IMAGERY.DAT file
BOOL Force15Bit = FALSE;    // Forces video mode to assume 15 bit
BOOL Force16Bit = FALSE;    // Forces video mode to assume 16 bit
BOOL FullScreen = FALSE;    // Full screen mode
BOOL ShowZBuffer      = FALSE;  // Show Z buffer for debugging
BOOL ShowNormalBuffer = FALSE;  // Show normal buffer for debugging
BOOL UnlockImmediately = TRUE; // Unlock DD Surf after lock (for debugging, allows stepping through draw code)

// Video capture globals (set in command line parse, used after display is initialized)
static BOOL dovideocap = FALSE;
static int videocapmegs, videocapfps;

// Driver name string (allows user to select driver by just typing in name of card)
char DXDriverMatchStr[FILENAMELEN]; // Will use first DX driver who's description has the given string in it
                                    // i.e. use if string is "permidia" and driver desc is "Glint Permidia 2 3D"

// Tick Sync variable
BOOL TickOccured = FALSE;

// Disable Screen calls to timer functions
BOOL DisableTimer = FALSE;

// Last Frame Milliseconds (Do we show it?)
int LastFrameTicks;
BOOL ShowFramesPerSecond = FALSE;

// Character movement mode
BOOL GridSnap = FALSE;

// Music and sound effects
BOOL SoundSystemOn = TRUE;

// Is Control, Shift, or Alt down
BOOL CtrlDown, ShiftDown, AltDown;

// Is System Closing Down?
BOOL Closing = FALSE;

// For setting up 3D object bounding rects in the editor
BOOL UpdatingBoundingRect = FALSE;

// Is joystick available?
BOOL HasJoyStick = FALSE;
JOYINFO JoyInfo;

// Memory used by program
DWORD ImageryMemUsage = 0;

// Frame ticks for doubletap checking
int DoubleTapTicks = 6;

// Global Map Pane Sizes (Set to initial frame sizes to insure
int MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT;
int SCROLLBUFWIDTH, SCROLLBUFHEIGHT;

// ************************************************************************
// *                          Support Functions                           *
// ************************************************************************

BOOL ThreadsPaused = FALSE;

void PauseThreads()
{
    ThreadsPaused = TRUE;
    TObjectImagery::PauseLoader();
    PAUSEUPDATE;
}

void ResumeThreads()
{
    ThreadsPaused = FALSE;
    TObjectImagery::ResumeLoader();
    MapPane.ResumeUpdate();
}

// *************** Simple Support Functions *****************

char *itos(int val, char *buf, int buflen)
{
    if (!buf)
        return buf;
    if (buflen < 2)
    {
        if (buf > 0)
            buf[0] = NULL;
        return buf;
    }
    if (buflen >= 12) // Twelve is big enough for anything
    {
        _itoa(val, buf, 10);
        return buf;
    }
    char b[12];
    _itoa(val, b, 10);
    strncpy(buf, b, buflen - 1);
    buf[buflen - 1] = NULL;
    return buf;
}

// Copies a file, or group of files using wildcards
int copyfiles(char *from, char *to, BOOL overwrite)
{
    if (!from || !to)
        return 0;

    struct _finddata_t data;

    char fdrive[_MAX_DRIVE];
    char fdir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char fext[_MAX_EXT];

    char fromdir[MAX_PATH];
    char todir[MAX_PATH];
    char source[MAX_PATH];
    char dest[MAX_PATH];

    _splitpath(from, fdrive, fdir, fname, fext);
    sprintf(fromdir, "%s%s", fdrive, fdir);
    if (!fromdir[0])
        return 0;
    if (fromdir[strlen(fromdir) - 1] != '\\')
        strncatz(fromdir, "\\", MAX_PATH);

    _splitpath(to, fdrive, fdir, fname, fext);
    sprintf(todir, "%s%s", fdrive, fdir);
    if (!todir[0])
        return 0;
    if (todir[strlen(todir) - 1] != '\\')
        strncatz(todir, "\\", MAX_PATH);

    int copied = 0;

    long found, handle;
    found = handle = _findfirst(from, &data);
    while (found != -1)
    {
        sprintf(source, "%s%s", fromdir, data.name);
        sprintf(dest, "%s%s", todir, data.name);

        if (CopyFile(source, dest, !overwrite))
            copied++;

        found = _findnext(handle, &data);
    }

    return copied;
}

// Deletes a file, or group of files using wildcards
int deletefiles(char *name)
{
    struct _finddata_t data;

    char dir[MAX_PATH], file[MAX_PATH];

    char fdrive[_MAX_DRIVE];
    char fdir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char fext[_MAX_EXT];

    _splitpath(name, fdrive, fdir, fname, fext);
    sprintf(dir, "%s%s", fdrive, fdir);

    int deleted = 0;

    long found = _findfirst(name, &data);
    while (found != -1)
    {
        strncpyz(file, dir, MAX_PATH);
        strncatz(file, data.name, MAX_PATH);
        if (DeleteFile(file))
            deleted++;

        found = _findfirst(name, &data);
    }

    return deleted;
}

// Returns system ticks in milliseconds since system was turned on
DWORD tickcount()
{
    return GetTickCount();
}

// *************** Error Functions *****************

void ThreadError(char *error, char *extra)
{
    char buf[101];
    sprintf(buf, error, extra);

    if (extra)
        _RPT1(_CRT_ERROR, error, extra);
    else
        _RPT0(_CRT_ERROR, error);

//  MSG Message;
//  while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
//  {
//      TranslateMessage(&Message);
//      DispatchMessage(&Message);
//  }

//  MessageBox(NULL, buf, "FATAL ERROR", MB_ICONSTOP | MB_OK);
}

void FatalError(char *error, char *extra)
{
    static BOOL alreadyin;

    if (alreadyin)
        exit(1);
    alreadyin = TRUE;

    char buf[101];
    sprintf(buf, error, extra);

  // Write error string to display
    Status(buf);
    Status("Press any key to exit");

  // Prevent passing of windows message (input etc) to current screen
    CurrentScreen = NULL;

  // Stop timer stuff
    Timer.Close();

  // Close the display
    Display->Close();

  // Close Direct Draw
    CloseDirectDraw();

  // Close the window!  
    MainWindow.Close();

    MSG Message;
    while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    if(!DirectDraw)
    {
        if (extra)
            _RPT1(_CRT_ERROR, error, extra);
        else
            _RPT0(_CRT_ERROR, error);
    }

//  ShowWindow(MainWindow.Hwnd(), 0);
//  MessageBox(NULL, buf, "FATAL ERROR", MB_ICONSTOP | MB_OK);

    exit(1);
}

void Error(char *error, char *extra)
{
    FatalError(error, extra);
/*
    char buf[101];
    sprintf(buf, error, extra);

  // Process windows messages (for Jong)
    MSG Message;
    while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    MessageBox(NULL, buf, "FATAL ERROR", MB_ICONSTOP | MB_OK);

    exit(1);
*/
}

void WaitSingleErr(HANDLE obj)
{
    if (WaitForSingleObject(obj, INFINITE /*10000*/) == WAIT_TIMEOUT)
    {
        FatalError("Wait Failed");
    }
}

void WaitMultipleErr(DWORD objs, CONST HANDLE *obj, BOOL all)
{
    if (WaitForMultipleObjects(objs, obj, all, INFINITE /*10000*/) == WAIT_TIMEOUT)
    {
        FatalError("Wait Failed");
    }
}

// *************** Display Status Functions *****************

void Status(char *fmt,...)
{
    static char buf[1024]; // Temporary buf for output
    static y = 10;

    if (!DirectDraw || !SystemFont || !Display->GetSurface())
        return;

    va_list marker;
    va_start(marker, fmt);
    vsprintf(buf, fmt, marker);

    char *s = buf;
    char *p = buf;
    while (1)
    {
        if (*p == '\n' || *p == NULL)
        {
            char save = *p;
            *p = NULL;
            if (strlen(s) > 0)
            {
                Display->WriteText(s, 10, y, 1, SystemFont, NULL, DM_TRANSPARENT | DM_ALIAS);
                Display->PutToScreen(0, 0, Display->Width(), Display->Height());
                y += 10;
            }
            if (!save)
                break;
            s = p + 1;
        }
        p++;
    }
}

// *************** Critical Section Functions *****************

void BEGIN_CRITICAL()
{
    EnterCriticalSection(&CriticalSection);
}

void END_CRITICAL()
{
    LeaveCriticalSection(&CriticalSection);
}

// ****** Exit the Game - Why would anyone want to do that? ******

void ExitGame()
{
    PostQuitMessage(0);
}

// *************** Settings Functions *****************

char INIPath[RUNPATHLEN];
char INISection[64];

void INISetSection(char *newsection)
{
    strncpyz(INISection, newsection, 64);
}

void INISetPath(char *runpath)
{
    char *ininame = "Revenant.ini";

    strcpy(INIPath, SavePath);
    strcat(INIPath, ininame);

  // Make sure INI file is in writable (SavePath) directory
    if (stricmp(RunPath, SavePath) != 0) // Run/Save path are different
    {
        FILE *fp = fopen(INIPath, "r");
        if (!fp)    // INI file not in SavePath
        {
            char from[MAX_PATH];
            strcpy(from, RunPath);
            strcat(from, ininame);
            copyfiles(from, INIPath);
        }
    }
}

int INIGetInt(char *key, int def, char *format)
{
    int i = GetPrivateProfileInt(INISection, key, def, INIPath);

    INISetInt(key, i, format);

    return i;
}

void INISetInt(char *key, int i, char *format)
{
    if (!format)
        format = "%d";

    char buf[20];
    sprintf(buf, format, i);

    WritePrivateProfileString(INISection, key, buf, INIPath);
}

char *INIGetText(char *key, char *def, char *buf, int buflen)
{
    static char s[128];
    if (!buf)
    {
        buf = s;
        buflen = 128;
    }

    if (!def)
        def = "";

  // Quote the string
    char qdef[128];
    strncpyz(qdef, "\"", 128);
    strncatz(qdef, def, 128);
    strncatz(qdef, "\"", 128);

    GetPrivateProfileString(INISection, key, qdef, buf, buflen, INIPath);

    if (buf[0] == '\"')
    {
        int l = strlen(buf);
        memmove(buf, buf + 1, l - 2);
        buf[l - 2] = NULL;
    }

    INISetText(key, buf);

    return buf;
}

void INISetText(char *key, char *str)
{
  // Quote the string
    char qstr[128];
    strncpyz(qstr, "\"", 128);
    strncatz(qstr, str, 128);
    strncatz(qstr, "\"", 128);

    WritePrivateProfileString(INISection, key, qstr, INIPath);
}

char *INIGetStr(char *key, char *def, char *buf, int buflen)
{
    static char s[128];
    if (!buf)
    {
        buf = s;
        buflen = 128;
    }

    if (!def)
        def = "";

    GetPrivateProfileString(INISection, key, def, buf, buflen, INIPath);

    INISetStr(key, buf);

    return buf;
}

void INISetStr(char *key, char *str)
{
    WritePrivateProfileString(INISection, key, str, INIPath);
}

int INIGetArray(char *key, int size, int *ary, int defsize, int *defary, char *format)
{
    char buf[128];

    if (ary != defary)
        memset(ary, 0, sizeof(int) * size);

    GetPrivateProfileString(INISection, key, "", buf, 128, INIPath);

    int newsize = 0;
    if (!buf[0])
    {
        if (defary)
        {
            while (newsize < defsize && newsize < size)
            {
                ary[newsize] = defary[newsize];
                newsize++;
            }
        }
    }
    else
    {
        char *tok = NULL;
        do
        {
            tok = strtok((newsize < 1) ? buf : NULL, ",");
            if (tok)
            {
                ary[newsize] = atol(tok);
                newsize++;
            }
        } while (tok && newsize < size);
    }   

    INISetArray(key, newsize, ary, format);

    return newsize;
}

void INISetArray(char *key, int size, int ary[], char *format)
{
    char buf[128];

    if (!format)
        format = "%d";

    buf[0] = NULL;
    for (int c = 0; c < size; c++)
    {
        if (c >= 1)
            strcat(buf, ",");
        int len = strlen(buf);
        sprintf(buf + len, format, ary[c]);
    }

    WritePrivateProfileString(INISection, key, buf, INIPath);
}

BOOL INIGetBool(char *key, BOOL def, char *yes, char *no)
{
    char buf[30];
    char getyes[30], getno[30];

    if (!yes)
        strcpy(getyes,"yes on true 1");
    else
    {
        strcpy(getyes, yes);
        strlwr(getyes);
    }

    if (!no)
        strcpy(getno, "no off false 0");
    else
    {
        strcpy(getno, no);
        strlwr(getno);
    }

    GetPrivateProfileString(INISection, key, "", buf, 30, INIPath);
    strlwr(buf);

    BOOL b; 
    if (!strstr(yes, buf))
        b = TRUE;
    else if (!strstr(no, buf))
        b = FALSE;
    else
        b = def;

    INISetBool(key, b, yes, no);

    return b;
}

void INISetBool(char *key, BOOL on, char *yes, char *no)
{
    if (!yes)
        yes = "1";
    if (!no)
        no = "0";

    WritePrivateProfileString(INISection, key, on ? yes : no, INIPath);
}

BOOL INIGetYesNo(char *key, BOOL def)
{
    return INIGetBool(key, def, "Yes", "No");
}

void INISetYesNo(char *key, BOOL on)
{
    INISetBool(key, on, "Yes", "No");
}

BOOL INIGetTrueFalse(char *key, BOOL def)
{
    return INIGetBool(key, def, "True", "False");
}

void INISetTrueFalse(char *key, BOOL on)
{
    INISetBool(key, on, "True", "False");
}

BOOL INIGetOnOff(char *key, BOOL def)
{
    return INIGetBool(key, def, "On", "Off");
}

void INISetOnOff(char *key, BOOL on)
{
    INISetBool(key, on, "On", "Off");
}

// Grab the ParseAnything function from PARSE.CPP
BOOL ParseAnything(BOOL stack, TToken &t, char *format, va_list ap);

BOOL INIParse(char *key, char *def, char *format, ...)
{
    char buf[128];

    INIGetStr(key, def, buf, 128);

    va_list ap;
    va_start(ap, format);

    TStringParseStream s(buf, strlen(buf));
    TToken t(s);
    t.Get();

    BOOL retval = ParseAnything(TRUE, t, format, ap);

    va_end(ap);

    return retval;
}

void INIPrint(char *key, char *format, ...)
{
    char buf[128];

    va_list marker;
    va_start(marker, format);

    vsprintf(buf, format, marker);

    va_end(marker);

    INISetStr(key, buf);
}

// *************** Allocation Functions *****************

int TotalAllocated, MaxAllocated;

#ifdef _DEBUG

#undef malloc
#undef free
#undef realloc

void *xmalloc(int size, char *file, int line)
{
    TotalAllocated += size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
    void *p = _malloc_dbg(size, _NORMAL_BLOCK, file, line);
    return p;
}

void *xrealloc(void *p, int size, char *file, int line)
{
    TotalAllocated = TotalAllocated - _msize(p) + size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
    p = _realloc_dbg(p, size, _NORMAL_BLOCK, file, line);
    return p;
}

void xfree(void *p)
{
    if (p != NULL)
        TotalAllocated -= _msize(p);
    _free_dbg(p, _NORMAL_BLOCK);
}

void* __cdecl operator new(size_t size)
{
    void *p = xmalloc(size,"new",0);
    if (!p)
        FatalError("OUT OF MEMORY - Increase your virtual swap file size", NULL);
    else
        memset(p, 0, size);

    return p;
}

void __cdecl operator delete(void *pointer)
{
    xfree(pointer);
}

#else

#undef malloc
#undef free
#undef realloc

void *xmalloc(int size)
{
    TotalAllocated += size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
    void *p = malloc(size);
    memset(p, 0, size);
    return p;
}

void *xrealloc(void *p, int size)
{
    TotalAllocated = TotalAllocated - _msize(p) + size;
    if (TotalAllocated > MaxAllocated)
        MaxAllocated = TotalAllocated;
    p = realloc(p, size);
    return p;
}

void xfree(void *p)
{
    if (p != NULL)
        TotalAllocated -= _msize(p);
    free(p);
}

void* __cdecl operator new(size_t size)
{
    void *p = xmalloc(size);
    if (!p)
        FatalError("OUT OF MEMORY - Increase your virtual swap file size", NULL);
    return p;
}

void __cdecl operator delete(void *pointer)
{
    xfree(pointer);
}

#endif

char *makepath(char *name, char *buf, int buflen)
{
  // If root path expicitly given, (i.e. "c:\", or "\" or "\\" or "..") use it
    if (name[0] == '\\' || name[1] == ':' || (name[0] == '.' && name[1] == '.'))
    {
        strncpyz(buf, name, buflen);
        return buf;
    }

  // If root path is ".", substitute SavePath or RunPath
    if (name[0] == '.')
    {
        name++;
        while (name[0] == '\\')
            name++;
    }

  // Always try SavePath first (SavePath will always be writable directory on hard drive)
    strncpyz(buf, SavePath, buflen);
    strncatz(buf, name, buflen);

    return buf;
}

FILE *popen(char *name, char *flags)
{
    char fn[MAX_PATH];
    
  // If root path expicitly given, (i.e. "c:\", or "\" or "\\" or "..") use it
    if (name[0] == '\\' || name[1] == ':' || (name[0] == '.' && name[1] == '.'))
        return fopen(name, flags);

  // If root path is ".", substitute SavePath or RunPath
    if (name[0] == '.')
    {
        name++;
        while (name[0] == '\\')
            name++;
    }

  // Always try SavePath first (SavePath will always be writable directory on hard drive)
    strncpyz(fn, SavePath, MAX_PATH);
    strncatz(fn, name, MAX_PATH);

  // If SavePath fails, and we have a different run path, try it 
  // (RunPath can be read only CD-ROM)
    FILE *fp = fopen(fn, flags);
    if (!fp && stricmp(SavePath, RunPath) != 0)
    {
        strncpyz(fn, RunPath, MAX_PATH);
        strncatz(fn, name, MAX_PATH);
        fp = fopen(fn, flags);
    }

    return fp;
}

// Random number function
int random(int min, int max)
{
    if (min == max)
        return min;

    if (max < min)
    {
        int t = min;
        min = max;
        max = t;
    }

    int r = rand() % (max - min + 1);
    r += min;
    return r;
}

// ********* Comma Delimited String Lists **********

// Comma delimited list functions (useful for strings in "abcd,defg,hijk" format)
// If dst is NULL, retuns result pointer from static internal buffer

#define LISTBUFLEN 128
static char listbuf[LISTBUFLEN];

// Get num string from comma list
char *listget(char *src, int num, char *dst, int len)
{
    if (!dst)
    {
        dst = listbuf;
        len = LISTBUFLEN;
    }

    if (len <= 0)
        return dst;

    dst[0] = NULL;

    char *p = src;
    int comma = 0;
    while (*p && comma < num)
    {
        if (*p == ',')
            comma++;
        p++;
    }

    char *d = dst;
    int l = 0;
    while (*p && *p != ',' && l < len - 1)
    {
        *d = *p;
        d++;
        p++;
        l++;
    }
    *d = NULL;

    return dst;
}

// Get total number of strings in comma list
int listnum(char *src)
{
    char *p = src;
    int comma = 0;
    while (*p)
    {
        if (*p == ',')
            comma++;
        p++;
    }

    return comma + 1;
}

// Get random string from comma list
char *listrnd(char *src, char *dst, int len)
{
    return listget(src, random(0, listnum(src) - 1), dst, len); 
}

// Returns TRUE if string is in comma list (case insensitive)
BOOL listin(char *src, char *in)
{
    char *s = src;
    char *i = in;

    while (*s)
    {
        if (*s == ',')
        {
            if (*i == NULL)
                break;
            i = in;
            s++;
        }
        else if (!*i || toupper(*i) != toupper(*s))
        {
            while (*s && *s != ',')
                s++;
            if (*s == ',')
                s++;
            i = in;
        }
        else
        {
            s++;
            i++;
        }
    }

    return *i == NULL;
}

// ********* Free Memory Functions **********

DWORD MemUsed()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwMemoryLoad;
}

DWORD FreeMem()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwAvailVirtual;
}

DWORD TotalMem()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwTotalVirtual;
}

DWORD FreePhys()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwAvailPhys;
}

DWORD TotalPhys()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwTotalPhys;
}

DWORD FreePage()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwAvailPageFile;
}

DWORD TotalPage()
{
    MEMORYSTATUS ms;
    ms.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&ms);
    return ms.dwTotalPageFile;
}

// ************************************************************************
// *                     Initialization Functions                         *
// ************************************************************************

#define cpuid __asm _emit 0x0f __asm _emit 0xa2

#define MMXBIT (1 << 23)

BOOL HasMMX()
{
    DWORD version;
    DWORD features;

    __asm
    {
        mov eax, 1
        cpuid
        mov version, eax
        mov features, edx
    }

    return (features & MMXBIT) != 0;
}

void GetProgramPaths(char *lpCmdLine, char *RunPath, char *SavePath)
{

  char *p;
  // Get The Run Path
    GetModuleFileName(hInstance, RunPath, RUNPATHLEN - 1);
    p = RunPath + strlen(RunPath) - 1;
    while (p != RunPath && *p != '\\')
        *p-- = NULL;

  // Get save path
    strcpy(SavePath, RunPath);
    char *tfname = "test.fil";
    char fname[60];
    strcpy(fname, SavePath);
    strcat(fname, tfname);
    FILE *f = fopen(fname, "wb");
    if (f)
    {
        fclose(f);
        unlink(fname);
    }
    else
    {
        strcpy(SavePath, "c:\\Revenant");
        for (int qq = 0; qq < 5; qq++)
        {
            if (mkdir(SavePath) == 0)
                break;
            strcpy(fname, SavePath);
            strcat(fname, "\\");
            strcat(fname, tfname);
            f = fopen(fname, "wb");
            if (f)
            {
                fclose(f);
                unlink(fname);
                break;
            }
            SavePath[0]++;
        }
        strcat(SavePath, "\\");
    }

    strlwr(SavePath);
    strlwr(RunPath);
}

void GetParameters(char *lpCmdLine)
{
    char *ptr;

  // Find out game speed first!
    ptr = strstr(lpCmdLine, "GAMESPEED=");  // Set game speed
    if (ptr)                                
    {
        GameSpeed = atoi(ptr + 10);
        if (GameSpeed < 1)
            GameSpeed = 1;
        if (GameSpeed > 5)
            GameSpeed = 5;
    }

  // Setup performance variables
    if (GameSpeed <= 1)
    {
        SmoothScroll = FALSE;
        MaxLights = 1;
    }
    if (GameSpeed < 3)
    {
        UseDirLight = FALSE;
    } 

  // Do rest of stuff...

    if (strstr(lpCmdLine, "DEBUG"))
        Debug = TRUE;                       // Sets up debug compatible mode.

    if (strstr(lpCmdLine, "FORCE15BIT"))
        Force15Bit = TRUE;                  // Forces 15 bit mode

    if (strstr(lpCmdLine, "FORCE16BIT"))
        Force16Bit = TRUE;                  // Forces 15 bit mode

    if (strstr(lpCmdLine, "EDITOR"))        // Start in editor mode
        StartInEditor = TRUE;

    if (strstr(lpCmdLine, "WINDOWED"))      // Run game in a window
        Windowed = TRUE;

    if (strstr(lpCmdLine, "NOQUICKLOAD"))   // Run game in a window
        NoQuickLoad = TRUE;

    if (strstr(lpCmdLine, "BORDERLESS"))    // Run game with no border
        Borderless = TRUE;

    if (strstr(lpCmdLine, "NOWIDE"))        // Prevents wide video buffers (stride > width)
        NoWideBuffers = TRUE;

    if (strstr(lpCmdLine, "FULLSCREEN"))    // Causes game map to start in full screen mode
        FullScreen = TRUE;

    if (strstr(lpCmdLine, "NOPRELOADSECTORS"))  // Causes sectors to NOT be cached
    {
        PreloadSectors = FALSE;
        PreloadSectorSize = 3;
    }

    ptr = strstr(lpCmdLine, "MONITOR=");    // Set monitor number
    if (ptr)                                
        MonitorNum = atoi(ptr + 8);

    ptr = strstr(lpCmdLine, "VIOLENCELEVEL=");  // Set Violence level
    if (ptr)                                
    {
        ViolenceLevel = atoi(ptr + 14);
        if (ViolenceLevel < 0)
            ViolenceLevel = 0;
        if (ViolenceLevel > 5)
            ViolenceLevel = 5;
    }

    ptr = strstr(lpCmdLine, "PRELOADSIZE=");    // Set game speed
    if (ptr)                                
    {
        PreloadSectorSize = atoi(ptr + 12);
        if (PreloadSectorSize > 8)
            PreloadSectorSize = 8;
        if (PreloadSectorSize < 3)
        {
            PreloadSectors = FALSE;
            PreloadSectorSize = 3;
        }
    }

    ptr = strstr(lpCmdLine, "CHUNKCACHESIZE="); // Set game speed
    if (ptr)                                
    {
        ChunkCacheSize = atoi(ptr + 15);
        if (ChunkCacheSize > 256)
            ChunkCacheSize = 256;
    }

    ptr = strstr(lpCmdLine, "DRIVER="); // Set display device
    if (!ptr)
        ptr = strstr(lpCmdLine, "DEVICE="); // Set display device (same thing)
    if (ptr)
    {
        ptr += 7;
        char *d = DXDriverMatchStr;
        while (*ptr && *ptr != ' ')
            *d++ = *ptr++;
        *d = 0;
        strlwr(DXDriverMatchStr);
    }

    if (strstr(lpCmdLine, "IGNORE3D"))      // Run game without Direct3D objects
        Ignore3D = TRUE;

    if (strstr(lpCmdLine, "OLDDIRECT3D"))   // Prevents game from using draw primitives
    {
        UseDirect3D2 = FALSE;               // Doesn't initialize Direct3D 2 stuff
        UseDrawPrimitive = FALSE;           // Forces system to use execute buffers
    }

    if (strstr(lpCmdLine, "SOFTWARE3D"))    // Turns on default software 3D (BLUE)
    {
        UseSoftware3D = TRUE;
    }

    if (strstr(lpCmdLine, "RAMPMODE"))      // Runs software 3D in ramp emulation mode
    {
        SoftRampMode = TRUE;
        SoftRGBMode = FALSE;
        UseSoftware3D = TRUE;
    }

    if (strstr(lpCmdLine, "RGBMODE"))       // Runs software 3D in rgb mode
    {
        SoftRampMode = FALSE;
        SoftRGBMode = TRUE;
        UseSoftware3D = TRUE;
    }

    if (strstr(lpCmdLine, "BLUE"))  // Causes game to use software 3D
    {
        UseBlue = TRUE;
        SoftRampMode = TRUE;        // Initializes Direct3D as if it was going to use 
        SoftRGBMode = FALSE;        // The software RGB mode (BUT WE NEVER DO!)
        UseSoftware3D = TRUE;
    }

    if (strstr(lpCmdLine, "NODRAWPRIM"))    // Forces system to use execute buffers
        UseDrawPrimitive = FALSE;

    if (strstr(lpCmdLine, "NOCACHEEXBUFS")) // Prevents system from caching execute buffers
        CacheExBufs = FALSE;

    if (strstr(lpCmdLine, "NOVIDZLOCK"))    // Prevents system from simultaneously lcoking the
        NoVidZBufLock = TRUE;               // display video and z surfaces (locks the VooDoo)

    if (strstr(lpCmdLine, "CLEARZ"))        // Causes system to allocate an extra ZBuffer so 
    {                                       // we can use the Viewport->Clear() function to 
        UseClearZBuffer = TRUE;             // update the screen ZBuffer, and so that we can
        NoVidZBufLock = FALSE;              // draw to the ZBuffer in TDisplay (since 
    }                                       // it won't be the tree screen ZBuffer.

    if (strstr(lpCmdLine, "VOODOO"))        // This is a voodoo card
    {                                       // we can use the Viewport->Clear() function to 
        IsVooDoo = TRUE;                    
        NoVidZBufLock = TRUE;   // Don't try to lock video and zbuffer at same time
        NoBlitZBuffer = TRUE;   // Can't blit to or from the zbuffer
        UseClearZBuffer = TRUE; // Use a secondary clear zbuffer for drawing instead of display zbuffer 
    }

    if (strstr(lpCmdLine, "NOMMX"))         // Force into no MMX mode
        IsMMX = FALSE;

    ptr = strstr(lpCmdLine, "VIDEOCAP="); // VIDEOCAP=megs,fps 
    if (ptr)
    {
        dovideocap = TRUE;
        videocapmegs = atoi(ptr + 9);
        if (videocapmegs < 4)
            videocapmegs = 4;
        else if (videocapmegs > 128)
            videocapmegs = 128;
        if (ptr[10] == ',')
            ptr += 11;
        else if (ptr[11] == ',')
            ptr += 12;
        else if (ptr[12] == ',')
            ptr += 13;
        else
            ptr = NULL;
        videocapfps = FRAMERATE;
        if (ptr)
            videocapfps = atoi(ptr);
        if (videocapfps < 8)
            videocapfps = 8;
        else if (videocapfps > FRAMERATE)
            videocapfps = FRAMERATE;
    }
    else
        dovideocap = FALSE;

    ptr = strstr(lpCmdLine, "FASTLOCK=");
    if (ptr)                                // Sets the buffer locking mode.  If on, causes 
    {                                       // buffers to be unlocked immediately after locked (faster)
        UnlockImmediately = !strnicmp(ptr + 9, "on", 2) ||
                            !strnicmp(ptr + 9, "1", 1);
    }
}

void GetINISettings()
{
  // ***** Get Program Paths *****

    INISetSection("Paths");
    INIGetText("ClassDefPath", ".", ClassDefPath, MAX_PATH);
    INIGetText("ExileRCPath", ".", ExileRCPath, MAX_PATH);
    INIGetText("ResourcePath", ".", ResourcePath, MAX_PATH);
    INIGetText("CurMapPath", ".", CurMapPath, MAX_PATH);
    INIGetText("BaseMapPath", ".", BaseMapPath, MAX_PATH);

    // Make sure each string ends with a backslash
    if (ClassDefPath[strlen(ClassDefPath) - 1] != '\\')
        strcat(ClassDefPath, "\\");

    if (ExileRCPath[strlen(ExileRCPath) - 1] != '\\')
        strcat(ExileRCPath, "\\");

    if (ResourcePath[strlen(ResourcePath) - 1] != '\\')
        strcat(ResourcePath, "\\");

    if (CurMapPath[strlen(CurMapPath) - 1] != '\\')
        strcat(CurMapPath, "\\");

    if (BaseMapPath[strlen(BaseMapPath) - 1] != '\\')
        strcat(BaseMapPath, "\\");

    INISetSection("Lighting");
    MaxLights = INIGetInt("MaxLights", 1);  
    Ambient3D = INIGetInt("Ambient3D", 100);
    LightRange3D = INIGetInt("LightRange3D", 180);

    INISetSection("Options");
    DoubleTapTicks = INIGetInt("DoubleTapTicks", 6);
}

// This function gets called as soon as the display system finds the right driver.
// You can check the parameters of the driver and set system flags here so that things
// work correctly.
void DriverSetupCallback()
{
    char buf[DRIVERDESCLEN];
    strncpyz(buf, DirectDrawDesc, DRIVERDESCLEN);
    strlwr(buf);

    // Set special flags for VooDoo cards
    if (strstr(buf, "voodoo"))  // Only voodoo cards would match this!
    {
        NoVidZBufLock = TRUE; // Don't try to lock video and zbuffer at same time
        NoBlitZBuffer = TRUE; // Can't blit to or from the zbuffer
        UseClearZBuffer = TRUE; // Use a secondary clear zbuffer for drawing instead of display zbuffer 
    }
}

BOOL InitLanguage()
{
  // Set language
    strcpy(Language, "english");

  // Load Language file
    DialogList.Initialize();

// Old MAYHEM stuff
#if 0
   // Get Language file
    p = strstr(lpCmdLine, "LANG=");
    if (p)
        Language = atoi(p + 5);
    else
        Language = GetProfileInt("intl", "iCountry", 1);
    char buf[20];
    wsprintf(buf, "LANGUAGE.%03d", Language);
    f = fopen(buf, "rb");
    if (!f)
        Language = ENGLISH;
    else
        fclose(f);

    if (Language == ENGLISH)
        SecondLang = ENGLISH;

    if (strstr(lpCmdLine, "KOR"))
        SecondLang = KOREAN;

    LoadLanguage(Language);
#endif

    return TRUE;
}

BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hDC, LPRECT lpRect, LPARAM)
{
    MonitorInfo.cbSize = sizeof(MonitorInfo);
    Monitor = hMonitor;
    GetMonitorInfo(hMonitor, (MONITORINFO*)&MonitorInfo);
    MonitorX = MonitorInfo.rcMonitor.left;
    MonitorY = MonitorInfo.rcMonitor.top;
    MonitorW = MonitorInfo.rcMonitor.right - MonitorInfo.rcMonitor.left;
    MonitorH = MonitorInfo.rcMonitor.bottom - MonitorInfo.rcMonitor.top;

    char *ptr = strstr(MonitorInfo.szDevice, "Display");
    if (!ptr)
        ptr = strstr(MonitorInfo.szDevice, "display");
    if (!ptr)
        ptr = strstr(MonitorInfo.szDevice, "DISPLAY");
    int dispnum = atoi(ptr + 7);

    if (MonitorNum <= 1 && ((dispnum == 1) || (dispnum == 0)))
    {
        if (MonitorNum == 0)
            strcpy(MonitorInfo.szDevice, "display");
        return FALSE;
    }

    if (MonitorNum == dispnum)
        return FALSE;

    return TRUE;
} 

BOOL InitMonitor()
{
    if (MonitorNum > GetSystemMetrics(SM_CMONITORS) || MonitorNum < 0)
        return FALSE;
    EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, NULL);

    return TRUE;
}

// ************************************************************************
// *                    Initialize and Close System                       *
// ************************************************************************

BOOL InitSystem()
{
    SystemFont = TFont::LoadFont(100);
    DialogFont = TFont::LoadFont(101);
    DialogFontShadow = TFont::LoadFont(102);
    SmallFont = TFont::LoadFont(104);
    GameFont = TFont::LoadFont(105);
//  GoldFont = TFont::LoadFont(105);
    
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Set correct imagery path
    if (NoNormals == TRUE)
        TObjectImagery::SetImageryPath(NONORMALPATH);
    else
        TObjectImagery::SetImageryPath(NORMALPATH);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Allocate the chunk cache as 1/4 free physical memory or 4MB
    int physmegs = TotalPhys() / (1024 * 1024);
    if (ChunkCacheSize < 1)
    {
        if (physmegs < 16)
        {
            DWORD res = MessageBox(NULL, "This game requires at least 16MB of system minimum to run well. "
                             "If you press OK the game will load normally, but game "
                             "performance will be severely degraded.",
                             "WARNING", MB_ICONSTOP | MB_OK);
            
            if (res == IDCANCEL)
                return FALSE;

            ChunkCacheSize = 2;
        }
        else if (physmegs <= 24)
            ChunkCacheSize = 3;
        else if (physmegs <= 32)
            ChunkCacheSize = 4;
        else
            ChunkCacheSize = min(physmegs - 32, 16);
    }
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
    if (PreloadSectorSize < 0)
    {
        if (physmegs < 16)
        {
            PreloadSectorSize = 3;
            PreloadSectors = FALSE;
        }
        else if (physmegs <= 24)
        {
            PreloadSectorSize = 3;
            PreloadSectors = FALSE;
        }
        else if (physmegs <= 32)
        {
            PreloadSectorSize = 4;
            PreloadSectors = TRUE;
        }
        else
        {
            PreloadSectorSize = 5;
            PreloadSectors = TRUE;
        }
    }
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
    ChunkCache.AllocCache(ChunkCacheSize);
    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }

  // Do the critical section object
    InitializeCriticalSection(&CriticalSection);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Create main display
    if (!Display->Initialize(WIDTH, HEIGHT, BPP))
        FatalError("Couldn't open main display", NULL);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Now display is initialized, set up video cap system (if needed)
    if (dovideocap)
        VideoCapture.Initialize(videocapmegs, videocapfps);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Setup system color tables
    MakeColorTables();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Print stats
    Status("Revenant Version 0.30 Build %s %s\n"
           "Copyright (C) Cinematix Studios, Inc. 1998\n", __DATE__, __TIME__);
    if (IsMMX)
        Status("MMX detected, using advanced lighting\n");
    else
        Status("No MMX detected, using normal lighting\n");
    Status("Physical memory %d MB\n", physmegs);
    Status("Chunk cache size %d MB\n", ChunkCacheSize);
    Status("Available display devices: %s\n", DriversAvailable);
    Status("Current display device: %s - %s\n", DirectDrawName, DirectDrawDesc);
    Status("Preload sectors %s, preload size %d\n", (PreloadSectors?"ON":"OFF"), PreloadSectorSize);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Initialize the timer
    Status("Initializing timers\n");
    if (!Timer.Initialize())
        FatalError("Couldn't initialize timer", NULL);

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Capture the mouse cursor
//  SetCapture(MainWindow.Hwnd());

  // Initialize Direct Input
    Status("Initializing direct input\n");
    InitializeDirectInput();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Initialize Joysticks (if any)
    Status("Initializing joysticks\n");
    InitializeJoysticks();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Start loader thread..
    TObjectImagery::BeginLoaderThread();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Load class.def
    Status("Loading classes with %s option\n", NoQuickLoad?"NOQUICKLOAD":"QUICKLOAD");
    TObjectClass::LoadClasses();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Setup player manager
    Status("Initializing multi-player manager\n");
    PlayerManager.Initialize();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
  // Load rules data
    Status("Loading game rules\n");
    Rules.Initialize();

    if (!_CrtCheckMemory())
    {
//      _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
    return TRUE;
}

void CloseSystem()
{
  // Delete dialog list
    DialogList.Close();

  // Kill fonts
    delete SystemFont;
    delete DialogFont;
    delete DialogFontShadow;
    delete SmallFont;
    delete GameFont;
//  delete GoldFont;

  // Kill video capture buffers (if they are allocated)
    VideoCapture.Close();

  // Kill rules data
    Rules.Close();

  // Close player manager
    PlayerManager.Close();

  // Free all object classes in the class system
    TObjectClass::FreeClasses();

  // Resume all threads
    if (ThreadsPaused)
        ResumeThreads();        

    if (CurrentScreen)
    {
        CurrentScreen->Close();
        CurrentScreen = NULL;
    }

  // End loader thread..
    TObjectImagery::EndLoaderThread();

  // Close Sound System
    CDStop();
    CDClose();
    SoundPlayer.Close();

  // Release DirectInput (including joystick)
    CloseDirectInput();

  // Release the mouse capture
    ReleaseCapture();

  // Stop timer stuff
    Timer.Close();

  // Close the display
    Display->Close();

    MSG Message;
    while (PeekMessage(&Message, NULL, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

  // Close the main window
    MainWindow.Close();

  // Clear out any clipping rectangle
    ClipCursor(NULL);
}

// ************************************************************************
// *                   WinMain - Main Program Function                    *
// ************************************************************************

/* Pretty simple here.  Just make a main window, start its message loop
 * going, then when the message loop returns, close the program. */

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
#ifdef _DEBUG
   _CrtMemState s1;

   _CrtMemCheckpoint( &s1 );

    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_WNDW);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
#endif

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Preset capabilities flags
    IsMMX = HasMMX();

    StartMemory.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&StartMemory);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

    ::hInstance = hInstance;

    int size = sizeof(SObjectInfo);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Set windows to return fail when file not found..
    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOOPENFILEERRORBOX);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Calls small heap alloc routine for requests < MAXSMALLHEAP
    _set_sbh_threshold( MAXSMALLHEAP );

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Randomize!!
    srand( (unsigned)time( NULL ) );

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Get upper case command line
    strupr(lpCmdLine);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Gets location of program path and save path
    GetProgramPaths(lpCmdLine, RunPath, SavePath);
    _chdir(RunPath);
    INISetPath(RunPath);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // What version of Windows is this
    DWORD ver  = GetVersion();
    BYTE major = LOBYTE(ver);
    BYTE minor = HIBYTE(ver);

    if (major <= 3) FatalError("This game requires Windows '95/NT 4.0 (or higher)!", NULL);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Get INI file settings
    GetINISettings();

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Get Program Parameters
    GetParameters(lpCmdLine);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif
  // Init Pause Thread Mutex
    PauseMutex = CreateMutex(NULL, FALSE, NULL);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Init monitor information
    if (!InitMonitor())
        FatalError("Invalid monitor selected", NULL);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Initialize language resources
    if (!InitLanguage());

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Create the main window
    if (!MainWindow.Initialize(hInstance, hPrevInstance, lpCmdLine, nCmdShow))
        FatalError("Couldn't create main window", NULL);

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Ok, now initialize the rest of the system
    if (!InitSystem())
        return 0;

#ifdef _DEBUG
    if (!_CrtCheckMemory())
    {
        _CrtMemDumpAllObjectsSince(&s1);
        _RPT0(_CRT_ERROR, "Memory Error");
    }
#endif

  // Figure out first screen
    PTScreen NextScreen = &PlayScreen;

  // Do game screens
    while (NextScreen)
        NextScreen = TScreen::ShowScreen(NextScreen, 0);

  // Close the system down
    CloseSystem();

    return 0;
}
