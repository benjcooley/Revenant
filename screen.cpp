// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                screen.cpp  - EXILE Screen Object File                 *
// *************************************************************************

#include <stdio.h>
#include <windows.h>

#include "revenant.h"
#include "bitmap.h"
#include "display.h"
#include "mainwnd.h"
#include "timer.h"
#include "cursor.h"
#include "mappane.h"
#include "screen.h"
#include "sound.h"
#include "videocap.h"

int cursorx = 0;        // Mouse cursor positions
int cursory = 0;
int mousebutton = 0;    // Mouse button

extern BOOL LoaderWait;

// Screen Display Functions

TScreen::TScreen()
{
    nextscreen = NULL;
}

TScreen::~TScreen()
{
}

BOOL TScreen::BeginScreen()
{
    int loop;

    Display->InitBackgroundSystem();
    firstframe = TRUE;  // Tell timer tick function not to flip a page the first time

    panes.Clear();
    screenframes = 0;
    for (loop = 0; loop < NUMEXCLUSIVEPANES; loop++)
        exclusive[loop] = 0;
    numexclusive = 0;

    BOOL ok = Initialize();
    if (!ok)
        return FALSE;

    return TRUE;
}

void TScreen::EndScreen()
{
    int loop;

    Close();

    panes.Clear();
    screenframes = 0;
    for (loop = 0; loop < NUMEXCLUSIVEPANES; loop++)
        exclusive[loop] = 0;
    numexclusive = 0;
}

// Copies contents of display's back buffer to the user's display immediately. (For
// 'loading' screens which have to update from within a single timer tick).
void TScreen::PutToScreen()
{
    Display->PutToScreen(0, 0, Display->Width(), Display->Height());
}

// *************************
// * Screen Pane Functions *
// *************************

int TScreen::FindPane(PTPane pane)
{
    for (TPaneIterator p(&panes); p; p++)
        if (pane == p.Item())
            return p.ItemNum();

    return -1;
}

int TScreen::AddPane(PTPane pane, int panenum)
{
  // Do pane list
    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (panes[loop] == pane)
        {
            if (panenum < 0)
                return loop;
            else
                panes.Remove(loop);
        }
    }

    int newnum;
    if (panenum < 0)
        newnum = panes.Set(pane, panes.NumItems());
    else
        newnum = panes.Set(pane, panenum);

    pane->SetScreen(this);

    return newnum;
}

BOOL TScreen::RemovePane(PTPane pane)
{
    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (panes[loop] == pane)
        {
            panes.Remove(loop);
            pane->SetScreen(NULL);
            return TRUE;
        }
    }

    return FALSE;
}

BOOL TScreen::SetExclusivePane(int panenum, BOOL completeexclusion)
{
    if (!panes[panenum] || numexclusive >= NUMEXCLUSIVEPANES)
        return FALSE;

    exclusive[numexclusive] = panenum;
    complete[numexclusive] = completeexclusion;
    numexclusive++;

    return TRUE;
}

void TScreen::ReleaseExclusivePane(int panenum)
{
    int pos = 0;
    for (int c = 0; c < numexclusive; c++)
    {
        if (exclusive[c] != panenum)
        {
            exclusive[pos] = exclusive[c];
            pos++;
        }
    }

    numexclusive = pos;
}

void TScreen::RedrawAllPanes()
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->SetDirty(TRUE);

        if (complete[numexclusive - 1])
        {
            Display->Reset();
            return;
        }
    }

  // Do pane list
    for (int loop = 0; loop < panes.NumItems(); loop++)
        if (panes.Used(loop) && !panes[loop]->IsHidden())
            panes[loop]->SetDirty(TRUE);

    Display->Reset();
}

// *****************************
// * Virtual Handler Functions *
// *****************************

void TScreen::DrawBackground()
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            if (dirty)
                pane->Update();

            pane->SetClipRect();
            pane->DrawBackground();
        }

        if (complete[numexclusive - 1])
        {
            dirty = FALSE;
            Display->Reset();
            return;
        }
    }

  // Do pane list
    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;

        if (dirty)
            panes[loop]->Update();

        panes[loop]->SetClipRect();
        panes[loop]->DrawBackground();
    }

    dirty = FALSE;
    Display->Reset();
}

void TScreen::Pulse()
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->Pulse();
        }

        if (complete[numexclusive - 1])
        {
            Display->Reset();
            return;
        }
    }

  // Do pane list
    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->Pulse();
    }

    Display->Reset();
}

void TScreen::Animate(BOOL draw)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->Animate(draw);
        }

        if (complete[numexclusive - 1])
        {
            Display->Reset();
            return;
        }
    }

  // Do pane list
    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->Animate(draw);
    }

    Display->Reset();
}

