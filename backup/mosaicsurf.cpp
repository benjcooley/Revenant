// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              mosaicsurf.cpp - 3D Surface Include File                 *
// *************************************************************************

#include <windows.h>

#include "revenant.h"
#include "mosaicsurf.h"
#include "directdraw.h"
#include "ddsurface.h"
#include "multisurface.h"
#include "bitmap.h"
#include "bmsurface.h"

TMosaicSurface::TMosaicSurface()
{
    tilex = tiley = numtilex = numtiley = 0;
    flags = 0;
    tiles = NULL;
}

TMosaicSurface::~TMosaicSurface()
{
    Close();
}

BOOL TMosaicSurface::Initialize(
    int ntilex, int ntiley, int nnumtilex, int nnumtiley, DWORD ncreateflags)
{
    tilex = ntilex;
    tiley = ntiley;
    numtilex = nnumtilex;
    numtiley = nnumtiley;
    createflags = ncreateflags;

    if (createflags & MOSAICSURF_CLONEGRAPHICS)
        createflags &= ~(MOSAICSURF_SYSTEMMEM | 
            MOSAICSURF_VIDEOMEM | MOSAICSURF_VIDEOMEMONLY | MOSAICSURF_BMSURFACE |
            MOSAICSURF_8BIT | MOSAICSURF_16BIT | MOSAICSURF_24BIT | MOSAICSURF_32BIT);

    if (createflags & MOSAICSURF_CLONEZBUFFER)
        createflags &= ~(MOSAICSURF_ZBUFFER | MOSAICSURF_ZSYSTEMMEM | MOSAICSURF_ZVIDEOMEM | MOSAICSURF_ZVIDEOMEMONLY);

    if (createflags & MOSAICSURF_CLONENORMALS)
        createflags &= ~(MOSAICSURF_NORMALS | MOSAICSURF_NSYSTEMMEM | MOSAICSURF_NVIDEOMEM | MOSAICSURF_NVIDEOMEMONLY);

    if (!(createflags & (MOSAICSURF_BMSURFACE | MOSAICSURF_CLONEGRAPHICS))
       && !(createflags & (MOSAICSURF_SYSTEMMEM | MOSAICSURF_VIDEOMEM)))
        return FALSE;  // Must be one or other

    if ((createflags & MOSAICSURF_ZBUFFER) 
       && !(createflags & (MOSAICSURF_ZSYSTEMMEM | MOSAICSURF_ZVIDEOMEM)))
        return FALSE;  // Must be one or other

    if ((createflags & MOSAICSURF_NORMALS) 
       && !(createflags & (MOSAICSURF_NSYSTEMMEM | MOSAICSURF_NVIDEOMEM)))
        return FALSE;  // Must be one or other

    bitsperpixel = 16;
    if (createflags & MOSAICSURF_8BIT)
    {
        bitsperpixel = 8;
        createflags |= MOSAICSURF_BMSURFACE;
    }
    else if (createflags & MOSAICSURF_16BIT)
    {
        bitsperpixel = 16;
    }
    else if (createflags & MOSAICSURF_24BIT)
    {
        bitsperpixel = 24;
        createflags |= MOSAICSURF_BMSURFACE;
    }
    else if (createflags & MOSAICSURF_32BIT)
    {
        bitsperpixel = 32;
        createflags |= MOSAICSURF_BMSURFACE;
    }
    int bytesperpixel = bitsperpixel >> 3;

    int freevidmem = GetFreeVideoMem();

    if (createflags & MOSAICSURF_VIDEOMEM)
    {
        freevidmem -= tilex * tiley * numtilex * numtiley * bytesperpixel;

        if (freevidmem < TEXTURERESERVE)
        {
            if (createflags & MOSAICSURF_VIDEOMEMONLY)
                return FALSE;
            createflags &= ~MOSAICSURF_VIDEOMEM;
            createflags |= MOSAICSURF_SYSTEMMEM;
        }
    }

    if ((createflags & MOSAICSURF_ZBUFFER) && (createflags & MOSAICSURF_ZVIDEOMEM))
    {
        freevidmem -= tilex * tiley * numtilex * numtiley * 2;

        if (freevidmem < TEXTURERESERVE)
        {
            if (createflags & MOSAICSURF_ZVIDEOMEMONLY)
                return FALSE;
            createflags &= ~MOSAICSURF_ZVIDEOMEM;
            createflags |= MOSAICSURF_ZSYSTEMMEM;
        }
    }

    if ((createflags & MOSAICSURF_NORMALS) && (createflags & MOSAICSURF_NVIDEOMEM))
    {
        freevidmem -= tilex * tiley * numtilex * numtiley * 2;

        if (freevidmem < TEXTURERESERVE)
        {
            if (createflags & MOSAICSURF_NVIDEOMEMONLY)
                return FALSE;
            createflags &= ~MOSAICSURF_NVIDEOMEM;
            createflags |= MOSAICSURF_NSYSTEMMEM;
        }
    }
    
    int vflags;
    if (createflags & MOSAICSURF_VIDEOMEM)
        vflags = VSURF_VIDEOMEM;
    else
        vflags = VSURF_SYSTEMMEM;

    int zflags;
    if (createflags & MOSAICSURF_ZVIDEOMEM)
        zflags = VSURF_VIDEOMEM;
    else
        zflags = VSURF_SYSTEMMEM;

    int nflags;
    if (createflags & MOSAICSURF_NVIDEOMEM)
        nflags = VSURF_VIDEOMEM;
    else
        nflags = VSURF_SYSTEMMEM;

    int bmbits;
    if (createflags & MOSAICSURF_8BIT)
        bmbits = BM_8BIT;
    else if (createflags & MOSAICSURF_16BIT)
        bmbits = BM_16BIT;
    else if (createflags & MOSAICSURF_24BIT)
        bmbits = BM_24BIT;
    else if (createflags & MOSAICSURF_32BIT)
        bmbits = BM_32BIT;
    else
        bmbits = BM_16BIT;

    tiles = new PTSurface[numtilex * numtiley];

    PTSurface vsurf, zsurf, nsurf;
    TSurface **surf = tiles;

    int x, y, offx, offy;
    for (y = 0, offy = 0; y < numtiley; y++, offy += tiley)
    {
      for (x = 0, offx = 0; x < numtilex; x++, surf++, offx += tiley)
      {
        vsurf = zsurf = nsurf = NULL;

        if (!(createflags & MOSAICSURF_CLONEGRAPHICS))
        {
            if (createflags & MOSAICSURF_BMSURFACE)
                vsurf = new TBitmapSurface(tilex, tiley, bmbits);
            else
            {
                vsurf = new TDDSurface(tilex, tiley, vflags);
                if (vsurf->Stride() != tilex && NoWideBuffers)
                {
                    delete vsurf;
                    vflags = (vflags & (~(DWORD)VSURF_VIDEOMEM)) | VSURF_SYSTEMMEM;
                    vsurf = new TDDSurface(tilex, tiley, vflags);
                }
            }
        }
        if (createflags & MOSAICSURF_ZBUFFER)
        {
            zsurf = new TDDSurface(tilex, tiley, zflags | VSURF_ZBUFFER); // Voodoo doesn't like this
            if (zsurf->Stride() != tilex && NoWideBuffers)
            {
                delete zsurf;
                zflags = (zflags & (~(DWORD)VSURF_VIDEOMEM)) | VSURF_SYSTEMMEM;
                zsurf = new TDDSurface(tilex, tiley, zflags | VSURF_ZBUFFER); // Voodoo doesn't like this
            }
        }
//      if (createflags & MOSAICSURF_NORMALS)
//          nsurf = new TDDSurface(tilex, tiley, nflags, stride);

        *surf = new TMultiSurface(vsurf, zsurf, nsurf, TRUE);
      }
    }

    width        = tilex * numtilex;
    height       = tiley * numtiley;
    stride       = width;
    bitsperpixel = tiles[0]->BitsPerPixel();
    flags        = tiles[0]->flags;

    Reset();

    return TRUE;
}

