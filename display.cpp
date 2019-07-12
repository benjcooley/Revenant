// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 display.cpp  - Display Object File                    *
// *************************************************************************

#include <windows.h>
#include <ddraw.h>
#include <d3drmwin.h>

#include "revenant.h"
#include "directdraw.h"
#include "graphics.h"
#include "mainwnd.h"
#include "bitmap.h"
#include "multisurface.h"
#include "display.h"

LPDIRECTDRAWSURFACE front;      // Pointer to DirectDraw Surfaces for the display.   
LPDIRECTDRAWSURFACE back;
LPDIRECTDRAWCLIPPER clipper;    
LPDIRECTDRAWSURFACE zbuffer;

extern DWORD ZBufferBitDepth;
extern LPDIRECTDRAW         DirectDraw;       // DirectDraw pointer

// Initializes Color Tables
extern BOOL  Hardware3D;
extern BOOL  UsingHardware;
extern void  MakeColorTables();

// These functions are used to record a list of update ares on the screen.
// The low level graphics functions all are passed the address of a function called
// 'callback' in their drawparam structures.  For the display, this funciton is set
// to AddUpdateCallback(), which logs the rectangle drawn in a list.  When all the 
// drawing is complete, the low level draw functions return, and the list is added
// to the display dirty rectangle update list with a call to ProcessUpdateCallbacks().

void ResetUpdateCallbacks();
void ProcessUpdateCallbacks();
void AddUpdateCallback(PSDrawBlock db, PSDrawParam dp);

TDisplay::TDisplay()
{
    Front = Back = NULL;

    clipx  = clipy  = 0;
    currentpage     = 0;

    clipmode        = CLIP_EDGES; 
}

BOOL TDisplay::Initialize(int dwidth, int dheight, int dbitsperpixel)
{
    if (Front)
        return TRUE;

    DDSURFACEDESC ddsd;
    DDSCAPS ddscaps;

    SaveZBuffer = NULL;

    ResetUpdateCallbacks(); // Make sure our update list is reset

    InitDirectDraw();

  // We must use single buffering when UsingHardware=FALSE, or Windowed=TRUE    
    if (Windowed || !UsingHardware)
        SingleBuffer = TRUE;

    if (SingleBuffer)
    {
        if (Windowed)
        {
            SetNormalMode();
        }
        else
        {
            SetExclusiveMode();
            EnterVideoMode(dwidth, dheight, dbitsperpixel);
        }

        // First, create complex flipping primary surface

        memset(&ddsd, sizeof(DDSURFACEDESC), 0);
        ddsd.dwSize            = sizeof(DDSURFACEDESC);
        ddsd.dwFlags           = DDSD_CAPS;
        ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE;

        // Create the primary surface
        TRY_DD(DirectDraw->CreateSurface(&ddsd, &front, NULL))

        ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
        ddsd.dwWidth = WIDTH;
        ddsd.dwHeight = HEIGHT;
        ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;
        if (UsingHardware)
            ddsd.ddsCaps.dwCaps |= DDSCAPS_VIDEOMEMORY;     // Use video memory for back buffer
        else                                                // if not using hardware 3D.
            ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;    

        TRY_DD(DirectDraw->CreateSurface(&ddsd, &back, NULL))

        ddsd.dwSize  = sizeof (ddsd);
        ddsd.dwFlags = DDSD_PIXELFORMAT;
        TRY_DD(back->GetSurfaceDesc(&ddsd))

        if (ddsd.ddpfPixelFormat.dwRGBBitCount != 16)
            FatalError("Game must run in a 16 bit per pixel mode");

        TRY_DD(DirectDraw->CreateClipper(0, &clipper, NULL))
        TRY_DD(clipper->SetHWnd(0, MainWindow.Hwnd()))
        TRY_DD(front->SetClipper(clipper))
    }
    else  // Exclusive full screen mode
    {

        SetExclusiveMode();
        EnterVideoMode(dwidth, dheight, dbitsperpixel);

        // First, create complex flipping primary surface

        memset(&ddsd, sizeof(DDSURFACEDESC), 0);
        ddsd.dwSize            = sizeof(DDSURFACEDESC);
        ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
        ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | 
                                                  DDSCAPS_COMPLEX | DDSCAPS_3DDEVICE;
        ddsd.dwBackBufferCount = 1;

        // Create the primary surface with 1 back buffer
        TRY_DD(DirectDraw->CreateSurface(&ddsd, &front, NULL))

        // Get pointer to back buffer
        ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
    
        TRY_DD(front->GetAttachedSurface(&ddscaps, &back))
    }

    Front = new TDDSurface(front);
    Back  = new TDDSurface(back);

    surface      = back;
    flags        = Back->flags;
    stride       = Back->Stride();
    originx      = 0;
    originy      = 0;
    clipmode     = CLIP_EDGES;
    clipx        = 0;
    clipy        = 0;
    width        = dwidth;
    height       = dheight;
    clipwidth    = dwidth;
    clipheight   = dheight;

    ddsd.dwSize  = sizeof (ddsd);
    ddsd.dwFlags = DDSD_PIXELFORMAT;
    TRY_DD(Back->GetDDSurface()->GetSurfaceDesc(&ddsd))

    if (Force15Bit)
        bitsperpixel = 15;
    else if (Force16Bit)
        bitsperpixel = 16;
    else if (ddsd.ddpfPixelFormat.dwRBitMask == 0xf800)
        bitsperpixel = 16;
    else 
        bitsperpixel = 15;

    // Only create a zbuffer if ZBufferBitDepth > 0
    if (ZBufferBitDepth) 
    {
        // Then, create Z-Buffer. The ZBufferMemType and ZBufferBitDepth variables
        // are set up when the Direct3D device enumeration is done at runtime
        memset(&ddsd, sizeof(DDSURFACEDESC), 0);
        ddsd.dwSize   = sizeof(DDSURFACEDESC);
        ddsd.dwFlags  = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_ZBUFFERBITDEPTH;
        ddsd.dwWidth  = dwidth;
        ddsd.dwHeight = dheight;

        // If a hardware device is present allocate zbuffer in VRAM else use
        // system ram.

        if (UsingHardware == TRUE)
            ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY | DDSCAPS_ZBUFFER;
        
        else
            ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY | DDSCAPS_ZBUFFER;

        ddsd.dwZBufferBitDepth  = ZBufferBitDepth;

        // Create the zbuffer
        TRY_DD(DirectDraw->CreateSurface(&ddsd, &zbuffer, NULL))
        
        // Attach ZBuffer to the back buffer
        TRY_DD(Back->GetDDSurface()->AddAttachedSurface(zbuffer))

        ZBuffer = new TDDSurface(zbuffer);
    }

    Setup3D(dwidth, dheight);

    if (UseClearZBuffer)
        InitClearZBuffer();

    Front->Clear();
    Back->Clear();

    return TRUE;
}

