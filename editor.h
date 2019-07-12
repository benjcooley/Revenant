// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  editor.h - EXILE editor routines                     *
// *************************************************************************

#ifndef _EDITOR_H
#define _EDITOR_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

#ifndef _BUTTON_H
#include "button.h"
#endif

#ifndef _FONT_H
#include "font.h"
#endif

#ifndef _SCROLL_H
#include "scroll.h"
#endif

#define PROMPT      "% "
#define MINCMDABREV 1

void StartEditor(BOOL starting = FALSE);
void ShutDownEditor();
void LoadWorldBitmap();

// *********************************************
// * TTextPane - A generic pane for text entry *
// *********************************************

_CLASSDEF(TTextPane)
class TTextPane : public TPane
{
  public:
    TTextPane(int x, int y, int w, int h) : TPane(x, y, w, h) { text = NULL; }
    ~TTextPane() {}

    virtual BOOL Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void KeyPress(int key, BOOL down);
        // For extended keys (arrows, etc)
    virtual void CharPress(int key, BOOL down);
        // Normal text input (shift-processed characters)

    BOOL InsertText(char *newtext);
        // Insert text at cursor position
    BOOL RemoveText(int numchars, BOOL forward);
        // Remove text at cursor position, in direction indicated by forward
    BOOL SetCursor(int nx, int ny, BOOL ignorebounds = FALSE);
        // Move the cursor to given position
    void UpdateWindow();
        // Check cursor position and update window to make sure it is visible

    virtual BOOL Input(char *string) { return InsertText(string); }
        // Process and then input the string to the buffer
    virtual BOOL Output(char *string) { return InsertText(string); }
        // Process and then output the string to the buffer

    int GetOffset() { return curoffset; }
        // Current cursor offset in text buffer
    int GetTextLen() { return textlen; }
        // Gets length of text buffer
    char *GetText(int start, int len, char *buf);
        // Gets text from the buffer

  // Data Members
    char *text;                                 // text being displayed/edited
    int offset;                                 // offset to top line displayed in window
    int textlen;                                // length of text in buffer
    int curoffset;                              // offset of cursor in text buffer
    int cursorx, cursory;                       // location of cursor in characters

    int curstartx, curstarty;                       // cursor bounding box - start
    int curwidth, curheight;                        // cursor bounding box - size

    int windowx, windowy;                       // window position in text buffer
    int winheight, winwidth;                    // window size
    int wrapwidth;                              // wordwrap
};

// ****************************************************
// * TScrollEditorPane - Editing text data on scrolls *
// ****************************************************

_CLASSDEF(TScrollEditorPane)
class TScrollEditorPane : public TTextPane
{
  public:
    TScrollEditorPane() : TTextPane(CONSOLEX, CONSOLEY, CONSOLEWIDTH, CONSOLEHEIGHT) {}
    ~TScrollEditorPane() {}

    virtual BOOL Initialize();
    virtual void KeyPress(int key, BOOL down);

    void SetScroll(PTObjectInstance s);

    void SaveText();
        // Save text back out to the scroll

  private:
    PTScroll scroll;
};

// ***************
// * TScriptPane *
// ***************
_CLASSDEF(TScriptPane)
class TScriptPane : public TTextPane
{
  public:
    TScriptPane() : TTextPane(CONSOLEX, CONSOLEY, CONSOLEWIDTH, CONSOLEHEIGHT) {}
    ~TScriptPane() {}

    virtual BOOL Initialize();
    virtual void KeyPress(int key, BOOL down);
        // For extended keys (arrows, etc)

    void LoadScript(PTObjectInstance oi);
        // Load up the script from the object into the text buffer
    void SaveScript();
        // Save the text buffer as the script for the object

  private:
    PTObjectInstance inst;              // Instance for script being edited
};

_STRUCTDEF(SChained)
struct SChained
{
    void (*func)(int x, int y);         // Pointer to chained function
    int xmin, xspan, ymin, yspan;       // Chaining ranges
    int oldxval, oldyval;               // In case of abort
    PSChained next, prev;               // Linked list pointers
};

_STRUCTDEF(SNewRC)
struct SNewRC
{
    char type[MAXNAMELEN];
    char dir[MAXNAMELEN];
    char graphic[MAXNAMELEN];
    int objclass;
    int set;
};

