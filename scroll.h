// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      scroll.h - TScroll module                        *
// *************************************************************************

#ifndef _SCROLL_H
#define _SCROLL_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _MULTI_H
#include "multi.h"
#endif

#include "object.h"
#include "button.h"
#include "font.h"

_CLASSDEF(TScroll)
class TScroll : public TObjectInstance
{
  public:
    TScroll(PTObjectImagery newim) : TObjectInstance(newim) { text = NULL; }
    TScroll(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { text = NULL; }

    char *GetText() { return text; }
    void SetText(char *newtext);

    virtual BOOL Use(PTObjectInstance user, int with = -1);
        // Read scroll
    virtual int CursorType(PTObjectInstance inst = NULL) { if (inst) return CURSOR_NONE; return CURSOR_EYE; }
        // Indicate you can read the scroll

    virtual void Load(RTInputStream is, int version, int objversion);
    virtual void Save(RTOutputStream os);

    // Scroll stats
    STATFUNC(Value)

  private:
    char *text;             // Text written on the scroll
};

DEFINE_BUILDER("SCROLL", TScroll)

// ***************
// * TScrollPane *
// ***************

_CLASSDEF(TScrollPane)
class TScrollPane : public TButtonPane
{
  public:
    TScrollPane() : TButtonPane(0, 0, WIDTH, HEIGHT) { scroll = NULL; }
    
    virtual BOOL Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void KeyPress(int key, BOOL down);

    void SetScroll(PTScroll s);
        // Set the pane to read from the scroll object given

    void Scroll(int numscrolllines);
        // Can be negative or positive to scroll up or down

  protected:
    PTScroll scroll;                // Pointer to the scroll being read

    PTMulti scrolldata;             // Bitmap of scroll and buttons
    PTFont scrollfont;              // Text font

    int line;                       // For scrolling through the text
    int numlines;                   // Number of total lines in the text
};

// *************
// * TBookPane *
// *************

_CLASSDEF(TBookPane)
class TBookPane : public TScrollPane
{
  public:
    TBookPane() : TScrollPane() { }

    virtual BOOL Initialize();
    virtual void DrawBackground();
};

#endif
