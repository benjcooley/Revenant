// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  cursor.cpp - Mouse cursor handling                   * 
// *************************************************************************

#include <windows.h>

#include "revenant.h"
#include "bitmap.h"
#include "display.h"
#include "bmsurface.h"
#include "cursor.h"
#include "object.h"
#include "multi.h"
#include "playscreen.h"
#include "mainwnd.h"

// Globals for storing current mouse imagery
PTBitmap MouseCursor = NULL;
PTBitmap MouseShadow = NULL;
int shadowoffsetx = 0, shadowoffsety = 0;
PTBitmap MouseCursorAdd = NULL;     // Little picture that goes in the corner
BOOL priority;                      // Priority of mouse image

PTBitmap DragBitmap = NULL;
int grabx, graby;

PTObjectInstance DragObj = NULL;

BOOL cleardragbitmap = FALSE;

int oldcursorx = WIDTH/2, oldcursory = HEIGHT/2;    // Save cursor pos between activations

// List of cursor types
char *CursorTypes[NUMCURSORTYPES] = { "hand", "eye", "mouth", "door", "stairs", "hourglass", "swords" };


void SetMouseBitmap(PTBitmap cursor)
{
    MouseCursor = cursor;

    if (cursor == GameData->Bitmap("cursor"))
        SetMouseShadow(GameData->Bitmap("cursorshadow"));
    else
    {
        //MouseShadow = NULL;
        MouseCursorAdd = NULL;      // no corner bitmaps if not in regular cursor
        priority = TRUE;
    }
}

void SetMouseShadow(PTBitmap shadow, int offsetx, int offsety)
{
    MouseShadow = shadow;
    shadowoffsetx = offsetx;
    shadowoffsety = offsety;
}

void SetMouseCornerBitmap(PTBitmap corner, BOOL toppriority)
{
    if (corner || toppriority)
    {
        MouseCursorAdd = corner;
        priority = toppriority;
    }
}

void SetMouseCornerBitmap(int type, BOOL toppriority)
{
    if (type != CURSOR_NONE || toppriority)
        SetMouseCornerBitmap(type != CURSOR_NONE ? GameData->Bitmap(CursorTypes[type]) : NULL);
}

void SetDragBitmap(PTBitmap drag, int x, int y)
{
    DragBitmap = drag;
    grabx = x;
    graby = y;
}

void ClearDragBitmap()
{
    cleardragbitmap = TRUE;
}

void SetDragObj(PTObjectInstance inst)
{
    DragObj = inst;
}

PTObjectInstance GetDragObj()
{
    return DragObj;
}

void DrawMouseShadow(int x, int y, int width, int height)
{
    Display->SetClipRect(x, y, width, height);
    Display->Put(cursorx + shadowoffsetx, cursory + shadowoffsety, MouseShadow, DM_TRANSPARENT | DM_USEREG | DM_ALIAS);
}

void DrawMouseCursor()
{
    // draw the bitmap first so that the cursor appears over the top of it
    if (DragBitmap)
        Display->Put(cursorx - grabx, cursory - graby, DragBitmap, DM_TRANSPARENT | DM_USEREG);

    if (MouseCursor && !MouseCursorAdd)
        Display->Put(cursorx, cursory, MouseCursor, DM_TRANSPARENT | DM_USEREG | DM_ALIAS);

    if (MouseCursorAdd)
        Display->Put(cursorx, cursory, MouseCursorAdd, DM_TRANSPARENT | DM_USEREG | DM_ALIAS);

    if (cleardragbitmap)
    {
        SetDragBitmap(NULL);
        cleardragbitmap = FALSE;
    }

    MouseCursorAdd = NULL;
    priority = FALSE;
}

void DrawMouseShadow()
{
    if (MouseShadow)
    {
        Display->SetOrigin(0, 0);

        if (MouseShadow != GameData->Bitmap("cursorshadow"))
            DrawMouseShadow(MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT);
        else
        {
            if (PlayScreen.InCompleteExclusion())
                DrawMouseShadow(0, 0, WIDTH, HEIGHT);
            else
            {
                if (cursorx <= MAPPANEX)
                    DrawMouseShadow(0, 0, MAPPANEX, HEIGHT);
                else if ((cursorx + MouseShadow->width) >= (MAPPANEX + MAPPANEWIDTH))
                    DrawMouseShadow(MAPPANEX + MAPPANEWIDTH, 0, WIDTH - (MAPPANEX + MAPPANEWIDTH), HEIGHT);

                if (cursory <= MAPPANEY)
                    DrawMouseShadow(0, 0, WIDTH, MAPPANEY);
                else if ((cursory + MouseShadow->height) >= (MAPPANEY + MAPPANEHEIGHT))
                    DrawMouseShadow(0, MAPPANEY + MAPPANEHEIGHT, WIDTH, HEIGHT - (MAPPANEY + MAPPANEHEIGHT));
            }
        }

        Display->ResetClipRect();
    }
}

void CursorOverObject(PTObjectInstance inst, BOOL toppriority)
{
    if (toppriority || !priority)
    {
        int type = CURSOR_NONE;

        if (inst)
            type = inst->CursorType(DragObj);

        SetMouseCornerBitmap(type, toppriority);
    }
}

void RestrictCursor()
{
    // center the cursor
    RECT r;
    GetClientRect(MainWindow.Hwnd(), &r);
    ClientToScreen(MainWindow.Hwnd(), (LPPOINT)&r);
    SetCursorPos(r.left + oldcursorx, r.top + oldcursory);

    // clip if not windowed
    if (!Windowed || Borderless)
    {
        r.right = r.left + WIDTH;
        r.bottom = r.top + HEIGHT;
        ClipCursor(&r);
    }
}

void ReleaseCursor()
{
    // save the old position for restore after reactivation
    oldcursorx = cursorx;
    oldcursory = cursory;

    // clear out the clip rect (cursor can move anywhere)
    ClipCursor(NULL);
}

