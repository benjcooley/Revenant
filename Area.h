// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     area.h - Game Area Manager                        *
// *************************************************************************

#ifndef _AREA_H
#define _AREA_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _PARSE_H
#include "parse.h"
#endif

#ifndef _GRAPHICS_H
#include "graphics.h"
#endif

typedef TSizableArray<SRect, 4, 4> TRectArray;

#define MAXPLAYLISTSIZE 32

// If these flags are set, the area will perform the following functions
#define AREA_PLAYERIN           (1<<1)  // Player is currently in this area (Pulse() and Animate())
#define AREA_SETAMBIENT         (1<<2)  // This area should set the ambient light value
#define AREA_DONIGHT            (1<<3)  // This area should do day/night cycles
#define AREA_LOADSCRIPTS        (1<<4)  // This area should load a script file
#define AREA_PLAYCDMUSIC        (1<<5)  // This area should play cd music tracks
#define AREA_PLAYAMBIENT        (1<<6)  // This area should play ambient sound effects

_CLASSDEF(TArea)
class TArea
{
  private:
    char name[MAXNAMELEN];              // Name of area
    int level;                          // Level of area
    int flags;                          // Area flags
    TRectArray rects;                   // Array of rects which define the area
    char scriptfile[FILENAMELEN];       // Name of script file to load when in area 
    int amblight, nightamblight;        // Ambient lighting value
    SColor ambcolor, nightambcolor;     // Ambient color
    int lastdaylight;                   // Last daylight value

  // Ambient sound stuff
    void InitAmbientSounds();
      // Initializes ambient sounds for area
    void CloseAmbientSounds();
      // Deinitializes ambient sounds for area
    void PlayAmbientSounds();
      // Plays ambient sound effects when music not playing

  // CD audio music playing
    int cdplaynum;                      // Current track to play
    int cdplaylistsize;                 // Size of current play list
    int cdplaylist[MAXPLAYLISTSIZE];    // Play list
    BOOL cdplayisrandom;                // Play random tracks from list?
    int cdplaypause;                    // Seconds to pause between tracks
    int cdplaystart;
    int cdplaywait;
    int cdplaylength;

  // CD music functions
    void InitCDMusic();
      // Initializes data needed to play CD tracks
    void CloseCDMusic();
      // Frees data needed to play CD tracks
    void PlayCDMusic();
      // Plays CD music tracks

  public:
    TArea();
      // Initializes and loads an area
    ~TArea();
      // Gets rid of area

    BOOL Load(char *aname, TToken &t);
      // Loads the area from the "AREA.DEF" file

    int GetFlags() { return flags; }
      // Returns area flags
        
    BOOL In(S3DPoint &pos, int lev);
      // Returns TRUE if player's position is in this area

    void GetCurrentAmbient(int &ambient, SColor &color);
      // Returns the current ambient values based on the time of day

    void Pulse();
      // Called by the game screen Pulse() function to update area stuff
    void Animate(BOOL draw);
      // Called by the game screen Animate() function to update area stuff

    void Enter();
      // Called when the player enters the area
    void Exit();
      // Called when the player exits the area
};
typedef TPointerArray<TArea, 16, 16> TAreaArray;

_CLASSDEF(TAreaManager)
class TAreaManager
{
  private:
    BOOL initialized;
    TAreaArray areas;

  public:
    BOOL Initialize();
      // Initialize the area manager
    void Close();
      // Closes the area manager
    int NumAreas() { return areas.NumItems(); }
      // Returns number of areas
    PTArea GetArea(int index) { return areas[index]; }
      // Returns the control entry for the given index
    PTArea InArea(S3DPoint &pos, int lev);
      // Returns the area that the point is in, or NULL if not in any area
    BOOL Load();
      // Loads all areas from the "AREA.DEF" file
    void Pulse();
      // Called by the game screen Pulse() function to update area stuff
    void Animate(BOOL draw);
      // Called by the game screen Animate() function to update area stuff

};

#endif
