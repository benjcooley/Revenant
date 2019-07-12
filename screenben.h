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

// ****************************
// * SPaneMsg - A pane messge *
// ****************************

enum
{
    PM_NONE,
    PM_ACTIVATE,
    PM_DEACTIVATE,
    PM_SETFOCUS,
    PM_KILLFOCUS,
    PM_MOVE,
    PM_RESIZE,
    PM_PULSE,
    PM_DRAWBG,
    PM_ANIMATE,
    PM_RENDER,
    PM_OVERLAY,
    PM_MOUSECLICK,
    PM_MOUSEMOVE,
    PM_KEYPRESS,
    PM_CHARPRESS,
    PM_JOYSTICK
} PANEMSG;

_STRUCTDEF(SPaneMsg)
struct SPaneMsg
{
    PANEMSG msg;
    PTPane pane;
    
    SPaneMsg() {}
    SPaneMsg(PANEMSG m, PTPane p) { msg = m; pane = p };
};

_STRUCTDEF(SPaneMsgMove)
struct SPaneMsgMove : public SPaneMsg
{
    int newx, newy;

    SPaneMsgMove() {}
    SPaneMsgMove(PANEMSG m, PTPane p, int nx, int ny) : SPaneMsg(m, p) 
      { newx = nx; newy = ny; }
};

_STRUCTDEF(SPaneMsgResize)
struct SPaneMsgMove : public SPaneMsg
{
    int newx, newy, newwidth, newheight;

    SPaneMsgResize() {}
    SPaneMsgResize(PANEMSG m, PTPane p, int nx, int ny, int nw, int nh) : 
      SPaneMsg(m, p) { newx = nx; newy = ny; newwidth = nw; newheight = nh; }
};

_STRUCTDEF(SPaneMsgMouse)
struct SPaneMsgMouse : public SPaneMsg
{
    int button, x, y;

    SPaneMsgMouse() {}
    SPaneMsgMouse(PANEMSG m, PTPane p, int b, int nx, int ny) : 
      SPaneMsg(m, p) { button = b; x = nx; y = ny; }
};

_STRUCTDEF(SPaneMsgKeyPress)
struct SPaneMsgKeyPress : public SPaneMsg
{
    int key;
    BOOL down;

    SPaneMsgKeyPress() {}
    SPaneMsgKeyPress(PANEMSG m, PTPane p, int k, BOOL d) : 
      SPaneMsg(m, p) { key = k; down = d; }
};

_STRUCTDEF(SPaneMsgPulse)
struct SPaneMsgKey : public SPaneMsg
{
    BOOL draw;

    SPaneMsgPulse() {}
    SPaneMsgPulse(PANEMSG m, PTPane p, BOOL d) : 
      SPaneMsg(m, p) { draw = d; }
};

// ******************************
// * TPane - Screen pane object *
// ******************************

// The pane object creates a virtual view pane on a game screen.  The
// view pane has its own origin, its own clipping rectangle, and handles
// its own mouse movement, clicks, joystick, and keyboard input.

#define PF_ISOPEN      (1<<0)   // This pane is currently active
#define PF_HIDDEN      (1<<1)   // This pane is hidden (not shown)
#define PF_DISABLED    (1<<2)   // This pane is disabled (shown, but inactive)
#define PF_DIRTY       (1<<3)   // This pane is dirty - needs to be redrawn
#define PF_ACTIVE      (1<<4)   // This pane is the current input focus
#define PF_PULSE       (1<<5)   // This pane's Pulse() function should be called
#define PF_DRAWBG      (1<<6)   // This pane's DrawBg() function should be called
#define PF_ANIMATE     (1<<7)   // This pane's Animate() function should be callled
#define PF_RENDER      (1<<8)   // This pane's Render() function should be called
#define PF_OVERLAY     (1<<9)   // This pane's Overlay() function should be called

_CLASSDEF(TPane)
class TPane
{
  private:
    DWORD flags;                            // Pane flags
    TPaneArray panes;                       // Array of sub panes
    PTPane activepane;                      // Current active pane
    PTScreen screen;                        // Top level screen pane is on
    int  x, y, width, height;               // Pane's position in screen coordinates

    int  oldscrollx, oldscrolly;            // Previous scroll position
    int  scrollx, scrolly;                  // Current scroll position
    int  newscrollx, newscrolly;            // Next scroll position
    int  backgroundbuffer;                  // Background buffer index

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

