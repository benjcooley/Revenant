// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 effect2.cpp - Second Effects module                   *
// *************************************************************************

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>
#include <math.h>
#include "d3dmacs.h"
#include "d3dmath.h"

#include "revenant.h"
#include "effect.h"
#include "mappane.h"       
#include "character.h"
#include "statusbar.h"
#include "MissileEffect.H"
#include "StripEffect.H"
#include "food.h"
#include "textbar.h"
#include "rules.h"



/****************************************************************/
/*                         Paralize                             */
/****************************************************************/

// ***************
// * Para Effect *
// ***************

#define MAX_NUM_PARA 15
#define PARA_STATE_GROW 1
#define PARA_STATE_MOVE 2
#define PARA_STATE_SHRINK 3
#define PARA_GRAVITY 1.2f
#define PARA_KNOT_SCALE 0.5f
#define PARA_BASE_VELOCITY 0.24f
#define PARA_MAX_NUM_FLARES 10
#define PARA_FLARE_SCALE 0.4f
#define PARA_MAX_FLARE_SCALE 10.0f
#define PARA_MAX_TARGETS 5

_CLASSDEF(TParaEffect)

class TParaEffect : public TEffect
{
protected:
    D3DVECTOR direction[MAX_NUM_PARA];      // velocity vector
    D3DVECTOR acc[MAX_NUM_PARA];
    float home_radius[MAX_NUM_PARA];
    float speed[MAX_NUM_PARA];
    float home_z[MAX_NUM_PARA];
    float max_knot_scale;
    float min_radius;
    float max_radius;
    int state;                      // is the particle growing, shrinking, or moving
    PTCharacter target[PARA_MAX_TARGETS];
    int current_target[MAX_NUM_PARA];
    bool hit_flag;
public:
    int level;
    int start_time[MAX_NUM_PARA];
    int count;                      // keeps track of when to kill the particle
    D3DVECTOR pos[MAX_NUM_PARA];
    float val;
    float knot_scale;
    float scale;
    S3DPoint target_position[PARA_MAX_TARGETS];
    int num_targets;
    TParaEffect(PTObjectImagery newim) : TEffect(newim) { }
    TParaEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    void Initialize();
    void Pulse();
};

DEFINE_BUILDER("Paralize1", TParaEffect)
REGISTER_BUILDER(TParaEffect)

void TParaEffect::Initialize()
{
    // this whole thing finds a target to use
    int i;
    bool found = false;
    S3DPoint temp_point;
//  int itargets[MAXFOUNDOBJS];

/*
    if(strcmp(spell->VariantData()->name, "Paralize1") == 0)
    {
        level = 1;
        count = 200;
    }
    else if(strcmp(spell->VariantData()->name, "Paralize2") == 0)
    {
        level = 2;
        count = 300;
    }
    else if(strcmp(spell->VariantData()->name, "Paralize3") == 0)
    {
        level = 3;
        count = 300;
    }
    else if(strcmp(spell->VariantData()->name, "Paralize4") == 0)
    {
        level = 4;
        count = 200;
    }

    num_targets = 0;

    if(spell)
    {
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
        invoker->GetPos(temp_point);
        if(level == 3)
        {
            PTCharacter invoker = (PTCharacter)spell->GetInvoker();
            GetPos(temp_point);
            // figure out how many targets are in range
            int num = MapPane.FindObjectsInRange(temp_point, itargets, 5000, 0, OBJCLASS_CHARACTER);
            for (i = 0; i < num; i++) 
            {
                target[num_targets] = (PTCharacter)MapPane.GetInstance(itargets[i]);
                // check to see if the current target is groovy
                if(target[num_targets] && target[num_targets] != invoker && !((PTCharacter)target[num_targets])->IsDead())
                {
                    target[num_targets]->GetPos(target_position[num_targets]);  // and use it
                    target_position[num_targets].z += 40;
                    target_position[num_targets].z = (int)FIX_Z_VALUE(target_position[num_targets].z);
                    num_targets++;
                    if(num_targets == 5)
                        break;
                }
            }
            if(num_targets == 0)
            {
                target[0] = NULL;
                invoker->GetPos(target_position[0]);
                target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
            }

            invoker->GetPos(temp_point);

        }
        else
        {
            target[0] = (PTCharacter)invoker->Fighting();
            num_targets = 0;
            if(target[0])
            {
                target[0]->GetPos(target_position[0]);  // and use it
                target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
                num_targets = 1;
            }
            invoker->GetPos(temp_point);
        }
    }
    else
    {
        num_targets = 0;
        target[0] = NULL;
        GetPos(target_position[0]);
        GetPos(temp_point);
        target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
    }
*/

    num_targets = 0;
// we REALLY want a target on this spell!
    if(spell)
    {
        if(strcmp(spell->VariantData()->name, "Paralize1") == 0)
        {
            level = 1;
            count = 200;
        }
        else if(strcmp(spell->VariantData()->name, "Paralize2") == 0)
        {
            level = 2;
            count = 300;
        }
        else if(strcmp(spell->VariantData()->name, "Paralize3") == 0)
        {
            level = 3;
            count = 300;
        }
        else if(strcmp(spell->VariantData()->name, "Paralize4") == 0)
        {
            level = 4;
            count = 200;
        }
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
        if(invoker)
        {
            invoker->GetPos(temp_point);
            invoker->GetPos(target_position[0]);
            target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
            target[0] = (PTCharacter)invoker->Fighting();
            if(target[0])
            {
                target[0]->GetPos(target_position[0]);
                target_position[0].z = (int)FIX_Z_VALUE(target_position[0].z);
                num_targets++;
            }
            S3DPoint temp_point1;
            S3DPoint temp_point2;
            invoker->GetPos(temp_point1);
            for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
            {
                PTCharacter chr = (PTCharacter)i.Item();

                if (chr == invoker)
                    continue;

                if (chr->IsDead())
                    continue;

                chr->GetPos(temp_point2);
                if (::Distance(temp_point1,temp_point2) > 400)
                    continue;

                if (invoker && !invoker->IsEnemy(chr))
                    continue;   // We can't hurt our friends
                
                target[num_targets] = chr;
                target[num_targets]->GetPos(target_position[num_targets]);
                target_position[num_targets].z = (int)FIX_Z_VALUE(target_position[num_targets].z);
                num_targets++;
    
                if(num_targets >= PARA_MAX_TARGETS || level != 3)
                    break;
            }
        }
        else
        {
            target[0] = NULL;
            target_position[0].x = target_position[0].y = target_position[0].z = 0;
        }
    }
    else
    {
        target[0] = NULL;
        target_position[0].x = target_position[0].y = target_position[0].z = 0;
    }


// brought from animator

    D3DVECTOR point_in_relation_to_target;
    float initial_length;

    SetCommandDone( FALSE);

    GetPos(temp_point);
    
    temp_point.z = (int)FIX_Z_VALUE(temp_point.z);

    if(num_targets > 0)
    {
        min_radius = (float)((PTCharacter)target[0])->Radius();

        point_in_relation_to_target.x = (float)(temp_point.x - target_position[0].x);
        point_in_relation_to_target.y = (float)(temp_point.y - target_position[0].y);

        initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));
    }
    else
    {
        min_radius = 50;
        initial_length = 80;
        point_in_relation_to_target.x = 0;
        point_in_relation_to_target.y = 0;
    }
    max_knot_scale = min_radius * 0.4f;
    min_radius *= 1.5f;
    max_radius = min_radius + 1;

// initialize each of our particles to a random position relative
// to the casters position  and give them an initial random direction
    if(level == 4)
    {
        for(i = 0; i<MAX_NUM_PARA;i++)
        {
            pos[i].x =  temp_point.x + ((float)random(-15, 15) / 10);
            pos[i].y = temp_point.y + (float)random(-15, 15) / 10;
            pos[i].z = temp_point.z + 60 + ((float)random(-5, 5) / 10);
            direction[i].x = (float)random(-15,15) / 10;
            direction[i].y = (float)random(-15,15) / 10;
            direction[i].z = (float)random(-5,0) / 100;
            home_radius[i] = min_radius + ((float)random(0, 100 * (int)(max_radius-min_radius)) / 100);
            home_z[i] = 1;
            speed[i] = PARA_BASE_VELOCITY; //+ (((float)random(-10,10)) / 200);
            start_time[i] = count - (i*2);
            if(num_targets == 0)
                current_target[i] = 0;
            else
                current_target[i] = i%num_targets;
        }
    }
    else
    {
        for(i = 0; i<MAX_NUM_PARA;i++)
        {
            pos[i].x =  temp_point.x + ((float)random(-15, 15) / 10);
            pos[i].y = temp_point.y + (float)random(-15, 15) / 10;
            pos[i].z = temp_point.z + 60 + ((float)random(-5, 5) / 10);
            direction[i].x = -2 * (point_in_relation_to_target.x + ((float)random(-55,55))) / initial_length;
            direction[i].y = -2 * (point_in_relation_to_target.y + ((float)random(-55,55))) / initial_length;
            direction[i].z = (float)random(-5,0) / 100;
            home_radius[i] = min_radius + ((float)random(0, 100 * (int)(max_radius-min_radius)) / 100);
            home_z[i] = 1;
            speed[i] = PARA_BASE_VELOCITY; //+ (((float)random(-10,10)) / 200);
            start_time[i] = count - (i*2);
            if(num_targets == 0)
                current_target[i] = 0;
            else
                current_target[i] = i%num_targets;
        }
    }
    scale = 0.2f;   // start them out small
    knot_scale = 0.0f;

    val = 0.04f;

    state = PARA_STATE_GROW;    // and grow 'em

    hit_flag = true;
}