#define KILL_EVENT           0
#define CHAR_AVAILABLE_EVENT 1

_CLASSDEF(TConsolePane)
class TConsolePane : public TTextPane
{
  public:
    TConsolePane() : TTextPane(CONSOLEX, CONSOLEY, CONSOLEWIDTH, CONSOLEHEIGHT) {}
    ~TConsolePane() {}

    virtual BOOL Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void KeyPress(int key, BOOL down);
    virtual void CharPress(int key, BOOL down);
    virtual void MouseMove(int button, int x, int y);
    virtual void MouseClick(int button, int x, int y);

    void ChainMouse(void (*cfunc)(int, int), int x0 = 0, int x1 = 100, int xcur = 50,
                    int y0 = -1, int y1 = -1, int ycur = -1);
        // Mouse chaining - for adjusting values via the mouse
    void SetupMouseBitmap(int xspan, int yspan);
        // Setup new mouse arrow, if any

    void SetBounds();
        // Enter bound-adjust mode
    BOOL AdjustingBounds() { return (box != NULL); }
        // Returns whether bounding box is being edited
    BOOL AddAxis(PTObjectInstance inst);
        // Add the axis object at the location of inst
    void ClearAxis();
        // Clear out the axis object, if necessary

    virtual BOOL Input(char *string);
        // Send the string to console input
    virtual BOOL Output(char *string);
        // Output the string in the console

    // NOTE - These cannot be called from the main thread
    static int GetChar();
        // Gets a character from the input stream
    static int GetLine(char *buffer, int buffersize);
        // Gets an entire line from the input stream
    static unsigned _stdcall CommandThread(void *arg);

    PSChained head;                     // Chained mouse func list
    PSChained chained;                  // Node for list traversal
    PSChained tail;                     // Tail of list (new funcs go here)
    int cx, cy;                         // Last cursor pos
    int oldcx, oldcy;                   // Update only when changed
    int oldbuflen;                      // Mouse chain text update

    PTObjectInstance box;               // Bounding box display
    PTObjectInstance axis;              // Axis for lining up registration

    HANDLE cmdthreadhandle;             // Handle of the command processor thread
    static HANDLE cmdevents[2];         // Events used by the command processor thread
    static int cmdchar;                 // Character passed to the command processor thread
    static char *cmdline;               // Command line pointer for console
};

// *************************
// * Status Bar for Editor *
// *************************

// This is a pane running across the top of the screen which gives some basic
// object info and allows management of selected objects.

_CLASSDEF(TEditStatusPane)
class TEditStatusPane : public TButtonPane
{
  public:
    TEditStatusPane() : TButtonPane(STATUSBARX, STATUSBARY, STATUSBARWIDTH, STATUSBARHEIGHT)
        { numobjs = curobj = iterator = 0; }

    virtual BOOL Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void Hide() { TButtonPane::Hide(); SetIgnoreInput(FALSE); }

    void Validate();
        // Make sure all the selected objects are valid.

    BOOL Select(int index, BOOL add = FALSE);   // Add item to selected list
    BOOL Deselect();                            // Remove curobj from list
    BOOL Deselect(int index);                   // Search for and remove indexed item
    BOOL Reselect(int oldindex, int newindex);  // For sector updates
    BOOL SetCurObj(int newcurobj);              // Set the curobj
    BOOL Next();                                // Go to next item in list
    BOOL Prev();                                // Go to previous item in list
    void Delete();                              // Delete all items in list

    // Axis restriction buttons
    // (these assume the grid, x, y, and z buttons are first in the pane)
    BOOL GridSnap()  { return Button(0)->GetState(); }
    BOOL RestrictX() { return !Button(1)->GetState(); }
    BOOL RestrictY() { return !Button(2)->GetState(); }
    BOOL RestrictZ() { return !Button(3)->GetState(); }
    void InvertAxisButtons();
    BOOL EditWalkmap() { return Button(10)->GetState(); }

    // Object drag functions for setting flags
    void StartMoving();
    void StopMoving();

    void Undo();

    int GetSelectedObj()
        { if ((DWORD)curobj >= (DWORD)numobjs) return -1; return selected[curobj]; }
    int GetNextObj()
        { if (++iterator < numobjs) return selected[iterator]; return -1; }
    int GetFirstObj()
        { iterator = -1; return GetNextObj(); }

