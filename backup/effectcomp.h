// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               effectcomp.h - Effect component module                  *
// *************************************************************************

#ifndef _EFFECTCOMP_H
#define _EFFECTCOMP_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#include "object.h"
#include "sound.h"
#include "3dimage.h"

// *********************************************************
// * Storm Animator - Use for Meteor, Ice, Fire, Whatever! *
// *********************************************************

#define STORM_DEFAULT_MAX_INSTANCE      20

_STRUCTDEF(SStormParams)
struct SStormParams
{
    int particles;          // change the number of particles allowed
    int tex_u;              // the texture u size, for refreshing
    int tex_v;              // the texture v size, for refreshing

    int particle_u;         // the particle frame's u size
    int particle_v;         // the particle frame's v size
    int particle_begin;     // the particle's beginning frame
    int particle_end;       // the particle's ending frame

    int impact_u;           // the impact frame's u size
    int impact_v;           // the impact frame's v size
    int impact_begin;       // the impact's beginning frame
    int impact_end;         // the impact's ending frame

    float gravity;          // the force due to weight
    D3DVECTOR velocity;     // vectored velocity

    D3DVECTOR pos;          // the creation position base
    D3DVECTOR pos_spread;   // spread them around, random value

    float impact_frame_inc; // how much to increment the impact frame
    float particle_frame_inc;// how much to increment the particle frame

    D3DVECTOR particle_scale;// how big to scale it
    D3DVECTOR impact_scale; // how big to scale it
    D3DVECTOR rot;          // value for rotation
};


#define METEOR_DAMAGE_MAX 15
#define METEOR_DAMAGE_MIN 5

_STRUCTDEF(SStormInstance)
struct SStormInstance
{
    BOOL used;              // is this thing in use
    D3DVECTOR pos;          // the position of the instance
    BOOL is_particle;       // is this thing a particle or an impact
    BOOL explosion_sounded;  // has the explosion sound been played
    D3DVECTOR velocity;     // the velocity
    D3DVECTOR part_scl;     // scaling for individual meteor particles
    D3DVECTOR expl_scl;     // scaling for individual meteor explosions
    float gravity;          // gravity
    float frame;            // the frame count
    float impact_frame_inc; // how much to increment the impact frame
    float particle_frame_inc;// how much to increment the particle frame
};

_CLASSDEF(TStormAnimator)
class TStormAnimator
{
  protected:
    PT3DAnimator animator;          // pointer to the animator
    PS3DAnimObj obj;                // pointer to a 3D object
    SStormParams params;            // the storm parameters
    PSStormInstance storm_instance; // the instance stuff
    int size;                       // the size of the instance array

  protected:
    virtual void Create();          // create new particles if necessary
    virtual int GetCount();         // create a counter
  public:
    // my constructor
    TStormAnimator(const int& instance_size = STORM_DEFAULT_MAX_INSTANCE)
    { storm_instance = new SStormInstance[instance_size]; size = instance_size; }
    // my destructor
    virtual ~TStormAnimator()
    { delete [] storm_instance; }

    virtual void Init(PT3DAnimator anim, PS3DAnimObj o);    // init, must be called
    virtual void Set(PSStormParams nparams);                // set the parameters
    virtual void Get(PSStormParams nparams);                // get the parameters
    virtual void Animate();                                 // animate the storm effect
    virtual void Render();                                  // render the storm effect
    virtual void RefreshZBuffer();                          // refresh the storm effect
    virtual BOOL IsDone()                                   // check to see if effect is done
    { if(!GetCount()) return TRUE; else return FALSE; }
};

// **********************************
// * The SubParticle Animator Class *
// **********************************

#define SUBPARTICLE_MAX_PARTICLE    20

_STRUCTDEF(SSubParticleParams)
struct SSubParticleParams
{
    int particles;          // the amount of particles on the screen
    int chance;             // the chance of a new particle being created % wise

    D3DVECTOR pos;          // the base pos value
    D3DVECTOR pos_spread;   // the random portion of a generated pos

    D3DVECTOR scale;        // the base scale value
    D3DVECTOR scale_dec;    // how much scale is decremented each time
    D3DVECTOR scale_spread; // the random portion of a generated scale value
    // the initial scale value = (scale) + (random(0, scale_spread))

    D3DVECTOR velocity;     // the base velocity value
    D3DVECTOR velocity_dir; // -val for only - signs, 0 for both signs, +val for only + signs
    D3DVECTOR velocity_spread;// the random portion of a generated velocity value
    // the initial velocity = (velocity) + (random(0, velocity_spread))

    float gravity;          // how the particles are affected by gravity
    