void TParaEffect::Pulse()
{
    int i;
    D3DVECTOR point_on_circle;
    D3DVECTOR next_point_on_circle;
    D3DVECTOR point_in_relation_to_target;
    float initial_length;
    float ratio;
    float angle;
    float temp_angle;
    bool grow = false;

    for(i=0;i<num_targets;i++)  // if there is a target
    {
        target[i]->GetPos(target_position[i]);  // update our target position
        target_position[i].x += 20;
        target_position[i].y += 20;
        target_position[i].z = (int)FIX_Z_VALUE(target_position[i].z);
        ((PTCharacter)target[i])->Disable();        // stop the bad man from moving
        ((PTCharacter)target[i])->Stop();
        ((PTCharacter)target[i])->SetParalize(TRUE);        // stop the bad man from moving
    }

    switch(state)
    {
        case PARA_STATE_GROW:
            scale += 0.2f;      // if we're in the grow state, grow the particles
            if(scale >= 2.0f)   // if the particles are adult, let them move
                state = PARA_STATE_MOVE;
            break;
        case PARA_STATE_SHRINK:
            scale -= 0.2f;      // if we're in the shrink state, shrink the particles
            knot_scale -= PARA_KNOT_SCALE;
            if(scale <= 0.0f)   // once there' to small to see, kill the spell
            {
                for(i = 0;i<num_targets;i++)
                {
                    ((PTCharacter)target[i])->SetParalize(FALSE);       // stop the bad man from moving
                    target[i]->ClearFlag(OF_DISABLED);      // stop the bad man from moving
                }
                if(num_targets > 0 && level == 4 && spell)
                    spell->Damage(target[0]);
                KillThisEffect();
            }
        case PARA_STATE_MOVE:
            if(level == 4)
            {
                if(count < start_time[0])
                {
                    grow = true;
                    point_in_relation_to_target.x = pos[0].x - target_position[current_target[0]].x;
                    point_in_relation_to_target.y = pos[0].y - target_position[current_target[0]].y;

                    initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));

                    acc[0].x = pos[0].x - target_position[0].x * PARA_GRAVITY * 0.01f;
                    acc[0].y = pos[0].y - target_position[0].y * PARA_GRAVITY * 0.01f;
                    acc[0].z = (pos[0].z - (target_position[0].z + 20) + random(-5,5)) * PARA_GRAVITY * 0.01f;

                    // apply the acceleration to the velocity
                    direction[0].x -= acc[0].x;
                    direction[0].y -= acc[0].y;
                    direction[0].z -= acc[0].z;

                    // update the position based on the velocity
                    pos[0].x += direction[0].x;
                    pos[0].y += direction[0].y;
//                  pos[0].z += direction[0].z;

                    if(initial_length < 20 && !hit_flag)
                    {
                        if(spell)
                        {
                            if(target)
                            {
                                spell->Damage(target[current_target[0]]);
                                hit_flag = true;
                            }
                        }
                    }
                    else if(initial_length > 20)
                    {
                        hit_flag = false;
                    }
                }
                for(i = 1; i<MAX_NUM_PARA;i++)
                {
                    if(count < start_time[i])
                    {
                        point_in_relation_to_target.x = pos[i].x - target_position[current_target[i]].x;
                        point_in_relation_to_target.y = pos[i].y - target_position[current_target[i]].y;

                        initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));

                        acc[i].x = ((pos[i].x - target_position[0].x) * PARA_GRAVITY * 0.01f);
                        acc[i].y = ((pos[i].y - target_position[0].y) * PARA_GRAVITY * 0.01f);
                        acc[i].z = (pos[i].z - target_position[0].z + random(-5,5) + 20) * PARA_GRAVITY * 0.01f;

                        // apply the acceleration to the velocity
                        direction[i].x -= acc[i].x;
                        direction[i].y -= acc[i].y;
                        direction[i].z -= acc[i].z;

                        // update the position based on the velocity
                        pos[i].x += direction[i].x;
                        pos[i].y += direction[i].y;
//                      pos[i].z += direction[i].z;
                    }
                }
            }
            else
            {
                point_in_relation_to_target.x = pos[0].x - target_position[current_target[0]].x;
                point_in_relation_to_target.y = pos[0].y - target_position[current_target[0]].y;

                initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));

                ratio = home_radius[0] / initial_length;

                point_on_circle.x = point_in_relation_to_target.x * ratio;
                point_on_circle.y = point_in_relation_to_target.y * ratio;

                angle = (float)atan2(point_on_circle.y, point_on_circle.x);

                angle += speed[0];

                for(i = 0; i<MAX_NUM_PARA;i++)
                {
//                  if(count < start_time[i])
                    {
                        // calculate the circular velocity
                        point_in_relation_to_target.x = pos[i].x - target_position[current_target[i]].x;
                        point_in_relation_to_target.y = pos[i].y - target_position[current_target[i]].y;

                        initial_length = (float)sqrt((point_in_relation_to_target.y*point_in_relation_to_target.y) + (point_in_relation_to_target.x*point_in_relation_to_target.x));

                        ratio = home_radius[i] / initial_length;

                        point_on_circle.x = point_in_relation_to_target.x * ratio;
                        point_on_circle.y = point_in_relation_to_target.y * ratio;

                        temp_angle = (float)atan2(point_on_circle.y, point_on_circle.x);
                        temp_angle += speed[i];

                        if(ratio > 0.75f)
                        {
                            grow = true;
                            angle -= (float)(6.28 / MAX_NUM_PARA);
                            if(temp_angle > angle)
                                angle = (float)(temp_angle + (1.57 / MAX_NUM_PARA));
                        }
                        else
                        {
                            angle = temp_angle;
                        }


                        next_point_on_circle.x = home_radius[i] * (float)cos(angle);
                        next_point_on_circle.y = home_radius[i] * (float)sin(angle);

                        next_point_on_circle.x = next_point_on_circle.x - point_on_circle.x;
                        next_point_on_circle.y = next_point_on_circle.y - point_on_circle.y;

                        // set the acceleration so that particles hone in on the target
                        acc[i].x = (pos[i].x - target_position[current_target[i]].x + random(-25,25)) * PARA_GRAVITY / initial_length;
                        acc[i].y = (pos[i].y - target_position[current_target[i]].y + random(-25,25)) * PARA_GRAVITY / initial_length;
                        acc[i].z = (pos[i].z - (target_position[current_target[i]].z + home_z[i]) + random(-5,5)) * PARA_GRAVITY / initial_length;

                        // apply the acceleration to the velocity
                        ratio = home_radius[i] / initial_length;
                        direction[i].x = (ratio * next_point_on_circle.x) + ((1-ratio) * (direction[i].x - acc[i].x));
                        direction[i].y = (ratio * next_point_on_circle.y) + ((1-ratio) * (direction[i].y - acc[i].y));
                        if(home_z[i] == 1)
                            direction[i].z = -0.3f;
                        else
                            direction[i].z = 0.3f;

                        // update the position based on the velocity
                        pos[i].x += direction[i].x;
                        pos[i].y += direction[i].y;
                        pos[i].z += direction[i].z;
                        if(pos[i].z < target_position[current_target[i]].z + 10)
                            home_z[i] = -1;
                        else if(pos[i].z > target_position[current_target[i]].z + 60)
                            home_z[i] = 1;
                    }
                }
            }
            count--;    // make our effect older
            //if the particles are ready to die, or we don'e have a target, shrink them
            if(count == 0 || num_targets == 0)
                state = PARA_STATE_SHRINK;
            if(grow == true || level == 3)
                if(knot_scale < max_knot_scale && state != PARA_STATE_SHRINK)
                    knot_scale += PARA_KNOT_SCALE;
            break;
    }

    TEffect::Pulse();
}

// ************************
// *  Para Animator Code  *
// ************************

_CLASSDEF(TParaAnimator)
class TParaAnimator : public T3DAnimator
{
protected:
//  The first 3 vectors keep track of each particular particle
public:
    void Initialize();
    BOOL Render();
    TParaAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
};

REGISTER_3DANIMATOR("Paralize1", TParaAnimator)

void TParaAnimator::Initialize()
{
    T3DAnimator::Initialize();

    ((PTParaEffect)inst)->Initialize();

    PS3DAnimObj obj = GetObject(1);
    GetVerts(obj, D3DVT_LVERTEX);
    obj = GetObject(3);
    GetVerts(obj, D3DVT_LVERTEX);
    obj = GetObject(4);
    GetVerts(obj, D3DVT_LVERTEX);
}

BOOL TParaAnimator::Render()
{
    int i;

    SaveBlendState();
    SetBlendState();        // we want a cool glow effect

    PS3DAnimObj obj = GetObject(0);

    if(((PTParaEffect)inst)->level == 2)
        obj = GetObject(2);
    if(((PTParaEffect)inst)->level == 4)
        obj = GetObject(5);

    for(i = 0;i<MAX_NUM_PARA;i++)
    {
        // update the object with each of our particles new position and scale
        obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS;
        obj->scl.x = obj->scl.y = obj->scl.z = D3DVAL(((PTParaEffect)inst)->scale);
        obj->pos.x = ((PTParaEffect)inst)->pos[i].x;
        obj->pos.y = ((PTParaEffect)inst)->pos[i].y;
        obj->pos.z = ((PTParaEffect)inst)->pos[i].z;
    
        RenderObject(obj);
        if(((PTParaEffect)inst)->level == 2)
        {
            obj->pos.z = (120 + ((PTParaEffect)inst)->target_position[0].z) - (((PTParaEffect)inst)->pos[i].z - ((PTParaEffect)inst)->target_position[0].z);
            RenderObject(obj);
        }
    }

    SetAddBlendState();

// Render the Egg
    if(((PTParaEffect)inst)->level == 2)
        obj = GetObject(3);
    else if(((PTParaEffect)inst)->level == 4)
        obj = GetObject(4);
    else
        obj = GetObject(1);

    obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS | OBJ3D_VERTS;

// Do a little texture mapping to animate it
    for(i=0;i<151;i++)
    {
        obj->lverts[i].tu -= ((PTParaEffect)inst)->val;
        obj->lverts[i].tv -= ((PTParaEffect)inst)->val;
    }

    obj->scl.x = obj->scl.y = obj->scl.z = D3DVAL(((PTParaEffect)inst)->knot_scale);
    for(i=0;i<((PTParaEffect)inst)->num_targets;i++)
    {
        obj->pos.x = (float)((PTParaEffect)inst)->target_position[i].x-10;
        obj->pos.y = (float)((PTParaEffect)inst)->target_position[i].y-20;
        obj->pos.z = (float)((PTParaEffect)inst)->target_position[i].z+20;
        RenderObject(obj);
    }
// actually we want to animate the texture a few times
    for(int j = 0;j<2;j++)
    {
        for(i=0;i<151;i++)
        {
            obj->lverts[i].tu -= (((PTParaEffect)inst)->val*40);
            obj->lverts[i].tv -= (((PTParaEffect)inst)->val*40);
        }
        for(i=0;i<((PTParaEffect)inst)->num_targets;i++)
        {
            obj->pos.x = (float)((PTParaEffect)inst)->target_position[i].x-10;
            obj->pos.y = (float)((PTParaEffect)inst)->target_position[i].y-20;
            obj->pos.z = (float)((PTParaEffect)inst)->target_position[i].z+20;
            RenderObject(obj);
        }
    }
    for(i=0;i<151;i++)
    {
        obj->lverts[i].tu += (((PTParaEffect)inst)->val*80);
        obj->lverts[i].tv += (((PTParaEffect)inst)->val*80);
    }


    RestoreBlendState();

    return TRUE;
}

void TParaAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;
    int i;

    size.x = 20;
    size.y = 20;

    if(((PTParaEffect)inst)->level != 3)
    {
        for(i = 0;i<MAX_NUM_PARA;i++)
        {
            effect.x = (int)((PTParaEffect)inst)->pos[i].x;
            effect.y = (int)((PTParaEffect)inst)->pos[i].y;
            effect.z = (int)((PTParaEffect)inst)->pos[i].z;
            WorldToScreen(effect, screen);
            RestoreZ(screen.x - (size.x / 2), screen.y - size.y / 2, size.x, size.y);
        }
    }

    for(i=0;i<((PTParaEffect)inst)->num_targets;i++)
    {
        size.x = 50;
        size.y = 50;
        effect.x = (int)((PTParaEffect)inst)->target_position[i].x;
        effect.y = (int)((PTParaEffect)inst)->target_position[i].y;
        effect.z = (int)((PTParaEffect)inst)->target_position[i].z;
        WorldToScreen(effect, screen);
        RestoreZ(screen.x - (size.x / 2), screen.y - size.y / 2, size.x, size.y);
    }
}


/****************************************************************/
/*                         Mana Drain                           */
/****************************************************************/

enum
{
    MD_STARTSTAGE = 0,
    MD_SUCKSTAGE,
    MD_FINISHSTAGE,
};

enum
{
    MD_DRAINHEALTH = 0,
    MD_DRAINMANA,
};

#define MD_STARTTICKS       (20)
#define MD_SUCKTICKS        (50)
#define MD_FINISHTICKS      (20)

// ********************
// * ManaDrain Effect *
// ********************

_CLASSDEF(TManaDrainEffect)

class TManaDrainEffect : public TEffect
{
protected:
public:
    PTCharacter caster;
    PTCharacter victim;

    int dtype;      // health or mana drain...
    int suckamt;    // how much does this spell suck??  :)

    int stage;      // what stage its at...
    int ticks;      // how many ticks til the next stage...

    TManaDrainEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TManaDrainEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
};

DEFINE_BUILDER("ManaDrain", TManaDrainEffect)
REGISTER_BUILDER(TManaDrainEffect)


void TManaDrainEffect::Initialize()
{
    SetCommandDone(FALSE);

    stage = MD_STARTSTAGE - 1;
    ticks = 0;
}

