// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 effect3.cpp - Third Effects module                   *
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
/*                      RockStorm Effect                        */
/****************************************************************/

// *************************
// *    RockStorm Effect   *
// *************************


#define ROCKSTORM_MINROCKS      (4)
#define ROCKSTORM_VARROCKS      (3)
#define ROCKSTORM_MAXROCKS      (ROCKSTORM_MINROCKS + ROCKSTORM_VARROCKS)

#define ROCKSTORM_MROCKID       (0) // id for the normal rock
#define ROCKSTORM_SROCKID       (1) // id for the pebble
#define ROCKSTORM_LROCKID       (2) // id for the large rock
#define ROCKSTORM_MGLOWID       (3) // id for the normal glow
#define ROCKSTORM_LGLOWID       (4) // id for the large glow

enum
{
    ROCKSTORM_START,
    ROCKSTORM_STAGE1,
    ROCKSTORM_DAMAGE,
    ROCKSTORM_STAGE2,
    ROCKSTORM_DONE,
};

_CLASSDEF(TRockStormEffect)

class TRockStormEffect : public TEffect
{
protected:
public:
    int stage;
    TRockStormEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TRockStormEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
};

DEFINE_BUILDER("RockStorm", TRockStormEffect)
REGISTER_BUILDER(TRockStormEffect)


void TRockStormEffect::Initialize()
{
    stage = ROCKSTORM_START;
    SetCommandDone( FALSE);
}

void TRockStormEffect::Pulse()
{
    TEffect::Pulse();

    if (stage == ROCKSTORM_DAMAGE)
    {
        PTCharacter target = (PTCharacter)((PTCharacter)spell->GetInvoker())->Fighting();
        spell->Damage(target);
    }
}


// *******************************
// *   RockStorm Animator Code   *
// *******************************

_CLASSDEF(TRockStormAnimator)
class TRockStormAnimator : public T3DAnimator
{
protected:
    int         level, stage, num_rocks;
    PTCharacter target;
    S3DPoint    target_pos;
    float       min_radius;

    float radius[ROCKSTORM_MAXROCKS];
    float ang_vel[ROCKSTORM_MAXROCKS];
    float ang[ROCKSTORM_MAXROCKS];

    float dest_height[ROCKSTORM_MAXROCKS + 1];
    float lin_vel[ROCKSTORM_MAXROCKS + 1];
    float height[ROCKSTORM_MAXROCKS + 1];

    float scl_fac[ROCKSTORM_MAXROCKS + 1];
public:
    void Initialize();
    BOOL Render();
    TRockStormAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
    void Animate(BOOL);
};

REGISTER_3DANIMATOR("RockStorm", TRockStormAnimator)

void TRockStormAnimator::Initialize()
{
    int i;

    T3DAnimator::Initialize();

    PTSpell spell = ((PTRockStormEffect)inst)->GetSpell();


    if(spell)
    {
        // get the target of the spell...
        target = (PTCharacter)((PTCharacter)spell->GetInvoker())->Fighting();
        if(target)
        {           
            // if there was a target, get its position...
            target->GetPos(target_pos);
        }
        else
        {
            stage = ROCKSTORM_DONE;
        }
    }

    if (stage != ROCKSTORM_DONE)
    {
        if(strcmp(spell->VariantData()->name, "RockStorm") == 0)
        {
            level = 1;
            num_rocks = ROCKSTORM_MINROCKS + random(0, ROCKSTORM_VARROCKS);
            stage = ROCKSTORM_STAGE1;
        }
        else if(strcmp(spell->VariantData()->name, "RockStorm2") == 0)
        {
            level = 2;
            num_rocks = ROCKSTORM_MAXROCKS;

            lin_vel[ROCKSTORM_MAXROCKS] = 0.0f;
            height[ROCKSTORM_MAXROCKS] = 150.0f;
            dest_height[ROCKSTORM_MAXROCKS] = 45.0f;
            scl_fac[ROCKSTORM_MAXROCKS] = 0.001f;

            stage = ROCKSTORM_START;
        }

        min_radius = (float)target->Radius() + 5.0f;
    
        for(i=0; i < num_rocks; i++)
        {
            lin_vel[i] = 0.25;
            scl_fac[i] = 1.0f + ((float)random(-5, 10) / 10.0f);
            ang[i] = (float)random(0, 359);

            switch (level)
            {
                case 1:
                    height[i]       = -5.0f;
                    dest_height[i]  = (float)random(45, 55);
                    radius[i]       = (float)random(50, 60);
                    ang_vel[i]      = 0.1f + ((float)random(0, 5) / 50.0f);
                    break;

                case 2:
                    height[i]       = dest_height[ROCKSTORM_MAXROCKS] + (float)random(0, 10);
                    radius[i]       = (float)random(5, 15);
                    ang_vel[i]      = 0.0f;
                    break;
            }
        }
    }
}

