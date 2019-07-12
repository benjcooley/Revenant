// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      object.cpp - TObject module                      *
// *************************************************************************

#include <windows.h>
#include <process.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "revenant.h"
#include "bitmap.h"
#include "imagery.h"
#include "object.h"
#include "resource.h"
#include "graphics.h"
#include "display.h"

// Builder array for imagery builder class
int TImageryBuilder::numimagerytypes = 0;
PTImageryBuilder TImageryBuilder::builders[MAXIMAGERYTYPES];

// Imagery loading status values
#define QE_NONE      0
#define QE_QUEUED    1
#define QE_LOADING   2
#define QE_LOADED    3
#define QE_FAILED    4

extern DWORD ImageryMemUsage;

static TVirtualArray <SImageryEntry> EntryArray;

static char imagerypath[FILENAMELEN];

//BOOL LoaderWait = FALSE;

// *******************
// * TObjectAnimator *
// *******************

TObjectAnimator::TObjectAnimator(PTObjectInstance oi)
{
    inst = oi;
    image = oi->GetImagery();

  // These values are copied from 'inst' each frame 
  // (frame/framerate are copied back after ani)
    prevstate = state = -1;
    frame = 0;      
    framerate = 1;

    newstate = TRUE;
    complete = FALSE;
}

TObjectAnimator::~TObjectAnimator()
{
    Close();
}

void TObjectAnimator::ResetState()
{
    state = inst->GetState();
    prevstate = inst->GetPrevState();
    frame = inst->GetFrame();
    framerate = inst->GetFrameRate();

    newstate = TRUE;
    complete = FALSE;
}

void TObjectAnimator::Animate(BOOL draw)
{
    state = inst->GetState();
    prevstate = inst->GetPrevState();
    frame = inst->GetFrame();
    framerate = inst->GetFrameRate();
}

// *******************
// * TImageryBuilder *
// *******************

TImageryBuilder::TImageryBuilder(int newid)
{
    if (newid >= numimagerytypes)
        numimagerytypes = newid + 1;

    builders[newid] = this;
    imageryid = newid;
}

// ******************
// * TObjectImagery *
// ******************

TObjectImagery::TObjectImagery(int id)
{
    imageryid = id;
    entry = &EntryArray[id];

    entry->headerdirty = FALSE;
    entry->bodydirty = FALSE;

    if (!entry->body)
        LoadBody(FALSE); // Queue imagery load
}

TObjectImagery::~TObjectImagery()
{
    FreeBody();
}