void TManaDrainEffect::Pulse()
{
    PSSpellVariant variant;

    TEffect::Pulse();


    if (!ticks)
    {
        stage++;

        switch (stage)
        {
            case MD_STARTSTAGE:
                if (spell)  // if this was a spell, get which type, and what level...
                {
                    caster = (PTCharacter)(GetSpell()->GetInvoker());

                    if(caster)
                    {
                        victim = caster->Fighting();
                        if(!victim)
                        {
                            for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
                            {
                                PTCharacter chr = (PTCharacter)i.Item();

                                if (chr == caster)
                                    continue;

                                if (chr->IsDead())
                                    continue;

                                if (caster)
                                    if (caster->IsEnemy(chr))
                                        continue;   // We can't hurt our friends
                    
                                victim = chr;
                                    break;
                            }

                            if(!victim)
                            {
                                KillThisEffect();
                                return;
                            }
                        }
                    }
                    else
                    {
                        KillThisEffect();
                        return;
                    }

                    variant = spell->VariantData();
        
                    suckamt = variant->maxdamage;

                    if(!strcmp(variant->name, "ManaDrain1"))
                    {
                        level = 1;
                        dtype = MD_DRAINMANA;
                    }
                    else if(!strcmp(variant->name, "ManaDrain2"))
                    {
                        level = 2;
                        dtype = MD_DRAINMANA;
                    }
                    else if(!strcmp(variant->name, "LifeDrain1"))
                    {
                        level = 1;
                        dtype = MD_DRAINHEALTH;
                    }
                    else if(!strcmp(variant->name, "LifeDrain2"))
                    {
                        level = 2;
                        dtype = MD_DRAINHEALTH;
                    }
                }
                else
                {
                    KillThisEffect();
                    return;
                }

                ticks = MD_STARTTICKS;
                break;

            case MD_SUCKSTAGE:
                if (dtype == MD_DRAINMANA)
                {
                    if ((caster->Mana() + suckamt) <= caster->MaxMana())
                    {
                        suckamt = (caster->MaxMana() - caster->Mana());
                    }

                    if (suckamt > victim->Mana())
                    {
                        suckamt = victim->Mana();
                    }

                    caster->SetMana(caster->Mana() + suckamt);
                    victim->SetMana(victim->Mana() - suckamt);
                }
                else if(dtype == MD_DRAINHEALTH)
                {
                    if ((caster->Health() + suckamt) <= caster->MaxHealth())
                    {
                        suckamt = (caster->MaxHealth() - caster->Health());
                    }

                    if (suckamt > victim->Health())
                    {
                        suckamt = victim->Health();
                    }

                    caster->SetHealth(caster->Health() + suckamt);
                    victim->SetHealth(victim->Health() - suckamt);
                }

                ticks = MD_SUCKTICKS;
                break;

            case MD_FINISHSTAGE:
                ticks = MD_FINISHTICKS;
                break;

            default:
                KillThisEffect();
                break;
        }
    }       

    ticks--;
}


// *****************************
// *  ManaDrain Animator Code  *
// *****************************


#define MD_MAXSTREAMS       (15)
#define MD_MOVECHUNKS       (10)
#define MD_CONEHEIGHT       (30.0f)
#define MD_TOLERANCE        (18)

enum // this is weird, I know, but it goes: cone, glow, glow, cone...
{
    MD_HEALTHCONE = 0,
    MD_HEALTHGLOW,
    MD_MANAGLOW,
    MD_MANACONE,
};

_CLASSDEF(TManaDrainAnimator)
class TManaDrainAnimator : public T3DAnimator
{
protected:
    PTCharacter victim;
    S3DPoint victim_pos;

    PTCharacter caster;
    S3DPoint caster_pos;

    float     cone_rot;
    float     cone_scl;
    
    int       stream_init[MD_MAXSTREAMS];
    S3DPoint  stream_pos[MD_MAXSTREAMS];
    D3DVECTOR stream_vel[MD_MAXSTREAMS];
    float     stream_xscale[MD_MAXSTREAMS];

public:
    void Initialize();
    BOOL Render();
    TManaDrainAnimator(PTObjectInstance oi) : T3DAnimator(oi) { /* Initialize(); */ }
    void RefreshZBuffer();
    void Animate(BOOL);
};

REGISTER_3DANIMATOR("ManaDrain", TManaDrainAnimator)

void TManaDrainAnimator::Initialize()
{
    T3DAnimator::Initialize();

    for(int i = 0; i < MD_MAXSTREAMS; i++)
        stream_init[i] = (i * 3) + 1;

    cone_rot = 0.0f;
    cone_scl = 0.0f;
}

void TManaDrainAnimator::Animate(BOOL draw)
{
    int i;
    PTSpell spell = ((PTEffect)inst)->GetSpell();
    int parent_stage = ((PTManaDrainEffect)inst)->stage;



    if (spell)
    {       
        victim = ((PTManaDrainEffect)inst)->victim;
        caster = ((PTManaDrainEffect)inst)->caster;
        
        // if a victim exists, get its position...
        if(victim)
        {
            victim->GetPos(victim_pos);
            victim_pos.z = (int)FIX_Z_VALUE(victim_pos.z);
        }
        // if the caster exists, get its position...
        if(caster)
        {
            caster->GetPos(caster_pos);
            caster_pos.z = (int)FIX_Z_VALUE(caster_pos.z);
        }


        if (parent_stage < MD_FINISHSTAGE)
        {
            if (cone_scl < 1.0)
                cone_scl += 0.05f;
        }
        else
        {
            if (cone_scl > 0.0)
                cone_scl -= 0.05f;
        }
        cone_rot += 0.123f;


//      if (parent_stage >= MD_SUCKSTAGE)
            for(i = 0; i < MD_MAXSTREAMS; i++)
            {
                if (stream_init[i] == 1)
                {
                    if (parent_stage != MD_FINISHSTAGE)
                    {
                        // initialize this stream's position...
                        stream_pos[i].x = (int)(victim_pos.x + ((float)(random(-200, 200) / 10.0f)));
                        stream_pos[i].y = (int)(victim_pos.y + ((float)(random(-200, 200) / 10.0f)));
                        stream_pos[i].z = (int)((victim_pos.z + MD_CONEHEIGHT) + ((float)(random(-200, 200) / 10.0f)));

                        stream_vel[i].x = stream_vel[i].y = stream_vel[i].z = 0.0f;

                        stream_init[i] = 0;
                    }

                    // reset the streams size...
                    stream_xscale[i]  = 0.0f;
                }
                else if (stream_init[i] > 1) stream_init[i]--;
                else
                {
                    int dist = ::Distance(stream_pos[i], caster_pos);

                    // set each stream's velocity each frame... that way if the caster or victim move, things will still work.
                    stream_vel[i].x = (float)((caster_pos.x - stream_pos[i].x) / ((dist / random(3, 5)) + 0.001));
                    stream_vel[i].y = (float)((caster_pos.y - stream_pos[i].y) / ((dist / random(3, 5)) + 0.001));
                    stream_vel[i].z = (float)(((caster_pos.z + MD_CONEHEIGHT) - stream_pos[i].z) / ((dist / random(3, 5)) + 0.001));

                    // update position
                    stream_pos[i].x += (int)stream_vel[i].x;
                    stream_pos[i].y += (int)stream_vel[i].y;
                    stream_pos[i].z += (int)stream_vel[i].z;

                    // make the particle grow.
                    if (parent_stage < MD_FINISHSTAGE)
                    {
                        if (stream_xscale[i] < 0.5f)
                            stream_xscale[i] += 0.01f;
                    }
                    else
                    {
                        if (stream_xscale[i] > 0.0f)
                            stream_xscale[i] -= 0.025f;
                    }

                    // if we get close enough, reset this particle...
                    if (dist < MD_TOLERANCE)
                        stream_init[i] = 1;
                }
            }
    }

    T3DAnimator::Animate(draw);
}

BOOL TManaDrainAnimator::Render()
{
    int i;
    D3DVECTOR scale_vector, translate_vector;
    float ang = (float)((ConvertToFacing(caster_pos, victim_pos) + 127) * (3.14159f / 127.0f));


    SaveBlendState();
    SetBlendState();

    PS3DAnimObj glow_obj, cone_obj;

    if (((PTManaDrainEffect)inst)->dtype == MD_DRAINMANA)
    {
        cone_obj = GetObject(MD_MANACONE);
        glow_obj = GetObject(MD_MANAGLOW);
    }
    else if (((PTManaDrainEffect)inst)->dtype == MD_DRAINHEALTH)
    {
        cone_obj = GetObject(MD_HEALTHCONE);
        glow_obj = GetObject(MD_HEALTHGLOW);
    }

    ResetExtents();

    glow_obj->flags |= OBJ3D_MATRIX | OBJ3D_ABSPOS;
    cone_obj->flags |= OBJ3D_MATRIX | OBJ3D_ABSPOS;

    // first the streams (DON'T CROSS THE STREAMS, RAY!!)
    if (((PTManaDrainEffect)inst)->stage >= MD_SUCKSTAGE)
    {
        for(i = 0; i < MD_MAXSTREAMS; i++)
        {
            D3DMATRIXClear(&glow_obj->matrix);

            // scale
            scale_vector.x = scale_vector.y = scale_vector.z = stream_xscale[i];
            D3DMATRIXScale(&glow_obj->matrix, &scale_vector);

            // rotate to face the camera...
            D3DMATRIXRotateZ(&glow_obj->matrix, 30.0f * (3.14159f / 127.0f));
            D3DMATRIXRotateY(&glow_obj->matrix, 45.0f * (3.14159f / 127.0f));

            // translate last
            translate_vector.x = (float)stream_pos[i].x;
            translate_vector.y = (float)stream_pos[i].y;
            translate_vector.z = (float)stream_pos[i].z;
            D3DMATRIXTranslate(&glow_obj->matrix, &translate_vector);

            RenderObject(glow_obj);
        }
    }

    // next, the cone... and glow (since they're in roughly the same position).
    {
        DWORD oldcullstate;
        Scene3D.GetRenderState(D3DRENDERSTATE_CULLMODE, &oldcullstate);
        Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
        
        D3DMATRIXClear(&cone_obj->matrix);
        D3DMATRIXClear(&glow_obj->matrix);
        
        // scale the glow object...
        scale_vector.x = cone_scl + (float)(random(0, 5) / 10.0f);
        scale_vector.y = scale_vector.z = scale_vector.x;
        D3DMATRIXScale(&glow_obj->matrix, &scale_vector);

        // scale the cone object...
        scale_vector.x = scale_vector.y = scale_vector.z = cone_scl * 2.0f;
        D3DMATRIXScale(&cone_obj->matrix, &scale_vector);

        // rotate the glow object to face the camera...
        D3DMATRIXRotateZ(&glow_obj->matrix, 30.0f * (3.14159f / 127.0f));
        D3DMATRIXRotateY(&glow_obj->matrix, 45.0f * (3.14159f / 127.0f));

        // rotate the cone 90 degrees to face upwards...
        D3DMATRIXRotateZ(&cone_obj->matrix, cone_rot);
        D3DMATRIXRotateX(&cone_obj->matrix, (64.0f * (3.14159f / 127.0f)));

        // push both objects out a little bit...
        translate_vector.y = 1.0f; // 15.0f;
        translate_vector.x = translate_vector.z = 0.0f;
        D3DMATRIXTranslate(&cone_obj->matrix, &translate_vector);

        // now rotate the cone on the z-axis to face the enemy...
        D3DMATRIXRotateZ(&cone_obj->matrix, ang);

        // move both objects to where the character is...
        translate_vector.x = (float)caster_pos.x + (float)(cos(ang) * 15);
        translate_vector.y = (float)caster_pos.y + (float)(sin(ang) * 15);
        translate_vector.z = (float)caster_pos.z + MD_CONEHEIGHT;
        D3DMATRIXTranslate(&cone_obj->matrix, &translate_vector);
        D3DMATRIXTranslate(&glow_obj->matrix, &translate_vector);

        RenderObject(cone_obj);
        UpdateExtents();

        Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullstate);

        RenderObject(glow_obj);
    }

    
    RestoreBlendState();
    return TRUE;
}

void TManaDrainAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 200;
    size.y = 150;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x + (size.x / 2), size.y + (size.y / 2));
}

/****************************************************************/
/*                       Wraith Shriek                          */
/****************************************************************/

// ********************
// *   Shriek Effect  *
// ********************

#define SHRIEK_MAX_SHRIEKS 8
#define SHRIEK_MAX_SHRIEK_SCALE 2.0f

_CLASSDEF(TShriekEffect)

class TShriekEffect : public TEffect
{
protected:
public:
    TShriekEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TShriekEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
};

DEFINE_BUILDER("Shriek", TShriekEffect)
REGISTER_BUILDER(TShriekEffect)


void TShriekEffect::Initialize()
{
    SetCommandDone( FALSE);
}

void TShriekEffect::Pulse()
{
    TEffect::Pulse();
}


// **************************
// *  Shriek Animator Code  *
// **************************

_CLASSDEF(TShriekAnimator)
class TShriekAnimator : public T3DAnimator
{
protected:
    float shriek_scale[SHRIEK_MAX_SHRIEKS];
    int start_time[SHRIEK_MAX_SHRIEKS];
    float shriek_pos[SHRIEK_MAX_SHRIEKS];
    float shriek_fade[SHRIEK_MAX_SHRIEKS];
    int life;
    float anglec;
    float angles;
public:
    void Initialize();
    BOOL Render();
    void Animate(BOOL);
    TShriekAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
};

REGISTER_3DANIMATOR("Shriek", TShriekAnimator)

