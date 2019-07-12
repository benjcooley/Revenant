// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     bitmap.cpp - Bitmap objects                       *
// *************************************************************************

#include <windows.h>
#include <stdio.h>

#include "revenant.h"
#include "display.h"
#include "bitmapdata.h"
#include "bitmap.h"
#include "graphics.h"
#include "resource.h"
#include "font.h"
#include "chunkcache.h"
#include "decompdata.h"

PTBitmap TBitmap::NewBitmap(int width, int height, int bmflags, 
    int aliasbufsize)
{
    int bytesperpixel;

    switch (bmflags & (BM_8BIT + BM_15BIT + BM_16BIT + BM_24BIT +
        BM_32BIT))
    {
        case BM_8BIT:
        bytesperpixel=1;
        break;
    
        case BM_15BIT:
        case BM_16BIT:
        bytesperpixel=2;
        break;

        case BM_24BIT:
        bytesperpixel=3;
        break;

        case BM_32BIT:
        bytesperpixel=4;
        break;

        default:
        return NULL;
        break;
    }   

    int allocval = (bytesperpixel * width * height) + sizeof(TBitmap) - 
        4 + aliasbufsize;
    
    int zbuffersize = 0;
    int normalsize  = 0;
    int alphasize   = 0;
    int palettesize = 0;

    if (bmflags & BM_ZBUFFER) 
    {
        allocval   += (width << 1) * height;
        zbuffersize = (width << 1) * height;
    }
    
    if (bmflags & BM_NORMALS)
    {
        allocval  += (width << 1) * height;
        normalsize = (width << 1) * height;
    }

    if (bmflags & BM_ALPHA)
    {
        allocval += (width << 1) * height;
        alphasize = (width << 1) * height;
    }
    
    if (bmflags & BM_PALETTE) 
    {
        allocval   += sizeof(SPalette);
        palettesize = sizeof(SPalette);
    }
    
    PTBitmap bitmap = (PTBitmap) new BYTE[allocval];
    if (bitmap == NULL) return NULL;
    
    bitmap->width  = width;
    bitmap->height = height;
    bitmap->flags  = bmflags;

    int drawmode = 0;

    if (bmflags & BM_ZBUFFER) 
        drawmode |= DM_ZBUFFER;
    
    if (bmflags & BM_NORMALS) 
        drawmode |= DM_NORMALS;

    if (bmflags & BM_ALIAS) 
        drawmode |= DM_ALIAS;

    if (bmflags & BM_ALPHA) 
        drawmode |= DM_ALPHA;

    bitmap->drawmode    = drawmode;
    bitmap->keycolor    = 0;

    bitmap->aliassize   = aliasbufsize;
    if (aliasbufsize)
      bitmap->alias.set((void *)((BYTE*)bitmap + bytesperpixel * width * height + sizeof(TBitmap) - 4));
    else
      bitmap->alias.set(0);

    bitmap->alphasize   = alphasize;
    if (alphasize)
      bitmap->alpha.set((void *)((BYTE*)bitmap + bytesperpixel * width * height + sizeof(TBitmap) 
                        + aliasbufsize - 4));
    else
      bitmap->alpha.set(0);

    bitmap->zbuffersize = zbuffersize;
    if (zbuffersize)
      bitmap->zbuffer.set((void *)((BYTE*)bitmap + bytesperpixel * width * height 
                        + sizeof(TBitmap) + aliasbufsize + alphasize - 4));
    else
      bitmap->zbuffer.set(0);

    bitmap->normalsize  = normalsize;
    if (normalsize)
      bitmap->normal.set((void *)((BYTE*)bitmap + bytesperpixel * width * height 
                        + sizeof(TBitmap) + aliasbufsize + alphasize + zbuffersize - 4));
    else
      bitmap->normal.set(0);

    bitmap->palettesize = palettesize;
    if (palettesize)
      bitmap->palette.set((void *)((BYTE*)bitmap + bytesperpixel * width * height + sizeof(TBitmap) + aliasbufsize
                        + alphasize + zbuffersize + normalsize - 4));
    else
      bitmap->palette.set(0);

    bitmap->datasize    = bytesperpixel * width * height;

    return bitmap;
}

