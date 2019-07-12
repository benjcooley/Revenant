// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 RevDefs.h - definitions/limits/etc.                   *
// *************************************************************************

#ifndef _REVDEFS_H
#define _REVDEFS_H

// Max Objects States (NOTE: changing this value will not cause any game resource file to 
// need to be resaved as all resource files have an unlimited number of states).
#define MAXOBJSTATES 256

// Game directories
#define BASEMAPDIR "map"
#define CURMAPDIR "curmap"

// Screen size and color depth defines
#define WIDTH  640
#define HEIGHT 480
#define BPP    16

// math defines
#define M_PI ((double)3.141592654)
#define M_2PI (M_PI + M_PI)
#define TORADIAN ((double)M_PI / (double)180.0)
#define TODEGREE ((double)180.0 / (double)M_PI)
#define CAMERAANGLE   ((double)30.0)
#define CAMERARADIANS (CAMERAANGLE * TORADIAN)
#define ZSCALE (0.7)

// --- Texture memory reserve ---
// System will use system memory instead of video memory for various buffers and try to leave
// at least this amount of memory free for textures.
#define TEXTURERESERVE (512*1024)

// X file substitute character
#define SUBSTITUTE_CHAR "x3ds"

// The distance from the chars feet to the point where lighting is calculated
#define LIGHTINGCHARHEIGHT 50

// String lengths
#define NAMELEN        32
#define RESNAMELEN     32
#define SOUNDLISTLEN   32

// File name lengths
#define FILENAMELEN    128

// Calls Small heap alloc for sizes less than MAXSMALLHEAP
#define MAXSMALLHEAP   1920 // 1920 is max size (in 16 bytes increments)

// Animator Edge defines
#define ANIMATOREDGEWIDTH  256
#define ANIMATOREDGEHEIGHT 256

// Sector defines
#define SECTORWIDTH     1024                        // in world coords
#define SECTORWIDTHSCR  (SECTORWIDTH * 2)           // in screen coords
#define SECTORWSHIFT    10
#define SECTORHEIGHT    1024                        // in world coords
#define SECTORHEIGHTSCR (SECTORHEIGHT * 2)          // in screen coords
#define SECTORHSHIFT    10

#define SECTORWINDOWX   3
#define SECTORWINDOWY   3
#define SECTORWINDOWWIDTH   (SECTORWIDTH * SECTORWINDOWX)
#define SECTORWINDOWHEIGHT  (SECTORHEIGHT * SECTORWINDOWY)

#define MAXSECTORX      32
#define MAXSECTORY      32
#define MAXMAPWIDTH     (MAXSECTORX * SECTORWIDTH)
#define MAXSCREENWIDTH  (MAXSECTORX * SECTORWIDTHSCR)
#define MAXMAPHEIGHT    (MAXSECTORY * SECTORHEIGHT)
#define MAXSCREENHEIGHT (MAXSECTORY * SECTORHEIGHTSCR)

#define PLATESPERSECTX  8
#define PLATESPERSECTY  8
#define PLATEWIDTH      (SECTORWIDTH / PLATESPERSECTX)
#define PLATEHEIGHT     (SECTORHEIGHT / PLATESPERSECTY)
#define MAXPLATESX      (MAXSECTORX * PLATESPERSECTX)
#define MAXPLATESY      (MAXSECTORY * PLATESPERSECTY)

// Shift to convert to and from walkmap coordinates
#define WALKMAPSHIFT        4
#define WALKMAPGRANULARITY  (1 << WALKMAPSHIFT)

// Ratio of walkmap height to S3DPoint world z position
#define WALKMAPCONST    1
#define ZPOSCONST       1

// Step distance. Maximum distance a character can step up or down
#define STEPDISTANCE 32

// Maximum distance a character can move without checking walkmap for obstacles
#define MOVECHECKDIST 8

// Map View Position and Size
#define FRAMEMAPPANEX       32
#define FRAMEMAPPANEY       16
#define FRAMEMAPPANEWIDTH   576
#define FRAMEMAPPANEHEIGHT  320

// Map units per meter
#define UNITSPERMETER       64

// Scroll buffer
#define UPDATEWIDTH         64
#define UPDATEHEIGHT        64
#define UPDATEMASKX         (~(DWORD)(UPDATEWIDTH - 1))
#define UPDATEMASKY         (~(DWORD)(UPDATEHEIGHT - 1))
#define MOSAICTILEX         (2)
#define MOSAICTILEY         (1)

// Map orientation
#define MAPTOSCREENYSHIFT 1
#define MAPTOSCREENXRATIO 1
#define MAPTOSCREENYRATIO 2

