// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 charanimator.cpp - TCharAnimator module               *
// *************************************************************************

#include <windows.h>
#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>
#include <math.h>
#include "d3dmacs.h"
#include "d3dmath.h"

#include "character.h"
#include "player.h"
#include "charanimator.h"
#include "effect.h"
#include "mappane.h"

REGISTER_3DANIMATOR("CHARACTER", TCharAnimator)
REGISTER_3DANIMATOR("PLAYER", TPlayerAnimator)

TCharAnimator::TCharAnimator(PTObjectInstance oi) : T3DAnimator(oi)
{
    InitPoisonColor();
    InitTransparency();
    InitUtilityImagery();

    visindicator_shiftval = 0.0f;
}

TCharAnimator::~TCharAnimator()
{
    CloseUtilityImagery();
    ClosePoisonColor();
    weaponswipe.Close();

    Close();
}

void TCharAnimator::Animate(BOOL draw)
{
    if (!weaponswipe.GetInitialized())
        SetupWeaponSwipe();

    T3DAnimator::Animate(draw); 
}

BOOL TCharAnimator::Render()
{
    // Programmers note: it's a good idea to put your functions down below, as
    // these functions here can get to be pretty confusing if you add all your 
    // crap to them.  BEN

    SetPoisonColor();       // Sets green color for character
    UpdateTransparency();   // Updates animator's transparency value based on char
    if (transparency < 0.01f)
        return TRUE;        // Too dim to render

  // Unhide all objects (except for weapon/sword)
    for (int c = 0; c < NumObjects(); c++)
    {
        PS3DAnimObj obj = GetObject(c);
        if (!stricmp(Get3DImagery()->GetObjectName(obj->objnum), "sword") ||
            !stricmp(Get3DImagery()->GetObjectName(obj->objnum), "weapon"))
            obj->flags |= OBJ3D_HIDE;
        else
            obj->flags &= ~OBJ3D_HIDE;
    }

    if (inst->ObjClass() == OBJCLASS_PLAYER)
        HideCharParts();        // Hides char parts replaced by equipment before rendering
    
  // Now reset transparency (if needed)
    if (abs(transparency - 1.0f) > 0.001f)
        SetMaterialTransparency(Get3DImagery());

    RenderShadow();

    T3DAnimator::Render();  // Renders character (minus hidden parts)

    if (inst->ObjClass() == OBJCLASS_CHARACTER)
        if (Player)
            if (Player->IsSneakMode())
                RenderVisionIndicator();

    RenderBloodyChunks();

  // Now reset transparency (if needed)
    if (abs(transparency - 1.0f) > 0.001f)
        ResetMaterialTransparency(Get3DImagery());

    if (inst->ObjClass() == OBJCLASS_PLAYER)
        RenderEquipment();      // Renders equipment geometry

    if (weaponswipe.GetInitialized() &&                                     // Ready
      ((PTCharacter)inst)->IsAttack() &&                                    // Is an attack
      !(inst->ObjClass() == OBJCLASS_PLAYER &&                              // Is not hand to hand
        ((PTPlayer)inst)->PrimeHand() == NULL ))    
          weaponswipe.Render();

//  RenderCombatFlashes();

    return TRUE;
}


// ********************
// * Poison Functions *
// ********************

void TCharAnimator::InitPoisonColor()
{

  // Get poison material    
    origmatred = new float[MAXMATERIALS];
    origmatgreen = new float[MAXMATERIALS];
    oldpoison = 0;

    S3DMat mat;
    for (int i = 0; i < Get3DImagery()->NumMaterials(); i++)
    {
        memset(&mat, 0, sizeof(S3DMat));
        Get3DImagery()->GetMaterial(i, &mat);

        D3DMATERIAL &m = mat.matdesc;
        origmatred[i] = m.ambient.r;
        origmatgreen[i] = m.ambient.g;
    }
}

void TCharAnimator::ClosePoisonColor()
{
    S3DMat mat;
    for (int i = 0; i < Get3DImagery()->NumMaterials(); i++)
    {
        memset(&mat, 0, sizeof(S3DMat));
        Get3DImagery()->GetMaterial(i, &mat);

        D3DMATERIAL &m = mat.matdesc;
        m.ambient.r = D3DVAL(origmatred[i]);
        m.ambient.b = D3DVAL(origmatgreen[i]);

        Get3DImagery()->SetMaterial(i, &mat);
    }

    delete [] origmatred;
    delete [] origmatgreen;
}

