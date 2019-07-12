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
#define MAXTAGS             128

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

// Animation keys...
// All keys are 32 bit, and can contain either 10 bit x,y,z information, or 24 bit
// fixed point individual values.  Ordinarily, the first frame will have the 24 bit
// values, and the remaining frames will have the 10 bit values.  Next frame is 
// implied when a key repeats, or a SKIP is encountered.  Keys are garanteed to 
// repeat for next frames (i.e no POS frame1, then SCL frame 2, then ROT frame 3...
// will have (POS frame 1, POS frame2 SCL frame2, SCL frame3 ROT frame 3)..

#define SMALLKEYBITS        10                      // Number of bits for small key value
#define SMALLKEYMAX         ((1 << (SMALLKEYBITS - 1)) - 1)
#define SMALLKEYMIN         (-(1 << (SMALLKEYBITS - 1)))

#define ANIKEY32_POSSCALE   (4.0)                   // Object pos's must be in range 511 to -512
#define MAXPOSKEY32         ((float)SMALLKEYMAX / (float)ANIKEY32_POSSCALE)
#define MINPOSKEY32         ((float)SMALLKEYMIN / (float)ANIKEY32_POSSCALE)

#define ANIKEY32_ROTSCALE   ((float)(1 << (SMALLKEYBITS - 1)) / 3.1415965359f) // Angles must be in range -PI to PI (511 to -512)
#define MAXROTKEY32         ((float)SMALLKEYMAX / (float)ANIKEY32_ROTSCALE)
#define MINROTKEY32         ((float)SMALLKEYMIN / (float)ANIKEY32_ROTSCALE)

#define ANIKEY32_SCLSCALE   (64.0)                  // Scales are in range of 7.0 to -8.0
#define MAXSCLKEY32         ((float)SMALLKEYMAX / (float)ANIKEY32_SCLSCALE)
#define MINSCLKEY32         ((float)SMALLKEYMIN / (float)ANIKEY32_SCLSCALE)

#define ANIKEY32_CODESCALE  (256.0)                 // Codes in range 65535 to -65536 with 8 bit fixed

// Compressed format is very small..
#define ANIFLAG32_CODE      0       // Key uses code format
#define ANIFLAG32_POS       1       // Key is 10 bit x,y,z pos
#define ANIFLAG32_ROT       2       // Key is 10 bit x,y,z rot
#define ANIFLAG32_SCL       3       // Key is 10 bit x,y,z scl

// Code format trades space for precision... First frame ususally is code.
#define ANICODE32_NEXTFRAME 0
#define ANICODE32_SKIP      1
#define ANICODE32_POSX      2
#define ANICODE32_POSY      3
#define ANICODE32_POSZ      4
#define ANICODE32_ROTX      5
#define ANICODE32_ROTY      6
#define ANICODE32_ROTZ      7
#define ANICODE32_SCLX      8
#define ANICODE32_SCLY      9
#define ANICODE32_SCLZ      10

_STRUCTDEF(SAniKey32)
struct SAniKey32
{
    union
    {
      struct {
        unsigned int flags : 2; // Indicates what kind of structure this is
        int x : 10;
        int y : 10;
        int z : 10;
      };
      struct {
        unsigned int dummy : 2; // Flags dummy
        unsigned int code : 6;
        int value : 24;
      };
    };
};

// Old animation key stuff

#define ANIKEY_POSSCALE  (4.0)                      // Object pos's must be in range 0-512
#define ANIKEY_ANGSCALE  (256.0 / 3.14159265359)    // Angles must be in range -PI to PI

_STRUCTDEF(SAniKey)
struct SAniKey
{
    union
    {
        int data[2];
        struct
        {
            unsigned __int64 x : 12;    // Use macros below!!
            unsigned __int64 y : 12;
            unsigned __int64 z : 12;
            unsigned __int64 rx : 9;
            unsigned __int64 ry : 9;
            unsigned __int64 rz : 9;
            unsigned __int64 dummy : 1; // So equals 64 bits
        };
    };
};

#define ak_x(ak)  ((int)((ak).x) - 0x800)
#define ak_y(ak)  ((int)((ak).y) - 0x800)
#define ak_z(ak)  ((int)((ak).z) - 0x800)
#define ak_rx(ak) ((int)((ak).rx) - 0x100)
#define ak_ry(ak) ((int)((ak).ry) - 0x100)
#define ak_rz(ak) ((int)((ak).rz) - 0x100)

#define ak_setx(ak, d)  (ak).data[0] = (((ak).data[0] & 0xFFFFF000) | ((((int)(d) + 0x800) & 0xFFF) << 0))
#define ak_sety(ak, d)  (ak).data[0] = (((ak).data[0] & 0xFF000FFF) | ((((int)(d) + 0x800) & 0xFFF) << 12))
#define ak_setz(ak, d)  (ak).data[0] = (((ak).data[0] & 0x00FFFFFF) | ((((int)(d) + 0x800) & 0x0FF) << 24));\
                        (ak).data[1] = (((ak).data[1] & 0xFFFFFFF0) | ((((int)(d) + 0x800) & 0xF00) >> 8))
