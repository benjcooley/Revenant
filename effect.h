// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      effect.h - TEffect module                        *
// *************************************************************************

#ifndef _EFFECT_H
#define _EFFECT_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "effect.h"
#include "object.h"
#include "sound.h"
#include "3dimage.h"
#include "effectcomp.h"
#include "charanimator.h"
#include "spell.h"

BOOL SaveBlendState();
BOOL RestoreBlendState();
BOOL SetBlendState();
BOOL SetAddBlendState();
void DamageCharactersInRange(PTObjectInstance invoker, RS3DPoint pos, int range, int minamount, int maxamount, int type);
void BlastCharactersInRange(PTObjectInstance invoker, RS3DPoint pos, int range, int minamount, int maxamount, int type, int interior_range = 0);
void PulpCharactersInRange(PTObjectInstance invoker, RS3DPoint pos, int range);
void PulpGuy(PTCharacter ch, S3DPoint pos, S3DPoint blast);
void RestoreZ(int x, int y, int width, int height);

_CLASSDEF(TEffect)
_CLASSDEF(TSpellBlock)

// ***********
// * TEffect *
// ***********

// The effect class - individual effects from spells
class TEffect : public TObjectInstance
{
  public:
    TEffect(PTObjectImagery newim) : TObjectInstance(newim) { flags |= OF_IMMOBILE | OF_PULSE; }
    TEffect(PSObjectDef def, PTObjectImagery newim) : TObjectInstance(def, newim) { flags |= OF_IMMOBILE | OF_PULSE; }

    virtual void Pulse();

    void SetSpell(PTSpell sp) { spell = sp; }    
    PTSpell GetSpell() { return spell; }

    void SetSubSpell(int sp) { subspell = sp; }    
    int GetSubSpell() { return subspell; }

    void KillThisEffect();  // Call when effect is finished

    int GetAngle();         // Returns aiming angle

  protected:
    PTSpell spell;      // Spellblock which spawned this effect
    int subspell;           // TEMP TEMP TEMP subspell (psuedo-talisman)
    int angle;              // The direction we are cast
};

// ***************
// * TFireEffect *
// ***************

_CLASSDEF(TFireEffect)

class TFireEffect : public TEffect
{
  public:
    TFireEffect(PTObjectImagery newim) : TEffect(newim) {}
    TFireEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) {}

    virtual void Pulse();
};

// **************
// * TIceEffect *
// **************

#define ICEDIST     256
#define ICERATE     16

#define ICE_FLY     0
#define ICE_IMPACT  1

_CLASSDEF(TIceEffect)

class TIceEffect : public TEffect
{
  public:
    TIceEffect(PTObjectImagery newim) : TEffect(newim) { flags |= OF_MOVING; }
    TIceEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim)
        { pos.y += ICEDIST; pos.x -= ICEDIST; pos.z += ICEDIST; flags |= OF_MOVING; }

    virtual void Pulse();
};

#define LIGHTNING_SOUND     "lightning"

//*// ********************
//*// * TLightningEffect *
//*// ********************
//*
//*_CLASSDEF(TLightningEffect)
//*
//*class TLightningEffect : public TEffect
//*{
//*  public:
//* TLightningEffect(PTObjectImagery newim) : TEffect(newim)
//*     { flags |= OF_MOVING; firsttime = TRUE; SoundPlayer.Mount(LIGHTNING_SOUND); }
//* TLightningEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim)
//*     { flags |= OF_MOVING; firsttime = TRUE; SoundPlayer.Mount(LIGHTNING_SOUND); }
//* virtual ~TLightningEffect() { SoundPlayer.Unmount(LIGHTNING_SOUND); }
//*
//* virtual void Pulse();
//*
//*  protected:     
//* BOOL firsttime;         // flag for the first time through
//*};

// ***************
// * THealEffect *
// ***************

_CLASSDEF(THealEffect)

class THealEffect : public TEffect
{
  private:
    BOOL first_time;
  public:
    int level;

    THealEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    THealEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }

    virtual void Initialize();
    virtual void Pulse();
};

//*// *********************
//*// * TLightning2Effect *
//*// *********************
//*
//*_CLASSDEF(TLightning2Effect)
//*
//*class TLightning2Effect : public TEffect
//*{
//*  public:
//*    int angle;
//*
//*  public:
//*    TLightning2Effect(PTObjectImagery newim) : TEffect(newim) { }
//*    TLightning2Effect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
//*
//* virtual void Pulse();
//*};

// *****************
// * Test Animator *
// *****************

#define NUMPARTICLES 15

_CLASSDEF(TTest3DAnimator)

class TTest3DAnimator : public T3DAnimator
{
  private:
    D3DVECTOR v[NUMPARTICLES];
    D3DVECTOR p[NUMPARTICLES];
    S3DMat mat[NUMPARTICLES];

  public:
    TTest3DAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TTest3DAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
};

// ******************
// * Flare Animator *
// ******************

#define NUMFLARES 15

_CLASSDEF(TFlareAnimator)
class TFlareAnimator : public T3DAnimator
{
  private:
    D3DVECTOR v[NUMFLARES];
    D3DVECTOR p[NUMFLARES];
    S3DMat mat[NUMFLARES];

  public:
    TFlareAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFlareAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
};

// ***************************
// * Pulsating Ball Animator *
// ***************************

#define BALL_MAX_SCALE      10.0f
#define BALL_SCALE_FACTOR    0.1f

_CLASSDEF(TBallAnimator)
class TBallAnimator : public T3DAnimator
{
  private:
    D3DVECTOR p;            // where is it at?
    D3DVALUE scale_factor;  // current scaling measure
    D3DVALUE scale;         // the scale used

