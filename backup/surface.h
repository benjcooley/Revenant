// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 Surface.h - 3D Surface Include File                   *
// *************************************************************************

#ifndef _SURFACE_H
#define _SURFACE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _GRAPHICS_H
#include "graphics.h"
#endif

#ifndef __DDRAW_INCLUDED__
struct IDirectDrawSurface;
typedef struct IDirectDrawSurface *LPDIRECTDRAWSURFACE;
struct _DDBLTFX;
typedef struct _DDBLTFX DDBLTFX;
typedef struct _DDBLTFX *LPDDBLTFX;
#endif

#define SURFACE_UNKNOWN 0
#define SURFACE_VIDEO   1
#define SURFACE_DISPLAY 2
#define SURFACE_BITMAP  3
#define SURFACE_MULTI   4
#define SURFACE_MOSAIC  5

_STRUCTDEF(SClipState)
struct SClipState
{
    int clipmode;
    int originx, originy;
    int clipx, clipy, clipwidth, clipheight;
};

_CLASSDEF(TSurface)
class TSurface
{
  protected:
    int width;           // Width of Surface
    int height;          // Height of Surface
    int bitsperpixel;    // Surface Color Depth
    int stride;          // Size of one horizontal screen line in pixels
    int originx;         // Current drawing origin
    int originy;
    int clipmode;        // Current clipmode
    int clipx;           // Current clipping rectangle
    int clipy;
    int clipwidth;
    int clipheight;
    DWORD keycolor;      // Surface transparent color
    LPVOID locked;       // True if surface locked

  public:
    DWORD flags;         // 'BM' flags for surface

    TSurface();
      // Initializes Surface
    virtual ~TSurface();
      // Releases Surface

    virtual int SurfaceType() { return 0; }
      // Returns type of surface this is

    virtual LPDIRECTDRAWSURFACE GetDDSurface() { return SURFACE_UNKNOWN; }
      // Returns LPDIRECTDRAWSURFACE pointer or Null if not Direct Draw Surface.

    virtual int BitsPerPixel() {return bitsperpixel;}
      // Returns current bits per pixel
    int  Stride() {return stride;}
      // Returns current stride
    int  Width() {return width;}
      // Returns current screen width
    int  Height() {return height;}
      // Returns current screen height
    int  KeyColor() {return keycolor;}
    void SetKeyColor(DWORD key){keycolor = key;}
      // Sets KeyColor for surface.

    virtual void Reset();
      // Resets originx, originy, cliprect and clipmode to screen defaults.
    virtual BOOL Lost() = 0;
      // Returns TRUE if the surface needs to be regenerated.
    
    virtual void *Lock() = 0;
      // Locks surface. Returns pointer to surface or NULL 
      // if buffer couldn't be locked. (must set locked ptr)
    virtual BOOL Unlock() = 0;
      // Unlocks surface.(must clear locked ptr)
    BOOL IsLocked() { return locked != NULL; }
    
    
    virtual PTSurface GetGraphicsBuffer() { return this; }
      // Returns ZBuffer surface for this surface (if it has one)
    virtual PTSurface GetZBuffer() { return NULL; }
      // Returns ZBuffer surface for this surface (if it has one)
    virtual PTSurface GetNormalBuffer() { return NULL; }
      // Returns the normal buffer for this surface (if it has one)

    virtual void SetOrigin(int x, int y) { originx = x; originy = y; }
      // Sets drawing origin
    void GetOrigin(int &x, int &y) { x = originx; y = originy; }
      // Gets drawing origin

    virtual void SetClipRect(int x, int y, int w, int h) {clipx = x; clipy = y; clipwidth = w; 
                     clipheight = h;}
      // Sets display clipping rectangle
    void SetClipRect(RSRect r) 
      { SetClipRect(r.left, r.top, r.right - r.left + 1, r.bottom - r.top + 1); }
      // Alternative cliping rect set function
    void ResetClipRect() { SetClipRect(0, 0, width, height); } 
      // Resets the clip rect to origin and width height of surface
    void GetClipRect(int &x, int &y, int &w, int &h)
        { x = clipx;  y = clipy; w = clipwidth; h = clipheight; }
      // Gets display clipping rectangle
    void GetClipRect(RSRect r)
        { GetClipRect(r.left, r.top, r.right, r.bottom); r.right += r.left - 1; r.bottom += r.top - 1; }    
      // Alternative clipping rect get function

    virtual void SetClipMode(int mode) { clipmode = mode; }
      // Sets clipping to normal of wrap around
    void GetClipMode(int &mode) { mode = clipmode; }
      // Gets clipping mode.

