// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                resource.cpp - Resource object module                  *
// *************************************************************************

#include <stdio.h>
#include <string.h>
#include <io.h>

#include "revenant.h"

#include "bitmap.h"
#include "graphics.h"
#include "resource.h"
#include "resourcehdr.h"
//#include "box.h"

static char erropen[]    = "Couldn't open resource file %s!";
static char errnotcgs[]  = "%s is not a CGS resource file!";
static char errreading[] = "Error reading resource file %s!";
static char errwriting[] = "Error writing resource file %s!";
static char erroldver[]  = "%s has an older resource file version!";
static char errnewver[]  = "%s has a newer resource file version!";
static char erralloc[]   = "Couldn't alloc memory for %s!";

//CBox ImageryBox("c:\\exile\\nonormal\\imagery.box", BOX_READ_WRITE);

void *LoadResource(char *name, int id, DWORD *ressize)
{
    char *ptr;
    char filename[81];
    char fname[81];
    FileResHdr frh;
    DWORD *bitmaptable;

    if (id >= 0 && id <= 999)
    {
        sprintf(fname, "%s.%03d", name, id);
        strcpy(filename, ResourcePath);
        strcat(filename, fname);
    }

    else
    {
        strcpy(filename, ResourcePath);
        strcat(filename, name);
        if (!strchr(filename, '.'))
            strcat(filename, ".DAT");
    }

    strupr(filename);

    FILE *fl = fopen(filename, "rb");

    if (!fl)
        FatalError(erropen, filename);

  // Load header and check validity
    if (fread(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errreading, filename);

    if (frh.resmagic != RESMAGIC)
        FatalError(errnotcgs, filename);

    if (frh.version < RESVERSION)
        FatalError(erroldver, filename);

    if (frh.version > RESVERSION)
        FatalError(errnewver, filename);

  // Skip over header
    if (frh.hdrsize > 0)
        fseek(fl, frh.hdrsize + sizeof(FileResHdr), 0);

  // Read bitmap table
    //int here = ftell(fl);

    if (frh.topbm)
    {
        bitmaptable = (DWORD *)malloc(sizeof(DWORD) * frh.topbm);
        if (fread(bitmaptable, sizeof(DWORD), frh.topbm, fl) < frh.topbm)
            FatalError(errreading, filename);
    }

  // Read resource
    ptr = (char *)malloc(frh.objsize);

    if (!ptr)
        FatalError(erralloc, name);

    int retval = fread(ptr, frh.datasize, 1, fl);
    fclose(fl);

    if (retval < 1)
        FatalError(errreading, filename);

    // Touch resource to force vm system to keep pages loaded
    char *m = ptr;
    for (int c = 0; c < (int)frh.datasize; c += 2048, m += 2048)
    {
        int dummy = *m;
    }

    // Do runtime conversion to 16 bit, if necessary.
    if (frh.topbm)
    {
        for (int i = 0; i < frh.topbm; i++)
        {
            PTBitmap bm = (PTBitmap) (ptr + bitmaptable[i]);

            // Sanity check
            if ((DWORD)bm->width > (DWORD)8192 || (DWORD)bm->height > (DWORD)8192)
                FatalError("Corrupted bitmap list in resource %s", name);

            if (bm->flags & BM_15BIT)
                Convert15to16(bm);

            if (bm->flags & BM_8BIT)
                ConvertPal15to16(bm);
        }
        free(bitmaptable);
    }

    if (ressize)
        *ressize = frh.objsize;

    return ptr;
}

void *LoadResourceHeader(char *name, int id, DWORD *ressize)
{
    static char filename[81];
    static char fname[81];

    FileResHdr frh;

    if (id >= 0 && id <= 999)
    {
        sprintf(fname, "%s.%03d", name, id);
        strcpy(filename, ResourcePath);
        strcat(filename, fname);
    }

    else
    {
        strcpy(filename, ResourcePath);
        strcat(filename, name);
        if (!strchr(filename, '.'))
            strcat(filename, ".DAT");
    }

    strupr(filename);

    FILE *fl = fopen(filename, "rb");

    if (!fl)
    {
        Status(erropen, filename);
        return NULL;
    }

    if (fread(&frh, sizeof(FileResHdr), 1, fl) < 1)
    {
        fclose(fl);
        Status(errreading, filename);
        return NULL;
    }

    if (frh.resmagic != RESMAGIC)
    {
        fclose(fl);
        Status(errnotcgs, filename);
        return NULL;
    }

    if (frh.version < RESVERSION)
    {
        fclose(fl);
        Status(erroldver, filename);
        return NULL;
    }

    if (frh.version > RESVERSION)
    {
        fclose(fl);
        Status(errnewver, filename);
        return NULL;
    }

    char *ptr = NULL;

    if (frh.hdrsize > 0)
    {
        ptr = (char *)malloc(frh.hdrsize);

        if (!ptr)
        {
            fclose(fl);
            Status(erralloc, name);
            return NULL;
        }

        int retval = fread(ptr, frh.hdrsize, 1, fl);
        fclose(fl);

        if (retval < 1)
        {
            Status(errreading, filename);
            return NULL;
        }
    }

    if (ressize)
        *ressize = frh.hdrsize;

    return ptr;
}

// This doesn't work right now!
/*BOOL SaveResource(char *name, void *ptr, int id)
{
    static char filename[81];
    static char fname[81];
    FileResHdr frh;

    if (id >= 0)
    {
        wsprintf(fname, "%s.%03d", name, id);
        strcpy(filename, ResourcePath);
        strcat(filename, fname);
    }

    else
    {
        strcpy(filename, ResourcePath);
        strcat(filename, name);
        if (!strchr(filename, '.'))
            strcat(filename, ".dat");
    }

    FILE *fl = fopen(filename, "wb");

    if (!fl)
        FatalError(erropen, filename);

    frh.resmagic = RESMAGIC;
    frh.topbm = 0;
    frh.comptype = COMP_ZIP;
    frh.version = RESVERSION;
    frh.objsize = GlobalSize((HGLOBAL)LOWORD(GlobalHandle
                                ((const void *)HIWORD((DWORD)ptr))));
    frh.datasize = 0;
    frh.hdrsize = 0;

    if (fwrite(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errwriting, filename);

    if (fwrite(ptr, frh.objsize, 1, fl) < 1)
        FatalError(errwriting, filename);

    frh.datasize = ftell(fl) - sizeof(FileResHdr);
    fseek(fl, 0, 0);
    if (fwrite(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errwriting, filename);

    fclose(fl);

    return FALSETRUE;
}
*/

BOOL SaveResourceHeader(char *name, void *header, int hdrsize, int id)
{
    static char filename[81];
    static char fname[81];
    char errreading[] = "Error reading resource file %s!";

    FileResHdr frh;

    if (hdrsize == 0 || !header)
        return TRUE;

    if (id >= 0 && id <= 999)
    {
        sprintf(fname, "%s.%03d", name, id);
        strcpy(filename, ResourcePath);
        strcat(filename, fname);
    }

    else
    {
        strcpy(filename, ResourcePath);
        strcat(filename, name);
        if (!strchr(filename, '.'))
            strcat(filename, ".DAT");
    }

    strupr(filename);

    FILE *fl = fopen(filename, "rb+");

    if (!fl)
        FatalError(erropen, filename);

    if (fread(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errreading, filename);

    if (frh.resmagic != RESMAGIC)
        FatalError(errnotcgs, filename);

    if (frh.version < RESVERSION)
        FatalError(erroldver, filename);

    if (frh.version > RESVERSION)
        FatalError(errnewver, filename);

  // Load in old header
    char *oldheader = (char *)malloc(frh.hdrsize);

    if (!oldheader)
        FatalError(erralloc, name);

    if (fread(oldheader, frh.hdrsize, 1, fl) < 1)
        FatalError(errreading, filename);

  // Check to see if header is different
    if ((int)frh.hdrsize == hdrsize && memcmp(oldheader, header, hdrsize) == 0)
    {
        free(oldheader);
        fclose(fl);
        return TRUE;
    }

    free(oldheader);

  // Read bitmap table
    DWORD *bitmaptable;
    if (frh.topbm)
    {
        bitmaptable = (DWORD *)malloc(sizeof(DWORD) * frh.topbm);
        if (fread(bitmaptable, sizeof(DWORD), frh.topbm, fl) < frh.topbm)
            FatalError(errreading, filename);
    }
    else
        bitmaptable = NULL;

  // Read resource
    char *data = (char *)malloc(frh.datasize);
    if (!data)
        FatalError(erralloc, name);

    if (fread(data, 1, frh.datasize, fl) < 1)
        FatalError(errreading, filename);

  // Close and reopen file
    rewind(fl);

  // Write out NEW header block
    frh.hdrsize = hdrsize;

  // Resave resource header
    if (fwrite(&frh, sizeof(FileResHdr), 1, fl) < 1)
        FatalError(errwriting, filename);

    if (fwrite(header, frh.hdrsize, 1, fl) < 1)
        FatalError(errwriting, filename);

  // Write out old bitmap table
    if (frh.topbm > 0)
    {
        if (fwrite(bitmaptable, sizeof(DWORD), frh.topbm, fl) < frh.topbm)
            FatalError(errwriting, filename);
    }

  // Write out old resource data
    if (fwrite(data, frh.datasize, 1, fl) < 1)
        FatalError(errwriting, filename);

  // Delete allocated memory
    if (data)
        free(data);

    if (bitmaptable)
        free(bitmaptable);

    fclose(fl);

    return TRUE;
}
