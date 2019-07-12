// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             directdraw.cpp - direct draw oject module                 *  
// *************************************************************************

#define INITGUID
#include <ddraw.h>
#include <d3drmwin.h>
#include <stdio.h>

#include "revenant.h"
#include "directdraw.h"
#include "monitor.h"
#include "display.h"
#include "mainwnd.h"
#include "3dscene.h"

#define DRIVERNAMELEN 80
#define DRIVERDESCLEN 80

// Direct Draw Global variables
LPDIRECTDRAW         DirectDraw;        // DirectDraw pointer
LPDIRECTDRAW2        DirectDraw2;       // DirectDraw 2 pointer
char                 DirectDrawDesc[DRIVERNAMELEN]; // DirectDraw device desc
char                 DirectDrawName[DRIVERDESCLEN]; // DirectDraw device name
char                 DriversAvailable[DRIVERNAMELEN]; // Lists names of available drivers

DDCAPS               DDHWCaps;          // Direct Draw Hardware Caps
DDCAPS               DDHELCaps;         // Direct Draw Hel Caps

// Direct 3d 2 Global variables
LPDIRECT3D            Direct3D;         // Direct3D object
LPDIRECT3D2           Direct3D2;        // Direct3D object
LPDIRECT3DDEVICE      Device;           // Direct3D device
LPDIRECT3DDEVICE2     Device2;          // Direct3D device 2
LPDIRECT3DVIEWPORT    Viewport;         // Direct3D Viewport
LPDIRECT3DVIEWPORT2   Viewport2;        // Direct3D Viewport
D3DDEVICEDESC         D3DHWCaps;        // Direct3D Hardware Caps
D3DDEVICEDESC         D3DHELCaps;       // Direct3D Hardware Emulation Layer Caps

// Direct X Variables
DWORD         GDIMem;                   // Memory in available from GDI's surface
D3DDeviceInfo DeviceInfo;               // 3D device info
LPGUID        DeviceGuid;               // Guid to 3D device
DDCAPS        HWCaps;                   // Hardware Caps
DDCAPS        HELCaps;                  // Hardware Emulation Layer Caps
DWORD         ZBufferBitDepth;          // ZBuffer bit depth
BOOL          BlitHardware;             // Is blit hardware available?
BOOL          Hardware3D = FALSE;       // Do we have hardware?
BOOL          UsingHardware = FALSE;    // Are we using hardware?

// External surfaces
extern LPDIRECTDRAWSURFACE back;

// The driver setup function in exlmain.cpp.  Sets system flags based on driver
// we are using.
void DriverSetupCallback();

// Get available enum
BOOL FAR PASCAL DDEnumGetAvailable(GUID FAR* GUID, LPSTR DriverDesc, LPSTR DriverName, 
    LPVOID Context);

BOOL InitDirectDraw()
{
    DDSURFACEDESC DirectDrawSurfaceDesc;

    DriversAvailable[0] = NULL;
    TRY_DD(DirectDrawEnumerate(DDEnumGetAvailable, NULL))

    // Enumerate DirectDraw drivers to see what is installed, preferring one with
    // Hardware 3D capabilities

    DirectDraw = NULL;
    TRY_DD(DirectDrawEnumerate(DDEnumCallback, NULL))

    // If DirectDraw is NULL, there is no 3D hardware present
    if (!DirectDraw)
        TRY_DD(DirectDrawCreate(NULL, &DirectDraw, NULL))

    // Call the setup callback in the main module to set system flags before continueing
    DriverSetupCallback();

    // Get other interfaces
    TRY_DD(DirectDraw->QueryInterface(IID_IDirectDraw2, (LPVOID *)&DirectDraw2))

    DDHWCaps.dwSize = sizeof(DDCAPS);
    DDHELCaps.dwSize = sizeof(DDCAPS);
    TRY_DD(DirectDraw2->GetCaps(&DDHWCaps, &DDHELCaps))

    // Get the current display mode as we can use that memory when full
    // screen exclusive
    memset(&DirectDrawSurfaceDesc, 0, sizeof(DirectDrawSurfaceDesc));
    DirectDrawSurfaceDesc.dwSize = sizeof(DirectDrawSurfaceDesc);

    TRY_DD(DirectDraw->GetDisplayMode(&DirectDrawSurfaceDesc));
    GDIMem = DirectDrawSurfaceDesc.lPitch * DirectDrawSurfaceDesc.dwHeight *
             (DirectDrawSurfaceDesc.ddpfPixelFormat.dwRGBBitCount / 8);

    // Zero out caps structures
    memset(&HWCaps, 0, sizeof(DDCAPS));
    HWCaps.dwSize = sizeof(DDCAPS);

    memset(&HELCaps, 0, sizeof(DDCAPS));
    HELCaps.dwSize = sizeof(DDCAPS);

    // Get hardware capabilities
    TRY_DD(DirectDraw->GetCaps(&HWCaps, &HELCaps))

    if (HWCaps.dwCaps & DDCAPS_BLT)
        BlitHardware = TRUE;

    if (Ignore3D)
        return TRUE;

    // Global to determine whether we have hardware 3D capabilities or not
    Hardware3D = (HWCaps.dwCaps & DDCAPS_3D) && !UseSoftware3D;

    // Create Direct3D object
    if (!Ignore3D)
    {
        TRY_DD(DirectDraw->QueryInterface(IID_IDirect3D, (LPVOID *)&Direct3D))
        if (UseDirect3D2)
        {   
            Direct3D2 = NULL;
            if (DirectDraw->QueryInterface(IID_IDirect3D2, (LPVOID *)&Direct3D2) != DD_OK)
                UseDirect3D2 = FALSE; // Direct3D II isn't available! 
            if (!Direct3D2)
                UseDirect3D2 = FALSE; // Direct3D II isn't available! 
        }

        // Enumerate Direct3D devices, preferring hardware rendering over software    
        if (!InitD3DDevice())
        {
            FatalError("Error locating suitable Direct3D driver!");
            return FALSE;
        }
    }

    // Enumerate all the display modes, this is done after locating the 3D device so
    // that any mode that is not compatible with the 3D device does not get added to
    // the list of valid modes.
    TRY_DD(DirectDraw->EnumDisplayModes(0, NULL, NULL, DDEnumDisplayModesCallback))

    return TRUE;
}