// Sets poison color for character
void TCharAnimator::SetPoisonColor()
{
    int poison = 9 * ((PTCharacter)inst)->Poisoned();

    if (poison != oldpoison)
    {
        S3DMat mat;

        float amount = ((float)10.0 - (float)poison) / (float)10.0;

        if (poison > 10)
            amount = 0.0;
        else if (poison < 0)
            amount = 1.0;

        for (int i = 0; i < Get3DImagery()->NumMaterials(); i++)
        {
            memset(&mat, 0, sizeof(S3DMat));
            Get3DImagery()->GetMaterial(i, &mat);

            D3DMATERIAL &m = mat.matdesc;

            if (amount < 0.1)
            {
                m.ambient.r = D3DVAL(origmatred[i]);
                m.ambient.b = D3DVAL(origmatgreen[i]);
            }
            else
            {
                m.ambient.r = D3DVAL(origmatred[i] * amount);
                m.ambient.b = D3DVAL(origmatgreen[i] * amount);
            }

            Get3DImagery()->SetMaterial(i, &mat);
        }
    }
}

// *********************
// * Equipment Methods *
// *********************

#define PROCESSEQUIPMENT_HIDECHARPARTS    1
#define PROCESSEQUIPMENT_RENDEREQUIPPARTS 2

// Finds the weapon in the character object and hides it
void TCharAnimator::ProcessEquipment(int task)
{
    if (inst->ObjClass() != OBJCLASS_PLAYER)
        return;

    S3DAnimObj equipobj;
    memset(&equipobj, 0, sizeof(S3DAnimObj));

    PTPlayer player = (PTPlayer)inst;

    for (int eq = 0; eq < NUM_EQ_SLOTS; eq++)
    {
        PTObjectInstance oi = player->GetEquip(eq);
        if (!oi)
            continue;
            
        if (oi->GetImagery()->GetHeader()->imageryid != OBJIMAGE_MESH3D)
            continue;

        PT3DImagery equipimagery = (PT3DImagery)oi->GetImagery();
        PSImageryHeader equipheader = equipimagery->GetHeader();

      // Find equipment state for player's body type
        int nlen = 0;
        for (int st = 0; st < equipheader->numstates; st++)
        {
          // Objects in this state are for all chars
            if (!stricmp(equipheader->states[st].animname, "still") ||
                !stricmp(equipheader->states[st].animname, "all"))
            {
                nlen = 0;
                break;
            }

          // Has specific state for this body type
            if (!stricmp(equipheader->states[st].animname, player->BodyType()))
            {
                nlen = strlen(player->BodyType());
                break;
            }
        }

      // No geometry for player's body type!  Exit (this should never happen!)
        if (st >= equipheader->numstates)
            continue;

      // Go through equip objects, and processes for matching player objects    
        for (int o = 0; o < equipimagery->NumObjects(); o++)
        {
            if (equipimagery->IsHidden(o, st))
                continue;

            char *n = equipimagery->GetObjectName(o);

            if (nlen > 0 && !strnicmp(n, player->BodyType(), nlen))
                n += nlen;

            int playerobjnum;

            if (eq == EQ_PRIMEHAND)     // If weapon slot
            {   
                if (((PTCharacter)inst)->IsBowMode())   // Not in combat mode.. don't replace
                    playerobjnum = -1;
                else
                {
                    playerobjnum = GetObjectNum("weapon");  // Find weapon or sword object to replace
                    if (playerobjnum < 0)
                        playerobjnum = GetObjectNum("sword");
                    if (playerobjnum < 0)
                        playerobjnum = GetObjectNum(n);     // All else fails, try object name
                }
            }
            else if (eq == EQ_RANGEDWEAPON)     // If bow slot
            {
                if (!((PTCharacter)inst)->IsBowMode())  // Not in bow mode.. don't replace
                    playerobjnum = -1;
                else
                {
                    playerobjnum = GetObjectNum("weapon");  // Find weapon or sword object to replace
                    if (playerobjnum < 0)
                        playerobjnum = GetObjectNum("sword");
                    if (playerobjnum < 0)
                        playerobjnum = GetObjectNum(n); // All else fails, try object name
                }
            }
            else    // Otherwise find the object name specified in the inventory object
                playerobjnum = GetObjectNum(n);

            if (playerobjnum < 0)
                continue;

            PS3DAnimObj playerobj = GetObject(playerobjnum);

          // Do the actual thing now
            if (task == PROCESSEQUIPMENT_HIDECHARPARTS)
            {
                playerobj->flags |= OBJ3D_HIDE;
            }
            else if (task == PROCESSEQUIPMENT_RENDEREQUIPPARTS)
            {
                equipobj.objnum = o;
                equipobj.flags = OBJ3D_MATRIX;

              // Copy the matrix for the weapon in the character, to the inventory item weapon!
                memcpy(&(equipobj.matrix), &(playerobj->matrix), sizeof(D3DMATRIX));

              // Set transparency (if needed)
                if (abs(transparency - 1.0f) > 0.001f)
                    SetMaterialTransparency(equipimagery);

             // Render the inventory item weapon via it's own imagery object!!
                equipimagery->RenderObject(&equipobj, 0, 0, &matrix, -1, FALSE);

              // Now reset transparency (if needed)
                if (abs(transparency - 1.0f) > 0.001f)
                    ResetMaterialTransparency(equipimagery);
            }
        }
    }
}

