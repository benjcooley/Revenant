// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             Decompdata.h - Decompression Structure File               *
// *************************************************************************

#ifndef _DECOMPDATA_H
#define _DECOMPDATA_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

// Width and height of Chunk blocks
#define CHUNKWIDTH  64
#define CHUNKHEIGHT 64

_STRUCTDEF(SChunkHeader)

struct SChunkHeader
{
    BOOL    type;       // Compressed Flag;
    int     width;      // Width and height of chunked bitmap in blocks
    int     height;
    OFFSET  block[1];   // Pointer to start of offsets for each block. 
                        // An offset of 0 is a blank block.
};

#endif