void TScreen::MouseClick(int button, int x, int y)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->MouseClick(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display->Reset();
        return;
    }

  // Do pane list
    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        int nx = x - panes[loop]->GetPosX();
        int ny = y - panes[loop]->GetPosY();

      // send mouseup buttons to all panes, not just the owner of that screen space
        if (panes[loop]->InPane(nx, ny) ||
            (button == MB_LEFTUP || button == MB_RIGHTUP || button == MB_MIDDLEUP))
            panes[loop]->MouseClick(button, nx, ny);
    }

    Display->Reset();
}

void TScreen::MouseMove(int button, int x, int y)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
        {
            pane->SetClipRect();
            pane->MouseMove(button, x - pane->GetPosX(), y - pane->GetPosY());
        }
        Display->Reset();
        return;
    }

  // Do pane list
    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        int nx = x - panes[loop]->GetPosX();
        int ny = y - panes[loop]->GetPosY();
        panes[loop]->MouseMove(button, nx, ny);
    }

    Display->Reset();
}

void TScreen::KeyPress(int key, BOOL down)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->KeyPress(key, down);
        Display->Reset();
        return;
    }

    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->KeyPress(key, down);
    }

    Display->Reset();
}

void TScreen::CharPress(int key, BOOL down)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->CharPress(key, down);
        Display->Reset();
        return;
    }

    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;
        panes[loop]->SetClipRect();
        panes[loop]->CharPress(key, down);
    }

    Display->Reset();
}

void TScreen::Joystick(int key, BOOL down)
{
    Display->Reset();

  // Do exclusive
    if (numexclusive > 0)
    {
        PTPane pane = panes[exclusive[numexclusive - 1]];
        if (pane && !pane->IsHidden())
            pane->Joystick(key, down);
        Display->Reset();
        return;
    }

    for (int loop = 0; loop < panes.NumItems(); loop++)
    {
        if (!panes.Used(loop) || panes[loop]->IsHidden() || panes[loop]->IsIgnoringInput())
            continue;

        panes[loop]->SetClipRect();
        panes[loop]->Joystick(key, down);
    }

    Display->Reset();
}

// ***************************
// * Screen System Functions *
// ***************************

PTScreen TScreen::ShowScreen(PTScreen screen, int ticks)
{
    if (!screen)
        return NULL;

    CurrentScreen = screen;

    CurrentScreen->screenframes = 0;

    // clear the screen
//  Display->Clear(0, 0xffff, 0, DM_NOCLIP | DM_NORESTORE);
//  Display->FlipPage();
//  Display->Clear(0, 0xffff, 0, DM_NOCLIP | DM_NORESTORE);

    BOOL init = CurrentScreen->BeginScreen();

    if (init)
    {
        CurrentScreen->TimerLoop(ticks);
        CurrentScreen->EndScreen();
    }

    CurrentScreen = NULL;

    if (init && !Closing)
        return screen->GetNextScreen();
    else
        return NULL;
}

