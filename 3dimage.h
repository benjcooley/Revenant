// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  3dobject - 3D Object Include File                    *
// *************************************************************************

#ifndef _3DIMAGE_H
#define _3DIMAGE_H

#ifndef _WINDOWS_
#error WINDOWS.H Must be included at the top of your .CPP file
#endif

#include <ddraw.h>
#include <d3d.h>
#include <d3drmwin.h>

#include "3dimagebody.h"
#include "3dscene.h"
#include "directdraw.h"
#include "revenant.h"
#include "object.h"
#include "resource.h"
#include "display.h"
#include "parse.h"

// **************************************************************
// * T3DImagery - Shared imagery resource for a given 3D object *
// **************************************************************

_CLASSDEF(T3DImagery)
_CLASSDEF(T3DAnimator)
_CLASSDEF(T3DImageryRes)

_STRUCTDEF(S3DMat)
struct S3DMat
{
    D3DMATERIAL matdesc;
    LPDIRECT3DMATERIAL material;
    D3DMATERIALHANDLE hmaterial;
    int texture;
};
typedef TVirtualArray<S3DMat, 4, 4> T3DMatArray;

_STRUCTDEF(S3DTex)
struct S3DTex
{
    DDSURFACEDESC ddsd;                 // Surface description
    int numframes;                      // Number of frames
    int framenum;                       // Current frame this texture is showing

    LPDIRECTDRAWSURFACE *framesurfs;    // Points to array of frame surfaces if numframes > 1
    LPDIRECT3DTEXTURE *frametexs;       // Points to array of frame textures if copyframe is FALSE and numframes > 1
    D3DTEXTUREHANDLE *framehtexs;       // Points to array of frame texture handles if copyframe is FALSE and numframes > 1

    BOOL copyframes;                    // If TRUE surface and texture below are allocated,
                                        // otherwise they are pointers to frame array
                                        // textures above

    LPDIRECTDRAWSURFACE surface;        // Main video buffer frame (NULL if Current frame video surface
    LPDIRECT3DTEXTURE texture;          // Current frame texture
    D3DTEXTUREHANDLE htexture;          // Curent frame htexture
};
typedef TVirtualArray<S3DTex, 4, 4> T3DTexArray;

_STRUCTDEF(S3DObj)
struct S3DObj
{
    char name[RESNAMELEN];
    int *parent;                        // Parent object for each state
    int numverts;
    int startvert;
    int numfaces;
    int startface;
    int *texfaces;                      // 0 is no texture, 1 is texture 0, etc.
    int *numtexfaces;                   // 0 is no texture, 1 is texture 0, etc.
    int material;
    int *numanikeys;    
    void **anikeys;                     
};
typedef TVirtualArray<S3DObj, 16, 16> T3DObjArray;

_STRUCTDEF(S3DTag)
struct S3DTag
{
    int state;          // Tag state num
    int frame;          // Frame
    char *name;         // Identifier of tag
    char *str;          // Text for tag
    S3DTag() { state = frame = 0; name = str = NULL; }
    S3DTag(int pstate, int pframe, char *pname, char *pstr)
      { state = pstate; frame = pframe; name = strdup(pname); str = strdup(pstr); }
    ~S3DTag() { if (name) free(name); if (str) free(str); }
};
typedef TVirtualArray<S3DTag, 16, 16> T3DTagArray;

_STRUCTDEF(S3DImageryIcons)
struct S3DImageryIcons
{
    PTBitmap invitem;
    PTAnimation invanim;
};

#define MAX3DANIMOBJECTS 256