int TObjectImagery::RegisterImagery(char *filename, PSImageryHeader header, DWORD headersize)
{
    for (int c = 0; c < EntryArray.NumItems(); c++)
    {
        if (EntryArray.Used(c) && !stricmp(EntryArray[c].filename, filename))
        {
            if (header)
                free(header);
            return c;
        }
    }

    if (!header)
    {
        char buf[FILENAMELEN];
        strcpy(buf, imagerypath);
        strcat(buf, filename);
        header   = (PSImageryHeader)LoadResourceHeader(buf, -1, (DWORD *)&headersize);
    }

    if (!header)
    {
        Status("WARNING: Unable to load imagery header for %s", filename);
        return -1;
    }

    SImageryEntry ie;
    memset(&ie, 0, sizeof(SImageryEntry));
    strncpy(ie.filename, filename, MAXIMFNAMELEN - 1);

    ie.status     = QE_NONE;
    ie.header     = header;
    ie.headersize = headersize;
    ie.body       = NULL;
    ie.usecount   = 0;
    ie.imagery    = NULL;

    ie.headerdirty = FALSE;
    ie.bodydirty = FALSE;

//  VirtualLock(ie.header, ie.headersize);

#ifdef CHECK_IMAGERY_HEADER_VALIDITY

// This stuff checks the header validity of the imagery by checking the size
// of the bounding box compared to the walkmap.  To turn checking off, comment
// out the define above.  To turn on auto-fix mode (rebuilds any corrupted headers
// found, killing whatever walkmaps they might have had) comment the three FatalError()
// calls out, below, and change the #if 0 to #if 1.

    int size = sizeof(SImageryHeader);

    BOOL fixit = FALSE;

    for (int i = 0; i < ie.header->numstates; i++)
    {
        if (i > 0)
            size += sizeof(SImageryStateHeader);

        size += ie.header->states[i].wwidth * ie.header->states[i].wlength;
        if (ie.header->states[i].wwidth < 1 || ie.header->states[i].wlength < 1)
        {
            if (ie.header->states[i].walkmap.ptr() != NULL)
            {
                FatalError("State has walkmap but no bounding box");
                fixit = TRUE;
            }
        }
        else
        {
            if (ie.header->states[i].walkmap.ptr() == NULL)
            {
                FatalError("State has bounding box but no walkmap");
                fixit = TRUE;
            }
        }
    }

    size = (size + 3) & 0xFFFFFFFCL;         // Round to even 4 bytes

    if ((fixit || size != ie.headersize) && !strstr(ie.filename, "i3d"))
    {
        FatalError("'Holy shitballs, Batman!  The imagery is fucked up again!  Curse that infernal Joker!'");
#if 0
        BYTE *newbuf = (BYTE *)malloc(size);
        int skipdist = sizeof(SImageryHeader) + (sizeof(SImageryStateHeader) * (ie.header->numstates - 1));
        BYTE *ptr = newbuf + skipdist;

        PSImageryHeader head = (PSImageryHeader)newbuf;
        memcpy(newbuf, ie.header, sizeof(SImageryHeader));

        for (i = 0; i < ie.header->numstates; i++)
        {
            PSImageryStateHeader sh = &(head->states[i]);

            memcpy(sh, &ie.header->states[i], sizeof(SImageryStateHeader));

            int w = ie.header->states[i].wwidth;
            int l = ie.header->states[i].wlength;

            if (w && l)
            {
                sh->walkmap.set(ptr);
                memset(ptr, 0, w * l);
                ptr += w * l;
            }
            else
                sh->walkmap.set(NULL);
        }

        ie.header = (PSImageryHeader)newbuf;
        ie.headersize = size;
        ie.headerdirty = TRUE;
#endif
    }
#endif

    int id = EntryArray.Add(ie);

    if (id < 0)
        FatalError("Not enough room for imagery in imagery entry array!");

    return id;
}

PSImageryEntry TObjectImagery::GetImageryEntry(int id)
{
    return &(EntryArray[id]);
}

void TObjectImagery::FreeImageryEntry(int imageryentry)
{
    if (EntryArray[imageryentry].header)
        free(EntryArray[imageryentry].header);

    if (EntryArray[imageryentry].body)
        free(EntryArray[imageryentry].body);

    memset(&EntryArray[imageryentry], 0, sizeof(SImageryEntry));
}

void TObjectImagery::FreeAllImagery()
{
    if (Editor)
        SaveAllHeaders();

    for (int c = 0; c < EntryArray.NumItems(); c++)
        FreeImageryEntry(c);
}

void TObjectImagery::ReloadImagery()
{
    BEGIN_CRITICAL();

    for (int loop = 0; loop < EntryArray.NumItems(); loop++)
    {
        if (EntryArray.Used(loop))
        {
            EntryArray[loop].status  = QE_NONE;
            EntryArray[loop].imagery = NULL;
            EntryArray[loop].body    = NULL;
            EntryArray[loop].ressize = 0;
        }
    }

    END_CRITICAL();
}

void TObjectImagery::SetImageryPath(char *path)
{
    if (path)
        strcpy(imagerypath, path);
}

char *TObjectImagery::GetImageryPath()
{
    return imagerypath;
}

BOOL TObjectImagery::RenameImageryFile(int imageryid, char *newfile)
{
    if ((imageryid < 0) || (EntryArray.Used(imageryid) == FALSE) ||
        (imageryid >= EntryArray.NumItems()) || (strlen(newfile) >= MAXIMFNAMELEN))
        return FALSE;

    strcpy(EntryArray[imageryid].filename, newfile);
    EntryArray[imageryid].headerdirty = TRUE;

    return TRUE;
}

void TObjectImagery::SaveAllHeaders()
{
    for (int c = 0; c < EntryArray.NumItems(); c++)
        SaveHeader(c);

    QuickSaveHeaders();  // Save a single file with all headers in it
}