BOOL TDisplay::Close()
{
    if (!Front)
        return TRUE;

    if (Front && Front->GetDDSurface() == back)
    {
        PTDDSurface tmp = Front;
        Front = Back;
        Back = tmp;
    }
    
    CloseClearZBuffer();
    Close3D();

    if (Back && Back->GetDDSurface())
    {
        Back->GetDDSurface()->Release();

        delete Back;
        Back    = NULL;
        back    = NULL;
        surface = NULL;
    }

    // Destroy zbuffer surface
    if (ZBuffer && ZBuffer->GetDDSurface())
    {
        ZBuffer->GetDDSurface()->Release();

        delete ZBuffer;
        ZBuffer = NULL;
        zbuffer = NULL;
    }

    // Destroy front surface
    if (Front && Front->GetDDSurface())
    {
        Front->GetDDSurface()->Release();

        delete Front;
        Front = NULL;
        front = NULL;
    }

    CloseDirectDraw();

    width = height = 0;

    return TRUE;
}

TDisplay::~TDisplay()
{
    Close();
}

BOOL TDisplay::Restore()
{
    if (!Front)
        return FALSE;
    
    if (!Windowed)
    {
        SetExclusiveMode();
        EnterVideoMode(width, height, (bitsperpixel == 15) ? 16 : bitsperpixel);
    }

    if (front->IsLost() == DDERR_SURFACELOST)
    {
        TRY_DD(front->Restore());
        if (!Windowed)
            Front->Clear();
    }

    if (back->IsLost() == DDERR_SURFACELOST)
    {
        TRY_DD(front->Restore());
        Back->Clear();
    }


    return TRUE;
}

// Initializes a secondary zbuffer for use with the Viewport->Clear() function
void TDisplay::InitClearZBuffer()
{
    PTDDSurface s = new TDDSurface(width, height, VSURF_SYSTEMMEM);
    if (!s)
        FatalError("Unable to allocate clear ZBuffer");
    
    SaveZBuffer = ZBuffer;
    ZBuffer = s;
}

// Closes the secondary zbuffer
void TDisplay::CloseClearZBuffer()
{
    if (!SaveZBuffer)
        return;

    delete ZBuffer;
    ZBuffer = SaveZBuffer;
    SaveZBuffer = NULL;
}

// Flips the front/back buffer
BOOL TDisplay::FlipPage(BOOL Wait)
{
    if (!DoPageFlip)
        return TRUE;

    if (!Front || !Front->GetDDSurface() || (front->IsLost() == DDERR_SURFACELOST))
        return FALSE;

    if (Windowed)
    {
        RECT r;
        GetClientRect(MainWindow.Hwnd(), &r);
        ClientToScreen(MainWindow.Hwnd(), (LPPOINT)&r);
        Front->Blit(r.left - MonitorX, r.top - MonitorY, Back, 0, 0, min(r.right, WIDTH), min(r.bottom, HEIGHT));   
    }
    else if (SingleBuffer)
    {
        Front->Blit(0, 0, Back, 0, 0, WIDTH, HEIGHT);   
    }
    else 
    {
        while(front->GetFlipStatus(DDGFS_ISFLIPDONE) == DDERR_WASSTILLDRAWING)
            Sleep(1);

        TRY_DD(front->Flip(NULL, (Wait ? DDFLIP_WAIT : NULL)))
    }

    if (!SingleBuffer)              // do NOT switch when single buffered
        currentpage = !currentpage;

  // Show drawing (this shows drawing).. causes drawing to be shown >*
    if ((ShowDrawing == FALSE && Front->GetDDSurface() == back) ||
        (ShowDrawing == TRUE && Front->GetDDSurface() == front))
    {       
        PTDDSurface tmp = Front;
        Front = Back;
        Back  = tmp;
    }

    return TRUE;
}

