// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   Revenant.h - Main Include File                      *
// *************************************************************************

#ifndef _REVENANT_H
#define _REVENANT_H

#ifdef EXILE
#error Please change your EXILE macro definition in your Build/Settings menu to REVENANT
#endif

// NOTE: DO NOT INCLUDE WINDOWS.H HERE!!
// MAKES COMPILING VERY VERY SLOOOOW

#ifndef _WINDOWS_

// Forces files that NEED windows to included BEFORE exile.h
#define _INC_WINDOWS

#ifndef WINVER
#define WINVER 0x0400
#endif

#if !defined(_PPC_) && !defined(_ALPHA_) && !defined(_MIPS_) && !defined(_X86_) && defined(_M_IX86)
#define _X86_
#endif

#if !defined(_PPC_) && !defined(_ALPHA_) && !defined(_X86_) && !defined(_MIPS_) && defined(_M_MRX000)
#define _MIPS_
#endif

#if !defined(_PPC_) && !defined(_ALPHA_) && !defined(_X86_) && !defined(_MIPS_) && defined(_M_ALPHA)
#define _ALPHA_
#endif

#if !defined(_PPC_) && !defined(_ALPHA_) && !defined(_X86_) && !defined(_MIPS_) && defined(_M_PPC)
#define _PPC_
#endif

#include <windef.h>

#endif // _WINDOWS_

#include <stdio.h>
#include <string.h>
#include <crtdbg.h>

#ifndef _REVDEFS_H
#include "revdefs.h"
#endif

#ifndef _REVTYPES_H
#include "revtypes.h"
#endif

// *****************************
// * Game Specific Definitions *
// *****************************

#ifdef REVENANT

// ****** Setup Memory Checking Stuff ******

#ifdef _DEBUG

#ifdef _CRTDBG_MAP_ALLOC
#error Hey, we're doing the debug functions ourselves!
#endif

void *xmalloc(int size, char *file, int line);
void *xrealloc(void *p, int size, char *file, int line);
void xfree(void *p);

// Use our alloc functions
#define malloc(m) xmalloc(m, __FILE__, __LINE__)
#define calloc(c,m) xmalloc(c*m, __FILE__, __LINE__)
#define realloc(p, m) xrealloc(p, m, __FILE__, __LINE__)
#define free(p) xfree(p)

// Force _strdup() to use debug malloc too..
#define _strdup(s) strcpy((char*)malloc(strlen(s) + 1), s)

#else

#ifdef _CRTDBG_MAP_ALLOC
#error Can't use debug mem functions in release mode
#endif

void *xmalloc(int size);
void *xrealloc(void *p, int size);
void xfree(void *p);

// Use our alloc functions
#define malloc(m) xmalloc(m)
#define calloc(c,m) xmalloc(c*m)
#define realloc(p, m) xrealloc(p, m)
#define free(p) xfree(p)

// Force _strdup() to use debug malloc too..
#define _strdup(s) strcpy((char*)malloc(strlen(s) + 1), s)

#endif

// *** Global Variables ***
extern HINSTANCE hInstance;
extern HANDLE PauseMutex;

// Display Parameters

// Run path
#define RUNPATHLEN 100
extern char RunPath[MAX_PATH];
extern char SavePath[MAX_PATH];

extern char ClassDefPath[MAX_PATH]; // Where to load / save Class.Def
extern char ExileRCPath[MAX_PATH];  // Where to run ExileRC from & where
                                    // the graphics for the resources are
extern char ResourcePath[MAX_PATH]; // Where to read / write the resources
extern char BaseMapPath[MAX_PATH];  // Where the untouched version of the game map is stored
extern char CurMapPath[MAX_PATH];   // Where the current map is stored

// Multi-Monitor Variables (Note: Additional Monitor globals are defined in MONITOR.H)
extern int MonitorNum;          // Monitor game will run on (default is 1, primary)
extern int MonitorX, MonitorY;  // Relative position of monitor in desktop coordinates
extern int MonitorW, MonitorH;  // Width and height of monitor (before changing video modes)
extern char DXDriverMatchStr[FILENAMELEN]; // Will use first DX driver who's description contains the given string (not case sensitive)
                                           // i.e. use if string is "permidia" and driver desc is "Glint Permidia 2 3D"

// Game speed variable
extern int GameSpeed;           // Value 1-5 which determines how fast the game is running
                                // Used to switch on/off processor intensive effects