  public:
    TBallAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TBallAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
};


// *****************
// * Fire Animator *
// *****************

#define NUMFIRES 15

_CLASSDEF(TFireAnimator)
class TFireAnimator : public T3DAnimator
{
  private:
    D3DVECTOR p[NUMFIRES];
    int f[NUMFIRES];
    S3DMat mat[NUMFIRES];

  public:
    TFireAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFireAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
};

// ****************
// * Ice Animator *
// ****************

#define NUM_ICE_CRYSTALS 10
#define ICE_SCALE_STEP .25

_CLASSDEF(TIceAnimator)
class TIceAnimator : public T3DAnimator
{
  private:
    D3DVECTOR p[NUM_ICE_CRYSTALS];
    D3DVALUE scale[NUM_ICE_CRYSTALS];    // Current scale value of each ice crystal
    int framenum[NUM_ICE_CRYSTALS];      // Frame counters (used for delays & timing)

  public:
    TIceAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TIceAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
};

// *****************
// * Heal Animator *
// *****************

#define NUM_HEAL_BUBBLES  60
#define HEAL_SCALE_STEP   .15
#define HEALING_RADIUS    20
#define HEAL_DURATION     40

_CLASSDEF(THealAnimator)

class THealAnimator : public T3DAnimator
{
  private:
    D3DVECTOR p[NUM_HEAL_BUBBLES];       // Position of each healing bubble
    D3DVALUE scale[NUM_HEAL_BUBBLES];    // Current scale value of each healing bubble
    D3DVALUE rise[NUM_HEAL_BUBBLES];     // Rate at which each bubble rises
    int framenum[NUM_HEAL_BUBBLES];      // Frame counters (used for delays & timing)
    int activebubbles;                   // How many bubbles are currently floating up
    D3DVALUE rotation;
    int heal_num;
    int glow_num;

  public:
    THealAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~THealAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
      // make everything cool
};

//*// **********************
//*// * Lightning Animator *
//*// **********************
//*
//*#define LIGHTNING_DURATION      40
//*#define NUM_LIGHTNING_SEGMENTS  20
//*
//*_CLASSDEF(TLightningAnimator)
//*
//*class TLightningAnimator : public T3DAnimator
//*{
//*  private:
//*    D3DVECTOR p[NUM_LIGHTNING_SEGMENTS];   // Position of each lightning segment
//*    int framenum;                          // Keeps track of time
//*
//*  public:
//*    TLightningAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
//*   // Constructor (initialization handled by Initialize)
//*    virtual ~TLightningAnimator() { Close(); }
//*   // Call close function
//*
//* virtual void Initialize();
//*   // Initializes velocity vectors and positions
//* virtual void Animate(BOOL draw);
//*   // Called to update frame state
//* virtual BOOL Render();
//*   // Called to render a frame
//*};

// *****************************
// * Fountain Sparkle Animator *
// *****************************

#define NUM_FOUNTAIN_BUBBLES  10
#define FOUNTAIN_SCALE_STEP   .15
#define FOUNTAIN_RADIUS       20

_CLASSDEF(TFountainAnimator)

class TFountainAnimator : public T3DAnimator
{
  private:
    D3DVECTOR p[NUM_FOUNTAIN_BUBBLES];       // Position of each fountain bubble
    D3DVALUE scale[NUM_FOUNTAIN_BUBBLES];    // Current scale value of each fountain bubble
    D3DVALUE rise[NUM_FOUNTAIN_BUBBLES];     // Rate at which each bubble rises
    int framenum[NUM_FOUNTAIN_BUBBLES];      // Frame counters (used for delays & timing)

  protected:
    int colorobj;                            // Which object to use for the desired texture color

  public:
    TFountainAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFountainAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void SetColorObject() = 0;
      // Pure virtual function used to set colorobj
    virtual void RefreshZBuffer();
      // Called to update Z buffer information
};

// **************************
// * Cyan Fountain Animator *
// **************************

_CLASSDEF(TCyanFountainAnimator)

class TCyanFountainAnimator : public TFountainAnimator
{
  public:
    TCyanFountainAnimator(PTObjectInstance oi) : TFountainAnimator(oi) {}
      // Constructor (initialization handled by Initialize)

    virtual void SetColorObject() { colorobj = 0; }
};

// *************************
// * Red Fountain Animator *
// *************************

_CLASSDEF(TRedFountainAnimator)

class TRedFountainAnimator : public TFountainAnimator
{
  public:
    TRedFountainAnimator(PTObjectInstance oi) : TFountainAnimator(oi) {}
      // Constructor (initialization handled by Initialize)

    virtual void SetColorObject() { colorobj = 1; }
};

// ***************************
// * Green Fountain Animator *
// ***************************

_CLASSDEF(TGreenFountainAnimator)

class TGreenFountainAnimator : public TFountainAnimator
{
  public:
    TGreenFountainAnimator(PTObjectInstance oi) : TFountainAnimator(oi) {}
      // Constructor (initialization handled by Initialize)

    virtual void SetColorObject() { colorobj = 2; }
};

// **************************
// * Blue Fountain Animator *
// **************************

_CLASSDEF(TBlueFountainAnimator)

class TBlueFountainAnimator : public TFountainAnimator
{
  public:
    TBlueFountainAnimator(PTObjectInstance oi) : TFountainAnimator(oi) {}
      // Constructor (initialization handled by Initialize)

    virtual void SetColorObject() { colorobj = 3; }
};

// *******************
// * Ribbon Animator *
// *******************

