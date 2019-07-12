// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  MainWnd.cpp - Main Window Module                     *
// *************************************************************************

#include <windows.h>

#include <mmsystem.h>
#include <stdio.h>
#include <string.h>

#include "multimon.h"

#include "revenant.h"
#include "rc.h"
#include "graphics.h"
#include "display.h"
#include "imagery.h"
#include "mainwnd.h"
#include "mappane.h"
#include "screen.h"
#include "player.h"
#include "cursor.h"
#include "videocap.h"

#define ID_ICON 101

BOOL AppActive;         // Flag for if the game is the active application

// Used to set the cursor position for "screen.h"
extern BOOL InitDirectDraw();
extern void CloseSystem();

// *** Main Window Procedure ***

/* Take all the parameters for WndMain, process them, create a window
 * that takes up the whole screen, then return. */

BOOL TMainWindow::Initialize(HANDLE hInstance, HANDLE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow)
{
  // Set instance handles
    ::hInstance = hInstance;

    if (Windowed == TRUE && !Borderless)
    {
     // Make a class
        WNDCLASS c;
        c.style         = CS_HREDRAW | CS_VREDRAW;
        c.lpfnWndProc   = MainWndProc;
        c.cbClsExtra    = 0;
        c.cbWndExtra    = 0;
        c.hInstance     = hInstance;
        c.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROGRAMICON));
        c.hCursor       = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_BLANKCURSOR));
        c.hbrBackground = NULL;
        c.lpszMenuName  = NULL;
        c.lpszClassName = "REVENANTClass";
        RegisterClass(&c);

        DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_BORDER; 
        DWORD exstyle = WS_EX_OVERLAPPEDWINDOW;

        RECT r;
        r.left = MonitorX + ((MonitorW - WIDTH) / 2);
        r.top = MonitorY + ((MonitorH - HEIGHT) / 2);
        r.right = r.left + WIDTH;
        r.bottom = r.top + HEIGHT;

        AdjustWindowRectEx(&r, style, TRUE, exstyle);
        
        r.bottom -= GetSystemMetrics(SM_CYMENU); // Get rid of menu bar space

      // Make the main window
        hwnd = CreateWindowEx(exstyle, "REVENANTClass", "Revenant", style, 
                              r.left, r.top, r.right - r.left, r.bottom - r.top,
                              NULL, NULL /*LoadMenu(hInstance, MAKEINTRESOURCE(IDR_PROGRAMMENU))*/,
                              hInstance, NULL);
    }
    else if (Windowed == TRUE && Borderless)
    {
     // Make a class
        WNDCLASS c;
        c.style         = CS_HREDRAW | CS_VREDRAW;
        c.lpfnWndProc   = MainWndProc;
        c.cbClsExtra    = 0;
        c.cbWndExtra    = 0;
        c.hInstance     = hInstance;
        c.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROGRAMICON));
        c.hCursor       = LoadCursor(hInstance, MAKEINTRESOURCE(IDC_BLANKCURSOR));
        c.hbrBackground = NULL;
        c.lpszMenuName  = "REVENANTClass";
        c.lpszClassName = "REVENANTClass";
        RegisterClass(&c);

        DWORD style = WS_POPUP;
        DWORD exstyle = 0;

        RECT r;
        r.left = MonitorX + ((MonitorW - WIDTH) / 2);
        r.top = MonitorY + ((MonitorH - HEIGHT) / 2) + 19;
        r.right = r.left + WIDTH;
        r.bottom = r.top + HEIGHT;

        AdjustWindowRectEx(&r, style, TRUE, exstyle);

      // Make the main window
        hwnd = CreateWindowEx(exstyle, "REVENANTClass", "Revenant", style, 
                              r.left, r.top, r.right - r.left, r.bottom - r.top,
                              NULL, NULL, hInstance, NULL);
    }
    else
    {
     // Make a class
        WNDCLASS c;
        c.style         = CS_HREDRAW | CS_VREDRAW;
        c.lpfnWndProc   = MainWndProc;
        c.cbClsExtra    = 0;
        c.cbWndExtra    = 0;
        c.hInstance     = hInstance;
        c.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROGRAMICON));
        c.hCursor       = NULL;
        c.hbrBackground = NULL;
        c.lpszMenuName  = "REVENANTClass";
        c.lpszClassName = "REVENANTClass";
        RegisterClass(&c);

      // Make the main window
       hwnd = CreateWindowEx(WS_EX_TOPMOST, "REVENANTClass", "Revenant", WS_POPUP,
                             MonitorX, MonitorY, WIDTH, HEIGHT,
                             NULL, NULL, hInstance, NULL);

        ShowCursor(FALSE);
    }

    if (!hwnd)
        FatalError("Couldn't create main window");

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    SetFocus(hwnd);

    return TRUE;
}

