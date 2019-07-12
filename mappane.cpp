// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    mappane.cpp - Map pane object                      *
// *************************************************************************

#include <windows.h>
#include <mmsystem.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h> 
#include <memory.h>
#include <math.h>
#include <process.h>
#include <time.h>

#include "revenant.h"
#include "graphics.h"
#include "mappane.h"
#include "stream.h"
#include "3dscene.h"
#include "bmsurface.h"
#include "ddsurface.h"
#include "multisurface.h"
#include "mosaicsurf.h"
#include "display.h"
#include "dls.h"
#include "editor.h"
#include "playscreen.h"
#include "inventory.h"
#include "player.h"
#include "spell.h"
#include "script.h"
#include "cursor.h"
#include "multi.h"
#include "savegame.h"
#include "money.h"
#include "exit.h"
#include "textbar.h"
#include "statusbar.h"
#include "directdraw.h"
#include "directinput.h"
#include "spellpane.h"
#include "sound.h"

extern TEditStatusPane StatusBar;
extern TConsolePane Console;
extern TEditClassPane ClassPane;
extern PTBitmap PointerCursor;
extern PTMulti EditorData;

extern PTObjectInstance TakenObject;
extern PTObjectInstance DroppedObject;

#define GRIDZOFF    334

// NoSmoothScroll stuff

#define SCROLLEDGESIDES     32
#define SCROLLEDGETOP       32
#define SCROLLEDGEBOTTOM    32

#define CHARACTER_HEIGHT    32

#define MAPGRIDWIDTH        (MAPPANEWIDTH - (SCROLLEDGESIDES * 2))
#define MAPGRIDHEIGHT       (MAPPANEHEIGHT - SCROLLEDGETOP - SCROLLEDGEBOTTOM)

inline int ScreenGrid(int n, int s)
{
    if (absval(n) < (s/2))
        return 0;

    if (n < 0)
        n -= s / 2;
    else
        n += s / 2;

    return (n / s);
}

// Generate a random map index
int TMapPane::MakeIndex()
{
    static int incrementor = 0;

    // Start with the current time
    time_t t;
    time(&t);

    int index = t - 0x34d6574c;
    if (index < 0)
    {
        // hrm...messed up clock, just flip it
        index *= -1;
    }

    // Throw in a semi-random element
    // make sure that the index level does NOT match old style indexes
    if (MapPane.GetMapLevel() == incrementor)
        incrementor++;

    if (incrementor > 0x1f)
        incrementor = 0;

    index |= incrementor++ << 26;

    return index;
}

// ****************
// * TMapIterator *
// ****************

// Initializes iterator with a range value (makes rect and calls Initialize)
TMapIterator::TMapIterator(RS3DPoint pos, int range, int fl, int objset)
{
    SRect r;
    r.left = pos.x - range; 
    r.top = pos.y - range;
    r.right = pos.x + range; 
    r.bottom = pos.y + range;
    Initialize(&r, fl, objset);
}

// Initializes iterator with an object using default LOCALRANGE value.  This is the 
// standard way to do searches in the map system for objects nearby other objects.
TMapIterator::TMapIterator(RTObjectInstance oi, int fl, int objset)
{
    S3DPoint pos;
    oi.GetPos(pos); 
    SRect r;
    r.left = pos.x - LOCALRANGE; 
    r.top = pos.y - LOCALRANGE;
    r.right = pos.x + LOCALRANGE; 
    r.bottom = pos.y + LOCALRANGE;
    Initialize(&r, fl | CHECK_MAPRECT, objset);
}

void TMapIterator::Initialize(PSRect sr, int fl, int os)
{
    if (sr)
    {
        if (!(fl & (CHECK_RECT | CHECK_MAPRECT | CHECK_SCRRECT | CHECK_SECTRECT)))
            fl |= CHECK_RECT;
        memcpy(&r, sr, sizeof(SRect));
    }
    else
    {
        memset(&r, 0, sizeof(SRect));
    }
    flags = fl;
    objset = os;
    sx = -1;        // Causes NextItem() to start at sx=0
    sy = 0;
    index = 0;
    numitems = 0;
    sector = NULL;
    parent = NULL;
    invindex = 0;
    parent = item = NULL;
    NextItem();
}


PTObjectInstance TMapIterator::NextItem()
{
    // OPTIONAL Inventory Iterator
    // ***************************

  // Check Inventory    
    if (flags & CHECK_NOINVENT)
        item = NULL;
    else
    {
        if (item && item->NumInventoryItems() > 0)
        {
            parent = item;
            invindex = 0;
        }

        item = NULL;

        while (parent && !item)
        {
            if (invindex < parent->NumInventoryItems())
                item = parent->GetInventory(invindex++);
            else
            {
                invindex = parent->InvIndex() + 1;
                parent = parent->GetOwner();
            }
        }
    }

    // Main Sector Iterator Loop
    // *************************

    while (!item)
    {
        index++;                    // Try to increment index

        // INFREQUENT Get Next Sector
        // **************************

        while (index >= numitems)   // If at end of array (or no array) get next sector
        {
            if (++sx >= SECTORWINDOWX)
            {
                sx = 0;
                sy++;
            }
                
            if (sy >= SECTORWINDOWY)
                return NULL;
            
            if (!MapPane.sectors[sx][sy])
                continue;

            if (flags & CHECK_SECTRECT)
            {
                // Does rectangle intersect sector
                SRect sr;
                MapPane.sectors[sx][sy]->GetMaxScreenRect(sr);
                if (!r.Intersects(sr))
                    continue;
            }
            else if (flags & CHECK_MAPRECT)
            {
                // Does rectangle intersect sector
                SRect sr;
                MapPane.sectors[sx][sy]->GetMaxMapRect(sr);
                if (!r.Intersects(sr))
                    continue;
            }

            sector = MapPane.sectors[sx][sy];
            numitems = sector->NumObjSetItems(objset);
            index = 0;
        }

        item = sector->GetObjSetInstance(objset, index);    // Get item

        // OPTIONAL: Checks
        // ****************

        while (item && (flags & (CHECK_MOVING | CHECK_INVIS | CHECK_SCRRECT | CHECK_LIGHT | CHECK_MAPRECT)))
        {
            if (flags & CHECK_MOVING)
            {
                if (item->GetFlags() & OF_MOVING)
                {
                    item = NULL;
                    break;
                }
            }

            if (flags & CHECK_INVIS)
            {
                if (item->GetFlags() & OF_INVISIBLE)
                {
                    item = NULL;
                    break;
                }
            }

            BOOL good, checked = FALSE;
            SRect ir;

            if (flags & CHECK_SCRRECT)
            {
                checked = TRUE;
                item->GetScreenRect(ir);
                good = r.Intersects(ir);
            }
            else if (flags & CHECK_MAPRECT)
            {
                checked = TRUE;
                S3DPoint p;
                item->GetPos(p);
                good = r.In(SPoint(p.x, p.y));
            }

            if (flags & CHECK_LIGHT && item->GetFlags() & OF_LIGHT &&
                item->GetLightIntensity() != 0)
            {
                if (!checked || !good)
                {
                    checked = TRUE;
                    item->GetLightRect(ir);
                    good = r.Intersects(ir);
                }
            }

            if (checked && !good)
            {
                item = NULL;            // go to next item, screen rect doesn't intersect
                break;
            }

            break;      // all tests passed, exit loop
        }
    }

    return item;
}

void TMapIterator::Nuke()
{
    if (item)
    {
        MapPane.DeleteObject(item);
        item = NULL;
    }
}

// ************************
// * Initialize and Close *
// ************************

BOOL TMapPane::Initialize()
{
  // Initialize the 3D
    Scene3D.Initialize();

    TPane::Initialize(); // Initialize values and create background areas

    int px = MAPPANEX = GetPosX();
    int py = MAPPANEY = GetPosY();
    int pw = MAPPANEWIDTH = GetWidth();
    int ph = MAPPANEHEIGHT = GetHeight();

    oldposx = posx = oldposy = posy = 
    oldsectorx = oldsectory = sectorx = sectory =
    oldscrollx = scrollx = oldscrolly = scrolly = 0x80000000;
    oldlevel = level = 0;

    for (int sy = 0; sy < SECTORWINDOWY; sy++)
        for (int sx = 0; sx < SECTORWINDOWX; sx++)
            sectors[sx][sy] = NULL;

    SColor c;
    c.red = 255;
    c.green = 255;
    c.blue = 255;
    SetLightColor(0, c, 35);

  // Clear mouse click last key
    lastkey = -1;

  // Setup ambient stuff
    SetAmbientLight(10);
    SColor color = {(BYTE)255, (BYTE)255, (BYTE)255};
    SetAmbientColor(color);

  // Initialize Dynamic Light
    dlight.intensity = 0;
    dlight.lightindex = -1;
    dlight.pos.x = dlight.pos.y = dlight.pos.z = 0;

  // Init cash
    TMoney::Initialize();

  // Init exits
    TExit::Initialize();

  // Target finder
    targetcallback = NULL;
    abortcallback = NULL;

  // Walkmap draw
    wmrevealx = 0;
    wmrevealy = 0;
    wmrevealsizex = (SECTORHEIGHT >> WALKMAPSHIFT);
    wmrevealsizey = (SECTORWIDTH >> WALKMAPSHIFT);

    dragmode = FALSE;
    clicked = FALSE;

    memset(&center, 0, sizeof(S3DPoint));

  // Make sure update thread is going  
    BeginUpdateThread();

    return TRUE;
}

void TMapPane::Close()
{
  // Make sure update thread is finished  
    EndUpdateThread();

  // Delete all sectors (saving them if in editor)
    FreeAllSectors();

  // Free imagery in imagery system
    TObjectImagery::FreeAllImagery();

  // Death to realtime 3D
    Scene3D.Close();
 
  // Call base class close function
    TPane::Close(); // Free's background areas
}

// This function gets called when the pane is created, or resized, to set the pane's
// background buffer.  We also put our 3D viewport resize here too.
void TMapPane::CreateBackgroundBuffers()
{
  // Set global map size variables
    MAPPANEX = GetPosX();
    MAPPANEY = GetPosY();
    MAPPANEWIDTH = GetWidth();
    MAPPANEHEIGHT = GetHeight();
    SCROLLBUFWIDTH = MAPPANEWIDTH + UPDATEWIDTH * 2;
    SCROLLBUFHEIGHT = MAPPANEHEIGHT + UPDATEHEIGHT * 2;

  // Set 3D system viewport size
    Scene3D.SetSize(MAPPANEX, MAPPANEY, MAPPANEWIDTH, MAPPANEHEIGHT);

  // Do lit multi for map pane update surface for the update thread
    litmulti = new TMosaicSurface(
            SCROLLBUFWIDTH / MOSAICTILEX,
            SCROLLBUFHEIGHT / MOSAICTILEY,
            MOSAICTILEX,
            MOSAICTILEY,
            MOSAICSURF_SYSTEMMEM | MOSAICSURF_ZBUFFER | MOSAICSURF_ZSYSTEMMEM);
    litmulti->SetClipMode(CLIP_WRAP);

  // Make a clone of the litmulti for the main thread so it has its own origin/clip rect
  // which can be set independently even while the update thread is running.
    updatemulti = new TMosaicSurface(litmulti);
    updatemulti->SetClipMode(CLIP_WRAP);

  // Clone tiles and zbuffer from lit multi, and add 32 bit buffer tiles
//  if (!IsMMX)
//  {
        unlitmulti = new TMosaicSurface(litmulti,
            MOSAICSURF_BMSURFACE | MOSAICSURF_32BIT | MOSAICSURF_CLONEZBUFFER);
        unlitmulti->SetClipMode(CLIP_WRAP);
        zbufferiscloned = TRUE;
//      lightmulti = NULL;
//  }
//  else
//  {
//      unlitmulti = new TMosaicSurface(litmulti,
//          MOSAICSURF_BMSURFACE | MOSAICSURF_CLONEZBUFFER);
//      unlitmulti->SetClipMode(CLIP_WRAP);
//      zbufferiscloned = TRUE;
//      lightmulti = new TMosaicSurface(lightmulti,
//          MOSAICSURF_BMSURFACE | MOSAICSURF_8BIT);
//  }


  // IMPORTANT NOTE:
  //
  // THE 'litmulti' SURFACE MUST ONLY BE USED IN THE UPDATE THREAD (i.e. DrawUnlit(),
  // DrawLights(), DrawLit(), DrawSelected(), etc.  Conversely, the updatemulti
  // MUST ****NOT**** be used in the update thread.  If these rules are not followed,
  // the program will not work correctly!!!

    int bgbuffer = Display->UseBackgroundArea(
        GetPosX(), GetPosY(), GetWidth(), GetHeight(), updatemulti);
    SetBackgroundBuffer(bgbuffer);

  // Start the update thread going
    BeginUpdateThread();
    Update(); // Cause whole pane to be redrawn
}

void TMapPane::FreeBackgroundBuffers()
{
  // End the update thread
    EndUpdateThread();
    
  // Free background scrolling area for this pane
    Display->FreeBackgroundArea(GetBackgroundBuffer());
    ClearBackgroundBuffer();

  // Kill surfaces
    if (unlitmulti && litmulti && updatemulti)
    {
        delete unlitmulti;
        unlitmulti = NULL;
        delete litmulti;
        litmulti = NULL;
        delete updatemulti;
        updatemulti = NULL;
    }
}

// ******************
// * Map Management *
// ******************

// Loads the current map in the "curmap" directory from the given directory 
// (i.e. "savegame.001"), or clears the "curmap" directory if NULL, forces
// reload of all sectors.
void TMapPane::LoadCurMap(char *from)
{
    char frompath[MAX_PATH], topath[MAX_PATH];
    
    ClearCurMap();

    if (!from)
        return;

    makepath(from, frompath, MAX_PATH);
    strcat(frompath, "\\*.DAT");
    makepath(CURMAPDIR, topath, MAX_PATH);

    copyfiles(frompath, topath);
}

// Saves map in curmap to the given game subdirectory 
//(i.e. "savegame.001") or "map" if null
void TMapPane::SaveCurMap(char *to)
{
    ReloadSectors(); // Forces sectors to be saved

    LOCKSECTORS;        // Prevent update thread from accessing sectors while we change them
                        // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                        //  BETWEEN THESE TWO FUNCTIONS!!)

    TSector::ClearPreloadSectors();
    
    UNLOCKSECTORS;      // Do not return between these two things

    if (!to)
        to = BASEMAPDIR;

    char frompath[MAX_PATH], topath[MAX_PATH];

    makepath(CURMAPDIR, frompath, MAX_PATH);
    strncatz(frompath, "\\*.DAT", MAX_PATH);
    makepath(to, topath, MAX_PATH);
    strncatz(topath, "\\", MAX_PATH);

    copyfiles(frompath, topath);
}

// Deletes all files in the "curmap" directory, and forces sectors to reload.
void TMapPane::ClearCurMap()
{
    char path[MAX_PATH];
    
    ReloadSectors();

    LOCKSECTORS;        // Prevent update thread from accessing sectors while we change them
                        // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                        //  BETWEEN THESE TWO FUNCTIONS!!)

    TSector::ClearPreloadSectors();
    
    UNLOCKSECTORS;      // Do not return between these two things

    makepath("curmap", path, MAX_PATH);
    strncatz(path, "\\*.DAT", MAX_PATH);

    deletefiles(path);
}

// ******************
// * Input Handling *
// ******************

