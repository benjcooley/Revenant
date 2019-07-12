// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 animimage.cpp - TAnimImagery module                   *
// *************************************************************************

#include "revenant.h"
#include "animimage.h"
#include "graphics.h"
#include "animation.h"
#include "bitmap.h"
#include "surface.h"
#include "display.h"
#include "mappane.h"
#include "playscreen.h"
#include "inventory.h"

REGISTER_IMAGERYBUILDER(TAnimImagery);

BOOL TAnimImagery::AlwaysOnTop(PTObjectInstance oi)
{
    int state = oi->GetState();

    if (state >= NumStates())
        return FALSE;

    PTBitmap bm = oi->GetStillImage();
    if (!bm)
        return FALSE;

    if (bm->flags & BM_ZBUFFER)
        return FALSE;

    return (GetImageFlags(state) & ANIIM_LIT) != 0;
}

void TAnimImagery::CacheImagery()
{
    for (int state = 0; state < NumStates(); state++)
    {
        PTBitmap bm = GetStillImage(state);
        if (bm)
            bm->CacheChunks();
    }
}

void TAnimImagery::DrawLit(PTObjectInstance oi, PTSurface surface)
{
    int state = oi->GetState();

    if (state >= NumStates() || (oi->GetFlags() & OF_SELDRAW))
        return;

    /*
    if (GetImageFlags(state) & ANIIM_CUTOFF && !Editor)
    {
        if (SmoothScroll)
            return;             // never show trees on smoothscrolling mode

        // make sure the bitmap is cut off at the top of the screen
        SRect r;
        oi->GetScreenRect(r);
        if (r.top > MapPane.GetScrollY())
            return;
    }
    */

    PTBitmap bm = oi->GetStillImage();

    DWORD flags = DM_TRANSPARENT;
    if (oi->GetFlags() & OF_DRAWFLIP)
        flags |= DM_REVERSEHORZ;

    if (GetImageFlags(state) & ANIIM_LIT && bm)
    {
        S3DPoint screenpos;
        oi->GetScreenPos(screenpos);

        if (bm->flags & BM_ALPHA)
            flags |= DM_ALPHA;

        if (bm->flags & BM_ZBUFFER)
        {
            int dim = 0;
            flags |= DM_ZBUFFER;
            surface->ZPutDim(screenpos.x - GetRegX(state),
                             screenpos.y - GetRegY(state),
                             screenpos.z - GetRegZ(state), bm, flags, dim);
        }
        else
            surface->Put(screenpos.x - GetRegX(state), screenpos.y - GetRegY(state), bm, flags);
    }
}       

void TAnimImagery::DrawUnlit(PTObjectInstance oi, PTSurface surface)
{
    int state = oi->GetState();

    if (state >= NumStates() || (oi->GetFlags() & OF_SELDRAW))
        return;

    /*
    if (GetImageFlags(state) & ANIIM_CUTOFF && !Editor)
    {
        if (SmoothScroll)
            return;             // never show trees on smoothscrolling mode

        // make sure the bitmap is cut off at the top of the screen
        SRect r;
        oi->GetScreenRect(r);
        if (r.top > MapPane.GetScrollY())
            return;
    }
    */

    PTBitmap bm = oi->GetStillImage();
    if (!bm)
        return;

    S3DPoint screenpos;
    oi->GetScreenPos(screenpos);

    DWORD flags = DM_TRANSPARENT;

    if (bm->flags & BM_ALPHA)
        flags |= DM_ALPHA;
    else
    {
        if (bm->flags & BM_ZBUFFER)
            flags |= DM_ZBUFFER; 
        else
            flags |= DM_ZSTATIC;
    }

    if (bm->flags & BM_NORMALS && !NoNormals)
        flags |= DM_NORMALS; 
    
    if (oi->GetFlags() & OF_DRAWFLIP)
        flags |= DM_REVERSEHORZ;

    if (oi->GetFlags() & OF_REVEAL)
        flags |= DM_SHUTTER;

    if (GetImageFlags(state) & ANIIM_UNLIT && bm)
        surface->ZPut(screenpos.x - GetRegX(state),
                      screenpos.y - GetRegY(state),
                      screenpos.z - GetRegZ(state), bm, flags);
}