BOOL TScreen::TimerTick(BOOL draw)
{
    // Make sure we pick up where we left off if TimerTick() is called recursively
    // from the message handling functions!
    static BOOL washandlingmessage = FALSE;
    MSG  Message;

    HWND hwnd = MainWindow.Hwnd();

    if (hwnd == NULL || Closing || !Display->Width())
        return FALSE;

  // Update game toggles here...
  // ***************************

  // Allow scrolling of ZBuffer if game speed is high enough
    if (GameSpeed >= 4)
        NoScrollZBuffer = FALSE;

  // ***************************
  // End update game toggles

    if (!washandlingmessage)
    {
      // Process windows messages
        washandlingmessage = TRUE;
        while (PeekMessage(&Message, hwnd, 0, 0, PM_REMOVE))
        {
            if (Message.message == WM_QUIT)
            {   
                Closing = TRUE;
                return FALSE;
            }
            if (Message.message == WM_PAINT)
                ValidateRect(hwnd, NULL);
            else
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
        }

      // If no current screen, forget it!
        if (CurrentScreen != this || Closing)
            return FALSE;

      // Simulate mouse messages by polling the mouse
        POINT cursorpos;
        GetCursorPos(&cursorpos);
        RECT r;
        GetClientRect(MainWindow.Hwnd(), &r);
        ClientToScreen(MainWindow.Hwnd(), (LPPOINT)&r);
        cursorpos.x -= r.left;
        cursorpos.y -= r.top;

        // only bother if it's changed
        if (cursorx != cursorpos.x || cursory != cursorpos.y)
        {
            cursorx = cursorpos.x;
            cursory = cursorpos.y;

            CurrentScreen->MouseMove(mousebutton, cursorx, cursory);
        }

      // Get joystick state and send down to screen
        DWORD joystate, joychanged, dbljoystate, dbljoychanged;
        GetJoystickState(0, &joystate, &joychanged, &dbljoystate, &dbljoychanged);
        if (joychanged || dbljoychanged)
        {
            int joykey = -1;
            BOOL joydown = FALSE;

            while (GetJoystickKeyCode(joystate, joychanged, dbljoystate, dbljoychanged, joykey, joydown))
                Joystick(joykey, joydown);
        }

      // Reset clipping stuff
        Display->Reset();

      // Resize pane before pulsing (and possibly setting new map position)
        for (int loop = 0; loop < panes.NumItems(); loop++)
        {
            if (!panes.Used(loop))
                continue;

          // Update pane sizes
            if (panes[loop]->WasResized())
                panes[loop]->PaneResized();
        }

      // Pulse the screen (and it's panes) before we do any drawring....
        Pulse();    

      // Now update pane scroll position (after pulsing and possibly setting new map pos)
        for (loop = 0; loop < panes.NumItems(); loop++)
        {
            if (!panes.Used(loop))
                continue;

          // Set panes do dirty if screen is dirty
            if (dirty)
                panes[loop]->SetDirty(TRUE);    

          // Update the scrollpos for panes 
            panes[loop]->UpdateBackgroundScrollPos();
        }

      // Calls the DrawBackground() function
      // Note: All drawing is done BEFORE the dirty rectangle restore functions are called.
      // This means that you can actually override what the restore system would do before
      // it has a chance to do it.  Any background draws will merge their update rectangles
      // with the restore rects in the restore system, thus reducing the total amount of the
      // display to restore.  If you update the entire background, all restore rects are
      // deleted (this actually happens during screen scrolling).

        if (draw)
            DrawBackground();

      // Restores the video buffer
        if (draw)
            Display->RestoreBackgroundAreas();

    }

    washandlingmessage = FALSE;

 // Call screen's Animate() virtual function
    Display->Reset();
    Animate(draw);

 // Draw new mouse cursor
    Display->Reset();
    if (draw)
        DrawMouseCursor();  

    return TRUE;
}

BOOL TScreen::TimerLoop(long ticks)
{
    DWORD totalticks, totalframes, skipframes;
    float showrate, framerate, frameaccum;
    BOOL lastskipped;

    showrate = framerate = (float)FRAMERATE;
    frameaccum = (float)0.0;
    totalticks = totalframes = skipframes = 0;
    lastskipped = FALSE;

    while (TRUE)
    {
      // If clock has ticked, call timertick
        if (!DisableTimer)
            Timer.WaitForTick();

        while (PauseWhenNotActive && !AppActive)
        {
          // Process windows messages
            MSG  Message;
            HWND hwnd = MainWindow.Hwnd();

            GetMessage(&Message, hwnd, 0, 0);       // use getmessage instead of peekmessage for pause

            if (Message.message == WM_QUIT)
            {
                Closing = TRUE;
                return FALSE;
            }
            if (Message.message == WM_PAINT)
                ValidateRect(hwnd, NULL);
            else
            {
                TranslateMessage(&Message);
                DispatchMessage(&Message);
            }
        }
        
        if (CurrentScreen)
        {
            static DWORD lastcount;

            LastFrameTicks = GetTickCount() - lastcount;
            lastcount = GetTickCount();

            totalticks += LastFrameTicks;
            totalframes++;
            if (lastskipped)
                skipframes++;

            if (PauseFrameSkip)
                totalframes = totalticks = skipframes = 0;

            if (totalframes >= 5)
            {
                showrate = (float)1000.0 / (float)(totalticks / totalframes);
                int realframes = totalframes - skipframes;
                if (realframes <= 0)
                    realframes = 1; 
                framerate = (float)1000.0 / (float)(totalticks / realframes);

                if (skipframes >= totalframes)
                    PauseFrameSkip = TRUE;

                totalticks = 0;
                totalframes = 0;
                skipframes = 0;
            }

            if (ShowFramesPerSecond)
            {
                char buf[80];
                int usleep = MapPane.GetUpdateSleep();
                sprintf(buf, "Frame rate: %4.1f %4.1f %d - %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s", showrate, framerate, usleep,
                    (SmoothScroll)?"Scr ":"",
                    (NoFrameSkip)?"NSkp ":"",
                    (NoScrollZBuffer)?"NSZb ":"",
                    (ShowDrawing)?"Shw ":"",
                    (NoNormals)?"NNrm ":"",
                    (ClearBeforeDraw)?"Clr ":"",
                    (FlatShade)?"FSh ":"",
                    (DitherEnable)?"Dth ":"",
                    (BlendEnable)?"Bln ":"",
                    (ZEnable)?"Zbf ":"",
                    (SpecularEnable)?"Spc ":"",
                    (UseTextures)?"Tex ":"",
                    (BilinearFilter)?"Flt ":"",
                    (NoUpdateRects)?"NUp ":"",
                    (Show3D)?"3D ":"",
                    (GridSnap)?"Grd ":"",
                    (UseDrawPrimitive)?"Dpr ":"",
                    (NoPulseObjs)?"NPls ":"",
                    (NoAnimateObjs)?"NAni ":"",
                    (Interpolate)?"":"NInt ",
                    (NoAI)?"NAI ":"");
                Display->Reset(); // Reset display clipping rect
                Display->WriteText(buf, 
                    MapPane.GetPosX() + 10,
                    MapPane.GetPosY() + 10,
                    1, SystemFont, NULL, DM_TRANSPARENT | DM_ALIAS);
            }
            
            frameaccum += framerate; // Accumulator is greater than framerate to draw a frame

          // Do we draw this frame?
            if (frameaccum > (FRAMERATE - 0.5) ||               // Frame rate caught up or..
                firstframe || PauseFrameSkip || NoFrameSkip ||  // game flags say draw every frame or..
                VideoCapture.IsCapturing())                     // if capturing video frames
            {
                PauseFrameSkip = FALSE;         // resume normal frame skipping

                Timer.ResetTick();
                if (frameaccum > (float)100.0)
                    frameaccum = (float)0.0;
                else
                    frameaccum -= FRAMERATE;

                if (!firstframe)
                    Display->FlipPage(TRUE);  // Don't flip page the first time
                firstframe = FALSE;

                if (!CurrentScreen->TimerTick(TRUE))
                    return FALSE;

                if (VideoCapture.IsCapturing())
                    VideoCapture.SaveFrame();  

                screenframes++;

                lastskipped = FALSE;
            }
            else
            {
                if (!CurrentScreen->TimerTick(FALSE))
                    return FALSE;

                screenframes++;

                lastskipped = TRUE;
            }
        }

        if (ticks)
        {
            ticks--;
            if (!ticks)
                return TRUE;
        }
    }

    return FALSE;
}

