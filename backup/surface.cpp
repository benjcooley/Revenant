// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   Surface.cpp - TSurface Object                       *
// *************************************************************************

#include "revenant.h"
#include "surface.h"
#include "font.h"
#include "display.h"

// If this is TRUE, surfaces are unlocked as soon as we have the pointer.  This is great for
// debugging, and seems to work fine on every video card except for the VooDoo and VooDoo 2
//
// If you notice graphics getting screwed up, try setting this to FALSE..  This should always
// be false if _DEBUG is not defined.

TSurface::TSurface()
{
    locked     = NULL;
    width      = height = stride = 0;
    clipx      = clipy  = 0;
    clipwidth  = width;
    clipheight = height;
    
    clipmode   = CLIP_EDGES; 
}

TSurface::~TSurface()
{
}

void TSurface::Reset()
{
    SetOrigin(0, 0);
    SetClipRect(0, 0, width, height);
    SetClipMode(CLIP_EDGES);
}

// This function sets up the ParamDraw DrawParam structure.  This function is also
// called in higher level surfaces before being called here, so MAKE SURE that it
// can be called multiple times without continuously adding values like registration
// offsets each time.

BOOL TSurface::ParamDrawSetup(RSDrawParam dpv, PTBitmap bitmap)
{
  // Set drawmode to bitmap drawmode if DM_USEDEFAULT
    if (dpv.drawmode == DM_USEDEFAULT)
    {
        if (bitmap != NULL)
            dpv.drawmode = bitmap->drawmode;
        else
            dpv.drawmode = DM_DEFAULT;
    }

  // If surface is wrap clipping and drawmode isn't, set drawmode
    if ((clipmode == CLIP_WRAP) &&
      !(dpv.drawmode & (DM_WRAPCLIP | DM_WRAPCLIPSRC | DM_NOWRAPCLIP)))
        dpv.drawmode |= DM_WRAPCLIP;

  // Adjust for registration point
    if (bitmap && (dpv.drawmode & DM_USEREG))
    {
        dpv.dx -= bitmap->regx;
        dpv.dy -= bitmap->regy;
        dpv.drawmode &= ~DM_USEREG; // Don't do USERGEG again
    }

  // Set up highlight color for selecting
    if (dpv.drawmode & DM_SELECTED)
    {
        SColor color = { 220, 10, 30 };
        dpv.color = TranslateColor(color);
    }

  // Set up cliping and origin
    dpv.originx     = originx;
    dpv.originy     = originy;

    dpv.clipx       = clipx;
    dpv.clipy       = clipy;
    dpv.clipwidth   = clipwidth;
    dpv.clipheight  = clipheight;

    dpv.callback    = GetDrawCallBack();

  // Do a quick clip check before we go to the trouble to lock the buffers
    int dx = dpv.dx + originx;
    int dy = dpv.dy + originy;
    if (dx + dpv.dwidth <= clipx || dy + dpv.dheight <= clipy ||
        dx >= clipx + clipwidth || dy >= clipy + clipheight)
            return FALSE;

    return TRUE;
}

// This function calls the low level graphics Draw function.. It takes care
// of wrap clipping, clipping, and decoding the DrawMode flags and DrawPrimitive
// commands.  This is THE central low level asm code drawing function for all
// graphics.  All low level graphics calls MUST go through this function or they
// WILL NOT WORK on all surface classes!

