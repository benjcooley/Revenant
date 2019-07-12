// *************************************************************************
// *                           Cinematix EXILE                             *
// *                    Copyright (C) 1996 Cinematix                       *
// *              MissileEffect.cpp - TMissileEffect module                *
// *************************************************************************

#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>
#include <math.h>
#include "d3dmacs.h"
#include "d3dmath.h"
#include "character.h"

#include "revenant.h"
#include "missileeffect.h"
#include "effect.h"
#include "character.h"

// ******************
// * TMissileEffect *
// ******************

void TMissileEffect::Initialize()
{
    SetState(MISSILE_LAUNCH);
    angle = 0;
    range = 32768;

    SetSpeed( 16);

    status = FALSE;
}

int TMissileEffect::GetSpeed()
{
    return speed / ROLLOVER;
}

void TMissileEffect::SetSpeed(int newspeed)
{
    speed = newspeed * ROLLOVER;
}

void TMissileEffect::Pulse()
{
    DWORD bits = Move();

    switch(state)
    {
        case MISSILE_LAUNCH :
        {
            if (status)
            {
                if (speed)
                {
                    ConvertToVector(GetAngle(), speed, vel); 
                    flags = (flags & ~OF_IMMOBILE) | OF_MOVING | OF_WEIGHTLESS;
                    vel.z = (speed / -16);

                    range = (240 * MISSILE_RANGE) / (speed / ROLLOVER);
                }
                SetState( MISSILE_FLY);
            }
            break;
        }

        case MISSILE_FLY :
        {
            BOOL explode = FALSE;

            range--;
            if (range <= 0)
                explode = TRUE;

            // See if we hit the ground
            if (bits & MOVE_BLOCKED)
                explode = TRUE;
            else
            {
                PTCharacter invoker;
                if (spell)
                    invoker = (PTCharacter)spell->GetInvoker();
                else
                    invoker = NULL;

                // See if we've hit a character
                for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
                {
                    PTCharacter chr = (PTCharacter)i.Item();

                    if (chr == invoker)
                        continue;

                    if (chr->IsDead())
                        continue;

                    if (this->Distance(chr) > 32)
                        continue;

                    if (invoker && !invoker->IsEnemy(chr))
                        continue;   // We can't hurt our friends
                    
                    explode = TRUE;
                    break;
                }
            }

            // See if we need to change the state to MISSILE_EXPLODE
            if (explode)
            {
                // Stop the missile moving and cause it to explode
                flags = (flags & ~OF_MOVING & ~OF_WEIGHTLESS) | OF_IMMOBILE;
                SetState(MISSILE_EXPLODE);
            }
            break;
        }

        case MISSILE_EXPLODE :
        {
            if (!animator)
                KillThisEffect();
            break;
        }
    }

    TEffect::Pulse();
}

void TMissileEffect::OffScreen()
{
    if (animator && (state == MISSILE_EXPLODE) && status)
        TObjectInstance::OffScreen();
}

// *****************
// * TPhotonEffect *
// *****************

DEFINE_BUILDER("Photon", TPhotonEffect)
REGISTER_BUILDER(TPhotonEffect)

void TPhotonEffect::Initialize()
{
    TMissileEffect::Initialize();

    SetSpeed(PHOTON_SPEED);

    SoundPlayer.Mount(LIGHTNING_SOUND);

    firsttime = TRUE;
}

void TPhotonEffect::Pulse()
{
    TMissileEffect::Pulse();

    if ((state == MISSILE_FLY) && (firsttime))
    {
        firsttime = FALSE;
        SoundPlayer.Play(LIGHTNING_SOUND);
    }
}

// *******************
// * Photon Animator *
// *******************