// Initializes a cloned mosaic surface
// 
// A Cloned surface uses the same surface buffers as the original surface, but stores
// its own local origin and clipping rectangle.  This is very useful for... say.. 
// a surface used simultaneously by two different threads.

BOOL TMosaicSurface::Initialize(PTMosaicSurface clone, DWORD ncreateflags)
{
    Initialize(clone->tilex, clone->tiley, 
        clone->numtilex, clone->numtiley, ncreateflags | MOSAICSURF_ISCLONE);

    PTSurface vsurf, zsurf, nsurf;
    TMultiSurface **surf = (TMultiSurface **)tiles;

    int x, y, offx, offy;
    for (y = 0, offy = 0; y < numtiley; y++, offy += tiley)
    {
      for (x = 0, offx = 0; x < numtilex; x++, surf++, offx += tiley)
      {
    
        PTMultiSurface tile = clone->GetTile(x, y);
        vsurf = zsurf = nsurf = NULL;

        if ((createflags & MOSAICSURF_CLONEGRAPHICS) && tile->GetGraphicsBuffer())
        {
            if (clone->createflags & MOSAICSURF_BMSURFACE)
                vsurf = new TBitmapSurface(
                    ((PTBitmapSurface)tile->GetGraphicsBuffer())->GetBitmap());
            else
                vsurf = new TDDSurface(tile->GetGraphicsBuffer()->GetDDSurface());
            (*surf)->SetGraphicsBuffer(vsurf);
        }
        if ((createflags & MOSAICSURF_CLONEZBUFFER) && tile->GetZBuffer())
        {
            zsurf = new TDDSurface(tile->GetZBuffer()->GetDDSurface());
            (*surf)->SetZBuffer(zsurf);
        }
        if ((createflags & MOSAICSURF_CLONENORMALS) && tile->GetNormalBuffer())
        {
            nsurf = new TDDSurface(tile->GetNormalBuffer()->GetDDSurface());
            (*surf)->SetNormalBuffer(nsurf);
        }

      }
    }

    width        = tilex * numtilex;
    height       = tiley * numtiley;
    stride       = width;
    bitsperpixel = tiles[0]->BitsPerPixel();
    flags        = tiles[0]->flags;

    Reset();

    return TRUE;
}