    int min_life;           // minimum life span in frames
    int max_life;           // maximum life span in frames
    // life = random(min_life, max_life)

    BOOL flicker;           // can it flicker
    float flicker_size;     // how much does it flicker
};

_STRUCTDEF(SParticle)
struct SParticle
{
    BOOL used;              // is this particle used
    D3DVECTOR pos;          // position of particle
    D3DVECTOR velocity;     // velocity of particle
    D3DVECTOR scale;        // scale of particle
    D3DVECTOR scale_dec;    // the decrementor!
    float gravity;          // how gravity affects the particle
    int life;               // the lifespan in frames of the particle
    BOOL flicker;           // can it flicker
    BOOL flicker_status;    // is it flickering?
    float flicker_size;     // how much does it flicker 
};

_CLASSDEF(TSubParticleAnimator)

class TSubParticleAnimator
{
  private:
    SSubParticleParams params;  // the parameters to use when creating a new particle
    SParticle *particle;        // the list of particles
    int max_particles;          // the total number of particles
    PT3DAnimator animator;      // animator
    PS3DAnimObj obj;            // 3d object
  protected:
  public:
    // constructor
    TSubParticleAnimator(int max_subparticles = SUBPARTICLE_MAX_PARTICLE)
        {  max_particles = max_subparticles; particle = new SParticle[max_particles]; }
    // destructor
    virtual ~TSubParticleAnimator() { delete [] particle;}

    // set the parameters
    virtual void Set(PSSubParticleParams nparams);
    virtual void Get(PSSubParticleParams nparams);

    // animate the existing particles
    virtual void Animate();

    // render the particles
    virtual void Render();
    // refresh their zbuffers
    virtual void RefreshZBuffer(int tex_u, int tex_v);

    // find the total number of active particles
    int GetCount();

    // create a particle
    void Create();

    // init the particle parameters
    virtual void Init(PT3DAnimator anim, PS3DAnimObj o);
};

// ************************
// * A Shockwave Animator *
// ************************

#define SHOCKWAVE_GROW      1
#define SHOCKWAVE_SHRINK    2

#define SHOCKWAVE_FLAG_SHRINK       (1 << 0)
#define SHOCKWAVE_FLAG_FADE         (1 << 1)
#define SHOCKWAVE_FLAG_START_SHRINK (1 << 2)


_STRUCTDEF(SShockParam)
struct SShockParam
{
    int flags;              // the flags for the shockwave
    D3DVECTOR pos;          // the center of the shockwave
    D3DVECTOR rot;          // how is it rotated
    D3DVECTOR scale;        // what is the scaling
    D3DVECTOR scale_factor; // how much is it scaled each time
    D3DVECTOR shrink_factor;// how fast will it shrink
    D3DVECTOR max_size;     // how big can it get
    D3DVECTOR min_size;     // how small can it get
};

_CLASSDEF(TShockAnimator)

class TShockAnimator
{
  private:
    PT3DAnimator animator;          // 3d animator
    PS3DAnimObj obj;                // the object 
    SShockParam params;             // describe the shockwave
    int ring_count;                 // the number of rings
    int vertex_count;               // the number of vertices
    D3DCOLOR *ring;                 // the rings
    D3DVECTOR init_scale;           // the original size of the scale
    BOOL done;                      // is it done animating
    int grow;                       // is it growing
  public:
    TShockAnimator()                // constructor
    { done = TRUE; }
    virtual ~TShockAnimator()       // destructor
    { delete [] ring; }

    virtual void Set(PSShockParam nparams); // set the parameters
    virtual void Init(PT3DAnimator anim, PS3DAnimObj o, int rings, int vertices);// init the animator
    virtual void Animate();                 // animate the shockwave
    virtual void RefreshZBuffer(int tex_u, int tex_v);// refresh the zbuffer
    virtual void Render();                  // render the shockwave
    BOOL IsDone()                           // check to see if the shockwave is done
    { return done; }
    void SetRingColor(int ring_num, D3DCOLOR color)
    { ring[ring_num] = color; }
};

_CLASSDEF(TStripAnimator)
class TStripAnimator
{
    public:
    PS3DAnimObj obj;
    int pbeg, pend;     // The beginning and end points
    int startsize;      // How thick is the strip at the origin
    int endsize;        // How thick is the strip at the end
    int maxpoints;      // Maximum number of points we can have
    int curpoints;      // Current number of points we have
    D3DVECTOR *points;  // The points themselves
    D3DVALUE ufrequency;    // Texture U Incement added for each point
    D3DVALUE uoff;          // Texture U Offset for strip
    D3DVALUE ubeg;          // Texture U beginning range
    D3DVALUE uend;          // Texture U ending range