void TCharAnimator::HideCharParts()
{
    ProcessEquipment(PROCESSEQUIPMENT_HIDECHARPARTS);
}

void TCharAnimator::RenderEquipment()
{
    ProcessEquipment(PROCESSEQUIPMENT_RENDEREQUIPPARTS);
}

// ************************
// * Visibility Functions *
// ************************

#define MAX_TRANSPARENCY_GROW (0.05f)

void TCharAnimator::InitTransparency()
{
    float transparency = (float)((PTCharacter)inst)->Transparency() / 100.0f;
}

void TCharAnimator::UpdateTransparency()
{
    float t = (float)((PTCharacter)inst)->Transparency() / 100.0f;

    if (Editor || abs(t - transparency) < MAX_TRANSPARENCY_GROW)
        transparency = t;
    else if (t < transparency)
        transparency -= MAX_TRANSPARENCY_GROW;
    else
        transparency += MAX_TRANSPARENCY_GROW;
}

void TCharAnimator::SetMaterialTransparency(PT3DImagery img)
{
    for (int c = 0; c < img->NumMaterials(); c++)
    {
        S3DMat m;
        img->GetMaterial(0, &m);
        m.matdesc.ambient.a = m.matdesc.diffuse.a = 
            m.matdesc.specular.a = m.matdesc.emissive.a = transparency;
        img->SetMaterial(0, &m);
    }
}

void TCharAnimator::ResetMaterialTransparency(PT3DImagery img)
{
    for (int c = 0; c < img->NumMaterials(); c++)
    {
        S3DMat m;
        img->GetMaterial(0, &m);
        m.matdesc.ambient.a = m.matdesc.diffuse.a = 
            m.matdesc.specular.a = m.matdesc.emissive.a = 100.0f;
        img->SetMaterial(0, &m);
    }
}

// ***************************************************************
// * Utility stuff (shadow, combat flashes, bloody chunks, etc.) *
// ***************************************************************

void TCharAnimator::InitUtilityImagery()
{
    extern TObjectClass EffectClass;

    SObjectDef def;
    memset(&def, 0, sizeof(SObjectDef));

    def.objclass = OBJCLASS_EFFECT;
    def.objtype = EffectClass.FindObjType("CharUtility");
    PSObjectInfo info = EffectClass.GetObjType(def.objtype);
    if (info)
        utilityimagery = (PT3DImagery)TObjectImagery::LoadImagery(info->imageryid);
}

void TCharAnimator::CloseUtilityImagery()
{
    extern TObjectClass EffectClass;

    TObjectImagery::FreeImagery(utilityimagery);
}

PS3DAnimObj TCharAnimator::GetNewImObject(PT3DImagery imagery, int objnum, int flags)
{
    PS3DAnimObj obj = new S3DAnimObj;
    memset(obj, 0, sizeof(S3DAnimObj));
    obj->objnum = objnum;
    obj->parent = NULL;

    S3DObj o;
    imagery->GetObject(objnum, &o);

    obj->animtrack = min(objnum, imagery->NumObjects() - 1);
    obj->primtype = D3DPT_TRIANGLELIST;
    obj->verttype = D3DVT_VERTEX;
    //obj->verttype = D3DVT_LVERTEX;
    obj->hmaterial = imagery->GetMaterialHandle(o.material);
    for (int c = 0; c < MAXTEXTURES; c++)
        obj->htextures[c] = imagery->GetTextureHandle(c);

    if (flags & OBJ3D_COPYVERTS)
        GetImVerts(imagery, obj, obj->verttype); 

    if (flags & OBJ3D_COPYFACES)
        GetImFaces(imagery, obj);

    return obj;
}

void TCharAnimator::GetImVerts(PT3DImagery imagery, PS3DAnimObj obj, D3DVERTEXTYPE verttype)
{
    obj->flags |= (OBJ3D_VERTS | OBJ3D_COPYVERTS | OBJ3D_OWNSVERTS);
    obj->verttype = verttype;

    obj->numverts = imagery->NumObjVerts(obj->objnum);
    if (obj->verttype == D3DVT_VERTEX)
        obj->verts = new D3DVERTEX[obj->numverts];
    else if (obj->verttype == D3DVT_LVERTEX)
        obj->lverts = new D3DLVERTEX[obj->numverts];
    else if (obj->verttype == D3DVT_TLVERTEX)
        obj->tlverts = new D3DTLVERTEX[obj->numverts];
    imagery->GetObjVerts(obj->objnum, obj->verts, 0, 0, obj->verttype);
}

