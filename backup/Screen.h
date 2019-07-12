// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 screen.h  - EXILE Screen Include File                 *
// *************************************************************************

#ifndef _SCREEN_H
#define _SCREEN_H

#define NUMEXCLUSIVEPANES 4

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _DIRECTINPUT_H
#include "directinput.h"
#endif

// ******************************
// * TPane - Screen pane object *
// ******************************

// The pane object creates a virtual view pane on a game screen.  The
// view pane has its own origin, its own clipping rectangle, and handles
// its own mouse movement, clicks, joystick, and keyboard input.

_CLASSDEF(TPane)
class TPane
{
  private:
    int  x, y, width, height;               // Pane's position in screen coordinates
    int  newx, newy, newwidth, newheight;   // Size and position to change to on next frame
    int  oldscrollx, oldscrolly;// Previous scroll position
    int  scrollx, scrolly;      // Current scroll position
    int  newscrollx, newscrolly;// Next scroll position
    PTScreen screen;            // Screen pane is on
    BOOL isopen;                // Pane is currently active
    BOOL hidden;                // Flag set if Pane is hidden
    BOOL ignoreinput;           // To allow hidden panes to still process input
    BOOL dirty;                 // Pane needs update
    int  backgroundbuffer;      // Background buffer index

   public:

    TPane() {}
    TPane(int px, int py, int pw, int ph, BOOL phide = FALSE)
      { newx = x = px; newy = y = py; newwidth = width = pw; newheight = height = ph; hidden = phide; }
      // Create pane

  // Init & Close functions
    virtual BOOL Initialize();
      // Initializes pane before it is displayed
    virtual void Close();
      // Called for pane to delete internal structures before closing
    BOOL IsOpen() { return isopen; }

  // Pane state variables
    int GetPosX() { return x; }
      // Gets pane screen x pos
    int GetPosY() { return y; }
      // Gets pane screen y pos
    int GetWidth() { return width; }
      // Gets width
    int GetHeight() { return height; }
      // Gets height
    void GetRect(SRect &r) { r.left = x; r.top = y; r.right = r.left + width - 1; r.bottom = r.top + height - 1; }
      // Gets the pane's rectangle
    BOOL InPane(int x, int y) { return (x >= 0 && y >= 0 && x < width && y < height); }
      // Quick bounds checker
    BOOL IsDirty() { return dirty; }
      // Get update status

  // Scrolling functions
    void SetScrollPos(int sx, int sy) { newscrollx = sx; newscrolly = sy; }
      // Sets scroll position of pane for next frame (returned with GetNewScrollX, ScrollY)         
    int GetScrollX() { return scrollx; }
      // Gets pane draw origin x pos for current frame
    int GetScrollY() { return scrolly; }
      // Gets pane draw origin y pos for current frame
    int GetOldScrollX() { return oldscrollx; }
      // Gets pane draw origin x pos for previous frame
    int GetOldScrollY() { return oldscrolly; }
      // Gets pane draw origin y pos for previous frame
    int GetNewScrollX() { return newscrollx; }
      // Gets pane draw origin x pos for next frame
    int GetNewScrollY() { return newscrolly; }
      // Gets pane draw origin y pos for next frame

  // Background stuff
    virtual void CreateBackgroundBuffers() {}
      // Allocates any background buffers used by the pane
    virtual void FreeBackgroundBuffers() {}
      // Frees any background buffers used by the pane
    virtual void SetBackgroundBuffer(int buf) { backgroundbuffer = buf; }
      // Sets the special primary background buffer for this pane (used for scrolling)
    virtual void ClearBackgroundBuffer() { backgroundbuffer = -1; }
      // Clears the special primary background buffer for this pane
    int GetBackgroundBuffer() { return backgroundbuffer; }
      // Gets restore buffer index.
    void UpdateBackgroundScrollPos();
      // Called by the screen TimerTick() function to update bg buf's scroll pos
    void DrawRestoreRect(int x, int y, int width, int height, 
        DWORD drawmode =  DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER);
      // Restores the rectangle x,y,width,height to the screen for this pane