void TObjectImagery::SetEntryReg(int imageryid, int state, int regx, int regy, int regz)
{
    EntryArray[imageryid].header->states[state].regx = regx;
    EntryArray[imageryid].header->states[state].regy = regy;
    EntryArray[imageryid].header->states[state].regz = regz;
    EntryArray[imageryid].headerdirty = TRUE;
}

int TObjectImagery::FindImagery(char *imageryname)
{
    for (int c = 0; c < EntryArray.NumItems(); c++)
    {
        if (!EntryArray.Used(c))
            continue;

        RSImageryEntry ie = EntryArray[c];

        if (!stricmp(ie.filename, imageryname))
            return c;
    }

    return -1; 
}

PTObjectImagery TObjectImagery::LoadImagery(int imgid)
{
    if (imgid < 0 || EntryArray.Used(imgid) == FALSE ||
        imgid >= EntryArray.NumItems())
        return NULL;

    RSImageryEntry ie = EntryArray[imgid];

    if (ie.imagery)
    {
        ie.usecount++;
    }

    else
    {
        PTImageryBuilder imbuilder = TImageryBuilder::GetBuilder(ie.header->imageryid);
        if (!imbuilder)
            return NULL;

        ie.imagery = imbuilder->Build(imgid);
        if (!ie.imagery)
            return NULL;

        ie.usecount = 1;
    }

    ie.imagery->imageryid = imgid;

    return ie.imagery;
}

void TObjectImagery::FreeImagery(PTObjectImagery imagery)
{
    if (!imagery || (DWORD)imagery->imageryid >= (DWORD)EntryArray.NumItems())
        return;

    if (!EntryArray.Used(imagery->imageryid))
        return;

    RSImageryEntry ie = EntryArray[imagery->imageryid];

    ie.usecount--;
    if (ie.usecount <= 0)
    {
        imagery->FreeBody();

        if (Editor && ie.imagery)   // Save header information if in editor
        {
            ie.imagery->SaveHeader();

            ImageryMemUsage -= ie.imagery->GetResSize();

            delete ie.imagery;
            ie.imagery = NULL;
        }

        ie.usecount = 0;
    }
}

// Restores all lost imagery surfaces, etc.
void TObjectImagery::RestoreAll()
{
    for (int c = 0; c < EntryArray.NumItems(); c++)
    {
        RSImageryEntry ie = EntryArray[c];
        if (ie.imagery != NULL)
            ie.imagery->Restore();
    }
}

int TObjectImagery::FindState(char *name, int pcnt)
{
    if (!name)
        return -1;

    int lowest = 100000; // Lowest frequency value
    int found = -1;      // State with closest percent

    for (int loop = 0; loop < GetHeader()->numstates; loop++)
    {
        char *n = GetHeader()->states[loop].animname;
        char *pcntchar;
        int freq;

        if (n[0] <= '9' && n[0] >= '0' &&           // Starts with a number AND
            (pcntchar = strchr(n, ':')) != NULL)    // Number is a frequency percentage
        {
            freq = atoi(n);
            n = pcntchar + 1;
        }
        else
            pcntchar = NULL;

        if (!strcmpi(n, name))
        {
          // Found what we're looking for, and it doesn't have a frequency prefix.. DONE!
            if (!pcntchar)
            {
                found = loop;
                break;
            }
        
          // We have a frequency prefix.. do we have a random percentage value?
            if (pcnt < 0)
                pcnt = random(1, 100);  // No.. then get one!

          // Check to see if frequency is closest to, but still >= percentage value
            if (freq >= pcnt && freq < lowest)
            {
                lowest = freq;
                found = loop;
            }
        }
    }

    return found;
}

// This function checks string against statename allowing for the "or" seperator.
// For example, "one" would match up against "one or two or three".
BOOL StateMatch(char *string, char *statename)
{
    if (!string || !statename)
        return FALSE;

    char buf[80];

    char *ptr = statename;
    do
    {
        char *sep = strstr(ptr, " or ");
        if (sep == NULL)
        {
            if (strcmpi(string, ptr) == 0)
                return TRUE;

            break;
        }
        else
        {
            int len = (int)(sep - ptr);
            memcpy(buf, ptr, len);
            buf[len] = 0;
        }

        if (strcmpi(string, buf) == 0)
            return TRUE;

        ptr = sep + 4;          // skip past the "or"
    } while (ptr);

    return FALSE;
}