void TCharAnimator::GetImFaces(PT3DImagery imagery, PS3DAnimObj obj)
{
    obj->flags |= (OBJ3D_FACES | OBJ3D_COPYFACES | OBJ3D_OWNSFACES);

    obj->numfaces = imagery->NumObjFaces(obj->objnum);
    obj->faces = new S3DFace[obj->numfaces];
    imagery->GetObjFaces(obj->objnum, obj->faces);
}

void TCharAnimator::RenderShadow()
{
    PS3DAnimObj obj;
    obj = GetNewImObject(utilityimagery, 0, OBJ3D_COPYVERTS | OBJ3D_COPYFACES);
    /*for(int i = 0; i < obj->numverts; i++)
    {
        obj->lverts[i].color = D3DRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    }*/
    
    D3DMATRIX pos;
    D3DMATRIXClear(&pos);
    D3DMATRIXClear(&obj->matrix);

  // Scale shadow based on radius
    D3DVECTOR scl;
    scl.x = scl.y = scl.z = (float)((PTCharacter)inst)->Radius() / 24.0f;
    D3DMATRIXScale(&obj->matrix, &scl);

  // Add in char's position
    D3DVECTOR v;
    S3DPoint charpos;
    inst->GetPos(charpos);

#define SHADOW_OFFSET   5.0f
    v.x = (float)(charpos.x + SHADOW_OFFSET * 2);
    v.y = (float)(charpos.y + SHADOW_OFFSET * 2);
    v.z = (float)FIX_Z_VALUE(charpos.z) + (float)SHADOW_OFFSET;
    D3DMATRIXTranslate(&obj->matrix, &v);
    obj->flags |= OBJ3D_MATRIX | OBJ3D_FACES | OBJ3D_VERTS | OBJ3D_OWNSFACES | OBJ3D_OWNSVERTS;
    
    // Set start (offset, usually 0!!) and number of faces for texture[0 no texture]
    obj->texfaces[0] = 0;
    obj->numtexfaces[0] = 0;//obj->numfaces;
    obj->texfaces[1] = 0;
    obj->numtexfaces[1] = obj->numfaces;

    SaveBlendState();
    SetBlendState();
    utilityimagery->RenderObject(obj, 0, 0, &pos);
    RestoreBlendState();
}

void TCharAnimator::RenderCombatFlashes()
{
    int frame = ((PTCharacter)inst)->GetCombatFlashTicks();
    if (!frame)
        return;

    PS3DAnimObj obj;
    obj = GetNewImObject(utilityimagery, 1, OBJ3D_COPYVERTS | OBJ3D_COPYFACES);
    /*for(int i = 0; i < obj->numverts; i++)
    {
        obj->lverts[i].color = D3DRGBA(1.0f, 1.0f, 1.0f, 1.0f);
    }*/
    
    D3DMATRIX pos;
    D3DMATRIXClear(&pos);
    D3DMATRIXClear(&obj->matrix);

    D3DVECTOR scl;
    scl.x = scl.y = scl.z = 0.6f;
    D3DMATRIXScale(&obj->matrix, &scl);
    D3DMATRIXRotateX(&obj->matrix, -(float)(90 * TORADIAN));
    D3DMATRIXRotateZ(&obj->matrix, -(float)(M_PI / 4.0f));
    // spin 45 degrees every other frame
    D3DMATRIXRotateY(&obj->matrix, -(float)(45 * TORADIAN * (frame & 1)));
        
    D3DVECTOR v;
    S3DPoint charpos;
    inst->GetPos(charpos);

#define CF_OFFSET   25.0f
#define CF_HEIGHT   50.0f
    v.x = (float)(charpos.x + CF_OFFSET * 2);
    v.y = (float)(charpos.y + CF_OFFSET * 2);
    v.z = (float)FIX_Z_VALUE(charpos.z) + (float)(CF_OFFSET + CF_HEIGHT);
    D3DMATRIXTranslate(&obj->matrix, &v);
    obj->flags |= OBJ3D_MATRIX | OBJ3D_FACES | OBJ3D_VERTS | OBJ3D_OWNSFACES | OBJ3D_OWNSVERTS;
    
    // Set start (offset, usually 0!!) and number of faces for texture[0]
    // these aren't correct (it's drawing a shadow)
    obj->texfaces[0] = 0;
    obj->numtexfaces[0] = 0;
    obj->texfaces[1] = 0;
    obj->numtexfaces[1] = 0;
    obj->texfaces[2] = 0;
    obj->numtexfaces[2] = obj->numfaces;

    SaveBlendState();
    SetBlendState();
    utilityimagery->RenderObject(obj, 0, 0, &pos);
    RestoreBlendState();
}