BOOL TAnimImagery::GetZ(PTObjectInstance oi, PTSurface surface)
{
    int state = oi->GetState();

    if (state >= NumStates())
        return FALSE;

    /*
    if (GetImageFlags(state) & ANIIM_CUTOFF && !Editor)
    {
        if (SmoothScroll)
            return FALSE;               // never show trees on smoothscrolling mode

        // make sure the bitmap is cut off at the top of the screen
        SRect r;
        oi->GetScreenRect(r);
        if (r.top > MapPane.GetScrollY())
            return FALSE;
    }
    */

    PTBitmap bm = oi->GetStillImage();
    if (!bm)
        return FALSE;

    S3DPoint screenpos;
    oi->GetScreenPos(screenpos);

    DWORD stateflags = GetImageFlags(state);

    if (stateflags & ANIIM_LIT)
    {
        if (!(bm->flags & BM_ZBUFFER))
            return TRUE;
    }
    else if (!(stateflags & ANIIM_UNLIT))
        return Editor;

    DWORD flags = DM_TRANSPARENT;

    if (bm->flags & BM_ZBUFFER)
        flags |= DM_ZBUFFER;
    else if (bm->flags & BM_ALPHA)
        return Editor;
    else    
        flags |= DM_ZSTATIC;

    if (oi->GetFlags() & OF_DRAWFLIP)
        flags |= DM_REVERSEHORZ;

    return surface->ZFind(screenpos.x - GetRegX(state),
                          screenpos.y - GetRegY(state),
                          screenpos.z - GetRegZ(state), bm, flags);
}

void TAnimImagery::DrawSelected(PTObjectInstance oi, PTSurface surface)
{
    int state = oi->GetState();

    // find selected imagery, if any
    for (int i = 0; i < NumStates(); i++)
    {
        if (GetImageFlags(i) & ANIIM_SELECTED)
        {
            state = i;
            break;
        }
    }

    if (state >= NumStates())
        return;

    PTBitmap bm = oi->GetStillImage(state);

    if (!bm)
        return;

    S3DPoint screenpos;
    oi->GetScreenPos(screenpos);

    DWORD flags = DM_TRANSPARENT;
    if (oi->GetFlags() & OF_DRAWFLIP)
        flags |= DM_REVERSEHORZ;

    if (bm->flags & BM_ALPHA)
        flags |= DM_ALPHA;

    if (bm->flags & BM_ZBUFFER)
        flags |= DM_ZBUFFER;
    else
        flags |= DM_ZSTATIC;

    if (!(oi->GetFlags() & OF_SELDRAW))
    {
        if (!(GetImageFlags(state) & ANIIM_SELECTED))
            flags |= DM_SELECTED;

        surface->ZPut(screenpos.x - GetRegX(state),
                      screenpos.y - GetRegY(state),
                      screenpos.z - GetRegZ(state), bm, flags);
    }
    else
    {
        if (NoScrollZBuffer)
        {
            flags &= ~(DM_ZBUFFER | DM_ZSTATIC);
            surface->Put(screenpos.x - GetRegX(state),
                         screenpos.y - GetRegY(state), bm, flags);
        }
        else
        {
            surface->ZPut(screenpos.x - GetRegX(state),
                          screenpos.y - GetRegY(state),
                          screenpos.z - GetRegZ(state), bm, flags);
        }
    }
}

