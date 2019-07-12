// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    lightdef.h - Light Definition                      *
// *************************************************************************

#ifndef _LIGHTDEF_H
#define _LIGHTDEF_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _GRAPHICS_H
#include "graphics.h"
#endif

// Light flags
#define LIGHT_DIR       (1 << 0)                // Directional light
#define LIGHT_SUN       (1 << 1)                // Sunlight
#define LIGHT_MOON      (1 << 2)                // Moonlight

_STRUCTDEF(SLightDef)
struct SLightDef
{
    BYTE flags;         // LIGHT_x
    short multiplier;   // Multiplier
    S3DPoint pos;       // Position of light
    SColor color;       // RGB Color of light 
    BYTE intensity;     // Intensity of light
    int lightindex;     // Light index for 3d system
    int lightid;        // Light id for dls system
};

#endif