void TMosaicSurface::Close()
{
    if (tiles == NULL)
        return;

    TSurface **surf = tiles;
    for (int y = 0; y < numtiley; y++)
    {
      for (int x = 0; x < numtilex; x++, surf++)
      {
        delete (*surf);
      }
    }

    delete tiles;
    tiles = NULL;
}

void TMosaicSurface::SetOrigin(int x, int y)
{
    TSurface::SetOrigin(x, y);

    TSurface **surf = tiles;
    int tx, ty, offx, offy;
    for (ty = 0, offy = 0; ty < numtiley; ty++, offy += tiley)
    {
      for (tx = 0, offx = 0; tx < numtilex; tx++, surf++, offx += tilex)
      {
        (*surf)->SetOrigin(x - offx, y - offy);
      }
    }
}

void TMosaicSurface::SetClipRect(int x, int y, int w, int h)
{
    TSurface::SetClipRect(x, y, w, h);

    TSurface **surf = tiles;
    int tx, ty, offx, offy;
    for (ty = 0, offy = 0; ty < numtiley; ty++, offy += tiley)
    {
      for (tx = 0, offx = 0; tx < numtilex; tx++, surf++, offx += tilex)
      {
        (*surf)->SetClipRect(0, 0, width, height); // Use standard clip rect for subtiles
      }
    }
}

// NOTE: Mosaic surfaces don't support CLIP_WRAP
void TMosaicSurface::SetClipMode(int mode)
{
    TSurface::SetClipMode(mode);

    // Note: Tile surfaces are ALWAYS CLIP_EDGES, wrap clipping is implemented by this object
    // for tiles, as a wrapped blits can span multiple tiles
    TSurface **surf = tiles;
    for (int ty = 0; ty < numtiley; ty++)
    {
      for (int tx = 0; tx < numtilex; tx++, surf++)
      {
        (*surf)->SetClipMode(CLIP_EDGES);       // Use standard clip mode for subtiles
      }
    }
}

// Copies specified bitmap to current bitmap
BOOL TMosaicSurface::ParamDraw(PSDrawParam dp, PTBitmap bitmap)
{
    SDrawParam dpv = *dp;
    BOOL drew = FALSE;
    SDrawParam dparray[4];
    SDrawBlock db;
    int numrects;

    if (!ParamDrawSetup(dpv, bitmap))
        return FALSE;

    memset(&db, 0, sizeof(SDrawBlock));

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = stride;

    if (bitmap)
    {
        db.sbufwidth  = bitmap->width;
        db.sbufheight = bitmap->height;
        db.sstride    = bitmap->width;
    }
    else
    {
        db.sbufwidth = db.sbufheight = db.sstride = 0;
    }

    if (!Clip(&db, &dpv, dparray, numrects))
        return FALSE;

    for (int cliploop = 0; cliploop < numrects; cliploop++)
    {
        RSDrawParam dpa = dparray[cliploop];
                    
        dpa.drawmode &= ~(DWORD)(DM_WRAPCLIP | DM_WRAPCLIPSRC);

        TSurface **surf = tiles;
        for (int ty = 0; ty < numtiley; ty++)
        {
          for (int tx = 0; tx < numtilex; tx++, surf++)
          {
            drew |= (*surf)->ParamDraw(&dpa, bitmap);
          }
        }
    }

    return drew;
}