void TCharAnimator::RenderBloodyChunks()
{
}

void TCharAnimator::RenderVisionIndicator()
{
    if (!(((PTCharacter)inst)->CanSeeCharacter(Player, -1)))
    {
        PS3DAnimObj obj;
        S3DPoint charpos;
        D3DVECTOR translate_vector, head_pos;
        D3DVECTOR scl;
        D3DMATRIX pos;
        //D3DMATRIX head_matrix;
        float facing_angle;
        float tmp_shiftval;

        obj = GetNewImObject(utilityimagery, 2, OBJ3D_COPYVERTS | OBJ3D_COPYFACES);

        obj->flags |= OBJ3D_MATRIX | OBJ3D_FACES | OBJ3D_VERTS | OBJ3D_OWNSFACES | OBJ3D_OWNSVERTS;
    
#if 1
//      D3DMATRIXClear(&head_matrix);

        GetObjectMatrix(GetObjectNum("head"), &pos);

        // extract the translation info from the head object's matrix
        head_pos.x = pos._41;
        head_pos.y = pos._42;
        head_pos.z = pos._43;

/*
        head_matrix._11 = pos._11;
        head_matrix._12 = pos._12;
        head_matrix._21 = pos._21;
        head_matrix._22 = pos._22;
*/
#endif

        D3DMATRIXClear(&obj->matrix);
        D3DMATRIXClear(&pos);

        inst->GetPos(charpos);

        // get the angle that the character is facing in radians, converted from a 255-degree range
        facing_angle = ((float)inst->GetFace()) * ((float)M_PI / 127.0f);

        // scale first: base it on the character's maximum sight range
        scl.y = 128.0f / (float)(((PTCharacter)inst)->GetCharData())->sightangle;
        scl.x = ((float)(((PTCharacter)inst)->GetCharData())->sightmax / 100.0f) + 1.0f;
        D3DMATRIXScale(&obj->matrix, &scl);


        D3DMATRIXRotateZ(&obj->matrix, (-90.0f * (float)TORADIAN));


        // translate next: put it in front of the character
        translate_vector.y = -50.0f;
        translate_vector.x = translate_vector.z = 0.0f;
        D3DMATRIXTranslate(&obj->matrix, &translate_vector);


        // rotate last: aim it in the direction they are facing
        D3DMATRIXRotateZ(&obj->matrix, facing_angle);


        // now place it where the character's head is...
        translate_vector.x = (float)charpos.x + head_pos.x;
        translate_vector.y = (float)charpos.y + head_pos.y;
        translate_vector.z = (float)charpos.z + head_pos.z;
        D3DMATRIXTranslate(&obj->matrix, &translate_vector);

        //MultiplyD3DMATRIX(&obj->matrix, &obj->matrix, &head_matrix);

        // Set start (not an offset... starts at 1) and number of faces for texture[0]
        obj->texfaces[0] = 0;
        obj->texfaces[1] = 0;
        obj->texfaces[2] = 0;
        obj->texfaces[3] = 0;

        obj->numtexfaces[0] = 0;
        obj->numtexfaces[1] = 0;
        obj->numtexfaces[2] = 0;
        obj->numtexfaces[3] = obj->numfaces;


        // offset the texture u-coordinate so that the visor changes color appropriately
        tmp_shiftval = (float)(((PTCharacter)inst)->LastGlimpse() / 100.0f);
        if (tmp_shiftval > 1.0f) tmp_shiftval = 1.0f;
        if (tmp_shiftval < 0.0f) tmp_shiftval = 0.0f;
        
        if (visindicator_shiftval < tmp_shiftval) visindicator_shiftval += 0.033f;
        else visindicator_shiftval -= 0.033f;
        if (visindicator_shiftval > 0.999f) visindicator_shiftval = 0.999f;
        if (visindicator_shiftval < 0.001f) visindicator_shiftval = 0.001f;

        for(int k = 0; k < obj->numverts; k++)
        {
            obj->lverts[k].tu = visindicator_shiftval;
        }


        SaveBlendState();
        SetBlendState();
        utilityimagery->RenderObject(obj, 0, 0, &pos);
        RestoreBlendState();
    }
}

// *****************************************
// * WeaponSwipe Functions and Attachments *
// *****************************************