BOOL InitD3DDevice()
{
    if (Ignore3D)
        return TRUE;        

    memset(&DeviceInfo, 0, sizeof(DeviceInfo));

    // Record the color model that we wish to search for in the structure passed
    // to the enumeration call back

    if (Hardware3D && !UseSoftware3D) // Hardware
        DeviceInfo.cm = D3DCOLOR_RGB;
    else
    {
        if (SoftRampMode)
            DeviceInfo.cm = D3DCOLOR_MONO;
        else if (SoftRGBMode)
            DeviceInfo.cm = D3DCOLOR_RGB;
        else
            DeviceInfo.cm = D3DCOLOR_MONO;
    }

    // Enumerate the drivers
    TRY_D3D(Direct3D->EnumDevices(D3DEnumDeviceCallBack, &DeviceInfo));

    // Test to see whether we have hardware or software
    if (DeviceInfo.HWGuid && (DeviceInfo.HWDeviceDesc.dwDeviceZBufferBitDepth 
        & DDBD_16))
    {
       // We have a hardware driver!

        ZBufferBitDepth = 16;
        
        // Use Hardware device
        DeviceGuid = DeviceInfo.HWGuid;
        
        UsingHardware = TRUE;
    }
    
    else
    {
        // We have a software driver!

        // And force the bit depth to 16
        ZBufferBitDepth = 16;

        // Default to the software device
        DeviceGuid = DeviceInfo.SWGuid;
    
        UsingHardware = FALSE;
    }

    return TRUE;
}

BOOL SetExclusiveMode()
{
    TRY_DD(DirectDraw->SetCooperativeLevel(MainWindow.Hwnd(), DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN ))

    return TRUE;
}

BOOL SetNormalMode()
{
    TRY_DD(DirectDraw->SetCooperativeLevel(NULL, DDSCL_NORMAL ))

    return TRUE;
}

// Terminates and releases DirectX components.
BOOL CloseDirectDraw()
{
    if (!Ignore3D)
    {
        // Destroy Direct3D viewport
        if (Viewport2 && UseDirect3D2)
            Viewport2->Release();
        else if (Viewport)
            Viewport->Release();
        Viewport2 = NULL;
        Viewport = NULL;

        // Destroy rendering device
        if (Device2 && UseDirect3D2)
            Device2->Release();
        else if (Device)
            Device->Release();
        Device2 = NULL;
        Device = NULL;

        // Destroy Direct3D object
        if (Direct3D2 && UseDirect3D2)
            Direct3D2->Release();
        else if (Direct3D)
            Direct3D->Release();
        Direct3D2 = NULL;
        Direct3D = NULL;
    }

    // Destroy DirectDraw object
    if (DirectDraw)
    {
        DirectDraw->RestoreDisplayMode();
        DirectDraw->Release();
        DirectDraw = NULL;
    }

    return TRUE;
}

BOOL EnterVideoMode(int width, int height, int bitdepth)
{
    // Switch video mode
    TRY_DD(DirectDraw->SetDisplayMode(width, height, bitdepth))

    return TRUE;
}