void TMapPane::SnapWalkDisplay(int mapindex)
{
    if (mapindex < 0)
        return;

    PTObjectInstance inst = GetInstance(mapindex);
    if (inst && inst->GetImagery())
    {
        S3DPoint pos;
        inst->GetPos(pos);
        pos.x >>= WALKMAPSHIFT;
        pos.y >>= WALKMAPSHIFT;
        pos.x %= SECTORWIDTH >> WALKMAPSHIFT;
        pos.y %= SECTORHEIGHT >> WALKMAPSHIFT;

        int x, y, width, length;
        inst->GetFacingBoundBox(x, y, width, length);

        wmrevealx = pos.x - x;
        wmrevealy = pos.y - y;

        wmrevealsizex = wmrevealx + width;
        wmrevealsizey = wmrevealy + length;

        RedrawAll();
    }
}

void TMapPane::KeyPress(int key, BOOL down)
{
    BOOL interuptforreals = TRUE;

    if (down && Editor && StatusBar.EditWalkmap())
    {
        switch (key)
        {
          case VK_UP:
            if (CtrlDown)
                wmrevealy--;
            else
                wmrevealsizey--;

            RedrawAll();
            break;
          case VK_DOWN:
            if (CtrlDown)
                wmrevealy++;
            else
                wmrevealsizey++;

            RedrawAll();
            break;
          case VK_LEFT:
            if (CtrlDown)
                wmrevealx--;
            else
                wmrevealsizex--;

            RedrawAll();
            break;
          case VK_RIGHT:
            if (CtrlDown)
                wmrevealx++;
            else
                wmrevealsizex++;

            RedrawAll();
            break;
          case VK_HOME:
            if (CtrlDown)
            {
                wmrevealx = (center.x - ((sectorx + 1) << SECTORWSHIFT)) >> WALKMAPSHIFT;
                wmrevealy = (center.y - ((sectory + 1) << SECTORHSHIFT)) >> WALKMAPSHIFT;
            }
            else
            {
                wmrevealsizex = (center.x - ((sectorx + 1) << SECTORWSHIFT)) >> WALKMAPSHIFT;
                wmrevealsizey = (center.y - ((sectory + 1) << SECTORHSHIFT)) >> WALKMAPSHIFT;
            }

            RedrawAll();
            break;
          case VK_END:
            if (CtrlDown)
            {
                wmrevealx = 0;
                wmrevealy = 0;
                wmrevealsizex = (SECTORHEIGHT >> WALKMAPSHIFT);
                wmrevealsizey = (SECTORWIDTH >> WALKMAPSHIFT);
            }
            else
            {
                SnapWalkDisplay(StatusBar.GetSelectedObj());
            }
            RedrawAll();
            break;
        }
    }

    if (down && key == VK_ESCAPE)
        PostQuitMessage(0);
}

void TMapPane::FindClickPos(int x, int y, RS3DPoint start, RS3DPoint target)
{
    int bottomy = posy + GetHeight() + ((255 * 866) / 1000);

    S3DPoint top, bottom;
    ScreenToWorld(x + posx, y + posy, top);
    ScreenToWorld(x + posx, bottomy, bottom);
    top &= (DWORD)~(WALKMAPGRANULARITY - 1);
    bottom &= (DWORD)~(WALKMAPGRANULARITY - 1);

    BOOL found = FALSE;

    for ( ; bottom.y >= top.y && !found; bottom.x -= 16, bottom.y -= 16)
        for (int i = 0; i < 3; i++)
        {
            S3DPoint oldbottom = bottom;
            if (i == 1)
                bottom.y -= 16;
            else if (i == 2)
                bottom.x -= 16;

            int height = GetWalkHeight(bottom);

            S3DPoint walkpos;
            ScreenToWorld(x+posx, y+posy + ((height * 866) / 1000), walkpos);
            walkpos.x += 8;
            walkpos.y += 8;

            if (walkpos.x >> WALKMAPSHIFT == bottom.x >> WALKMAPSHIFT &&
                walkpos.y >> WALKMAPSHIFT == bottom.y >> WALKMAPSHIFT)
            {
                bottom.z = height;
                found = TRUE;
                break;
            }

            bottom = oldbottom;
        }

    if (!found)
        ScreenToWorld(x+posx, y+posy, bottom, start.z);

    target = bottom;
}

// ************* Draw order change functions ***************

BOOL TMapPane::SwapDrawOrder(PTObjectInstance inst0, PTObjectInstance inst1)
{
    for (TMapIterator i; i; i++)
        if (i == inst0)
            break;

    for (TMapIterator j; j; j++)
        if (j == inst1)
            break;

    if (!i || !j || i.SectorX() != j.SectorX() || i.SectorY() != j.SectorY())
        return FALSE;       // no dice, items are in different sectors

    int pos0 = i.SectorIndex();
    int pos1 = j.SectorIndex();

    PTSector sect = sectors[i.SectorX()][i.SectorY()];

    sect->RemoveObject(pos0);
    sect->RemoveObject(pos1);

    sect->SetObject(inst0, pos1);
    sect->SetObject(inst1, pos0);

    return TRUE;
}

void TMapPane::PushToFront(PTObjectInstance inst)
{
    for (TMapIterator i; i; i++)
        if (i == inst)
            break;

    if (!i)
        return;

    PTObjectInstance top = NULL;
    for (TMapIterator j; j; j++)
        top = j;

    SwapDrawOrder(inst, top);
}

void TMapPane::PushToBack(PTObjectInstance inst)
{
    for (TMapIterator i; i; i++)
        if (i == inst)
            break;

    if (!i)
        return;

    TMapIterator j;
    SwapDrawOrder(inst, j);
}

// ************** Movement control ******************

char *Directions[] = { "ne", "e", "se", "s", "sw", "w", "nw", "n", "ne" };

// Gets the world position (given character's z pos) that mouse is hovering over
void TMapPane::GetMouseMapPos(S3DPoint &p, int zoffset)
{
    int x = cursorx - GetPosX();
    int y = cursory - GetPosY();

    S3DPoint curpos;
    Player->GetPos(curpos);

    ScreenToWorld(x + posx, y + posy, p, curpos.z + zoffset);
}

// Gets the angle of the mouse from the character's current position
int TMapPane::GetMouseMapAngle()
{
    S3DPoint curpos, target;

    Player->GetPos(curpos);
    GetMouseMapPos(target);

    return ConvertToFacing(curpos, target);
}

void TMapPane::UpdateMouseMovement(int x, int y)
{
    if (!Player)
        return;

    S3DPoint curpos, target;

    Player->GetPos(curpos);
    GetMouseMapPos(target, 50);

    int angle = ConvertToFacing(curpos, target);

    target -= curpos;
    if (absval(target.x) < 16 && absval(target.y) < 16)
    {
        SetMouseBitmap(GameData->Bitmap("cursor"));
        Player->Stop();
    }
    else
    {
        angle += 0x10;
        angle = angle & 0xe0;

      // Get wedge cursor name
        char buf[32];
        sprintf(buf, "wedge-%s", Directions[angle >> 5]);
        SetMouseBitmap(GameData->Bitmap(buf));
        strcat(buf, "shadow");
        SetMouseShadow(GameData->Bitmap(buf), 0, 43);
        SetMouseCornerBitmap((PTBitmap)NULL, TRUE);

      // Get movement command   
        if (lastkey != -1)
            CurrentScreen->KeyPress(lastkey, FALSE);
        lastkey = -1;
        switch (angle)
        { 
          case 0:   { lastkey = VK_JOYUPRIGHT;   break; }
          case 32:  { lastkey = VK_JOYRIGHT;     break; }
          case 64:  { lastkey = VK_JOYDOWNRIGHT; break; }
          case 96:  { lastkey = VK_JOYDOWN;      break; }
          case 128: { lastkey = VK_JOYDOWNLEFT;  break; }
          case 160: { lastkey = VK_JOYLEFT;      break; }
          case 192: { lastkey = VK_JOYUPLEFT;    break; }
          case 224: { lastkey = VK_JOYUP;        break; }
        }

        if (lastkey != -1)
            CurrentScreen->KeyPress(lastkey, TRUE);
    }

    clicked = TRUE;
}

void TMapPane::MouseClick(int button, int x, int y)
{
    if (Editor)
    {
        if (button == MB_LEFTDOWN && InPane(x, y))
        {
            if (dragmode)
            {
                StatusBar.StopMoving();
            }
            else if (StatusBar.EditWalkmap())
            {
                for (int sy = SECTORWINDOWY - 1; sy >= 0; sy--)
                    for (int sx = SECTORWINDOWX - 1; sx >= 0; sx--)
                        if (sectors[sx][sy])
                        {
                            for (int gy = min(wmrevealsizey, (SECTORHEIGHT >> WALKMAPSHIFT)) - 1; gy >= wmrevealy; gy--)
                                for (int gx = min(wmrevealsizex, (SECTORWIDTH >> WALKMAPSHIFT)) - 1; gx >= wmrevealx; gx--)
                                {
                                    S3DPoint pos, screenpos, wpos;
                                    pos.x = gx;
                                    pos.y = gy;
                                    wpos.x = ((sectorx + sx) << SECTORWSHIFT) + (gx << WALKMAPSHIFT);
                                    wpos.y = ((sectory + sy) << SECTORHSHIFT) + (gy << WALKMAPSHIFT);
                                    wpos.z = sectors[sx][sy]->ReturnWalkmap(pos.x, pos.y);

                                    int scrx, scry;
                                    WorldToScreen(wpos, scrx, scry);

                                    // wacky formula to get diamond shape
                                    int dx = (scrx - (x + posx)) / 2;
                                    int dy = scry - (y + posy);
                                    if ((absval(dx) + absval(dy)) < 8)
                                    {
                                        // hang onto the data for dragging
                                        grabx = sectorx + sx;
                                        graby = sectory + sy;
                                        objx = gx;
                                        objy = gy;
                                        onobject = wpos.z;
                                        oldz = y + posy;

                                        SRect ir;
                                        ir.left = scrx - 20;
                                        ir.top = scry - 20;
                                        ir.right = scrx + 20;
                                        ir.bottom = scry + 20;
                                        AddBgUpdateRect(ir, BGDRAW_LIT);
                                        return;
                                    }
                                }
                        }

                grabx = graby = objx = objy = oldz = onobject = -1;
            }
            else
            {
                PTObjectInstance oi = OnObject(x, y);
                if (oi)
                {
                    S3DPoint pos;
                    oi->GetPos(pos);
                    WorldToScreen(pos, objx, objy);
                    grabx = posx - objx;
                    graby = posy - objy;
                    objx = x + grabx;
                    objy = y + graby;
                    oldz = pos.z;
                    StatusBar.Select(oi->GetMapIndex(), CtrlDown);
                }
            }
        }
        if (button == MB_MIDDLEDOWN && InPane(x, y))
        {
            PTObjectInstance oi = OnObject(x, y);
            if (oi)
                StatusBar.Select(oi->GetMapIndex());
            Console.Input("follow\n");
        }
        else if (button == MB_LEFTUP)
        {
            if (targetcallback)
            {
                S3DPoint pos;
                ScreenToWorld(x+posx-objx, y+posy-objy, pos, center.z);

                if (StatusBar.GridSnap())
                {
                    pos.x = pos.x & GRIDMASK;
                    pos.y = pos.y & GRIDMASK;
                    // z is kind of a special case, so just use the exact value
                    //pos.z = pos.z & GRIDMASK;
                }

                (*targetcallback)(pos);
                if (mode == MODE_MOVE)
                {
                    dragmode = FALSE;
                    targetcallback = NULL;
                    SetMouseBitmap(PointerCursor);
                }
            }
            else if (StatusBar.EditWalkmap())
            {
                if (objx >= 0 && objy >= 0)
                {
                    S3DPoint wpos, pos;
                    pos.x = objx;
                    pos.y = objy;
                    wpos.x = (grabx << SECTORWSHIFT) + (objx << WALKMAPSHIFT);
                    wpos.y = (graby << SECTORHSHIFT) + (objy << WALKMAPSHIFT);
                    wpos.z = sectors[grabx - sectorx][graby - sectory]->ReturnWalkmap(pos.x, pos.y);
                    int scrx, scry;
                    WorldToScreen(wpos, scrx, scry);
                    SRect ir;
                    ir.left = scrx - 20;
                    ir.top = scry - 20;
                    ir.right = scrx + 20;
                    ir.bottom = scry + 20;
                    AddBgUpdateRect(ir);
                    grabx = graby = objx = objy = oldz = onobject = -1;
                }
            }
            else
                StatusBar.StopMoving();
        }
        else if (button == MB_RIGHTDOWN)
        {
            if (targetcallback)
            {
                if (abortcallback)
                    (*abortcallback)();

                targetcallback = NULL;
                SetMouseBitmap(PointerCursor);
                //PlayScreen.ReleaseExclusivePane(&MapPane);
                dragmode = FALSE;
            }
            else
            {
                if (mousebutton == MB_LEFTDOWN)
                    StatusBar.Undo();       // command was aborted, move everything back

                while (StatusBar.Deselect());
                dragmode = FALSE;
            }
        }
    }
    else        // if not in the editor (normal gameplay)
    {
        Notify(N_CANCELCONTROL, Player);

        if (button == MB_RIGHTDOWN)
        {
            if (Player)
            {
                UpdateMouseMovement(x, y);
            }
        }
        else if (button == MB_RIGHTUP)
        {
            if (Player && clicked)
            {
                SetMouseBitmap(GameData->Bitmap("cursor"));

              // Stop character moving (release fake joystick key)
                if (lastkey != -1)
                {
                    CurrentScreen->KeyPress(lastkey, FALSE);
                    lastkey = -1;
                }

                clicked = FALSE;
            }
        }
        else if (button == MB_LEFTDOWN)
        {
            if (Player && InPane(x, y))
            {
                if (Player->IsCombat())
                {
                    Player->ButtonAttack(random(1,3));
                }
                else if (Player->IsBowMode())
                {
                    if (!Player->IsBowDrawn())
                    {
                        Player->DrawBow();
                        Player->AimBow(GetMouseMapAngle());
                    }
                }
                else
                {
                    PTObjectInstance oi = OnObject(x, y);
                    if (oi)
                        onobject = oi->GetMapIndex();
                    else
                        onobject = -1;
                    clicked = TRUE;
                }
            }
        }
        else if (button == MB_LEFTUP)
        {
            if (Player && InPane(x, y))
            {
                if (Player->IsCombat())
                {
                }
                else if (Player->IsBowMode())
                {
                    if (Player->IsBowDrawn())
                        Player->ShootBow(GetMouseMapAngle());
                }
                else
                {
                    PTObjectInstance inst = Inventory.GetContainer()->GetInventorySlot(Inventory.GetHeldSlot());
                    if (!inst && Player)
                        inst = Player->GetInventorySlot(EquipPane.GetHeldSlot() + 256);

                    int objindex = -1;
                    PTObjectInstance oninst = OnObject(x, y, inst);
                    if (oninst)
                        objindex = oninst->GetMapIndex();

                    if (clicked)
                    {
                        PTObjectInstance inst = oninst;

                        BOOL used = FALSE;

                        if (onobject == objindex && inst)
                        {
                            if (!inst->IsInventoryItem())
                                used = inst->Use(Player);
                            else
                            {
                                TakenObject = GetInstance(objindex);
                                Player->Pickup(TakenObject);
                                TextBar.Print("Picked up %s.", inst->GetTypeName());
                                used = TRUE;
                            }
                        }

//                      if (!used)  // Goto the point the mouse clicked
//                      {
//                          S3DPoint target, curpos;
//                          Player->GetPos(curpos);
//                          FindClickPos(x, y, curpos, target);
//                          Player->Goto(target.x, target.y);
//                      }
                    }
                    else // Not clicked
                    {
                        // dragging from the inventory or equipment to map pane
                        if (inst)
                        {
                            BOOL used = FALSE;
                            if (objindex >= 0)
                            {
                                // use the dragged object with the object clicked on
                                PTObjectInstance oi = GetInstance(objindex);
                                if (oi)
                                {
                                    used = oi->Use(Player, inst->GetMapIndex());
                                    if (used)
                                        Inventory.Update();
                                }
                            }

                            if (!used)
                            {
                                // didn't click on anything special, so just drop it on the ground
                                if (inst->InventNum() >= 256)
                                    ((PTPlayer)Player)->Equip(NULL, EquipPane.GetHeldSlot());   // clear from eq list

                                S3DPoint curpos, target;
                                Player->GetPos(curpos);
                                ScreenToWorld(x+posx, y+posy, target, curpos.z + 30);

                                inst->RemoveFromInventory();
                                inst->SetPos(target, Player->GetLevel());
                                inst->AddToMap();

                                if (inst->Amount() > 1)
                                    TextBar.Print("%d %ss dropped.", inst->Amount(), inst->GetName());
                                else
                                    TextBar.Print("%s dropped.", inst->GetName());

                                DroppedObject = inst;
                            }
                        }
                    }

                } // clicked?

            } // In pane and has char?

            clicked = FALSE;
        }
    }
}

