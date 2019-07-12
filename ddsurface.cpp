// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                ddsurface.cpp - EXILE Surface Object                   *
// *************************************************************************

#include <windows.h>
#include <ddraw.h>

#include "revenant.h"
#include "directdraw.h"
#include "bitmap.h"
#include "display.h"
#include "ddsurface.h"
#include "graphics.h"

// Direct Draw Global variables

extern LPDIRECTDRAW DirectDraw;       // DirectDraw pointer
extern BOOL         BlitHardware;
extern DWORD        ZBufferBitDepth;

TDDSurface::TDDSurface()
{
    surface = NULL;
}

void TDDSurface::Initialize(int vidwidth, int vidheight, int createflags, int vidstride)
{
    if (surface != NULL)
        return;

  // Don't allow any real zbuffer surfaces if in voodoo mode
    if (NoBlitZBuffer)
        createflags &= ~VSURF_ZBUFFER;

  // Convert video memory buffers to system memory buffers if we are in software mode
    if ((createflags & VSURF_VIDEOMEM) && !UsingHardware)
    {
        createflags &= ~VSURF_VIDEOMEM;
        createflags |= VSURF_SYSTEMMEM;
    }

  // Make sure width is even multiple of 4
    vidwidth &= 0xfffe;

  // Make sure we don't use too much video memory
    if ((createflags & VSURF_VIDEOMEM) &&
        (GetFreeVideoMem() - (vidwidth * vidheight * 2) < TEXTURERESERVE))
    {
        if (createflags & VSURF_VIDEOONLY)
            Error("Not enough video memory for textures");
        createflags &= ~VSURF_VIDEOMEM;
        createflags |= VSURF_SYSTEMMEM;
    }

    HRESULT             DirectDrawReturn;
    DDSURFACEDESC       ddsd;
    LPDIRECTDRAWSURFACE Surface;

    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH /* | DDSD_PIXELFORMAT*/;
    if (createflags & VSURF_VIDEOMEM)
        ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY;  
    else if (createflags & VSURF_SYSTEMMEM)
        ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY; 
    else
        Error("Invalid TDDSurface initialize flag");

    if (createflags & VSURF_ZBUFFER)
    {
        ddsd.ddsCaps.dwCaps |= DDSCAPS_ZBUFFER;
        ddsd.dwFlags |= DDSD_ZBUFFERBITDEPTH;
        ddsd.dwZBufferBitDepth = ZBufferBitDepth;
    }
    else
    {
        ddsd.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
    }

    ddsd.dwHeight       = vidheight;
    ddsd.dwWidth        = vidwidth;
    if (vidstride != 0)
        ddsd.dwWidth = vidstride;

/*  ddsd.ddpfPixelFormat.dwFlags = DDPF_RGB;
    ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
    ddsd.ddpfPixelFormat.dwRBitMask = 0x7C00;
    ddsd.ddpfPixelFormat.dwGBitMask = 0x03E0;
    ddsd.ddpfPixelFormat.dwBBitMask = 0x001F;
    ddsd.ddpfPixelFormat.dwRGBAlphaBitMask = 0;
*/
    DirectDrawReturn = DirectDraw->CreateSurface(&ddsd, &Surface, NULL);

    if (DirectDrawReturn != DD_OK)
    {
        if ((createflags & VSURF_VIDEOMEM) && (createflags & VSURF_VIDEOONLY))
            Error("Unable to allocate video memory for surface");

        ddsd.dwSize         = sizeof(ddsd);
        ddsd.dwFlags        = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
        ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY; 
        ddsd.dwHeight       = vidheight;
        ddsd.dwWidth        = vidwidth;
        if (vidstride != 0)
            ddsd.dwWidth = vidstride;

        if (createflags & VSURF_ZBUFFER)
        {
            ddsd.ddsCaps.dwCaps |= DDSCAPS_ZBUFFER;
            ddsd.dwFlags |= DDSD_ZBUFFERBITDEPTH;
            ddsd.dwZBufferBitDepth = ZBufferBitDepth;
        }
        else
        {
            ddsd.ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN;
        }

        DirectDrawReturn = DirectDraw->CreateSurface(&ddsd, &Surface, NULL);

        if (DirectDrawReturn != DD_OK)
            Surface = NULL;
    }

    if (Surface == NULL)
        FatalError("Couldn't allocate video surface");

    Initialize(Surface);

    if (vidstride) // Adjust width/stride if stride was specified
    {
        width = vidwidth;
        stride = vidstride;
    }

    ownssurface = TRUE;
}

