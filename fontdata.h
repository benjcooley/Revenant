// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             fontdata.h - EXILE Font Data Definition File              *
// *************************************************************************

#ifndef _FONTDATA_H
#define _FONTDATA_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#define CURSOR      ('z' + 1)

_CLASSDEF(TFontData)
class TFontData
{
  public:
    short firstchar;
    short numchars;
    short height;
    short startheight[MAXFONTCHARS];
    BYTE drawleft[MAXFONTCHARS];
    BYTE drawright[MAXFONTCHARS];
    OTBitmapData chars[MAXFONTCHARS];
};

#endif