#define MOUSESCROLLSPEED    8
#define MOUSEBOUNDARY       1

void TMapPane::MouseMove(int button, int x, int y)
{
    if (Editor)
    {
        // mouse scrolling on screen edges
        mx = my = 0;

        if (cursorx < MOUSEBOUNDARY)
            mx = -MOUSESCROLLSPEED;
        else if (cursorx >= (Display->Width() - MOUSEBOUNDARY))
            mx = MOUSESCROLLSPEED;

        if (cursory < MOUSEBOUNDARY)
            my = -MOUSESCROLLSPEED;
        else if (cursory >= (Display->Height() - MOUSEBOUNDARY))
            my = MOUSESCROLLSPEED;

        if (InPane(x, y) &&
            ((button == MB_LEFTDOWN && !dragmode) || (dragmode && button != MB_LEFTDOWN)))
        {
            if (StatusBar.EditWalkmap() && !dragmode)
            {
                if (objx >= 0 && objy >= 0)
                {
                    S3DPoint pos;
                    pos.x = objx;
                    pos.y = objy;
                    pos.z = min(255, max(0, (((oldz - (y + posy)) * 1000) / 866) + onobject));
                    sectors[grabx - sectorx][graby - sectory]->SetWalkmap(pos.x, pos.y, pos.z);

                    SRect ir;
                    ir.left = x + GetScrollX() - 32;
                    ir.top = y + GetScrollY() - 32;
                    ir.right = ir.left + 63;
                    ir.bottom = ir.top + 63;
                    AddBgUpdateRect(ir);
                }
            }
            else
            {
                if (ShiftDown)
                {
                    PTObjectInstance inst = OnObject(x, y);
                    if (inst)
                        StatusBar.Select(inst->GetMapIndex(), TRUE);
                }
                else
                {
                    PTObjectInstance oi = GetInstance(StatusBar.GetSelectedObj());
                    if (oi && !(oi->GetFlags() & OF_EDITORLOCK))
                    {
                        if (!(oi->GetFlags() & OF_SELDRAW))
                        {
                            // avoid accidental dragging
                            if (!dragmode &&
                                (absval((posx + x) - grabx) + absval((posy + y) - graby)) <= 3)
                                return;

                            StatusBar.StartMoving();
                        }
                        
                        S3DPoint oldpos;
                        oi->GetPos(oldpos);

                        MapPane.MoveObjScreen(StatusBar.GetSelectedObj(), x - objx, y - objy);

                        S3DPoint newpos;
                        oi->GetPos(newpos);

                        // handle z moving here, since it is relative to the object, not the map
                        if (!StatusBar.RestrictZ())
                        {
                            newpos.z = oldz - (y - (objy - graby));
                            if (StatusBar.GridSnap())
                                newpos.z &= GRIDMASK;
                            oi->SetPos(newpos);
                        }

                        // move other selected objects relative to this one
                        S3DPoint delta = newpos;
                        delta -= oldpos;

                        for (int i = StatusBar.GetFirstObj(); i >= 0; i = StatusBar.GetNextObj())
                        {
                            PTObjectInstance inst = GetInstance(i);
                            if (!inst || inst == oi)
                                continue;

                            S3DPoint pos;
                            inst->GetPos(pos);
                            pos += delta;
                            inst->SetPos(pos);
                        }
                    }
                }
            }
        }
    }
    else        // if not in the editor (normal gameplay)
    {
        if (button == MB_RIGHTDOWN && clicked)
            UpdateMouseMovement(x, y);

      // Aim the bow if we have it drawn
        if (Player && Player->IsBowDrawn())
        {
            int angle = GetMouseMapAngle();
            Player->AimBow(angle);
        }
    }
}

// utility function for dragging objects with the mouse
void TMapPane::MoveObjScreen(int index, int x, int y)
{
    if (index < 0)
        return;

    PTObjectInstance oi = GetInstance(index);
    if (oi)
    {
        S3DPoint newpos;
        oi->GetPos(newpos);
/*
        PTObjectInstance overinst = GetInstance(OnObject(x, y));
        if (overinst)
        {
            S3DPoint opos;
            overinst->GetPos(opos);
            ScreenToWorld(x + posx, y + posy + opos.z, newpos);
            newpos.z = opos.z;
        }
        else*/
        {
            S3DPoint tmppos = newpos;
            ScreenToWorld(x + posx, y + posy, newpos, tmppos.z);

            if (StatusBar.GridSnap())
            {
                newpos.x = newpos.x & GRIDMASK;
                newpos.y = newpos.y & GRIDMASK;
            }
            if (StatusBar.RestrictX())
                newpos.x = tmppos.x;
            if (StatusBar.RestrictY())
                newpos.y = tmppos.y;
        }

        oi->SetPos(newpos);
        StatusBar.SetDirty(TRUE);
    }
}

// *********************
// * Pane Manipulation *
// *********************

// Set Map Pos
void TMapPane::SetMapPos(RS3DPoint newpos)
{
    int nposx, nposy;

  // Set new pane scroll pos
    WorldToScreen(newpos, nposx, nposy);
    SetScrollPos(nposx - (MAPPANEWIDTH/2), nposy - (MAPPANEHEIGHT/2));
    center = newpos;
}

// ********************
// * Object Functions *
// ********************

// Create object
int TMapPane::NewObject(PSObjectDef def)
{
    PTObjectClass oc = TObjectClass::GetClass(def->objclass);
    if (!oc)
    {
        _RPT0(_CRT_WARN, "MAPPANE: Bad class in NewObject");
        return -1;
    }
    PTObjectInstance oi = oc->NewObject(def);
    if (!oi)
    {
        _RPT0(_CRT_WARN, "MAPPANE: Unable to create obj in NewObject");
        return -1;
    }

    int shadowindex = AddShadow(oi);

    oi->SetMapIndex(-1);
    int index = AddObject(oi);

    if (index < 0 && shadowindex >= 0)
    {
        PTObjectInstance shadow = GetInstance(shadowindex);
        if (shadow)
        {
            DeleteObject(shadow);
            delete shadow;
        }
    }

    return index;
}

// Add object
int TMapPane::AddObject(PTObjectInstance oi)
{
    if (oi->GetSector() != NULL)    // Object is already in the map
    {
        _RPT0(_CRT_WARN, "MAPPANE: Object already in a sector in AddObject");
        return -1;
    }

    if (oi->GetLevel() != GetMapLevel())
    {
        _RPT0(_CRT_WARN, "MAPPANE: Invalid level in AddObject");
        return -1;
    }

    S3DPoint pos;
    oi->GetPos(pos);

    int sx = (pos.x >> SECTORWSHIFT);
    int sy = (pos.y >> SECTORHSHIFT);

    if ((DWORD)sx >= MAXSECTORX || sx < sectorx || sx >= sectorx + SECTORWINDOWX ||
        (DWORD)sy >= MAXSECTORY || sy < sectory || sy >= sectory + SECTORWINDOWY)
    {
        _RPT0(_CRT_WARN, "MAPPANE: Object not in current map area in AddObject");
        return -1;
    }

    PTSector sect = sectors[sx - sectorx][sy - sectory];
    if (!sect)
    {
        _RPT0(_CRT_WARN, "MAPPANE: Sector unavailable in AddObject");
        return -1;
    }

    LOCKSECTORS;       // Lock sector system so update won't access it
                       // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                       // BETWEEN THESE TWO FUNCTIONS!!)

    sect->AddObject(oi);

    UNLOCKSECTORS;      // If this isn't called, system will lock up

    if (oi->GetMapIndex() < 0)
        oi->SetMapIndex(MakeIndex());

    if (oi->InventNum() < 0)
    {
        TransferWalkmap(oi);
        AddObjectUpdateRect(oi->GetMapIndex());
    }

    return oi->GetMapIndex();
}

// Remove object (The main place objects are removed)
PTObjectInstance TMapPane::RemoveObject(int index)
{
    PTObjectInstance inst = NULL;
    TMapIterator i;

    for ( ; i; i++)
        if (i->GetMapIndex() == index)
        {
            inst = i;
            break;
        }

    if (!inst)
        return NULL;

  // Notify that object is being deleted
    Notify(N_DELETINGOBJECT, inst);

    if (i.Parent())
    {
        // extract from inventory
        inst->RemoveFromInventory();
    }
    else
    {
        RemoveFromSector(inst, i.SectorX(), i.SectorY(), i.SectorIndex());
    }

    if (inst)
    {
        if (inst->GetShadow() >= 0)
            RemoveObject(inst->GetShadow());

        if (inst->HasAnimator())
            inst->FreeAnimator();
    }

    return inst;
}

// Delete object
void TMapPane::DeleteObject(PTObjectInstance obj)
{
    if (RemoveObject(obj->GetMapIndex()) == NULL)
        FatalError("Tried to delete an object not in the sector.  This is a Very Bad Thing(tm).  Get Adam to check this out RIGHT AWAY!");
    delete obj;
}

// Causes object to be put into new OBJSET_xxx lists when flags change in game
void TMapPane::ObjectFlagsChanged(PTObjectInstance oi, DWORD oldflags, DWORD newflags)
{
    if (!oi->GetSector())
        return;

    oi->GetSector()->ObjectFlagsChanged(oi, oldflags, newflags);
}

// Delete sector
void TMapPane::DeleteSector(PTSector sect)
{
    Notify(N_DELETINGSECTOR, sect);

  // Make sure all objects know they are off screen now
    for (TObjectIterator i(sect->ObjectArray()); i; i++)
    {
        if (!i.Item())
            continue;
        i.Item()->OffScreen();
    }

  // Saves and deletes
    TSector::CloseSector(sect);
}

PTObjectInstance TMapPane::RemoveFromSector(PTObjectInstance inst, int sx, int sy, int sectindex)
{
    // extract from sector
    ExtractWalkmap(inst);

    LOCKSECTORS;        // Prevent update system from accessing sector stuff
                        // Group the update rect and the actual removal below so update system doesn't
                        // accidently redraw this object before it is deleted

    AddObjectUpdateRect(inst->GetMapIndex());  
    sectors[sx][sy]->RemoveObject(sectindex);

    UNLOCKSECTORS;

    return inst;
}

// Add the shadow (if any) associated with an object
int TMapPane::AddShadow(PTObjectInstance oi)
{
    extern TObjectClass ShadowClass;

    if (oi->ObjClass() == OBJCLASS_SHADOW)
    {
        oi->SetShadow(-1);
        return -1;
    }

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));

    char buf[80];
    sprintf(buf, "shadow_%s", oi->GetTypeName());

    def.objclass = OBJCLASS_SHADOW;
    int objtype = ShadowClass.FindObjType(buf);
    if (objtype < 0)
        return -1;

    def.objtype = objtype;
    oi->GetPos(def.pos);

    int index = NewObject(&def);
    if (index >= 0)
        oi->SetShadow(index);

    return index;
}

// Object find functions
PTObjectInstance TMapPane::FindObject(char *name, int occurance, int objset)
{
    int found = 0;

    for (TMapIterator i(NULL, CHECK_NOINVENT, objset); i; i++)
    {
        char *instname = i->GetName();

        if (!stricmp(name, instname) && ++found == occurance)
            return i;
    }

    return NULL;
}

PTObjectInstance TMapPane::FindClosestObject(char *name, S3DPoint frompos, BOOL partial, int objset)
{
    PTObjectInstance closest = NULL;
    int closestdist = 0x800000;

    for (TMapIterator i(NULL, CHECK_NOINVENT, objset); i; i++)
    {
        char *instname = i->GetName();

        BOOL found = FALSE;

        if (partial)
        {
            if (instname && stricmp(name, instname) == 0)
                 found = TRUE;
        }
        else
        {
            if (instname && abbrevcmp(name, instname) > 0)
                 found = TRUE;
        }

        if (found)
        {
            S3DPoint pos;
            i->GetPos(pos);

            if (SQRDIST(pos, frompos) < closestdist)
            {
                closest = i;
                closestdist = SQRDIST(pos, frompos);
            }
        }
    }

    return closest;
}

PTObjectInstance TMapPane::FindClosestObject(char *name, PTObjectInstance from, BOOL partial, int objset)
{
    S3DPoint frompos;
    if (from)
        from->GetPos(frompos);
    else
        frompos = center;

    return FindClosestObject(name, frompos, partial, objset);
}

int TMapPane::FindObjectsInRange(S3DPoint pos, int *array, int width, int height, int objclass, int maxnum, int objset)
{
    if (maxnum < 1)
        return 0;

    S3DPoint itempos;
    int sqrwidth = sqr(width);
    int *dist = new int[maxnum];
    int found = 0, d;

    for (TMapIterator i(NULL, CHECK_NOINVENT, objset); i; i++)
    {
        PTObjectInstance inst = i;

        if (inst->IsInInventory() || (objclass != -1 && inst->ObjClass() != objclass))
            continue;

        inst->GetPos(itempos);

        if (height < 1) // This is a radius check
        {
            int r;

            // if we're looking for a character or player, take into account radius
            if (objclass != OBJCLASS_CHARACTER && objclass != OBJCLASS_PLAYER)
                r = 0;
            else
            {
                float tr = ((PTCharacter)inst)->Radius() / 2.5f;
                r = (int)(tr * tr);
            }

            if ((d = (SQRDIST(pos, itempos) - r)) < sqrwidth)
            {
                for (int j = 0; j < found && d > dist[j]; j++)
                    ;

                if (j >= maxnum)
                    continue;

                if (j != found)
                    memcpy(&(dist[j+1]), &(dist[j]), min(maxnum - 1, found) - j);

                array[j] = inst->GetMapIndex();
                dist[j] = d;

                if (found < maxnum)
                    found++;
            }
        }

        else
        {
            int dx = absval(pos.x - itempos.x);
            int dy = absval(pos.y - itempos.y);

            if (dx <= (width >> 1) && dy <= (height >> 1))
            {
                d = sqr(dx) + sqr(dy);

                for (int j = 0; j < found && d > dist[j]; j++)
                    ;

                if (j >= maxnum)
                    continue;

                if (j != found)
                    memcpy(&(dist[j+1]), &(dist[j]), min(maxnum - 1, found) - j);

                array[j] = inst->GetMapIndex();
                dist[j] = d;

                if (found < maxnum)
                    found++;
            }
        }
    }

    delete dist;
    return found;
}

PTObjectInstance TMapPane::ObjectInCube(PS3DRect cube, int objset)
{
    PTObjectInstance in = NULL;

    for (TMapIterator i(NULL, CHECK_NOINVENT, objset); i; i++)
    {
        if (cube->In(i->Pos()))
            return i;
    }

    return NULL;
}   