    float myx[8];
    float myz[8];

    TStripAnimator( PS3DAnimObj myobj, int maxp);
        // Build strip with maximum number of points

    PS3DAnimObj GetStripObj() { return obj; };
        // Returns pointer to obj.  Why?  I'm not sure

    void GenerateStrip( int angle);
        // takes current point list and sets verts and faces in obj

    void AddPoint( D3DVECTOR *p);
        // Adds point to head of point array, moves array 1, and

    void DelEndPoint();
        // Deletes point from the end of the point array

    void DelStartPoint();
        // Deletes point from the start of the point array

    void AdvanceStrip();
        // Shifts the points once away from the origin

    void ReverseStrip();
        // Shifts the points once toward the origin

    void SetTextureRange( D3DVALUE startu, D3DVALUE endu);
        // Which texture map to use

    void ScrollTexture( D3DVALUE deltau);
        // Scroll Texture base by deltau

    void SetWidth( int start, int end);
        // Which texture map to use

    void AddUpdateRects( LPD3DRECT drawextents);
        // Called after outside function calls RenderObject()

    D3DVECTOR& operator [] (int i)
        { return points[ (i + pbeg) >= maxpoints ? (i + pbeg - maxpoints) : (i + pbeg)]; }
        // Fancy way of saying points[i]
};

_STRUCTDEF(SParticleSystemInfo)
struct SParticleSystemInfo
{
    D3DVECTOR pos;
    D3DVECTOR scl;
    D3DVECTOR rot;
    D3DVECTOR acc;
    D3DVECTOR vel;

    D3DVECTOR temp;

    BOOL flicker;

    int life;
    int life_span;

    BOOL used;
};

_CLASSDEF(TParticleSystem)

class TParticleSystem
{
  private:
    PSParticleSystemInfo particle;      // information about the particles
    PT3DAnimator animator;              // pointer to the animator used
    PS3DAnimObj object;                 // pointer to the object
    S3DPoint size;                      // the size for zbuffer refresh
    int count;                          // the number of particles in the system
    float facing;                       // the facing of the thing
    BOOL check_move;                    // is this thing supposed to move
  public:
    // constructor
    TParticleSystem(int maxsize)    { particle = new SParticleSystemInfo[maxsize]; for(int i = 0; i < maxsize; ++i) particle[i].used = FALSE; count = maxsize; }
    // destructor
    virtual ~TParticleSystem()      { delete [] particle; }

    // our basic functions
    virtual void Init(PT3DAnimator a, PS3DAnimObj o, S3DPoint s, BOOL move = FALSE, float facing_angle = 0.0f);
    virtual void Animate();
    virtual void Render(BOOL flicker = FALSE, BOOL abs_pos = FALSE);
    virtual void RefreshZBuffer();
    virtual void Add(PSParticleSystemInfo p);

    // an accessor or two for good measure
    PSParticleSystemInfo Get(int i) { if(i >= count || i < 0) return NULL; else return &particle[i]; }
};

_STRUCTDEF(SBloodParticle)
struct SBloodParticle
{
    D3DVECTOR vel, pos;
    float scl;
    BOOL used;
    BYTE size, stage, count, delay;
};

_STRUCTDEF(SBloodSystemParams)
struct SBloodSystemParams
{
    PT3DAnimator a;
    PS3DAnimObj s, m, b, sp, s2, m2, b2, sp2;
    int maxsize, num, height, hangle, vangle, hspread, vspread;
    S3DPoint zbuf, effectpos;
};

#define MAX_BLOODS      30

_CLASSDEF(TBloodSystem)
class TBloodSystem
{
  private:
    PSBloodParticle blood;              // blood particles
    PT3DAnimator animator;              // animator pointer
    PS3DAnimObj sml, med, big, spl, sml2, med2, big2, spl2;// objects (small, med, or large--would you like fries?)
    S3DPoint size, eff;
    BOOL done;
    float height, bifscale;//facing, height;
    int bifon;

  public:
    TBloodSystem() { blood = new SBloodParticle[MAX_BLOODS]; for (int i = 0; i < MAX_BLOODS; i++) blood[i].used = 0; };
    virtual ~TBloodSystem()     { delete [] blood; }

    // our basic functions
                                // size = 0, 1, or 2
    virtual void Init(SBloodSystemParams me);
    virtual void Animate();
    virtual void Render();
    virtual void RefreshZBuffer();

    virtual void DoLighting(float x, float y, float z, PS3DAnimObj object);

    virtual BOOL GetDone() { return done; }
};

#endif