// Inventory organization
#define INVITEMREALWIDTH    40
#define INVITEMREALHEIGHT   40
#define INVITEMSPACEX       5
#define INVITEMSPACEY       5
#define INVITEMWIDTH        (INVITEMREALWIDTH + INVITEMSPACEX)
#define INVITEMHEIGHT       (INVITEMREALHEIGHT + INVITEMSPACEY)
#define INVITEMSX           7
#define INVITEMSY           2
#define MAXINVITEMS         (INVITEMSX * INVITEMSY)

// Multipane position and size
#define MULTIPANEX      459
#define MULTIPANEY      346
#define MULTIPANEWIDTH  168
#define MULTIPANEHEIGHT 128

// The editor class pane
#define EDMULTIPANEX        493
#define EDMULTIPANEY        348
#define EDMULTIPANEWIDTH    132
#define EDMULTIPANEHEIGHT   124

// Multipane control panel (buttons) position and size
#define MULTICTRLPANEX      410
#define MULTICTRLPANEY      338
#define MULTICTRLPANEWIDTH  72
#define MULTICTRLPANEHEIGHT 142

// Quick spell buttons
#define QUICKSPELLX         2
#define QUICKSPELLY         343
#define QUICKSPELLWIDTH     437
#define QUICKSPELLHEIGHT    35
#define NUMQUICKSPELLS      4

// Inventory position and size
#define INVENTORYPANEX      16
#define INVENTORYPANEY      386
#define INVENTORYPANEWIDTH  382
#define INVENTORYPANEHEIGHT 85

#define INVENTORYCONTX      9
#define INVENTORYCONTY      23

#define INVENTORYSTARTX     72
#define INVENTORYSTARTY     0

// Text bar position and size
#define TEXTBARX        122
#define TEXTBARY        336
#define TEXTBARWIDTH    198
#define TEXTBARHEIGHT   14

// Status bars
#define HEALTHBARX          16
#define HEALTHBARY          94
#define HEALTHBARWIDTH      12
#define HEALTHBARHEIGHT     164

#define STAMINABARX         612
#define STAMINABARY         94
#define STAMINABARWIDTH     12
#define STAMINABARHEIGHT    164

// Status bar for editor
#define STATUSBARX      4
#define STATUSBARY      (FRAMEMAPPANEY+FRAMEMAPPANEHEIGHT+4)
#define STATUSBARWIDTH  632
#define STATUSBARHEIGHT 16

// Console position and size
#define CONSOLEX        4
#define CONSOLEY        (STATUSBARY+STATUSBARHEIGHT)
#define CONSOLEWIDTH    381
#define CONSOLEHEIGHT   124

// Console text buffer size
#define TEXTBUFSIZE     10000

// Tool bar for editor
#define TOOLBARX        (CONSOLEX + CONSOLEWIDTH)
#define TOOLBARY        CONSOLEY
#define TOOLBARWIDTH    (EDMULTIPANEX - TOOLBARX)
#define TOOLBARHEIGHT   CONSOLEHEIGHT

// Background redraw flags
#define BGDRAW_UNLIT    0
#define BGDRAW_LIGHTS   1
#define BGDRAW_AMBIENT  2
#define BGDRAW_LIT      3
#define BGDRAW_NONE     4
#define BGDRAW_REDRAW   0x10000000
  // REDRAW forces drawing from buffer

// Max. Number of 3D frames
#define MAXSCENEFRAMES 40

// Max. Number of Lights on screen
#define MAXLIGHTS 4

// Max. Number of Objects returned by FindObjectsInRange
#define MAXFOUNDOBJS 10

// Max. Number of 3D animations in an object
#define MAXANIMS 256

// Clipping constants
#define CLIP_EDGES 0
#define CLIP_WRAP  1

// Timer tick defines
#define FRAMERATE 24
#define RESOLUTION 5

// Animation limit definitions
#define MAXSPRITES          256
#define MAXPATHANIMATIONS   16

// Maximum font characters
#define MAXFONTCHARS 256

// Maximum Restore Buffers
#define MAXRESTOREBUFS 10

// Default bitmap compression
#define DEFBITMAPCOMP 1280

// Default animation compression
#define DEFANICOMP  1280

// Max compression
#define MAXCOMP 65535

// Orientation defines
#define OR_REVERSEHORZ 1
#define OR_REVERSEVERT 2

// Alias code defines
#define AL_EOD      0xFF                    // end of data code
#define AL_EOL      0xFE                    // end of line code
#define AL_MAXRUN   0xFD                    // maximum run length