// Copies contents of back buffer to front buffer or window so it is immediately displayed
BOOL TDisplay::PutToScreen(int x, int y, int width, int height)
{
    SClipState cs;
    SaveClipState(cs);
    Reset();

    if (Windowed)
    {
        RECT r;
        GetClientRect(MainWindow.Hwnd(), &r);
        ClientToScreen(MainWindow.Hwnd(), (LPPOINT)&r);
        Front->Blit(r.left - MonitorX + x, r.top - MonitorY + y, Back, x, y, width, height);    
    }
    else if (SingleBuffer)
    {
        Front->Blit(x, y, Back, x, y, width, height);   
    }
    else
    {
        Front->Blit(x, y, Back, x, y, width, height);
    }

    RestoreClipState(cs);

    return TRUE;
}

// **************************************** 
// * Call Dirty Rectangle Update Routines *
// ****************************************

// These functions are used to record a list of update ares on the screen.
// The low level graphics functions all are passed the address of a function called
// 'callback' in their drawparam structures.  For the display, this funciton is set
// to AddUpdateCallback(), which logs the rectangle drawn in a list.  When all the 
// drawing is complete, the low level draw functions return, and the list is added
// to the display dirty rectangle update list with a call to ProcessUpdateCallbacks().

  // Copies specified bitmap to current bitmap
BOOL TDisplay::ParamDraw(PSDrawParam dp, PTBitmap bitmap)
{
    if (front->IsLost() == DDERR_SURFACELOST)
        return FALSE;

  // Set update rectangles to 0 
    ResetUpdateCallbacks();

    BOOL ret = TDDSurface::ParamDraw(dp, bitmap);

  // Process update rectangles returned by draw routines
    ProcessUpdateCallbacks();

    return ret;
}

  // Blits from surface to this surface. RECT sets size of blit. 
  // X & Y specifies dest origin.  If no hardware available, uses software
  // blitting
BOOL TDisplay::ParamBlit(PSDrawParam dp, PTSurface surface, int flags, LPDDBLTFX fx)
{
    if (front->IsLost() == DDERR_SURFACELOST)
        return FALSE;

 // Set update rectangles to 0  
    ResetUpdateCallbacks();

    BOOL ret = TDDSurface::ParamBlit(dp, surface, flags, fx);
    
  // Process update rectangles returned by draw routines
    ProcessUpdateCallbacks();

    return ret;
}

  // Blits from this surface to surface. RECT sets size of blit. 
  // X & Y specifies dest origin.  If no hardware available, uses software
  // blitting.  Called by ParamBlit when blitting from a complex surface
  // to an ordinary surface.
BOOL TDisplay::ParamGetBlit(PSDrawParam dp, PTSurface surface, int flags, LPDDBLTFX fx)
{
    if (front->IsLost() == DDERR_SURFACELOST)
        return FALSE;

  // Set update rectangles to 0 
    ResetUpdateCallbacks();

    BOOL ret = TDDSurface::ParamGetBlit(dp, surface, flags, fx);
    
  // Process update rectangles returned by draw routines
    ProcessUpdateCallbacks();

    return ret;
}

// ******************************   
// * Background system routines *
// ******************************

typedef struct updaterect
{
    int x, y;
    int width, height;
    int flags;
} UpdateRect;

#define MAXRESTORERECTS 128

struct RestoreBuf
{
    int x;                      // Starting position of Background Buffer on screen.
    int y;                      
    int originx;                // Starting position of Background Buffer on restore 
    int originy;                // buffer.
    int oldoriginx;             // Old origins
    int oldoriginy;             // 
    int width;                  // Size of Background Buffer
    int height;

    PTSurface surface;          // Video surface for restore buffer

    BOOL        deletesurface;  // BOOL to determine if surfaces are deleted on exit

    UpdateRect  *rects[2];      // Update rect list for front and back surface
    int          numrects[2];   // Number of rects in the list

    int          start[2];      // Start of rect list
    int          emptystart[2]; // Next empty space in rect list
};

RestoreBuf RestoreBufs[MAXRESTOREBUFS];

BOOL TDisplay::InitBackgroundSystem()
{
    for (int loop = 0; loop < MAXRESTOREBUFS; loop++)
    {
        RestoreBufs[loop].x = 0;
        RestoreBufs[loop].y = 0;
        
        RestoreBufs[loop].originx = 0;
        RestoreBufs[loop].originy = 0;

        RestoreBufs[loop].oldoriginx = 0;
        RestoreBufs[loop].oldoriginy = 0;
        
        RestoreBufs[loop].width   = 0;
        RestoreBufs[loop].height  = 0;

        RestoreBufs[loop].surface       = NULL;

        RestoreBufs[loop].deletesurface = FALSE;

        RestoreBufs[loop].rects[0]      = NULL;
        RestoreBufs[loop].numrects[0]   = 0;

        RestoreBufs[loop].rects[1]      = NULL;
        RestoreBufs[loop].numrects[1]   = 0;

        RestoreBufs[loop].start[0]      = 0;
        RestoreBufs[loop].emptystart[0] = 0;
    
        RestoreBufs[loop].start[1]      = 0;
        RestoreBufs[loop].emptystart[1] = 0;
    }

    updateenabled = TRUE;

    return TRUE;
}