BOOL Setup3D(int width, int height)
{
    if (Ignore3D)
        return TRUE;

    // Destroy Direct3D viewport
    if (Viewport2 && UseDirect3D2)
        Viewport2->Release();
    else if (Viewport)
        Viewport->Release();
    Viewport2 = NULL;
    Viewport = NULL;

    // Destroy Direct3D device
    if (Device2 && UseDirect3D2)
        Device2->Release();
    else if (Device)
        Device->Release();
    Device2 = NULL;
    Device = NULL;

    // Create Direct3D device
    if (UseDirect3D2)
    {
        TRY_D3D(Direct3D2->CreateDevice(*DeviceGuid, back, &Device2))
        TRY_D3D(Device2->QueryInterface(IID_IDirect3DDevice, (LPVOID *)&Device))
    }
    else
    {
        Device2 = NULL;
        TRY_DD(back->QueryInterface(*DeviceGuid, (LPVOID *)&Device))
    }

    memset(&D3DHWCaps, 0, sizeof(D3DDEVICEDESC));

    memset(&D3DHELCaps, 0, sizeof(D3DDEVICEDESC));

    if (UseDirect3D2)
    {
        D3DHWCaps.dwSize = sizeof(D3DDEVICEDESC);
        D3DHELCaps.dwSize = sizeof(D3DDEVICEDESC);
        TRY_D3D(Device2->GetCaps(&D3DHWCaps, &D3DHELCaps));
    }
    else
    {
        D3DHWCaps.dwSize = sizeof(D3DDEVICEDESC);
        D3DHELCaps.dwSize = sizeof(D3DDEVICEDESC);

        TRY_D3D(Device->GetCaps(&D3DHWCaps, &D3DHELCaps));
    }

    return TRUE;        
}

// Closes 3D scene
void Close3D()
{       
    Scene3D.Close();

    // Destroy Direct3D viewport
    if (Viewport2 && UseDirect3D2)
        Viewport2->Release();
    else if (Viewport)
        Viewport->Release();
    Viewport2 = NULL;
    Viewport = NULL;

    // Destroy Direct3D device
    if (Device2 && UseDirect3D2)
        Device2->Release();
    else if (Device)
        Device->Release();
    Device2 = NULL;
    Device = NULL;
}

DWORD GetVideoMem(DWORD type, BOOL getavail)
{
    if (!DirectDraw)
        return 0;

    // Get hardware capabilities
    DDSCAPS DDSCaps;
    memset(&DDSCaps, 0, sizeof(DDSCaps));

    DWORD total, avail;
    if (DirectDraw2->GetAvailableVidMem(&DDSCaps, &total, &avail) != DD_OK)
    {
        total = avail = 0;
    }

    if (total <= 0 && ((type == DDSCAPS_VIDEOMEMORY) || (type == DDSCAPS_TEXTURE)))
    {
        total = avail = 4096 * 1024;  // Fix for cards with shitty DirectX support
    }

    if (getavail)
        return avail;
    else
        return total;
}

DWORD GetTotalVideoMem()
{
    return GetVideoMem(DDSCAPS_VIDEOMEMORY, FALSE);
}

DWORD GetFreeVideoMem()
{
    return GetVideoMem(DDSCAPS_VIDEOMEMORY, TRUE);
}

DWORD GetTotalLocalVideoMem()
{
    return GetVideoMem(DDSCAPS_LOCALVIDMEM, FALSE);
}

DWORD GetFreeLocalVideoMem()
{
    return GetVideoMem(DDSCAPS_LOCALVIDMEM, TRUE);
}

DWORD GetTotalNonLocalVideoMem()
{
    return GetVideoMem(DDSCAPS_NONLOCALVIDMEM, FALSE);
}

DWORD GetFreeNonLocalVideoMem()
{
    return GetVideoMem(DDSCAPS_NONLOCALVIDMEM, TRUE);
}

DWORD GetTotalTextureMem()
{
    return GetVideoMem(DDSCAPS_TEXTURE, FALSE);
}

DWORD GetFreeTextureMem()
{
    return GetVideoMem(DDSCAPS_TEXTURE, TRUE);
}

int GetColorMode()
{
    if (DeviceInfo.cm == D3DCOLOR_MONO)
        return MONO;
    
    else if (DeviceInfo.cm == D3DCOLOR_RGB)
        return COLOR;
    
    else 
        return 0;
}

BOOL IsUsingHardware()
{
    return UsingHardware;
}

// Global Direct Draw Driver Callback Routine
BOOL FAR PASCAL DDEnumGetAvailable(GUID FAR* GUID, LPSTR DriverDesc, LPSTR DriverName, 
    LPVOID Context)
{
    if (DriversAvailable[0] != NULL)
        strncatz(DriversAvailable, ",", DRIVERNAMELEN);
    strncatz(DriversAvailable, DriverName, DRIVERNAMELEN);
    return DDENUMRET_OK;    // Keep going through list
}

