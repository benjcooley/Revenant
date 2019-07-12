// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 TStripEffect.h - TStripEffect module                  *
// *************************************************************************

#ifndef _STRIPEFFECT_H
#define _STRIPEFFECT_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "object.h"
#include "effect.h"
#include "effectcomp.h"
#include "3dimage.h"
#include "mappane.h"
#include "missileeffect.h"

/*
	//* I'm not sure how to do this since the points exist in the strip, but the rectangle
	//* needs to be in the animator, and I don't know how to get or translate the visible
	//* rectangle for any point and add it individually to the Extents list

	// WorldToScreen( RS3DPoint pos, int &x, int &y);
	// WorldToScreen( RS3DPoint pos, RS3DPoint spos);

	AddUpdateRects(SRect drawextents); // Called after outside function calls RenderObject()
	    // For obj returned by GetStrip()..
		// Adds an update rect size of segboundrect for each point drawn
		// Should check size of extents, and size of each bound rect added together, and
		// add smaller amount.

	SetBoundingRect(&SRect r);  // Any rects based on segboundrect completely outside this
	  // rect won't be added to in the Generate function
*/


// ******************
// * TStripEffect   *
// ******************

enum {
	STRIP_LAUNCH = 0,
	STRIP_FLY,
	STRIP_EXPLODE
};

#define STRIP_SPEED	16
#define STRIP_RANGE	2

_CLASSDEF(TStripEffect)

class TStripEffect : public TEffect
{
	public:
	int duration;
	int	targetrange;
	int my_state;
	PTCharacter chr;

	TStripEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
	TStripEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }

	virtual void Initialize();

	virtual void Pulse();

	virtual void OffScreen();	// StripEffects stay on-screen until the effect is finished
};



// **********************
// * Lightning Animator *
// **********************

// Consists of a lightning strip, a glow above the caster,
// and a shower of sparks

#define SMOOTH_SIZE		4
#define ADD_FACTOR		2
#define TARGET_POS		1	// make this 0 to test lightning effect, 1 to actually use it
#define LIGHTNING_SCALE_N	2
#define LIGHTNING_SCALE_D	1
#define MAX_MAXPOINTS		100

_CLASSDEF(TLightningAnimator)
class TLightningAnimator : public T3DAnimator
{
protected:
	TSubParticleAnimator spark;	// da sparks!
	TSubParticleAnimator impact_spark;	// da sparks!
	float glow_scale;
	int total_offset;
	D3DVECTOR end_p;
	BOOL damage_flag;
	BOOL firsttime;
public:
	PTStripAnimator	mystrip;
	int	numpoints;		// number of points on strip
	int	maxpoints;		// maxpoints
	//D3DVECTOR next_strips[SMOOTH_SIZE];	// the strips to add

	int showangle;		// Angle of strip
	int rotdegree;		// Rotation of first disc
	int morrotdegree;	// Rotation of last disc
	bool maxsize;		// Have all the points been added?  (Used when deleting points)
	bool down;

    TLightningAnimator(PTObjectInstance oi) : T3DAnimator(oi) { mystrip = NULL; numpoints = 0; }
	  // Constructor (initialization handled by Initialize)
    virtual ~TLightningAnimator() { Close(); }
	  // Call close function

    virtual void SetupObjects();
	  // Called to create S3DAnimObj's and add to object array
	virtual void Initialize();
      // Initializes the materials
	virtual void Animate(BOOL draw);
	  // Called to upsdate frame state
	virtual BOOL Render();
	  // Called to render a frame
	virtual void RefreshZBuffer();
	  // cause the zbuffer to refresh itself
};

// ********************
// * TWindStripEffect *
// ********************

_CLASSDEF(TWindStripEffect)

class TWindStripEffect : public TEffect
{
  private:
	PTCharacter targets[10];
	int end_of_targets;
	int count;
  public:
	TWindStripEffect(PTObjectImagery newim) : TEffect(newim) { Initialize();}
    TWindStripEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize();}
	virtual ~TWindStripEffect() {}

	virtual void Initialize();
	virtual void Pulse();
};

// **********************
// * WindStrip Animator *
// **********************

// Consists of a white/blue strip w/ a fadeout-alpha gradient

#define WINDSTRIP_MAX_STRIPS	20

_STRUCTDEF(SDropSparkSystem)
struct SDropSparkSystem
{
	D3DVECTOR pos, vel;
	D3DVALUE scl;
	
	BOOL used;
};

_CLASSDEF(TWindStripAnimator)
class TWindStripAnimator : public T3DAnimator
{
protected:
	int total_offset;
	D3DVECTOR front, center;
	D3DVALUE r[WINDSTRIP_MAX_STRIPS], th[WINDSTRIP_MAX_STRIPS], h[WINDSTRIP_MAX_STRIPS],
		ac[WINDSTRIP_MAX_STRIPS], dr[WINDSTRIP_MAX_STRIPS], dth[WINDSTRIP_MAX_STRIPS],
		dh[WINDSTRIP_MAX_STRIPS], dac[WINDSTRIP_MAX_STRIPS], haloscale;
	int frameon, centerang;
	PSDropSparkSystem drops;

public:
	PTStripAnimator	mystrip[WINDSTRIP_MAX_STRIPS];
	
	int subspell;
	D3DVECTOR realpos;

	TWindStripAnimator(PTObjectInstance oi) : T3DAnimator(oi) { for (int i = 0; i < WINDSTRIP_MAX_STRIPS; i++) mystrip[i] = NULL; }
	  // Constructor (initialization handled by Initialize)
    virtual ~TWindStripAnimator() { Close(); }
	  // Call close function

    virtual void SetupObjects();
	  // Called to create S3DAnimObj's and add to object array
	virtual void Initialize();
      // Initializes the materials
	virtual void Animate(BOOL draw);
	  // Called to upsdate frame state
	virtual BOOL Render();
	  // Called to render a frame
	virtual void RefreshZBuffer();
	  // cause the zbuffer to refresh itself
	virtual void InitDrops();
	virtual void AddDrop(D3DVECTOR pos);
};

#endif	// _STRIPEFFECT_H