PT3DImagery TCharAnimator::GetWeaponImagery(int objnum)
{
    if (objnum == -1)
        return NULL;
    if (inst->ObjClass() != OBJCLASS_PLAYER)
        return NULL;

    PTPlayer player = (PTPlayer)inst;
    PTObjectInstance oi = player->GetEquip(EQ_PRIMEHAND);
    if (!oi)
        return NULL;
            
//      if (oi->GetImagery()->ImageryId() != OBJIMAGE_MESH3D)
//          return 0;

    PT3DImagery equipimagery = (PT3DImagery)oi->GetImagery();
    
    return equipimagery;
}

int TCharAnimator::GetWeaponNum()
{
    if (inst->ObjClass() != OBJCLASS_PLAYER)
        return -1;

    S3DAnimObj equipobj;
    memset(&equipobj, 0, sizeof(S3DAnimObj));

    PTPlayer player = (PTPlayer)inst;
    PTObjectInstance oi = player->GetEquip(EQ_PRIMEHAND);
    if (!oi)
        return -1;
            
//      if (oi->GetImagery()->ImageryId() != OBJIMAGE_MESH3D)
//          return 0;

    PT3DImagery equipimagery = (PT3DImagery)oi->GetImagery();
    PSImageryHeader equipheader = equipimagery->GetHeader();

  // Find equipment state for player's body type
    int nlen = 0;
    for (int st = 0; st < equipheader->numstates; st++)
    {
      // Objects in this state are for all chars
        if (!stricmp(equipheader->states[st].animname, "still") ||
            !stricmp(equipheader->states[st].animname, "all"))
        {
            nlen = 0;
            break;
        }

      // Has specific state for this body type
        if (!stricmp(equipheader->states[st].animname, player->BodyType()))
        {
            nlen = strlen(player->BodyType());
            break;
        }
    }

  // No geometry for player's body type!  Exit (this should never happen!)
    if (st >= equipheader->numstates)
        return -1;

  // Go through equip objects, and processes for matching player objects    
    for (int o = 0; o < equipimagery->NumObjects(); o++)
    {
        if (equipimagery->IsHidden(o, st))
            continue;

        char *n = equipimagery->GetObjectName(o);

        if (nlen > 0 && !strnicmp(n, player->BodyType(), nlen))
            n += nlen;

      // Find matching player object
        int playerobjnum = GetObjectNum(n);
        
      // Hack for weapons
        if (playerobjnum < 0 && !stricmp(n, "weapon"))
            playerobjnum = GetObjectNum("sword");

        if (playerobjnum < 0)
            continue;

    // Do the actual thing now (return weapon num)
        return o;
    }

    return -1;
}

int TCharAnimator::GetWeaponNumVerts()
{
    int weaponnum = GetWeaponNum();
    PT3DImagery imagery = GetWeaponImagery(weaponnum);
            
    if (imagery)
    {
        return imagery->NumVerts();
    }           
    else if (inst->ObjClass() != OBJCLASS_PLAYER)
    // this is a character, not a player (no weapon equiped)
    {
        weaponnum = GetObjectNum("weapon");
        if (weaponnum < 0)
            weaponnum = GetObjectNum("sword");
        if (weaponnum < 0)
            weaponnum = GetObjectNum("ogrokaxe");
        if (weaponnum >= 0)
        {
            PS3DAnimObj weaponobj = GetObject(weaponnum);
            GetVerts(weaponobj, D3DVT_LVERTEX);
            return weaponobj->numverts;
        }

    }

    return 0;
}

LPD3DLVERTEX TCharAnimator::GetWeaponVertices(int len)
{
    if (len <= 0)
        return NULL;

    int weaponnum = GetWeaponNum();
    PT3DImagery imagery = GetWeaponImagery(weaponnum);
        
    if (imagery)
    {
        LPD3DLVERTEX verts;
        verts = new D3DLVERTEX[len];
        imagery->GetObjVerts(weaponnum, verts, 0, 0, D3DVT_LVERTEX);

        return verts;
    }
    else if (inst->ObjClass() != OBJCLASS_PLAYER)
    // this is a character, not a player (no weapon equiped)
    {
        weaponnum = GetObjectNum("weapon");
        if (weaponnum < 0)
            weaponnum = GetObjectNum("sword");
        if (weaponnum < 0)
            weaponnum = GetObjectNum("ogrokaxe");
        if (weaponnum >= 0)
        {
            PS3DAnimObj weaponobj = GetObject(weaponnum);
            GetVerts(weaponobj, D3DVT_LVERTEX);
            return weaponobj->lverts;
        }
    }
    return NULL;
}

