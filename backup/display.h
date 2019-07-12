// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               display.h  - EXILE Display Include File                 *
// *************************************************************************

#ifndef _DISPLAY_H
#define _DISPLAY_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _GRAPHICS_H
#include "graphics.h"
#endif

#ifndef _SURFACE_H
#include "surface.h"
#endif

#ifndef _DDSURFACE_H
#include "ddsurface.h"
#endif

_STRUCTDEF(RestoreRect)
_CLASSDEF(TDisplay)

class TDisplay : public TDDSurface
{
  protected:
    int currentpage;            // Currently Displayed Front/Back Suface
    BOOL updateenabled;         // Whether restore system is enabled
    PTDDSurface Front;          // Screen Surface Structures.

  public:
    PTDDSurface Back;
    PTDDSurface ZBuffer;
    PTDDSurface SaveZBuffer;    // Where the real zbuffer goes when we're using a secondary z

  public:
    TDisplay();
      // Creates Display Structures and Surfaces
    ~TDisplay();
      // Destructor

      // Clear ZBuffer Info:
      // -------------------
      // Several cards will NOT allow you to access the video ZBuffer directly (i.e. the
      // voodoo cards).  This means that the scrolling restore buffers which use the 
      // dirty rectangle system CAN'T update the display ZBuffer.  Luckily, Microsoft
      // has a specific ZBuffer updating system inside DirectX itself.. SetBackgroundDepth()
      // function and the Clear() function for Viewports.  What we do here is create an
      // extra ZBuffer (yes, it does take up a lot more memory), and quietly swap the zbuffer
      // in the Display class (here) with this backround z buffer.  Other than taking up
      // more video memory, this doesn't slow the system down since the dirty rectangle system
      // never draws to the video zbuffer except when drawing 3D objects, and the Viewport->Clear()
      // function is actually faster than our dirty rectangle routine anyway.

    void InitClearZBuffer();
      // Creates a clear zbuffer for use with the Viewport->Clear() function
      // and swaps the actual zbuffer with the clear buffer so the rest of the program
      // thinks the clear buffer is the actual zbuffer
    void CloseClearZBuffer();
      // Closes the clear zbuffer and puts the real video zbuffer back as the main zbuffer
    BOOL UsingClearZBuffer() { return SaveZBuffer != NULL; }
      // True if we're currently using a secondary zbuffer
    PTSurface GetRealZBuffer() { if (SaveZBuffer) return SaveZBuffer; else return ZBuffer; }
      // Returns the real display zbuffer (used by the Scene3D.RestoreZBuffer() function)

    virtual int SurfaceType() { return SURFACE_DISPLAY; }
      // Returns type of surface this is

    DWORD GetSurface() {return (DWORD)surface;}
      // Returns LPDIRECTDRAWSURFACE pointer or Null if not Direct Draw Surface.

    PTSurface BackBuffer() { return Back; }
      // Returns the back buffer surface
    PTSurface FrontBuffer() { return Front; }
      // Returns the front buffer surface

    BOOL Initialize(int dwidth, int dheight, int dbitsperpixel);
      // Initializes the Display Structures and Sets up the screen.
    virtual BOOL Close();
      // Shuts down and frees the display
    BOOL Restore();
      // Restores the display device after having been tabbed out of
  
    virtual PTSurface GetZBuffer() { return ZBuffer; }  // Returns NULL if display ZBuffer disabled
      // Returns ZBuffer surface for this surface (if it has one)
    virtual PTSurface GetNormalBuffer() { return NULL; }
      // Returns the normal buffer for this surface (if it has one)

    BOOL FlipPage(BOOL Wait = TRUE);
      // Flips front and back surfaces

    BOOL PutToScreen(int x, int y, int width, int height);
      // Copies specific area from back buffer to front buffer (or window) so it can
      // be seen immediately on the screen without having to wait for the FlipPage() 
      // function to be called.  This is useful for when you are drawing (Loading...)
      // indicators and can't call the update Tick() functions.
    BOOL PutToScreen(SRect &r)
      { return PutToScreen(r.x(), r.y(), r.w(), r.h()); }
      // Copies specific area from back buffer to front buffer (or window).  Calls the
      // above function.

  // Override blit functions to call dirty rectangle update routines
  // ---------------------------------------------------------------

  // Put and blit functions which do primary, zbuffer, and normal buffer surface
    virtual BOOL ParamDraw(PSDrawParam dp, PTBitmap bitmap = NULL);
      // Copies specified bitmap to current bitmap
    virtual BOOL ParamBlit(PSDrawParam dp, PTSurface surface, int flags = 0, LPDDBLTFX fx = NULL);
      // Blits from surface to this surface. RECT sets size of blit. 
      // X & Y specifies dest origin.  If no hardware available, uses software
      // blitting
    virtual BOOL ParamGetBlit(PSDrawParam dp, PTSurface surface, int flags = 0, LPDDBLTFX fx = NULL);
      // Blits from this surface to surface. RECT sets size of blit. 
      // X & Y specifies dest origin.  If no hardware available, uses software
      // blitting.  Called by ParamBlit when blitting from a complex surface
      // to an ordinary surface.