#define OBJ3D_HIDE      0x000001    // Hide this object
#define OBJ3D_ANIMTRACK 0x000002    // Override objects default anim track with given track
#define OBJ3D_POS1      0x000004    // Override object animation - Sets object position first
#define OBJ3D_POS2      0x000008    // Sets object position second (after either rot or scl)
#define OBJ3D_POS3      0x00000C    // Sets object position third (after both rot and scl)
#define OBJ3D_POSMASK   0x00000C    // Pos override mask (2 bits)
#define OBJ3D_ROT1      0x000010    // Override object animation - Sets object rotation first
#define OBJ3D_ROT2      0x000020    // Sets object rotation second (after either pos or scl)
#define OBJ3D_ROT3      0x000030    // Sets object rotation third (after both pos and scl)
#define OBJ3D_ROTMASK   0x000030    // Rot override mask (2 bits)
#define OBJ3D_SCL1      0x000040    // Override objects animation - Sets object scale first
#define OBJ3D_SCL2      0x000080    // Sets object scale second (after either rot or pos)
#define OBJ3D_SCL3      0x0000C0    // Sets object scale third (after both rot and pos)
#define OBJ3D_SCLMASK   0x0000C0    // Scale override mask (2 bits)
#define OBJ3D_MATRIX    0x000100    // Override object matrix with given matrix (ignores anikey, pos, rot, and scl)
#define OBJ3D_ADDTOANI  0x000200    // Causes the above transforms to be added to the default obj anim, otherwise they override it
#define OBJ3D_PRIMTYPE  0x000400    // Override objects default primitive type with given type
#define OBJ3D_VERTTYPE  0x000800    // Override objects default primitive type with given type
#define OBJ3D_VERTONLY  0x001000    // Ignores face list (uses DrawPrim insted of DrawIndexPrim)
#define OBJ3D_VERTS     0x002000    // Overrides objects default vertex list stored in T3DImagery
#define OBJ3D_COPYVERTS 0x004000    // Causes NewObject() to allocate local copy of vertices for this obj
#define OBJ3D_OWNSVERTS 0x008000    // Set if object should delete its own vertex buffer when deleted
#define OBJ3D_FACES     0x010000    // Overrides objects default face list stored in T3DImagery
#define OBJ3D_COPYFACES 0x020000    // Causes NewObject() to allocate local copy of faces for this obj
#define OBJ3D_OWNSFACES 0x040000    // Set if object should delete its own face buffer when deleted
#define OBJ3D_MAT       0x080000    // Overrides objects default material handle
#define OBJ3D_TEXFRAME  0x100000    // Overrides objects default texture frame number
#define OBJ3D_TEX       0x200000    // Overrides objects default texture handles
#define OBJ3D_ABSPOS    0x400000    // Treats the position as absolute (already converted to world space)
#define OBJ3D_PARENT    0x800000    // Override parent pointer

#define OBJ3D_RENDERED  0x80000000  // Used internally to indicate if obect was rendered

_STRUCTDEF(S3DAnimObj) // Approx 200 bytes per obj
struct S3DAnimObj
{
    int flags;
    int objnum;                         // This is not an override.. must always be set
    PS3DAnimObj parent;                 // Pointer to a parent object (will concatenate matrices)
    int animtrack;                      // Set this object to use animation track for another object
    D3DVECTOR pos;                      // Use with OBJ3D_POS, OBJ3D_ROT, and OBJ3D_SCL
    D3DVECTOR rot;                      // These can not be used together with OBJ3D_MATRIX!
    D3DVECTOR scl;                      
    D3DMATRIX matrix;                   // Matrix (can be set directly using OBJ3D_MATRIX, or calculated by pos/rot/scl)
    D3DPRIMITIVETYPE primtype;          // Type of primitive (face, line, etc.)
    D3DVERTEXTYPE verttype;             // Type of verts (VERTEX, LVERTEX, TLVERTEX)
    int numverts;                       // Number of verts for object
    union {
        LPD3DVERTEX verts;              // Vert array
        LPD3DLVERTEX lverts;
        LPD3DTLVERTEX tlverts;
    };
    int numfaces;                       // Total faces for object (must set texfaces/numtexfaces too)
    S3DFace *faces;                     // Face array
    int texfaces[MAXTEXTURES + 1];      // Start face for each texture (index 0 is no texture 1 is first)
    int numtexfaces[MAXTEXTURES + 1];   // Num faces to render for each texture (index 0 is no texture 1 is first)
    int textureframe[MAXTEXTURES + 1];  // Frame number for animating textures for each texture
    D3DTEXTUREHANDLE htextures[MAXTEXTURES]; // Texture handles for each texture (0 is first texture)
    LPDIRECTDRAWSURFACE surfaces[MAXTEXTURES]; // Surface pointers for each texture (0 is first texture)
    D3DMATERIALHANDLE hmaterial;        // Material for object (only 1 per obj)
};
typedef TPointerArray<S3DAnimObj, 16, 16> T3DAnimObjArray;

class T3DImagery : public TObjectImagery
{
  protected:
    BOOL meshinitialized;
    BOOL hastextures;

    int baseobj;

    DWORD flags;    // Flags for this 3D object
    DWORD version;  // version of this 3D object

    int numverts;                       // Global vert list
    D3DVERTEX ***verts;

    int numfaces;                       // Global face list
    S3DFace *faces;

    T3DMatArray materials;              // Object resource arrays
    T3DTexArray textures;
    T3DObjArray objects;
    T3DTagArray tags;

    int numframes;                      // Mesh/Ani/Icon/etc. arrays
    SMotionData **motion;               // Points to motion data
    S3DImageryIcons *icons;
    unsigned char *hierarchy;

    PTExecuteBuf executebuf;            // Current execute buffer cache
    BOOL cacheexbuf;                    // TRUE if we want to cache the execute buffer

  // State data for rendering   
    int prevstate;                      // Used to control interpolation between states
    int prevframe;                      // Last frame drawn in previous state

