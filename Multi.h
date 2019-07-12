// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              multi.h - TMulti multiple resource class                 *
// *************************************************************************

#ifndef _MULTI_H
#define _MULTI_H

// ******************************************
// * TMulti - Multiple resource array class *
// ******************************************

// The TMulti class is basically a big buffer of resources accessed by an
// offset array.

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _RESOURCE_H
#include "resource.h"
#endif

#ifndef _MULTIDAT_H
#include "multidat.h"
#endif

_CLASSDEF(TMulti)
class TMulti : public TMultiData
{
  public:

  // Functions
    static PTMulti LoadMulti(char *name)
      { return (PTMulti)LoadResource(name); }
    static PTMulti LoadMulti(char *name, int id)
      { return (PTMulti)LoadResource(name, id); }
    void operator delete(void *p)
      { free(p); }

    void *Object(int i)
      { return offsets[i]; }
    void *Object(char *name);

    PTAnimation Animation(int i)
      { return (PTAnimation)(void *)offsets[i]; }
    PTBitmap Bitmap(int i)
      { return (PTBitmap)(void *)offsets[i]; }
    PTFont Font(int i)
      { return (PTFont)(void *)offsets[i]; }

    PTAnimation Animation(char *name)
      { return (PTAnimation)Object(name); }
    PTBitmap Bitmap(char *name)
      { return (PTBitmap)Object(name); }
    PTFont Font(char *name)
      { return (PTFont)Object(name); }
    PTWaveData Wave(char *name)
      { return (PTWaveData)Object(name); }
};

#endif