// Thumbnail pics for editor
#define THUMBNAILWIDTH  16
#define THUMBNAILHEIGHT 16

// Mouse buttons defines
#define MB_NOTHING      0
#define MB_LEFTDOWN     1
#define MB_RIGHTDOWN    2
#define MB_MIDDLEDOWN   3
#define MB_LEFTUP       4
#define MB_RIGHTUP      5
#define MB_MIDDLEUP     6
#define MB_LEFTDBLCLK   7
#define MB_RIGHTDBLCLK  8
#define MB_MIDDLEDBLCLK 9

// Bitmap Flags
#define BM_8BIT        0x0001   // Bitmap data is 8 bit.
#define BM_15BIT       0x0002   // Bitmap data is 15 bit.
#define BM_16BIT       0x0004   // Bitmap data is 16 bit.
#define BM_24BIT       0x0008   // Bitmap data is 24 bit.
#define BM_32BIT       0x0010   // Bitmap data is 24 bit.
#define BM_ZBUFFER     0x0020   // Bitmap has ZBuffer.
#define BM_NORMALS     0x0040   // Bitmap has Normal Buffer.
#define BM_ALIAS       0x0080   // Bitmap has Alias Buffer.
#define BM_ALPHA       0x0100   // Bitmap has Alpha Buffer.
#define BM_PALETTE     0x0200   // Bitmap has 256 Color SPalette Structure.
#define BM_REGPOINT    0x0400   // Bitmap has registration point
#define BM_NOBITMAP    0x0800   // Bitmap has no pixel data
#define BM_5BITPAL     0x1000   // Bitmap palette is 5 bit for r,g,b instead of 8 bit
#define BM_COMPRESSED  0x4000   // Bitmap is compressed.
#define BM_CHUNKED     0x8000   // Bitmap is chunked out

// Animation Drawing mode flags
#define DM_DEFAULT      0x00000000  // Default is always defined as 0
#define DM_NOCLIP       0x00000001  // Disables clipping when drawing
#define DM_WRAPCLIP     0x00000002  // Enables wrap clipping (forces wrap clipping regardless of surface wrap mode)
#define DM_WRAPCLIPSRC  0x00000004  // Enables wrap clipping of source buffer (forces wrapsrc clipping regardless of surface wrap mode)
#define DM_STRETCH      0x00000008  // Enables Stretching when drawing
#define DM_BACKGROUND   0x00000010  // Draws bitmap to background
#define DM_NORESTORE    0x00000020  // Disables automatic background restoring
#define DM_REVERSEVERT  0x00000040  // Reverses vertical orientation
#define DM_REVERSEHORZ  0x00000080  // Reverses horizontal orientation
#define DM_TRANSPARENT  0x00000100  // Enables transparent drawing.
#define DM_ZMASK        0x00000200  // Enables ZBuffer Masking.
#define DM_ZBUFFER      0x00000400  // Draws bitmap ZBuffer to destination ZBuffer
#define DM_NORMALS      0x00000800  // Draws bitmap Normals to dest. Normal buffer
#define DM_ALIAS        0x00001000  // Antiailiases edges using bitmap alias data.
#define DM_ALPHA        0x00002000  // Enables Alpha drawing.
#define DM_SHUTTER      0x00004000  // Enable Shutter transparent drawing.
#define DM_TRANSLUCENT  0x00008000  // Enables Translucent drawing.
#define DM_FADE         0x00010000  // Fade image to key color.
#define DM_USEREG       0x00020000  // Draws bitmap based on registration point
#define DM_SELECTED     0x00040000  // Draw selection highlight around bitmap
#define DM_CHANGECOLOR  0x00080000  // Draw in a different color
#define DM_CHANGEHUE    0x00100000  // Use color to modify hue of image
#define DM_CHANGESV     0x00200000  // Use color to modify saturation and brightness
#define DM_NODRAW       0x00400000  // Prevents bitmap graphics buffer from drawing to destination
#define DM_ZSTATIC      0x00800000  // Draws bitmap at a static z value
#define DM_ALPHALIGHTEN 0x01000000  // Enables Alpha drawing lighten only.
#define DM_DOESCALLBACK 0x02000000  // Flag set by low level draw func indicating it calls rect CALLBACK itself
#define DM_NOWRAPCLIP   0x04000000  // Overrides surface clipping mode to not wrap clip
#define DM_NOHARDWARE   0x08000000  // Force no hardware use
#define DM_NOCHECKZ     0x10000000  // Causes ZBuffer Draws to use transparency only (no Z intersection) 
#define DM_FILL         0x20000000  // Fills the destination with the current color
#define DM_USEDEFAULT   0x80000000  // Causes draw routines to supply a default value

