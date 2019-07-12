// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     sector.cpp - TSector object                       *
// *************************************************************************

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <math.h>

#include "revenant.h"
#include "bitmap.h"
#include "sector.h"
#include "stream.h"
#include "parse.h"
#include "textbar.h"

char sectorfilename[80] = "%d_%d_%d.DAT";
DWORD SectrorMapFCC = (('M' << 0) | ('A' << 8) | ('P' << 16) | (' ' << 24));
#define STARTSIZE 32768
#define GROWSIZE 16284

// ******************************
// * Constructor and Destructor *
// ******************************

TSector::TSector(int newlevel, int newsectorx, int newsectory)
{
    level = newlevel;
    sectorx = newsectorx;
    sectory = newsectory;
    usecount = 0;
    objects.Clear();
    for (int c = 1; c < NUMOBJSETS; c++)
        objsets[c-1].Clear();

    walkmap = new WORD[WALKMAPSIZE];

    memset(walkmap, 0, sizeof(WORD) * WALKMAPSIZE);

    sprintf(filename, sectorfilename, level, sectorx, sectory);
}

TSector::~TSector()
{
    if (walkmap)
        delete walkmap;

    for (int c = 1; c < NUMOBJSETS; c++)
        objsets[c-1].Clear();

    for (TObjectIterator i(&objects); i; i++)
    {
        PTObjectInstance inst = i.Item();

        if (!inst)
            continue;

        inst->ForceSector(NULL);

        objects.Remove(i);

        if (inst->Flags() & OF_NONMAP) // Don't delete NONMAP objects
            continue;
        else
            delete inst;
    }
}

// ************************
// * Load and Save Sector *
// ************************

// Loads the sector.. keeps file open so sector is locked
PTSector TSector::LoadSector(int newlevel, int newsectorx, int newsectory, BOOL preload)
{
    PTSector sector;

    if (preload)
    {
        sector = FindPreloadSector(newlevel, newsectorx, newsectory);
        if (sector)
        {
            sector->usecount++;
            return sector;
        }
    }

    sector = new TSector(newlevel, newsectorx, newsectory);
    sector->Load(); // Assume this works

    return sector;
}

// Save and delete the sector (doesn't really delete it if sector is preload)
void TSector::CloseSector(PTSector sector)
{
    if (!sector->preloaded)
    {
        sector->Save();
        delete sector;
    }
    else
        sector->usecount--;
}

// Straight (no preloaded sectors) load/save functions

#define MAKEINDEX(level, sx, sy, item)  ((level<<24) | (sx<<18) | (sy<<12) | (item & 0xFFF))

BOOL TSector::Load(BOOL lock)
{
    int version = 0;
    FILE *fp;

    char mappath[MAX_PATH];

    strcpy(mappath, CurMapPath);
    strcat(mappath, CURMAPDIR "\\");
    strcat(mappath, filename);
    fp = popen(mappath, "rb"); // Path open (uses program path)

    if (!fp)
    {
        strcpy(mappath, BaseMapPath);
        strcat(mappath, BASEMAPDIR "\\");
        strcat(mappath, filename);
        fp = popen(mappath, "rb"); // Path open (uses program path)
    
        if (!fp)
            return FALSE;
    }

    fseek(fp, 0, SEEK_SET);

    int bufsize = _filelength(fileno(fp));
    BYTE *buf;

    if (bufsize > 0)
    {
        buf = (BYTE *)malloc(bufsize);
        fread(buf, bufsize, 1, fp);
    }
    else
    {
        buf = (BYTE *)malloc(sizeof(int));
        bufsize = 4;
        *((int *)buf) = 0;
    }

    //if (!lock)
    {
        fclose(fp);
        fp = NULL;
    }

    TInputStream is(buf, bufsize);

    int numobjects;
    is >> numobjects;

    // See if this is a sector map with header information
    if ((DWORD)numobjects == SectrorMapFCC)
    {
        // Get which sector map version this is
        is >> version;
        is >> numobjects;
    }

    for (int c = 0; c < numobjects; c++)
    {
        PTObjectInstance inst = TObjectInstance::LoadObject(is, version, TRUE);
        // Note: inst can be NULL here if a placeholder (-1) was saved for the obj class id

        if (inst)
        {
            inst->ForceSector(this);
            inst->ForceLevel(level); // Directly sets the inst's level variable
        }

        objects.Set(inst, c); 

        if (inst)
        {
            for (int d = 1; d < NUMOBJSETS; d++)
            {
                if (InObjSet(inst, d))
                    objsets[d-1].Set(c, objsets[d-1].NumItems());
            }
        }
    
        if (version < 3 && inst) // This is nolonger used (indexes are now unique id's)
            inst->SetMapIndex(MAKEINDEX(level, sectorx, sectory, c));
    }

    return TRUE;
}