// Finds a transition state, such as "stand to walk" if you passed in "stand" and "walk".
// Note that "stand or walk to walk", "stand to walk or run", and "stand or turn to walk" would also work.
int TObjectImagery::FindTransitionState(char *from, char *to, int pcnt)
{
    if (!from || !to)
        return -1;

    BOOL samestate = (strcmpi(from, to) == 0);

    char buf[80];

    int lowest = 100000; // Lowest frequency value
    int found  = -1;     // State with closest percent
    int highest = -1;

    for (int loop = 0; loop < GetHeader()->numstates; loop++)
    {
        char *left = GetHeader()->states[loop].animname;
        char *pcntchar;
        int freq;

      // Check to see if state has frequency prefix
        if (left[0] <= '9' && left[0] >= '0' &&     // Starts with a number AND
            (pcntchar = strchr(left, ':')) != NULL) // Number is a frequency percentage
        {
            freq = atoi(left);
            left = pcntchar + 1;
        }
        else
            pcntchar = NULL;

        BOOL any_state;
        char *right;
        
        if(strncmp(left, "to ", 3) == 0)
        {
            any_state = TRUE;
            right = left + 3;
        }
        else
        {
            any_state = FALSE;
          // Find the " to " in the name if it has one
            right = strstr(left, " to ");

           // If has " to ", get separate right/left strings
            if (right)
            {
                int len = (int)(right - left);
                memcpy(buf, left, len);
                buf[len] = 0;
                left = buf;
                right += 4;     // skip the "to"
            }
        }

      // State names match?
        if ((StateMatch(from, left) || any_state) &&                  // Matches left AND...
            ((right == NULL && samestate) || StateMatch(to, right)))  // no right, or matches right
        {
          // Found what we're looking for, and it doesn't have a frequency prefix.. DONE!
            if (!pcntchar)
            {
                found = loop;
                break;
            }
        
          // We have a frequency prefix.. do we have a random percentage value?
            if (pcnt < 0)
                pcnt = random(1, 100);  // No.. then get one!

          // Check to see if frequency is closest to, but still >= percentage value
            if (freq >= pcnt && freq < lowest)
            {
                lowest = freq;
                found = loop;
            }
        }
    }

    return found;
}

int TObjectImagery::GetUseCount()
{
    return entry->usecount;
}

extern BOOL UpdatingBoundingRect;

// Returns screen update area and 'onscreen' intersection rectangle given data from state
void TObjectImagery::GetScreenRect(PTObjectInstance oi, SRect &r)
{
    if ((DWORD)oi->GetState() >= (DWORD)NumStates())
    {
        r.left = r.right = r.top = r.bottom = 0;
        return;
    }

    int x, y;
    oi->GetScreenPos(x, y);

    PSImageryStateHeader st = GetState(oi->GetState());

    if (!UpdatingBoundingRect && (st->width == 0 || st->height == 0)) // Set some kind of initial value here
    {
        st->regx = 16;
        st->regy = 32;
        st->width = 32;
        st->height = 32;
    }

    r.left   = x - st->regx;
    r.right  = r.left + st->width - 1;
    r.top    = y - st->regy;
    r.bottom = r.top + st->height - 1;
}

void TObjectImagery::GetAnimRect(PTObjectInstance oi, SRect &r)
{
    PSImageryStateHeader st = GetState(oi->GetState());

    if (!st || (st->animregx == 0 && st->animregy == 0))
    {
        r.left = r.right = r.top = r.bottom = 0;
        return;
    }

    int x, y;
    oi->GetScreenPos(x, y);

    r.left   = x + st->animregx;
    r.right  = r.left + st->width - 1;
    r.top    = y + st->animregy;
    r.bottom = r.top + st->height - 1;
}