  public:
    T3DImagery(int imageid);
      // Constructor. Initializes states to zero, and clears all flags and
      // status codes
    virtual ~T3DImagery();
      // Destructor.

    BOOL OldInitializeMesh(PSOld3DImageryBody mesh);
      // Initializes old style 3DImageBody mesh
    BOOL InitializeMesh(PS3DImageryBody mesh);
      // Creates D3D objects, from the given S3DMesh pointer
    void ClearMesh();
      // Deletes 3D objects from the given mesh pointer
    BOOL MeshInitialized() { return meshinitialized; }
      // Returns status of mesh initialization
    virtual BOOL Restore();
      // Restors lost surfaces, textures, etc.

  // GENERAL NOTE: The T3DImagery object is a global object which is instantiated once
  // for all T3DAnimator objects based on a given imagery file.  This means that the
  // vertices, faces, object list, material list, and texture list for the imagery
  // can not be changed or all animators that are drawing that object will be screwed
  // up.  GOT IT!  Hey.. I'm talking to you.. are you listening?  I'm not just a saying
  // this for my health you know.  Ahhhh... forget it.

  // Vertex/Face Functions (Note: vertices can't be added or deleted by outside program)
    int NumVerts();
      // Returns number of vertex in the vertex array for a single frame
    void GetVerts(LPVOID vertbuf, int state = 0, int frame = 0,
        D3DVERTEXTYPE verttype = D3DVT_VERTEX, int beg = 0, int len = -1);
      // Copies single frame of vertices (default is state 0, frame 0, beg = 0, len = numverts)
      // Note: GetVerts will translate standard obj verts to LVERTEX and TLVERTEX arrays
    int NumObjVerts(int objnum);
      // Returns number of object vertices
    void GetObjVerts(int objnum, LPVOID vertbuf, int state = 0, int frame = 0,
      D3DVERTEXTYPE verttype = D3DVT_VERTEX);
      // Copies single frame of vertices (default is state 0, frame 0) for an object.
      // Note: GetObjVerts will translate standard obj verts to LVERTEX and TLVERTEX arrays
    int NumFaces();
      // Returns the number of faces for this imagery object
    void GetFaces(PS3DFace facesbuf);
      // Copies all faces for this imagery object
    int NumObjFaces(int objnum);
      // Returns number of object faces
    void GetObjFaces(int objnum, PS3DFace facesbuf, int *texfaces = NULL, int *numtexfaces = NULL);
      // Copies all faces for this imagery object, also will fill out the texfaces and numtexfaces
      // array if these params aren't NULL.

  // Object functions (Note: objects can't be added or deleted by outside program)
  private:
    int AddObject(PS3DObj obj);
      // Adds a new object to object list (internal function)
    void RemoveObject(int objnum);
      // Removes an object from the object list  (internal function)
    void ClearObjects();
      // Clears all objects in list (internal function)
  public:
    int NumObjects();
      // Returns number of objects in object list
    char *GetObjectName(int objnum);
      // Gets the name of a given object
    int GetObjectNum(char *objname);
      // Gets the object number given the object name
    void GetObject(int objnum, PS3DObj obj);
      // Copies object info for the given object
    BOOL IsHidden(int objnum, int state);
      // Returns TRUE if object is hidden for this state (i.e. no animation keys)
    BOOL GetUninterpolatedAniKey(int objnum, int state, int frame, 
        D3DVECTOR &pos, D3DVECTOR &rot, D3DVECTOR &scl);
      // Gets the position, rotation, and scale for the given state and frame, or returns FALSE
      // if there is no animation for that frame. This version retrieves the animation only,
      // it does not interpolate.
    BOOL GetAniKey(int objnum, int state, int frame, 
        D3DVECTOR &pos, D3DVECTOR &rot, D3DVECTOR &scl);
      // Gets the position, rotation, and scale for the given state and frame, or returns FALSE
      // if there is no animation for that frame.  This version also will interpolate the 
      // beginning and ending of an animation based on the next and previous states to make sure
      // all animation is continuous.
    void SetPrevState(int pstate, int pframe)
      { prevstate = pstate; prevframe = pframe; }
      // Tells animator of previous state and last frame for previous state so
      // it can correctly interpolate between states.
    int GetObjectParent(int objnum, int state) { return objects[objnum].parent[state]; }
      // Returns the object parent for the given state
    void BeginRender(BOOL cachebuf, BOOL clearbuf);
      // This function must be called before any calls to RenderObject().  This function
      // initializes render variables, and will cause the execute buffer to be cached if
      // 'cacheexbuf' is TRUE, and the global UseDrawPrimitive is false.  If 'clearexbuf' is
      // TRUE, the current execute buffer will be cleared.
    BOOL CalcObjectMatrix(PS3DAnimObj animobj, int state, int frame, LPD3DMATRIX pos,
        BOOL calcparents = TRUE);
      // Calculates the objects matrix for the given state, frame, and main object position.  If
      // calcparents is TRUE, it recursively calculates the objects parent matrices as well, otherwise
      // it simply concatenates the objects matrix with the current contents of its parent's
      // matrices without recalculating them.
    BOOL CalcObjectMatrixCopy(PS3DAnimObj animobj, int state, int frame, LPD3DMATRIX pos,
        BOOL calcparents = TRUE);
      // virtually the same as the above, but this one copies the result into pos...
      // also, it only deals with the matrices.
    BOOL RenderObject(PS3DAnimObj animobj, int state, int frame, LPD3DMATRIX pos,
        int tex = -1, BOOL uselastmatrix = FALSE);
      // Renders an object individually.  Specify tex to render only the faces in the object
      // for a given texture number where 0 is no texture, 1 - texture 0, 2 is tex 1, etc.
      // If rendering groups of objects by texture, specify TRUE for uselastmatrix for each
      // texture AFTER the first to prevent the object matrix from being recalculated.
      // 'exbufcached' is true if the object's execute buffer was cached, and we only want to
      // set the object's position matrices, instead of calling the DrawPrimitive functions.
    void EndRender();
      // This function is called after all calls to RenderObject are complete.  This function
      // will grab the execute buffer created by the calls to RenderObject and store it if
      // it doesn't yet exist.
    void PlaySound(PTObjectInstance inst, int state, int frame);
      // Called by animator to play a sound (if there is one) for the current frame and state
    PTExecuteBuf GetExecuteBuf() { return executebuf; }
      // If the imagery object currently has an execute buffer cache, this function will 
      // return it.