#define NUM_RIBBONS              3
#define RIBBON_DURATION          50
#define NUM_RIBBON_SPARKS        30
#define RIBBON_RADIUS            35
#define RIBBON_SPARK_DURATION    24
#define RIBBON_SPARK_SCALE_STEP  0.25

_CLASSDEF(TRibbonAnimator)

class TRibbonAnimator : public T3DAnimator
{
  private:
    D3DVECTOR p[NUM_RIBBON_SPARKS];       // Position of each ribbon spark
    D3DVECTOR v[NUM_RIBBON_SPARKS];       // Movement vector for each ribbon spark
    D3DVALUE scale[NUM_RIBBON_SPARKS];    // Current scale value of each ribbon spark
    int framenum[NUM_RIBBON_SPARKS];      // Frame counters (used for delays & timing)
    D3DVECTOR ribpos;                     // Position of the ribbons
    D3DVALUE rotation[NUM_RIBBONS];       // Current rotation values for each ribbon
    int ribbontimer;                      // Timer for the entire ribbon effect
    int numtexframes;                     // How many frames the texture has
    D3DVALUE ribscale;                    // Scale value for the ribbons
    D3DVALUE centertilt;
    D3DVALUE centertilt_dx;

  public:
    TRibbonAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TRibbonAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
};

// *******************
// * Shield Animator *
// *******************

#define SHIELD_SCALE  2.0

_CLASSDEF(TShieldAnimator)

class TShieldAnimator : public T3DAnimator
{
  private:
    D3DVECTOR pos;     // Position of the sphere
    int framenum;      // Frame counter

  public:
    TShieldAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TShieldAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
};

// ******************
// * Flame Animator *
// ******************

_CLASSDEF(TFlameAnimator)

class TFlameAnimator : public T3DAnimator
{
  private:
    int frame;      // Frame counter
  
  public:
    TFlameAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFlameAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

// ********************
// * SymGlow Animator *
// ********************

_CLASSDEF(TSymGlowAnimator)

class TSymGlowAnimator : public T3DAnimator
{
  private:
    D3DVALUE u;      // Current u offset in the texture
    D3DVALUE zscale;
    D3DVALUE dz;
    int timer;

  public:
    TSymGlowAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TSymGlowAnimator() { Close(); }
      // Call close function

    virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

// *********************
// * Particle Animator *
// *********************

_STRUCTDEF(SParticleParams)
struct SParticleParams
{
    int particles;                  // duh..
    D3DVECTOR pos, pspread, dir, spread; // direction and spread of particles
    D3DVALUE gravity;               // gravity value added each tick to vector
    int trails;                     // number of trailing particles
    int minstart, maxstart;         // minimum start, maximum start delay for particle
    int minlife, maxlife;           // number of trail items, minlife, maxlife
    BOOL bounce;                    // bounce
    BOOL killobj;                   // true if this should kill the object it's made for when done
    DWORD objflags;                 // 1 bit set for each object that should randomly be included

    BOOL seektargets;               // seektargets?
    BOOL seekz;                     // seek on the z value (or FALSE to not change z value)
    int numtargets;                 // number of targets to seek (up to 5)
    D3DVECTOR targetpos[5];         // positions of targets to seek (if numtargets > 0)
    D3DVALUE turnang;               // the turn angle per frame (in radians) for seeking targets
    D3DVALUE seekspeed;                 // speed (vel mag) of seeking particle
    D3DVALUE autorange;             // "radius" where turning stops, just interpolating
    D3DVALUE hitrange;              // "radius" where particle has "hit" target, stop
};

_CLASSDEF(TParticle3DAnimator)
class TParticle3DAnimator : public T3DAnimator
{
  private:
    SParticleParams params;         // Particle parameters
    D3DVECTOR *v;                   // current vectors
    D3DVECTOR *p;                   // current position
    int *l, *s, *o;                 // Current ticks left for life and start, and cur object
    int *ti;                        // ti = target index (0 to numtargets - 1)

  public:
    TParticle3DAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TParticle3DAnimator() { Close(); }
      // Call close function

    virtual void InitParticles(PSParticleParams nparams);
      // Sets up particle animator
    virtual void ResetTargetInfo(SParticleParams nparams);
      // Change particle seekpeed or targets, etc.

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Close();
      // Deletes all objects, etc.
//  virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

};

// *****************************
// * SmokeEffect Animator     *
// *****************************
#define NUMSMOKEBALLS 50
#define SMOKE_GRAV -0.01f

_STRUCTDEF(SSmoke);
struct SSmoke
{
    float x,y,z,size, vx, vy, vz,vsize;
    float rot; 
    int life;
};


// ****************
// * Meteor Storm *
// ****************

_CLASSDEF(TMeteorStormEffect)
class TMeteorStormEffect : public TEffect
{
  public:
    TMeteorStormEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TMeteorStormEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TMeteorStormEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

#define METEOR_STORM_SIZE       20
#define METEOR_STORM_TICKS      100

_CLASSDEF(TMeteorStormAnimator)
class TMeteorStormAnimator : public T3DAnimator
{
  private:
    TStormAnimator meteor_storm;        // the meteor storm
    int ticks;                          // tick, tock --, tick, tock --> see also Mouse, and Clock
    int tracker;                        // counts meteors in flight
  public:
    // construction...
    TMeteorStormAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
    // destruction!
    virtual ~TMeteorStormAnimator() { Close(); }