  // Pane display functions
    void PutToScreen();
      // Causes the pane to be immediately shown on the screen.  Useful for when
      // the pane contains a status or 'loading' bar that is updated during a single
      // timer tick.
    void Draw();
      // Manually calls the Pulse(), DrawBackground(), and Animate() functions.  Useful
      // for drawing within a timer tick (like a loading bar)
    void Resize(int nx, int ny, int nwidth, int nheight)
        { newx = nx; newy = ny; newwidth = nwidth; newheight = nheight; }
      // Resizes pane for next frame
    void SetPos(int nx, int ny) { newx = nx; newy = ny; }
      // Sets the pane's x, y position for next frame
    void SetSize(int nwidth, int nheight) { newwidth = nwidth; newheight = nheight; }
      // Sets the pane's width, height position for next frame
    BOOL WasResized()
        { return x != newx || y != newy || width != newwidth || height != newheight; }
      // True if any changes to position or size was made during the previous frame
    virtual void SetClipRect();
      // Sets the display's origin and clipping rectangle to clip this pane
    virtual void Update() { dirty = TRUE; }
      // Flag the pane as needing a background update
    virtual void SetDirty(BOOL newdirty) { dirty = newdirty; }
      // For setting the update status explicitly
    
    virtual void Show() { hidden = FALSE; ignoreinput = FALSE; Update(); }
      // Draws pane's imagery to backbuffer
    virtual void Hide() { hidden = TRUE; ignoreinput = TRUE; }
      // Erases pane's imagery from backbuffer
    virtual void SetIgnoreInput(BOOL val) { ignoreinput = val; }
      // So that the child classes can set it manually
    virtual BOOL IsHidden() { return hidden || !IsOnScreen(); }
      // Returns the pane's visibility status
    virtual BOOL IsOnScreen();
      // Returns the pane's visibility status
    virtual BOOL IsIgnoringInput() { return ignoreinput; }
      // Whether or not the pane is processing input (keyboard only)
    PTScreen Screen() { return screen; }
      // Returns the screen this pane is currently on
    void SetScreen(PTScreen newscreen) { screen = newscreen; }
      // Called to set the pane's screen (CALLED ONLY BY SCREEN FUNCTIONS!!!)

  // Pane virtual handler functions
    virtual void PaneResized()
        { FreeBackgroundBuffers(); 
          x = newx; y = newy; width = newwidth; height = newheight; 
          CreateBackgroundBuffers(); 
         }
      // Called before anything else to resize the pane
    virtual void Pulse() {}
      // Called to cause pane to do object AI, set pane position, etc. before DrawBackground() is called
    virtual void DrawBackground() {}
      // Called before animation is drawn to allow drawing to background buf
    virtual void Animate(BOOL draw) {}
      // Animate function called every timer tick (draw is false if this frame is skipped)
    virtual void Overlay(BOOL draw) {}
      // Animate function called after 2d and 3d objects are rendered
    virtual void MouseClick(int button, int x, int y) {}
      // Handles mouse clicks in pane.
    virtual void MouseMove(int button, int x, int y) {}
      // Handles mouse movement in pane.
    virtual void KeyPress(int key, BOOL down) {}
      // Handles keyboard presses in pane.
    virtual void CharPress(int key, BOOL down) {}
      // Handles processed (ASCII) keyboard presses in pane.
    virtual void Joystick(int key, BOOL down) {}
      // Handles joystick input in pane.

  // Converts points in pane to points on the screen
    void PaneToScreen(int panex, int paney, int &screenx, int &screeny)
      { screenx = x + panex - scrollx; screeny = y + paney - scrolly; }
    void PaneToScreen(SRect &r) 
      { r.left = x + r.left - scrollx; r.top = y + r.top - scrolly;
        r.right = x + r.right - scrollx; r.bottom = y + r.bottom - scrolly; }
};

// ********************************
// * TScreen - Game screen object *
// ********************************

// The TScreen object represents a game screen.  Each game screen can access
// the display surface and handle input directly, or it can have multiple
// TPane objects which handle various parts of the screen.

#define MAXSCREENPANES 32

typedef TPointerArray<TPane, MAXSCREENPANES> TPaneArray;
typedef TPointerIterator<TPane> TPaneIterator;