REGISTER_3DANIMATOR("Photon", TPhotonAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for a photon animator. The
//               photon effect starts with a launch sequence which is made up
//               of a sparkle effect around the player.  The initial positions
//               of each of the sparks are set here.
//==============================================================================

void TPhotonAnimator::Initialize()
{
    S3DTex tex;

    T3DAnimator::Initialize();

    // Get how many frames are in the texture
    Get3DImagery()->GetTexture(0, &tex);
    numtexframes = tex.numframes;

    // Set the beginning state of the effect
    state = oldstate = MISSILE_LAUNCH;

    // Initialize the values for the central flare
    p[0].x = p[0].y = p[0].z = 0;
    scale[0] = 1.0;
    framenum[0] = 0;

    // Initialize the starting position of each spark, its scale and a delay
    // before it starts (a neg. framenum)
    for (int n = 1; n < NUM_PHOTON_SPARKS; n++)
    {
        p[n].x = (D3DVALUE)(random(-PHOTON_LAUNCH_RADIUS, PHOTON_LAUNCH_RADIUS));
        p[n].y = (D3DVALUE)(random(-PHOTON_LAUNCH_RADIUS, PHOTON_LAUNCH_RADIUS));
        p[n].z = (D3DVALUE)(random(-PHOTON_LAUNCH_RADIUS, PHOTON_LAUNCH_RADIUS));

        // Set a vector for most of the sparks to move them to the center of the effect
        if (n > NUM_PHOTON_SPARKS / 3)
        {
            v[n].x = - p[n].x / (PHOTON_LAUNCH_DURATION * 3 / 4);
            v[n].y = - p[n].y / (PHOTON_LAUNCH_DURATION * 3 / 4);
            v[n].z = - p[n].z / (PHOTON_LAUNCH_DURATION * 3 / 4);
        }
        else
            v[n].x = v[n].y = v[n].z = 0;

        scale[n] = 0.0;

        framenum[n] = random(-PHOTON_LAUNCH_DURATION / 2, 0);
    }

    // Initialize the framenums for the trailing photon sparks
    for (n = NUM_PHOTON_SPARKS; n < NUM_PHOTON_SPARKS * 2; n++)
        framenum[n] = -1;

    // Set how many photon sparks are active at the beginning
    activesparks = NUM_PHOTON_SPARKS - 1;
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the frame number, position, and scale values
//               for the three different states of the photon effect.
//==============================================================================

void TPhotonAnimator::Animate(BOOL draw)
{
    int n;
    int t;

    T3DAnimator::Animate(draw);
    ((PTMissileEffect)inst)->SetStatus(FALSE);
    inst->SetCommandDone(TRUE);

    switch(state)
    {
        case MISSILE_LAUNCH :
        {
            // Adjust the framenum & scale for the central flare
            framenum[0]++;
            if ((framenum[0] > PHOTON_LAUNCH_DURATION / 2) && (scale[0] < (D3DVALUE)5.5))
                scale[0] += PHOTON_LAUNCH_SCALE_STEP;

            // Adjust the framenum, position, and scale vals for all the other sparks
            for (n = 1; n < NUM_PHOTON_SPARKS; n++)
            {
                framenum[n]++;

                if (framenum[n] >= 0)
                {
                    // Move the sparks
                    p[n].x += v[n].x;
                    p[n].y += v[n].y;
                    p[n].z += v[n].z;

                    // If the effect is over half over, scale the sparks down
                    if (framenum[n] > PHOTON_LAUNCH_DURATION / 2)
                    {
                        if (scale[n])
                        {
                            scale[n] -= PHOTON_LAUNCH_SCALE_STEP;
                            // Check for the end of this spark
                            if (scale[n] <= 0)
                            {
                                // Take this spark off the active spark list
                                // and see if there are any sparks left
                                activesparks--;
                                if (activesparks < NUM_PHOTON_SPARKS / 3)
                                    ((PTMissileEffect)inst)->SetStatus(TRUE);
                                scale[n] = 0;
                            }
                        }
                    }
                    else
                    // Otherwise, scale the sparks up
                        scale[n] += PHOTON_LAUNCH_SCALE_STEP;
                }
            }
            break;
        }

        case MISSILE_FLY :
        {
            // See if this is our first time
            if (state != oldstate)
            {
                S3DPoint dir;
                inst->GetVel(dir);

                // Initialize the variables for the leading photon
                p[0].x = p[0].y = p[0].z = 0;
                scale[0] = (D3DVALUE)3.5;
                framenum[0] = 0;

                // Initialize the variables for the trailing photons
                for (n = NUM_PHOTON_SPARKS, t = 1; n < NUM_PHOTON_SPARKS * 2; n++, t++)
                {
                    framenum[n] = -t;
                    framenum[t] = -1;
                    p[n].x = (D3DVALUE)-dir.x * t / ROLLOVER;
                    p[n].y = (D3DVALUE)-dir.y * t / ROLLOVER;
                    p[n].z = (D3DVALUE)-dir.z * t / ROLLOVER;
                    scale[n] = (D3DVALUE)3.5 - t * (D3DVALUE)PHOTON_LAUNCH_SCALE_STEP;
                }
            }
            else
            {
                // Otherwise, just update the frame counter
                for (n = NUM_PHOTON_SPARKS; n < NUM_PHOTON_SPARKS * 2; n++)
                    framenum[n]++;
            }
            break;
        }

        case MISSILE_EXPLODE :
        {
            // See if this is our first time
            if (state != oldstate)
            {
                S3DPoint dir;
                inst->GetVel(dir);

                // Initialize the sparks that come off of the explosion
                for (n = 0; n < NUM_PHOTON_SPARKS; n++)
                {
                    p[n].x = p[n].y = p[n].z = 0;
                    scale[n] = (D3DVALUE)3.0;

                    v[n].x = (D3DVALUE)random(-3, 3) * (D3DVALUE)3.0 / (D3DVALUE)2.0;
                    v[n].y = (D3DVALUE)random(-3, 3) * (D3DVALUE)3.0 / (D3DVALUE)2.0;
                    v[n].z = (D3DVALUE)random(-3, 3) * (D3DVALUE)3.0 / (D3DVALUE)2.0;

                    framenum[n] = 0;
                }
                activesparks = NUM_PHOTON_SPARKS;
            }
            else
            {
                // It's not our first time, so update the frame numbers, the
                // positions and the scale values
                for (n = 0, t = NUM_PHOTON_SPARKS; n < NUM_PHOTON_SPARKS; n++, t++)
                {
                    framenum[n]++;

                    // Move the sparks
                    p[n].x += v[n].x;
                    p[n].y += v[n].y;
                    p[n].z += v[n].z;

                    if (scale[n] > 0)
                    {
                        scale[n] -= PHOTON_LAUNCH_SCALE_STEP / 2;
                        // Check for the end of this spark
                        if (scale[n] <= 0)
                        {
                            scale[n] = 0;
                            // If it has, take it off the active spark list and
                            // see if there are any sparks left
                            activesparks--;
                            if (activesparks == 0)
                                inst->SetCommandDone(TRUE);
                        }
                    }

                    // Update any remaining trailing photons
                    if (scale[t])
                    {
                        framenum[t]++;
                        scale[t] -= PHOTON_LAUNCH_SCALE_STEP / 2;
                        // Check for the end of this trailing photon
                        if (scale[t] <= 0)
                            scale[t] = 0;
                    }
                }
            }
            break;
        }
    }

    // Keep track of the state we're in
    oldstate = state;
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : This loops through all the photon sparks and renders each
//               at the appropriate location and scale value.
//==============================================================================

BOOL TPhotonAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(0);

    for (int n = 0; n < NUM_PHOTON_SPARKS * 2; n++)
    {
        // Only render the spark if it's framenum is positive
        if (framenum[n] >= 0)
        {
            // Reset render extents
            Get3DImagery()->ResetExtents();

            // Set the scale and position of the object
            obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;
            obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)scale[n];
            obj->pos = p[n];

            // Set the frame number manually
            frame = framenum[n] % numtexframes;

            RenderObject(obj);

            // Update bounding rect and screen rect
            UpdateExtents();
        }
    }

    RestoreBlendState();

    return TRUE;
}

void TPhotonAnimator::RefreshZBuffer()
{
    S3DPoint effect, map, screen;
    ((PTEffect)inst)->GetPos(effect);
    int tex_u= 16;
    int tex_v = 16;
    int x_size, y_size;

    for (int n = 0; n < NUM_PHOTON_SPARKS * 2; n++)
    {
        // Only refresh the spark if it's framenum is positive
        if (framenum[n] >= 0)
        {
            // Set the scale and position of the object
            x_size = (int)((float)scale[n] * (float)tex_u);
            y_size = (int)((float)scale[n] * (float)tex_v);

            // get the coordinates
            map.x = (int)p[n].x + effect.x;
            map.y = (int)p[n].y + effect.y;
            map.z = (int)p[n].z + effect.z;

            // convert
            WorldToScreen(map, screen);

            RestoreZ(screen.x - (x_size / 2), screen.y - (y_size / 2), x_size, y_size);
        }
    }
}

// ***************************
// * The FireBall Effect *
// ***************************

extern TObjectClass EffectClass;

DEFINE_BUILDER("FireBall", TFireBallEffect)
REGISTER_BUILDER(TFireBallEffect)

void TFireBallEffect::Initialize()
{
    TMissileEffect::Initialize();

    SetSpeed(FIREBALL_SPEED);
}

void TFireBallEffect::Pulse()
{
    TMissileEffect::Pulse();
}

// *************************
// * The FireBall Animator *
// *************************

REGISTER_3DANIMATOR("FireBall", TFireBallAnimator)

// init the fireball animation, just for kicks
void TFireBallAnimator::Initialize()
{
    // call the 3d animator init
    T3DAnimator::Initialize();
    state = 0;

    PS3DAnimObj o = GetObject( 0);
    GetVerts(o, D3DVT_LVERTEX);
    // *** FIREBALL ***

    S3DPoint temp_point;
    inst->GetPos(temp_point);
    temp_point.z += 50;
    inst->SetPos(temp_point);
    // init the fireball's position
    fireball.pos.x = fireball.pos.y = fireball.pos.z = 0;
    //PLAY("fireball grow");
    // init the fireball's scale
    fireball.scale = FIREBALL_MAX_SIZE / 2.0f;
    // init the fireball's rotation
    fireball.rotation = 0;
    // increment the fireball's frame
    fireball.frame = 0.0f;

    // frame stuff
    frame_count = FIREBALL_MAX_FRAME;
    glow_frame = FIREBALL_GLOW_FRAME;

    // sparkle stuff
    fireball.glow = 1.5f;

    // spark init
    o = GetObject( 1);
    spark.Init(this, o);

    // init the ring
    o = GetObject( 2);
    ring.Init(this, o, 4, 24);

    explode = 0;

    for(int i = 0; i < FIREBALL_MAX_BURST; ++i)
        burst[i].used = 0;

    old_state = state;
}


void TFireBallAnimator::Animate(BOOL draw)
{
    // base class animation
    T3DAnimator::Animate(draw);

    // override being done
    inst->SetCommandDone(FALSE);
    ((PTMissileEffect)inst)->SetStatus(FALSE);

    if(old_state != state)
        firsttime = 1;
    else
        firsttime = 0;

    // explosion state stuff
    if(explode == 1)
        explode = -1;
    else if(explode == 0 && state == MISSILE_EXPLODE)
        explode = 1;

    S3DPoint effect_pos;
    ((PTEffect)inst)->GetPos(effect_pos);

    // set the new spark parameters
    SSubParticleParams params;

    if(explode == -1)
        params.particles = 0;
    else if(explode == 1)
        params.particles = FIREBALL_MAX_SPARK;
    else
        params.particles = FIREBALL_NORM_SPARK;

    if(state == MISSILE_LAUNCH)
        params.chance = 25;
    else if(state == MISSILE_FLY)
        params.chance = 30;
    else
        params.chance = 100;

    params.velocity_dir.x = 0.0f;
    params.velocity_dir.y = 0.0f;
    params.velocity_dir.z = 0.0f;

    params.pos.x = (float)effect_pos.x;
    params.pos.y = (float)effect_pos.y;
    params.pos.z = (float)effect_pos.z;
    params.pos_spread.x = params.pos_spread.y = params.pos_spread.z = 0.0f;

    params.scale.x = params.scale.y = params.scale.z = .15f;
    params.scale_dec.x = params.scale_dec.y = params.scale_dec.z = .90f;
    params.scale_spread.x = params.scale_spread.y = params.scale_spread.z = 0.0f;

    params.velocity.x = params.velocity.y = params.velocity.z = 0.0f;

    if(state == MISSILE_LAUNCH)
    {
        params.velocity_spread.z = 1.0f;
        params.velocity_spread.x = params.velocity_spread.y = 3.0f;
    }
    else
        params.velocity_spread.x = params.velocity_spread.y = params.velocity_spread.z = 1.0f;


    params.gravity = 0.37f;

    params.min_life = 15;
    params.max_life = 20;

    params.flicker = TRUE;
    params.flicker_size = 1.75f;

    spark.Set(&params);

    // do the spark animation
    spark.Animate();

    // do the trails
    for(int i = FIREBALL_TRAIL_SIZE - 1; i > 0; --i)
    {
        trail[i] = trail[i - 1];
        trail[i].scale *= FIREBALL_TRAIL_SCALE;
    }
    trail[0] = fireball;
    trail[0].scale *= FIREBALL_TRAIL_SCALE;
    trail[0].pos.x += (float)effect_pos.x;
    trail[0].pos.y += (float)effect_pos.y;
    trail[0].pos.z += (float)effect_pos.z;

    // change the glow's size
    fireball.glow = 1.0f + (.05f * (float)random(0, 15));

    // change the frame
    fireball.frame += 1.0f;
    if(fireball.frame > frame_count)
        fireball.frame = 0.0f;
    if((int)fireball.frame == glow_frame)
    {
        fireball.frame += 1.0f;
        if(fireball.frame > frame_count)
            fireball.frame = 0.0f;
    }
#ifdef FIREBALL_WHITE_FADE
    fireball.rotation = (fireball.rotation + 12) % 360;
#else
    fireball.rotation = (fireball.rotation + 2) % 360;
#endif

    int count = 0;
    switch(state)
    {
        // make the fireball grow above the caster
        case MISSILE_LAUNCH: 
            ((PTMissileEffect)inst)->SetStatus(TRUE);
            break;
        // when the fireball is launched
        case MISSILE_FLY:
            if(fireball.scale < FIREBALL_MAX_SIZE)
                fireball.scale += FIREBALL_GROW_RATE;           
            break;

        // when the fireball impacts the target
        case MISSILE_EXPLODE:           
            if(fireball.scale < FIREBALL_MAX_SIZE)
                fireball.scale += FIREBALL_GROW_RATE;
            if(firsttime)
            {
                S3DPoint pos;
                pos.x = (int)fireball.pos.x + effect_pos.x;
                pos.y = (int)fireball.pos.y + effect_pos.y;
                pos.z = (int)fireball.pos.z + effect_pos.z;

                BlastCharactersInRange(((PTEffect)inst)->GetSpell()->GetInvoker(), 
                    pos, 150, ((PTEffect)inst)->GetSpell()->VariantData()->mindamage, 
                    ((PTEffect)inst)->GetSpell()->VariantData()->maxdamage, 
                    ((PTEffect)inst)->GetSpell()->SpellData()->damagetype);

                //PLAY("fireball explosion");
                for(i = 0; i < FIREBALL_MAX_BURST; ++i)
                {
                    burst[i].used = 1;
                    burst[i].pos.x = fireball.pos.x + random(-15, 20);
                    burst[i].pos.y = fireball.pos.y + random(-15, 20);
                    burst[i].pos.z = fireball.pos.z + random(-15, 20);
                    burst[i].scale = .75f + (float)random(0, 5) * .15f;
                    burst[i].rotation = 0;
                    burst[i].glow = 1.0f + (.05f * (float)random(0, 15));
                    burst[i].frame = (float)random(0, frame_count - 1);
                    if(burst[i].frame == glow_frame)
                        ++burst[i].frame;
                }
                // ring setting code
                SShockParam param;
                // position
                param.pos.x = fireball.pos.x + (float)effect_pos.x;
                param.pos.y = fireball.pos.y + (float)effect_pos.y;
                param.pos.z = fireball.pos.z + (float)effect_pos.z;
                // rotation
                param.rot.x = 0.0f;
                param.rot.y = 0.0f;
                param.rot.z = 0.0f;
                // scale
                param.scale.x = 25.0f;
                param.scale.y = 25.0f;
                param.scale.z = 25.0f;
                // scale factor
                param.scale_factor.x = 1.085f;
                param.scale_factor.y = 1.085f;
                param.scale_factor.z = 1.085f;
                // flags
                param.flags = SHOCKWAVE_FLAG_FADE;
                // setting limits
                param.max_size.x = 120.0f;
                param.max_size.y = 120.0f;
                param.max_size.z = 120.0f;

                ring.Set(&param);

                // color
                ring.SetRingColor(0, D3DRGBA(.62f, .06f, .05f, 0.0f));
                ring.SetRingColor(1, D3DRGBA(.78f, .24f, .06f, 1.0f));
                ring.SetRingColor(2, D3DRGBA(.97f, .61f, .06f, .75f));
                ring.SetRingColor(3, D3DRGBA(.99f, .83f, .52f, 0.0f));
            }
            else
            {
                ring.Animate();
                for(i = 0; i < FIREBALL_MAX_BURST; ++i)
                {
                    if(!burst[i].used)
                        continue;
                    ++count;
                    burst[i].scale *= .90f;
                    if(burst[i].scale < .50f)
                        burst[i].used = 0;                      
                    burst[i].glow = 1.0f + (.05f * (float)random(0, 15));
                    ++burst[i].frame;
                    if(burst[i].frame == glow_frame)
                        ++burst[i].frame;
                    burst[i].frame = (float)((int)burst[i].frame % frame_count);
                }
            }
            // rewrite this section for an explosion
            if(!IsTrail() && !spark.GetCount() && !count && ring.IsDone())
            {
                ((PTEffect)inst)->KillThisEffect();
            }
            break;
    }
    old_state = state;
}

void TFireBallAnimator::SetAnimFrame(int frame_num, PS3DAnimObj obj)
{
    float u = (float)(frame_num % 4) * .25f;
    float v = (float)(frame_num / 4) * .25f;

    obj->flags |= OBJ3D_VERTS;

    obj->lverts[0].tu = u;
    obj->lverts[0].tv = v;
    obj->lverts[2].tu = u + .25f;
    obj->lverts[2].tv = v;


    obj->lverts[1].tu = u;
    obj->lverts[1].tv = v + .25f;

    obj->lverts[3].tu = u + .25f;
    obj->lverts[3].tv = v + .25f;
}

int TFireBallAnimator::IsTrail()
{
    if(trail[FIREBALL_TRAIL_SIZE - 1].pos.x != trail[0].pos.x)
        return 1;
    if(trail[FIREBALL_TRAIL_SIZE - 1].pos.y != trail[0].pos.y)
        return 1;
    if(trail[FIREBALL_TRAIL_SIZE - 1].pos.z != trail[0].pos.z)
        return 1;

    return 0;
}

void TFireBallAnimator::RenderFireBallBurst()
{
    PS3DAnimObj obj;

    // grab the object
    obj = GetObject( 0);

    for(int i = 0; i < FIREBALL_MAX_BURST; ++i)
    {
        if(!burst[i].used)
            continue;       

        // *** GLOW ***
        ResetExtents();

        // texture rearranging here
        obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3;

        SetAnimFrame(glow_frame, obj);

        // rotate the to face user
        obj->rot.z = (float)(-M_PI / 3.0f);
        obj->rot.x = (float)(-M_PI / 4.0f);
        obj->rot.y = 0;

        // scale the thing
        obj->scl.x = obj->scl.y = obj->scl.z = (burst[i].scale * burst[i].glow);

        // position the thing
        obj->pos.x = burst[i].pos.x;
        obj->pos.y = burst[i].pos.y;
        obj->pos.z = burst[i].pos.z;

        RenderObject( obj);

        UpdateExtents();
        // *** END GLOW ***

        // *** FIREBALL ***
        // reset the extents DUH!
        ResetExtents();

        obj->flags = OBJ3D_MATRIX;

#ifndef FIREBALL_WHITE_FADE
        SetAnimFrame((int)burst[i].frame, obj);
#else
        SetAnimFrame(0, obj);
#endif

        // clear the matrix
        D3DMATRIXClear( &obj->matrix);
    
        D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -burst[i].rotation * TORADIAN));
        D3DMATRIXRotateX( &obj->matrix, D3DVAL( -30 * TORADIAN));
        D3DMATRIXRotateY( &obj->matrix, D3DVAL( 60 * TORADIAN));

        // Offset for our base rotation
        D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -((inst->GetFace() * 360) / 256) * TORADIAN));

        // scale the thing
        obj->scl.x = obj->scl.y = obj->scl.z = burst[i].scale;
        D3DMATRIXScale( &obj->matrix, &obj->scl);

        // position the thing
        obj->pos.x = burst[i].pos.x;
        obj->pos.y = burst[i].pos.y;
        obj->pos.z = burst[i].pos.z;
        D3DMATRIXTranslate( &obj->matrix, &obj->pos);

        RenderObject( obj);

        // update the extents DUH!
        UpdateExtents();
        // *** END FIREBALL ***
    }
}