// *******************
// * TPane Functions *
// *******************

BOOL TPane::Initialize() 
{ 
    if (isopen)
        return TRUE;
    
    x = newx; y = newy; width = newwidth; height = newheight;

    backgroundbuffer = -1; 
    oldscrollx = scrollx = newscrollx = oldscrolly = scrolly = newscrolly = 0;
    dirty = TRUE;
    ignoreinput = FALSE;

  // Create any background buffers for this pane
    CreateBackgroundBuffers();

    isopen = TRUE;

    return TRUE; 
}

void TPane::Close()
{
  // Free all background buffers for this pane  
    FreeBackgroundBuffers();

    isopen = FALSE;
}

// Causes the pane to be immediately shown on the screen.  Useful for when
// the pane contains a status or 'loading' bar that is updated during a single
// timer tick.
void TPane::PutToScreen()
{
    Display->PutToScreen(x, y, width, height);
}

// This function can be called to draw a pane immediately (instead of waiting for the
// screen TimerTick() function to call the various draw routines.  Note that no user
// input will be processed, and only the Pulse(), DrawBackground(), and Animate() functions
// are called.  The PutToScreen() function can be called immediately after this function
// to render the results to the display.
void TPane::Draw()
{
    if (!IsOpen() || IsHidden())
        return;

    SClipState cs;
    Display->SaveClipState(cs);

    SetClipRect();

    Pulse();
    DrawBackground();
    Animate(TRUE);

    Display->RestoreClipState(cs);
}


void TPane::SetClipRect()
{
    Display->SetOrigin(x - scrollx, y - scrolly);
    Display->SetClipRect(x, y, width, height);
    Display->SetClipMode(CLIP_EDGES);
}

void TPane::UpdateBackgroundScrollPos()
{
    oldscrollx = scrollx; oldscrolly = scrolly;
    scrollx = newscrollx; scrolly = newscrolly;
    if (backgroundbuffer >= 0)
        Display->ScrollBackground(backgroundbuffer, scrollx, scrolly);
}   

void TPane::DrawRestoreRect(int x, int y, int width, int height, DWORD drawmode)
{
    Display->DrawRestoreRect(backgroundbuffer, x, y, width, height, drawmode);
}

BOOL TPane::IsOnScreen()
{
    return screen == CurrentScreen;
}