  // Material functions (Note: materials can't be added or deleted by outside program)
  private:
    int AddMaterial(LPD3DMATERIAL newmat, int tex = -1);
      // Called to add a material (internal function)
    void RemoveMaterial(int matnum);
      // Called to remove a particualr material (internal function)
    void ClearMaterials();
      // Called to release all materials when imagery is destroyed (internal function)
  public:
    int NumMaterials() { return materials.NumItems(); }
      // Returns the number of materials for the imagery
    void GetMaterial(int matnum, PS3DMat mat);
      // Gets the current color, texture, etc. for the given material
    D3DMATERIALHANDLE GetMaterialHandle(int matnum);
      // Quickie get handle function
    void SetMaterial(int matnum, PS3DMat mat);
      // Sets new color, texture, etc. for the given material (NOTE: setting the properties
      // for the material will cause all objects animated with this imagery resource to change)

  // Texture functions (Note: textures can't be added or deleted by outside program)
  private:
    int AddTexture(LPDDSURFACEDESC srcsd,
        OFFSET *pixels, int frames, LPPALETTEENTRY palette);
      // Creates a new texture (internal function)
    BOOL LoadTexture(PS3DTex tex, LPDDSURFACEDESC srcsd,
        OFFSET *pixels, int frames, LPPALETTEENTRY palette, BOOL copyframes);
      // Loads (or reloads) a texture given a texture structure (internal function)
      // If copyframes is TRUE, texture animation frames are allocated in system memory and
      // copied to a single texture buffer in texture memory when playing.  Also, if there is not
      // enough texture memory to store all frames in vid mem, copyframes will be forced
      // to TRUE regardless.
    void RemoveTexture(int texnum);
      // Releases texture and removes texture from texture list
    void ClearTextures();
      // Kills all textures
  public:
    int NumTextures();
      // Returns number of textures
    void GetTexture(int texnum, PS3DTex tex);
      // Gets the current texture info
    D3DTEXTUREHANDLE GetTextureHandle(int texnum);
      // Quickie get handle function
    int GetTextureFrame(int texnum);
      // Returns the current texture frame for the texture
    BOOL SetTextureFrame(int texnum, int framenum);
      // Sets texture frame.  If copyframes is set, this function copies the texture frame
      // from system memory to video memory.  If not, it simply switches the texture 
      // handles.
    BOOL SurfacesLost();
      // Returns TRUE if any texture surfaces have been lost
    void RestoreSurfaces();
      // Causes texture surfaces to be reloaded from imagery file

  // Miscellaneous support functions
    void RefreshZBuffer(PTObjectInstance oi);
      // Called for all imagery before rendering to refresh the z buffer behind the object
    void ResetExtents();
      // Resets the rendered extents to default values
    void GetExtents(LPD3DRECT extents);
      // Retrieves the extents of any imagery drawn since the ResetExtents() function was called
    void AddUpdateRect(LPD3DRECT extents, int uflags = UPDATE_RESTORE);
      // Adds an update rectangle to the screen for the current rendered extents
    void UpdateBoundingRect(PTObjectInstance oi, int state, LPD3DRECT extents);
      // This function sets the maximum bounding box for a state given the supplied extents rect
    int NumTags() { return tags.NumItems(); }
      // Returns number of tags
    S3DTag *GetTag(int tagnum) { return &(tags[tagnum]); }
      // Returns number of tags
    char *FindTag(char *name, int state, int frame = -1, int *foundstate = NULL, int *foundframe = NULL);
      // Finds a tag in the tag list given the tag name, state, and optional frame.  If
      // state or frame are less < 0, will find any state, or any frame.  Optional
      // foundstate and foundframe will return the state and frame actually found.

