// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             multisurface.cpp - 3D Surface Include File                *
// *************************************************************************

#include "revenant.h"
#include "multisurface.h"

TMultiSurface::TMultiSurface()
{
    graphics = zbuffer = normals = NULL;
    deletesurf = FALSE;
}

TMultiSurface::~TMultiSurface()
{
    Close();
}

void TMultiSurface::Initialize(PTSurface surface1, 
    PTSurface surface2, PTSurface surface3, BOOL delsurf)
{
    SetGraphicsBuffer(surface1);
    SetZBuffer(surface2);
    SetNormalBuffer(surface3);
    Reset();
    deletesurf = delsurf;
}

void TMultiSurface::SetGraphicsBuffer(PTSurface surface)
{
    graphics = surface;

    if (surface)
    {
        width        = surface->Width();
        height       = surface->Height();
        bitsperpixel = surface->BitsPerPixel();
        stride       = surface->Stride();
        flags        = surface->flags;

        Reset();
    }
    else
    {
        width        = 0;
        height       = 0;
        bitsperpixel = 0;
        stride       = 0;
        flags        = 0;
    }

}

void TMultiSurface::Reset()
{
    char *sizeerr = "Multisurface sizes don't match";
    if (graphics && zbuffer)
      if (graphics->Width() != zbuffer->Width() ||
          graphics->Height() != zbuffer->Height())
      FatalError(sizeerr);
    if (graphics && normals)
      if(graphics->Width() != normals->Width() ||
         graphics->Height() != normals->Height())
      FatalError(sizeerr);

    TSurface::Reset();
    if (graphics)
    {
        graphics->SetOrigin(0, 0);
        graphics->ResetClipRect();
        graphics->SetClipMode(CLIP_EDGES);
    }
    if (zbuffer)
    {
        zbuffer->SetOrigin(0, 0);
        zbuffer->ResetClipRect();
        zbuffer->SetClipMode(CLIP_EDGES);
    }
    if (normals)
    {
        normals->SetOrigin(0, 0);
        normals->ResetClipRect();
        normals->SetClipMode(CLIP_EDGES);
    }
}

void TMultiSurface::SetOrigin(int x, int y)
{
    TSurface::SetOrigin(x, y);
    if (graphics)
        graphics->SetOrigin(x, y);
    if (zbuffer)
        zbuffer->SetOrigin(x, y);
    if (normals)
        normals->SetOrigin(x, y);
}

void TMultiSurface::SetClipRect(int x, int y, int w, int h)
{
    TSurface::SetClipRect(x, y, w, h);
    if (graphics)
        graphics->SetClipRect(x, y, w, h);
    if (zbuffer)
        zbuffer->SetClipRect(x, y, w, h);
    if (normals)
        normals->SetClipRect(x, y, w, h);
}

void TMultiSurface::SetClipMode(int mode)
{
    TSurface::SetClipMode(mode);
    if (graphics)
        graphics->SetClipMode(mode);
    if (zbuffer)
        zbuffer->SetClipMode(mode);
    if (normals)
        normals->SetClipMode(mode);
}

void TMultiSurface::Close()
{
    if (deletesurf)
    {
        if (graphics)
            delete graphics;
        if (zbuffer)
            delete zbuffer;
        if (normals)
            delete normals;
        graphics = zbuffer = normals = NULL;
        deletesurf = FALSE;
    }
}

