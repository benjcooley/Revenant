// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              directdraw.h  - DirectDraw Include File                  *
// *************************************************************************

#ifndef _DIRECTDRAW_H
#define _DIRECTDRAW_H

#ifndef _WINDOWS_
#error WINDOWS.H Must be included at the top of your .CPP file
#endif

#ifndef _REVENANT_H
#include "revenant.h"
#endif

#define INITGUID
#include <ddraw.h>
#include <d3drmwin.h>
#include <dsound.h>

// Global DeviceInfo Structure
typedef struct _D3DDeviceInfo
{
    D3DCOLORMODEL cm;
    LPGUID        HWGuid;
    D3DDEVICEDESC HWDeviceDesc;
    LPGUID        SWGuid;
    D3DDEVICEDESC SWDeviceDesc;
} D3DDeviceInfo;

#define DRIVERNAMELEN 80
#define DRIVERDESCLEN 80

// Direct Draw Global variables
extern LPDIRECTDRAW      DirectDraw;        // DirectDraw pointer
extern LPDIRECTDRAW2     DirectDraw2;       // DirectDraw 2 pointer
extern char              DirectDrawDesc[DRIVERNAMELEN]; // DirectDraw device desc
extern char              DirectDrawName[DRIVERNAMELEN]; // DirectDraw device name
extern char              DriversAvailable[DRIVERNAMELEN]; // Lists names of available drivers

extern DDCAPS                DDHWCaps;          // Direct Draw Hardware Caps
extern DDCAPS                DDHELCaps;         // Direct Draw Hel Caps

// external 3d interfaces
extern LPDIRECT3D            Direct3D;         // Direct3D object
extern LPDIRECT3D2           Direct3D2;        // Direct3D object 2
extern LPDIRECT3DDEVICE      Device;           // Direct3D device
extern LPDIRECT3DDEVICE2     Device2;          // Direct3D device 2
extern LPDIRECT3DVIEWPORT    Viewport;         // Direct3D RM Viewport
extern LPDIRECT3DVIEWPORT2   Viewport2;        // Direct3D RM Viewport 2

extern D3DDEVICEDESC         D3DHWCaps;        // Direct3D Hardware Caps
extern D3DDEVICEDESC         D3DHELCaps;       // Direct3D Hardware Emulation Layer Caps

// external surfaces
extern LPDIRECTDRAWSURFACE  front;      // Pointer to DirectDraw Surfaces for the 
extern LPDIRECTDRAWSURFACE  back;       // display.
extern LPDIRECTDRAWSURFACE  zbuffer;

extern D3DDeviceInfo        DeviceInfo; // 3D device info

extern D3DMATRIX            identity;
extern D3DMATRIXHANDLE      hProj, hView, hWorld, hPos;

BOOL InitDirectDraw();
  // Sets up Direct X
BOOL InitD3DDevice();
  // Locate and setup D3D device
BOOL SetExclusiveMode();
     // Sets Exclusive access to screen
BOOL SetNormalMode();
     // Sets normal windowed access to screen
BOOL CloseDirectDraw();
  // Terminates and releases Direct X.
BOOL EnterVideoMode(int width, int height, int bitdepth);
  // Sets up DirectDraw screen resolution and color bitdepth

BOOL Setup3D(int width, int height);
  // Sets up Direct 3D
void Close3D();
  // Closes Direct 3D

int GetColorMode();
  // Returns ColorMode.
BOOL IsUsingHardware();
  // Returns ColorMode.
char *GetCapsString();
  // Returns a formatted editor string with all caps information

// Get video memory available
DWORD GetVideoMem(DWORD type, BOOL getavail = FALSE);
DWORD GetTotalVideoMem();
DWORD GetFreeVideoMem();
DWORD GetTotalLocalVideoMem();
DWORD GetFreeLocalVideoMem();
DWORD GetTotalNonLocalVideoMem();
DWORD GetFreeNonLocalVideoMem();
DWORD GetTotalTextureMem();
DWORD GetFreeTextureMem();

// *****************************************
// * Global Direct X Error/Setup Functions *
// *****************************************

#define TRY_DD(exp) { { HRESULT rval = exp; if (rval != DD_OK) { TraceErrorDD(rval, __FILE__, __LINE__); } } }
  //Global Direct Draw error handler 
#define TRY_D3D(exp) { { HRESULT rval = exp; if (rval != D3D_OK) { TraceErrorDD(rval, __FILE__, __LINE__); } } }
  //Global Direct3D error handler 
#define TRY_D3DRM(exp) { { HRESULT rval = exp; if (rval != D3DRM_OK) { TraceErrorDD(rval, __FILE__, __LINE__); } } }
  //Global Direct3DRM error handler 

BOOL FAR PASCAL DDEnumCallback(GUID FAR* GUID, LPSTR DriverDesc, LPSTR DriverName, LPVOID Context);
  // Global Direct Draw Driver Callback Routine
HRESULT CALLBACK DDEnumDisplayModesCallback(LPDDSURFACEDESC ddsd, LPVOID Context);
  // Global Direct Draw Mode Callback routine
HRESULT WINAPI D3DEnumDeviceCallBack(LPGUID Guid, LPSTR DeviceDescription, LPSTR DeviceName,
                                     LPD3DDEVICEDESC HWDesc, LPD3DDEVICEDESC HELDesc, LPVOID Context);
  // Global Direct3D driver callback routine

void TraceErrorDD(HRESULT hErr, char *sFile, int nLine);
  // Global DirectDraw error function
void TraceErrorD3D(HRESULT hErr, char *sFile, int nLine);
  // Global Direct3D error function
void TraceErrorD3DRM(HRESULT hErr, char *sFile, int nLine);
  // Global Direct3DRM error function

#endif
