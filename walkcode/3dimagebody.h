// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 3dimagebody.h - S3dImageryBody object                 *
// *************************************************************************

#ifndef _3DIMAGEBODY_H
#define _3DIMAGEBODY_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _IMAGERES_H
#include "imageres.h"
#endif

// To-do list for 3D imagery:
// - Alpha channel textures
// - Morph animations
// - Texture states 
// - Object replacement (for weaponry mainly)
// - Dynamicly set size etc of various objects (spell effects)

// ******************
// * S3DImageryBody *
// ******************

// Stores the loadable data for a 3d image resource.  This data is
// copied into internal mesh structures in the imagery object, then
// discarded.

// Maximimums: Note: these maximums are only used in the exporter, and the Textures variables
// for the 3DAnimObj structure.  All other 3D system arrays are unlimited.
// NOTE: EVENTUALLY ALL SYSTEM ARRAYS WILL BE UNLIMITED!

#define MAXOBJECTS          64
#define MAXMATERIALS        8
#define MAXTEXTURES         8
#define MAXPLAYSOUNDS       128

typedef TOffset<D3DVERTEX> OD3DVERTEX;
typedef TOffset<D3DMATERIAL> OD3DMATERIAL;

typedef TOffset<D3DVERTEX> OD3DVERTEX;          // Offset to an array of vertices
typedef TOffset<OD3DVERTEX> OOD3DVERTEX;        // Offset to an array of offsets to array of vertices (Frame array) 
typedef TOffset<OOD3DVERTEX> OOOD3DVERTEX;      // Offset to an array of offsets to array of offsets to vertex array (State array)

_STRUCTDEF(S3DFace)
struct S3DFace
{
    WORD v1, v2, v3;
};

#define ANIKEY_POSSCALE  (4.0)                      // Object pos's must be in range 0-512
#define ANIKEY_ANGSCALE  (256.0 / 3.14159265359)    // Angles must be in range -PI to PI

_STRUCTDEF(SAniKey)
struct SAniKey
{
    int data[2];
//  int x : 12; // Use macros below!!
//  int y : 12;
//  int z : 12;
//  int rx : 9;
//  int ry : 9;
//  int rz : 9;
//  int dummy : 1; // So equals 64 bits
};

#define ak_x(ak)  ((((ak).data[0] >>  0) & 0x00000FFF) - 0x800)
#define ak_y(ak)  ((((ak).data[0] >> 12) & 0x00000FFF) - 0x800)
#define ak_z(ak) (((((ak).data[0] >> 24) & 0x000000FF)) | ((((ak).data[1] << 8) & 0x00000F00) - 0x800))
#define ak_rx(ak) ((((ak).data[1] >> 4) & 0x000001FF) - 0x100)
#define ak_ry(ak) ((((ak).data[1] >> 13) & 0x000001FF) - 0x100)
#define ak_rz(ak) ((((ak).data[1] >> 22) & 0x000001FF) - 0x100)

#define ak_setx(ak, d)  (ak).data[0] = (((ak).data[0] & 0xFFFFF000) | ((((int)(d) + 0x800) & 0xFFF) << 0))
#define ak_sety(ak, d)  (ak).data[0] = (((ak).data[0] & 0xFF000FFF) | ((((int)(d) + 0x800) & 0xFFF) << 12))
#define ak_setz(ak, d)  (ak).data[0] = (((ak).data[0] & 0x00FFFFFF) | ((((int)(d) + 0x800) & 0x0FF) << 24));\
                        (ak).data[1] = (((ak).data[1] & 0xFFFFFFF0) | ((((int)(d) + 0x800) & 0xF00) >> 8))
#define ak_setrx(ak, d) (ak).data[1] = (((ak).data[1] & 0xFFFFE00F) | ((((int)(d) + 0x100) & 0x1FF) << 4))
#define ak_setry(ak, d) (ak).data[1] = (((ak).data[1] & 0xFFC01FFF) | ((((int)(d) + 0x100) & 0x1FF) << 13))
#define ak_setrz(ak, d) (ak).data[1] = (((ak).data[1] & 0x003FFFFF) | ((((int)(d) + 0x100) & 0x1FF) << 22))