_CLASSDEF(TScreen)
class TScreen
{
  protected:
    TPaneArray panes;                   // Array of panes
    int exclusive[NUMEXCLUSIVEPANES];   // Current exclusive pane list or NULL if no exclusive.
    BOOL complete[NUMEXCLUSIVEPANES];   // Whether the exclusive pane is completely exclusive
    int curexclusive;                   // Current exclusive pane
    int numexclusive;                   // Number of exclusive panes
    PTScreen nextscreen;                // Pointer to nextscreen
    BOOL firstframe;                    // True just after screen is initialized before first frame
    BOOL dirty;                         // Needs redraw
    int screenframes;                   // Number of ticks since screen initialized

  public:
    TScreen();
    virtual ~TScreen();

  // Initialization
    virtual BOOL Initialize() { return FALSE; }
      // Initializes the screen.
    virtual void Close() {}
      // Closes the screen.
    void PutToScreen();
      // Causes the back buffer to be immediately shown on the users screen.  Useful for when
      // the screen contains a status or 'loading' bar that is updated during a single
      // timer tick.

  // Pane stuff
    int FindPane(PTPane pane);
      // Returns the index to the given pane in the screen, or -1 if none
    int AddPane(PTPane pane, int panenum = -1);
      // Adds pane to screen at given panenum (or bottom if panenum = -1)
    BOOL RemovePane(PTPane pane);
      // Removes pane from the pane list (returns TRUE if pane was actually in pane list)
    BOOL SetExclusivePane(int panenum, BOOL completeexclusion = FALSE);
      // Sets pane to handle all input/output (for error or popup panes)
      // If complete is TRUE then *nothing* from the other panes (including
      // Animate() and DrawBackground()) will be called during exclusive mode.
    BOOL SetExclusivePane(PTPane pane, BOOL completeexclusion = FALSE)
        { return SetExclusivePane(FindPane(pane), completeexclusion); }
      // Sets pane to handle all input/output (for error or popup panes)
    void ReleaseExclusivePane(int panenum);
      // Releases exclusive pane
    void ReleaseExclusivePane(PTPane pane) { ReleaseExclusivePane(FindPane(pane)); }
      // Releases exclusive pane
    BOOL InCompleteExclusion() { return (numexclusive > 0 && complete[curexclusive]); }
      // Whether or not all i/o is stopped except for one pane
    BOOL FirstFrame() { return firstframe; }
      // Is this the first frame for this screen?
    void RedrawAllPanes();
      // Redraw all non-hidden panes

   // Next screen stuff
    void SetNextScreen(PTScreen screen) {nextscreen = screen;}
      // Sets nextscreen variable
    PTScreen GetNextScreen() {return nextscreen;}
      // Gets nextscreen variable

  // Virtual handlers
    virtual void Pulse();
      // Called to cause pane to do object AI, set pane position, etc. before DrawBackground() is called
    virtual void DrawBackground();
      // Draws to background before animation is drawm
    virtual void Animate(BOOL draw);
      // Animate function called every timer tick.
    virtual void MouseClick(int button, int x, int y);
      // Handles mouse clicks to screen. Calls TPane MouseClick functions.
    virtual void MouseMove(int button, int x, int y);
      // Handles mouse movement on screen.  Calls TPane MouseMove functions.
    virtual void KeyPress(int key, BOOL down);
      // Handles keyboard presses. Calls TPane KeyPress functions.
    virtual void CharPress(int key, BOOL down);
      // Handles shifted keypresses.
    virtual void Joystick(int key, BOOL down);
      // Handles joystick input. Calls TPane Joystick functions.
    virtual void Redraw() { dirty = TRUE; }
      // Redraw the current screen

  // Screen loops
    virtual BOOL TimerTick(BOOL draw);
      // Performs required actions every timer tick (calls DrawBackground() and Animate())
    virtual BOOL TimerLoop(long ticks);
      // Wait for ticks to pass (Calls TimerTick())

    static PTScreen ShowScreen(PTScreen screen, int ticks);
      // Shows screen

  // Get screen frames
    int FrameCount() { return screenframes; }
      // Get the current frame number since this screen was initialized
    void ResetFrameCount() { screenframes = 0; }
      // Resets the screen framecount

  private:
    BOOL BeginScreen();
      // Calls all pane and screen initialize functions
    void EndScreen();
      // Calls all pane and screen close functions
};

#endif