    virtual void Initialize();          // init stuff
    virtual void Animate(BOOL draw);    // do the animation
    virtual BOOL Render();              // render it to the screen
    virtual void RefreshZBuffer();      // refresh the z buffer
};

// VORTEX is a teleport spell of sorts
/*
_CLASSDEF(TVortexEffect)
class TVortexEffect : public TEffect
{
  public:
    TVortexEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TVortexEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TVortexEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

#define VORTEX_LIFE_SPAN        30

#define VORTEX_STATE_GROW       1
#define VORTEX_STATE_SHRINK     2
#define VORTEX_STATE_CONSTANT   3

#define VORTEX_ALPHA_FACTOR     .03f
#define VORTEX_ALPHA_MAX        1.0f

#define VORTEX_RING             9
#define VORTEX_COUNT            15
#define VORTEX_GLOW_SIDE        16
#define VORTEX_BASE             30
#define VORTEX_VERTEX           (VORTEX_RING * VORTEX_COUNT + VORTEX_BASE)
#define VORTEX_GLOW_COUNT       (VORTEX_RING * 4)

#define VORTEX_SCALE_X_MAX      .6f
#define VORTEX_SCALE_Y_MAX      .6f
#define VORTEX_SCALE_Z_MAX      1.0f
#define VORTEX_ROT_MAX          12.0f

#define VORTEX_PARTICLE_COUNT   150
#define VORTEX_RADIUS           37
#define VORTEX_HEIGHT           150
#define VORTEX_SCALE_POINT      25 

struct vortex_particle_info
{
    D3DVALUE rot;
    D3DVALUE rot_inc;
    D3DVALUE height_inc;
    D3DVALUE face_rot;
    D3DVALUE face_rot_inc;
    D3DVALUE scale;
    D3DVALUE radius;
    BOOL flicker;
};

_CLASSDEF(TVortexAnimator) 
class TVortexAnimator : public T3DAnimator 
{
  private:
    D3DVECTOR pos;          // position of vortex
    D3DVECTOR scale;        // scaling of vortex
    D3DVECTOR rot;          // rotation of vortex
    D3DVALUE alpha_blend;   // blending of alpha
    D3DVECTOR rotation;     // rotation the thing
    int count;              // the counter
    int vortex_state;       // the state of the vortex
    D3DVECTOR scale_factor; // the amount to scale
    D3DVALUE rot_factor;    // the rotation factor
    D3DVALUE rot_speed;     // the rotation speed
    D3DVALUE vertex[VORTEX_VERTEX];// vertex alpha value
    D3DVALUE glow[VORTEX_GLOW_COUNT];// vertex alpha value
    int frame;              // the frame of the turn
    TParticleSystem particle_system;// the particle system
    vortex_particle_info particle_info[VORTEX_PARTICLE_COUNT];// particle stuff
    int phase;
    float fade;
    int level;
  public:
    // constructor
    TVortexAnimator(PTObjectInstance oi) : T3DAnimator(oi), particle_system(VORTEX_PARTICLE_COUNT) {}
    //TVortexAnimator(PTObjectInstance oi);
    // destructor
    virtual ~TVortexAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};
*/
// ******************
// * Handling Flies *
// ******************

_CLASSDEF(TFlyEffect)

class TFlyEffect : public TEffect
{
  public:
    TFlyEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TFlyEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TFlyEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

#define FLY_COUNT       20
#define FLY_RANGE_X     30
#define FLY_RANGE_Y     30
#define FLY_RANGE_Z     30

_CLASSDEF(TFlyAnimator)

class TFlyAnimator : public T3DAnimator
{
  protected:
    TParticleSystem flies;  // the flies    
  public:
    // constructor
    TFlyAnimator(PTObjectInstance oi) : T3DAnimator(oi), flies(FLY_COUNT) {}
    // destructor
    virtual ~TFlyAnimator() { Close(); }

    // init
    virtual void Initialize();
    // animate
    virtual void Animate(BOOL draw);
    // render
    virtual BOOL Render();
    // refresh the z buffer
    virtual void RefreshZBuffer();
};

_CLASSDEF(TFogAnimator)

struct _color_data
{
    float c, a;
};

#define FOG_VERTEX      36
#define FOG_VERTEX_X    6
#define FOG_VERTEX_Y    6

class TFogAnimator : public T3DAnimator
{
  private:
    _color_data color[FOG_VERTEX];  // the color of each vertex
    D3DVECTOR pos[FOG_VERTEX];      // the position of each vertex
    D3DVECTOR dpos[FOG_VERTEX];     // the change in position of each vertex
    D3DVECTOR velocity[FOG_VERTEX]; // the velocity of the fog
    D3DVALUE color_velocity[FOG_VERTEX];// the velocity of the color changing
    D3DVALUE alpha_velocity[FOG_VERTEX];// the velocity of the color changing
  public:
    TFogAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
    virtual ~TFogAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};

// *** PULP GUYS ***
// like destroy them by blowing up their parts

_STRUCTDEF(SBloodDrop)
struct SBloodDrop
{
    D3DVECTOR pos;
    D3DVECTOR scl;
    D3DVECTOR vel;
    BOOL used;
};

_STRUCTDEF(SBloodSplat)
struct SBloodSplat
{
    D3DVECTOR pos;
    D3DVECTOR scl;
    D3DVECTOR rot;
    int frame;
    BOOL used;
};

_STRUCTDEF(SBodyPartPulp)
struct SBodyPartPulp
{
    D3DVECTOR vel;
    D3DVECTOR rot_vel;
    D3DVECTOR pos;
    D3DVECTOR rot;
    D3DVECTOR scl;
    BOOL done;
    int obj_num;
    int object;
};

#define MAX_BODY_PART       5
#define MAX_BLOOD_DROP      (MAX_BODY_PART)// * 30)
#define MAX_BLOOD_SPLAT     (MAX_BODY_PART)// * 3)
#define PULP_GRAVITY        .8f
#define PULP_SLOW_RATE      .45f
#define PULP_SIDE_SLOW      .65f
#define PULP_MISC_SLOW_RATE .75f
#define PULP_ANG_SLOW_RATE  .6f
#define PULP_CUTOFF         .1f

_CLASSDEF(TPulpEffect)
class TPulpEffect : public TEffect
{
  private:
    // pulping info
    PTCharacter character;
    PSBloodDrop blood;
    PSBloodSplat blood_splat;
    PSBodyPartPulp body_part;

