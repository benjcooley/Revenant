// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  animimage.h - TAnimImagery object                    *
// *************************************************************************

#ifndef _ANIMIMAGE_H
#define _ANIMIMAGE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _ANIMIMAGEBODY_H
#include "animimagebody.h"
#endif

#ifndef _BITMAP_H
#include "bitmap.h"
#endif

#ifndef _ANIMATION_H
#include "animation.h"
#endif

#ifndef _RESOURCE_H
#include "resource.h"
#endif

#ifndef _OBJECT_H
#include "object.h"
#endif

// ****************
// * TAnimImagery *
// ****************

// This object displays an anim image resource to the screen.
// It draws lit or unlit data for various states, and also
// creates a 2D sprite animator for animated objects.

_CLASSDEF(TAnimImagery)
class TAnimImagery : public TObjectImagery
{
  public:
    TAnimImagery(int imageryid) : TObjectImagery(imageryid) {}
    virtual ~TAnimImagery() {}

    PSAnimImageryState GetAnimState(int state)
      { return  &(((PSAnimImageryBody)GetBody())->states[state]); }
        // Returns pointer to anim imagery state

  // Caching functions
    virtual void CacheImagery();
      // Causes imagery to load itself and get ready for drawing
      // (Called by the SectorCache system to cause imagery to be loaded and decompressed)
      // AnimImagery uses this to call the TBitmap::CacheChunks() function  

    virtual void DrawUnlit(PTObjectInstance oi, PTSurface surface);
        // Draws unlit imagery to background
    virtual void DrawLit(PTObjectInstance oi, PTSurface surface);
        // Draws lit imagery to background
    virtual BOOL GetZ(PTObjectInstance oi, PTSurface surface);
        // Get first unclipped zbuffer point by simulating drawing to the surface
    virtual void DrawSelected(PTObjectInstance oi, PTSurface surface);
        // Causes image to draw selection (hilighting) around itself
    virtual BOOL AlwaysOnTop(PTObjectInstance oi);
        // Lit imagery is always on top because it has no zbuffer
    virtual PTBitmap GetStillImage(int state, int num = 0)
      { return (PTBitmap)GetAnimState(state)->still.ptr(); }
      // Returns the still image for the item
    virtual PTBitmap GetInvImage(int state, int num = 0)
      { return (PTBitmap)GetAnimState(state)->invitem.ptr(); }
      // Returns the inventory image for the item
    virtual PTAnimation GetInvAnimation(int state)
      { return (PTAnimation)GetAnimState(state)->invanim.ptr(); }
        // Get inventory animation for state
    virtual PTAnimation GetAnimation(int state) { return (PTAnimation)GetAnimState(state)->anim.ptr(); }
        // Get animation for state
    virtual DWORD GetImageFlags(int state) { return GetAnimState(state)->flags; }
        // Gets imagery flags for state
    virtual int GetAniLength(int state)
      { if (GetAnimation(state)) return GetAnimation(state)->NumFrames(); return 0; }
      // Returns length (in frames) of animation for this state
    virtual BOOL SaveBitmap(char *path, int state = 0, BOOL zbuffer = TRUE);
        // Saves a bitmap of the current object given the current state

    virtual PTObjectAnimator NewObjectAnimator(PTObjectInstance oi);
        // Creates an animtor for the given object
    virtual BOOL NeedsAnimator(PTObjectInstance oi);
      // Returns whether or not an animator is necessary
};

class TAnimAnimator : public TObjectAnimator
{
  public:
    TAnimAnimator(PTObjectInstance oi);
      // Constructor. Sets objectimagery. and object instance to NULL.
    virtual void Animate(BOOL draw);
      // Animates object
    virtual ~TAnimAnimator();
      // Destructor.
};

DEFINE_IMAGERYBUILDER(OBJIMAGE_ANIMATION, TAnimImagery);

#endif

