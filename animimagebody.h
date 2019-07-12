// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              animimagebody.h - TAnimImageryBody object                *
// *************************************************************************

#ifndef _ANIIMAGEBODY_H
#define _ANIIMAGEBODY_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _IMAGERES_H
#include "imageres.h"
#endif

#define ANIIM_LIT       0x1         // draw anim imagery lit
#define ANIIM_UNLIT     0x2         // draw anim imagery unlit
#define ANIIM_SELECTED  0x4         // draw imagery when selected
#define ANIIM_CUTOFF    0x8         // don't draw imagery unless top is cut off

// *********************
// * SAnimImageryState *
// *********************

// The animation imagery state data structure contains the information
// necessary to describe the imagery and dimensions of a 2D animated
// object such as an item or a wall.

_STRUCTDEF(SAnimImageryState)

struct SAnimImageryState
{
    OTBitmap    still;                  // Still image
    OTAnimation anim;                   // Animation associated with state
    OTBitmap    invitem;                // Inventory image
    OTAnimation invanim;                // Inventory image animation
    DWORD       flags;                  // Imagery state flags
};

// ********************
// * SAnimImageryBody *
// ********************

// The anim imagery resource stores 2D animation graphics
// data for each state of the object.  The states are in
// consecutive order, and transitional states are considered
// to also be states.

_CLASSDEF(SAnimImageryBody)

class SAnimImageryBody : public SImageryBody
{
  public:
    SAnimImageryState states[1];        // Imagery for state
};

#endif