    // size info
    int blood_count;
    int blood_splat_count;
    int body_part_count;

    BOOL valid;                     // has this be inited
    int index;                      // a unique id for the character
  public:
    // functions
    TPulpEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TPulpEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TPulpEffect() { if(blood_splat) delete [] blood_splat; if(body_part) delete [] body_part; if(blood) delete [] blood; }

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...

    // specific functions
    PTCharacter GetCharacter()          { return character; }
        // get a pointer to the character
    PSBloodDrop GetBlood(int i)         { if (i < 0 || i >= blood_count) return NULL; else return &blood[i]; }
        // get a pointer to the blood
    PSBloodSplat GetBloodSplat(int i)   { if(i < 0 || i >= blood_splat_count) return NULL; else return &blood_splat[i]; }
        // get a pointer to the blood splat
    PSBodyPartPulp GetBodyPart(int i)   { if(i < 0 || i >= body_part_count) return NULL; else return &body_part[i]; }
        // get a pointer to the body parts
    BOOL IsDone()                       { if(!valid) return TRUE; else return FALSE; }
        // get the status
    void Set(S3DPoint vel, PTCharacter ch, int num_body_part = MAX_BODY_PART, int num_blood = MAX_BLOOD_DROP, int num_splat = MAX_BLOOD_SPLAT);

    virtual void Notify(int notify, void *ptr);
        // notify that our character has been deleted
};

_CLASSDEF(TPulpAnimator)
class TPulpAnimator : public T3DAnimator
{
  public:
    TPulpAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
    virtual ~TPulpAnimator() { Close(); }

        // virtual functions
    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};

// *********************
// * TBurnEffect *
// *********************

_CLASSDEF(TBurnEffect)

class TBurnEffect : public TEffect
{
private:    
    PTCharacter character;
    int frame;
public:
    TBurnEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TBurnEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }

    virtual void Initialize();
    virtual void Pulse();
    PTCharacter GetCharacter()  { return character; }
    void Set(PTCharacter ch);
    virtual void Notify(int notify, void *ptr);
    void ResetFrameCount();
};

#define BURN_COUNT      70

// ***********************
// * Burn Animator *
// ***********************

_CLASSDEF(TBurnAnimator)
class TBurnAnimator : public T3DAnimator
{
  private:
    TParticleSystem fire;
    TParticleSystem smoke;
    int size;
    int frame;
    int to_add;
  public:
    TBurnAnimator(PTObjectInstance oi) : T3DAnimator(oi), fire(BURN_COUNT), smoke(BURN_COUNT) {}
    virtual ~TBurnAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
    virtual void ResetFrameCount() { frame = 0; }
};

// *********************
// * TAuraEffect *
// *********************

_CLASSDEF(TAuraEffect)

class TAuraEffect : public TEffect
{
private:    
    PTCharacter character;
    BOOL first_time;
public:
    TAuraEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TAuraEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }

    virtual void Initialize();
    virtual void Pulse();
    PTCharacter GetCharacter()  { if(spell) return (PTCharacter)spell->GetInvoker(); else return NULL;}
};

#define AURA_COUNT      100

// ***********************
// * Aura Animator *
// ***********************

_CLASSDEF(TAuraAnimator)
class TAuraAnimator : public T3DAnimator
{
  private:
    TParticleSystem fire;
    int size;
    int frame;
    int to_add;
  public:
    TAuraAnimator(PTObjectInstance oi) : T3DAnimator(oi), fire(AURA_COUNT) {}
    virtual ~TAuraAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};

// ***********************
// * TIceBoltEffect *
// ***********************

_CLASSDEF(TIceBoltEffect)

class TIceBoltEffect : public TEffect
{
  private:
  public:
    TIceBoltEffect(PTObjectImagery newim) : TEffect(newim) { }
    TIceBoltEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TIceBoltEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **************************
// * Electric Bolt Animator *
// **************************

//#define NUM_EBOLT_SPARKS  25

_CLASSDEF(TIceBoltAnimator)

/*class TIceBoltAnimator : public T3DAnimator
{
  private:
    int angle;                          // moving at this angle
    D3DVECTOR p[NUM_EBOLT_SPARKS];      // Position of each spark
    D3DVECTOR v[NUM_EBOLT_SPARKS];      // Direction of each spark
    int frame[NUM_EBOLT_SPARKS];        // delay before spark appears
    float size[NUM_EBOLT_SPARKS];       // size of spark
    int initexplode;                    // start exploding!
    int frameon;                        // frame on... (for timing)
    TShockAnimator ring;                // da ring!

  public:
    TIceBoltAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TIceBoltAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
      // make everything cool
    void AddSparks(S3DPoint pos, int angle);
};*/

#define MAX_FROST_PARTICLES     50
#define MAX_SNOW_PARTICLES      50
class TIceBoltAnimator : public T3DAnimator
{
  private:
    int frameon;                        // frame on... (for timing)
    int angle;
    D3DVALUE length,
        spherescale[2],
        cylscale,
        ringout,
        spiralang,
        spiralscale,
        cy;//, sws, rx, ry, rz;
    D3DVECTOR p[MAX_FROST_PARTICLES], v[MAX_FROST_PARTICLES];
    D3DVALUE s[MAX_FROST_PARTICLES], t[MAX_FROST_PARTICLES];
    //PTParticle3DAnimator particleanim;