void TDDSurface::Initialize(PTBitmap bitmap, int intensity, BOOL usevideomem)
{
    if (surface != NULL)
        return;

    Initialize(bitmap->width, bitmap->height, usevideomem);

    flags = bitmap->flags;

    SDrawParam  dp;
    
    MakeDP(dp, 0, 0, 0, 0, bitmap->width, bitmap->height, 
        bitmap->drawmode | DM_NORESTORE);

    dp.intensity = intensity;

    ParamDraw(&dp, bitmap);
}

void TDDSurface::Initialize(LPDIRECTDRAWSURFACE ddsurface)
{
    DDSURFACEDESC ddsd;
    memset(&ddsd, 0, sizeof(DDSURFACEDESC));
    ddsd.dwSize = sizeof(DDSURFACEDESC);
    ddsurface->GetSurfaceDesc(&ddsd);

    width        = ddsd.dwWidth;
    height       = ddsd.dwHeight;
    bitsperpixel = ddsd.ddpfPixelFormat.dwRGBBitCount;
    stride       = ddsd.lPitch / (bitsperpixel / 8);
    originx      = 0;
    originy      = 0;
    clipmode     = CLIP_EDGES;
    clipx        = 0;
    clipy        = 0;
    clipwidth    = ddsd.dwWidth;
    clipheight   = ddsd.dwHeight;
    keycolor     = 0;
    if (bitsperpixel == 8)
        flags = BM_8BIT;
    else if (bitsperpixel == 15)
        flags = BM_15BIT;
    else if (bitsperpixel == 16)
        flags = BM_16BIT;
    else if (bitsperpixel == 24)
        flags = BM_24BIT;
    else if (bitsperpixel == 32)
        flags = BM_32BIT;
    lost         = FALSE;
    surface      = ddsurface;
    ownssurface  = FALSE;
    if (ddsd.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
        vsflags = VSURF_SYSTEMMEM;
    else if (ddsd.ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY)
        vsflags = VSURF_VIDEOMEM;
}

void TDDSurface::Close()
{
    if (ownssurface && surface != NULL) 
    {
        surface->Release();
        surface = NULL;     
    }
}

TDDSurface::~TDDSurface()
{
    Close();
}

// Restore Surface
BOOL TDDSurface::Restore()
{
    // Attempt to restore surface
    if (surface)
    {
        if (surface->IsLost() == DDERR_SURFACELOST)
        {
            if (surface->Restore() != DD_OK)
                return FALSE;
        }
    }

    return TRUE;
}

void *TDDSurface::Lock()
{
    if (!surface)
        return NULL;

    if (locked || !surface)
        return locked;

    // NOTE: !!! MAKE SURE FUNCTION DOES NOT EXIT UNTIL LeaveCriticalSection IS CALLED !!!
    BEGIN_CRITICAL();

    DDSURFACEDESC ddsd;
    HRESULT DirectDrawReturn;

    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    // Loop until an error occurs or the lock succeeds
    while (1)
    {
        DirectDrawReturn = surface->Lock(0, &ddsd, 0, 0);

        if (DirectDrawReturn == DD_OK)
        {
            switch(bitsperpixel)
            {
                case 8:
                    stride = ddsd.lPitch;
                    break;
                case 15:
                case 16:
                    stride = (ddsd.lPitch) >> 1;
                    break;
                case 24:
                    stride = (ddsd.lPitch) / 3;
                    break;
                case 32:
                    stride = (ddsd.lPitch) >> 2;
                    break;
                default:
                    stride = 0;
                    break;
            }

            locked = ddsd.lpSurface;
            break;
        }

        else if (DirectDrawReturn == DDERR_SURFACELOST)
        {
            if (!surface->Restore())
            {
                ddsd.lpSurface = NULL;
                break;
            }
        }

        else if (DirectDrawReturn != DDERR_WASSTILLDRAWING)
        {
            TRY_DD(DirectDrawReturn)
        }

        Sleep(1);   // Release time slice and try again next slice
    }

    if (UnlockImmediately)
        Unlock();

    END_CRITICAL();

    return ddsd.lpSurface;
}

BOOL TDDSurface::Unlock()
{
    if (!locked)
        return TRUE;

    HRESULT DirectDrawReturn;

    // NOTE: !!! MAKE SURE FUNCTION DOES NOT EXIT UNTIL LeaveCriticalSection IS CALLED !!!
    BEGIN_CRITICAL();

    if (!surface) return FALSE;
    // Loop until an error occurs or the unlock succeeds
    while (1)
    {        
        DirectDrawReturn = surface->Unlock(0);

        if (DirectDrawReturn == DD_OK || DirectDrawReturn == DDERR_NOTLOCKED)
        {
            locked = NULL;
            break;
        }

        else if (DirectDrawReturn == DDERR_SURFACELOST)
        {
            if (!(surface->Restore()))
                FatalError("Couldn't restore surface");
        }

        else if (DirectDrawReturn != DDERR_WASSTILLDRAWING) 
        {
            TRY_DD(DirectDrawReturn)
        }
        
        Sleep(1); // Release time slice and try again next slice
    }

    END_CRITICAL();

    return TRUE;
}

BOOL TDDSurface::BlitHandler(PSDrawParam dp, PTSurface srcsurface, int ddflags, LPDDBLTFX fx)
{
    HRESULT error;

    LPDIRECTDRAWSURFACE ddsrcsurf = NULL;
    if (srcsurface)
        ddsrcsurf = srcsurface->GetDDSurface();

  // Can't blit to the zbuffer..
    if (NoBlitZBuffer && 
      (this == Display->GetRealZBuffer() || srcsurface == Display->GetRealZBuffer()) )
        dp->drawmode |= DM_NOHARDWARE;

    if (!(dp->drawmode & DM_NOHARDWARE) && BlitHardware && (!srcsurface || ddsrcsurf) && !dp->func)
    {
        SDrawParam  tmpdp = *dp;
        SDrawBlock  tmpdb;

        if (!ParamBlitSetup(tmpdp, srcsurface, ddflags, fx))
            return FALSE;

        tmpdb.dstbitmapflags = flags;
        tmpdb.dbufwidth  = width;
        tmpdb.dbufheight = height;
        tmpdb.dstride = stride;

        if (srcsurface)
        {
            tmpdb.srcbitmapflags = srcsurface->flags;
            tmpdb.keycolor   = srcsurface->KeyColor();
            tmpdb.sbufwidth  = srcsurface->Width();
            tmpdb.sbufheight = srcsurface->Height();
            tmpdb.sstride = srcsurface->Stride();

          // Clip source
            if (!(tmpdp.drawmode & DM_WRAPCLIPSRC) &&
               (tmpdp.sx >= tmpdb.sbufwidth || tmpdp.sy >= tmpdb.sbufheight ||
                tmpdp.sx + tmpdp.swidth < 0 || tmpdp.sy + tmpdp.sheight < 0))
                    return FALSE;
        }
        else
        {   
            tmpdb.srcbitmapflags = 0;
            tmpdb.keycolor = 0;
            tmpdb.sbufwidth = tmpdb.sbufheight = tmpdb.sstride = 0;
        }

      // Get draw call back (for restoring)
        DRAWCALLBACK callback = tmpdp.callback;

        DDCOLORKEY DirectDrawColorKey;
    
        SDrawParam dparray[4];
    
        int ArrayIndex = 0;    
        dparray[0]     = tmpdp;

        if (tmpdp.drawmode & DM_NOCLIP)
            ArrayIndex = 1;
        else
            Clip(&tmpdb, &tmpdp, dparray, ArrayIndex);

        for (int loop = 0; loop < ArrayIndex; loop++)
        {
            SDrawParam dpm = dparray[loop];

            if (dpm.dwidth <= 0 || dpm.dheight <= 0)
                return FALSE;
            
            if (srcsurface && (dpm.swidth <= 0 || dpm.sheight <= 0))
                return FALSE;

            RECT drc = {
                 dpm.dx + dpm.originx,
                 dpm.dy + dpm.originy,
                 dpm.dx + dpm.originx + dpm.dwidth,
                 dpm.dy + dpm.originy + dpm.dheight };

            RECT src = {dpm.sx, dpm.sy, dpm.sx + dpm.swidth,
                        dpm.sy + dpm.sheight};

            DDBLTFX ddbltfx;
            memset(&ddbltfx, 0, sizeof(DDBLTFX));
            ddbltfx.dwSize = sizeof(DDBLTFX);
            if (!fx)
                fx = &ddbltfx;

            if ((dpm.drawmode & DM_TRANSPARENT) && ddsrcsurf)
            {
                DirectDrawColorKey.dwColorSpaceLowValue  = (tmpdb.keycolor & 0xffff);
                DirectDrawColorKey.dwColorSpaceHighValue = (tmpdb.keycolor & 0xffff);
                TRY_DD(ddsrcsurf->SetColorKey(DDCKEY_SRCBLT, &DirectDrawColorKey));
                ddflags = DDBLT_KEYSRC | DDBLT_ASYNC;
            }
            else
            {
                if (ddflags != DDBLT_WAIT)
                    ddflags |= DDBLT_ASYNC;
            }

            LPDIRECTDRAWSURFACE2 dstdd2, srcdd2;
            TRY_DD(surface->QueryInterface(IID_IDirectDrawSurface2, (LPVOID*)&dstdd2));

            if (ddsrcsurf)
            {
                TRY_DD(ddsrcsurf->QueryInterface(IID_IDirectDrawSurface2, (LPVOID*)&srcdd2));
            }
            else
                srcdd2 = NULL;

            BEGIN_CRITICAL(); // Never do two blits at once

            do
            {

            // Check if somebody else locked the surface, and if so, temporarily unlock it
//              BOOL dstlocked = IsLocked();
//              BOOL srclocked = FALSE;
//              if (srcsurface)
//                  srclocked = srcsurface->IsLocked();
//              if (dstlocked)
//                  Unlock();
//              if (srclocked)
//                  srcsurface->Unlock();

                if (dstdd2->IsLost() == DDERR_SURFACELOST)
                {
                    dstdd2->Restore(); // Attempt to restore, don't check for errors
                }

                if (srcdd2 && srcdd2->IsLost() == DDERR_SURFACELOST)
                {
                    srcdd2->Restore(); // Attempt to restore, don't check for errors
                }

                error = dstdd2->Blt(&drc, srcdd2, &src, ddflags, fx);
            
             // Now relock the surface 
//              if (dstlocked)
//                  Lock();
//              if (srclocked)
//                  srcsurface->Lock();

                if (error == DDERR_WASSTILLDRAWING || error == DDERR_SURFACEBUSY)
                {
                    END_CRITICAL();
                    Sleep(1); // Release time slice and try again next slice
                    BEGIN_CRITICAL();
                }

            } while (error == DDERR_WASSTILLDRAWING || error == DDERR_SURFACEBUSY);


            END_CRITICAL();

            if (error == DDERR_SURFACELOST)
                return FALSE; // Just bomb out if surface was lost

            if (error == DDERR_NOBLTHW)
            {
                BlitHardware = FALSE;
                break;
            }
            else if (error != DD_OK)
            {
//              char buf[80];
//              sprintf(buf, "UNSUPPORED: %d %d %d %d - %d %d %d %d - %d", 
//                  src.left, src.top, src.right, src.bottom, 
//                  drc.left, drc.top, drc.right, drc.bottom,
//                  ddflags);
//              _RPT0(_CRT_ASSERT, buf); 
                TRY_DD(error)
            }

            if (callback)
                callback(&tmpdb, &dpm);
        }
    }

    if ((dp->drawmode & DM_NOHARDWARE) ||   // If user chose no hardware
        !BlitHardware ||                    // or if there ain't no hardware 
        !(!srcsurface || ddsrcsurf) ||      // or one or both surfaces aren't DD surfaces
//      error != DD_OK ||                   // or there was an error using hardware blit
        dp->func)                           // or user chose to use his own blit function
    {
        return TSurface::BlitHandler(dp, srcsurface, ddflags, fx);
    }

    return TRUE;
}

BOOL TDDSurface::Box(int dx, int dy, int dwidth, int dheight, 
        DWORD color, WORD zpos, WORD normal, DWORD drawmode)
{
    if (drawmode == DM_USEDEFAULT)
        drawmode = DM_ZBUFFER | DM_NORMALS;

  // Can we do a hardware clear?
    if (!BlitHardware ||
        (!((DDHWCaps.dwCaps & DDCAPS_BLTCOLORFILL) && 
          (!(drawmode & DM_ZBUFFER) || DDHWCaps.dwCaps & DDCAPS_BLTDEPTHFILL))))
        return TSurface::Box(dx, dy, dwidth, dheight, color, zpos, normal, drawmode);

  // Hardware clear (doesn't seem to work for some reason)
    DWORD dm = (drawmode & ~(DM_ZBUFFER | DM_NORMALS)) | DM_FILL;

    DDBLTFX fx;
    memset(&fx, 0, sizeof(DDBLTFX));
    fx.dwSize = sizeof(DDBLTFX);
    
    if (!(drawmode & DM_NODRAW))
    {
        fx.dwFillColor = color;
        Blit(dx, dy, dwidth, dheight, dm, DDBLT_ASYNC | DDBLT_COLORFILL, &fx);
    }

    if ((drawmode & DM_ZBUFFER) && GetZBuffer())
    {
        fx.dwFillDepth = zpos & 0xFFFF;
        Blit(dx, dy, dwidth, dheight, 
            dm | DM_NODRAW | DM_ZBUFFER | DM_NORESTORE, DDBLT_ASYNC | DDBLT_DEPTHFILL, &fx);
    }

    if ((drawmode & DM_NORMALS) && GetNormalBuffer())
    {
        fx.dwFillColor = normal;
        Blit(dx, dy, dwidth, dheight, 
            dm | DM_NODRAW | DM_NORMALS | DM_NORESTORE, DDBLT_ASYNC | DDBLT_COLORFILL | DDBLT_DDFX, &fx);
    }

    return TRUE;
}


