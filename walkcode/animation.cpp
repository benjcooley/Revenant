// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            animation.cpp - EXILE Animation Objects File               *
// *************************************************************************

#include "revenant.h"
#include "display.h"
#include "bitmap.h"
#include "graphics.h"
#include "resource.h"
#include "animation.h"

TAnimation::TAnimation()
{
}

TAnimation::~TAnimation()
{
}

PTAnimation TAnimation::Load(int resource)
{
    return (PTAnimation)LoadResource("animation", resource);
}    

BOOL TAnimation::Decompress(PSDrawBlock db, int frame, BOOL Transparent)
{
    return TRUE;
}

void TAnimation::Put(PTSurface surface, void *decbuf, int frame, PSDrawParam dp)
{
    SDrawBlock  db;
    SDrawParam  dp2 = *dp;

    if (surface == NULL || (decbuf == NULL && (flags & AF_INTERFRAME))) 
        return;

    if (frame > numframes) 
        return;

    PTBitmap framebmp = frames[frame].bitmap;
    surface->ParamDraw(dp, framebmp);
    return;
    
    if (flags & AF_INTERFRAME) 
    {

        framebmp      = (PTBitmap)(frames[frame].bitmap.ptr());

        db.sbufwidth  = db.dbufwidth  = framebmp->width;
        db.sbufheight = db.dbufheight = framebmp->height;
        db.sstride    = db.dstride    = framebmp->width;

        db.source     = &framebmp->data16;
        db.dest       = decbuf;

        db.szbuffer   = (WORD *)framebmp->zbuffer.ptr();
        db.snormals   = (WORD *)framebmp->normal.ptr();

        db.dzbuffer   = db.dnormals = NULL;
        
        db.palette    = (WORD *)framebmp->palette.ptr();
        db.alpha      = (BYTE *)framebmp->alpha.ptr();
        db.alias      = (BYTE *)framebmp->alias.ptr();

        db.keycolor   = framebmp->keycolor;

        dp2.sx        = 0;  
        dp2.sy        = 0;

        dp2.dx       += frames[frame].x;    
        dp2.dy       += frames[frame].y;

        dp2.swidth    = framebmp->width;
        dp2.sheight   = framebmp->height;

        dp2.drawmode  = drawmode | DM_NORESTORE;

        Draw(&db, &dp2);

        surface->GetOrigin(dp->originx, dp->originy);
        surface->GetClipRect(dp->clipx, dp->clipy, dp->clipwidth, dp->clipheight);

        db.source     = decbuf;
        db.dest       = surface->Lock();
        surface->Unlock();

        db.dbufwidth  = surface->Width();
        db.dbufheight = surface->Height();
        db.dstride    = surface->Stride();
        Draw(&db, dp);
    }
}

void TAnimation::Stretch(PTSurface surface, PTSurface decbuf, int x, int y, int size, 
                         int frame)
{
}

int TAnimation::NumFrames()
{
    int n = numframes;

//  if (flags & AF_HALFSPEED) // Not supported
//      n <<= 1;
//  else if (flags & AF_THIRDSPEED)
//      n *= 3;
//  else if (flags & AF_QUARTERSPEED)
//      n <<= 2;

    return n;
}

PTBitmap TAnimation::GetFrame(int frame)
{
//  if (flags & AF_HALFSPEED)       // Not supported
//      frame >>= 1;
//  else if (flags & AF_THIRDSPEED)
//      frame /= 3;
//  else if (flags & AF_QUARTERSPEED)
//      frame >>= 2;

    if (frame >= numframes)
        return NULL;

    return frames[frame].bitmap;
}

void TAnimation::Move(int &x, int &y, int frame, int orientation)
{
}

void TAnimation::SizeMove(int &x, int &y, int size, int frame, int orientation)
{
}

