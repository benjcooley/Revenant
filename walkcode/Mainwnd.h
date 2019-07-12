// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   mainwnd.h - Main Window Object                      *
// *************************************************************************

#ifndef _MAINWND_H
#define _MAINWND_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

_CLASSDEF(TMainWindow)

class TMainWindow
{
  public:
  // Main Window hwnd handle
    HWND          hwnd;

    TMainWindow() { hwnd = NULL; }
    virtual ~TMainWindow();

    BOOL Initialize(HANDLE hInstance, HANDLE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
      // Create window
    void Close();
      // Destroy window
    HWND Hwnd() {return hwnd;}
};

static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam);
#endif