PTObjectInstance TMapPane::OnObject(int screenx, int screeny, PTObjectInstance with)
{
    BOOL IsPriorityItem = FALSE;
    PTObjectInstance on = NULL;

    screenx += posx;
    screeny += posy;

    SRect r;
    SPoint p;
    p.x = screenx;
    p.y = screeny;

    updatemulti->SetClipRect(screenx, screeny, 1, 1);

    for (TMapIterator i(NULL, CHECK_NOINVENT); i; i++)
    {
        PTObjectInstance inst = i;
        if (!inst->OnObject(p))
            continue;

        if (inst->IsInInventory() || (!Editor && inst->ObjClass() == OBJCLASS_TILE))
            continue;

        if (!on || inst->AlwaysOnTop() || (!IsPriorityItem && inst->GetZ(updatemulti)))
        {
            BOOL good = TRUE;

            if (!Editor)
            {
                if ((GetDragObj() || !inst->IsInventoryItem()) && inst->CursorType(with) == CURSOR_NONE)
                    good = FALSE;
            }

            if (good)
            {
                on = inst;
                IsPriorityItem = inst->AlwaysOnTop();
            }
        }
    }

    updatemulti->ResetClipRect();

    return on;
}

// Returns the instance structure given an object index
PTObjectInstance TMapPane::GetInstance(int index, int objset)
{
    if (index < 0)
        return NULL;

    TMapIterator i(NULL, 0, objset);

    for ( ; i; i++)
        if (i->GetMapIndex() == index)
            break;

    return i;

#ifdef OLD_STYLE_MAP_INDEXES
    int lv = GETLEVEL(index);

    if (lv != level)
        return NULL;

    int sx = GETSECTORX(index);
    int sy = GETSECTORY(index);

    if ((DWORD)sx >= MAXSECTORX || sx < sectorx || sx >= sectorx + SECTORWINDOWX ||
        (DWORD)sy >= MAXSECTORY || sy < sectory || sy >= sectory + SECTORWINDOWY)
            return NULL;

    if (!sectors[sx - sectorx][sy - sectory])
        return NULL; 

    return sectors[sx - sectorx][sy - sectory]->GetInstance(GETITEM(index));
#endif
}

// *********************
// * Walkmap Functions *
// *********************

void PosToWalkGrid(RS3DPoint pos, int &x, int &y)
{
    // because 0, 0 is actually the center of a walk grid, not the edge,
    // everything has to be offset by half the walk grid size

    x = (pos.x + (GRIDSIZE / 2)) >> WALKMAPSHIFT;
    y = (pos.y + (GRIDSIZE / 2)) >> WALKMAPSHIFT;
}

void WalkGridToPos(int x, int y, RS3DPoint pos)
{
    // because 0, 0 is actually the center of a walk grid, not the edge,
    // everything has to be offset by half the walk grid size

    pos.x = (x << WALKMAPSHIFT) - (GRIDSIZE / 2);
    pos.y = (y << WALKMAPSHIFT) - (GRIDSIZE / 2);
    pos.z = 0;
}

// General purpose handler for transfer, extract, clear etc
void TMapPane::WalkmapHandler(PTObjectInstance oi, int mode)
{
    if (!oi || oi->GetMapIndex() < 0 || 
      ((mode == WALK_TRANSFER) && (oi->Flags() & OF_NOWALK)))
        return;

    PTObjectImagery imagery = oi->GetImagery();
    if (!imagery)
        return;

    BYTE *walk = imagery->GetWalkMap(oi->GetState());
    if (!walk)
        return;

    int width, length, height;
    imagery->GetWorldBoundBox(oi->GetState(), width, length, height);

    int regx = imagery->GetWorldRegX(oi->GetState());
    int regy = imagery->GetWorldRegY(oi->GetState());

    BYTE *appliedwalk = NULL;
    if (oi->GetFace() != 0)
    {
        // apply rotations to walkmap based on facing
        appliedwalk = (BYTE *)malloc(width * length);

        int nx, ny, nsx, nsy;
        oi->GetFacingBoundBox(nx, ny, nsx, nsy);

        if (oi->GetFace() < 128)
        {
            for (int y = 0; y < nsy; y++)
                for (int x = 0; x < nsx; x++)
                    *(appliedwalk+(y*nsx)+x) = *(walk+((length-x-1)*width)+y);
        }
        else if (oi->GetFace() < 192)
        {
            for (int y = 0; y < nsy; y++)
                for (int x = 0; x < nsx; x++)
                    *(appliedwalk+(y*nsx)+x) = *(walk+(y*width)+(width-x-1));
        }
        else
        {
            for (int y = 0; y < nsy; y++)
                for (int x = 0; x < nsx; x++)
                    *(appliedwalk+(y*nsx)+x) = *(walk+((length-x-1)*width)+(width-x-1));
        }

        regx = nx;
        regy = ny;
        width = nsx;
        length = nsy;
    }

    S3DPoint pos;
    oi->GetPos(pos);

    BOOL override = (oi->ObjClass() == OBJCLASS_EXIT);
    if (oi->ObjClass() == OBJCLASS_EXIT)
        override = TRUE;

    int x = (pos.x >> WALKMAPSHIFT) - regx;
    int y = (pos.y >> WALKMAPSHIFT) - regy;

    int startsectx = (x << WALKMAPSHIFT) >> SECTORWSHIFT;
    int startsecty = (y << WALKMAPSHIFT) >> SECTORHSHIFT;
    int endsectx = ((x + width - 1) << WALKMAPSHIFT) >> SECTORWSHIFT;
    int endsecty = ((y + length - 1) << WALKMAPSHIFT) >> SECTORHSHIFT;

    for (int sy = startsecty; sy <= endsecty; sy++)
        for (int sx = startsectx; sx <= endsectx; sx++)
        {
            if ((DWORD)sx >= MAXSECTORX || sx < sectorx || sx >= sectorx + SECTORWINDOWX ||
                (DWORD)sy >= MAXSECTORY || sy < sectory || sy >= sectory + SECTORWINDOWY)
                continue;

            PTSector sect = sectors[sx - sectorx][sy - sectory];
            if (!sect)
                continue;

            if (mode == WALK_EXTRACT)
            {
                sect->WalkmapHandler(WALK_CLEAR, appliedwalk ? appliedwalk : walk, 0, x, y, width, length, width);
                RedrawWalkmapRect(oi, x, y, width, length, sect);
            }
            else
                sect->WalkmapHandler(mode, appliedwalk ? appliedwalk : walk, pos.z, x, y, width, length, width, override);
        }

    imagery->SetHeaderDirty(TRUE);

    if (appliedwalk)
        free(appliedwalk);
}

void TMapPane::RedrawWalkmapRect(PTObjectInstance oi, int x, int y, int w, int l, PTSector dsect)
{
    for (int sx = 0; sx < SECTORWINDOWX; sx++)
        for (int sy = 0; sy < SECTORWINDOWY; sy++)
        {
            PTSector sect = sectors[sx][sy];
            if (!sect)
                continue;

            for (TObjectIterator i(sect->ObjectArray()); i; i++)
            {
                PTObjectInstance inst = i.Item();

                if (!inst || oi == inst || inst->IsInInventory() || (inst->Flags() & OF_NOWALK))
                    continue;

                PTObjectImagery imagery = inst->GetImagery();
                if (!imagery)
                    continue;

                BYTE *walk = imagery->GetWalkMap(inst->GetState());
                if (!walk)
                    continue;

                int iw, il, ih;
                imagery->GetWorldBoundBox(inst->GetState(), iw, il, ih);

                int rx, ry;
                rx = imagery->GetWorldRegX(inst->GetState());
                ry = imagery->GetWorldRegY(inst->GetState());

                S3DPoint pos;
                inst->GetPos(pos);

                int ix = (pos.x >> WALKMAPSHIFT) - rx;
                int iy = (pos.y >> WALKMAPSHIFT) - ry;

                // clip this bad boy
                int cx = ix - x, cy = iy - y, cw = iw, cl = il;
                int ax = 0, ay = 0;

                if (cx < 0)
                {
                    cw += cx;
                    ax = -cx;
                    cx = 0;
                }

                if (cy < 0)
                {
                    cl += cy;
                    ay = -cy;
                    cy = 0;
                }

                if ((cx + cw) > w)
                    cw = w - cx;

                if ((cy + cl) > l)
                    cl = l - cy;

                if (cw <= 0 || cl <= 0)
                    continue;

                cx += x;
                cy += y;
                walk += (ay * iw) + ax;

                dsect->WalkmapHandler(WALK_TRANSFER, walk, pos.z, cx, cy, cw, cl, iw);
            }
        }
}

int TMapPane::GetWalkHeight(RS3DPoint pos)
{
    // because 0, 0 is actually the center of a walk grid, not the edge,
    // everything has to be offset by half the walk grid size

    int x = pos.x + (GRIDSIZE / 2);
    int y = pos.y + (GRIDSIZE / 2);

    int sx = (x >> SECTORWSHIFT) - sectorx;
    int sy = (y >> SECTORHSHIFT) - sectory;

    if (sx < 0 || sx >= SECTORWINDOWX || sy < 0 || sy >= SECTORWINDOWY || !sectors[sx][sy])
        return 0;

    x = (x & (SECTORWIDTH - 1)) >> WALKMAPSHIFT;
    y = (y & (SECTORHEIGHT - 1)) >> WALKMAPSHIFT;

    return sectors[sx][sy]->ReturnWalkmap(x, y);
}

int TMapPane::GetWalkGridHeight(int x, int y)
{
    // because 0, 0 is actually the center of a walk grid, not the edge,
    // everything has to be offset by half the walk grid size

    int sx = (x >> (SECTORWSHIFT - WALKMAPSHIFT)) - sectorx;
    int sy = (y >> (SECTORHSHIFT - WALKMAPSHIFT)) - sectory;

    if (sx < 0 || sx >= SECTORWINDOWX || sy < 0 || sy >= SECTORWINDOWY || !sectors[sx][sy])
        return 0;

    int wx = x & ((1 << (SECTORWSHIFT - WALKMAPSHIFT)) - 1);
    int wy = y & ((1 << (SECTORHSHIFT - WALKMAPSHIFT)) - 1);

    return sectors[sx][sy]->ReturnWalkmap(wx, wy);
}

// Returns the maximum walk height in the area bounded by 
// pos - width/2,height/2 to pos + with/2,height/2
int TMapPane::GetWalkHeightArea(RS3DPoint pos, int width, int height)
{
    if (width == 0)
        return GetWalkHeight(pos);

    width /= 2;     // to make the calculations simpler
    height /= 2;

    // make sure they are aligned values
    width &= GRIDMASK;
    height &= GRIDMASK;

    S3DPoint p(0, 0, 0);
    int highest = 0;

    for (p.y = (pos.y - height); p.y <= (pos.y + height); p.y += GRIDSIZE)
        for (p.x = (pos.x - width) ; p.x <= (pos.x + width); p.x += GRIDSIZE)
        {
            int h = GetWalkHeight(p);

            if (h == 0)
                return 0;

            if (h > highest)
                highest = h;
        }

    return highest;
}

// Returns the minimum delta (fall), and maximum delta (rise) between any two walk grids 
// within the radius, and also the height at the current position.

#define MAXRADIUS 8

void TMapPane::GetWalkHeightRadius(RS3DPoint pos, int radius, 
    int &mindelta, int &maxdelta, int &curheight)
{
    static int heights[MAXRADIUS][MAXRADIUS]; 
    mindelta = 0;
    maxdelta = 0;

    curheight = GetWalkHeight(pos);

    if (radius <= 0)
        return;

    if (((radius + (GRIDSIZE - 1)) >> WALKMAPSHIFT) >= MAXRADIUS)
        radius = ((MAXRADIUS - 1) << WALKMAPSHIFT) - 1;

    S3DPoint p(0, 0, 0);

    int startx = (pos.x - radius) & (int)GRIDMASK;
    int starty = (pos.y - radius) & (int)GRIDMASK;
    int endx =   (pos.x + radius + GRIDSIZE) & (int)GRIDMASK;
    int endy =   (pos.y + radius + GRIDSIZE) & (int)GRIDMASK;

    mindelta = 1000;
    maxdelta = -1000;

    for (p.y = starty; p.y <= endy; p.y += GRIDSIZE)
    {
        for (p.x = startx; p.x <= endx; p.x += GRIDSIZE)
        {
            int x = (p.x - startx) >> WALKMAPSHIFT;
            int y = (p.y - starty) >> WALKMAPSHIFT;

            int xdelta, ydelta, height;
            
            xdelta = ydelta = 0;

          // Test closest corners of walk grids
            if (p.y <= pos.y)
            {
              if (p.x <= pos.x) 
              {
                if (dist(p.x + GRIDSIZE / 2 - 1, p.y + GRIDSIZE / 2 - 1, pos.x, pos.y) > radius)
                {
                    heights[x][y] = -1;
                    continue;
                }
  
              // Within radius, now get deltas between this grid and grid to left and above
              // Deltas will be positive or negative depending on direction from center
                height = heights[x][y] = GetWalkHeight(p);
                if (x > 0 && heights[x - 1][y] >= 0)
                    xdelta = heights[x - 1][y] - height;
                if (y > 0 && heights[x][y - 1] >= 0)
                    ydelta = heights[x][y - 1] - height;
              }
              else
              {
                if (dist(p.x - GRIDSIZE / 2, p.y + GRIDSIZE / 2 - 1, pos.x, pos.y) > radius)
                {
                    heights[x][y] = -1;
                    continue;
                }

              // Within radius, now get deltas between this grid and grid to left and above
              // Deltas will be positive or negative depending on direction from center
                height = heights[x][y] = GetWalkHeight(p);
                if (x > 0 && heights[x - 1][y] >= 0)
                    xdelta = height - heights[x - 1][y];
                if (y > 0 && heights[x][y - 1] >= 0)
                    ydelta = heights[x][y - 1] - height;
              }
            }
            else
            {
              if (p.x <= pos.x) 
              {
                if (dist(p.x + GRIDSIZE / 2 - 1, p.y - GRIDSIZE / 2, pos.x, pos.y) > radius)
                {
                    heights[x][y] = -1;
                    continue;
                }

              // Within radius, now get deltas between this grid and grid to left and above
              // Deltas will be positive or negative depending on direction from center
                height = heights[x][y] = GetWalkHeight(p);
                if (x > 0 && heights[x - 1][y] >= 0)
                    xdelta = heights[x - 1][y] - height;
                if (y > 0 && heights[x][y - 1] >= 0)
                    ydelta = height - heights[x][y - 1];
              }
              else
              {
                if (dist(p.x - GRIDSIZE / 2, p.y - GRIDSIZE / 2, pos.x, pos.y) > radius)
                {
                    heights[x][y] = -1;
                    continue;
                }

              // Within radius, now get deltas between this grid and grid to left and above
              // Deltas will be positive or negative depending on direction from center
                height = heights[x][y] = GetWalkHeight(p);
                if (x > 0 && heights[x - 1][y] >= 0)
                    xdelta = height - heights[x - 1][y];
                if (y > 0 && heights[x][y - 1] >= 0)
                    ydelta = height - heights[x][y - 1];
              }
            }

          // Get minimum/maximum deltas
            if (xdelta > maxdelta)
                maxdelta = xdelta;
            else if (xdelta < mindelta)
                mindelta = xdelta;
            if (ydelta > maxdelta)
                maxdelta = ydelta;
            else if (ydelta < mindelta)
                mindelta = ydelta;
        }
    }
}

void TMapPane::TransferAllWalkmaps()
{
    for (TMapIterator i(NULL, CHECK_NOINVENT); i; i++)
        TransferWalkmap(i);
}

void TMapPane::ClearWalkmaps()
{
    for (int sx = 0; sx < SECTORWINDOWX; sx++)
    {
        for (int sy = 0; sy < SECTORWINDOWY; sy++)
        {
            PTSector sect = sectors[sx][sy];
            if (!sect)
                continue;

            sect->ClearWalkmap();
        }
    }
}