void TFireBallAnimator::RenderFireBallTrail()
{
    S3DPoint effect_pos;
    ((PTEffect)inst)->GetPos(effect_pos);

    PS3DAnimObj obj;

    // grab the object
    obj = GetObject( 0);

    int start = FIREBALL_TRAIL_SIZE;
    for(int i = FIREBALL_TRAIL_SIZE - 1; i >= 1; --i)
    {
        if(trail[i].pos.x == trail[0].pos.x && trail[i].pos.y == trail[0].pos.y && trail[i].pos.z == trail[0].pos.z)
            break;
        start = i;
    }

    for(i = FIREBALL_TRAIL_SIZE - 1; i >= start; --i)
    {
#ifdef FIREBALL_SPACEOUT
        if(!(i % 2))
            continue;
#endif
        ResetExtents();

        // texture rearranging here
        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);

        SetAnimFrame(glow_frame, obj);

        // rotate the to face user
        D3DMATRIXRotateX( &obj->matrix, D3DVAL( -30 * TORADIAN));
        D3DMATRIXRotateY( &obj->matrix, D3DVAL( 60 * TORADIAN));
        D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -((inst->GetFace() * 360) / 256) * TORADIAN));

        // scale the thing
        obj->scl.x = obj->scl.y = obj->scl.z = (trail[i].scale * trail[i].glow);
        D3DMATRIXScale(&obj->matrix, &obj->scl);

        // position the thing
        obj->pos.x = trail[i].pos.x - effect_pos.x;
        obj->pos.y = trail[i].pos.y - effect_pos.y;
        obj->pos.z = trail[i].pos.z - effect_pos.z;

        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        RenderObject( obj);

        UpdateExtents();
    }
    for(i = FIREBALL_TRAIL_SIZE - 1; i >= start; --i)
    {
#ifdef FIREBALL_SPACEOUT
        if(!(i % 2))
            continue;
#endif
        // reset the extents
        ResetExtents();

        obj->flags = OBJ3D_MATRIX;

#ifndef FIREBALL_WHITE_FADE
        SetAnimFrame((int)trail[i].frame, obj);
#else
        int n = i + 1;
        if(n >= glow_frame)
            ++n;
        if(n > frame_count)
            n = frame_count - 1;
        SetAnimFrame(n, obj);
#endif

        // clear the matrix
        D3DMATRIXClear( &obj->matrix);
    
        D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -trail[i].rotation * TORADIAN));
        D3DMATRIXRotateX( &obj->matrix, D3DVAL( -30 * TORADIAN));
        D3DMATRIXRotateY( &obj->matrix, D3DVAL( 60 * TORADIAN));

        // Offset for our base rotation
        D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -((inst->GetFace() * 360) / 256) * TORADIAN));


        // scale the thing
        obj->scl.x = obj->scl.y = obj->scl.z = trail[i].scale;
        D3DMATRIXScale( &obj->matrix, &obj->scl);

        // position the thing
        obj->pos.x = trail[i].pos.x - effect_pos.x;
        obj->pos.y = trail[i].pos.y - effect_pos.y;
        obj->pos.z = trail[i].pos.z - effect_pos.z;

        D3DMATRIXTranslate( &obj->matrix, &obj->pos);
        
        // render the object
        RenderObject( obj);

        // update the extents
        UpdateExtents();
    }
}