    virtual BOOL GetZ(PTObjectInstance oi, PTSurface surface) { return TRUE; }
        // At some point a pixel-perfect routine might be nice
    virtual BOOL AlwaysOnTop(PTObjectInstance oi) { return TRUE; }
        // This wouldn't be necessary if the pixel routine worked
    virtual PTBitmap GetInvImage(int state, int num = 0);
      // Returns the inventory image for the item
    virtual PTAnimation GetInvAnimation(int state);
        // Get inventory animation for state

    virtual PTObjectAnimator NewObjectAnimator(PTObjectInstance oi);
        // Creates an animator for the given object
    virtual BOOL NeedsAnimator(PTObjectInstance oi);
      // Returns whether or not an animator is necessary

  // Motion functions
    BOOL GetMotion(int state, int frame, 
        int &dist, int &vert, int &ang, int &rotx, int &roty, int &rotz);
      // Gets motion deltas for the current frame (returns FALSE if no motion or invalid state/frame)
    virtual void SetObjectMotion(PTObjectInstance inst);
      // Sets the motion for an object based on its state, frame, and 3D motion data
};

DEFINE_IMAGERYBUILDER(OBJIMAGE_MESH3D, T3DImagery);

// *******************************************************************************
// * T3DControllerBuilder - Used to register and automatically build controllers *
// *******************************************************************************

#define MAX3DCONTROLLERTYPES 128

_CLASSDEF(T3DControllerBuilder)
class T3DControllerBuilder
{
  public:
    T3DControllerBuilder();
      // Used for default builder
    T3DControllerBuilder(char *name);
      // Sets name and adds builder to builder array
    virtual PT3DController Build(int ptagstate, int ptagframe, 
        PT3DAnimator panimator, PT3DImagery pimagery, PTObjectInstance pinst) { return NULL; }
      // Creates a new object from the given 'def' structure
    static PT3DControllerBuilder GetBuilder(char *name);
      // Gets a pointer to a builder in the builder array

  private:
    static int numconttypes;                                     // Number of animator types
    static PT3DControllerBuilder builders[MAX3DCONTROLLERTYPES]; // Object builder for each type

    char *controllername;
};

#define REGISTER_3DCONTROLLER(name, obj)                                        \
class obj##Builder : public T3DControllerBuilder                                \
{                                                                               \
  public:                                                                       \
    obj##Builder() : T3DControllerBuilder(name) {}                              \
    virtual PT3DController Build(int ptagstate, int ptagframe,                  \
      PT3DAnimator panimator, PT3DImagery pimagery, PTObjectInstance pinst)     \
        { return new obj(ptagstate, ptagframe, panimator, pimagery, pinst); }   \
};                                                                              \
obj##Builder obj##BuilderInstance;

// *********************************************************************
// * T3DController - Controls Animated Effects Based on Animation Tags *
// *********************************************************************

// 3D Controllers allow animators to add special effects to their animations
// by adding 'controller' tags into the animation note track.  When the 
// animation is loaded, a controller is instantiated for every recognized
// controller tag in the animation note track, and each controller parses
// its own parameters.  Controllers are useful for adding 'procedural'
// animation such as scrolling textures, morphing points, blood, etc.

_CLASSDEF(T3DController)
class T3DController
{
  protected:  // This stuff is none of your business        
    int tagstate, tagframe;                   // State and frame this controller was tagged
    PT3DAnimator animator;                    // Pointer to animator
    PT3DImagery imagery;                      // Pointer to imagery
    PTObjectInstance inst;                    // Pointer to object instance
    T3DAnimObjArray animobjs;                 // Object list for this controller

    virtual BOOL ParseParams(TToken &t);
      // Parses the parameters for a controller.  Parameters are in the format
      // "param=val,param=val,..." where "param" is a minimal 2-5 character id, and
      // val is either a literal value (number, name, or text), or a parentheses
      // delimited list as in "(val,val,val)".
      // EXAMPLE: "scrolltex:obj=(lfoot,rfoot,head),dx=20,dy=-5"
    virtual BOOL ParseItem(char *param, TToken &t);
      // Parses a single parameter item in the parameter list (given the format
      // "param=val".  The token will be set to the first token of 'val'.  Virtual
      // functions should call the base class ParseItem() if it doesn't handle
      // a particular 'param' tag.