BOOL TSurface::ParamDraw(PSDrawParam dp, PTBitmap bitmap)
{
    SDrawParam dpv = *dp;  // Don't mess up the draw param struct passed to us

    if (!ParamDrawSetup(dpv, bitmap))
        return FALSE;

  // Setup drawblock and lock surfaces
    SDrawBlock  db;
    db.dstbitmapflags = flags;

  // If we have a bitmap.. use it
    if (bitmap)  // Get bitmap for blit
    {
        db.sbufwidth      = bitmap->width;
        db.sbufheight     = bitmap->height;
        db.sstride        = bitmap->width;
        db.keycolor       = bitmap->keycolor;

      // Return now if we don't draw anything
        if (dpv.sx >= db.sbufwidth || dpv.sy >= db.sbufheight ||
            dpv.sx + dpv.swidth < 0 || dpv.sy + dpv.sheight < 0)
                return FALSE;

        db.srcbitmapflags = bitmap->flags;
        db.source         = &bitmap->data16;
        db.szbuffer       = (WORD *)bitmap->zbuffer.ptr();
        db.szstride       = bitmap->width;

        if (!db.szbuffer)
            db.srcbitmapflags  &= ~BM_ZBUFFER;

        db.snormals       = (WORD *)bitmap->normal.ptr();

        if (!db.snormals)
            db.srcbitmapflags  &= ~BM_NORMALS;

        db.alpha          = (BYTE *)bitmap->alpha.ptr();

        if (!db.alpha)
            db.srcbitmapflags  &= ~BM_ALPHA;

        db.palette        = (WORD *)bitmap->palette.ptr();
        db.alias          = (BYTE *)bitmap->alias.ptr();

        if (!db.alias)
            db.srcbitmapflags  &= ~BM_ALIAS;
    }

    else 
    {
        db.srcbitmapflags = 0;
        db.source         = NULL;
        db.szbuffer       = NULL; 
        db.snormals       = NULL; 
        db.palette        = NULL;
        db.alpha          = NULL;
        db.alias          = NULL;
        db.sbufwidth      = 0;
        db.sbufheight     = 0;
        db.sstride        = 0;
        db.szstride       = 0;
        db.keycolor       = 0;
    }

  // Lock graphics buffer!!
    db.dest = Lock();
    if (db.dest == NULL) 
        return FALSE;  // Duh!
    if (UnlockImmediately)
        Unlock();

  // Lock z buffer
    if (GetZBuffer() == NULL || !(dp->drawmode & (DM_ZBUFFER | DM_ZSTATIC)) || 
        (bitmap && !(bitmap->flags & BM_ZBUFFER) && !(dp->drawmode & DM_ZSTATIC)))
    {
        db.dzbuffer = NULL;
        db.dzstride = 0;
        db.dstbitmapflags  &= ~BM_ZBUFFER;
    }
    else
    {
        // NoVidZBufLock - Means you can't lock the display video surface and the 
        //                 display zbuffer at the same time (voodoo cards can't do this).
        // UseClearZBuffer - Means we are using a sneaky alternate fake ZBuffer for
        //                 the Display object.  An app can get the real zbuffer surface
        //                 in this mode by calling GetRealZBuffer() for the display.
        //                 We use this sneaky buffer so we can get around the simultaneous
        //                 access problems below, and to eliminate any read/write/lock
        //                 problems a non cooperative zbuffer (i.e. voodoo) may have.
        //
        // We can get a simultaneous zbuffer/video buffer if NoVidZBufLock is FALSE,
        // or if UseClearZBuffer is TRUE, or we're not drawing to the display.

        if ((void *)this == (void *)Display && NoVidZBufLock && !UseClearZBuffer)
            db.dzbuffer = NULL;
        else
            db.dzbuffer  = (WORD *)(GetZBuffer()->Lock());

        if (db.dzbuffer == db.dest) // Video card can only lock one buffer at a time (voodoo)
            db.dzbuffer = NULL;     // If can't lock it, don't do zbuffer draw

        if (UnlockImmediately)
            GetZBuffer()->Unlock();

        if (db.dzbuffer)
        {
            db.dstbitmapflags  |= BM_ZBUFFER;
            db.dzstride = GetZBuffer()->Stride();
        }
        else
        {
            db.dstbitmapflags  &= ~BM_ZBUFFER;
            db.dzstride = 0;
        }
    }

  // Lock normal buffer
/*  if (GetNormalBuffer() == NULL || !(dp->drawmode & DM_NORMALS) ||
        (bitmap && !(bitmap->flags & BM_NORMALS)))
    {
        db.dnormals = NULL;
        db.dstbitmapflags  &= ~BM_NORMALS;
    }
            
    else
    {
        db.dnormals  = (WORD *)GetNormalBuffer()->Lock();
        GetNormalBuffer()->Unlock();
        if (db.dnormals)
            db.dstbitmapflags  |= BM_NORMALS;   

        else
            db.dstbitmapflags  &= ~BM_NORMALS;  
    } */
    db.dnormals = NULL;

  // Set width/height stuff
    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = stride;

  // Okay baby, now do it!
    BOOL result = Draw(&db, &dpv);

    if (IsLocked())
        Unlock();
    if (GetZBuffer() && GetZBuffer()->IsLocked())
        GetZBuffer()->Unlock();

    return result;
}