extern int ViolenceLevel;       // Value 0-5 which determines how much blood and gore, etc.
                                // 0-none

// Animation toggles..
extern BOOL WaitVertRetrace;    // Does vertical retrace checking (not used now)
extern BOOL ShowDrawing;        // Swaps the back for the front buffer so we can wach redraw
extern BOOL ShowBufNames;       // Shows labels for back/front buffer (not used now)
extern BOOL NoNormals;          // Turns normal lights on/off
extern BOOL NoFrameSkip;        // Turns frame skipping on/off
extern BOOL PauseFrameSkip;     // Set during disk loading to avoid skipping afterwards
extern BOOL ClearBeforeDraw;    // Clear before drawing update buffers?
extern BOOL NoScrollZBuffer;    // Causes the system to not scroll the display z buffer
extern BOOL SmoothScroll;       // Do smooth scrolling
extern BOOL ScrollLock;         // Lock scrolling to the character
extern BOOL DrawRealtimeShadows;    // Draw alpha shadows for 3D characters
extern BOOL Show3D;             // Shows 3D objects and chars
extern BOOL Interpolate;        // Causes 3D animations to interpolate when state changes
extern BOOL DoPageFlip;         // Causes pages to flip (if off, screen will stop updating)
extern BOOL PauseWhenNotActive; // Causes program and threads to pause when not active
extern BOOL Windowed;           // True if game is to be run in a window
extern BOOL SingleBuffer;       // True if game using single buffering (TRUE if Windowed, or UsingHardware FALSE)
extern BOOL Borderless;         // When windowed, cut out the border (menu, other windows garabage)?
extern BOOL NoWideBuffers;      // Doesn't allow video buffers with stides wider than their widths
extern BOOL UseDirect3D2;       // True if want to be able to use drawprimive stuff
extern BOOL UseSoftware3D;      // Use software 3D (False assumes we want hardware if available)
extern BOOL SoftRampMode;       // Use RAMP mode for software 3D (hardware defaults to what's available)
extern BOOL SoftRGBMode;        // Use RGB mode for software 3D (hardware defaults to what's available)
extern BOOL UseBlue;            // Causes system to use BLUE software renderer
extern BOOL UseDrawPrimitive;   // If FALSE, 3D system does everything with Execute Buffers (always FALSE if UseDirect3D2 is FALSE)
extern BOOL UsingHardware;      // Set by 3D system to TRUE if 3D hardware accel or FALSE if software only
extern BOOL SoftRGBMode;        // Use RGB mode for software 3D (hardware defaults to what's available)
extern BOOL CacheExBufs;        // Attempt to cache execute buffers for chars and other animation
extern BOOL PreloadSectors;     // If true, will load a large sector cache for faster scrolling
extern BOOL NoPulseObjs;        // Prevents the game objects from being pulsed
extern BOOL NoAnimateObjs;      // Prevents the game objects from being animated
extern BOOL NoVidZBufLock;      // Prevents simultaneous locking of video and zbuffer (for voodoo)
extern BOOL NoBlitZBuffer;      // Card is incapable of blitting to the display ZBuffer (voodoo)
extern BOOL UseClearZBuffer;    // Use a clear z buffer for z restores (i.e. Viewport->Clear())
extern BOOL UnlockImmediately;  // Causes video buffers to be unlocked immediately after they're locked (faster)
extern BOOL IsVooDoo;           // True if using a voodoo card
extern BOOL IsMMX;              // Has MMX extensions
extern BOOL CaptureVideo;       // True when system is capturing video
extern BOOL NoAI;               // Turns off monster AI
extern BOOL AutoBeginCombat;    // Automatically begins combat if enemy in range and facing him
extern BOOL PlaySpeech;         // Play speech wave files
extern BOOL ShowDialog;         // Show dialog text if speech wave is played

// Watch zbuffer and normal draws
extern BOOL ShowZBuffer;
extern BOOL ShowNormalBuffer;

// Program run states
extern BOOL AppActive;          // Flag for if the game is the active application
extern BOOL Editor;             // This is true if we are in edit mode
extern BOOL StartInEditor;      // Whether to start the program in editor mode
extern BOOL NoQuickLoad;        // Prevent the program from attempting to load the IMAGERY.DAT file

// Render state controls
extern BOOL FlatShade;
extern BOOL SimpleLight;
extern BOOL DitherEnable;
extern BOOL BlendEnable;
extern BOOL SpecularEnable;
extern BOOL UseTextures;
extern BOOL ZEnable;
extern BOOL BilinearFilter;
extern BOOL NoUpdateRects;
extern BOOL Double3D, Triple3D;
extern BOOL UseDirLight;
extern int DirLightPercent;