PTBitmap TBitmap::Load(int resource)
{
    PTBitmap bitmap = (PTBitmap)LoadResource("bitmap", resource);
    return bitmap;
}

// ****************************
// * Bitmap Drawing Functions *
// ****************************

void TBitmap::WriteText(char *text, int x, int y, int lines, PTFont font, PSColor color, DWORD drawmode)
{
    SDrawBlock db;
    SDrawParam dp;
    STextParam tp;

    memset(&db, 0, sizeof(SDrawBlock));
    memset(&dp, 0, sizeof(SDrawParam));
    memset(&tp, 0, sizeof(STextParam));

    db.srcbitmapflags = BM_15BIT | BM_ALIAS;
    db.dstbitmapflags = flags;

    db.dest = &data16;
    if (db.dest == NULL) 
        return;

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = width;
    db.keycolor   = 0;

    dp.func = TextDraw;

    if (drawmode == DM_USEDEFAULT)
        dp.drawmode = FONT_DRAWMODE;
    else
        dp.drawmode = drawmode;

    dp.originx    = 0;
    dp.originy    = 0;
                   
    dp.clipx      = 0;
    dp.clipy      = 0;
    dp.clipwidth  = width;
    dp.clipheight = height;

    dp.data = (void *)&tp;

    if (color)
    {
        dp.drawmode |= DM_CHANGECOLOR;
        dp.color = TranslateColor(*color);
    }
    else
        dp.color = 0;

    dp.sx         = 0;
    dp.sy         = 0;

    dp.dx         = x;
    dp.dy         = y;

    dp.dwidth  = width;
    dp.dheight = height;

    tp.text = text;
    tp.numlines = lines;
    tp.font = font;
    tp.wrapwidth = width;
    tp.startline = 0;
    tp.justify = 0;
    tp.length = 0; // Length of text drawn

    Draw(&db, &dp);
};

// **********************************
// * Bitmap Decompressing Functions *
// **********************************

// *********** Chunk Bitmap Transfer Routines *********
BOOL TBitmap::CacheChunks()
{
    if (!(flags & BM_CHUNKED))
        return FALSE;

    PSChunkHeader hdr  = (PSChunkHeader)(void *)data8;
    PSChunkHeader zhdr = (PSChunkHeader)(void *)zbuffer;
    PSChunkHeader nhdr = (PSChunkHeader)(void *)normal;

    int type   = hdr->type;
    int width  = hdr->width;
    int height = hdr->height;

    for (int outerloop = 0; outerloop < hdr->height; outerloop++)
    {
        for (int innerloop = 0; innerloop < hdr->width; innerloop++)
        {
            ChunkCache.AddChunk(hdr->block[outerloop * width + innerloop].ptr(), 1);
            
            if (zhdr)
                ChunkCache.AddChunkZ(zhdr->block[outerloop * width + innerloop].ptr(), 2);

            if (nhdr)
                ChunkCache.AddChunk16(nhdr->block[outerloop * width + innerloop].ptr(), 1);
        }
    }

    return TRUE;
}

// **************************
// * Misc. Bitmap Functions *
// **************************

BOOL TBitmap::Clear(SColor &color, DWORD bmdrawmode, WORD zpos)
{
    if (width < 1 || height < 1) return FALSE;

    if (datasize < 1)
        return FALSE;

    if (bmdrawmode == DM_USEDEFAULT)
        bmdrawmode = DM_ZBUFFER | DM_NORMALS;

    SDrawBlock  db;
    memset(&db, 0, sizeof(SDrawBlock));

    SDrawParam  dp;
    MakeDP(dp, 0, 0, 0, 0, width, height, bmdrawmode);

    db.dest       = (WORD *)data16;
    db.dzbuffer   = (WORD *)zbuffer.ptr();
    db.dzstride   = width;
    db.dnormals   = (WORD *)normal.ptr();

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = width;

    DWORD bytesperpixel;

    switch (flags & (BM_8BIT | BM_15BIT | BM_16BIT | BM_24BIT | BM_32BIT))
    {
        case BM_8BIT:
            db.dstbitmapflags = BM_8BIT;
            bytesperpixel = 1;
            return FALSE;
        break;
    
        case BM_15BIT:
        case BM_16BIT:
            db.dstbitmapflags = BM_16BIT;
            bytesperpixel = 2;
        break;

        case BM_24BIT:
            db.dstbitmapflags = BM_24BIT;
            bytesperpixel = 3;
        break;

        case BM_32BIT:
            db.dstbitmapflags = BM_32BIT;
            bytesperpixel = 4;
            break;

        default:
            return FALSE;
        break;
    }   

    DWORD newcolor;

    if (bytesperpixel == 2) 
    {
        newcolor  = TranslateColor(color);
        newcolor  = (newcolor << 16) | newcolor;
    }

    else
        newcolor  = (color.red << 24) | (color.green << 16) | (color.blue << 8);
    
    dp.func       = ::Box;
    dp.intensity  = newcolor;
    dp.zpos       = zpos;

    dp.clipx      = 0;
    dp.clipy      = 0;
    dp.clipwidth  = width;
    dp.clipheight = height;
    dp.color      = newcolor;

    return Draw(&db, &dp);
}