_STRUCTDEF(SMotionData)
struct SMotionData
{
    union
    {
      DWORD data;           // use defines below
      struct
      {
        unsigned int dist : 16;
        unsigned int face : 8;
        unsigned int ang : 8;
      };
    };

};

#define md_dist(md)         (((md).data & 0x0000FFFF) << 8)
#define md_ang(md)          ((md).data >> 24)
#define md_face(md)         (((md).data & 0x00FF0000) >> 16)

#define md_setdist(md, d)   ((md).data = ((md).data & 0xFFFF0000) | ((int)(d >> 8) & 0x0000FFFF))
#define md_setang(md, a)    ((md).data = ((md).data & 0x00FFFFFF) | (((int)(a) & 0xFF) << 24))
#define md_setface(md, a)   ((md).data = ((md).data & 0xFF00FFFF) | (((int)(a) & 0xFF) << 16))

// Old motion data without facing deltas
#define md_old_dist(md)         ((md).data & 0x00FFFFFF)
#define md_old_ang(md)          ((md).data >> 24)

#define md_old_setdist(md, d)   ((md).data = ((md).data & 0xFF000000) | ((int)(d) & 0x00FFFFFF))
#define md_old_setang(md, a)    ((md).data = ((md).data & 0x00FFFFFF) | (((int)(a) & 0xFF) << 24))

_STRUCTDEF(S3DImageryTexture)
struct S3DImageryTexture
{
    DDSURFACEDESC desc;                 // Direct draw surface desc for texture
    OFFSET bits;                        // Offset to array of texture frame offsets 
    OFFSET pals;                        // Texture palette (if not null, 1 per texture)
    int frames;                         // Number of texture frames
};

_STRUCTDEF(S3DImageryObjectTexture)
struct S3DImageryObjectTexture
{
    WORD facepos;
    WORD facenum;
};

_STRUCTDEF(S3DImageryObjectState)
struct S3DImageryObjectState
{
    int parent;                     // Contains object hierarchy for state where
                                    // Hierarchy is array of signed byte indexes for each
                                    // obj indicating its parent, -1 means root};
    OSAniKey anikeys;               // Offset to array of SAniKeys for this object's state
};

_STRUCTDEF(S3DImageryObject)
struct S3DImageryObject
{
    char name[RESNAMELEN];
    WORD material;
    WORD vertpos;                       // Position of object verts in vert array
    WORD vertnum;                       // Num object verts in vert array
    OS3DImageryObjectTexture textures;  // Offset to array of S3DImageryObjectTextures for
                                        // each texture in imagery
    OS3DImageryObjectState states;      // Offset to array of S3DImageryObjectState for
                                        // each state in imagery
};

_STRUCTDEF(S3DImageryState)
struct S3DImageryState
{
    int         invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSMotionData motion;                // Offset to motion data for this state
};

_STRUCTDEF(S3DImageryPlaySound)
struct S3DImageryPlaySound
{
    int state;                          // State to play sound at
    int frame;                          // Frame number to play sounds at
    BOOL mount;                         // Should we mount this sound when imagery is loaded
    char sounds[SOUNDLISTLEN];          // List of comma delimited sounds to randomly play
};

#define VERSION3DIMAGEBODY 1

#define I3D_ISMORPH      1  // If this flag is set, the vertex array will have multiple images
                            // stored sequentially for each object state, otherwise there will
                            // be only 1 vertex image buffer for object state [0] which will be
                            // used for all animations

#define I3D_HASICONS     2  // True if imagery has icons

#define I3D_HASHIERARCHY 4  // True if imagery has a hierarchy

#define I3D_FACINGMOTION 8  // Has facing motion data (stores hip facing deltas)