  // Pane stuff
    int NumPanes() { return panes.NumItems(); }
      // Returns number of panes
    PTPane GetPane(int index) { return panes[index]; }
      // Returns the index to the given pane in the screen, or -1 if none
    PTPane FindPane(char *name);
      // Returns the index to the given pane in the screen, or -1 if none
    int AddPane(PTPane pane, int panenum = -1);
      // Adds pane to screen at given panenum (or bottom if panenum = -1)
    BOOL RemovePane(PTPane pane);
      // Removes pane from the pane list (returns TRUE if pane was actually in pane list)
    BOOL SetActivePane(PTPane pane);
      // Sets the current active pane (returns TRUE if pane was set)

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

  // Movement and resizing functions (causes resize/pos messages to be posted to CurrentScreen)
    void Resize(int nx, int ny, int nwidth, int nheight);
      // Resizes pane for next frame
    void SetPos(int nx, int ny);
      // Sets the pane's x, y position for next frame
    void SetSize(int nwidth, int nheight);
      // Sets the pane's width, height position for next frame

  // Pane display functions
    void Draw(BOOL puttoscreen = TRUE);
      // Manually sends PM_PULSE, PM_DRAWBG, PM_ANIMATE, PM_RENDER, and PM_OVERLAY functions
      // for drawing within a timer tick (like a loading bar).  If 'puttoscreen' is TRUE,
      // copies off screen buffer immediately to screen as well.
    void PutToScreen();
      // Causes the pane to be immediately shown on the screen.  Useful for when
      // the pane contains a status or 'loading' bar that is updated during a single
      // timer tick.

  // Clipping functions
    virtual void SetClipRect();
      // Sets the display's origin and clipping rectangle to clip this pane
    virtual void Update() { dirty = TRUE; }
      // Flag the pane as needing a background update
    virtual void SetDirty(BOOL newdirty) { dirty = newdirty; }
      // For setting the update status explicitly
    
  // Flags functions
    DWORD GetFlags() { return flags; }
      // Gets the current flags
    virtual SetFlags(DWORD flags, BOOL on = TRUE);
      // Basic set flag function
    void ClearFlags(DWORD flags, BOOL on) { SetFlags(flags, FALSE); }
      // Quick function for clearing flags
    void Show() { ClearFlags(PF_HIDDEN | PF_DISABLED); Update(); }
      // Draws pane's imagery to backbuffer
    void Hide() { SetFlags(PF_HIDDEN | PF_DISABLED); }
      // Erases pane's imagery from backbuffer
    void Disable(BOOL on) { SetFlags(PF_DISABLED, on); }
      // So that the child classes can set it manually
    BOOL IsHidden() { return (flags & PF_HIDDEN) || !IsOnScreen(); }
      // Returns the pane's visibility status
    BOOL IsOnScreen() { screen == CurrentScreen; }
      // Returns the pane's visibility status
    BOOL IsDisabled() { return flags & PF_DISABLED; }
      // Whether or not the pane is processing input (keyboard only)

  // Parent functions
    PTPane Parent() { return parent; }
      // Returns the screen this pane is currently on
    void SetParent(PTPane newparent) { parent = newparent; }
      // Called to set the pane's screen (CALLED ONLY BY AddPane() and RemovePane()!!!)

  // Screen functions
    PTScreen Screen() { return screen; }
      // Returns the screen this pane is currently on
    void SetScreen(PTScreen newscreen) { screen = newscreen; }
      // Called to set the pane's screen (CALLED ONLY BY AddPane() and RemovePane()!!!)

  // Message handler function
    virtual void SendMessage(SPaneMsg &msg);
      // Called by system to send a message to this pane.  Will either call its
      // message handler, and/or
    virtual BOOL Message(SPaneMsg &msg);
      // Pane message handler function

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
    virtual void Render() {}
      // Animate function called every timer tick (draw is false if this frame is skipped)
    virtual void Overlay(BOOL draw) {}
      // Animate function called after 2d and 3d objects are rendered

  // Pane input functions
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
class TScreen : public TPane
{
  protected:
    PTScreen nextscreen;                // Pointer to nextscreen
    BOOL firstframe;                    // True just after screen is initialized before first frame
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