BOOL TDisplay::CloseBackgroundSystem()
{
    ClearBackgroundAreas();
    updateenabled = FALSE;
    
    return TRUE;
}

void TDisplay::FreeBackgroundArea(int index)
{
    if (RestoreBufs[index].width == 0) 
        return;

    if (RestoreBufs[index].surface && RestoreBufs[index].deletesurface)
          delete RestoreBufs[index].surface;

    delete(RestoreBufs[index].rects[0]);
    delete(RestoreBufs[index].rects[1]);

    memset(&RestoreBufs[index], 0, sizeof(RestoreBuf));
}

BOOL TDisplay::ClearBackgroundAreas()
{
    for (int loop = 0; loop < MAXRESTOREBUFS; loop++)
        FreeBackgroundArea(loop);

    return TRUE;
}

int TDisplay::CreateBackgroundArea(int x, int y, int width, int height, BOOL createzbuf, int vsflags)
{
    PTSurface surface;
    if (createzbuf)
    {
        PTSurface zbuf = new TDDSurface(width, height, vsflags);
        if (!zbuf)
            return NULL;
        PTSurface graphics = new TDDSurface(width, height, vsflags);
        if (!graphics)
            return NULL;
        surface = new TMultiSurface(graphics, zbuf, NULL, TRUE);
    }
    else
    {
        surface = new TDDSurface(width, height, vsflags);
    }

    int buf = UseBackgroundArea(x, y, width, height, surface);
    
    RestoreBufs[buf].deletesurface = TRUE;

    return buf;
}

int TDisplay::UseBackgroundArea(int x, int y, int width, int height, PTSurface surface)
{
    if (surface == NULL) return FALSE;

    for (int loop = 0; loop < MAXRESTOREBUFS; loop++)
    {
        if (RestoreBufs[loop].width == 0) 
            break;
    }

    if (loop == MAXRESTOREBUFS)
        return -1;

    RestoreBufs[loop].originx = 0;
    RestoreBufs[loop].originy = 0;

    RestoreBufs[loop].oldoriginx = 0;
    RestoreBufs[loop].oldoriginy = 0;

    RestoreBufs[loop].x = x;
    RestoreBufs[loop].y = y;

    RestoreBufs[loop].width  = width;
    RestoreBufs[loop].height = height;

    RestoreBufs[loop].surface       = surface;
    RestoreBufs[loop].deletesurface = FALSE;

    RestoreBufs[loop].rects[0]   = new UpdateRect[MAXRESTORERECTS];
    RestoreBufs[loop].rects[1]   = new UpdateRect[MAXRESTORERECTS];

    RestoreBufs[loop].start[0]      = 0;
    RestoreBufs[loop].emptystart[0] = 0;
    
    RestoreBufs[loop].start[1]      = 0;
    RestoreBufs[loop].emptystart[1] = 0;
    
    return loop;
}

BOOL TDisplay::RestoreBackgroundAreas()
{
    for (int loop = 0; loop < MAXRESTOREBUFS; loop++)
    {
        if (RestoreBufs[loop].width == 0) 
            break;

        RestoreBuf *rbuf = &RestoreBufs[loop];

        for (int loop2 = 0; loop2 < rbuf->numrects[currentpage]; loop2++)
        {
            UpdateRect *ur = &rbuf->rects[currentpage][loop2]; 

            int drawmode = DM_WRAPCLIPSRC | DM_NORESTORE;
            if (!NoScrollZBuffer)
                drawmode |= DM_ZBUFFER;

            DrawRestoreRect(loop, ur->x, ur->y, ur->width, ur->height, drawmode);
        }

        rbuf->numrects[currentpage]   = 0;
        rbuf->start[currentpage]      = 0;
        rbuf->emptystart[currentpage] = 0;
    }

    return TRUE;
}

BOOL TDisplay::DrawRestoreRect(int index, int x, int y, int width, int height, DWORD drawmode)
{
    int saveoriginx, saveoriginy;
    int saveclipx, saveclipy, saveclipwidth, saveclipheight;
    int saveclipmode;

    GetOrigin(saveoriginx, saveoriginy);
    GetClipRect(saveclipx, saveclipy, saveclipwidth, saveclipheight);
    GetClipMode(saveclipmode);

    RestoreBuf *rbuf = &RestoreBufs[index];

    Reset();
    SetOrigin(0, 0);
    SetClipRect(rbuf->x, rbuf->y, rbuf->width, rbuf->height);

    Blit(x - rbuf->originx + rbuf->x,
         y - rbuf->originy + rbuf->y,
         rbuf->surface,
         x % rbuf->surface->Width(), y % rbuf->surface->Height(), 
         width, height, drawmode | DM_WRAPCLIPSRC | DM_NORESTORE);

    SetOrigin(saveoriginx, saveoriginy);
    SetClipRect(saveclipx, saveclipy, saveclipwidth, saveclipheight);
    SetClipMode(saveclipmode);

    return TRUE;
}