    D3DVALUE h[MAX_SNOW_PARTICLES], th[MAX_SNOW_PARTICLES], r[MAX_SNOW_PARTICLES],
        rs[MAX_SNOW_PARTICLES], sz[MAX_SNOW_PARTICLES];
    
  public:
    TIceBoltAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TIceBoltAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();    
};

// ***************
// * TIcedEffect *
// ***************

_CLASSDEF(TIcedEffect)

class TIcedEffect : public TEffect
{
  private:
  public:
    TIcedEffect(PTObjectImagery newim) : TEffect(newim) { }
    TIcedEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TIcedEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *****************
// * Iced Animator *
// *****************

_CLASSDEF(TIcedAnimator)

#define MAX_ICED_CHUNKS     30
class TIcedAnimator : public T3DAnimator
{
  private:
    int frameon;                        // frame on... (for timing)
    D3DVALUE angle;
    int donebouncing;
    D3DVECTOR p[MAX_ICED_CHUNKS], v[MAX_ICED_CHUNKS], t[MAX_ICED_CHUNKS], w[MAX_ICED_CHUNKS];
    D3DVECTOR s[MAX_ICED_CHUNKS];
    int l[MAX_ICED_CHUNKS];
    PTObjectInstance icedchar;
    
  public:
    TIcedAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TIcedAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    void InitIced(PTObjectInstance iceme);
      // Tell who to ice!
};

// ********************
// * TQuicksandEffect *
// ********************

_CLASSDEF(TQuicksandEffect)

class TQuicksandEffect : public TEffect
{
  private:
    BOOL first_time;
  public:
    TQuicksandEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TQuicksandEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TQuicksandEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * Quicksand Animator *
// **********************

_CLASSDEF(TQuicksandAnimator)

class TQuicksandAnimator : public T3DAnimator
{
  private:
    D3DVALUE scalesize, cylscale, cylheight, cylrot, cylcount;
    int stage, count;
    PTObjectInstance quicksandchar;
    float ang;
    float zscalefactor;
  protected:
    void SetAnimFrame(int frame_num, PS3DAnimObj obj);
      // Called to change texture frame
    
  public:
    int frameon;                        // frame on... (for timing)
    int z_level;
    float target_rotation;
    int level;
    PTCharacter target[10];
    int num_targets;
    S3DPoint target_position[10];

    TQuicksandAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TQuicksandAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    void InitQuicksand(PTObjectInstance quicksandme, int delay);
      // Tell who to quicksand!
};

// ********************
// * TSandswirlEffect *
// ********************

_CLASSDEF(TSandswirlEffect)

class TSandswirlEffect : public TEffect
{
  private:
  public:
    TSandswirlEffect(PTObjectImagery newim) : TEffect(newim) { }
    TSandswirlEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TSandswirlEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * Sandswirl Animator *
// **********************

_CLASSDEF(TSandswirlAnimator)

class TSandswirlAnimator : public T3DAnimator
{
  private:
    int frameon;                        // frame on... (for timing)
    int angle;
    PTParticle3DAnimator particleanim;
    
  public:
    TSandswirlAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TSandswirlAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

//void SetParticles(PTObjectInstance inst) { particles = inst; }
//PTObjectInstance GetParticles(void) { return particles; }
};

// ********************
// * TTornadoEffect *
// ********************

_CLASSDEF(TTornadoEffect)

class TTornadoEffect : public TEffect
{
  private:
  public:
    TTornadoEffect(PTObjectImagery newim) : TEffect(newim) { }
    TTornadoEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TTornadoEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * Tornado Animator *
// **********************

_CLASSDEF(TTornadoAnimator)

_STRUCTDEF(STornadoParticle)
struct STornadoParticle
{
    D3DVECTOR pos;
    D3DVALUE scl, dscl, th, dth, h, dh, r, dr;
    int count;
};

_STRUCTDEF(SIceChunkParticle)
struct SIceChunkParticle
{
    D3DVECTOR pos, vel, w, th, scl;
    int time, stage;
};

_STRUCTDEF(SFlameParticle)
struct SFlameParticle
{
    D3DVECTOR pos, vel;
    D3DVALUE scl, dscl;
};

class TTornadoAnimator : public T3DAnimator
{
  private:
    int flameon;                        // frame on... (for timing)
    PSTornadoParticle torn;
    PSIceChunkParticle ice;
    PSFlameParticle flame;
        
