// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            ddsurface.h - Direct Draw Surface Include File             *
// *************************************************************************

#ifndef _DDSURFACE_H
#define _DDSURFACE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SURFACE_H
#include "surface.h"
#endif

#define VSURF_VIDEOMEM     1       // Attempts to grab video, if non available, uses system
#define VSURF_SYSTEMMEM    2       // Grabs system memory
#define VSURF_VIDEOONLY    4       // Forces system to only grab video memory or exit with error
#define VSURF_ZBUFFER      8       // This surface will be a zbuffer

_CLASSDEF(TDDSurface)
class TDDSurface : public TSurface
{
  protected:
    BOOL                lost;          // Indicates if a surface was lost
    LPDIRECTDRAWSURFACE surface;       // DirectDraw surface pointer
    BOOL                ownssurface;   // Determines if surface is released
    int                 vsflags;       // VSURF flags for surface (SYSTEMMEM or VIDEOMEM)

  public:
  // Constructor
    TDDSurface();
      // No initialize - must call Initialize() and Close() 
    TDDSurface(int width, int height, int flags = VSURF_VIDEOMEM, int stride = 0)
        { surface = NULL; Initialize(width, height, flags, stride); ownssurface = TRUE;}
      // Initializes Video Surface
    TDDSurface(PTBitmap bitmap, int intensity, int flags = VSURF_VIDEOMEM)
        { surface = NULL; Initialize(bitmap, intensity, flags);  ownssurface = TRUE;}
      // Initializes Video Surface
    TDDSurface(LPDIRECTDRAWSURFACE ddsurface)
        { surface = NULL; Initialize(ddsurface);  ownssurface = FALSE;}
      // Initializes Video Surface from DirectDraw surface.
    virtual ~TDDSurface();
      // Closes Video Surface.

    virtual int SurfaceType() { return SURFACE_VIDEO; }
      // Returns type of surface this is

    virtual LPDIRECTDRAWSURFACE GetDDSurface() { return surface; }
      // Returns LPDIRECTDRAWSURFACE pointer or Null if not Direct Draw Surface.

  // Initialize functions
    void Initialize(int width, int height, BOOL usevideomem = TRUE, int stride = 0);
      // Initializes Video Surface
    void Initialize(PTBitmap bitmap, int intensity, BOOL usevideomem = TRUE);
      // Initializes Video Surface
    void Initialize(LPDIRECTDRAWSURFACE ddsurface);
      // Initializes Video Surface from DirectDraw surface.

    void Close();
      // Close video surface

    virtual BOOL Lost() {return lost;}
      // Returns TRUE if the surface needs to be regenerated.

    virtual BOOL Restore();
      // Checks and Restores Surface if needed

    int VSFlags() { return vsflags; }
      // Returns the direct draw video surface specific flags
    
    virtual void *Lock();
      // Locks surface. Returns pointer to surface or NULL 
      // if buffer couldn't be locked.
    virtual BOOL Unlock();
      // Unlocks surface.

  // Low level Put and Blit which ONLY do primary surface (no Z or Normal Buffer)
    virtual BOOL BlitHandler(PSDrawParam dp, PTSurface surface, int ddflags = 0, LPDDBLTFX fx = NULL);
      // Blits from surface to surface. RECT sets size of blit. 
      // X & Y specifies dest. origin

    virtual BOOL Box(int dx, int dy, int dwidth, int dheight, 
        DWORD color = 0, WORD zpos = 0xFFFF, 
        WORD normal = 0x7F7F, DWORD drawmode = DM_USEDEFAULT);
      // Check caps structure and do hardware clear if available..
};

#endif
