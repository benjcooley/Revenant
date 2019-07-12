// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      SpellPane.h - Spell Pane                         *
// *************************************************************************

#ifndef _SPELLPANE_H
#define _SPELLPANE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

#ifndef _BUTTON_H
#include "button.h"
#endif

#ifndef _SPELL_H
#include "spell.h"
#endif

extern char *Talismans[];
extern char *Old[];

// *******************
// * TTalismanButton *
// *******************

_CLASSDEF(TTalismanButton)
class TTalismanButton : public TButton
{
  public:
    TTalismanButton(char *bname, int bx, int by, int bw, int bh, WORD keypr,
            void (*bfunc)(), PTBitmap dbm = NULL, PTBitmap ubm = NULL,
            BOOL rad = FALSE, BOOL tog = FALSE, BOOL notsquare = FALSE,
            int qspellid = -1, int xoff = 0) :
        TButton(bname, bx, by, bw, bh, keypr, bfunc, dbm, ubm, rad, tog, notsquare, -1, 0)
            { quickspellid = qspellid; xoffset = xoff; Clear(); }

    virtual void Draw();
        // Draw the button to the screen

    void AddTalisman(char t);
        // Add a talisman to the button
    void Backspace();
        // Backspace a single talsiman
    void Clear();
        // Clear all talismans
    void Invoke();
        // Invoke the spell on the button
    char *GetSpell();
        // get the talismans for this button
    void SetSpell(char *talismans);
        // Get the talismans for this button
    BOOL HasTalismans();
        // Checks to see if player has talismans for this spell

  protected:
    int quickspellid;               // Player quickspell for this button (-1 is no player spell)
    int xoffset;                    // Offset 
};

// **************
// * TSpellPane *
// **************

// This pane is where the user assembles spells with various talismans and invokes them.

_CLASSDEF(TSpellPane)
class TSpellPane : public TButtonPane
{
  public:
    TSpellPane() : TButtonPane(MULTIPANEX, MULTIPANEY, MULTIPANEWIDTH, MULTIPANEHEIGHT) {}
    ~TSpellPane() {}

    virtual BOOL Initialize();
    virtual void DrawBackground();
    virtual void MouseClick(int button, int x, int y);
    virtual void MouseMove(int button, int x, int y);

    void Scroll(int numlines);
        // Scroll the pane's contents

    void ToggleTalismanNames();
        // Change between showing the english name and just the symbol
    BOOL ShowTalismanNames() { return shownames; }
        // Whether to show the name of the talisman next to its icon

    BOOL AddTal(int tal);
        // Add a talisman to the current spell
    BOOL RemoveTal(int numtals = 1);
        // Backspace numtals of talismans

    void Invoke();
        // Invoke the spell in the button

    char *GetSpell();
        // get the spell info

  private:
    int OnTal(int x, int y);
        // Which talisman mouse is on

    BOOL shownames;                     // expand names
    int startline;                      // for scrolling

    int clickedtal;                     // which talisman is currently clicked
    BOOL onclickedtal;                  // whether mouse arrow is still on the clicked talisman
};

// *******************
// * TQuickSpellPane *
// *******************

#define NUMBUTTONS      4

// A bunch of buttons above the inventory giving the player quicker access to
// their spells.

_CLASSDEF(TQuickSpellPane)
class TQuickSpellPane : public TButtonPane
{
  public:
    TQuickSpellPane() : TButtonPane(QUICKSPELLX, QUICKSPELLY, QUICKSPELLWIDTH, QUICKSPELLHEIGHT) {}
    ~TQuickSpellPane() {}

    virtual BOOL Initialize();
    virtual void DrawBackground();
    virtual void MouseClick(int button, int x, int y);

    void Invoke(int button);
        // Invoke the spell on the given button
    void Clear(int button);
    void Backspace(int button);
    void AddTalisman(int button, char tal);
    void Set(int button);
};

#endif
