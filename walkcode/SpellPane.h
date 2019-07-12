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

#define SPELLSIZE		4				// max talismans usable in a given spell

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
			int xoff = 0, int group = -1, int repeat = 0) :
		TButton(bname, bx, by, bw, bh, keypr, bfunc, dbm, ubm, rad, tog, notsquare, group, repeat)
			{ len = 0; xoffset = xoff; }

	virtual void Draw();
		// Draw the button to the screen

	void AddTalisman(int t);
		// Add a talisman to the button
	void Backspace();
		// Backspace a single talsiman
	void Clear();
		// Clear all talismans
	void Invoke();
		// Invoke the spell on the button

  protected:
	int spell[SPELLSIZE];		// spell on button
	char list[SPELLSIZE];
	int len;					// length of spell

	int xoffset;				// xoffset for printing the talisman symbols
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

  private:
	int OnTal(int x, int y);
		// Which talisman mouse is on

	BOOL shownames;						// expand names
	int startline;						// for scrolling

	int clickedtal;						// which talisman is currently clicked
	BOOL onclickedtal;					// whether mouse arrow is still on the clicked talisman
};

// *******************
// * TQuickSpellPane *
// *******************

#define NUMBUTTONS		4

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

	void Invoke(int button);
		// Invoke the spell on the given button
};

#endif