void TMainWindow::Close()
{
    if (hwnd)
        hwnd = NULL;
}

TMainWindow::~TMainWindow()
{
    if (hwnd)
        hwnd = NULL;
}

LRESULT CALLBACK MainWndProc(HWND hWnd, UINT wMessage, WPARAM wParam, LPARAM lParam)
{
    long result = 0;
    int x, y;//, button;

    switch (wMessage)
    {
      case WM_ACTIVATEAPP:
      {
        AppActive = wParam;
        result = DefWindowProc(hWnd, wMessage, wParam, lParam);

        if (AppActive)
        {
            if (PauseWhenNotActive)
                ResumeThreads();
            RestrictCursor();
            Display->Restore();
            TObjectImagery::RestoreAll();
            if (CurrentScreen)
                CurrentScreen->Redraw();
        }
        else
        {
            if (PauseWhenNotActive)
                PauseThreads();
            ReleaseCursor();
        }

        break;
      }

      case WM_CREATE:
      {
        break;
      }

      case WM_DESTROY:
      {
        MainWindow.hwnd = NULL;
        PostQuitMessage(0);
        ShowCursor(TRUE);
        Closing = TRUE;
        break;
      }

      case WM_PAINT:
      {
        if (Windowed)
        {
            Display->FlipPage();
        }
      }

      case WM_SYSCOMMAND:
      { 
        if((wParam & 0xFFF0) == SC_SCREENSAVE || (wParam & 0xFFF0) ==
            SC_MONITORPOWER)
            return 0;
        result = DefWindowProc(hWnd, wMessage, wParam, lParam);
        break;
      }

      case WM_KEYDOWN:
      {
        if (wParam == VK_CONTROL)
            CtrlDown = TRUE;
        else if (wParam == VK_MENU)
            AltDown = TRUE;
        else if (wParam == VK_SHIFT)
            ShiftDown = TRUE;

        if (!AppActive)
            return 0;

        if (LOWORD(lParam) > 1)
            break;

        // Handle system keypresses
        if (CtrlDown && ShiftDown)
        {
            switch (wParam)
            {
                case ' ':   {TOGGLE(Show3D);                break;}
                case 'D':   {TOGGLE(ShowDrawing);           break;}
                /*case 'N': {TOGGLE(NoNormals);
                             MapPane.ReloadImagery();       break;}*/
                case 'F':   {TOGGLE(ShowFramesPerSecond);   break;}
                case 'T':   {TOGGLE(DisableTimer);          break;}
                case 'S':   {TOGGLE(FlatShade);             break;}
                case 'E':   {TOGGLE(DitherEnable);          break;}
                case 'B':   {TOGGLE(BlendEnable);           break;}
                case 'H':   {TOGGLE(SpecularEnable);        break;}
                case 'X':   {TOGGLE(UseTextures);           break;}
                case 'Z':   {TOGGLE(ZEnable);               break;}
                case 'L':   {TOGGLE(BilinearFilter);        break;}
                case 'K':   {TOGGLE(NoFrameSkip);           break;}
                case 'C':   {TOGGLE(ClearBeforeDraw);       break;}
                case 'Q':   {TOGGLE(NoScrollZBuffer);       break;}
                case 'O':   {TOGGLE(SmoothScroll);          break;}
                case 'U':   {TOGGLE(NoUpdateRects);         break;}
                case 'R':   {TOGGLE(ScrollLock);            break;}
                case 'G':   {TOGGLE(GridSnap);              break;}
//              case 'P':   {TOGGLE(UseDrawPrimitive);      break;} // This doesn't work right now
                case 'M':   {TOGGLE(NoPulseObjs);           break;}
                case 'N':   {TOGGLE(NoAnimateObjs);         break;}
                case 'I':   {TOGGLE(Interpolate);           break;}
                case 'A':   {TOGGLE(NoAI);                  break;}
                case '2':   {TOGGLE(Double3D);              break;}
                case '3':   {TOGGLE(Triple3D);              break;}
                case '0':   {TOGGLE(UseDirLight);           break;}

                case 'V':
                  {
                    if (VideoCapture.IsCapturing())
                        VideoCapture.Stop();
                    else
                        VideoCapture.Start();
                    break;
                  }

              // Special... make this one hard to hit since it will stop screen updating
                case VK_BACK: {TOGGLE(DoPageFlip);          break;}

            }

            if (!UseDirect3D2) // Can't toggle drawprimitive if using direct3D 2
                UseDrawPrimitive = FALSE;

            break;
        }

        // check for cheat codes
        if (ShiftDown && !Editor && wParam != VK_SHIFT)
        {
            static char keywordbuf[11];
            static int end = 0;

            if ((char)wParam == VK_RETURN)
            {
                if (!strcmp(keywordbuf, "HOG"))
                {
                    if (Player)
                        Player->GetOnYerHog();
                }

                keywordbuf[end = 0] = 0;
            }
            else
            {
                if (end < 10)
                {
                    keywordbuf[end++] = (char)wParam;
                    keywordbuf[end] = 0;
                }
            }

            return 0;
        }

        if (CurrentScreen)
            CurrentScreen->KeyPress(wParam, TRUE);

        break;
      }

      case WM_CHAR:
      {
        if (!AppActive)
            return 0;

        if (CtrlDown && ShiftDown)
            break;

        if (CurrentScreen)
            CurrentScreen->CharPress(wParam, TRUE);
        break;
      }

      case WM_KEYUP:
      {
        if (wParam == VK_CONTROL)
            CtrlDown = FALSE;
        else if (wParam == VK_MENU)
            AltDown = FALSE;
        else if (wParam == VK_SHIFT)
            ShiftDown = FALSE;

        if (!AppActive)
            return 0;

        if (CtrlDown && ShiftDown)
            break;;

        if (CurrentScreen)
            CurrentScreen->KeyPress(wParam, FALSE);
        break;
      }

      case WM_SHOWWINDOW: 
      {
        if (Windowed == TRUE)
            result = DefWindowProc(hWnd, wMessage, wParam, lParam);
        break;
      }

/*
      case WM_MOUSEMOVE:
      {
        if (wParam & MK_LBUTTON) button = MB_LEFTDOWN;
        else if (wParam & MK_RBUTTON) button = MB_RIGHTDOWN;
        else if (wParam & MK_MBUTTON) button = MB_MIDDLEDOWN;

        x = LOWORD(lParam);
        y = HIWORD(lParam);
        cursorx = x;
        cursory = y;

        if (CurrentScreen)
            CurrentScreen->MouseMove(button, x, y);
        break;
      }
*/

      case WM_LBUTTONDOWN:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_LEFTDOWN, x, y);
        mousebutton |= MB_LEFTDOWN;
        break;
      }

      case WM_LBUTTONUP:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_LEFTUP, x, y);
        mousebutton &= ~MB_LEFTDOWN;
        break;
      }

      case WM_LBUTTONDBLCLK:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_LEFTDBLCLK, x, y);
        break;
      }

      case WM_MBUTTONDOWN:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_MIDDLEDOWN, x, y);
        mousebutton |= MB_MIDDLEDOWN;
        break;
      }

      case WM_MBUTTONUP:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_MIDDLEUP, x, y);
        mousebutton &= ~MB_MIDDLEDOWN;
        break;
      }

      case WM_MBUTTONDBLCLK:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_MIDDLEDBLCLK, x, y);
        break;
      }

      case WM_RBUTTONDOWN:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_RIGHTDOWN, x, y);
        mousebutton |= MB_RIGHTDOWN;
        break;
      }

      case WM_RBUTTONUP:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_RIGHTUP, x, y);
        mousebutton &= ~MB_RIGHTDOWN;
        break;
      }

      case WM_RBUTTONDBLCLK:
      {
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        //cursorx = x;
        //cursory = y;

        if (!AppActive)
            return 0;

        if (CurrentScreen)
            CurrentScreen->MouseClick(MB_RIGHTDOWN, x, y);
        break;
      }

      default:
      {
        result = DefWindowProc(hWnd, wMessage, wParam, lParam);
        break;
      }

    }

    return result;
}