BOOL TDisplay::ScrollBackground(int index, int originx, int originy)
{
    RestoreBuf &rb = RestoreBufs[index];

    if (index > MAXRESTOREBUFS) 
        return FALSE;

    rb.oldoriginx = rb.originx; 
    rb.oldoriginy = rb.originy; 

    if (rb.originx == originx && 
        rb.originy == originy)  
        return TRUE;

    rb.originx = originx;   
    rb.originy = originy;   

    rb.rects[0]->x      = rb.originx;
    rb.rects[0]->y      = rb.originy;
    rb.rects[0]->width  = rb.surface->Width();
    rb.rects[0]->height = rb.surface->Height();
    rb.numrects[0]      = 1;
    rb.start[0]         = 0;
    rb.emptystart[0]    = 1;

    rb.rects[1]->x      = rb.originx;
    rb.rects[1]->y      = rb.originy;
    rb.rects[1]->width  = rb.surface->Width();
    rb.rects[1]->height = rb.surface->Height();
    rb.numrects[1]      = 1;
    rb.start[1]         = 0;
    rb.emptystart[1]    = 1;

    return TRUE;
}

// **************************** 
// * Dirty rectangle routines *
// ****************************

#define MERGELIMITX    10
#define MERGELIMITY    10
#define MARKMERGED(r)  r.x = -10000
#define RECTMERGED(r) (r.x == -10000)

// ******* Display Update Functions *******

// Adds update rectangle for display (display coordinates relative to 0,0 of screen)
void TDisplay::AddUpdateRect(int x, int y, int width, int height, int flags)
{
    RestoreBuf *rb = RestoreBufs;

    for (int loop = 0; loop < MAXRESTOREBUFS; loop++, rb++)
    {
        if (rb->width == 0)
            continue;
      // Clip to screen rect    
        if (x >= rb->x + rb->width || y >= rb->y + rb->height ||
            x + width <= rb->x || y + height <= rb->y)
                continue;

        int cx = x;
        int cy = y;
        int cw = width;
        int ch = height;

        if (cx < rb->x)
        {
            cw += cx - rb->x;
            cx = rb->x;
        }

        if (cy < rb->y)
        {
            ch += cy - rb->y;
            cy = rb->y;
        }

        if (cx + cw > rb->x + rb->width)
           cw = (rb->x + rb->width) - cx;
        if (cy + ch > rb->y + rb->height)
           ch = (rb->y + rb->height) - cy;

        Display->AddBackgroundUpdateRect(loop, 
            cx - rb->x + rb->originx,   // Convert from screen to buffer pos X
            cy - rb->y + rb->originy,   // Convert from screen to buffer pos Y
            cw, ch, flags);
    }
}

// These functions are used to record a list of update ares on the screen.
// The low level graphics functions all are passed the address of a function called
// 'callback' in their drawparam structures.  For the display, this funciton is set
// to AddUpdateCallback(), which logs the rectangle drawn in a list.  When all the 
// drawing is complete, the low level draw functions return, and the list is added
// to the display dirty rectangle update list with a call to ProcessUpdateCallbacks().

struct SUpdateCallbackRec
{
    int x, y, w, h, flags;
};

// 64 should be more than adequate
#define MAXCALLBACKRECS 64
static int numcallbacks;
static SUpdateCallbackRec callbacks[MAXCALLBACKRECS];

void ResetUpdateCallbacks()
{
    numcallbacks = 0;
}

void ProcessUpdateCallbacks()
{
    SUpdateCallbackRec *r = callbacks;
    for (int c = 0; c < numcallbacks; c++, r++)
        Display->AddUpdateRect(r->x, r->y, r->w, r->h, r->flags);
    numcallbacks = 0;
}

void AddUpdateCallback(PSDrawBlock db, PSDrawParam dp)
{
    if (dp->drawmode & DM_NORESTORE)
        return;

    if (numcallbacks >= MAXCALLBACKRECS)
    {
#ifdef _DEBUG
        FatalError("Too many callback recs"); // Should never happen
#endif
        return;
    }

    SUpdateCallbackRec *r = &(callbacks[numcallbacks]);

    r->x = dp->dx + dp->originx;
    r->y = dp->dy + dp->originy;
    r->w = dp->dwidth;
    r->h = dp->dheight;
    r->flags = (dp->drawmode & DM_BACKGROUND) ? (UPDATE_BACKGROUND) : (UPDATE_RESTORE);

    numcallbacks++;
}

// Returns the AddUpdateRect() function as a draw callback to the Draw() routine
DRAWCALLBACK TDisplay::GetDrawCallBack()
{
    if (updateenabled)
        return (DRAWCALLBACK)AddUpdateCallback;

    return NULL;
}

// ******* Buffer Update Functions *******

inline void TDisplay::AddSubRect(int index, int x1, int y1, int x2, int y2, int flags)
{
    AddBackgroundUpdateRect(index, x1, y1, x2 - x1 + 1, y2 - y1 + 1, flags);
}

