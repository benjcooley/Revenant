// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 3dScene.h - 3D Surface Include File                   *
// *************************************************************************

#ifndef _3DSCENE_H
#define _3DSCENE_H

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#ifndef _GRAPHICS_H
#include "graphics.h"
#endif

#ifndef __DDRAW_INCLUDED__
#include <ddraw.h>
#endif

#ifndef __D3DRMWIN_H__
#include <d3drmwin.h>
#endif

_CLASSDEF(T3DScene)
_CLASSDEF(T3DAnimator)
_CLASSDEF(TExecuteBuf)

// ***********************************************
// * TExecuteBuf - Execute Buffer Wrapper Object *
// ***********************************************

// The TExecuteBuf object is used by T3DScene to convert DrawPrimitive calls to
// a D3DEXECUTEBUFFER.  When the 3DScene begins drawing, it calls Begin() for its
// own internal TExecuteBuf object to begin recording draw primitive calls to an 
// execute buffer.  Then when the scene is finished, the End() function is called,
// and the execute buffer is rendered.
//
// The 3DScene uses the functions BeginScene(), EndScene(), BeginMesh(), and EndMesh()
// to control DrawPrimitive recording to its own internal execute buffer.  The buffer
// is cleared whenever BeginScene() or BeginMesh() is called, (BeginScene() also
// calls D3D's Device->BeginScene(), thus the difference), then all calls to the DrawPrimitive
// functions are recorded in the internal execute buffer until a EndMesh() or EndScene() is
// encountered, at which point the execute buffer's End() is called, and then Render() is
// called.

// The user can grab a copy of an execute buffer by calling 3DScene.CopyExecuteBuf().
// This buffer can then be used to override the scenes internal execute buffer, 
// with a call to UseExecuteBuf() which causes the 3Dscene to ignore all new 
// DrawPrimitive calls after the current BeginMesh() and just render
// the cached buffer when EndMesh() is finally reached.

class TExecuteBuf
{
  private:
    int verttype;                       // Current vertex type for this buffer (there can be only one)
    LPDIRECT3DEXECUTEBUFFER exBuf;      // Pointer to execute buffer (NULL if not recorded yet)
    D3DEXECUTEBUFFERDESC debDesc;       // Execute buffer description (for quick access to stuff)

  // Recording helper functions
    void FlushStates();
      // Causes 
    void CheckVertBufSize(int size);
      // Checks buffer size, and reallocates buffer if too small
    void CheckTransBufSize(int size);
      // Checks buffer size, and reallocates buffer if too small
    void CheckFaceBufSize(int size);
      // Checks buffer size, and reallocates buffer if too small
  public:
    static void Initialize();
      // Initilizes matrix array and buffers for execute buffer system (called only once)
    static void Close();
      // Gets rid of matrices and buffers for execute buffer system (called only once)

    TExecuteBuf(); 
    TExecuteBuf(TExecuteBuf &eb);
    ~TExecuteBuf();
    void BeginRecord();
      // Creates fresh buffer for recording primitives
    void EndRecord();
      // Ends recording, and creates execute buffer
    void Render(LPD3DCLIPSTATUS cs = NULL);
      // Renders the execute buffer (sets extents to render extents if not null)
    BOOL IsRecording();
      // Returns true if currently recording execute buf
    LPDIRECT3DEXECUTEBUFFER Buffer() { return exBuf; }
      // Returns current execute buffer, or NULL if none recorded yet
    LPD3DEXECUTEBUFFERDESC BufferDesc() { return &debDesc; }
      // Returns current execute buffer description
    D3DVERTEXTYPE VertexType() { return (D3DVERTEXTYPE)verttype; }
      // Returns type of vertex used in the execute buffer or FFFFFFFF if not recorded yet

  // D3D Compatible functions 
    HRESULT SetClipStatus(LPD3DCLIPSTATUS cs);
    HRESULT SetRenderState(D3DRENDERSTATETYPE rs, DWORD data);                    
    HRESULT SetLightState(D3DLIGHTSTATETYPE ls, DWORD data);
    HRESULT SetTransform(D3DTRANSFORMSTATETYPE ts, D3DMATRIXHANDLE matrix); // Note: THIS TAKES HANDLES!!!
    HRESULT DrawIndexedPrimitive(
        D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, LPWORD i, DWORD ic, DWORD f);
    HRESULT DrawPrimitive(
        D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, DWORD f)
        { return DrawIndexedPrimitive(pt, vt, v, vc, NULL, 0, f); }
};