BOOL TMapPane::LineOfSight(RS3DPoint pos, RS3DPoint to, PS3DPoint obst)
{
    int sx, sy, sz, ex, ey, ez, dx, dy, dz;

  // Get starting values in grid coordinates
    PosToWalkGrid(pos, sx, sy);
    PosToWalkGrid(to, ex, ey);
    sz = pos.z;
    ez = to.z;

    dx = ex - sx;
    dy = ey - sy;
    dz = ez - sz;

  // Make fixed point increment values for drawing line, and set loop to bigest value   
    int loop;
    if (abs(dx) >= abs(dy))
    {
        loop = abs(dx);
        if (loop > 0)
        {
          dx = (dx << 16) / loop;
          dy = (dy << 16) / loop + ((dy >= 0)?1:-1);  
          dz = (dz << 16) / loop + ((dz >= 0)?1:-1);
        }
    }
    else
    {
        loop = abs(dy);
        if (loop > 0)
        {
          dx = (dx << 16) / loop + ((dx >= 0)?1:-1);
          dy = (dy << 16) / loop;  
          dz = (dz << 16) / loop + ((dz >= 0)?1:-1);
        }
    }

    int x = sx << 16; 
    int y = sy << 16;
    int z = sz << 16;

    for (int c = 0; c <= loop; c++)
    {
        int gridheight = GetWalkGridHeight(x >> 16, y >> 16);
        if (gridheight == 0 || gridheight > (z >> 16))
        {
            if (obst)
            {
                WalkGridToPos(x >> 16, y >> 16, *obst);
                obst->z = z >> 16;
            }
            return FALSE;
        } 
        x += dx;
        y += dy;
        z += dz;
    }
                        
    return TRUE;
}

// ***************************
// * Dynamic Light Functions *
// ***************************

void TMapPane::DrawDLight()
{
    static int oldintensity = 0;

    return;

    if (oldintensity != dlight.intensity && dlight.lightindex >= 0)
    {
        Scene3D.DeleteLight(dlight.lightindex);
        dlight.lightindex = -1;
    }
    oldintensity = dlight.intensity;

    if (dlight.intensity == 0)
        return;

    if (dlight.lightindex < 0)
    {
        dlight.lightindex = Scene3D.AddLight(dlight.pos, dlight.color, dlight.intensity, dlight.multiplier);
    }
    else
    {
        Scene3D.SetLightPosition(dlight.lightindex, dlight.pos);
    }

    S3DPoint spos;
    WorldToScreen(dlight.pos, spos);

//  if (NoNormals)
        DrawLightNoNormals(spos, dlight.color, dlight.intensity, Display);
//  else
//      DrawLight(spos, dlight.color, dlight.intensity, Display);

    PTBitmap bitmap = GameData->Bitmap("dlight");
    Display->Put(spos.x - bitmap->width / 2,
                 spos.y - bitmap->width / 2 - 20, 
                 bitmap, DM_ALPHALIGHTEN); //, NULL, 31 - (dlight.intensity / 8));
}

void TMapPane::SetAmbientLight(int light, BOOL stopfade)
{
    if (stopfade)
        totambfadeframes = totambfadesteps = 0; // Stop fading

    if (totambfadeframes > 0)  // Doing fade? Then change target value
    {
        newambient = light;
    }
    else
    {
        if (ambient == light)
            return;
        ambient = light; 
        ambientchanged = TRUE;  // Causes ambient to be reset in next call to DrawBackground()
    }
}   

void TMapPane::SetAmbientColor(SColor &color, BOOL stopfade)
{
    if (stopfade)
        totambfadeframes = totambfadesteps = 0; // Stop fading

    if (totambfadeframes > 0)  // Doing fade? Then change target value
    {
        newambcolor = color;
    }
    else
    {
        if (ambcolor.red == color.red &&
            ambcolor.green == color.green &&
            ambcolor.blue == color.blue)
                return;
        ambcolor = color;
        ambientchanged = TRUE;  // Causes ambient to be reset in next call to DrawBackground()
    }
}

// Fades to the given ambient light in 'frame' frames in 'steps' discreet steps
void TMapPane::FadeAmbient(int light, SColor &color, int frames, int steps)
{
    if (GameSpeed == 5)
        steps = frames; // Do smooth ambient fading for fast computers

    if (light == ambient && 
        color.red == ambcolor.red && 
        color.green == ambcolor.green && 
        color.blue == ambcolor.blue)
            return;

    oldambient = ambient;
    oldambcolor = ambcolor;
    newambient = light;
    newambcolor = color;
    totambfadeframes = frames;
    totambfadesteps = steps;
    ambfadeframes = 0;
    ambfadesteps = 0;
}

// Called to actually set the new ambient values
void TMapPane::PulseFadeAmbient()
{
    if (totambfadeframes <= 0 || totambfadesteps <= 0)
        return;

    if (ambfadeframes != totambfadeframes * ambfadesteps / totambfadesteps)
    {
        ambfadeframes++;
        return;
    }

  // Interpolate between old and new ambient values
    ambient = oldambient * (totambfadesteps - ambfadesteps) / totambfadesteps + 
            newambient * ambfadesteps / totambfadesteps;
    ambcolor.red = (BYTE)((int)((int)oldambcolor.red * (totambfadesteps - ambfadesteps) / totambfadesteps + 
            (int)newambcolor.red * ambfadesteps / totambfadesteps));
    ambcolor.green = (BYTE)((int)((int)oldambcolor.green * (totambfadesteps - ambfadesteps) / totambfadesteps + 
            (int)newambcolor.green * ambfadesteps / totambfadesteps));
    ambcolor.blue = (BYTE)((int)((int)oldambcolor.blue * (totambfadesteps - ambfadesteps) / totambfadesteps + 
            (int)newambcolor.blue * ambfadesteps / totambfadesteps));
    ambientchanged = TRUE;

  // Next fade step
    ambfadeframes++;
    ambfadesteps++;
    if (ambfadesteps > totambfadesteps)         // Done
        totambfadeframes = totambfadesteps = 0;
}

// ********************************
// * Background Drawing Functions *
// ********************************

void TMapPane::Update3DScenePos()
{
    S3DPoint source;
    S3DPoint target;
    S3DPoint scrn;

    target = center;

    // convert the position to have no z height for d3d - it makes no difference
    // for what you see on the screen, and seems to fix some problems with the d3d view
    if (target.z > 0)
    {
        int x, y;
        WorldToScreen(target, x, y);
        ScreenToWorld(x, y, target);
    }


    WorldToScreen(target, scrn);

    Scene3D.SetCameraPos(target, (int)(scrn.z * ZSCALE));
}

// UpdateMapPos - Checks the system movement params and sets the map pane position.

void TMapPane::UpdateMapPos()
{

    if (Editor)     // Set map position code for editor
    {               // --------------------------------
        if (SmoothScroll)
        {
            if (mx || my)
            {
                S3DPoint mpos;
                ScreenToWorld(mx, my, mpos);
                mpos += center;
                SetMapPos(mpos);
                StatusBar.SetDirty(TRUE);
            }
        }
        else
        {
            int sx, sy;
            WorldToScreen(center, sx, sy);
            int x = ScreenGrid(sx, MAPGRIDWIDTH) * MAPGRIDWIDTH;
            int y = ScreenGrid(sy - CHARACTER_HEIGHT, MAPGRIDHEIGHT) * MAPGRIDHEIGHT;

            if (x != sx || y != sy)
            {
                S3DPoint newpos;
                ScreenToWorld(x, y, newpos);
                SetMapPos(newpos);
                RedrawAll();
            }
        }
    }
    else        // Set map position code for regular game
    {           // --------------------------------------

        S3DPoint pos, newpos;
        GetMapPos(newpos);
        int newlevel = GetMapLevel();
        static S3DPoint vel(0, 0, 0);
        static S3DPoint lastpos;
        static int lastlevel;

      // Regular game uses the contents of the 'centeron' structure to attempt
      // to center on either a point or an object.  The center on structure is
      // set by the CenterOn() functions, and can be called from a script.

      // Get what we're supposed to center on
        if (centeron.flags & CENTERON_OBJ)
        {
            if (centeron.obj)   // Note: check for validity of this object in Notify()
            {
                centeron.obj->GetPos(pos);
                newlevel = centeron.obj->GetLevel();
            }
        }
        else if (centeron.flags & CENTERON_POS)
        {
            pos = centeron.pos;
            newlevel = centeron.level;
        }
        else
        {
            pos = newpos; // Center on nothing
            newlevel = GetMapLevel();
        }

      // Smooth out z (average last five z positions)
        static int zsmoothlist[5];
        static int zsmoothpos;
        if (dist(lastpos.x, lastpos.y, pos.x, pos.y) < 32)
        {
            zsmoothlist[zsmoothpos] = pos.z;
            pos.z = (zsmoothlist[0] + 
                     zsmoothlist[1] +
                     zsmoothlist[2] +
                     zsmoothlist[3] +    
                     zsmoothlist[4]) / 5;
            zsmoothpos++;
            if (zsmoothpos >= 5)
                zsmoothpos = 0;
        }
        else
        {
            zsmoothlist[0] = 
                zsmoothlist[1] =     
                zsmoothlist[2] =      
                zsmoothlist[3] =     
                zsmoothlist[4] = pos.z;
            zsmoothpos = 0;
        }

      // If scrolling, handle scroll velocities to new position (only if scroll on and level is same)
        if (SmoothScroll && newlevel == GetMapLevel() && (centeron.flags & CENTERON_SCROLL))
        {
            S3DPoint newvel;

            int factor = 10;
            if (ScrollLock)
                factor = 6;

            newvel.x = (pos.x - center.x) / factor;
            newvel.y = (pos.y - center.y) / factor;
            newvel.z = (pos.z - center.z) / factor;

            if (newvel.x > vel.x)
                vel.x++;
            else if (newvel.x < vel.x)
                vel.x--;

            if (newvel.y > vel.y)
                vel.y++;
            else if (newvel.y < vel.y)
                vel.y--;

            if (newvel.z > vel.z)
                vel.z++;
            else if (newvel.z < vel.z)
                vel.z--;

            vel.x = max(-12, min(12, vel.x));
            vel.y = max(-12, min(12, vel.y));
            vel.z = max(-12, min(12, vel.z));

            newpos += vel;
        }
        else
        {
            vel.x = vel.y = vel.z = 0;
            newpos = pos;   // No scrolling,.. just set pos
        }
        
        if (SmoothScroll)
            SetMapPos(newpos);
        else                // Non smooth scrolling, calculate screen grid and set pos
        {
            int x, y;
            WorldToScreen(pos, x, y);
            x = ScreenGrid(x, MAPGRIDWIDTH);
            y = ScreenGrid(y - CHARACTER_HEIGHT, MAPGRIDHEIGHT);

            int sx, sy;
            WorldToScreen(center, sx, sy);
            sx = ScreenGrid(sx, MAPGRIDWIDTH);
            sy = ScreenGrid(sy, MAPGRIDHEIGHT);

            x *= MAPGRIDWIDTH;
            y *= MAPGRIDHEIGHT;
            ScreenToWorld(x, y, newpos);
            if (absval(newpos.x - center.x) > 4 || absval(newpos.y - center.y) > 4)
            //if (x != sx || y != sy)
            {
                //x *= MAPGRIDWIDTH;
                //y *= MAPGRIDHEIGHT;
                //ScreenToWorld(x, y, newpos);
                SetMapPos(newpos);
                MapPane.RedrawAll();
            }
        }

        SetMapLevel(newlevel);

      // Set Last values
        lastpos = pos;
        lastlevel = level;
    }       
}

// The pulse function is pretty much the first function that's called in a screen refresh.
// You can move objects around, change the map pane position, and do anything else you
// want to before the map pane is scrolled or moved, the background is drawn, etc.
//
// The Pulse function should be where the object AI stuff is called so that it can be
// updated before the objects are displayed in the DrawBackground() and Animate() funcitons.

void TMapPane::Pulse()
{
  // Pulse fading ambient values
    PulseFadeAmbient(); 

  // Do next frame for objects except if first frame of screen
    if (!CurrentScreen->FirstFrame())
        NextFrameObjects();

  // Pulse objects (does object AI)
    PulseObjects();

  // Moves all objects
    MoveObjects();

  // Update map position (uses mouse scrolling for editor or 'centeron' state for game)
    UpdateMapPos();

  // Set new screen map position now that objects have been pulsed
    oldposx     = posx;
    posx        = GetScrollX();
    oldposy     = posy;
    posy        = GetScrollY();

  // Update 3D position
    Update3DScenePos();

  // Update Sectors
    UpdateSectors();

  // Leave Pulse with new map position completely set
}

void TMapPane::DrawBackground()
{
  // *********************** GLOBAL STATE CHANGES *************************
  
  // **** THIS IS WHERE ANY GLOBAL GAME IMAGERY CHANGES SHOULD BE MADE ****
  // **** FUNCTIONS WHICH CHANGE IMAGERY STATES (like ambient lighting,****
  // **** ETC. SHOULD FLAG THAT THEY NEED TO BE UPDATED, AND THIS      ****
  // **** FUNCTION SHOULD ACTUALLY DO THE UPDATING.  THIS PREVENTS A   ****
  // **** GOBAL STATE FROM BEING SET HALFWAY THROUGH A FRAME           ****
    
  // Causes ambient values to be changed at the beginning of a frame refresh
    if (ambientchanged)
    {
        ::SetAmbientLight(ambient);
        Scene3D.SetAmbientLight(ambient);
        ::SetAmbientColor(ambcolor);
        Scene3D.SetAmbientColor(ambcolor);
    
        ambientchanged = FALSE;

        SRect r;
        r.left = GetScrollX() - 10000;
        r.top = GetScrollY() - 10000;
        r.right = GetScrollX() + 10000;
        r.bottom = GetScrollY() + 10000;
        AddBgUpdateRect(r, BGDRAW_AMBIENT);
    }

  // Update edges of screen
    int updatex = GetScrollX() & UPDATEMASKX;
    int updatey = GetScrollY() & UPDATEMASKY;
    UpdateEdges(updatex, updatey);

  // If redraw all, refresh the whole zbuffer
    if (IsDirty())
    {
        SRect r;
        r.left = posx;
        r.top = posy;
        r.right = r.left + GetWidth() - 1;
        r.bottom = r.top + GetHeight() - 1;
        Scene3D.RestoreZBuffer(r);
    }

    SetDirty(FALSE);
}

void TMapPane::RedrawAll()
{
    SetDirty(TRUE);
    scrollx += 1000000;
    scrolly += 1000000;
    ClearLights();

    // stuff to make the smoothscroll command work better
    if (!Editor && Player && !SmoothScroll)
    {
        S3DPoint pos, newpos;
        Player->GetPos(pos);

        int x, y;
        WorldToScreen(pos, x, y);
        x = ScreenGrid(x, MAPGRIDWIDTH);
        y = ScreenGrid(y - CHARACTER_HEIGHT, MAPGRIDHEIGHT);
        x *= MAPGRIDWIDTH;
        y *= MAPGRIDHEIGHT;
        ScreenToWorld(x, y, newpos);

        if (absval(newpos.x - center.x) > 4 || absval(newpos.y - center.y) > 4)
            SetMapPos(newpos);
    }
}

void TMapPane::AddBgUpdateRect(SRect &r, int bgdraw)
{
    if (IsDirty() || bgdraw >= BGDRAW_NONE)
        return; // No need since everything will be redrawn anyway

    if (numbgrects >= MAXMAPBGRECTS)
    {
        SetDirty(TRUE);     // This pane is nasty bad (causes pane to redraw everything)
        numbgrects = 0;
        return;
    }

    bgrects[numbgrects].rect = r;
    bgrects[numbgrects].bgdraw = bgdraw;
    numbgrects++;
}

