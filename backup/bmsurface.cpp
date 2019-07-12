// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                bmsurface.cpp - EXILE Surface Object                   *
// *************************************************************************

#include "revenant.h"
#include "bitmap.h"
#include "bmsurface.h"

TBitmapSurface::TBitmapSurface()
{
}

TBitmapSurface::~TBitmapSurface()
{
    if (ownsbitmap && bitmap)
        delete bitmap;
}

void TBitmapSurface::Initialize(int bmwidth, int bmheight, int bmflags)
{
    bitmap = TBitmap::NewBitmap(bmwidth, bmheight, bmflags);
    width  = bitmap->width;
    height = bitmap->height;
    flags  = bmflags;
    stride = width;

    if (bitmap->flags & BM_8BIT)
    {
        bitsperpixel = 8;
    }

    else if (bitmap->flags & BM_15BIT)
    {
        bitsperpixel = 15;
    }
    
    else if (bitmap->flags & BM_16BIT)
    {
        bitsperpixel = 16;
    }
    
    else if (bitmap->flags & BM_24BIT)
    {
        bitsperpixel = 24;
    }
    
    else if (bitmap->flags & BM_32BIT)
    {
        bitsperpixel = 32;
    }
    
    ownsbitmap = TRUE;
}

void TBitmapSurface::Initialize(PTBitmap newbitmap)
{
    bitmap     = newbitmap;
    ownsbitmap = FALSE;

    width      = bitmap->width;
    height     = bitmap->height;
    clipwidth  = bitmap->width;
    clipheight = bitmap->height;
    flags      = bitmap->flags;
    stride     = width;

    if (bitmap->flags & BM_8BIT)
    {
        bitsperpixel = 8;
    }

    else if (bitmap->flags & BM_15BIT)
    {
        bitsperpixel = 15;
    }
    
    else if (bitmap->flags & BM_16BIT)
    {
        bitsperpixel = 16;
    }
    
    else if (bitmap->flags & BM_24BIT)
    {
        bitsperpixel = 24;
    }
    
    else if (bitmap->flags & BM_32BIT)
    {
        bitsperpixel = 32;
    }
}

BOOL TBitmapSurface::Rect(int x, int y, int w, int h, SColor &color)
{
    return TRUE;
}

BOOL TBitmapSurface::Rect(SRect r, SColor &color)
{
    return TRUE;
}

BOOL TBitmapSurface::Box(int x, int y, int w, int h, SColor &color)
{
    return TRUE;
}

BOOL TBitmapSurface::Line(int x1, int y1, int x2, int y2, SColor &color)
{
    return TRUE;
}

BOOL TBitmapSurface::Copy(PTBitmap bitmap)
{
    return TRUE;
}

BOOL TBitmapSurface::BlitPrimary(PSDrawParam dp, PTSurface surface, int ddflags)
{
    return TRUE;
}
