// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *         automapdata.h - Automapper Data Definition File               *
// *************************************************************************

#ifndef _AUTOMAPDATA_H
#define _AUTOMAPDATA_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#define MAX_AUTOMAPS 20                // How many separate automaps we can have
#define MAX_AUTOMAP_OVERLAYS 20        // How many graphics can be overlayed
                                       // over an automap background


_STRUCTDEF(SAutoMapOverlay)
struct SAutoMapOverlay
{
  public:

  char Tag[80];                        // Name of this overlay (for scripts)

  OTBitmap Image;                      // Graphics that can be overlayed over
                                       // the map when active

  BYTE ActiveColor;                    // Which color activates the overlay

  int XOffset;                         // X & Y offset to draw the graphic at
  int YOffset;
};



_CLASSDEF(TAutoMapGraphics)
class TAutoMapGraphics
{
  public:

  OTBitmap Background;                 // Map background graphic

  OTBitmap OverlayMap;                 // Map that is drawn over the background
                                       // graphic as it is revealed

  OTBitmap MaskMap;                    // 8 bit graphic holding the mask data
                                       // specifying what to copy from the
                                       // OverlayMap to the background

  int NumOverlays;                     // How many overlays are in the array
  SAutoMapOverlay Overlays[MAX_AUTOMAP_OVERLAYS];
                                       // Array holding all the overlays that
                                       // can be put on top of the background
};



_STRUCTDEF(SAutoMapData)
struct SAutoMapData
{
  public:

  char MapName[80];                    // Name of the map (same name as the
                                       // corresponding Inventory map object)

  int ResID;                           // Resource ID for the AUTOMAP.xxx
                                       // specifying which graphics to use

  int Level;                           // Level the map is used on

  int Left;                            // Area of the level the map is used on
  int Top;                             // Area of the level the map is used on
  int Right;                           // Area of the level the map is used on
  int Bottom;                          // Area of the level the map is used on
};



_CLASSDEF(TAutoMapList)
class TAutoMapList
{
  public:

  int NumMaps;                         // How many maps are in the maplist array
  SAutoMapData MapList[MAX_AUTOMAPS];  // Array holding the descriptions of
                                       // each auto map
};


#endif