void TShriekAnimator::Initialize()
{
    int i;
    PS3DAnimObj obj;

    T3DAnimator::Initialize();

    for(i=0;i<SHRIEK_MAX_SHRIEKS;i++)
    {
        shriek_scale[i] = 0.0f;
        start_time[i] = random(0,SHRIEK_MAX_SHRIEKS*4);
        shriek_pos[i] = 0.0f;
        shriek_fade[i] = 1.0f;
    }

    life = 0;

    for(i=0;i<4;i++)
    {
        obj = GetObject(i);
        GetVerts(obj, D3DVT_LVERTEX);
    }

    if(((PTShriekEffect)inst)->GetSpell())
    {
        if(((PTShriekEffect)inst)->GetSpell()->GetInvoker())
        {
            anglec = (float)cos(((PTShriekEffect)inst)->GetSpell()->GetInvoker()->GetFace() * (6.28318 / 256));
            angles = (float)sin(((PTShriekEffect)inst)->GetSpell()->GetInvoker()->GetFace() * (6.28318 / 256));
        }
    }
}

void TShriekAnimator::Animate(BOOL draw)
{
    int i;

    life++;

    for(i=0;i<SHRIEK_MAX_SHRIEKS;i++)
    {
        if(start_time[i] > 0)
            start_time[i]--;
        else if(shriek_scale[i] != 0.0f || life < 50)
        {
            shriek_scale[i] += 0.12f;
            shriek_pos[i] += 1.0f;
            if(shriek_scale[i] > SHRIEK_MAX_SHRIEK_SCALE)
            {
                shriek_scale[i] = 0.0f;
                shriek_pos[i] = 0.0f;
                shriek_fade[i] = 1.0f;
            }
            else if(shriek_scale[i] > (SHRIEK_MAX_SHRIEK_SCALE - .48f))
            {
                shriek_fade[i] -= 0.25f;
            }
        }
    }

    PTSpell spell = ((PTShriekEffect)inst)->GetSpell();
    PTCharacter invoker;
    S3DPoint position;
    int min = 0,max = 0;
    int dtype;
    if(spell)
    {
        invoker = (PTCharacter)spell->GetInvoker();
        min = spell->VariantData()->mindamage;
        max = spell->VariantData()->maxdamage;
        dtype = spell->SpellData()->damagetype;
    }
    inst->GetPos(position);
    
    position.x -= (int)angles * (int)shriek_scale[0] * 8;
    position.y -= (int)anglec * (int)shriek_scale[0] * 8;
    position.y -= (int)shriek_pos[i];

    BlastCharactersInRange(invoker, position, (int)(shriek_scale[0] * 60), min, max, dtype);

    T3DAnimator::Animate(draw);

    if(life > 40)
        ((PTShriekEffect)inst)->KillThisEffect();
}
BOOL TShriekAnimator::Render()
{
    int i;
//  int j;

    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;

    for(i=0;i<SHRIEK_MAX_SHRIEKS;i++)
    {
        obj = GetObject( i%4);

        obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;// | OBJ3D_VERTS;// | OBJ3D_ABSPOS; // | OBJ3D_VERTS;// | OBJ3D_ABSPOS | OBJ3D_VERTS;

        obj->scl.x = obj->scl.y = obj->scl.z = shriek_scale[i];

        obj->pos.x = obj->pos.y = shriek_scale[i] * -4;
        obj->pos.y -= shriek_pos[i];
        obj->pos.z = FIX_Z_VALUE(54.0f);

//      for(j = 0; j < obj->numverts; j++)
//      {
//          obj->lverts[j].color = D3DRGBA(shriek_fade[i],shriek_fade[i],shriek_fade[i],shriek_fade[i]);
//      }

        RenderObject( obj);
    }

    RestoreBlendState();
    return TRUE;
}

void TShriekAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 50;
    size.y = 50;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}






/****************************************************************/
/*                         Zombie Puke                          */
/****************************************************************/

// ********************
// *    Puke Effect   *
// ********************

#define PUKE_MAX_NUM 25

_CLASSDEF(TPukeEffect)

class TPukeEffect : public TEffect
{
protected:
public:
    TPukeEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TPukeEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
};

DEFINE_BUILDER("Puke", TPukeEffect)
REGISTER_BUILDER(TPukeEffect)


void TPukeEffect::Initialize()
{
    SetCommandDone( FALSE);
}

void TPukeEffect::Pulse()
{
    TEffect::Pulse();
}


// **************************
// *   Puke Animator Code   *
// **************************

_CLASSDEF(TPukeAnimator)
class TPukeAnimator : public T3DAnimator
{
protected:
    D3DVECTOR position[PUKE_MAX_NUM];
    D3DVECTOR velocity[PUKE_MAX_NUM];
    int start_time[PUKE_MAX_NUM];
    float scale;
    int life;
    BOOL firsttime;
public:
    void Initialize();
    BOOL Render();
    TPukeAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
    void Animate(BOOL);
};

REGISTER_3DANIMATOR("Puke", TPukeAnimator)

void TPukeAnimator::Initialize()
{
//  int i;

    T3DAnimator::Initialize();

    life = 50;
    scale = 0.1f;

//  PS3DAnimObj obj = GetObject(1);
//    GetVerts(obj, D3DVT_LVERTEX);

    firsttime = TRUE;
}

void TPukeAnimator::Animate(BOOL draw)
{
    int i;

    life--;

    if(life == 30)
    {
      // If no source, use character's hand as the source
        S3DPoint sourcepos;
        sourcepos.x = sourcepos.y = 0;
        sourcepos.z = 50;
        PTSpell spell = ((PTPukeEffect)inst)->GetSpell();
        if(spell)
        {
            PTCharacter invoker = (PTCharacter)spell->GetInvoker();
            if(invoker)
            {
                if (invoker->GetAnimator() /*&& (invoker->GetImagery()->ImageryId() != OBJIMAGE_MESH3D)*/)
                {
                    if (((PT3DAnimator)invoker->GetAnimator())->GetObjectMapPos("warrior_he", sourcepos))
                    {
                        int z = sourcepos.z;
                        ConvertToVector(invoker->GetFace(), Distance(sourcepos), sourcepos);
                        sourcepos.z = z + 50;
                    }
                    else
                    {
                        sourcepos.x = sourcepos.y = 0;
                        sourcepos.z = 50;
                    }
                }
            }
        }
        for(i=0;i<PUKE_MAX_NUM;i++)
        {
            position[i].x = (float)sourcepos.x;
            position[i].y = (float)sourcepos.y;
            position[i].z = (float)sourcepos.z;

            velocity[i].x = (float)random(-5,5) / 10;
            velocity[i].y = -(float)random(35,45) / 10;
            velocity[i].z = -(float)random(20,50) / 30;
            start_time[i] = (int) i >> 4;
        }
    }
    else if(life < 30)
    {
        PTSpell spell = ((PTPukeEffect)inst)->GetSpell();

        for(i=0;i<PUKE_MAX_NUM;i++)
        {
            if(start_time[i] <= 0)
            {
                position[i].x += velocity[i].x;
                position[i].y += velocity[i].y;
                position[i].z += velocity[i].z;
                velocity[i].z -= 0.5f;
                velocity[i].y -= 0.2f;
                if(position[i].z < 10.0f && life > 10)
                {
                    life = 10;
                }
            }
            else
                start_time[i]--;
        }

        if(firsttime)
        {
            if(spell)
            {
                PTCharacter invoker = (PTCharacter)spell->GetInvoker();
                if(invoker)
                {
                    S3DPoint temp_point1, temp_point2;
                    float ang = (float)((6.283f * (float)invoker->GetFace()) / 256);
                    invoker->GetPos(temp_point1);
                    temp_point1.x += (int)((position[0].x * cos(ang)) - (position[0].y * sin(ang)));
                    temp_point1.y += (int)((position[0].x * sin(ang)) + (position[0].y * cos(ang)));
                    temp_point1.z = (int)(position[0].z);
                    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
                    {
                        PTCharacter chr = (PTCharacter)i.Item();

                        if (chr == invoker)
                            continue;

                        if (chr->IsDead())
                            continue;

                        chr->GetPos(temp_point2);
                        if (Distance(temp_point1,temp_point2) > 16)
                            continue;

                        if (invoker && !invoker->IsEnemy(chr))
                            continue;   // We can't hurt our friends
                        
                        spell->Damage(chr);
                        firsttime = FALSE;
                        break;
                    }
                }
            }
        }

        if(life < 10)
        {
            scale -= 0.01f;
            if(scale < 0.0f)
                scale = 0.0f;
        }
        if(life <= 0)
        {
            ((PTEffect)inst)->KillThisEffect();
        }
    }

    T3DAnimator::Animate(draw);
}

BOOL TPukeAnimator::Render()
{
    int i;
    PS3DAnimObj obj = GetObject(0);

    if(life < 30)
    {
        SaveBlendState();
        SetBlendState();

        obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;

        obj->scl.x = obj->scl.y = obj->scl.z = scale;

        for(i=0;i<PUKE_MAX_NUM;i+=3)
        {
            if(start_time[i] <= 0)
            {
                obj->pos.x = position[i].x;
                obj->pos.y = position[i].y;
                obj->pos.z = position[i].z;

                RenderObject(obj);
            }
        }

        obj = GetObject(1);
        obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;

        obj->scl.x = obj->scl.y = obj->scl.z = scale;

        for(i=1;i<PUKE_MAX_NUM;i+=3)
        {
            if(start_time[i] <= 0)
            {
                obj->pos.x = position[i].x;
                obj->pos.y = position[i].y;
                obj->pos.z = position[i].z;

                RenderObject(obj);
            }
        }


        obj = GetObject(2);
        obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2;

        obj->scl.x = obj->scl.y = obj->scl.z = scale;

        for(i=2;i<PUKE_MAX_NUM;i+=3)
        {
            if(start_time[i] <= 0)
            {
                obj->pos.x = position[i].x;
                obj->pos.y = position[i].y;
                obj->pos.z = position[i].z;

                RenderObject(obj);
            }
        }

        RestoreBlendState();
    }

    return TRUE;
}

void TPukeAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 50;
    size.y = 50;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x + (size.x / 2), size.y + (size.y / 2));
}













/****************************************************************/
/*                        Tornado Helper                        */
/****************************************************************/

#define TORNADO_STATE_GROW 1
#define TORNADO_STATE_MOVE 2
#define TORNADO_STATE_SHRINK 3
#define TORNADO_STATE_DONE 4

_STRUCTDEF(STornadoDust)

struct STornadoDust
{
    S3DPoint position;
    D3DVECTOR velocity;
    D3DVECTOR acceleration;
    float fade;
    float scale;
    BOOL used;
};

#define TORNADO_NUM_DUST 20

_CLASSDEF(TTornadoSystem)

class TTornadoSystem
{
private:
    PT3DAnimator animator;              // pointer to the animator used
    PS3DAnimObj object;                 // pointer to the object for the funnel
    PS3DAnimObj dust_object;            // pointer to the object for the dust
    int num_sides;                      // how many sides
    int num_rings;                      // how many verticle segments
    S3DPoint center;                    // center of the tornado
    float height;                       // tornado height
    float magnitude;                    // oscillation magnitude
    D3DVECTOR velocity;                 // velocity of tornado
    float funnel_scale;                 // the scale of the funnel
    float min_radius;
    float max_radius;
    LPD3DVECTOR ring_velocity;
    S3DPoint target_position;
    int *direction;
    float *change;
    int *ring_magnitude;
    int *ring_start_time;
    PSTornadoDust dust;
    int life;
    int vert_position;
public:
    int my_state;
    TTornadoSystem(){}
    ~TTornadoSystem();
    PS3DAnimObj GetTornadoObj(){return object;}
    BOOL Init(PT3DAnimator,PS3DAnimObj,PS3DAnimObj,int,int,S3DPoint,float,float,D3DVECTOR,float,float,S3DPoint);
    void Pulse();
    BOOL Render();
    void GetPos(S3DPoint&);
};


void TTornadoSystem::GetPos(S3DPoint &ret_point)
{
    if(object)
    {
        ret_point.x = (int)object->verts[0].x;
        ret_point.y = (int)object->verts[0].y;
        ret_point.z = (int)object->verts[0].z;
    }
}