// Global Direct Draw Driver Callback Routine
BOOL FAR PASCAL DDEnumCallback(GUID FAR* GUID, LPSTR DriverDesc, LPSTR DriverName, 
    LPVOID Context)
{
    // Try to create a DirectDraw object
    LPDIRECTDRAW Driver;
    TRY_DD(DirectDrawCreate(GUID, &Driver, NULL))

    BOOL found = FALSE;
    if (DXDriverMatchStr[0] != NULL)
    {
        char buf[80];
        strncpyz(buf, DriverName, 80);
        strncatz(buf, " ", 80);
        strncatz(buf, DriverDesc, 80);
        strlwr(buf);

        found = strstr(buf, DXDriverMatchStr) != NULL;
    }
    else if (MonitorNum != 0)
    {
        found = stricmp(MonitorInfo.szDevice, DriverName) == 0;
    }
    else
    {
        // If no default monitor, search for the best 3D so that it doesn't
        // do something stupid like using software emulation when there's
        // actually a 3DFX card in the system
        DDCAPS HWCaps, HELCaps;

        // Get the DirectDraw capabilities
        memset(&HWCaps, 0, sizeof(DDCAPS));
        HWCaps.dwSize = sizeof(DDCAPS);
    
        memset(&HELCaps, 0, sizeof(DDCAPS));
        HELCaps.dwSize = sizeof(DDCAPS);
    
        TRY_DD(Driver->GetCaps(&HWCaps, &HELCaps))

        found = (HWCaps.dwCaps & DDCAPS_3D) != 0;
    }

    // If first device, or device matches criteria above, set device..
    if (!DirectDraw || found) 
    {
       if (DirectDraw)              // Override's first device found
            DirectDraw->Release();
        DirectDraw = Driver;
        strncpyz(DirectDrawName, DriverName, DRIVERNAMELEN);
        strncpyz(DirectDrawDesc, DriverDesc, DRIVERDESCLEN);
    }
    else  // This one didn't match, and we already have the default, so delete it
    {
        Driver->Release();
        Driver = NULL;
    }

    if (found)          // Found a match, don't do any more enums
        return FALSE;
    else
        return DDENUMRET_OK;    // Keep going through list
}


// Global Direct Draw Mode Callback routine
HRESULT CALLBACK DDEnumDisplayModesCallback(LPDDSURFACEDESC DirectDrawSurfaceDesc, 
    LPVOID Context)
{       
    // Ensure mode supports 3d device
    if (DeviceInfo.HWGuid)
    {           
        // Make sure there is enough video ram to support this mode
        // if hardware is in use
        DWORD BitDepthMultiplier;
        
        switch(DirectDrawSurfaceDesc->ddpfPixelFormat.dwRGBBitCount)
        {
            case 8: 
                BitDepthMultiplier = 1; 
                break;

            case 16: 
                BitDepthMultiplier = 2; 
                break;

            case 24: 
                BitDepthMultiplier = 3; 
                break;

            case 32: 
                BitDepthMultiplier = 4; 
                break;
        }

        DWORD VidRamNeeded = ((DirectDrawSurfaceDesc->dwWidth * 
            DirectDrawSurfaceDesc->dwHeight) * BitDepthMultiplier) * 3;

        if (VidRamNeeded > (HWCaps.dwVidMemFree + GDIMem))
            return DDENUMRET_OK;

        // Make sure the Direct3D device can render at a given bit depth
        switch (DirectDrawSurfaceDesc->ddpfPixelFormat.dwRGBBitCount)
        {
            case 8 : 
            {
                if (!(DeviceInfo.HWDeviceDesc.dwDeviceRenderBitDepth & DDBD_8)) 
                    return DDENUMRET_OK;
            }
            break;

            case 16 :
            {
                if (!(DeviceInfo.HWDeviceDesc.dwDeviceRenderBitDepth & DDBD_16)) 
                    return DDENUMRET_OK;
            }
            break;

            case 24 : 
            {
                if (!(DeviceInfo.HWDeviceDesc.dwDeviceRenderBitDepth & DDBD_24)) 
                    return DDENUMRET_OK;
            }
            break;

            case 32 :
            {
                if (!(DeviceInfo.HWDeviceDesc.dwDeviceRenderBitDepth & DDBD_32)) 
                    return DDENUMRET_OK;
            }
            break;
        }

        // If we have hardware, start up in 640x480x16 if possible
        if ((DirectDrawSurfaceDesc->dwWidth == 640) && 
            (DirectDrawSurfaceDesc->dwHeight == 480) && 
            (DirectDrawSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 16))
            return DDENUMRET_CANCEL;
    }

    return DDENUMRET_OK;
}