// ************************************
// * T3DScene - 3D System Main Object *
// ************************************

// The main 3D system object does the following:
//
// 1. It initializes the viewport to the correct game view.
//
// 2. It sets the camera position in realtime 3D space (based on map coordinates)
//
// 3. It allows for the setting of the 3D ambient light
//
// 4. It allows for the adding and deleting of realtime 3D lights
//
// 5. It maintains a list of realtime 3D animators (objects based on T3DAnimator)
//    and calls their individual Render() functions when RenderScene() is called.
// 
// 6. It maintains a list of valid texture formats, and provides the function
//    ConvertTexture() to automatically convert textures from the format saved in the I3D
//    resource file to a format supported by the device.
//
// 7. Provides functions to virtualize a subset of the DrawPrimitive DirectX 5.0 functions
//    and convert them to DirectX 3.0 compatible Execute Buffers.  Game code should avoid
//    using specific DirectX 5.0 functions, and instead call the functions provided here.

class T3DScene
{
  protected:
    BOOL initialized;                               // 3DScene is initialized

  public:
    T3DScene();
      // Constructor. Marks all frames and lights free
    ~T3DScene();
      // Destructor.

    BOOL Initialize();
      // Sets up viewport, ambient light, etc. etc.
    BOOL SetSize(int x, int y, int width, int height);
      // Sets the 3D Vieport size
    BOOL InitializeMatrices();
      // Initialize view matrices
    BOOL Close();
      // Closes scene, removes viewport, etc.

    void RestoreZBuffer(SRect &r);
      // Restores a rectangle in the screen zbuffer... uses either DrawRestoreRect, or
      // Viewport->Clear() depending on whether the display is using a clear zbuffer surface
      // or not.  This function is called by the T3DImage RefreshZBuffer() routine to 
      // set the zbuffer correctly before a 3D object is drawn to the screen.

  // Ambient light funcitons
    void SetAmbientLight(int ambient);
      // Changes ambient lighting
    void SetAmbientColor(RSColor color);
      // Sets Ambient Light value.

  // Miscellaneous functions  
    void SetCameraPos(RS3DPoint pos, int zdist);
      // Sets Camera Position
    BOOL UpdateCamera();
      // Recalculates camera frame and position.
    void RefreshZBuffer();
      // Refresh the zbuffer - call before any realtime zbuffer animation occurs
    BOOL DrawScene();
      //  Draws all objects using Direct 3D

  // Light functions
    int AddLight(RS3DPoint pos, SColor color, int intensity, int multiplier);
      // Adds light to scene. Returns lightid
    BOOL DeleteLight(int lightid);
      // Removes light from scene. 
    BOOL SetLightIntensity(int lightid, int intensity);
      // Changes position of frame relative to camera frame.
    BOOL SetLightMultiplier(int lightid, int multiplier);
      // Changes position of frame relative to camera frame.
    BOOL SetLightColor(int lightid, SColor color);
      // Changes position of frame relative to camera frame.
    BOOL SetLightPosition(int lightid, RS3DPoint pos);
      // Changes position of frame relative to camera frame.
    int GetNumLights();
      // Returns number of lights in scene.
    BOOL GetClosestLights(int x, int y, int z, int &minlight1, int &minlight2, int &minlight3);
      // Gets index number of up to three closest lights (depending on system MaxLights
      // maximum light per object value)
    BOOL LightAffectObject(int x, int y, int z);
      // Causes first light near character to turn on and affect character
    void GetLightBrightness(int lightid, int x, int y, int z, int &brightness);
      // Gets the light brightness value for the given light in (0-255) range.
    void GetLightBrightness(int lightid, int x, int y, int z, float &brightness);
      // Gets the light brightness value for the given light in 0.0-1.0.
    void GetLightColor(int lightid, int x, int y, int z, int &r, int &g, int &b);
      // Gets the light color value for the given light in (0-255) range.
    void GetLightColor(int lightid, int x, int y, int z, D3DVALUE &r, D3DVALUE &g, D3DVALUE &b);
      // Gets the light color value for the given light in (0-255) range.
    void ResetAllLights();
      // Causes all lights to turn back off

