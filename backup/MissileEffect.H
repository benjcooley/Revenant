// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               missilerffect.h - TMissileEffect module                 *
// *************************************************************************

#ifndef _MISSILEEFFECT_H
#define _MISSILEEFFECT_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "object.h"
#include "effect.h"
#include "effectcomp.h"
#include "3dimage.h"
#include "mappane.h"

// ******************
// * TMissileEffect *
// ******************

enum {
	MISSILE_LAUNCH  = 0,
	MISSILE_FLY     = 1,
	MISSILE_EXPLODE = 2
};

#define MISSILE_SPEED	16
#define MISSILE_RANGE	2

_CLASSDEF(TMissileEffect)

class TMissileEffect : public TEffect
{
  protected:
    int range;
    int speed;
	BOOL status;

  public:
    TMissileEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TMissileEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }

    virtual void Initialize();

	virtual void Pulse();

	void OffScreen();	// MissileEffects stay on-screen until the effect is finished

    int GetSpeed();					// Returns speed / ROLLOVER
    void SetSpeed(int newspeed);	// Speed will be multiplied by ROLLOVER

	void SetStatus(BOOL newstatus)
	{ status = newstatus; }

	int GetMissileSpeed() { return speed; }
};

// *****************
// * TPhotonEffect *
// *****************

#define PHOTON_SPEED 16

_CLASSDEF(TPhotonEffect)

class TPhotonEffect : public TMissileEffect
{
    protected:
    BOOL firsttime;

	public:
	TPhotonEffect(PTObjectImagery newim) : TMissileEffect(newim) { Initialize(); }
	TPhotonEffect(PSObjectDef def, PTObjectImagery newim) : TMissileEffect(def, newim) { Initialize(); }
	virtual ~TPhotonEffect() { SoundPlayer.Unmount(LIGHTNING_SOUND); }

    virtual void Initialize();

	virtual void Pulse();
};

// *******************
// * Photon Animator *
// *******************

#define NUM_PHOTON_SPARKS 16
#define PHOTON_LAUNCH_RADIUS 20
#define PHOTON_LAUNCH_DURATION 25
#define PHOTON_LAUNCH_SCALE_STEP .25

_CLASSDEF(TPhotonAnimator)
class TPhotonAnimator : public T3DAnimator
{
  private:
    D3DVECTOR p[NUM_PHOTON_SPARKS * 2];    // Position of each of the photon sparks
    D3DVECTOR v[NUM_PHOTON_SPARKS * 2];    // Direction vectors for each photon sparks
    D3DVALUE scale[NUM_PHOTON_SPARKS * 2]; // Current scale value of each photon spark
    int framenum[NUM_PHOTON_SPARKS * 2];   // Frame counters (used for delays & timing)
    int activesparks;                      // How many sparks are active (when it reaches 0, launch the photon)
    int numtexframes;                      // How many frames the texture has
    int oldstate;		// Used to keep track of state changes

  public:
    TPhotonAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
	  // Constructor (initialization handled by Initialize)
    virtual ~TPhotonAnimator() { Close(); }
	  // Call close function

	virtual void Initialize();
      // Initializes the materials
	virtual void Animate(BOOL draw);
	  // Called to update frame state
	virtual BOOL Render();
	  // Called to render a frame
	virtual void RefreshZBuffer();
	  // Called to refresh the Z Buffer
};

// *********************************
// * The FireBall Effect Class *
// *********************************

#define FIREBALL_SPEED			8

_CLASSDEF(TFireBallEffect)

class TFireBallEffect : public TMissileEffect
{
  private:
	BOOL fireball_done;
  public:
    TFireBallEffect(PTObjectImagery newim) : TMissileEffect(newim) { Initialize(); }
    TFireBallEffect(PSObjectDef def, PTObjectImagery newim) : TMissileEffect(def, newim) { Initialize(); }
	virtual ~TFireBallEffect() {}

    virtual void Initialize();

	virtual void Pulse();

	void SetStatus(BOOL status)	{ fireball_done = status; }
};

// ***********************************
// * The Fireball Animator Class *
// ***********************************

#define FIREBALL_GROW_RATE		0.05f
#define FIREBALL_MAX_SIZE		.60f

#define FIREBALL_TEXTURE_U		64
#define FIREBALL_TEXTURE_V		64
#define FIREBALL_SPARK_TEXTURE_U	32
#define FIREBALL_SPARK_TEXTURE_V	32

#define FIREBALL_MAX_FRAME		16
#define FIREBALL_GLOW_FRAME		3

#define FIREBALL_TRAIL_SCALE	.85f
#define FIREBALL_TRAIL_NUM		10

#define FIREBALL_MAX_SPARK		40
#define FIREBALL_NORM_SPARK		(int((float)FIREBALL_MAX_SPARK * .75f))

// rem this if no white fade is used
//#define FIREBALL_WHITE_FADE

// rem this if you don't want to skip every other trail
//#define FIREBALL_SPACEOUT

#ifdef FIREBALL_SPACEOUT
	#define FIREBALL_TRAIL_SIZE		(FIREBALL_TRAIL_NUM * 2)
#else
	#define FIREBALL_TRAIL_SIZE		(FIREBALL_TRAIL_NUM)
#endif

#define FIREBALL_MAX_BURST		10

#define FIREBALL_DAMAGE_MIN		10		// CHANGE THESE !
#define FIREBALL_DAMAGE_MAX		25

struct FireBallData
{
	D3DVECTOR pos;
	float scale;
	int rotation;
	float frame;
	float glow;
	int used;
};