void TSector::Save()
{
    int version = MAP_VERSION; // Current sector map version #

    if (objects.NumItems() == 0)
    {
        // don't save empty sectors, and clear out old save file
        unlink(filename);
        return;
    }

    BOOL closeit = FALSE;
    TOutputStream os(STARTSIZE, GROWSIZE);
    TPointerIterator<TObjectInstance> i(&objects);

    // Write out the "header" with "MAP " followed by the version #
    os << SectrorMapFCC;
    os << version;

    os << objects.NumItems();

    for ( ; i; i++)
    {
        TObjectInstance::SaveObject(i.Item(), os, TRUE);
    }

    char mappath[MAX_PATH];

    strcpy(mappath, CurMapPath);
    strcpy(mappath, CURMAPDIR "\\");
    strcat(mappath, filename);

    FILE *fp = popen(mappath, "wb");    // Path open (uses program path)
    if (!fp)
        return;

    fseek(fp, 0, 0);
    fwrite(os.Buffer(), os.DataSize(), 1, fp);
    fclose(fp);
}

// ****************************
// * Rectangles for Intersect *
// ****************************

// Added to the basic sector screen rect to include tiles which extend outside the sector rect
#define SECTORLREDGE    512             // Left/right edge
#define SECTORUEDGE     1024            // Upper edge
#define SECTORLEDGE     1024            // Lower edge

void TSector::GetMaxScreenRect(RSRect r)
{
    SRect maprect;
    GetMaxMapRect(maprect);

    S3DPoint p1, p2;

    p1.x = maprect.left;
    p1.y = maprect.top;
    p1.z = 0;

    p2.x = maprect.right;
    p2.y = maprect.bottom;
    p2.z = 0;

    WorldToScreen(p1, r.left, r.top);
    WorldToScreen(p2, r.right, r.bottom);
    r.left -= SECTORWIDTH;
    r.right += SECTORWIDTH;

  // Widen sector screen rect to include EVERYTHING that might overlap the sector boundries
    r.left -= SECTORLREDGE;
    r.right += SECTORLREDGE;
    r.top -= SECTORUEDGE;
    r.bottom += SECTORLEDGE;
}

void TSector::GetMaxMapRect(RSRect r)
{
    if ((DWORD)sectorx >= MAXSECTORX || (DWORD)sectory >= MAXSECTORY)
        r.left = r.top = r.right = r.bottom = 0;
    else
    {
        r.left = sectorx << SECTORWSHIFT;
        r.top = sectory << SECTORHSHIFT;
        r.right = r.left + SECTORWIDTH - 1;
        r.bottom = r.top + SECTORHEIGHT - 1;
    }
}

// ********************
// * Sector Utilities *
// ********************

// Adds an object to the sector
int TSector::AddObject(PTObjectInstance oi, int item)
{
    if (!oi)
        return -1;

    oi->ForceSector(this);
    oi->ForceLevel(level);

    if (item < 0)
        item = objects.Add(oi);
    else
        item = objects.Set(oi, item);

    for (int d = 1; d < NUMOBJSETS; d++)
    {
        if (InObjSet(oi, d))
            objsets[d-1].Set(item, objsets[d-1].NumItems());
    }

    return item;
}

// Sets an object into the sector at the given item index
int TSector::SetObject(PTObjectInstance oi, int item)
{
    item = objects.Set(oi, item);

    if (oi)
    {
        oi->ForceSector(this);
        oi->ForceLevel(level);

        for (int d = 1; d < NUMOBJSETS; d++)
        {
            if (InObjSet(oi, d))
                objsets[d-1].Set(item, objsets[d-1].NumItems());
        }
    }

    return item;
}

// Removes an object from the sector
PTObjectInstance TSector::RemoveObject(int item)
{
    PTObjectInstance oi = objects[item];
    objects.Remove(item);
    oi->ForceSector(NULL);

  // Remove object from sets
    for (int d = 1; d < NUMOBJSETS; d++)
    {
        TObjSetArray &set = objsets[d-1];

        for (int c = 0; c < set.NumItems(); c++)
        {
            if (set[c] == item)
            {
                set.Collapse(c);
                break;
            }
        }
    }

    return oi;
}