  private:
    int selected[MAXSELECTEDOBJS];
    int numobjs;                            // number of objects in selected array
    int curobj;                             // offset into selected array
    int iterator;                           // for findfirst, findnext

    // saved data for undo
    BOOL canundo;                           // if this isn't set, there's nothing to undo
    S3DPoint lastpos[MAXSELECTEDOBJS];      // the objects' position prior to dragging
};

// **********************
// * General Edit Tools *
// **********************

// These tools run down the lefthand side of the screen, and consist of
// general object and sector manipulation functions.  Most of the functions operate
// on the currently selected object(s) in TEditStatusPane.

#define NUMBOOKMARKS    4

_CLASSDEF(TEditToolsPane)
class TEditToolsPane : public TButtonPane
{
  public:
    TEditToolsPane() : TButtonPane(TOOLBARX, TOOLBARY, TOOLBARWIDTH, TOOLBARHEIGHT) { ClearBookmarks(); }

    virtual BOOL Initialize();
        // Only need init to set up buttons, then buttonpane takes care of the rest
    virtual void DrawBackground();
        // Clear before button draw
    virtual void KeyPress(int key, BOOL down);
        // For bookmarking
    virtual void Hide() { TButtonPane::Hide(); SetIgnoreInput(FALSE); }
        // Still want to be able to use the buttons even when hidden

    void AddBookmark(int mark, S3DPoint pos, int lev);
        // Add a new bookmark
    void GetBookmark(int mark, RS3DPoint pos, int &lev);
        // Get the given bookmark
    void ClearBookmarks();
        // Clear out all bookmarks

  protected:
    S3DPoint bookmark[NUMBOOKMARKS];        // saved positions in the map pane
    int bookmarklev[NUMBOOKMARKS];          // saved map level
};

// *********************
// * Object Class Pane *
// *********************

// This pane manipulates object types within the various classes, as well as providing
// a palette from which the user can choose object types for map creation.

_STRUCTDEF(SBitmapList)
struct SBitmapList
{
    char filename[MAXIMFNAMELEN];   // filename of imagery
    PTBitmap bm;                    // bitmap
    PSBitmapList next;              // next in list
};

_CLASSDEF(TEditClassPane)
class TEditClassPane : public TButtonPane
{
  public:
    TEditClassPane() : TButtonPane(EDMULTIPANEX, CONSOLEY, 636 - EDMULTIPANEX, CONSOLEHEIGHT)
        { curclass = curobj = firstobj = maxlines = scrollrate = accumulator = 0; clicked = FALSE; thumbnails = NULL; }

    virtual BOOL Initialize();
    virtual void Close();
    virtual void DrawBackground();
    virtual void Animate(BOOL draw);
    virtual void MouseClick(int button, int x, int y);
    virtual void MouseMove(int button, int x, int y);

    void SelClass(int newclass) { curclass = newclass; firstobj = 0; SetDirty(TRUE); }
        // Select a new class to display
    void SelObjType(int newtype) { curobj = firstobj = newtype; SetDirty(TRUE); }
        // Select a new object to display
    void SelectSame(int index);
        // Select the type and class matching the one given
    int GetClass() { return curclass; }
    int GetObjType() { return curobj; }

    void Scroll(int lines);
        // Scrolls the given number of lines in the pane

    int PutObject(S3DPoint pos);
        // Put the currect object at pos
    PTBitmap GetThumbnail(char *fname);
        // Get the thumbnail pic for an object

    int GetMaxLines() { return maxlines; }
        // Gets...well, maxlines

  private:
    int curclass;                               // selected class
    int curobj;                                 // selected object
    int firstobj;                               // first displayed obj

    int maxlines;                               // maximum lines displayable in pane
    int scrollrate;                             // scrolling
    int accumulator;                            // for < 1 fps scrolling

    PSBitmapList thumbnails;                    // Thumbnail pics to display

    BOOL clicked;                               // Whether click was in this pane
};

// Editor externals
extern PTMulti EditorData;
extern TConsolePane Console;
extern TScriptPane ScriptEditor;
extern TScrollEditorPane ScrollEditor;
extern TEditStatusPane StatusBar;
extern TEditToolsPane ToolBar;
extern TEditClassPane ClassPane;

#endif
