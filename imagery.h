// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  imagery.h - TObjectImagery module                    *
// *************************************************************************

#ifndef _IMAGERY_H
#define _IMAGERY_H

#include <stdio.h>
#include <time.h>

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _IMAGERES_H
#include "imageres.h"
#endif

// *************************************************************************
// * TObjectImagery/TImageryBuilder - Represents the imagery for an object *
// *************************************************************************

// The TObjectImagery class encapsulates the imagery information and functions
// for an object.  When an object is initialized, the object class creates an
// object imagery object for that type.  If an object needs to be animated, the
// object imagery object creates a TObjectAnimator object to handle the animation
// for the object.  This system allows the object to ignore the details of its own
// graphics and animation, and decouples the graphics/animation system from the
// more abstract object system.

#define MAXIMAGERYTYPES 16

// *******************
// * TImageryBuilder *
// *******************

// Builder which builds imagery objects after imagery buffer is loaded
_CLASSDEF(TImageryBuilder)
class TImageryBuilder
{
  public:
    TImageryBuilder(int newid);
        // Sets id and adds builder to builder array
    virtual PTObjectImagery Build(int id) = 0;
        // Builds an imagery object given an imagery buffer
    static PTImageryBuilder GetBuilder(int imageryid)
      { if ((DWORD)imageryid < (DWORD)numimagerytypes) return builders[imageryid]; else return NULL; }
        // Gets a pointer to a builder in the builder array

  private:
    static int numimagerytypes;                             // Number of object types
    static PTImageryBuilder builders[MAXIMAGERYTYPES];      // Imagery builder for each type

    int imageryid;
};

#define DEFINE_IMAGERYBUILDER(id, obj)                                          \
class obj##Builder : public TImageryBuilder                                     \
{                                                                               \
  public:                                                                       \
    obj##Builder() : TImageryBuilder(id) {}                                     \
    virtual PTObjectImagery Build(int id)                                       \
        { return new obj(id); }                                 \
};

#define REGISTER_IMAGERYBUILDER(obj) obj##Builder obj##BuilderInstance;

_CLASSDEF(TObjectAnimator);
_CLASSDEF(TObjectImagery)

#define MAXIMFNAMELEN       80

// ******************
// * TObjectImagery *
// ******************

_STRUCTDEF(SImageryEntry);
struct SImageryEntry
{
    char                filename[MAXIMFNAMELEN];    // Filename of resource
    DWORD               status;     // Status of loading
    PSImageryHeader     header;     // Imagery header
    int                 headersize; // Size of header
    BOOL                headerdirty;    // Header has been changed
    PSImageryBody       body;       // Imagery body
    BOOL                bodydirty;  // Body has been changed
    int                 ressize;    // Size of body resource
    int                 usecount;   // usecount
    PTObjectImagery     imagery;    // Pointer to imagery
};

class TObjectImagery
{
  public:
    TObjectImagery(int id);
    virtual ~TObjectImagery();
    virtual BOOL Restore() { return TRUE; }
      // Restors lost surfaces, textures, etc.
    int ImageryId() { return imageryid; }
      // Returns the id of this imagery (OBJIMAGE_ANIMATION, OBJIMAGE_MESH3D, etc.)

  // Main imagery functions..
    static int RegisterImagery(char *filename, PSImageryHeader header = NULL, DWORD headersize = 0);
      // Adds imagery file to internal image array.  Called by object class system when
      // object definitions are loaded from 'class.def' file.  Register's imagery file and'
      // returns an id number for that imagery.
    static PSImageryEntry GetImageryEntry(int id);
      // Return the entery array structure for the given imagery id
    static void FreeImageryEntry(int imageryentry);
      // Frees an individual imagery entry
    static void FreeAllImagery();
      // Called when program closes to cause all imagery to be freed (also optionally saves headers)
    static void ReloadImagery();
      // Deletes imagery body.
    static void SaveAllHeaders();
      // Causes header data for all imagery to be dumped to the imagery files
    static void SetImageryPath(char *path);
      // Set the imagery filepath
    static char *GetImageryPath();
      // Returns the current imagery path
    static int FindImagery(char *imageryname);
      // Finds the id of imagery given imagery file name
    static BOOL TObjectImagery::RenameImageryFile(int imgid, char *newfile);
      // Renames the file that a specific imagery id uses
    static PTObjectImagery LoadImagery(int imageryid);
      // Loads an imagery object given the imagery id (same id returned from RegisterImagery)
    static PTObjectImagery LoadImagery(char *imageryname) { return LoadImagery(FindImagery(imageryname)); }
      // Loads an imagery object given the object name and class
    static void FreeImagery(PTObjectImagery imagery);
      // Reduces use count of imagery by 1, and frees if no longer used
    static void SaveHeader(int imageryid);
      // Saves the header info for the given imagery id
    static void SetEntryReg(int imageryid, int state, int regx, int regy, int regz);
      // Sets the registration points without having an instance of the imagery
    static void RestoreAll();
      // Restores all lost imagery surfaces, etc.