  public:
    T3DController(int ptagstate, int ptagframe, PT3DAnimator panimator, PT3DImagery pimagery, PTObjectInstance pinst)
      { tagstate = ptagstate; tagframe = ptagframe; 
        animator = panimator; imagery = pimagery; inst = pinst; }
      // Constructor
    virtual ~T3DController() { Close();}
      // Closes the object

    virtual BOOL Initialize(char *params);
      // Called to initialize the controller and parse controller parameters.
    virtual void Close();
      // Kills the controller

    int TagState() { return tagstate; }
      // Returns state tag is in
    int TagFrame() { return tagframe; }
      // Returns frame tag is in
    PT3DAnimator GetAnimator() { return animator; }
      // Gets the animator for this controller
    PT3DImagery GetImagery() { return imagery; }
      // Gets the imagery for this controller
    PTObjectInstance GetInstance() { return inst; }
      // Returns the object instance for this controller
    int NumObjects() { return animobjs.NumItems(); }
      // Returns number of S3DAnimObj strucutres controlled by controller
    PS3DAnimObj GetObject(int objnum) { return animobjs[objnum]; }
      // Returns one of the S3DAnimObj structures controlled by this controller

    virtual void Pulse() {}
      // Called during each timer Pulse() to update animaton.
    virtual void Render() {}
      // Called during render phase to render controller objects
    virtual BOOL KillMe() { return FALSE; }
      // This controller is done doing whatever it's going to do
};
typedef TPointerArray<T3DController, 0, 16> T3DControllerArray;

// ***************************************************************************
// * T3DAnimatorBuilder - Used to register and automatically build animators *
// ***************************************************************************

#define MAX3DANIMATORTYPES 128

_CLASSDEF(T3DAnimatorBuilder)
class T3DAnimatorBuilder
{
  public:
    T3DAnimatorBuilder();
      // Used for default builder
    T3DAnimatorBuilder(char *name);
      // Sets name and adds builder to builder array
    virtual PT3DAnimator Build(PTObjectInstance oi);
      // Creates a new object from the given 'def' structure
    static PT3DAnimatorBuilder GetBuilder(char *name);
      // Gets a pointer to a builder in the builder array

  private:
    static int numanimtypes;                                 // Number of animator types
    static PT3DAnimatorBuilder builders[MAX3DANIMATORTYPES]; // Object builder for each type

    char *animatorname;
};

#define REGISTER_3DANIMATOR(name, obj)                                          \
class obj##Builder : public T3DAnimatorBuilder                                  \
{                                                                               \
  public:                                                                       \
    obj##Builder() : T3DAnimatorBuilder(name) {}                                \
    virtual PT3DAnimator Build(PTObjectInstance oi)                             \
        { return new obj(oi); }                                                 \
};                                                                              \
obj##Builder obj##BuilderInstance;

#define REGISTER_MULTI_3DANIMATOR_(name, obj, anim)                             \
class obj##Builder : public T3DAnimatorBuilder                                  \
{                                                                               \
  public:                                                                       \
    obj##Builder() : T3DAnimatorBuilder(name) {}                                \
    virtual PT3DAnimator Build(PTObjectInstance oi)                             \
        { return new anim(oi); }                                                \
};                                                                              \
obj##Builder obj##BuilderInstance;

// ********************************************
// * T3DAnimator - Default 3D Animator Object *
// ********************************************

#define ANI3D_ADDUPDATERECT         1   // (default TRUE) adds update rect for extents of all objects
#define ANI3D_UPDATEBOUNDRECT       2   // (default FALSE) uses extents to reset obj state bounding rects
#define ANI3D_UPDATEALLSTATES       4   // (default FALSE) resets extents for all states
#define ANI3D_UPDATEFRONTONLY       8   // (default FALSE) resets extents for front rotation only
#define ANI3D_CACHEEXECUTEBUFFER    16  // (default TRUE) caches obj exbuf if not using DrawPrimitive

// Notes on ANI3D_CACHEEXECUTEBUFFER: 
//
// Setting ANI3D_CACHEEXECUTEBUFFER when CacheExBufs global state is true should improve 
// rendering performance substantially.  However, make sure you use the flag only when it
// will work correctly.  Below are the rules
//
// Objects which are prime canidates for execute buffer caching are: 
//
// 1. Character models where only the martrices change between frames.
// 2. Spell effects where only the positions of the effect objects change between frames, and
//    not the number of objects, or the order in which the objects are rendered. (And there is
//    no call to GetExtents() or UpdateExtents(), which causes the buffers to be flushed).
//
// Objects which should NOT be cached are:
//
// 1. Any object where the vertex, texture, face, or other rendering parameter other than
//    the matrix changes from frame to frame.  Caching these objects will prevent these
//    changes from taking place.
//
// 2. Any object which uses calls to GetExtents() DURING the render function.  Each call to
//    set extents causes the execute buffer system to flush the current buffer and render it,
//    then start a new buffer.  This means that the buffer you cache will only have the last
//    object rendered after the last call to GetExtents().
//
// 3. Any object in which the order in which the object are drawn changes over time.  This will
//    have the effect of causing the wrong matrix (position) to be applied to the wrong object.
//
// Note, the system will automatically detect calls to GetExtents(), and prevent objects which
// call this function during a render from being cached.  However, texture, material, vertex, 
// and face changes will not be detected, and won't work unless you set the cache flag correctly.