int TSector::GetObjIndex(PTObjectInstance oi)
{
    for (int i = 0; i < objects.NumItems(); i++)
    {
        if (objects[i] == oi)
            return i;
    }
    return -1;
}

void TSector::ObjectFlagsChanged(PTObjectInstance oi, DWORD oldflags, DWORD newflags)
{
    if (oi->GetSector() != this)
        return;

    if (OBJSETOBJFLAGS & (oldflags ^ newflags))
    {
        int index = GetObjIndex(oi);
        if (index < 0)
            return;

        RemoveObject(index);
        AddObject(oi);
    }
}

// *****************
// * Walkmap Stuff *
// *****************

void TSector::WalkmapHandler(int mode, BYTE *walk, int zpos, int x, int y, int width, int length, int stride, BOOL override)
{
    if (!walkmap || !walk)
        return;

    // first clip vars to be relative to the sector
    x -= (sectorx << SECTORWSHIFT) >> WALKMAPSHIFT;
    y -= (sectory << SECTORHSHIFT) >> WALKMAPSHIFT;

    int cx = 0, cy = 0, w = width, l = length;

    // now clip
    if (x < 0)
    {
        w += x;
        cx = -x;
        x = 0;
    }

    if (y < 0)
    {
        l += y;
        cy = -y;
        y = 0;
    }

    if ((x + w) > (SECTORWIDTH >> WALKMAPSHIFT))
        w = (SECTORWIDTH >> WALKMAPSHIFT) - x;

    if ((y + l) > (SECTORHEIGHT >> WALKMAPSHIFT))
        l = (SECTORHEIGHT >> WALKMAPSHIFT) - y;

    // cliped out completely?
    if (!w && !l)
        return;

    // Just Do It(tm)
    WORD *start = walkmap + (y * (SECTORWIDTH >> WALKMAPSHIFT)) + x;
    walk += (cy * stride) + cx;

    int srcstride = stride - w;
    int dststride = (SECTORWIDTH >> WALKMAPSHIFT) - w;

    for (int outerloop = 0; outerloop < l; outerloop++)
    {
        for (int innerloop = 0; innerloop < w; innerloop++)
        {
            if (mode == WALK_TRANSFER)
            {
                if (*walk)
                {
                    int walkval = *walk + zpos;

                    walkval = min(0xffff, max(1, walkval));

                    if (*start < walkval || override)
                        *start = walkval;
                }
            }
            else if (mode == WALK_CAPTURE)
            {
                if (*start == 0)
                    *walk = 0;
                else
                    *walk = min(255, max(1, *start - zpos));
            }
            else if (mode == WALK_CLEAR)
            {
                *start = 0;
            }

            start++;
            walk++;
        }

        walk += srcstride;
        start += dststride;
    }
}

void TSector::ClearWalkmap()
{
    memset(walkmap, 0, WALKMAPSIZE * 2);
}

// ----------------------- Preload Sector System -------------------

TSectorArray TSector::preloads;
int TSector::preloadlevel = -1;
int TSector::numpreloadrects = 0;
SRect TSector::preloadrects[MAXPRELOADRECTS];