BOOL TSurface::ParamBlitSetup(RSDrawParam tmpdp, PTSurface srcsurface, int ddflags, LPDDBLTFX fx)
{
  // Set defaults
    if (tmpdp.drawmode == DM_USEDEFAULT)
        tmpdp.drawmode = DM_DEFAULT;

  // If surface is wrap clipping and drawmode isn't, set drawmode
    if ((clipmode == CLIP_WRAP) &&
      !(tmpdp.drawmode & (DM_WRAPCLIP | DM_WRAPCLIPSRC | DM_NOWRAPCLIP)))
        tmpdp.drawmode |= DM_WRAPCLIP;

    int sox, soy;
    if (srcsurface)
        srcsurface->GetOrigin(sox, soy);
    else
        sox = soy = 0;
    tmpdp.sx += sox;
    tmpdp.sy += soy;

    tmpdp.originx    = originx;
    tmpdp.originy    = originy;

    tmpdp.clipx      = clipx;
    tmpdp.clipy      = clipy;
    tmpdp.clipwidth  = clipwidth;
    tmpdp.clipheight = clipheight;
    
    tmpdp.callback   = GetDrawCallBack();

    return TRUE;
}

BOOL TSurface::BlitHandler(PSDrawParam dp, PTSurface srcsurface, int ddflags, LPDDBLTFX fx)
{
    SDrawParam  tmpdp = *dp;
    SDrawBlock  tmpdb;

    if (!ParamBlitSetup(tmpdp, srcsurface, ddflags, fx))
        return FALSE;

    tmpdb.dbufwidth  = width;
    tmpdb.dbufheight = height;
    tmpdb.dstride = stride;
    tmpdb.dstbitmapflags = flags;

    if (srcsurface)
    {
      // Voodoo can't use zbuffer as a source surface
//      if (NoVidZBufLock && srcsurface == Display->GetZBuffer())
//          return TRUE;

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

        tmpdb.source = srcsurface->Lock();
        if (!tmpdb.source)
            return FALSE;
        if (UnlockImmediately)
            srcsurface->Unlock();
    }
    else
    {   
        tmpdb.srcbitmapflags = 0;
        tmpdb.sbufwidth = tmpdb.sbufheight = tmpdb.sstride = 0;
        tmpdb.source = NULL;
        tmpdb.keycolor = 0;
    }

    tmpdb.dest = Lock();
    if (!tmpdb.dest)
        return FALSE;
    if (UnlockImmediately)
        Unlock();

    if (!tmpdb.dest)
    {
        if (srcsurface && srcsurface->IsLocked())
            srcsurface->Unlock();
        return FALSE;
    }

    tmpdb.szbuffer = tmpdb.dzbuffer = NULL;
    tmpdb.szstride = tmpdb.dzstride = 0;
    tmpdb.snormals = tmpdb.dnormals = NULL;

    tmpdb.palette  = NULL;
    tmpdb.alias    = tmpdb.alpha    = NULL;

    tmpdp.drawmode &= ~(DM_ZBUFFER | DM_NORMALS | DM_ALPHA | DM_ALIAS);

    BOOL result = Draw(&tmpdb, &tmpdp);

    if (IsLocked())
        Unlock();
    if (srcsurface && srcsurface->IsLocked())
        srcsurface->Unlock();

    return result;
}