class T3DAnimator : public TObjectAnimator
{
  protected:  // This stuff is none of your business        
    DWORD            flags;                   // Rendering flags for this object
    int              animid;                  // Id of 3DScene object
    T3DAnimObjArray  animobjs;                // Array of animation objects for this animator
    T3DControllerArray controllers;           // Array of 3D controllers for this object
    int contprevstate;                        // Previous state for controller system
    BOOL             changed;                 // True if pos or rot has changed
    D3DVECTOR        pos;                     // Position of object
    D3DVECTOR        rot;                     // Rotation for object
    D3DVECTOR        scl;                     // Scale for object
    D3DRECT          extents;                 // Last render extents
    D3DMATRIX        matrix;                  // Current render matrix
    BOOL             updated;                 // Background was updated (call update funcs if still false after render)

  public:
    T3DAnimator(PTObjectInstance oi) : TObjectAnimator(oi) {}
      // Constructor (initialization handled by Initialize)
    virtual ~T3DAnimator();
      // Closes the object

    virtual void Initialize();
      // Calls the SetupObjects() function to setup 
    virtual void Close();
      // Deletes all objects, etc.

    DWORD GetFlags() { return flags; }
      // Gets rendering flags
    void SetFlags(DWORD newflags) { flags = newflags; }
      // Sets rendering flags
    PT3DImagery Get3DImagery() { return (PT3DImagery)image; }
      // Easy way to get 3DImagery pointer

    virtual void RefreshControllers(int state);
      // Called every pulse to add or remove controllers from the controller list
    int NumControllers() { return controllers.NumItems(); }
      // Returns number of controllers
    PT3DController GetController(int num) { return controllers[num]; }
      // Gets a particular controller
    void AddController(PT3DController cont) { controllers.Add(cont); }
      // Adds a controller
    void RemoveController(int num) { controllers.Collapse(num, TRUE); }
      // Removes a controller from controller list
    
    virtual void Pulse();
      // Called each timer tick to update animation state.
    virtual void Animate(BOOL draw);
      // Called each timer tick to update objects animation state, most likely more stuff
      // will be done in the Render() function, which is called when the 3D system needs
      // to draw stuff.

    virtual BOOL SurfacesLost();
      // Returns TRUE if any surfaces are lost for this animator
    virtual void RestoreSurfaces();
      // If SurfacesLost() reports TRUE, is called to cause the surfaces to be reloaded.