// Adds update rectangle (uses buffer coordinates based on origin of buffer)
BOOL TDisplay::AddBackgroundUpdateRect(int index, int x, int y, int width, int height, int flags)
{
    RestoreBuf &rb = RestoreBufs[index];
    
    int cx = rb.originx;
    int cy = rb.originy;
    int cw = rb.surface->Width();
    int ch = rb.surface->Height();

  // Clip x and y
    if ((x >= cx + cw) || (x + width <= cx) ||
        (y >= cy + ch) || (y + height <= cy))
       return FALSE;

  // Prevent two threads at once from entering the update rectangle area    
    BEGIN_CRITICAL();  // REMEMBER: MUST NOT RETURN BETWEEN HERE AND
                       // END CRITICAL SECTION BELOW OR PROGRAM WILL
                       // LOCK UP!! !!!!! !!!!! ((!!!!) ****!!!!***

    if (x < cx)
    {
        width -= cx - x;
        x = cx;
    }

    if (y < cy)
    {
        height -= cy - y;
        y = cy;
    }

    if ((x + width) > (cx + cw))
        width = (cx + cw) - x;  
    
    if ((y + height) > (cy + ch))
        height = (cy + ch) - y; 

  // Update whatever we drew to the screen to the background..
    if (flags & UPDATE_SCREENTOBUFFER)
    {
        SRect saveclip;
        rb.surface->GetClipRect(saveclip);
        rb.surface->SetClipRect(
                          rb.originx, rb.originy,
                          rb.width, rb.height);
        rb.surface->Blit(
            x, y,
            Back,
            rb.x + (x - rb.originx), rb.y + (y - rb.originy),
            width, height, DM_WRAPCLIP | DM_NORESTORE);

        rb.surface->SetClipRect(saveclip);
    }
    flags &= ~UPDATE_SCREENTOBUFFER;  // Done.. now clear flag

  // Don't bother copying while scrolling - whole thing will update anyway
    if (flags & UPDATE_BUFFERTOSCREEN &&
        rb.originx == rb.oldoriginx && rb.originy == rb.oldoriginy)
    {
        SRect saveclip;
        Back->GetClipRect(saveclip);
        Back->SetClipRect(rb.x, rb.y, rb.width, rb.height);
        Back->Blit(
            rb.x + (x - rb.originx), rb.y + (y - rb.originy),
            rb.surface,
            x, y,
            width, height, DM_WRAPCLIPSRC | DM_NORESTORE);
        Back->SetClipRect(saveclip);
    }
    flags &= ~UPDATE_BUFFERTOSCREEN;  // Done.. now clear flag

  // Ok, now handle just the UPDATE_THISFRAME, UPDATE_NEXTFRAME flags by adding
  // dirty rectangles to the update rectangle list for each display page...
    for (int p = 0; p < 2; p++)
    {                              
        int f, page;

      // What page are we adding update rectangle rectange to...
        if (p == 0)
        {
            page = currentpage;
            if (!(flags & UPDATE_THISFRAME))
                continue;
    
            else
                f = UPDATE_THISFRAME;
        }
    
        else
        {
            page = !currentpage;
            if (!(flags & UPDATE_NEXTFRAME))
                continue;
    
            else
                f = UPDATE_NEXTFRAME;
        }

        BOOL merged = FALSE;

      // Hey.. there's too many rectangles for this page!!
      // Just make one rectangle for the whole buffer and forget it...
        if (rb.numrects[page] >= (MAXRESTORERECTS - 4)) // Leave at least four rects
        {
            // out of restore rects - nuke them all and just make one for the entire restore buf
            rb.numrects[page] = 0;
            rb.start[page] = 0;
            rb.emptystart[page] = 0;
            x = rb.originx;
            y = rb.originy;
            width = rb.surface->Width();
            height = rb.surface->Height();
        }

      // Otherwise, let's try to merge any overlapping rectangles (to save time)
        else if (!(flags & UPDATE_NOMERGERECT))
        {
            int sx1 = x;
            int sx2 = x + width - 1;
            int sy1 = y;
            int sy2 = y + height - 1;

            int c = rb.start[page];
            while (c != rb.emptystart[page])
            {
              UpdateRect &r = rb.rects[page][c];

              if (!(r.flags & UPDATE_NOMERGERECT))
              {
                int dx1 = r.x;
                int dx2 = r.x + r.width - 1;
                int dy1 = r.y;
                int dy2 = r.y + r.height - 1;

              // Make merge tolerance smaller for larger block sizes
                int minmergex = 16 - (min(256, r.height) >> 4);
                int minmergey = 16 - (min(256, r.width) >> 4);

              // Check if intersects at all
                if (!(sx1 > dx2 || sy1 > dy2 || sx2 < dx1 || sy2 < dy1))
                {
                    int in = 0;
                    if (sx1 >= dx1 - minmergex) // Left line in
                    {
                        dx1 = min(dx1, sx1);
                        in |= 1;
                    }
                    if (sx2 <= dx2 + minmergex) // Right line in
                    {
                        dx2 = max(dx2, sx2);
                        in |= 2;
                    }
                    if (sy1 >= dy1 - minmergey) // Top line in
                    {
                        dy1 = min(dy1, sy1);
                        in |= 4;
                    }
                    if (sy2 <= dy2 + minmergey) // Bottom line in
                    {
                        dy2 = max(dy2, sy2);
                        in |= 8;
                    }

                    r.x = dx1;
                    r.y = dy1;
                    r.width = dx2 - dx1 + 1;
                    r.height = dy2 - dy1 + 1;

                    switch (in)
                    {
                      case 0: // Source encompasses dest
                        r.x = sx1;
                        r.y = sy1;
                        r.width = sx2 - sx1 + 1;
                        r.height = sy2 - sy1 + 1;
                        break;
                      case 1: // Left line in
                        AddSubRect(index, sx1, sy1, sx2, dy1 - 1, f);
                        AddSubRect(index, dx2 + 1, dy1, sx2, dy2, f);
                        AddSubRect(index, sx1, dy2 + 1, sx2, sy2, f);
                        break;
                      case 2: // Right line in
                        AddSubRect(index, sx1, sy1, sx2, dy1 - 1, f);
                        AddSubRect(index, sx1, dy1, dx1 - 1, dy2, f);
                        AddSubRect(index, sx1, dy2 + 1, sx2, sy2, f);
                        break;
                      case 3: // Left and right line in
                        AddSubRect(index, sx1, sy1, sx2, dy1 - 1, f);
                        AddSubRect(index, sx1, dy2 + 1, sx2, sy2, f);
                        break;
                      case 4: // Top line in
                        AddSubRect(index, sx1, sy1, dx1 - 1, dy2, f);
                        AddSubRect(index, dx2 + 1, sy1, sx2, dy2, f);
                        AddSubRect(index, sx1, dy2 + 1, sx2, sy2, f);
                        break;
                      case 5: // Top and left line in
                        AddSubRect(index, dx2 + 1, sy1, sx2, dy2, f);
                        AddSubRect(index, sx1, dy2 + 1, sx2, sy2, f);
                        break;
                      case 6: // Top and right line in
                        AddSubRect(index, sx1, sy1, dx1 - 1, dy2, f);
                        AddSubRect(index, sx1, dy2 + 1, sx2, sy2, f);
                        break;
                      case 7: // Top, left, and right line in
                        AddSubRect(index, sx1, dy2 + 1, sx2, sy2, f);
                        break;
                      case 8: // Bottom line in
                        AddSubRect(index, sx1, sy1, sx2, dy1 - 1, f);
                        AddSubRect(index, sx1, dy1, dx1 - 1, sy2, f);
                        AddSubRect(index, dx2 + 1, dy1, sx2, sy2, f);
                        break;
                      case 9: // Bottom and left in
                        AddSubRect(index, sx1, sy1, sx2, dy1 - 1, f);
                        AddSubRect(index, dx2 + 1, dy1, sx2, sy2, f);
                        break;
                      case 10: // Bottom and right in
                        AddSubRect(index, sx1, sy1, sx2, dy1 - 1, f);
                        AddSubRect(index, sx1, dy1, dx1 - 1, sy2, f);
                        break;
                      case 11: // Bottom, left and right in
                        AddSubRect(index, sx1, sy1, sx2, dy1 - 1, f);
                        break;
                      case 12: // Top, bottom in
                        AddSubRect(index, sx1, sy1, dx1 - 1, sy2, f);
                        AddSubRect(index, dx2 + 1, sy1, sx2, sy2, f);
                        break;
                      case 13: // Top, bottom and left in
                        AddSubRect(index, dx2 + 1, sy1, sx2, sy2, f);
                        break;
                      case 14: // Top, bottom and right in
                        AddSubRect(index, sx1, sy1, dx1 - 1, sy2, f);
                        break;
                      case 15: // All in!!
                        break;
                    }

                    merged = TRUE;
                    break;
                }
              }
                
              c++;
              if (c >= MAXRESTORERECTS)
                  c = 0;
            }
        }

        if (merged)
            continue;
        
      // Add to array
        UpdateRect &r = rb.rects[page][rb.emptystart[page]];

        if (width > 0)
        {
            r.x        = x;
            r.y        = y;
            r.width    = width;
            r.height   = height;
            r.flags    = flags;
        }

        rb.numrects[page]++;
        rb.emptystart[page]++;

        if (rb.emptystart[page] >= MAXRESTORERECTS)
            rb.emptystart[page] = 0;
    }

   // If you've returned before this... you've screwed up the system!!  
    END_CRITICAL();

    return TRUE;
}