void TMapPane::AddObjectUpdateRect(int index)
{
    PTObjectInstance inst = GetInstance(index);
    if (!inst)
        return;

    SRect ir;
    inst->GetScreenRect(ir);
    AddBgUpdateRect(ir);

    inst->RedrawLight();            // in case it's a light
}

void TMapPane::ReloadImagery()
{
    if (NoNormals == TRUE)
        TObjectImagery::SetImageryPath(NONORMALPATH);
    else
        TObjectImagery::SetImageryPath(NORMALPATH);
    
    TObjectImagery::ReloadImagery();

    RedrawAll();
}

// ***************************************************************************************
// ***************************  Progressive Update System   ******************************
// ***************************************************************************************

static BOOL ThreadRunning, QuitThread, Updating, UpdateCancelled;
static HANDLE UpdateStartEvent, UpdateDoneEvent, PauseUpdateMutex;
static HANDLE SectorMutex, ObjectMutex;
static HANDLE UpdateThreadHandle;
static unsigned UpdateThreadId;

#define UPDATEWAITSAMPLES 5
static int UpdateWaitSample, UpdateSleep;
static int UpdateWaitFrames[UPDATEWAITSAMPLES];
static int UpdateWaitTime[UPDATEWAITSAMPLES];

#define MAXQUEUERECTS (MAXMAPBGRECTS + 2)
static int numqueuerects;
static SBgUpdateRect queuerects[MAXQUEUERECTS];

static char *PauseMutexFile, *SectorMutexFile, *ObjectMutexFile;
static int PauseMutexLine, SectorMutexLine, ObjectMutexLine;

static TObjectArray UpdateObjs;

void TMapPane::BeginUpdateThread()
{
    if (ThreadRunning)
        return;

    UpdateWaitSample = 0;
    UpdateSleep = 0;

    UpdateStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    UpdateDoneEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    PauseUpdateMutex = CreateMutex(NULL, FALSE, NULL);
    SectorMutex = CreateMutex(NULL, FALSE, NULL);
    ObjectMutex = CreateMutex(NULL, FALSE, NULL);

    QuitThread = FALSE;
    Updating = FALSE;
    UpdateCancelled = FALSE;

    UpdateThreadHandle = (HANDLE)_beginthreadex(NULL, 0, TMapPane::UpdateThread, NULL, TRUE,
        &UpdateThreadId);

    ThreadRunning = TRUE;
}

void TMapPane::EndUpdateThread()
{
    if (!ThreadRunning)
        return;

    while (ReleaseMutex(PauseUpdateMutex));
    while (ReleaseMutex(SectorMutex));
    while (ReleaseMutex(ObjectMutex));

    CancelUpdate();
    QuitThread = TRUE;
    PulseEvent(UpdateStartEvent);

    WaitForSingleObject(UpdateThreadHandle, INFINITE);
    
    ThreadRunning = FALSE;
}

void ClearMutex(HANDLE hmutex)
{
#ifdef DEBUG
    char *err = "%s not released in file %s line %s";
    char buf[80];
    if (hmutex == PauseUpdateMutex && PauseMutexFile != NULL)
    {
        sprintf(buf, err, "PauseUpdateMutex", PauseMutexFile, PauseMutexLine);
        ThreadError(buf);
    }
    else if (hmutex == SectorMutex && SectorMutexFile != NULL)
    {
        sprintf(buf, err, "SectorMutex", SectorMutexFile, SectorMutexLine);
        ThreadError(buf);
    }
    else if (hmutex == ObjectMutex && ObjectMutexFile != NULL)
    {
        sprintf(buf, err, "ObjectMutex", ObjectMutexFile, ObjectMutexLine);
        ThreadError(buf);
    }
#endif

    while (ReleaseMutex(hmutex));
}

int TMapPane::GetUpdateSleep()
{
    return UpdateSleep;
}

void TMapPane::UpdateTimeSlice(BOOL draw)
{
    if (!ThreadRunning)
        return;
    
 // Check to see if queued update is finished yet
    if (isrecqueued)
    {
        if (UpdateDone())
        {
            scrollrect = queuedrect; // Set valid scroll area to queued rect if done
            isrecqueued = FALSE;
            PutQueueRectsToDisplay();  // Causes display to show new rects
            ClearQueueRects();
        }
        else // If still waiting, allow time for update thread to run
        {
            if (UpdateSleep > 0)
            {
                int start = GetTickCount();
                SetEvent(UpdateStartEvent);
                WaitForSingleObject(UpdateDoneEvent, UpdateSleep); // Not too long
                ResetEvent(UpdateDoneEvent);
                int end = GetTickCount();
                int time = end - start;
//              if (time < UpdateSleep)
//                  Sleep(UpdateSleep - time);
                UpdateWaitTime[UpdateWaitSample] += time;
            }
        } 
    }

    UpdateWaitFrames[UpdateWaitSample]++;
    if (UpdateWaitFrames[UpdateWaitSample] > 24)
        AdjustTimeSlice(0);
}

void TMapPane::AdjustTimeSlice(int milliseconds)
{
    if (!ThreadRunning)
        return;

    UpdateWaitTime[UpdateWaitSample] += milliseconds;

    UpdateSleep = 0;
    for (int c = 0; c < UPDATEWAITSAMPLES; c++)
    {
        UpdateSleep = max(UpdateSleep, 
            (UpdateWaitTime[c] * 100 + 80) / (max(UpdateWaitFrames[c], 1) * 100));
    }

    if (UpdateSleep > 70)
        UpdateSleep = 70;

    UpdateWaitSample++;
    if (UpdateWaitSample >= UPDATEWAITSAMPLES)
        UpdateWaitSample = 0;

    UpdateWaitTime[UpdateWaitSample] = 0;
    UpdateWaitFrames[UpdateWaitSample] = 0;
}

void TMapPane::FlushUpdate(BOOL updatetimeslice)
{
    int wait = WaitUpdate();    // Wait for previous queue to finish
    if (updatetimeslice)
        AdjustTimeSlice(wait);  // Adjust update wait value by how long we waited here
    if (isrecqueued)
    {
        scrollrect = queuedrect;    // Update current refreshed rect
        isrecqueued = FALSE;        // Set no rect as being queued
    }
    PutQueueRectsToDisplay();   // Causes display to show new rects
    ClearQueueRects();          // Make sure queue is empty
}

void TMapPane::UpdateEdges(int updatex, int updatey)
{
    if (!ThreadRunning)
        return;

    oldscrollx = scrollx;
    oldscrolly = scrolly;

    scrollx    = updatex;
    scrolly    = updatey;

  // Check to see if queued update is finished yet
    if (isrecqueued && UpdateDone())
        FlushUpdate();

  // Hey, do we update everything?
    BOOL redrawall = level != oldlevel || IsDirty();

  // If no scrolling occured, just draw bg rects and return
    if (scrollx == oldscrollx && 
        scrolly == oldscrolly && 
        !redrawall)
    {
        if (numbgrects > 0)
        {
            FlushUpdate();

          // Add new bg rects to fresh queue
            for (int c = 0; c < numbgrects; c++)
            {
                SRect dr;
                if (!ClipRect(scrollrect, bgrects[c].rect, dr))
                    continue;
                QueueUpdateRect(dr, bgrects[c].bgdraw);
            }
            numbgrects = 0;

            BeginUpdate(); // Start updating
            FlushUpdate(); // Flush it right now
        }

        return;
    }

  // Screen is always within this rect
    SRect innerscrollrect = SRect(scrollx, scrolly, 
        scrollx + SCROLLBUFWIDTH - UPDATEWIDTH - 1, 
        scrolly + SCROLLBUFHEIGHT - UPDATEHEIGHT - 1);
    SRect outerscrollrect = innerscrollrect;

  // Redraw the whole scroll buffer
    if (redrawall || 
        (!outerscrollrect.Intersects(scrollrect) &&
         !outerscrollrect.Intersects(queuedrect)))
    {
        CancelUpdate(); // Cancel any current updating and clear queue rects

     // Draw whole screen with one update edge on each side
        innerscrollrect = SRect(scrollx, scrolly,
            scrollx + SCROLLBUFWIDTH - 1, scrolly + SCROLLBUFHEIGHT - 1);
        outerscrollrect = innerscrollrect;

        QueueUpdateRect(outerscrollrect, BGDRAW_UNLIT);

        queuedrect = outerscrollrect;
        isrecqueued = TRUE;

        BeginUpdate();
        FlushUpdate();

        numbgrects = 0;

        return;
    }

  // Redraw just the edges
    else
    {
        FlushUpdate(TRUE);      // TRUE = Update timeslice values

      // Since we're drawing two update edges in front of us, and there are only 2 edges
      // extra in the scroll buffer, make sure we don't draw the right edge when scrolling
      // left, or the left edge when scrolling right, etc. etc.   If we draw all outside
      // edges, the drawing will wrap around.

      // Figure out our movment vector (What's our vector Victor,.  rodger Rodger.. etc. etc.)

        if (scrollx > oldscrollx)       // Moving right
        {
          // We've drawing two edges in front of us to the right, there are no edges to the left
            outerscrollrect.right += UPDATEWIDTH;
        }
        else if (scrollx < oldscrollx)  // Moving left
        {
          // We're drawing two edges in front of us to the left, there are no edges to the right
            outerscrollrect.left -= UPDATEWIDTH;
        }
        else
        {
            outerscrollrect.left = scrollrect.left;
            outerscrollrect.right = scrollrect.right;
        }

        if (scrolly > oldscrolly)       // Moving down
        {
          // We've drawing two edges in front of us to the bottom, there are no edges to the top
            outerscrollrect.bottom += UPDATEHEIGHT;
        }
        else if (scrolly < oldscrolly)  // Moving up
        {
          // We've drawing two edges in front of us to the top, there are no edges to the bottom
            outerscrollrect.top -= UPDATEHEIGHT;
        }
        else
        {
          // Just keep the edges we already have
            outerscrollrect.top = scrollrect.top;
            outerscrollrect.bottom = scrollrect.bottom;
        }

      // Subtract the current valid area from new area and draws invalid edges
        SRect edgerects[4];
        int numedgerects;
        if (SubtractRect(outerscrollrect, scrollrect, edgerects, numedgerects))
        {
            for (int c = 0; c < numedgerects; c++)
                QueueUpdateRect(edgerects[c], BGDRAW_UNLIT);
        }

      // Get part of valid scroll area that is within the new area
        ClipRect(scrollrect, outerscrollrect, scrollrect);

      // Cause update system to update background rects
        for (int c = 0; c < numbgrects; c++)
        {
            SRect r = bgrects[c].rect;

          // Clip to not intersect edge rects which we're already drawing
            if (!ClipRect(r, scrollrect, r))
                continue;

            QueueUpdateRect(r, bgrects[c].bgdraw);
        }

        queuedrect = outerscrollrect;
        isrecqueued = TRUE;

        BeginUpdate();

      // If inner scrolling rect not in current valid scroll rect, or bgrects, wait for update  
        if (numbgrects > 0 || !innerscrollrect.In(scrollrect))
            FlushUpdate(TRUE);      // TRUE = Adjust time slice

        numbgrects = 0;
    }

}

void TMapPane::BeginUpdate()
{
    if (!ThreadRunning)
        return;

    BEGIN_CRITICAL();

    Updating = TRUE;
    UpdateCancelled = FALSE;
    SetEvent(UpdateStartEvent);
    
    END_CRITICAL();
}

BOOL TMapPane::UpdateDone()
{
    return !Updating;
}

void TMapPane::CancelUpdate()
{
    if (!ThreadRunning)
        return;

    if (!Updating)
    {
        UpdateCancelled = FALSE;
        return;
    }

    BEGIN_CRITICAL();

    Updating = FALSE;
    UpdateCancelled = TRUE;

    END_CRITICAL();

    FlushUpdate();
}

DWORD TMapPane::WaitUpdate()
{
    if (!ThreadRunning)
        return 0;

    if (!Updating)
    {
        UpdateCancelled = FALSE;
        return 0;
    }

    // Get start time in milliseconds
    DWORD start = GetTickCount();

  // Release all mutexes just in case we're still blocking the loader
    ClearMutex(PauseUpdateMutex);
    ClearMutex(SectorMutex);
    ClearMutex(ObjectMutex);

    while (Updating)
    {
        SetEvent(UpdateStartEvent);
        WaitForSingleObject(UpdateDoneEvent, INFINITE);
        ResetEvent(UpdateDoneEvent);
    }

    UpdateCancelled = FALSE;

    // Get end time in milliseconds
    DWORD end = GetTickCount();

    return end - start; // Return elapsed time for wait
}

void TMapPane::ClearQueueRects()
{
    if (!ThreadRunning)
        return;

    if (Updating)
        WaitUpdate();   // Wait for update to finish before clearing queue

    BEGIN_CRITICAL();

    numqueuerects = 0;

    END_CRITICAL();
}

void TMapPane::QueueUpdateRect(RSRect rect, int bgdraw)
{
    if (!ThreadRunning)
        return;

    if (Updating)
        FlushUpdate();

    if (numqueuerects >= MAXQUEUERECTS)  // This should never happen
        return;

//  SRect dr;
//  if (!ClipRect(scrollrect, rect, dr))
//      return;

    BEGIN_CRITICAL();

    queuerects[numqueuerects].rect = rect; //dr;
    queuerects[numqueuerects].bgdraw = bgdraw;
    numqueuerects++;
    
    END_CRITICAL();
}

void TMapPane::PutQueueRectsToDisplay()
{
    if (!ThreadRunning)
        return;

    if (Updating)
        WaitUpdate();  // Wait for update to finish before adding display rects

    for (int c = 0; c < numqueuerects; c++)
    {
        Display->AddBackgroundUpdateRect(       
            GetBackgroundBuffer(),  
            queuerects[c].rect.x(), queuerects[c].rect.y(), queuerects[c].rect.w(), queuerects[c].rect.h(),
            UPDATE_BUFFERTOSCREEN | UPDATE_NEXTFRAME);
    }
}

unsigned _stdcall TMapPane::UpdateThread(void *)
{
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_BELOW_NORMAL);

    MapPane.UpdateLoop();

  // Kill the events
    ClearMutex(PauseUpdateMutex); // Release if I own it just in case
    ClearMutex(SectorMutex);
    ClearMutex(ObjectMutex);

    CloseHandle(UpdateStartEvent);
    CloseHandle(UpdateDoneEvent);
    CloseHandle(PauseUpdateMutex);
    CloseHandle(SectorMutex);
    CloseHandle(ObjectMutex);

    _endthreadex( 0 );

    return 0;
}

void TMapPane::UpdateLoop()
{
    HANDLE pause[2];
    pause[0] = PauseUpdateMutex; 
    pause[1] = SectorMutex;

    for (;;)
    {
        if (!Updating)
            WaitForSingleObject(UpdateStartEvent, INFINITE);

        ResetEvent(UpdateStartEvent);

        if (QuitThread)
            break;

        if (!Updating)
        {
            ClearMutex(PauseUpdateMutex);
            ClearMutex(SectorMutex);
            ClearMutex(ObjectMutex);
            SetEvent(UpdateDoneEvent);
            continue;
        }

        BEGIN_CRITICAL();
        BOOL dodraw = numqueuerects > 0;
        END_CRITICAL();     

      // Draw update rect list  
        if (dodraw)
        {
            for (int c = 0; ; c++)
            {
                BEGIN_CRITICAL();

                SRect r;
                int bgdraw;
                BOOL done;

                if (c < numqueuerects)
                {
                    r = queuerects[c].rect;
                    bgdraw = queuerects[c].bgdraw;
                    done = FALSE;
                }
                else
                    done = TRUE;
                
                END_CRITICAL();

                if (done == TRUE || UpdateCancelled)
                    break;

                WaitForMultipleObjects(2, pause, TRUE, INFINITE);

              // Draw the update region 
                DrawUpdateRect(r, bgdraw);
                
                ReleaseMutex(PauseUpdateMutex);
                ReleaseMutex(SectorMutex);
                ReleaseMutex(ObjectMutex);

                ClearMutex(PauseUpdateMutex);
                ClearMutex(SectorMutex);
                ClearMutex(ObjectMutex);
            }
        }
        
        BEGIN_CRITICAL();

        if (UpdateCancelled)   // If cancelled, values were reset in cancel, don't change
        {                      // Now because we could have more new rects queued already
            UpdateCancelled = FALSE;
        }
        else                   // If not cancelled, reset all values here
        {
            Updating = FALSE;
            UpdateCancelled = FALSE;
            SetEvent(UpdateDoneEvent);
        }

        END_CRITICAL();

        if (QuitThread)
            break;
    }

}

