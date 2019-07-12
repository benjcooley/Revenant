// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     resource.h - Bitmap resource                      *
// *************************************************************************

#ifndef _RESOURCE_H
#define _RESOURCE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

void *LoadResource(char *name, int id = -1, DWORD *ressize = NULL);
void *LoadResourceHeader(char *name, int id = -1, DWORD *ressize = NULL);
void *LoadResourceBinary(char *name, int id = -1, DWORD *ressize = NULL);
BOOL SaveResource(char *name, void *ptr, int id = -1);
BOOL SaveResourceHeader(char *name, void *header, int hdrsize, int id = -1);

#endif
