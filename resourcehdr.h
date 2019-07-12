// *********************************************************************
// *             C I N E M A T I X   G A M E   S Y S T E M             *
// *                          Resource Compiler                        *
// *                             EXILERC.EXE                           *
// *              Copyright (C) 1993 by Cinematix Studios              *
// *********************************************************************

#ifndef _RESOURCEHDR_H
#define _RESOURCEHDR_H

#define RESVERSION  1   // Increment this when the resource file changes

#define COMP_NONE   0   // No Compression
#define COMP_ZIP    1   // ZIP implode compression

#define RESMAGIC ('C' | ('G' << 8) | ('S' << 16) | ('R' << 24))

struct FileResHdr
{
    DWORD resmagic;
    WORD  topbm;
    BYTE  comptype;
    BYTE  version;
    DWORD datasize;
    DWORD objsize;
    DWORD hdrsize;
};

#endif