void TMapPane::PauseUpdate(char *file, int line)
{
    if (!ThreadRunning)
        return;

    WaitSingleErr(PauseUpdateMutex);

    PauseMutexFile = file;
    PauseMutexLine = line;
}

void TMapPane::ResumeUpdate()
{
    if (!ThreadRunning)
        return;

    ReleaseMutex(PauseUpdateMutex);
    ClearMutex(PauseUpdateMutex);

    PauseMutexFile = NULL;
    PauseMutexLine = 0;

}   

void TMapPane::LockSectors(char *file, int line)
{
    if (!ThreadRunning)
        return;

    WaitSingleErr(SectorMutex);

    SectorMutexFile = file;
    SectorMutexLine = line;
}

void TMapPane::UnlockSectors()
{
    if (!ThreadRunning)
        return;

    ReleaseMutex(SectorMutex);
    ClearMutex(SectorMutex);

    SectorMutexFile = NULL;
    SectorMutexLine = 0;
}

void TMapPane::LockObjects(char *file, int line)
{
    if (!ThreadRunning)
        return;

    WaitSingleErr(ObjectMutex);

    ObjectMutexFile = file;
    ObjectMutexLine = line;
}

void TMapPane::UnlockObjects()
{
    if (!ThreadRunning)
        return;

    ReleaseMutex(ObjectMutex);
    ClearMutex(ObjectMutex);

    ObjectMutexFile = NULL;
    ObjectMutexLine = 0;
}

void TMapPane::DrawUpdateRect(RSRect r, int bgdraw)
{
    bgdraw = bgdraw & (~(DWORD)BGDRAW_REDRAW);

    GetUpdateObjs(r); // Get list of objects to draw

    SRect dr = r;

  // Leave origin at 0,0 (for wrap clipping) and set clip rect to current rect)
    unlitmulti->SetClipRect(dr.x(), dr.y(), dr.w(), dr.h());
    unlitmulti->SetClipMode(CLIP_WRAP);

  // Leave origin at 0,0 (for wrap clipping) and set clip rect to current rect)
    litmulti->SetClipRect(dr.x(), dr.y(), dr.w(), dr.h());
    litmulti->SetClipMode(CLIP_WRAP);

  // Draw the tile and lighting pipeline - unlit first, then lights, then lit
    if (bgdraw <= BGDRAW_UNLIT)
        DrawUnlitObjects(dr);
    if (QuitThread || UpdateCancelled)
        return;

    if (bgdraw <= BGDRAW_LIGHTS)
    {
        if (bgdraw > BGDRAW_UNLIT) // Clear lighting values
            DrawAmbientLight(unlitmulti, dr);
        DrawStaticLights(dr);
    }
    if (QuitThread || UpdateCancelled)
        return;

    if (bgdraw <= BGDRAW_AMBIENT)
        TransferUnlitToLit(dr);
    if (QuitThread || UpdateCancelled)
        return;

    if (bgdraw <= BGDRAW_LIT)
        DrawLitObjects(dr);
    if (QuitThread || UpdateCancelled)
        return;

    if (Editor)
        DrawSelectedObjects(dr);

    if (QuitThread || UpdateCancelled)
        return;
}

void TMapPane::GetUpdateObjs(SRect &r)
{
    UpdateObjs.Clear();

    for (TMapIterator i(&r, CHECK_RECT|CHECK_LIGHT|CHECK_MOVING|CHECK_INVIS|CHECK_NOINVENT); i; i++)
    {
        UpdateObjs.Add(i);

    // Check to see if update was canceled
        if (UpdateCancelled)
            return;
    }
}

void TMapPane::DrawUnlitObjects(SRect &r)
{
    if (ClearBeforeDraw || level != 0)
        unlitmulti->Box(r.x(), r.y(), r.w(), r.h(), 0, 0xFFFF, 0,
            DM_WRAPCLIP | DM_ZBUFFER | DM_NORMALS);
    else
        unlitmulti->Box(r.x(), r.y(), r.w(), r.h(), 0, 0xFFFF, 0,
            DM_WRAPCLIP | DM_ZBUFFER | DM_NORMALS | DM_NODRAW);

    for (TObjectIterator i(&UpdateObjs); i; i++)
    {
        WaitSingleErr(ObjectMutex);
        i.Item()->DrawUnlit(unlitmulti);
        ReleaseMutex(ObjectMutex);

    // Check to see if update was canceled
        if (UpdateCancelled)
            return;

    }
}

void TMapPane::DrawStaticLights(SRect &r)
{
    for (TObjectIterator i(&UpdateObjs); i; i++)
    {
        WaitSingleErr(ObjectMutex);
        i.Item()->DrawLight(unlitmulti, IsDirty());
        ReleaseMutex(ObjectMutex);

    // Check to see if update was canceled
        if (UpdateCancelled)
            return;
    }
}

void TMapPane::TransferUnlitToLit(SRect &r)
{
  // Transfer graphic/lighting data to lit buffer
    TransferAndLight32to16(litmulti, unlitmulti, r);

  // Transfer zbuffer data to lit buffer
    if (!zbufferiscloned)
        litmulti->Blit(r.x(), r.y(), unlitmulti, r.x(), r.y(), r.w(), r.h(), DM_NODRAW | DM_ZBUFFER | DM_WRAPCLIP);
}

void TMapPane::DrawLitObjects(SRect &r)
{
    if (Editor && StatusBar.EditWalkmap())
        DrawWalkMap(r);

    for (TObjectIterator i(&UpdateObjs); i; i++)
    {
        if (i.Item()->GetFlags() & OF_EDITOR && !Editor)
            continue;

        WaitSingleErr(ObjectMutex);
        i.Item()->DrawLit(litmulti);
        ReleaseMutex(ObjectMutex);

    // Check to see if update was canceled
        if (UpdateCancelled)
            return;
    }

    // debug stuff
//  if (ShowZBuffer)
//      litmulti->Blit(0, 0, litmulti->GetZBuffer()); 
//  if (ShowNormalBuffer)
//      litmulti->Blit(0, 0, litmulti->GetNormalBuffer()); 
}

void TMapPane::DrawWalkMap(SRect &r)
{
    SRect sr;
    int sx, sy;

    // draw walkmap
    for (sx = 0; sx < SECTORWINDOWX; sx++)
    {
        for (sy = 0; sy < SECTORWINDOWY; sy++)
        {
            if (sectors[sx][sy])
            {
                sectors[sx][sy]->GetMaxScreenRect(sr);
                if (!r.Intersects(sr))
                    continue;

                for (int y = wmrevealy; y < (SECTORHEIGHT >> WALKMAPSHIFT) && y < wmrevealsizey; y++)
                    for (int x = wmrevealx; x < (SECTORWIDTH >> WALKMAPSHIFT) && x < wmrevealsizex; x++)
                    {
//                      if (x == objx && y == objy &&
//                          (sectorx+sx) == grabx && (sectory+sy) == graby)
//                          continue;           // dragging it

                        S3DPoint pos, screenpos;

                        pos.x = x;
                        pos.y = y;
                        pos.z = sectors[sx][sy]->ReturnWalkmap(pos.x, pos.y);

                        pos.x = ((sectorx + sx) << SECTORWSHIFT) + (x << WALKMAPSHIFT);
                        pos.y = ((sectory + sy) << SECTORHSHIFT) + (y << WALKMAPSHIFT);
                        WorldToScreen(pos, screenpos);
                        screenpos.y += 8;
                        screenpos.z -= GRIDZOFF;

                        SColor color;
                        color.red = max(30, 255 - 4*absval(pos.z - 20));
                        color.green = max(30, 255 - 4*absval(pos.z - 100));
                        color.blue = max(30, 255 - 4*absval(pos.z - 180));
                        if (pos.z > 0)
                            litmulti->ZPut(screenpos.x, screenpos.y, screenpos.z, EditorData->Bitmap("grid"), DM_TRANSPARENT | DM_WRAPCLIP | DM_ZBUFFER | DM_USEREG | DM_BACKGROUND);
                        litmulti->Put(screenpos.x, screenpos.y, EditorData->Bitmap("gridout"), DM_TRANSPARENT | DM_WRAPCLIP | DM_USEREG | DM_BACKGROUND | DM_CHANGECOLOR, &color);
                    }
            }
        }
    }
}

void TMapPane::DrawSelectedObjects(SRect &r)
{
    SRect sr;
    SColor color;
    color.red = 220; color.green = 20; color.blue = 50;

    for (int i = StatusBar.GetFirstObj(); i >= 0; i = StatusBar.GetNextObj())
    {
        PTObjectInstance inst = GetInstance(i);
        if (!inst || inst->IsInInventory() || inst->GetFlags() & OF_SELDRAW)
            continue;

      // Check to see if update was canceled
        if (UpdateCancelled)
            return;

        inst->DrawSelected(litmulti);
    }
}

// ***************************************************************************************
// ******************************  End Of UPDATE SYSTEM  *********************************
// ***************************************************************************************

void TMapPane::AnimateSelectedObjects()
{
    SetClipRect();

    if (StatusBar.EditWalkmap())
    {
        if (objx >= 0 && objy >= 0)
        {
            S3DPoint pos, screenpos;
            pos.x = objx;
            pos.y = objy;
            pos.z = sectors[grabx - sectorx][graby - sectory]->ReturnWalkmap(pos.x, pos.y);
            pos.x = (grabx << SECTORWSHIFT) + (objx << WALKMAPSHIFT);
            pos.y = (graby << SECTORHSHIFT) + (objy << WALKMAPSHIFT);
            WorldToScreen(pos, screenpos);
            screenpos.x += MAPPANEX - PosX();
            screenpos.y += MAPPANEY - PosY();
            screenpos.y += 8;
            screenpos.z -= GRIDZOFF;

            SColor color;
            color.red = max(30, 255 - 4*absval(pos.z - 20));
            color.green = max(30, 255 - 4*absval(pos.z - 100));
            color.blue = max(30, 255 - 4*absval(pos.z - 180));
            Display->ZPut(screenpos.x, screenpos.y, screenpos.z, EditorData->Bitmap("grid"), DM_TRANSPARENT | DM_WRAPCLIP | DM_ZBUFFER | DM_USEREG);
            Display->Put(screenpos.x, screenpos.y, EditorData->Bitmap("gridout"), DM_TRANSPARENT | DM_WRAPCLIP | DM_USEREG | DM_CHANGECOLOR, &color);
        }
    }

    for (int i = StatusBar.GetFirstObj(); i >= 0; i = StatusBar.GetNextObj())
    {
        PTObjectInstance inst = GetInstance(i);
        if (!inst || inst->IsInInventory() || !(inst->GetFlags() & OF_SELDRAW))
            continue;

        inst->DrawSelected(Display);

        if (inst->GetShadow() >= 0)
        {
            PTObjectInstance s = GetInstance(inst->GetShadow());
            if (!s || s->IsInInventory() || !(s->GetFlags() & OF_SELDRAW))
                continue;

            s->DrawSelected(Display);
        }
    }

    Display->ResetClipRect();
}

void TMapPane::NextFrameObjects()
{
    if (NoAnimateObjs)
        return;

    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_ANIMATE); i; i++)
    {
        PTObjectInstance inst = i.Item();
        inst->NextFrame();
    }
}

void TMapPane::PulseObjects()
{
    if (NoPulseObjs)
        return;

    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_PULSE); i; i++)
    {
        if (i->GetFlags() & OF_KILL)
            i.Nuke();
        else
            i->Pulse();
    }
}

void TMapPane::MoveObjects()
{
    if (NoPulseObjs)
        return;

  // Move objects based on previous frame movement, and results of Pulse()
    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_MOVING); i; i++)
    {
        i->SetMoveBits(i->Move());  // Call to move object and get blocked/hit bits
    }

  // Set next movement for next frame based on current state/frame
    for (TMapIterator j(NULL, CHECK_NOINVENT, OBJSET_MOVING); j; j++)
    {
        j->SetObjectMotion();       // Call to get imagery/object's next position
        // NOTE: after SetObjectMotion() is called, any calls to SetNextMove() or
        // SetMoveAngle() will OVERIDE the movement in the animation!
    }
}

void TMapPane::AnimateObjects(BOOL draw)
{
    if (NoAnimateObjs)
        return;

    SRect r; // Get area of screen where we should turn on animators
    r.top    = scrolly - 128;
    r.left   = scrollx - 64;
    r.bottom = scrolly + GetHeight() + 128 - 1;
    r.right  = scrollx + GetWidth() + 64 - 1;

    Display->SetClipRect(MAPPANEX, MAPPANEY, GetWidth(), GetHeight());

    for (TMapIterator i(&r, CHECK_SECTRECT|CHECK_INVIS|CHECK_NOINVENT, OBJSET_ANIMATE); i; i++)
    {
        PTObjectInstance inst = i.Item();

        SRect ir, ar;
        inst->GetScreenRect(ir);
        inst->GetAnimRect(ar);

        BOOL intersects = r.Intersects(ir) || r.Intersects(ar);

        if (intersects && !inst->HasAnimator())
            inst->OnScreen();

        else if (!intersects)
            inst->OffScreen();

        inst->Animate(draw);
    }

    Display->ResetClipRect();
}

// *******************************
// * Sector Management Functions *
// *******************************

void TMapPane::SaveAllSectors()
{
    int sx, sy;

    LOCKSECTORS;        // Prevent update thread from accessing sectors while we change them
                        // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                        //  BETWEEN THESE TWO FUNCTIONS!!)

    for (sx = 0; sx < SECTORWINDOWX; sx++)
        for (sy = 0; sy < SECTORWINDOWY; sy++)
            if (sectors[sx][sy])
                sectors[sx][sy]->Save();

    UNLOCKSECTORS;       // Allow update system to access sector arrays again
                         // If lock is called without unlock, system will CRASH!!
}

void TMapPane::FreeAllSectors()
{
    int sx, sy;

    LOCKSECTORS;        // Prevent update thread from accessing sectors while we change them
                        // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                        //  BETWEEN THESE TWO FUNCTIONS!!)

    // NOTE: We don't need to call the LockSectors() function here because the
    // update system is turned off by the time we get here

    for (sx = 0; sx < SECTORWINDOWX; sx++)
        for (sy = 0; sy < SECTORWINDOWY; sy++)
            if (sectors[sx][sy])
            {
                DeleteSector(sectors[sx][sy]);
                sectors[sx][sy] = NULL;
            }

    UNLOCKSECTORS;       // Allow update system to access sector arrays again
                         // If lock is called without unlock, system will CRASH!!
}

void TMapPane::ReloadSectors()
{
    FreeAllSectors();
    sectorx += 10000000;
    sectory += 10000000;
    RedrawAll();
}