void TFireBallAnimator::RenderFireBall()
{
    PS3DAnimObj obj;

    // grab the object
    obj = GetObject( 0);

// *** FIREBALL ***
    // reset the extents DUH!
    ResetExtents();

    obj->flags = OBJ3D_MATRIX;

#ifndef FIREBALL_WHITE_FADE
    SetAnimFrame((int)fireball.frame, obj);
#else
    SetAnimFrame(0, obj);
#endif

    // clear the matrix
    D3DMATRIXClear( &obj->matrix);
    
    D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -fireball.rotation * TORADIAN));
    D3DMATRIXRotateX( &obj->matrix, D3DVAL( -30 * TORADIAN));
    D3DMATRIXRotateY( &obj->matrix, D3DVAL( 60 * TORADIAN));

    // Offset for our base rotation
    D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -((inst->GetFace() * 360) / 256) * TORADIAN));


    // scale the thing
    obj->scl.x = obj->scl.y = obj->scl.z = fireball.scale;
    D3DMATRIXScale( &obj->matrix, &obj->scl);

    // position the thing
    obj->pos.x = fireball.pos.x;
    obj->pos.y = fireball.pos.y;
    obj->pos.z = fireball.pos.z;
    D3DMATRIXTranslate( &obj->matrix, &obj->pos);

    RenderObject( obj);

    // update the extents DUH!
    UpdateExtents();