    static BOOL QuickLoadHeaders(time_t iflater);
      // Does a quck load of imagery headers if IMAGERY.DAT file is later than 'iflater'
    static BOOL QuickSaveHeaders();
      // Rewrites IMAGERY.DAT file with current header data

  // Access functions for private data
    char *GetResFilename() { return entry->filename; }
      // Filename
    PSImageryHeader GetHeader() { return entry->header; }
      // Returns imagery header info
    PSImageryBody GetBody() { if (entry->body) return entry->body; else return LoadBody(TRUE); }
      // Gets the pointer to the imagery body.. loads it now if it's not there
    int GetHeaderSize() { return entry->headersize; }
      // Returns imagery header info
    DWORD GetResSize() { return entry->ressize; }
      // Size for memory tracking

  // Progressive loading stuff
    static void BeginLoaderThread();
      // Starts the progressive load thread going.. call this at the beginning of the game
    static void EndLoaderThread();
      // Ends the progressive loader thread
    static unsigned _stdcall LoaderThread(void *args);
      // Progressive loading thread for imagery.
    static void PauseLoader();
      // Causes the imagery loader to pause temporarily
    static void ResumeLoader();
      // Resumes the imagery loader
    PSImageryBody LoadBody(BOOL wait = TRUE);
      // Adds body load to the loader thread's load queue, waits for load or returns NULL.
    void FreeBody();
      // Free imagery body and sets body pointer to NULL

  // Caching functions
    virtual void CacheImagery() {}
      // Causes imagery to load itself and get ready for drawing
      // (Called by the SectorCache system to cause imagery to be loaded and decompressed)
      // AnimImagery uses this to call the TBitmap::CacheChunks() function  

  // Drawing functions, etc.
    virtual void DrawUnlit(PTObjectInstance oi, PTSurface surface) {}
        // Draws unlit imagery to background
    virtual void DrawLit(PTObjectInstance oi, PTSurface surface) {}
        // Draws lit imagery to background
    virtual BOOL GetZ(PTObjectInstance oi, PTSurface surface) { return FALSE; }
        // Find first uncliped portion of zbuffer
    virtual void DrawSelected(PTObjectInstance oi, PTSurface surface) { }
        // Causes image to draw selection (hilighting) around itself
    virtual void DrawLight(PTObjectInstance oi, PTSurface surface) {}
        // Draws a light
    virtual void DrawInvItem(PTObjectInstance oi, int x, int y);
        // Draws object's inventory bitmap to the screen
    int GetUseCount();
        // Returns number of times imagery is used by objects
    virtual BOOL AlwaysOnTop(PTObjectInstance oi) { return FALSE; }
        // Returns true if this is nonzbuffered imagery that should always be considered 'on top'

    virtual void SaveHeader() { SaveHeader(imageryid); }
        // Saves header data for this imagery object
    virtual BOOL SaveBitmap(char *path, int state = 0, BOOL zbuffer = TRUE) { return FALSE; }
        // Saves a bitmap of the current object given the current state

    virtual PTObjectAnimator NewObjectAnimator(PTObjectInstance oi) { return NULL; }
      // Creates an animator for the given object
    virtual BOOL NeedsAnimator(PTObjectInstance oi) { return FALSE; }
      // Returns whether or not an animator is necessary