#if 0

// Stuck this code into surface.cpp's ParamDraw function

// *******************************************
// * Workaround function for crazy Monster3D *
// *******************************************

BOOL TDisplay::ZPut(int x, int y, int z, PTBitmap bitmap, DWORD drawmode)
{
    if (!bitmap)
        return FALSE;

    static int status = 0;
    static PTSurface multi = NULL;

    if (status == 0)
    {
        // this is the first call, so determine what the status is
        void *zbuf = GetZBuffer()->Lock();
        GetZBuffer()->Unlock();

        void *graphics = Lock();
        Unlock();

        if (!graphics || !zbuf)
            return FALSE;

        if (graphics == zbuf)
        {
            status = 1;         // dang, gotta use the workaround
#if 0
            PTDDSurface zbuffer = new TDDSurface(WIDTH, HEIGHT, VSURF_SYSTEMMEM);
            multi = new TMultiSurface(Back, zbuffer, NULL, TRUE);
#endif
        }
        else
            status = 2;         // good, it's a normal card
    }

    if (status == 1)
    {
        return TDDSurface::Put(x, y, bitmap, drawmode & ~DM_ZBUFFER);
#if 0
        int saveoriginx, saveoriginy;
        int saveclipx, saveclipy, saveclipwidth, saveclipheight;
        int saveclipmode;

        GetOrigin(saveoriginx, saveoriginy);
        GetClipRect(saveclipx, saveclipy, saveclipwidth, saveclipheight);
        GetClipMode(saveclipmode);

        RestoreBuf *rbuf = &RestoreBufs[4];

        Reset();
        SetOrigin(0, 0);
        SetClipRect(rbuf->x, rbuf->y, rbuf->width, rbuf->height);

        multi->Reset();

        multi->Blit(x - rbuf->originx + rbuf->x,
                    y - rbuf->originy + rbuf->y,
                    rbuf->surface,
                    x % rbuf->surface->Width(), y % rbuf->surface->Height(), 
                    width, height,
                    DM_WRAPCLIPSRC | DM_NORESTORE | DM_NODRAW | DM_ZBUFFER | DM_NOHARDWARE);

        SetOrigin(saveoriginx, saveoriginy);
        SetClipRect(saveclipx, saveclipy, saveclipwidth, saveclipheight);
        SetClipMode(saveclipmode);

        PTSurface tmp = (PTSurface)ZBuffer;
        ZBuffer = (PTDDSurface)multi->GetZBuffer();

        ZPut(x, y, z, bitmap, drawmode);

        ZBuffer = (PTDDSurface)tmp;

        Reset();
        SetOrigin(0, 0);
        SetClipRect(rbuf->x, rbuf->y, rbuf->width, rbuf->height);

        rbuf->surface->Blit(x - rbuf->originx + rbuf->x,
                            y - rbuf->originy + rbuf->y,
                            multi,
                            x - rbuf->originx + rbuf->x, y - rbuf->originy + rbuf->y,
                            width, height,
                            DM_NORESTORE | DM_NODRAW | DM_ZBUFFER);

        SetOrigin(saveoriginx, saveoriginy);
        SetClipRect(saveclipx, saveclipy, saveclipwidth, saveclipheight);
        SetClipMode(saveclipmode);
#endif
#if 0
        // special case - monster3d which can't lock zbuffer and graphics at the same time
        //PTDDSurface graphics = new TDDSurface(bitmap->width, bitmap->height, VSURF_SYSTEMMEM);
        //PTDDSurface zbuffer = new TDDSurface(bitmap->width, bitmap->height, VSURF_SYSTEMMEM);
        //PTMultiSurface multi = new TMultiSurface(Back, zbuffer, NULL, TRUE);
        //multi->SetClipRect(0, 0, bitmap->width, bitmap->height);

        if (drawmode & DM_USEREG)
        {
            // handle registration points ourselves
            x -= bitmap->regx;
            y -= bitmap->regy;
            drawmode &= ~DM_USEREG;
        }

        // first copy from the card
        //graphics->Blit(0, 0, Back, x, y, bitmap->width, bitmap->height, 0);

        // then the zbuffer, manually
        SDrawParam dp;
        MakeDP(dp, 0, 0, x, y, bitmap->width, bitmap->height, 0);
        dp.drawmode = DM_WRAPCLIPSRC;
        //BackgroundZBuffer->GetOrigin(dp.originx, dp.originy);
        BackgroundZBuffer->GetClipRect(dp.clipx, dp.clipy, dp.clipwidth, dp.clipheight);

        SDrawBlock db;
        memset(&db, 0, sizeof(SDrawBlock));

        db.dest = zbuffer->Lock();
        zbuffer->Unlock();

        db.dstbitmapflags = BM_16BIT;
        db.dbufwidth      = bitmap->width;
        db.dbufheight     = bitmap->height;
        db.dstride        = bitmap->width;

        db.source = BackgroundZBuffer->Lock();
        BackgroundZBuffer->Unlock();

        db.srcbitmapflags = BM_16BIT;
        db.sbufwidth  = BackgroundZBuffer->Width();
        db.sbufheight = BackgroundZBuffer->Height();
        db.sstride    = BackgroundZBuffer->Stride();

        Draw(&db, &dp);

        //zbuffer->Put(0, 0, BackgroundZBuffer, x, y, bitmap->width, bitmap->height, 0);

        // now do the blit to our temporary surface
        multi->ZPut(0, 0, z, bitmap, drawmode);

        // now copy that multisurface back to the display, one surface at a time
        drawmode &= ~(DM_ZBUFFER | DM_ZSTATIC);
        Blit(x, y, graphics, 0, 0, bitmap->width, bitmap->height, drawmode);
        GetZBuffer()->Blit(x, y, zbuffer, 0, 0, bitmap->width, bitmap->height, drawmode);

        // kill the temporary buffer
        delete multi;
#endif
//      return TRUE;
    }
    
    // otherwise, just do it like normal
    return TDDSurface::ZPut(x, y, z, bitmap, drawmode);
}

#endif