// Copies one bitmap to another 

BOOL TBitmap::RawPut(int x, int y, 
                  PTBitmap bitmap, int srcx, int srcy, int srcw, int srch, int drawmode,
                  DWORD color, int intensity, WORD zpos, DRAWFUNCTION func, void *data)
{
    if (!bitmap)
        return FALSE;

    SDrawBlock  db;
    SDrawParam  dp;

    memset(&db, 0, sizeof(SDrawBlock));
    memset(&dp, 0, sizeof(SDrawParam));

    db.srcbitmapflags = bitmap->flags;
    db.dstbitmapflags = flags;

    db.dest = &data16;
    if (db.dest == NULL) 
        return FALSE;

    db.source = &bitmap->data16;
    if (db.source == NULL) 
        return FALSE;

    db.sbufwidth  = bitmap->width;
    db.sbufheight = bitmap->height;
    db.sstride    = db.sbufwidth;

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = width;

    db.szbuffer   = (WORD *)bitmap->zbuffer.ptr();
    db.szstride   = bitmap->width;
    db.dzbuffer   = (WORD *)zbuffer.ptr();
    db.dzstride   = width;

    db.snormals   = (WORD *)bitmap->normal.ptr();
    db.dnormals   = (WORD *)normal.ptr();

    db.palette    = (WORD *)bitmap->palette.ptr();
    db.alpha      = (BYTE *)bitmap->alpha.ptr();
    db.alias      = (BYTE *)bitmap->alias.ptr();

    db.keycolor   = bitmap->keycolor;

    if (drawmode == DM_USEDEFAULT)
        drawmode = bitmap->drawmode;

    dp.drawmode   = drawmode | DM_NORESTORE;
    dp.originx    = 0;
    dp.originy    = 0;
                   
    dp.clipx      = 0;
    dp.clipy      = 0;
    dp.clipwidth  = width;
    dp.clipheight = height;

    dp.sx         = srcx;
    dp.sy         = srcy;

    dp.dx         = x;
    dp.dy         = y;

    if (drawmode & DM_USEREG)
    {
        dp.dx -= bitmap->regx;
        dp.dy -= bitmap->regy;
    }

    dp.swidth     = dp.dwidth  = srcw;
    dp.sheight    = dp.dheight = srch;

    dp.func       = func;
    dp.data       = data;
    dp.color      = color;
    dp.intensity  = intensity;
    dp.zpos       = zpos;

    return Draw(&db, &dp);
}

