// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             effectcomp.cpp - Effect components module                 *
// *************************************************************************

#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>
#include <math.h>
#include "d3dmacs.h"
#include "d3dmath.h"

#include "revenant.h"
#include "3dscene.h"
#include "3dimage.h"
#include "playscreen.h"
#include "object.h"
#include "mappane.h"       
#include "character.h"
#include "parse.h"


// **************************************************************************
// * TScrollTexController - Scrolls a texture for an object given delta u,v *
// **************************************************************************

// TAG FORMAT
//
//      scrolltex:obj=<objlist>,du=<udelta>,dv=<vdelta>
//
//      <objlist> = Single object, or list of objects surrounded by ()
//      <udelta>  = U texture delta floating point value
//      <vdelta>  = V texture delta floating point value

struct UV
{
    float u, v;
};

_CLASSDEF(TScrollTexController)
class TScrollTexController : public T3DController
{
  private:
    float du, dv;           // Floating point delta u and v
    UV **olduv;             // Original uv's for object

  protected:
    virtual BOOL ParseItem(char *param, TToken &t);
      // Parses a parameter item

  public:
    TScrollTexController(int s, int f, PT3DAnimator a, PT3DImagery i, PTObjectInstance o) :
      T3DController(s, f, a, i, o) {}
      // Constructor
    virtual ~TScrollTexController() { Close(); }
      // Destructor
    virtual BOOL Initialize(char *params);
      // Initialize the controller
    virtual void Close();
      // Closes the controller
    virtual void Render();
      // Called during render phase to render controller objects
};

REGISTER_3DCONTROLLER("scrolltex", TScrollTexController)

BOOL TScrollTexController::ParseItem(char *param, TToken &t)
{
    double d;

    if (!stricmp(param, "du"))
    {
        if (!Parse(t, "%f", &d))
            return FALSE;
        du = (float)d;
    }
    else if (!stricmp(param, "dv"))
    {
        if (!Parse(t, "%f", &d))
            return FALSE;
        dv = (float)d;
    }
    else
        return T3DController::ParseItem(param, t);

    return TRUE;
}

BOOL TScrollTexController::Initialize(char *params)
{
    if (!T3DController::Initialize(params))
        return FALSE;

  // Add all objects by default
    if (animobjs.NumItems() <= 0)
    {
        for (int c = 0; c < animator->NumObjects(); c++)
            animobjs.Add(animator->GetObject(c));
    }

  // No objects??
    if (animobjs.NumItems() <= 0)
        return FALSE;

  // Get UV's pointer array
    olduv = new UV*[animobjs.NumItems()];

  // Override vertices for this object
    for (int o = 0; o < animobjs.NumItems(); o++)
    {
        PS3DAnimObj obj = animobjs[o];

        animator->GetVerts(obj);
        olduv[o] = new UV[obj->numverts];

      // Save original uv's for this object
        LPD3DVERTEX v = obj->verts;
        UV *uv = olduv[o];
        for (int c = 0; c < obj->numverts; c++, v++, uv++)
        {
            uv->u = v->tu;
            uv->v = v->tv;
        }
    }
    
    return TRUE;
}   

void TScrollTexController::Close()
{
    for (int o = 0; o < animobjs.NumItems(); o++)
    {
        delete olduv[o];
    }
    delete olduv;

    T3DController::Close();
}
      
void TScrollTexController::Render()
{
  // Don't animate until first frame is reached
    if (inst->GetFrame() < tagframe)
        return;

  // This little trick keeps all textures synchronized everywhere always
    float f = (float)PlayScreen.FrameCount();
    float newdu = du * f;
    float newdv = dv * f;

  // Scroll texture vertices for each object
    for (int o = 0; o < animobjs.NumItems(); o++)
    {
        PS3DAnimObj obj = animobjs[o];

      // Add new deltau and deltav to original u and v for each object
        LPD3DVERTEX v = obj->verts;
        UV *uv = olduv[o];
        for (int c = 0; c < obj->numverts; c++, v++, uv++)
        {
            v->tu = uv->u + newdu;
            v->tv = uv->v + newdv;
        }
    }
}           