  // State functions
    virtual int NumStates() { return entry->header->numstates; }
      // Returns the state structure for the given imagery
    virtual int FindState(char *name, int pcnt = -1);
      // Returns state number for a state name equal to character string.  If 'pcnt'
      // is specified, uses specified 'pcnt' value instead of random percent to 
      // find random states...
      // --------------------- RANDOM STATES -----------------------
      // If states are in the format "##%state" where ## is a number from 1-100,
      // FindState() will use 'pcnt', or generate a 1-100 number if pcnt=-1, and find
      // the state which has the percent value closest to, but not above, the random 1-100
      // pcnt value.  For three 'walk' states with a frequency of 20%, 20%, and 60%, you 
      // would use "20%walk","40%walk", and "100%walk".  NOTE: you can't use two "20%walk"
      // states as the state names would then not be unique.  The closest to but not
      // above way of specifying percentages in the name ensures that each state name IS
      // unique.
    virtual int FindTransitionState(char *from, char *to, int pcnt = -1);
      // Returns state number for transition state matching "<from> to <to>".  The
      // 'pcnt' value specifies the 'pcnt' used to find a random state (see above).  If 
      // pcnt is -1, function will generate a value from 1-100 to find a random state.
      // Note: random states must be in the format "##%state" where ## is value from 1-100.
    PSImageryStateHeader GetState(int index)
        { if ((DWORD)index >= (DWORD)entry->header->numstates) return NULL;
            else return &(entry->header->states[index]); }
        // Returns the state structure for the given imagery

  // Motion functions
    virtual BOOL GetMotion(int state, int frame, int &dist, int &ang, int &face)
      { return FALSE; }
      // Gets the motion data for the given state and frame
    virtual void SetObjectMotion(PTObjectInstance inst) { } 
      // Sets the motion for an object based on its state, frame, and 3D motion data

  // Get/set Header Info (will also cause header to reallocate itself)
    virtual void SetHeaderDirty(BOOL newdirty) { entry->headerdirty = newdirty; }
      // Set the header's update status
    virtual BOOL GetHeaderDirty() { return entry->headerdirty; }
      // Get the header's update status
    virtual void SetBodyDirty(BOOL newdirty) { entry->bodydirty = newdirty; }
      // Set the body's update status
    virtual BOOL GetBodyDirty() { return entry->bodydirty; }
      // Get the body's update status
    virtual BYTE *GetWalkMap(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            ((BYTE *)entry->header->states[state].walkmap.ptr()) : NULL; }
      // Returns the walk map for the item
    virtual int GetRegX(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].regx) : (0); }
      // Gets the screen registration point for the state
    virtual int GetRegY(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].regy) : (0); }
      // Gets the screen registration point for the state
    virtual int GetRegZ(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].regz) : (0); }
      // Gets the screen registration point for the state
    virtual void SetReg(int state, int regx, int regy, int regz)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
        {entry->header->states[state].regx = regx;
         entry->header->states[state].regy = regy;
         entry->header->states[state].regz = regz;
         entry->headerdirty = TRUE; } }
      // Sets the registration point for the state
    virtual int GetAnimRegX(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].animregx) : (0); }
      // Gets the screen registration point for the state
    virtual int GetAnimRegY(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].animregy) : (0); }
      // Gets the screen registration point for the state
    virtual int GetAnimRegZ(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].animregz) : (0); }
      // Gets the screen registration point for the state
    virtual void SetAnimReg(int state, int regx, int regy, int regz)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
        {entry->header->states[state].animregx = regx;
         entry->header->states[state].animregy = regy;
         entry->header->states[state].animregz = regz;
         entry->headerdirty = TRUE; } }
      // Sets the registration point for the state
    virtual int GetFlags(int state)
      { return entry->header->states[state].flags; }
      // Returns imagery state flags.
    virtual char *GetAniName(int state)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
            return entry->header->states[state].animname;
            return NULL; }
      // Returns state animation name
    virtual int GetAniFlags(int state)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
            return entry->header->states[state].aniflags;
            return 0; }
      // Returns state animation flags (ping pong, reverse, etc.)
    virtual int GetAniLength(int state)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
            return entry->header->states[state].frames;
            return 0; }
      // Returns length (in frames) of animation for this state
    virtual int GetInvAniFlags(int state)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
            return entry->header->states[state].invaniflags;
            return 0; }
      // Returns inventory animation flags (ping pong, reverse, etc.)
    virtual int GetInvAniLength(int state)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
            return entry->header->states[state].invframes;
            return 0; }
      // Returns length (in frames) of inventory animation for this state
    virtual int GetWidth(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].width) : (0); }
      // Returns the maximum width of states graphics imagery for all frames in screen pixels
    virtual int GetHeight(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].height) : (0); }
      // Returns the maximum height of states graphics screen imagery for all frames    in screen pixels
    virtual void SetWidthHeight(int state, int width, int height)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
        {entry->header->states[state].width = width;
         entry->header->states[state].height = height;
         entry->headerdirty = TRUE; } }
      // Sets the imagery width and height for a state
    virtual void GetScreenRect(PTObjectInstance oi, SRect &r);
      // Returns screen rectangle for object given the values returned by GetWidth(),GetHeight()
    virtual void GetAnimRect(PTObjectInstance oi, SRect &r);
      // Returns screen rectangle for object given the values returned by GetWidth(),GetHeight()
    virtual void ResetScreenRect(PTObjectInstance oi, int state = -1, BOOL frontonly = FALSE);
      // Reset screen rectangle for object for given state, or all states if -1
    virtual void GetWorldBoundBox(int state, int &width, int &length, int &height)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
        { width = entry->header->states[state].wwidth;
          length = entry->header->states[state].wlength;
          height = entry->header->states[state].wheight;} }
      // Returns the width, length, and height of the object in 3D world space
    virtual void SetWorldBoundBox(int state, int width, int length, int height);
      // Sets the world bounding box for this state.  The width and length is used for walk map size
    virtual int GetWorldRegX(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].wregx) : (0); }
      // Gets the world registration point for the state for the world bounding box and walk map
    virtual int GetWorldRegY(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].wregy) : (0); }
      // Gets the world registration point for the state for the world bounding box and walk map
    virtual int GetWorldRegZ(int state)
      { return ((DWORD)state < (DWORD)entry->header->numstates) ?
            (entry->header->states[state].wregz) : (0); }
      // Gets the world registration point for the state for the world bounding box and walk map
    virtual void SetWorldReg(int state, int regx, int regy, int regz)
      { if ((DWORD)state < (DWORD)entry->header->numstates)
        { entry->header->states[state].wregx = regx;
          entry->header->states[state].wregy = regy;
          entry->header->states[state].wregz = regz;
          entry->headerdirty = TRUE; } }
      // Sets the world registration in world space
    virtual PTBitmap GetStillImage(int state, int num = 0) { return NULL; }
        // Get still bitmap, if any
    virtual PTAnimation GetAnimation(int state) { return NULL; }
        // Get animation for state
    virtual PTBitmap GetInvImage(int state, int num = 0) { return NULL; }
        // Returns the inventory image for the item
    virtual PTAnimation GetInvAnimation(int state) { return NULL; }
        // Get inventory animation for state
    virtual DWORD GetImageFlags(int state) { return 0; }
        // Gets imagery flags for state
    virtual char *GetObjName(int obj) { return NULL; }
        // Get the name of an object within the 3d mesh
    virtual int GetWeaponObj() { return -1; }
        // Get the object number for the character's weapon
    virtual int GetNumObjs() { return 0; }
        // Get the number of objects in the mesh

  protected:
    PSImageryEntry      entry;                      // Pointer to entry in entry array

  private:
    int                 imageryid;                  // Index into imagery entry array
};

