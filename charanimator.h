// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   charanimator.h - TCharAnimator module               *
// *************************************************************************

#ifndef _CHARANIMATOR_H
#define _CHARANIMATOR_H

#ifndef _WINDOWS_
#error WINDOWS.H Must be included at the top of your .CPP file
#endif

#include "3dimage.h"

_CLASSDEF(TWeaponSwipe)
_CLASSDEF(TCharAnimator)

// weapon shtuffs

_STRUCTDEF(SWeaponSwipeParams)
struct SWeaponSwipeParams
{
    LPD3DLVERTEX weaponverts;   // vertices of weapon, used for extents
    int numverts;               // number of vertices in weaponverts
    PTCharAnimator charanim;    // animator of character this swipe is for
    float r, g, b;              // color (to be normalized) of swipe
    int maxsegs;                // number of segments/length of swipe
    int smooth;                 // number of triangles per frame such that maxsegs % smooth == 0
    char* primehand;                // using this weapon...(to test for change of weapon)
};

class TWeaponSwipe
{
  private:
    S3DAnimObj theobj;
    PS3DAnimObj obj;
    int maxpoints, maxverts;        // Maximum number of points we can have, max verts
    D3DVECTOR *points[2];   // The points themselves points[0] = hilt, points[1] = tip or vv
    D3DVECTOR vweapbeg, vweapend; // vertex of hilt, tip of weapon
    D3DMATRIX* weaponmat; // translation table for weapon
    BOOL initialized;   // is this real
    // duplicate los parameteros
    LPD3DLVERTEX weaponverts;   // vertices of weapon, used for extents
    int numverts;               // number of vertices in weaponverts
    PTCharAnimator charanim;    // animator of character this swipe is for
    float r, g, b;              // color (to be normalized) of swipe
    int maxsegs;                // number of segments/length of swipe
    int smooth;                 // number of triangles per frame such that maxsegs % smooth == 0
    char* primehand;                // using this weapon...(to test for change of weapon)

  public:
    TWeaponSwipe() { initialized = FALSE; maxpoints = 0; primehand = NULL; }
    virtual ~TWeaponSwipe() { }

    void Init(PSWeaponSwipeParams p);
    void GenerateStrip();
    void Animate();
    void CycleStrip();
    void GetWeaponExtents();
    void NormalizeColors();
    void ChangeColor(float r, float g, float b);
    void Render();
    void Close();
    D3DMATRIX* GetCharsWeaponMatrix();
    BOOL GetInitialized() { return initialized; }
    PTCharAnimator GetCharAnim() { return charanim; }
};




// *****************
// * TCharAnimator *
// *****************

class TCharAnimator : public T3DAnimator
{
  public:
    TCharAnimator(PTObjectInstance oi);
    virtual ~TCharAnimator();

    virtual void Animate(BOOL draw);
    virtual BOOL Render();
    PTWeaponSwipe GetWeaponSwipe() { return &weaponswipe; }
      // get the weaponswipe

    // WeaponSwipe handling stuff
    void SetupWeaponSwipe();
      // Called from SetupObjects() to init the weaponswipe!
    LPD3DLVERTEX GetWeaponVertices(int len);
      // Called from SetupWeaponSwipe to get the correct weapon vertices
    int GetWeaponNumVerts();
      // Called from SetupWeaponSwipe to get the correct number of weapon vertices
    PT3DImagery GetWeaponImagery(int objnum);
      // called from GetWeaponVertices and GetWeaponNumVerts
    int GetWeaponNum();
      // called from GetWeaponVertices and GetWeaponNumVerts

  protected:

  // Poison functions
    void InitPoisonColor();
      // Initializes poison material
    void ClosePoisonColor();
      // Clears poison material
    void SetPoisonColor();
      // Sets poison color for character

  // Vision Indicator (to show the player in sneak mode how close he is to making himself known)
    void RenderVisionIndicator();

  // Equipment rendering functions
    void ProcessEquipment(int task);
      // Iterates through equipment list for player and hides char parts or renders equip
    void HideCharParts();
      // Hides the character parts that will be replaced by the equipment
      // Calls ProcessEquipment()
    void RenderEquipment();
      // Renders the equipment parts (
      // Calls ProcessEquipment()

  // Transparency functions
    void InitTransparency();
      // Sets initial transparency level for character
    void UpdateTransparency();
      // Updates the character's transparency each frame
    void SetMaterialTransparency(PT3DImagery img);
      // Sets the transparency for a given imagery (all materials) based on current
      // transparency level
    void ResetMaterialTransparency(PT3DImagery img);
      // Resets all imagery transparency back to 1.0

  // Utility Imagery stuff (shadow, combat flashes, bloody chunks, etc.)
    void InitUtilityImagery();
    void CloseUtilityImagery();
    void RenderShadow();
    void RenderCombatFlashes();
    void RenderBloodyChunks();

  // cool function (like T3DAnimator::NewObject except uses imagery passed to it!
    PS3DAnimObj GetNewImObject(PT3DImagery imagery, int objnum, int flags = 0);
    void GetImFaces(PT3DImagery imagery, PS3DAnimObj obj);
    void GetImVerts(PT3DImagery imagery, PS3DAnimObj obj, D3DVERTEXTYPE verttype);

    TWeaponSwipe weaponswipe;               // swipe structure!
    PT3DImagery utilityimagery;

    float *origmatred, *origmatgreen;       // saved material values
    int oldpoison;                          // update only when needed
    float transparency;                     // Current transparency level for character
    float visindicator_shiftval;
};

_CLASSDEF(TPlayerAnimator)
class TPlayerAnimator : public TCharAnimator
{
  public:
    TPlayerAnimator(PTObjectInstance oi) : TCharAnimator(oi) {}
    virtual ~TPlayerAnimator()              {}
};

#endif