// ******************************************************************
// * TAnimTexController - Animates texture from a uv grid of frames *
// ******************************************************************

// TAG FORMAT
//
//      animtex:obj=<objlist>,u=<uframes>,v=<vframes>
//
//      <objlist> = Single object, or list of objects surrounded by ()
//      <uframes>  = Number of frame grids in u direction
//      <vframes>  = Number of frame grids in v direction

_CLASSDEF(TAnimTexController)
class TAnimTexController : public T3DController
{
  private:
    int ugrid, vgrid;       // Number of frames in u and v directions
    float usize, vsize;     // Size of each frame in u and v space
    UV **olduv;             // Original uv's for object

  protected:
    virtual BOOL ParseItem(char *param, TToken &t);
      // Parses a parameter item

  public:
    TAnimTexController(int s, int f, PT3DAnimator a, PT3DImagery i, PTObjectInstance o) :
      T3DController(s, f, a, i, o) {}
      // Constructor
    virtual ~TAnimTexController() { Close(); }
      // Destructor
    virtual BOOL Initialize(char *params);
      // Initialize the controller
    virtual void Close();
      // Closes the controller
    virtual void Render();
      // Called during render phase to render controller objects
};

REGISTER_3DCONTROLLER("animtex", TAnimTexController)

BOOL TAnimTexController::ParseItem(char *param, TToken &t)
{
    if (!stricmp(param, "u"))
    {
        if (!Parse(t, "%i", &ugrid))
            return FALSE;
        usize = 1.0f / (float)ugrid;
    }
    else if (!stricmp(param, "v"))
    {
        if (!Parse(t, "%i", &vgrid))
            return FALSE;
        vsize = 1.0f / (float)vgrid;
    }
    else
        return T3DController::ParseItem(param, t);

    return TRUE;
}

BOOL TAnimTexController::Initialize(char *params)
{
    if (!T3DController::Initialize(params))
        return FALSE;

  // Add all objects by default
    if (animobjs.NumItems() <= 0)
    {
        for (int c = 0; c < animator->NumObjects(); c++)
            animobjs.Add(animator->GetObject(c));
    }

  // No objects??
    if (animobjs.NumItems() <= 0)
        return FALSE;

  // Get UV's pointer array
    olduv = new UV*[animobjs.NumItems()];

  // Override vertices for this object
    for (int o = 0; o < animobjs.NumItems(); o++)
    {
        PS3DAnimObj obj = animobjs[o];

        animator->GetVerts(obj);
        olduv[o] = new UV[obj->numverts];

      // Save original uv's for this object
        LPD3DVERTEX v = obj->verts;
        UV *uv = olduv[o];
        for (int c = 0; c < obj->numverts; c++, v++, uv++)
        {
            uv->u = v->tu;
            uv->v = v->tv;
        }
    }
    
    return TRUE;
}   

void TAnimTexController::Close()
{
    for (int o = 0; o < animobjs.NumItems(); o++)
    {
        delete olduv[o];
    }
    delete olduv;

    T3DController::Close();
}
      
void TAnimTexController::Render()
{
  // Don't animate until first frame is reached
    if (inst->GetFrame() < tagframe)
        return;

  // This little trick keeps all textures synchronized everywhere always
    int f = PlayScreen.FrameCount() % (ugrid * vgrid);
    int u = f % ugrid;
    int v = f - (u * ugrid);
    float newdu = usize * (float)u;
    float newdv = vsize * (float)v;

  // Scroll texture vertices for each object
    for (int o = 0; o < animobjs.NumItems(); o++)
    {
        PS3DAnimObj obj = animobjs[o];

      // Add new deltau and deltav to original u and v for each object
        LPD3DVERTEX v = obj->verts;
        UV *uv = olduv[o];
        for (int c = 0; c < obj->numverts; c++, v++, uv++)
        {
            v->tu = uv->u + newdu;
            v->tv = uv->v + newdv;
        }
    }
}           

