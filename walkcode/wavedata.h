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

#include <mmreg.h>

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
