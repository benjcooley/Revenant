// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  button.h - EXILE button routines                     *
// *************************************************************************

#ifndef _BUTTON_H
#define _BUTTON_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _SCREEN_H
#include "screen.h"
#endif

#ifndef _BITMAP_H
#include "bitmap.h"
#endif

_CLASSDEF(TButton)
class TButton
{
  public:
    TButton(char *bname, int bx, int by, int bw, int bh, WORD keypr,
            void (*bfunc)(), PTBitmap dbm = NULL, PTBitmap ubm = NULL,
            BOOL rad = FALSE, BOOL tog = FALSE, BOOL notsquare = FALSE, int group = -1, int repeat = 0);

    char *GetName() { return name; }
        // Name of the button
    BOOL OnButton(int bx, int by);
        // Check whether x, y is on the button
    BOOL IsKey(int keypr, BOOL keydown);
        // Check whether the given keypress is that of the button
    BOOL IsDirty() { return dirty; }
        // Check update status
    BOOL IsToggle() { return toggle; }
        // Returns whether button is a toggle-type
    BOOL RadioGroup() { return radiogroup; }
        // Returns the radio group or -1 if none
    BOOL Repeats() { return (repeatrate > 0); }
        // Returns if the button is a repeater
    void SetDirty() { dirty = TRUE; }
        // Sets update status
    BOOL IsHidden() { return hidden; }
    void Hide() { if (!hidden) { hidden = TRUE; SetDirty(); } }
    void Show() { if (hidden) { hidden = FALSE; SetDirty(); } }
    void SetState(BOOL newstate) { down = newstate; counter = 0; SetDirty(); }
        // Sets the button state
    BOOL GetState() { return down; }
        // Returns TRUE if the button is down, FALSE if not
    void Invert() { if (down) down = FALSE; else down = TRUE; SetDirty(); }
        // Invert button value
    void SetPosX(int nx) { x = nx; }
    void SetPosY(int ny) { y = ny; }
    void ButtonFunc() { if (buttonfunc) (*(buttonfunc))(); }
        // Call the button's function
    void SetLevel(int lev) { if (lev != level) { level = lev; SetDirty(); } }
        // Set SV level
    void SetUpBitmap(PTBitmap ubm) { upbitmap = ubm; SetDirty(); }
        // Set the up bitmap
    void SetDownBitmap(PTBitmap dbm) { downbitmap = dbm; SetDirty(); }
        // Set the down bitmap

    virtual void Draw();
        // Draw the button to the screen
    void Animate(BOOL draw = TRUE);
        // Animation pulse

  protected:
    char name[NAMELEN];                 // Button name
    int x, y, w, h;                     // Button position
    BOOL radial;                        // Circular buttons
    BOOL toggle;                        // Toggle buttons
    BOOL pixelcheck;                    // Check pixels on click
    int level;                          // SV level
    int radiogroup;                     // Radio buttons, or -1 if not radio
    int repeatrate;                     // Repeats like a keypress, or 0 for none
    int counter;                        // For repeating
    PTBitmap upbitmap;                  // Button up bitmap
    PTBitmap downbitmap;                // Button down bitmap
    WORD key;                           // Hotkey for button
    BOOL down;                          // Is button down
    BOOL dirty;                         // Needs to be redrawn
    BOOL hidden;                        // Whether button is visible or not
    void (*buttonfunc)();               // Function associated with button
};

class TButtonPane : public TPane
{
  public:
    TButtonPane(int px, int py, int pw, int ph) : TPane(px, py, pw, ph) {}
      // Create pane

    virtual BOOL Initialize();
    virtual void Close();
    virtual void KeyPress(int key, BOOL down);
    virtual void MouseClick(int button, int x, int y);
    virtual void MouseMove(int button, int x, int y);
    virtual void DrawBackground();
    virtual void Animate(BOOL draw = TRUE);

    void RedrawButtons();
        // Redraw all the buttons
    virtual void Update() { TPane::Update(); RedrawButtons(); }

    void ClearGroup(int group);
        // Clear a radio group
    void CheckGroup(int group);
        // Called on init to set the first button of a radio group down

    BOOL NewButton(PTButton b);
        // Add a new button to the list
    BOOL NewButton(char *bname, int bx, int by, int bw, int bh, WORD key, void (*bfunc)(),
                        PTBitmap dbm = NULL, PTBitmap ubm = NULL, BOOL radial = FALSE,
                        BOOL tog = FALSE, BOOL notsquare = FALSE, int radiogroup = -1, int repeat = 0)
    { return NewButton(new TButton(bname, bx, by, bw, bh, key, bfunc, dbm, ubm, radial, tog, notsquare, radiogroup, repeat)); }
        // Shortcut for normal buttons

    PTButton Button(int b) { return Buttons[b]; }

  protected:
    TPointerArray<TButton, MAXBUTTONS> Buttons;
    int clicked;                        // Index to last clicked button
};

#endif