BOOL TTornadoSystem::Init(PT3DAnimator ani,PS3DAnimObj o,PS3DAnimObj o2,int ns,int nr,S3DPoint c,float h,float m,D3DVECTOR v,float minr,float maxr,S3DPoint tp)
{
    int i,j,val;

    animator = ani;
    object = o;
    dust_object = o2;
    num_sides = ns;
    num_rings = nr;
    center = c;
    height = h;
    magnitude = m;
    velocity = v;
    min_radius = minr;
    max_radius = maxr;
    target_position = tp;

    life = 0;

    funnel_scale = 0.0f;

    animator->GetVerts(dust_object, D3DVT_LVERTEX);

// Build the actual tornado model
    // Paraphrased GetVerts
    object->numverts = (num_sides * (num_rings + 1));
    object->lverts = new D3DLVERTEX[object->numverts];
    object->verttype = D3DVT_LVERTEX;

    // Paraphrased GetFaces
    object->numfaces = (num_sides * 2 * num_rings);
    if (!object->faces)
        object->faces = new S3DFace[object->numfaces];
    object->texfaces[0] = 0;
    object->texfaces[1] = 0;
    object->numtexfaces[0] = 0;
    object->numtexfaces[1] = object->numfaces;

    ring_velocity = new D3DVECTOR[num_rings+1];
    direction = new int[num_rings+1];
    change = new float[num_rings+1];
    ring_magnitude = new int[num_rings+1];
    ring_start_time = new int[num_rings+1];

    j = 0;
    // Build Vertex index for each Face
    for (i = 0; i < object->numfaces; i++)
    {
        switch(i%2)
        {
            case 0:
                if((j%num_sides) == (num_sides - 1))
                {
                    object->faces[i].v1 = (j%8) + (num_sides * (int)(j / 8));
                    object->faces[i].v2 = object->faces[i].v1 + 1 - num_sides;
                    object->faces[i].v3 = object->faces[i].v1 + num_sides;
                }
                else
                {
                    object->faces[i].v1 = (j%8) + (num_sides * (int)(j / 8));
                    object->faces[i].v2 = object->faces[i].v1 + 1;
                    object->faces[i].v3 = object->faces[i].v1 + num_sides;
                }
                j++;
                break;
            case 1:
                if(((j-1)%num_sides) == (num_sides - 1))
                {
                    object->faces[i].v1 = object->faces[i-1].v1 + 1 - num_sides;
                    object->faces[i].v2 = object->faces[i-1].v3 + 1 - num_sides;
                    object->faces[i].v3 = object->faces[i-1].v3;
                }
                else
                {
                    object->faces[i].v1 = object->faces[i-1].v1 + 1;
                    object->faces[i].v2 = object->faces[i-1].v3 + 1;
                    object->faces[i].v3 = object->faces[i-1].v3;
                }
                break;
        }
    }

    // Build the Verex list
    float ang, radius,height_val;
    for(i=0;i<object->numverts;i++)
    {
        ang = (float)(i%num_sides);
        ang = (float)ang / num_sides;
        ang = ang * 6.28318f;
        radius = max_radius - min_radius;
        radius = radius * (i / num_sides);
        radius = radius / num_rings;
        radius = radius + min_radius;
        height_val = height;
        height_val = height_val * i;
        height_val = (float)(height_val / num_sides);
        height_val = height_val / num_rings;
        object->verts[i].x = center.x + (float)(radius * cos(ang));
        object->verts[i].y = center.y + (float)(radius * sin(ang));
        object->verts[i].z = center.z + (float)height_val;
        object->lverts[i].color = D3DRGBA(1.0f,1.0f,1.0f,0.8f);
//      object->lverts[i].color = D3DRGBA(0.7f,0.5f,0.2f,0.8f);
        object->lverts[i].tu = ((float)(i % num_sides) / (float)num_sides);
        object->lverts[i].tv = ((float)((int)(i / num_sides)) / (float)(num_rings+1));
    }       

    for(i=0;i<(num_rings+1);i++)
    {
        val = i*6;
        change[i] = (float)val;
        if(val > 0)
            direction[i] = -1;
        else
            direction[i] = 1;
        for(j=0;j<num_sides;j++)
        {
            object->verts[(i*num_sides)+j].x += val;
            object->verts[(i*num_sides)+j].y += val;
        }
        ring_start_time[i] = i * 5;
    }

    for(i=0;i<(int)(num_rings+1)/2;i++)
    {
        ring_magnitude[i] = (int)(magnitude * (int)(i)) / num_rings;
    }

    for(i=(int)(num_rings+1)/2;i<(int)(num_rings+1);i++)
    {
        ring_magnitude[i] = (int)(magnitude * (int)((num_rings + 1 - i))) / num_rings;
    }

    object->flags = OBJ3D_MATRIX | OBJ3D_VERTS | OBJ3D_FACES | OBJ3D_OWNSVERTS | OBJ3D_OWNSFACES;
    D3DMATRIXClear( &object->matrix);


// Initialize the tornados dust
    dust = new STornadoDust[TORNADO_NUM_DUST];

    for(i=0;i<TORNADO_NUM_DUST;i++)
    {
        dust[i].used = FALSE;
    }

    my_state = TORNADO_STATE_GROW;

    return TRUE;
}

void TTornadoSystem::Pulse()
{
    int i,j,k;
    float speed;
    BOOL start_dust = FALSE;

    life++;

    if(my_state == TORNADO_STATE_GROW)
    {
        if(funnel_scale < 1.0f)
        {
            funnel_scale += 0.1f;
            D3DMATRIXClear( &object->matrix);
            object->scl.x = object->scl.y = object->scl.z = funnel_scale;
            D3DMATRIXScale(&object->matrix, &object->scl);
        }
        else
            my_state = TORNADO_STATE_MOVE;
    }
    else if(my_state == TORNADO_STATE_SHRINK)
    {
        funnel_scale -= 0.1f;
//      D3DMATRIXClear( &object->matrix);
//      object->scl.x = object->scl.y = object->scl.z = funnel_scale;
//      D3DMATRIXScale(&object->matrix, &object->scl);
        if(funnel_scale <= 0.0f)
        {
            funnel_scale = 0.0f;
            my_state = TORNADO_STATE_DONE;
        }
    }
    k=0;
    for(i=0;i<object->numverts;i+=num_sides)
    {
//      valx = random(-3,3);
//      valy = random(-3,3);
        speed = 0.8f;
        if(direction[k] == -1)
        {
            change[k] -= speed;
            if(change[k] < -ring_magnitude[k])
            {
                direction[k] = 1;
            }
            for(j=0;j<num_sides;j++)
            {
                object->verts[i+ j].x -= speed;
                object->verts[i+ j].y -= speed;
    //          object->verts[i+ j].z += velocity.z;
                object->lverts[i + j].tu += 0.06f;
            }
        }
        else
        {
            change[k] += speed;
            if(change[k] > ring_magnitude[k])
            {
                direction[k] = -1;
            }
            for(j=0;j<num_sides;j++)
            {
                object->verts[i+ j].x += speed;
                object->verts[i+ j].y += speed;
    //          object->verts[i+ j].z += velocity.z;
                object->lverts[i + j].tu += 0.06f;
            }
        }
        if(ring_start_time[k] <= 0)
        {
            for(j=0;j<num_sides;j++)
            {
                object->verts[i + j].x += velocity.x;
                object->verts[i + j].y += velocity.y;
            }
        }
        else
            ring_start_time[k]--;
        k++;
    }

// handle the dust

    if(life < 5)
    {
        start_dust = TRUE;
    }
    else
    {
        life = 0;
    }
    for(i=0;i<TORNADO_NUM_DUST;i++)
    {
        if(dust[i].used == TRUE)
        {
            dust[i].position.x += (int)dust[i].velocity.x;
            dust[i].position.y += (int)dust[i].velocity.y;
            dust[i].position.z += (int)dust[i].velocity.z;

            if(dust[i].velocity.x != 0.0f)
                dust[i].velocity.x += dust[i].acceleration.x;
            if(dust[i].velocity.x != 0.0f)
                dust[i].velocity.y += dust[i].acceleration.y;
            dust[i].velocity.z += dust[i].acceleration.z;

            dust[i].scale += 0.02f;
            dust[i].fade -= 0.025f;

            if(dust[i].fade <= 0.0f)
            {
                dust[i].used = FALSE;
            }
        }
        else if(start_dust)
        {
            start_dust = FALSE;

            dust[i].fade = 0.5f;
            dust[i].scale = 0.1f;
            dust[i].used = TRUE;

            dust[i].position.x = (int)object->verts[0].x + random(-1,1);
            dust[i].position.y = (int)object->verts[0].y + random(-1,1);
            dust[i].position.z = (int)object->verts[0].z;

            if(random(0,1) == 1)
            {
                dust[i].velocity.x = 0.0f;
                dust[i].velocity.y = 0.0f;
                dust[i].velocity.z = 1.5f;

                dust[i].acceleration.x = 0.0f;
                dust[i].acceleration.y = 0.0f;
                dust[i].acceleration.z = 0.1f;
            }
            else
            {
                dust[i].velocity.x = 0.0f;
                dust[i].velocity.y = 0.0f;
                dust[i].velocity.z = 1.5f;

                dust[i].acceleration.x = 0.0f;
                dust[i].acceleration.y = 0.0f;
                dust[i].acceleration.z = 0.1f;
            }
        }
    }
}


TTornadoSystem::~TTornadoSystem()
{
    delete ring_velocity ;
    delete direction;
    delete change;
    delete ring_magnitude;
    delete ring_start_time;
    delete dust;
}

BOOL TTornadoSystem::Render()
{
    int i,j;

    SaveBlendState();
    SetBlendState();

    DWORD oldcullmode;
    Scene3D.GetRenderState(D3DRENDERSTATE_CULLMODE, &oldcullmode);
    Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);


//  object->scl.x = object->scl.y = object->scl.z = funnel_scale;

    animator->RenderObject(object);

//  GetExtents(&extents);
//  AddUpdateRect(&extents, UPDATE_RESTORE);
//  UpdateBoundingRect(&extents);

    Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullmode);

    for(i=0;i<TORNADO_NUM_DUST;i++)
    {
        if(dust[i].used)
        {
            dust_object->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_VERTS;
            dust_object->pos.x = (float)dust[i].position.x;
            dust_object->pos.y = (float)dust[i].position.y;
            dust_object->pos.z = (float)dust[i].position.z;

            dust_object->scl.x = dust[i].scale;
            dust_object->scl.y = dust[i].scale;
            dust_object->scl.z = dust[i].scale;

            for(j=0;j<dust_object->numverts;j++)
            {
                dust_object->lverts[j].color = D3DRGBA(0.5f,0.4f,0.4f,dust[i].fade);
            }
        
            animator->RenderObject(dust_object);
        }
    }

    RestoreBlendState();

    return TRUE;
}








/****************************************************************/
/*                        Funnel Spell                         */
/****************************************************************/


_CLASSDEF(TInvisibleEffect)

class TFunnelEffect : public TEffect
{
protected:
    float angles;   // the sin of the angle
    float anglec;   // the cos of the angle
public:
    int life;
    TFunnelEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TFunnelEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
};

DEFINE_BUILDER("Funnel", TFunnelEffect)
REGISTER_BUILDER(TFunnelEffect)

_CLASSDEF(TFunnelAnimator)
class TFunnelAnimator : public T3DAnimator
{
protected:
public:
    PTTornadoSystem my_tornado;
    void Initialize();
    BOOL Render();
    TFunnelAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
    void Animate(BOOL);
};

REGISTER_3DANIMATOR("Funnel", TFunnelAnimator)


// *************************
// *     Funnel Effect     *
// *************************



void TFunnelEffect::Initialize()
{
    life = 0;
}