// Blits from surface to surface. RECT sets size of blit. 
// X & Y specifies dest. origin
BOOL TMosaicSurface::ParamBlit(PSDrawParam dp, PTSurface surface, int ddflags, LPDDBLTFX fx)
{
    SDrawParam dpv = *dp;
    BOOL drew = FALSE;
    SDrawParam dparray[4];
    SDrawBlock db;
    int numrects;

  // Allow special case of blitting from one mosaic surface with identical size and tile
  // layout to another!!
    PTMosaicSurface mosaicsrc = NULL;
    if (surface && surface->SurfaceType() == SURFACE_MOSAIC)
    {
        mosaicsrc = (PTMosaicSurface)surface;
        if (mosaicsrc->tilex != tilex || mosaicsrc->tiley != tiley ||
            mosaicsrc->numtilex != numtilex || mosaicsrc->numtiley != numtiley)
                FatalError("Attempt to blit between non-identical mosaic surfaces");
    }

    if (!ParamBlitSetup(dpv, surface, ddflags, fx))
        return FALSE;

    memset(&db, 0, sizeof(SDrawBlock));

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = stride;

    if (surface)
    {
        db.sbufwidth  = surface->Width();
        db.sbufheight = surface->Height();
        db.sstride    = surface->Stride();
    }
    else
    {
        db.sbufwidth = db.sbufheight = db.sstride = 0;
    }

    if (!Clip(&db, &dpv, dparray, numrects))
        return FALSE;

    int soriginx, soriginy;
    surface->GetOrigin(soriginx, soriginy);

    PTSurface src = surface;
    for (int cliploop = 0; cliploop < numrects; cliploop++)
    {
        RSDrawParam dpa = dparray[cliploop];
        
      // Re-add originx and originy since Clip ParamBlitSetup will subtract these out               
        dpa.sx -= soriginx;
        dpa.sy -= soriginy;

        dpa.drawmode &= ~(DWORD)(DM_WRAPCLIP | DM_WRAPCLIPSRC);

        TSurface **surf = tiles;
        for (int ty = 0; ty < numtiley; ty++)
        {
          for (int tx = 0; tx < numtilex; tx++, surf++)
          {
            if (mosaicsrc)
                src = mosaicsrc->GetTile(tx, ty);

            drew |= (*surf)->ParamBlit(&dpa, src, ddflags, fx);
          }
        }
    }

    return drew;
}

// Blits from surface to surface. RECT sets size of blit. 
// X & Y specifies dest. origin
BOOL TMosaicSurface::ParamGetBlit(PSDrawParam dp, PTSurface surface, int ddflags, LPDDBLTFX fx)
{
    SDrawParam dpv = *dp;
    BOOL drew = FALSE;
    SDrawParam dparray[4];
    SDrawBlock db;
    int numrects;

  // Get blits always have two surfaces
    if (!surface)
        return FALSE;

  // Allow special case of blitting from one mosaic surface with identical size and tile
  // layout to another!!
    PTMosaicSurface mosaicdest = NULL;
    if (surface->UseGetBlit())
    {
        BOOL err = FALSE;
        if (surface->SurfaceType() != SURFACE_MOSAIC)
            err = TRUE;
        if (!err)
        {
            mosaicdest = (PTMosaicSurface)surface;
            if (mosaicdest->tilex != tilex || mosaicdest->tiley != tiley ||
                mosaicdest->numtilex != numtilex || mosaicdest->numtiley != numtiley)
                err = TRUE;
        }
        if (err)
            FatalError("Attempt to blit between non-identical mosaic surfaces");
    }

    if (!surface->ParamBlitSetup(dpv, this, ddflags, fx))
        return FALSE;

    memset(&db, 0, sizeof(SDrawBlock));

    db.dbufwidth  = surface->Width();
    db.dbufheight = surface->Height();
    db.dstride    = surface->Stride();

    db.sbufwidth  = width;
    db.sbufheight = height;
    db.sstride    = stride;

    if (!Clip(&db, &dpv, dparray, numrects))
        return FALSE;

    PTSurface dest = surface;
    for (int cliploop = 0; cliploop < numrects; cliploop++)
    {
        RSDrawParam dpa = dparray[cliploop];

        dpa.drawmode &= ~(DWORD)(DM_WRAPCLIP | DM_WRAPCLIPSRC);
                    
        TSurface **surf = tiles;
        for (int ty = 0; ty < numtiley; ty++)
        {
          for (int tx = 0; tx < numtilex; tx++, surf++)
          {
            if (mosaicdest)
                dest = mosaicdest->GetTile(tx, ty);

            drew |= dest->ParamBlit(&dpa, *surf, ddflags, fx);
          }
        }
    }

    return drew;
}