void TRockStormAnimator::Animate(BOOL draw)
{
    int i;
    PTSpell spell = ((PTRockStormEffect)inst)->GetSpell();


    ((PTRockStormEffect)inst)->stage = stage;

    if(spell)
    {
        switch (stage)
        {
            case ROCKSTORM_START:
                if (scl_fac[ROCKSTORM_MAXROCKS] < 1.0f)
                    scl_fac[ROCKSTORM_MAXROCKS] += 0.05f;
                else
                    stage = ROCKSTORM_STAGE1;
                break;

            case ROCKSTORM_STAGE1:
                switch (level)
                {
                    case 1:
                        for (i = 0; i < num_rocks; i++)
                        {
                            ang[i] += ang_vel[i];

                            if (height[i] < dest_height[i])
                            {
                                height[i] += lin_vel[i];
                                lin_vel[i] += 0.1f;
                            }
                            else
                            {
                                radius[i] -= 0.93f;
                                ang_vel[i] += 0.001f;
                            }
                        }

                        for (i = 0; i < num_rocks; i++)
                        {
                            if (radius[i] > min_radius)
                                break;
                        }

                        if (i == num_rocks)
                        {
                            stage = ROCKSTORM_DAMAGE;
                        }
                        break;

                    case 2:
                        if (height[ROCKSTORM_MAXROCKS] > dest_height[ROCKSTORM_MAXROCKS])
                            height[ROCKSTORM_MAXROCKS] -= lin_vel[ROCKSTORM_MAXROCKS];
                        else
                            stage = ROCKSTORM_DAMAGE;

                        lin_vel[ROCKSTORM_MAXROCKS] += 0.25;
                        break;
                }
                break;
            
            case ROCKSTORM_DAMAGE:
                stage = ROCKSTORM_STAGE2;

                for (i = 0; i < num_rocks; i++)
                    lin_vel[i] = 4.0f;
                break;
            
            case ROCKSTORM_STAGE2:
                for (i = 0; i < num_rocks; i++)
                {
                    if (scl_fac[i] > 0.001f)
                    {
                        scl_fac[i] -= 0.05f;
                    }
                    height[i] += lin_vel[i];
                    lin_vel[i] -= 0.5f;
                    radius[i] += 1.0f;
                }

                for (i = 0; i < num_rocks; i++)
                {
                    if (height[i] > -5.0f)
                        break;
                }

                if (i == num_rocks)
                {
                    stage = ROCKSTORM_DONE;
                }

                break;
            
            case ROCKSTORM_DONE:
                ((PTEffect)inst)->KillThisEffect();
                break;
        }
    }


    T3DAnimator::Animate(draw);
}

BOOL TRockStormAnimator::Render()
{
    int i;
    PS3DAnimObj rock_obj, glow_obj;

    SaveBlendState();

    if ((level == 1) || (stage == ROCKSTORM_STAGE2))
    {
        rock_obj = GetObject(ROCKSTORM_MROCKID);
        glow_obj = GetObject(ROCKSTORM_MGLOWID);

        rock_obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;
        glow_obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;

        glow_obj->rot.x = D3DVAL(-30);
        glow_obj->rot.z = D3DVAL(15);
                
        for (i = 0; i < num_rocks; i++)
        {
            rock_obj->scl.x = rock_obj->scl.y = rock_obj->scl.z = scl_fac[i];

            rock_obj->rot.x = rock_obj->rot.y = rock_obj->rot.z = ang[i];
            
            rock_obj->pos.x = (float)(radius[i] * cos(ang[i])) + target_pos.x;
            rock_obj->pos.y = (float)(radius[i] * sin(ang[i])) + target_pos.y;
            rock_obj->pos.z = height[i];

            if (stage == ROCKSTORM_STAGE1)
            {
                glow_obj->scl.x = glow_obj->scl.z = scl_fac[i] * 1.7f;
                if (height[i] < 10.0f)
                {
                    glow_obj->scl.y = 7.0f;
                    glow_obj->pos.z = 5.0f;
                }
                else
                {
                    glow_obj->scl.y = scl_fac[i] * 1.7f;
                    glow_obj->pos.z = rock_obj->pos.z;
                }

                glow_obj->pos.x = rock_obj->pos.x;
                glow_obj->pos.y = rock_obj->pos.y;

                ResetExtents();
                SetAddBlendState();
                RenderObject(glow_obj);
                UpdateExtents();
            }
            
            ResetExtents();
            SetBlendState();
            RenderObject(rock_obj);
            UpdateExtents();
        }
    }

    if ((level == 2) && (stage <= ROCKSTORM_STAGE1))
    {
        rock_obj = GetObject(ROCKSTORM_LROCKID);
        glow_obj = GetObject(ROCKSTORM_LGLOWID);

        rock_obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;
        glow_obj->flags |= OBJ3D_SCL1 | OBJ3D_ROT2 | OBJ3D_POS3 | OBJ3D_ABSPOS;

        if (stage == ROCKSTORM_START)
        {
            rock_obj->scl.x = rock_obj->scl.y = scl_fac[ROCKSTORM_MAXROCKS];
            rock_obj->scl.z = 1.0f;
            glow_obj->scl.z = scl_fac[ROCKSTORM_MAXROCKS] * 1.25f;
            glow_obj->scl.x = glow_obj->scl.y = 1.7f + ((float)random(-3, 3) / 10);
        }
        else
        {
            rock_obj->scl.x = rock_obj->scl.y = rock_obj->scl.z = scl_fac[ROCKSTORM_MAXROCKS];
            glow_obj->scl.z = scl_fac[ROCKSTORM_MAXROCKS] * 1.7f;
            glow_obj->scl.x = glow_obj->scl.y = 1.7f + ((float)random(-3, 3) / 10);
        }

        rock_obj->rot.x = glow_obj->rot.x = D3DVAL(-30);
        rock_obj->rot.z = glow_obj->rot.z = D3DVAL(15);

        rock_obj->pos.x = glow_obj->pos.x = (float)target_pos.x;
        rock_obj->pos.y = glow_obj->pos.y = (float)target_pos.y;
        rock_obj->pos.z = glow_obj->pos.z = height[ROCKSTORM_MAXROCKS];

        ResetExtents();
        SetAddBlendState();
        RenderObject(glow_obj);
        UpdateExtents();
        
        ResetExtents();
        SetBlendState();
        RenderObject(rock_obj);
        UpdateExtents();
    }
    
    RestoreBlendState();
    return TRUE;
}

void TRockStormAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 150;
    size.y = 150;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x + (size.x / 2), size.y + (size.y / 2));
}