void TFunnelEffect::Pulse()
{
    life++;
    if(life == 1)
    {
        int angle;
        angle = GetFace();
        angles = (float)sin(((float)angle * 6.283180) / 255);
        anglec = (float)cos(((float)angle * 6.283180) / 255);
    }
// this is WAY to complicated!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    if(animator)
    {
        if(((PTFunnelAnimator)animator)->my_tornado != NULL)
        {
            if(life >= 100 && ((PTFunnelAnimator)animator)->my_tornado->my_state != TORNADO_STATE_SHRINK && ((PTFunnelAnimator)animator)->my_tornado->my_state != TORNADO_STATE_DONE)
            {
                S3DPoint temp_point, temp_point2;
                
                GetPos(temp_point);
                ((PTFunnelAnimator)animator)->my_tornado->GetPos(temp_point2);

                temp_point.x += (int)((temp_point2.x * anglec) - (temp_point2.y * angles));
                temp_point.y += (int)((temp_point2.x * angles) + (temp_point2.y * anglec));

                ((PTFunnelAnimator)animator)->my_tornado->my_state = TORNADO_STATE_SHRINK;
                BlastCharactersInRange(spell->GetInvoker(),temp_point,50,spell->VariantData()->mindamage,spell->VariantData()->maxdamage,spell->VariantData()->type);
            }
            if(((PTFunnelAnimator)animator)->my_tornado->my_state == TORNADO_STATE_DONE)
            {
                delete ((PTFunnelAnimator)animator)->my_tornado;
                ((PTFunnelAnimator)animator)->my_tornado = NULL;
                KillThisEffect();
            }
            else if(((PTFunnelAnimator)animator)->my_tornado->my_state != TORNADO_STATE_SHRINK)//if((life % 5) == 0)
            {
                S3DPoint temp_point, temp_point2;
                
                GetPos(temp_point);
                ((PTFunnelAnimator)animator)->my_tornado->GetPos(temp_point2);

                temp_point.x += (int)((temp_point2.x * anglec) - (temp_point2.y * angles));
                temp_point.y += (int)((temp_point2.x * angles) + (temp_point2.y * anglec));

                if(MapPane.GetWalkHeight(temp_point) > (temp_point.z + 5) || MapPane.GetWalkHeight(temp_point) == 0)
                {
                    if(spell)
                    {
                        if(spell->GetInvoker())
                        {
                            S3DPoint temp_point3;
                            spell->GetInvoker()->GetPos(temp_point3);
                            if(::Distance(temp_point,temp_point3) > 10)
                            {
                                ((PTFunnelAnimator)animator)->my_tornado->my_state = TORNADO_STATE_SHRINK;
                                BlastCharactersInRange(spell->GetInvoker(),temp_point,10,spell->VariantData()->mindamage,spell->VariantData()->maxdamage,spell->VariantData()->type);
                            }
                        }
                        else
                        {
                                ((PTFunnelAnimator)animator)->my_tornado->my_state = TORNADO_STATE_SHRINK;
                                BlastCharactersInRange(spell->GetInvoker(),temp_point,10,spell->VariantData()->mindamage,spell->VariantData()->maxdamage,spell->VariantData()->type);
                        }
                    }
                    else
                    {
                            ((PTFunnelAnimator)animator)->my_tornado->my_state = TORNADO_STATE_SHRINK;
                            BlastCharactersInRange(spell->GetInvoker(),temp_point,10,spell->VariantData()->mindamage,spell->VariantData()->maxdamage,spell->VariantData()->type);
                    }
                }
                else
                {
                    if(spell)
                    {
                        PTCharacter invoker;
                        invoker = (PTCharacter)spell->GetInvoker();

                        S3DPoint temp_point3;

                        // See if we've hit a character
                        for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
                        {
                            PTCharacter chr = (PTCharacter)i.Item();

                            if (chr == invoker)
                                continue;

                            if (chr->IsDead())
                                continue;

                            chr->GetPos(temp_point3);

                            if(::Distance(temp_point,temp_point3) > 10)
                                continue;

                            if (invoker && !invoker->IsEnemy(chr))
                                continue;   // We can't hurt our friends

                            ((PTFunnelAnimator)animator)->my_tornado->my_state = TORNADO_STATE_SHRINK;
                            BlastCharactersInRange(spell->GetInvoker(),temp_point,10,spell->VariantData()->mindamage,spell->VariantData()->maxdamage,spell->VariantData()->type);
                            
                            break;
                        }
                    }
                }
            }
        }
    }
}



// *******************************
// *    Funnel Animator Code     *
// *******************************

void TFunnelAnimator::Initialize()
{
    PS3DAnimObj obj,obj2;

    T3DAnimator::Initialize();

    my_tornado = new TTornadoSystem;


    S3DPoint c;
    S3DPoint t;
    D3DVECTOR v;
    v.x = 0;
    v.y = -1.3f;
    v.z = 0.0f;
    c.x = c.y = c.z = 0;
    t.x = t.y = 22;
    t.z = 0;
    obj = GetObject(0);
    obj2 = GetObject(1);
    my_tornado->Init(this,obj,obj2,8,5,c,150,25,v,5,55,t);

}

void TFunnelAnimator::Animate(BOOL draw)
{

    if(my_tornado)
        my_tornado->Pulse();

    T3DAnimator::Animate(draw);
}

BOOL TFunnelAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    if(my_tornado)
        my_tornado->Render();

    RestoreBlendState();

    return TRUE;
}

void TFunnelAnimator::RefreshZBuffer()
{
}














/****************************************************************/
/*                       Invisible Spell                        */
/****************************************************************/

// *************************
// *    Invisible Effect   *
// *************************

#define INVIS_MAX_LIFE 300
#define INVIS_RING_MAX_LIFE 30
#define INVIS_RING_MAX_NUM 6
#define INVIS_STATE_HIDE 1
#define INVIS_STATE_WAIT 2
#define INVIS_STATE_SHOW 3
#define INVIS_MAX_NUM 30

_CLASSDEF(TInvisibleEffect)

class TInvisibleEffect : public TEffect
{
protected:
public:
    int life;
    TInvisibleEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TInvisibleEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
    void OffScreen();
};

DEFINE_BUILDER("Invisible", TInvisibleEffect)
REGISTER_BUILDER(TInvisibleEffect)


void TInvisibleEffect::Initialize()
{
    SetCommandDone( FALSE);
    life = 0;
}

void TInvisibleEffect::Pulse()
{
    TEffect::Pulse();
    
    if(life == 0)
    {
        if(spell)
        {
            if(spell->GetInvoker())
            {
                ((PTCharacter)spell->GetInvoker())->SetInvisibleSpell(TRUE);
            }
        }
    }

    life++;

    if(life >= INVIS_MAX_LIFE)
    {
        KillThisEffect();
        if(spell)
        {
            if(spell->GetInvoker())
            {
                ((PTCharacter)spell->GetInvoker())->SetInvisibleSpell(FALSE);
            }
        }
    }
}

void TInvisibleEffect::OffScreen()
{
}

// *******************************
// *   Invisible Animator Code   *
// *******************************

_CLASSDEF(TInvisibleAnimator)
class TInvisibleAnimator : public T3DAnimator
{
protected:
    D3DVECTOR position[INVIS_RING_MAX_NUM];
    float scale[INVIS_RING_MAX_NUM];
    int start_time[INVIS_RING_MAX_NUM];
    int my_state;
    PTCharacter invoker;
    float rotation;
    S3DPoint particle_position[INVIS_MAX_NUM];
    int particle_velocity[INVIS_MAX_NUM];
    float particle_scale[INVIS_MAX_NUM];
public:
    void Initialize();
    BOOL Render();
    TInvisibleAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
    void Animate(BOOL);
};

REGISTER_3DANIMATOR("Invisible", TInvisibleAnimator)

void TInvisibleAnimator::Initialize()
{
    int i;
    PS3DAnimObj obj;

    T3DAnimator::Initialize();

    obj = GetObject(0);
    GetVerts(obj, D3DVT_LVERTEX);

    rotation = 0;

    for(i=0;i<INVIS_MAX_NUM;i++)
    {
        particle_position[i].x = random(-10,10);
        particle_position[i].y = random(-10,10);
        particle_position[i].z = 0;
        particle_velocity[i] = 2;
        particle_scale[i] = (float)random(20,30) / 10.0f;
    }


    for(i=0;i<INVIS_RING_MAX_NUM;i++)
    {
        start_time[i] = (int)(i / 2) * 8;
        position[i].x = position[i].y = 0;
        scale[i] = 0.0f;
        if((i%2) == 0)
            position[i].z = 0.0f;
        else
            position[i].z = 80.0f;
    }

    my_state = INVIS_STATE_HIDE;

    PTSpell spell = ((PTInvisibleEffect)inst)->GetSpell();
    if(spell)
    {
        invoker = (PTCharacter)spell->GetInvoker();
        invoker->SetFade(100,5,30);
    }

}

void TInvisibleAnimator::Animate(BOOL draw)
{
    int i;

    if(invoker)
        invoker->UpdateFade();

    rotation += 0.2f;

    switch(my_state)
    {
        case INVIS_STATE_HIDE:
            for(i=0;i<INVIS_RING_MAX_NUM;i+=2)
            {
                start_time[i]--;
                if(start_time[i] <= 0)
                {
                    if(position[i].z < 20.0f)
                    {
                        position[i].z += 2.0f;
                        scale[i] += 0.05f;
                        if(scale[i] > 0.8f)
                            scale[i] = 0.8f;
                    }
                    else if(position[i].z < 40.0f)
                    {
                        position[i].z += 2.0f;
                        scale[i] -= 0.05f;
                        if(scale[i] < 0.0f)
                            scale[i] = 0.0f;
                    }
                    else
                        position[i].z = 40.0f;
                }
            }
            for(i=1;i<INVIS_RING_MAX_NUM;i+=2)
            {
                start_time[i]--;
                if(start_time[i] <= 0)
                {
                    if(position[i].z > 60.0f)
                    {
                        position[i].z -= 2.0f;
                        scale[i] += 0.05f;
                        if(scale[i] > 0.8f)
                            scale[i] = 0.8f;
                    }
                    else if(position[i].z > 40.0f)
                    {
                        position[i].z -= 2.0f;
                        scale[i] -= 0.05f;
                        if(scale[i] < 0.0f)
                            scale[i] = 0.0f;
                    }
                    else
                        position[i].z = 40.0f;
                }
            }
            for(i=0;i<INVIS_MAX_NUM;i++)
            {
                particle_position[i].z += particle_velocity[i];
                particle_scale[i] -= 0.4f;
            }
            if(position[INVIS_RING_MAX_NUM - 1].z == 40.0f)
            {
                my_state = INVIS_STATE_WAIT;
                for(i=0;i<INVIS_RING_MAX_NUM;i++)
                {
                    start_time[i] = (int)(i / 2) * 8;
                }
                for(i=0;i<INVIS_MAX_NUM;i++)
                {
                    particle_position[i].x = random(-10,10);
                    particle_position[i].y = random(-10,10);
                    particle_position[i].z = 0;
                    particle_velocity[i] = 2;
                    particle_scale[i] = (float)random(20,30) / 10.0f;
                }
            }
            break;
        case INVIS_STATE_WAIT:
            if(((PTInvisibleEffect)inst)->life > (INVIS_MAX_LIFE - 40))
            {
                my_state = INVIS_STATE_SHOW;
            }
            break;
        case INVIS_STATE_SHOW:
            for(i=0;i<INVIS_RING_MAX_NUM;i+=2)
            {
                start_time[i]--;
                if(start_time[i] <= 0)
                {
                    if(position[i].z > 20.0f)
                    {
                        position[i].z -= 2.0f;
                        scale[i] += 0.05f;
                        if(scale[i] > 0.8f)
                            scale[i] = 0.8f;
                    }
                    else if(position[i].z > 0.0f)
                    {
                        position[i].z -= 2.0f;
                        scale[i] -= 0.05f;
                        if(scale[i] < 0.0f)
                            scale[i] = 0.0f;
                    }
                    else
                        position[i].z = 0.0f;
                }
            }
            for(i=1;i<INVIS_RING_MAX_NUM;i+=2)
            {
                start_time[i]--;
                if(start_time[i] <= 0)
                {
                    if(position[i].z < 60.0f)
                    {
                        position[i].z += 2.0f;
                        scale[i] += 0.05f;
                        if(scale[i] > 0.8f)
                            scale[i] = 0.8f;
                    }
                    else if(position[i].z < 80.0f)
                    {
                        position[i].z += 2.0f;
                        scale[i] -= 0.05f;
                        if(scale[i] < 0.0f)
                            scale[i] = 0.0f;
                    }
                    else
                        position[i].z = 80.0f;
                }
            }
            for(i=0;i<INVIS_MAX_NUM;i++)
            {
                particle_position[i].z += particle_velocity[i];
                particle_scale[i] -= 0.4f;
            }
            if(((PTInvisibleEffect)inst)->life <= (INVIS_MAX_LIFE - 20))
            {
                if(invoker)
                    invoker->SetFade(30,-5);
            }
            break;
    }

    T3DAnimator::Animate(draw);
}