// Preload sectors in this area (takes an area rectangle array from the area system)
BOOL TSector::LoadPreloadSectors(int level, int numrects, SRect *rects)
{
    ClearPreloadSectors(level, numrects, rects);

    preloadlevel = level;

    SRect srect;

    if (numrects > MAXPRELOADRECTS)
        numrects = MAXPRELOADRECTS;

    numpreloadrects = numrects;

    int maxcount = 0;
    int count = 0;

    for (int c = 0; c < numrects; c++)
    {
      // Copy to rect list
        memcpy(&(preloadrects[c]), &(rects[c]), sizeof(SRect));

      // convert map coords to sector coords
        srect.left = rects[c].left >> SECTORWSHIFT;
        srect.top = rects[c].top >> SECTORHSHIFT;
        srect.right = (rects[c].right + (SECTORWIDTH - 1)) >> SECTORWSHIFT;
        srect.bottom = (rects[c].bottom + (SECTORHEIGHT - 1)) >> SECTORHSHIFT;

      // sanity check
        srect.left = max(0, min(srect.left, MAXSECTORX - 1));
        srect.top = max(0, min(srect.top, MAXSECTORY - 1));
        srect.right = max(0, min(srect.right, MAXSECTORX - 1));
        srect.bottom = max(0, min(srect.bottom, MAXSECTORY - 1));

        maxcount = srect.w() * srect.h() * 2;
        
      // Preload sectors here       
        for (int y = srect.top; y <= srect.bottom; y++)
        {
          for (int x = srect.left; x <= srect.right; x++)
          {

          // Is this sector left over from last preloading
            BOOL alreadyloaded = FALSE;
            for (int s = 0; s < preloads.NumItems(); s++)
            {
              PTSector loaded = preloads[s];
              if (loaded->level == level && loaded->sectorx == x && loaded->sectory == y)
              {
                alreadyloaded = TRUE;
                break;
              }
            }

          // If not already in sector list, load it
            if (!alreadyloaded)
            {
                PTSector sector = new TSector(level, x, y);
                if (sector)
                {
                    sector->Load();
                    sector->preloaded = TRUE;
                    preloads.Add(sector);
                }
            }

          // Show levels in textbar
            if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
            {
                int level = 300 * count / (maxcount * 2);
                TextBar.SetLevels(level, level);
                TextBar.Draw();
                TextBar.PutToScreen();
            }
            count++;
          }
        }

      // Cache object graphics here 
      // (telescope in from outer sector borders to inner sectors so that the 
      // center sectors are the ones most likely to have cached graphics!)
        SRect b = srect;

        while (b.left < b.right && b.top < b.bottom)
        {
            for (int c = 0; c < preloads.NumItems(); c++)
            {
                PTSector s = preloads[c];
                if (s->sectorx == b.left || s->sectorx == b.right || // On borders of rect
                    s->sectory == b.top || s->sectory == b.bottom)
                {
                    TObjectArray &objects = s->objects;
                    for (int c = 0; c < objects.NumItems(); c++)
                    {
                        if (objects[c])
                            objects[c]->GetImagery()->CacheImagery();
                    }

                  // Show levels in textbar
                    if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
                    {
                        int level = 300 * count / (maxcount * 2);
                        TextBar.SetLevels(level, level);
                        TextBar.Draw();
                        TextBar.PutToScreen();
                    }
                    count++;
                }
            }

            b.left++;
            b.right--;
            b.top++;
            b.bottom--;

        }
    }

    return TRUE;
}

// Clear preload sectors
void TSector::ClearPreloadSectors(int level, int numrects, SRect *rects)
{
    for (int c = preloads.NumItems() - 1; c >= 0; c--)
    {
        PTSector sector = preloads[c];

      // Can't delete sectors which are currently in use!!
        if (sector->usecount > 0)
            continue;

      // Does this sector match our current level?
        BOOL in = TRUE;
        if (level != sector->level)
            in = FALSE; // No.. kill it

      // Is this sector in any of our current rectangles?
        if (in && numrects > 0 && rects)
        {
            SRect r;
            sector->GetMaxMapRect(r);
            for (int d = 0; d < numrects; d++)
            {
                if (rects[d].Intersects(r))
                    break;
            }
            if (d >= numrects)
                in = FALSE;     // Uh uh.. so kill it!
        }

      // This sector isn't in anything, so get rid of it!
        if (!in)
        {
            sector->Save();
            preloads.Collapse(c, TRUE);
        }
    }

  // Clear the rectangle list
    numpreloadrects = 0;
    preloadlevel = -1;
}

// Returns a preload sector
PTSector TSector::FindPreloadSector(int level, int sectorx, int sectory)
{
    for (int c = 0; c < preloads.NumItems(); c++)
    {
        if (preloads[c]->level == level &&
            preloads[c]->sectorx == sectorx &&
            preloads[c]->sectory == sectory)
                return preloads[c];
    }

    return NULL;
}

// Are we in the preload area?
BOOL TSector::InPreloadArea(RS3DPoint p, int level)
{
    if (level != preloadlevel)
        return FALSE;

    SPoint a;
    a.x = p.x;
    a.y = p.y;

    for (int c = 0; c < numpreloadrects; c++)
    {
        if (preloadrects[c].In(a))
            return TRUE;
    }

    return FALSE;
}

#ifdef _MAPPANE_H
#error If you need to access TMapPane from this file, you're doing something wrong... BEN
#endif