// Global Direct3D driver callback routine
HRESULT WINAPI D3DEnumDeviceCallBack(LPGUID Guid, LPSTR DeviceDescription,
    LPSTR DeviceName, LPD3DDEVICEDESC HWDesc, LPD3DDEVICEDESC HELDesc, LPVOID Context)
{
    static BOOL FoundHardwareDevice = FALSE;   

    // No need to enumerate if we already found the device that supports hardware
    if (FoundHardwareDevice) return D3DENUMRET_OK;

    D3DDeviceInfo *Info = (D3DDeviceInfo *)Context;
    
    // Is this a hardware device?
    if ((HWDesc->dcmColorModel & Info->cm) && !UseSoftware3D)
    {
        // Make sure the driver has ZBuffering capabilities
        if (HWDesc->dwDeviceZBufferBitDepth & DDBD_16)
        {                                       
            // Record the HAL description for later use
            memcpy(&Info->HWDeviceDesc, HWDesc, sizeof(D3DDEVICEDESC));

            // Record the guid for later use
            Info->HWGuid = Guid;
            
            // No need to keep looking for any more devices
            FoundHardwareDevice = TRUE;
        }
        
        return D3DENUMRET_OK;
    }

    // Is this a software device?
    if (HELDesc->dcmColorModel & Info->cm) 
    {
        // Record the HEL description for later use
        memcpy(&Info->SWDeviceDesc, HELDesc, sizeof(D3DDEVICEDESC));
            
        // Record the guid for later use
        Info->SWGuid = Guid;
    }

    return D3DENUMRET_OK;
}


struct DDERRORSTRING
{
    HRESULT Error;
    char *lpszErrorStr;
};