void TCharAnimator::SetupWeaponSwipe()
{
    if (!inst)
        return;

    SWeaponSwipeParams p;
    p.numverts = GetWeaponNumVerts();
    p.weaponverts = GetWeaponVertices(p.numverts);
    if (!p.weaponverts)
        return;
    
    if (inst->ObjClass() == OBJCLASS_PLAYER)
    {
        PTPlayer player = (PTPlayer)inst;
        PTObjectInstance oi = player->PrimeHand();
        if (oi)
        {
            p.primehand = oi->GetTypeName();
        }
    }
    else
        p.primehand = NULL;

    p.charanim = this;

    PSCharData chardata = ((PTCharacter)inst)->GetCharData();
    if (chardata->swipecolor.red == 0 &&
        chardata->swipecolor.green == 0 &&
        chardata->swipecolor.blue == 0)
        return;

    p.r = (D3DVALUE)(chardata->swipecolor.red) / 255.0f;
    p.g = (D3DVALUE)(chardata->swipecolor.green) / 255.0f;
    p.b = (D3DVALUE)(chardata->swipecolor.blue) / 255.0f;

    int length = 5;
    p.smooth = 8;
    p.maxsegs = length * p.smooth;
    
    weaponswipe.Init(&p);
}

void TWeaponSwipe::Init(PSWeaponSwipeParams p)
{
    initialized = TRUE;
    r = p->r;
    g = p->g;
    b = p->b;
    charanim = p->charanim;
    numverts = p->numverts;
    weaponverts = p->weaponverts;
    maxsegs = p->maxsegs;
    smooth = p->smooth;
    primehand = p->primehand;
    //eff = p->eff;
    
    if (!weaponverts)
    {
        initialized = FALSE;
        return;
    }

    obj = &theobj;
    memset(obj, 0, sizeof(S3DAnimObj));
    
    maxpoints = ((maxsegs / smooth) + 3);
    points[0] = new D3DVECTOR[maxpoints];
    points[1] = new D3DVECTOR[maxpoints];
    memset(points[0], 0, sizeof(points));
    memset(points[1], 0, sizeof(points));

    // Paraphrased GetVerts
    maxverts = (maxpoints - 1) * smooth * 2;
    obj->numverts = maxverts;   
    obj->lverts = new D3DLVERTEX[obj->numverts];
    obj->verttype = D3DVT_LVERTEX;

    // Paraphrased GetFaces
    obj->numfaces = (maxsegs * 2);
    //if (!obj->faces)
    obj->faces = new S3DFace[obj->numfaces];
    for (int i = 0; i < obj->numfaces; i++)
    {
        obj->faces[i].v1 = i;
        obj->faces[i].v2 = i + 1;
        obj->faces[i].v3 = i + 2;
    }

  // Set start and number of faces for texture 0 (no texture)   
    obj->texfaces[0] = 0;
    obj->numtexfaces[0] = obj->numfaces;

    obj->primtype = D3DPT_TRIANGLELIST;

    for (i = 0; i < maxsegs + 1; i++)
    {
        Animate();
    }

    GetWeaponExtents(); // set vweapbeg, vweapend
    //weaponmat = GetCharsWeaponMatrix(); // get a pointer the matrix of the weapon (in the character's animator)!
    NormalizeColors();  // fix ratios on r, g, b
}

void TWeaponSwipe::GenerateStrip()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;

    obj->flags = OBJ3D_MATRIX | OBJ3D_ROT1 | OBJ3D_VERTS | OBJ3D_FACES | OBJ3D_OWNSVERTS | OBJ3D_OWNSFACES;
    D3DMATRIXClear(&obj->matrix);

    float start = 0.3f, startr = min(r + 0.4f, 1.0f), startg = min(g + 0.4f, 1.0f), startb = min(b + 0.4f, 1.0f);
    float fadeoutstep = (float)(start * 2.0f / maxverts), alpha = start;
    float wfade = (float)(4.0f / maxverts), ratio = 0.0f, smoothinv = (float)(1.0f / smooth);
    D3DVECTOR avert; // your eyes
    int o, k, i, vn = 0;
    for (o = 0; o < maxpoints - 2; o++)
    {
        ratio = 0.0f;
        for (i = 0; i < smooth; i++)
        {
            for (k = 0; k < 2; k++)
            {
                spline(&avert, ratio, &points[k][max(0, o - 1)], &points[k][o], &points[k][o + 1], &points[k][o + 2]);
                obj->lverts[vn].x = avert.x;
                obj->lverts[vn].y = avert.y;
                obj->lverts[vn].z = avert.z;
                obj->lverts[vn].color = D3DRGBA(startr, startg, startb, alpha);
                vn++;
                if (startr > r)
                    startr -= wfade;
                if (startr <= r)
                    startr = r;
                if (startg > g)
                    startg -= wfade;
                if (startg <= g)
                    startg = g;
                if (startb > b)
                    startb -= wfade;
                if (startb <= b)
                    startb = b;
                alpha -= fadeoutstep;
                if (alpha < 0.0f)
                    alpha = 0.0f;
            }
            ratio += smoothinv;
        }
    }
}