BOOL TBitmap::Put(int x, int y, PTSurface surface, int srcx, int srcy, int srcw, 
    int srch, int drawmode, int intensity)
{
    SDrawBlock  db;
    SDrawParam  dp;

    memset(&db, 0, sizeof(SDrawBlock));
    memset(&dp, 0, sizeof(SDrawParam));

    db.srcbitmapflags = surface->flags;
    db.dstbitmapflags = flags;

    db.dest = &data16;
    db.source = surface->Lock();
    surface->Unlock();

    if (!db.source || !db.dest) 
        return FALSE;

    db.sbufwidth  = surface->Width();
    db.sbufheight = surface->Height();
    db.sstride    = surface->Stride();

    db.dbufwidth  = width;
    db.dbufheight = height;
    db.dstride    = width;

    db.szbuffer   = (WORD *)surface->GetZBuffer();
    db.szstride   = surface->Stride();
    db.dzbuffer   = (WORD *)zbuffer.ptr();
    db.dzstride   = width;

    db.snormals   = (WORD *)surface->GetNormalBuffer();
    db.dnormals   = (WORD *)normal.ptr();

    db.palette    = NULL;
    db.alpha      = (BYTE *)NULL;
    db.alias      = (BYTE *)NULL;

    dp.drawmode   = drawmode | DM_NORESTORE;
    dp.drawmode   = dp.drawmode & ~DM_ALIAS;
    dp.drawmode   = dp.drawmode & ~DM_ALPHA;
    dp.originx    = 0;
    dp.originy    = 0;
                   
    dp.clipx      = 0;
    dp.clipy      = 0;
    dp.clipwidth  = width;
    dp.clipheight = height;

    dp.sx         = srcx;
    dp.sy         = srcy;

    dp.dx         = x;
    dp.dy         = y;

    dp.swidth     = dp.dwidth  = srcw;
    dp.sheight    = dp.dheight = srch;

    dp.intensity  = intensity;

    return Draw(&db, &dp);
}

BOOL TBitmap::SaveBMP(char *filename)
{
    if (this->width < 1 || this->height < 1 || !(flags & (BM_15BIT | BM_16BIT)))
        return FALSE;
    
    int dstwidth = (width + 3) & 0xFFFFFFFC; // Round up to even 4 pixels

    FILE *f = fopen(filename, "wb");
    if (!f)
        return FALSE;

  // Bitmap file header
    BITMAPFILEHEADER header;
    memset(&header, 0, sizeof(BITMAPFILEHEADER));
    BITMAPINFOHEADER info;
    memset(&info, 0, sizeof(BITMAPINFOHEADER));

    header.bfType = ((WORD)'M' << 8U) + (WORD)'B';
    header.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (dstwidth * 3) * height;
    header.bfReserved1 = 0;
    header.bfReserved2 = 0;
    header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    
    if (fwrite(&header, sizeof(header), 1, f) != 1)
    {
        fclose(f);
        return FALSE;
    }

  // Bitmap info header
    info.biSize = sizeof(BITMAPINFOHEADER);
    info.biWidth = dstwidth;
    info.biHeight = height;
    info.biPlanes = 1;
    info.biBitCount = 24;
    info.biCompression = BI_RGB;
//  info.biSizeImage = (dstwidth * 3) * (DWORD)height;
    
    if (fwrite(&info, sizeof(info), 1, f) != 1)
    {
        fclose(f);
        return FALSE;
    }

    BYTE *line = new BYTE[dstwidth * 3];
    if (!line)
    {
        fclose(f);
        return FALSE;
    }

    memset(line, 0, dstwidth * 3);

    WORD *src = ((WORD *)data16) + (width * (height - 1));

    for (int loop = 0; loop < height; loop++)
    {
        WORD *s = src;
        BYTE *d = line;
        for (int x = 0; x < width; x++)
        {
            BYTE red, green, blue;
            if (flags & BM_16BIT)
            {
                red = (BYTE)((WORD)(*s & 0xF800) >> 8);
                green = (BYTE)((WORD)(*s & 0x07E0) >> 3);
                blue = (BYTE)((WORD)(*s & 0x001F) << 3);
            }
            else
            {
                red = (BYTE)((WORD)(*s & 0x7C00) >> 7);
                green = (BYTE)((WORD)(*s & 0x03E0) >> 2);
                blue = (BYTE)((WORD)(*s & 0x001F) << 3);
            }
            s++;
            *d++ = blue;
            *d++ = green;
            *d++ = red;
        }
        
        if (fwrite(line, dstwidth * 3, 1, f) != 1)
        {
            delete line;
            fclose(f);
            return FALSE;
        }

        src -= width;
    }

    delete line;

    fclose(f);

    return TRUE;
}

BOOL TBitmap::SaveZBF(char *filename)
{
    if (this->width < 1 || 
        this->height < 1 || 
        !(flags & BM_ZBUFFER) ||
        zbuffer.ptr() == NULL)
            return FALSE;
    
    FILE *f = fopen(filename, "wb");
    if (!f)
        return FALSE;

    if (fwrite(zbuffer.ptr(), width * height * 2, 1, f) != 1)
    {
        fclose(f);
        return FALSE;
    }

    return TRUE;
}