  // Object Functions
    virtual void SetupObjects();
      // Redefine this function to override the default object setup.  The default
      // setup creates references to each of the imagery's objects so that imagery object
      // 0 will be the animator's object 0, 1 will be animator 1, etc.  The user can
      // completely redefine this function to create his own set of objects by
      // using the S3DAnimObj structure to override the various properties of the objects 
      // stored in the T3DImagery resource.
    void GetVerts(PS3DAnimObj obj, D3DVERTEXTYPE verttype = D3DVT_VERTEX);
      // Copies the imagery vertices for this object to the object.  Sets the OBJ3D_VERTS,
      // OBJ3D_COPYVERTS, and OBJ3D_OWNSVERTS flags.
    void FreeVerts(PS3DAnimObj obj);
      // Frees the vertices allocated by GetVerts() associated with the given object
    void GetFaces(PS3DAnimObj obj);
      // Copies the imagery faces for this object to the object.  Sets the OBJ3D_FACES,
      // OBJ3D_COPYFACES, and OBJ3D_OWNSFACES flags.  Note that it is not often necessary
      // to copy the face list if you are doing vertex animation.
    void FreeFaces(PS3DAnimObj obj);
      // Frees the faces allocated by GetFaces() associated with the give object
    int NumObjects() { return animobjs.NumItems(); }
      // Return number of objects
    PS3DAnimObj NewObject(int objnum, int flags = 0);
      // Creates an AnimObj structure based on imagery object 'objnum' to the object array.
      // All object overrides will be set to their default values after call, at which
      // point you can just set whatever override flags you want and everything will work.
      // User can specify OBJ3D_COPYVERTS and OBJ3D_COPYFACES in flags to 
      // automatically allocate a copy of an objects vertex or face array.  After you 
      // create an object you can set the flags and values, then you can (but don't have to)
      // call the AddObject() function to cause the object to be automatically rendered 
      // in the T3DAnimator Render() function.
    int AddObject(PS3DAnimObj obj);
      // Adds an animator object override structure to the animators object list.  This
      // default Render() function automatically renders all objects in this list by 
      // rendering each object one texture at a time.  
    void RemoveObject(int objnum);
      // Removes the object from the object list
    int GetObjectNum(char *name);
      // Returns object number for first object based on the imagery object with given name
      // For default SetupObjects(), the animator and imagery obj num will be the same.
    PS3DAnimObj GetObject(int objnum);
      // Returns object override data
    BOOL IsObjectEnabled(int objnum);
      // Returns true if object is enabled
    void EnableObject(int objnum, BOOL enable);
      // Enables or disables object
    BOOL RenderObject(PS3DAnimObj animobj, int tex = -1, BOOL uselastmatrix = FALSE)
      { return Get3DImagery()->RenderObject(animobj, state, frame, &matrix, tex, uselastmatrix); }
      // Calls T3DImagery RenderObject function with current state, frame, and matrix
    BOOL GetObjectMatrix(int objnum, D3DMATRIX *m);
      // Gets an object's matrix given an object number
    BOOL GetObjectPos(int objnum, D3DVECTOR &v, D3DVECTOR *s = NULL);
      // Convenient function to get an object's position given an object number. 
      // Can even translate a point 's' in that object's space if 's' is not NULL.
    BOOL GetObjectPos(char *objname, D3DVECTOR &v, D3DVECTOR *s = NULL);
      // Same as above, but allows you to use the object name
    BOOL GetObjectMapPos(int objnum, S3DPoint &pos);
      // Same as above, but returns point in MAP space (Z is slightly different)
    BOOL GetObjectMapPos(char *objname, S3DPoint &pos);
      // Same as above, but using the name instead of the number

  // Render functions   
    virtual void RefreshZBuffer()
      { ((PT3DImagery)image)->RefreshZBuffer(inst); }
      // Called for all imagery before rendering to refresh the z buffer behind the object
    void MakeMatrix(LPD3DMATRIX m);
      // Calculates a position matrix based on the current position of the object.  This
      // function is called in PreRender() to set the local 'matrix' member variable for
      // rendering.  You can call this function yourself in Animate() or Pulse() to get
      // the correct position of the object (since 'matrix' will not be set yet).
    void PreRender();
      // Called to set up lighting, model, etc. for this object before virtual Render called.
      // Can't override this.
    virtual BOOL Render();
      // Causes the 3D imagery to be rendered to the screen.  Called between 
      // a BeginScene and EndScene call to compose and execute and ExecuteBuffer.  The user
      // can completely override this function to draw his custom objects, or simply call
      // Direct3D functions himself.  This function goes through the object list and render's
      // each objects faces one texture at a time.
    void PostRender();
      // Called to reset lighting, model, etc. after virtual Render() called.  Can't override
      // this (why would you?)
    D3DMATRIX* GetMatrix() { return &matrix; };
      // Called to get the matrix of the animator!

  // Extents crap
    void ResetExtents() 
      { Get3DImagery()->ResetExtents(); updated = FALSE; }
      // Called to clear the render extents for the last rendered 3d object
    void GetExtents(LPD3DRECT extents)
      { Get3DImagery()->GetExtents(extents); }
      // Called to get the extents of the last rendered 3d object
    void AddUpdateRect(LPD3DRECT extents, int uflags = UPDATE_RESTORE)
      { if (flags & ANI3D_ADDUPDATERECT)
            Get3DImagery()->AddUpdateRect(extents, uflags); updated = TRUE; }
      // Called after rendering to cause the screen to update the 'extents' rectangle
    void UpdateBoundingRect(LPD3DRECT extents)
      { if (flags & ANI3D_UPDATEBOUNDRECT) 
            Get3DImagery()->UpdateBoundingRect(inst, state, extents); updated = TRUE; }
      // Called after rendering to cause the bounding rectangle for the current state to be updated
    BOOL WasUpdated() { return updated; }
      // True if background was refreshed for the previous render
    void UpdateExtents()
      { GetExtents(&extents); AddUpdateRect(&extents); UpdateBoundingRect(&extents); }
      // Called to do all that extents crap automatically!!
    void RecordNewExtents(PTObjectInstance oi, int state = -1, BOOL frontonly = FALSE);
      // Record screen rectangle for object for given state, or all states if -1
      // If 'front' is TRUE, records extents for 'angle=0' only (the front)

  private:
    void AnimateResetBoundRect();
      // Called from animate function to reset bounding rectangle

};

#endif