void TObjectImagery::ResetScreenRect(PTObjectInstance oi, int state, BOOL frontonly)
{
    if (state < 0)
    {
        for (int i = 0; i < NumStates(); i++)
            ResetScreenRect(oi, i);
    }
    else if ((DWORD)state < (DWORD)NumStates())
    {
        PSImageryStateHeader st = GetState(state);
        st->regx = st->regy = 0;
        st->width = st->height = 0;

        SetHeaderDirty(TRUE);
    }
}

void TObjectImagery::SaveHeader(int imgid)
{
    if (imgid < 0 || EntryArray.Used(imgid) == FALSE ||
        imgid >= EntryArray.NumItems())
         FatalError("Invalid id when saving header");

    RSImageryEntry ie = EntryArray[imgid];

    if (ie.headerdirty)
    {
        char buf[120];
        strcpy(buf, imagerypath);
        strcat(buf, ie.filename);
        SaveResourceHeader(buf, ie.header, ie.headersize);

        ie.headerdirty = FALSE;
    }
}

void TObjectImagery::SetWorldBoundBox(int state, int width, int length, int height)
{
    if ((DWORD)state >= (DWORD)entry->header->numstates)
        return;

    if (width != entry->header->states[state].wwidth ||
        length != entry->header->states[state].wlength)
    {
        // reallocating the walkmap requires changing the structure size;
        // therefore, must start over with a new buffer and copy over
        // all the old data
        int size = sizeof(SImageryHeader);

        for (int i = 0; i < entry->header->numstates; i++)
        {
            if (i > 0)
                size += sizeof(SImageryStateHeader);

            if (i == state)
                size += width * length;
            else
                size += entry->header->states[i].wwidth * entry->header->states[i].wlength;
        }

        size = (size + 3) & 0xFFFFFFFCL;         // Round to even 4 bytes
        BYTE *newbuf = (BYTE *)malloc(size);
        int skipdist = sizeof(SImageryHeader) + (sizeof(SImageryStateHeader) * (entry->header->numstates - 1));
        BYTE *ptr = newbuf + skipdist;

        PSImageryHeader head = (PSImageryHeader)newbuf;
        memcpy(newbuf, entry->header, sizeof(SImageryHeader));

        for (i = 0; i < entry->header->numstates; i++)
        {
            PSImageryStateHeader sh = &(head->states[i]);

            memcpy(sh, &entry->header->states[i], sizeof(SImageryStateHeader));

            if (i == state)
            {
                if (width && length)
                {
                    sh->walkmap.set(ptr);
                    memset(ptr, 0, width * length);
                    ptr += width * length;
                }
                else
                    sh->walkmap.set(NULL);
            }
            else
            {
                int w = entry->header->states[i].wwidth;
                int l = entry->header->states[i].wlength;

                if (w && l)
                {
                    sh->walkmap.set(ptr);
                    memcpy(ptr, entry->header->states[i].walkmap.ptr(), w * l);
                    ptr += w * l;
                }
                else
                    sh->walkmap.set(NULL);
            }
        }

//      VirtualUnlock(entry->header, entry->headersize);
        free(entry->header);

        entry->header = (PSImageryHeader)newbuf;
        entry->headersize = size;
//      VirtualLock(entry->header, entry->headersize);
    }

    entry->header->states[state].wwidth = width;
    entry->header->states[state].wlength = length;
    entry->header->states[state].wheight = height;

    entry->headerdirty = TRUE;
}

void TObjectImagery::DrawInvItem(PTObjectInstance oi, int x, int y)
{
    int state = oi->GetState();

    if (state >= NumStates())
        return;

    PTBitmap invitem = oi->InventoryImage();
    if (!invitem)
        return;

    Display->Put(x, y, invitem, DM_TRANSPARENT | DM_BACKGROUND);
}

// ******************* Progressive Load System ******************

static BOOL QuitThread;
static HANDLE LoadBodyEvent, LoadCompleteEvent, PauseLoaderMutex, LoaderThreadHandle;
static unsigned LoaderThreadId;

void TObjectImagery::BeginLoaderThread()
{
    LoadBodyEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    LoadCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    PauseLoaderMutex = CreateMutex(NULL, FALSE, NULL);

    QuitThread = FALSE;

    LoaderThreadHandle = (HANDLE)_beginthreadex(
        NULL, 0,
        &TObjectImagery::LoaderThread, NULL, TRUE,
        &LoaderThreadId );
}