// Animation flags
#define AF_LOOPING          0x0001 // Circles back to original position
#define AF_FACEMOTION       0x0002 // This animation has facing motion data
#define AF_INTERFRAME       0x0004 // Uses interframe compression
#define AF_NOREG            0x0008 // Use 0,0 of FLC as registration pt of animation
#define AF_SYNCHRONIZE      0x0010 // Causes animation frame to mach 'targets' animation frame
#define AF_MOVE             0x0020 // Use the deltas in the ani to move the x,y position
#define AF_NOINTERPOLATION  0x0040 // Prevents system from interpolating between animations
#define AF_PINGPONG         0x0080 // Pingpong the animation
#define AF_REVERSE          0x0100 // Play the animation backwards
#define AF_NORESTORE        0x0200 // Draw to screen but don't bother to restore
#define AF_ROOT             0x0400 // This animation is a root state
#define AF_FLY              0x0800 // This animation is a flying animation (jump, etc.)
#define AF_SYNC             0x1000 // Synchronize all animations on screen
#define AF_NOMOTION         0x2000 // Ignore motion deltas
#define AF_ACCURATEKEYS     0x4000 // Has only high accuracy 'code' syle 3D ani keys
#define AF_ROOT2ROOT        0x8000 // This animation starts at root and returns to root
// NOTE: The AF_XXX flags are !!!!16 BIT!!! you can't add any more!

// Font justification
#define JUSTIFY_LEFT    (1 << 0)        // force text to left side
#define JUSTIFY_CENTER  (1 << 1)        // force text to center
#define JUSTIFY_RIGHT   (1 << 2)        // force text to right
#define JUSTIFY_CLIP    (1 << 3)        // force text to stay inside clipping rect

// Run Buffer defines
#define RM_DATA     0x0001
#define RM_NORMAL   0x0002

// Cursor types
#define CURSOR_NONE         -1      // Nothing extra added to cursor
#define CURSOR_HAND         0       // For using or getting objects
#define CURSOR_EYE          1       // Examining or reading objects
#define CURSOR_MOUTH        2       // Talking to people or consuming food/drink
#define CURSOR_DOOR         3       // Go through a door
#define CURSOR_STAIRS       4       // Go up/down stairs
#define CURSOR_HOURGLASS    5       // Wait for system
#define CURSOR_SWORDS       6       // Crossed swords, for combat

#define NUMCURSORTYPES      7

// Language defines
#define ENGLISH         1
#define KOREAN          82

// Max header string for .def file
#define MAXHEADERLEN 64

// Generic text input max size
#define MAXINPUTLEN 192

// Size of input buffers
#define INPUT_MOUSEBUFSIZE      64
#define INPUT_KEYBOARDBUFSIZE   64

// Maximum number of events to allow in MainWindow
#define MAXEVENTS 4

// Maximum number of selected objects in the editor
#define MAXSELECTEDOBJS 256

// Maximum buttons per button list
#define MAXBUTTONS      64

// For parsing
#define MAXLINE         160
#define MAXIDENTLEN     32

// Walkmap handler
#define WALK_TRANSFER   0
#define WALK_CAPTURE    1
#define WALK_CLEAR      2
#define WALK_EXTRACT    3

// Effect States
#define EF_NONE     0   // No effect
#define EF_BEGIN    1   // Beginning animation
#define EF_ANIMATE  2   // Normal animation
#define EF_EXPLODE  4   // Ending explosion
#define EF_HIT      8   // Hit character

#define AVG_MASS    128         // Average mass of a character in FES (ForExileSaken) units

#define TOGGLE(f)   (f = !f)

// ColorMode defines
#define COLOR 1
#define MONO  2

// Queue Entry defines
#define QE_NONE    0
#define QE_QUEUED  1
#define QE_LOADING 2
#define QE_LOADED  3

// Chunk Type
#define CT_NONE         0
#define CT_8BIT         1
#define CT_16BIT        2
#define CT_COMPRESSED   4
#define CT_UNCOMPRESSED 8

// Pathnames for imagery
#define NORMALPATH      "NORMAL\\"
#define NONORMALPATH    "IMAGERY\\"

// Gravity sucks.
#define ROLLOVER            (1 << 16)       // gives us 2^16 precision between integer position units
#define GRAVITY             (6 * ROLLOVER)  // distance / time^2
#define TERMINAL_VELOCITY   (50 * ROLLOVER) // realism? pshaw

// Map version
#define MAP_VERSION     9

#endif