  public:
    int frameon;
    TTornadoAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TTornadoAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void InitTornadoParticle(int i);
    virtual void InitIceParticle(int i);
    virtual void InitFlameParticle(float _x, float _y, float _z, float _vx, float _vy, float _vz, int sc);
};

// ********************
// * TStreamerEffect *
// ********************

_CLASSDEF(TStreamerEffect)

class TStreamerEffect : public TEffect
{
  private:
    BOOL first_time;
  public:
    TStreamerEffect(PTObjectImagery newim) : TEffect(newim) { Initialize();}
    TStreamerEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize();}
    virtual ~TStreamerEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * Streamer Animator *
// **********************

_CLASSDEF(TStreamerAnimator)

_STRUCTDEF(SStreamerParticle)
struct SStreamerParticle
{
    D3DVECTOR pos;
    D3DVALUE scl;
    int count;
};

#define STREAMER_MAXSTREAMS     4

class TStreamerAnimator : public T3DAnimator
{
  private:
    int frameon;                        // frame on... (for timing)
    D3DVALUE dscl[STREAMER_MAXSTREAMS], scl[STREAMER_MAXSTREAMS], th[STREAMER_MAXSTREAMS],
        dth[STREAMER_MAXSTREAMS], h[STREAMER_MAXSTREAMS], dh[STREAMER_MAXSTREAMS];
    PSStreamerParticle stream[STREAMER_MAXSTREAMS];
        
  public:
    TStreamerAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TStreamerAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void AddStreamer(int num, D3DVECTOR pos, D3DVALUE scl);
      // Add/Update a particle to a trail
    virtual void InitStreamer(int x);
      // Move a trail/Initialize pos/scl
};

// ********************
// * TFireSwarmEffect *
// ********************

_CLASSDEF(TFireSwarmEffect)

class TFireSwarmEffect : public TEffect
{
  private:
  public:
    TFireSwarmEffect(PTObjectImagery newim) : TEffect(newim) { }
    TFireSwarmEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TFireSwarmEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * FireSwarm Animator *
// **********************

_CLASSDEF(TFireSwarmAnimator)

class TFireSwarmAnimator : public T3DAnimator
{
  private:
    int frameon;                        // frame on... (for timing)
    D3DVALUE cylhscl, cylvscl, cylth;
            
  public:
    TFireSwarmAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFireSwarmAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

};

// ***************
// * THaloEffect *
// ***************

_CLASSDEF(THaloEffect)

class THaloEffect : public TEffect
{
  private:
      D3DVALUE halostep;
      int totframes;
  public:
    THaloEffect(PTObjectImagery newim) : TEffect(newim) { }
    THaloEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~THaloEffect() {}

    virtual void Initialize();
    virtual void Pulse();

    virtual void InitParams(int totalframes, float step) { totframes = totalframes; halostep = step; }
    virtual int GetTotalFrames() { return totframes; }
    virtual float GetHaloStep() { return halostep; }
};

// *****************
// * Halo Animator *
// *****************

_CLASSDEF(THaloAnimator)

class THaloAnimator : public T3DAnimator
{
  private:
    D3DVALUE haloscale;
    int frameon;
            
  public:
    THaloAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~THaloAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

// *****************
// * TRippleEffect *
// *****************

_CLASSDEF(TRippleEffect)

class TRippleEffect : public TEffect
{
  private:
     int len;
  public:
    TRippleEffect(PTObjectImagery newim) : TEffect(newim) { }
    TRippleEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TRippleEffect() {}

    virtual void Initialize();
    virtual void Pulse();

    virtual int GetLength() { return len; }
    virtual void SetLength(int length) { len = length; }
};

// *******************
// * Ripple Animator *
// *******************

_STRUCTDEF(SDropParticle)
struct SDropParticle
{
    D3DVECTOR pos;
    D3DVECTOR vel;
    BOOL dead;
};

_CLASSDEF(TRippleAnimator)

class TRippleAnimator : public T3DAnimator
{
  private:
    int length, ripframe, frameon, numdrops;                // frame on... (for timing)
    float scale;
    BOOL hassplashed;

    PSDropParticle drops;

  protected:
    void SetAnimFrame(int frame_num, PS3DAnimObj obj);
      // Called to change texture frame
    
  public:
    TRippleAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TRippleAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void AddNewRipple(int x, int y, int z, int len);
};

// *****************
// * TDripEffect *
// *****************

_CLASSDEF(TDripEffect)

class TDripEffect : public TEffect
{
  private:
    int ripplesize, height, period;
  public:
    TDripEffect(PTObjectImagery newim) : TEffect(newim) {  }
    TDripEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TDripEffect() {}

    virtual void Initialize();
    virtual void Pulse();

    virtual void SetParams(int ri, int he, int pe) { ripplesize = ri; height = he; period = pe; };
    virtual void GetParams(int* ri, int* he, int* pe) { *ri = ripplesize; *he = height; *pe = period; };

    virtual void Load(RTInputStream is, int version, int objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector
};

// *******************
// * Drip Animator *
// *******************

_CLASSDEF(TDripAnimator)

class TDripAnimator : public T3DAnimator
{
  private:
    D3DVECTOR pos;
    D3DVECTOR vel;
    BOOL dead;
    int time, ripplesize, height, period;

  protected:
    
  public:
    TDripAnimator(PTObjectInstance oi) : T3DAnimator(oi) { ripplesize = 64; height = 128; period = 48;}
      // Constructor (initialization handled by Initialize)
    virtual ~TDripAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void AddNewRipple(int x, int y, int z, int len);
};

// ********************
// * TFaultFireEffect *
// ********************

_CLASSDEF(TFaultFireEffect)

class TFaultFireEffect : public TEffect
{
  private:
  public:
    TFaultFireEffect(PTObjectImagery newim) : TEffect(newim) { }
    TFaultFireEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TFaultFireEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// **********************
// * FaultFire Animator *
// **********************

#define FF_GROUPS   10

_CLASSDEF(TFaultFireAnimator)

//static BOOL ff_init = FALSE;
class TFaultFireAnimator : public T3DAnimator
{
  private:
    float th, tvs[4];
    //static float per[FF_GROUPS], phase[FF_GROUPS];
  public:
    TFaultFireAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TFaultFireAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void SetupObjects();
};

// *****************
// * TBloodEffect *
// *****************

_CLASSDEF(TBloodEffect)

class TBloodEffect : public TEffect
{
  private:
    int height, hangle, vangle, hspread, vspread, num;
  public:
    TBloodEffect(PTObjectImagery newim) : TEffect(newim) {  }
    TBloodEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TBloodEffect() {}