  // Object functions   
    int AddAnimator(PT3DAnimator animator);
      // Adds animator object to 3DScene system
    BOOL RemoveAnimator(DWORD animid);
      // Removes animator object from scene.
    int GetNumAnimators();
      // Returns number of animators in scene.

  // Texture mapping stuff
    void GetTextureFormats();
      // Causes the system to find the closest supported texture format to the desired format
    void GetClosestTextureFormat(LPDDSURFACEDESC srcsd, LPDDSURFACEDESC dstsd);
      // Searches through texture format list to find closest format to srcsd
    void ConvertTexture(
        LPDDSURFACEDESC srcsd, LPVOID srcpixels, LPPALETTEENTRY srcpal,  
        LPDDSURFACEDESC dstsd, LPVOID dstpixels, LPPALETTEENTRY dstpal);
      // Converts the source texture described by srcsd to the destination texture..
      // Will also optionally generate mask alpha data if the keycolor parameter is
      // not NULL.

  // Duplicate D3D Functions (Can only be called between BeginRender() and EndRender())
    HRESULT BeginScene();
    HRESULT EndScene();
    HRESULT SetClipStatus(LPD3DCLIPSTATUS cs);
    HRESULT GetClipStatus(LPD3DCLIPSTATUS cs);
    HRESULT SetRenderState(D3DRENDERSTATETYPE rs, DWORD data);
    HRESULT GetRenderState(D3DRENDERSTATETYPE rs, DWORD *data);
    HRESULT SetLightState(D3DLIGHTSTATETYPE ls, DWORD data);
    HRESULT GetLightState(D3DLIGHTSTATETYPE ls, DWORD *data);
    HRESULT SetTransform(D3DTRANSFORMSTATETYPE ts, LPD3DMATRIX matrix);
    HRESULT GetTransform(D3DTRANSFORMSTATETYPE ts, LPD3DMATRIX matrix);
    HRESULT DrawIndexedPrimitive(
        D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, LPWORD i, DWORD ic, DWORD f);
    HRESULT DrawPrimitive(
        D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, DWORD f);

  // Blue software renderer function for setting texture.  Use this instead
  // of the SetRenderState(HTEXTURE) function!!! OR ELSE!
    HRESULT SetTexture(DWORD hTexture, LPDIRECTDRAWSURFACE surface);

  // Creates and manages matrix arrays for the SetTransform() function.  Since DrawPrimitive
  // functions don't use matrix handles, we have to allocate an array of matrix handles that
  // we can use with execute bufs to imitate the DrawPrimitive matrix system.
    void CreateMatrixList();
      // Creates an array of D3DMATRIXHANDLES handles for the SetTransform() function to use
    void ClearMatrixList();
      // Clears this matrix list when program exits
    D3DMATRIXHANDLE GetMatrix(int matrixnum);
      // Use this to get one of these legendary matrices
    int GetCurMatrix();
      // Returns the current matrix number in the recording (use in GetMatrix() above)

  // The functions below may be used by the application to save an execute buffer, and re-use
  // it over and over again instead of having the system create a new one every time.   
    BOOL WasRenderFlushed();
      // Returns TRUE if we had to flush the current execute buffer in the middle of a Render() so
      // we could get the extents.  This means we won't be able to cache the last render.
    HRESULT BeginRender();
      // Begins rendering for a single object (sets up execute buffer, etc.)
      // Must be called before all rendering to a particular buffer takes place (i.e.
      // 3DAnimator's PreRender() function.
    HRESULT EndRender();
      // Ends rendering for a single object (ends recording of current execute buffer, and renders it)
      // Must be called after all rendering to a particular buffer takes place (i.e.
      // 3DAnimator's PostRender() function.
    PTExecuteBuf GetExecuteBuf();
      // Returns a pointer to the current execute buffer (during, before, or after a render)
    PTExecuteBuf CopyExecuteBuf();
      // Gets a copy of the previously recorded execute buffer.  Must be called AFTER EndRender().
    void UseExecuteBuf(PTExecuteBuf usebuf);
      // Ignores all calls to DrawPrimitive functions, and just renders the supplied buffer
      // to render when EndMesh() or EndScene() is called.  Must be called before BeginRender()
      // is called.
    
};

// Inline functions to do rotations about a center h, k

// rotate x coords
float RotateX(float h, float k, float x, float y, float angle);
// rotate y coords
float RotateY(float h, float k, float x, float y, float angle);
// add z rotation here

#endif
