// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               bmsurface.h - bitmap surface include file               *
// *************************************************************************

#ifndef _BMSURFACE_H
#define _BMSURFACE_H

#include "surface.h"
#include "bitmap.h"

#ifndef _REVENANT_H
#include "revenant.h"
#endif

_CLASSDEF(TBitmapSurface)

class TBitmapSurface : public TSurface
{
  private:
    PTBitmap bitmap;      // Bitmap pointer
    BOOL     ownsbitmap;  // Determines if bitmap is deleted on destruction.   

  public:
    TBitmapSurface();
    TBitmapSurface(int bmwidth, int bmheight, int bmflags)
        { Initialize(bmwidth, bmheight, bmflags); }
    TBitmapSurface(PTBitmap bitmap)
        { Initialize(bitmap); }

    virtual int SurfaceType() { return SURFACE_BITMAP; }
      // Returns type of surface this is

      // Constructor for BitmapSurface.
    virtual ~TBitmapSurface();
      // Destructor for BitmapSurface.

    void Initialize(int width, int height, int bmflags);
      // Initializes bitmap surface using width and height. Sets variables.
    void Initialize(PTBitmap bitmap);
      // Initializes bitmap surface from an existing bitmap. Sets variables.

    DWORD GetSurface() { return NULL; }
      // Returns Direct Draw pointer or Null if not Direct Draw Surface.
    virtual void *Lock() { stride = width; return locked = (void *)bitmap->data16; }
      // Locks surface. Returns pointer to surface or NULL 
      // if buffer couldn't be locked.
    virtual BOOL Unlock() { locked = NULL; return TRUE;}
      // Unlocks surface.
    
    virtual BOOL Lost() {return FALSE;}
      // Returns TRUE if the surface needs to be regenerated. Never needed for bitmap.

    virtual BOOL Rect(int x, int y, int w, int h, SColor &color);
      // Draws a unfilled box in specified color
    virtual BOOL Rect(SRect r, SColor &color);
      // Draws a unfilled box in specified color
    virtual BOOL Box(int x, int y, int w, int h, SColor &color);
      // Draws a box in specified color
    virtual BOOL Line(int x1, int y1, int x2, int y2, SColor &color);
      // Draws a line in specified color
    virtual BOOL Copy(PTBitmap bitmap);
      // Copies specified bitmap to current Direct Draw Surface

    virtual BOOL BlitPrimary(PSDrawParam dp, PTSurface surface, int ddflags = 0);
      // Blits from surface to surface. RECT sets size of blit. 
      // X & Y specifies dest. origin

    PTBitmap GetBitmap() { return bitmap; }
      // Allows access to bitmap

};

#endif