#define ak_setrx(ak, d) (ak).data[1] = (((ak).data[1] & 0xFFFFE00F) | ((((int)(d) + 0x100) & 0x1FF) << 4))
#define ak_setry(ak, d) (ak).data[1] = (((ak).data[1] & 0xFFC01FFF) | ((((int)(d) + 0x100) & 0x1FF) << 13))
#define ak_setrz(ak, d) (ak).data[1] = (((ak).data[1] & 0x003FFFFF) | ((((int)(d) + 0x100) & 0x1FF) << 22))

// Very old motion data structure without facing angle
_STRUCTDEF(SOldOldMotionData)
struct SOldOldMotionData
{
    unsigned int dist: 24;
    unsigned int ang : 8;
};

// Old motion data structure
_STRUCTDEF(SOldMotionData)
struct SOldMotionData
{
    unsigned int dist : 16;
    unsigned int face : 8;
    unsigned int ang : 8;
};

// New 64 bit full motion data structure (has x,y rotation and vertical motion)
_STRUCTDEF(SMotionData)
struct SMotionData
{
    unsigned int dist : 16;     // Distance forward on x,y plane (regular movement)
    signed int vert : 16;       // Vertical movement on z axis (flying/jumping)
    unsigned int ang : 8;       // Angle forward on x,y plane (moveangle)
    unsigned int rotx : 8;      // Rotation x for object (rarely used)
    unsigned int roty : 8;      // Rotation y for object (rarely used)
    unsigned int rotz : 8;      // Rotation z for object (face)
};

#define md_dist(md)         ((md).dist << 8)
#define md_vert(md)         ((md).vert << 8)

#define md_setdist(md, d)   ((md).dist = (d) >> 8)
#define md_setvert(md, v)   ((md).data = (v) >> 8)

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

// Previous state with old animation keys
_STRUCTDEF(S3DOldImageryObjectState)
struct S3DOldImageryObjectState
{
    int parent;                     // Contains object hierarchy for state where
                                    // Hierarchy is array of signed byte indexes for each
                                    // obj indicating its parent, -1 means root};
    OSAniKey anikeys;               // Offset to array of SAniKeys for this object's state
                                    // If this is NULL, object is HIDDEN!
};

_STRUCTDEF(S3DImageryObjectState)
struct S3DImageryObjectState
{
    int parent;                     // Contains object hierarchy for state where
                                    // Hierarchy is array of signed byte indexes for each
                                    // obj indicating its parent, -1 means root};
    int numanikeys;                 // Number of keys for this state
    OSAniKey32 anikeys;             // Offset to array of SAniKeys for this object's state
                                    // If this is NULL, object is HIDDEN!
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

_STRUCTDEF(S3DOldImageryState)
struct S3DOldImageryState
{
    int         invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSMotionData motion;                // Offset to motion data for this state
};

_STRUCTDEF(S3DOldImageryState2)
struct S3DOldImageryState2
{
    int         invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSAniKey32   motion;                // Offset to motion data for this state
};

_STRUCTDEF(S3DOldImageryState3)
struct S3DOldImageryState3
{
    int         invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSAniKey32   motion;                // Offset to motion data for this state
    char        begstate[RESNAMELEN];   // Object starts with animation matching end of this state
    char        endstate[RESNAMELEN];   // Object ends with animation matchin start of this state
};

_STRUCTDEF(S3DImageryState)
struct S3DImageryState
{
    int         invsize;                // Size of inventory icon data (image or anim)
    OTBitmap    invitem;                // Inventory image data (image)
    OTAnimation invanim;                // Inventory image animation data (anim)
    OSAniKey32  motion;                 // Offset to motion data for this state
};

_STRUCTDEF(S3DImageryPlaySound)
struct S3DImageryPlaySound
{
    int state;                          // State to play sound at
    int frame;                          // Frame number to play sounds at
    BOOL mount;                         // Should we mount this sound when imagery is loaded
    char sounds[SOUNDLISTLEN];          // List of comma delimited sounds to randomly play
};

_STRUCTDEF(S3DImageryTag)
struct S3DImageryTag
{
    int state;                          // State to play sound at
    int frame;                          // Frame number to play sounds at
    OFFSET name;                        // Name of tag
    OFFSET str;                         // String for tag
};

// Version 2 - Added begstate,endstate to state structure
// Version 3 - Removed begstate,endstate and changed playsounds to animation tags
#define VERSION3DIMAGEBODY 3

#define I3D_ISMORPH      1  // If this flag is set, the vertex array will have multiple images
                            // stored sequentially for each object state, otherwise there will
                            // be only 1 vertex image buffer for object state [0] which will be
                            // used for all animations

// The following flags indicate the presence of additional imagery data
#define I3D_HASICONS     2  // True if imagery has icons
#define I3D_HASHIERARCHY 4  // True if imagery has a hierarchy
#define I3D_FACINGMOTION 8  // Has facing motion data (stores hip facing deltas)
#define I3D_3DIMAGEBODY2 16 // Uses 3D image body 2 header format
#define I3D_HASPLAYSOUND 32 // Has playsound structure
#define I3D_ROOTMOTION   64 // Uses root motion and SAniKey motion data
#define I3D_ANIKEY32     128// New compressed animation key format

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

    int numtags;
    OS3DImageryTag tags;                // Offset to array of animation tags (sorted by state/frame)
};

#endif
