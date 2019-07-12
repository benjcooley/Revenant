// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   cursor.h - Cursor handler file                      *
// *************************************************************************

#ifndef _CURSOR_H
#define _CURSOR_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

extern int cursorx;
extern int cursory;
extern int mousebutton;

void SetMouseBitmap(PTBitmap cursor);
void SetMouseShadow(PTBitmap shadow, int offsetx = 1, int offsety = 3);
void SetMouseCornerBitmap(PTBitmap corner, BOOL toppriority = FALSE);
void SetMouseCornerBitmap(int type, BOOL toppriority = FALSE);
void CursorOverObject(PTObjectInstance inst, BOOL toppriority = FALSE);

void SetDragBitmap(PTBitmap drag, int x = 0, int y = 0);
void ClearDragBitmap();

void SetDragObj(PTObjectInstance inst);
PTObjectInstance GetDragObj();

void DrawMouseCursor();
void DrawMouseShadow();

void RestrictCursor();
void ReleaseCursor();

#endif