// *** END FIREBALL ***
}

void TFireBallAnimator::RenderFireBallGlow()
{
    PS3DAnimObj obj;

    // grab the object
    obj = GetObject( 0);
// *** GLOW ***
    ResetExtents();

    // texture rearranging here
    obj->flags = OBJ3D_MATRIX;

    SetAnimFrame(glow_frame, obj);

    // clear the matrix
    D3DMATRIXClear( &obj->matrix);
    
    D3DMATRIXRotateX( &obj->matrix, D3DVAL( -30 * TORADIAN));
    D3DMATRIXRotateY( &obj->matrix, D3DVAL( 60 * TORADIAN));
    D3DMATRIXRotateZ( &obj->matrix, D3DVAL( -((inst->GetFace() * 360) / 256) * TORADIAN));

    // scale the thing
    obj->scl.x = obj->scl.y = obj->scl.z = (fireball.scale * fireball.glow);
    D3DMATRIXScale(&obj->matrix, &obj->scl);

    // position the thing
    obj->pos.x = fireball.pos.x;
    obj->pos.y = fireball.pos.y;
    obj->pos.z = fireball.pos.z;
    D3DMATRIXTranslate(&obj->matrix, &obj->pos);

    RenderObject( obj);

    UpdateExtents();

// *** END GLOW ***
}