BOOL TInvisibleAnimator::Render()
{
    int i;
    PS3DAnimObj obj = GetObject(0);
    SaveBlendState();
    SetBlendState();

    if(my_state == INVIS_STATE_HIDE || my_state == INVIS_STATE_SHOW)
    {

        if(invoker)
        {
            S3DPoint invoker_position;
            invoker->GetPos(invoker_position);

            obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;// | OBJ3D_VERTS;

            for(i=0;i<INVIS_RING_MAX_NUM;i++)
            {
                obj->scl.x = obj->scl.y = obj->scl.z = scale[i];
                
                obj->rot.x = obj->rot.y = 0.0f;
                obj->rot.z = rotation;

                obj->pos.x = position[i].x + invoker_position.x;
                obj->pos.y = position[i].y + invoker_position.y;
                obj->pos.z = position[i].z + invoker_position.z;

                RenderObject(obj);
            }
        }
        else
        {
            obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3;// | OBJ3D_VERTS;

            for(i=0;i<INVIS_RING_MAX_NUM;i++)
            {
                obj->scl.x = obj->scl.y = obj->scl.z = scale[i];

                obj->rot.x = obj->rot.y = 0.0f;
                obj->rot.z = rotation;

                obj->pos.x = position[i].x;
                obj->pos.y = position[i].y;
                obj->pos.z = position[i].z;

                RenderObject(obj);
            }
        }
        obj = GetObject(1);
        obj->flags = OBJ3D_SCL1 | OBJ3D_POS2;
        for(i=0;i<INVIS_MAX_NUM;i++)
        {
            obj->pos.x = (float)particle_position[i].x;
            obj->pos.y = (float)particle_position[i].y;
            obj->pos.z = (float)particle_position[i].z;

            obj->scl.x = obj->scl.y = obj->scl.z = particle_scale[i];

            RenderObject(obj);
        }
    }

    RestoreBlendState();

    return TRUE;
}

void TInvisibleAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 100;
    size.y = 100;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}




















/****************************************************************/
/*                          Rest Spell                          */
/****************************************************************/

// *************************
// *       Rest Effect     *
// *************************

#define REST_MAX_NUM 40
#define REST_STATE_INTAKE 1
#define REST_STATE_EXPLODE 2

_CLASSDEF(TRestEffect)

class TRestEffect : public TEffect
{
protected:
public:
    TRestEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TRestEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
    void OffScreen();
};

DEFINE_BUILDER("Rest", TRestEffect)
REGISTER_BUILDER(TRestEffect)

_CLASSDEF(TRestAnimator)
class TRestAnimator : public T3DAnimator
{
protected:
    float ball_scale;
    float ball_fade;
    D3DVECTOR velocity[REST_MAX_NUM];
    S3DPoint position[REST_MAX_NUM];
    int life;
    int my_state;
public:
    void Initialize();
    BOOL Render();
    TRestAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
    void Animate(BOOL);
};

REGISTER_3DANIMATOR("Rest", TRestAnimator)


void TRestEffect::Initialize()
{
    SetCommandDone( FALSE);
}

void TRestEffect::Pulse()
{
    TEffect::Pulse();
    
}

void TRestEffect::OffScreen()
{
}



// *******************************
// *      Rest Animator Code     *
// *******************************

void TRestAnimator::Initialize()
{
    int i;
    int range;
    float xratio,yratio,zratio;

    T3DAnimator::Initialize();

    PS3DAnimObj obj = GetObject(0);
    GetVerts(obj,D3DVT_LVERTEX);

    life = 0;
    my_state = REST_STATE_INTAKE;
    ball_scale = 0.0f;
    ball_fade = 1.0f;

    for(i=0;i<REST_MAX_NUM;i++)
    {
        range = random(400,550);

        xratio = (float)random(10,30) / 100.0f;
        yratio = (float)random(10,30) / 100.0f;

        zratio = 1.0f - xratio - yratio;

        if(random(0,1) == 1)
        {
            position[i].x = (int)(range * xratio);
            velocity[i].x = range * xratio / 15;
        }
        else
        {
            position[i].x = (int)(-range * xratio);
            velocity[i].x = -range * xratio / 15;
        }
        if(random(0,1) == 1)
        {
            position[i].y = (int)(range * yratio);
            velocity[i].y = range * yratio / 15;
        }
        else
        {
            position[i].y = (int)(-range * yratio);
            velocity[i].y = -range * yratio / 15;
        }
        position[i].z = 30 + (int)((range - 300) * zratio);
        velocity[i].z = (range - 300) * zratio / 15;
    }
}

void TRestAnimator::Animate(BOOL draw)
{
    int i;
    T3DAnimator::Animate(draw);

    life++;

    if(life > 15)
        my_state = REST_STATE_EXPLODE;

    switch(my_state)
    {
        case REST_STATE_INTAKE:
            for(i=0;i<REST_MAX_NUM;i++)
            {
                position[i].x -= (int)velocity[i].x;
                position[i].y -= (int)velocity[i].y;
                position[i].z -= (int)velocity[i].z;
            }
            break;
        case REST_STATE_EXPLODE:
            ball_scale += 0.3f;
            ball_fade -= 0.075f;
            if(ball_fade <= 0.0f)
            {
                ball_fade = 0.0f;
                ((PTRestEffect)inst)->KillThisEffect();
            }
            break;
    }
}

BOOL TRestAnimator::Render()
{
    int i;

    PS3DAnimObj obj = GetObject(0);
    PS3DAnimObj obj2 = GetObject(1);

    SaveBlendState();
    SetBlendState();

    switch(my_state)
    {
        case REST_STATE_INTAKE:
            obj2->flags |= OBJ3D_SCL1 | OBJ3D_POS2;
            for(i=0;i<REST_MAX_NUM;i++)
            {
                obj2->scl.x = obj2->scl.y = obj2->scl.z = 0.1f;

                obj2->pos.x = (float)position[i].x;
                obj2->pos.y = (float)position[i].y;
                obj2->pos.z = (float)position[i].z;
                RenderObject(obj2);
            }
            break;
        case REST_STATE_EXPLODE:
            obj->flags |= OBJ3D_SCL1 | OBJ3D_VERTS;

            obj->scl.x = obj->scl.y = obj->scl.z = ball_scale;

            for(i=0;i<obj->numverts;i++)
            {
                obj->lverts[i].color = D3DRGBA(0.85f,0.45f,0.20f,ball_fade);
            }
            RenderObject(obj);
            break;
    }

    RestoreBlendState();

    return TRUE;
}

void TRestAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 200;
    size.y = 200;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}























/****************************************************************/
/*                        Cataclysm Spell                       */
/****************************************************************/

#define CATA_STATE_START 1
#define CATA_STATE_FALL 2
#define CATA_STATE_BOOM 3
#define CATA_MAX_NUM 40
#define CATA_MAX_NUM_DUST 70
#define CATA_MAX_NUM_GlOW 50

_CLASSDEF(TCataclysmEffect)

class TCataclysmEffect : public TEffect
{
protected:
    int life;
    PTCharacter target;
public:
    int my_state;
    D3DVECTOR velocity;
    S3DPoint position;
    TCataclysmEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TCataclysmEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
    void OffScreen();
    int GetState(){return my_state;}
    void SetUp();
};

DEFINE_BUILDER("Cataclysm", TCataclysmEffect)
REGISTER_BUILDER(TCataclysmEffect)

_CLASSDEF(TCataclysmAnimator)
class TCataclysmAnimator : public T3DAnimator
{
protected:
    float xrotation;
    float yrotation;
    float zrotation;
    float boom_scale;
    float boom_fade;
    D3DVECTOR velocity[CATA_MAX_NUM];
    S3DPoint position[CATA_MAX_NUM];
    float zacceleration[CATA_MAX_NUM];
    float scale[CATA_MAX_NUM];
    D3DVECTOR dust_acceleration[CATA_MAX_NUM_DUST];
    D3DVECTOR dust_velocity[CATA_MAX_NUM_DUST];
    S3DPoint dust_position[CATA_MAX_NUM_DUST];
    float dust_scale[CATA_MAX_NUM_DUST];
    float dust_size[CATA_MAX_NUM_DUST];
    float glow_scale[CATA_MAX_NUM_GlOW];
    S3DPoint glow_position[CATA_MAX_NUM_GlOW];
public:
    void Initialize();
    BOOL Render();
    TCataclysmAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
    void Animate(BOOL);
    void SetUp();
};

REGISTER_3DANIMATOR("Cataclysm", TCataclysmAnimator)



// *************************
// *   Cataclysm Effect    *
// *************************

void TCataclysmEffect::SetUp()
{
    if(random(0,1) == 0)
    {
        position.x = random(150,200);
        position.y = -random(150,200);
    }
    else
    {
        position.x = -random(150,200);
        position.y = random(150,200);
    }
    velocity.x = (float)position.x / 20.0f;
    velocity.y = (float)position.y / 20.0f;
    position.z = 350;
    velocity.z = (float)position.z / 20.0f;
    int distance;
    PTCharacter invoker = NULL;
    target = NULL;
    if(spell)
    {
        invoker = (PTCharacter)spell->GetInvoker();
    }
    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
    {
        PTCharacter chr = (PTCharacter)i.Item();

        if (chr == invoker)
            continue;

        distance = ::Distance(pos, chr->Pos());
        if (distance > 400)
            continue;

        if (chr->IsDead())
            continue;

        if (invoker && !invoker->IsEnemy(chr))
            continue;   // We can't hurt our friends
        
        if(chr->IsDoing(ACTION_IMPACT))     
            continue;

        target = chr;
        break;
    }

    if(target)
    {
        S3DPoint temp_point;
        target->GetPos(temp_point);
        position.x += temp_point.x;
        position.y += temp_point.y;
        position.z += temp_point.z;
    }
    else if(invoker)
    {
        S3DPoint temp_point;
        invoker->GetPos(temp_point);
        position.x += temp_point.x;
        position.y += temp_point.y;
        position.z += temp_point.z;
    }               

    life = 0;
    my_state = CATA_STATE_FALL;

}

void TCataclysmEffect::Initialize()
{
    SetCommandDone( FALSE);
    my_state = CATA_STATE_START;
}

void TCataclysmEffect::Pulse()
{
    switch(my_state)
    {
        case CATA_STATE_START:
            SetUp();
            break;
        case CATA_STATE_FALL:
            life++;
            position.x -= (int)velocity.x;
            position.y -= (int)velocity.y;
            position.z -= (int)velocity.z;
            if(life >= 20)
            {
                if(spell)
                {
                    S3DPoint temp_point;
                    if(spell->GetInvoker())
                    {
                        if(target)
                        {
                            target->GetPos(temp_point);
                            spell->Damage(target);
                        }
                        else
                            spell->GetInvoker()->GetPos(temp_point);
                        BlastCharactersInRange(spell->GetInvoker(),temp_point,400,spell->VariantData()->mindamage,spell->VariantData()->maxdamage,spell->VariantData()->type);
                    }
                }
                my_state = CATA_STATE_BOOM;
                ((PTCataclysmAnimator)animator)->SetUp();
            }
            break;
        case CATA_STATE_BOOM:
            life++;
            if(life >= 80)
                KillThisEffect();
            break;
    }

    TEffect::Pulse();
}

void TCataclysmEffect::OffScreen()
{
}



// *******************************
// *      Rest Animator Code     *
// *******************************

void TCataclysmAnimator::SetUp()
{
    int i;

    for(i=0;i<CATA_MAX_NUM;i++)
    {
        position[i].x = random(0,50) + ((PTCataclysmEffect)inst)->position.x;
        position[i].y = random(0,50) + ((PTCataclysmEffect)inst)->position.y;
        position[i].z = random(0,50) + ((PTCataclysmEffect)inst)->position.z;
    }

    for(i=0;i<CATA_MAX_NUM_DUST;i++)
    {
        dust_position[i].x = 0 + ((PTCataclysmEffect)inst)->position.x;//random(0,50);
        dust_position[i].y = 0 + ((PTCataclysmEffect)inst)->position.y;//random(0,50);
        dust_position[i].z = 0 + ((PTCataclysmEffect)inst)->position.z;//random(0,50);
    }
}