BOOL TSurface::ParamBlit(PSDrawParam dp, PTSurface surface, int ddflags, LPDDBLTFX fx)
{
    if (surface && surface->UseGetBlit())  // If source surface is complex, call its Get function instead
    {
        return surface->ParamGetBlit(dp, this, ddflags, fx);
    }

    if (!(dp->drawmode & DM_NODRAW))
    {
        if (!BlitHandler(dp, surface, ddflags, fx))
            return FALSE;
    }
    if (GetZBuffer() && 
        (!surface || surface->GetZBuffer()) && 
        (surface && (GetZBuffer() != surface->GetZBuffer())) &&
        (dp->drawmode & DM_ZBUFFER))
    {
        PTSurface zbuffer = NULL;
        if (surface)
            zbuffer = surface->GetZBuffer();
//      if ((void *)this == (void *)Display && NoVidZBufLock)
//      {
//          dp->drawmode |= DM_NOHARDWARE;
//      }
        if (!GetZBuffer()->BlitHandler(dp, zbuffer, ddflags, fx))
             return FALSE;
    }
    if (GetNormalBuffer() && 
        (!surface || surface->GetNormalBuffer()) &&
        (surface && (GetNormalBuffer() != surface->GetNormalBuffer())) &&
        (dp->drawmode & DM_NORMALS))
    {
        PTSurface normals = NULL;
        if (surface)
            normals = surface->GetNormalBuffer();
        if (!GetNormalBuffer()->BlitHandler(dp, normals, ddflags, fx))
            return FALSE;
    }

    return TRUE;
}

BOOL TSurface::ParamGetBlit(PSDrawParam dp, PTSurface surface, int ddflags, LPDDBLTFX fx)
{
    return surface->ParamBlit(dp, this, ddflags, fx);
}

BOOL TSurface::Put(int x, int y, PTBitmap bitmap, DWORD drawmode, PSColor color)
{ 
    if (!bitmap)
        return FALSE;

    SDrawParam dp;
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);

    if (color)
    {
        dp.color = TranslateColor(*color);
        dp.drawmode |= DM_CHANGECOLOR;
    }

    return ParamDraw(&dp, bitmap); 
}

BOOL TSurface::PutHue(int x, int y, PTBitmap bitmap, DWORD drawmode, int hue)
{ 
    if (!bitmap)
        return FALSE;

    SDrawParam dp;
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);

    dp.color = hue;
    dp.drawmode |= DM_CHANGEHUE;

    return ParamDraw(&dp, bitmap); 
}

BOOL TSurface::PutSV(int x, int y, PTBitmap bitmap, DWORD drawmode, int saturation, int brightness)
{ 
    if (!bitmap)
        return FALSE;

    SDrawParam dp;
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);

    saturation &= 0xFF;
    brightness &= 0xFF;

    dp.color = (saturation << 8) | brightness;
    dp.drawmode |= DM_CHANGESV;

    return ParamDraw(&dp, bitmap); 
}

BOOL TSurface::PutDim(int x, int y, PTBitmap bitmap, DWORD drawmode, int dim)
{ 
    if (!bitmap)
        return FALSE;

    SDrawParam dp;
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);

    dp.intensity = 31 - min(31, dim);

    return ParamDraw(&dp, bitmap);
}

BOOL TSurface::ZPut(int x, int y, int z, PTBitmap bitmap, DWORD drawmode)
{ 
    SDrawParam dp; 
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);
    dp.zpos = (WORD)z;
    return ParamDraw(&dp, bitmap); 
}

BOOL TSurface::ZPutDim(int x, int y, int z, PTBitmap bitmap, DWORD drawmode, int dim, PSColor color)
{ 
    SDrawParam dp; 
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);
    dp.zpos = (WORD)z;
    dp.intensity = 31 - min(31, dim);
    if (color)
        dp.color = TranslateColor(*color);
    else
        dp.color = 0xffff;

    return ParamDraw(&dp, bitmap); 
}