BOOL TFireBallAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    spark.Render();

    switch(state) 
    {
        case MISSILE_LAUNCH:
        case MISSILE_FLY:
            RenderFireBallGlow();
            RenderFireBallTrail();
            RenderFireBall();
            break;
        case MISSILE_EXPLODE:           
            RenderFireBallTrail();
            RenderFireBallBurst();
            ring.Render();
            break;
    }

    RestoreBlendState();

    return TRUE;
}

void TFireBallAnimator::RefreshZBuffer()
{
    S3DPoint effect_pos, pos, map_pos, screen_pos;

    ((PTEffect)inst)->GetPos(effect_pos);

    int rect_u, rect_v;
    int i;

    spark.RefreshZBuffer(FIREBALL_SPARK_TEXTURE_U, FIREBALL_SPARK_TEXTURE_V);

    for(i = 0; i < FIREBALL_MAX_BURST; ++i)
    {
        if(!burst[i].used)
            continue;
        rect_u = (int)(FIREBALL_TEXTURE_U * burst[i].scale * burst[i].glow);
        rect_v = (int)(FIREBALL_TEXTURE_V * burst[i].scale * burst[i].glow);

        pos.x = (int)(burst[i].pos.x + effect_pos.x);
        pos.y = (int)(burst[i].pos.y + effect_pos.y);
        pos.z = (int)(burst[i].pos.z + effect_pos.z);

        WorldToScreen(pos, screen_pos);

        RestoreZ((int)screen_pos.x - (rect_u / 2), (int)screen_pos.y - (rect_v / 2), rect_u, rect_v);
    }

    switch(state) 
    {
        case MISSILE_LAUNCH:
        case MISSILE_FLY:
        case MISSILE_EXPLODE:
            ring.RefreshZBuffer(6, 3);
            rect_u = (int)(FIREBALL_TEXTURE_U * fireball.scale * fireball.glow);
            rect_v = (int)(FIREBALL_TEXTURE_V * fireball.scale * fireball.glow);

            pos.x = (int)(fireball.pos.x + effect_pos.x);
            pos.y = (int)(fireball.pos.y + effect_pos.y);
            pos.z = (int)(fireball.pos.z + effect_pos.z);

            WorldToScreen(pos, screen_pos);

            RestoreZ((int)screen_pos.x - (rect_u / 2), (int)screen_pos.y - (rect_v / 2), rect_u, rect_v);

            for(i = 0; i < FIREBALL_TRAIL_SIZE; ++i)
            {
#ifdef FIREBALL_SPACEOUT
                if(!(i % 2))
                    continue;
#endif
                rect_u = (int)(FIREBALL_TEXTURE_U * trail[i].scale);
                rect_v = (int)(FIREBALL_TEXTURE_V * trail[i].scale);

                pos.x = (int)(trail[i].pos.x);
                pos.y = (int)(trail[i].pos.y);
                pos.z = (int)(trail[i].pos.z);

                WorldToScreen(pos, screen_pos);

                RestoreZ((int)screen_pos.x - (rect_u / 2), (int)screen_pos.y - (rect_v / 2), rect_u, rect_v);
            }
            break;
    }
}



// *********************
// * TFireColumnEffect *
// *********************

DEFINE_BUILDER("FIRECOLUMN", TFireColumnEffect)
REGISTER_BUILDER(TFireColumnEffect)

void TFireColumnEffect::Initialize()
{
    TMissileEffect::Initialize();

    SetSpeed( FIRECOLUMN_SPEED);

//    SoundPlayer.Mount(LIGHTNING_SOUND);

    firsttime = TRUE;
}

void TFireColumnEffect::Pulse()
{
    TMissileEffect::Pulse();

    switch(state)
    {
        case MISSILE_LAUNCH:
        {
            if (firsttime)
            {
                firsttime = FALSE;
//              SoundPlayer.Play(LIGHTNING_SOUND);
            }
            break;
        }
        case MISSILE_FLY:
        {
            if (commanddone)
                range = 0;
            break;
        }
    }
}

