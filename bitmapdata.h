// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 bitmapdata.h - Bitmap data objects                    *
// *************************************************************************

#ifndef _BITMAPDATA_H
#define _BITMAPDATA_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "graphics.h"

_STRUCTDEF(SPalette);

struct SPalette
{
    WORD colors[256];
    DWORD rgbcolors[256];
};

#ifndef _WINGDI_
typedef struct tagRGBTRIPLE {
        BYTE    rgbtBlue;
        BYTE    rgbtGreen;
        BYTE    rgbtRed;
} RGBTRIPLE;
#endif

// *******************************************
// * TBitmapData Class Varible/Function List *
// *******************************************

_CLASSDEF(TBitmapData)

class TBitmapData
{
  public:

    int    width;     // Bitmap Width
    int    height;    // Bitmap Height
    int    regx;      // Registration point x
    int    regy;      // Registration point y
    DWORD  flags;     // Bitmap Flags. See exiledef.h for Bitmap flags.
    DWORD  drawmode;  // Default drawing mode of bitmap. See exiledef.h
                      // for drawing mode flags.
    DWORD  keycolor;  // Color to use as Transparent color.
    DWORD  aliassize; // Size of Alias Buffer
    OFFSET alias;     // Relative Offset to alias data.
    DWORD  alphasize;
    OFFSET alpha;   
    DWORD  zbuffersize;
    OFFSET zbuffer;
    DWORD  normalsize;
    OFFSET normal;
    DWORD  palettesize;
    OFFSET palette;
    DWORD  datasize;

    union            // Bitmap Data in 8/15/24/32 bit
    {
        BYTE      data8[1];
        WORD      data16[1];
        RGBTRIPLE data24[1];
        DWORD     data32[1];
    };

};
#endif