// 3D Lighting adjustment percent values
extern int MaxLights;           // Maximum number of lights that can affect a 3d object
extern int Ambient3D;           // Adjust this below 100 for darker ambient, or above for lighter
extern int LightRange3D;        // Adjust this below 100 to decrease 3D light range, or above to increase

// Whether map pane is full screen or not
extern BOOL FullScreen;

// Music and sound effects
extern BOOL SoundSystemOn;

// Is control, shift, or alt down
extern BOOL CtrlDown, ShiftDown, AltDown;

// Character movement mode
extern BOOL GridSnap;

// Total size of preload area (in sectors) if PreloadSectors is on
extern int PreloadSectorSize;

// Total size of chunk cache to allocate (in MB's)
extern int ChunkCacheSize;

// Frame ticks for doubletap checking
extern int DoubleTapTicks;

// Global Map Pane Sizes
extern int MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT;
extern int SCROLLBUFWIDTH, SCROLLBUFHEIGHT;

// GetString variables
extern char *TempStr;
extern int MaxStringLength;
extern int CurGetStringChar;
extern BOOL GetStringDone;

// Ambient light variables
extern int    ambient;
extern double lred;
extern double lgreen;
extern double lblue;

// Language
extern char Language[NAMELEN];              // Where the current map is stored

// Simple Support Functions
inline char *strncpyz(char *dst, char *src, int n)
  { strncpy(dst, src, n-1); dst[n-1] = NULL; return dst; }
  // Does a strncpy and insures that last character is always null
inline char *strncatz(char *dst, char *src, int n)
  { int l = strlen(dst); return strncpyz(dst + l, src, n - l); }
  // Concatenates a src with dst, but does not go beyound n-1 chars for dest, and insures
  // a null is appended at end of dst
char *itos(int val, char *buf, int buflen);
  // Efficiently converts an int to a string given a buffer of 'buflen' size
int copyfiles(char *from, char *to, BOOL overwrite = TRUE);
  // Copy file command (uses wildcards!!)
int deletefiles(char *name);
  // Delete file command (uses wildcards!!)
DWORD tickcount();
  // Returns system ticks (in milliseconds) since system was turned on

// Prints error, or fatal error message, and exits
void Error(char *error, char *extra = NULL);
void FatalError(char *error, char *extra = NULL);
void ThreadError(char *error, char *extra = NULL);

// Event and Mutex error stuff
void WaitSingleErr(HANDLE obj);
void WaitMultipleErr(DWORD objs, CONST HANDLE *obj, BOOL all);

// Critical Section Functions
void BEGIN_CRITICAL();
void END_CRITICAL();

// Load a resource functions
void *LoadRCResource(char *name, int id); // Shouldn't be used anymore
void FreeRC(void *p);                     // Same here

// Makes a file path given the current settings of RunPath and SavePath
char *makepath(char *name, char *buf, int buflen);
// Open a FILE in the program path using the makepath() function
FILE *popen(char *file, char *flags);

// Random number generation
int random(int min, int max);

// Comma delimited list functions (useful for strings in "abcd,defg,hijk" format)
// If dst is NULL, retuns result pointer from static internal buffer
char *listrnd(char *src, char *dst = NULL, int len = 0);
    // Get random string from comma list
char *listget(char *src, int num, char *dst = NULL, int len = 0);
    // Get num string from comma list
int listnum(char *src);
    // Get total number of strings in comma list
BOOL listin(char *src, char *in);
    // Returns TRUE if string is in comma list (case insensitive)

// Causes all game threads to pause
void PauseThreads();
void ResumeThreads();

// Exit the game (Now why would somebody want to do that?)
void ExitGame();

// Prints out game status info to the display as game loads
void Status(char *fmt, ...);

// Memory free functions
DWORD MemUsed();    // Percentage of memory used
DWORD FreeMem();    // Free system memory (virtual)
DWORD TotalMem();   // Total system memory (virtual)
DWORD FreePhys();   // Free physical memory
DWORD TotalPhys();  // Total physical memory
DWORD FreePage();   // Free paging file memory
DWORD TotalPage();  // Total paging file memory