void TCataclysmAnimator::Initialize()
{
    int i;
    PS3DAnimObj obj;

    T3DAnimator::Initialize();

    obj = GetObject(1);
    GetVerts(obj,D3DVT_LVERTEX);
    obj = GetObject(2);
    GetVerts(obj,D3DVT_LVERTEX);

    xrotation = (float)random(0,10) / 10.0f;
    yrotation = (float)random(0,10) / 10.0f;
    zrotation = (float)random(0,10) / 10.0f;
    boom_scale = 0.0f;
    boom_fade = 1.0f;

    for(i=0;i<CATA_MAX_NUM;i++)
    {
        velocity[i].x = (float)random(5,130) / 10.0f;
        velocity[i].y = (float)random(5,130) / 10.0f;
        velocity[i].z = (float)random(20,70) / 10.0f;
        zacceleration[i] = -0.4f;
        if(random(0,1) == 0)
            velocity[i].x = -velocity[i].x;
        if(random(0,1) == 0)
            velocity[i].y = -velocity[i].y;
        scale[i] = (float)random(4,20) * 0.2f;
    }

    for(i=0;i<CATA_MAX_NUM_DUST;i++)
    {
        dust_velocity[i].x = (float)random(10,120) / 10.0f;
        dust_velocity[i].y = (float)random(10,120) / 10.0f;
        dust_velocity[i].z = (float)random(5,10) / 10.0f;
        dust_acceleration[i].x = -0.5f;
        dust_acceleration[i].y = -0.5f;
        dust_acceleration[i].z = (float)random(2,5) / 10.0f;
        if(random(0,1) == 0)
        {
            dust_velocity[i].x = -dust_velocity[i].x;
            dust_acceleration[i].x = -dust_acceleration[i].x;
        }
        if(random(0,1) == 0)
        {
            dust_velocity[i].y = -dust_velocity[i].y;
            dust_acceleration[i].y = -dust_acceleration[i].y;
        }
        dust_scale[i] = 1.0f;
        dust_size[i] = (float)random(3,6) / 10.0f;
    }

    for(i=0;i<CATA_MAX_NUM_GlOW;i++)
    {
        glow_scale[i] = 0.0f;
    }
}

void TCataclysmAnimator::Animate(BOOL draw)
{
    int i;
    int start_count = 0;

    T3DAnimator::Animate(draw);

    switch(((PTCataclysmEffect)inst)->GetState())
    {
        case CATA_STATE_FALL:
            xrotation += 0.2f;
            yrotation -= 0.2f;
            zrotation += 0.1f;
            for(i=0;i<CATA_MAX_NUM_GlOW;i++)
            {
                if(glow_scale[i] > 0.0f)
                {
                    glow_scale[i] -= 0.15f;
                }
                else
                {
                    if(start_count < 3)
                    {
                        int temp_val;
                        temp_val = random(-40,40);
                        glow_scale[i] = 2.0f;//(float)random(10,20) / 10.0f;
                        glow_position[i].x = ((PTCataclysmEffect)inst)->position.x + temp_val;
                        glow_position[i].y = ((PTCataclysmEffect)inst)->position.y + temp_val;
                        if(abs(temp_val) > 20)
                        {
                            glow_scale[i] = 1.5f;
                        }
                        else if(abs(temp_val) > 10)
                        {
                            glow_scale[i] = 1.8f;
                        }

                        glow_position[i].z = ((PTCataclysmEffect)inst)->position.z;
                        start_count++;
                    }
                    else
                        glow_scale[i] = 0.0f;
                }
            }
            break;
        case CATA_STATE_BOOM:
            for(i=0;i<CATA_MAX_NUM;i++)
            {
                position[i].x += (int)(velocity[i].x);// - (((PTCataclysmEffect)inst)->velocity.x) / 3);
                position[i].y += (int)(velocity[i].y);// - (((PTCataclysmEffect)inst)->velocity.y) / 3);
                position[i].z += (int)velocity[i].z;
                velocity[i].z += zacceleration[i];
                if(scale[i] > 0.0f)
                    scale[i] -= 0.1f;
            }
            for(i=0;i<CATA_MAX_NUM_DUST;i++)
            {
                dust_position[i].z += (int)dust_velocity[i].z;
                if(dust_scale[i] > 0.8f)
                {
                    dust_position[i].x += (int)(dust_velocity[i].x);// - (((PTCataclysmEffect)inst)->velocity.x) / 3);
                    dust_position[i].y += (int)(dust_velocity[i].y);// - (((PTCataclysmEffect)inst)->velocity.y) / 3);
                    dust_velocity[i].x += dust_acceleration[i].x;
                    dust_velocity[i].y += dust_acceleration[i].y;
                    dust_velocity[i].z += dust_acceleration[i].z;
                }
                dust_scale[i] -= 0.03f;
            }
            boom_scale += 0.5f;
            boom_fade -= 0.05f;
            if(boom_fade < 0.0f)
                boom_fade = 0.0f;
            break;
    }
}

BOOL TCataclysmAnimator::Render()
{
    int i,j;    
    PS3DAnimObj obj;

    SaveBlendState();
    SetBlendState();

    switch(((PTCataclysmEffect)inst)->GetState())
    {
        case CATA_STATE_FALL:
            SetAddBlendState();

            obj = GetObject(3);

            obj->flags = OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS;

            for(i=0;i<CATA_MAX_NUM_GlOW;i++)
            {
                if(glow_scale[i] > 0.0f)
                {
                    obj->pos.x = (float)glow_position[i].x;
                    obj->pos.y = (float)glow_position[i].y;
                    obj->pos.z = (float)glow_position[i].z;

                    obj->scl.x = obj->scl.y = obj->scl.z = glow_scale[i];

                    RenderObject(obj);
                }
            }

            SetBlendState();

//          obj = GetObject(3);
//          obj->flags = OBJ3D_SCL1 | OBJ3D_POS2;

//          obj->scl.x = obj->scl.y = obj->scl.z = 7.0f;

//          obj->pos.x = (float)((PTCataclysmEffect)inst)->position.x;
//          obj->pos.y = (float)((PTCataclysmEffect)inst)->position.y;
//          obj->pos.z = (float)((PTCataclysmEffect)inst)->position.z;

//          RenderObject(obj);

            obj = GetObject(0);
            obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;

            obj->scl.x = obj->scl.y = obj->scl.z = 10.0f;

            obj->rot.x = xrotation;
            obj->rot.y = yrotation;
            obj->rot.z = zrotation;

            obj->pos.x = (float)((PTCataclysmEffect)inst)->position.x;
            obj->pos.y = (float)((PTCataclysmEffect)inst)->position.y;
            obj->pos.z = (float)((PTCataclysmEffect)inst)->position.z;

            RenderObject(obj);

            break;
        case CATA_STATE_BOOM:
            obj = GetObject(1);

            obj->flags |= OBJ3D_POS2 | OBJ3D_SCL1 | OBJ3D_VERTS | OBJ3D_ABSPOS;

            obj->scl.x = obj->scl.y = obj->scl.z = boom_scale;

            obj->pos.x = (float)((PTCataclysmEffect)inst)->position.x;
            obj->pos.y = (float)((PTCataclysmEffect)inst)->position.y;
            obj->pos.z = (float)((PTCataclysmEffect)inst)->position.z;

            for(i=0;i<obj->numverts;i++)
            {
                obj->lverts[i].color = D3DRGBA(0.7f,0.5f,0.1f,boom_fade);
            }

            RenderObject(obj);
//          GetExtents(&extents);
//          AddUpdateRect(&extents, UPDATE_RESTORE);
//          UpdateBoundingRect(&extents);

            obj = GetObject(0);
            for(i=0;i<CATA_MAX_NUM;i++)
            {
                if(position[i].z > 0.0f)
                {
                    obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3;

                    obj->pos.x = (float)position[i].x;
                    obj->pos.y = (float)position[i].y;
                    obj->pos.z = (float)position[i].z;

                    obj->rot.x = obj->rot.y = obj->rot.z = 0.0f;

                    switch(i%9)
                    {
                        case 0:
                            obj->rot.x = xrotation;
                        case 1:
                            obj->rot.y = xrotation;
                        case 2:
                            obj->rot.z = xrotation;
                        case 3:
                            obj->rot.x = yrotation;
                        case 4:
                            obj->rot.y = yrotation;
                        case 5:
                            obj->rot.z = yrotation;
                        case 6:
                            obj->rot.x = zrotation;
                        case 7:
                            obj->rot.y = zrotation;
                        case 8:
                            obj->rot.z = zrotation;
                    }

                    obj->scl.x = obj->scl.y = obj->scl.z = scale[i];

                    RenderObject(obj);
                }
            }

            for(i=0;i<CATA_MAX_NUM_DUST;i++)
            {
                if(dust_scale[i] > 0.0f)
                {
                    obj = GetObject(2);
                    obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_VERTS | OBJ3D_ABSPOS;

                    obj->scl.x = obj->scl.y = obj->scl.z = dust_size[i];

                    obj->pos.x = (float)dust_position[i].x;
                    obj->pos.y = (float)dust_position[i].y;
                    obj->pos.z = (float)dust_position[i].z;
                    for(j=0;j<obj->numverts;j++)
                    {
                        obj->lverts[j].color = D3DRGBA(0.3f,0.3f,0.3f,dust_scale[i]);
                    }
                    RenderObject(obj);
                }
            }


            break;
    }

    RestoreBlendState();

    return TRUE;
}

void TCataclysmAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 600;
    size.y = 600;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}























/****************************************************************/
/*                     Ice Storm Spell                          */
/****************************************************************/

// *************************
// *    Ice Storm Effect   *
// *************************


_CLASSDEF(TIceStormEffect)

class TIceStormEffect : public TMissileEffect
{
protected:
public:
    TIceStormEffect(PTObjectImagery newim) : TMissileEffect(newim) { Initialize(); }
    TIceStormEffect(PSObjectDef def, PTObjectImagery newim) : TMissileEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
    void OffScreen();
};

DEFINE_BUILDER("IceStorm", TIceStormEffect)
REGISTER_BUILDER(TIceStormEffect)

_CLASSDEF(TIceStormAnimator)
class TIceStormAnimator : public T3DAnimator
{
protected:
    float ball_scale;
    float ball_fade;
    D3DVECTOR velocity[REST_MAX_NUM];
    S3DPoint position[REST_MAX_NUM];
    int life;
    int my_state;
public:
    void Initialize();
    BOOL Render();
    TIceStormAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
    void Animate(BOOL);
};

REGISTER_3DANIMATOR("IceStorm", TIceStormAnimator)


void TIceStormEffect::Initialize()
{
    TMissileEffect::Initialize();

    SetSpeed(16);

    pos.z += 50;
}

void TIceStormEffect::Pulse()
{
    TMissileEffect::Pulse();
    
    if(state == MISSILE_EXPLODE)
    {
        if(spell)
        {
            PTCharacter invoker = (PTCharacter)spell->GetInvoker();
            if(invoker)
            {
                S3DPoint temp_point;
                GetPos(temp_point);
                BlastCharactersInRange(invoker, temp_point, 50, spell->VariantData()->mindamage, spell->VariantData()->maxdamage, spell->VariantData()->type);
            }
        }
        KillThisEffect();
    }
}

void TIceStormEffect::OffScreen()
{
}



// *******************************
// *   Ice Storm Animator Code   *
// *******************************

void TIceStormAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PS3DAnimObj obj = GetObject(0);
    GetVerts(obj,D3DVT_LVERTEX);

    ((PTIceStormEffect)inst)->SetStatus(TRUE);
}

void TIceStormAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

}

BOOL TIceStormAnimator::Render()
{
    PS3DAnimObj obj = GetObject(0);

    SaveBlendState();
    SetAddBlendState();

    obj->flags = OBJ3D_POS1 | OBJ3D_ABSPOS;

    S3DPoint temp_point;

    inst->GetPos(temp_point);
    obj->pos.x = (float)temp_point.x;
    obj->pos.y = (float)temp_point.y;
    obj->pos.z = (float)temp_point.z;
    RenderObject(obj);

    obj->pos.x -= 10;
    obj->pos.y -= 20;
    RenderObject(obj);

    obj->pos.x -= 10;
    obj->pos.y += 40;
    RenderObject(obj);

    obj->pos.x -= 10;
    RenderObject(obj);

    obj->pos.x -= 10;
    obj->pos.y -= 40;
    RenderObject(obj);

    RestoreBlendState();

    return TRUE;
}

void TIceStormAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 100;
    size.y = 100;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}