void TMapPane::UpdateSectors()
{
    int x, y;

  // Change sector position
    oldlevel = level;
    level = newlevel;
    oldsectorx = sectorx;
    oldsectory = sectory;

  // account for the character actually being at (sectorx+1, sectory+1)
    sectorx = (center.x >> SECTORWSHIFT) - 1;
    sectory = (center.y >> SECTORHSHIFT) - 1;

  // If sector has changed.. reload sectors
    if (sectorx != oldsectorx || sectory != oldsectory || level != oldlevel || IsDirty())
    {
        LOCKSECTORS;        // Prevent update thread from accessing sectors while we change them
                            // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                            //  BETWEEN THESE TWO FUNCTIONS!!)

      // Temporary sectors
        PTSector newsectors[SECTORWINDOWX][SECTORWINDOWY];
        memset(newsectors, 0, sizeof(PTSector) * SECTORWINDOWX * SECTORWINDOWY);

      // Delete old sectors
        int nx, ny;
        for (x = 0; x < SECTORWINDOWX; x++)
        {
            for (y = 0; y < SECTORWINDOWY; y++)
            {
                // Offset by one, because sectorx refers to middle sector
                nx = oldsectorx - sectorx + x;
                ny = oldsectory - sectory + y;

                // Old sector is out of sector window.. delete it
                if (level != oldlevel || (DWORD)nx >= SECTORWINDOWX || (DWORD)ny >= SECTORWINDOWY)
                {
                    if (sectors[x][y])
                        DeleteSector(sectors[x][y]);
                    sectors[x][y] = NULL;
                }
                else // Sector still there.. put it in a new position
                    newsectors[nx][ny] = sectors[x][y];
            }
        }

      // Copy new sector list to sector list
        memcpy(sectors, newsectors, sizeof(PTSector) * SECTORWINDOWX * SECTORWINDOWY);

      // Preload sectors if level changed
        if (!PreloadSectors)
            TSector::ClearPreloadSectors();
        else if (!TSector::InPreloadArea(center, level)) // Reload cache if we're not in cache rect
        {

          // Get new sector rectangle area
            SRect r;
            r.left = center.x - SECTORWIDTH * PreloadSectorSize / 2;
            r.right = center.x + SECTORWIDTH * PreloadSectorSize / 2;
            r.top = center.y - SECTORHEIGHT * PreloadSectorSize / 2;
            r.bottom = center.y + SECTORHEIGHT * PreloadSectorSize / 2;

            if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
            {
                TextBar.Print("Loading Map... Please Wait");
                TextBar.Draw();
                TextBar.PutToScreen();
            }

          // Now reload cache around current pos
            TSector::LoadPreloadSectors(level, 1, &r); // Don't care if this works or not

            if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
            {
                TextBar.Print("");
                TextBar.Draw();
                TextBar.PutToScreen();
            }
        }

      // Load new sectors if needed
        BOOL loaded = FALSE;

        for (x = 0; x < SECTORWINDOWX; x++)
        {
            for (y = 0; y < SECTORWINDOWY; y++)
            {
                if (!sectors[x][y])
                {
                    if ((DWORD)(sectorx + x) < MAXSECTORX && (DWORD)(sectory + y) < MAXSECTORY)
                    {
                        sectors[x][y] = TSector::LoadSector(level, sectorx+x, sectory+y);
                        loaded = TRUE;
                    }
                }
            }
        }

        if (loaded)
            TransferAllWalkmaps();

        // Make sure all selected objects are still valid
        if (Editor)
            StatusBar.Validate();

        UNLOCKSECTORS;       // Allow update system to access sector arrays again
                             // If lock is called without unlock, system will CRASH!!


      // Now that sectors have changed, attempt to readd player characters to map
      // if they aren't in it yet.
      //
      // Since player characters are OF_NONMAP. They aren't saved or deleted by the
      // sector system.  When the map changes, we simply go through the list of characters.
      // and add them into the current map if they aren't in there already.

        for (int player = 0; player < PlayerManager.NumPlayers(); player++)
        {
            PTPlayer p = PlayerManager.GetPlayer(player);
            if (p && !p->GetSector())
                AddObject(p); // Attempt to add player to current sector area
        }

    }
}

/*void TMapPane::GetPlayerFocus(RS3DPoint pos)
{
    if (!Player)
        return;

    if (ScrollLock)
        Player->GetPos(pos);
    else
    {
        // find the vector of facing (where's he's currently focusing his eyes)
        ConvertToVector(((PTCharacter)Player)->GetFacingTarget(), 96, pos);

        S3DPoint curpos;        // add in current position
        Player->GetPos(curpos);
        pos += curpos;
    }

    pos.x -= 42;            // account for character height on screen
    pos.y -= 42;
}
*/

// ***************************
// * Main Animation Function *
// ***************************

void TMapPane::Animate(BOOL draw)
{
    // Draw dynamic light
    if (!Editor && Player)
    {
        S3DPoint pos;
        Player->GetPos(pos);
        static int brighttick;
        brighttick++;
        S3DPoint lpos = pos;
        lpos.x += (int)(10.0 * sin((double)brighttick / 7.0));
        lpos.y += (int)(10.0 * cos((double)brighttick / 7.0));
        lpos.z += 100 + (int)(20.0 * sin((double)brighttick / 12.0));
        MapPane.SetDLightPos(lpos);

        if (draw)
        {
            SetClipRect();
            DrawDLight();
        }
    }

  // Update zbuffers before animation begins
    if (draw)
    {
        SetClipRect();
        Scene3D.RefreshZBuffer();
    }

  // Draw objects being dragged around
    if (Editor && draw && !mx && !my)
        AnimateSelectedObjects();

  // Draw object animations
    AnimateObjects(draw);

    // Draw 3D scene stuff  
    SetClipRect();
    if (draw)
        Scene3D.DrawScene();

  // Update mouse cursor
    if (draw)
    {
        int x = cursorx - GetPosX();
        int y = cursory - GetPosY();

        if (InPane(x, y))
        {
            int type = CURSOR_NONE;

            if (!Editor)
            {
                PTObjectInstance inst = OnObject(x, y);
                if (inst)
                    if (GetDragObj() == NULL && inst->IsInventoryItem())
                        type = CURSOR_HAND;         // can pick up while in the map pane
                    else
                        type = inst->CursorType(GetDragObj());
            }

            SetMouseCornerBitmap(type);
        }
    }

  // Release time slice to update thread if necessary
    UpdateTimeSlice(draw);  
}

// ***********************
// * Object Manipulation *
// ***********************

BOOL TMapPane::MoneyHandler(PTObjectInstance oi, int amount)
{
    if (oi == NULL)
        return 0;

    BOOL subtract = (amount > 0);

    int total = 0;

    for (TInventoryIterator i(oi); i; i++)
    {
        if (i->ObjClass() == OBJCLASS_MONEY)
        {
            int val = i->Amount() * i->Value();

            if (subtract)
            {
                if (amount < val)
                    i->SetAmount((val - amount) / i->Value());
                else    // extract the object, it's used up
                    DeleteObject( i);
            }

            total += val;
        }

        if (subtract && total >= amount)
            break;
    }

    for (TInventoryIterator j(oi); j; j++)
    {
        if (subtract && total >= amount)
            break;

        total += MoneyHandler(j, subtract ? amount - total : 0);
    }

    return total;
}

int TMapPane::SubtractMoney(PTObjectInstance oi, int amount)
{
    if (amount < 1 || GetTotalMoney(oi) < amount)
        return 0;

    return MoneyHandler(oi, amount);
}

int TMapPane::GetTotalMoney(PTObjectInstance oi)
{
    return MoneyHandler(oi, 0);
}

// Checks new position and transfers object between sectors if object crosses a sector
// boundry.  Also prevents objects from going outside of loaded sector list
int TMapPane::CheckPos(PTObjectInstance inst, RS3DPoint newpos, int newlevel)
{
  // If default, or object is owned by map, set level to object level
  // Note: only floating NONMAP objects like TPlayer objects can change their level
    if (newlevel == -1 || !(inst->Flags() & OF_NONMAP))
        newlevel = inst->GetLevel();

    // basic bounds checking
    if (newpos.x < 0)
        newpos.x = 0;
    else if (newpos.x >= MAXMAPWIDTH)
        newpos.x = MAXMAPWIDTH;

    if (newpos.y < 0)
        newpos.y = 0;
    else if (newpos.y >= MAXMAPHEIGHT)
        newpos.y = MAXMAPHEIGHT;

    int newsx = (newpos.x >> SECTORWSHIFT) - sectorx;
    int newsy = (newpos.y >> SECTORHSHIFT) - sectory;


    // Note: Since non map objects are usually characters, it's ok to move them outside of
    if (inst->Flags() & OF_NONMAP)  // Non map can be outside of current sector list (it's OK)
    {

    // Non map objects (usually characters), can be teleported outside of the current map.
    // When this happens, they are removed from the current sector list until the DrawBackground()
    // function reloads a new sector list which contains them again.  Objects outside of sectors
    // will never get Pulse or Animate calls (to make sure they don't somehow hose the system).

        if (newsx < 0 || newsx >= SECTORWINDOWX || 
            newsy < 0 || newsy >= SECTORWINDOWY || 
            newlevel != GetMapLevel())
        {
            RemoveObject(inst);
            return 0;
        }
    }
    else
    {

    // For normal objects, make sure new sector is within loaded sectors - 
    // if not leave it bumping up against the sector boundry

        if (newsx < 0)
        {
            newsx = 0;
            newpos.x = max(sectorx, 0) << SECTORWSHIFT;
        }
        else if (newsx >= SECTORWINDOWX)
        {
            newsx = SECTORWINDOWX - 1;
            newpos.x = ((sectorx + newsx) << SECTORWSHIFT) + SECTORWIDTH - 1;
        }

        if (newsy < 0)
        {
            newsy = 0;
            newpos.y = max(sectory, 0) << SECTORHSHIFT;
        }
        else if (newsy >= SECTORWINDOWY)
        {
            newsy = SECTORWINDOWY - 1;
            newpos.y = ((sectory + newsy) << SECTORHSHIFT) + SECTORHEIGHT - 1;
        }
    }

    // Check to see if it changed sectors while moving
    if (!inst->IsInInventory())
    {
        S3DPoint pos;
        inst->GetPos(pos);
        int sx = (pos.x >> SECTORWSHIFT) - sectorx;
        int sy = (pos.y >> SECTORHSHIFT) - sectory;

      // Position is OUT OF RANGE!!!!
        if ((DWORD)sx >= SECTORWINDOWX || (DWORD)sy >= SECTORWINDOWY)
        {
            pos.x = (inst->GetSector()->SectorX() << SECTORWSHIFT) + (SECTORWIDTH >> 1);
            pos.y = (inst->GetSector()->SectorY() << SECTORHSHIFT) + (SECTORHEIGHT >> 1);
            pos.z = 16;
            inst->ForcePos(pos);
            sx = (pos.x >> SECTORWSHIFT) - sectorx;
            sy = (pos.y >> SECTORHSHIFT) - sectory;
        }

        if (newsx != sx || newsy != sy)
            return TransferObject(inst, sx, sy, newsx, newsy);
    }

    return inst->GetMapIndex();
}

// Moves an object from one sector to another
int TMapPane::TransferObject(PTObjectInstance inst, int sx, int sy, int newsx, int newsy)
{
    if (!inst || (DWORD)sx >= SECTORWINDOWX || (DWORD)sy >= SECTORWINDOWY)
        return -1;

    TMapIterator i;

    for ( ; i; i++)
        if (i == inst)
            break;

    if (!i)
        return -1;

    LOCKSECTORS;      // Prevent update system from accessing sectors

    sectors[sx][sy]->RemoveObject(i.SectorIndex());
    sectors[newsx][newsy]->AddObject(inst);

    UNLOCKSECTORS;    // Allow update system to access sectors again

    return inst->GetMapIndex();
}

// Walkmap auto-generator for current sector
void TMapPane::CalculateWalkmap()
{
    PTSector sector = sectors[1][1];
    if (!sector)
        return;

    for (int y = 0; y < (SECTORWIDTH >> WALKMAPSHIFT); y++)
        for (int x = 0; x < (SECTORHEIGHT >> WALKMAPSHIFT); x++)
        {
            S3DPoint pos, screenpos;

            pos.x = ((sectorx+1) << SECTORWSHIFT) + (x << WALKMAPSHIFT);
            pos.y = ((sectory+1) << SECTORHSHIFT) + (y << WALKMAPSHIFT);
            pos.z = 0;
            WorldToScreen(pos, screenpos);

            if (screenpos.x < posx || screenpos.x >= (posx+GetWidth()) ||
                screenpos.y < posy || screenpos.y >= (posy+GetHeight()))
                continue;

            for (pos.z = 255; pos.z > 1; pos.z--)
            {
                pos.x = ((sectorx+1) << SECTORWSHIFT) + (x << WALKMAPSHIFT);
                pos.y = ((sectory+1) << SECTORHSHIFT) + (y << WALKMAPSHIFT);
                WorldToScreen(pos, screenpos);

                if (screenpos.x < posx || screenpos.x >= (posx+GetWidth()) ||
                    screenpos.y < posy || screenpos.y >= (posy+GetHeight()))
                    continue;

                updatemulti->SetClipRect(screenpos.x, screenpos.y, 1, 1);
                screenpos.y += 8;
                screenpos.z -= GRIDZOFF;

                if (!updatemulti->ZFind(screenpos.x, screenpos.y, screenpos.z, EditorData->Bitmap("grid"), DM_USEREG | DM_ZBUFFER))
                {
                    pos.x = x;
                    pos.y = y;
                    sector->SetWalkmap(pos.x, pos.y, pos.z);
                    break;
                }
            }
        }

    MapPane.RedrawAll();
}

void TMapPane::AdjustWalkmap(int deltaz, BOOL absolute, BOOL nonzero)
{
    PTSector sect = sectors[1][1];
    if (!sect || !deltaz)
        return;

    S3DPoint pos;
    for (pos.y = min(wmrevealsizey, (SECTORHEIGHT >> WALKMAPSHIFT)) - 1; pos.y >= wmrevealy; pos.y--)
        for (pos.x = min(wmrevealsizex, (SECTORWIDTH >> WALKMAPSHIFT)) - 1; pos.x >= wmrevealx; pos.x--)
        {
            pos.z = sect->ReturnWalkmap(pos.x, pos.y);
            if (nonzero && pos.z == 0)
                continue;

            if (absolute)
                pos.z = 0;

            pos.z = min(255, max(0, pos.z + deltaz));
            sect->SetWalkmap(pos.x, pos.y, pos.z);
        }

    RedrawAll();
}


char *ModeCursorName[NUMMODES] = { "crosshar", "stamp", "marrow" };

// Handler for special editor modes
void TMapPane::SetMode(void (*tfunc)(S3DPoint), void (*afunc)(), int newmode)
{
    if (newmode >= NUMMODES || newmode < 0)
        return;

    targetcallback = tfunc;
    abortcallback = afunc;
    mode = newmode;

    if (mode == MODE_CLONE || mode == MODE_MOVE)
    {
        dragmode = TRUE;
        PTObjectInstance inst = GetInstance(StatusBar.GetSelectedObj());
        if (inst)
        {
            SRect r;
            inst->GetScreenRect(r);
            objx = 0;
            objy = 0; //-((r.bottom - r.top + 1) / 2);
        }
    }

    SetMouseBitmap(EditorData->Bitmap(ModeCursorName[mode]));

    //PlayScreen.SetExclusivePane(&MapPane);
}

// Notify objects of changes
void TMapPane::Notify(DWORD notify, void *ptr)
{
  // If nothing has requested notify, just quit 
    if (!(notify & notifyflags))
        return;

    BOOL notifyanything = FALSE;

    // Find objects to notify
    for (TMapIterator i(NULL, CHECK_NONE, OBJSET_NOTIFY); i; i++)
    {
        if (i->GetFlags() & OF_NOTIFY)
        {
            i->Notify(notify, ptr);
            notifyanything = TRUE;
        }
    }

    // Nothing wants to be notified anymore.. so set flags to 0
    if (!notifyanything)
        notifyflags = 0;
}