// INI File Functions
void INISetSection(char *newsection);
int INIGetInt(char *key, int def = 0, char *format = NULL);
void INISetInt(char *key, int i, char *format = NULL);
char *INIGetText(char *key, char *def, char *buf, int buflen);
void INISetText(char *key, char *str);
char *INIGetStr(char *key, char *def = NULL, char *buf = NULL, int buflen = 0);
void INISetStr(char *key, char *str);
int INIGetArray(char *key, int size, int *ary, int defsize = 0, int *defary = NULL, char *format = NULL);
void INISetArray(char *key, int size, int ary[], char *format = NULL);
BOOL INIGetBool(char *key, BOOL def = FALSE, char *yes = NULL, char *no = NULL);
void INISetBool(char *key, BOOL on, char *yes = NULL, char *no = NULL);
BOOL INIGetYesNo(char *key, BOOL def = FALSE);
void INISetYesNo(char *key, BOOL on);
BOOL INIGetTrueFalse(char *key, BOOL def = FALSE);
void INISetTrueFalse(char *key, BOOL on);
BOOL INIGetOnOff(char *key, BOOL def = FALSE);
void INISetOnOff(char *key, BOOL on);
BOOL INIParse(char *key, char *def, char *format, ...);
void INIPrint(char *key, char *format, ...);

// Debug BOOL variable
extern BOOL Debug;
extern BOOL Ignore3D;   // Whether to disallow 3D imagery (to make it run on systmes without Direct3D)

// Forces video mode to assume 15 or 16 bit
extern BOOL Force15Bit;
extern BOOL Force16Bit;

// Tick Sync variable
extern BOOL TickOccured;

// Disable timer
extern BOOL DisableTimer;

// Is System Closing Down?
extern BOOL Closing;

// For setting up 3D object bounding rects in the editor
extern BOOL UpdatingBoundingRect;

// Last Frame Milliseconds
extern int LastFrameTicks;
extern BOOL ShowFramesPerSecond;

// Global Objects
extern PTScreen     CurrentScreen;      // Currently displayed screen object 
extern PTScreen     NextScreen;         // Next Screen to be display object
extern TDisplay     display;            // Display object
extern T3DScene     Scene3D;            // 3d Object
extern PTDisplay    Display;            // Display object
extern TPlayScreen  PlayScreen;         // PlayScreen Object
extern TLogoScreen  LogoScreen;         // LogoScreen Object
extern TMapPane     MapPane;            // Main map pane for PlayScreen
extern THealthBar   HealthBar;          // Character's health
extern TStaminaBar  StaminaBar;         // Character's fatigue
extern TInventory   Inventory;          // Inventory of objects
extern TTextBar     TextBar;            // Info line for player
extern TMultiCtrlPane   MultiCtrl;      // Multipane control panel (buttons)
extern TEquipPane   EquipPane;          // Equipment pane
extern TAutoMap     AutoMap;            // Automapper
extern TSpellPane   SpellPane;          // Talisman stuff
extern TQuickSpellPane  QuickSpells;    // Quick spell buttons
extern TStatPane    StatPane;           // Display of char's abilities
extern TDialogPane  DialogPane;         // Dialog panel
extern TMainWindow  MainWindow;         // Windows Object
extern TScriptManager   ScriptManager;  // Manages all scripts in the game
extern TSaveGame    SaveGame;           // SaveGame Object
extern TChunkCache  ChunkCache;         // Tile Cache
extern TTimer       Timer;              // Timer Object
extern TVideoCapture VideoCapture;      // Video capture object
extern PTFont       SystemFont;         // Basic utility font
extern PTFont       DialogFont;         // Dialog font
extern PTFont       DialogFontShadow;   // Dialog font shadow
extern PTFont       SmallFont;          // Small game font
extern PTFont       GameFont;           // Medium game font
extern PTFont       GoldFont;           // Medium gold font
extern PTFont       MetalFont;          // Small gold/metal font
extern PTFont       MenuFont;           // Menu font
extern PTPlayer     Player;             // The active player for current game
extern PTMulti      GameData;           // Global game data pointer
extern TSoundPlayer SoundPlayer;        // Sound effects player
extern TControlMap  ControlMap;         // Contains the key/joystick mappings for game control
extern TAreaManager AreaManager;        // Manages the game area system
extern TPlayerManager PlayerManager;    // Stores the current player list for the game
extern TRules       Rules;              // Stores rules, like classes, char types, attacks, tables, etc.
extern TSpellList   SpellList;          // a list of spells in the game
extern TDialogList  DialogList;         // List of dialog and other game messages for current language

#endif

#endif
