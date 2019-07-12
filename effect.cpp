// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     effect.cpp - TEffect module                       *
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
#include "player.h"
//#include "charanimator.h"

#define FROST_HEIGHT    70

DWORD SaveBlendMode;
DWORD SaveZWriteEnable;
DWORD SaveZEnable;
DWORD SaveCullMode;
DWORD SaveSrcBlend;
DWORD SaveDestBlend;

void RestoreZ(int x, int y, int width, int height)
{
    SRect r;

    r.left = x;
    r.top = y;
    r.right = x + width - 1;
    r.bottom = y + height - 1;
    Scene3D.RestoreZBuffer(r);
}


//==============================================================================
//    Function : SaveBlendState.
//------------------------------------------------------------------------------
// Description : This saves the current states of several of the 3DRenderState
//               settings which are modified by the Effects animators.
//==============================================================================

BOOL SaveBlendState()
{
    TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, &SaveBlendMode));
    TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_ZWRITEENABLE, &SaveZWriteEnable));
    TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_ZENABLE, &SaveZEnable));
    TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_CULLMODE, &SaveCullMode));
    TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_SRCBLEND, &SaveSrcBlend));
    TRY_D3D(Scene3D.GetRenderState(D3DRENDERSTATE_DESTBLEND, &SaveDestBlend));

    return TRUE;
}

//==============================================================================
//    Function : RestoreBlendState.
//------------------------------------------------------------------------------
// Description : This restores the render states that were previously saved
//               by SaveBlendState.
//==============================================================================

BOOL RestoreBlendState()
{
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, SaveBlendMode));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, SaveZWriteEnable));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, SaveZEnable));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, SaveCullMode));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, SaveSrcBlend));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, SaveDestBlend));

    return TRUE;
}

//==============================================================================
//    Function : SetBlendState.
//------------------------------------------------------------------------------
// Description : This sets several of the render states settings to values used
//               by most of the Effect animatiors.  This will need to be
//               modified to set the appropriate settings depending on the
//               capabilities of the users 3D card.
//==============================================================================

BOOL SetBlendState()
{
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE));
    //if (D3DHWCaps == "FIRE GL 1000")
    //{
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA));
    //}

    return TRUE;
}

BOOL SetAddBlendState()
{
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_DECALALPHA));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE));

    return TRUE;
}

// Utility function for hurting folks (could probably be a lot better, but this will do for now)
void DamageCharactersInRange(PTObjectInstance invoker, RS3DPoint pos, int range, int minamount, int maxamount, int type)
{
    PTCharacter invchar;

    if (invoker && 
        invoker->ObjClass() == OBJCLASS_CHARACTER || invoker->ObjClass() == OBJCLASS_PLAYER)
      invchar = (PTCharacter)invoker;
    else
      invchar = NULL;

    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
    {
        PTCharacter chr = (PTCharacter)i.Item();

        if (chr == invchar)
            continue;

        if (Distance(pos, chr->Pos()) > range)
            continue;

        if (chr->IsDead())
            continue;

        if (invchar && !invchar->IsEnemy(chr))
            continue;   // We can't hurt our friends
        
        chr->Damage(random(minamount, maxamount), type);
    }
}

// Utility function for hurting folks (could probably be a lot better, but this will do for now)
void BlastCharactersInRange(PTObjectInstance invoker, RS3DPoint pos, int range, int minamount, int maxamount, int type, int interior_range)
{
    PTCharacter invchar;
    int distance;

    if (invoker && 
        invoker->ObjClass() == OBJCLASS_CHARACTER || invoker->ObjClass() == OBJCLASS_PLAYER)
      invchar = (PTCharacter)invoker;
    else
      invchar = NULL;

    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
    {
        PTCharacter chr = (PTCharacter)i.Item();

        if (chr == invchar)
            continue;

        distance = Distance(pos, chr->Pos());
        if (distance > range || distance < interior_range)
            continue;

        if (chr->IsDead())
            continue;

        if (invchar && !invchar->IsEnemy(chr))
            continue;   // We can't hurt our friends
        
        if(chr->IsDoing(ACTION_IMPACT))     
            continue;

        chr->Damage(random(minamount, maxamount), type);
        chr->KnockBack(pos);
    }
}

void BurnCharactersInRange(PTObjectInstance invoker, RS3DPoint pos, int range)
{
    PTCharacter invchar;

    if (invoker && 
        invoker->ObjClass() == OBJCLASS_CHARACTER || invoker->ObjClass() == OBJCLASS_PLAYER)
      invchar = (PTCharacter)invoker;
    else
      invchar = NULL;

    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
    {
        PTCharacter chr = (PTCharacter)i.Item();

        if (chr == invchar)
            continue;

        if (Distance(pos, chr->Pos()) > range)
            continue;

        if (chr->IsDead())
            continue;

        if (invchar && !invchar->IsEnemy(chr))
            continue;   // We can't hurt our friends
        
        chr->Burn();
    }
}

void PulpCharactersInRange(PTObjectInstance invoker, RS3DPoint pos, int range)
{
    PTCharacter invchar;

    if (invoker && 
        invoker->ObjClass() == OBJCLASS_CHARACTER || invoker->ObjClass() == OBJCLASS_PLAYER)
      invchar = (PTCharacter)invoker;
    else
      invchar = NULL;

    for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
    {
        PTCharacter chr = (PTCharacter)i.Item();

        if (chr == invchar)
            continue;

        if (Distance(pos, chr->Pos()) > range)
            continue;

        if (chr->IsDead())
            continue;

        if (invchar && !invchar->IsEnemy(chr))
            continue;   // We can't hurt our friends
        
        S3DPoint cpos;
        chr->GetPos(cpos);
        PulpGuy(chr, cpos, pos);
    }
}

#define PULP_SPEEDMAGNITUDE     2.0f

void PulpGuy(PTCharacter ch, S3DPoint pos, S3DPoint blast)
{
    S3DPoint vel;

    vel.x = pos.x - blast.x;
    vel.x /= 4;
    vel.y = pos.y - blast.y;
    vel.y /= 4;
    vel.z = pos.z - blast.z;
    vel.z = random(7, 12);

    float most = (float)(max(vel.x, vel.y), vel.z), velx, vely, velz;
    // normalize the vector and multiply by speed
    if (most > 0)
    {
        velx = vel.x / most;
        vely = vel.y / most;
        velz = vel.z / most;
        velx *= PULP_SPEEDMAGNITUDE;
        vely *= PULP_SPEEDMAGNITUDE;
        velz *= PULP_SPEEDMAGNITUDE;
        vel.x = (int)velx;
        vel.y = (int)vely;
        vel.z = (int)velz;
    }

    int count = random(6, 16);
    ch->Pulp(vel, count, 5);//count * 30);
}

TObjectClass EffectClass("EFFECT", OBJCLASS_EFFECT, 0);
extern TObjectClass TileClass;

// ***********
// * TEffect *
// ***********

DEFINE_BUILDER("effect", TEffect)
REGISTER_BUILDER(TEffect)

void TEffect::Pulse()
{
    TObjectInstance::Pulse();

    SetFrame(0);
    SetCommandDone(FALSE);
}

void TEffect::KillThisEffect()
{
    if (spell)
        spell->Kill();  // signal death of this effect to parent spell

    flags |= OF_KILL | OF_PULSE;// flag it for deletion next frame
}

int TEffect::GetAngle()
{
    S3DPoint targetpos;

    if(spell)
    {
        PTObjectInstance invoker = spell->GetInvoker();
        PTObjectInstance target = spell->GetTarget();

        if ((target == NULL) || (target == invoker))
            angle = invoker->GetFace();
        else
        {
            target->GetPos(targetpos);
            // Aim the missile (in the X Y plane)
            angle = ConvertToFacing(pos, targetpos);
        }
    }
    else
        angle = 0;
    return angle;
}

// ***************
// * THealEffect *
// ***************

DEFINE_BUILDER("Heal", THealEffect)
REGISTER_BUILDER(THealEffect)

void THealEffect::Initialize()
{
    first_time = TRUE;
}

void THealEffect::Pulse()
{
    TEffect::Pulse();
    if(first_time)
    {
        if(spell)
        {
            PTCharacter invoker = (PTCharacter)(spell->GetInvoker());

            
            if(strcmp(spell->VariantData()->name, "Heal") == 0)
            {
                level = 1;
            }
            else if(strcmp(spell->VariantData()->name, "Heal2") == 0)
            {
                level = 2;
            }
            else if(strcmp(spell->VariantData()->name, "Heal3") == 0)
            {
                level = 3;
            }
            else if(strcmp(spell->VariantData()->name, "Heal4") == 0)
            {
                level = 4;
                if(invoker)
                    invoker->SetHealth(invoker->MaxHealth());
            }

            if(invoker)
            {
                if(level < 4)
                    invoker->SetHealth(invoker->Health() + spell->VariantData()->maxdamage);

                if(invoker->Health() > invoker->MaxHealth())
                    invoker->SetHealth(invoker->MaxHealth());
            }
            first_time = FALSE;
        }
    }
}

// ******************
// * Flare Animator *
// ******************

REGISTER_3DANIMATOR("Flare", TFlareAnimator)

void TFlareAnimator::Initialize()
{
    T3DAnimator::Initialize();

    for (int c = 0; c < NUMFLARES; c++)
    {
        p[c].x = p[c].y = p[c].z = (D3DVALUE)0;
        v[c].x = v[c].y = v[c].z = (D3DVALUE)0;
    }

    Get3DImagery()->GetMaterial(0, &mat[0]);

    D3DMATERIAL &m = mat[0].matdesc;
    m.ambient.r = D3DVAL(0.0);
    m.ambient.g = D3DVAL(0.0);
    m.ambient.b = D3DVAL(0.0);
    m.ambient.a = D3DVAL(0.0);
    m.diffuse.r = D3DVAL(0.0);
    m.diffuse.g = D3DVAL(0.0);
    m.diffuse.b = D3DVAL(0.0);
    m.diffuse.a = D3DVAL(0.0);
    m.specular.r = D3DVAL(0.0);
    m.specular.g = D3DVAL(0.0);
    m.specular.b = D3DVAL(0.0);
    m.specular.a = D3DVAL(0.0);
    m.emissive.r = D3DVAL(0.0);
    m.emissive.g = D3DVAL(0.0);
    m.emissive.b = D3DVAL(0.0);
    m.emissive.a = D3DVAL(0.0);
    m.power = D3DVAL(0.0);
    m.dwRampSize = 0;

    Get3DImagery()->SetMaterial(0, &mat[0]);
}

//void TTest3DAnimator::SetupObjects()
//{
//  T3DAnimator::SetupObjects(); // Don't change
//}

void TFlareAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    for (int c = 0; c < 10; c++)
    {
        if (p[c].z <= 0)
        {
            p[c].z = 0;
            v[c].z = -v[c].z * (D3DVALUE)0.5;
            if (abs(v[c].z) < (D3DVALUE)0.4)
            {
                p[c].x = p[c].y = p[c].z = (D3DVALUE)0;
                v[c].x = (D3DVALUE)(random(0,6) - (D3DVALUE)3) / (D3DVALUE)2.0;
                v[c].y = (D3DVALUE)(random(0,6) - (D3DVALUE)3) / (D3DVALUE)2.0;
                v[c].z = (D3DVALUE)random(5,8);
            }
        }
        v[c].z -= (D3DVALUE)0.5;

        p[c].x += v[c].x;
        p[c].y += v[c].y;
        p[c].z += v[c].z;
    }
}

BOOL TFlareAnimator::Render()
{
    PS3DAnimObj obj = GetObject(0);

    SaveBlendState();
    SetBlendState();

    for (int c = 0; c < 10; c++)
    {
        Get3DImagery()->ResetExtents();             // Reset render extents

        obj->flags |= OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3;
        obj->rot.x = D3DVAL(-(M_PI / 2.0));
        obj->rot.y = D3DVAL(0.0);
        obj->rot.z = D3DVAL(-(M_PI / 4.0));
        obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)4.0;
        obj->pos = p[c];

        RenderObject(obj);

        GetExtents(&extents);
        AddUpdateRect(&extents, UPDATE_BACKGROUND);
        UpdateBoundingRect(&extents);
    }

    RestoreBlendState();

    return TRUE;
}

// *****************
// * Heal Animator *
// *****************

REGISTER_3DANIMATOR("Heal", THealAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for a heal animator. The
//               healing effect consists of a circular object at the players
//               feet and a number of bubbles rising out of it.
//==============================================================================

void THealAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PLAY("Heal");

// figure out which heal we are
    PTSpell spell = ((PTEffect)inst)->GetSpell();
    switch (((PTHealEffect)inst)->level)
    {
        case 2:
            heal_num = 2;
            glow_num = 3;
            break;

        case 3:
            heal_num = 4;
            glow_num = 5;
            break;

        case 4:
            heal_num = 6;
            glow_num = 7;
            break;

        default:
            heal_num = 0;
            glow_num = 1;
            break;
    }

    // change the health level
    if(spell)
    {
        PTCharacter invoker = (PTCharacter)(spell->GetInvoker());


        if(invoker == ((PTCharacter)Player))
            HealthBar.ChangeLevel(invoker->Health() * 1000 / invoker->MaxHealth());
    }

    // Initialize the glow object at the players feet
    p[0].x = p[0].y = p[0].z = 0;
    scale[0] = 2.0;
    framenum[0] = 0; 
    rotation = (D3DVALUE)0.0;

    // Initialize the starting positions of the magic bubbles
    for (int n = 1; n < NUM_HEAL_BUBBLES; n++)
    {
        p[n].x = (D3DVALUE)(random(-HEALING_RADIUS, HEALING_RADIUS));
        p[n].y = (D3DVALUE)(random(-HEALING_RADIUS, HEALING_RADIUS));
        p[n].z = (D3DVALUE)2.0;
        rise[n] = (D3DVALUE)(random(3, 6) * (D3DVALUE)4.0 / (D3DVALUE)3.0);
        scale[n] = 0;
        scale[n] = 4.5;
        framenum[n] = random(-NUM_HEAL_BUBBLES / 2, -1);
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the frame number, position, and scale values
//               of all the healing bubbles.
//==============================================================================

void THealAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    // Update the glow object under the players feet
    framenum[0]++;
    // It grows during the first half of the spell
    if (framenum[0] < HEAL_DURATION / 2)
    {
        if (scale[0] < 7.0)
            scale[0] += 1.0;
    }
    else
    // And shrinks once the spell is done with
    if ((framenum[0] > HEAL_DURATION) && (activebubbles == 0))
    {
        if (scale[0] > 0)
            scale[0] -= 1.0;
        else
        {
            ((PTEffect)inst)->KillThisEffect();
            return;
        }
    }

    // Rotate the cylindrical glow
    rotation += (D3DVALUE)0.1;

    // Update all the magic bubbles
    for (int n = 1; n < NUM_HEAL_BUBBLES; n++)
    {
        framenum[n]++;

        // Keep track of how many bubbles we are floating up
        if (framenum[n] == 0)
            activebubbles++;

        if (framenum[n] > 0)
        {
            // Float the bubbles up
            p[n].z += rise[n];

            if (scale[n] > 0)
            {
                // Scale the bubbles down
                scale[n] -= (D3DVALUE)HEAL_SCALE_STEP;
                if (scale[n] <= 0)
                {
                    // Once a bubbles scales out of sight, decrement how many we have
                    activebubbles--;

                    // If the spells not over yet, start a new bubble
                    if (framenum[0] < HEAL_DURATION)
                    {
                        p[n].x = (D3DVALUE)(random(-HEALING_RADIUS, HEALING_RADIUS));
                        p[n].y = (D3DVALUE)(random(-HEALING_RADIUS, HEALING_RADIUS));
                        p[n].z = (D3DVALUE)2.0;
                        rise[n] = (D3DVALUE)(random(3, 6) * (D3DVALUE)4.0 / (D3DVALUE)3.0);
                        scale[n] = 4.5;
                        framenum[n] = -1;
                    }
                }
            }
        }
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : This loops through all the healing bubbles and renders each
//               at the appropriate location and scale value.
//==============================================================================

BOOL THealAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(heal_num);

    for (int n = 0; n < NUM_HEAL_BUBBLES; n++)
    {
        if ((framenum[n] > 0) && (scale[n] > 0))
        {
            ResetExtents();             // Reset render extents

            // See if were doing the glow at the players feet or one of the bubbles
            // The glow doesn't get rotated, but the bubbles do
            obj->flags = OBJ3D_MATRIX;
            D3DMATRIXClear(&obj->matrix);
            if (n == 0)
            {
                D3DMATRIXRotateX(&obj->matrix, 0);
                D3DMATRIXRotateY(&obj->matrix, 0);
                D3DMATRIXRotateZ(&obj->matrix, 0);
            }
            else
            {
                obj->rot.x = D3DVAL(-(M_PI / 2.0));
                D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
                D3DMATRIXRotateY(&obj->matrix, 0.0f);
                D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
                D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
            }

            obj->scl.x = obj->scl.y = obj->scl.z = scale[n];
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            obj->pos = p[n];
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            RenderObject(obj);

            UpdateExtents();            // Updates bound rect and screen rect
        }
    }

    // Draw the cylindrical glow around the player
    obj = GetObject(glow_num);

    ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3;
    obj->rot.x = obj->rot.y = D3DVAL(0.0);
    obj->rot.z = rotation;
    obj->scl.x = obj->scl.y = obj->scl.z = scale[0] * 3 / 4;
    obj->pos = p[0];

    RenderObject(obj);
    obj->rot.z = -rotation;
    RenderObject(obj);

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

void THealAnimator::RefreshZBuffer()
{
    S3DPoint map, screen, effect;

    ((PTEffect)inst)->GetPos(effect);

    map.x = (int)p[0].x + effect.x;
    map.y = (int)p[0].y + effect.y;
    map.z = (int)p[0].z + effect.z;

    int size_x = 100;
    int size_y = 220;

    WorldToScreen(map, screen);
    RestoreZ(screen.x - (size_x / 2), screen.y - size_y + 25, size_x, size_y);
}

// *********************
// * TCreateFoodEffect *
// *********************

// general defines for create food effect
#define CFD_SOUPS_ON                60      //  time (in frames) at which food appears
#define CFD_NOMOREOFTHISEFFECT      80      //  time when the effect must desist its behavior entirely
#define CFD_NUMPARTICLES            150     //  number of particles used in both flames and swirls

// defines for particles
#define CFD_STATE_USEME             0
#define CFD_STATE_SWIRLEY1          1
#define CFD_STATE_SWIRLEY2          2
#define CFD_STATE_SMOKEY            3

// swirley defines
#define CFD_SWIRLON         3       //  frame when swirling begins
#define CFD_SWIRLOFF        10      //  frame when swirleys stop being added
#define CFD_SMALLSWIRLEY    0.1f    //  smallest size of a swirley
#define CFD_LARGESWIRLEY    0.1f    //  largest size of a swirley
#define CFD_SWIRLSIZEVEL    0.02f   //  how fast the size changes
#define CFD_SWIRLMINDIST    20      //  minimum distance to pivot (stage 1)
#define CFD_SWIRLMINDIST2   15      //  minimum distance to pivot (stage 2)
#define CFD_SWIRLCHANGE     20      //  when zvalue is this, pivot is moved to center above fire
#define CFD_SWIRLDISTFUDGE  CFD_SWIRLMINDIST + 8    //  range for random 'distance to pivot' values
#define CFD_SWIRLDISTVEL    3       //  how fast swirley distance grows (first stage)
#define CFD_SWIRLDISTVEL2   4       //  how fast swirley distance shrinks (second stage)
#define CFD_SWIRLANGVEL     8       //  angular velocity (how fast particles rotate)
#define CFD_SWIRLZVEL1      2       //  up\down velocity for pivot of particle (first stage)
#define CFD_SWIRLZVEL2      2       //  up\down velocity for pivot of particle (second stage)

//  more defines to regulate the flow of the swirleys
#define CFD_SWIRLTIME1      (CFD_SWIRLON + 5)   //  for each of these frames...
#define CFD_ADDSWIRL1       1                   //      add this many particles for swirl action
#define CFD_SWIRLTIME2      (CFD_SWIRLON + 10)  //  for each of these frames...
#define CFD_ADDSWIRL2       1                   //      add this many particles for swirl action
#define CFD_SWIRLTIME3      CFD_SWIRLOFF        //  for each of these frames...
#define CFD_ADDSWIRL3       1                   //      add this many particles for swirl action

// fire defines
#define CFD_SMOKEON         30      //  frame when fire begins
#define CFD_SMOKEOFF        70      //  frame when fire stops
#define CFD_MAXDIST         15      //  maximum radius at the base of the flame
#define CFD_FADEMIN         5       //  min height at which flame particles may begin fading
#define CFD_MAXFADE         15      //  max height at which flame particles must fade out
#define CFD_STARTPCT        20      //  used as a factor for fading smoke
#define CFD_BOTTOM          1       //  distance of flame above ground

//  the following gets the fire in front of the food
#define CFD_FLAMEINFRONT    500     //  offset for base of fire (towards camera- straight down) along the ground plane
#define CFD_FLAMEUPOFFSET   280     //  (56% of INFRONT) offset along Z-axis (straight up) to adjust for FLAMEINFRONT offset

//  more defines to regulate the flow of the flame
#define CFD_SMOKETIME1      40              //  for each of these frames...
#define CFD_ADDSMOKE1       1               //      add this many particles for smoke
#define CFD_MAINSCALE1      0.3f            //      each beginning about this size
#define CFD_SMOKETIME2      55              //  for each of these frames...
#define CFD_ADDSMOKE2       7               //      add this many particles for smoke
#define CFD_MAINSCALE2      0.3f            //      each beginning about this size
#define CFD_SMOKETIME3      65              //  for each of these frames...
#define CFD_ADDSMOKE3       7               //      add this many particles for smoke
#define CFD_MAINSCALE3      0.3f            //      each beginning about this size
#define CFD_SMOKETIME4      CFD_SMOKEOFF    //  for each of these frames...
#define CFD_ADDSMOKE4       2               //      add this many particles for smoke
#define CFD_MAINSCALE4      0.3f            //      each beginning about this size

// defines for the GLOWING GLOVES(tm)
#define CFD_GSCALEVEL       0.2f    //  glowing gloves scale velocity fading while raising\lowering hands
#define CFD_GSCALEFLICKER   0.03f   //  glowing gloves flickering rate while hands raised
#define CFD_MAXGSCALE       1.2f    //  maximum fade value of glowing gloves
#define CFD_GTIME1          10      //  fade-in time while raising hands
#define CFD_GTIME2          20      //  flicker time 1  (fade slightly out)
#define CFD_GTIME3          30      //  flicker time 2  (fade slightly in)
#define CFD_GTIME4          40      //  flicker time 3  (fade slightly out)
#define CFD_GTIME5          50      //  flicker time 4  (fade slightly in)
#define CFD_GTIME6          80      //  fade-out tim while lowering hands

_CLASSDEF(TCreateFoodAnimator)

typedef struct _CFD_PARTICLE
{
    S3DPoint pos;       //  x,y,z position of the particle
    S3DPoint pivot;     //  point around which particle rotates
    S3DPoint vel;       //  this is the velocity, my friend (up, down, sideways translation)
    S3DPoint angvel;    //  angular velocity (rotation speed around its pivot)
    int state;          //  particle is unused, a SWIRLEY, or a SMOKEY
    float size;         //  size of the particle, my friend
    int angle;          //  current angle around pivot point
    int dist;           //  distance from pivot point (or radius)
    int life;           //  the particle is alive (displayed while life > 0) or dead (0)
    int startfade;      //  used to control fading (size of particle)
    int stopfade;       //  used to control fading (size of particle)
    int color;
}CFD_PARTICLE;

class TCreateFoodAnimator : public T3DAnimator
{
  private:
    S3DPoint localfoodpos;
    S3DPoint flameoffset;
    D3DVECTOR glove1;
    D3DVECTOR glove2;
    D3DVECTOR fxpos;
    D3DVECTOR fxscale;
    D3DVALUE facing;
    int fooddist;
    float mainscale;
    float gscale;
    CFD_PARTICLE sm[CFD_NUMPARTICLES];  // array of particles used for both the flame and the swirls

  public:
    S3DPoint foodpos;
    int framenum;
    TCreateFoodAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
    virtual ~TCreateFoodAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};

REGISTER_3DANIMATOR("CreateFood", TCreateFoodAnimator)



_CLASSDEF(TCreateFoodEffect)

class TCreateFoodEffect : public TEffect
{
  public:
    TCreateFoodEffect(PTObjectImagery newim) : TEffect(newim) { }
    TCreateFoodEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }

    virtual void Initialize();
    virtual void Pulse();
};

DEFINE_BUILDER("CreateFood", TCreateFoodEffect)
REGISTER_BUILDER(TCreateFoodEffect)

void TCreateFoodEffect::Initialize()
{
}

void TCreateFoodEffect::Pulse()
{
    TEffect::Pulse();

    if(animator)
    {
        if(((PTCreateFoodAnimator)animator)->framenum == CFD_SOUPS_ON)
        {
            // create the new food object
            PTObjectClass cl = TObjectClass::GetClass(OBJCLASS_FOOD);

            // Get the number of various food types (i.e. watermelon, cheese, beer)
            int num = cl->NumTypes();
            
            SObjectDef def;
            memset(&def, 0, sizeof(SObjectDef));
            def.objclass = OBJCLASS_FOOD;
            def.objtype = random(0, num - 1);
            def.level = MapPane.GetMapLevel();
            def.pos.x = ((PTCreateFoodAnimator)animator)->foodpos.x;
            def.pos.y = ((PTCreateFoodAnimator)animator)->foodpos.y;
            def.pos.z = ((PTCreateFoodAnimator)animator)->foodpos.z;
            int index = MapPane.NewObject(&def);

            if(!stricmp(cl->GetObjType(def.objtype)->name, "bottle"))
                TextBar.Print("got milk?");
            else
                TextBar.Print("Created %s.", cl->GetObjType(def.objtype)->name);
        }
    }
}

// ***********************
// * CreateFood Animator *
// ***********************


void TCreateFoodAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PLAY("heal sound");

    fxscale.x = 1.0f;
    fxscale.y = 1.0f;
    fxscale.z = 1.0f;
    framenum = 0;
    fooddist = 25;

    // Get the direction the fatman is facing.. will be used immediately and later on
    facing = (float)((PTEffect)inst)->GetAngle();

    ConvertToVector((int)facing, fooddist, localfoodpos);

    //  calculate the offset of the flame
    ConvertToVector((int)((255 - facing) + ((138 * 255) / 360)) & 255, CFD_FLAMEINFRONT, flameoffset);
    flameoffset.y -= (int)fooddist;
    flameoffset.z = CFD_FLAMEUPOFFSET;

    // Get the position of the fatman
    S3DPoint fatman;
    ((PTEffect)inst)->GetPos(fatman);

    foodpos.x = fatman.x + localfoodpos.x;
    foodpos.y = fatman.y + localfoodpos.y;
    foodpos.z = fatman.z;

    fxpos.x = 0.0f;
    fxpos.y = (float)-fooddist;
    fxpos.z = 0.0f;
    
    memset(sm, 0, sizeof(CFD_PARTICLE) * CFD_NUMPARTICLES);

//  initialize the glowing gloves (the two effects near fatman's hands)
//  int objnum = ((PTEffect)inst)->GetSpell()->GetInvoker()->GetAnimator()->GetObject("lhand");

//  TextBar.Print("left hand: %d", objnum);

    glove1.x = 20.0f;
    glove1.y = -5.0f;
    glove1.z = 75.0f;

    glove2.x = -20.0f;
    glove2.y = -5.0f;
    glove2.z = 75.0f;

    gscale = 0.01f;
}

void TCreateFoodAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    framenum++;

//  Perform the glowing gloves action
    if(framenum < CFD_GTIME1)
    {
        if(gscale < CFD_MAXGSCALE)
            gscale += CFD_GSCALEVEL;
        else
            gscale = CFD_MAXGSCALE;
    }
    else if(framenum < CFD_GTIME2)
    {
        gscale -= CFD_GSCALEFLICKER;
    }
    else if(framenum < CFD_GTIME3)
    {
        gscale += CFD_GSCALEFLICKER;
    }
    else if(framenum < CFD_GTIME4)
    {
        gscale -= CFD_GSCALEFLICKER;
    }
    else if(framenum < CFD_GTIME5)
    {
        gscale += CFD_GSCALEFLICKER;
    }
    else if(framenum < CFD_GTIME6)
    {
        if(gscale > 0.0f)
            gscale -= CFD_GSCALEVEL;
        else
            gscale = 0.0f;
    }
    else
    {
        gscale = 0.0f;
    }


//  Handle swirl action
    int newswirleys = 0;

    if(framenum < CFD_SWIRLON)
        newswirleys = 0;
    
    else if(framenum < CFD_SWIRLTIME1)
        newswirleys = CFD_ADDSWIRL1;
    
    else if(framenum < CFD_SWIRLTIME2)
        newswirleys = CFD_ADDSWIRL2;
    
    else if(framenum < CFD_SWIRLTIME3)
        newswirleys = CFD_ADDSWIRL3;

//  Handle flame crap
    int newsmokeys = 0;

    if(framenum < CFD_SMOKEON)
    {
        newsmokeys = 0;
    }
    else if(framenum < CFD_SMOKETIME1)
    {
        newsmokeys = CFD_ADDSMOKE1;
        mainscale = CFD_MAINSCALE1;
    }
    else if(framenum < CFD_SMOKETIME2)
    {
        newsmokeys = CFD_ADDSMOKE2;
        mainscale = CFD_MAINSCALE2;
    }   
    else if(framenum < CFD_SMOKETIME3)
    {
        newsmokeys = CFD_ADDSMOKE3;
        mainscale = CFD_MAINSCALE3;
    }
    else if(framenum < CFD_SMOKETIME4)
    {
        newsmokeys = CFD_ADDSMOKE4;
        mainscale = CFD_MAINSCALE4;
    }

    for(int i = 0; i < CFD_NUMPARTICLES; i++)
    {
        if(sm[i].state == CFD_STATE_USEME)
        {
        //  particle is waiting to be assigned for duty 

            if(newswirleys)
            {
            //  add a new SWIRLEY

                sm[i].color = random(0, 1);

                sm[i].pivot.x = 0;
                sm[i].pivot.y = -fooddist;
                sm[i].pivot.z = (int)glove1.z - 10;

                sm[i].angvel.x = 0;
                sm[i].angvel.y = 0;
                sm[i].angvel.z = CFD_SWIRLANGVEL;

                sm[i].dist = random(CFD_SWIRLMINDIST, CFD_SWIRLDISTFUDGE);
                sm[i].angle = random(0, 255);
                sm[i].size = CFD_SMALLSWIRLEY;
                sm[i].state = CFD_STATE_SWIRLEY1;

                newswirleys--;
            }
            else if(newsmokeys)
            {
            //  add a new SMOKEY

                int dist = random(0, CFD_MAXDIST);
                int angle = random(0, 255);
                ConvertToVector(angle, dist, sm[i].pos);
                sm[i].pos.z = CFD_BOTTOM;
                sm[i].life = CFD_FADEMIN + CFD_MAXFADE * (CFD_MAXDIST - random(0, dist)) / CFD_MAXDIST;
                sm[i].startfade = sm[i].life * CFD_STARTPCT / 100;
                sm[i].stopfade = sm[i].life;
                sm[i].vel.z = 2 + random(0, 2);
                sm[i].state = CFD_STATE_SMOKEY;

                newsmokeys--;
            }
        }
        else if(sm[i].state == CFD_STATE_SMOKEY)
        {
        //  particle is a SMOKEY, so make it act like one
            
            sm[i].pos.z += sm[i].vel.z;
            sm[i].life--;

            if(sm[i].life == 0)
                sm[i].state = CFD_STATE_USEME;
        }
        else if(sm[i].state == CFD_STATE_SWIRLEY1)
        {
        //  particle is a SWIRLEY, hovering around the glowing glove

            sm[i].pivot.z += CFD_SWIRLZVEL1;
            
            if(sm[i].size < CFD_LARGESWIRLEY)
                sm[i].size += CFD_SWIRLSIZEVEL;

            sm[i].angle += sm[i].angvel.z;
            sm[i].dist += CFD_SWIRLDISTVEL;
            
            if(sm[i].pivot.z >= glove1.z + CFD_SWIRLCHANGE)
                sm[i].state = CFD_STATE_SWIRLEY2;
        }
        else if(sm[i].state == CFD_STATE_SWIRLEY2)
        {
        //  particle is a SWIRLEY, making its way toward the fire
            sm[i].pivot.z -= CFD_SWIRLZVEL2;
            sm[i].angle += sm[i].angvel.z;
            sm[i].size -= CFD_SWIRLSIZEVEL;

            if(sm[i].dist > CFD_SWIRLMINDIST2)
                sm[i].dist -= CFD_SWIRLDISTVEL2;
            else
                sm[i].dist = CFD_SWIRLMINDIST2;
            
            if(sm[i].pivot.z <= 10)
                sm[i].state = CFD_STATE_USEME;
        }
    }

//  make the food appear when the time is right

    if(framenum >= CFD_NOMOREOFTHISEFFECT)
        ((PTEffect)inst)->KillThisEffect();

    return;
}

BOOL TCreateFoodAnimator::Render()
{
    PS3DAnimObj obj;
    int i;
    
    SaveBlendState();
    SetAddBlendState();

    ResetExtents();

//  All this crap renders the flame
    for(i = 0; i < CFD_NUMPARTICLES; i++)
    {
        if(sm[i].state == CFD_STATE_SMOKEY)
        {
            if((sm[i].stopfade - sm[i].life) >= ((sm[i].stopfade - sm[i].startfade) / 2))
                obj = GetObject(0);
            else
                obj = GetObject(1);

            if(sm[i].life > 0)
            {
                obj->flags = OBJ3D_MATRIX;

                D3DMATRIXClear(&obj->matrix);

                D3DMATRIXRotateX(&obj->matrix, -(float)(M_2PI / 3.0));
                D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
                D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((facing / 256.0f) * M_2PI)));

                float scale;

                if((sm[i].stopfade - sm[i].life) >= sm[i].startfade)
                    scale = mainscale * sm[i].life / (sm[i].stopfade - sm[i].startfade);
                else
                    scale = mainscale;

                obj->scl.x = scale;
                obj->scl.y = scale;
                obj->scl.z = scale;
                D3DMATRIXScale(&obj->matrix, &obj->scl);

                obj->pos.x = (float)sm[i].pos.x + (float)flameoffset.x;
                obj->pos.y = (float)sm[i].pos.y + (float)flameoffset.y;
                obj->pos.z = (float)sm[i].pos.z + (float)flameoffset.z;
                
                D3DMATRIXTranslate(&obj->matrix, &obj->pos);

                RenderObject(obj);
            }
        }
        else if(sm[i].state == CFD_STATE_SWIRLEY1 || sm[i].state == CFD_STATE_SWIRLEY2)
        {
            obj = GetObject(sm[i].color);

            obj->flags = OBJ3D_MATRIX;

            D3DMATRIXClear(&obj->matrix);

            D3DMATRIXRotateX(&obj->matrix, -(float)(M_2PI / 3.0));
            D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
            D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((facing / 256.0f) * M_2PI)));

            obj->scl.x = sm[i].size;
            obj->scl.y = sm[i].size;
            obj->scl.z = sm[i].size;
            D3DMATRIXScale(&obj->matrix, &obj->scl);

            S3DPoint pt;
            ConvertToVector(sm[i].angle, sm[i].dist, pt);

            obj->pos.x = (float)sm[i].pivot.x + pt.x;
            obj->pos.y = (float)sm[i].pivot.y + pt.y;
            obj->pos.z = (float)sm[i].pivot.z;
            
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            RenderObject(obj);
        }
    }

    if(gscale > 0.0f)
    {
    //  Render first glowing glove..    
        obj = GetObject(0);

        obj->flags = OBJ3D_MATRIX;

        D3DMATRIXClear(&obj->matrix);

        D3DMATRIXRotateX(&obj->matrix, -(float)(M_2PI / 3.0));
        D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((facing / 256.0f) * M_2PI)));

        obj->scl.x = gscale;
        obj->scl.y = gscale;
        obj->scl.z = gscale;
        D3DMATRIXScale(&obj->matrix, &obj->scl);

        obj->pos.x = glove1.x;
        obj->pos.y = glove1.y;
        obj->pos.z = glove1.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        RenderObject(obj);

    //  Render glowing glove #2..   
        obj = GetObject(1);

        obj->flags = OBJ3D_MATRIX;

        D3DMATRIXClear(&obj->matrix);

        D3DMATRIXRotateX(&obj->matrix, -(float)(M_2PI / 3.0));
        D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((facing / 256.0f) * M_2PI)));

        obj->scl.x = gscale;
        obj->scl.y = gscale;
        obj->scl.z = gscale;
        D3DMATRIXScale(&obj->matrix, &obj->scl);

        obj->pos.x = glove2.x;
        obj->pos.y = glove2.y;
        obj->pos.z = glove2.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        RenderObject(obj);
    }

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

void TCreateFoodAnimator::RefreshZBuffer()
{
    S3DPoint map, screen, effect;

    int size_x = 200;
    int size_y = 200;

    WorldToScreen(foodpos, screen);
    RestoreZ(screen.x - (size_x / 2), screen.y - size_y, size_x, size_y);
//  Display->Box(screen.x - (size_x / 2), screen.y - size_y, size_x, size_y);
}

// *********************
// * TCureEffect *
// *********************

_CLASSDEF(TCureEffect)

class TCureEffect : public TEffect
{
  private:
    BOOL first_time;
  public:
    TCureEffect(PTObjectImagery newim) : TEffect(newim) { Initialize();}
    TCureEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize();}

    virtual void Initialize();
    virtual void Pulse();
};

DEFINE_BUILDER("Cure", TCureEffect)
REGISTER_BUILDER(TCureEffect)

void TCureEffect::Initialize()
{
    first_time = TRUE;
}

void TCureEffect::Pulse()
{
    if(first_time)
    {
        if(spell)
        {
            PTCharacter invoker = (PTCharacter)spell->GetInvoker();
            if(invoker)
            {
                PTCharacter target = invoker->Fighting();
                if(!target)
                {
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

                        target = chr;
                        break;
                    }
                }
                if(target)
                {
                    S3DPoint target_pos;
                    target->GetPos(target_pos);
                    SetPos(target_pos);
                    target->SetPoisoned(TRUE);
                }
                first_time = FALSE;
            }
        }
    }
    TEffect::Pulse();
}

// ***********************
// * Cure Animator *
// ***********************

// general defines for create food effect
//#define CURE_NOMOREOFTHISEFFECT       80      //  time when the effect must desist its behavior entirely
#define CURE_NUMPARTICLES           80      //  number of particles used
#define CURE_NUMBALLS               5       //  number of balls (particles swirl around balls)
#define CURE_NUMCOLORS              1       //  number of different particle colors (must be supported by imagery, tho)
#define CURE_IS_COLOR_RANDOM        1       //  Is each ball its own color or random colors?
#define CURE_MINZ                   10      //  stops the balls from going thru the ground

// defines for particles
#define CURE_STATE_INACTIVE         0
#define CURE_STATE_BALL             1
#define CURE_STATE_BALL2            2
#define CURE_STATE_SWIRLEY1         3
#define CURE_STATE_DEAD             4

// defines for balls
#define CURE_BALL_STARTDIST         60      //  distance ball is from its pivot
#define CURE_BALL_STARTDISTAMP      10      //  amplitude of in\out distance from pivot
#define CURE_BALL_DISTTHETAVEL      0.04f   //  in\out velocity for distance from pivot
#define CURE_BALL_MIN_STARTANGVEL   5       //  (0-360 degrees) starting angular velocity (min random value)
#define CURE_BALL_MAX_STARTANGVEL   9       //  (0-360 degrees) starting angular velocity (max random value)
#define CURE_BALL_MIN_STARTTIME     0       //  time when ball becomes active (min random value)
#define CURE_BALL_MAX_STARTTIME     15      //  time when ball becomes active (max random value)
#define CURE_BALL_SIZEVEL1          0.1f    //  rate at which ball size grows (stage 1)
#define CURE_BALL_STARTSIZE         10.0f   //  size of ball when first active
#define CURE_BALL_MAINDISTVEL       1.5f
#define CURE_BALL_MERGEVEL          0.5f
#define CURE_BALL_SIZEVEL2          0.0f    //  rate at which ball size shrinks (stage 2)
#define CURE_BALL_MINSIZE2          0.1f
#define CURE_BALL_SIZEVEL3          1.0f

// swirley defines
#define CURE_SWIRL_SMALL            0.01f   //  smallest size of a swirley
#define CURE_SWIRL_LARGE            0.5f    //  largest size of a swirley
#define CURE_SWIRL_SIZEVEL          0.03f   //  how fast the size changes
#define CURE_SWIRL_SIZEAMP          0.2f    //  amplitude of size
#define CURE_SWIRL_STARTDISTAMP     10      //  amplitude of in\out distance from pivot
#define CURE_SWIRL_DISTTHETAVEL     0.2f    //  in\out velocity for distance from pivot
#define CURE_SWIRL_MIN_DIST         3       //  distance to pivot (min random value)
#define CURE_SWIRL_MAX_DIST         20      //  distance to pivot (max random value)
#define CURE_SWIRL_MIN_STARTANGVEL  5       //  (0-360 degrees) starting angular velocity (min random value)
#define CURE_SWIRL_MAX_STARTANGVEL  10      //  (0-360 degrees) starting angular velocity (max random value)
#define CURE_SWIRL_SIZEVEL2         0.05f   //  rate at which ball size shrinks (stage 2)
#define CURE_SWIRL_MINSIZE2         0.01f
#define CURE_SWIRL_DISTVEL2         5       //  rate at which ball dist shrinks (stage 2)
#define CURE_SWIRL_MINDIST2         0
#define CURE_HOWMANYKILLS           1       //  How many swirls are killed per frame
#define CURE_MORESWIRLEYS           1       //  How many swirls are added per frame during the above interval

_CLASSDEF(TCureAnimator)

typedef struct _CURE_PARTICLE
{
    D3DVECTOR pos;      //  x,y,z position of the particle
    D3DVECTOR pivot;    //  point around which particle rotates
    D3DVECTOR angvel;   //  angular velocity in radians (rotation speed around its pivot)
    D3DVECTOR angle;    //  current angle of rotation around pivot point in radians (each axis)
    int state;          //  particle is unused, a SWIRLEY, or a SMOKEY
    float size;         //  size of the particle, my friend
    float dist;         //  distance from pivot point (or radius)
    float maindist;
    float disttheta;    //  factor used to change distance
    int starttime;      //  time at which particle becomes active
    int whichball;      //  which ball the particle is currently attached to
    int color;
    int linedup;
}CURE_PARTICLE;

class TCureAnimator : public T3DAnimator
{
  private:
    int framenum;
    CURE_PARTICLE sm[CURE_NUMPARTICLES];    // array of particles used for both the flame and the swirls
    CURE_PARTICLE balls[CURE_NUMBALLS];
    int numactiveballs;     //  current number of active balls
    int ballslinedup;
    float facing;           //  which way the fatman is facing
    int killing;

  public:
    TCureAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
    virtual ~TCureAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};

REGISTER_3DANIMATOR("Cure", TCureAnimator)

void TCureAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PLAY("heal sound");

    framenum = 0;
    numactiveballs = 0;
    ballslinedup = 0;
    killing = 0;

    // Get the direction the fatman is facing.. will be used immediately and later on
    facing = (float)((PTEffect)inst)->GetAngle();

    memset(sm, 0, sizeof(CURE_PARTICLE) * CURE_NUMPARTICLES);
    memset(balls, 0, sizeof(CURE_PARTICLE) * CURE_NUMBALLS);

    for(int i = 0; i < CURE_NUMBALLS; i++)
    {
    //  balls are given random start times so they don't all start at the same time (duh!)
        if(i == 0)
            balls[i].starttime = 0;
        else
            balls[i].starttime = random(CURE_BALL_MIN_STARTTIME, CURE_BALL_MAX_STARTTIME);

    //  ball is set inactive and will become active later when starttime is reached
        balls[i].state = CURE_STATE_INACTIVE;
    }
}

void TCureAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    framenum++;

    for(int i = 0; i < CURE_NUMBALLS; i++)
    {
        if(balls[i].state == CURE_STATE_INACTIVE)
        {
            if(balls[i].starttime > 0)
                balls[i].starttime--;
            
            if(balls[i].starttime == 0)
            {
            //  the ball wants to start immediately so make it become active
                balls[i].pivot.x = 0;
                balls[i].pivot.y = 0;
                balls[i].pivot.z = 35.0f + (float)((i - (CURE_NUMBALLS / 2)) * (100 / CURE_NUMBALLS));
                balls[i].size = CURE_BALL_STARTSIZE;
                balls[i].angle.x = (float)random(0, 359) / 360.0f * (float)M_2PI;
                balls[i].angle.y = (float)random(0, 359) / 360.0f * (float)M_2PI;
                balls[i].angle.z = (float)random(0, 359) / 360.0f * (float)M_2PI;
                balls[i].maindist = CURE_BALL_STARTDIST;
                balls[i].disttheta = 0.0f;
                
                if(CURE_IS_COLOR_RANDOM)
                    balls[i].color = random(0, CURE_NUMCOLORS - 1);
                else
                    balls[i].color = i % CURE_NUMCOLORS;
                
            //  assign a random rotation velocity around only 2 axes                
                int arandomvalue = random(0, 2);
                
                if(arandomvalue == 0)
                {
                    balls[i].angvel.x = (float)random(CURE_BALL_MIN_STARTANGVEL, CURE_BALL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                    balls[i].angvel.y = (float)random(CURE_BALL_MIN_STARTANGVEL, CURE_BALL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                }
                else if(arandomvalue == 1)
                {
                    balls[i].angvel.x = (float)random(CURE_BALL_MIN_STARTANGVEL, CURE_BALL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                    balls[i].angvel.z = (float)random(CURE_BALL_MIN_STARTANGVEL, CURE_BALL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                }
                else
                {
                    balls[i].angvel.y = (float)random(CURE_BALL_MIN_STARTANGVEL, CURE_BALL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                    balls[i].angvel.z = (float)random(CURE_BALL_MIN_STARTANGVEL, CURE_BALL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                }

                balls[i].state = CURE_STATE_BALL;

                numactiveballs++;
            }
        }

        if(balls[i].state == CURE_STATE_BALL)
        {
            balls[i].angle.x += balls[i].angvel.x;
            balls[i].angle.y += balls[i].angvel.y;
            balls[i].angle.z += balls[i].angvel.z;
            
            balls[i].size += CURE_BALL_SIZEVEL1;
            balls[i].disttheta += CURE_BALL_DISTTHETAVEL;
            balls[i].dist = balls[i].maindist + CURE_BALL_STARTDISTAMP * (float)cos(balls[i].disttheta); 
            
            if(!balls[i].linedup)
            {
                if(balls[i].maindist > 2.0f)
                    balls[i].maindist -= CURE_BALL_MAINDISTVEL;
                else
                {
                    balls[i].maindist = 2.0f;
                    balls[i].linedup = 1;
                    ballslinedup++;

                    if(ballslinedup == CURE_NUMBALLS)
                    {
                        for(int j = 0; j < CURE_NUMBALLS; j++)
                            balls[j].state = CURE_STATE_BALL2;
                    }
                }
            }

            D3DMATRIX mat;
            D3DMATRIXClear(&mat);
            
            D3DMATRIXRotateX(&mat, balls[i].angle.x);
            D3DMATRIXRotateY(&mat, balls[i].angle.y);
            D3DMATRIXRotateZ(&mat, balls[i].angle.z);
            
            D3DVECTOR v;
            
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = balls[i].dist;

            D3DMATRIXTransform(&mat, &v, &v);

            balls[i].pos.x = balls[i].pivot.x + v.x;
            balls[i].pos.y = balls[i].pivot.y + v.y;
            balls[i].pos.z = balls[i].pivot.z + (v.z / 2.0f);

            if(balls[i].pos.z < CURE_MINZ)
                balls[i].pos.z = CURE_MINZ;
        }
        else if(balls[i].state == CURE_STATE_BALL2)
        {
            balls[i].angle.x += balls[i].angvel.x;
            balls[i].angle.y += balls[i].angvel.y;
            balls[i].angle.z += balls[i].angvel.z;

            float sizevel;

            if(balls[i].pivot.z > 35.0f - 5.0f || balls[i].pivot.z < 35.0f + 5.0f)
                sizevel = CURE_BALL_SIZEVEL3;
            else
                sizevel = CURE_BALL_SIZEVEL2;

            if(balls[i].size > CURE_BALL_MINSIZE2)
                balls[i].size -= sizevel;
            else
                balls[i].size = CURE_BALL_MINSIZE2;

            if(balls[i].pivot.z > 35.0f)
                balls[i].pivot.z -= CURE_BALL_MERGEVEL;
            else if(balls[i].pivot.z < 35.0f)
                balls[i].pivot.z += CURE_BALL_MERGEVEL;

            D3DMATRIX mat;
            D3DMATRIXClear(&mat);
            
            D3DMATRIXRotateX(&mat, balls[i].angle.x);
            D3DMATRIXRotateY(&mat, balls[i].angle.y);
            D3DMATRIXRotateZ(&mat, balls[i].angle.z);
            
            D3DVECTOR v;
            
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = balls[i].dist;

            D3DMATRIXTransform(&mat, &v, &v);

            balls[i].pos.x = balls[i].pivot.x + v.x;
            balls[i].pos.y = balls[i].pivot.y + v.y;
            balls[i].pos.z = balls[i].pivot.z + (v.z / 2.0f);

            if(balls[i].pos.z < CURE_MINZ)
                balls[i].pos.z = CURE_MINZ;
        }
    }

//  Handle swirl action
    int newswirleys = 0;


    if(ballslinedup == CURE_NUMBALLS)
    {
        for(int i = 0; i < CURE_HOWMANYKILLS; i++)
        {
            sm[killing].state = CURE_STATE_DEAD;
            killing++;

            if(killing >= CURE_NUMPARTICLES)
            {
                ((PTEffect)inst)->KillThisEffect();
                break;
            }
        }
    }
    else
        newswirleys = CURE_MORESWIRLEYS;

    for(i = 0; i < CURE_NUMPARTICLES; i++)
    {
        if(sm[i].state == CURE_STATE_INACTIVE)
        {
        //  particle is waiting to be assigned for duty 

            if(newswirleys)
            {
            //  add a new SWIRLEY

                sm[i].whichball = random(0, numactiveballs - 1);
                sm[i].color = balls[sm[i].whichball].color;
                sm[i].angle.x = (float)random(0, 359) / 360.0f * (float)M_2PI;
                sm[i].angle.y = (float)random(0, 359) / 360.0f * (float)M_2PI;
                sm[i].angle.z = (float)random(0, 359) / 360.0f * (float)M_2PI;
                sm[i].disttheta = (float)random(0, 359) / 360.0f * (float)M_2PI;
                
                sm[i].size = CURE_SWIRL_SMALL;

            //  assign a random rotation velocity around only 2 axes                
                int arandomvalue = random(0, 2);
                
                if(arandomvalue == 0)
                {
                    sm[i].angvel.x = (float)random(CURE_SWIRL_MIN_STARTANGVEL, CURE_SWIRL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                    sm[i].angvel.y = (float)random(CURE_SWIRL_MIN_STARTANGVEL, CURE_SWIRL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                }
                else if(arandomvalue == 1)
                {
                    sm[i].angvel.x = (float)random(CURE_SWIRL_MIN_STARTANGVEL, CURE_SWIRL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                    sm[i].angvel.z = (float)random(CURE_SWIRL_MIN_STARTANGVEL, CURE_SWIRL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                }
                else
                {
                    sm[i].angvel.y = (float)random(CURE_SWIRL_MIN_STARTANGVEL, CURE_SWIRL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                    sm[i].angvel.z = (float)random(CURE_SWIRL_MIN_STARTANGVEL, CURE_SWIRL_MAX_STARTANGVEL) / 360.0f * (float)M_2PI;
                }

                sm[i].state = CURE_STATE_SWIRLEY1;

                newswirleys--;
            }
        }
        
        if(sm[i].state == CURE_STATE_SWIRLEY1)
        {
        //  particle is a SWIRLEY, hovering around one of the balls

            sm[i].angle.x += sm[i].angvel.x;
            sm[i].angle.y += sm[i].angvel.y;
            sm[i].angle.z += sm[i].angvel.z;

//          sm[i].disttheta += CURE_SWIRL_DISTTHETAVEL;
//          float cosval = (float)cos(sm[i].disttheta);
//          sm[i].dist = balls[sm[i].whichball].size + (CURE_SWIRL_STARTDISTAMP * cosval);
//          sm[i].size = CURE_SWIRL_LARGE + (CURE_SWIRL_SIZEAMP * (cosval + 1.0f));
            
            sm[i].dist = balls[sm[i].whichball].size;

            if(ballslinedup == CURE_NUMBALLS)
            {
/*
                float proportion = (float)fabs(balls[sm[i].whichball].pos.z - 35.0f) / 35.0f;
                sm[i].size = proportion * CURE_SWIRL_LARGE;
                sm[i].dist = proportion * balls[sm[i].whichball].size;
*/
/*
                if(sm[i].size > CURE_SWIRL_MINSIZE2)
                    sm[i].size -= CURE_SWIRL_SIZEVEL2;
                else
                    sm[i].size = CURE_SWIRL_MINSIZE2;
*/
/*
                if(sm[i].dist > CURE_SWIRL_MINDIST2)
                    sm[i].dist -= CURE_SWIRL_DISTVEL2;
                else
                    sm[i].dist = CURE_SWIRL_MINDIST2;
*/
            }
            else
            {
                if(sm[i].size < CURE_SWIRL_LARGE)
                    sm[i].size += CURE_SWIRL_SIZEVEL;
                else
                    sm[i].size = CURE_SWIRL_LARGE;
            }

            sm[i].pivot.x = balls[sm[i].whichball].pos.x;
            sm[i].pivot.y = balls[sm[i].whichball].pos.y;
            sm[i].pivot.z = balls[sm[i].whichball].pos.z;

            D3DMATRIX mat;
            D3DMATRIXClear(&mat);
            
            D3DMATRIXRotateX(&mat, sm[i].angle.x);
            D3DMATRIXRotateY(&mat, sm[i].angle.y);
            D3DMATRIXRotateZ(&mat, sm[i].angle.z);
            
            D3DVECTOR v;
            
            v.x = 0.0f;
            v.y = 0.0f;
            v.z = sm[i].dist;

            D3DMATRIXTransform(&mat, &v, &v);

            sm[i].pos.x = sm[i].pivot.x + v.x;
            sm[i].pos.y = sm[i].pivot.y + v.y;
            sm[i].pos.z = sm[i].pivot.z + v.z;
        }
    }

//  if(framenum >= CURE_NOMOREOFTHISEFFECT)
//      inst->SetCommandDone(TRUE);

    return;
}

BOOL TCureAnimator::Render() 
{
    PS3DAnimObj obj;
    
    SaveBlendState();
    SetAddBlendState();

    ResetExtents();

    for(int i = 0; i < CURE_NUMPARTICLES; i++)
    {
        if(sm[i].state == CURE_STATE_SWIRLEY1)
        {
            if(sm[i].size > 0.0f)
            {
                obj = GetObject(sm[i].color);

                obj->flags = OBJ3D_MATRIX;

                D3DMATRIXClear(&obj->matrix);

                D3DMATRIXRotateX(&obj->matrix, -(float)(M_2PI / 3.0));
                D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
                D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((facing / 256.0f) * M_2PI)));

                obj->scl.x = sm[i].size;
                obj->scl.y = sm[i].size;
                obj->scl.z = sm[i].size;
                D3DMATRIXScale(&obj->matrix, &obj->scl);

                obj->pos.x = (float)sm[i].pos.x;
                obj->pos.y = (float)sm[i].pos.y;
                obj->pos.z = (float)sm[i].pos.z;
                D3DMATRIXTranslate(&obj->matrix, &obj->pos);

                RenderObject(obj);
            }
        }
    }

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

void TCureAnimator::RefreshZBuffer()
{
    S3DPoint map, screen, effect;

    int size_x = 225;
    int size_y = 220;

    S3DPoint fatman;
    ((PTEffect)inst)->GetPos(fatman);

    WorldToScreen(fatman, screen);
    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2) - 45, size_x, size_y);
//  Display->Box(screen.x - (size_x / 2), screen.y - (size_y / 2) - 45, size_x, size_y);
}

// *********************
// * TFireFlashEffect *
// *********************

_CLASSDEF(TFireFlashEffect)

class TFireFlashEffect : public TEffect
{
  public:
    TFireFlashEffect(PTObjectImagery newim) : TEffect(newim) {  }
    TFireFlashEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) {  }

    virtual void Initialize();
    virtual void Pulse();
};

DEFINE_BUILDER("FireFlash", TFireFlashEffect)
REGISTER_BUILDER(TFireFlashEffect)

void TFireFlashEffect::Initialize()
{
    if(spell)
    {
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();

        if (invoker)
        {
            PTCharacter target = (PTCharacter)invoker->Fighting();
            if (target)
                spell->Damage(target);
        }
    }   
}

void TFireFlashEffect::Pulse()
{
    TEffect::Pulse();
}

// ***********************
// * FireFlash Animator *
// ***********************

// general defines for create food effect
#define FFLASH_NOMOREOFTHISEFFECT       100
#define FFLASH_NUMPARTICLES             150     //  number of particles used in both flames and swirls

// defines for particles
#define FFLASH_STATE_USEME              0       //  particle is available for use
#define FFLASH_STATE_SMOKEY1            1       //  particle is part of initial flame
#define FFLASH_STATE_GSPHERE            2       //  particle is part of glowing sphere
#define FFLASH_STATE_RING               3       //  particle is part of explosion ring

// fire defines
#define FFLASH_F1_ON            0       //  frame when fire begins
#define FFLASH_F1_OFF           25      //  frame when fire ends
#define FFLASH_F1_MAXDIST       15      //  maximum radius at the base of the flame
#define FFLASH_F1_FADEMIN       20      //  min height at which flame particles may begin fading
#define FFLASH_F1_MAXFADE       25      //  max height at which flame particles must fade out
#define FFLASH_F1_FADEPCT       20      //  percent of its lifespan when particle starts fading
#define FFLASH_F1_BOTTOM        1       //  distance of flame above ground
#define FFLASH_F1_COLORPCT      70      //  color changes when particle reaches this percent of flame
#define FFLASH_F1_MINSIZE       0.05f   //  fade out to this size, then re-use particle

//  more defines to regulate the flow of the flame
#define FFLASH_F1_TIME1         5               //  for each of these frames...
#define FFLASH_F1_ADD1          4               //      add this many particles for smoke
#define FFLASH_F1_MAINSCALE1    0.3f            //      each beginning about this size
#define FFLASH_F1_TIME2         15              //  for each of these frames...
#define FFLASH_F1_ADD2          7               //      add this many particles for smoke
#define FFLASH_F1_MAINSCALE2    0.3f            //      each beginning about this size
#define FFLASH_F1_TIME3         16              //  for each of these frames...
#define FFLASH_F1_ADD3          7               //      add this many particles for smoke
#define FFLASH_F1_MAINSCALE3    0.3f            //      each beginning about this size
#define FFLASH_F1_TIME4         FFLASH_F1_OFF   //  for each of these frames...
#define FFLASH_F1_ADD4          2               //      add this many particles for smoke
#define FFLASH_F1_MAINSCALE4    0.3f            //      each beginning about this size

#define FFLASH_EXPLOSION        35      //  frame when explosion starts

#define FFLASH_GS_ON            26      //  frame when glowing sphere begins
#define FFLASH_GS_NUMPARTICLES  75      //  number of particles which compose the glowing sphere
#define FFLASH_GS_MINSIZE1      1.0f    //  size of glowing sphere initially
#define FFLASH_GS_MAXSIZE1      20.0f   //  size of glowing sphere when it explodes
#define FFLASH_GS_SIZEVEL1      2.0f    //  rate glowing sphere grows
#define FFLASH_GS_SIZETOSCALE   0.01f   //  size of each particle relative to the size of the sphere
#define FFLASH_GS_MINANGVEL     0.12f   //  initial rotation velocity of particles in sphere
#define FFLASH_GS_INFRONT       10      //  moves the glowing sphere towards the camera along the ground
#define FFLASH_GS_INFRONTUP     5       //  (56% of INFRONT) moves the glowing sphere up to adjust for INFRONT

#define FFLASH_RI_NUMPARTICLES  150     //  number of particles which compose the explosion ring
#define FFLASH_RI_MAXSIZE       300.0f  //  ring reaches this size and disappears
#define FFLASH_RI_SIZEVEL1      8.0f    //  rate ring grows
#define FFLASH_RI_MINANGVEL     0.3f    //  initial rotation velocity of particles in ring
#define FFLASH_RI_PSIZE1        0.1f    //  particles in ring start this big

_CLASSDEF(TFireFlashAnimator)

typedef struct _FFLASH_PARTICLE
{
    D3DVECTOR pos;      //  x,y,z position of the particle
    D3DVECTOR pivot;    //  point around which particle rotates
    D3DVECTOR vel;      //  this is the velocity, my friend (up, down, sideways translation)
    D3DVECTOR angle;    //  current angle of rotation around pivot point (in 255 degs)
    D3DVECTOR angvel;   //  angular velocity (rotation speed around its pivot)
    int state;          //  particle is unused, a SWIRLEY, or a SMOKEY
    float scale;        //  size of the particle, my friend
    float dist;         //  distance from pivot point (or radius)
    int life;           //  the particle is alive (displayed while life > 0) or dead (0)
    int startfade;      //  used to control fading (size of particle)
    int stopfade;       //  used to control fading (size of particle)
    int color;
}FFLASH_PARTICLE;

class TFireFlashAnimator : public T3DAnimator
{
  private:
    D3DVALUE facing;
    S3DPoint goffset;
    D3DVECTOR gsphere;
    int framenum;
    float mainscale;
    float gsize;
    float rsize;
    float gscale;
    FFLASH_PARTICLE sm[FFLASH_NUMPARTICLES];        // particles used for the flame

    PTCharacter target;
  
  public:
    TFireFlashAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
    virtual ~TFireFlashAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};

REGISTER_3DANIMATOR("FireFlash", TFireFlashAnimator)

void TFireFlashAnimator::Initialize()
{
    T3DAnimator::Initialize();

    ((PTFireFlashEffect)inst)->Initialize();
    
    PLAY("FireFlash");

    framenum = 0;

    // Get the direction the fatman is facing.. will be used immediately and later on
    facing = (float)((PTEffect)inst)->GetAngle();

    //  calculate the offset of the flame
    ConvertToVector((int)((255 - facing) + ((138 * 255) / 360)) & 255, FFLASH_GS_INFRONT, goffset);
    goffset.z = FFLASH_GS_INFRONTUP;

    gsphere.x = 0.0f;
    gsphere.y = 0.0f;
    gsphere.z = 30.0f;
    gsize = FFLASH_GS_MINSIZE1;

    target = NULL;

    PTSpell spell = ((PTFireFlashEffect)inst)->GetSpell();
    if(spell)
    {
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
        if (invoker)
        {
            target = (PTCharacter)invoker->Fighting();
        }
    }


    memset(sm, 0, sizeof(FFLASH_PARTICLE) * FFLASH_NUMPARTICLES);

//  assign the appropriate particles to be part of the glowing sphere instead of the fire
    for(int i = 0; i < FFLASH_GS_NUMPARTICLES; i++)
    {
        sm[i].angle.x = (float)random(0, 359) / 360.0f * (float)M_2PI;
        sm[i].angle.y = (float)random(0, 359) / 360.0f * (float)M_2PI;
        sm[i].angle.z = (float)random(0, 359) / 360.0f * (float)M_2PI;
        sm[i].angvel.x = FFLASH_GS_MINANGVEL;
        sm[i].angvel.y = FFLASH_GS_MINANGVEL;
        sm[i].angvel.z = FFLASH_GS_MINANGVEL;

        sm[i].state = FFLASH_STATE_GSPHERE;
    }
}

void TFireFlashAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    framenum++;

    int newringparts = 0;

//  Handle glowing sphere crap
    if(framenum >= FFLASH_GS_ON && framenum < FFLASH_EXPLOSION)
    {
        if(gsize < FFLASH_GS_MAXSIZE1)
            gsize += FFLASH_GS_SIZEVEL1;
        else 
            gsize = FFLASH_GS_MAXSIZE1;
    }
    else if(framenum == FFLASH_EXPLOSION)
    {
/*      
        for(int i = 0; i < FFLASH_NUMPARTICLES; i++)
        {
            if(sm[i].state == FFLASH_STATE_USEME || sm[i].state == FFLASH_STATE_GSPHERE)
            {
                if(newringparts > 0)
                {
                    memset(&sm[i], 0, sizeof(FFLASH_PARTICLE));

                    sm[i].angle.x = 0.0f;
                    sm[i].angle.y = 0.0f;
                    sm[i].angle.z = ((float)newringparts * (float)M_2PI) / (float)FFLASH_RI_NUMPARTICLES;
                    sm[i].angvel.x = 0.0f;
                    sm[i].angvel.y = 0.0f;
                    sm[i].angvel.z = FFLASH_RI_MINANGVEL;
                    sm[i].scale = FFLASH_RI_PSIZE1;

                    sm[i].state = FFLASH_STATE_RING;

                    newringparts--;
                }
                else
                {
                    sm[i].state = FFLASH_STATE_USEME;
                    continue;
                }
            }
        }
*/

    //  Make all the particles used for the glowing sphere available for use for the ring
        for(int i = 0; i < FFLASH_NUMPARTICLES; i++)
        {
            if(sm[i].state == FFLASH_STATE_GSPHERE)
                sm[i].state = FFLASH_STATE_USEME;
        }

    //  initialize the ring some more
        rsize = gsize;
                
        newringparts = FFLASH_RI_NUMPARTICLES;
    }
    else if(framenum > FFLASH_EXPLOSION)
    {
        rsize += FFLASH_RI_SIZEVEL1;

        if(rsize >= FFLASH_RI_MAXSIZE)
        {
            for(int i = 0; i < FFLASH_NUMPARTICLES; i++)
                sm[i].state = FFLASH_STATE_USEME;
        }
    }

//  Handle flame1 crap
    int newsmokey1s = 0;

    if(framenum < FFLASH_F1_ON)
    {
        newsmokey1s = 0;
    }
    else if(framenum < FFLASH_F1_TIME1)
    {
        newsmokey1s = FFLASH_F1_ADD1;
        mainscale = FFLASH_F1_MAINSCALE1;
    }
    else if(framenum < FFLASH_F1_TIME2)
    {
        newsmokey1s = FFLASH_F1_ADD2;
        mainscale = FFLASH_F1_MAINSCALE2;
    }   
    else if(framenum < FFLASH_F1_TIME3)
    {
        newsmokey1s = FFLASH_F1_ADD3;
        mainscale = FFLASH_F1_MAINSCALE3;
    }
    else if(framenum < FFLASH_F1_TIME4)
    {
        newsmokey1s = FFLASH_F1_ADD4;
        mainscale = FFLASH_F1_MAINSCALE4;
    }

    for(int i = 0; i < FFLASH_NUMPARTICLES; i++)
    {
        if(sm[i].state == FFLASH_STATE_USEME)
        {
        //  particle is waiting to be assigned for duty 

            if(newsmokey1s)
            {
            //  add a new SMOKEY1

                S3DPoint s3dp;
                int dist = random(0, FFLASH_F1_MAXDIST);
                int angle = random(0, 255);
                ConvertToVector(angle, dist, s3dp);
                sm[i].pos.x = (float)s3dp.x;
                sm[i].pos.y = (float)s3dp.y;
                sm[i].pos.z = FFLASH_F1_BOTTOM;
                sm[i].life = FFLASH_F1_FADEMIN + FFLASH_F1_MAXFADE * (FFLASH_F1_MAXDIST - random(0, dist)) / FFLASH_F1_MAXDIST;
                sm[i].startfade = (sm[i].life * FFLASH_F1_FADEPCT / 100);
                sm[i].stopfade = sm[i].life;
                sm[i].vel.z = 2.0f + (float)random(0, 2);
                sm[i].state = FFLASH_STATE_SMOKEY1;

                newsmokey1s--;
            }
            else if(newringparts)
            {
                S3DPoint s3dp;
                int dist = (int)rsize;
                int angle = random(0, 255);
                ConvertToVector(angle, dist, s3dp);
                sm[i].pos.x = gsphere.x + (float)s3dp.x;
                sm[i].pos.y = gsphere.y + (float)s3dp.y;
                sm[i].pos.z = gsphere.z;
                sm[i].life = FFLASH_F1_FADEMIN + FFLASH_F1_MAXFADE * (FFLASH_F1_MAXDIST - random(0, dist)) / FFLASH_F1_MAXDIST;
                sm[i].startfade = (sm[i].life * FFLASH_F1_FADEPCT / 100);
                sm[i].stopfade = sm[i].life;
                sm[i].vel.z = 2.0f + (float)random(0, 2);
                sm[i].state = FFLASH_STATE_SMOKEY1;

                newringparts--;
            }
        }
        else if(sm[i].state == FFLASH_STATE_SMOKEY1)
        {
        //  particle is a SMOKEY1, so throw it in with the other flames (hehe)
            
            sm[i].pos.z += sm[i].vel.z;
            sm[i].life--;

            if(sm[i].life == 0)
                sm[i].state = FFLASH_STATE_USEME;
        }
        else if(sm[i].state == FFLASH_STATE_GSPHERE)
        {
        //  particle is a GSPHERE, so make it glow around the sphere deal

            sm[i].pivot.x = gsphere.x;
            sm[i].pivot.y = gsphere.y;
            sm[i].pivot.z = gsphere.z;

            sm[i].angle.x += sm[i].angvel.x;
            sm[i].angle.y += sm[i].angvel.y;
            sm[i].angle.z += sm[i].angvel.z;

            sm[i].dist = gsize;
            sm[i].scale = gsize * FFLASH_GS_SIZETOSCALE;

            D3DMATRIX mat;
            D3DMATRIXClear(&mat);
            
            D3DMATRIXRotateX(&mat, sm[i].angle.x);
            D3DMATRIXRotateY(&mat, sm[i].angle.y);
            D3DMATRIXRotateZ(&mat, sm[i].angle.z);
            
            D3DVECTOR v;
            
            v.x = 0.0f;
            v.y = sm[i].dist;
            v.z = 0.0f;

            D3DMATRIXTransform(&mat, &v, &v);

            sm[i].pos.x = sm[i].pivot.x + v.x;
            sm[i].pos.y = sm[i].pivot.y + v.y;
            sm[i].pos.z = sm[i].pivot.z + v.z;
        }
        else if(sm[i].state == FFLASH_STATE_RING)
        {
        //  particle is a RING particle, make an explosion ring

            sm[i].pivot.x = gsphere.x;
            sm[i].pivot.y = gsphere.y;
            sm[i].pivot.z = gsphere.z;

            sm[i].angle.x += sm[i].angvel.x;
            sm[i].angle.y += sm[i].angvel.y;
            sm[i].angle.z += sm[i].angvel.z;

            sm[i].dist = rsize;

            D3DMATRIX mat;
            D3DMATRIXClear(&mat);
            
            D3DMATRIXRotateX(&mat, sm[i].angle.x);
            D3DMATRIXRotateY(&mat, sm[i].angle.y);
            D3DMATRIXRotateZ(&mat, sm[i].angle.z);
            
            D3DVECTOR v;
            
            v.x = 0.0f;
            v.y = sm[i].dist;
            v.z = 0.0f;

            D3DMATRIXTransform(&mat, &v, &v);

            sm[i].pos.x = sm[i].pivot.x + v.x;
            sm[i].pos.y = sm[i].pivot.y + v.y;
            sm[i].pos.z = sm[i].pivot.z + v.z;
        }
    }

    if(framenum >= FFLASH_NOMOREOFTHISEFFECT)
        ((PTEffect)inst)->KillThisEffect();

    return;
}

BOOL TFireFlashAnimator::Render()
{
    PS3DAnimObj obj;
    int i;
    
    SaveBlendState();
    SetAddBlendState();

    ResetExtents();

//  All this crap renders the flame
    for(i = 0; i < FFLASH_NUMPARTICLES; i++)
    {
        if(sm[i].state == FFLASH_STATE_SMOKEY1)
        {
            if((sm[i].stopfade - sm[i].life) >= ((sm[i].stopfade - sm[i].startfade) * FFLASH_F1_COLORPCT / 100))
                obj = GetObject(0);
            else
                obj = GetObject(1);

            if(sm[i].life > 0)
            {
                if((sm[i].stopfade - sm[i].life) >= sm[i].startfade)
                    sm[i].scale = mainscale * sm[i].life / (sm[i].stopfade - sm[i].startfade) + FFLASH_F1_MINSIZE;
                else
                    sm[i].scale = mainscale;
            }
        }
        else if(sm[i].state == FFLASH_STATE_GSPHERE)
        {
            if(framenum >= FFLASH_GS_ON)
            {
            //  Render glowing sphere
                obj = GetObject(1);
            }
            else
                continue;
        }
        else if(sm[i].state == FFLASH_STATE_RING)
        {
        //  Render glowing sphere
            obj = GetObject(1);
        }
        else
            continue;

        if(sm[i].scale > 0.0f)
        {
            S3DPoint origin;
            
            obj->flags = OBJ3D_MATRIX;
            if (target)
            {
                obj->flags |= OBJ3D_ABSPOS;
                target->GetPos(origin);
            }
            else
            {
                origin.x = origin.y = origin.z = 0;
            }

            D3DMATRIXClear(&obj->matrix);

            D3DMATRIXRotateX(&obj->matrix, -(float)(M_2PI / 3.0));
            D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
            D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((facing / 256.0f) * M_2PI)));

            obj->scl.x = sm[i].scale;
            obj->scl.y = sm[i].scale;
            obj->scl.z = sm[i].scale;
            D3DMATRIXScale(&obj->matrix, &obj->scl);

            obj->pos.x = (float)sm[i].pos.x + origin.x;
            obj->pos.y = (float)sm[i].pos.y + origin.y;
            obj->pos.z = (float)sm[i].pos.z + origin.z;
            
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            RenderObject(obj);
        }
    }

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

void TFireFlashAnimator::RefreshZBuffer()
{
    S3DPoint map, screen;

    int size_x = 200;
    int size_y = 200;

    map.x = 0;
    map.y = 0;
    map.z = 0;
    WorldToScreen(map, screen);
    RestoreZ(screen.x - (size_x / 2), screen.y - size_y, size_x, size_y);
//  Display->Box(screen.x - (size_x / 2), screen.y - size_y, size_x, size_y);
}

// *********************
// * TFireWindEffect *
// *********************

_CLASSDEF(TFireWindEffect)

class TFireWindEffect : public TEffect
{
  private:
    int rad;
    int last_range;
    int life;
  public:
    TFireWindEffect(PTObjectImagery newim) : TEffect(newim) { }
    TFireWindEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }

    virtual void Initialize();
    virtual void Pulse();
};

DEFINE_BUILDER("FireWind", TFireWindEffect)
REGISTER_BUILDER(TFireWindEffect)

void TFireWindEffect::Initialize()
{
    rad = 0;
    last_range = 0;
    life = 0;
}

void TFireWindEffect::Pulse()
{
    life++;
    if(life >=40)
    {
        rad += 5;

        if (!(((int)abs(rad)) % 40))
        {
            if(spell)
            {
                PTCharacter caster = (PTCharacter)spell->GetInvoker();
                S3DPoint cpos;
                GetPos(cpos);
                BlastCharactersInRange(caster, cpos, rad,spell->VariantData()->mindamage,spell->VariantData()->maxdamage,spell->VariantData()->type, last_range);
                last_range = rad;
            }
        }
    }
    TEffect::Pulse();
}

// ***********************
// * FireWind Animator *
// ***********************

// general defines for create food effect
#define FWIND_NUMPARTICLES          400     //  total number of particles used for everything (size of the particle array)
#define FWIND_RI_NUMPARTICLES       200     //  number of particles which compose the explosion ring (this amount comes out of the total number of particles, they aren't extra)

// defines for particles
#define FWIND_STATE_USEME           0       //  particle is available for use
#define FWIND_STATE_SMOKEY1         1       //  particle is part of a flame
#define FWIND_STATE_GSPHERE         2       //  particle is part of glowing sphere
#define FWIND_STATE_RING            3       //  particle is part of explosion ring
#define FWIND_STATE_FIRERING        4       //  particle is part of a fire ring

// fire defines
#define FWIND_F1_ON             0       //  frame when fire begins
#define FWIND_F1_OFF            25      //  frame when fire ends
#define FWIND_F1_MAXDIST        15      //  maximum radius at the base of the flame
#define FWIND_F1_FADEMIN        20      //  min height at which flame particles may begin fading
#define FWIND_F1_MAXFADE        25      //  max height at which flame particles must fade out
#define FWIND_F1_FADEPCT        20      //  percent of its lifespan when particle starts fading
#define FWIND_F1_BOTTOM         1       //  distance of flame above ground
#define FWIND_F1_COLORPCT       70      //  color changes when particle reaches this percent of flame
#define FWIND_F1_MINSIZE        0.05f   //  fade out to this size, then re-use particle

//  more defines to regulate the flow of the flame
#define FWIND_F1_TIME1          5               //  for each of these frames...
#define FWIND_F1_ADD1           4               //      add this many particles for smoke
#define FWIND_F1_MAINSCALE1     0.3f            //      each beginning about this size
#define FWIND_F1_TIME2          16              //  for each of these frames...
#define FWIND_F1_ADD2           7               //      add this many particles for smoke
#define FWIND_F1_MAINSCALE2     0.3f            //      each beginning about this size
#define FWIND_F1_TIME3          FWIND_F1_OFF    //  for each of these frames...
#define FWIND_F1_ADD3           2               //      add this many particles for smoke
#define FWIND_F1_MAINSCALE3     0.3f            //      each beginning about this size

#define FWIND_GS_ON             28      //  frame when glowing sphere begins
#define FWIND_GS_NUMPARTICLES   100     //  number of particles which compose the glowing sphere
#define FWIND_GS_MINSIZE1       1.0f    //  size of glowing sphere initially
#define FWIND_GS_MAXSIZE1       40.0f   //  size of glowing sphere when it explodes
#define FWIND_GS_SIZEVEL1       3.0f    //  rate glowing sphere grows
#define FWIND_GS_SIZETOSCALE    0.01f   //  size of each particle relative to the size of the sphere
#define FWIND_GS_MINANGVEL      0.12f   //  initial rotation velocity of particles in sphere
#define FWIND_GS_INFRONT        10      //  moves the glowing sphere towards the camera along the ground
#define FWIND_GS_INFRONTUP      5       //  (56% of INFRONT) moves the glowing sphere up to adjust for INFRONT

#define FWIND_RI_MAXSIZE        300.0f  //  ring reaches this size and disappears
#define FWIND_RI_SIZEVEL1       13.0f   //  rate ring grows
#define FWIND_RI_MINANGVEL      0.3f    //  initial rotation velocity of particles in ring
#define FWIND_RI_FADEMIN        3       //  min height at which particles may begin fading
#define FWIND_RI_MAXFADE        6       //  max height at which particles must fade out
#define FWIND_RI_FADEPCT        20      //  percent of its lifespan when particle starts fading
#define FWIND_RI_COLORPCT       70      //  color changes when particle reaches this percent of flame
#define FWIND_RI_MINSIZE        0.05f   //  fade out to this size, then re-use particle
#define FWIND_RI_PSIZE1         0.55f   //  particles in ring start this big

//  defines to regulate the flow of each flame ring
#define FWIND_FR_LIFERAND       10      //  just use it and love it
#define FWIND_FR_FADEMIN        10      //  min height at which flame particles may begin fading
#define FWIND_FR_MAXFADE        15      //  max height at which flame particles must fade out
#define FWIND_FR_FADEPCT        20      //  percent of its lifespan when particle starts fading
#define FWIND_FR_BOTTOM         1       //  distance of flame above ground
#define FWIND_FR_COLORPCT       80      //  color changes when particle reaches this percent of lifespan
#define FWIND_FR_MINSIZE        0.05f   //  fade out to this size, then re-use particle
#define FWIND_FR_NUMKEYS        3       //  number of keyframes (time intervals)
#define FWIND_FR_RINGSIZE1      30      //  number of keyframes (time intervals)
#define FWIND_FR_RINGSPACING    30      //  number of keyframes (time intervals)
#define FWIND_RINGSIZEVEL       4

#define FWIND_FR_RINGTIME1          5       //  wait this many frames before starting first ring
#define FWIND_NEXTRINGWAITOFFSET    0       //  frame difference when last ring ends and next ring starts (can be a negative value to begin before the previous one ends)
#define FWIND_HOWMANYRINGS          1       //  how many fire rings occur after the explosion ring

#define FWIND_EXPLOSION                 40      //  frame when explosion starts

#define FWIND_WAITTOEND                 5       //  wait this many frames before ending to cleanup any leftovers

_CLASSDEF(TFireWindAnimator)

typedef struct _FWIND_PARTICLE
{
    D3DVECTOR pos;      //  x,y,z position of the particle
    D3DVECTOR pivot;    //  point around which particle rotates
    D3DVECTOR vel;      //  this is the velocity, my friend (up, down, sideways translation)
    D3DVECTOR angle;    //  current angle of rotation around pivot point (in 255 degs)
    D3DVECTOR angvel;   //  angular velocity (rotation speed around its pivot)
    int state;          //  particle is unused, a SWIRLEY, or a SMOKEY
    float scale;        //  size of the particle, my friend
    float dist;         //  distance from pivot point (or radius)
    int life;           //  the particle is alive (displayed while life > 0) or dead (0)
    int startfade;      //  used to control fading (size of particle)
    int stopfade;       //  used to control fading (size of particle)
    int color;
    int whichring;
}FWIND_PARTICLE;

typedef struct _FWIND_FIRERING
{
    int starttime;                      //  time when this fire ring begins
    float keyscale[FWIND_FR_NUMKEYS];   //  particles begin about this size for each time interval
    int keyadd[FWIND_FR_NUMKEYS];       //  add this many particles to fire ring per frame during each time interval
    int keytime[FWIND_FR_NUMKEYS];      //  time intervals for controlling flow of fire (how many particles are added at which times)
    int newparticles;                   //  counter to help add the particles for each time interval
    int ringsize;                       //  size of the ring (or distance to the pivot point if you prefer)
    int dead;                           //  Ring is dead after it has gone thru all its keys
}FWIND_FIRERING;

class TFireWindAnimator : public T3DAnimator
{
  private:
    D3DVALUE facing;
    S3DPoint goffset;
    D3DVECTOR gsphere;
    int framenum;
    float mainscale1;
    float mainscale2;
    float gsize;
    float rsize;
    float gscale;
    int howmanyrings;
    int stopclock;
    int waitingtofinish;
    FWIND_PARTICLE sm[FWIND_NUMPARTICLES];  // particles used for the flame
    FWIND_FIRERING fr[FWIND_HOWMANYRINGS];  // properties for each fire ring

  public:
    TFireWindAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
    virtual ~TFireWindAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};

REGISTER_3DANIMATOR("FireWind", TFireWindAnimator)

void TFireWindAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PLAY("FireWind");

    framenum = 0;
    // Get the direction the fatman is facing.. will be used immediately and later on
    facing = (float)((PTEffect)inst)->GetAngle();

    //  calculate the offset of the flame
    ConvertToVector((int)((255 - facing) + ((138 * 255) / 360)) & 255, FWIND_GS_INFRONT, goffset);
    goffset.z = FWIND_GS_INFRONTUP;

    gsphere.x = 0.0f;
    gsphere.y = 0.0f;
    gsphere.z = 30.0f;
    gsize = FWIND_GS_MINSIZE1;

    waitingtofinish = 0;
    howmanyrings = 0;       //  number of active fire rings (of course 0 until the explosion)

//  setup the properties of each fire ring ahead of time
    memset(fr, 0, sizeof(FWIND_FIRERING) * FWIND_HOWMANYRINGS);
    
    fr[0].starttime = FWIND_EXPLOSION + FWIND_FR_RINGTIME1;
    fr[0].ringsize = FWIND_FR_RINGSIZE1;
    fr[0].keyscale[0] = 0.5f;
    fr[0].keyscale[1] = 0.5f;
    fr[0].keyscale[2] = 0.5f;
    fr[0].keyadd[0] = 28;
    fr[0].keyadd[1] = 28;
    fr[0].keyadd[2] = 28;
    fr[0].keytime[0] = 5;
    fr[0].keytime[1] = 70;
    fr[0].keytime[2] = 75;
/*
    fr[0].starttime = FWIND_EXPLOSION + FWIND_FR_RINGTIME1;
    fr[0].ringsize = FWIND_FR_RINGSIZE1;
    fr[0].keyscale[0] = 0.5f;
    fr[0].keyscale[1] = 0.5f;
    fr[0].keyscale[2] = 0.5f;
    fr[0].keyadd[0] = 10;
    fr[0].keyadd[1] = 15;
    fr[0].keyadd[2] = 8;
    fr[0].keytime[0] = 5;
    fr[0].keytime[1] = 11;
    fr[0].keytime[2] = 15;

    fr[1].keyscale[0] = 0.6f;
    fr[1].keyscale[1] = 0.6f;
    fr[1].keyscale[2] = 0.6f;
    fr[1].keyadd[0] = 10;
    fr[1].keyadd[1] = 15;
    fr[1].keyadd[2] = 8;
    fr[1].keytime[0] = 5;
    fr[1].keytime[1] = 11;
    fr[1].keytime[2] = 15;

    fr[2].keyscale[0] = 0.7f;
    fr[2].keyscale[1] = 0.7f;
    fr[2].keyscale[2] = 0.7f;
    fr[2].keyadd[0] = 22;
    fr[2].keyadd[1] = 30;
    fr[2].keyadd[2] = 20;
    fr[2].keytime[0] = 5;
    fr[2].keytime[1] = 11;
    fr[2].keytime[2] = 15;

    fr[3].keyscale[0] = 0.7f;
    fr[3].keyscale[1] = 0.7f;
    fr[3].keyscale[2] = 0.7f;
    fr[3].keyadd[0] = 22;
    fr[3].keyadd[1] = 30;
    fr[3].keyadd[2] = 20;
    fr[3].keytime[0] = 5;
    fr[3].keytime[1] = 11;
    fr[3].keytime[2] = 15;
*/
    for(int i = 1; i < FWIND_HOWMANYRINGS; i++)
    {
        fr[i].starttime = fr[i - 1].starttime + fr[i - 1].keytime[FWIND_FR_NUMKEYS - 1] + FWIND_NEXTRINGWAITOFFSET;
        fr[i].ringsize = fr[i - 1].ringsize + FWIND_FR_RINGSPACING;
    }

//  assign the particles to their first positions
    memset(sm, 0, sizeof(FWIND_PARTICLE) * FWIND_NUMPARTICLES);

//  assign the appropriate particles to be part of the glowing sphere instead of the fire
    for(i = 0; i < FWIND_GS_NUMPARTICLES; i++)
    {
        sm[i].angle.x = (float)random(0, 359) / 360.0f * (float)M_2PI;
        sm[i].angle.y = (float)random(0, 359) / 360.0f * (float)M_2PI;
        sm[i].angle.z = (float)random(0, 359) / 360.0f * (float)M_2PI;
        sm[i].angvel.x = FWIND_GS_MINANGVEL;
        sm[i].angvel.y = FWIND_GS_MINANGVEL;
        sm[i].angvel.z = FWIND_GS_MINANGVEL;

        sm[i].state = FWIND_STATE_GSPHERE;
    }
}

void TFireWindAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    framenum++;

//  Handle glowing sphere crap
    if(framenum >= FWIND_GS_ON && framenum < FWIND_EXPLOSION)
    {
        if(gsize < FWIND_GS_MAXSIZE1)
            gsize += FWIND_GS_SIZEVEL1;
        else 
            gsize = FWIND_GS_MAXSIZE1;
    }
    else if(framenum == FWIND_EXPLOSION)
    {
        int newringparts = FWIND_RI_NUMPARTICLES;
        
        for(int i = 0; i < FWIND_NUMPARTICLES; i++)
        {
            if(sm[i].state == FWIND_STATE_USEME || sm[i].state == FWIND_STATE_GSPHERE)
            {
                if(newringparts > 0)
                {
                    memset(&sm[i], 0, sizeof(FWIND_PARTICLE));

                    sm[i].angle.x = 0.0f;
                    sm[i].angle.y = 0.0f;
                    sm[i].angle.z = ((float)newringparts * (float)M_2PI) / (float)FWIND_RI_NUMPARTICLES;
                    sm[i].angvel.x = 0.0f;
                    sm[i].angvel.y = 0.0f;
                    sm[i].angvel.z = FWIND_RI_MINANGVEL;
                    sm[i].scale = FWIND_RI_PSIZE1;

                    sm[i].state = FWIND_STATE_RING;

                    newringparts--;
                }
                else
                {
                    sm[i].state = FWIND_STATE_USEME;
                    continue;
                }
            }
        }

    //  initialize the explosion ring and some more
        rsize = gsize;
    }
    else if(framenum > FWIND_EXPLOSION)
    {
        rsize += FWIND_RI_SIZEVEL1;

        if(rsize >= FWIND_RI_MAXSIZE)
        {
            for(int i = 0; i < FWIND_NUMPARTICLES; i++)
                sm[i].state = FWIND_STATE_USEME;

            rsize = 0.0f;
        }
    }

//  Handle flame1 crap
    int newsmokey1s = 0;

    if(framenum < FWIND_F1_ON)
    {
        newsmokey1s = 0;
    }
    else if(framenum < FWIND_F1_TIME1)
    {
        newsmokey1s = FWIND_F1_ADD1;
        mainscale1 = FWIND_F1_MAINSCALE1;
    }
    else if(framenum < FWIND_F1_TIME2)
    {
        newsmokey1s = FWIND_F1_ADD2;
        mainscale1 = FWIND_F1_MAINSCALE2;
    }   
    else if(framenum < FWIND_F1_TIME3)
    {
        newsmokey1s = FWIND_F1_ADD3;
        mainscale1 = FWIND_F1_MAINSCALE3;
    }

//  Handle fire rings
    for(int i = 0; i < FWIND_HOWMANYRINGS; i++)
    {
        fr[i].newparticles = 0;

        if(!fr[i].dead)     //  if the ring is dead, don't need to do anything further with it
        {
            if(framenum >= fr[i].starttime && framenum < fr[i].starttime + fr[i].keytime[FWIND_FR_NUMKEYS - 1])
            {
                if(framenum == fr[i].starttime)
                    howmanyrings++;

                fr[i].ringsize += FWIND_RINGSIZEVEL;

                for(int key = 0; key < FWIND_FR_NUMKEYS; key++)
                {
                    if(framenum < fr[i].starttime + fr[i].keytime[key])
                    {
                        fr[i].newparticles = fr[i].keyadd[key];
                        mainscale1 = fr[i].keyscale[key];
                        break;
                    }
                }

                if(key == FWIND_FR_NUMKEYS)
                {
                    fr[i].dead = 1;

                    if(howmanyrings == FWIND_HOWMANYRINGS)
                    {
                    //  done with effect.. set the flag to wait a couple more frames then end
                        stopclock = framenum;
                        waitingtofinish = 1;
                    }
                }
            }
        }
    }

    //if(waitingtofinish && framenum >= stopclock + FWIND_WAITTOEND)
    if(framenum > 140)
        ((PTEffect)inst)->KillThisEffect();
    
    for(i = 0; i < FWIND_NUMPARTICLES; i++)
    {
        if(sm[i].state == FWIND_STATE_USEME)
        {
        //  particle is waiting to be assigned for duty 

            if(newsmokey1s)
            {
            //  add a new SMOKEY1 as part of the initial base fire

                memset(&sm[i], 0, sizeof(FWIND_PARTICLE));

                S3DPoint s3dp;
                int dist = random(0, FWIND_F1_MAXDIST);
                int angle = random(0, 255);
                ConvertToVector(angle, dist, s3dp);
                sm[i].pos.x = (float)s3dp.x;
                sm[i].pos.y = (float)s3dp.y;
                sm[i].pos.z = FWIND_F1_BOTTOM;
                sm[i].life = FWIND_F1_FADEMIN + FWIND_F1_MAXFADE * (FWIND_F1_MAXDIST - random(0, dist)) / FWIND_F1_MAXDIST;
                sm[i].startfade = (sm[i].life * FWIND_F1_FADEPCT / 100);
                sm[i].stopfade = sm[i].life;
                sm[i].vel.z = 2.0f + (float)random(0, 2);
                sm[i].state = FWIND_STATE_SMOKEY1;

                newsmokey1s--;
            }
            else
            {
                for(int ring = 0; ring < FWIND_HOWMANYRINGS; ring++)
                {
                    if(fr[ring].newparticles)
                    {
                    //  add a new SMOKEY2 as part of a fire ring

                        memset(&sm[i], 0, sizeof(FWIND_PARTICLE));

                        sm[i].whichring = ring;

                        S3DPoint s3dp;
                        sm[i].angle.z = (float)random(0, 255);
                        ConvertToVector((int)sm[i].angle.z, fr[sm[i].whichring].ringsize, s3dp);
                        sm[i].pos.x = (float)s3dp.x;
                        sm[i].pos.y = (float)s3dp.y;
                        sm[i].pos.z = FWIND_FR_BOTTOM;
                        sm[i].life = FWIND_FR_FADEMIN + FWIND_FR_MAXFADE * random(0, FWIND_FR_LIFERAND) / FWIND_FR_LIFERAND;
                        sm[i].startfade = (sm[i].life * FWIND_FR_FADEPCT / 100);
                        sm[i].stopfade = sm[i].life;
                        sm[i].vel.z = 1.0f + (float)random(0, 3);
                        sm[i].state = FWIND_STATE_FIRERING;

                        fr[ring].newparticles--;
                        break;
                    }
                }
            }
        }
        else if(sm[i].state == FWIND_STATE_SMOKEY1)
        {
        //  particle is a SMOKEY1, so throw it in with the other flames (hehe)
            
            sm[i].pos.z += sm[i].vel.z;
            sm[i].life--;

            if(sm[i].life == 0)
                sm[i].state = FWIND_STATE_USEME;
        }
        else if(sm[i].state == FWIND_STATE_FIRERING)
        {
        //  particle is a SMOKEY1, so throw it in with the other flames (hehe)
            
            S3DPoint s3dp;
            ConvertToVector((int)sm[i].angle.z, fr[sm[i].whichring].ringsize, s3dp);
            sm[i].pos.x = (float)s3dp.x;
            sm[i].pos.y = (float)s3dp.y;
            sm[i].pos.z += sm[i].vel.z;
            sm[i].life--;

            if(sm[i].life == 0)
                sm[i].state = FWIND_STATE_USEME;
        }
        else if(sm[i].state == FWIND_STATE_GSPHERE)
        {
        //  particle is a GSPHERE, so make it glow around the sphere deal

            sm[i].pivot.x = gsphere.x;
            sm[i].pivot.y = gsphere.y;
            sm[i].pivot.z = gsphere.z;

            sm[i].angle.x += sm[i].angvel.x;
            sm[i].angle.y += sm[i].angvel.y;
            sm[i].angle.z += sm[i].angvel.z;

            sm[i].dist = gsize;
            sm[i].scale = gsize * FWIND_GS_SIZETOSCALE;

            D3DMATRIX mat;
            D3DMATRIXClear(&mat);
            
            D3DMATRIXRotateX(&mat, sm[i].angle.x);
            D3DMATRIXRotateY(&mat, sm[i].angle.y);
            D3DMATRIXRotateZ(&mat, sm[i].angle.z);
            
            D3DVECTOR v;
            
            v.x = 0.0f;
            v.y = sm[i].dist;
            v.z = 0.0f;

            D3DMATRIXTransform(&mat, &v, &v);

            sm[i].pos.x = sm[i].pivot.x + v.x;
            sm[i].pos.y = sm[i].pivot.y + v.y;
            sm[i].pos.z = sm[i].pivot.z + v.z;
        }
        else if(sm[i].state == FWIND_STATE_RING)
        {
        //  particle is a RING particle, make an explosion ring

            sm[i].pivot.x = gsphere.x;
            sm[i].pivot.y = gsphere.y;
            sm[i].pivot.z = gsphere.z;

            sm[i].angle.x += sm[i].angvel.x;
            sm[i].angle.y += sm[i].angvel.y;
            sm[i].angle.z += sm[i].angvel.z;

            sm[i].dist = rsize;

            D3DMATRIX mat;
            D3DMATRIXClear(&mat);
            
            D3DMATRIXRotateX(&mat, sm[i].angle.x);
            D3DMATRIXRotateY(&mat, sm[i].angle.y);
            D3DMATRIXRotateZ(&mat, sm[i].angle.z);
            
            D3DVECTOR v;
            
            v.x = 0.0f;
            v.y = sm[i].dist;
            v.z = 0.0f;

            D3DMATRIXTransform(&mat, &v, &v);

            sm[i].pos.x = sm[i].pivot.x + v.x;
            sm[i].pos.y = sm[i].pivot.y + v.y;
            sm[i].pos.z = sm[i].pivot.z + v.z;
        }
    }

//  if(framenum >= FWIND_NOMOREOFTHISEFFECT)
//      inst->SetCommandDone(TRUE);

    // find the radius of the fire blast
    float rad = 0;
//  float newrad;
//  for(i = 0; i < FWIND_NUMPARTICLES; i++)
//  {
//      if (sm[i].pos.x > -10 && sm[i].pos.x < 10)
//      {
//          newrad = sm[i].pos.y;
//          if (newrad > rad)
//              rad = newrad;
//      }
//  }
//  if (rad == 125)
    //else
    //{
        //int k = rad;
    //}

    return;
}

BOOL TFireWindAnimator::Render()
{
    PS3DAnimObj obj;
    int i;
    
    SaveBlendState();
    SetAddBlendState();

    ResetExtents();

//  All this crap renders the flame
    for(i = 0; i < FWIND_NUMPARTICLES; i++)
    {
        if(sm[i].state == FWIND_STATE_SMOKEY1)
        {
            if((sm[i].stopfade - sm[i].life) >= ((sm[i].stopfade - sm[i].startfade) * FWIND_F1_COLORPCT / 100))
                obj = GetObject(0);
            else
                obj = GetObject(1);

            if(sm[i].life > 0)
            {
                if((sm[i].stopfade - sm[i].life) >= sm[i].startfade)
                    sm[i].scale = mainscale1 * sm[i].life / (sm[i].stopfade - sm[i].startfade) + FWIND_F1_MINSIZE;
                else
                    sm[i].scale = mainscale1;
            }
        }
        else if(sm[i].state == FWIND_STATE_FIRERING)
        {
            if((sm[i].stopfade - sm[i].life) >= ((sm[i].stopfade - sm[i].startfade) * FWIND_F1_COLORPCT / 100))
                obj = GetObject(0);
            else
                obj = GetObject(1);

            if(sm[i].life > 0)
            {
                if((sm[i].stopfade - sm[i].life) >= sm[i].startfade)
                    sm[i].scale = mainscale1 * sm[i].life / (sm[i].stopfade - sm[i].startfade) + FWIND_F1_MINSIZE;
                else
                    sm[i].scale = mainscale1;
            }
        }
        else if(sm[i].state == FWIND_STATE_GSPHERE)
        {
            if(framenum >= FWIND_GS_ON)
            {
            //  Render glowing sphere
                obj = GetObject(1);
            }
            else
                continue;
        }       
        else if(sm[i].state == FWIND_STATE_RING)
        {
        //  Render glowing sphere
            obj = GetObject(1);
        }
        else
            continue;

        if(sm[i].scale > 0.0f)
        {
            obj->flags = OBJ3D_MATRIX;

            D3DMATRIXClear(&obj->matrix);

            D3DMATRIXRotateX(&obj->matrix, -(float)(M_2PI / 3.0));
            D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
            D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((facing / 256.0f) * M_2PI)));

            obj->scl.x = sm[i].scale;
            obj->scl.y = sm[i].scale;
            obj->scl.z = sm[i].scale;
            D3DMATRIXScale(&obj->matrix, &obj->scl);

            obj->pos.x = (float)sm[i].pos.x;
            obj->pos.y = (float)sm[i].pos.y;
            obj->pos.z = (float)sm[i].pos.z;
            
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            RenderObject(obj);
        }
    }

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

void TFireWindAnimator::RefreshZBuffer()
{
    S3DPoint map, screen;

    int size_x = 640;
    int size_y = 480;

    ((PTEffect)inst)->GetPos(map);
    WorldToScreen(map, screen);
    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2), size_x, size_y);
//  Display->Box(screen.x - (size_x / 2), screen.y - (size_y / 2), size_x, size_y);
}

// **********************
// * Burn the Character *
// **********************

#define BURN_PART_MIN   50 
#define BURN_PART_MAX   50
#define BURN_ADD        8
#define BURN_SPREAD     2
#define BURN_MIN_SCL    15
#define BURN_MAX_SCL    40
#define BURN_MIN_Z      30
#define BURN_MAX_Z      75
#define BURN_MIN_LIFE   5 
#define BURN_MAX_LIFE   15
#define BURN_DEC        .97f
#define BURN_FRAME      50

DEFINE_BUILDER("BURN", TBurnEffect)
REGISTER_BUILDER(TBurnEffect)

void TBurnEffect::Set(PTCharacter ch)
{
    // get character info
    character = ch;

    if (character)
    {
        S3DPoint pos;
        character->GetPos(pos);
        SetPos(pos);
    }
}


void TBurnEffect::ResetFrameCount()
{
    PT3DAnimator anim = (PT3DAnimator)GetAnimator();
    if (anim)
    {
        ((PTBurnAnimator)anim)->ResetFrameCount();
    }
}

void TBurnEffect::Initialize()
{
    character = NULL; 
    SetNotify(N_DELETING);          // Check for deleted objs in action blocks
    frame = 0;
}

void TBurnEffect::Pulse()
{
    TEffect::Pulse();

    frame++;
    if (random(0, 17) < 5 && frame < BURN_FRAME)
    {
        if (spell)
            spell->Damage(character);
        else if (character) 
            character->Damage(random(1, 5), DT_BURN);
    }

    if (character)
    {
        S3DPoint char_pos;
        character->GetPos(char_pos);
        SetPos(char_pos);
    }
}

void TBurnEffect::Notify(int notify, void *ptr)
{
    // **** WARNING!!! MAKE SURE YOU CHECK FOR BROKEN LINKS AND DELETED OBJECTS HERE!!! ****
    // If you want to be notified, you must call SetNotify() in your contsructor

    if (sector == (PTSector)ptr)
        return;

    TObjectInstance::Notify(notify, ptr);

    if (character && NOTIFY_DELETED(ptr, character))
    {
        SetFlags(OF_KILL);
        character->ClearBurn();
        character = NULL;
    }
}

// *******************************
// * Burn the Character Animator *
// *******************************

REGISTER_3DANIMATOR("Burn", TBurnAnimator)

void TBurnAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PTCharacter character = ((PTBurnEffect)inst)->GetCharacter();

    if(!character)
    {
        ((PTEffect)inst)->KillThisEffect();
        return;
    }

    PTCharAnimator ca = (PTCharAnimator)character->GetAnimator();
    size = ca->NumObjects();

    S3DPoint s;
    fire.Init(this, GetObject(1), s, TRUE);
    smoke.Init(this, GetObject(0), s, TRUE);
    
    frame = 0;
    to_add = 0;
}

void TBurnAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

    PTCharacter character = ((PTBurnEffect)inst)->GetCharacter();

    // check for non-existant characters
    if(!character)
    {
        ((PTEffect)inst)->KillThisEffect();
        return;
    }

    ++frame;
    if(frame < BURN_FRAME && to_add < BURN_ADD)
        ++to_add;
    else if(frame >= BURN_FRAME && to_add > 0)
        --to_add;

    PTCharAnimator ca = (PTCharAnimator)character->GetAnimator();
    PSParticleSystemInfo particle;

    int i;

    for(i = 0; i < BURN_COUNT; ++i)
    {
        particle = fire.Get(i);
        if(!particle->used)
            continue;
        
        if(particle->life == particle->life_span)
        {
            particle->used = FALSE;

            particle->life_span = (particle->life_span * 2) / 3;

            particle->scl.x *= 1.25f;
            particle->scl.y *= 1.25f;
            particle->scl.z *= 1.25f;

            smoke.Add(particle);
        }
    }

    fire.Animate();
    smoke.Animate();

    D3DMATRIX mtx;
    ca->MakeMatrix(&mtx);

    for(i = 0; i < to_add; ++i)
    {
        int j = random(0, size - 1);

        SParticleSystemInfo p;

        PS3DAnimObj obj = ca->GetObject(j);
        ca->Get3DImagery()->CalcObjectMatrix(obj, ((PTObjectInstance)character)->GetState(), character->GetFrame(), &mtx);

        D3DVECTOR vp;
        vp.x = vp.y = vp.z = 0.0f;

        D3DMATRIX dest;
        MultiplyD3DMATRIX(&dest, &obj->matrix, &mtx);

        D3DMATRIXTransform(&dest, &vp, &p.pos);

        S3DPoint char_pos;
        character->GetPos(char_pos);

        // position the thing
        p.pos.x += (float)random(-BURN_SPREAD, BURN_SPREAD);
        p.pos.y += (float)random(-BURN_SPREAD, BURN_SPREAD);
        p.pos.z += (float)random(-BURN_SPREAD, BURN_SPREAD);

        // scale the thing
        float scale = (float)random(BURN_MIN_SCL, BURN_MAX_SCL) * .01f;
        p.scl.x = scale;
        p.scl.y = scale;
        p.scl.z = scale;

        // rotate the thing
        p.rot.x = -90.0f;
        p.rot.z = -45.0f;
        p.rot.y = 0.0f;

        // set the acceleration
        p.acc.x = p.acc.y = p.acc.z = 1.0f;

        // set the velocity
        p.vel.x = 0.0f;
        p.vel.y = 0.0f;
        p.vel.z = (float)random(BURN_MIN_Z, BURN_MAX_Z) * .1f;

        p.life_span = random(BURN_MIN_LIFE, BURN_MAX_LIFE);

        fire.Add(&p);
    }

    BOOL done = TRUE;

    // special fire animation
    for(i = 0; i < BURN_COUNT; ++i)
    {
        particle = fire.Get(i);
        if(!particle->used)
            continue;

        done = FALSE;
        particle->scl.x *= BURN_DEC;
        particle->scl.y *= BURN_DEC;
        particle->scl.z *= BURN_DEC;
    }

    for(i = 0; i < BURN_COUNT; ++i)
    {
        particle = smoke.Get(i);
        if(!particle->used)
            continue;

        done = FALSE;
        particle->scl.x *= BURN_DEC;
        particle->scl.y *= BURN_DEC;
        particle->scl.z *= BURN_DEC;
    }

    if(frame >= BURN_FRAME && to_add == 0 && done)
    {
        ((PTEffect)inst)->KillThisEffect();
        character->ClearBurn();
    }
}

BOOL TBurnAnimator::Render()
{
    SaveBlendState();
    SetAddBlendState();

    fire.Render(FALSE, TRUE);
    smoke.Render(FALSE, TRUE);

    RestoreBlendState();

    return TRUE;
}

void TBurnAnimator::RefreshZBuffer()
{
    S3DPoint pos, screen, size;
    ((PTBurnEffect)inst)->GetCharacter()->GetPos(pos);

    WorldToScreen(pos, screen);

    size.x = 300;
    size.y = 300;

    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

// *************************
// * Aura of the Character *
// *************************

DEFINE_BUILDER("Aura", TAuraEffect)
REGISTER_BUILDER(TAuraEffect)

void TAuraEffect::Initialize()
{
    first_time = TRUE;
}


void TAuraEffect::Pulse()
{
    TEffect::Pulse();

    if(first_time)
    {
        // set the defensive bonus
        if(spell)
        {
            spell->SetDefense(spell->VariantData()->mindamage);
            spell->SetOffense(spell->VariantData()->maxdamage);
            first_time = FALSE;
        }
    }
    S3DPoint effect_pos;
    if(GetCharacter())
        GetCharacter()->GetPos(effect_pos);
    else
        GetPos(effect_pos);
    effect_pos.z = (int)FIX_Z_VALUE(effect_pos.z);
    SetPos(effect_pos);

    // set the defensive bonus
    GetSpell()->SetDefense(GetSpell()->VariantData()->mindamage);
    GetSpell()->SetOffense(GetSpell()->VariantData()->maxdamage);
}

#define AURA_PART_MIN   50 
#define AURA_PART_MAX   50
#define AURA_ADD        2
#define AURA_SPREAD     2
#define AURA_MIN_SCL    25
#define AURA_MAX_SCL    60
#define AURA_MIN_Z      5
#define AURA_MAX_Z      30
#define AURA_MIN_LIFE   15
#define AURA_MAX_LIFE   30
#define AURA_DEC        .97f
#define AURA_SEC        30
#define AURA_FRAME      (AURA_SEC * 24)

// *******************************
// * Aura the Character Animator *
// *******************************

REGISTER_3DANIMATOR("Aura", TAuraAnimator)

void TAuraAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PTCharacter character = ((PTAuraEffect)inst)->GetCharacter();

    if(!character)
    {
        ((PTEffect)inst)->KillThisEffect();
        return;
    }

    PTCharAnimator ca = (PTCharAnimator)character->GetAnimator();
    size = ca->NumObjects();

    S3DPoint s;
    fire.Init(this, GetObject(0), s, TRUE);
    
    frame = 0;
    to_add = 0;

}

void TAuraAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

    PTCharacter character = ((PTAuraEffect)inst)->GetCharacter();

    // check for non-existant characters
    if(!character)
    {
        ((PTEffect)inst)->KillThisEffect();
        return;
    }

    ++frame;
    if(frame < AURA_FRAME && to_add < AURA_ADD)
        ++to_add;
    else if(frame >= AURA_FRAME && to_add > 0)
        --to_add;

    PTCharAnimator ca = (PTCharAnimator)character->GetAnimator();
    PSParticleSystemInfo particle;

    int i;

    fire.Animate();

    D3DMATRIX mtx;
    ca->MakeMatrix(&mtx);

    for(i = 0; i < to_add; ++i)
    {
        int j = random(0, size - 1);

        SParticleSystemInfo p;

        PS3DAnimObj obj = ca->GetObject(j);
        ca->Get3DImagery()->CalcObjectMatrix(obj, ((PTObjectInstance)character)->GetState(), character->GetFrame(), &mtx);

        D3DVECTOR vp;
        vp.x = vp.y = vp.z = 0.0f;

        D3DMATRIX dest;
        MultiplyD3DMATRIX(&dest, &obj->matrix, &mtx);

        D3DMATRIXTransform(&dest, &vp, &p.pos);

        S3DPoint char_pos;
        character->GetPos(char_pos);

        // position the thing
        p.pos.x += (float)random(-AURA_SPREAD, AURA_SPREAD) - char_pos.x;
        p.pos.y += (float)random(-AURA_SPREAD, AURA_SPREAD) - char_pos.y;
        p.pos.z += (float)random(-AURA_SPREAD, AURA_SPREAD);// - char_pos.z;
        p.pos.z = FIX_Z_VALUE(p.pos.z);

        // scale the thing
        float scale = (float)random(AURA_MIN_SCL, AURA_MAX_SCL) * .01f;
        p.scl.x = scale;
        p.scl.y = scale;
        p.scl.z = scale;

        // rotate the thing
        p.rot.x = -90.0f;
        p.rot.z = -45.0f;
        p.rot.y = 0.0f;

        // set the acceleration
        p.acc.x = p.acc.y = p.acc.z = 1.0f;

        // set the velocity
        p.vel.x = 0.0f;
        p.vel.y = 0.0f;
        p.vel.z = (float)random(AURA_MIN_Z, AURA_MAX_Z) * .1f;

        p.life_span = random(AURA_MIN_LIFE, AURA_MAX_LIFE);

        fire.Add(&p);
    }

    BOOL done = TRUE;

    // special fire animation
    for(i = 0; i < AURA_COUNT; ++i)
    {
        particle = fire.Get(i);
        if(!particle->used)
            continue;

        done = FALSE;
        particle->scl.x *= AURA_DEC;
        particle->scl.y *= AURA_DEC;
        particle->scl.z *= AURA_DEC;
    }

    if(frame >= AURA_FRAME && to_add == 0 && done)
        ((PTEffect)inst)->KillThisEffect();
}

BOOL TAuraAnimator::Render()
{
    SaveBlendState();
    SetAddBlendState();

    fire.Render();

    RestoreBlendState();

    return TRUE;
}

void TAuraAnimator::RefreshZBuffer()
{
    S3DPoint pos, screen, size;
    PTCharacter character = ((PTAuraEffect)inst)->GetCharacter();

    if(!character)
    {
        ((PTEffect)inst)->KillThisEffect();
        return;
    }

    character->GetPos(pos);

    WorldToScreen(pos, screen);

    size.x = 300;
    size.y = 300;

    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

//*// **********************
//*// * Lightning Animator *
//*// **********************
//*
//*REGISTER_3DANIMATOR("LIGHTNING2", TLightningAnimator)
//*
//*//==============================================================================
//*//    Function : Initialize.
//*//------------------------------------------------------------------------------
//*// Description :
//*//
//*//==============================================================================
//*
//*void TLightningAnimator::Initialize()
//*{
//* T3DAnimator::Initialize();
//* S3DPoint pos;
//*    S3DPoint dir;
//*
//*    inst->GetPos(pos);
//*    inst->GetVel(dir);
//*
//*    for (int n = 0; n < NUM_LIGHTNING_SEGMENTS; n++)
//*    {
//*        p[n].x = (D3DVALUE)pos.x + dir.x / ROLLOVER * n;
//*        p[n].y = (D3DVALUE)pos.y + dir.y / ROLLOVER * n;
//*        p[n].z = FIX_Z_VALUE(pos.z);
//*    }
//*
//*    framenum = 0;
//*}
//*
//*//==============================================================================
//*//    Function : Animate.
//*//------------------------------------------------------------------------------
//*// Description :
//*//
//*//==============================================================================
//*
//*void TLightningAnimator::Animate(BOOL draw)
//*{
//* T3DAnimator::Animate(draw);
//*
//*    framenum++;
//*    if (framenum >= LIGHTNING_DURATION)
//*        inst->SetCommandDone(TRUE);
//*}
//*
//*//==============================================================================
//*//    Function : Render.
//*//------------------------------------------------------------------------------
//*// Description :
//*//
//*//==============================================================================
//*
//*BOOL TLightningAnimator::Render()
//*{
//*    SaveBlendState();
//*    SetBlendState();
//*
//*    PS3DAnimObj obj = GetObject(0);
//*
//*    for (int n = 0; n < NUM_LIGHTNING_SEGMENTS; n++)
//* {
//*        int r = random(1, 2);
//*        for (int c = 0; c < r; c++)
//*        {
//*            Get3DImagery()->ResetExtents();             // Reset render extents
//*
//*            //obj->flags = OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS;
//*            obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3 | OBJ3D_ABSPOS;
//*            obj->rot.x = obj->rot.y = D3DVAL(0.0);
//*            obj->rot.z = (D3DVALUE)((PTLightning2Effect)inst)->angle * (D3DVALUE)M_PI * 2 / 255;
//*            obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)1.75;
//*            obj->pos = p[n];
//*
//*            if ((c) && (random(0, 1)))
//*            {
//*                obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3 | OBJ3D_ABSPOS;
//*                obj->rot.x = obj->rot.y = D3DVAL(0.0);
//*                obj->rot.z = D3DVAL(random(1, 20)) / D3DVAL(3.0);
//*            }
//*
//*            frame = random(0, 2);
//*
//*            RenderObject(obj);
//*
//*            UpdateExtents();            // Updates bound rect and screen rect
//*        }
//* }
//*
//*    RestoreBlendState();
//*
//* return TRUE;
//*}

// *****************************
// * Fountain Sparkle Animator *
// *****************************

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

void TFountainAnimator::Initialize()
{
    T3DAnimator::Initialize();

    // Initialize the starting positions of the bubbles
    for (int n = 0; n < NUM_FOUNTAIN_BUBBLES; n++)
    {
        p[n].x = (D3DVALUE)(random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS));
        p[n].y = (D3DVALUE)(random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS));
        p[n].z = (D3DVALUE)0.0;
        rise[n] = (D3DVALUE)(random(1, 3) / (D3DVALUE)2.0);
        scale[n] = 2.0;
        framenum[n] = random(-NUM_FOUNTAIN_BUBBLES / 2, 0);
    }

    SetColorObject();
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the frame number, position, and scale values
//               of all the bubbles.
//==============================================================================

void TFountainAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

    // Update all the bubbles
    for (int n = 0; n < NUM_FOUNTAIN_BUBBLES; n++)
    {
        // Update the frame counter
        framenum[n]++;

        if (framenum[n] > 0)
        {
            // Float the bubbles up
            p[n].z += rise[n];

            // Scale the bubbles down
            scale[n] -= (D3DVALUE)FOUNTAIN_SCALE_STEP;

            // See if the bubble has shrunk out of sight, and reset it if it has
            if (scale[n] <= 0)
            {
                p[n].x = (D3DVALUE)(random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS));
                p[n].y = (D3DVALUE)(random(-FOUNTAIN_RADIUS, FOUNTAIN_RADIUS));
                p[n].z = (D3DVALUE)0.0;
                rise[n] = (D3DVALUE)(random(1, 3) / (D3DVALUE)2.0);
                scale[n] = 2.0;
                framenum[n] = random(-NUM_FOUNTAIN_BUBBLES / 2, 0);
            }
        }
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : This loops through all the fountain bubbles and renders each
//               at the appropriate location and scale value.
//==============================================================================

BOOL TFountainAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(colorobj);

    for (int n = 0; n < NUM_FOUNTAIN_BUBBLES; n++)
    {
        if (framenum[n] > 0)
        {
            Get3DImagery()->ResetExtents();             // Reset render extents

            obj->flags = OBJ3D_SCL1 | OBJ3D_POS2;
            obj->scl.x = obj->scl.y = obj->scl.z = scale[n];
            obj->pos = p[n];

            RenderObject(obj);

            UpdateExtents();            // Updates bound rect and screen rect
        }
    }

    RestoreBlendState();

    return TRUE;
}


void TFountainAnimator::RefreshZBuffer()
{
    S3DPoint screen, effect;

    ((PTEffect)inst)->GetPos(effect);


    int size_x = 100;
    int size_y = 75;

    WorldToScreen(effect, screen);
    RestoreZ((screen.x - (size_x / 2))+0, screen.y - size_y + 30, size_x, size_y);
    //Display->Box((screen.x - (size_x / 2))+0, screen.y - size_y + 30, size_x, size_y);
}

// **************************
// * Cyan Fountain Animator *
// **************************

REGISTER_3DANIMATOR("CYANFONT", TCyanFountainAnimator)

// *************************
// * Red Fountain Animator *
// *************************

REGISTER_3DANIMATOR("REDFONT", TRedFountainAnimator)

// ***************************
// * Green Fountain Animator *
// ***************************

REGISTER_3DANIMATOR("GREENFONT", TGreenFountainAnimator)

// **************************
// * Blue Fountain Animator *
// **************************

REGISTER_3DANIMATOR("BLUEFONT", TBlueFountainAnimator)


// ********************************************
// * Revive Effect (uses Ribbon Animator)
// ********************************************


// the stage identifiers
enum
{
    REVIVE_STARTSTAGE = 0,
    REVIVE_STAYSTAGE,
    REVIVE_DECAYSTAGE,
};

// tick counts for each stage...
#define REVIVE_STARTTICKS       (20)
#define REVIVE_STAYTICKS        (90)
#define REVIVE_DECAYTICKS       (20)



_CLASSDEF(TReviveEffect)

class TReviveEffect : public TEffect
{
protected:
public:
    int stage;
    int ticks;
    PTCharacter target;
    int first_time;

    TReviveEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TReviveEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
};

DEFINE_BUILDER("Ribbon", TReviveEffect)
REGISTER_BUILDER(TReviveEffect)


void TReviveEffect::Initialize()
{
    SetCommandDone(FALSE);

    first_time = 0;
    target = NULL;

    stage = REVIVE_STARTSTAGE;
    ticks = REVIVE_STARTTICKS;
}

void TReviveEffect::Pulse()
{
    TEffect::Pulse();

    if (first_time)
    {
        if (GetSpell())
        {
            S3DPoint temp_point1;
            S3DPoint temp_point2;
            PTCharacter caster = (PTCharacter)((PTSpell)GetSpell())->GetInvoker();

            caster->GetPos(temp_point1);

            for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
            {
                PTCharacter chr = (PTCharacter)i.Item();

                if (chr == caster)
                    continue;

                if (!(chr->IsDead()))
                    continue;

                chr->GetPos(temp_point2);

                if (::Distance(temp_point1,temp_point2) > 200)
                    continue;

                if (!(caster->IsEnemy(chr)))
                        target = chr;

                break;
            }

        }

        first_time = 0;
    }

    if ((!(target)) && GetSpell())
    {
        KillThisEffect();
        return;
    }

    if (!ticks)
    {
        stage++;

        switch (stage)
        {
            case  REVIVE_STAYSTAGE:
                ticks = REVIVE_STAYTICKS;
                break;

            case REVIVE_DECAYSTAGE:
                if (GetSpell())
                {
                    if (target)
                    {
                        // get the target, and try to set it's walk animation as the root...
                        ((PTComplexObject)target)->Force("walk");
                        // restore the character's health...
                        target->RestoreHealth();
                    }
                    ticks = REVIVE_DECAYTICKS;
                }
                else
                {
                    stage--;
                    ticks = 1;
                }
                break;
        }

        if (stage > REVIVE_DECAYSTAGE)  KillThisEffect();
    }

    ticks--;
}


// *******************
// * Ribbon Animator *
// *******************

REGISTER_3DANIMATOR("RIBBON", TRibbonAnimator)

#define RIBBON_MINSCALE         (0.05f)
#define RIBBON_MAXSCALE         (4.5f)
#define RIBBON_GROWFRAMES       REVIVE_STARTTICKS

#define RIBBON_SCALEINC         ((RIBBON_MAXSCALE - RIBBON_MINSCALE) / RIBBON_GROWFRAMES)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

void TRibbonAnimator::Initialize()
{
    int n;
    S3DTex tex;
    S3DPoint target_pos;

    T3DAnimator::Initialize();

    // Get how many frames are in the texture
    Get3DImagery()->GetTexture(0, &tex);
    numtexframes = tex.numframes;

    ribbontimer = 0;

    // Initialize the starting positions of the ribbons
    if (((PTReviveEffect)inst)->target)
    {
        (((PTReviveEffect)inst)->target)->GetPos(target_pos);

        ribpos.x = (float)target_pos.x;
        ribpos.y = (float)target_pos.y;
        ribpos.z = (float)target_pos.z;
    }
    else
    {
        ribpos.x = ribpos.y = 0;
        ribpos.z = 0;
    }

    ribscale = RIBBON_MINSCALE;
    for (n = 0; n < NUM_RIBBONS; n++)
        rotation[n] = (D3DVALUE)n * 2;

    // Initialize the starting positions of the ribbon sparks
    for (n = 0; n < NUM_RIBBON_SPARKS; n++)
    {
        p[n].x = (D3DVALUE)(random(-RIBBON_RADIUS, RIBBON_RADIUS));
        p[n].y = (D3DVALUE)(random(-RIBBON_RADIUS, RIBBON_RADIUS));
        p[n].z = (D3DVALUE)(random(0, RIBBON_RADIUS / 2));

        v[n].x = -p[n].x / RIBBON_SPARK_DURATION / 2;
        v[n].y = -p[n].y / RIBBON_SPARK_DURATION / 2;
        v[n].z = .5;

        scale[n] = 0.0;
        framenum[n] = random(-NUM_RIBBON_SPARKS, 0);
    }

    centertilt = 0.0f;
    centertilt_dx = 0.001f;
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

void TRibbonAnimator::Animate(BOOL draw)
{
    int n;

    T3DAnimator::Animate(draw);
//  inst->SetCommandDone(FALSE);

    // Update the ribbon timer
    ribbontimer++;

    // Update all ribbons
    for (n = 0; n < NUM_RIBBONS; n++)
    {
        // Rotate the ribbons
        rotation[n] += (D3DVALUE)0.2;
    }

    if (((PTReviveEffect)inst)->GetSpell())
    {
        switch (((PTReviveEffect)inst)->stage)
        {
            case REVIVE_STARTSTAGE: 
                if (ribscale <= RIBBON_MAXSCALE)
                {
                    ribscale += RIBBON_SCALEINC;
                }
                break;

            case REVIVE_DECAYSTAGE:
                if (ribscale >= RIBBON_MINSCALE)
                {
                    ribscale -= RIBBON_SCALEINC;
                }
                ribpos.z += 10.0f;
                break;
        }
    }
    else
    {
        if (ribscale <= RIBBON_MAXSCALE)
        {
            ribscale += RIBBON_SCALEINC;
        }
    }



    // Update all ribbon sparks
    for (n = 0; n < NUM_RIBBON_SPARKS; n++)
    {
        framenum[n]++;

        if (framenum[n] > 0)
        {
            // See if this spark is done
            if (framenum[n] >= RIBBON_SPARK_DURATION)
            {
                //if (ribbontimer < RIBBON_DURATION)
                {
                    p[n].x = (D3DVALUE)(random(-RIBBON_RADIUS, RIBBON_RADIUS));
                    p[n].y = (D3DVALUE)(random(-RIBBON_RADIUS, RIBBON_RADIUS));
                    p[n].z = (D3DVALUE)(random(0, RIBBON_RADIUS / 2));

                    v[n].x = -p[n].x / RIBBON_SPARK_DURATION / 2;
                    v[n].y = -p[n].y / RIBBON_SPARK_DURATION / 2;
                    v[n].z = .5;

                    framenum[n] = 0;
                }
                scale[n] = 0.0;
            }
            else
            if (framenum[n] > RIBBON_SPARK_DURATION / 2)
                scale[n] -= RIBBON_SPARK_SCALE_STEP;
            else
                scale[n] += RIBBON_SPARK_SCALE_STEP;

            // Move the sparks
            p[n].x += v[n].x;
            p[n].y += v[n].y;
            p[n].z += v[n].z;

            // Accelerate the sparks movements upward
            v[n].z += 0.5;
        }
    }

    centertilt += centertilt_dx;
    if ((centertilt > 0.25f) || (centertilt < -0.25f))
    {
        centertilt_dx = -(centertilt_dx);
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

BOOL TRibbonAnimator::Render()
{
    int n;

    SaveBlendState();
    SetBlendState();

    //
    // Draw the ribbons sparks here
    //

    PS3DAnimObj obj = GetObject(0);

    for (n = 0; n < NUM_RIBBON_SPARKS; n++)
    {
        if ((framenum[n] > 0) && (scale[n] > 0))
        {
            Get3DImagery()->ResetExtents();             // Reset render extents

            obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3;
            obj->rot.x = D3DVAL(-(M_PI / 2.0));
            obj->rot.y = D3DVAL(0.0);
            obj->rot.z = D3DVAL(-(M_PI / 4.0));
            obj->scl.x = obj->scl.y = obj->scl.z = scale[n];
            obj->pos = p[n];

            // Set the frame number manually
            frame = framenum[n] % numtexframes;

            RenderObject(obj);

            UpdateExtents();            // Updates bound rect and screen rect
        }
    }

    //
    // Draw the center floor spark here
    //

    Get3DImagery()->ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_SCL1;// | OBJ3D_ROT2;
    obj->scl.x = obj->scl.y = obj->scl.z = ribscale * 2.5f + ((float)random(0, 10) / 20.0f);
//    obj->rot.x = obj->rot.y = obj->rot.z = centertilt;

    // Set the frame number manually
    frame = ribbontimer % numtexframes;

    RenderObject(obj);

    UpdateExtents();            // Updates bound rect and screen rect


    //
    // Draw the ribbons here
    //

    obj = GetObject(1);

    for (n = 0; n < NUM_RIBBONS; n++)
    {
        Get3DImagery()->ResetExtents();             // Reset render extents

        obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3;
        obj->scl.x = obj->scl.y = ribscale;
        obj->scl.z = ribscale * 2.0f;
        obj->rot.z = rotation[n];
        obj->pos = ribpos;

        RenderObject(obj);

        UpdateExtents();            // Updates bound rect and screen rect
    }

    RestoreBlendState();

    return TRUE;
}

// *******************
// * Shield Animator *
// *******************

REGISTER_3DANIMATOR("SHIELD", TShieldAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

void TShieldAnimator::Initialize()
{
    T3DAnimator::Initialize();

    pos.x = pos.y = 0;
    pos.z = 40;
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

void TShieldAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

BOOL TShieldAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(0);

    Get3DImagery()->ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_MATRIX; 
    obj->scl.x = obj->scl.y = obj->scl.z = SHIELD_SCALE;
    obj->pos = pos;

    D3DMATRIXClear(&obj->matrix);

    D3DMATRIXRotateX(&obj->matrix, D3DVAL(-(M_PI / 3.0)));
    D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-(M_PI / 4.0)));

    D3DMATRIXScale(&obj->matrix, &obj->scl);
    D3DMATRIXTranslate(&obj->matrix, &obj->pos);

    RenderObject(obj);

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

// ******************
// * Flame Animator *
// ******************

REGISTER_3DANIMATOR("FLAME", TFlameAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

void TFlameAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PS3DAnimObj obj = GetObject(0);
    GetVerts(obj, D3DVT_LVERTEX);
    
    frame = 0;
}

//============================================================================== 
//    Function : Animate.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

void TFlameAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

    ++frame;
    if (frame >= 18)
        frame = 0;
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

BOOL TFlameAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(0);

    ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_MATRIX | OBJ3D_VERTS;
    D3DMATRIXClear(&obj->matrix);

    D3DMATRIXRotateX( &obj->matrix, D3DVAL( 45 * TORADIAN));
    D3DMATRIXRotateY( &obj->matrix, D3DVAL( 30 * TORADIAN));
    D3DMATRIXRotateZ( &obj->matrix, D3DVAL(160 * TORADIAN));

    obj->scl.x = .5f;
    obj->scl.y = .5f;
    obj->scl.z = .5f;
    D3DMATRIXScale(&obj->matrix, &obj->scl);

    obj->pos.x = obj->pos.y = obj->pos.z = 0.0f;
    D3DMATRIXTranslate(&obj->matrix, &obj->pos);

    float xpos = (float)((int)(frame * 11 / 24) % 4) * .25f;
    float ypos = (float)((int)(frame * 11 / 24) / 4) * .5f;

    obj->lverts[0].tu = xpos;
    obj->lverts[0].tv = ypos;

    obj->lverts[1].tu = xpos + .25f;
    obj->lverts[1].tv = ypos;

    obj->lverts[2].tu = xpos;
    obj->lverts[2].tv = ypos + .5f;

    obj->lverts[3].tu = xpos + .25f;
    obj->lverts[3].tv = ypos + .5f;

    RenderObject(obj);

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

void TFlameAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = (int)(50.0f * .5f);
    size.y = (int)(125.0f * .5f);

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - size.y / 2, size.x, size.y);
}

// ********************
// * SymGlow Animator *
// ********************

REGISTER_3DANIMATOR("SymGlow", TSymGlowAnimator)

// Sets up the objects to animate (simply uses whatever objects are in the imagery)
// Override this function to set up whatever objects you need for your effect, etc.
void TSymGlowAnimator::SetupObjects()
{
    timer = 0;
    zscale = (D3DVALUE)2.0;
    dz = (D3DVALUE)0.1;

    for (int c = 0; c < Get3DImagery()->NumObjects(); c++)
    {
        PS3DAnimObj o = NewObject(c);
        GetVerts(o, D3DVT_VERTEX);
        AddObject(o);
        for (int v = 0; v < o->numverts; v++)
        {
    //      objBarrier->lverts[v].tu += 0.01f;
            o->lverts[v].tv -= 0.01f;
        }
    }

}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

void TSymGlowAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

    timer++;

    if (!(timer % 40))
        dz *= -1;

    zscale += dz;

    if (zscale < 2.0)
        zscale = 2.0;
    else
    if (zscale > 5.0)
        zscale = 5.0;

    u = (D3DVALUE)random(2, 8) / (D3DVALUE)100.0;
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description :
//
//==============================================================================

BOOL TSymGlowAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(0);

    Get3DImagery()->ResetExtents();             // Reset render extents

    for (int n = 0; n < obj->numverts; n++)
        obj->verts[n].tu += u;

    obj->flags = OBJ3D_SCL1 | OBJ3D_VERTS;
    obj->scl.x = obj->scl.y = (D3DVALUE)1.4;
    obj->scl.z = zscale;
    
    RenderObject(obj);

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}


void TSymGlowAnimator::RefreshZBuffer()
{
    S3DPoint screen, effect;

    ((PTEffect)inst)->GetPos(effect);


    int size_x = 100;
    int size_y = 170;

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size_x / 2), screen.y - size_y + 30, size_x, size_y);
    //Display->Box(screen.x - (size_x / 2), screen.y - size_y + 30, size_x, size_y);
}

// *********************
// * Particle Animator *
// *********************

//REGISTER_3DANIMATOR_("blood", TBloodAnimator, TBloodParticle3DAnimator)
REGISTER_MULTI_3DANIMATOR_("sparks", TSparkAnimator, TParticle3DAnimator)
/*REGISTER_MULTI_3DANIMATOR_("icedsparks", TIcedSparksAnimator, TParticle3DAnimator)                    \
REGISTER_MULTI_3DANIMATOR_("sand", TSandAnimator, TParticle3DAnimator)                                  \
REGISTER_MULTI_3DANIMATOR_("blood", TBloodAnimator, TParticle3DAnimator)                                    \
REGISTER_MULTI_3DANIMATOR_("snow", TSnowAnimator, TParticle3DAnimator)*/

void TParticle3DAnimator::Initialize()
{
    T3DAnimator::Initialize();

    memset(&params, 0, sizeof(SParticleParams));
    p = NULL;
    v = NULL;
    l = NULL;
    s = NULL;
    o = NULL;
    ti = NULL;
}

void TParticle3DAnimator::Close()
{
    if (p)
        delete p;
    if (v)
        delete v;
    if (l)
        delete l;
    if (s)
        delete s;
    if (o)
        delete o;
    if (ti)
        delete ti;

    T3DAnimator::Close();
}

//void TParticle3DAnimator::SetupObjects()
//{
//  T3DAnimator::SetupObjects(); // Don't change
//}


void TParticle3DAnimator::ResetTargetInfo(SParticleParams nparams)
{
    int i;
    params.seektargets = nparams.seektargets;
    params.seekspeed = nparams.seekspeed;
    params.turnang = nparams.turnang;
    params.autorange = nparams.autorange;
    params.hitrange = nparams.hitrange;
    if (ti)
        delete ti;
    if (params.numtargets)
        ti = new int[params.particles];
    params.numtargets = nparams.numtargets;
    for (i = 0; i < params.numtargets; i++)
        params.targetpos[i] = nparams.targetpos[i];
    for (i = 0; i < params.particles; i++)
        if (params.numtargets > 0)
            ti[i] = random(0, params.numtargets - 1);
}

// Sets up particle animator
void TParticle3DAnimator::InitParticles(PSParticleParams nparams)
{
    memcpy(&params, nparams, sizeof(SParticleParams)); 

    p = new D3DVECTOR[params.particles];
    v = new D3DVECTOR[params.particles];
    s = new int[params.particles];
    l = new int[params.particles];
    o = new int[params.particles];
    if (params.numtargets)
        ti = new int[params.particles];

    BYTE objs[32];
    int numobjs = 0;
    for (int c = 0; c < NumObjects(); c++)
    {
        if (params.objflags & (1 << c))
            objs[numobjs++] = c;
    }
    if (numobjs <= 0)
        objs[numobjs++] = 0;

    for (c = 0; c < params.particles; c++)
    {
        p[c].x = params.pos.x + params.pspread.x * ((D3DVALUE)random(-100, 100) / (D3DVALUE)100.0); 
        p[c].y = params.pos.y + params.pspread.y * ((D3DVALUE)random(-100, 100) / (D3DVALUE)100.0); 
        p[c].z = params.pos.z + params.pspread.z * ((D3DVALUE)random(-100, 100) / (D3DVALUE)100.0); 
        v[c].x = params.dir.x + params.spread.x * ((D3DVALUE)random(-100, 100) / (D3DVALUE)100.0); 
        v[c].y = params.dir.y + params.spread.y * ((D3DVALUE)random(-100, 100) / (D3DVALUE)100.0); 
        v[c].z = params.dir.z + params.spread.z * ((D3DVALUE)random(-100, 100) / (D3DVALUE)100.0); 
        l[c] = random(params.minlife, params.maxlife);
        s[c] = random(params.minstart, params.maxstart);
        o[c] = objs[random(0, numobjs - 1)]; 
        if (params.numtargets > 0)
            ti[c] = random(0, params.numtargets - 1);
    }
}

void TParticle3DAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

    if (params.particles == 0) // Do sample particles
    {
        SParticleParams pr;
        pr.particles = 10;
        pr.pos.x = (D3DVALUE)0.0;
        pr.pos.y = (D3DVALUE)0.0;
        pr.pos.z = (D3DVALUE)70.0;
        pr.pspread.x = (D3DVALUE)3.0;
        pr.pspread.y = (D3DVALUE)3.0;
        pr.pspread.z = (D3DVALUE)3.0;
        pr.dir.x = (D3DVALUE)1.0;
        pr.dir.y = (D3DVALUE)-1.0;
        pr.dir.z = (D3DVALUE)0.5;
        pr.spread.x = (D3DVALUE)0.5;
        pr.spread.y = (D3DVALUE)0.5;
        pr.spread.z = (D3DVALUE)0.5;
        pr.gravity = (D3DVALUE)0.2;
        pr.trails = 1;
        pr.minstart = 0;
        pr.maxstart = 8;
        pr.minlife = 10;
        pr.maxlife= 30;
        pr.bounce = FALSE;
        pr.killobj = FALSE; 
        pr.objflags = 0xF;
        pr.numtargets = 0;
        pr.seektargets = FALSE;
        pr.seekz = FALSE;
        
        InitParticles(&pr);
    }

    BOOL isdone = TRUE;
    D3DVALUE tx, ty, tz, dx, dy, dz,
        angxy, angz, actang, actangz, rightdif, leftdif, rad, actrad;
    for (int c = 0; c < params.particles; c++)
    {
        if (l[c] <= 0)
            continue;

        isdone = FALSE;

        if (s[c] > 0)
        {
            s[c]--;
            continue;
        }

        p[c].x += v[c].x;
        p[c].y += v[c].y;
        p[c].z += v[c].z;
        // decrease time //(if not seeking targets)
            l[c]--;
        if (!params.seektargets)
        {
            // apply gravity (if not seeking targets)
            v[c].z -= params.gravity;
        }
        if (params.bounce && p[c].z < (D3DVALUE)0.0)
        {
            p[c].z = -p[c].z;
            v[c].z = -v[c].z * (D3DVALUE)0.5;
            if (v[c].z < (D3DVALUE)2.0)     // Don't bounce too much
                l[c] = 0;
        }
        if (params.seektargets && params.numtargets > 0)
        {
            // acquire target x, y, z
            tx = params.targetpos[ti[c]].x;
            ty = params.targetpos[ti[c]].y;
            tz = params.targetpos[ti[c]].z;
            // if close, interpolate
            if (p[c].x > tx - params.autorange && p[c].x < tx + params.autorange &&
                p[c].y > ty - params.autorange && p[c].y < ty + params.autorange)
            {
                p[c].x = (D3DVALUE)(p[c].x + tx) / 2;
                p[c].y = (D3DVALUE)(p[c].y + ty) / 2;
                p[c].z = (D3DVALUE)(p[c].z + tz) / 2;
                v[c].x = (D3DVALUE)0.0;
                v[c].y = (D3DVALUE)0.0;
                v[c].z = (D3DVALUE)0.0;
            }
            else
            // far away, seek with turning
            {
                // calculate angle in the x-y plane
                dx = tx - p[c].x;
                dy = ty - p[c].y;
                dz = tz - p[c].z;
                angxy = (D3DVALUE)atan2(dy, dx),
                rad = (D3DVALUE)sqrt((dy * dy) + (dx * dx));
                angz = (D3DVALUE)atan2(dz, rad);

                // in the up-down direction
                actang = (D3DVALUE)atan2(v[c].y, v[c].x);
                actrad = (D3DVALUE)sqrt((v[c].y * v[c].y) + (v[c].x * v[c].x));
                actangz = (D3DVALUE)atan2(v[c].z, actrad);

                leftdif = angxy - actang; // calculate which way to turn . . .
                if (leftdif < 0.0)
                    leftdif += (D3DVALUE)(M_2PI);
                if (leftdif > M_2PI)
                    leftdif -= (D3DVALUE)M_2PI;
                rightdif = actang - angxy;
                if (rightdif < 0.0)
                    rightdif += (D3DVALUE)(M_2PI);
                if (rightdif > M_2PI)
                    rightdif -= (D3DVALUE)M_2PI;
                if (leftdif < rightdif) // turn left
                    actang += min(params.turnang, leftdif);
                else // turn right
                    actang -= min(params.turnang, rightdif);

                if (params.seekz)
                {
                    leftdif = angz - actangz; // calculate which way to turn (not really right
                    if (leftdif < 0.0)          // or left, but up and down)
                        leftdif += (D3DVALUE)(M_2PI);
                    if (leftdif > M_2PI)
                        leftdif -= (D3DVALUE)M_2PI;
                    rightdif = actangz - angz;
                    if (rightdif < 0.0)
                        rightdif += (D3DVALUE)(M_2PI);
                    if (rightdif > M_2PI)
                        rightdif -= (D3DVALUE)M_2PI;
                    if (leftdif < rightdif) // turn left
                        actangz += params.turnang;
                    else // turn right
                        actangz -= params.turnang;
                }

                v[c].x = (D3DVALUE)(params.seekspeed * cos(actang));
                v[c].y = (D3DVALUE)(params.seekspeed * sin(actang));
                rad = (float)sqrt((v[c].y * v[c].y) + (v[c].x * v[c].x));
                v[c].z = (D3DVALUE)(rad * sin(actangz));
            }
            // hit target! be done!
            if (p[c].x > tx - params.hitrange && p[c].x < tx + params.hitrange &&
                p[c].y > ty - params.hitrange && p[c].y < ty + params.hitrange)
            {
                l[c] = 0;
            }
        }
    }

    inst->SetCommandDone(isdone);
    if (params.killobj && isdone)
        inst->SetFlags(OF_KILL);  // Causes object to commit suicide
}

BOOL TParticle3DAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    ResetExtents();             // Reset render extents

    for (int c = 0; c < params.particles; c++)
    {
        if (s[c] > 0 || l[c] <= 0)
            continue;

        D3DVECTOR pp = p[c];
        D3DVECTOR vv = v[c];

        PS3DAnimObj obj = GetObject(o[c]);

        for (int d = 0; d < params.trails; d++)
        {
            obj->flags |= OBJ3D_POS1;
            obj->pos = pp;
            RenderObject(obj);

            pp.x += vv.x;
            pp.y += vv.y;
            pp.z += vv.z;
            vv.z -= params.gravity;
            if (params.bounce && pp.z < (D3DVALUE)0.0)
            {
                pp.z = -pp.z;
                vv.z = -vv.z * (D3DVALUE)0.5;
                if (vv.z < (D3DVALUE)2.0)       // Don't bounce too much
                    break;
            }
        }

    }

    UpdateExtents();            // Updates screen rect and bound rect
    RestoreBlendState();

    return TRUE;
}


void TParticle3DAnimator::RefreshZBuffer()
{
    S3DPoint screen, effect;

    ((PTEffect)inst)->GetPos(effect);


    int size_x = 75;
    int size_y = 50;

    WorldToScreen(effect, screen);
    RestoreZ((screen.x - (size_x / 2))+25, screen.y - size_y - 50, size_x, size_y);
    //Display->Box((screen.x - (size_x / 2))+25, screen.y - size_y - 50, size_x, size_y);
}

// *********
// * Blood *
// *********

/*void TBloodParticle3DAnimator::Initialize()
{
    TParticle3DAnimator::Initialize();
}

void TBloodParticle3DAnimator::Close()
{
    TParticle3DAnimator::Close();
}

void TBloodParticle3DAnimator::Animate(BOOL draw)
{
    TParticle3DAnimator::Animate(draw);

    // tenshu   
}

BOOL TBloodParticle3DAnimator::Render()
{
    TParticle3DAnimator::Render();
}


void TBloodParticle3DAnimator::RefreshZBuffer()
{
    TParticle3DAnimator::RefreshZBuffer();  
}*/

// **********************
// * TIrisFlareAnimator *
// **********************

_CLASSDEF(TIrisFlareAnimator)
class TIrisFlareAnimator : public T3DAnimator
{
  public:
    D3DVALUE rotation;
    D3DVECTOR p[5];       // Position of the flares
    int iterations,ticks;
    float cylsize1, cylsize2;
    D3DVALUE growx, growy;

    TIrisFlareAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    virtual ~TIrisFlareAnimator() { Close(); }

    virtual void Initialize();
      // Initializes velocity vectors and positions

    virtual void Animate(BOOL draw);
    virtual BOOL Render();
};

REGISTER_3DANIMATOR("IrisFlare", TIrisFlareAnimator)

void TIrisFlareAnimator::Initialize()
{
    int i;

    T3DAnimator::Initialize();

    inst->Face( -32);

    for (i = 0; i < 5; i++)
    {
        p[i].y = 20;
        p[i].z = 45;
    }
    p[1].x = -5;
    p[2].x = 5;
    p[3].z += 20;
    p[4].z -= 20;

    iterations = 0;
    ticks = 0;
    cylsize1 = cylsize2 = 1.0;
    growx = D3DVAL(15.0 / 50);
    growy = growx / 3;
}

void TIrisFlareAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    // Rotate the cylindrical glow
    rotation += (D3DVALUE)0.1;

    if (iterations < 50)
    {
        if (iterations >= 25)
        {
            p[1].x += D3DVAL( 0.5);
            p[2].x -= D3DVAL( 0.5);
        }
        p[3].z -= D3DVAL( 1);
        p[4].z += D3DVAL( 1);

        iterations++;
    }
    ticks++;
    
    if (ticks > 200)
    {
        iterations = 0;
        ticks = 0;
        for (int i = 0; i < 5; i++)
        {
            p[i].y = 20;
            p[i].z = 45;
        }
        p[1].x = -5;
        p[2].x = 5;
        p[3].z += 20;
        p[4].z -= 20;

        iterations = 0;
        ticks = 0;
        cylsize1 = cylsize2 = 1.0;
        growx = D3DVAL(15.0 / 50);
        growy = growx / 3;
    }

}

BOOL TIrisFlareAnimator::Render()
{
    int i;

    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;

    // Draw the Flare
    obj = GetObject(0);

    Get3DImagery()->ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_MATRIX;

    obj->scl.x = obj->scl.y = D3DVAL(growx * iterations);
    obj->scl.z = 15;
    obj->pos = p[0];

    D3DMATRIXClear( &obj->matrix);

    D3DMATRIXScale( &obj->matrix, &obj->scl);
    D3DMATRIXRotateX( &obj->matrix, D3DVAL( -90 * TORADIAN));

    D3DMATRIXTranslate( &obj->matrix, &obj->pos);

    RenderObject(obj);

    // Vertical flares
    obj->scl.x = obj->scl.y = D3DVAL(growy * iterations);
    obj->scl.z = 8 + D3DVAL(growy * iterations);

    for (i = 1; i < 3; i++)
    {
        D3DMATRIXClear( &obj->matrix);

        D3DMATRIXScale( &obj->matrix, &obj->scl);
        D3DMATRIXRotateX( &obj->matrix, D3DVAL( -90 * TORADIAN));

        obj->pos = p[i];

        D3DMATRIXTranslate( &obj->matrix, &obj->pos);

        RenderObject(obj);

        UpdateExtents();            // Updates bound rect and screen rect
    }
    
    // Horizontal flares
    obj->flags = OBJ3D_SCL1 | OBJ3D_ROT2; // | OBJ3D_POS3;
    obj->scl.x = obj->scl.y = D3DVAL(growy * iterations);
    obj->scl.z = 8 + D3DVAL(growy * iterations);

    for (i = 3; i < 5; i++)
    {
        D3DMATRIXClear( &obj->matrix);

        D3DMATRIXScale( &obj->matrix, &obj->scl);
        D3DMATRIXRotateX( &obj->matrix, D3DVAL( -90 * TORADIAN));
        D3DMATRIXRotateY( &obj->matrix, D3DVAL( 90 * TORADIAN));

        obj->pos = p[i];

        D3DMATRIXTranslate( &obj->matrix, &obj->pos);

        RenderObject(obj);

        UpdateExtents();            // Updates bound rect and screen rect
    }
    obj->rot.z = D3DVAL((float)(ticks*(360.0 / 50.0)) *TORADIAN);

    // Draw the cylindrical glow around the player
    obj = GetObject(1);

    Get3DImagery()->ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2; // | OBJ3D_POS3;

    obj->rot.x = obj->rot.y = D3DVAL(0.0);
    obj->rot.z = rotation;

    if (iterations < 50)
    {
        cylsize1 = (float)((float)(iterations) / 10.0);
        cylsize2 = (float)((float)(iterations) / 10.0);
    }
    else if (iterations > 150)
    {
        cylsize1 -= (float)0.1;
        cylsize2 -= (float)0.1;
    }

    // funky code to make it go zweerree vwweerree zweeree (etc..)
    /*
    int change = (ticks % 10) - 5;
    if (change < 0)
    {
        cylsize1 -= 0.2;
        cylsize2 += 0.2;
    }   
    else
    {
        cylsize1 += 0.2;
        cylsize2 -= 0.2;
    }*/

    obj->scl.x = obj->scl.y = obj->scl.z = cylsize1;

    RenderObject(obj);
    obj->rot.z = -rotation;
    obj->scl.x = obj->scl.y = obj->scl.z = cylsize2;
    RenderObject(obj);

    if (ticks < 20)
    {
        obj->scl.z = (float)(20 - ticks);
        if (obj->scl.z > 10)
            obj->scl.z = 10;
        obj->scl.x = obj->scl.y = (float)ticks*3;
        RenderObject(obj);
    }

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

#if 0

void MakeFractal(int numpoints, int points[], int start, int stop, int min, int max)
{
    int range = max - min;

    int *p = points;

    

    for (int c = 0; c < ; )
    {
    }


}

#endif


/****************************************************************/
/*                       SetVortex Code                         */
/****************************************************************/

// *************************
// *    SetVortex Effect   *
// *************************

_CLASSDEF(TSetVortexEffect)

class TSetVortexEffect : public TEffect
{
protected:
public:
    TSetVortexEffect(PTObjectImagery newim) : TEffect(newim) { }
    TSetVortexEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { }
    void Initialize();
    void Pulse();
};

DEFINE_BUILDER("SetVortex", TSetVortexEffect)
REGISTER_BUILDER(TSetVortexEffect)


void TSetVortexEffect::Initialize()
{
    SetCommandDone( FALSE);
    if(spell)
    {
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
        if(invoker)
        {
            S3DPoint temp_point;
            int temp_level;
            invoker->GetPos(temp_point);
            temp_level = MapPane.GetMapLevel();
            invoker->SetTeleportPosition(temp_point);
            invoker->SetTeleportLevel(temp_level);

            PTSetVortexEffect old = (PTSetVortexEffect)MapPane.FindObject("SetVortex",1);
            if(old)
            {
                if(old != this)
                    old->KillThisEffect();
                else
                {
                    old = (PTSetVortexEffect)MapPane.FindObject("SetVortex",2);
                    if(old)
                    {
                        if(old != this)
                            old->KillThisEffect();
                    }
                }
            }
        }       
    }
}

void TSetVortexEffect::Pulse()
{
    TEffect::Pulse();
}


// *******************************
// *   SetVortex Animator Code   *
// *******************************

_CLASSDEF(TSetVortexAnimator)
class TSetVortexAnimator : public T3DAnimator
{
protected:
    int direction;
    float current_height;
public:
    void Initialize();
    BOOL Render();
    TSetVortexAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    void RefreshZBuffer();
    void Animate(BOOL);
};

REGISTER_3DANIMATOR("SetVortex", TSetVortexAnimator)

void TSetVortexAnimator::Initialize()
{
    T3DAnimator::Initialize();
    ((PTSetVortexEffect)inst)->Initialize();

    direction = 1;
    
    S3DPoint temp_point;
    inst->GetPos(temp_point);

    pos.x = (float)temp_point.x;
    pos.y = (float)temp_point.y;
    pos.z = temp_point.z + 30.0f;
    pos.z = FIX_Z_VALUE(pos.z);
    current_height = 30.0f;
//  ((PTSetVortexEffect)inst)->Initialize();
}

void TSetVortexAnimator::Animate(BOOL)
{
    if(direction == 1)
    {
        pos.z += 0.4f;
        current_height += 0.4f;

        if(current_height > 40.0f)
            direction = -1;
    }
    else
    {
        pos.z -= 0.4f;
        current_height -= 0.4f;

        if(current_height < 20.0f)
            direction = 1;
    }
}

BOOL TSetVortexAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(0);

    obj->flags |= OBJ3D_SCL1 | OBJ3D_POS2 | OBJ3D_ABSPOS;

    obj->pos.x = pos.x;
    obj->pos.y = pos.y;
    obj->pos.z = pos.z;

    obj->scl.x = 0.5f;
    obj->scl.y = 0.5f;
    obj->scl.z = 0.5f;

    RenderObject(obj);

    RestoreBlendState();

    return TRUE;
}

void TSetVortexAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 50;
    size.y = 50;

    ((PTEffect)inst)->GetPos(effect);

    effect.z = (int)FIX_Z_VALUE(effect.z);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x + (size.x / 2), size.y + (size.y / 2));
}


// *********************
// *     Teleporter    *
// *********************

#define TELE_STATE_INIT 1
#define TELE_STATE_OUT 2
#define TELE_STATE_MOVE 3
#define TELE_STATE_IN 4

_CLASSDEF(TTeleporterEffect)

class TTeleporterEffect : public TEffect
{
private:
public:
    int level;
    int life;
    int my_state;
    BOOL has_moved;
    S3DPoint new_position;
    TTeleporterEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TTeleporterEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    void Initialize();
    void Pulse();
    void OffScreen(){}
};

DEFINE_BUILDER("Teleporter", TTeleporterEffect)
REGISTER_BUILDER(TTeleporterEffect)

_CLASSDEF(TTeleporterAnimator)
class TTeleporterAnimator : public T3DAnimator
{
  public:
    D3DVALUE rotation;
    D3DVECTOR p[5];       // Position of the flares
    int iterations,ticks;
    float cylsize[5];
    D3DVALUE growx, growy;

    TTeleporterAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    virtual ~TTeleporterAnimator() { Close(); }

    virtual void Initialize();
      // Initializes velocity vectors and positions

    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    void RefreshZBuffer();
};

REGISTER_3DANIMATOR("Teleporter", TTeleporterAnimator)



// *********************
// * TTeleporterEffect *
// *********************

void TTeleporterEffect::Initialize()
{
    my_state = TELE_STATE_INIT;
}

void TTeleporterEffect::Pulse()
{
    life++;
    switch(my_state)
    {
        case TELE_STATE_INIT:
            level = 1;
            if(spell)
            {
                if(strcmp(spell->VariantData()->name, "Teleport") == 0)
                {
                    level = 1;
                }
                if(strcmp(spell->VariantData()->name, "Teleport2") == 0)
                {
                    level = 2;
                }
                else if(strcmp(spell->VariantData()->name, "VortexM") == 0)
                {
                    level = 0;
                }
                if(spell)
                {
                    ((PTCharacter)spell->GetInvoker())->SetFade(100,10);
                }
                my_state = TELE_STATE_OUT;
                life = 0;
            }
            break;
        case TELE_STATE_OUT:
            if(life == 50)
                my_state = TELE_STATE_MOVE;
            break;
        case TELE_STATE_MOVE:
            switch(level)
            {
                case 0:
                case 1:
                    if(spell)
                    {
                        PTCharacter target;
                        S3DPoint temp_point;
                        target = NULL;
                        if(spell->GetInvoker())
                            target = (PTCharacter)((PTCharacter)spell->GetInvoker())->Fighting();
                        if(!target)
                        {
                            S3DPoint temp_point1;
                            S3DPoint temp_point2;
                            PTCharacter invoker = (PTCharacter)spell->GetInvoker();
                            if(invoker)
                            {
                                invoker->GetPos(temp_point1);
                                for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
                                {
                                    PTCharacter chr = (PTCharacter)i.Item();

                                    if (chr == invoker)
                                        continue;

                                    if (chr->IsDead())
                                        continue;

                                    chr->GetPos(temp_point2);
                                    if (::Distance(temp_point1,temp_point2) > 300)
                                        continue;

                                    if (invoker && !invoker->IsEnemy(chr))
                                        continue;   // We can't hurt our friends
                                    
                                    target = chr;
                                    break;
                                }
                            }
                        }                           
                        if(target)
                        {
                            int changex,changey;
                            int count;
                            changex = changey = 20;
                            count = 0;
                            do
                            {
                                count++;
                                target->GetPos(new_position);
                                GetPos(temp_point);
                                if(new_position.x > temp_point.x)
                                {
                                    new_position.x += changex;
                                }
                                else
                                {
                                    new_position.x -= changex;
                                }
                                if(new_position.y > temp_point.y)
                                {
                                    new_position.y += changey;
                                }
                                else
                                {
                                    new_position.y -= changey;
                                }
                                changex -= 2;
                                changey += 2;
                            }
                            while((MapPane.GetWalkHeight(new_position) > (temp_point.z + 5) || MapPane.GetWalkHeight(new_position) == 0) && count < 10);

                            if(count == 10)
                            {
                                new_position.x = temp_point.x;
                                new_position.y = temp_point.y;
                                new_position.z = temp_point.z;
                            }
                            temp_point.x = new_position.x;
                            temp_point.y = new_position.y;
                            temp_point.z = new_position.z;
                            SetPos(temp_point);
                            spell->GetInvoker()->SetPos(new_position);
                            ((PTCharacter)spell->GetInvoker())->SetFade(0,-10);
                            has_moved = FALSE;
                        }
                        else
                        {
                            int count;
                            int changex,changey;
                            S3DPoint temp_point;
                            count = 0;
                            GetPos(temp_point);
                            changex = changey = 400; 
                            do
                            {
                                GetPos(new_position);
                                new_position.x += changex;
                                new_position.y += changey;
                                changex -= 100;
                                changey -= 100;
                                count++;
                            }
                            while((MapPane.GetWalkHeight(new_position) > (temp_point.z + 5) || MapPane.GetWalkHeight(new_position) == 0) && count < 10);

                            if(count == 10)
                            {
                                new_position.x = temp_point.x;
                                new_position.y = temp_point.y;
                                new_position.z = temp_point.z;
                            }
                            temp_point.x = new_position.x;
                            temp_point.y = new_position.y;
                            temp_point.z = new_position.z;
                            SetPos(temp_point);
                            spell->GetInvoker()->SetPos(new_position);
                            ((PTCharacter)spell->GetInvoker())->SetFade(0,-10);
                            has_moved = FALSE;
                        }
                    }       
                    else
                    {
                        int count;
                        int changex,changey;
                        S3DPoint temp_point;
                        count = 0;
                        GetPos(temp_point);
                        changex = changey = 400; 
                        do
                        {
                            GetPos(new_position);
                            new_position.x += changex;
                            new_position.y += changey;
                            changex -= 100;
                            changey -= 100;
                            count++;
                        }
                        while((MapPane.GetWalkHeight(new_position) > (temp_point.z + 5) || MapPane.GetWalkHeight(new_position) == 0) && count < 10);

                        if(count == 10)
                        {
                            new_position.x = temp_point.x;
                            new_position.y = temp_point.y;
                            new_position.z = temp_point.z;
                        }
                        temp_point.x = new_position.x;
                        temp_point.y = new_position.y;
                        temp_point.z = new_position.z;
                        SetPos(temp_point);
                        spell->GetInvoker()->SetPos(new_position);
                        ((PTCharacter)spell->GetInvoker())->SetFade(0,-10);
                        has_moved = FALSE;
                    }
                    break;
                case 2:
                    if(spell)
                    {
                        S3DPoint temp_point, temp_point2, temp_point3;
                        int temp_level;
                        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
                        invoker->GetPos(temp_point2);
                        if(invoker)
                        {
                            temp_point2 = invoker->GetTeleportPosition();
                            temp_level = invoker->GetTeleportLevel();
                            temp_point3.x = temp_point3.y = temp_point3.z = -1;
                            invoker->SetTeleportPosition(temp_point3);
                            invoker->SetTeleportLevel(-1);
                            if(temp_point2.x == -1 || temp_point2.y == -1)
                            {
                                invoker->GetPos(temp_point2);
                                temp_level = MapPane.GetMapLevel();
                            }

                            temp_point.x = temp_point2.x;
                            temp_point.y = temp_point2.y;
                            temp_point.z = temp_point2.z;
                            new_position.x = temp_point2.x;
                            new_position.y = temp_point2.y;
                            new_position.z = temp_point2.z;
                            SetPos(temp_point);
                            spell->GetInvoker()->SetPos(temp_point2, temp_level);
                            ((PTCharacter)spell->GetInvoker())->SetFade(0,-10);
                            has_moved = FALSE;
                        }
                    }
                    break;
                default:
                    GetPos(new_position);
                    break;
            }
//          ((PTTeleporterAnimator)animator)->Initialize();
            my_state = TELE_STATE_IN;
            break;
        case TELE_STATE_IN:
            if(!has_moved)
            {
                PTSetVortexEffect old = (PTSetVortexEffect)MapPane.FindObject("SetVortex",1);
                if(old)
                {
                    old->KillThisEffect();
                    has_moved = TRUE;
                }
            }
            S3DPoint temp_point;
            temp_point.x = new_position.x;
            temp_point.y = new_position.y;
            temp_point.z = new_position.z;
            SetPos(temp_point);
            break;
    }
}



// ***********************
// * TTeleporterAnimator *
// ***********************

void TTeleporterAnimator::Initialize()
{
    int i;

    T3DAnimator::Initialize();

    inst->Face( -32);

    for (i = 0; i < 5; i++)
    {
        p[i].y = 20;
        p[i].z = 45;
        cylsize[i] = 0.0;
    }
    p[1].x = -5;
    p[2].x = 5;
    p[3].z += 20;
    p[4].z -= 20;

    iterations = 0;
    ticks = 0;
    growx = D3DVAL(15.0 / 50);
    growy = growx / 3;
}

void TTeleporterAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

// handle fading the character in and out
    PTSpell spell = ((PTEffect)inst)->GetSpell();
    if(spell)
    {
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
        if(invoker)
            invoker->UpdateFade();
    }

    // Rotate the cylindrical glow
    rotation += (D3DVALUE)0.1;

    if (iterations < 50)
    {
        if (iterations >= 25)
        {
            p[1].x += D3DVAL( 0.5);
            p[2].x -= D3DVAL( 0.5);
        }
        p[3].z -= D3DVAL( 1);
        p[4].z += D3DVAL( 1);

        iterations++;
    }
    ticks++;

    if(((PTTeleporterEffect)inst)->life >= 100)
        ((PTTeleporterEffect)inst)->KillThisEffect();
}

BOOL TTeleporterAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;

    // Draw the cylindrical glow around the player
    obj = GetObject(1);

    Get3DImagery()->ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2; // | OBJ3D_POS3;

    obj->rot.x = obj->rot.y = D3DVAL(0.0);

    int curticks = ticks % 100;
    if (curticks < 50)
    {
        for (int z = 0; z < 5; z++)
        {
            obj->rot.z = rotation+(float)((float)z*(float)0.5);
            obj->scl.z = (float)(10 - 5.0f*((float)curticks / 30.0f) - (float)z);
            obj->scl.x = obj->scl.y = (float)((0.5f * (float)z) + 2.5f*((float)curticks / 30.0f));
            if (obj->scl.z > .01)
                RenderObject(obj);
        }
    }
    else
    {
        for (int z = 0; z < 5; z++)
        {
            int tempticks = 50 - (curticks - 50);
            obj->rot.z = rotation+(float)((float)z*(float)0.5);
            obj->scl.z = (float)(10 - 5.0f*((float)tempticks / 30.0f) - (float)z);
            obj->scl.x = obj->scl.y = (float)((0.5f * (float)z) + 2.5f*((float)tempticks / 30.0f));
            if (obj->scl.z > .01)
                RenderObject(obj);
        }
    }
    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;
}

void TTeleporterAnimator::RefreshZBuffer()
{
    S3DPoint effect, screen, size;

    size.x = 100;
    size.y = 200;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}




// ********************
// * Barrier Animator *
// ********************

#define MAXFLARES 30

_CLASSDEF(TBarrierAnimator)
class TBarrierAnimator : public T3DAnimator
{
  private:
    enum
    {
        FL_INACTIVE,
        FL_BLUE,
        FL_CYAN
    };
    enum
    {
        M_LEFTTORIGHT,
        M_RIGHTTOLEFT,
        M_TOPTOBOTTOM,
        M_BOTTOMTOTOP
    };

    D3DVECTOR vel[MAXFLARES];   // velocity
    D3DVECTOR pos[MAXFLARES];   // position
    int type[MAXFLARES];        // type, 0 = inactive, 1 = blue flare, 2 = cyan flare
    int motion[MAXFLARES];      // motion, 0 = L->R, 1 = R->L, 2 = T->B, 3 = B->T
    int spawndelay;

    D3DVALUE ybarmid;
    D3DVECTOR upperleft;
    D3DVECTOR lowerright;

    inline int FindEmptyFlare()
        { for (int i = 0; i < MAXFLARES; i++) if (type[i] == FL_INACTIVE) return i; return -1; }

  public:
    TBarrierAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~TBarrierAnimator() { Close(); }
      // Call close function

    virtual void Initialize();
      // Initializes velocity vectors and positions
    virtual void SetupObjects();
      // Called to create S3DAnimObj's and add to object array
    virtual void Animate(BOOL draw);
      // Called to update frame state
    virtual BOOL Render();
      // Called to render a frame
    virtual void RefreshZBuffer();
};

REGISTER_MULTI_3DANIMATOR_("BarrierEW", TBarrierEW, TBarrierAnimator)                               \
REGISTER_MULTI_3DANIMATOR_("BarrierNS", TBarrierNS, TBarrierAnimator)                               \

void TBarrierAnimator::Initialize()
{
    T3DAnimator::Initialize();
}

void TBarrierAnimator::SetupObjects()
{
    upperleft.x = upperleft.y = 100000;
    lowerright.x = lowerright.y = -100000;

    upperleft.z = -100000;
    lowerright.z = 100000;

    spawndelay = 0;

    for (int i = 0; i < MAXFLARES; i++)
    {
        vel[i].x = vel[i].y = vel[i].z = 0;
        pos[i].x = pos[i].y = pos[i].z = 0;
        type[i] = FL_INACTIVE;
        motion[i] = 0;
    }

    ybarmid = 0;

    for (int c = 0; c < Get3DImagery()->NumObjects(); c++)
    {
        PS3DAnimObj o = NewObject(c);
        GetVerts(o, D3DVT_LVERTEX);
        AddObject(o);

        // if this is the barrier object, determine middle y, 
        // and upper left and lower right coordinates
        if (c == 0)
        {
            for (int v = 0; v < o->numverts; v++)
            {
                if (o->lverts[v].x <= upperleft.x && o->lverts[v].y <= upperleft.y && o->lverts[v].z >= upperleft.z)
                {
                    upperleft.x = o->lverts[v].x;
                    upperleft.y = o->lverts[v].y;
                    upperleft.z = o->lverts[v].z;
                }

                if (o->lverts[v].x >= lowerright.x && o->lverts[v].y >= lowerright.y && o->lverts[v].z <= lowerright.z)
                {
                    lowerright.x = o->lverts[v].x;
                    lowerright.y = o->lverts[v].y;
                    lowerright.z = o->lverts[v].z;
                }
                ybarmid += o->lverts[v].y;
            }

            ybarmid /= o->numverts;
        }
    }
}

void TBarrierAnimator::Animate(BOOL draw)
{
    int i;

    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    for (i = 0; i < MAXFLARES; i++)
    {
        if (type[i] != FL_INACTIVE)
        {
            pos[i].x += vel[i].x;
            pos[i].z += vel[i].z;

            switch (motion[i])
            {
                case M_LEFTTORIGHT:
                {
                    if (pos[i].x > lowerright.x)
                        type[i] = FL_INACTIVE;
                    break;
                }
                case M_RIGHTTOLEFT:
                {
                    if (pos[i].x < upperleft.x)
                        type[i] = FL_INACTIVE;
                    break;
                }
                case M_TOPTOBOTTOM:
                {
                    if (pos[i].z < lowerright.z)
                        type[i] = FL_INACTIVE;
                    break;
                }
                case M_BOTTOMTOTOP:
                {
                    if (pos[i].z > upperleft.z)
                        type[i] = FL_INACTIVE;
                    break;
                }
            }

        }
    }

    if (spawndelay)
        spawndelay--;
    else
    {
        i = FindEmptyFlare();

        if (i != -1)
        {
            vel[i].y = 0;
            pos[i].y = ybarmid;
            motion[i] = random(M_LEFTTORIGHT,M_BOTTOMTOTOP);

            switch (motion[i])
            {
                case M_LEFTTORIGHT:
                {
                    pos[i].x = upperleft.x;
                    pos[i].z = D3DVALUE(random((int)lowerright.z, (int)upperleft.z));

                    vel[i].x = D3DVALUE(random(2,10));
                    vel[i].z = 0;

                    break;
                }
                case M_RIGHTTOLEFT:
                {
                    pos[i].x = lowerright.x;
                    pos[i].z = D3DVALUE(random((int)lowerright.z, (int)upperleft.z));

                    vel[i].x = D3DVALUE(-(random(2,10)));
                    vel[i].z = 0;

                    break;
                }
                case M_TOPTOBOTTOM:
                {
                    pos[i].x = D3DVALUE(random((int)upperleft.x*100, (int)lowerright.x*100) / 100);
                    pos[i].z = upperleft.z;

                    vel[i].x = 0;
                    vel[i].z = D3DVALUE(-(random(2,10)));

                    break;
                }
                case M_BOTTOMTOTOP:
                {
                    pos[i].x = D3DVALUE(random((int)upperleft.x, (int)lowerright.x));
                    pos[i].z = lowerright.z;

                    vel[i].x = 0;
                    vel[i].z = D3DVALUE(random(2,10));

                    break;
                }
            }

            type[i] = random(FL_BLUE, FL_CYAN);
        }

        spawndelay = random(1, 5);
    }
}

BOOL TBarrierAnimator::Render()
{
    int i, v; 

    SaveBlendState();
    SetBlendState();

    PS3DAnimObj objBarrier = GetObject(0);
    PS3DAnimObj objFlare;

    ResetExtents();

    for (i = 0; i < MAXFLARES; i++)
    {
        objFlare = NULL;

        switch (type[i])
        {
            case FL_BLUE:
            {
                objFlare = GetObject(2);
                break;
            }
            case FL_CYAN:
            {
                objFlare = GetObject(4);
                break;
            }
        }

        if (objFlare)
        {
            switch (motion[i])
            {
                case M_LEFTTORIGHT:
                case M_RIGHTTOLEFT:
                {
                    objFlare->flags = OBJ3D_SCL1 | OBJ3D_POS2;
                    objFlare->pos = pos[i];
                    objFlare->scl.x = D3DVALUE(2.0);
                    objFlare->scl.y = 0;
                    objFlare->scl.z = D3DVALUE(0.5);

                    break;
                }

                case M_TOPTOBOTTOM:
                case M_BOTTOMTOTOP:
                {
                    objFlare->flags = OBJ3D_SCL1 | OBJ3D_POS2;
                    objFlare->pos = pos[i];
                    objFlare->scl.x = D3DVALUE(0.5);
                    objFlare->scl.y = 0;
                    objFlare->scl.z = D3DVALUE(2.0);

                    break;
                }
            }

            RenderObject(objFlare);
        }
    }

    // reset animation so we can mess with vertices...
    objBarrier->flags |= OBJ3D_POS1;
    objBarrier->pos.x = objBarrier->pos.y = objBarrier->pos.z = 0;

    for (v = 0; v < objBarrier->numverts; v++)
    {
        objBarrier->lverts[v].tu += 0.01f;
        objBarrier->lverts[v].tv += 0.01f;
    }

    RenderObject(objBarrier);

    UpdateExtents();

    RestoreBlendState();

    return TRUE;
}

void TBarrierAnimator::RefreshZBuffer()
{
    S3DPoint effect, size, screen;

    ((PTEffect)inst)->GetPos(effect);

    size.x = 150;
    size.y = 225;

    WorldToScreen(effect, screen);
    RestoreZ(screen.x - 20, screen.y - (size.y / 2) - (size.y / 4), size.x, size.y);
}

// **********************
// * TSmokeEffectAnimator *
// **********************


_CLASSDEF(TSmokeEffectAnimator)
class TSmokeEffectAnimator : public T3DAnimator
{
  public:
    int ticks;
    float centerx, centery;
    float centervx, centervy;
    SSmoke smoke[NUMSMOKEBALLS];

    TSmokeEffectAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    virtual ~TSmokeEffectAnimator() { Close(); }

    virtual void Initialize();
      // Initializes velocity vectors and positions

    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
    void ResetBall(int b);
};

void TSmokeEffectAnimator::ResetBall(int b)
{
    SSmoke *s = &smoke[b];
    s->x = ((float)rand() / (float)RAND_MAX) * 50.0f - 25.0f + centerx;
    s->y = ((float)rand() / (float)RAND_MAX) * 50.0f - 25.0f + centery;
    s->z = 0;
    s->rot = 0;
    s->vx = (((float)rand() / (float)RAND_MAX) * 1.0f - 0.5f);
    s->vy = (((float)rand() / (float)RAND_MAX) * 1.0f - 0.5f);
    s->vz = (((float)rand() / (float)RAND_MAX) * 1.0f);
    s->life = (int)(((float)rand() / (float)RAND_MAX) * 100.0f);
    s->size = ((float)rand() / (float)RAND_MAX) * 2.7f + 0.1f;
}

REGISTER_3DANIMATOR("Smoke", TSmokeEffectAnimator)

void TSmokeEffectAnimator::Initialize()
{
    int i;
    centerx = centery = 0.0f;
    centervx = centervy = 0.2f;

    T3DAnimator::Initialize();

    for (i = 0; i < NUMSMOKEBALLS; i++)
        ResetBall(i);

//  inst->Face( -32);

    ticks = 0;
}

void TSmokeEffectAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    centerx += centervx;
    centery += centervy;

    if (centerx > 50 || centerx < -50)
        centervx = -centervx;
    if (centery > 50 || centery < -50)
        centervy = -centervy;
    
    SSmoke *s = smoke;  
    for (int i = 0; i < NUMSMOKEBALLS; i++, s++)
    {
        s->vz += SMOKE_GRAV;
        s->x += s->vx;
        s->y += s->vy;
        if (s->z > 0)
            s->z += s->vz;
        else
            s->z -= 0.008f;

        s->life++;
        s->size += 0.02f;

        if (s->life > 200)
        {
            s->size -= 0.2f;
            if (s->size <= 0.01f)
                ResetBall(i);
        }
    }

    ticks++;
}

BOOL TSmokeEffectAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;

    // Draw the cylindrical glow around the player
    obj = GetObject(0);

    ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_SCL1 | OBJ3D_POS2;

    for (int i = 0; i < NUMSMOKEBALLS; i++)
    {
        SSmoke s = smoke[i];
        obj->scl.x = obj->scl.y = s.size;
        obj->scl.z = s.size;
        obj->pos.x = s.x;
        obj->pos.y = s.y;
        obj->pos.z = s.z;

        RenderObject(obj);
    }

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;

}

void TSmokeEffectAnimator::RefreshZBuffer()
{
    S3DPoint map, effect, screen, size;

    size.x = 450;
    size.y = 300;

    ((PTEffect)inst)->GetPos(effect);

    map.x = effect.x;
    map.y = effect.y;
    map.z = effect.z;

    WorldToScreen(map, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

// ****************
// * Meteor Storm *
// ****************
DEFINE_BUILDER("MeteorStorm", TMeteorStormEffect)
REGISTER_BUILDER(TMeteorStormEffect)

void TMeteorStormEffect::Initialize()
{
}

void TMeteorStormEffect::Pulse()
{
    TEffect::Pulse();
}

REGISTER_3DANIMATOR("MeteorStorm", TMeteorStormAnimator)

void TMeteorStormAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PS3DAnimObj o = GetObject(0);
    meteor_storm.Init(this, o);

    SStormParams params;

    ticks = 0;
    tracker = 0;

    S3DPoint effect_pos;
    ((PTEffect)inst)->GetPos(effect_pos);
//  if(!((PTEffect)inst)->GetSpell()->GetTarget())
//  {
//      float angle = ((float)((PTEffect)inst)->GetAngle() / 255.0f) * 360.0f * (float)TORADIAN;
//
//      float x = 100.0f * (float)cos(angle);
//      float y = 100.0f * (float)sin(angle);
//
//      effect_pos.x += (int)x;
//      effect_pos.y += (int)y;
//  }

    // how many meteors
    params.particles = 0;
    // their texture size
    params.tex_u = params.tex_v = 64;
    // what is the meteor grid
    params.particle_u = 8;
    params.particle_v = 2;
    // where do the frames begin and end
    params.particle_begin = 0;
    params.particle_end = 7;
    // what is the impact grid
    params.impact_u = 4;
    params.impact_v = 4;
    // where do the frames begin and end
    params.impact_begin = 8;
    params.impact_end = 15;
    // gravity, duh...take physics
    params.gravity = .37f;
    // velocity, see above suggestion
    params.velocity.y = 0.0f;
    params.velocity.x = -10.0f;
    params.velocity.z = -15.0f;
    // base position value
    params.pos.x = (float)effect_pos.x + 120.0f;
    params.pos.y = (float)effect_pos.y - 20.0f;
    params.pos.z = (float)effect_pos.z + 300.0f;
    // the spread
    params.pos_spread.x = 100.0f;
    params.pos_spread.y = 100.0f;
    params.pos_spread.z = 0.0f;
    // frame incrementors
    params.impact_frame_inc = 0.7f;
    params.particle_frame_inc = 0.5f;
    float ratio = (float)random(5,20) / 10.0f;
    // scaling
    params.impact_scale.x = 1.0f * ratio;
    params.impact_scale.y = 1.0f * ratio;
    params.impact_scale.z = 1.0f * ratio;
    // scaling
    params.particle_scale.x = 0.5f * ratio;
    params.particle_scale.y = 1.5f * ratio;
    params.particle_scale.z = 1.0f * ratio;

    params.rot.x = 0.0f;
    params.rot.y = 0.0f;
    params.rot.z = 0.0f;

    meteor_storm.Set(&params);

    // init the rotation
    rot.x = 0.0f;
    rot.y = 0.0f;
    rot.z = 0.0f;
}

void TMeteorStormAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    if(!(ticks%10))
    {
        if(tracker<METEOR_STORM_SIZE&&ticks<=METEOR_STORM_TICKS)
            tracker++;

        if(ticks>METEOR_STORM_TICKS&&tracker!=0)
            tracker--;

        SStormParams params;
        meteor_storm.Get(&params);
        params.particles = tracker;
        meteor_storm.Set(&params);
    }

/*  if(!(ticks%10))
    {
        SStormParams params;
        meteor_storm.Get(&params);
        params.particles = 1;
        meteor_storm.Set(&params);
    }*/


    // animate the storm!
    meteor_storm.Animate();

    // check to see if finished
    if(meteor_storm.IsDone() && ticks >= METEOR_STORM_TICKS)
        ((PTEffect)inst)->KillThisEffect();

    ++ticks;
}

BOOL TMeteorStormAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    // render the storm!
    
    meteor_storm.Render();

    RestoreBlendState();

    return TRUE;
}

void TMeteorStormAnimator::RefreshZBuffer()
{
    // refresh the storm!
    meteor_storm.RefreshZBuffer();
}





// FLIES, FLIES, FLIES!

DEFINE_BUILDER("Flies", TFlyEffect)
REGISTER_BUILDER(TFlyEffect)

void TFlyEffect::Initialize()
{
}

void TFlyEffect::Pulse()
{
    TEffect::Pulse();
}

REGISTER_3DANIMATOR("Flies", TFlyAnimator)

void TFlyAnimator::Initialize()
{
    // create all the flies
    T3DAnimator::Initialize();

    PS3DAnimObj o = GetObject(0);
    S3DPoint size;
    size.x = 10;
    size.y = 10;
    flies.Init(this, o, size);

    for(int i = 0; i < FLY_COUNT; ++i)
    {
        PSParticleSystemInfo fly = flies.Get(i);
        fly->pos.x = (float)random(-FLY_RANGE_X, FLY_RANGE_X);
        fly->pos.y = (float)random(-FLY_RANGE_Y, FLY_RANGE_Y);
        fly->pos.z = (float)random(-FLY_RANGE_Z, FLY_RANGE_Z);
        fly->used = FALSE;
        fly->scl.x = .3f;
        fly->scl.y = .3f;
        fly->scl.z = .3f;
        fly->rot.x = 0.0f;
        fly->rot.y = 0.0f;
        fly->rot.z = 0.0f;
    }
}

void TFlyAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    // restrict their flying range
    int chance = random(0, 1);

    for(int i = 0; i < FLY_COUNT; ++i)
    {
        PSParticleSystemInfo fly = flies.Get(i);

        if(fly->used == FALSE && chance)
        {
            fly->used = TRUE;
            chance = 0;
        }

        D3DVECTOR velocity;
        velocity.x = (float)random(-2, 2);
        velocity.y = (float)random(-2, 2);
        velocity.z = (float)random(-2, 2);

        fly->pos.x += velocity.x;
        fly->pos.y += velocity.y;
        fly->pos.z += velocity.z;

        if(fly->pos.x > (float)FLY_RANGE_X || fly->pos.x < (float)-FLY_RANGE_X)
            fly->pos.x -= velocity.x + velocity.x;
        if(fly->pos.y > (float)FLY_RANGE_Y || fly->pos.y < (float)-FLY_RANGE_Y)
            fly->pos.y -= velocity.y + velocity.y;
        if(fly->pos.z > (float)FLY_RANGE_Z || fly->pos.z < (float)-FLY_RANGE_Z)
            fly->pos.z -= velocity.z + velocity.z;
    }
}

BOOL TFlyAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    DWORD oldcullmode;
    Scene3D.GetRenderState(D3DRENDERSTATE_CULLMODE, &oldcullmode);
    Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

    flies.Render();

    Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullmode);

    RestoreBlendState();

    return TRUE;
}

void TFlyAnimator::RefreshZBuffer()
{
    S3DPoint size, map, screen;

    ((PTEffect)inst)->GetPos(map);

    WorldToScreen(map, screen);

    size.x = FLY_RANGE_X * 3;
    size.y = FLY_RANGE_Y * 3;

    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

REGISTER_3DANIMATOR("Fog", TFogAnimator)

void TFogAnimator::Initialize()
{
    T3DAnimator::Initialize();

    PS3DAnimObj obj = GetObject(0);
    GetVerts(obj, D3DVT_LVERTEX);

    for(int i = 0; i < FOG_VERTEX; ++i)
    {
        if(i == 0 || i == 4 || i == 6 || i == 8 || i == 10 || i == 3 || i == 13 || i == 19 || i == 25 ||
            i == 31 || i == 30 || i == 32 || i == 33 || i == 34 || i == 35 || i == 29 || i == 23 ||
            i == 17 || i == 11 || i == 1)
        {
            color[i].c = .7f;
            color[i].a = .125f;
            dpos[i].x = 0.0f;
            dpos[i].y = 0.0f;
            dpos[i].z = 0.0f;
        }
        else
        {
            color[i].c = (float)random(400, 1000) / 1000.0f;
            color[i].a = (float)random(100, 400) / 1000.0f;
            dpos[i].x = (float)random(-50, 50) / 100.0f;
            dpos[i].y = (float)random(-50, 50) / 100.0f;
            dpos[i].z = (float)random(-50, 50) / 100.0f;
        }
        pos[i].x = obj->lverts[i].x;
        pos[i].y = obj->lverts[i].y;
        pos[i].z = obj->lverts[i].z;
        velocity[i].x = 0.0f;       
        velocity[i].y = 0.0f;
        velocity[i].z = 0.0f;
        color_velocity[i] = 0.0f;       
        alpha_velocity[i] = 0.0f;       
    }
}

void TFogAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    
    for(int i = 0; i < FOG_VERTEX; ++i)
    {
        if(i == 0 || i == 4 || i == 6 || i == 8 || i == 10 || i == 3 || i == 13 || i == 19 || i == 25 ||
            i == 31 || i == 30 || i == 32 || i == 33 || i == 34 || i == 35 || i == 29 || i == 23 ||
            i == 17 || i == 11 || i == 1)
            continue;
        color_velocity[i] += (float)random(-10, 10) / 1000.0f;
        alpha_velocity[i] += (float)random(-10, 10) / 1000.0f;

        color[i].c += color_velocity[i];
        if(color_velocity[i] > 1.0f || color_velocity[i] < .4f)
            color_velocity[i] = 0.0f;
        color[i].c = min(1.0f, max(.4f, color[i].c));
        color[i].a += alpha_velocity[i];
        if(alpha_velocity[i] > .4f || alpha_velocity[i] < .1f)
            alpha_velocity[i] = 0.0f;
        color[i].a = min(.4f, max(.1f, color[i].a));

        velocity[i].x += (float)random(-10, 10) / 1000.0f;
        velocity[i].y += (float)random(-10, 10) / 1000.0f;
        velocity[i].z += (float)random(-10, 10) / 1000.0f;

        dpos[i].x += velocity[i].x;
        if(dpos[i].x < -.5f || dpos[i].x > .5f)
            velocity[i].x = 0.0f;
        dpos[i].x = min(.5f, max(-.5f, dpos[i].x));
        dpos[i].y += velocity[i].y;
        if(dpos[i].y < -.5f || dpos[i].y > .5f)
            velocity[i].y = 0.0f;
        dpos[i].y = min(.5f, max(-.5f, dpos[i].y));
        dpos[i].z += velocity[i].z;
        if(dpos[i].z < -.5f || dpos[i].z > .5f)
            velocity[i].z = 0.0f;
        dpos[i].z = min(.5f, max(-.5f, dpos[i].z));
    }
}

BOOL TFogAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(0);

    ResetExtents();
    obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3 | OBJ3D_VERTS;

    for(int i = 0; i < FOG_VERTEX; ++i)
    {
        obj->lverts[i].color = D3DRGBA(color[i].c, color[i].c, color[i].c, color[i].a);
        obj->lverts[i].x = pos[i].x + dpos[i].x;
        obj->lverts[i].y = pos[i].y + dpos[i].y;
        obj->lverts[i].z = pos[i].z + dpos[i].z;
    }

    obj->rot.x = 0.0f;
    obj->rot.y = 0.0f;
    obj->rot.z = 0.0f;

    obj->scl.x = 7.475f;
    obj->scl.y = 7.475f;
    obj->scl.z = 1.0f;

    obj->pos.x = 0.0f;
    obj->pos.y = 0.0f;
    obj->pos.z = 0.0f;

    RenderObject(obj);
    UpdateExtents();

    RestoreBlendState();

    return TRUE;
}

void TFogAnimator::RefreshZBuffer()
{
    S3DPoint size, effect, screen;

    size.x = 225;
    size.y = 125;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

// *******************
// * PULP CHARACTERS *
// *******************

REGISTER_3DANIMATOR("PULP", TPulpAnimator)

void TPulpAnimator::Initialize()
{
    T3DAnimator::Initialize();
}

void TPulpAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    
    // catch notified deletion and not initialized pulp stuff
    if(((PTPulpEffect)inst)->IsDone() || ((PTPulpEffect)inst)->GetCharacter() == NULL)
        return;

    PTCharAnimator charanim = (PTCharAnimator)((PTPulpEffect)inst)->GetCharacter()->GetAnimator();

    // *** if(not notified) ***
    inst->SetCommandDone(FALSE);

    // do blood stuff!
    int i = 0;
    PSBloodDrop blood;
    while(blood = ((PTPulpEffect)inst)->GetBlood(i))
    {
        ++i;
        if(!blood->used)
            continue;

        blood->pos.x += blood->vel.x;
        blood->pos.y += blood->vel.y;
        blood->pos.z += blood->vel.z;

        blood->vel.x *= .75f;
        blood->vel.y *= .75f;

        S3DPoint map;
        map.x = (int)blood->pos.x;
        map.y = (int)blood->pos.y;
        map.z = (int)blood->pos.z;

        int height = MapPane.GetWalkHeight(map);
        if(blood->pos.z <= height)
            blood->used = FALSE;

        blood->vel.z -= (PULP_GRAVITY * .75f);
    }

    // animate stuff

    // animate body parts
    PSBodyPartPulp body_part;
    i = 0;
    while(body_part = ((PTPulpEffect)inst)->GetBodyPart(i))
    {
        if(body_part->done)
        {
            ++i;
            continue;
        }

        // rotate the x axis
        body_part->rot.x += body_part->rot_vel.x;
        while(body_part->rot.x > (float)(float)M_2PI)
            body_part->rot.x -= (float)M_2PI;
        while(body_part->rot.x < 0)
            body_part->rot.x += (float)M_2PI;

        // rotate the y axis
        body_part->rot.y += body_part->rot_vel.y;
        while(body_part->rot.y > (float)M_2PI)
            body_part->rot.y -= (float)M_2PI;
        while(body_part->rot.y < 0)
            body_part->rot.y += (float)M_2PI;

        // rotate the z axis
        body_part->rot.z += body_part->rot_vel.z;
        while(body_part->rot.z > (float)M_2PI)
            body_part->rot.z -= (float)M_2PI;
        while(body_part->rot.z < 0)
            body_part->rot.z += (float)M_2PI;

        // get the walk map height above him
        S3DPoint test_point, point;
        int height;
        point.x = (int)body_part->pos.x;
        point.y = (int)body_part->pos.y;
        point.z = (int)body_part->pos.z;

        // check for x axis collision
        test_point = point;
        test_point.x += (int)body_part->vel.x;
        height = MapPane.GetWalkHeight(test_point);
        if(test_point.z <= height || !height)
        {
            body_part->vel.x = -body_part->vel.x * PULP_SLOW_RATE;
            body_part->vel.y *= PULP_SIDE_SLOW;
            body_part->vel.z *= PULP_SIDE_SLOW;
            body_part->rot_vel.x *= PULP_ANG_SLOW_RATE;
            body_part->rot_vel.y *= PULP_ANG_SLOW_RATE;
            body_part->rot_vel.z *= PULP_ANG_SLOW_RATE;
        }

        // check for y axis collision
        test_point = point;
        test_point.y += (int)body_part->vel.y;
        height = MapPane.GetWalkHeight(test_point);
        if(test_point.z <= height || !height)
        {
            body_part->vel.y = -body_part->vel.y * PULP_SLOW_RATE;
            body_part->vel.x *= PULP_SIDE_SLOW;
            body_part->vel.z *= PULP_SIDE_SLOW;
            body_part->rot_vel.x *= PULP_ANG_SLOW_RATE;
            body_part->rot_vel.y *= PULP_ANG_SLOW_RATE;
            body_part->rot_vel.z *= PULP_ANG_SLOW_RATE;
        }

        // check for z axis collision
        test_point = point;
        test_point.z += (int)body_part->vel.z;
        height = MapPane.GetWalkHeight(test_point);
        if(test_point.z <= height || !height)
        {
            int n;
            char buf[128];

            if(abs(body_part->vel.z) < (PULP_GRAVITY + PULP_CUTOFF) * 2.0f)
            {
                body_part->done = TRUE;
                body_part->vel.x = 0.0f;
                body_part->vel.y = 0.0f;
                body_part->vel.z = 0.0f;
                body_part->pos.z = (float)height;
                body_part->rot_vel.x = 0.0f;
                body_part->rot_vel.y = 0.0f;
                body_part->rot_vel.z = 0.0f;
                n = random(1, 3);
                sprintf(buf, "Blood%d", n);
            }
            else
            {
                body_part->vel.z = -body_part->vel.z * PULP_SLOW_RATE * .75f;
                body_part->vel.x *= PULP_SIDE_SLOW;
                body_part->vel.y *= PULP_SIDE_SLOW;
                body_part->rot_vel.x *= PULP_ANG_SLOW_RATE;
                body_part->rot_vel.y *= PULP_ANG_SLOW_RATE;
                body_part->rot_vel.z *= PULP_ANG_SLOW_RATE;
                n = random(1, 4);
                sprintf(buf, "BloodS%d", n);
            }

            S3DPoint charpos;
            ((PTPulpEffect)inst)->GetCharacter()->GetPos(charpos);

            if(body_part->pos.z < charpos.z + 20)
            {
                SObjectDef def;
                S3DPoint pos;

                memset(&def, 0, sizeof(SObjectDef));
                def.level = MapPane.GetMapLevel();
                pos.z = height + 1;
                pos.x = (int)body_part->pos.x + 5;
                pos.y = (int)body_part->pos.y - 25;
                def.pos = pos;
                def.objclass = OBJCLASS_TILE;
                def.objtype = TileClass.FindObjType(buf);
                MapPane.NewObject(&def);
            }
        }

        body_part->pos.x += body_part->vel.x;
        body_part->pos.y += body_part->vel.y;
        body_part->pos.z += body_part->vel.z;

        if(abs(body_part->vel.x) < PULP_CUTOFF)
            body_part->vel.x = 0.0f;
        if(abs(body_part->vel.y) < PULP_CUTOFF)
            body_part->vel.y = 0.0f;
        // gravity
        if(!body_part->done)
            body_part->vel.z -= PULP_GRAVITY;

        // make blood drops
        for(int j = 0; j < 2; ++j)
        {
            int x = 0;
            while(blood = ((PTPulpEffect)inst)->GetBlood(x))
            {
                ++x;
                if(blood->used)
                    continue;
                blood->used = TRUE;

                blood->scl.x = blood->scl.y = blood->scl.z = (float)random(30, 60) * .001f;

                blood->pos = body_part->pos;

                blood->vel = body_part->vel;
                blood->vel.x *= (float)random(60, 140) * .01f;
                blood->vel.y *= (float)random(60, 140) * .01f;
                blood->vel.z *= (float)random(60, 140) * .01f;

                break;
            }
        }

        ++i;
    }

    // animate blood
    
    // animate blood splotches
}

BOOL TPulpAnimator::Render()
{
    // catch not initialized pulp stuff
    if(((PTPulpEffect)inst)->IsDone())
        return FALSE;

    PS3DAnimObj obj;
    int i;

    PSBodyPartPulp body_part;
    S3DPoint pos;

    // cyle through the parts
    i = 0;
    while(body_part = ((PTPulpEffect)inst)->GetBodyPart(i))
    {
        ResetExtents();

        obj = GetObject(body_part->object);

        obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3 | OBJ3D_ABSPOS;

        obj->pos = body_part->pos;
        obj->rot = body_part->rot;
        obj->scl = body_part->scl;

      // Reset lights   
        Scene3D.LightAffectObject((int)body_part->pos.x, (int)body_part->pos.y, (int)body_part->pos.z); // Add 50 to get middle of guy
        RenderObject(obj);
        Scene3D.ResetAllLights();

        UpdateExtents();
        ++i;
    }

    // draw all the blood drops
    obj = GetObject(1);
    i = 0;
    PSBloodDrop blood;
    while(blood = ((PTPulpEffect)inst)->GetBlood(i))
    {
        ++i;
        if(!blood->used)
            continue;

        ResetExtents();

        obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3 | OBJ3D_ABSPOS;

        obj->pos = blood->pos;

        obj->scl = blood->scl;

        obj->rot.x = -30.0f * (float)TORADIAN;
        obj->rot.y = 0.0f * (float)TORADIAN;
        obj->rot.z = -45.0f * (float)TORADIAN;

        RenderObject(obj);

        UpdateExtents();
    }

    return TRUE;
}

void TPulpAnimator::RefreshZBuffer()
{
    //  refresh stuff
}

// **********************
// * TPulpEffect Object *
// **********************

DEFINE_BUILDER("PULP", TPulpEffect)
REGISTER_BUILDER(TPulpEffect)

void TPulpEffect::Set(S3DPoint vel, PTCharacter ch, int num_body_part, int num_blood, int num_splat)
{
    // create all the info
    character = ch;
    PTCharAnimator charanim = (PTCharAnimator)character->GetAnimator();
    if(!charanim)
        return;
    int maxnum = charanim->NumObjects();
    if (maxnum < num_body_part)
        num_body_part = maxnum;

    body_part = new SBodyPartPulp[num_body_part];
    body_part_count = num_body_part;
    blood_splat = new SBloodSplat[num_splat];
    blood_splat_count = num_splat;
    blood = new SBloodDrop[num_blood];
    blood_count = num_blood;

    // give all the body parts unique object numbers
    int j;
    for(int i = 0; i < num_body_part; ++i)
    {
        BOOL flag;
        do
        {
            flag = FALSE;
            body_part[i].obj_num = random(0, maxnum - 1);
            for(j = 0; j < i; ++j)
            {
                if(body_part[j].obj_num == body_part[i].obj_num)
                {
                    flag = TRUE;
                    break;
                }
            }
        }
        while(flag);
    }

    // modify the velocity
    float vel_avg = (vel.x + vel.y + vel.z) / 3.0f;
    float overall = (abs(vel.x) + abs(vel.y) + abs(vel.z)) / 6.0f;

    // get the cool stuff
    // cycle through all he needed body parts
    PS3DAnimObj obj;
    for(i = 0; i < num_body_part; ++i)
    {
        // get the object
        obj = charanim->GetObject(body_part[i].obj_num);
        obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3;
        // velocity
        body_part[i].vel.x = vel.x + (float)random((int)-overall, (int)overall);
        body_part[i].vel.y = vel.y + (float)random((int)-overall, (int)overall);
        body_part[i].vel.z = vel.z + (float)random((int)-overall, (int)overall);
        // angular velocity
        body_part[i].rot_vel.x = (float)random(-30, 30) * (float)TORADIAN;
        body_part[i].rot_vel.y = (float)random(-30, 30) * (float)TORADIAN;
        body_part[i].rot_vel.z = (float)random(-30, 30) * (float)TORADIAN;

        S3DPoint char_pos;
        character->GetPos(char_pos);
        body_part[i].pos.x = (float)random(-15, 15) + (int)char_pos.x;
        body_part[i].pos.y = (float)random(-15, 15) + (int)char_pos.y;
        body_part[i].pos.z = (float)random(25, 75) + (int)char_pos.z;
        S3DPoint test_pos;
        test_pos.x = (int)body_part[i].pos.x;
        test_pos.y = (int)body_part[i].pos.x;
        test_pos.z = (int)body_part[i].pos.x;
        int height = MapPane.GetWalkHeight(test_pos);
        if(test_pos.z <= height)
            body_part[i].pos.z = (float)height + 1.0f;

        body_part[i].rot = obj->rot;
        /*
        body_part[i].scl.x = (float)random(5, 600) * .001f;
        body_part[i].scl.y = (float)random(5, 600) * .001f;
        body_part[i].scl.z = (float)random(5, 600) * .001f;
        */
        body_part[i].scl.x = .4f * (float)random(75, 125) * .01f;
        body_part[i].scl.y = .4f * (float)random(75, 125) * .01f;
        body_part[i].scl.z = .4f * (float)random(75, 125) * .01f;
        body_part[i].done = FALSE;
        body_part[i].object = random(2, 10);
    }

    character->SetFlags(OF_INVISIBLE);

    for(i = 0; i < num_blood; ++i)
        blood[i].used = FALSE;

    for(i = 0; i < num_blood / 2; ++i)
    {
        blood[i].used = TRUE;
        blood->scl.x = blood->scl.y = blood->scl.z = (float)random(1, 25) * .001f;

        blood->pos = body_part->pos;
 
        blood->vel.x = ((float)vel.x + vel_avg * 2.0f) / 3.0f * ((float)random(60, 140) * .01f);
        blood->vel.y = ((float)vel.y + vel_avg * 2.0f) / 3.0f * ((float)random(60, 140) * .01f);
        blood->vel.z = ((float)vel.z + vel_avg * 2.0f) / 3.0f * ((float)random(60, 140) * .01f);
    }

    // init the splats
    for(i = 0; i < num_splat; ++i)
        blood_splat[i].used = FALSE;

    valid = TRUE;
}

void TPulpEffect::Initialize()
{
    // init all the vars
    character = NULL; 
    blood = NULL; 
    blood_splat = NULL; 
    body_part = NULL; 
    valid = FALSE; 
    SetNotify(N_DELETING);          // Check for deleted objs in action blocks
}

void TPulpEffect::Pulse()
{
    TEffect::Pulse();
}

void TPulpEffect::Notify(int notify, void *ptr)
{
    // **** WARNING!!! MAKE SURE YOU CHECK FOR BROKEN LINKS AND DELETED OBJECTS HERE!!! ****
    // If you want to be notified, you must call SetNotify() in your contsructor

    if (sector == (PTSector)ptr)
        return;

    TObjectInstance::Notify(notify, ptr);

    if (character && NOTIFY_DELETED(ptr, character))
    {
        SetFlags(OF_KILL);
        character = NULL;
    }
}

// ***************************
// *** THE DREAD FIRE CONE ***
// ***************************

// effect
_CLASSDEF(TFireConeEffect)
class TFireConeEffect : public TEffect
{
  public:
    BOOL firsttime;
    TFireConeEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TFireConeEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TFireConeEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

#define FLAME_COUNT         80
#define FLAME_BURST         100
#define FLAME_SMOKE         (FLAME_COUNT / 2)
#define FLAME_VEL           10
#define FLAME_ANG           15
#define FLAME_MIN_H         83
#define FLAME_MAX_H         87
#define FLAME_MIN_V         20
#define FLAME_MAX_V         30
#define FLAME_FRAME_COUNT   17
#define FLAME_CREATE        4
#define FLAME_MIN_LIFE      10
#define FLAME_MAX_LIFE      45
#define FLAME_STATE_START   1
#define FLAME_STATE_MID     2
#define FLAME_STATE_BLAST   3
#define FLAME_SPEED         1800

// animator for the fire cone class
_CLASSDEF(TFireConeAnimator)
class TFireConeAnimator : public T3DAnimator
{
  private:
    TParticleSystem smoke;
    TParticleSystem burst;
    BOOL done;
    int frame_count;
    int state;
  public:
    TParticleSystem fire;
    TFireConeAnimator(PTObjectInstance oi) : T3DAnimator(oi), fire(FLAME_COUNT), smoke(FLAME_COUNT), burst(FLAME_BURST) {}
    virtual ~TFireConeAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};


// **************************
// * TFireConeEffect Object *
// **************************

DEFINE_BUILDER("FIRECONE", TFireConeEffect)
REGISTER_BUILDER(TFireConeEffect)

// initialize the fire cone effect
void TFireConeEffect::Initialize()
{
    firsttime = TRUE;
}

// pulse through the fire cone
void TFireConeEffect::Pulse()
{
    TEffect::Pulse();
    if(firsttime)
    {
        if(spell)
        {
            PTCharacter invoker = (PTCharacter)spell->GetInvoker();
            if(invoker)
            {
                S3DPoint temp_point1, temp_point2;
                PSParticleSystemInfo flame;
                float ang = (float)((6.283f * (float)invoker->GetFace()) / 256);
                invoker->GetPos(temp_point1);
                flame = ((PTFireConeAnimator)animator)->fire.Get(0);
                temp_point1.x += (int)((flame->pos.x * cos(ang)) - (flame->pos.y * sin(ang)));
                temp_point1.y += (int)((flame->pos.x * sin(ang)) + (flame->pos.y * cos(ang)));
                temp_point1.z = (int)(flame->pos.z);
                for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
                {
                    PTCharacter chr = (PTCharacter)i.Item();

                    if (chr == invoker)
                        continue;

                    if (chr->IsDead())
                        continue;

                    chr->GetPos(temp_point2);
                    if (::Distance(temp_point1,temp_point2) > 16)
                        continue;

                    if (invoker && !invoker->IsEnemy(chr))
                        continue;   // We can't hurt our friends
                    
                    spell->Damage(chr);
                    chr->Burn();

                    firsttime = FALSE;
                    break;
                }
            }
        }
    }

}

// ****************************
// * TFireConeAnimator Object *
// ****************************

REGISTER_3DANIMATOR("FIRECONE", TFireConeAnimator)

// initialize the fire particles
void TFireConeAnimator::Initialize()
{
    T3DAnimator::Initialize();

    S3DPoint size;
    size.x = 10;
    size.y = 10;


    S3DPoint temp_point;

    inst->GetPos(temp_point);
    temp_point.z += 100;
//  SetPos(temp_point);
    inst->SetPos(temp_point);

    //fire.Init(this, obj, size, -((float)(inst->GetFace() * 360.0f) / 256.0f) * (float)TORADIAN);

    float facing = -(((float)inst->GetFace() * 360.0f) / 256.0f) * (float)TORADIAN;

    PS3DAnimObj obj = GetObject(0);
    fire.Init(this, obj, size, TRUE, facing);
    burst.Init(this, obj, size, TRUE, facing);

    obj = GetObject(1);
    smoke.Init(this, obj, size, TRUE, facing);

    done = FALSE;
    frame_count = 0;
    state = FLAME_STATE_START;

/*  PTSpell spell = ((PTEffect)inst)->GetSpell();
    if(spell)
    {
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
        if(invoker)
        {
            PTCharacter target = (PTCharacter)invoker->Fighting();
            if(target)
            {
                target->Burn();
            }
        }
    }
*/
}

// animate the fire particles
void TFireConeAnimator::Animate(BOOL draw)
{
    // animate stuff
    T3DAnimator::Animate(draw);

    inst->SetCommandDone(FALSE);

    PSParticleSystemInfo flame;
    SParticleSystemInfo new_flame;
    int flame_count = 0;

    smoke.Animate();

    for(int i = 0; i < FLAME_COUNT; ++i)
    {
        flame = fire.Get(i);
        if(!flame->used)        
            continue;
        if(flame->life >= flame->life_span)
        {
            new_flame.pos = flame->pos;
            new_flame.temp = flame->temp;
            new_flame.vel.x = 0.0f;
            new_flame.vel.y = 0.0f;
            new_flame.vel.z = flame->vel.z;
            new_flame.acc.x = 0.0f;
            new_flame.acc.y = 0.0f;
            new_flame.acc.z = flame->acc.z;
            new_flame.scl = flame->scl;
            new_flame.rot = flame->rot;
            new_flame.life_span = 15;
            smoke.Add(&new_flame);
        }
    }

    fire.Animate();
    burst.Animate();

    if(state == FLAME_STATE_START)
    {
        if(frame_count >= 5)
        {
            state = FLAME_STATE_MID;
            frame_count = 0;
        }
        else
            ++frame_count;
        for(i = 0; i < FLAME_CREATE / 2 && !done; ++i)
        {
            // create a new flame structure
            new_flame.pos.x = (float)random(-4, 4) + 20.0f;
            new_flame.pos.y = (float)random(-4, 4);
            new_flame.pos.z = (float)random(-4, 4) - 7.0f;

            new_flame.rot.x = -90.0f;
            new_flame.rot.z = -45.0f;
            new_flame.rot.y = 0.0f;

            float scale = (float)random(5, 25) * .01f;

            new_flame.temp.x = scale;
            new_flame.temp.y = scale;
            new_flame.temp.z = scale;

            new_flame.vel.x = (float)random(-100, 100) * .01f;
            new_flame.vel.y = (float)random(-100, 100) * .01f;
            new_flame.vel.z = (float)random(100, 400) * .01f;

            new_flame.acc.x = (float)random(FLAME_MIN_H, FLAME_MAX_H) * .01f;
            new_flame.acc.y = (float)random(FLAME_MIN_H, FLAME_MAX_H) * .01f;
            new_flame.acc.z = (float)random(FLAME_MIN_V, FLAME_MAX_V) * .01f;

            new_flame.life_span = random(3, 15);

            new_flame.flicker = FALSE;

            fire.Add(&new_flame);
        }
        for(i = 0; i < FLAME_CREATE / 2 && !done; ++i)
        {
            // create a new flame structure
            new_flame.pos.x = (float)random(-4, 4) - 20.0f;
            new_flame.pos.y = (float)random(-4, 4);
            new_flame.pos.z = (float)random(-4, 4) - 7.0f;

            new_flame.rot.x = -90.0f;
            new_flame.rot.z = -45.0f;
            new_flame.rot.y = 0.0f;

            float scale = (float)random(5, 25) * .01f;

            new_flame.temp.x = scale;
            new_flame.temp.y = scale;
            new_flame.temp.z = scale;

            new_flame.vel.x = (float)random(-100, 100) * .01f;
            new_flame.vel.y = (float)random(-100, 100) * .01f;
            new_flame.vel.z = (float)random(0, 300) * .01f;

            new_flame.acc.x = (float)random(FLAME_MIN_H, FLAME_MAX_H) * .01f;
            new_flame.acc.y = (float)random(FLAME_MIN_H, FLAME_MAX_H) * .01f;
            new_flame.acc.z = (float)random(FLAME_MIN_V, FLAME_MAX_V) * .01f;

            new_flame.life_span = random(3, 15);

            new_flame.flicker = FALSE;

            fire.Add(&new_flame); 
        }
    }
    else if(state == FLAME_STATE_MID)
    {
        if(frame_count >= 8)
        {
            state = FLAME_STATE_BLAST;
            frame_count = 0;

            int r = random(10, 25);
            for(i = 0; i < r; ++i)
            {
                new_flame.rot.x = -90.0f;
                new_flame.rot.z = -45.0f;
                new_flame.rot.y = 0.0f;

                new_flame.vel.x = (float)random(-800, 800) * .01f;
                new_flame.vel.y = (float)random(-800, 800) * .01f;
                new_flame.vel.z = (float)random(-800, 800) * .01f;

                new_flame.acc.x = (float)random(65, 95) * .01f;
                new_flame.acc.y = (float)random(65, 95) * .01f;
                new_flame.acc.z = (float)random(65, 95) * .01f;

                int j = random(1, 3);
                for(int x = 0; x < j; ++x)
                {
                    // create a new flame structure
                    new_flame.pos.x = (float)random(-2, 2);
                    new_flame.pos.y = (float)random(-2, 2) - 30.0f;
                    new_flame.pos.z = (float)random(-2, 2) - 30.0f;

                    float scale = (float)random(5, 25) * .01f;

                    new_flame.scl.x = scale;
                    new_flame.scl.y = scale;
                    new_flame.scl.z = scale;

                    new_flame.life_span = random(10, 25);
                    new_flame.flicker = FALSE;

                    burst.Add(&new_flame);
                }
            }
        }
        else
            ++frame_count;
    }
    else if(state == FLAME_STATE_BLAST)
    {
        if(frame_count >= FLAME_FRAME_COUNT)
            done = TRUE;
        else
            ++frame_count;

        for(i = 0; i < FLAME_CREATE && !done; ++i)
        {
            // create a new flame structure
            new_flame.pos.x = (float)random(-4, 4);
            new_flame.pos.y = (float)random(-4, 4) - 30.0f;
            new_flame.pos.z = (float)random(-4, 4) - 30.0f;

            new_flame.rot.x = -90.0f;
            new_flame.rot.z = -45.0f;
            new_flame.rot.y = 0.0f;

            float scale = (float)random(5, 25) * .01f;

            new_flame.temp.x = scale;
            new_flame.temp.y = scale;
            new_flame.temp.z = scale;

            new_flame.vel.x = (float)random(-200, 200) * .01f;
            new_flame.vel.y = -(float)FLAME_SPEED * .01f;
            new_flame.vel.z = (float)random(0, 300) * .01f;

            new_flame.acc.x = (float)random(FLAME_MIN_H, FLAME_MAX_H) * .01f;
            new_flame.acc.y = (float)random(FLAME_MIN_H, FLAME_MAX_H) * .01f;
            new_flame.acc.z = (float)random(FLAME_MIN_V, FLAME_MAX_V) * .01f;

            new_flame.life_span = random(FLAME_MIN_LIFE, FLAME_MAX_LIFE);
            if(i == 0)
                new_flame.flicker = TRUE;
            else
                new_flame.flicker = FALSE;

            fire.Add(&new_flame);
        }
    }

    // do the red particles
    for(i = 0; i < FLAME_COUNT; ++i)
    {
        flame = fire.Get(i);
        if(!flame->used)
            continue;
        ++flame_count;

        if(state == FLAME_STATE_START)
            flame->vel.z += 1.5f;
        else if(state == FLAME_STATE_MID)
        {
            flame->acc.x = 0.0f;
            flame->acc.y = 0.0f;
            flame->acc.z = 0.0f;
            flame->vel.x = 0.0f;
            flame->vel.y = 0.0f;
            flame->vel.z = 0.0f;
            float diff;
            if(flame->pos.x != 0.0f)
            {
                diff = (0.0f - flame->pos.x) * .25f;
                flame->pos.x += diff;
            }
            if(flame->pos.y != -30.0f)
            {
                diff = (-30.0f - flame->pos.y) * .25f;
                flame->pos.y += diff;
            }
            if(flame->pos.z != -30.0f)
            {
                diff = (-30.0f - flame->pos.z) * .25f;
                flame->pos.z += diff;
            }
        }
        else if((float)flame->life / (float)flame->life_span > .5f)
            flame->vel.z += 1.5f;

        float equ = (125.0f - flame->pos.z) * .01f;
        flame->scl.x = flame->temp.x * equ;
        flame->scl.y = flame->temp.y * equ;
        flame->scl.z = flame->temp.z * equ;

    }

    for(i = 0; i < FLAME_COUNT; ++i)
    {
        flame = smoke.Get(i);
        if(!flame->used)
            continue;
        ++flame_count;
        
        flame->vel.z += 3.0f;

        float equ = (125.0f - flame->pos.z) * .01f;
        if(equ < 0.0f)
            equ = 0.0f;

        flame->scl.x = flame->temp.x * equ;
        flame->scl.y = flame->temp.y * equ;
        flame->scl.z = flame->temp.z * equ;
    }

    for(i = 0; i < FLAME_BURST; ++i)
    {
        flame = burst.Get(i);
        if(!flame->used)
            continue;

        ++flame_count;

        flame->scl.x *= .95f;
        flame->scl.y *= .95f;
        flame->scl.z *= .95f;
/*
        if(flame->flicker)
        {
            S3DPoint pos;
            S3DPoint effect;
            ((PTEffect)inst)->GetPos(effect);
            ((PTEffect)inst)->GetSpell()->GetInvoker()->GetPos(pos);
            pos.x = (int)flame->pos.x + effect.x;
            pos.y = (int)flame->pos.y + effect.y;
            BurnCharactersInRange(((PTEffect)inst)->GetSpell()->GetInvoker(), pos, 60);
        }
*/
    }



//  PTSpell spell = ((PTEffect)inst)->GetSpell();
//  if(spell)
//  {
//      PTCharacter invoker = (PTCharacter)spell->GetInvoker();
//      if(invoker)
//      {
//          PTCharacter target = (PTCharacter)invoker->Fighting();

//          S3DPoint temp_point;
//          S3DPoint temp_point2;
//          target->GetPos(temp_point);
//          invoker->GetPos(temp_point2);

//          temp_point.x = temp_point.x - temp_point2.x;
//          temp_point.y = temp_point.y - temp_point2.y;
//          temp_point.z = temp_point.z - temp_point2.z;

//          for(int j = 0; j < FLAME_COUNT; j++)
//          {
//              flame = fire.Get(j);
//              temp_point.x = flame->pos.x;
//              temp_point.y = flame->pos.y;
//              temp_point.z = flame->pos.z;
//              BurnCharactersInRange(((PTEffect)inst)->GetSpell()->GetInvoker(), temp_point, 60);
//          }
//          if(flame->pos.x < (temp_point.x + 30) && (flame->pos.x > temp_point.x - 30)
//              && flame->pos.y < (temp_point.y + 30) && (flame->pos.y > temp_point.y - 30))
//          {
//              target->Burn();
//          }
//          flame = burst.Get(0);
//          temp_point.x = flame->pos.x;
//          temp_point.y = flame->pos.y;
//          temp_point.z = flame->pos.z;
//          BurnCharactersInRange(((PTEffect)inst)->GetSpell()->GetInvoker(), temp_point, 60);
//          if(flame->pos.x < (temp_point.x + 10) && (flame->pos.x > temp_point.x - 10)
//              && flame->pos.y < (temp_point.y + 10) && (flame->pos.y > temp_point.y - 10))
//          {
//              target->Burn();
//          }
//      }
//  }


    // finish the effect
    if(!flame_count && done)
    {
        ((PTEffect)inst)->KillThisEffect();
    }
}

// render the fire particles
BOOL TFireConeAnimator::Render()
{
    SaveBlendState();
    SetAddBlendState();

    fire.Render();
    smoke.Render();
    burst.Render();

    RestoreBlendState();

    return TRUE;
}

// refresh the z buffer
void TFireConeAnimator::RefreshZBuffer()
{
    S3DPoint size;
    S3DPoint map, screen;

    size.x = 500;
    size.y = 250;

    ((PTEffect)inst)->GetPos(map);

    WorldToScreen(map, screen);

    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

// ***************************
// *** THE DREAD FIRE CONE ***
// ***************************

#define DRAGON_FLAME_CREATE     18
#define DRAGON_FLAME_COUNT      300

// effect
_CLASSDEF(TDragonFireEffect)
class TDragonFireEffect : public TEffect
{
  public:
    TDragonFireEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TDragonFireEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TDragonFireEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

// animator for the fire cone class
_CLASSDEF(TDragonFireAnimator)
class TDragonFireAnimator : public T3DAnimator
{
  private:
    TParticleSystem fire;
    TParticleSystem smoke;
    BOOL done;
    int frame_count;
  public:
    TDragonFireAnimator(PTObjectInstance oi) : T3DAnimator(oi), fire(DRAGON_FLAME_COUNT), smoke(DRAGON_FLAME_COUNT) {}
    virtual ~TDragonFireAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};


// **************************
// * TFireConeEffect Object *
// **************************

DEFINE_BUILDER("DragonFire", TDragonFireEffect)
REGISTER_BUILDER(TDragonFireEffect)

// initialize the fire cone effect
void TDragonFireEffect::Initialize()
{
}

// pulse through the fire cone
void TDragonFireEffect::Pulse()
{
    TEffect::Pulse();
}

// ****************************
// * TFireConeAnimator Object *
// ****************************

REGISTER_3DANIMATOR("DragonFire", TDragonFireAnimator)

// initialize the fire particles
void TDragonFireAnimator::Initialize()
{
    T3DAnimator::Initialize();

    S3DPoint size;
    size.x = 10;
    size.y = 10;

    //fire.Init(this, obj, size, -((float)(inst->GetFace() * 360.0f) / 256.0f) * (float)TORADIAN);

    float facing = -(((float)inst->GetFace() * 360.0f) / 256.0f) * (float)TORADIAN;

    PS3DAnimObj obj = GetObject(0);
    fire.Init(this, obj, size, TRUE, facing);

    obj = GetObject(1);
    smoke.Init(this, obj, size, TRUE, facing);

    done = FALSE;
    frame_count = 0;
}

// animate the fire particles
void TDragonFireAnimator::Animate(BOOL draw)
{
    // animate stuff
    T3DAnimator::Animate(draw);

    inst->SetCommandDone(FALSE);

    if(frame_count >= FLAME_FRAME_COUNT * 2)
        done = TRUE;
    else
        ++frame_count;

    PSParticleSystemInfo flame;
    SParticleSystemInfo new_flame;
    int flame_count = 0;

    smoke.Animate();

    for(int i = 0; i < DRAGON_FLAME_COUNT; ++i)
    {
        flame = fire.Get(i);
        if(!flame->used)        
            continue;
        if(flame->life >= flame->life_span)
        {
            new_flame.pos = flame->pos;
            new_flame.temp = flame->temp;
            new_flame.vel.x = 0.0f;
            new_flame.vel.y = 0.0f;
            new_flame.vel.z = flame->vel.z;
            new_flame.acc.x = 0.0f;
            new_flame.acc.y = 0.0f;
            new_flame.acc.z = flame->acc.z;
            new_flame.scl = flame->scl;
            new_flame.rot = flame->rot;
            new_flame.life_span = 15;
            smoke.Add(&new_flame);
        }
    }

    fire.Animate();

    for(i = 0; i < DRAGON_FLAME_CREATE && !done; ++i)
    {
        // create a new flame structure
        new_flame.pos.x = (float)random(-4, 4) + 18.0f;
        new_flame.pos.y = (float)random(-4, 4) - 50.0f;
        new_flame.pos.z = (float)random(-4, 4) + 15.0f;

        new_flame.rot.x = 0.0f;
        new_flame.rot.y = 0.0f;
        new_flame.rot.z = 0.0f;

        float scale = (float)random(5, 25) * .01f;

        new_flame.temp.x = scale;
        new_flame.temp.y = scale;
        new_flame.temp.z = scale;

        new_flame.vel.x = (float)random(-200, 200) * .01f;
        new_flame.vel.y = -(float)2000 * .01f;
        new_flame.vel.z = (float)random(-800, -300) * .01f;

        new_flame.acc.x = (float)random(FLAME_MIN_H, FLAME_MAX_H) * .01f;
        new_flame.acc.y = (float)random(FLAME_MIN_H, FLAME_MAX_H) * .01f;
        new_flame.acc.z = (float)random(FLAME_MIN_V, FLAME_MAX_V) * .01f;

        new_flame.life_span = random(FLAME_MIN_LIFE, FLAME_MAX_LIFE);

        fire.Add(&new_flame);
    }

    // do the red particles
    for(i = 0; i < DRAGON_FLAME_COUNT; ++i)
    {
        flame = fire.Get(i);
        if(!flame->used)
            continue;
        ++flame_count;

        if((float)flame->life / (float)flame->life_span > .5f)
            flame->vel.z += 1.5f;

        float equ = (125.0f - flame->pos.z) * .01f;
        flame->scl.x = flame->temp.x * equ;
        flame->scl.y = flame->temp.y * equ;
        flame->scl.z = flame->temp.z * equ;
    }

    for(i = 0; i < DRAGON_FLAME_COUNT; ++i)
    {
        flame = smoke.Get(i);
        if(!flame->used)
            continue;
        ++flame_count;
        
        flame->vel.z += 3.0f;

        float equ = (125.0f - flame->pos.z) * .01f;
        if(equ < 0.0f)
            equ = 0.0f;

        flame->scl.x = flame->temp.x * equ;
        flame->scl.y = flame->temp.y * equ;
        flame->scl.z = flame->temp.z * equ;
    }

    // finish the effect
    if(!flame_count && done)
        ((PTEffect)inst)->KillThisEffect();
}

// render the fire particles
BOOL TDragonFireAnimator::Render()
{
    SaveBlendState();
    SetAddBlendState();

    fire.Render();
    smoke.Render();

    RestoreBlendState();

    return TRUE;
}

// refresh the z buffer
void TDragonFireAnimator::RefreshZBuffer()
{
    S3DPoint size;
    S3DPoint map, screen;

    size.x = 500;
    size.y = 250;

    ((PTEffect)inst)->GetPos(map);

    WorldToScreen(map, screen);

    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}

// ******************
// * IceBolt Effect *
// ******************

#define GROW_DURATION       10
#define SPHERE_STEP         0.5
#define RING_SPEED          20
#define SPIRAL_STEP         1.0
#define SPIRAL_SCALE_STEP   0.1
#define FROST_INIT_SIZE     0.5
#define FROST_STEP          0.03
#define FROST_DAMAGE_MIN    10
#define FROST_DAMAGE_MAX    25
#define SW_ROTSPEED         0.5
#define SW_SCALESPEED       0.2

DEFINE_BUILDER("IceBolt", TIceBoltEffect)
REGISTER_BUILDER(TIceBoltEffect)

void TIceBoltEffect::Initialize()
{
//  TMissileEffect::Initialize();

//  SetSpeed(ELECTRIC_BOLT_SPEED);
}

void TIceBoltEffect::Pulse()
{
//  TMissileEffect::Pulse();
    TEffect::Pulse();
}

// ********************
// * IceBolt Animator *
// ********************

REGISTER_3DANIMATOR("IceBolt", TIceBoltAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for an electric bolt animator.
//               The bolt effect consists of sparks that gather together
//               to form a big spark which fires
//
//==============================================================================

void TIceBoltAnimator::Initialize()
{
    T3DAnimator::Initialize();
    int i;

    S3DPoint temp_point;

    inst->GetPos(temp_point);
    temp_point.z += 50;

//  SetPos(temp_point);
    inst->SetPos(temp_point);

    frameon = 0;
    spherescale[0] = (D3DVALUE)SPHERE_STEP;
    spherescale[1] = (D3DVALUE)1.0;
    cylscale = (D3DVALUE)SPHERE_STEP;
    ringout = (D3DVALUE)0.0;
    spiralang = (D3DVALUE)0.0;
    spiralscale = (D3DVALUE)SPIRAL_SCALE_STEP;
    for (i = 0; i < MAX_FROST_PARTICLES; i++)
    {
        p[i].x = (D3DVALUE)random(-10, 10);
        p[i].y = (D3DVALUE)random(-10, 10);
        p[i].z = (D3DVALUE)random(-10, 10);
        v[i].x = (D3DVALUE)random(-3, 3);
        v[i].y = (D3DVALUE)random(-3, 3);
        v[i].z = (D3DVALUE)random(-3, 3);
        s[i] = (D3DVALUE)FROST_INIT_SIZE;
        t[i] = (float)random(0, (int)(FROST_INIT_SIZE * 2 / FROST_STEP));
    }

    angle = ((PTEffect)inst)->GetAngle();

    ((PTEffect)inst)->SetSubSpell(3);

    int subspell = ((PTEffect)inst)->GetSubSpell();
    /*if (subspell > 2)
    {
        SObjectDef def;
        memset(&def, 0, sizeof(SObjectDef));
        def.objclass = OBJCLASS_EFFECT;

        def.objtype = EffectClass.FindObjType("snow");
        def.level = MapPane.GetMapLevel();
        S3DPoint pos;
        ((PTEffect)inst)->GetPos(pos);
        def.pos = pos;
        PTObjectInstance instance = MapPane.GetInstance(MapPane.NewObject(&def));
        if (!instance)
            return;

        instance->CreateAnimator();
        PTParticle3DAnimator anim = (PTParticle3DAnimator)instance->GetAnimator();
        particleanim = anim;

        //int ang = (angle + 64) & 0xff; // fly back
        int ang = angle;

        S3DPoint vect;
        ConvertToVector(ang, 100, vect);

        if (anim)
        {
            SParticleParams pr;

            pr.particles = random(50, 100);
            pr.pos.x = (D3DVALUE)0.0;
            pr.pos.y = (D3DVALUE)0.0;
            pr.pos.z = (D3DVALUE)0.0;
            pr.pspread.x = (D3DVALUE)25.0;
            pr.pspread.y = (D3DVALUE)25.0;
            pr.pspread.z = (D3DVALUE)50.0;
            pr.dir.x = (D3DVALUE)((float)vect.x / (float)100.0);
            pr.dir.y = (D3DVALUE)((float)vect.y / (float)100.0);
            pr.dir.z = (D3DVALUE)((float)vect.z / (float)100.0);
            pr.spread.x = (D3DVALUE)0.0;
            pr.spread.y = (D3DVALUE)0.0;
            pr.spread.z = (D3DVALUE)0.0;
            pr.gravity = (D3DVALUE)0.0;
            pr.trails = 1;
            pr.minstart = 0;
            pr.maxstart = pr.particles / 2;
            pr.minlife = 100;
            pr.maxlife = 150;
            pr.bounce = FALSE;
            pr.killobj = TRUE;
            pr.objflags = 0xF;

            pr.numtargets = 1;
            pr.targetpos[0].x = (D3DVALUE)0.0;
            pr.targetpos[0].y = (D3DVALUE)0.0;
            pr.targetpos[0].z = (D3DVALUE)0.0;
            
            pr.seektargets = TRUE;
            pr.turnang = (D3DVALUE)0.1;
            pr.seekspeed = (D3DVALUE)5.0;
            pr.autorange = 0.0;
            pr.hitrange = 0.0;
            pr.seekz = FALSE;

            anim->InitParticles(&pr);
        }
    }*/
    if (subspell > 2)
    {
        for (i = 0; i < MAX_SNOW_PARTICLES; i++)
        {
            h[i] = (float)random(-50, 50);
            th[i] = (float)(random(0, 359) * TORADIAN);
            rs[i] = (float)(random(5, 15) / 100.0);
            sz[i] = (float)(random((int)(FROST_STEP * 100), (int)(FROST_STEP * GROW_DURATION * 100)) / 100.0);
            r[i] = (float)random(25, 50);
        }
        cy = (float)0;
        /*sws = (float)0.5;
        rx = (float)0.0;
        ry = (float)0.0;
        rz = (float)0.0;*/
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the sparks and bolt
//
//==============================================================================

void TIceBoltAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    int subspell = ((PTEffect)inst)->GetSubSpell();
    S3DPoint effect_pos;
    S3DPoint pos;
        
    if (frameon == 0)
    {
        float r = 10.0f;
        float dx = r * (float)sin((float)TORADIAN * (((float)inst->GetFace() * 360.0f) / 256.0f));
        float dy = -r * (float)cos((float)TORADIAN * (((float)inst->GetFace() * 360.0f) / 256.0f));
        S3DPoint effect;
        ((PTEffect)inst)->GetPos(effect);
        D3DVECTOR point;
        point.x = (float)effect.x;
        point.y = (float)effect.y;
        point.z = (float)effect.z;
        int ticks = 0;
        float zheight;
//      int num;
        BOOL flag = FALSE;
        S3DPoint new_pos;
        do
        {
            point.x += dx;
            point.y += dy;

            effect.x = (int)point.x;
            effect.y = (int)point.y;
            effect.z = (int)point.z;

            zheight = (float)MapPane.GetWalkHeight(effect);

//          int targets[MAXFOUNDOBJS];

            new_pos.x = (int)point.x;
            new_pos.y = (int)point.y;
            new_pos.z = (int)point.z - FROST_HEIGHT;

            // See if we've hit a character
            S3DPoint temp_point1, temp_point2;
            temp_point1.x = new_pos.x;
            temp_point1.y = new_pos.y;
            temp_point1.z = new_pos.z;
            PTCharacter invoker;

            PTSpell spell = ((PTIceBoltEffect)inst)->GetSpell();
            
            if(spell)
            {
                invoker = (PTCharacter)spell->GetInvoker();
            }
            for (TMapIterator i(NULL, CHECK_NOINVENT, OBJSET_CHARACTER); i; i++)
            {
                PTCharacter chr = (PTCharacter)i.Item();

                if (chr == invoker)
                    continue;

                if (chr->IsDead())
                    continue;

                chr->GetPos(temp_point2);
                if (Distance(temp_point1,temp_point2) > 10)
                    continue;

                if (invoker && !invoker->IsEnemy(chr))
                    continue;   // We can't hurt our friends
                
                flag = TRUE;
                break;
            }

/*          num = MapPane.FindObjectsInRange(new_pos, targets, 10, 0, OBJCLASS_CHARACTER);

            for(int i = 0; i < num; ++i) 
            {
                PTObjectInstance inst = MapPane.GetInstance(targets[i]);
                if (inst && inst != ((PTEffect)inst)->GetSpell()->GetInvoker() &&
                    !((PTCharacter)inst)->IsDead())
                        flag = TRUE;
            }
*/
            ++ticks;
        }
        while(point.z > zheight && flag == FALSE && ticks < MAX_MAXPOINTS);
        length = (float)(ticks * 10.0);
    }
    frameon++;
//
    if (subspell > 2)
    {
        for (int i = 0; i < MAX_SNOW_PARTICLES; i++)
        {
            th[i] += rs[i];
            if (th[i] > M_2PI)
                th[i] -= (float)M_2PI;
            rs[i] += (float)0.01;
        }
        if (frameon > GROW_DURATION * 2)
        {
            cy += (float)(length / (GROW_DURATION * 2));
            if (cy >= length)
                cy = (float)length;
        }
        if (frameon > GROW_DURATION * 5)
        {
            /*rx += (float)SW_ROTSPEED;
            ry += (float)SW_ROTSPEED;
            rz += (float)SW_ROTSPEED;
            if (rx > M_2PI)
                rx -= (float)M_2PI;
            if (ry > M_2PI)
                ry -= (float)M_2PI;
            if (rz > M_2PI)
                rz -= (float)M_2PI;
            sws += (float)SW_SCALESPEED;*/
            // float up, together, and faster
            for (int i = 0; i < MAX_SNOW_PARTICLES; i++)
            {
                h[i] += (float)5.0;
                r[i] -= (float)1.0;
                if (r[i] < 1)
                    r[i] = (float)1.0;
                rs[i] += (float)0.05;
            }
        }
    }
    /*if (subspell > 2 && frameon < GROW_DURATION * 4)
    {
        SParticleParams pr;
        pr.numtargets = 1;

        pr.targetpos[0].x = (D3DVALUE)0.0;
        pr.targetpos[0].y = (D3DVALUE)0.0;
        pr.targetpos[0].z = (D3DVALUE)0.0;
        
        pr.seektargets = TRUE;
        pr.seekz = FALSE;
        pr.turnang = (D3DVALUE)0.1;
        pr.seekspeed = (D3DVALUE)(5.0 + (frameon / 16));
        pr.autorange = 0.0;
        pr.hitrange = 0.0;

        particleanim->ResetTargetInfo(pr);
    }
    if (subspell > 2 && frameon == GROW_DURATION * 4)
    {
        SParticleParams pr;
        pr.numtargets = 0;

        int targets[MAXFOUNDOBJS];
        S3DPoint effect_pos;
        ((PTEffect)inst)->GetPos(effect_pos);
        effect_pos.y -= (int)length;
        PTObjectInstance caster = ((PTEffect)inst)->GetSpell()->GetInvoker();
        int num = MapPane.FindObjectsInRange(effect_pos, targets, 200, 0, OBJCLASS_CHARACTER);
        effect_pos.y += (int)length;
        for (int i = 0; i < num && pr.numtargets < 5; i++)
        {
            PTObjectInstance instan = MapPane.GetInstance(targets[i]);
            if (instan && instan != caster && !((PTCharacter)instan)->IsDead())// && !(instan->GetFlags() & OF_ICED))
            {
                S3DPoint putpos;
                ((PTCharacter)instan)->GetPos(putpos);

                pr.targetpos[pr.numtargets].x = (float)(putpos.x - effect_pos.x);
                pr.targetpos[pr.numtargets].y = (float)(putpos.y - effect_pos.y);
                pr.targetpos[pr.numtargets].z = (float)(putpos.z - effect_pos.z);// + 70;
                pr.numtargets++;
            }
        }
        //
        if (pr.numtargets == 0)
            pr.seektargets = FALSE;
        else
            pr.seektargets = TRUE;
        pr.seekz = FALSE;
        pr.turnang = (D3DVALUE)0.1;
        pr.seekspeed = (D3DVALUE)5.0;
        pr.autorange = 0.0;
        pr.hitrange = 0.0;

        particleanim->ResetTargetInfo(pr);
    }*/
    for (int i = 0; i < MAX_FROST_PARTICLES; i++)
    {
        t[i]--;
        if (t[i] > 0)
            continue;
        else
            t[i] = 0;
        p[i].x += v[i].x;
        p[i].y += v[i].y;
        p[i].z += v[i].z;
        s[i] -= (float)FROST_STEP;
        if (s[i] < 0)
        {
            p[i].x = (D3DVALUE)random(-10, 10);
            p[i].y = (D3DVALUE)random(-10, 10);
            p[i].z = (D3DVALUE)random(-10, 10);
            v[i].x = (D3DVALUE)random(-3, 3);
            v[i].y = (D3DVALUE)random(-3, 3);
            v[i].z = (D3DVALUE)random(-3, 3);
            s[i] = (D3DVALUE)FROST_INIT_SIZE;
            t[i] = (float)random(0, (int)(FROST_INIT_SIZE / FROST_STEP));
            if (frameon > GROW_DURATION * 3)
                t[i] = 100;
        }
    }
    if (frameon < GROW_DURATION)
    {
        spherescale[0] += (D3DVALUE)SPHERE_STEP;
        spherescale[1] += (D3DVALUE)(SPHERE_STEP * 2);
        cylscale += (D3DVALUE)SPHERE_STEP;
        spiralscale += (D3DVALUE)SPIRAL_SCALE_STEP;
        spiralang += (D3DVALUE)SPIRAL_STEP;
        if (spiralang > M_2PI)
            spiralang -= (D3DVALUE)M_2PI;
    }
    if (frameon >= GROW_DURATION && frameon < GROW_DURATION * 2)
    {
        cylscale -= (D3DVALUE)(SPHERE_STEP / 2);
        spherescale[0] += (D3DVALUE)SPHERE_STEP;
        spherescale[1] += (D3DVALUE)SPHERE_STEP;
        spiralang += (D3DVALUE)SPIRAL_STEP;
        if (spiralang > M_2PI)
            spiralang -= (D3DVALUE)M_2PI;
    }
    if (frameon >= GROW_DURATION * 2 && frameon < GROW_DURATION * 3)
    {
        cylscale -= (D3DVALUE)(SPHERE_STEP / 2);
        spherescale[0] -= (D3DVALUE)SPHERE_STEP;
        spherescale[1] -= (D3DVALUE)(SPHERE_STEP * 1);
        spiralang += (D3DVALUE)SPIRAL_STEP;
        if (spiralang > M_2PI)
            spiralang -= (D3DVALUE)M_2PI;
    }
    if (frameon >= GROW_DURATION * 3 && frameon < GROW_DURATION * 4)
    {
        spherescale[0] -= (D3DVALUE)SPHERE_STEP;
        spherescale[1] -= (D3DVALUE)(SPHERE_STEP * 2);
        spiralscale -= (D3DVALUE)SPIRAL_SCALE_STEP;
    }
    ringout += RING_SPEED;
    if (ringout > length)
        ringout -= length;
    if (frameon == GROW_DURATION)
    {
        // deal damage proporational to subspell level
        ((PTEffect)inst)->GetPos(effect_pos);
        effect_pos.y -= (int)length;
        PTObjectInstance caster = ((PTEffect)inst)->GetSpell()->GetInvoker();
//      DamageCharactersInRange(caster, effect_pos, 200, ((PTEffect)inst)->GetSpell()->VariantData()->mindamage, ((PTEffect)inst)->GetSpell()->VariantData()->maxdamage, ((PTEffect)inst)->GetSpell()->SpellData()->damagetype);
        DamageCharactersInRange(caster, effect_pos, 200, ((PTEffect)inst)->GetSpell()->VariantData()->mindamage, ((PTEffect)inst)->GetSpell()->VariantData()->maxdamage, DAMAGE_ICE);
    }
    if (frameon == GROW_DURATION * 5 && subspell > 2) // kill them dead
    {
        ((PTEffect)inst)->GetPos(effect_pos);
        effect_pos.y -= (int)length;
        PTObjectInstance caster = ((PTEffect)inst)->GetSpell()->GetInvoker();
//      DamageCharactersInRange(caster, effect_pos, 200, 250, 250, DAMAGE_ICE);
    }
    // freeze guys (if high enough spell level)
    if (frameon == GROW_DURATION * 2 && subspell > 1)
    {
        ((PTEffect)inst)->GetPos(effect_pos);
        PTObjectInstance caster = ((PTEffect)inst)->GetSpell()->GetInvoker();

        // find me some characters to ice!
        S3DPoint putpos, vel;
        ((PTEffect)inst)->GetPos(putpos);
        ConvertToVector((int)angle, (int)length, vel);
        effect_pos.x = putpos.x + vel.x;
        effect_pos.y = putpos.y + vel.y;
        effect_pos.z = putpos.z;

        PTObjectInstance toice = NULL;
        int targets[MAXFOUNDOBJS];
        int num = MapPane.FindObjectsInRange(effect_pos, targets, 200, 0, OBJCLASS_CHARACTER);
        for (int i = 0; i < num; i++)
        {
            PTObjectInstance instan = MapPane.GetInstance(targets[i]);
            if (instan && instan != caster && !((PTCharacter)instan)->IsDead()/* && !(instan->GetFlags() & OF_ICED)*/)
            {
                // create the iced effect
                SObjectDef def;
                memset(&def, 0, sizeof(SObjectDef));
                def.objclass = OBJCLASS_EFFECT;
                def.level = MapPane.GetMapLevel();
                S3DPoint putpos;
                ((PTCharacter)instan)->GetPos(putpos);
                def.pos = putpos;
                def.objtype = EffectClass.FindObjType("Iced");
                PTIcedEffect effect = (PTIcedEffect)MapPane.GetInstance(MapPane.NewObject(&def));

                // now ice them!
                if (!effect->HasAnimator())
                    effect->CreateAnimator();
                PTIcedAnimator ia = (PTIcedAnimator)effect->GetAnimator();
                if (ia)
                    ia->InitIced(instan);
            }
        }
    }
    if (frameon == GROW_DURATION * 12) // 4
    {
        ((PTEffect)inst)->KillThisEffect();
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders spheres and cylinders, etc.
//
//==============================================================================

BOOL TIceBoltAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;
    int i, j, subspell = ((PTEffect)inst)->GetSubSpell();

    // draw the cylinder
    if (subspell > 1)
    {
        for (i = 0; i < 4; i++)
        {
            obj = GetObject(i);
            D3DMATRIXClear(&obj->matrix);
            obj->flags = OBJ3D_MATRIX;
            // scale to size of spark
            //obj->scl.x = (D3DVALUE)((4 - i) * cylscale / 4);
            //obj->scl.y = (D3DVALUE)((4 - i) * cylscale / 4);
            obj->scl.x = (D3DVALUE)(cylscale / 2);
            obj->scl.y = (D3DVALUE)(cylscale / 2);
            obj->scl.z = (D3DVALUE)((float)length / 64.0f); // scale to length...?
            D3DMATRIXScale(&obj->matrix, &obj->scl);

            D3DMATRIXRotateX(&obj->matrix, D3DVALUE((M_PI / 2.0)));//-
            D3DMATRIXRotateY(&obj->matrix, 0.0f);
            D3DMATRIXRotateZ(&obj->matrix, 0.0f);

            /*S3DPoint vel;
            ConvertToVector((angle + 64) & 255, -32, vel); 
            obj->pos.x = (D3DVALUE)vel.x;
            obj->pos.y = (D3DVALUE)vel.y;*/
            obj->pos.x = (D3DVALUE)0.0;
            obj->pos.y = (D3DVALUE)0.0;
            obj->pos.z = (D3DVALUE)0.0;
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);
            
            ResetExtents();
            RenderObject(obj);
            UpdateExtents();
        }
    }

    if (frameon < GROW_DURATION * 3)
    {
        // draw the spirals around the cylinder
        if (subspell > 0)
        {
            obj = GetObject(6);
            int numrevs = (int)length / 100;
            D3DVALUE zscale = (D3DVALUE)(length / numrevs),
                ypos;
            for (j = 0; j < numrevs; j++)
            {
                ypos = (D3DVALUE)(-j * zscale);
                for (i = 0; i < 2; i++)
                {
                    D3DMATRIXClear(&obj->matrix);
                    obj->flags = OBJ3D_MATRIX;
                    // scale to size of spark
                    obj->scl.x = (D3DVALUE)(spiralscale);
                    obj->scl.y = (D3DVALUE)(spiralscale);
                    //obj->scl.z = (D3DVALUE)(0.1250); // scale to make ring look fat, not long
                    obj->scl.z = (D3DVALUE)(zscale / 64.0f); // scale to length...?
                    D3DMATRIXScale(&obj->matrix, &obj->scl);
                    
                    D3DMATRIXRotateX(&obj->matrix, D3DVALUE((M_PI / 2.0)));//-
                    D3DMATRIXRotateY(&obj->matrix, (float)(i ? M_PI - spiralang : spiralang));
                    D3DMATRIXRotateZ(&obj->matrix, 0.0f);

                    // translate to place should be
                    obj->pos.x = (D3DVALUE)0.0f;
                    obj->pos.y = (D3DVALUE)ypos;
                    obj->pos.z = (D3DVALUE)0.0f;
                    D3DMATRIXTranslate(&obj->matrix, &obj->pos);

                    ResetExtents();
                    RenderObject(obj);
                    UpdateExtents();
                }
            }
        }
        // draw the rings around the cylinder
        obj = GetObject(5);
        for (i = 0; i < int(length / 100); i++) // draw more if longer
        {
            D3DMATRIXClear(&obj->matrix);
            obj->flags = OBJ3D_MATRIX;
            // scale to size of spark
            obj->scl.x = (D3DVALUE)(cylscale / 2);
            obj->scl.y = (D3DVALUE)(cylscale / 2);
            obj->scl.z = (D3DVALUE)(1.0); // scale to make ring look fat, not long
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            
            D3DMATRIXRotateX(&obj->matrix, D3DVALUE((M_PI / 2.0)));//-
            D3DMATRIXRotateY(&obj->matrix, spiralang);
            D3DMATRIXRotateZ(&obj->matrix, 0.0f);

            obj->pos.x = (D3DVALUE)0.0f;
            obj->pos.y = (D3DVALUE)(-((int)(ringout + 100 * i) % (int)(length)));
            obj->pos.z = (D3DVALUE)0.0f;
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            ResetExtents();
            RenderObject(obj);
            UpdateExtents();
        }
    }
    
    obj = GetObject(4);
    if (subspell > 0)
    {
        for (i = 0; i < 2; i++) // draw one glow sphere on each end
        {
            // draw the ball thing
            ResetExtents();
            // clear matrix and get ready to transform and render
            D3DMATRIXClear(&obj->matrix);
            obj->flags = OBJ3D_MATRIX;
            // scale to size of spark
            obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)(spherescale[i] / 2.0);
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            // rotate to face screen
            D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
            D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
            D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
            D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
            //
            //D3DMATRIXRotateY(&obj->matrix, (float)(M_PI / 8.0));
            // translate to place should be
            obj->pos.x = (D3DVALUE)0.0f;
            obj->pos.y = (D3DVALUE)(-length * i); // one on Locke, one on target
            obj->pos.z = (D3DVALUE)0.0f;
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);
            RenderObject(obj);
            UpdateExtents();
        }
    }
    // draw frost particles
    for (i = 0; i < MAX_FROST_PARTICLES; i++)
    {
        if (t[i] > 0)
            continue;
        ResetExtents();
        // clear matrix and get ready to transform and render
        D3DMATRIXClear(&obj->matrix);
        obj->flags = OBJ3D_MATRIX;
        // scale to size of spark
        obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)(s[i]);
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face screen
        D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
        D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
        D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
        //
        //D3DMATRIXRotateY(&obj->matrix, (float)(M_PI / 8.0));
        // translate to place should be
        obj->pos.x = (D3DVALUE)p[i].x;
        obj->pos.y = (D3DVALUE)(p[i].y - length);
        obj->pos.z = (D3DVALUE)p[i].z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);
        RenderObject(obj);
        UpdateExtents();
    }
    // draw snow flurry and a shockwave
    if (subspell > 2)
    {
        for (i = 0; i < MAX_SNOW_PARTICLES; i++)
        {
            ResetExtents();
            // clear matrix and get ready to transform and render
            D3DMATRIXClear(&obj->matrix);
            obj->flags = OBJ3D_MATRIX;
            // scale to size of spark
            obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)(sz[i]);
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            // rotate to face screen
            D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
            D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
            D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
            D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
            //
            //D3DMATRIXRotateY(&obj->matrix, (float)(M_PI / 8.0));
            // translate to place should be
            obj->pos.x = (D3DVALUE)(r[i] * cos(th[i]));
            obj->pos.y = (D3DVALUE)(r[i] * sin(th[i]));
            if (frameon > GROW_DURATION * 2)
                obj->pos.y -= (float)(cy);
            obj->pos.z = (D3DVALUE)h[i];
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);
            RenderObject(obj);
            UpdateExtents();        
        }
        /*if (frameon > GROW_DURATION * 5)
        {
            obj = GetObject(7);
            ResetExtents();
            // clear matrix and get ready to transform and render
            D3DMATRIXClear(&obj->matrix);
            obj->flags = OBJ3D_MATRIX;
            // scale to size of spark
            obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)(sws);
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            // rotate to face screen
            D3DMATRIXRotateX(&obj->matrix, rx);
            D3DMATRIXRotateX(&obj->matrix, ry);
            D3DMATRIXRotateZ(&obj->matrix, rz);
            //
            //D3DMATRIXRotateY(&obj->matrix, (float)(M_PI / 8.0));
            // translate to place should be
            obj->pos.x = (D3DVALUE)(r[i] * cos(th[i]));
            obj->pos.y = (D3DVALUE)(r[i] * sin(th[i]));
            if (frameon > GROW_DURATION * 2)
                obj->pos.y -= (float)(cy);
            obj->pos.z = (D3DVALUE)h[i];
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);
            RenderObject(obj);
            UpdateExtents();            
        }*/
    }
    RestoreBlendState();

    return TRUE;
}

void TIceBoltAnimator::RefreshZBuffer()
{
    S3DPoint mapstart, mapend, screenstart, screenend, effect, vel;

    ((PTEffect)inst)->GetPos(effect);

    ConvertToVector((int)angle, (int)length, vel); 
    
    mapstart.x = (int)effect.x;
    mapstart.y = (int)effect.y;
    mapstart.z = (int)effect.z;
    WorldToScreen(mapstart, screenstart);

    mapend.x = (int)effect.x + vel.x;
    mapend.y = (int)effect.y + vel.y;
    mapend.z = (int)effect.z;
    WorldToScreen(mapend, screenend);

    int x1 = min(screenstart.x, screenend.x) - 100,
        x2 = max(screenstart.x, screenend.x) + 100,
        y1 = min(screenstart.y, screenend.y) - 100,
        y2 = max(screenstart.y, screenend.y) + 100;
    RestoreZ(x1, y1, x2 - x1, y2 - y1);
}

// ***************
// * Iced Effect *
// ***************

#define ICED_LENGTH         10
#define ICED_DURATION       (24 * ICED_LENGTH)
#define ICED_CHUNK_GRAVITY  0.25

DEFINE_BUILDER("Iced", TIcedEffect)
REGISTER_BUILDER(TIcedEffect)

void TIcedEffect::Initialize()
{
}

void TIcedEffect::Pulse()
{
    TEffect::Pulse();
}

// *****************
// * Iced Animator *
// *****************

REGISTER_3DANIMATOR("Iced", TIcedAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Iced effect.
//               The iced effect consists of a big crystal thing over
//               a character who is disabled, then it shatters
//==============================================================================

void TIcedAnimator::Initialize()
{
    T3DAnimator::Initialize();
    frameon = 0;
    angle = (D3DVALUE)(random(0, 359) * TORADIAN);
    icedchar = NULL;
    donebouncing = 0;
}

void TIcedAnimator::InitIced(PTObjectInstance iceme)
{
    icedchar = iceme;
    if (icedchar)
    {
//      icedchar->SetFlag(OF_DISABLED);
        ((PTCharacter)icedchar)->SetParalize(TRUE);
        ((PTCharacter)icedchar)->SetIced(TRUE);
        //icedchar->SetFlag(OF_ICED);
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the sparks and bolt
//
//==============================================================================

void TIcedAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    S3DPoint pos;

    frameon++;

    if (icedchar)
    {
        //icedchar->SetFlag(OF_DISABLED);
        ((PTCharacter)icedchar)->Stop();
        //icedchar->SetFlag(OF_ICED);
        if(!((PTCharacter)icedchar)->IsIced() && frameon < ICED_DURATION)
            frameon = ICED_DURATION;
    }

    if (icedchar && ((PTCharacter)icedchar)->IsDead() && frameon < ICED_DURATION)
    {
        frameon = ICED_DURATION;
    }
    if (frameon == ICED_DURATION)
    {
        if (icedchar)
        {
            icedchar->SetParalize(FALSE);
            //icedchar->SetFlag(OF_DISABLED, FALSE);
            //icedchar->SetFlag(OF_ICED, FALSE);
        }
        // init ice chunks
        int a = 0, b = 0, c, d;
        for (int i = 0; i < MAX_ICED_CHUNKS; i++)
        {
            c = (a ? 1 : -1);
            d = (b ? 1 : -1);
            // velocity
            v[i].x = (D3DVALUE)(c * random(5, 10) / 10.0 + random(-5, 5) / 10.0);
            v[i].y = (D3DVALUE)(d * random(5, 10) / 10.0 + random(-5, 5) / 10.0);
            v[i].z = (D3DVALUE)(random(0, 10) / 10.0);
            // position
            p[i].x = (D3DVALUE)(c * random(20, 40) + random(-20, 20));
            p[i].y = (D3DVALUE)(d * random(20, 40) + random(-20, 20));
            p[i].z = (D3DVALUE)((int)(i / 6) * 25); // spread in rows of 4, 25 apart
            // angular velocity
            w[i].x = w[i].y = (D3DVALUE)(random(0, 25) / 100.0);
            //w[i].z = (D3DVALUE)(random(-25, 25) / 100.0);
            // angular position
            //t[i].x = (D3DVALUE)random(0, 6);
            //t[i].y = (D3DVALUE)random(0, 6);
            t[i].z = (D3DVALUE)(atan2(v[i].y, v[i].x));
            // scale
            if (random(0, 1))
            {
                s[i].x = (D3DVALUE)(random(25, 75) / 100.0);
                s[i].y = (D3DVALUE)(random(25, 75) / 100.0);
                s[i].z = (D3DVALUE)(random(25, 75) / 100.0);
            }
            else
            {
                s[i].x = (D3DVALUE)(random(10, 15) / 100.0);
                s[i].y = (D3DVALUE)(random(10, 15) / 100.0);
                s[i].z = (D3DVALUE)(random(10, 15) / 100.0);
            }
            // life (bounces left)
            l[i] = random(2, 3);
            // separator
            a = 1 - a;
            if (a)
                b = 1 - b;
        }
    }
    if (frameon > ICED_DURATION)
    {
        donebouncing = 1;
        for (int i = 0; i < MAX_ICED_CHUNKS; i++)
        {
            if (l[i] == 0)
                continue;
            donebouncing = 0;
            p[i].x += v[i].x;
            p[i].y += v[i].y;
            p[i].z += v[i].z;
            v[i].z -= ICED_CHUNK_GRAVITY;
            t[i].x += w[i].x;
            t[i].y += w[i].y;
            t[i].z += w[i].z;
            if (t[i].x > M_2PI)
                t[i].x -= (float)M_2PI;
            if (t[i].y > M_2PI)
                t[i].y -= (float)M_2PI;
            if (t[i].z > M_2PI)
                t[i].z -= (float)M_2PI;
            if (t[i].x < 0)
                t[i].x += (float)M_2PI;
            if (t[i].y < 0)
                t[i].y += (float)M_2PI;
            if (t[i].z < 0)
                t[i].z += (float)M_2PI;
            // bounce
            if (p[i].z <= 16) // fix this
            {
                p[i].z = 20;
                v[i].z *= (float)(-0.5);
                w[i].x *= (D3DVALUE)2.0;
                w[i].y *= (D3DVALUE)2.0;
                w[i].z *= (D3DVALUE)2.0;
                l[i]--;
            }
        }
    }
    if (donebouncing)
    {
        ((PTEffect)inst)->KillThisEffect();
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders ice chunk, possibly shattered chunks
//
//==============================================================================

BOOL TIcedAnimator::Render()
{
    int i;
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;
    if (frameon < ICED_DURATION)
    {
        for (i = 1; i < min(frameon * 4, 50); i++)
        {
            obj = GetObject(i);

            D3DMATRIXClear(&obj->matrix);
            obj->flags = OBJ3D_MATRIX;
            D3DMATRIXRotateZ(&obj->matrix, angle);
            ResetExtents();
            RenderObject(obj);
            UpdateExtents();
        }
    }
    else
    {
        obj = GetObject(0);
        for (int i = 0; i < MAX_ICED_CHUNKS; i++)
        {
            if (l[i] == 0)
                continue;
            ResetExtents();
            // clear matrix and get ready to transform and render
            D3DMATRIXClear(&obj->matrix);
            obj->flags = OBJ3D_MATRIX;
            // scale to size of chunk
            obj->scl.x = s[i].x;
            obj->scl.y = s[i].y;
            obj->scl.z = s[i].z;
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            // rotate to face correct way
            D3DMATRIXRotateX(&obj->matrix, t[i].x);
            D3DMATRIXRotateY(&obj->matrix, t[i].y);
            D3DMATRIXRotateZ(&obj->matrix, t[i].z);
            // translate to place should be
            obj->pos.x = (D3DVALUE)p[i].x;
            obj->pos.y = (D3DVALUE)p[i].y;
            obj->pos.z = (D3DVALUE)p[i].z;
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);
            RenderObject(obj);
            UpdateExtents();
        }
    }
    RestoreBlendState();

    return TRUE;
}

void TIcedAnimator::RefreshZBuffer()
{
    S3DPoint screen, effect, pos, po;
    ((PTEffect)inst)->GetPos(effect);
    if (frameon < ICED_DURATION)
    {
        WorldToScreen(effect, screen);
        int size_x = 150,
            size_y = 200;
        RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2) - 50, size_x, size_y);
    }
    else
    {
        int size_x = 70,
            size_y = 70;
        for (int i = 0; i < MAX_ICED_CHUNKS; i++)
        {
            if (l[i] == 0)
                continue;
            ((PTEffect)inst)->GetPos(effect);
            WorldToScreen(effect, screen);
            po.x = (int)p[i].x;
            po.y = (int)p[i].y;
            po.z = (int)p[i].z;
            WorldToScreen(po, pos);
            screen.x += pos.x;
            screen.y += pos.y;
            RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2) - 10, size_x, size_y);
        }
    }
}



// ********************
// * Quicksand Effect *
// ********************

#define QUICKSAND_DURATION      40
#define QUICKSAND_SWITCH        10.0
#define QUICKSAND_CYLROT        0.5

#define QUICKSAND_SPIN_START    10.0



DEFINE_BUILDER("Quicksand", TQuicksandEffect)
REGISTER_BUILDER(TQuicksandEffect)

void TQuicksandEffect::Initialize()
{
    first_time = TRUE;
}

void TQuicksandEffect::Pulse()
{
    S3DPoint temp_point;
    float temp_z_level;

    TEffect::Pulse();

// added by pepper to move quicksand to the target
    if(animator)
    {
        if(first_time)
        {
            if(spell)
            {
                first_time = FALSE;
                if(strcmp(spell->VariantData()->name, "Quicksand") == 0)
                {
                    ((PTQuicksandAnimator)animator)->level = 1;
                }
                else if(strcmp(spell->VariantData()->name, "Quicksand2") == 0)
                {
                    ((PTQuicksandAnimator)animator)->level = 2;
                }

                PTCharacter invoker = (PTCharacter)spell->GetInvoker();
                if(invoker)
                {
                    invoker->GetPos(temp_point);
                    invoker->GetPos(((PTQuicksandAnimator)animator)->target_position[0]);
                    ((PTQuicksandAnimator)animator)->target_position[0].z = (int)FIX_Z_VALUE(((PTQuicksandAnimator)animator)->target_position[0].z);
                    ((PTQuicksandAnimator)animator)->target[0] = (PTCharacter)invoker->Fighting();
                    if(((PTQuicksandAnimator)animator)->target[0])
                    {
                        ((PTQuicksandAnimator)animator)->target[0]->GetPos(((PTQuicksandAnimator)animator)->target_position[0]);
                        ((PTQuicksandAnimator)animator)->target_position[0].z = (int)FIX_Z_VALUE(((PTQuicksandAnimator)animator)->target_position[0].z);
                        ((PTQuicksandAnimator)animator)->num_targets++;
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
                        
                        ((PTQuicksandAnimator)animator)->target[((PTQuicksandAnimator)animator)->num_targets] = chr;
                        ((PTQuicksandAnimator)animator)->target[((PTQuicksandAnimator)animator)->num_targets]->GetPos(((PTQuicksandAnimator)animator)->target_position[((PTQuicksandAnimator)animator)->num_targets]);
                        ((PTQuicksandAnimator)animator)->target_position[((PTQuicksandAnimator)animator)->num_targets].z = (int)FIX_Z_VALUE(((PTQuicksandAnimator)animator)->target_position[((PTQuicksandAnimator)animator)->num_targets].z);
                        ((PTQuicksandAnimator)animator)->num_targets++;
            
                        if(((PTQuicksandAnimator)animator)->num_targets >= 5 || ((PTQuicksandAnimator)animator)->level != 2)
                            break;
                    }
                }
                else
                {
                    ((PTQuicksandAnimator)animator)->target[0] = NULL;
                    ((PTQuicksandAnimator)animator)->target_position[0].x = ((PTQuicksandAnimator)animator)->target_position[0].y = ((PTQuicksandAnimator)animator)->target_position[0].z = 0;
                }
            }
            else
            {
                ((PTQuicksandAnimator)animator)->num_targets = 0;
                ((PTQuicksandAnimator)animator)->target[0] = NULL;
                GetPos(((PTQuicksandAnimator)animator)->target_position[0]);
                GetPos(temp_point);
                level = 1;
            }
        }
    }

    if(animator)
    {
    // added by pepper to rotate the bad, bad man
        if(((PTQuicksandAnimator)animator)->frameon > QUICKSAND_SPIN_START)
        {
            for(int i = 0;i < ((PTQuicksandAnimator)animator)->num_targets;i++)
            {
                if(((PTQuicksandAnimator)animator)->target[i]  && !((PTQuicksandAnimator)animator)->target[i]->IsDead())
                {
                    ((PTQuicksandAnimator)animator)->target[i]->SetNoCollision(TRUE);
                    ((PTQuicksandAnimator)animator)->target[i]->SetRotateZ((int)((PTQuicksandAnimator)animator)->target_rotation);
                    if(!((PTQuicksandAnimator)animator)->target[i]->IsFlailing())
                    {
                        ((PTQuicksandAnimator)animator)->target[i]->Flail();
                        if(spell)
                        {
                            spell->Damage(((PTQuicksandAnimator)animator)->target[i]);
                        }
                    }
                    ((PTQuicksandAnimator)animator)->target_rotation+=10;
                    temp_z_level = (float)((PTQuicksandAnimator)animator)->target_position[i].z;
                    ((PTQuicksandAnimator)animator)->target[i]->GetPos(((PTQuicksandAnimator)animator)->target_position[i]);
                    ((PTQuicksandAnimator)animator)->target_position[i].z = (int)temp_z_level;
                    SetPos(((PTQuicksandAnimator)animator)->target_position[0]);
                }
            }
        }
    // end added by pepper
    }
}

// **********************
// * Quicksand Animator *
// **********************


REGISTER_3DANIMATOR("Quicksand", TQuicksandAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the quicksand
//==============================================================================

void TQuicksandAnimator::Initialize()
{
    S3DPoint temp_point;
//  int itargets[MAXFOUNDOBJS];
//  int i;

    T3DAnimator::Initialize();
    frameon = 0;
    scalesize = 0.0;
    stage = 0;
    count = 0;
    cylscale = (D3DVALUE)0.0;
    cylheight = (D3DVALUE)1.0;
    cylrot = (D3DVALUE)0.0;
    cylcount = 0;
    ang = 0.0f;
    z_level = 0;

    PS3DAnimObj o = GetObject(0);
    GetVerts(o, D3DVT_LVERTEX);

    if(o->lverts)
    {
        for(int i=0;i<o->numverts;i++)
        {
            o->lverts[i].tv -= 0.1f;
        }
    }

    o = GetObject(2);
    GetVerts(o, D3DVT_LVERTEX);


    zscalefactor = 1.0f;

//  PLAY("vortex sound");

}

void TQuicksandAnimator::InitQuicksand(PTObjectInstance quicksandme, int delay)
{
    frameon = -delay;
    quicksandchar = quicksandme;
    if (quicksandchar)
    {
        quicksandchar->SetFlag(OF_DISABLED);
        //quicksandchar->SetFlag(OF_ICED);
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates all of that dirty old sand
//
//==============================================================================

void TQuicksandAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    S3DPoint pos;

    frameon++;
    ang += 0.06f;
    if (frameon < 0)
    {
        cylscale += (D3DVALUE)(0.125 / QUICKSAND_SWITCH);
        cylheight += (float)0.025;
    }
    if (frameon > 0 && frameon < QUICKSAND_DURATION)
    {
        cylscale -= (D3DVALUE)(0.125 / QUICKSAND_SWITCH);
        cylheight -= (float)0.025;
    }
    if (frameon < 0)
        return;

    if(frameon > 60)
    {
        scalesize -= 0.05f;
    }
    if(scalesize < 1.0f && frameon > 30)
    {
        scalesize += 0.05f;
    }

    if (frameon > QUICKSAND_DURATION * 2)
    {
        zscalefactor -= 0.2f;
        if(zscalefactor <= 0.0f)
        {
            ((PTEffect)inst)->KillThisEffect();
            for(int i=0;i<num_targets;i++)
            {
                target[i]->SetNoCollision(FALSE);
            }
        }
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders quicksand
//
//==============================================================================

void TQuicksandAnimator::SetAnimFrame(int frame_num, PS3DAnimObj obj)
{
    if (!obj->lverts)
        return;
    frame_num = (frame_num + 2) % 4; // this fixes it to do the right frame...(hack)
    float u = (float)(frame_num % 2) * .5f;
    float v = (float)(frame_num / 2) * .5f;

    obj->flags |= OBJ3D_VERTS;

    obj->lverts[0].tu = 0.0f;
    obj->lverts[0].tv = 0.0f;
    obj->lverts[2].tu = 0.25f;
    obj->lverts[2].tv = 0.0f;
    obj->lverts[1].tu = 0.0f;
    obj->lverts[1].tv = 0.25f;
    obj->lverts[3].tu = 0.25f;
    obj->lverts[3].tv = 0.25f;
}

BOOL TQuicksandAnimator::Render()
{
    SaveBlendState();
    SetBlendState();
    int j;

    PS3DAnimObj obj;
    if (frameon < QUICKSAND_DURATION * 2)
    {
        // render spinny sand thing
        obj = GetObject(0);
        // clear matrix and get ready to transform and render
        obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS;
        D3DMATRIXClear(&obj->matrix);       
        // set frame
        SetAnimFrame(stage, obj);
        
        obj->scl.x = (D3DVALUE)scalesize;
        obj->scl.y = (D3DVALUE)scalesize;
        obj->scl.z = 1.0;
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face correct way
        D3DMATRIXRotateZ(&obj->matrix, (D3DVALUE)(ang));
        // translate to place should be

        for(j=0;j<num_targets;j++)
        {
            obj->pos.x = (D3DVALUE)target_position[j].x;
            obj->pos.y = (D3DVALUE)target_position[j].y;
            obj->pos.z = (D3DVALUE)target_position[j].z;
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            ResetExtents();
            RenderObject(obj);
            UpdateExtents();
        }
    }
    if (frameon < QUICKSAND_DURATION)
    {
        // render cylinder(s)
        obj = GetObject(1);
        for (int i = 0; i < 2; i++)
        {
            // clear matrix and get ready to transform and render
            obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS;
            D3DMATRIXClear(&obj->matrix);       
            // scale up and sideways
            obj->scl.x = (D3DVALUE)cylscale;
            obj->scl.y = (D3DVALUE)cylscale;
            obj->scl.z = cylheight;
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            // rotate to face correct way
            D3DMATRIXRotateZ(&obj->matrix, (float)(i ? M_PI - cylrot : cylrot));
            // translate to place should be

            for(j=0;j<num_targets;j++)
            {
                obj->pos.x = (D3DVALUE)target_position[j].x;
                obj->pos.y = (D3DVALUE)target_position[j].y;
                obj->pos.z = (D3DVALUE)target_position[j].z;
                D3DMATRIXTranslate(&obj->matrix, &obj->pos);

                ResetExtents();
                RenderObject(obj);
                UpdateExtents();
            }
        }
    }

// added by pepper: makes dust fly around and up
    SetBlendState();

    DWORD oldcullmode;
    Scene3D.GetRenderState(D3DRENDERSTATE_CULLMODE, &oldcullmode);
    Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

    obj = GetObject(2);
    obj->flags = OBJ3D_SCL1 | OBJ3D_POS2  | OBJ3D_VERTS| OBJ3D_ABSPOS;
    obj->scl.x = (D3DVALUE)scalesize;
    obj->scl.y = (D3DVALUE)scalesize;
    obj->scl.z = (D3DVALUE)1.0f;
//  ResetExtents();

    for(int i=0;i<34;i++)
    {
        obj->lverts[i].tu -= 0.06f;
    }

    for(j=0;j<num_targets;j++)
    {
        obj->pos.x = (D3DVALUE)target_position[j].x;
        obj->pos.y = (D3DVALUE)target_position[j].y;
        obj->pos.z = (D3DVALUE)target_position[j].z;

        RenderObject(obj);
    }

    GetExtents(&extents);
    AddUpdateRect(&extents, UPDATE_RESTORE);
    UpdateBoundingRect(&extents);

    obj->scl.x = (D3DVALUE)(scalesize / 1.25);
    obj->scl.y = (D3DVALUE)(scalesize / 1.25);
    obj->scl.z = (D3DVALUE)1.25 * zscalefactor;

    for(j=0;j<num_targets;j++)
    {
        obj->pos.x = (D3DVALUE)target_position[j].x;
        obj->pos.y = (D3DVALUE)target_position[j].y;
        obj->pos.z = (D3DVALUE)target_position[j].z;

        RenderObject(obj);
    }

//  GetExtents(&extents);
//  AddUpdateRect(&extents, UPDATE_RESTORE);
//  UpdateBoundingRect(&extents);

    obj->scl.x = (D3DVALUE)(scalesize / 1.5);
    obj->scl.y = (D3DVALUE)(scalesize / 1.5);
    obj->scl.z = (D3DVALUE)1.5 * zscalefactor;
    for(j=0;j<num_targets;j++)
    {
        obj->pos.x = (D3DVALUE)target_position[j].x;
        obj->pos.y = (D3DVALUE)target_position[j].y;
        obj->pos.z = (D3DVALUE)target_position[j].z;

        RenderObject(obj);
    }

//  GetExtents(&extents);
//  AddUpdateRect(&extents, UPDATE_RESTORE);
//  UpdateBoundingRect(&extents);

    obj->scl.x = (D3DVALUE)(scalesize / 1.75);
    obj->scl.y = (D3DVALUE)(scalesize / 1.75);
    obj->scl.z = (D3DVALUE)1.75 * zscalefactor;
    for(j=0;j<num_targets;j++)
    {
        obj->pos.x = (D3DVALUE)target_position[j].x;
        obj->pos.y = (D3DVALUE)target_position[j].y;
        obj->pos.z = (D3DVALUE)target_position[j].z;

        RenderObject(obj);
    }

    GetExtents(&extents);
    AddUpdateRect(&extents, UPDATE_RESTORE);
    UpdateBoundingRect(&extents);

//  obj->scl.x = (D3DVALUE)(scalesize / 2.0);
//  obj->scl.y = (D3DVALUE)(scalesize / 2.0);
//  obj->scl.z = (D3DVALUE)2.0;
//  RenderObject(obj);

//  GetExtents(&extents);
//  AddUpdateRect(&extents, UPDATE_RESTORE);
//  UpdateBoundingRect(&extents);

    Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullmode);

    RestoreBlendState();

    return TRUE;
}

void TQuicksandAnimator::RefreshZBuffer()
{
    int size_x = 640,
        size_y = 480;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2), screen.x + (size_x / 2), screen.y + (size_y / 2));
}

// ********************
// * Sandswirl Effect *
// ********************

DEFINE_BUILDER("Sandswirl", TSandswirlEffect)
REGISTER_BUILDER(TSandswirlEffect)

void TSandswirlEffect::Initialize()
{
}

void TSandswirlEffect::Pulse()
{
    TEffect::Pulse();   
}


// **********************
// * Sandswirl Animator *
// **********************

//#define SANDSWIRL_DURATION        50.0
#define SANDSWIRL_DURATION      75.0

REGISTER_3DANIMATOR("Sandswirl", TSandswirlAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the sandswirl effect
//==============================================================================

void TSandswirlAnimator::Initialize()
{
    T3DAnimator::Initialize();
    frameon = 0;
    angle = ((PTEffect)inst)->GetAngle();

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = OBJCLASS_EFFECT;

    def.objtype = EffectClass.FindObjType("sand");
    def.level = MapPane.GetMapLevel();
    S3DPoint pos;
    ((PTEffect)inst)->GetPos(pos);
    def.pos = pos;
    PTObjectInstance instance = MapPane.GetInstance(MapPane.NewObject(&def));
    if (!instance)
        return;

    instance->CreateAnimator();
    PTParticle3DAnimator anim = (PTParticle3DAnimator)instance->GetAnimator();
    particleanim = anim;

    //int ang = (angle + 128) & 0xff; // fly back
    int ang = angle;

    S3DPoint vect;
    ConvertToVector(ang, 100, vect);

    if (anim)
    {
        SParticleParams pr;

        pr.particles = random(50, 100);
        pr.pos.x = (D3DVALUE)0.0;
        pr.pos.y = (D3DVALUE)0.0;
        pr.pos.z = (D3DVALUE)70.0;
        pr.pspread.x = (D3DVALUE)50.0;
        pr.pspread.y = (D3DVALUE)50.0;
        pr.pspread.z = (D3DVALUE)50.0;
        pr.dir.x = (D3DVALUE)((float)vect.x / (float)100.0);
        pr.dir.y = (D3DVALUE)((float)vect.y / (float)100.0);
        pr.dir.z = (D3DVALUE)((float)vect.z / (float)100.0);
        pr.spread.x = (D3DVALUE)0.5;
        pr.spread.y = (D3DVALUE)0.5;
        pr.spread.z = (D3DVALUE)0.5;
        pr.gravity = (D3DVALUE)0.0;
        pr.trails = 1;
        pr.minstart = 0;
        pr.maxstart = pr.particles / 2;
        pr.minlife = 100;
        pr.maxlife = 150;
        pr.bounce = FALSE;
        pr.killobj = TRUE;
        pr.objflags = 0xF;

        pr.numtargets = 1;
        pr.targetpos[0].x = (D3DVALUE)0;
        pr.targetpos[0].y = (D3DVALUE)0;
        pr.targetpos[0].z = (D3DVALUE)100;
        //
        pr.seektargets = TRUE;
        pr.turnang = (float)0.5;
        pr.seekspeed = (float)5.0;
        pr.autorange = (float)0.0;
        pr.hitrange = (float)0.0;

        anim->InitParticles(&pr);
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
//
//==============================================================================

void TSandswirlAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    S3DPoint pos;

    frameon++;
    if (frameon < SANDSWIRL_DURATION)
    {
        SParticleParams pr;
        pr.numtargets = 1;
        pr.targetpos[0].x = (D3DVALUE)0;
        pr.targetpos[0].y = (D3DVALUE)0;
        pr.targetpos[0].z = (D3DVALUE)100;
        //
        pr.seektargets = TRUE;
        pr.turnang = (D3DVALUE)0.5;
        pr.seekspeed = (D3DVALUE)(5.0 + frameon / 10.0);
        pr.autorange = 0.0;
        pr.hitrange = 0.0;

        particleanim->ResetTargetInfo(pr);
    }
    else if (frameon == SANDSWIRL_DURATION)
    {
        SParticleParams pr;
        pr.numtargets = 0;

        int targets[MAXFOUNDOBJS];
        S3DPoint effect_pos;
        ((PTEffect)inst)->GetPos(effect_pos);
        //effect_pos.y -= length;
        PTObjectInstance caster = ((PTEffect)inst)->GetSpell()->GetInvoker();
        int num = MapPane.FindObjectsInRange(effect_pos, targets, 400, 0, OBJCLASS_CHARACTER);
        for (int i = 0; i < num && pr.numtargets < 5; i++)
        {
            PTObjectInstance instan = MapPane.GetInstance(targets[i]);
            if (instan && instan != caster && !((PTCharacter)instan)->IsDead()/* && !(instan->GetFlags() & OF_ICED)*/)
            {
                // create the quicksand effect
                SObjectDef def;
                memset(&def, 0, sizeof(SObjectDef));
                def.objclass = OBJCLASS_EFFECT;
                def.level = MapPane.GetMapLevel();
                S3DPoint putpos;
                ((PTCharacter)instan)->GetPos(putpos);
                def.pos = putpos;
                def.objtype = EffectClass.FindObjType("Quicksand");
                PTIcedEffect effect = (PTIcedEffect)MapPane.GetInstance(MapPane.NewObject(&def));

                // now quicksand them!
                if (!effect->HasAnimator())
                    effect->CreateAnimator();
                PTQuicksandAnimator qa = (PTQuicksandAnimator)effect->GetAnimator();
                if (qa)
                    qa->InitQuicksand(instan, (int)QUICKSAND_DURATION);

                pr.targetpos[pr.numtargets].x = (float)(putpos.x - effect_pos.x);
                pr.targetpos[pr.numtargets].y = (float)(putpos.y - effect_pos.y);
                pr.targetpos[pr.numtargets].z = (float)(putpos.z - effect_pos.z)  - 32;
                pr.numtargets++;
            }
        }
        //
        if (pr.numtargets == 0)
            pr.seektargets = FALSE;
        else
            pr.seektargets = TRUE;
        pr.turnang = (D3DVALUE)0.5;
        pr.seekspeed = (D3DVALUE)7.5;
        //pr.autorange = 25.0;
        //pr.hitrange = 10.0;
        pr.autorange = 0.0;
        pr.hitrange = 10.0;

        particleanim->ResetTargetInfo(pr);
    }
    //
    if (frameon == SANDSWIRL_DURATION * 4)
    {
        ((PTEffect)inst)->KillThisEffect();
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders sandswirl effect
//
//==============================================================================

BOOL TSandswirlAnimator::Render()
{

    /*SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;
    if (frameon < QUICKSAND_DURATION * 2)
    {
        obj = GetObject(0);
        // clear matrix and get ready to transform and render
        /*obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);       
        // set frame
        SetAnimFrame(stage, obj);
        
        obj->scl.x = (D3DVALUE)(scalesize);
        obj->scl.y = (D3DVALUE)(scalesize);
        obj->scl.z = 1.0;
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face correct way
        int ang = (int)(frameon * 360 / QUICKSAND_SWITCH) % 360;
        D3DMATRIXRotateZ(&obj->matrix, (D3DVALUE)(ang * TORADIAN));
        // translate to place should be
        obj->pos.x = (D3DVALUE)0;
        obj->pos.y = (D3DVALUE)0;
        obj->pos.z = (D3DVALUE)0;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        ResetExtents();
        RenderObject(obj);
        UpdateExtents();
    }
    RestoreBlendState();*/

    return TRUE;
}

void TSandswirlAnimator::RefreshZBuffer()
{
    /*int size_x = 100,
        size_y = 50;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2), size_x, size_y);*/
}

// ******************
// * Tornado Effect *
// ******************

#define TORNADO_MAXPARTICLES        100
#define TORNADO_DURATION            100
#define TORNADO_MODIFIER            200
#define TORNADO_MODIFIERV           100
#define TORNADO_MAXICECHUNKS        (5 * 6)
#define TORNADO_ICEGRAVITY          0.2
#define TORNADO_FLAMEUP             3
#define TORNADO_FLAMEMULTIPLIER     1
#define TORNADO_MAXFLAMES           (TORNADO_MAXICECHUNKS / 2)


DEFINE_BUILDER("Tornado", TTornadoEffect)
REGISTER_BUILDER(TTornadoEffect)

void TTornadoEffect::Initialize()
{
}

void TTornadoEffect::Pulse()
{
    TEffect::Pulse();
    // deal damage every T_D frames
    if(animator)
    {
        if (((PTTornadoAnimator)animator)->frameon % TORNADO_DURATION == 0)
        {
            if(spell)
            {
                PTObjectInstance invoker = spell->GetInvoker();
                S3DPoint pos;
                GetPos(pos);
                DamageCharactersInRange(invoker, pos, TORNADO_MODIFIER, spell->VariantData()->mindamage, spell->VariantData()->maxdamage, spell->SpellData()->damagetype);
            }
        }
    }
}

// ********************
// * Tornado Animator *
// ********************

/*

This could be a cure/restore spell

#define TORNADO_MAXPARTICLES    15
#define TORNADO_DURATION        15
#define TORNADO_MODIFIER        70
#define TORNADO_MODIFIERV       30

#define _INIT_TORNADO_PARTICLE(i)  {torn[i].scl = (float)(random(10, 15) / 100.0);  \
                                    torn[i].dscl = (float)(random(5, 10) / 100.0);  \
                                    torn[i].r = (float)0.0;                         \
                                    torn[i].dr = (float)(random(1, 10) / 100.0);    \
                                    torn[i].th = (float)(random(0, 359) * TORADIAN);\
                                    torn[i].dth = (float)(random(5, 15) / 100.0);   \
                                    torn[i].h = (float)0.0;                         \
                                    torn[i].dh = (float)(random(1, 10) / 100.0);        \
                                    torn[i].count = random(30, 40);}

*/

REGISTER_3DANIMATOR("Tornado", TTornadoAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Tornado
//==============================================================================

void TTornadoAnimator::InitTornadoParticle(int i)
{
    torn[i].scl = (float)(random(3, 7) / 100.0);
    torn[i].dscl = (float)(random(2, 6) / 1000.0);
    torn[i].r = (float)(random(0, 359) * TORADIAN);
    torn[i].dr = (float)0;
    torn[i].th = (float)(random(0, 359) * TORADIAN);
    torn[i].dth = (float)(random(1, 3) / 100.0);
    torn[i].h = (float)(random(0, 359) * TORADIAN);
    torn[i].dh = (float)(random(3, 5) / 100.0);
    torn[i].count = random(10, 20);
}

void TTornadoAnimator::InitFlameParticle(float x, float y, float z, float vx, float vy, float vz, int sc)
{
    flame[flameon].scl = (float)(random(1, 5));
    flame[flameon].dscl = (float)(sc * random(1, 5) / 10.0);
    flame[flameon].pos.x = x;
    flame[flameon].pos.y = y;
    flame[flameon].pos.z = z;
    flame[flameon].vel.x = vx;
    flame[flameon].vel.y = vy;
    flame[flameon].vel.z = vz;
    flameon++;
    if (flameon >= TORNADO_MAXFLAMES)
        flameon = 0;
}

void TTornadoAnimator::InitIceParticle(int i)
{
    ice[i].pos.x = (float)(random(-TORNADO_MODIFIER, TORNADO_MODIFIER));
    ice[i].pos.y = (float)(random(-TORNADO_MODIFIER, TORNADO_MODIFIER));
    ice[i].pos.z = (float)TORNADO_MODIFIERV * 2;
    ice[i].vel.x = (float)(random(0, TORNADO_MODIFIER) / 100.0);
    ice[i].vel.y = (float)(random(0, TORNADO_MODIFIER) / 100.0);
    ice[i].vel.z = (float)(random(-TORNADO_MODIFIERV, -TORNADO_MODIFIERV / 2) / 25.0);
    ice[i].th.x = (float)random(0, 6);
    ice[i].th.y = (float)random(0, 6);
    ice[i].th.z = (float)random(0, 6);
    ice[i].w.x = (float)(random(-50, 50) / 100.0);
    ice[i].w.y = (float)(random(-50, 50) / 100.0);
    ice[i].w.z = (float)(random(-50, 50) / 100.0);
    ice[i].scl.x = (float)(random(50, 100) / 100.0);
    ice[i].scl.y = (float)(random(50, 100) / 100.0);
    ice[i].scl.z = (float)(random(50, 100) / 100.0);
    ice[i].time = random(0, TORNADO_DURATION);
    ice[i].stage = 0;
}

void TTornadoAnimator::Initialize()
{
    int i;
    T3DAnimator::Initialize();
    frameon = 0;
    flameon = 0;

    PLAY("icestorm");

    torn = new STornadoParticle[TORNADO_MAXPARTICLES];
    ice = new SIceChunkParticle[TORNADO_MAXICECHUNKS];
    flame = new SFlameParticle[TORNADO_MAXFLAMES];
    for (i = 0; i < TORNADO_MAXPARTICLES; i++)
    {
        InitTornadoParticle(i);
    }

    for (i = 0; i < TORNADO_MAXICECHUNKS; i++)
    {
        ice[i].time = 1;
    }
    for (i = 0; i < TORNADO_MAXICECHUNKS; i += 6) // every sixth is an ice chunk, rest splash
    {
        InitIceParticle(i);
    }

    for (i = 0; i < TORNADO_MAXFLAMES; i++)
    {
        flame[i].scl = 0;
        flame[i].dscl = 0;
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the 
//
//==============================================================================

void TTornadoAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    int i, j, k;

    frameon++;

    BOOL done = TRUE;

    if (torn == NULL || ice == NULL || flame == NULL)
        return;

    for (i = 0; i < TORNADO_MAXPARTICLES; i++)
    {
        torn[i].pos.x = TORNADO_MODIFIER * (float)cos(torn[i].r) * (float)cos(torn[i].th);// * (1 + cos(torn[i].h));
        torn[i].pos.y = TORNADO_MODIFIER * (float)cos(torn[i].r) * (float)sin(torn[i].th);// * (1 + cos(torn[i].h));
        torn[i].pos.z = TORNADO_MODIFIERV * (1 + (float)cos(torn[i].h));

        torn[i].h += torn[i].dh;
        torn[i].r += torn[i].dr;
        torn[i].th += torn[i].dth;
        torn[i].count--;
        if (torn[i].count < 0)
            torn[i].scl -= torn[i].dscl;
        else
            torn[i].scl += torn[i].dscl;

        if (torn[i].r > M_PI)
            torn[i].r = (float)M_PI;
        if (torn[i].h > M_PI)
        {
            if (frameon < TORNADO_DURATION * 2)
            {
                InitTornadoParticle(i);
            }
            else
            {
                torn[i].h = (float)M_PI;
            }
        }
        else
            done = FALSE;
        if (torn[i].scl < 0)
        {
            if (frameon < TORNADO_DURATION * 2)
            {
                InitTornadoParticle(i);
            }
            else
            {
                torn[i].scl = 0;
            }
        }
        else
            done = FALSE;
        if (torn[i].th > M_2PI)
            torn[i].th -= (float)M_2PI;
    }

    for (i = 0; i < TORNADO_MAXICECHUNKS; i += 6) // chunks, not splashes
    {
        switch (ice[i].stage)
        {
        case 0:// falling stage
            if (ice[i].time)
                ice[i].time--;
            else
            {
                ice[i].pos.x += ice[i].vel.x;
                ice[i].pos.y += ice[i].vel.y;
                ice[i].pos.z += ice[i].vel.z;

                for (k = 0; k < TORNADO_FLAMEMULTIPLIER; k++)
                {
                    InitFlameParticle(ice[i].pos.x, ice[i].pos.y, ice[i].pos.z,
                        (float)(ice[i].vel.x * random(1, 9) / 10.0),
                        (float)(ice[i].vel.y * random(1, 9) / 10.0),
                        (float)(ice[i].vel.z * random(1, 9) / 10.0 + random(1, 4)), 1);
                }
                    
                ice[i].th.x += ice[i].w.x;
                if (ice[i].th.x > M_2PI)
                    ice[i].th.x -= (float)M_2PI;
                ice[i].th.y += ice[i].w.y;
                if (ice[i].th.y > M_2PI)
                    ice[i].th.y -= (float)M_2PI;
                ice[i].th.z += ice[i].w.z;
                if (ice[i].th.z > M_2PI)
                    ice[i].th.z -= (float)M_2PI;

                ice[i].vel.z -= (float)TORNADO_ICEGRAVITY;
                done = FALSE;
            }
            if (ice[i].pos.z < 40) // hit ground, explode, flame
            {
                ice[i].time = 10;
                ice[i].stage = 1;
                for (j = 1; j < 6; j++)
                {
                    ice[i + j].pos.x = (float)ice[i].pos.x;
                    ice[i + j].pos.y = (float)ice[i].pos.y;
                    ice[i + j].pos.z = (float)24;
                    ice[i + j].vel.x = (float)(random(-TORNADO_MODIFIER, TORNADO_MODIFIER) / 50.0);
                    ice[i + j].vel.y = (float)(random(-TORNADO_MODIFIER, TORNADO_MODIFIER) / 50.0);
                    ice[i + j].vel.z = (float)(random(0, TORNADO_MODIFIERV) / 15.0);
                    ice[i + j].th.x = (float)(random(0, 6));
                    ice[i + j].th.y = (float)(random(0, 6));
                    ice[i + j].th.z = (float)(random(0, 6));
                    ice[i + j].w.x = (float)(random(1, 5) / 100.0);
                    ice[i + j].w.y = (float)(random(1, 5) / 100.0);
                    ice[i + j].w.z = (float)(random(1, 5) / 100.0);
                    ice[i + j].scl.x = (float)(random(15, 35) / 100.0);
                    ice[i + j].scl.y = (float)(random(15, 35) / 100.0);
                    ice[i + j].scl.z = (float)(random(15, 35) / 100.0);
                    ice[i + j].time = 0;
                }
                // deal damage was here
            }
        break;
        case 1:// shattering, flaming stage
            if (ice[i].time)
            {
                done = FALSE;
                ice[i].time--;
                if (ice[i].time > 5)
                {
                    for (k = 0; k < TORNADO_FLAMEMULTIPLIER; k++)
                    {
                        // flame on explosion!
                        InitFlameParticle((float)(random(-10, 10) + ice[i].pos.x),
                            (float)(random(-10, 10) + ice[i].pos.y),
                            (float)(random(-10, 10) + ice[i].pos.z),
                            (float)(ice[i].vel.x * random(1, 9) / 10.0),
                            (float)(ice[i].vel.y * random(1, 9) / 10.0),
                            (float)(random(4, 10)), 1);
                    }
                }
            }
            else
            {
                ice[i].stage = 2;
                ice[i].time = 1;
            }
        break;
        case 2:
            // dead, reboot me stage
            if (frameon < TORNADO_DURATION * 2 - 20)
            {
                InitIceParticle(i);
                /*for (k = 0; k < TORNADO_FLAMEMULTIPLIER * 3; k++)
                {
                    InitFlameParticle(ice[i].pos.x, ice[i].pos.y, ice[i].pos.z,
                        (float)(ice[i].vel.x * random(1, 9) / 10.0),
                        (float)(ice[i].vel.y * random(1, 9) / 10.0),
                        (float)(ice[i].vel.z * random(1, 9) / 10.0), 1);
                }*/
            }
            else
            {
                ice[i].time = 1;
                ice[i].stage = 3;
            }
        break;
        }
    }

    for (i = 0; i < TORNADO_MAXICECHUNKS; i++) // skip chunks, rest splash
    {
        if (i % 6 == 0)
            continue;
        if (!ice[i].time)
        {
            ice[i].pos.x += ice[i].vel.x;
            ice[i].pos.y += ice[i].vel.y;
            ice[i].pos.z += ice[i].vel.z;
            
            ice[i].th.x += ice[i].w.x;
            if (ice[i].th.x > M_2PI)
                ice[i].th.x -= (float)M_2PI;
            ice[i].th.y += ice[i].w.y;
            if (ice[i].th.y > M_2PI)
                ice[i].th.y -= (float)M_2PI;
            ice[i].th.z += ice[i].w.z;
            if (ice[i].th.z > M_2PI)
                ice[i].th.z -= (float)M_2PI;

            ice[i].vel.z -= (float)TORNADO_ICEGRAVITY;

            if (ice[i].pos.z < 20)
                ice[i].time = TORNADO_DURATION * 2;
            else
                done = FALSE;
        }
    }

    for (i = 0; i < TORNADO_MAXFLAMES; i++)
    {
        flame[i].scl -= flame[i].dscl;
        if (flame[i].scl <= 0)
            continue;
        flame[i].pos.x += flame[i].vel.x;
        flame[i].pos.y += flame[i].vel.y;
        flame[i].pos.z += (float)(flame[i].vel.z + TORNADO_FLAMEUP);
    }

    if (frameon >= TORNADO_DURATION * 3 && done)
    {
        if (torn != NULL)
        {
            delete torn;
            torn = NULL;
        }
        if (ice != NULL)
        {
            delete ice;
            ice = NULL;
        }
        if (flame != NULL)
        {
            delete flame;
            flame = NULL;
        }
        ((PTEffect)inst)->KillThisEffect();
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders tornado
//
//==============================================================================

BOOL TTornadoAnimator::Render()
{
    if (torn == NULL || ice == NULL || flame == NULL)
        return FALSE;

    SaveBlendState();
    SetAddBlendState();

    PS3DAnimObj obj;
    int i;
    
//  int num = TORNADO_MAXPARTICLES;
//  if (frameon < TORNADO_DURATION)
        //num = (int)(frameon * TORNADO_MAXPARTICLES / TORNADO_DURATION);
//  if (frameon > TORNADO_DURATION * 2)
        //num = (int)((TORNADO_DURATION * 3 - frameon) * TORNADO_MAXPARTICLES / TORNADO_DURATION);

    ResetExtents();
    obj = GetObject(0);
    for (i = 0; i < TORNADO_MAXFLAMES; i++)
    {
        if (flame[i].scl <= 0)
            continue;

        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);       
                
        obj->scl.x = (D3DVALUE)(flame[i].scl);
        obj->scl.y = (D3DVALUE)(flame[i].scl);
        obj->scl.z = (D3DVALUE)(flame[i].scl);
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face screen
        D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
        D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
        D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
        // translate to place should be
        obj->pos.x = (D3DVALUE)flame[i].pos.x;
        obj->pos.y = (D3DVALUE)flame[i].pos.y;
        obj->pos.z = (D3DVALUE)flame[i].pos.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        RenderObject(obj);
    }
    UpdateExtents();

    SetBlendState();

    obj = GetObject(2);
    for (i = 0; i < TORNADO_MAXPARTICLES; i++)
    {
        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);       
                
        obj->scl.x = (D3DVALUE)(torn[i].scl);
        obj->scl.y = (D3DVALUE)(torn[i].scl);
        obj->scl.z = (D3DVALUE)(torn[i].scl);
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face screen
        D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
        D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
        D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
        // translate to place should be
        obj->pos.x = (D3DVALUE)torn[i].pos.x;
        obj->pos.y = (D3DVALUE)torn[i].pos.y;
        obj->pos.z = (D3DVALUE)torn[i].pos.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        ResetExtents();
        RenderObject(obj);
        UpdateExtents();
    }

    for (i = 0; i < TORNADO_MAXICECHUNKS; i++)
    {
        if (ice[i].time || ice[i].stage)
            continue;
        if (i % 6 == 0)
            obj = GetObject(3);
        else
            obj = GetObject(1);

        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);       
                
        obj->scl.x = (D3DVALUE)(ice[i].scl.x);
        obj->scl.y = (D3DVALUE)(ice[i].scl.y);
        obj->scl.z = (D3DVALUE)(ice[i].scl.z);
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face screen
        D3DMATRIXRotateX(&obj->matrix, ice[i].th.x);
        D3DMATRIXRotateY(&obj->matrix, ice[i].th.y);
        D3DMATRIXRotateZ(&obj->matrix, ice[i].th.z);
        // translate to place should be
        obj->pos.x = (D3DVALUE)ice[i].pos.x;
        obj->pos.y = (D3DVALUE)ice[i].pos.y;
        obj->pos.z = (D3DVALUE)ice[i].pos.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        ResetExtents();
        RenderObject(obj);
        UpdateExtents();
    }

    RestoreBlendState();

    return TRUE;
}

void TTornadoAnimator::RefreshZBuffer()
{
    int size_x = 640,
        size_y = 480;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2), size_x, size_y);
    /*for (i = 0; i < TORNADO_MAXFLAMES; i++)
    {
        if (flame[i].scl <= 0)
            continue;
    }*/
}

// ******************
// * Streamer Effect *
// ******************

DEFINE_BUILDER("Streamer", TStreamerEffect)
REGISTER_BUILDER(TStreamerEffect)

void TStreamerEffect::Initialize()
{
    first_time = TRUE;
}

void TStreamerEffect::Pulse()
{
    if(first_time)
    {
        if(spell)
        {
            if(spell->GetInvoker())
            {
                ((PTCharacter)spell->GetInvoker())->SetPoisoned(FALSE);
                first_time = FALSE;
            }
        }
    }
    TEffect::Pulse();
}

// ********************
// * Streamer Animator *
// ********************

#define STREAMER_MAXPARTICLES   50
#define STREAMER_DURATION       100
#define STREAMER_MODIFIER       40
#define STREAMER_MODIFIERV      50
#define STREAMER_SKIP           1

REGISTER_3DANIMATOR("Streamer", TStreamerAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Tornado
//==============================================================================

/*
if (h <= M_PI)
    modif = (float)(sqrt((M_PI * M_PI) - ((2.0 * h - M_PI) * (2.0 * h - M_PI))) / M_PI);
else
    modif = (float)(sqrt((M_PI * M_PI) - ((2.0 * (h - M_PI) - M_PI) * (2.0 * (h - M_PI) - M_PI))) / M_PI);

// good sphere
float modif = (float)0.5 * (1 + cos(2.0 * h + M_PI));\
*/

void TStreamerAnimator::InitStreamer(int x)
{
    D3DVECTOR pos;
    float modif = (float)(0.5 * (1 + cos(2.0 * h[x] + M_PI)));
    pos.x = (float)(STREAMER_MODIFIER * cos(th[x]) * modif * (x + 1) / STREAMER_MAXSTREAMS);
    pos.y = (float)(STREAMER_MODIFIER * sin(th[x]) * modif * (x + 1) / STREAMER_MAXSTREAMS);
    pos.z = (float)(STREAMER_MODIFIERV * (cos(h[x]) + 1));
    h[x] += dh[x];
    if (h[x] > M_PI)
    {
    //  h[x] -= M_PI;   
        frameon = STREAMER_DURATION + 1;
    }
    if (h[x] > M_2PI)
        h[x] -= (float)M_2PI;
    th[x] += dth[x];
    if (th[x] > M_2PI)
        th[x] -= (float)M_2PI;
    AddStreamer(x, pos, scl[x] * modif);
}

void TStreamerAnimator::Initialize()
{
    int i, j;
    T3DAnimator::Initialize();
    frameon = 0;
        
    for (j = 0; j < STREAMER_MAXSTREAMS; j++)
    {
        scl[j] = (float)0.15 * (j + 1);
        dscl[j] = (float)(scl[j] / STREAMER_MAXPARTICLES);
        h[j] = (float)0;
        dh[j] = (float)0.02 / (4 - j);
        th[j] = (float)0;//(M_2PI * j / STREAMER_MAXSTREAMS);
        dth[j] = (float)0.15;
        stream[j] = new SStreamerParticle[STREAMER_MAXPARTICLES];
        for (i = 0; i < STREAMER_MAXPARTICLES; i++)
        {
            stream[j][i].count = 0;
        }

        /*for (i = 0; i < STREAMER_MAXPARTICLES; i++)
        {
            InitStreamer(j);
        }*/
    }
}

void TStreamerAnimator::AddStreamer(int num, D3DVECTOR pos, D3DVALUE scl)
{
    if (stream[num] == NULL)
        return;
    int p = -1, i;
    for (i = 0; i < STREAMER_MAXPARTICLES; i++)
    {
        if (stream[num][i].count <= 0)
            p = i;
    }
    if (p > -1)
    {
        stream[num][p].pos.x = pos.x;
        stream[num][p].pos.y = pos.y;
        stream[num][p].pos.z = pos.z;
        stream[num][p].scl = scl;
        stream[num][p].count = STREAMER_MAXPARTICLES;
    }
    for (i = 0; i < STREAMER_MAXPARTICLES; i++)
    {
        stream[num][i].count--;
        stream[num][i].scl -= dscl[num];
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the 
//
//==============================================================================

void TStreamerAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    
    frameon++;

    int i, j;
    for (j = 0; j < STREAMER_MAXSTREAMS; j++)
    {
        for (i = 0; i < STREAMER_SKIP * (4 - j); i++)
        {
            InitStreamer(j);
        }
    }
    
    if (frameon > STREAMER_DURATION)
    {
        for (j = 0; j < STREAMER_MAXSTREAMS; j++)
        {
            delete stream[j];
            stream[j] = NULL;
        }
        ((PTEffect)inst)->KillThisEffect();
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders streamers
//
//==============================================================================

BOOL TStreamerAnimator::Render()
{
    if (stream[0] == NULL || stream[1] == NULL || stream[2] == NULL || stream[3] == NULL)
        return FALSE;

    SaveBlendState();
    SetAddBlendState();

    PS3DAnimObj obj;
    int i, j;
    
    for (j = 0; j < STREAMER_MAXSTREAMS; j++)
    {
        obj = GetObject(j);
        for (i = 0; i < STREAMER_MAXPARTICLES; i++)
        {
            if (stream[j][i].count <= 0 || stream[j][i].scl <= 0)
                continue;
            obj->flags = OBJ3D_MATRIX;
            D3DMATRIXClear(&obj->matrix);       
                    
            obj->scl.x = (D3DVALUE)(stream[j][i].scl);
            obj->scl.y = (D3DVALUE)(stream[j][i].scl);
            obj->scl.z = (D3DVALUE)(stream[j][i].scl);
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            // rotate to face screen
            D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
            D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
            D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
            D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
            // translate to place should be
            obj->pos.x = (D3DVALUE)stream[j][i].pos.x;
            obj->pos.y = (D3DVALUE)stream[j][i].pos.y;
            obj->pos.z = (D3DVALUE)stream[j][i].pos.z;
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            ResetExtents();
            RenderObject(obj);
            UpdateExtents();
        }
    }
    RestoreBlendState();

    return TRUE;
}

void TStreamerAnimator::RefreshZBuffer()
{
    int size_x = STREAMER_MODIFIER * 2 * STREAMER_MAXSTREAMS,
        size_y = STREAMER_MODIFIERV * 2 * STREAMER_MAXSTREAMS;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y + 20 - (size_y / 2), size_x, size_y);
}

// ******************
// * FireSwarm Effect *
// ******************

DEFINE_BUILDER("FireSwarm", TFireSwarmEffect)
REGISTER_BUILDER(TFireSwarmEffect)

void TFireSwarmEffect::Initialize()
{
}

void TFireSwarmEffect::Pulse()
{
    TEffect::Pulse();
}

// ********************
// * FireSwarm Animator *
// ********************

#define FIRESWARM_DURATION      75
#define FIRESWARM_CYLHSCLSTEP   0.4
#define FIRESWARM_CYLTHSTEP     0.5
#define FIRESWARM_CYLVSCLINIT   30
#define FIRESWARM_CYLVSCLSTEP   0.4

REGISTER_3DANIMATOR("FireSwarm", TFireSwarmAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the FireSwarm
//==============================================================================

void TFireSwarmAnimator::Initialize()
{
    T3DAnimator::Initialize();
    frameon = 0;
    cylth = (float)0;
    cylhscl = (float)FIRESWARM_CYLHSCLSTEP;
    cylvscl = (float)FIRESWARM_CYLVSCLINIT;
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the FireSwarm
//
//==============================================================================

void TFireSwarmAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    
    frameon++;

    cylth += (float)FIRESWARM_CYLTHSTEP;
    if (cylth > M_2PI)
        cylth -= (float)M_2PI;
    cylhscl += (float)FIRESWARM_CYLHSCLSTEP;
    cylvscl -= (float)FIRESWARM_CYLVSCLSTEP;
    if (frameon > FIRESWARM_DURATION)
    {
        ((PTEffect)inst)->KillThisEffect();
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders FireSwarm
//
//==============================================================================

BOOL TFireSwarmAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;
        
    obj = GetObject(1);
    obj->flags = OBJ3D_MATRIX;
            D3DMATRIXClear(&obj->matrix);       
                    
    obj->scl.x = (D3DVALUE)(cylhscl);
    obj->scl.y = (D3DVALUE)(cylhscl);
    obj->scl.z = (D3DVALUE)(cylvscl);
    D3DMATRIXScale(&obj->matrix, &obj->scl);
    // rotate to face screen
    D3DMATRIXRotateZ(&obj->matrix, cylth);

    ResetExtents();
    RenderObject(obj);
    UpdateExtents();
        
    RestoreBlendState();

    return TRUE;
}

void TFireSwarmAnimator::RefreshZBuffer()
{
    int size_x = 50,
        size_y = 50;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    //RestoreZ(screen.x - (size_x / 2), screen.y + 20 - (size_y / 2), size_x, size_y);
}

// ******************
// * Halo Effect *
// ******************

DEFINE_BUILDER("Halo", THaloEffect)
REGISTER_BUILDER(THaloEffect)

void THaloEffect::Initialize()
{
}

void THaloEffect::Pulse()
{
    TEffect::Pulse();
}

// ********************
// * Halo Animator *
// ********************

REGISTER_3DANIMATOR("Halo", THaloAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Halo
//==============================================================================

void THaloAnimator::Initialize()
{
    T3DAnimator::Initialize();
    haloscale = (float)0;
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the FireSwarm
//
//==============================================================================

void THaloAnimator::Animate(BOOL draw)
{
    int totframes = ((PTHaloEffect)inst)->GetTotalFrames();
    float halostep = ((PTHaloEffect)inst)->GetHaloStep();
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    
    if (frameon < totframes / 2)
        haloscale += halostep;
    else
        haloscale -= halostep;
    frameon++;

    if (frameon > totframes)
    {

        ((PTEffect)inst)->KillThisEffect();
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders FireSwarm
//
//==============================================================================

BOOL THaloAnimator::Render()
{
    SaveBlendState();
    SetAddBlendState();

    PS3DAnimObj obj = GetObject(0);

    if (haloscale > 0)
    {
        ResetExtents();
        // clear matrix and get ready to transform and render
        D3DMATRIXClear(&obj->matrix);
        obj->flags = OBJ3D_MATRIX;
        // scale to size of spark
        obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)haloscale;
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face screen
        D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
        //D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
        D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        // translate to place should be
        obj->pos.x = (D3DVALUE)-5.0f;
        obj->pos.y = (D3DVALUE)-5.0f;
        obj->pos.z = (D3DVALUE)5.0f;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);
        // fix for direction
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
        //D3DMATRIXRotateY(&obj->matrix, (float)(M_PI / 8.0));
        RenderObject(obj);
        UpdateExtents();
    }
        
    RestoreBlendState();

    return TRUE;
}

void THaloAnimator::RefreshZBuffer()
{
    int size_x = (int)(haloscale * 20),
        size_y = (int)(haloscale * 20);
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y + 20 - (size_y / 2), size_x, size_y);
}

// *****************
// * Ripple Effect *
// *****************

DEFINE_BUILDER("Ripple", TRippleEffect)
REGISTER_BUILDER(TRippleEffect)

void TRippleEffect::Initialize()
{
}

void TRippleEffect::Pulse()
{
    TEffect::Pulse();
}

// *******************
// * Ripple Animator *
// *******************

#define RIPPLE_SMALLDURATION    24
#define RIPPLE_GRAVITY          0.37

REGISTER_3DANIMATOR("Ripple", TRippleAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Ripple Effect
//==============================================================================

void TRippleAnimator::Initialize()
{
    T3DAnimator::Initialize();
    frameon = 0;
    ripframe = 0;
    scale = (float)0.5;
    length = ((PTRippleEffect)inst)->GetLength();
    
    PS3DAnimObj o = GetObject(0);
    GetVerts(o, D3DVT_LVERTEX);

    hassplashed = FALSE;
    numdrops = 0;
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the Ripple
//
//==============================================================================

void TRippleAnimator::AddNewRipple(int x, int y, int z, int len)
{
    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = OBJCLASS_EFFECT;
    def.objtype = -1;
    def.level = MapPane.GetMapLevel();

    PTEffect effect;
    def.pos.x = x;
    def.pos.y = y;
    def.pos.z = z;
    def.objtype = EffectClass.FindObjType("ripple");
    def.facing = 0;

    effect = (PTEffect)MapPane.GetInstance(MapPane.NewObject(&def));
    effect->SetSpell(((PTEffect)inst)->GetSpell());
    if (effect)
        ((PTRippleEffect)effect)->SetLength(len);
}

void TRippleAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    S3DPoint pos;
    ((PTEffect)inst)->GetPos(pos);
    int i;

    frameon++;
    if (frameon % 2 == 0)
    {
        if (frameon <= length - 24)
        {
            ripframe++;
            if (ripframe == 4)
                ripframe = 0;
        }
        else
        {
            if (frameon > length - 24 && ripframe < 4)
            {
                if (length > 24)
                    ripframe = 4;
                else
                    ripframe = 4 + (24 - length) / 2;
            }
            else
            {
                ripframe++;
                if (ripframe > 15)
                    ripframe = 15;
            }
        }
    }
    scale += (float)(1.0 / 16.0);
    
    if (frameon > length && ripframe == 15)
    {
        inst->SetFlags(OF_KILL);  // Causes object to commit suicide
    }

    if (!hassplashed && length > RIPPLE_SMALLDURATION)
    {
        hassplashed = TRUE;
        numdrops = random(2, 3);
        drops = new SDropParticle[numdrops];

        for (i = 0; i < numdrops; i++)
        {
            drops[i].vel.x = (float)(random(-length, length) / 32.0);
            drops[i].vel.y = (float)(random(-length, length) / 32.0);
            drops[i].vel.z = (float)(random(length / 2, length) / 20.0);
            drops[i].pos.x = 0;
            drops[i].pos.y = 0;
            drops[i].pos.z = 5;
            drops[i].dead = FALSE;
        }
    }
    if (hassplashed)
    {
        for (i = 0; i < numdrops; i++)
        {
            if (drops[i].dead)
                continue;
            drops[i].pos.x += drops[i].vel.x;
            drops[i].pos.y += drops[i].vel.y;
            drops[i].pos.z += drops[i].vel.z;
            drops[i].vel.z -= (float)RIPPLE_GRAVITY;
            if (drops[i].pos.z <= 0)
            {
                AddNewRipple(int(drops[i].pos.x + pos.x), int(drops[i].pos.y + pos.y),
                    int(pos.z), (int)(length / 2));
                drops[i].dead = TRUE;
            }
        }
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders quicksand
//
//==============================================================================

void TRippleAnimator::SetAnimFrame(int frame_num, PS3DAnimObj obj)
{
    if (!obj->lverts)
        return;
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

char rippleframeof[16] = {3, 2, 1, 0, 7, 6, 5, 4, 11, 10, 9, 8, 15, 14, 13, 12 };

BOOL TRippleAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;
    // render ripple
    obj = GetObject(0);

    if (scale > 0)
    {
        // clear matrix and get ready to transform and render
        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);       
        // set frame
        SetAnimFrame(rippleframeof[15 - ripframe], obj);
        
        obj->scl.x = (D3DVALUE)(scale);
        obj->scl.y = (D3DVALUE)(scale);
        obj->scl.z = 1.0;
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face correct way
        
        // translate to place should be
        obj->pos.x = (D3DVALUE)0;
        obj->pos.y = (D3DVALUE)0;
        obj->pos.z = (D3DVALUE)0;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        ResetExtents();
        RenderObject(obj);
        UpdateExtents();
    }

    if (hassplashed)
    {
        obj = GetObject(1);
        for (int i = 0; i < numdrops; i++)
        {
            if (drops[i].dead)
                continue;
            // clear matrix and get ready to transform and render
            obj->flags = OBJ3D_MATRIX;
            D3DMATRIXClear(&obj->matrix);       
                        
            obj->scl.x = (D3DVALUE)(0.25);
            obj->scl.y = (D3DVALUE)(0.25);
            obj->scl.z = (D3DVALUE)(0.25);
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            // rotate to face correct way
            
            // translate to place should be
            obj->pos.x = (D3DVALUE)drops[i].pos.x;
            obj->pos.y = (D3DVALUE)drops[i].pos.y;
            obj->pos.z = (D3DVALUE)drops[i].pos.z;
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            ResetExtents();
            RenderObject(obj);
            UpdateExtents();            
        }
    }

    RestoreBlendState();

    return TRUE;
}

void TRippleAnimator::RefreshZBuffer()
{
    int size_x = int(50 * scale),
        size_y = int(25 * scale);
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2), size_x, size_y);
}

// ***************
// * Drip Effect *
// ***************

DEFINE_BUILDER("Drip", TDripEffect)
REGISTER_BUILDER(TDripEffect)

void TDripEffect::Initialize()
{
}

void TDripEffect::Pulse()
{
    TEffect::Pulse();
}

void TDripEffect::Load(RTInputStream is, int version, int objversion)
{
    TObjectInstance::Load(is, version, objversion);
    is >> ripplesize >> height >> period;
}

void TDripEffect::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
    os << ripplesize << height << period;
}

// *****************
// * Drip Animator *
// *****************

REGISTER_3DANIMATOR("Drip", TDripAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Drip Effect
//==============================================================================

void TDripAnimator::Initialize()
{
    T3DAnimator::Initialize();
    dead = TRUE;
    time = 0;

    int ri, he, pe;
    ((PTDripEffect)inst)->GetParams(&ri, &he, &pe);

    if (ri == 0 || he == 0 || pe == 0)
        ((PTDripEffect)inst)->SetParams(ripplesize, height, period);
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the Ripple
//
//==============================================================================

void TDripAnimator::AddNewRipple(int x, int y, int z, int len)
{
    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = OBJCLASS_EFFECT;
    def.objtype = -1;
    def.level = MapPane.GetMapLevel();

    PTEffect effect;
    def.pos.x = x;
    def.pos.y = y;
    def.pos.z = z;
    def.objtype = EffectClass.FindObjType("ripple");
    def.facing = 0;

    effect = (PTEffect)MapPane.GetInstance(MapPane.NewObject(&def));
    //effect->SetSpell(((PTEffect)inst)->GetSpell());
    if (effect)
        ((PTRippleEffect)effect)->SetLength(len);
}

void TDripAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    S3DPoint effectpos;
    ((PTEffect)inst)->GetPos(effectpos);
    ((PTDripEffect)inst)->GetParams(&ripplesize, &height, &period);
    
    if (dead)
    {
        time++;
        if (time > period && !random(0, period / 2))
        {
            time = 0;
            vel.x = 0.0f;
            vel.y = 0.0f;
            vel.z = -(float)(RIPPLE_GRAVITY * 5);
            pos.x = 0;
            pos.y = 0;
            pos.z = (float)height;
            dead = FALSE;
        }
    }
    if (!dead)
    {
        pos.x += vel.x;
        pos.y += vel.y;
        pos.z += vel.z;
        vel.z -= (float)RIPPLE_GRAVITY;
        if (pos.z <= 0)
        {
            AddNewRipple(int(pos.x + effectpos.x), int(pos.y + effectpos.y),
                int(effectpos.z), ripplesize);
            dead = TRUE;
            PLAY("drip");
        }
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders Drip
//
//==============================================================================

BOOL TDripAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;
    
    if (!dead)
    {
        obj = GetObject(0);
        // clear matrix and get ready to transform and render
        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);       
                    
        obj->scl.x = (D3DVALUE)(0.3);
        obj->scl.y = (D3DVALUE)(0.3);
        obj->scl.z = (D3DVALUE)(0.3);
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face correct way
        
        // translate to place should be
        obj->pos.x = (D3DVALUE)pos.x;
        obj->pos.y = (D3DVALUE)pos.y;
        obj->pos.z = (D3DVALUE)pos.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        ResetExtents();
        RenderObject(obj);
        UpdateExtents();
    }

    RestoreBlendState();

    return TRUE;
}

void TDripAnimator::RefreshZBuffer()
{
    int size_x = 10,
        size_y = height + 10;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - size_y, size_x, size_y);
}

// ******************
// * FaultFire Effect *
// ******************

DEFINE_BUILDER("FaultFire", TFaultFireEffect)
REGISTER_BUILDER(TFaultFireEffect)

void TFaultFireEffect::Initialize()
{
}

void TFaultFireEffect::Pulse()
{
    TEffect::Pulse();
}

// ********************
// * FaultFire Animator *
// ********************

#define FF_STEP     0.1

REGISTER_3DANIMATOR("FaultFire", TFaultFireAnimator)

void TFaultFireAnimator::SetupObjects()
{
    PS3DAnimObj o = NewObject(0);
    GetVerts(o, D3DVT_VERTEX);
    AddObject(o);
    for (int v = 0; v < o->numverts; v++)
    {
        o->verts[v].tv -= 0.01f;
        tvs[v] = o->verts[v].tv;
    }
}

void TFaultFireAnimator::Initialize()
{
    T3DAnimator::Initialize();
    th = 0.0f;
}

void TFaultFireAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    
    th += (float)FF_STEP;
    if (th > M_2PI)
        th -= (float)M_2PI;

    float du = (float)(random(2, 8) / 100.0f);
    PS3DAnimObj obj = GetObject(0);
    for (int n = 0; n < obj->numverts; n++)
    {
        obj->verts[n].tu += du;
    }
}

BOOL TFaultFireAnimator::Render()
{
    
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj = GetObject(0);

    ResetExtents();
    for (int i = 0; i < 2; i++)
    {
        float scale = (float)(0.125 * (cos(th + ((float)(i * M_PI / 2))) + 7));
        for (int n = 0; n < obj->numverts; n++)
        {
            obj->verts[n].tv = tvs[n] * scale;
        }
        // clear matrix and get ready to transform and render
        D3DMATRIXClear(&obj->matrix);
        obj->flags = OBJ3D_MATRIX | OBJ3D_VERTS;
        // scale to size of spark
        obj->scl.x = obj->scl.y = (D3DVALUE)0.25;
        obj->scl.z = (float)(4.0 * 0.25);
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face screen
        //DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 2.0));
        //D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 6.0));
        D3DMATRIXRotateZ(&obj->matrix, (float)(M_PI / 2.0));
        //D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));
        // translate to place should be
        obj->pos.x = 0.0f;
        obj->pos.y = 0.0f;
        obj->pos.z = 32.0f;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);
        // fix for direction
        //D3DMATRIXRotateY(&obj->matrix, (float)(M_PI / 8.0));
        RenderObject(obj);
    }
    UpdateExtents();
        
    RestoreBlendState();

    return TRUE;
}

void TFaultFireAnimator::RefreshZBuffer()
{
    int size_x = 32,
        size_y = 128;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - size_y, size_x, size_y);
}

// *********
// * Blood *
// *********

DEFINE_BUILDER("Blood", TBloodEffect)
REGISTER_BUILDER(TBloodEffect)

void TBloodEffect::Initialize()
{
}

void TBloodEffect::Pulse()
{
    TEffect::Pulse();
}



// *****************
// * Blood Animator *
// *****************

REGISTER_3DANIMATOR("Blood", TBloodAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the blood
//==============================================================================

void TBloodAnimator::Initialize()
{
    T3DAnimator::Initialize();
    S3DPoint zbuf, effectpos;

    SBloodSystemParams me;
    ((PTBloodEffect)inst)->GetParams(&me.height, &me.hangle, &me.vangle, &me.hspread, &me.vspread, &me.num);

    inst->GetPos(effectpos);

    zbuf.x = 10;
    zbuf.y = 10;
    zbuf.z = 0;

    me.a = this;
    me.s = GetObject(4);
    me.m = GetObject(5);
    me.b = GetObject(6);
    me.sp = GetObject(7);
    me.s2 = GetObject(0);
    me.m2 = GetObject(1);
    me.b2 = GetObject(2);
    me.sp2 = GetObject(3);
    me.maxsize = 2;
    me.zbuf = zbuf;
    me.effectpos = effectpos;

    bloods.Init(me);
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the blood
//
//==============================================================================

void TBloodAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    bloods.Animate();

    if (bloods.GetDone())
    {
        inst->SetFlags(OF_KILL);
    }   
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders blood
//
//==============================================================================

BOOL TBloodAnimator::Render()
{
    SaveBlendState();
    
    bloods.Render();

    RestoreBlendState();

    return TRUE;
}

void TBloodAnimator::RefreshZBuffer()
{
    bloods.RefreshZBuffer();
}

// ***************
// * Mist Effect *
// ***************

DEFINE_BUILDER("Mist", TMistEffect)
REGISTER_BUILDER(TMistEffect)

void TMistEffect::Initialize()
{
}

void TMistEffect::Pulse()
{
    TEffect::Pulse();
}

// *****************
// * Mist Animator *
// *****************

REGISTER_3DANIMATOR("Mist", TMistAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Mist Effect
//==============================================================================

#define MIST_MAXDROPS       50
#define MIST_LENGTH         64
#define MIST_WIDTH          20
#define MIST_HEIGHT         4
#define MIST_SCALE          0.7

void TMistAnimator::Initialize()
{
    T3DAnimator::Initialize();
    numdrops = MIST_MAXDROPS;
    drops = new SDropParticle[numdrops];
    int i;

    for (i = 0; i < numdrops; i++)
    {
        drops[i].vel.x = (float)(random(-MIST_LENGTH, MIST_LENGTH) / 32.0);
        drops[i].vel.y = (float)(random(-MIST_WIDTH, MIST_WIDTH) / 32.0);
        drops[i].vel.z = (float)(random(MIST_HEIGHT / 2, MIST_HEIGHT));
        drops[i].pos.x = (float)(random(-MIST_LENGTH, MIST_LENGTH) / 2.0);
        drops[i].pos.y = (float)(random(-MIST_WIDTH, MIST_WIDTH) / 2.0);
        drops[i].pos.z = 5;
        drops[i].dead = FALSE;
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the Mist
//
//==============================================================================

void TMistAnimator::AddNewRipple(int x, int y, int z, int len)
{
    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));
    def.objclass = OBJCLASS_EFFECT;
    def.objtype = -1;
    def.level = MapPane.GetMapLevel();

    PTEffect effect;
    def.pos.x = x;
    def.pos.y = y;
    def.pos.z = z;
    def.objtype = EffectClass.FindObjType("ripple");
    def.facing = 0;

    effect = (PTEffect)MapPane.GetInstance(MapPane.NewObject(&def));
    //effect->SetSpell(((PTEffect)inst)->GetSpell());
    if (effect)
        ((PTRippleEffect)effect)->SetLength(len);
}

void TMistAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    int i;
    S3DPoint effectpos;
    ((PTEffect)inst)->GetPos(effectpos);

    for (i = 0; i < numdrops; i++)
    {
        if (drops[i].dead)
        {
            drops[i].vel.x = (float)(random(-MIST_LENGTH, MIST_LENGTH) / 32.0);
            drops[i].vel.y = (float)(random(-MIST_WIDTH, MIST_WIDTH) / 32.0);
            drops[i].vel.z = (float)(random(MIST_HEIGHT / 2, MIST_HEIGHT));
            drops[i].pos.x = (float)(random(-MIST_LENGTH, MIST_LENGTH) / 2.0);
            drops[i].pos.y = (float)(random(-MIST_WIDTH, MIST_WIDTH) / 2.0);
            drops[i].pos.z = 5;
            drops[i].dead = FALSE;
        }
        drops[i].pos.x += drops[i].vel.x;
        drops[i].pos.y += drops[i].vel.y;
        drops[i].pos.z += drops[i].vel.z;
        drops[i].vel.z -= (float)RIPPLE_GRAVITY;
        if (drops[i].pos.z <= 0)
        {
            drops[i].dead = TRUE;
            //AddNewRipple(int(pos.x + effectpos.x), int(pos.y + effectpos.y), (int)effectpos.z, 16);
        }
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders Mist
//
//==============================================================================

BOOL TMistAnimator::Render()
{
    SaveBlendState();
    SetAddBlendState();

    PS3DAnimObj obj;
    obj = GetObject(0);
    ResetExtents();

    for (int i = 0; i < numdrops; i++)
    {
        if (drops[i].dead)
            continue;
        // clear matrix and get ready to transform and render
        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);
        
        obj->scl.x = (D3DVALUE)MIST_SCALE;
        obj->scl.y = (D3DVALUE)MIST_SCALE;
        obj->scl.z = (D3DVALUE)MIST_SCALE;
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        
        // rotate to face correct way
        D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));

        // translate to place should be
        obj->pos.x = (D3DVALUE)drops[i].pos.x;
        obj->pos.y = (D3DVALUE)drops[i].pos.y;
        obj->pos.z = (D3DVALUE)drops[i].pos.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        RenderObject(obj);
    }

    UpdateExtents();
    RestoreBlendState();

    return TRUE;
}

void TMistAnimator::RefreshZBuffer()
{
    int size_x = 150,
        size_y = 150;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2), size_x, size_y);
}

// **********************
// * TMistFogAnimator *
// **********************

#define NUMMISTFOG      25

_CLASSDEF(TMistFogAnimator)
class TMistFogAnimator : public T3DAnimator
{
  public:
    int ticks;
    float centerx, centery;
    float centervx, centervy;
    SSmoke smoke[NUMMISTFOG];

    TMistFogAnimator(PTObjectInstance oi) : T3DAnimator(oi) { }
    virtual ~TMistFogAnimator() { Close(); }

    virtual void Initialize();
      // Initializes velocity vectors and positions

    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
    void ResetBall(int b);
};

void TMistFogAnimator::ResetBall(int b)
{
    SSmoke *s = &smoke[b];
    s->x = (float)(random(-32, 32)) + centerx;
    s->y = (float)centery;
    s->z = 0;
    s->rot = 0;
    s->vx = (((float)rand() / (float)RAND_MAX) * 0.5f - 0.25f);
    s->vy = (((float)rand() / (float)RAND_MAX) * 0.5f - 0.25f);
    s->vz = (((float)rand() / (float)RAND_MAX) * 0.5f);
    s->life = random(0, 200);//(int)(((float)rand() / (float)RAND_MAX) * 50.0f);
    s->size = ((float)rand() / (float)RAND_MAX) * 2.7f + 0.1f;
}

REGISTER_3DANIMATOR("MistFog", TMistFogAnimator)

void TMistFogAnimator::Initialize()
{
    int i;
    centerx = centery = 0.0f;
    centervx = centervy = 0.2f;

    T3DAnimator::Initialize();

    for (i = 0; i < NUMMISTFOG; i++)
        ResetBall(i);

//  inst->Face( -32);

    ticks = 0;
}

void TMistFogAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    /*centerx += centervx;
    centery += centervy;

    if (centerx > 50 || centerx < -50)
        centervx = -centervx;
    if (centery > 50 || centery < -50)
        centervy = -centervy;*/
    
    SSmoke *s = smoke;  
    for (int i = 0; i < NUMMISTFOG; i++, s++)
    {
        s->vz += SMOKE_GRAV;
        s->x += s->vx;
        s->y += s->vy;
        if (s->z > 0)
            s->z += s->vz;
        else
            s->z -= 0.008f;

        s->life++;
        s->size += 0.02f;

        if (s->life > 200)
        {
            s->size -= 0.2f;
            if (s->size <= 0.01f)
                ResetBall(i);
        }
    }

    ticks++;
}

BOOL TMistFogAnimator::Render()
{
    SaveBlendState();
    SetAddBlendState();

    PS3DAnimObj obj;

    // Draw the cylindrical glow around the player
    obj = GetObject(0);

    ResetExtents();             // Reset render extents

    obj->flags = OBJ3D_SCL1 | OBJ3D_POS2;

    for (int i = 0; i < NUMMISTFOG; i++)
    {
        SSmoke s = smoke[i];
        obj->scl.x = obj->scl.y = s.size;
        obj->scl.z = s.size;
        obj->pos.x = s.x;
        obj->pos.y = s.y;
        obj->pos.z = s.z;

        RenderObject(obj);
    }

    UpdateExtents();            // Updates bound rect and screen rect

    RestoreBlendState();

    return TRUE;

}

void TMistFogAnimator::RefreshZBuffer()
{
    S3DPoint map, effect, screen, size;

    size.x = 180;
    size.y = 150;

    ((PTEffect)inst)->GetPos(effect);

    map.x = effect.x;
    map.y = effect.y;
    map.z = effect.z;

    WorldToScreen(map, screen);
    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2) - 40, size.x, size.y);
}

// ***************
// * WaterFall Effect *
// ***************

DEFINE_BUILDER("WaterFall", TWaterFallEffect)
REGISTER_BUILDER(TWaterFallEffect)

void TWaterFallEffect::Initialize()
{
}

void TWaterFallEffect::Pulse()
{
    TEffect::Pulse();
}

// *****************
// * WaterFall Animator *
// *****************

REGISTER_3DANIMATOR("WaterFall", TWaterFallAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Water Effect
//==============================================================================

#define WATERFALL_MAXDROPS      100
#define WATERFALL_LENGTH        64
#define WATERFALL_WIDTH         0
#define WATERFALL_HEIGHT        64
#define WATERFALL_SCALE         0.1f
#define WATERFALL_GRAVITY       0.25f
#define WATERFALL_TALLSCALE     2.0f

void TWaterFallAnimator::InitParticle(int i)
{
    float grav = (float)(WATERFALL_GRAVITY * ((i % 2) + 1));
    drops[i].pos.x = (float)(random(-WATERFALL_LENGTH, WATERFALL_LENGTH) / 2.0);
    drops[i].pos.y = (float)0.0f;//(random(-WATER_WIDTH, WATER_WIDTH) / 2.0);
    drops[i].pos.z = (float)WATERFALL_HEIGHT;//(i * WATER_HEIGHT / WATER_MAXDROPS);//(random(0, WATER_HEIGHT));
    drops[i].vel.x = (float)0.0f;
    drops[i].vel.y = (float)0.0f;
    drops[i].vel.z = (float)0.0f;//(-WATER_GRAVITY * sqrt(2 * (WATER_HEIGHT - drops[i].pos.z) / WATER_GRAVITY));
    drops[i].scale.x = (float)(random(2, 4) * WATERFALL_SCALE);
    drops[i].scale.y = (float)(WATERFALL_TALLSCALE * grav * sqrt(2 * (WATERFALL_HEIGHT - drops[i].pos.z) / grav));
    //(random(27, 30) * WATER_SCALE);
    drops[i].scale.z = (float)1.0f;
    drops[i].time = i;
}

void TWaterFallAnimator::UpdateStuff()
{
    int i;
    for (i = 0; i < numdrops; i++)
    {
        if (drops[i].time > 0)
        {
            drops[i].time--;
            continue;
        }
        if (drops[i].time == -1)
        {
            InitParticle(i);
            drops[i].pos.z = (float)WATERFALL_HEIGHT;
            drops[i].vel.z = 0.0f;
            drops[i].time = 0;
        }
        drops[i].pos.x += drops[i].vel.x;
        drops[i].pos.y += drops[i].vel.y;
        drops[i].pos.z += drops[i].vel.z;
        float grav = (float)(WATERFALL_GRAVITY * ((i % 2) + 1));
        drops[i].vel.z -= grav;
        drops[i].scale.y = (float)(WATERFALL_TALLSCALE * grav * sqrt(2 * (WATERFALL_HEIGHT - drops[i].pos.z) / grav));
        if (drops[i].pos.z <= 0)
        {
            drops[i].time = -1;
        }
    }
}

void TWaterFallAnimator::Initialize()
{
    T3DAnimator::Initialize();
    PS3DAnimObj o = GetObject(0);
    GetVerts(o, D3DVT_LVERTEX);

    inst->GetPos(eff);
    numdrops = WATERFALL_MAXDROPS;
    drops = new SWaterParticle[numdrops];
    int i;

    for (i = 0; i < numdrops; i++)
    {
        InitParticle(i);
    }
    // now run it through a couple hundred times
    for (int k = 0; k < numdrops; k++)
    {
        UpdateStuff();
    }
}

void TWaterFallAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    UpdateStuff();
}

void TWaterFallAnimator::DoLighting(float x, float y, float z, PS3DAnimObj object)
{
    float r, g, b, temp;
    r = g = b = 0;
    int l1, l2, l3, tx, ty, tz;
    tx = (int)(eff.x + x);
    ty = (int)(eff.y + y);
    tz = (int)(eff.z + z);
    Scene3D.GetClosestLights(tx, ty, tz, l1, l2, l3);
    if (l1 > -1)
    {
        Scene3D.GetLightBrightness(l1, tx, ty, tz, (float)temp);
        r += temp;
    }
    if (l2 > -1)
    {
        Scene3D.GetLightBrightness(l2, tx, ty, tz, (float)temp);
        r += temp;
    }
    if (l3 > -1)
    {
        Scene3D.GetLightBrightness(l3, tx, ty, tz, (float)temp);
        r += temp;
    }
    temp = (float)(MapPane.GetAmbientLight() / 255.0f);
    r += temp;
    if (r > 4.0)
        r = 4.0f;
    r *= 0.25f;
    g = b = r;
    for(int j = 0; j < 4; j++)
    {
        object->lverts[j].color = D3DRGB(r, g, b);
    }
}

BOOL TWaterFallAnimator::Render()
{
    SaveBlendState();
    //SetAddBlendState();
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE));

    PS3DAnimObj obj;
    obj = GetObject(0);
    ResetExtents();

    for (int i = 0; i < numdrops; i++)
    {
        if (drops[i].time != 0)
            continue;
        // clear matrix and get ready to transform and render
        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);
        
        obj->scl.x = (D3DVALUE)drops[i].scale.x;
        obj->scl.y = (D3DVALUE)drops[i].scale.y;
        obj->scl.z = (D3DVALUE)drops[i].scale.z;
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        
        // rotate to face correct way
        //D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));       
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));

        // translate to place should be
        obj->pos.x = (D3DVALUE)drops[i].pos.x;
        obj->pos.y = (D3DVALUE)drops[i].pos.y;
        obj->pos.z = (D3DVALUE)drops[i].pos.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        DoLighting(drops[i].pos.x, drops[i].pos.y, drops[i].pos.z, obj);

        RenderObject(obj);
    }

    UpdateExtents();
    RestoreBlendState();

    return TRUE;
}

void TWaterFallAnimator::RefreshZBuffer()
{
    int size_x = 150,
        size_y = 150;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2) - 30, size_x, size_y);
}

// ***************
// * Water Effect *
// ***************

DEFINE_BUILDER("Water", TWaterEffect)
REGISTER_BUILDER(TWaterEffect)

void TWaterEffect::Initialize()
{
}

void TWaterEffect::Pulse()
{
    TEffect::Pulse();
}

// *****************
// * Water Animator *
// *****************

REGISTER_3DANIMATOR("Water", TWaterAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the Water Effect
//==============================================================================

#define WATER_MAXDROPS      25
#define WATER_LENGTH        64
#define WATER_WIDTH         64
#define WATER_HEIGHT        0
#define WATER_SCALE         0.2f
#define WATER_TALLSCALE     0.1f
#define WATER_SPEED         2.0f

void TWaterAnimator::InitParticle(int i)
{
    drops[i].pos.x = (float)(-WATER_LENGTH / 2);//(random(-WATER_LENGTH, WATER_LENGTH) / 2.0);
    drops[i].pos.y = (float)(random(-WATER_WIDTH, WATER_WIDTH) / 2.0);
    drops[i].pos.z = (float)0.0f;
    drops[i].vel.x = (float)(WATER_SPEED * ((i % 2) + 1));
    drops[i].vel.y = (float)0.0f;
    drops[i].vel.z = (float)0.0f;
    drops[i].scale.x = (float)(random(17, 30) * WATER_TALLSCALE);
    drops[i].scale.y = (float)(random(2, 4) * WATER_SCALE);
    drops[i].scale.z = (float)1.0f;
    drops[i].time = i;
}

void TWaterAnimator::UpdateStuff()
{
    int i;
    for (i = 0; i < numdrops; i++)
    {
        if (drops[i].time > 0)
        {
            drops[i].time--;
            continue;
        }
        if (drops[i].time == -1)
        {
            InitParticle(i);
            drops[i].time = 0;
        }
        drops[i].pos.x += drops[i].vel.x;
        drops[i].pos.y += drops[i].vel.y;
        drops[i].pos.z += drops[i].vel.z;
        if (drops[i].pos.x >= WATER_LENGTH / 2)
        {
            drops[i].time = -1;
        }
    }
}

void TWaterAnimator::Initialize()
{
    T3DAnimator::Initialize();
    PS3DAnimObj o = GetObject(0);
    GetVerts(o, D3DVT_LVERTEX);

    inst->GetPos(eff);
    numdrops = WATER_MAXDROPS;
    drops = new SWaterParticle[numdrops];
    int i;

    for (i = 0; i < numdrops; i++)
    {
        InitParticle(i);
    }
    // now run it through a couple hundred times
    for (int k = 0; k < numdrops; k++)
    {
        UpdateStuff();
    }
}

void TWaterAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    UpdateStuff();
}

void TWaterAnimator::DoLighting(float x, float y, float z, PS3DAnimObj object)
{
    float r, g, b, temp;
    r = g = b = 0;
    int l1, l2, l3, tx, ty, tz;
    tx = (int)(eff.x + x);
    ty = (int)(eff.y + y);
    tz = (int)(eff.z + z);
    Scene3D.GetClosestLights(tx, ty, tz, l1, l2, l3);
    if (l1 > -1)
    {
        Scene3D.GetLightBrightness(l1, tx, ty, tz, (float)temp);
        r += temp;
    }
    if (l2 > -1)
    {
        Scene3D.GetLightBrightness(l2, tx, ty, tz, (float)temp);
        r += temp;
    }
    if (l3 > -1)
    {
        Scene3D.GetLightBrightness(l3, tx, ty, tz, (float)temp);
        r += temp;
    }
    temp = (float)(MapPane.GetAmbientLight() / 255.0f);
    r += temp;
    if (r > 4.0)
        r = 4.0f;
    r *= 0.25f;
    g = b = r;
    for(int j = 0; j < 4; j++)
    {
        object->lverts[j].color = D3DRGB(r, g, b);
    }
}

BOOL TWaterAnimator::Render()
{
    SaveBlendState();
    //SetAddBlendState();
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE));

    PS3DAnimObj obj;
    obj = GetObject(0);
    ResetExtents();

    for (int i = 0; i < numdrops; i++)
    {
        if (drops[i].time != 0)
            continue;
        // clear matrix and get ready to transform and render
        obj->flags = OBJ3D_MATRIX;
        D3DMATRIXClear(&obj->matrix);
        
        obj->scl.x = (D3DVALUE)drops[i].scale.x;
        obj->scl.y = (D3DVALUE)drops[i].scale.y;
        obj->scl.z = (D3DVALUE)drops[i].scale.z;
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        
        // rotate to face correct way
        //D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 4.0));
        //D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        //D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));

        // translate to place should be
        obj->pos.x = (D3DVALUE)drops[i].pos.x;
        obj->pos.y = (D3DVALUE)drops[i].pos.y;
        obj->pos.z = (D3DVALUE)drops[i].pos.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        DoLighting(drops[i].pos.x, drops[i].pos.y, drops[i].pos.z, obj);

        RenderObject(obj);
    }

    UpdateExtents();
    RestoreBlendState();

    return TRUE;
}

void TWaterAnimator::RefreshZBuffer()
{
    int size_x = 150,
        size_y = 150;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2) - 30, size_x, size_y);
}

// ***************
// * Pixie Effect *
// ***************

DEFINE_BUILDER("Pixie", TPixieEffect)
REGISTER_BUILDER(TPixieEffect)

void TPixieEffect::Initialize()
{
    
}

void TPixieEffect::Pulse()
{
    TEffect::Pulse();
}

// *****************
// * Pixie Animator *
// *****************

REGISTER_3DANIMATOR("Pixie", TPixieAnimator)

#define PIX_NUMPARTS    25
#define PIX_ACC         0.4f
#define PIX_HEIGHT      32
#define PIX_MINSCALE    0.06f
#define PIX_MAXSCALE    0.08f
#define PIX_CHARSRANGE  90
#define PIX_SPREADPERC  500

void TPixieAnimator::Initialize()
{
    T3DAnimator::Initialize();
    pix = new SWaterParticle[PIX_NUMPARTS];
    for (int i = 0; i < PIX_NUMPARTS; i++)
    {
        pix[i].scale.x = pix[i].scale.y = pix[i].scale.z = (float)(random((int)(PIX_MINSCALE * 1000),
            (int)(PIX_MAXSCALE * 1000)) / 1000.0f);
        pix[i].pos.x = (float)random(-32, 32);
        pix[i].pos.y = (float)random(-32, 32);
        pix[i].pos.z = (float)random(-32, 32);
        pix[i].vel.x = (float)(random(-32, 32) / 25.0f);
        pix[i].vel.y = (float)(random(-32, 32) / 25.0f);
        pix[i].vel.z = (float)(random(-32, 32) / 25.0f);
        pix[i].time = random(0, 1);
    }
    charnear = 100;
    inst->GetPos(origpos);
}

void TPixieAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);

    for (int i = 0; i < PIX_NUMPARTS; i++)
    {
        pix[i].scale.x += (float)(random(-35, 35) / 1000.0f);
        //pix[i].scale.y = pix[i].scale.z = pix[i].scale.x;
        if (pix[i].scale.x > 0.07f)
            pix[i].scale.x = 0.07f;
        pix[i].pos.x += pix[i].vel.x;
        pix[i].pos.y += pix[i].vel.y;
        pix[i].pos.z += pix[i].vel.z;
        if (pix[i].pos.x > 0)
            pix[i].vel.x -= PIX_ACC;
        if (pix[i].pos.x < 0)
            pix[i].vel.x += PIX_ACC;
        if (pix[i].pos.y > 0)
            pix[i].vel.y -= PIX_ACC;
        if (pix[i].pos.y < 0)
            pix[i].vel.y += PIX_ACC;
        if (pix[i].pos.z > 0)
            pix[i].vel.z -= PIX_ACC;
        if (pix[i].pos.z < 0)
            pix[i].vel.z += PIX_ACC;
        if (random(0, 48) == 17)
            pix[i].time = 1 - pix[i].time;
        if (pix[i].pos.z < -PIX_HEIGHT)
            pix[i].pos.z = -PIX_HEIGHT;
        if (pix[i].scale.x < PIX_MINSCALE)
            pix[i].scale.x = PIX_MINSCALE;
        if (pix[i].scale.x > PIX_MAXSCALE)
            pix[i].scale.x = PIX_MAXSCALE;
    }
    S3DPoint pos;
    inst->GetPos(pos);
    BOOL acharnear = FALSE;
    int targets[MAXFOUNDOBJS];
    int num = MapPane.FindObjectsInRange(pos, targets, PIX_CHARSRANGE, 0, OBJCLASS_CHARACTER);
    for (i = 0; i < num; i++) 
    {
        if (targets[i])
        {
            acharnear = TRUE;
            break;
        }
    }
    if (!acharnear)
    {
        num = MapPane.FindObjectsInRange(pos, targets, PIX_CHARSRANGE, 0, OBJCLASS_PLAYER);
        for (i = 0; i < num; i++) 
        {
            if (targets[i])
            {
                acharnear = TRUE;
                break;
            }
        }
    }
    if (acharnear)
    {
        //run awee run awee
        PTObjectInstance charinst = MapPane.GetInstance(targets[i]);
        if (charinst)
        {
            S3DPoint charpos;
            charinst->GetPos(charpos);
            if (pos.x >= charpos.x)
                pos.x += random(2, 4);
            if (pos.x < charpos.x)
                pos.x -= random(2, 4);
            if (pos.y >= charpos.y)
                pos.y += random(2, 4);
            if (pos.y < charpos.y)
                pos.y -= random(2, 4);
        }
        /*charnear += 35;
        if (charnear > PIX_SPREADPERC)
            charnear = PIX_SPREADPERC;*/
    }
    else
    {
        /*if (charnear > 100)
            charnear -= 15;*/
        // resume positions, men!
        if (pos.x > origpos.x)
            pos.x -= random(2, 4);
        if (pos.x < origpos.x)
            pos.x += random(2, 4);
        if (pos.y > origpos.y)
            pos.y -= random(2, 4);
        if (pos.y < origpos.y)
            pos.y += random(2, 4);
    }
    //pos.x += random(-2, 2);
    //pos.y += random(-2, 2);
    inst->SetPos(pos);
}

BOOL TPixieAnimator::Render()
{
    SaveBlendState();
    SetBlendState();
    PS3DAnimObj obj;

    for (int i = 0; i < PIX_NUMPARTS; i++)
    {
        obj = GetObject(pix[i].time);

        obj->flags = OBJ3D_MATRIX;
        
        D3DMATRIXClear(&obj->matrix);
        
        obj->scl.x = obj->scl.y = obj->scl.z = (D3DVALUE)(pix[i].scale.x / (1 + (pix[i].time * 2.0f)));
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        
        // rotate to face correct way
        //D3DMATRIXRotateX(&obj->matrix, -(float)(M_PI / 4.0));
        //D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&obj->matrix, D3DVAL(-((inst->GetFace() * 360) / 256) * TORADIAN));

        // translate to place should be
        obj->pos.x = (float)((pix[i].pos.x * charnear) / 100.0f);
        obj->pos.y = (float)((pix[i].pos.y * charnear) / 100.0f);
        obj->pos.z = (float)(PIX_HEIGHT + pix[i].pos.z);
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        ResetExtents();
        RenderObject(obj);
        UpdateExtents();
    }
    RestoreBlendState();

    return TRUE;
}

void TPixieAnimator::RefreshZBuffer()
{
    int size_x = 150,
        size_y = 150;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2) - 30, size_x, size_y);
}

// effect
_CLASSDEF(TFizzleEffect)
class TFizzleEffect : public TEffect
{
  public:
    TFizzleEffect(PTObjectImagery newim) : TEffect(newim) { Initialize(); }
    TFizzleEffect(PSObjectDef def, PTObjectImagery newim) : TEffect(def, newim) { Initialize(); }
    virtual ~TFizzleEffect() {}

    virtual void Initialize();          // init

    virtual void Pulse();               // da heartbeat of america...
};

#define DUST_COUNT          30
#define DUST_FRAME          15
#define DUST_SPREAD         15
#define DUST_MIN_Z          5
#define DUST_MAX_Z          45
#define DUST_ROT            15
#define DUST_MIN_SCL        5
#define DUST_MAX_SCL        25
#define DUST_SCL_INC        .05f
#define DUST_SCL_DEC        .02f
#define DUST_ADD            1.5f

// animator for the fire cone class
_CLASSDEF(TFizzleAnimator)
class TFizzleAnimator : public T3DAnimator
{
  private:
    TParticleSystem blue;
    TParticleSystem red;
    TParticleSystem purple;
    float add;
    int frame_count;
  public:
    TFizzleAnimator(PTObjectInstance oi) : T3DAnimator(oi), blue(DUST_COUNT), red(DUST_COUNT), purple(DUST_COUNT) {}
    virtual ~TFizzleAnimator() { Close(); }

    virtual void Initialize();
    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    virtual void RefreshZBuffer();
};

// **********************
// * Fizzle Effect Code *
// **********************

DEFINE_BUILDER("Fizzle", TFizzleEffect)
REGISTER_BUILDER(TFizzleEffect)

void TFizzleEffect::Initialize()
{
}

void TFizzleEffect::Pulse()
{
    TEffect::Pulse();
}

// ************************
// * Fizzle Animator Code *
// ************************

REGISTER_3DANIMATOR("Fizzle", TFizzleAnimator)

void TFizzleAnimator::Initialize()
{
    T3DAnimator::Initialize();

    frame_count = 0;

    S3DPoint s;
    blue.Init(this, GetObject(0), s, TRUE);
    red.Init(this, GetObject(2), s, TRUE);
    purple.Init(this, GetObject(1), s, TRUE);

    add = 0.0f;
}

void TFizzleAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);

    inst->SetCommandDone(FALSE);

    add += DUST_ADD;

    int i;

    ++frame_count;

    blue.Animate();
    red.Animate();
    purple.Animate();

    // add new particles
    while(add > 1.0f && frame_count < DUST_FRAME)
    {
        add -= 1.0f;

        SParticleSystemInfo p;

        // set up the particle params
        p.pos.x = (float)random(-DUST_SPREAD, DUST_SPREAD);
        p.pos.y = (float)random(-DUST_SPREAD, DUST_SPREAD);
        p.pos.z = (float)random(70, 130);

        // velocity
        p.vel.x = 0.0f;
        p.vel.y = 0.0f;
        p.vel.z = -(float)random(DUST_MIN_Z, DUST_MAX_Z) * .1f;

        // acceleration
        p.acc.x = p.acc.y = p.acc.z = 1.0f;

        // scale
        p.scl.x = p.scl.y = p.scl.z = 0.0f;

        // rotation
        p.rot.z = (float)random(0, 359);
        p.rot.x = p.rot.y = p.rot.z = 0.0f;

        // life span
        p.life_span = 100;

        // flicker state
        p.flicker = random(0, 1);

        // rotational velocity
        p.temp.y = (float)random(-DUST_ROT, DUST_ROT);
        // scale max
        p.temp.z = (float)random(DUST_MIN_SCL, DUST_MAX_SCL) *.01f;

        int r = random(1, 3);

        if(r == 1)
            blue.Add(&p);
        else if(r == 2)
            red.Add(&p);
        else
            purple.Add(&p);
    }

    // do special stuff
    PSParticleSystemInfo particle;
    BOOL done = TRUE;

    for(int j = 0; j < 3; ++j)
    {
        PTParticleSystem ps;
        if(j == 0)
            ps = &blue;
        else if(j == 1)
            ps = &red;
        else
            ps = &purple;

        for(i = 0; i < DUST_COUNT; ++i)
        {
            particle = ps->Get(i);
            if(!particle->used)
                continue;

            if(particle->life_span == 100)
            {
                particle->scl.x += DUST_SCL_INC;
                particle->scl.y += DUST_SCL_INC;
                particle->scl.z += DUST_SCL_INC;

                if(particle->scl.x > particle->temp.z)
                    particle->life_span = 200;
            }
            else if(particle->life_span == 200)
            {
                particle->scl.x -= DUST_SCL_DEC;
                particle->scl.y -= DUST_SCL_DEC;
                particle->scl.z -= DUST_SCL_DEC;

                if(particle->scl.x <= 0)
                {
                    particle->scl.x = particle->scl.y = particle->scl.z = 0.0f;
                    particle->life_span = 0;
                }
            }
            
            particle->rot.z += particle->temp.y;

            if(particle->rot.z < 0.0f)
                particle->rot.z += 360.0f;
            if(particle->rot.z >= 360.0f)
                particle->rot.z -= 360.0f;

            // do special stuff to animating particles
            particle->flicker = random(0, 1);

            done = FALSE;
        }
    }

    // check to see if we are done
    if(frame_count >= DUST_FRAME && done)
    {
        ((PTEffect)inst)->KillThisEffect();
        return;
    }
}

BOOL TFizzleAnimator::Render()
{
    // basically cycle through the particles and render them...duh!
    SaveBlendState();
    SetBlendState();

    blue.Render(TRUE);
    red.Render(TRUE);
    purple.Render(TRUE);

    RestoreBlendState();

    return TRUE;
}

void TFizzleAnimator::RefreshZBuffer()
{
    S3DPoint pos, screen, size;

    ((PTEffect)inst)->GetPos(pos);

    WorldToScreen(pos, screen);

    size.x = 300;
    size.y = 300;

    RestoreZ(screen.x - (size.x / 2), screen.y - (size.y / 2), size.x, size.y);
}



/*****************************************************************
    Old Quicksand: Redone by Pepper

// **********************
// * Quicksand Animator *
// **********************

#define QUICKSAND_DURATION      40
#define QUICKSAND_SWITCH        10.0
#define QUICKSAND_CYLROT        0.5

#define SANDSWIRL_DURATION      75.0
#define QUICKSAND_SPIN_START    10.0

REGISTER_3DANIMATOR("Quicksand", TQuicksandAnimator)

//==============================================================================
//    Function : Initialize.
//------------------------------------------------------------------------------
// Description : This will setup the initial state for the quicksand
//==============================================================================

void TQuicksandAnimator::Initialize()
{
    T3DAnimator::Initialize();
    frameon = 0;
    scalesize = 1.0;
    stage = 0;
    count = 0;
    cylscale = (D3DVALUE)0.0;
    cylheight = (D3DVALUE)1.0;
    cylrot = (D3DVALUE)0.0;
    cylcount = 0;
    PS3DAnimObj o = GetObject(0);
    GetVerts(o, D3DVT_LVERTEX);

// added by pepper to move quicksand to the target
    PTSpell spell = ((PTEffect)inst)->GetSpell();
    if(spell)
    {
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
        if(invoker)
        {
            target = invoker->Fighting();
            if(target)
            {
                target->GetPos(target_position);    // and use it
            }
            else
            {
                invoker->GetPos(target_position);
            }
        }
        else
        {
            ((PTEffect)inst)->GetPos(target_position);
        }
    }


    PLAY("vortex sound");

    PS3DAnimObj obj = GetObject(2);
    GetVerts(obj, D3DVT_LVERTEX);
}

void TQuicksandAnimator::InitQuicksand(PTObjectInstance quicksandme, int delay)
{
    frameon = -delay;
    quicksandchar = quicksandme;
    if (quicksandchar)
    {
        quicksandchar->SetFlag(OF_DISABLED);
        //quicksandchar->SetFlag(OF_ICED);
    }
}

//==============================================================================
//    Function : Animate.
//------------------------------------------------------------------------------
// Description : This updates the sparks and bolt
//
//==============================================================================

void TQuicksandAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
    inst->SetCommandDone(FALSE);
    S3DPoint pos;

    frameon++;
    cylrot += (D3DVALUE)QUICKSAND_CYLROT;
    if (cylrot > M_2PI)
        cylrot -= (float)M_2PI;
    if (frameon < 0)
    {
        cylscale += (D3DVALUE)(0.125 / QUICKSAND_SWITCH);
        cylheight += (float)0.025;
    }
    if (frameon > 0 && frameon < QUICKSAND_DURATION)
    {
        cylscale -= (D3DVALUE)(0.125 / QUICKSAND_SWITCH);
        cylheight -= (float)0.025;
    }
    if (frameon < 0)
        return;
    if (quicksandchar && ((PTCharacter)quicksandchar)->IsDead() && frameon < ICED_DURATION)
    {
        frameon = QUICKSAND_DURATION * 2;
    }
    if (frameon > 0 && frameon < QUICKSAND_DURATION)
    {
        if (stage == 0)
            scalesize += (D3DVALUE)(1.0 / QUICKSAND_SWITCH);
        if (stage == 1)
            scalesize += (D3DVALUE)(0.5 / QUICKSAND_SWITCH);
        count++;
        if (count == QUICKSAND_SWITCH)
        {
            scalesize = 1.0;
            count = 0;
            stage++;
            if (stage > 3)
                stage = 3;
        }
    }
    if (frameon == QUICKSAND_DURATION)
    {
        stage = 3;
        count = 0;
        scalesize = 1.0;
    }
    if (frameon > QUICKSAND_DURATION && frameon < QUICKSAND_DURATION * 2)
    {
        if (stage == 0)
            scalesize -= (D3DVALUE)(1.0 / QUICKSAND_SWITCH);
        if (stage == 1)
            scalesize -= (D3DVALUE)(0.5 / QUICKSAND_SWITCH);
        count++;
        if (count == QUICKSAND_SWITCH && stage > 0)
        {
            count = 0;
            stage--;
            if (stage < 0)
                stage = 0;
            if (stage == 1)
                scalesize = 1.5;
            else if (stage == 0)
                scalesize = 2.0;
            else
                scalesize = 1.0;
        }
    }
    if (frameon == QUICKSAND_DURATION * 2)
    {
        if (quicksandchar)
        {
            quicksandchar->SetFlag(OF_DISABLED, FALSE);
            //icedchar->SetFlag(OF_ICED, FALSE);
        }
    }

// added by pepper to rotate the bad, bad man
    if(frameon > QUICKSAND_SPIN_START)
    {
        if(target  && !target->IsDead())
        {
            target->SetRotateZ(target_rotation);
            if(!target->IsFlailing())
            {
                target->Flail();
                PTSpell spell = ((PTEffect)inst)->GetSpell();
                if(spell)
                {
                    spell->Damage(target);
                }
            }
            target_rotation+=10;
            target->GetPos(target_position);
        }
    }
    dust_rotation += 2.0f;
// end added by pepper

    if (frameon > QUICKSAND_DURATION * 2)
    {
        ((PTEffect)inst)->KillThisEffect();
    }
}

//==============================================================================
//    Function : Render.
//------------------------------------------------------------------------------
// Description : Renders quicksand
//
//==============================================================================

void TQuicksandAnimator::SetAnimFrame(int frame_num, PS3DAnimObj obj)
{
    if (!obj->lverts)
        return;
    frame_num = (frame_num + 2) % 4; // this fixes it to do the right frame...(hack)
    float u = (float)(frame_num % 2) * .5f;
    float v = (float)(frame_num / 2) * .5f;

    obj->flags |= OBJ3D_VERTS;

    obj->lverts[0].tu = u + .05f;
    obj->lverts[0].tv = v + .05f;
    obj->lverts[2].tu = u + .5f - .05f;
    obj->lverts[2].tv = v + .05f;
    obj->lverts[1].tu = u + .05f;
    obj->lverts[1].tv = v + .5f - .05f;
    obj->lverts[3].tu = u + .5f - .05f;
    obj->lverts[3].tv = v + .5f - .05f;
}

BOOL TQuicksandAnimator::Render()
{
    SaveBlendState();
    SetBlendState();

    PS3DAnimObj obj;
    if (frameon < QUICKSAND_DURATION * 2)
    {
        // render spinny sand thing
        obj = GetObject(0);
        // clear matrix and get ready to transform and render
        obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS;
        D3DMATRIXClear(&obj->matrix);       
        // set frame
        SetAnimFrame(stage, obj);
        
        obj->scl.x = (D3DVALUE)cylscale;
        obj->scl.y = (D3DVALUE)cylscale;
        obj->scl.z = 1.0;
        D3DMATRIXScale(&obj->matrix, &obj->scl);
        // rotate to face correct way
        int ang = (int)(frameon * 360 / QUICKSAND_SWITCH) % 360;
        D3DMATRIXRotateZ(&obj->matrix, (D3DVALUE)(ang * TORADIAN));
        // translate to place should be
        obj->pos.x = (D3DVALUE)target_position.x;
        obj->pos.y = (D3DVALUE)target_position.y;
        obj->pos.z = (D3DVALUE)target_position.z;
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        ResetExtents();
        RenderObject(obj);
        UpdateExtents();
    }
    if (frameon < QUICKSAND_DURATION)
    {
        // render cylinder(s)
        obj = GetObject(1);
        for (int i = 0; i < 2; i++)
        {
            // clear matrix and get ready to transform and render
            obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS;
            D3DMATRIXClear(&obj->matrix);       
            // scale up and sideways
            obj->scl.x = (D3DVALUE)cylscale;
            obj->scl.y = (D3DVALUE)cylscale;
            obj->scl.z = cylheight;
            D3DMATRIXScale(&obj->matrix, &obj->scl);
            // rotate to face correct way
            D3DMATRIXRotateZ(&obj->matrix, (float)(i ? M_PI - cylrot : cylrot));
            // translate to place should be
            obj->pos.x = (D3DVALUE)target_position.x;
            obj->pos.y = (D3DVALUE)target_position.y;
            obj->pos.z = (D3DVALUE)target_position.z;
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);

            ResetExtents();
            RenderObject(obj);
            UpdateExtents();
        }
    }

// added by pepper: makes dust fly around and up
    SetAddBlendState();
    obj = GetObject(2);
    obj->flags = OBJ3D_SCL1 | OBJ3D_POS2  | OBJ3D_VERTS| OBJ3D_ABSPOS;
    obj->scl.x = (D3DVALUE)1.2f;
    obj->scl.y = (D3DVALUE)1.2f;
    obj->scl.z = (D3DVALUE)1.2f;
    obj->pos.x = (D3DVALUE)target_position.x;
    obj->pos.y = (D3DVALUE)target_position.y;
    obj->pos.z = (D3DVALUE)target_position.z;
//  ResetExtents();

    for(int i=0;i<176;i++)
    {
        obj->lverts[i].tu -= 0.02f;
//      obj->lverts[i].tv -= 0.02f;
    }

//  RenderObject(obj);
    RestoreBlendState();

    return TRUE;
}

void TQuicksandAnimator::RefreshZBuffer()
{
    int size_x = 200,
        size_y = 150;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - (size_y / 2), size_x, size_y);
}


******************************* end of old quicksand*/





/*  Ye Old Vortex

// VORTEX is a teleport spell of sorts
DEFINE_BUILDER("Vortex", TVortexEffect)
REGISTER_BUILDER(TVortexEffect)

void TVortexEffect::Initialize()
{
    // add code here if necessary
}

void TVortexEffect::Pulse()
{
    TEffect::Pulse();
}

REGISTER_3DANIMATOR("Vortex", TVortexAnimator)

void TVortexAnimator::Initialize()
{
    T3DAnimator::Initialize();
    PTSpell spell;


    level = 1;

    spell = ((PTEffect)inst)->GetSpell();
    if(spell)
    {
        if(strcmp(spell->VariantData()->name, "Vortex") == 0)
        {
            level = 1;
        }
        if(strcmp(spell->VariantData()->name, "Vortex2") == 0)
        {
            level = 2;
        }
        else if(strcmp(spell->VariantData()->name, "VortexM") == 0)
        {
            level = 0;
        }
    }
    if(level != 0)
        PLAY("vortex sound");

    // get the vertices
    PS3DAnimObj obj = GetObject(0);
    GetVerts(obj, D3DVT_LVERTEX);

    // set the colors
    float r;
    for(int i = 0; i < VORTEX_VERTEX; ++i)
    {
        r = (float)random(100, 200) / 100.0f;
        obj->lverts[i].color = D3DRGBA(min(0.15f * r, 1.0f), min(0.15f * r, 1.0f), min(.5f * r, 1.0f), 0.0f);
    }
    // set the alpha
    for(int j = 0; j < VORTEX_RING; ++j)
    {
        for(int i = 0; i < VORTEX_COUNT; ++i)
        {
            switch(j)
            {
                case VORTEX_RING - 1:
                    vertex[(j * VORTEX_COUNT) + i] = .0f;
                    break;
                case VORTEX_RING - 2:
                    vertex[(j * VORTEX_COUNT) + i] = .1429f + (.00125f * (float)random(-20, 20));
                    break;
                case VORTEX_RING - 3:
                    vertex[(j * VORTEX_COUNT) + i] = .2857f + (.0025f * (float)random(-20, 20));
                    break;
                case VORTEX_RING - 4:
                    vertex[(j * VORTEX_COUNT) + i] = .5714f + (.005f * (float)random(-20, 20));
                    break;
                default:
                    vertex[(j * VORTEX_COUNT) + i] = .6f + (.0125f * (float)random(0, 32));
                    break;
            }
        }
    }
    for(i = VORTEX_VERTEX - VORTEX_BASE; i < VORTEX_VERTEX; ++i)
    {
        if(i % 2)
            vertex[i] = .8f;
        else
            vertex[i] = 0.0f;
        r = (float)random(100, 200) / 100.0f;
        obj->lverts[i].color = D3DRGBA(min(0.15f * r, 1.0f), min(0.15f * r, 1.0f), min(.5f * r, 1.0f), 0.0f);
    }

    obj = GetObject(1);
    GetVerts(obj, D3DVT_LVERTEX);

    for(i = 0; i < 36; ++i)
    {
        r = (float)random(100, 200) / 100.0f;
        obj->lverts[i].color = D3DRGBA(min(0.15f * r, 1.0f), min(0.15f * r, 1.0f), min(.5f * r, 1.0f), 0.0f);
    }

    for(i = 0; i < 9; ++i)
    {
        glow[i] = .0f;
    }
    for(i = 9; i < 18; ++i)
    {
        switch(i)
        {
            case 9:
                glow[i] = .0f;
                break;
            case 10:
                glow[i] = .1679f;
                break;
            case 11:
                glow[i] = .3357f;
                break;
            case 12:
                glow[i] = .6714f;
                break;
            case 17:
                glow[i] = .0f;
                break;
            default:
                glow[i] = .9f;
                break;
        }
    }
    for(i = 18; i < 27; ++i)
    {
        glow[i] = .0f;
    }
    for(i = 27; i < 36; ++i)
    {
        switch(i)
        {
            case 27:
                glow[i] = .0f;
                break;
            case 28:
                glow[i] = .1679f;
                break;
            case 29:
                glow[i] = .3357f;
                break;
            case 30:
                glow[i] = .6714f;
                break;
            case 35:
                glow[i] = .0f;
                break;
            default:
                glow[i] = .9f;
                break;
        }
    }

    // the particles!
    obj = GetObject(2);
    S3DPoint size;
    size.x = 75;
    size.y = 75;
    particle_system.Init(this, obj, size);

    alpha_blend = 0.0f;
    vortex_state = VORTEX_STATE_GROW;
    count = 0;

    // init the scaling
    scale.x = 0.0f;
    scale.y = 0.0f;
    scale.z = 0.0f;

    // init the position
    spell = ((PTEffect)inst)->GetSpell();
    if(spell)
    {
        S3DPoint p;
        spell->GetInvoker()->GetPos(p);
        ((PTCharacter)spell->GetInvoker())->SetFade(100,5);
        pos.x = (float)p.x;
        pos.y = (float)p.y;
        pos.z = (float)p.z;
        pos.z = FIX_Z_VALUE(pos.z);
    }
    else
    {
        S3DPoint p;
        inst->GetPos(p);

        pos.x = (float)p.x;
        pos.y = (float)p.y;
        pos.z = (float)p.z;
    }
    // init the rotation
    rot.x = 0.0f;
    rot.y = 0.0f;
    rot.z = 0.0f;

    int ticks = (int)(VORTEX_ALPHA_MAX / VORTEX_ALPHA_FACTOR) + 1;
    // others
    scale_factor.x = VORTEX_SCALE_X_MAX / (float)ticks;
    scale_factor.y = VORTEX_SCALE_Y_MAX / (float)ticks;
    scale_factor.z = VORTEX_SCALE_Z_MAX / (float)ticks;
    rot_factor = (VORTEX_ROT_MAX / ((float)ticks + (float)(VORTEX_LIFE_SPAN / 2)));
    //rot_speed = 0.0f;
    rot_speed = 32.0f;
//  rot_speed = 16.0f;
    frame = 0;

    phase = 1;
    fade = 1.0f;
// make me invisible
}

void TVortexAnimator::Animate(BOOL draw)
{
    PTSpell spell;

    T3DAnimator::Animate(draw);

    inst->SetCommandDone(FALSE);

    int flag = random(0, 1);
    int done = 1;

// handle fading the character in and out
    spell = ((PTEffect)inst)->GetSpell();
    if(spell)
    {
        PTCharacter invoker = (PTCharacter)spell->GetInvoker();
        if(invoker)
            invoker->UpdateFade();
    }

    if(phase == 2)
    {
        for(int i = 0; i < VORTEX_PARTICLE_COUNT; ++i)
        {
            PSParticleSystemInfo particle = particle_system.Get(i);

            if(!particle->used && flag && vortex_state != VORTEX_STATE_SHRINK)
            {
                done = 0;
                // create a particle
                // set the position
                particle_info[i].rot = (float)random(0, 360);
                particle_info[i].rot_inc = (float)random(1, 20);
                particle_info[i].height_inc = (float)random(40, 100) / 20.0f;
                particle_info[i].face_rot = (float)random(0, 360);
                particle_info[i].face_rot_inc = (float)random(1, 15);
                particle_info[i].flicker = random(0, 1);
                particle_info[i].radius = VORTEX_RADIUS;

                particle->pos.x = particle_info[i].radius * (float)cos(particle_info[i].rot * TORADIAN);
                particle->pos.y = particle_info[i].radius * (float)sin(particle_info[i].rot * TORADIAN);
                particle->pos.z = 0.0f;
                // set the rotation
                particle->rot.x = 0.0f;
                particle->rot.y = particle_info[i].face_rot;
                particle->rot.z = 0.0f;
                // set the scale
                particle_info[i].scale = (float)random(50, 125) / 400.0f;
                if(particle_info[i].flicker)
                {
                    particle->scl.x = particle_info[i].scale;
                    particle->scl.y = particle_info[i].scale;
                    particle->scl.z = particle_info[i].scale;
                }
                else
                {
                    particle->scl.x = particle_info[i].scale * .5f;
                    particle->scl.y = particle_info[i].scale * .5f;
                    particle->scl.z = particle_info[i].scale * .5f;
                }

                particle->used = TRUE;

                flag = 0;
            }
            else if(particle->used)
            {
                done = 0;
                particle_info[i].rot += particle_info[i].rot_inc;
                if(particle_info[i].rot > 360.0f)
                    particle_info[i].rot -= 360.0f;
                particle_info[i].face_rot += particle_info[i].face_rot_inc;
                if(particle_info[i].face_rot > 360.0f)
                    particle_info[i].face_rot -= 360.0f;
                particle_info[i].flicker = random(0, 3);

                particle->pos.x = particle_info[i].radius * (float)cos(particle_info[i].rot * TORADIAN);
                particle->pos.y = particle_info[i].radius * (float)sin(particle_info[i].rot * TORADIAN);
                particle->pos.z += particle_info[i].height_inc;
                if(particle->pos.z > VORTEX_HEIGHT)
                    particle->used = FALSE;
                particle->rot.y = particle_info[i].face_rot;

                particle_info[i].flicker = random(0, 1);
                if(particle_info[i].flicker)
                {
                    particle->scl.x = particle_info[i].scale;
                    particle->scl.y = particle_info[i].scale;
                    particle->scl.z = particle_info[i].scale;
                }
                else
                {
                    particle->scl.x = particle_info[i].scale * .5f;
                    particle->scl.y = particle_info[i].scale * .5f;
                    particle->scl.z = particle_info[i].scale * .5f;
                }
            }
            if(particle->pos.z < VORTEX_SCALE_POINT)
            {
                particle->scl.x *= (float)(particle->pos.z) / (float)VORTEX_SCALE_POINT;
                particle->scl.y *= (float)(particle->pos.z) / (float)VORTEX_SCALE_POINT;
                particle->scl.z *= (float)(particle->pos.z) / (float)VORTEX_SCALE_POINT;
            }
            else if(VORTEX_HEIGHT - particle->pos.z < VORTEX_SCALE_POINT && VORTEX_HEIGHT - particle->pos.z > 0)
            {
                particle->scl.x *= (float)(VORTEX_HEIGHT - particle->pos.z) / (float)VORTEX_SCALE_POINT;
                particle->scl.y *= (float)(VORTEX_HEIGHT - particle->pos.z) / (float)VORTEX_SCALE_POINT;
                particle->scl.z *= (float)(VORTEX_HEIGHT - particle->pos.z) / (float)VORTEX_SCALE_POINT;
            }
        }
    }
    rot.z += rot_speed;
    if(rot.z > 360.0f)
        rot.z -= 360.0f;

    //if(count < (VORTEX_LIFE_SPAN / 2))
    //  rot_speed += rot_factor;
    //else
    //  rot_speed -= rot_factor;

    switch(vortex_state)
    {
        // when the vortex is fading in
        case VORTEX_STATE_GROW:
            alpha_blend += VORTEX_ALPHA_FACTOR;
            //scale.y += scale_factor.y;
            //scale.x += scale_factor.x;
            scale.x = VORTEX_SCALE_X_MAX;
            scale.y = VORTEX_SCALE_Y_MAX;
            scale.z += scale_factor.z;
            if(alpha_blend >= VORTEX_ALPHA_MAX)
            {
                alpha_blend = VORTEX_ALPHA_MAX;
                vortex_state = VORTEX_STATE_CONSTANT;
            }
            break;
        // when the vortex is fading out
        case VORTEX_STATE_SHRINK:
            alpha_blend -= VORTEX_ALPHA_FACTOR;
            //scale.y -= scale_factor.y;
            //scale.x -= scale_factor.x;
            scale.x = VORTEX_SCALE_X_MAX;
            scale.y = VORTEX_SCALE_Y_MAX;
            scale.z -= scale_factor.z;
            if(alpha_blend <= 0.0f)
            {
                alpha_blend = 0.0f;
//              if(done)
                {
                    if(phase == 1)
                    {
                            // get the vertices
                            PS3DAnimObj obj = GetObject(0);
                            GetVerts(obj, D3DVT_LVERTEX);

                            // set the colors
                            float r;
                            for(int i = 0; i < VORTEX_VERTEX; ++i)
                            {
                                r = (float)random(100, 200) / 100.0f;
                                obj->lverts[i].color = D3DRGBA(min(0.15f * r, 1.0f), min(0.15f * r, 1.0f), min(.5f * r, 1.0f), 0.0f);
                            }
                            // set the alpha
                            for(int j = 0; j < VORTEX_RING; ++j)
                            {
                                for(int i = 0; i < VORTEX_COUNT; ++i)
                                {
                                    switch(j)
                                    {
                                        case VORTEX_RING - 1:
                                            vertex[(j * VORTEX_COUNT) + i] = .0f;
                                            break;
                                        case VORTEX_RING - 2:
                                            vertex[(j * VORTEX_COUNT) + i] = .1429f + (.00125f * (float)random(-20, 20));
                                            break;
                                        case VORTEX_RING - 3:
                                            vertex[(j * VORTEX_COUNT) + i] = .2857f + (.0025f * (float)random(-20, 20));
                                            break;
                                        case VORTEX_RING - 4:
                                            vertex[(j * VORTEX_COUNT) + i] = .5714f + (.005f * (float)random(-20, 20));
                                            break;
                                        default:
                                            vertex[(j * VORTEX_COUNT) + i] = .6f + (.0125f * (float)random(0, 32));
                                            break;
                                    }
                                }
                            }
                            for(i = VORTEX_VERTEX - VORTEX_BASE; i < VORTEX_VERTEX; ++i)
                            {
                                if(i % 2)
                                    vertex[i] = .8f;
                                else
                                    vertex[i] = 0.0f;
                                r = (float)random(100, 200) / 100.0f;
                                obj->lverts[i].color = D3DRGBA(min(0.15f * r, 1.0f), min(0.15f * r, 1.0f), min(.5f * r, 1.0f), 0.0f);
                            }

                            obj = GetObject(1);
                            GetVerts(obj, D3DVT_LVERTEX);

                            for(i = 0; i < 36; ++i)
                            {
                                r = (float)random(100, 200) / 100.0f;
                                obj->lverts[i].color = D3DRGBA(min(0.15f * r, 1.0f), min(0.15f * r, 1.0f), min(.5f * r, 1.0f), 0.0f);
                            }

                            for(i = 0; i < 9; ++i)
                            {
                                glow[i] = .0f;
                            }
                            for(i = 9; i < 18; ++i)
                            {
                                switch(i)
                                {
                                    case 9:
                                        glow[i] = .0f;
                                        break;
                                    case 10:
                                        glow[i] = .1679f;
                                        break;
                                    case 11:
                                        glow[i] = .3357f;
                                        break;
                                    case 12:
                                        glow[i] = .6714f;
                                        break;
                                    case 17:
                                        glow[i] = .0f;
                                        break;
                                    default:
                                        glow[i] = .9f;
                                        break;
                                }
                            }
                            for(i = 18; i < 27; ++i)
                            {
                                glow[i] = .0f;
                            }
                            for(i = 27; i < 36; ++i)
                            {
                                switch(i)
                                {
                                    case 27:
                                        glow[i] = .0f;
                                        break;
                                    case 28:
                                        glow[i] = .1679f;
                                        break;
                                    case 29:
                                        glow[i] = .3357f;
                                        break;
                                    case 30:
                                        glow[i] = .6714f;
                                        break;
                                    case 35:
                                        glow[i] = .0f;
                                        break;
                                    default:
                                        glow[i] = .9f;
                                        break;
                                }
                            }

                            // the particles!
                            alpha_blend = 0.0f;
                            vortex_state = VORTEX_STATE_GROW;
                            count = 0;

                            // init the scaling
                            scale.x = 0.0f;
                            scale.y = 0.0f;
                            scale.z = 0.0f;

                            // init the position
                            pos.x = 0.0f;
                            pos.y = 0.0f;
                            pos.z = 0.0f;

                            // init the rotation
                            rot.x = 0.0f;
                            rot.y = 0.0f;
                            rot.z = 0.0f;

                            int ticks = (int)(VORTEX_ALPHA_MAX / VORTEX_ALPHA_FACTOR) + 1;
                            // others
                            scale_factor.x = VORTEX_SCALE_X_MAX / (float)ticks;
                            scale_factor.y = VORTEX_SCALE_Y_MAX / (float)ticks;
                            scale_factor.z = VORTEX_SCALE_Z_MAX / (float)ticks;
                            rot_factor = (VORTEX_ROT_MAX / ((float)ticks + (float)(VORTEX_LIFE_SPAN / 2)));
                            //rot_speed = 0.0f;
                            rot_speed = 32.0f;
                        //  rot_speed = 16.0f;
                            frame = 0;
                        S3DPoint temp_point2;
                        int temp_level = -1;
                        int x,y,dir_x,dir_y;
                        spell = ((PTEffect)inst)->GetSpell();
                        if(spell)
                        {
                            if(level == 2)
                            {
                                PTCharacter invoker = (PTCharacter)spell->GetInvoker();
                                invoker->GetPos(temp_point2);
                                if(invoker)
                                {
                                    temp_point2 = invoker->GetTeleportPosition();
                                    temp_level = invoker->GetTeleportLevel();
                                    S3DPoint temp_point3;
                                    temp_point3.x = temp_point3.y = temp_point3.z = -1;
                                    invoker->SetTeleportPosition(temp_point3);
                                    invoker->SetTeleportLevel(-1);
                                    if(temp_point2.x == -1 || temp_point2.y == -1)
                                    {
                                        invoker->GetPos(temp_point2);
                                        temp_level = MapPane.GetMapLevel();
                                    }
                                    PTSetVortexEffect old = (PTSetVortexEffect)MapPane.FindObject("SetVortex",1);
                                    if(old)
                                    {
                                        old->KillThisEffect();
                                    }
                                }
                            }
                            else
                            {
                                PTCharacter invoker = (PTCharacter)spell->GetInvoker();
                                invoker->GetPos(temp_point2);
                                invoker->GetPos(temp_point);
                                x = random(-100, 100);
                                if(x > 0)
                                {
                                    dir_x = 1;
                                    x += 50;
                                }
                                else if(x < 0)
                                {
                                    dir_x = -1;
                                    x -= 50;
                                }
                                else
                                    dir_x = 0;
                                y = random(-100, 100);
                                if(y > 0)
                                {
                                    dir_y = 1;
                                    y += 50;
                                }
                                else if(y < 0)
                                {
                                    dir_y = -1;
                                    y -= 50;
                                }
                                else
                                    dir_y = 0;
                                PTCharacter temp_char;
                                BOOL blocked = FALSE;
                                do
                                {
                                    temp_point2.x+=dir_x;
                                    temp_point2.y+=dir_y;
                                    x-=dir_x;
                                    y-=dir_y;
                                    blocked = invoker->Blocked(temp_point, temp_point2, 0, NULL, &temp_char);
                                    if(temp_char != NULL && blocked)
                                    {
                                        blocked = FALSE;
                                    }
                                }
                                while(x != 0 && y != 0 && !blocked);
                                while(!blocked)
                                {
                                    temp_point2.x-=dir_x;
                                    temp_point2.y-=dir_y;
                                    blocked = invoker->Blocked(temp_point, temp_point2, 0, NULL, &temp_char);
                                    if(temp_char != NULL && blocked)
                                    {
                                        blocked = FALSE;
                                    }
                                }
                            }

                            temp_point2.z = MapPane.GetWalkHeight(temp_point2);
                            pos.x = (float)temp_point2.x;
                            pos.y = (float)temp_point2.y;
                            temp_point2.z = (int)FIX_Z_VALUE(temp_point2.z);
                            //pos.z = temp_point.z;

                            spell->GetInvoker()->SetPos(temp_point2, temp_level);
                            inst->SetPos(temp_point2);
                            ((PTCharacter)spell->GetInvoker())->SetFade(0,-5);

                            obj = GetObject(2);
                            S3DPoint size;
                            size.x = 75;
                            size.y = 75;
                            particle_system.Init(this, obj, size);
                            for(i = 0; i < VORTEX_PARTICLE_COUNT; ++i)
                            {
                                PSParticleSystemInfo particle = particle_system.Get(i);

                                particle->used = FALSE;
                            }
                        }
                        phase = 2;
                    }
                    else
                    {
                        ((PTEffect)inst)->KillThisEffect();
                    }
                }
            }
            break;
        // when the vortex is constant
        case VORTEX_STATE_CONSTANT:
            scale.y = VORTEX_SCALE_Y_MAX;
            scale.x = VORTEX_SCALE_X_MAX;
            scale.z = VORTEX_SCALE_Z_MAX;
            if(count >= VORTEX_LIFE_SPAN)
            {
                vortex_state = VORTEX_STATE_SHRINK;
            }
            ++count;
            break;
    }
}

BOOL TVortexAnimator::Render()
{
    
    SaveBlendState();
    SetBlendState();

    DWORD oldcullmode;
    Scene3D.GetRenderState(D3DRENDERSTATE_CULLMODE, &oldcullmode);
    Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);

    if(phase == 2 || level == 0)
        particle_system.Render();

    if(level != 0)
    {
        // BEGIN VORTEX
        PS3DAnimObj obj = GetObject(0);
        ResetExtents();

        obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3 | OBJ3D_VERTS | OBJ3D_ABSPOS;

        float r, g, b, a;
        for(int i = 0; i < VORTEX_VERTEX; ++i)
        {
            r = (float)RGBA_GETRED(obj->lverts[i].color) / 255.0f;
            g = (float)RGBA_GETGREEN(obj->lverts[i].color) / 255.0f;
            b = (float)RGBA_GETBLUE(obj->lverts[i].color) / 255.0f;
            a = vertex[i] * alpha_blend;
            obj->lverts[i].color = D3DRGBA(r, g, b, a);
        }

        // rotate
        obj->rot.x = (float)(rot.x * TORADIAN);
        obj->rot.y = (float)(rot.y * TORADIAN);
        obj->rot.z = (float)(rot.z * TORADIAN);

        // scale
        obj->scl.x = scale.x;
        obj->scl.y = scale.y;
        obj->scl.z = scale.z;

        // translate
        obj->pos.x = pos.x;
        obj->pos.y = pos.y;
        obj->pos.z = pos.z;

        RenderObject(obj);

        UpdateExtents();

        GetExtents(&extents);
        AddUpdateRect(&extents, UPDATE_RESTORE);
        UpdateBoundingRect(&extents);

        // END VORTEX

        // BEGIN GLOW
        obj = GetObject(1);

        ResetExtents();

        obj->flags = OBJ3D_ROT1 | OBJ3D_SCL2 | OBJ3D_POS3 | OBJ3D_VERTS | OBJ3D_ABSPOS;

        float factor;
        for(i = 0; i < VORTEX_GLOW_COUNT; ++i)
        {
            r = (float)RGBA_GETRED(obj->lverts[i].color) / 255.0f;
            g = (float)RGBA_GETGREEN(obj->lverts[i].color) / 255.0f;
            b = (float)RGBA_GETBLUE(obj->lverts[i].color) / 255.0f;
            factor = .75f + (.0125f * (float)random(0, 40));
            if(frame == 2)
            {
                a = min(glow[i] * factor * alpha_blend, 1.0f);
                frame = 0;
            }
            else
            {
                a = min(glow[i] * alpha_blend, 1.0f);
                ++frame;
            }
            obj->lverts[i].color = D3DRGBA(r, g, b, a);
        }

        // rotate
        obj->rot.x = 0.0f;
        obj->rot.y = 0.0f;
        obj->rot.z = (float)(-45.0f * TORADIAN);

        // scale
        obj->scl.x = scale.x;
        obj->scl.y = scale.y;
        obj->scl.z = scale.z;

        // position
        obj->pos.x = pos.x;
        obj->pos.y = pos.y;
        obj->pos.z = pos.z;

        RenderObject(obj);

        UpdateExtents();
        // END GLOW

        GetExtents(&extents);
        AddUpdateRect(&extents, UPDATE_RESTORE);
        UpdateBoundingRect(&extents);
    }

    Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullmode);

    RestoreBlendState();

    return TRUE;
}

void TVortexAnimator::RefreshZBuffer()
{
    D3DVECTOR size;
    size.x = 325;
    size.y = 325;
    S3DPoint effect, map, screen;

    ((PTEffect)inst)->GetPos(effect);

    map.x = (int)pos.x + effect.x;
    map.y = (int)pos.y + effect.y;
    map.z = (int)pos.z + effect.z;

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - ((int)size.x / 2), screen.y - ((int)size.y / 2) - ((int)size.y / 3), (int)size.x, (int)size.y);

    //particle_system.RefreshZBuffer();
    // don't need the above line...because well the vortex zbuffer is enough
}


*****************************  End 'O Ye old Vortex*/



// ***********************************
// * Ambient Sound Effect
// ***********************************


DEFINE_BUILDER("Speaker", TAmbSoundEffect)
REGISTER_BUILDER(TAmbSoundEffect)

void TAmbSoundEffect::Initialize()
{
    memset(sound_name, 0, sizeof(char) * SOUNDNAME_LEN);
    sample_length = 0;
    ticks = 0;
    id = -1;
}

void TAmbSoundEffect::Pulse()
{
    TEffect::Pulse();


    if (id > 0)     // if we have a sound defined for this speaker...
    {
        if (sample_length >= 0)
        {
            if (!ticks) // we reached the end of our sound loop, or we are starting it for the first time
            {
                ticks = sample_length;                  // set our tick count-down
                SoundPlayer.Play(id, 0, 0, &pos);   // play the sound...
            }

            ticks--;    // go down one time...
        }
        else
        {
            if (ticks == 1) // we reached the end of our sound loop, or we are starting it for the first time
            {
                ticks = 0;                              // set our tick count-down
                SoundPlayer.Play(id, 0, 0, &pos);   // play the sound...
            }
        }
    }
}

void TAmbSoundEffect::SetParams(char *name, int len)
{
    int tmp_id;

    
    if (strcmp(sound_name, name))                   // not the same file
    {
        tmp_id = SoundPlayer.FindSound(name);
        if (tmp_id > 0)
        {
            if (id > 0) SoundPlayer.Unmount(id);    // unload the old sound, if there was one

            SoundPlayer.Mount(tmp_id);              // pre-load the new sound

            strcpy(sound_name, name);               // save the new name
            sample_length = len;                    // save the new length
            id = tmp_id;                            // get the new id number

            if (sample_length >= 0)
                ((PTSound)SoundPlayer.GetSound(id))->SetLooping(FALSE); // make it looping
            else
                ((PTSound)SoundPlayer.GetSound(id))->SetLooping(TRUE);  // make it looping
        }
    }
    else    // same file... 
    {
        sample_length = len;                        // so let's update the play-length
    }

    if (sample_length >= 0)
        ticks = 0;
    else
        ticks = 1;
}

void TAmbSoundEffect::Load(RTInputStream is, int version, int objversion)
{
    TObjectInstance::Load(is, version, objversion);
    is >> sound_name >> sample_length;

    id = SoundPlayer.FindSound(sound_name);
    if (id > 0)
    {
        SoundPlayer.Mount(id);                                  // pre-load the new sound


        if (sample_length >= 0)
            ((PTSound)SoundPlayer.GetSound(id))->SetLooping(FALSE); // make it looping
        else
            ((PTSound)SoundPlayer.GetSound(id))->SetLooping(TRUE);  // make it looping

    }
    else
    {
        memset(sound_name, 0, sizeof(char) * SOUNDNAME_LEN);
        sample_length = 0;
    }

        if (sample_length >= 0)
            ticks = 0;
        else
            ticks = 1;
}

void TAmbSoundEffect::Save(RTOutputStream os)
{
    TObjectInstance::Save(os);
    os << sound_name << sample_length;
}

// ***************************
// * Ambient sound animator
// *
// * this will render a little speaker box where the ambient sound is coming from in the editor only
// ***************************

REGISTER_3DANIMATOR("Speaker", TAmbSoundAnimator)

void TAmbSoundAnimator::Initialize()
{
    T3DAnimator::Initialize();
}


void TAmbSoundAnimator::Animate(BOOL draw)
{
    T3DAnimator::Animate(draw);
}

BOOL TAmbSoundAnimator::Render()
{
    if (Editor)     // if we're in the editor, show this thing.
    {
        PS3DAnimObj obj = GetObject(0);

//      obj->flags  |= OBJ3D_ABSPOS;            // make the little speaker thingy appear in the right place
//      ((PTEffect)inst)->GetPos(obj->pos);

        SaveBlendState();
        SetBlendState();

        ResetExtents();

        RenderObject(obj);      // render speaker thingy...

        UpdateExtents();

        RestoreBlendState();
    }

    return TRUE;
}

void TAmbSoundAnimator::RefreshZBuffer()
{
    int size_x = 40,
        size_y = 40;
    S3DPoint effect, screen;

    ((PTEffect)inst)->GetPos(effect);

    WorldToScreen(effect, screen);

    RestoreZ(screen.x - (size_x / 2), screen.y - size_y, size_x, size_y);
}