BOOL TBitmap::OnPixel(int x, int y)
{
    if (x < 0 || y < 0 || x >= width || y >= height)
        return FALSE;

    if (flags & (BM_15BIT | BM_16BIT))
    {
        WORD pixel = *(data16 + (y * width) + x);
        return pixel != keycolor;
    }

    if (flags & BM_8BIT)
    {
        BYTE pixel = *(data8 + (y * width) + x);
        return pixel != keycolor;
    }

    return TRUE;
}

// *************************
// * Bitmap only functions *
// *************************

// ********** Line Drawing Routines **********
BOOL TBitmap::Line(int x1, int y1, int x2, int y2, SColor &color)
{
    int width = this->width;
    int height = this->height;

    TBitmap bitmap = *this;
    WORD *data=bitmap.data16;

    int bytesperpixel;

    switch (bitmap.flags & (BM_8BIT + BM_15BIT + BM_16BIT + BM_24BIT))
    {
        case BM_8BIT:
            return FALSE;
        break;
    
        case BM_15BIT:
        case BM_16BIT:
            bytesperpixel=2;
        break;

        case BM_24BIT:
            bytesperpixel=3;
        break;

        default:
            return FALSE;
        break;
    }   

    __asm
    {
        mov eax, x1
        cmp eax, x2
        jne noswap   
       
        xchg eax, x2
        mov x1, eax
        mov eax, y1
        xchg eax, y2
        mov y1, eax
noswap:
    }   
    
    if (x2 < 0 || x1 >= width || max(y1,y2) < 0 || min(y1,y2) >= height)
        return FALSE;

    int dx = x2 - x1;
    int dy = y2 - y1;

    if (x1 < 0)
    {
        y1 += -x1 * dy / dx;
        x1 = 0;
    }
    if (x2 >= width)
    {
        y2 += (width - x2 - 1) * dy / dx;
        x2 = width - 1;
    }

    if (y1 < 0)
    {
        x1 += -y1 * dx / dy;
        y1 = 0;
    }
    
    else if (y1 >= height)
    {
        x1 += (height - y1 - 1) * dx / dy;
        y1 = height - 1;
    }

    if (y2 < 0)
    {
        x2 += -y2 * dx / dy;
        y2 = 0;
    }

    else if (y2 >= height)
    {
        x2 += (height - y2 - 1) * dx / dy;
        y2 = height - 1;
    }

    return TRUE;
}

// ********** Fill Rectangle Routines **********

BOOL TBitmap::Box(int x1, int y1, int x2, int y2, SColor &color)
{
    int fillwidth = x2 - x1 + 1;
    int fillheight = y2 - y1 + 1;

  // Get offset/add stuff
    long dstoff = (y1 * width) + x1 ;
    long dstadd = width - fillwidth;

    TBitmap bitmap=*this;
    WORD *data=bitmap.data16;

    __asm
    {
        cld

  // Load destination
        mov     edi, DWORD PTR [data]
        add     edi, dstoff

        mov     eax, color
        shl     eax, 16
        or      eax, color

  // Load lines
        mov     edx, fillheight

forward:

  // Get Color int AX
        mov     ecx, fillwidth
        shr     ecx, 1
fillloop:
        mov     [edi], eax
        add     edi, 4
        dec     ecx
        jne     fillloop

        mov     ecx, fillwidth
        and     ecx, 1
        je      notodd

        mov     [edi], ax
        add     edi, 2

notodd:
        add     edi, dstadd
        dec     ecx
        jne     forward
    
        dec     edx
        jne     forward
    }
    
    return TRUE;
}

WORD TranslateColor(SColor &color)
{
    WORD red, green, blue, result;

    red = color.red;
    red = red >> 3;
    green = color.green;
    blue = color.blue;
    blue = blue >> 3;
    
    if (Display->BitsPerPixel() == 16)
    {
        green = green >> 2;
        result = (red <<11) | (green << 5) | blue;
    }

    else
    {
        green = green >> 3;
        result = (red <<10) | (green << 5) | blue;
    }
    
    return result;
}