#define I3D_3DIMAGEBODY2 16 // Uses 3D image body 2 header format

#define I3D_HASPLAYSOUND 32 // Has playsound structure

// *************** OLD 3DImagery Structures ****************

// Old version of icon structure
_STRUCTDEF(S3DStateImagery)
struct S3DStateImagery
{
    int         invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
};

// OLD Version of S3DImagery (SUCKS)
#define OLDMAXOBJSTATES         64
#define OLDMAXOBJECTS           64
#define OLDMAXMATERIALS         8
#define OLDMAXTEXTURES          8
_STRUCTDEF(SOld3DImageryBody)
struct SOld3DImageryBody : public SImageryBody
{
    DWORD flags;                        // If true, verts array points to multple vert images

    int numverts;                       // Verts stored in object order...
    OFFSET verts[OLDMAXOBJSTATES];          // There will be 'frames' number of sequentially stored 

    int numfaces;                       // vert images per state if morph true
    OFFSET faces;                       // Faces stored by texture, then object order

    int nummaterials;                    
    D3DMATERIAL material[OLDMAXMATERIALS];

    int numtextures;
    DDSURFACEDESC texturedesc[OLDMAXTEXTURES];
    OFFSET texturebits[OLDMAXTEXTURES]; // Offset to array of texture frame offsets 
    OFFSET texturepals[OLDMAXTEXTURES]; // Texture palette (if not null, 1 per texture)
    int textureframes[OLDMAXTEXTURES];      // Number of texture frames

    int numobjects;
    char objname[OLDMAXOBJECTS][RESNAMELEN];
    WORD objmaterial[OLDMAXOBJECTS];
    WORD objvertpos[OLDMAXOBJECTS];     // Position of object verts in vert array
    WORD objvertnum[OLDMAXOBJECTS];     // Num object verts in vert array
    WORD objfacepos[OLDMAXOBJECTS][OLDMAXTEXTURES];// Offset to array of face pos WORDs for each texture (0=none, 1=tex 0, 2 = tex 1, etc.)
    WORD objfacenum[OLDMAXOBJECTS][OLDMAXTEXTURES];// Offset to array of face num WORDs for each texture (0=none, 1=tex 0, 2 = tex 1, etc.)
//  SLinearBounds objbounds[OLDMAXOBJECTS];

    OFFSET motion[OLDMAXOBJSTATES];     // Contains motion info for state
    OFFSET anikeys[OLDMAXOBJSTATES];        // Contains all animation keys for state
    OFFSET imagery[OLDMAXOBJSTATES];        // Contains icon/iconani and other 2d imagery for state
    OFFSET objparent[OLDMAXOBJECTS];        // Contains object hierarchy for state where
                                        // Hierarchy is array of signed byte indexes for each
                                        // obj indicating its parent, -1 means root
};

// **********************************************************

// NEW Version of S3DImageryBody, no limits on arrays!
_STRUCTDEF(S3DImageryBody)
struct S3DImageryBody : public SImageryBody
{
    DWORD flags;                        // If true, verts array points to multple vert images
    DWORD version;                      // Version of imagery

    OS3DImageryState statedata;         // Array of S3DImageryState structures

    int numverts;                       // Verts stored in state,frame order...
    OOOD3DVERTEX verts;                 // Accessed via verts[state][frame][vert]
                                        // Where [state][frame] are always [0][0] if
                                        // I3D_ISMORPH is NOT set (not a morph animation)

    int numfaces;                       // Number of faces
    OS3DFace faces;                     // Faces stored by texture, then object order

    int nummaterials;                    
    OD3DMATERIAL materials;             // Offset to D3DMATERIAL array

    int numtextures;
    OS3DImageryTexture textures;        // Offset to S3DImageryTexture array

    int numobjects;
    OS3DImageryObject objects;          // Offset to S3DImageryObject array

    int numplaysounds;
    OS3DImageryPlaySound playsounds;    // Offset to array of sounds to play
};

#endif