    virtual void OffScreen() { KillThisEffect(); }

    virtual void Initialize();
    virtual void Pulse();

    virtual void SetParams(int he, int ha, int va, int hs, int vs, int nu) { height = he; hangle = ha; vangle = va; hspread = hs, vspread = vs; num = nu; }
    virtual void GetParams(int *he, int *ha, int *va, int *hs, int *vs, int *nu) { *he = height; *ha = hangle; *va = vangle; *hs = hspread; *vs = vspread; *nu = num; }
};

// *******************
// * Blood Animator *
// *******************

_CLASSDEF(TBloodAnimator)

class TBloodAnimator : public T3DAnimator
{
  private:
    TBloodSystem bloods;

  protected:
    
  public:
    TBloodAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TBloodAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

// *****************
// * TMistEffect *
// *****************

_CLASSDEF(TMistEffect)

class TMistEffect : public TEffect
{
  private:
    
  public:
    TMistEffect(PTObjectImagery newim) : TEffect(newim) {  }
    TMistEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TMistEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *******************
// * Mist Animator *
// *******************

_CLASSDEF(TMistAnimator)

class TMistAnimator : public T3DAnimator
{
  private:
    PSDropParticle drops;
    int numdrops;   

  protected:
    
  public:
    TMistAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TMistAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void AddNewRipple(int x, int y, int z, int len);
};

// *****************
// * TWaterFallEffect *
// *****************

_CLASSDEF(TWaterFallEffect)

class TWaterFallEffect : public TEffect
{
  private:
    
  public:
    TWaterFallEffect(PTObjectImagery newim) : TEffect(newim) {  }
    TWaterFallEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TWaterFallEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *******************
// * WaterFall Animator *
// *******************

_STRUCTDEF(SWaterParticle)
struct SWaterParticle
{
    D3DVECTOR pos;
    D3DVECTOR vel;
    D3DVECTOR scale;
    int time;
};

_CLASSDEF(TWaterFallAnimator)
class TWaterFallAnimator : public T3DAnimator
{
  private:
    PSWaterParticle drops;
    int numdrops;
    S3DPoint eff;

  protected:
    
  public:
    TWaterFallAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TWaterFallAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void InitParticle(int i);
    virtual void UpdateStuff();
    virtual void DoLighting(float x, float y, float z, PS3DAnimObj object);
};

// *****************
// * TWaterEffect *
// *****************

_CLASSDEF(TWaterEffect)

class TWaterEffect : public TEffect
{
  private:
    
  public:
    TWaterEffect(PTObjectImagery newim) : TEffect(newim) {  }
    TWaterEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TWaterEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *******************
// * Water Animator *
// *******************

_CLASSDEF(TWaterAnimator)
class TWaterAnimator : public T3DAnimator
{
  private:
    PSWaterParticle drops;
    int numdrops;
    S3DPoint eff;

  protected:
    
  public:
    TWaterAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TWaterAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

    virtual void InitParticle(int i);
    virtual void UpdateStuff();
    virtual void DoLighting(float x, float y, float z, PS3DAnimObj object);
};

// *****************
// * TPixieEffect *
// *****************

_CLASSDEF(TPixieEffect)

class TPixieEffect : public TEffect
{
  private:
    
  public:
    TPixieEffect(PTObjectImagery newim) : TEffect(newim) {  }
    TPixieEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    virtual ~TPixieEffect() {}

    virtual void Initialize();
    virtual void Pulse();
};

// *******************
// * Pixie Animator *
// *******************

_CLASSDEF(TPixieAnimator)
class TPixieAnimator : public T3DAnimator
{
  private:
    PSWaterParticle pix;
    int charnear;
    S3DPoint origpos;

  protected:
    
  public:
    TPixieAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
      // Constructor (initialization handled by Initialize)
    virtual ~TPixieAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();

};

// *** Ambient Sound Effect...

#define SOUNDNAME_LEN       (32)

_CLASSDEF(TAmbSoundEffect)

class TAmbSoundEffect : public TEffect
{
  protected:
    char sound_name[SOUNDNAME_LEN]; // the name of the sound effect
    int id;                         // the sound's id
    int sample_length;              // the number of frames (FRAMES, PEOPLE, ***FRAMES***) in the sound sample
    int ticks;                      // our current tick count...

  public:
    TAmbSoundEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TAmbSoundEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TAmbSoundEffect() { if (id > 0) SoundPlayer.Unmount(id); }

    virtual void Load(RTInputStream is, int version, int objversion);
        // Loads object data from the sector
    virtual void Save(RTOutputStream os);
        // Saves object data to the sector
    virtual void Initialize();
    virtual void Pulse();

    virtual void GetParams(char *name, int *len) { strcpy(name, sound_name); *len = sample_length; }
        // gets the current settings for this speaker object
    virtual void SetParams(char *name, int len);
        // sets the sound name and length of that sample... used by the editor
};


// *** Ambient Sound Animator...

_CLASSDEF(TAmbSoundAnimator)
class TAmbSoundAnimator : public T3DAnimator
{
  public:
    TAmbSoundAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void Initialize();
    void Animate(BOOL);
    BOOL Render();
    void RefreshZBuffer();
};



#endif // _EFFECT_H
