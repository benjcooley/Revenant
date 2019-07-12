// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 font.h - EXILE Font Definition File                   *
// *************************************************************************

#ifndef _FONT_H
#define _FONT_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "fontdata.h"
#include "bitmap.h"
#include "resource.h"

#define FONT_DRAWMODE   (DM_TRANSPARENT | DM_ALIAS | DM_BACKGROUND)

_CLASSDEF(TFont)

class TFont : public TFontData
{
  public:
    TFont() {}
    static PTFont LoadFont(int id)
        { return (PTFont)LoadResource("FONT", id); }
    PTBitmap GetChar(unsigned char ch);
    int FirstChar()
        { return firstchar; }
    int Numchars()
        { return numchars; }
    int StartHeight(unsigned char ch)
        { return (int) startheight[ch - firstchar]; }
    int DrawRight(unsigned char ch)
        { return (int) drawright[ch - firstchar]; }
    int DrawLeft(unsigned char ch)
        { return (int) drawleft[ch - firstchar]; }

    int FindNumLinesInText(char *text, int wrapwidth = -1, int justify = JUSTIFY_LEFT);
        // Find the number of lines in the given text and wrapwidth - this function
        // resides in the font because it is highly variable depending on the overlap
        // and size of each character in the font.
};

#endif