    void SaveClipState(SClipState &cs)
      { cs.clipmode = clipmode; cs.originx = originx; cs.originy = originy;
        cs.clipx = clipx; cs.clipy = clipy; cs.clipwidth = clipwidth; cs.clipheight = clipheight; }
      // Saves the current clipping state to the SClipState structure
    void RestoreClipState(SClipState &cs)
      { SetClipMode(cs.clipmode); SetOrigin(cs.originx, cs.originy); 
        SetClipRect(cs.clipx, cs.clipy, cs.clipwidth, cs.clipheight); }
      // Restores the current clipping state from the SClipState structure

    virtual DRAWCALLBACK GetDrawCallBack() { return NULL; }
      // Allows surfaces to specify a draw rectangle callback routine for
      // the low level Draw() function (supports adding update rectangles to
      // the Display)

  // These two functions are used to setup the clipping rect, origin, drawmode, and
  // do simple clipping for a draw param structure being passed to the ParamDraw() or
  // BlitHandler() functions.  A function can call these functions before a call to a
  // ParamDraw or ParamBlit function to get a copy of the DrawParam structure as it will be when
  // the lower level blits or draws are actually done.  The resulting DrawParam can then
  // be clipped with the Clip() function, or used directly with the ParamDraw or ParamBlit
  // functions.
    BOOL ParamDrawSetup(RSDrawParam dpv, PTBitmap bitmap);
      // Sets up a DrawParam structure for a call to DrawParam().  Returns FALSE if fails
    BOOL ParamBlitSetup(RSDrawParam tmpdp, PTSurface srcsurface, int ddflags, LPDDBLTFX fx);
      // Sets up a DrawParam structure for a call to BlitHandler.  Returns FALSE if fails

  // Low level Put and Blit which ONLY do primary surface (no Z or Normal Buffer)
    virtual BOOL BlitHandler(PSDrawParam dp, PTSurface surface, int ddflags = 0, LPDDBLTFX fx = NULL);
      // Blits from surface to surface. RECT sets size of blit. 
      // X & Y specifies dest. origin

// --------------------------------------------------------------------------------
// CORE SURFACE DRAW ROUTINES
//
// These routines are called by ALL surface drawing functions, and allow the 
// special surfaces such as TMultiSurface, and TMosaicSurface to redirect the
// drawing calls to one or more ordinary surface drawing calls.  It is therefore
// extremely important that NO drawing functions bypass these calls below.  All
// drawing functions MUST be called by the following functions below!!!!!!!

    virtual BOOL UseGetBlit() { return FALSE; }
      // TRUE when blitting from a complex surface like a TMosaicSurface.  This
      // causes the surface to use the complex source surfaces' GetBlit function 
      // instead of the ordinary destination surfaces Blit function when blitting
      // from a complex (mosaic) surface to an ordinary surface.

  // Put and blit functions which do primary, zbuffer, and normal buffer surface
    virtual BOOL ParamDraw(PSDrawParam dp, PTBitmap bitmap = NULL);
      // Copies specified bitmap to current bitmap
    virtual BOOL ParamBlit(PSDrawParam dp, PTSurface surface, int ddflags = 0, LPDDBLTFX fx = NULL);
      // Blits from surface to this surface. RECT sets size of blit. 
      // X & Y specifies dest origin.  If no hardware available, uses software
      // blitting
    virtual BOOL ParamGetBlit(PSDrawParam dp, PTSurface surface, int ddflags = 0, LPDDBLTFX fx = NULL);
      // Blits from this surface to surface. RECT sets size of blit. 
      // X & Y specifies dest origin.  If no hardware available, uses software
      // blitting.  Called by ParamBlit when blitting from a complex surface
      // to an ordinary surface.

// --------------------------------------------------------------------------------

  // Shortcut blit functions
    BOOL Blit(int x, int y, int w, int h, 
        DWORD drawmode = DM_USEDEFAULT, int ddflags = 0, LPDDBLTFX fx = NULL)
    { 
        SDrawParam dp;
        MakeDPNoSrc(dp, x, y, w, h, drawmode);
        return ParamBlit(&dp, NULL, ddflags, fx); 
    }

  // Shortcut blit functions
    BOOL Blit(int x, int y, PTSurface surface, int sx, int sy, int swidth, 
               int sheight, DWORD drawmode = DM_USEDEFAULT, int ddflags = 0, LPDDBLTFX fx = NULL)
    { 
        SDrawParam dp;
        MakeDP(dp, x, y, sx, sy, swidth, sheight, drawmode);
        return ParamBlit(&dp, surface, ddflags, fx); 
    }
    
    BOOL Blit(int x, int y, PTSurface surface, DWORD drawmode = DM_USEDEFAULT, 
               int ddflags = 0, LPDDBLTFX fx = NULL)
    { 
        return Blit(x, y, surface, 0, 0, surface->Width(), surface->Height(), drawmode, ddflags, fx);
    }