DWORD TSurface::ZFind(int x, int y, int z, PTBitmap bitmap, DWORD drawmode)
{ 
    SDrawParam dp; 
    MakeDP(dp, x, y, 0, 0, bitmap->width, bitmap->height, drawmode);
    dp.func = ::ZFind;
    dp.zpos = (WORD)z;

    return ParamDraw(&dp, bitmap);
}

BOOL TSurface::Box(int dx, int dy, int dwidth, int dheight, 
     DWORD color, WORD zpos, WORD normal, DWORD drawmode)
{ 
    if (drawmode & DM_USEDEFAULT)
        drawmode = DM_ZBUFFER | DM_NORMALS | DM_FILL;

    SDrawParam dp; 
    MakeDP(dp, dx, dy, 0, 0, dwidth, dheight, drawmode | DM_FILL);

    dp.func = ::Box;
    
    dp.zpos      = zpos;
    dp.normal    = normal;
    dp.color     = color;

    return ParamDraw(&dp);
}

BOOL TSurface::Line(int x1, int y1, int x2, int y2, SColor &color, DWORD drawmode)
{
        SDrawParam dp;
        SLineParam lp;

        MakeDP(dp, x1, y1, x1, y1, x2 - x1 + 1, y2 - y1 + 1, drawmode);
        dp.func = LineDraw;
        dp.color = TranslateColor(color);
        dp.data = (void *)&lp;
        lp.x1 = x1;
        lp.y1 = y1;
        lp.x2 = x2;
        lp.y2 = y2;

        return ParamDraw(&dp); 
}

BOOL TSurface::Rect(int x, int y, int w, int h, SColor &color, DWORD drawmode)
{
    if (!Line(x, y, x + w - 1, y, color, drawmode))
        return FALSE;
    if (!Line(x, y + 1, x, y + h - 2, color, drawmode))
        return FALSE;
    if (!Line(x + w - 1, y + 1, x + w - 1, y + h - 2, color, drawmode))
        return FALSE;
    if (!Line(x, y + h - 1, x + w - 1, y + h - 1, color, drawmode))
        return FALSE;
    return TRUE;
}

int TSurface::WriteText(char *text, int x, int y, int numlines, PTFont font, PSColor color,
                         DWORD drawmode, int wrapwidth, int startline, int justify, int hue, int linespace)
{
    if (!text || !*text)
        return TRUE;

    SDrawParam dp; 
    STextParam tp;

    MakeDP(dp, x, y, 0, 0, width, height, drawmode);
    if (drawmode == DM_USEDEFAULT)
        dp.drawmode = FONT_DRAWMODE;
    else
        dp.drawmode = drawmode;
    dp.dx = x;
    dp.dy = y;
    dp.dwidth  = width;
    dp.dheight = height;
    dp.func = TextDraw;
    dp.data = (void *)&tp;

    tp.text = text;
    tp.numlines = numlines;
    tp.font = font;
    tp.wrapwidth = wrapwidth;
    tp.startline = startline;
    tp.justify = justify;
    tp.draw = TRUE;
    tp.length = 0; // Length of text drawn
    tp.noclip = drawmode & DM_NOCLIP;       // save noclip value
    tp.linespace = linespace;
    dp.drawmode |= DM_NOCLIP;               // then set it temporarily

    if (color)
    {
        dp.drawmode |= DM_CHANGECOLOR;
        dp.color = TranslateColor(*color);
    }
    else if (hue >= 0)
    {
        dp.drawmode |= DM_CHANGEHUE;
        dp.color = hue;
    }
    else
        dp.color = 0;

    ParamDraw(&dp);

    return tp.length;
};

int TSurface::WriteTextShadow(char *text, int x, int y, int numlines, PTFont font, PSColor color,
                               DWORD drawmode, int wrapwidth, int startline, int justify, int hue, int linespace)
{
    SColor black = { 0, 0, 0 };
    WriteText(text, x + 1, y + 1, numlines, font, &black, drawmode, wrapwidth, startline, justify, linespace);
    int len = WriteText(text, x, y, numlines, font, color, drawmode, wrapwidth, startline, justify, hue, linespace);

    return len;
}
