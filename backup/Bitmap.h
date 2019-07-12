// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      bitmap.h - Bitmap objects                        *
// *************************************************************************

#ifndef _BITMAP_H
#define _BITMAP_H

#include <memory.h>

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "bitmapdata.h"
#include "surface.h"

// ***************************************
// * TBitmap Class Varible/Function List *
// ***************************************

_CLASSDEF(TBitmap)

class TBitmap : public TBitmapData
{
  public:
    static PTBitmap NewBitmap(int width, int height, int flags, int aliasbufsize = 0);
      // Creates bitmap 
    static PTBitmap Load(int resource);
      // Loads bitmap

    BOOL RawPut(int x, int y, 
          PTBitmap bitmap, int srcx, int srcy, int srcw, int srch, int drawmode,
          DWORD color, int intensity, WORD zpos, DRAWFUNCTION func, void *data);

    BOOL Put(int x, int y, PTBitmap bitmap, int srcx, int srcy, int srcw, int srch, 
             int drawmode  = NULL, int intensity = 31)
       { return RawPut(x, y, bitmap, 
            srcx, srcy, srcw, srch, drawmode, 0, intensity, 0, NULL, NULL); }
      // Draws a Rectangle of bitmap to bitmap.    
    BOOL Put(int x, int y, PTBitmap bitmap, int drawmode = DM_USEDEFAULT, 
             int intensity = 31)
       { return RawPut(x, y, bitmap, 
            0, 0, bitmap->width, bitmap->height, drawmode, 0, intensity, 0, NULL, NULL); }

      // Z Draws bitmap to bitmap
    BOOL ZPut(int x, int y, int z, PTBitmap bitmap, int srcx, int srcy, int srcw, int srch, 
             int drawmode  = NULL)
       { return RawPut(x, y, bitmap, 
            srcx, srcy, srcw, srch, drawmode, 0, 0, (WORD)z, NULL, NULL); }
      // Z Draws a Rectangle of bitmap to bitmap.    
    BOOL ZPut(int x, int y, int z, PTBitmap bitmap, int drawmode = DM_USEDEFAULT)
       { return RawPut(x, y, bitmap, 
            0, 0, bitmap->width, bitmap->height, drawmode, 0, 0, (WORD)z, NULL, NULL); }
      // Draws bitmap to bitmap
    
    BOOL Put(int x, int y, PTSurface surface, int srcx, int srcy, int srcw, int srch, 
             int drawmode  = NULL, int intensity = 31);
      // Draws a Rectangle of a surface to bitmap.    
    BOOL Put(int x, int y, PTSurface surface, int drawmode = DM_USEDEFAULT, 
             int intensity = 31)
       { return Put(x, y, surface, 0, 0, surface->Width(), surface->Height(), drawmode, 
            intensity); }
      // Draws surface to bitmap
    
    BOOL StretchPut(int x, int y, int w, int h, PTBitmap bitmap, int drawmode);
    BOOL StretchPut(int x, int y, int w, int h, PTBitmap bitmap, int srcx,
                    int srcy, int srcw, int srch, int drawmode);
      // Stretch versions of Normal Puts
    
    void WriteText(char *text, int x = 0, int y = 0, int lines = 1, PTFont font = SystemFont, PSColor color = NULL, DWORD drawmode = DM_USEDEFAULT);
      // Draws text to the bitmap

    BOOL Line(int x1, int y1, int x2, int y2, SColor &color);
    BOOL Rect(int x, int y, int w, int h, SColor &color);
    BOOL Box(int x, int y, int w, int h, SColor &color);
      // Draws a filled Rect.

  // Compression functions
    BOOL CacheChunks();
      // If this is a compressed bitmap, causes chunks to be decompressed into chunk cache
      // Returns TRUE if chunks were decompressed to cache

  // Miscellaneous bitmap buffer functions
    BOOL Clear(SColor &color, DWORD drawmode = DM_USEDEFAULT, WORD zpos = 0xffff);
      // Clears bitmap to keycolor.                 
    BOOL SaveBMP(char *filename);
      // Saves BMP of source bitmap.
    BOOL SaveZBF(char *filename);
      // Saves zbuffer file
    BOOL OnPixel(int x, int y);
      // Check to see if target pixel is not keycolor

    BOOL operator= (PTBitmap bitmap)
        { memcpy(bitmap, this, sizeof(this)); return TRUE; }
      // Allows use of '=' to copy values from one variable to another
};

WORD TranslateColor(SColor &color);
    // Translate 24 Bit Color into 15/16 bit WORD value

#endif
