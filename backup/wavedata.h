// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  wavedata.h - Wave (sound) objects                    *
// *************************************************************************

#ifndef _WAVEDATA_H
#define _WAVEDATA_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

/* general extended waveform format structure
   Use this for all NON PCM formats
   (information common to all formats)
*/
#ifndef _WAVEFORMATEX_
#define _WAVEFORMATEX_
typedef struct tWAVEFORMATEX
{
    WORD    wFormatTag;        /* format type */
    WORD    nChannels;         /* number of channels (i.e. mono, stereo...) */
    DWORD   nSamplesPerSec;    /* sample rate */
    DWORD   nAvgBytesPerSec;   /* for buffer estimation */
    WORD    nBlockAlign;       /* block size of data */
    WORD    wBitsPerSample;    /* Number of bits per sample of mono data */
    WORD    cbSize;            /* The count in bytes of the size of
                                    extra information (after cbSize) */

} WAVEFORMATEX;
typedef WAVEFORMATEX       *PWAVEFORMATEX;
typedef WAVEFORMATEX NEAR *NPWAVEFORMATEX;
typedef WAVEFORMATEX FAR  *LPWAVEFORMATEX;
#endif /* _WAVEFORMATEX_ */

// *************
// * TWaveData *
// *************

_CLASSDEF(TWaveData)
class TWaveData
{
  public:
    WAVEFORMATEX format;            // Format of wave; sent directly to DirectSound
    DWORD size;                     // Number of bytes of data
    int volume;                     // Volume adjustment
    long loopstart;                 // Begin loop location
    long loopend;                   // End loop location
    BYTE data[1];                   // Actual sound data
};

#endif