    // ***************************************************
    // Background System & Dirty Rectangle Update Routines
    // ***************************************************

    // A background area is a rectangle of the display for which there is a background buffer.
    // A background buffer stores the background imagery of the screen for that rectangle, and
    // is used to restore the background to the screen via dirty rectangle updates when
    // graphics are drawn to the screen (i.e. 3D or animation).  Areas of the screen without
    // background areas associated with them will not be restored when graphics are drawn to them.
    //
    // Background areas can be larger than the screen rectangle they update, and the origin for
    // the background buffer can be changed relative to the upper left corner of the screen rectangle, 
    // which means that scrolling screen backgrounds are easy to implement.  When a background 
    // area scrolls, an update rectangle is added for the whole screen rectangle.  In fact, this 
    // is how the main map scrolling system is implemented.  Also, when the origin of the background
    // area is such that the background buffer will not cover the entire screen buffer, the
    // dirty rectangle update routines will automatically wrap clip the updates so that the 
    // restore rectangles actually wrap around when the overlap the edge of the background 
    // buffer.  This allows the user to scroll the background buffer relative to the screen
    // rectangle, and constantly draw the unseen edges of the background to implement
    // a large virtual map (like we use in our game), without having to pay attention to 
    // any clipping or wrapping issues.

    // Note: Screen TPanes can be associated with a specific update area in order to simpify the
    // scrolling system.  When a screen pane is associated with a scrolling background area, all
    // the user need do is scroll the screen pane, and draw background graphics in the DrawBackground()
    // pane function.

    BOOL InitBackgroundSystem();
      // Initializes restore system.
    BOOL CloseBackgroundSystem();
      // Closes the background system
    void PauseBackgroundSystem() { updateenabled = FALSE; }
      // Temporarily toggle off background update system
    void UnPauseBackgroundSystem() { updateenabled = TRUE; }
      // Restart background update system
    int CreateBackgroundArea(int x, int y, int width, int height,
        BOOL createzbuf = FALSE, int vsflags = VSURF_SYSTEMMEM);
      // Uses surface as background area
    int UseBackgroundArea(int x, int y, int width, int height, PTSurface surface);
      // Creates a background area.   
    void FreeBackgroundArea(int index);
      // Frees the given background area
    BOOL ClearBackgroundAreas();
      // Clears all restore areas. Releases surfaces.
    BOOL RestoreBackgroundAreas();
      // Draws all restore areas to screen.
    BOOL DrawRestoreRect(int index, int x, int y, int width, int height,
        DWORD drawmode =  DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER);
      // Called by RestoreBackgroundAreas to draw a background rect to the display
    BOOL ScrollBackground(int index, int originx, int originy);
      // Sets origin of background Buffer

    #define UPDATE_THISFRAME        0x01        // Adds a dirty rectangle update rect for this frame
    #define UPDATE_NEXTFRAME        0x02        // Adds a dirty rectangle update rect for the next frame
    #define UPDATE_SCREENTOBUFFER   0x04        // Immediately copies the screen rect to the background
    #define UPDATE_BUFFERTOSCREEN   0x08        // Immediately copies the background rect to the screen
    #define UPDATE_NOMERGERECT      0x10        // Prevents dirty rectangle system from merging this rect

    #define UPDATE_RESTORE          (UPDATE_THISFRAME)
    #define UPDATE_BACKGROUND       (UPDATE_SCREENTOBUFFER | UPDATE_NEXTFRAME)

    void AddUpdateRect(int x, int y, int width, int height, int flags);
      // Adds restore rects for all screen buffers (uses screen coords)
    BOOL AddBackgroundUpdateRect(int index, int x, int y, int width, int height, int flags); 
      // Adds update rect to a given restore buffer (using buffer coordinates)

    virtual DRAWCALLBACK GetDrawCallBack();
      // Returns the AddUpdateRect() function as a draw callback to the Draw() routine

//  NOTE: This code is part of surface.cpp now (BEN)
//  virtual BOOL ZPut(int x, int y, int z, PTBitmap bitmap, DWORD drawmode = DM_USEDEFAULT);

  private:
    BOOL operator = (LPDIRECTDRAWSURFACE surface)
        { memcpy(surface, this, sizeof(this)); return TRUE; };
      // Redefines '=' to allow assigning strings.
  
    void AddSubRect(int index, int x1, int y1, int x2, int y2, int flags);
      // Calls AddUpdateRect with parameters of a smaller rect.
};

#endif 