static DDERRORSTRING dderrors[] =
{
  // DirectDraw errors...  
    {DDERR_ALREADYINITIALIZED, "DDERR_ALREADYINITIALIZED"},
    {DDERR_CANNOTATTACHSURFACE, "DDERR_CANNOTATTACHSURFACE"},
    {DDERR_CANNOTDETACHSURFACE, "DDERR_CANNOTDETACHSURFACE"},
    {DDERR_CURRENTLYNOTAVAIL, "DDERR_CURRENTLYNOTAVAIL"},
    {DDERR_EXCEPTION, "DDERR_EXCEPTION"},
    {DDERR_GENERIC, "DDERR_GENERIC"},
    {DDERR_HEIGHTALIGN, "DDERR_HEIGHTALIGN"},
    {DDERR_INCOMPATIBLEPRIMARY, "DDERR_INCOMPATIBLEPRIMARY"},
    {DDERR_INVALIDCAPS, "DDERR_INVALIDCAPS"},
    {DDERR_INVALIDCLIPLIST, "DDERR_INVALIDCLIPLIST"},
    {DDERR_INVALIDMODE, "DDERR_INVALIDMODE"},
    {DDERR_INVALIDOBJECT, "DDERR_INVALIDOBJECT"},
    {DDERR_INVALIDPARAMS, "DDERR_INVALIDPARAMS"},
    {DDERR_INVALIDPIXELFORMAT, "DDERR_INVALIDPIXELFORMAT"},
    {DDERR_INVALIDRECT, "DDERR_INVALIDRECT"},
    {DDERR_LOCKEDSURFACES, "DDERR_LOCKEDSURFACES"},
    {DDERR_NO3D, "DDERR_NO3D"},
    {DDERR_NOALPHAHW, "DDERR_NOALPHAHW"},
    {DDERR_NOCLIPLIST, "DDERR_NOCLIPLIST"},
    {DDERR_NOCOLORCONVHW, "DDERR_NOCOLORCONVHW"},
    {DDERR_NOCOOPERATIVELEVELSET, "DDERR_NOCOOPERATIVELEVELSET"},
    {DDERR_NOCOLORKEY, "DDERR_NOCOLORKEY"},
    {DDERR_NOCOLORKEYHW, "DDERR_NOCOLORKEYHW"},
    {DDERR_NODIRECTDRAWSUPPORT, "DDERR_NODIRECTDRAWSUPPORT"},
    {DDERR_NOEXCLUSIVEMODE, "DDERR_NOEXCLUSIVEMODE"},
    {DDERR_NOFLIPHW, "DDERR_NOFLIPHW"},
    {DDERR_NOGDI, "DDERR_NOGDI"},
    {DDERR_NOMIRRORHW, "DDERR_NOMIRRORHW"},
    {DDERR_NOTFOUND, "DDERR_NOTFOUND"},
    {DDERR_NOOVERLAYHW, "DDERR_NOOVERLAYHW"},
    {DDERR_NORASTEROPHW, "DDERR_NORASTEROPHW"},
    {DDERR_NOROTATIONHW, "DDERR_NOROTATIONHW"},
    {DDERR_NOSTRETCHHW, "DDERR_NOSTRETCHHW"},
    {DDERR_NOT4BITCOLOR, "DDERR_NOT4BITCOLOR"},
    {DDERR_NOT4BITCOLORINDEX, "DDERR_NOT4BITCOLORINDEX"},
    {DDERR_NOT8BITCOLOR, "DDERR_NOT8BITCOLOR"},
    {DDERR_NOTEXTUREHW, "DDERR_NOTEXTUREHW"},
    {DDERR_NOVSYNCHW, "DDERR_NOVSYNCHW"},
    {DDERR_NOZBUFFERHW, "DDERR_NOZBUFFERHW"},
    {DDERR_NOZOVERLAYHW, "DDERR_NOZOVERLAYHW"},
    {DDERR_OUTOFCAPS, "DDERR_OUTOFCAPS"},
    {DDERR_OUTOFMEMORY, "DDERR_OUTOFMEMORY"},
    {DDERR_OUTOFVIDEOMEMORY, "DDERR_OUTOFVIDEOMEMORY"},
    {DDERR_OVERLAYCANTCLIP, "DDERR_OVERLAYCANTCLIP"},
    {DDERR_OVERLAYCOLORKEYONLYONEACTIVE, "DDERR_OVERLAYCOLORKEYONLYONEACTIVE"},
    {DDERR_PALETTEBUSY, "DDERR_PALETTEBUSY"},
    {DDERR_COLORKEYNOTSET, "DDERR_COLORKEYNOTSET"},
    {DDERR_SURFACEALREADYATTACHED, "DDERR_SURFACEALREADYATTACHED"},
    {DDERR_SURFACEALREADYDEPENDENT, "DDERR_SURFACEALREADYDEPENDENT"},
    {DDERR_SURFACEBUSY, "DDERR_SURFACEBUSY"},
    {DDERR_CANTLOCKSURFACE, "DDERR_CANTLOCKSURFACE"},
    {DDERR_SURFACEISOBSCURED, "DDERR_SURFACEISOBSCURED"},
    {DDERR_SURFACELOST, "DDERR_SURFACELOST"},
    {DDERR_SURFACENOTATTACHED, "DDERR_SURFACENOTATTACHED"},
    {DDERR_TOOBIGHEIGHT, "DDERR_TOOBIGHEIGHT"},
    {DDERR_TOOBIGSIZE, "DDERR_TOOBIGSIZE"},
    {DDERR_TOOBIGWIDTH, "DDERR_TOOBIGWIDTH"},
    {DDERR_UNSUPPORTED, "DDERR_UNSUPPORTED"},
    {DDERR_UNSUPPORTEDFORMAT, "DDERR_UNSUPPORTEDFORMAT"},
    {DDERR_UNSUPPORTEDMASK, "DDERR_UNSUPPORTEDMASK"},
    {DDERR_VERTICALBLANKINPROGRESS, "DDERR_VERTICALBLANKINPROGRESS"},
    {DDERR_WASSTILLDRAWING, "DDERR_WASSTILLDRAWING"},
    {DDERR_XALIGN, "DDERR_XALIGN"},
    {DDERR_INVALIDDIRECTDRAWGUID, "DDERR_INVALIDDIRECTDRAWGUID"},
    {DDERR_DIRECTDRAWALREADYCREATED, "DDERR_DIRECTDRAWALREADYCREATED"},
    {DDERR_NODIRECTDRAWHW, "DDERR_NODIRECTDRAWHW"},
    {DDERR_PRIMARYSURFACEALREADYEXISTS, "DDERR_PRIMARYSURFACEALREADYEXISTS"},
    {DDERR_NOEMULATION, "DDERR_NOEMULATION"},
    {DDERR_REGIONTOOSMALL, "DDERR_REGIONTOOSMALL"},
    {DDERR_CLIPPERISUSINGHWND, "DDERR_CLIPPERISUSINGHWND"},
    {DDERR_NOCLIPPERATTACHED, "DDERR_NOCLIPPERATTACHED"},
    {DDERR_NOHWND, "DDERR_NOHWND"},
    {DDERR_HWNDSUBCLASSED, "DDERR_HWNDSUBCLASSED"},
    {DDERR_HWNDALREADYSET, "DDERR_HWNDALREADYSET"},
    {DDERR_NOPALETTEATTACHED, "DDERR_NOPALETTEATTACHED"},
    {DDERR_NOPALETTEHW, "DDERR_NOPALETTEHW"},
    {DDERR_BLTFASTCANTCLIP, "DDERR_BLTFASTCANTCLIP"},
    {DDERR_NOBLTHW, "DDERR_NOBLTHW"},
    {DDERR_NODDROPSHW, "DDERR_NODDROPSHW"},
    {DDERR_OVERLAYNOTVISIBLE, "DDERR_OVERLAYNOTVISIBLE"},
    {DDERR_NOOVERLAYDEST, "DDERR_NOOVERLAYDEST"},
    {DDERR_INVALIDPOSITION, "DDERR_INVALIDPOSITION"},
    {DDERR_NOTAOVERLAYSURFACE, "DDERR_NOTAOVERLAYSURFACE"},
    {DDERR_EXCLUSIVEMODEALREADYSET, "DDERR_EXCLUSIVEMODEALREADYSET"},
    {DDERR_NOTFLIPPABLE, "DDERR_NOTFLIPPABLE"},
    {DDERR_CANTDUPLICATE, "DDERR_CANTDUPLICATE"},
    {DDERR_NOTLOCKED, "DDERR_NOTLOCKED"},
    {DDERR_CANTCREATEDC, "DDERR_CANTCREATEDC"},
    {DDERR_NODC, "DDERR_NODC"},
    {DDERR_WRONGMODE, "DDERR_WRONGMODE"},
    {DDERR_IMPLICITLYCREATED, "DDERR_IMPLICITLYCREATED"},
    {DDERR_NOTPALETTIZED, "DDERR_NOTPALETTIZED"},
    {DDERR_UNSUPPORTEDMODE, "DDERR_UNSUPPORTEDMODE"},
    {DDERR_NOMIPMAPHW, "DDERR_NOMIPMAPHW"},
    {DDERR_INVALIDSURFACETYPE, "DDERR_INVALIDSURFACETYPE"},
    {DDERR_NOOPTIMIZEHW, "DDERR_NOOPTIMIZEHW"},
    {DDERR_NOTLOADED, "DDERR_NOTLOADED"},
    {DDERR_DCALREADYCREATED, "DDERR_DCALREADYCREATED"},
    {DDERR_NONONLOCALVIDMEM, "DDERR_NONONLOCALVIDMEM"},
    {DDERR_CANTPAGELOCK, "DDERR_CANTPAGELOCK"},
    {DDERR_CANTPAGEUNLOCK, "DDERR_CANTPAGEUNLOCK"},
    {DDERR_NOTPAGELOCKED, "DDERR_NOTPAGELOCKED"},
    {DDERR_MOREDATA, "DDERR_MOREDATA"},
    {DDERR_VIDEONOTACTIVE, "DDERR_VIDEONOTACTIVE"},
    {DDERR_DEVICEDOESNTOWNSURFACE, "DDERR_DEVICEDOESNTOWNSURFACE"},
    {DDERR_NOTINITIALIZED, "DDERR_NOTINITIALIZED"},

  // Direct 3D errors...  
    {D3DERR_BADMAJORVERSION,"D3DERR_BADMAJORVERSION"},
    {D3DERR_BADMINORVERSION,"D3DERR_BADMINORVERSION"},
    {D3DERR_INVALID_DEVICE,"D3DERR_INVALID_DEVICE"},
    {D3DERR_INITFAILED,"D3DERR_INITFAILED"},
    {D3DERR_DEVICEAGGREGATED,"D3DERR_DEVICEAGGREGATED"},
    {D3DERR_EXECUTE_CREATE_FAILED,"D3DERR_EXECUTE_CREATE_FAILED"},
    {D3DERR_EXECUTE_DESTROY_FAILED,"D3DERR_EXECUTE_DESTROY_FAILED"},
    {D3DERR_EXECUTE_LOCK_FAILED,"D3DERR_EXECUTE_LOCK_FAILED"},
    {D3DERR_EXECUTE_UNLOCK_FAILED,"D3DERR_EXECUTE_UNLOCK_FAILED"},
    {D3DERR_EXECUTE_LOCKED,"D3DERR_EXECUTE_LOCKED"},
    {D3DERR_EXECUTE_NOT_LOCKED,"D3DERR_EXECUTE_NOT_LOCKED"},
    {D3DERR_EXECUTE_FAILED,"D3DERR_EXECUTE_FAILED"},
    {D3DERR_EXECUTE_CLIPPED_FAILED,"D3DERR_EXECUTE_CLIPPED_FAILED"},
    {D3DERR_TEXTURE_NO_SUPPORT,"D3DERR_TEXTURE_NO_SUPPORT"},
    {D3DERR_TEXTURE_CREATE_FAILED,"D3DERR_TEXTURE_CREATE_FAILED"},
    {D3DERR_TEXTURE_DESTROY_FAILED,"D3DERR_TEXTURE_DESTROY_FAILED"},
    {D3DERR_TEXTURE_LOCK_FAILED,"D3DERR_TEXTURE_LOCK_FAILED"},
    {D3DERR_TEXTURE_UNLOCK_FAILED,"D3DERR_TEXTURE_UNLOCK_FAILED"},
    {D3DERR_TEXTURE_LOAD_FAILED,"D3DERR_TEXTURE_LOAD_FAILED"},
    {D3DERR_TEXTURE_SWAP_FAILED,"D3DERR_TEXTURE_SWAP_FAILED"},
    {D3DERR_TEXTURE_LOCKED,"D3DERR_TEXTURE_LOCKED"},
    {D3DERR_TEXTURE_NOT_LOCKED,"D3DERR_TEXTURE_NOT_LOCKED"},
    {D3DERR_TEXTURE_GETSURF_FAILED,"D3DERR_TEXTURE_GETSURF_FAILED"},
    {D3DERR_MATRIX_CREATE_FAILED,"D3DERR_MATRIX_CREATE_FAILED"},
    {D3DERR_MATRIX_DESTROY_FAILED,"D3DERR_MATRIX_DESTROY_FAILED"},
    {D3DERR_MATRIX_SETDATA_FAILED,"D3DERR_MATRIX_SETDATA_FAILED"},
    {D3DERR_MATRIX_GETDATA_FAILED,"D3DERR_MATRIX_GETDATA_FAILED"},
    {D3DERR_SETVIEWPORTDATA_FAILED,"D3DERR_SETVIEWPORTDATA_FAILED"},
    {D3DERR_INVALIDCURRENTVIEWPORT,"D3DERR_INVALIDCURRENTVIEWPORT"},
    {D3DERR_INVALIDPRIMITIVETYPE,"D3DERR_INVALIDPRIMITIVETYPE"},
    {D3DERR_INVALIDVERTEXTYPE,"D3DERR_INVALIDVERTEXTYPE"},
    {D3DERR_TEXTURE_BADSIZE,"D3DERR_TEXTURE_BADSIZE"},
    {D3DERR_INVALIDRAMPTEXTURE,"D3DERR_INVALIDRAMPTEXTURE"},
    {D3DERR_MATERIAL_CREATE_FAILED,"D3DERR_MATERIAL_CREATE_FAILED"},
    {D3DERR_MATERIAL_DESTROY_FAILED,"D3DERR_MATERIAL_DESTROY_FAILED"},
    {D3DERR_MATERIAL_SETDATA_FAILED,"D3DERR_MATERIAL_SETDATA_FAILED"},
    {D3DERR_MATERIAL_GETDATA_FAILED,"D3DERR_MATERIAL_GETDATA_FAILED"},
    {D3DERR_INVALIDPALETTE,"D3DERR_INVALIDPALETTE"},
    {D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY,"D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY"},
    {D3DERR_ZBUFF_NEEDS_VIDEOMEMORY,"D3DERR_ZBUFF_NEEDS_VIDEOMEMORY"},
    {D3DERR_SURFACENOTINVIDMEM,"D3DERR_SURFACENOTINVIDMEM"},
    {D3DERR_LIGHT_SET_FAILED,"D3DERR_LIGHT_SET_FAILED"},
    {D3DERR_LIGHTHASVIEWPORT,"D3DERR_LIGHTHASVIEWPORT"},
    {D3DERR_LIGHTNOTINTHISVIEWPORT,"D3DERR_LIGHTNOTINTHISVIEWPORT"},
    {D3DERR_SCENE_IN_SCENE,"D3DERR_SCENE_IN_SCENE"},
    {D3DERR_SCENE_NOT_IN_SCENE,"D3DERR_SCENE_NOT_IN_SCENE"},
    {D3DERR_SCENE_BEGIN_FAILED,"D3DERR_SCENE_BEGIN_FAILED"},
    {D3DERR_SCENE_END_FAILED,"D3DERR_SCENE_END_FAILED"},
    {D3DERR_INBEGIN,"D3DERR_INBEGIN"},
    {D3DERR_NOTINBEGIN,"D3DERR_NOTINBEGIN"},
    {D3DERR_NOVIEWPORTS,"D3DERR_NOVIEWPORTS"},
    {D3DERR_VIEWPORTDATANOTSET,"D3DERR_VIEWPORTDATANOTSET"},
    {D3DERR_VIEWPORTHASNODEVICE,"D3DERR_VIEWPORTHASNODEVICE"},
    {D3DERR_NOCURRENTVIEWPORT,"D3DERR_NOCURRENTVIEWPORT"}
};
#define NUMDDERRORS sizearray(dderrors)

 // Global DirectDraw Error function
void TraceErrorDD(HRESULT Err, char *file, int line)
{       
    char *dderr;
    char err[1024];

    if (Err == DDERR_SURFACELOST)
        return;

    for (int c = 0; c < NUMDDERRORS; c++)
    {
        if (Err == dderrors[c].Error)
        {
            dderr = dderrors[c].lpszErrorStr; 
            break;
        }
    }
    if (c >= NUMDDERRORS)
        sprintf(dderr, "Unknown Error"); 

    sprintf(err, "DirectX Error %s\nin file %s at line %d", dderr, file, line);
    FatalError(err);
}