_CLASSDEF(TFireBallAnimator)

class TFireBallAnimator : public T3DAnimator
{
  private:
	FireBallData fireball;		// data about the fireball
	FireBallData trail[FIREBALL_TRAIL_SIZE]; // the trail size
	int next_state;				// fireball states
	int frame_count;			// the number of textures
	int glow_frame;				// the glow frame
	TSubParticleAnimator spark;	// da sparks!
	TShockAnimator ring;		// da ring!
	int fireball_angle;			// the angle that we are shooting
	int explode;				// the explode tag! 0 = pre-explode, 1 = first explode frame, -1 = post-first explode rame
	FireBallData burst[FIREBALL_MAX_BURST];	// the maximum number of bursts
	int firsttime;				// first time flag
	int old_state;				// the old state
  protected:
	// render the main fireball
	void RenderFireBall();
	// render the fire ball burst
	void RenderFireBallBurst();
	// render the trail
	void RenderFireBallTrail();
	// render the glow
	void RenderFireBallGlow();
	// setanimframe
	void SetAnimFrame(int frame_num, PS3DAnimObj obj);
	// check for a trail
	int IsTrail();
  public:
	// another boring constructor
	TFireBallAnimator(PTObjectInstance oi) : T3DAnimator(oi), spark(FIREBALL_MAX_SPARK) { fireball_angle = ((PTEffect)inst)->GetAngle(); }	
	// another boring virtual destructor
	virtual ~TFireBallAnimator()	{ Close(); }

	virtual void Initialize();
	// Initialize the fireball :)
	virtual void Animate(BOOL draw);
	// animate the fireball
	virtual BOOL Render();
	// render the fireball
	virtual void RefreshZBuffer();
	// overwrite the refresh function
};

// *********************
// * TFireColumnEffect *
// *********************

#define FIRECOLUMN_SPEED 0

_CLASSDEF(TFireColumnEffect)
class TFireColumnEffect : public TMissileEffect
{
	protected:
    BOOL firsttime;

	public:
	TFireColumnEffect(PTObjectImagery newim) : TMissileEffect(newim) { Initialize(); }
	TFireColumnEffect(PSObjectDef def, PTObjectImagery newim) : TMissileEffect(def, newim) { Initialize(); }
	virtual ~TFireColumnEffect() { /*SoundPlayer.Unmount(LIGHTNING_SOUND);*/ }

    virtual void Initialize();

	virtual void Pulse();
};

// ***********************
// * FireColumn Animator *
// ***********************

#define NUM_FIRE_COLUMNS 15

_CLASSDEF(TFireColumnAnimator)
class TFireColumnAnimator : public T3DAnimator
{
  private:
	S3DPoint vctr;	// How much to offset each point

    D3DVECTOR p[NUM_FIRE_COLUMNS];	// Position of each of the fire sparks
    int framenum[NUM_FIRE_COLUMNS];	// Frame counters (used for delays & timing)
    D3DVALUE rotation[NUM_FIRE_COLUMNS];  // Current rotation values for each fireball
    int scale[NUM_FIRE_COLUMNS];
    int numtexframes;	// How many frames the texture has
    int activesparks;	// How many sparks are active (when it reaches 0, launch the photon)
    int oldstate;		// Used to keep track of state changes

  public:
    TFireColumnAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
	  // Constructor (initialization handled by Initialize)
    virtual ~TFireColumnAnimator() { Close(); }
	  // Call close function

	virtual void Initialize();
      // Initializes the materials
	virtual void Animate(BOOL draw);
	  // Called to update frame state
	virtual BOOL Render();
	  // Called to render a frame
	void OffsetPoint( int pnt);
};

// ********************
// * TFlameDiscEffect *
// ********************

#define FLAMEDISC_SPEED 16 // 2
#define FLAMEDISC_LAUNCH_RADIUS	20

_CLASSDEF(TFlameDiscEffect)

class TFlameDiscEffect : public TMissileEffect
{
    protected:
    bool firsttime;

	public:
	TFlameDiscEffect(PTObjectImagery newim) : TMissileEffect(newim) { Initialize(); }
	TFlameDiscEffect(PSObjectDef def, PTObjectImagery newim) : TMissileEffect(def, newim) { Initialize(); }
	virtual ~TFlameDiscEffect() { SoundPlayer.Unmount(LIGHTNING_SOUND); }

    virtual void Initialize();

	virtual void Pulse();
};

// **********************
// * FlameDisc Animator *
// **********************

#define NUM_FLAME_SPARKS 5

_CLASSDEF(TFlameDiscAnimator)

class TFlameDiscAnimator : public T3DAnimator
{
  private:
    D3DVECTOR p[NUM_FLAME_SPARKS+1];		// Position of each of the sparks
    D3DVECTOR v[NUM_FLAME_SPARKS+1];		// Direction vectors for each spark
    D3DVALUE scale[NUM_FLAME_SPARKS+1];	// Current scale value of each object
    int framenum[NUM_FLAME_SPARKS+1];	// Frame counters (used for delays & timing)
    int numtexframes;	// How many frames the texture has
    int activesparks;	// How many sparks are active
    int oldstate;		// Used to keep track of state changes
	int	angle;			// Player's Facing

  public:
    TFlameDiscAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
	  // Constructor (initialization handled by Initialize)
    virtual ~TFlameDiscAnimator() { Close(); }
	  // Call close function

	virtual void Initialize();
      // Initializes the materials
	virtual void Animate(BOOL draw);
	  // Called to update frame state
	virtual BOOL Render();
};

#endif	// _MISSILEEFFECT_H