REGISTER_3DANIMATOR("FireColumn", TFireColumnAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for a fireball animator. The
//               fireball effect starts with a launch sequence which is made up
//               of a sparkle effect around the player.  The initial positions
//               of each of the sparks are set here.
//==============================================================================

void TFireColumnAnimator::Initialize()
{
    S3DTex tex;
    int n;

    T3DAnimator::Initialize();

    // Get how many frames are in the texture
    Get3DImagery()->GetTexture(0, &tex);
    numtexframes = tex.numframes;

    // Set the beginning state of the effect
    state = oldstate = MISSILE_LAUNCH;

    ConvertToVector( inst->GetFace(), 10, vctr);
    p[0].x = p[0].y = 0;
    OffsetPoint( 0);
    for (n = 0; n < NUM_FIRE_COLUMNS; n++)
    {
        framenum[n] = (0 - (n / 3));
//      if (n < (NUM_FIRE_COLUMNS - 2))
//      {
            p[n].x = D3DVAL( vctr.x * (n + 3));
            p[n].y = D3DVAL( vctr.y * (n + 3));
//      }
//      else
//      {
//          p[n].x = D3DVAL( vctr.x * (n + 4));
//          p[n].y = D3DVAL( vctr.y * (n + 4));
//      }
        rotation[n] = D3DVAL( random( 0, 360) * TORADIAN);
        scale[n] = 100;
    }
    activesparks = 0;
}

void TFireColumnAnimator::OffsetPoint( int pnt) {
    p[pnt].x += D3DVAL( vctr.x);
    p[pnt].y += D3DVAL( vctr.y);
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the frame number, position, and scale values
//               for the three different states of the firecolumn effect.
//==============================================================================

void TFireColumnAnimator::Animate(BOOL draw)
{
    int n;

    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    switch (state)
    {
        case MISSILE_LAUNCH:
        {
            // No fancy launch, yet
            inst->SetCommandDone(TRUE);
            break;
        }

        case MISSILE_FLY:
        {
            for (n = 0; n < NUM_FIRE_COLUMNS; n++)
            {
                framenum[n]++;

                if (framenum[n] >= numtexframes)    // Add flames sequentially
                {
                    if (n == (NUM_FIRE_COLUMNS - 1))
                    {
                        activesparks++; // Loop counter
                        if (activesparks == 5)
                        {
                            activesparks = NUM_FIRE_COLUMNS;    // Used for finale
                            inst->SetCommandDone(TRUE);
                        }
                    }
                    framenum[n] = random( 0, 5); // -32768
                }
            }
            break;
        }

        case MISSILE_EXPLODE:
        {
            for (n = 0; n < NUM_FIRE_COLUMNS; n++)
            {
                // Remove trail sequentially, but keep explosion going
                if (activesparks > 2)
                {
                    if (n <= (NUM_FIRE_COLUMNS - activesparks)) // Start to Finish
                    {
                        if (scale[n] <= 10) // If too small to see, kill it
                        {
                            if (framenum[n] >= 0)   // Only need to do this once
                            {
                                activesparks--;
                                framenum[n] = -1;
                            }
                        } else
                            scale[n] -= 33;
                    }
                }
                else
                    scale[n] -= 8;

                if (framenum[n] >= 0)   // Animate if still visible
                {
                    framenum[n]++;
                    if (framenum[n] >= numtexframes)
                        framenum[n] = random( 0, 5); // -32768
                }

                // We're done when the big boy is 10% of its size
                if (scale[(NUM_FIRE_COLUMNS - 1)] < 10)
                    inst->SetCommandDone(TRUE);
            }
            break;
        }
    }

    // Keep track of the state we're in
    oldstate = state;
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : This loops through all the fire columns and renders each
//               at the appropriate location and scale value.
//==============================================================================

BOOL TFireColumnAnimator::Render()
{
    DWORD savedcull;
    PS3DAnimObj obj;

    SaveBlendState();
    SetBlendState();

    // So I can render any face
    TRY_D3D(Device2->GetRenderState( D3DRENDERSTATE_CULLMODE, &savedcull));
    TRY_D3D(Device2->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE));

    obj = GetObject(0); // Column

    for (int n = 0; n < (NUM_FIRE_COLUMNS - 2); n++)
    {
        // Only render the spark if it's framenum is positive
        if (framenum[n] >= 0)
        {
            // Reset render extents
            Get3DImagery()->ResetExtents();

            // Set the scale and position of the object
            obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS;
            if (n == (NUM_FIRE_COLUMNS - 3))
            {
                obj->scl.x = D3DVAL( 2 * scale[n] / 100);
                obj->scl.y = D3DVAL( 3 * scale[n] / 100);
                obj->scl.z = D3DVAL( 4 * scale[n] / 100);
            }
            else
            {
                obj->scl.x = D3DVAL( 1 * scale[n] / 100);
                obj->scl.y = D3DVAL( 2 * scale[n] / 100);
                obj->scl.z = D3DVAL( 3 * scale[n] / 100);
            }
            obj->pos.x = pos.x + p[n].x;
            obj->pos.y = pos.y + p[n].y;
            obj->pos.z = pos.z + 5;

            D3DMATRIXClear(&obj->matrix);

            D3DMATRIXScale(&obj->matrix, &obj->scl);
            D3DMATRIXRotateZ(&obj->matrix, rotation[n]);
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            // Set the frame number manually
            frame = framenum[n] % numtexframes;

            RenderObject(obj);

            // Update bounding rect and screen rect
            UpdateExtents();
        }
    }

    obj = GetObject(1); // Explosion

    for (; n < NUM_FIRE_COLUMNS; n++)
    {
        // Only render the spark if it's framenum is positive
        if (framenum[n] >= 0)
        {
            // Reset render extents
            Get3DImagery()->ResetExtents();

            // Set the scale and position of the object
            obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS;
            if (n == (NUM_FIRE_COLUMNS - 2))
            {
                obj->scl.x = D3DVAL( 2.5 * ((scale[n] / 2) + 50) / 100);
                obj->scl.y = D3DVAL( 2.5 * ((scale[n] / 2) + 50) / 100);
                obj->scl.z = D3DVAL( 3.5 * scale[n] / 100);
            }
            else
            {
                obj->scl.x = D3DVAL( 3 * ((scale[n] / 2) + 50) / 100);
                obj->scl.y = D3DVAL( 3 * ((scale[n] / 2) + 50) / 100);
                obj->scl.z = D3DVAL( 4 * scale[n] / 100);
            }
            obj->pos.x = pos.x + p[n].x;
            obj->pos.y = pos.y + p[n].y;
            obj->pos.z = pos.z + 5;

            D3DMATRIXClear(&obj->matrix);

            D3DMATRIXScale(&obj->matrix, &obj->scl);
            D3DMATRIXRotateZ(&obj->matrix, rotation[n]);
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            // Set the frame number manually
            frame = framenum[n] % numtexframes;

            RenderObject(obj);

            // Update bounding rect and screen rect
            UpdateExtents();
        }
    }

    TRY_D3D(Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, savedcull));

    RestoreBlendState();

    return TRUE;
}

// ********************
// * TFlameDiscEffect *
// ********************

DEFINE_BUILDER("FLAMEDISC", TFlameDiscEffect)
REGISTER_BUILDER(TFlameDiscEffect)

void TFlameDiscEffect::Initialize()
{
    // Initialize the values for the central flare
    pos.x += (random(-FLAMEDISC_LAUNCH_RADIUS, FLAMEDISC_LAUNCH_RADIUS));
    pos.y += (random(-FLAMEDISC_LAUNCH_RADIUS, FLAMEDISC_LAUNCH_RADIUS));
    pos.z += (random(-FLAMEDISC_LAUNCH_RADIUS, FLAMEDISC_LAUNCH_RADIUS));

    TMissileEffect::Initialize();

    SetSpeed(FLAMEDISC_SPEED);

    SoundPlayer.Mount(LIGHTNING_SOUND);

    firsttime = TRUE;
}

void TFlameDiscEffect::Pulse()
{
    TMissileEffect::Pulse();

    if ((state == MISSILE_FLY) && (firsttime))
    {
        firsttime = FALSE;
        SoundPlayer.Play(LIGHTNING_SOUND);
    }
}

// **********************
// * FlameDisc Animator *
// **********************

REGISTER_3DANIMATOR("FLAMEDISC", TFlameDiscAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for a flame disc animator. The
//               flame disc effect starts with a launch sequence which is made up
//               of a scaling, rotating disc
//==============================================================================

void TFlameDiscAnimator::Initialize()
{
    S3DTex tex;
    S3DPoint pnt;
    int n;

    T3DAnimator::Initialize();

    // Get how many frames are in the texture
    Get3DImagery()->GetTexture(0, &tex);
    numtexframes = tex.numframes;
    activesparks = 0;

    // Set the beginning state of the effect
    state = oldstate = MISSILE_LAUNCH;

    angle = ((TMissileEffect*)inst)->GetAngle();
    for (n = 0; n <= NUM_FLAME_SPARKS; n++)
    {
        scale[n] = 0.0;
        framenum[n] = -random(2,8);
        p[n].x = p[n].y = p[n].z = 0;

//        v[n].x = D3DVAL(2-random(0,4));
//        v[n].y = D3DVAL(random(1,4));
//        v[n].z = D3DVAL(-random(0,2));
        ConvertToVector(((angle + 128 + random(-32,32)) % 256), ((TMissileEffect*)inst)->GetSpeed(), pnt);
        v[n].x = D3DVAL(pnt.x);
        v[n].y = D3DVAL(pnt.y);
        v[n].z = D3DVAL(0);
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the frame number, position, and scale values
//               for the three different states of the photon effect.
//==============================================================================

#define FLAMEDISC_MAX_SCALE_STEP 3.0 // 5.0
#define FLAMEDISC_SCALE_STEP .2

void TFlameDiscAnimator::Animate(BOOL draw)
{
    S3DPoint pnt;
    int n;

    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    switch(state)
    {
        case MISSILE_LAUNCH :
        {
            // Adjust the framenum & scale for the central flare
            if (scale[0] < (D3DVALUE)FLAMEDISC_MAX_SCALE_STEP)
            {
                framenum[0]++;
                if (framenum[0] >= 0)
                    scale[0] += (float)FLAMEDISC_SCALE_STEP;
            }
            else
                inst->SetCommandDone(TRUE);
            break;
        }

        case MISSILE_FLY :
        {
            // Adjust the framenum & scale for the central flare
            for (n = 1; n <= NUM_FLAME_SPARKS; n++)
            {
                framenum[n]++;
                if (framenum[n] >= 0)
                {
                    if (framenum[n] == 0)
                    {
                        scale[n] = 3.0;
                        activesparks++;
                    }
                    else
                    {
                        scale[n] -= (float)FLAMEDISC_SCALE_STEP;
                        if (scale[n] <= 0)
                        {
                            activesparks--;
                            framenum[n] = -random(2,8);
                            p[n].x = p[n].y = p[n].z = 0;
//                          v[n].x = D3DVAL(2-random(0,4));
//                          v[n].y = D3DVAL(random(1,4));
//                          v[n].z = D3DVAL(-random(0,2));
                            ConvertToVector(((angle + 128 + random(-32,32)) % 256), ((TMissileEffect*)inst)->GetSpeed(), pnt);
                            v[n].x = D3DVAL(pnt.x);
                            v[n].y = D3DVAL(pnt.y);
                            v[n].z = D3DVAL(0);
                        }
                        else
                        {
                            p[n].x += v[n].x;
                            p[n].y += v[n].y;
                            p[n].z += v[n].z;
                        }
                    }
                }
            }
            break;
        }

        case MISSILE_EXPLODE :
        {
            if (activesparks == 0)
                inst->SetCommandDone(TRUE);

            // Adjust the framenum & scale for the central flare
            for (n = 1; n <= NUM_FLAME_SPARKS; n++)
            {
                framenum[n]++;
                if (framenum[n] >= 0)
                {
                    if (framenum[n] == 0)
                    {
                        activesparks++;
                        scale[n] = 3.0;
                    }
                    else
                    {
                        scale[n] -= (float)FLAMEDISC_SCALE_STEP;
                        if (scale[n] <= 0)
                        {
                            activesparks--;
                            framenum[n] = -32767;
                        }
                        else
                        {
                            p[n].x += v[n].x;
                            p[n].y += v[n].y;
                            p[n].z += v[n].z;
                        }
                    }
                }
            }
            break;
        }
    }

    // Keep track of the state we're in
    oldstate = state;
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : This loops through all the photon sparks and renders each
//               at the appropriate location and scale value.
//==============================================================================

BOOL TFlameDiscAnimator::Render()
{
    PS3DAnimObj obj;
    int n;

    SaveBlendState();
    SetBlendState();

    // Only render the spark if it's framenum is positive
    for (n = 0; n <= NUM_FLAME_SPARKS; n++)
    {
        if (framenum[n] >= 0)
        {
            if (n)
                obj = GetObject(0); // 0 is Spark, 1 is FlameDisc
            else
                obj = GetObject(1); // 0 is Spark, 1 is FlameDisc
            
            // Reset render extents
            Get3DImagery()->ResetExtents();

            // Set the scale and position of the object
            obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)scale[0];
            obj->rot.x = D3DVAL(-(M_PI / 2.0));
            obj->rot.y = D3DVAL(0.0);
            obj->rot.z = D3DVAL(0.0);
            if (n)
            {
                obj->flags |= OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3;
                obj->pos = p[n];
            }
            else
                obj->flags |= OBJ3D_ROT1 | OBJ3D_SCL2;;

            // Set the frame number manually
            frame = framenum[n] % numtexframes;

            RenderObject(obj);

            // Update bounding rect and screen rect
            UpdateExtents();
        }
    }

    RestoreBlendState();

    return TRUE;
}