  // Shortcut blit functions
    BOOL GetBlit(int x, int y, PTSurface surface, int sx, int sy, int swidth, 
               int sheight, DWORD drawmode = DM_USEDEFAULT, int ddflags = 0, LPDDBLTFX fx = NULL)
    { 
        SDrawParam dp;
        MakeDP(dp, x, y, sx, sy, swidth, sheight, drawmode);
        return ParamGetBlit(&dp, surface, ddflags, fx); 
    }
    
    BOOL GetBlit(int x, int y, PTSurface surface, DWORD drawmode = DM_USEDEFAULT, 
               int ddflags = 0, LPDDBLTFX fx = NULL)
    { 
        return GetBlit(x, y, surface, 0, 0, surface->Width(), surface->Height(), drawmode, ddflags, fx);
    }

  // Shortcut put functions
    BOOL Put(int x, int y, PTBitmap bitmap, int sx, int sy, int swidth, int sheight, 
              DWORD drawmode = DM_USEDEFAULT)
    { 
        SDrawParam dp; 
        MakeDP(dp, x, y, sx, sy, swidth, sheight, drawmode);
        return ParamDraw(&dp, bitmap); 
    }
    
    BOOL Put(int x, int y, PTBitmap bitmap, DWORD drawmode = DM_USEDEFAULT, PSColor color = NULL);

    BOOL PutHue(int x, int y, PTBitmap bitmap, DWORD drawmode = DM_USEDEFAULT, int hue = 0);

    BOOL PutSV(int x, int y, PTBitmap bitmap, DWORD drawmode = DM_USEDEFAULT, int saturation = 100, int brightness = 100);

    BOOL PutDim(int x, int y, PTBitmap bitmap, DWORD drawmode = DM_USEDEFAULT, int dim = 0);

    BOOL ZPut(int x, int y, int z, PTBitmap bitmap, int sx, int sy, int swidth, 
              int sheight, DWORD drawmode = DM_USEDEFAULT)
    { 
        SDrawParam dp; 
        MakeDP(dp, x, y, sx, sy, swidth, sheight, drawmode);
        dp.zpos = (WORD)z;
        return ParamDraw(&dp, bitmap); 
    }
    
    BOOL ZPut(int x, int y, int z, PTBitmap bitmap, DWORD drawmode = DM_USEDEFAULT);

    BOOL ZPutDim(int x, int y, int z, PTBitmap bitmap, DWORD drawmode = DM_USEDEFAULT, int dim = 0, PSColor color = NULL);

    DWORD ZFind(int x, int y, int z, PTBitmap bitmap, DWORD drawmode = DM_USEDEFAULT);

    virtual BOOL Box(int dx, int dy, int dwidth, int dheight, 
        DWORD color = 0, WORD zpos = 0xFFFF, 
        WORD normal = 0x7F7F, DWORD drawmode = DM_USEDEFAULT);

    BOOL Clear(DWORD color = 0, WORD zpos = 0xFFFF, WORD normal = 0x7F7F, DWORD drawmode = DM_USEDEFAULT)
    {
        return Box(0, 0, width, height, color, zpos, normal, drawmode);
    }

    BOOL Line(int x1, int y1, int x2, int y2, 
        SColor &color, DWORD drawmode = DM_USEDEFAULT);

    BOOL Rect(int x, int y, int w, int h, 
        SColor &color, DWORD drawmode = DM_USEDEFAULT);

    int WriteText(char *text, int x = 0, int y = 0, 
        int numlines = 1, PTFont font = SystemFont, 
        PSColor color = NULL, DWORD drawmode = DM_USEDEFAULT, 
        int wrapwidth = -1, int startline = 0, 
        int justify = JUSTIFY_LEFT, int hue = -1, int linespace = 0);
        
    int WriteTextShadow(char *text, int x = 0, int y = 0, 
        int numlines = 1, PTFont font = SystemFont, 
        PSColor color = NULL, DWORD drawmode = DM_USEDEFAULT, 
        int wrapwidth = -1, int startline = 0, 
        int justify = JUSTIFY_LEFT, int hue = -1, int linespace = 0);


    // *************************
    // ** NOTE TO EVERYBODY!! **
    // *************************

    // DON'T PUT GAME SPECIFIC DRAW ROUTINES IN THIS SURFACE HEADER.....

    // I set up the draw system so that low level draw routines can be passed in the 
    // draw param structure in the 'func' member.  This means that you shouldn't
    // glom new game specific functions like ZFind in here, instead make a class or
    // global function in your own module which sets up a drawparam structure and
    // calls the ParamDraw() function.  This will make it easier to keep the low
    // level system somewhat clean.

    // o Low level blit functions and general purpose drawing functions should go in 
    //   graphics.cpp
    //
    // o Surface versions of these general purpose functions should go here.
    //
    // o Game specific low level surface or bitmap functions should go in the module
    //   they're most closely connected with, and should have their own functions which
    //   use the 'func' member and the ParamDraw() function.
};

#endif
