// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                resource.cpp - Resource object module                  *
// *************************************************************************

#include <stdio.h>
#include <string.h>

#include "revenant.h"
#include "multi.h"

void *TMulti::Object(char *name)
{
    for (int c = 0; c < numoffsets; c++)
    {
        char *p = (char *)((void *)names[c]);
        if (p && !stricmp(p, name))
            return (void *)offsets[c]; 
    }

    char buf[80];
    sprintf(buf, "Unable to find \'%s\' in multiresource", name);
    FatalError(buf);

    return NULL;
}