void TWeaponSwipe::Animate()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;

    if (charanim->GetObjInst()->ObjClass() == OBJCLASS_PLAYER)
    {
        PTPlayer player = (PTPlayer)charanim->GetObjInst();
        PTObjectInstance oi = player->PrimeHand();
        if (oi)
        {
            char* name = oi->GetTypeName();
            if (stricmp(primehand, name)) // switched weapons! now reset the weaponswipe and all that
            {
                Close();
                return;
            }
        }
    }

    weaponmat = GetCharsWeaponMatrix(); // get a pointer the matrix of the weapon (in the character's animator)!
    D3DVECTOR beg, end;

    S3DPoint animpos;
        
    D3DMATRIXTransform(weaponmat, &vweapbeg, &beg);
    D3DMATRIXTransform(weaponmat, &vweapend, &end);
    
    CycleStrip();
    points[0][0].x = beg.x;
    points[0][0].y = beg.y;
    points[0][0].z = beg.z;
    points[1][0].x = end.x;
    points[1][0].y = end.y;
    points[1][0].z = end.z;
}

void TWeaponSwipe::GetWeaponExtents()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;
    
    int ivweapbeg = 0;
    int ivweapend = 0;
    float beg = 100000.0f, end = -100000.0f;
    for (int i = 0; i < numverts; i++)
    {
        if (weaponverts[i].z < beg)
        {
            beg = weaponverts[i].z;
            ivweapbeg = i;
        }
        if (weaponverts[i].z > end)
        {
            end = weaponverts[i].z;
            ivweapend = i;
        }
    }
    vweapbeg.x = 0.0f;
    vweapbeg.y = 0.0f;
    vweapbeg.z = 0.0f;
    //vweapbeg.x = weaponverts[ivweapbeg].x;
    //vweapbeg.y = weaponverts[ivweapbeg].y;
    //vweapbeg.z = weaponverts[ivweapbeg].z;
    vweapend.x = weaponverts[ivweapend].x;
    vweapend.y = weaponverts[ivweapend].y;
    vweapend.z = weaponverts[ivweapend].z;
}

void TWeaponSwipe::NormalizeColors()
{
    float most = max(r, max(g, b));
    if (most > 0.0f)
    {
        r /= most;
        g /= most;
        b /= most;
    }
    if (r > 1.0)
        r = 1.0;
    if (g > 1.0)
        g = 1.0;
    if (b > 1.0)
        b = 1.0;
}

void TWeaponSwipe::ChangeColor(float r, float g, float b)
{
    r = r;
    g = g;
    b = b;
    NormalizeColors();
}

void TWeaponSwipe::CycleStrip()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;

    for (int o = 0; o < 2; o++)
    {
        for (int i = maxpoints - 1; i > 0; i--)
        {
            points[o][i].x = points[o][i - 1].x;
            points[o][i].y = points[o][i - 1].y;
            points[o][i].z = points[o][i - 1].z;
        }
    }
}

void TWeaponSwipe::Render()
{
    if (!obj)
        return;
    if (obj->numverts == 0 || !initialized)
        return;

    SaveBlendState();

    // set the new render flags
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE));
    TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, FALSE));

    //charanim->ResetExtents();
    GenerateStrip();

    charanim->RenderObject(obj);
    //charanim->UpdateExtents();

    RestoreBlendState();
}

D3DMATRIX* TWeaponSwipe::GetCharsWeaponMatrix()
{
    int weaponnum;
    PS3DAnimObj weaponobj;

    weaponnum = charanim->GetObjectNum("weapon");

  // This is a hack here!
    if (weaponnum < 0)
        weaponnum = charanim->GetObjectNum("sword");
    if (weaponnum < 0)
        weaponnum = charanim->GetObjectNum("ogrokaxe");

    if (weaponnum >= 0)
    {
        D3DMATRIX pos;
        charanim->MakeMatrix(&pos);                 // Get character position
        weaponobj = charanim->GetObject(weaponnum);
        PTCharacter inst = (PTCharacter)charanim->GetObjInst();
        charanim->Get3DImagery()->CalcObjectMatrix(weaponobj, ((PTObjectInstance)inst)->GetState(), inst->GetFrame(), &pos);
        return &weaponobj->matrix;
    }
    else 
        return NULL;
}

void TWeaponSwipe::Close()
{
    initialized = FALSE;

    delete points[0];
    delete points[1];
}

