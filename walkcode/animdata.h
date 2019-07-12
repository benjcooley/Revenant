// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            animdata.h - EXILE Animation Definition File               *
// *************************************************************************

#ifndef _ANIMDATA_H
#define _ANIMDATA_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

_STRUCTDEF(SAnimationFrame)
_CLASSDEF(TAnimationData)

struct SAnimationFrame
{
    int      dx;                // Delta Offsets to next frame
    int      dy;
    int      regx;              // Bitmap registration offset from path point
    int      regy;
    int      x;                 // Position of frame in decompression buffer
    int      y;
    int      width;
    int      height;
    DWORD    decbufsize;        // Size of Decompression buffer.
    OTBitmap bitmap;            // Long offset to compressed bitmap data
};

class TAnimationData
{
  public:
    int             flags;      // Animation flags
    int             maxwidth;   // Animation frames max width and height
    int             maxheight;  
    int             numframes;  // Number of frames in the animation
    DWORD           drawmode;   // Default drawmode for animation
    SAnimationFrame frames[1];  // Long offset to animation data record
};

#endif