BOOL TAnimImagery::SaveBitmap(char *path, int state, BOOL zbuffer)
{
    char basename[FILENAMELEN];
    char name[FILENAMELEN];

    strcpy(basename, path);
    char *ptr = strchr(basename, '.');
    if (ptr)
        *ptr = NULL;

    if (state >= NumStates())
        return FALSE;

    PTBitmap bm = GetStillImage(state);

    int flags;
    if (Display->BitsPerPixel() == 16)
        flags = BM_16BIT;
    else
        flags = BM_15BIT;

    PTBitmap bigbm = TBitmap::NewBitmap(640, 480, flags | BM_ZBUFFER);

    SColor c;
    c.red = c.green = c.blue = 0;
    bigbm->Clear(c);

    bigbm->ZPut((640 - bm->width) / 2, (480 - bm->height) / 2, 0, bm, DM_ZBUFFER);

    strcpy(name, basename);
    strcat(name, ".BMP");
    if (!bigbm->SaveBMP(name))
    {
        delete bigbm;
        return FALSE;
    }

    if (zbuffer)
    {
        strcpy(name, basename);
        strcat(name, ".ZBF");
        if (!bigbm->SaveZBF(name))
        {
            delete bigbm;
            return FALSE;
        }
    }

    delete bigbm;

    return TRUE;
}

BOOL TAnimImagery::NeedsAnimator(PTObjectInstance oi)
{
    int state = oi->GetState();

    if (state < NumStates() && (GetAnimation(state) != NULL ||
        (oi->IsInInventory() && GetInvAnimation(state) != NULL)))
        return TRUE;

    return FALSE;
}

PTObjectAnimator TAnimImagery::NewObjectAnimator(PTObjectInstance oi)
{
    return (PTObjectAnimator)new TAnimAnimator(oi);
}

// *********************************
// * Anim Object Animator Funtions *
// *********************************

TAnimAnimator::TAnimAnimator(PTObjectInstance oi) : TObjectAnimator(oi) 
{
}

void TAnimAnimator::Animate(BOOL draw)
{
    if (state != inst->GetState() && inst->GetState() < inst->NumStates())
    {
        if (((PTAnimImagery)image)->GetAnimation(inst->GetState()) != NULL &&
            ((PTAnimImagery)image)->GetStillImage(inst->GetState()) == NULL)
        {
            inst->SetFlags(OF_MOVING);
        }
        else
        {
            inst->ResetFlags(inst->GetFlags() & ~(OF_MOVING));
        }
    }

    TObjectAnimator::Animate(draw);

    if (draw && !complete)
    {
        PTAnimation anim;

        if (inst->IsInInventory())
            anim = (PTAnimation)(image->GetInvAnimation(state));
        else
            anim = (PTAnimation)(image->GetAnimation(state));

        if (anim == NULL)
            return;

        PTBitmap bm = anim->GetFrame(frame);
        if (bm)
        {
            if (inst->IsInInventory())
                Inventory.DrawAnim(inst, bm);
            else
            {
                DWORD drawmode = DM_TRANSPARENT | DM_USEREG;

                if (bm->flags & BM_ALPHA)
                    drawmode |= DM_ALPHA;

                S3DPoint pos, spos;
                inst->GetPos(pos);
                WorldToScreen(pos, spos);
                spos.x += image->GetAnimRegX(state);
                spos.y += image->GetAnimRegY(state);
                spos.z -= image->GetAnimRegZ(state);

                if (NoScrollZBuffer)
                    MapPane.DrawRestoreRect(spos.x, spos.y, bm->width, bm->height, DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER | DM_NODRAW);

                if (bm->flags & BM_ZBUFFER)
                    Display->ZPut(spos.x, spos.y, spos.z, bm, drawmode | DM_ZBUFFER);
                else
                    PlayScreen.AddPostCharAnim(spos.x, spos.y, spos.z, bm, drawmode | DM_ZSTATIC);
            }
        }
    }
}

TAnimAnimator::~TAnimAnimator()
{
    if (inst->GetState() < inst->NumStates() &&
        ((PTAnimImagery)image)->GetStillImage(inst->GetState()) != NULL)
    {
        inst->ResetFlags(inst->GetFlags() & ~(OF_MOVING));
    }
}