void TObjectImagery::EndLoaderThread()
{
    while (ReleaseMutex(PauseLoaderMutex));
    QuitThread = TRUE;
    PulseEvent(LoadBodyEvent);

    WaitForSingleObject(LoaderThreadHandle, INFINITE);
}

unsigned _stdcall TObjectImagery::LoaderThread(void *)
{
    for (;;)
    {
        WaitForSingleObject(LoadBodyEvent, INFINITE);

        if (QuitThread)
            break;

        for (int loop = 0; loop < EntryArray.NumItems(); loop++)
        {
            WaitForSingleObject(PauseLoaderMutex, INFINITE);

            if (!EntryArray.Used(loop))
                continue;

            RSImageryEntry ie = EntryArray[loop];

            BEGIN_CRITICAL();

            if (ie.status == QE_QUEUED)
            {
                ie.status = QE_LOADING;

                END_CRITICAL();

                char buf[120];
                strcpy(buf, imagerypath);
                strcat(buf, ie.filename);

                DWORD ressize;
                PSImageryBody body = (PSImageryBody)LoadResource(buf, -1, &ressize);

                BEGIN_CRITICAL();

                if (!body)                              // Mark load as failed
                {
                    ie.status = QE_FAILED;

                    PulseEvent(LoadCompleteEvent);
                }
                if (ie.imagery != NULL) // Did system delete imagery while we were loading body
                {
                    ie.ressize = ressize;           // No, then fill out data
                    ie.body = body;
                    ImageryMemUsage += ressize;
                    ie.status = QE_LOADED;

//                  VirtualLock(body, ressize);

                    PulseEvent(LoadCompleteEvent);
                }
                else
                {
                    delete body;                        // YES?.. delete body and continue
                    ie.status = QE_NONE;
                }
            }

            END_CRITICAL();

            while (ReleaseMutex(PauseLoaderMutex));
        }
    }

  // Kill the events
    while (ReleaseMutex(PauseLoaderMutex)); // Release if I own it just in case
    CloseHandle(LoadCompleteEvent);
    CloseHandle(LoadBodyEvent);
    CloseHandle(PauseLoaderMutex);

    _endthreadex( 0 );

    return 0;
}

void TObjectImagery::PauseLoader()
{
    WaitForSingleObject(PauseLoaderMutex, INFINITE);
}

void TObjectImagery::ResumeLoader()
{
    ReleaseMutex(PauseLoaderMutex);
}

PSImageryBody TObjectImagery::LoadBody(BOOL wait)
{
    if (entry->status == QE_LOADED)
        return entry->body;

    ReleaseMutex(PauseLoaderMutex); // Just in case somebody forgot to unpause the loader thread

    BEGIN_CRITICAL();
    if (entry->status == QE_NONE || entry->status == QE_QUEUED)
    {
        if (wait)
        {
            entry->status = QE_LOADING;

            END_CRITICAL();

            char buf[120];
            strcpy(buf, imagerypath);
            strcat(buf, entry->filename);

            entry->body = (PSImageryBody)LoadResource(buf, -1, (DWORD *)&entry->ressize);

            BEGIN_CRITICAL();

            if (!entry->body)                               // Mark load as failed
            {
                entry->status = QE_FAILED;
            }
            else
            {
                ImageryMemUsage += entry->ressize;

                entry->status = QE_LOADED;
            }
        }
        else
        {
            entry->status = QE_QUEUED;
            QuitThread = FALSE;
            PulseEvent(LoadBodyEvent);
        }
    }
    END_CRITICAL();

    if (entry->status == QE_LOADING)
    {
        if (wait)
        {
            while (entry->status == QE_LOADING)
            {
                WaitForSingleObject(LoadCompleteEvent, INFINITE);
            }
        }
    }

    if (entry->status == QE_FAILED)
    {
        FatalError("Imagery body load failed!");
    }

    return entry->body;
}