// *******************
// * TObjectAnimator *
// *******************

class TObjectAnimator
{
  protected:
    TObjectInstance *inst;  // Instance associated with animator
    TObjectImagery  *image; // Imagery for animation
    int prevstate;          // Previous state (set when state changes, or animation loops)
    int state;              // Current state (checks against object every frame)
    int frame;              // Current anim frame
    int framerate;          // Direction of animation
    BOOL newstate;          // True if new state has just been entered
    BOOL complete;          // When a (non-looping) animation has exhausted its frames

  public:
    TObjectAnimator(PTObjectInstance oi);
      // Constructor. Sets objectimagery. and object instance to NULL.
    virtual ~TObjectAnimator();
      // Destructor.

    PTObjectInstance GetObjInst() { return inst; }
      // Returns the object instance for this animator
    PTObjectImagery GetImagery() { return image; }
      // Returns the animators imagery

    virtual void Initialize() {}
      // Initializes the animator for use
    virtual void Close() {}
      // Frees all allocated memory, etc.

    virtual void ResetState();
      // Restart the current state
    virtual void SetComplete(BOOL comp) { complete = comp; }
      // The animation is complete (set to FALSE when ResetState() caled and TRUE when anim is done)
    virtual void SetNewState(BOOL newst) { newstate = newst; }
      // The this is a new state (set to TRUE when ResetState() is called, and FALSE after first frame)
    virtual void Pulse() {}
      // Called during each timer Pulse() to update animaton.
    virtual void Animate(BOOL draw);
      // Animates object

    int GetFrame() { return frame; } 
      // Gets the current animation frame
    int GetFrameRate() { return framerate; }
      // Returns the direction and rate of animation

    void ClearPrevState() { state = prevstate = -1; }
      // Clears previous state (prevents interpolation)
};


#endif
