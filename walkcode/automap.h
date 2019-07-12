// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    automap.h  - Automapper File                       * 
// *************************************************************************

#ifndef _AUTOMAP_H
#define _AUTOMAP_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#include "automapdata.h"


// Define the area around the player that gets revealed
#define AUTOMAP_VIEW_WIDTH  8
#define AUTOMAP_VIEW_HEIGHT 8

// Define the maximum number of pixels the map can move while trying to
// reach the Target scroll position
#define MAX_AUTOMAP_SCROLL_SPEED 16


//============================================================================
// Class : TAutoMap.
//----------------------------------------------------------------------------
//  Desc : The automapper pane is a small version of the automatic mapping
//         system which keeps track of where the player has been and displays
//         a portion of this map on the mini-pane.
//============================================================================

_CLASSDEF(TAutoMap)
class TAutoMap : public TPane
{
    // Member Functions

    public:

    TAutoMap() : TPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT) {}
    ~TAutoMap() {}

    virtual BOOL Initialize();
    virtual void Close();

    virtual void DrawBackground();

    BOOL LoadMapGraphics(int MapNum);
    void FreeMapGraphics();

    void RecordTravels();

    int GetMapNumber(char *MapName);

    BOOL GetMaskColorActive(int MapNum, int Color);
    BOOL GetMaskColorActive(char *MapName, int Color);

    void SetMaskColorActive(int MapNum, int Color, BYTE Active);
    void SetMaskColorActive(char *MapName, int Color, BYTE Active);

    virtual void MouseClick(int button, int x, int y);
    virtual void MouseMove(int button, int x, int y);

    size_t WriteAutoMapData(FILE *fp);
    size_t ReadAutoMapData(FILE *fp);


    protected:

    void DrawPlayerMarker(int PlayerX, int PlayerY);
    void RestorePlayerMarker();

    void DrawAutoMap();

    inline WORD MergePixel(WORD *Background, WORD *Overlay, int Mul, int Div);
    inline WORD MergePixelToBlack(WORD *Overlay, int Mul, int Div);

    void CheckMaps();

    void GetPlayerOnAutoMap(int &PlayerX, int &PlayerY);


    // Member Variables

    protected:

    PTAutoMapList MapList;             // Pointer to an array holding
                                       // information about all the automaps

    SRect MapArea;                     // Where on the full map the current Automap
                                       // is showing (in World Coords.)

    int ScrollX, ScrollY;              // Current scroll position of the map
    int OldScrollX, OldScrollY;        // Scroll position of previous frame

    BOOL ScrollToTarget;               // Flag that we're supposed to scroll
                                       // the map to the TargetScroll
    int TargetScrollX, TargetScrollY;  // Target position to Scroll the map to
    int MapScrollDx, MapScrollDy;      // How many pixels the map moves when
                                       // trying to reach the TargetScroll

    int OldPlayerX, OldPlayerY;        // Previous player position

    WORD SavePlayerPixel;              // Saves the pixel underneath where the player is
    int MarkerX, MarkerY;              // Where the SavePlayerPixel is

    BOOL DraggingMap;                  // Flag for if the user is moving the
                                       // map with the mouse

    int DragMouseX, DragMouseY;        // Used to keep track of how far the mouse
                                       // has moved since starting to drag the map

    int DragScrollX, DragScrollY;      // Used to keep track of the original scroll
                                       // position when the map begins to scroll

    int CurrentMap;                    // Which map the automap is currently showing

    PTAutoMapGraphics MapGrfx;         // Pointer to the class holding all
                                       // the graphics needed for an automap

    PTBitmap DisplayMap;               // Version of the map shown to the user

    BOOL HasMap;                       // Flag for if the player has a map for
                                       // this map in his inventory or not

    BYTE *ActiveBuf;                   // Pointer to a buffer that holds the
                                       // active information for all the maps

    BYTE *Active;                      // Pointer to the array of colors used
                                       // to reveal the current automap
};

#endif

