// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                     helper.cpp - THelper module                       *
// *************************************************************************

#include <windows.h>

#include "revenant.h"
#include "helper.h"
#include "3dimage.h"
#include "mappane.h"

REGISTER_BUILDER(THelper)

TObjectClass HelperClass("HELPER", OBJCLASS_HELPER, 0);

// *******************
// * Helper Animator *
// *******************

_CLASSDEF(THelperAnimator)
class THelperAnimator : public T3DAnimator
{
  public:
    THelperAnimator(PTObjectInstance oi) : T3DAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~THelperAnimator() { Close(); }
      // Call close function

    virtual void SetupObjects();
      // Sets up objects
    virtual BOOL Render();
      // Renders objects
};

REGISTER_3DANIMATOR("Box", THelperAnimator)

// Sets up the objects to animate (simply uses whatever objects are in the imagery)
// Override this function to set up whatever objects you need for your effect, etc.
void THelperAnimator::SetupObjects()
{
    PS3DAnimObj o = NewObject(0, OBJ3D_VERTS | OBJ3D_COPYVERTS | OBJ3D_POS1);
    AddObject(o);
}

BOOL THelperAnimator::Render()
{
    PS3DAnimObj obj = GetObject(0);

    ResetExtents();

    int width, length, height;
    image->GetWorldBoundBox(state, width, length, height);

    obj->pos.x = -(float)((((float)width / 2.0) - (float)image->GetWorldRegX(state)) * (float)GRIDSIZE);
    obj->pos.y = -(float)((((float)length / 2.0) - (float)image->GetWorldRegY(state)) * (float)GRIDSIZE);
    obj->pos.z = -(float)((((float)height / 2.0) - (float)image->GetWorldRegZ(state)) * (float)GRIDSIZE);

    for (int i = 0; i < obj->numverts; i++)
    {
        D3DVERTEX *v = &(((D3DVERTEX *)obj->verts)[i]);

        if (v->x)
        {
            v->x = (float)((float)width / 2.0 * (float)GRIDSIZE);
            if (v->x < 0)
                v->x *= -1;
        }
        if (v->y)
        {
            v->y = (float)((float)length / 2.0 * (float)GRIDSIZE);
            if (v->y < 0)
                v->y *= -1;
        }
        if (v->z)
        {
            v->z = (float)((float)height / 2.0 * (float)GRIDSIZE);
            if (v->z < 0)
                v->z *= -1;
        }
    }

    RenderObject(obj);

    UpdateExtents();
    
    return TRUE;
}
