// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              MosaicSurf.h - Mosiac Surface Include File               *
// *************************************************************************

#ifndef _MOSAICSURF_H
#define _MOSAICSURF_H

#include "surface.h"

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#define MOSAICSURF_8BIT          (1<<1)
#define MOSAICSURF_16BIT         (1<<2)
#define MOSAICSURF_24BIT         (1<<3)
#define MOSAICSURF_32BIT         (1<<4)
#define MOSAICSURF_BMSURFACE     (1<<5)
#define MOSAICSURF_SYSTEMMEM     (1<<6)
#define MOSAICSURF_VIDEOMEM      (1<<7)
#define MOSAICSURF_VIDEOMEMONLY  (1<<8)
#define MOSAICSURF_ZBUFFER       (1<<9)
#define MOSAICSURF_ZSYSTEMMEM    (1<<10)
#define MOSAICSURF_ZVIDEOMEM     (1<<11)
#define MOSAICSURF_ZVIDEOMEMONLY (1<<12)
#define MOSAICSURF_NORMALS       (1<<13)
#define MOSAICSURF_NSYSTEMMEM    (1<<14)
#define MOSAICSURF_NVIDEOMEM     (1<<15)
#define MOSAICSURF_NVIDEOMEMONLY (1<<16)
#define MOSAICSURF_CLONEGRAPHICS (1<<17)
#define MOSAICSURF_CLONEZBUFFER  (1<<18)
#define MOSAICSURF_CLONENORMALS  (1<<19)
#define MOSAICSURF_ISCLONE       (1<<20)

#define MOSAICSURF_CLONEALL      (MOSAICSURF_CLONEGRAPHICS | MOSAICSURF_CLONEZBUFFER | MOSAICSURF_CLONENORMALS)

_CLASSDEF(TMosaicSurface)

class TMosaicSurface : public TSurface
{
  protected:
    DWORD createflags;      // Creation flags
    TSurface **tiles;       // Tile array
    int tilex, tiley;       // Size of each tile
    int numtilex, numtiley; // Number of tiles in x and y

  public:
    TMosaicSurface();
      // Initializes Mosaic surfaces
    TMosaicSurface(int ntilex, int ntiley, int nnumtilex, int nnumtiley, DWORD ncreateflags)
      { if (!Initialize(ntilex, ntiley, nnumtilex, nnumtiley, ncreateflags))
         FatalError("Couldn't initialize mosaic surface"); }
      // Initializes Mosaic surfaces
    TMosaicSurface(PTMosaicSurface clone, DWORD ncreateflags = MOSAICSURF_CLONEALL)
      { if (!Initialize(clone, ncreateflags))
         FatalError("Couldn't initialize mosaic surface"); }
      // Initializes a Cloned Mosaic surface
    virtual ~TMosaicSurface();
      // Destroys Mosaic surfaces.

    virtual int SurfaceType() { return SURFACE_MOSAIC; }
      // Returns type of surface this is

    BOOL Initialize(int ntilex, int ntiley, int nnumtilex, int nnumtiley, DWORD ncreateflags);
      // Causes the mosaicsurface to be initialized
    BOOL Initialize(PTMosaicSurface clone, DWORD ncreateflags = MOSAICSURF_CLONEALL);
      // Initializes a cloned mosaic surface
    void Close();
      // Closes the mosaic surface

    virtual PTSurface GetZBuffer() { return tiles[0]->GetZBuffer(); }
      // Returns ZBuffer surface for this surface (if it has one)
    virtual PTSurface GetNormalBuffer() { return tiles[0]->GetNormalBuffer(); }
      // Returns the normal buffer for this surface (if it has one)

    virtual int BitsPerPixel(){return tiles[0]->BitsPerPixel();}
      // Returns current bits per pixel
    virtual LPDIRECTDRAWSURFACE GetDDSurface() { return tiles[0]->GetDDSurface(); }
      // Returns LPDIRECTDRAWSURFACE pointer or Null if not Direct Draw Surface.

    virtual BOOL Lost() { return tiles[0]->Lost(); }
      // Returns TRUE if the surface needs to be regenerated.
    
    virtual void *Lock() { return locked = tiles[0]->Lock(); }
      // Locks surface. Returns pointer to surface or NULL 
      // if buffer couldn't be locked.
    virtual BOOL Unlock() { locked = NULL; return tiles[0]->Unlock(); }
      // Unlocks surface.
    
    virtual void SetOrigin(int x, int y);
      // Sets drawing origin
    virtual void SetClipRect(int x, int y, int w, int h);
      // Sets display clipping rectangle
    virtual void SetClipMode(int mode);
      // Sets clipping to normal of wrap around

    PTMultiSurface GetTile(int x, int y)
      { return (PTMultiSurface)tiles[y * numtilex + x]; }
      // Gets the given tile

// --------------------------------------------------------------------------------
// CORE SURFACE DRAW ROUTINES
//
// These routines are called by ALL surface drawing functions, and allow the 
// special surfaces such as TMultiSurface, and TMosaicSurface to redirect the
// drawing calls to one or more ordinary surface drawing calls.  It is therefore
// extremely important that NO drawing functions bypass these calls below.  All
// drawing functions MUST be called by the following functions below!!!!!!!

    virtual BOOL UseGetBlit() { return TRUE; }
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

};
#endif