void TObjectImagery::FreeBody()
{
    BEGIN_CRITICAL();

    if (entry->status == QE_LOADING)
    {
        END_CRITICAL();

        while (entry->status == QE_LOADING)
        {
            WaitForSingleObject(LoadCompleteEvent, INFINITE);
        }

        BEGIN_CRITICAL();
    }

    if (entry->status == QE_QUEUED || entry->status == QE_FAILED || entry->status == QE_NONE)
    {
        entry->status = QE_NONE;
        entry->body = NULL;

        END_CRITICAL();
        return;
    }

    if (entry->status == QE_LOADED)
    {
        entry->status = QE_NONE;
//      VirtualUnlock(entry->body, entry->ressize);
        free(entry->body);
        entry->body = NULL;

        END_CRITICAL();
        return;
    }
}

// ******************* END OF Progressive Load System ******************

// *******************************
// * Imagery Quickload Functions *
// *******************************

struct SQuickLoadHeader
{
    DWORD id;
    DWORD version;
    DWORD numheaders;
};

#define QUICKLOADFILEID   (('H') | ('D' << 8) | ('R' << 16) | ('S' << 24))
#define QUICKLOADFILEVER  (1)

BOOL TObjectImagery::QuickLoadHeaders(time_t iflater)
{
    char filename[FILENAMELEN];
    struct _stat s;

  // We can only do a quickload if there's nothing in there yet!!
    if (EntryArray.NumItems() > 0)
        return FALSE;

    strcpy(filename, ResourcePath);
    strcat(filename, "IMAGERY.DAT");

    FILE *f = fopen(filename, "rb");
    if (!f)
        return FALSE;

  // Is file later than the iflater time?
    _fstat(fileno(f), &s);
    if (s.st_mtime <= iflater)
    {
        fclose(f);
        return FALSE;   // No: don't use file, it's out of date
    }

    SQuickLoadHeader qh;

    if (fread(&qh, sizeof(SQuickLoadHeader), 1, f) < 1)
    {
        fclose(f);
        return FALSE;
    }

    if (qh.id != QUICKLOADFILEID || qh.version != QUICKLOADFILEVER)
    {
        fclose(f);
        return FALSE;
    }

    for (int c = 0; c < (int)qh.numheaders; c++)
    {
        char filename[MAXIMFNAMELEN];

        if (fread(&filename, MAXIMFNAMELEN, 1, f) < 1)
        {
            fclose(f);
            return FALSE;
        }

        DWORD headersize;
        if (fread(&headersize, 4, 1, f) < 1)
        {
            fclose(f);
            return FALSE;
        }

        PSImageryHeader header = (PSImageryHeader)malloc(headersize);
        if (fread(header, headersize, 1, f) < 1)
        {
            fclose(f);
            return FALSE;
        }

        RegisterImagery(filename, header, headersize);
    }

    fclose(f);

    return TRUE;
}

BOOL TObjectImagery::QuickSaveHeaders()
{
    char filename[FILENAMELEN];
    int c;

    strcpy(filename, ResourcePath);
    strcat(filename, "IMAGERY.DAT");

    FILE *f = fopen(filename, "wb");
    if (!f)
        return FALSE;

    SQuickLoadHeader qh;
    memset(&qh, 0, sizeof(SQuickLoadHeader));

    qh.id = QUICKLOADFILEID;
    qh.version = QUICKLOADFILEVER;
    qh.numheaders = 0;

    for (c = 0; c < EntryArray.NumItems(); c++)
    {
        if (EntryArray.Used(c))
            qh.numheaders++;
    }

    if (fwrite(&qh, sizeof(SQuickLoadHeader), 1, f) < 1)
    {
        fclose(f);
        return FALSE;
    }

    for (c = 0; c < EntryArray.NumItems(); c++)
    {
        if (!EntryArray.Used(c))
            continue;

        RSImageryEntry ie = EntryArray[c];

        if (fwrite(&ie.filename, MAXIMFNAMELEN, 1, f) < 1)
        {
            fclose(f);
            return FALSE;
        }

        if (fwrite(&ie.headersize, 4, 1, f) < 1)
        {
            fclose(f);
            return FALSE;
        }

        if (fwrite(ie.header, ie.headersize, 1, f) < 1)
        {
            fclose(f);
            return FALSE;
        }
    }

    fclose(f);

    return TRUE;
}
