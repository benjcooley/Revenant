// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *         imageres.h - TObjectImagery resource structures               *
// *************************************************************************

#ifndef _IMAGERES_H
#define _IMAGERES_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

// The object imagery data resource buffer is the base class
// for all object imagery data buffers.  The 'imageryid'
// member indicates which imagery object the imagery data
// will need to be able to display itself.  When the object
// class loades the imagery buffer, it checks the imagery
// id number, and creates an instance of the appropriate
// object imagery object for the buffer.

// ************************
// * TObjectImageryHeader *
// ************************

// The object imagery header is loaded when the object class system is initialized, and
// contains all non graphical imagery associated information about the object such as the
// states for the imagery, the walk map, the animation flags, etc.  This structure is store
// as a header at the beginning of an imagery resource file in uncompressed form so that
// it can be easily loaded when the system begins.  The graphics system will transparently
// load the other half of the imagery data when needed from the body portion of the imagery
// file.  The body portion will contain actual pixel, and/or model information for the 
// imagery.

// Imagery id's (index numbers into the imagery builder array)
enum
{
    OBJIMAGE_ANIMATION,
    OBJIMAGE_MESH3D,
    OBJIMAGE_MESH3DHELPER,
    OBJIMAGE_MULTI,
    OBJIMAGE_MULTIANIMATION
};

_STRUCTDEF(SImageryStateHeader)

#define MAXANIMNAME 32

struct SImageryStateHeader
{
    char        animname[MAXANIMNAME];  // Array of Ascii Names
    OFFSET      walkmap;    // Walkmap
    DWORD       flags;      // Imagery state flags
    short       aniflags;   // Animation state flags
    short       frames;     // Number of frames
    short       width;      // Graphics maximum width/height (for IsOnScreen and refresh rects) 
    short       height;     
    short       regx;       // Registration point x,y,z for graphics
    short       regy;       
    short       regz;
    short       animregx;   // Registration point of animation
    short       animregy;
    short       animregz;
    short       wregx;      // World registration x and y of walk and bounding box info
    short       wregy;
    short       wregz;
    short       wwidth;     // Object's world width, length, and height for walk map and bound box
    short       wlength;
    short       wheight;
    short       invaniflags;// Animation flags for inventory animation
    short       invframes;  // Number of frames of inventory animation
};

_STRUCTDEF(SImageryHeader)
struct SImageryHeader
{
    int                 imageryid;              // Id number for imagery handler (index to builder array)
    int                 numstates;              // Number of states
    SImageryStateHeader states[1];              // Imagery for state
};

// ****************
// * SImageryBody *
// ****************

_STRUCTDEF(SImageryBody)
struct SImageryBody
{
    // [insert tumbleweeds here]
};

#endif

