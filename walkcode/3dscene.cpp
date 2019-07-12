// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   3dScene.cpp - 3D Scene module                       *
// *************************************************************************

#include <windows.h>
#include <ddraw.h>
#include <d3drmwin.h>
#include <math.h>

#include "d3dmacs.h"
#include "3dimage.h"
#include "3dscene.h"
#include "d3dmath.h"

#include "revenant.h"
#include "directdraw.h"
#include "display.h"
#include "mainwnd.h"
#include "resource.h"
#include "mappane.h"

// 3D Render State Flags
BOOL FlatShade = FALSE;
BOOL SimpleLight = FALSE;
BOOL UseTextures = TRUE;
BOOL DitherEnable = TRUE;
BOOL BlendEnable = TRUE;
BOOL SpecularEnable = TRUE;
BOOL ZEnable = TRUE; 
BOOL BilinearFilter = FALSE;
BOOL NoUpdateRects = FALSE;
BOOL MaxLights = 1;
BOOL Double3D, Triple3D;		// Doubles or tripples 3D objects

// quick hacks
// rotate x coords
float RotateX(float h, float k, float x, float y, float angle)
{ return (float)(h + (x - h) * cos(angle) - (y - k) * sin(angle)); }
// rotate y coords
float RotateY(float h, float k, float x, float y, float angle)
{ return (float)(k + (y - k) * cos(angle) + (x - h) * sin(angle)); }
// add z rotation here

// DLS brightness routine (gives brightness given distance)
extern double GetLightBrightness(int dist, int intensity, int multiplier);

// ************************************************************
// * T3DLight - Information for an light in a 3D scene object *
// ************************************************************

_CLASSDEF(T3DLight)
class T3DLight
{
  public:
	LPDIRECT3DLIGHT light;
	D3DLIGHT lightdata;
	BOOL lighton;
	int intensity;
	int multiplier;

	T3DLight(D3DLIGHT &newlight, int newintensity, int newmultiplier);
	~T3DLight();

	BOOL Initialize();
	BOOL Close();

	void SetIntensity(int intensity);
	void SetMultiplier(int multiplier);
	void SetColor(D3DVALUE red, D3DVALUE green, D3DVALUE blue);
	void SetPosition(D3DVALUE x, D3DVALUE y, D3DVALUE z);
	D3DVALUE DistanceToObject(D3DVALUE x, D3DVALUE y, D3DVALUE z);
	BOOL AffectObject(D3DVALUE x, D3DVALUE y, D3DVALUE z);
	BOOL LightOff();
};

// ***************************************
// * T3DScene - 3DScene global variables *
// ***************************************

static SColor Ambient;						// Ambient Light Value
static S3DPoint CameraPos;					// Camera source pos

static int scenex, sceney, scenewidth, sceneheight; // Scene screen position

static int dirlight;

#define MAX3DANIMATORS 128
typedef TPointerArray<T3DAnimator, MAX3DANIMATORS> T3DAnimatorArray;
static T3DAnimatorArray AnimatorArray;

#define MAX3DLIGHTS 64
typedef TPointerArray<T3DLight, MAX3DLIGHTS> T3DLightArray;
static T3DLightArray LightArray;

static D3DMATRIX proj = {
    D3DVAL(1.0/65536.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0/65536.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0/65536.0/ZSCALE), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};

static D3DMATRIX view = {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(200.0), D3DVAL(1.0)
};

static D3DMATRIX world;

D3DMATRIX identity = {
    D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0), D3DVAL(0.0),
    D3DVAL(0.0), D3DVAL(0.0), D3DVAL(0.0), D3DVAL(1.0)
};

D3DMATRIXHANDLE hProj, hView, hWorld;

static LPDIRECT3DLIGHT lpD3DLight;
static D3DEXECUTEDATA d3dExData;
static D3DEXECUTEBUFFERDESC debDesc;
static LPDIRECT3DMATERIAL lpBmat;
static int AmbientValue;
static SColor AmbientColor256;
static D3DCOLOR AmbientColor;

// Variables used by texture format info thingy
#define MAXTEXFORMATS 64
static int numtexformats;
static DDSURFACEDESC texformats[MAXTEXFORMATS];

// **********************
// * T3DLight Functions *
// **********************

T3DLight::T3DLight(D3DLIGHT &newlight, int newintensity, int newmultiplier)
{
	memcpy(&lightdata, &newlight, sizeof(D3DLIGHT));
	intensity = newintensity;
	multiplier = newmultiplier;
	
	Initialize();
}

T3DLight::~T3DLight()
{
	Close();
}

BOOL T3DLight::Initialize()
{
    TRY_D3D(Direct3D->CreateLight(&light, NULL));
    TRY_D3D(light->SetLight(&lightdata));
	lighton = FALSE;

	D3DVALUE lm = (D3DVALUE)1.0 / max(lightdata.dcvColor.r, 
		max(lightdata.dcvColor.g, lightdata.dcvColor.b));
	lightdata.dcvColor.r = lightdata.dcvColor.r * lm;
	lightdata.dcvColor.g = lightdata.dcvColor.g * lm;
	lightdata.dcvColor.b = lightdata.dcvColor.b * lm;

	return TRUE;
}

BOOL T3DLight::Close()
{
	LightOff();
	RELEASE(light);

	return TRUE;
}		

void T3DLight::SetIntensity(int newintensity)
{
	intensity = newintensity;
}

void T3DLight::SetMultiplier(int newmultiplier)
{
	multiplier = newmultiplier;
}

void T3DLight::SetColor(D3DVALUE red, D3DVALUE green, D3DVALUE blue)
{
    lightdata.dcvColor.r = red;
    lightdata.dcvColor.g = green;
    lightdata.dcvColor.b = blue;
    lightdata.dcvColor.a = D3DVAL(1.0);

	D3DVALUE lm = (D3DVALUE)1.0 / max(lightdata.dcvColor.r, 
		max(lightdata.dcvColor.g, lightdata.dcvColor.b));
	lightdata.dcvColor.r = lightdata.dcvColor.r * lm;
	lightdata.dcvColor.g = lightdata.dcvColor.g * lm;
	lightdata.dcvColor.b = lightdata.dcvColor.b * lm;

    light->SetLight(&lightdata);
}

void T3DLight::SetPosition(D3DVALUE x, D3DVALUE y, D3DVALUE z)
{
	lightdata.dvPosition.x = x;
	lightdata.dvPosition.y = y;
	lightdata.dvPosition.z = z;

    light->SetLight(&lightdata);
}

D3DVALUE T3DLight::DistanceToObject(D3DVALUE x, D3DVALUE y, D3DVALUE z)
{
	x -= lightdata.dvPosition.x;
	y -= lightdata.dvPosition.y;
	z -= lightdata.dvPosition.z;

	D3DVALUE d = (float)sqrt((double)(x * x + y * y));
	return (float)sqrt((double)(d * d + z * z));
}

BOOL T3DLight::AffectObject(D3DVALUE x, D3DVALUE y, D3DVALUE z)
{
	x -= lightdata.dvPosition.x;
	y -= lightdata.dvPosition.y;
	z -= lightdata.dvPosition.z;

	D3DVALUE d = (float)sqrt((double)(x * x + y * y));
	d = (float)sqrt((double)(d * d + z * z));

	D3DVALUE r, g, b;
	if (d < intensity)
	{
		D3DVALUE i = min((D3DVALUE)GetLightBrightness((int)d, intensity, multiplier), (D3DVALUE)1.0);

		r = lightdata.dcvColor.r;
		lightdata.dcvColor.r = lightdata.dcvColor.r * i;
		g = lightdata.dcvColor.g;
		lightdata.dcvColor.g = lightdata.dcvColor.g * i;
		b = lightdata.dcvColor.b;
		lightdata.dcvColor.b = lightdata.dcvColor.b * i;
	    lightdata.dcvColor.a = D3DVAL(1.0);

		light->SetLight(&lightdata);

		lightdata.dcvColor.r = r;
		lightdata.dcvColor.g = g;
		lightdata.dcvColor.b = b;
		
		if (!lighton)
		{
			TRY_D3D(Viewport->AddLight(light));
			lighton = TRUE;
		}
		
		return TRUE;
	}
	return FALSE;
}

BOOL T3DLight::LightOff()
{
	if (!lighton)
		return TRUE;

	TRY_D3D(Viewport->DeleteLight(light));
	
	lighton = FALSE;

	return TRUE;
}

// *********************************
// * 3DScene - Main 3D interface!! *
// *********************************

// Constructor. Marks all frames and light free
T3DScene::T3DScene()
{
	initialized = FALSE;
}

T3DScene::~T3DScene()
{
	Close();
}

// Initialize 3D scene
BOOL T3DScene::Initialize()
{
	if (initialized || Ignore3D)
		return TRUE;

	initialized = TRUE;

	// 3D units per pixel is equal to the total screen pixels for the hypotenuse of a tile
    // from the left screen corner to the right screen corner (512.0) divided by the actual
	// 3D width of the hypotenuse in 3D units.
	float unitsperpixel = (float)512.0 / (float)sqrt(256.0 * 256.0 + 256.0 * 256.0);

	// Create the scene (parent) frame
	if (UseDirect3D2)
	{
	    TRY_D3D(Direct3D2->CreateViewport(&Viewport2, NULL));
		TRY_D3D(Viewport2->QueryInterface(IID_IDirect3DViewport, (LPVOID *)&Viewport));
		TRY_D3D(Device2->AddViewport(Viewport2));
		TRY_D3D(Device2->SetCurrentViewport(Viewport2));
	}
	else
	{
	    TRY_D3D(Direct3D->CreateViewport(&Viewport, NULL));
		TRY_D3D(Device->AddViewport(Viewport));
	}

	SetSize(0, 0, Display->Width(), Display->Height());

	TExecuteBuf::Initialize();  // Initializes execute buffer stuff

	CreateMatrixList();			// Creates the buffer system matrix list

	D3DMATERIAL bmat;
    memset(&bmat, 0, sizeof(D3DMATERIAL));
    bmat.dwSize = sizeof(D3DMATERIAL);
    bmat.diffuse.r = (D3DVALUE)0.0;
    bmat.diffuse.g = (D3DVALUE)0.0;
    bmat.diffuse.b = (D3DVALUE)0.0;
    bmat.ambient.r = (D3DVALUE)0.0;
    bmat.ambient.g = (D3DVALUE)0.0;
    bmat.ambient.b = (D3DVALUE)0.0;
    bmat.hTexture = NULL;
    bmat.dwRampSize = 1;

	D3DMATERIALHANDLE hBmat;
    TRY_D3D(Direct3D->CreateMaterial(&lpBmat, NULL));
    TRY_D3D(lpBmat->SetMaterial(&bmat));
    TRY_D3D(lpBmat->GetHandle(Device, &hBmat));
    TRY_D3D(Viewport->SetBackground(hBmat));

	if (Display->UsingClearZBuffer())
	{
		TRY_D3D(Viewport->SetBackgroundDepth(Display->GetZBuffer()->GetDDSurface()));
	}

    /*
     * Setup view, projection, world, and pos matrices
     */
	InitializeMatrices();

    /*
     * Setup ambient directional light
     */
    D3DLIGHT lightdata;
    memset(&lightdata, 0, sizeof(D3DLIGHT));
    lightdata.dwSize = sizeof(D3DLIGHT);
    lightdata.dltType = D3DLIGHT_DIRECTIONAL;
    lightdata.dcvColor.r = D3DVAL(1.0);
    lightdata.dcvColor.g = D3DVAL(1.0);
    lightdata.dcvColor.b = D3DVAL(1.0);
    lightdata.dcvColor.a = D3DVAL(1.0);
	lightdata.dvPosition.x = D3DVAL(0.0);
	lightdata.dvPosition.y = D3DVAL(0.0);
	lightdata.dvPosition.z = D3DVAL(200.0);
	lightdata.dvDirection.x = D3DVAL(1.0);
	lightdata.dvDirection.y = D3DVAL(-1.0);
	lightdata.dvDirection.z = D3DVAL(-1.0);

//	dirlight = LightArray.Add(new T3DLight(lightdata));

	/*
	 * Get the texture formats
	 */

	GetTextureFormats();

	return TRUE;
}

BOOL T3DScene::SetSize(int x, int y, int width, int height)
{
	if (!initialized || Ignore3D)
		return TRUE;

    scenex      = x;
	sceney      = y;
	scenewidth  = width;
	sceneheight = height;

	// 3D units per pixel is equal to the total screen pixels for the hypotenuse of a tile
    // from the left screen corner to the right screen corner (512.0) divided by the actual
	// 3D width of the hypotenuse in 3D units.
	float unitsperpixel = (float)512.0 / (float)sqrt(256.0 * 256.0 + 256.0 * 256.0);

    D3DVIEWPORT viewData;
    memset(&viewData, 0, sizeof(D3DVIEWPORT));
    viewData.dwSize = sizeof(D3DVIEWPORT);
    viewData.dwX = x;
	viewData.dwY = y;
    viewData.dwWidth = width;
    viewData.dwHeight = height;
    viewData.dvScaleX = D3DVAL(65536.0 * unitsperpixel);
    viewData.dvScaleY = D3DVAL(65536.0 * unitsperpixel);
    viewData.dvMaxX = D3DVAL(1.0);
    viewData.dvMaxY = D3DVAL(1.0);

    TRY_D3D(Viewport->SetViewport(&viewData));

	return TRUE;
}

BOOL T3DScene::InitializeMatrices()
{
    /*
     * Set the view, world and projection matrices
     * Create a buffer for matrix set commands etc.
     */
    MAKE_MATRIX(Device, hView, identity);
    MAKE_MATRIX(Device, hProj, proj);
    MAKE_MATRIX(Device, hWorld, identity);

	TExecuteBuf exbuf;

	exbuf.BeginRecord();
	exbuf.SetTransform(D3DTRANSFORMSTATE_VIEW, hView);
	exbuf.SetTransform(D3DTRANSFORMSTATE_PROJECTION, hProj);
	exbuf.SetTransform(D3DTRANSFORMSTATE_WORLD, hWorld);
	exbuf.SetLightState(D3DLIGHTSTATE_AMBIENT, RGBA_MAKE(64, 64, 64, 64));
	exbuf.EndRecord();
	BeginScene();
	exbuf.Render();
	EndScene();

	return TRUE;
}

BOOL T3DScene::Close()
{
	if (!initialized)
		return FALSE;

  // Release execute buffer buffers and matrices
	TExecuteBuf::Close();

	AnimatorArray.Clear();
	LightArray.DeleteAll();

    RELEASE(lpBmat);

	TRY_D3D(Device->DeleteViewport(Viewport));
	RELEASE(Viewport);
	Viewport = NULL;
	Viewport2 = NULL;

	initialized = FALSE;

	return TRUE;
}

// Clears a rectangle in the screen zbuffer... uses either DrawRestoreRect, or
// Viewport->Clear() depending on whether the display has a secondary background zbuffer
// set for it or not.  
void T3DScene::RestoreZBuffer(SRect &r)
{
	if (NoScrollZBuffer)
		MapPane.DrawRestoreRect(r.x(), r.y(), r.w(), r.h(),
			DM_WRAPCLIPSRC | DM_NORESTORE | DM_ZBUFFER | DM_NODRAW);
	if (Display->UsingClearZBuffer())
	{
		SRect sr, pr;
		sr = r;
		MapPane.PaneToScreen(sr);
		MapPane.GetRect(pr);
		if (!ClipRect(sr, pr, sr))
			return;
		Display->GetRealZBuffer()->Blit(sr.x(), sr.y(), 
			Display->GetZBuffer(), sr.x(), sr.y(), sr.w(), sr.h());
	}
}

void T3DScene::SetCameraPos(RS3DPoint pos, int zdist)
{
	if (!initialized)
		return;

	CameraPos = pos;

	D3DVECTOR v1;//, v2;

	
	D3DMATRIX view;
	D3DMATRIXClear(&view);

	v1.x = D3DVAL(CameraPos.x);
	v1.y = D3DVAL(CameraPos.y);
	v1.z = D3DVAL(CameraPos.z);
	D3DMATRIXMove(&view, &v1); 

	D3DMATRIXRotateZ(&view, D3DVAL(45.0 * TORADIAN));
	D3DMATRIXRotateX(&view, D3DVAL(-(90.0 + CAMERAANGLE) * TORADIAN));

	v1.x = D3DVAL(0.0);
	v1.y = D3DVAL(0.0);
	v1.z = D3DVAL(-zdist);
	D3DMATRIXMove(&view, &v1); 

    Device->SetMatrix(hView, &view);
}

void T3DScene::RefreshZBuffer()
{
  // Draw ZBuffer behind objects
	for (int c = 0; c < AnimatorArray.NumItems(); c++)
		if (AnimatorArray[c] != NULL)
			AnimatorArray[c]->RefreshZBuffer();
}

BOOL T3DScene::DrawScene()
{
	int c;

	if (!initialized )
		return FALSE;

	if (!Show3D)
		return TRUE;

	static BOOL oldFlatShade, oldDitherEnable, oldBlendEnable, oldSpecularEnable, oldZEnable, oldBilinearFilter;
	static D3DCOLOR oldAmbientColor;
	static BOOL b = FALSE;
	HRESULT DirectDrawReturn;

	if (FlatShade != oldFlatShade ||				// Render state has changed?
	    DitherEnable != oldDitherEnable ||
		BlendEnable != oldBlendEnable ||
		SpecularEnable != oldSpecularEnable ||
		ZEnable != oldZEnable ||
		BilinearFilter != oldBilinearFilter ||
		oldAmbientColor != AmbientColor)
	{
	    oldFlatShade = FlatShade;
	    oldDitherEnable = DitherEnable;
	    oldBlendEnable = BlendEnable;
	    oldSpecularEnable = SpecularEnable;
	    oldZEnable = ZEnable;
	    oldBilinearFilter = BilinearFilter;
	    oldAmbientColor = AmbientColor;

	  // Reset scene params
		BeginScene();
		BeginRender();

		TRY_D3D(SetRenderState(D3DRENDERSTATE_ZENABLE, ZEnable));
		TRY_D3D(SetRenderState(D3DRENDERSTATE_SHADEMODE, (FlatShade ? D3DSHADE_FLAT : D3DSHADE_GOURAUD)));
		TRY_D3D(SetRenderState(D3DRENDERSTATE_DITHERENABLE, DitherEnable));
		TRY_D3D(SetRenderState(D3DRENDERSTATE_BLENDENABLE, BlendEnable));
		TRY_D3D(SetRenderState(D3DRENDERSTATE_SPECULARENABLE, SpecularEnable));
		TRY_D3D(SetRenderState(D3DRENDERSTATE_TEXTUREMAG, (BilinearFilter ? D3DFILTER_LINEAR : D3DFILTER_NEAREST)));
		TRY_D3D(SetRenderState(D3DRENDERSTATE_TEXTUREMIN, (BilinearFilter ? D3DFILTER_LINEAR : D3DFILTER_NEAREST)));

		TRY_D3D(SetLightState(D3DLIGHTSTATE_AMBIENT, AmbientColor));

		EndRender();
		EndScene();
	}

    // Verify both surfaces
	if (!front) 
		return FALSE;

    if (!zbuffer) 
		return FALSE;

    if (b) 
	{
		b = FALSE;
    }

    // Restore the primary surface if it has been lost
    if (front->IsLost())
    {
		DirectDrawReturn = front->Restore();
		
		if (DirectDrawReturn != DD_OK) 
			return FALSE;

        b = TRUE;
    }
    
    // Restore the ZBuffer if it has been lost
    if (zbuffer->IsLost())
    {
        DirectDrawReturn = zbuffer->Restore();

        if (DirectDrawReturn != DD_OK) 
			return FALSE;

        b = TRUE;
    }

  // Draw shaodws
	for (c = 0; c < AnimatorArray.NumItems(); c++)
		if (AnimatorArray[c] != NULL)
			AnimatorArray[c]->DrawShadow();

  // Draw objects
	BeginScene();

	for (c = 0; c < AnimatorArray.NumItems(); c++)
	{
		if (AnimatorArray[c] != NULL)
		{
			PTObjectInstance inst = AnimatorArray[c]->GetObjInst();
			PTObjectImagery imagery = AnimatorArray[c]->GetImagery();

			if (inst->GetFlags() & OF_INVISIBLE ||
			  (DWORD)inst->GetState() >= (DWORD)imagery->GetHeader()->numstates ||
			  (DWORD)inst->GetFrame() >= (DWORD)imagery->GetHeader()->states[inst->GetState()].frames)
				continue;

			TRY_D3D(SetRenderState(D3DRENDERSTATE_BLENDENABLE, BlendEnable));
			TRY_D3D(SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
			TRY_D3D(SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA));
			TRY_D3D(SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA));
			TRY_D3D(SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID));

			AnimatorArray[c]->PreRender();		// Start rendering
			AnimatorArray[c]->Render();			// Object Render
			AnimatorArray[c]->PostRender();		// End rendering
		}
	}

	EndScene();

	return TRUE;
}

void T3DScene::SetAmbientLight(int ambient)  // What the heck 
{
	if (!initialized)
		return;

	AmbientValue = min((ambient * 6), 255);

	int red = AmbientColor256.red * AmbientValue / 256;
	int green = AmbientColor256.green * AmbientValue / 256;
	int blue = AmbientColor256.blue * AmbientValue / 256;
	
	AmbientColor = RGBA_MAKE(red, green, blue, 255);
}

void T3DScene::SetAmbientColor(SColor &color)
{
	if (!initialized)
		return;

	AmbientColor256 = color;

	int red = AmbientColor256.red * AmbientValue / 256;
	int green = AmbientColor256.green * AmbientValue / 256;
	int blue = AmbientColor256.blue * AmbientValue / 256;
	
	AmbientColor = RGBA_MAKE(red, green, blue, 255);
}

// ************ Light Functions **************

int T3DScene::AddLight(RS3DPoint pos, SColor color, int intensity, int multiplier)
{
	if (!initialized)
		return -1;

    /*
     * Setup lights
     */
    D3DLIGHT lightdata;
    memset(&lightdata, 0, sizeof(D3DLIGHT));
    lightdata.dwSize = sizeof(D3DLIGHT);
    lightdata.dltType = D3DLIGHT_POINT;
    lightdata.dcvColor.r = D3DVAL(color.red) / D3DVAL(256.0);
    lightdata.dcvColor.g = D3DVAL(color.green) / D3DVAL(256.0);
    lightdata.dcvColor.b = D3DVAL(color.blue) / D3DVAL(256.0);
    lightdata.dcvColor.a = D3DVAL(1.0);
	lightdata.dvPosition.x = D3DVAL(pos.x);
	lightdata.dvPosition.y = D3DVAL(pos.y);
	lightdata.dvPosition.z = D3DVAL(pos.z);
	lightdata.dvDirection.x = D3DVAL(0.0);
	lightdata.dvDirection.y = D3DVAL(0.0);
	lightdata.dvDirection.z = D3DVAL(0.0);
	lightdata.dvRange = D3DVAL(1000000.0);
	lightdata.dvFalloff = D3DVAL(0.0);
//	lightdata.dvAttenuation0 = D3DVAL(1.0);
//	lightdata.dvAttenuation1 = D3DVAL(1.0/(D3DVALUE)intensity);
//	lightdata.dvAttenuation2 = D3DVAL(0.0);
	lightdata.dvTheta = D3DVAL(0.0);
	lightdata.dvPhi = D3DVAL(0.0);

	return LightArray.Add(new T3DLight(lightdata, intensity, multiplier));
}

BOOL T3DScene::DeleteLight(int lightid)
{
    if (!initialized || lightid < 0 || lightid >= LightArray.NumItems())
		return FALSE;
		
	LightArray.Delete(lightid);

	return TRUE;
}

BOOL T3DScene::SetLightIntensity(int lightid, int intensity)
{
    if (!initialized || lightid < 0 || 
	  lightid >= LightArray.NumItems() ||
	  LightArray[lightid] == NULL)
		return FALSE;

	LightArray[lightid]->SetIntensity(intensity);

	return TRUE;
}

BOOL T3DScene::SetLightMultiplier(int lightid, int multiplier)
{
    if (!initialized || lightid < 0 || 
	  lightid >= LightArray.NumItems() ||
	  LightArray[lightid] == NULL)
		return FALSE;

	LightArray[lightid]->SetMultiplier(multiplier);

	return TRUE;
}

BOOL T3DScene::SetLightColor(int lightid, SColor color)
{
    if (!initialized || lightid < 0 || 
	  lightid >= LightArray.NumItems() ||
	  LightArray[lightid] == NULL)
		return FALSE;

	// Change light in the frame
	LightArray[lightid]->SetColor(
			D3DVAL(color.red) / D3DVAL(256.0),
			D3DVAL(color.green) / D3DVAL(256.0),
			D3DVAL(color.blue) / D3DVAL(256.0));

	return TRUE;
}

BOOL T3DScene::SetLightPosition(int lightid, RS3DPoint pos)
{
	if (!initialized)
		return FALSE;

    if (!initialized || lightid < 0 || 
	  lightid >= LightArray.NumItems() ||
	  LightArray[lightid] == NULL)
		return FALSE;

	// Change light in the frame
	LightArray[lightid]->SetPosition(
			D3DVAL(pos.x),
			D3DVAL(pos.y),
			D3DVAL(pos.z));

	return TRUE;
}

int T3DScene::GetNumLights()
{
	if (!initialized)
		return 0;

	return LightArray.NumItems();
}

  // The following functions are called by the character system to do lighting

BOOL T3DScene::LightAffectObject(int x, int y, int z)
{
	if (!initialized)
		return FALSE;

	D3DVALUE mindist = (D3DVALUE)10000.0;
	int minlight = -1;
	D3DVALUE mindist2 = (D3DVALUE)10000.0;
	int minlight2 = -1;
	D3DVALUE mindist3 = (D3DVALUE)10000.0;
	int minlight3 = -1;
	for (int c = 0; c < LightArray.NumItems(); c++)
	{
		if (LightArray[c] == NULL)
			continue;

		D3DVALUE dist = LightArray[c]->DistanceToObject((D3DVALUE)x, (D3DVALUE)y, (D3DVALUE)z);
		if (dist < mindist)
		{
			mindist3 = mindist2;
			minlight3 = minlight2;
			mindist2 = mindist;
			minlight2 = minlight;
			mindist = dist;
			minlight = c;
		}
		else if (dist < mindist2)
		{
			mindist3 = mindist2;
			minlight3 = minlight2;
			mindist2 = dist;
			minlight2 = c;
		}
		else if (dist < mindist3)
		{
			mindist3 = dist;
			minlight3 = c;
		}
	}

	if (minlight >= 0)
		if (!LightArray[minlight]->AffectObject((D3DVALUE)x, (D3DVALUE)y, (D3DVALUE)z))
			return FALSE;
	if (MaxLights > 1 && minlight2 >= 0)
		if (!LightArray[minlight2]->AffectObject((D3DVALUE)x, (D3DVALUE)y, (D3DVALUE)z))
			return FALSE;
	if (MaxLights > 2 && minlight3 >= 0)
		if (!LightArray[minlight3]->AffectObject((D3DVALUE)x, (D3DVALUE)y, (D3DVALUE)z))
			return FALSE;

	return FALSE;
}

void T3DScene::ResetAllLights()
{
	if (!initialized)
		return;

	for (int c = 0; c < LightArray.NumItems(); c++)
	{
		if (LightArray[c] != NULL)  // Stop on first light that affects char
			LightArray[c]->LightOff();
	}
}

// ************ Object Functions **************

int T3DScene::AddAnimator(PT3DAnimator animator)
{
	if (!initialized)
		return -1;

	return AnimatorArray.Add(animator);
}
        
BOOL T3DScene::RemoveAnimator(DWORD animid)
{
	if (!initialized || animid >= (DWORD)AnimatorArray.NumItems() || AnimatorArray[animid] == NULL)
		return FALSE;

	AnimatorArray.Remove(animid);	
	
	return TRUE;
}

int T3DScene::GetNumAnimators()
{
	if (!initialized)
		return 0;

	return AnimatorArray.NumItems();
}

// ************************
// * Texture Format Stuff *
// ************************

HRESULT CALLBACK EnumTextureFormatsCallback(LPDDSURFACEDESC lpDDSD, LPVOID lpContext)
{
	PT3DScene scene = (PT3DScene)lpContext;

	if (numtexformats < MAXTEXFORMATS)
	{
		memcpy(&texformats[numtexformats], lpDDSD, sizeof(DDSURFACEDESC));
		numtexformats++;
	}

    return DDENUMRET_OK;
}

void T3DScene::GetTextureFormats()
{
	numtexformats = 0;

    Device->EnumTextureFormats(EnumTextureFormatsCallback, (LPVOID)this);
}

inline int countbits(DWORD d)
{
	int numbits = 0;
	for (int c = 0; c < 32; c++)
	{
		if (d & (1 << c))
			numbits++;
	}
	return numbits;
}	

void T3DScene::GetClosestTextureFormat(LPDDSURFACEDESC srcsd, LPDDSURFACEDESC dstsd)
{
	if (!initialized)
		return;

	LPDDPIXELFORMAT spf = &srcsd->ddpfPixelFormat;

	int redbits, greenbits, bluebits, alphabits; // Yum.. Alphabits is a tasty part of a
												 // balanced breakfast
	if (srcsd->ddpfPixelFormat.dwFlags & DDPF_RGB)
	{	
		redbits = countbits(srcsd->ddpfPixelFormat.dwRBitMask);
		greenbits = countbits(srcsd->ddpfPixelFormat.dwGBitMask);
		bluebits = countbits(srcsd->ddpfPixelFormat.dwBBitMask);
		alphabits = countbits(srcsd->ddpfPixelFormat.dwRGBAlphaBitMask);
	}
	else if (srcsd->ddpfPixelFormat.dwFlags & 
		(DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1))
	{
		redbits = 5;	// Allows match system to find an RGB format if no matching
		greenbits = 6;	// palettized format can be found
		bluebits = 5;
		alphabits = 0;
	}	

	int diff;
	int closest = 0;
	int closestdiff = 10000;
	LPDDSURFACEDESC sd = texformats;
	for (int c = 0; c < numtexformats; c++, sd++)
	{
		LPDDPIXELFORMAT pf = &sd->ddpfPixelFormat;

	  // If palettized format exactly matches, return immediately
		if ((pf->dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4 
		  | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1)) != 0 &&
			(pf->dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4 
		  | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1)) ==
			(spf->dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXED4 
		  | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1)) &&
			(pf->dwRGBBitCount == spf->dwRGBBitCount))
		{
			closest = c;
			closestdiff = 0;
			break;
		}

		int dredbits, dgreenbits, dbluebits, dalphabits;
		if (pf->dwFlags & DDPF_PALETTEINDEXED8)
		{
		  // Rank palettized 8 bit above 434 RGB 8 bit, but below everything else
			dredbits = dgreenbits = dbluebits = 4; 
			dalphabits = 0;
		}
		else if (pf->dwFlags & DDPF_PALETTEINDEXED4)
		{
		  // Rank palettized 4 bit above palettized 2 bit
			dredbits = dgreenbits = dbluebits = 2; 
			dalphabits = 0;
		}
		else if (pf->dwFlags & DDPF_PALETTEINDEXED2)
		{
		  // Rank palettized 2 bit above palettized 1 bit
			dredbits = dgreenbits = dbluebits = 1; 
			dalphabits = 0;
		}
		else if (pf->dwFlags & DDPF_PALETTEINDEXED1)
		{
		  // Rank palettized 1 bit below everything else
			dredbits = 1;
			dgreenbits = dbluebits = dalphabits = 0;
		}
		else if (pf->dwFlags & DDPF_RGB)
		{
			dredbits = countbits(pf->dwRBitMask);
			dgreenbits = countbits(pf->dwGBitMask);
			dbluebits = countbits(pf->dwBBitMask);
			dalphabits = countbits(pf->dwRGBAlphaBitMask);
		}
		else
			dredbits = dgreenbits = dbluebits = dalphabits = 10000; // Forget it

		diff = abs(redbits - dredbits) + 
			   abs(greenbits - dgreenbits) +
			   abs(bluebits - dbluebits) +
			   abs(alphabits - dalphabits);

		if (diff < closestdiff)
		{	
			closest = c;
			closestdiff = diff;
		}
	}

	memcpy(dstsd, &texformats[closest], sizeof(DDSURFACEDESC));
}

inline int getmaskshift(DWORD mask)
{
	if (!mask)
		return 0;

	int shift = 0;
	while (!(mask & 1) && shift < 32)
	{
		mask = mask >> 1;
		shift++;
	}
	return shift;
}

inline int getmaskbits(DWORD mask)
{
	if (!mask)
		return 0;

	int bits = 0;
	for (int c = 0; c < 32; c++)
	{
		if (mask & (1 << c))
			bits++;
	}
	return bits;
}

void T3DScene::ConvertTexture(
	LPDDSURFACEDESC srcsd, LPVOID srcpixels, LPPALETTEENTRY srcpal,  
	LPDDSURFACEDESC dstsd, LPVOID dstpixels, LPPALETTEENTRY dstpal)
{
	if (!initialized)
		return;

  // How many colors do our palettes have
	int srcbufsize = (srcsd->ddpfPixelFormat.dwRGBBitCount * srcsd->lPitch * srcsd->dwHeight) >> 3;
	int srcpalcolors = 0;
	if (srcsd->ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
	  DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1))
		srcpalcolors = 1 << srcsd->ddpfPixelFormat.dwRGBBitCount;

	int dstbufsize = (dstsd->ddpfPixelFormat.dwRGBBitCount * dstsd->lPitch * dstsd->dwHeight) >> 3;
	int dstpalcolors = 0;
	if (dstsd->ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
	  DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1))
		dstpalcolors = 1 << dstsd->ddpfPixelFormat.dwRGBBitCount;
	int dstnumcolors = 0; // How many colors are in dst palette right now

  // Copy surface and return if it is already in the correct format (QUICK)
	if ( srcsd->dwWidth == dstsd->dwWidth && srcsd->dwHeight == dstsd->dwHeight && 
		 srcsd->lPitch == dstsd->lPitch &&
		 !memcmp(&srcsd->ddpfPixelFormat, &dstsd->ddpfPixelFormat, sizeof(DDPIXELFORMAT)) )
	{
		memcpy(dstpixels, srcpixels, dstbufsize);
		memcpy(dstpal, srcpal, sizeof(PALETTEENTRY) * dstpalcolors);
		return;
	}
	
  // Get shifts/etc. for source
	int srcbits = srcsd->ddpfPixelFormat.dwRGBBitCount;
	DWORD srcredshift, srcredmask, srcredbits,
		  srcgreenshift, srcgreenmask, srcgreenbits,
		  srcblueshift, srcbluemask, srcbluebits;
	DWORD srcalphashift, srcalphamask, srcalphabits;
	BOOL srcisrgb = !(srcsd->ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
	  DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1));
	BOOL srchasalpha = srcsd->ddpfPixelFormat.dwRGBAlphaBitMask != 0;
	if (srcisrgb)
	{
		srcredmask = srcsd->ddpfPixelFormat.dwRBitMask;
		srcredshift = getmaskshift(srcredmask);
		srcredbits = 8 - getmaskbits(srcredmask);
		srcgreenmask = srcsd->ddpfPixelFormat.dwGBitMask;
		srcgreenshift = getmaskshift(srcgreenmask);
		srcgreenbits = 8 - getmaskbits(srcgreenmask);
		srcbluemask = srcsd->ddpfPixelFormat.dwBBitMask;
		srcblueshift = getmaskshift(srcbluemask);
		srcbluebits = 8 - getmaskbits(srcbluemask);
		if (srchasalpha)
		{
			srcalphamask = srcsd->ddpfPixelFormat.dwRGBAlphaBitMask;
			srcalphashift = getmaskshift(srcalphamask);
			srcalphabits = 8 - getmaskbits(srcalphamask);
		}
		else
			srcalphamask = srcalphashift = srcalphabits = 0;
	}

  // Get shifts/etc. for dest
	int dstbits = dstsd->ddpfPixelFormat.dwRGBBitCount;
	DWORD dstredshift, dstredmask, dstredbits,
		  dstgreenshift, dstgreenmask, dstgreenbits,
		  dstblueshift, dstbluemask, dstbluebits;
	DWORD dstalphashift, dstalphamask, dstalphabits;
	BOOL dstisrgb = !(dstsd->ddpfPixelFormat.dwFlags & (DDPF_PALETTEINDEXED8 | DDPF_PALETTEINDEXEDTO8 |
	  DDPF_PALETTEINDEXED4 | DDPF_PALETTEINDEXED2 | DDPF_PALETTEINDEXED1));
	BOOL dsthasalpha = dstsd->ddpfPixelFormat.dwRGBAlphaBitMask != 0;
	if (dstisrgb)
	{
		dstredmask = dstsd->ddpfPixelFormat.dwRBitMask;
		dstredshift = getmaskshift(dstredmask);
		dstredbits = 8 - getmaskbits(dstredmask);
		dstgreenmask = dstsd->ddpfPixelFormat.dwGBitMask;
		dstgreenshift = getmaskshift(dstgreenmask);
		dstgreenbits = 8 - getmaskbits(dstgreenmask);
		dstbluemask = dstsd->ddpfPixelFormat.dwBBitMask;
		dstblueshift = getmaskshift(dstbluemask);
		dstbluebits = 8 - getmaskbits(dstbluemask);
		if (dsthasalpha)
		{
			dstalphamask = dstsd->ddpfPixelFormat.dwRGBAlphaBitMask;
			dstalphashift = getmaskshift(dstalphamask);
			dstalphabits = 8 - getmaskbits(dstalphamask);
		}
		else
		{
			dstalphamask = dstalphashift = 0;
			dstalphabits = 8; // Don't use alpha!
		}
	}

	BYTE *sline = (BYTE *)srcpixels;
	BYTE *dline = (BYTE *)dstpixels;

	for (int l = 0; l < (long)srcsd->dwHeight; l++,
		 sline += (srcsd->lPitch * srcbits) >> 3,
		 dline += (dstsd->lPitch * dstbits) >> 3)
	{
	  int srcbitpos = 0;
	  int dstbitpos = 0;
	  for (int c = 0; c < (int)srcsd->dwWidth; c++, srcbitpos += srcbits, dstbitpos += dstbits)
	  {
		BYTE *s = sline + (srcbitpos >> 3);	
		BYTE *d = dline + (dstbitpos >> 3);	
		DWORD r, g, b, a;
		int clrindex;

		clrindex = 0;

		if (srcisrgb)  // Do rgba extraction of any 8-32 bit pixel (including alpha/mask data)
		{
			DWORD s32;
			if (srcbits == 32)
			{
				s32 = *(DWORD *)s;
			}
			else if (srcbits == 24)
			{
				s32 = 0;
				*(((BYTE *)&s32) + 0) = *(s + 0);
				*(((BYTE *)&s32) + 1) = *(s + 1);
				*(((BYTE *)&s32) + 2) = *(s + 2);
			}
			else if (srcbits == 16)
			{
				s32 = *(WORD *)s;
			}
			else if (srcbits <= 8)
			{	
				s32 = *s;
			}
			r = ((s32 >> srcredshift) << srcredbits) & 0xFF;
			g = ((s32 >> srcgreenshift) << srcgreenbits) & 0xFF;
			b = ((s32 >> srcblueshift) << srcbluebits) & 0xFF;
			if (srchasalpha)
				a = ((s32 >> srcalphashift) << srcalphabits) & 0xFF;
			else
				a = 255;
		}
		else  // This is a palettized image.. get rgba info if dest is rgba, otherwise get clrindex
		{
			clrindex = *(BYTE *)s; // All indexed formats are less than 8 bits (duh)
			if (srcbits < 8)
				clrindex = clrindex >> (8 - ((srcbitpos & 7) + srcbits));
			if (dstisrgb)
			{
				r = srcpal[clrindex].peRed;
				g = srcpal[clrindex].peGreen;
				b = srcpal[clrindex].peBlue;
			}
			a = 255;
		}

		if (dstisrgb)	// Put rgba data back into dest pixel
		{
			DWORD d32 =
				((r >> dstredbits) << dstredshift) |
				((g >> dstgreenbits) << dstgreenshift) |
				((b >> dstbluebits) << dstblueshift) |
				((a >> dstalphabits) << dstalphashift);
			if (dstbits == 32)
			{
				*(DWORD *)d = d32;
			}
			else if (dstbits == 24)
			{
				*(d + 0) = *(((BYTE *)&d32) + 0);
				*(d + 1) = *(((BYTE *)&d32) + 1);
				*(d + 2) = *(((BYTE *)&d32) + 2);
			}
			else if (dstbits == 16)
			{
				*(WORD *)d = (WORD)d32;
			}
			else if (dstbits == 8)
			{
				*(BYTE *)d = (BYTE)d32;
			}
			else if (dstbits < 8)
			{
				if (!(dstbitpos & 7))
					*(BYTE *)d = (BYTE)d32;
				else 
					*(BYTE *)d |= (BYTE)d32;
			}
		}
		else  // clrindex dest.. put clrindex into bits, build palette if necessary
		{
			if (srcisrgb)  // Build palette	(note: doesn't do 16->8 or 24->8 color remapping!!)
			{
				PALETTEENTRY pe;
				pe.peRed = (BYTE)r;
				pe.peGreen = (BYTE)g;
				pe.peBlue = (BYTE)b;
				pe.peFlags = 0;
				for (clrindex = 0; clrindex < dstnumcolors; clrindex++)
				{
					if (*(DWORD *)&pe == *(DWORD *)&dstpal[clrindex])
						break;
				}
				if (clrindex >= dstnumcolors && clrindex < dstpalcolors)
				{
					dstpal[clrindex] = pe;
					dstnumcolors++;
				}
			}

			if (dstbits < 8)
				clrindex = clrindex << (8 - ((dstbitpos & 7) + dstbits));
			
			if (!(dstbitpos & 7)) 
				*d = (BYTE)clrindex;
			else
				*d |= (BYTE)clrindex;
		}
	  }
	}
}	

// ************************************
// * 3DScene Draw Primitive Functions *
// ************************************

// The functions below are a subset of the D3D Device interface, and are used to 
// map calls to that interface into calls to the execute buffer recorder object
// if UseDrawPrimitive is FALSE.  The theory is that a good D3DEXECUTEBUFFER is
// substantially faster than the DrawPrimitive functions.

// Current execute buffer
static TExecuteBuf InternalExecuteBuf;	// Internal execute buffer used by 3DScene
static PTExecuteBuf ExecuteBuf;			// Pointer to current execute buffer (records only if points to Internal)
static BOOL InRecord, WasRendered;		// Did we render this buffer yet?
static D3DCLIPSTATUS ClipStatus;		// Current clip status
static BOOL WasFlushed;					// Was this render reset by a call to GetClipStatus()

// Arrays to store current states...
static DWORD currenderstate[63]; // These ranges should be good for the life of this program
static DWORD curlightstate[16];
static D3DMATRIX curtransmatrix[4];

// Matrix lists
#define EXBUF_MAXMATRICES	  128
static D3DMATRIXHANDLE matrixlist[EXBUF_MAXMATRICES];
static int curmatrix;	// This is incremented on each call to SetTransform()

void T3DScene::CreateMatrixList()
{
	for (int c = 0; c < EXBUF_MAXMATRICES; c++)
	{
		TRY_D3D(Device->CreateMatrix(&matrixlist[c]));
	}
	curmatrix = 0;
}

void T3DScene::ClearMatrixList()
{
	for (int c = 0; c > EXBUF_MAXMATRICES; c++)
	{
		TRY_D3D(Device->DeleteMatrix(matrixlist[c]));
	}
	curmatrix = 0;
}

D3DMATRIXHANDLE T3DScene::GetMatrix(int matrixnum)
{
	return matrixlist[matrixnum];
}

int T3DScene::GetCurMatrix()
{
	return curmatrix;
}

HRESULT T3DScene::BeginRender()
{
	if (InRecord)
		return DD_OK;

	curmatrix = 0; // Set current matrix in matrix list to 0

	if (!UseDrawPrimitive && ExecuteBuf == &InternalExecuteBuf)
		ExecuteBuf->BeginRecord();	// Record if set to internal buf, otherwise just render

	WasRendered = FALSE;
	WasFlushed = FALSE;
	InRecord = TRUE;

	currenderstate[D3DRENDERSTATE_TEXTUREHANDLE] = 0xFFFFFFFF;
	curlightstate[D3DLIGHTSTATE_MATERIAL] = 0;

	return DD_OK;
}

HRESULT T3DScene::EndRender()
{
	if (!InRecord)
		return DD_OK;

	if (!UseDrawPrimitive)
	{
		if (ExecuteBuf == &InternalExecuteBuf)
			ExecuteBuf->EndRecord();// End record if doing internal buf, otherwise just render

		if (WasRendered == FALSE)	// Prevent object from being rendered in both EndMesh & EndScene
		{
			ExecuteBuf->Render(&ClipStatus);
			WasRendered = TRUE;
		}
	}

	ExecuteBuf = &InternalExecuteBuf;
	InternalExecuteBuf.EndRecord(); // Make sure internal buffer isn't recording

	curmatrix = 0;	// Clear matrix list counter to 0
	
	InRecord = FALSE;

	return DD_OK;
}

HRESULT T3DScene::BeginScene()
{
	HRESULT err;

	if (UseDrawPrimitive)
		err = Device2->BeginScene();
	else
		err = Device->BeginScene();

	if (err != DD_OK)
		return err;

	ExecuteBuf = &InternalExecuteBuf;

	return DD_OK;
}

HRESULT T3DScene::EndScene()
{
	HRESULT err;

	err = EndRender();
	if (err != DD_OK)
		return err;
		
	if (UseDrawPrimitive)
		return Device2->EndScene();
	else
		return Device->EndScene();
}

HRESULT T3DScene::SetClipStatus(LPD3DCLIPSTATUS cs)
{
	if (UseDrawPrimitive)
		return Device2->SetClipStatus(cs);

	if (ExecuteBuf == &InternalExecuteBuf)
		return ExecuteBuf->SetClipStatus(cs);
	else
		return DD_OK;
}

// Note: the GetClipStatus function forces the current execute buffer to be rendered, and a new
// one started!!!  If this is NOT what you want, don't call this function, or any derived function
// such as GetExtents() or UpdateExtents().  Also note that animations with complex extents
// calculations can NOT be buffered.

HRESULT T3DScene::GetClipStatus(LPD3DCLIPSTATUS cs)
{
	if (UseDrawPrimitive)
		return Device2->GetClipStatus(cs);

	BOOL isrecording = ExecuteBuf->IsRecording();

	if (isrecording)
		EndRender(); // Force whatever mesh was last recording to be drawn to screen

	memcpy(cs, &ClipStatus, sizeof(D3DCLIPSTATUS));

	if (isrecording)
	{
		BeginRender();
		WasFlushed = TRUE;	// Flag that we had to flush the buffer in the middle of a render
	}

	return DD_OK;
}

HRESULT T3DScene::SetRenderState(D3DRENDERSTATETYPE rs, DWORD data)
{
	if (UseDrawPrimitive)
		return Device2->SetRenderState(rs, data);

	currenderstate[rs] = data;

	if (ExecuteBuf == &InternalExecuteBuf)
		return ExecuteBuf->SetRenderState(rs, data);
	else 
		return DD_OK;
}                  

HRESULT T3DScene::GetRenderState(D3DRENDERSTATETYPE rs, DWORD *data)
{
	if (UseDrawPrimitive)
		return Device2->GetRenderState(rs, data);

	*data = currenderstate[rs];
	return DD_OK;
}

HRESULT T3DScene::SetLightState(D3DLIGHTSTATETYPE ls, DWORD data)
{
	if (UseDrawPrimitive)
		return Device2->SetLightState(ls, data);

	curlightstate[ls] = data;

	if (ExecuteBuf == &InternalExecuteBuf)
		return ExecuteBuf->SetLightState(ls, data);
	else
		return DD_OK;
}

HRESULT T3DScene::GetLightState(D3DLIGHTSTATETYPE ls, DWORD *data)
{
	if (UseDrawPrimitive)
		return Device2->GetLightState(ls, data);

	*data = curlightstate[ls];
	return DD_OK;
}

HRESULT T3DScene::SetTransform(D3DTRANSFORMSTATETYPE ts, LPD3DMATRIX matrix)
{
	if (UseDrawPrimitive)
		return Device2->SetTransform(ts, matrix);

	if (curmatrix >= EXBUF_MAXMATRICES)
		return D3DERR_MATRIX_SETDATA_FAILED;
	Device->SetMatrix(matrixlist[curmatrix], matrix);
	curmatrix++;

	memcpy(&curtransmatrix[ts], matrix, sizeof(D3DMATRIX)); // Save matrix for later

	if (ExecuteBuf == &InternalExecuteBuf)
		return ExecuteBuf->SetTransform(ts, matrixlist[curmatrix - 1]);
	else
		return DD_OK;
}

HRESULT T3DScene::GetTransform(D3DTRANSFORMSTATETYPE ts, LPD3DMATRIX matrix)
{
	if (UseDrawPrimitive)
		return Device2->GetTransform(ts, matrix);

	memcpy(matrix, &curtransmatrix[ts], sizeof(D3DMATRIX));

	return DD_OK;
}

HRESULT T3DScene::DrawIndexedPrimitive(
	D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, LPWORD i, DWORD ic, DWORD f)
{
	if (UseDrawPrimitive)
		return Device2->DrawIndexedPrimitive(pt, vt, v, vc, i, ic, f);
	
	if (ExecuteBuf == &InternalExecuteBuf)
		return ExecuteBuf->DrawIndexedPrimitive(pt, vt, v, vc, i, ic, f);
	else
		return DD_OK;
}

HRESULT T3DScene::DrawPrimitive(
	D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, DWORD f)
{
	if (UseDrawPrimitive)
		return Device2->DrawPrimitive(pt, vt, v, vc, f);

	if (ExecuteBuf == &InternalExecuteBuf)
		return ExecuteBuf->DrawPrimitive(pt, vt, v, vc, f);
	else
		return DD_OK;
}

// Returns TRUE if we had to flush the current execute buffer in the middle of a Render() so
// we could get the extents.  This means we won't be able to cache this render.
BOOL T3DScene::WasRenderFlushed()
{
	return WasFlushed;
}

// Returns a pointer to the current execute buffer
PTExecuteBuf T3DScene::GetExecuteBuf()
{
	return ExecuteBuf;

}

// Gets a copy of the previously recorded execute buffer
PTExecuteBuf T3DScene::CopyExecuteBuf()
{
	return new TExecuteBuf(*ExecuteBuf);
}

// Ignores all calls to DrawPrimitive functions, and just renders the supplied buffer
// to render when EndMesh() or EndScene() is called.
void T3DScene::UseExecuteBuf(PTExecuteBuf usebuf)
{
	if (InRecord) // Can't be recording when we set the buffer
		return;

	ExecuteBuf = usebuf;	
}

// **************************************
// * TExecuteBuf - Execute buffer stuff *
// **************************************

// The TExecuteBuf object basically contains a pointer to an execute buffer.  When BeginRecord()
// is called, the object will begin recording DrawPrimitive calls to the execute buffer.
// When EndRecord() is called, the data is copied into the execute buffer, and the execute buffer
// pointer is set.  After this, Render() can be called as many times as the user wants without
// having to re-record the buffer.

#define EXBUF_MAXRENDERSTATES 32
#define EXBUF_MAXLIGHTSTATES  32
#define EXBUF_MAXTRANSSTATES  32
#define EXBUF_BUFGROWSIZE	  8192	

struct STATEDATA
{
	DWORD state;
	DWORD data;
};

// General stuff
static PTExecuteBuf recording;	// Points to buffer we are currently recording

// Vertex values	
static DWORD vertmode;
static int vertsize;

// Vertex buffer
static int vertbufsize;		// Current vertex buf size
static BYTE *vertbuf, *vertptr; // Buffer
static LPVOID vb;			// Current vertex pointer
static int numverts;		// Current number of vertices 

// Transform op buffer (For vertices)
static int transbufsize;	// Current buf size
static BYTE *transbuf, *transptr; // Buffer
static LPVOID tb;			// Current op pointer
static LPVOID lasttb;		// Pointer to previous op
static DWORD tblastop;		// Id of previous op

// Face op buffer (For triangles)
static int facebufsize;	// Current buf size
static BYTE *facebuf, *faceptr; // Buffer
static LPVOID fb;			// Current op pointer
static LPVOID lastfb;		// Pointer to previous op
static DWORD fblastop;		// Id of previous op

// State lists
static int numrenderstates;
static STATEDATA renderstates[EXBUF_MAXRENDERSTATES];
static int numlightstates;
static STATEDATA lightstates[EXBUF_MAXLIGHTSTATES];
static int numtransstates;
static STATEDATA transstates[EXBUF_MAXTRANSSTATES];

TExecuteBuf::TExecuteBuf()
{
	exBuf = NULL;
	verttype = -1;
}

TExecuteBuf::TExecuteBuf(TExecuteBuf &eb) // Copy another execute buffer
{
	exBuf = NULL;
	verttype = -1;

	if (eb.exBuf == NULL)
		return;

  // Copy vert type
	verttype = eb.verttype;
	exBuf = eb.exBuf;
	memcpy(&debDesc, &eb.debDesc, sizeof(D3DEXECUTEBUFFERDESC));

  // Delete source
	eb.verttype = -1;
	eb.exBuf = NULL;
	memset(&eb.debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
}

TExecuteBuf::~TExecuteBuf()
{
	if (recording == this)
		EndRecord();
	if (exBuf)
		exBuf->Release();
}

void TExecuteBuf::BeginRecord()
{
	if (recording && recording != this)
		FatalError("Can only record 1 execute buffer at a time");
	if (recording == this)
		return;

	if (exBuf)
		exBuf->Release();
	exBuf = NULL;

  // Initialize vertex stuff
	verttype = -1;
	vertmode = 0;
	vertsize = 0;
	numverts = 0;
	CheckVertBufSize(128);	// Make sure we have some space to record to
	vb = vertbuf;			// Vertbuf pointer

  // Initialize buffers
	CheckTransBufSize(128);	// Make sure we have some space to record to
	lasttb = tb = transbuf;	// Set current pos
	tblastop = 0xFFFFFFFF;	// Set last op to crazy
	CheckFaceBufSize(128);	// Make sure we have some space to record to
	lastfb = fb = facebuf;	// Set current pos
	fblastop = 0xFFFFFFFF;	// Set last op to crazy

  // Initialize states
	numrenderstates = numlightstates = numtransstates = 0;

  // Initialize matrices
	curmatrix = 0;

	recording = this;
}

void TExecuteBuf::EndRecord()
{
	if (recording != this)  // Not recording.. no big deal..
		return;

  // Put last instructions into buffer
	FlushStates();	// Make sure all states have been flushed to buffers
	OP_EXIT(fb);	// Put exit instruction after faces

  // Kill old execute buf
	if (exBuf)
		exBuf->Release();
	exBuf = NULL;

  // Get sizes
	int vbufsize = (numverts * vertsize + 7) & 0xFFFFFFF8;
	int bufsize = (char *)tb - (char *)transbuf + (char *)fb - (char *)facebuf;

  // Make new buf
	size_t size = vbufsize + bufsize + 8; // Allow extra space for alignment
	debDesc;
	memset(&debDesc, 0, sizeof(D3DEXECUTEBUFFERDESC));
	debDesc.dwSize = sizeof(D3DEXECUTEBUFFERDESC);
	debDesc.dwFlags = D3DDEB_BUFSIZE;
	debDesc.dwBufferSize = size;
	TRY_D3D(Device->CreateExecuteBuffer(&debDesc, &exBuf, NULL));

  // Lock it so it can be filled
 	TRY_D3D(exBuf->Lock(&debDesc));

  // Grab verts 
	LPVOID lpBufStart = debDesc.lpData;
	memset(lpBufStart, 0, size);
	LPVOID lpPointer = lpBufStart;
	memcpy(lpPointer, vertbuf, numverts * vertsize);
	lpPointer = (char *)lpPointer + vbufsize;

  // Do transform and face buffers
	LPVOID lpInsStart = lpPointer;
	if (!QWORD_ALIGNED(lpPointer))		// Make sure quadword aligned
	{
		OP_NOP(lpPointer);
	}
	memcpy(lpPointer, transbuf, (char *)tb - (char *)transbuf);	// Transform buffer
	lpPointer = (char *)lpPointer + (int)((char *)tb - (char *)transbuf);
	if (!QWORD_ALIGNED(lpPointer))		// Make sure quadword aligned
	{
		OP_NOP(lpPointer);
	}
	memcpy(lpPointer, facebuf, (char *)fb - (char *)facebuf);
	lpPointer = (char *)lpPointer + (int)((char *)fb - (char *)facebuf);

  // Setup the execute data describing the buffer
	exBuf->Unlock();
	D3DEXECUTEDATA d3dExData;
	memset(&d3dExData, 0, sizeof(D3DEXECUTEDATA));
	d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
	d3dExData.dwVertexOffset = 0;
	d3dExData.dwVertexCount = numverts;
	d3dExData.dwInstructionOffset = (ULONG) ((char *)lpInsStart - (char *)lpBufStart);
	d3dExData.dwInstructionLength = (ULONG) ((char *)lpPointer  - (char *)lpInsStart);
	d3dExData.dwHVertexOffset = 0;
	exBuf->SetExecuteData(&d3dExData);

  // End recording
	recording = NULL;
	curmatrix = 0;
}

BOOL TExecuteBuf::IsRecording()
{
	return (recording == this);
}

void TExecuteBuf::Render(LPD3DCLIPSTATUS cs)
{
	if (recording || !exBuf) // Can't render while we're recording (stupid! stupid!)
		return;

  // Execute the buffer
	Device->Execute(exBuf, Viewport, D3DEXECUTE_UNCLIPPED);

  // Get bounding rectangle
	if (cs)
	{
		D3DEXECUTEDATA d3dExData;
		d3dExData.dwSize = sizeof(D3DEXECUTEDATA);
		TRY_D3D(exBuf->GetExecuteData(&d3dExData));
		cs->dwFlags = D3DCLIPSTATUS_EXTENTS2;
		cs->dwStatus = d3dExData.dsStatus.dwStatus;
		cs->minx = (float)d3dExData.dsStatus.drExtent.x1;
		cs->miny = (float)d3dExData.dsStatus.drExtent.y1;
		cs->minz = (float)0;
		cs->maxx = (float)d3dExData.dsStatus.drExtent.x2;
		cs->maxy = (float)d3dExData.dsStatus.drExtent.y2;
		cs->maxz = (float)65535;
	}
}

void TExecuteBuf::CheckVertBufSize(int size)
{
	if ((int)((DWORD)vb - (DWORD)vertbuf) + size < vertbufsize)
		return;

	BYTE *ptr = new BYTE[vertbufsize + EXBUF_BUFGROWSIZE + 16]; // Extra 16 bytes for alignment
	BYTE *buf = (BYTE *)(((DWORD)ptr + 15) & 0xFFFFFFF0); // Paragraph aligned!
	if (vertbuf)
	{
		int cursize = numverts * vertsize;
		memcpy(buf, vertbuf, cursize);
		delete vertptr;
	}
	vertptr = ptr;
	vertbuf = buf;
	vertbufsize += EXBUF_BUFGROWSIZE;
}

void TExecuteBuf::CheckTransBufSize(int size)
{
	if ((int)((DWORD)tb - (DWORD)transbuf) + size < transbufsize)
		return;

	BYTE *ptr = new BYTE[transbufsize + EXBUF_BUFGROWSIZE + 16]; // Extra 16 bytes for alignment
	BYTE *buf = (BYTE *)(((DWORD)ptr + 15) & 0xFFFFFFF0); // Paragraph aligned!
	if (transbuf)
	{
		int cursize = (char *)tb - (char *)transbuf;
		memcpy(buf, transbuf, cursize);
		delete transptr;
		tb = buf + cursize;
	}
	transptr = ptr;
	transbuf = buf;
	transbufsize += EXBUF_BUFGROWSIZE;
	if (!tb)
		tb = transbuf;
}

void TExecuteBuf::CheckFaceBufSize(int size)
{
	if ((int)((DWORD)fb - (DWORD)facebuf) + size < facebufsize)
		return;

	BYTE *ptr = new BYTE[facebufsize + EXBUF_BUFGROWSIZE + 16]; // Extra 16 bytes for alignment
	BYTE *buf = (BYTE *)(((DWORD)ptr + 15) & 0xFFFFFFF0); // Paragraph aligned!
	if (facebuf)
	{
		int cursize = (char *)fb - (char *)facebuf;
		memcpy(buf, facebuf, cursize);
		delete faceptr;
		fb = buf + cursize;
	}
	faceptr = ptr;
	facebuf = buf;
	facebufsize += EXBUF_BUFGROWSIZE;
	if (!fb)
		fb = facebuf;
}

void TExecuteBuf::Initialize()
{
  // Set initial values to 0 (just in case)
	vertbuf = vertptr = NULL;
	numverts = vertsize = vertbufsize = 0;
	transbuf = transptr = NULL;
	tb = NULL;
	transbufsize = 0;
	facebuf = faceptr = NULL;
	fb = NULL;
	facebufsize = 0;
}

void TExecuteBuf::Close()
{
	if (vertbuf)
		delete vertptr;
	vertbuf = vertptr = NULL;
	numverts = vertsize = vertbufsize = 0;
	if (transbuf)
		delete transptr;
	transbuf = transptr = NULL;
	tb = NULL;
	transbufsize = 0;
	if (facebuf)
		delete faceptr;
	facebuf = faceptr = NULL;
	fb = NULL;
	facebufsize = 0;
}

HRESULT TExecuteBuf::SetClipStatus(LPD3DCLIPSTATUS cs)
{
	if (recording != this)
		return D3DERR_NOTINBEGIN;

  // Flush out any pending state changes
	FlushStates();

  // Setup clipping status
	if (cs)
	{
		DWORD flags = 0;
		if (cs->dwFlags == D3DCLIPSTATUS_STATUS && 
			(cs->dwFlags == D3DCLIPSTATUS_EXTENTS2 || cs->dwFlags == D3DCLIPSTATUS_EXTENTS3))
			flags = D3DSETSTATUS_ALL;
		else if (cs->dwFlags == D3DCLIPSTATUS_STATUS)
			flags = D3DSETSTATUS_STATUS;
		else if (cs->dwFlags == D3DCLIPSTATUS_EXTENTS2 || cs->dwFlags == D3DCLIPSTATUS_EXTENTS3)
			flags = D3DSETSTATUS_EXTENTS;

		lasttb = tb;
		tblastop = D3DOP_SETSTATUS;
		OP_SET_STATUS(flags, cs->dwStatus, (long)cs->minx, (long)cs->miny, (long)cs->maxx, (long)cs->maxy, tb);
	}

	return DD_OK;
}

HRESULT TExecuteBuf::SetRenderState(D3DRENDERSTATETYPE rs, DWORD data)                    
{
	if (recording != this)
		return D3DERR_NOTINBEGIN;

	if (numrenderstates >= EXBUF_MAXRENDERSTATES)
		FlushStates();

	int c;
	for (c = 0; c < numrenderstates; c++)  // Never set the same render state twice!
	{
		if ((D3DRENDERSTATETYPE)renderstates[c].state == rs)
			break;
	}

	renderstates[c].state = rs;
	renderstates[c].data = data;

	if (numrenderstates < c + 1)
		numrenderstates = c + 1;

	return DD_OK;
}

HRESULT TExecuteBuf::SetLightState(D3DLIGHTSTATETYPE ls, DWORD data)
{
	if (recording != this)
		return D3DERR_NOTINBEGIN;

	if (numlightstates >= EXBUF_MAXLIGHTSTATES)
		FlushStates();

	int c;
	for (c = 0; c < numlightstates; c++)  // Never set the same light state twice!
	{
		if ((D3DLIGHTSTATETYPE)lightstates[c].state == ls)
			break;
	}

	lightstates[c].state = ls;
	lightstates[c].data = data;

	if (numlightstates < c + 1)
		numlightstates = c + 1;

	return DD_OK;
}

HRESULT TExecuteBuf::SetTransform(D3DTRANSFORMSTATETYPE ts, D3DMATRIXHANDLE matrix)
{
	if (recording != this)
		return D3DERR_NOTINBEGIN;

	if (numtransstates >= EXBUF_MAXTRANSSTATES)
		FlushStates();

	int c;
	for (c = 0; c < numtransstates; c++)  // Never set the same light state twice!
	{
		if ((D3DTRANSFORMSTATETYPE)transstates[c].state == ts)
			break;
	}

	transstates[c].state = ts;
	transstates[c].data = (DWORD)matrix;

	if (numtransstates < c + 1)
		numtransstates = c + 1;

	return DD_OK;
}

void TExecuteBuf::FlushStates()
{
  // Spew out any changes in the light state.. append to previous light state if possible
	if (numlightstates > 0)
	{
		if (tblastop == D3DOP_STATELIGHT)
		{
			((LPD3DINSTRUCTION)lasttb)->wCount += numlightstates;
		}
		else 
		{
			lasttb = tb;
			tblastop = D3DOP_STATELIGHT;
			OP_STATE_LIGHT(numlightstates, tb);
		}
		for (int c = 0; c < numlightstates; c++)
		{
		  STATE_DATA(lightstates[c].state, lightstates[c].data, tb);
		}
		numlightstates = 0;
	}

  // Spew out any changes in the transform state.. append to previous transform state if possible
	if (numtransstates > 0)
	{
		if (tblastop == D3DOP_STATETRANSFORM)
		{
			((LPD3DINSTRUCTION)lasttb)->wCount += numtransstates;
		}
		else 
		{
			lasttb = tb;
			tblastop = D3DOP_STATETRANSFORM;
			OP_STATE_TRANSFORM(numtransstates, tb);
		}
		for (int c = 0; c < numtransstates; c++)
		{
		  STATE_DATA(transstates[c].state, transstates[c].data, tb);
		}
		numtransstates = 0;
	}

  // Spew out any changes in the light state.. append to previous light state if possible
	if (numrenderstates > 0)
	{
		if (fblastop == D3DOP_STATERENDER)
		{
			((LPD3DINSTRUCTION)lastfb)->wCount += numrenderstates;
		}
		else 
		{
			lastfb = fb;
			fblastop = D3DOP_STATELIGHT;
			OP_STATE_LIGHT(numrenderstates, fb);
		}
		for (int c = 0; c < numrenderstates; c++)
		{
		  STATE_DATA(renderstates[c].state, renderstates[c].data, fb);
		}
		numrenderstates = 0;
	}
}

HRESULT TExecuteBuf::DrawIndexedPrimitive(
	D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, LPWORD i, DWORD ic, DWORD f)
{
	if (recording != this)
		return D3DERR_NOTINBEGIN;

	if (vc == 0)
		return DD_OK;

	int c;

  // Set the vertex type for this execute buffer if we don't know it yet
	if (verttype != -1 && verttype != vt)
		FatalError("Unable to create execute buffer with more than one type of vertex");
	else if (verttype == -1)
	{
		verttype = vt;
		if (verttype == D3DVT_VERTEX)
		{
			vertmode = D3DPROCESSVERTICES_TRANSFORMLIGHT | D3DPROCESSVERTICES_UPDATEEXTENTS;
			vertsize = sizeof(D3DVERTEX);
		}
		else if (verttype == D3DVT_LVERTEX)
		{
			vertmode = D3DPROCESSVERTICES_TRANSFORM | D3DPROCESSVERTICES_UPDATEEXTENTS;
			vertsize = sizeof(D3DLVERTEX);
		}
		else if (verttype == D3DVT_TLVERTEX)
		{
			vertmode = D3DPROCESSVERTICES_COPY | D3DPROCESSVERTICES_UPDATEEXTENTS;
			vertsize = sizeof(D3DTLVERTEX);
		}
	}

  // Flush all states
	FlushStates();
	
  // Get triangle indices if no index array is provided
	int startvert = numverts;
	if (!i)
		ic = vc;
	int numtri = ic / 3;

  // Spew the verts
	int size = vertsize * vc;
	CheckVertBufSize(size + 128);
	memcpy(vb, v, size);
	numverts += vc;
	vb = (char *)vb + size;

  // Spew the process vert op
	CheckTransBufSize(128);
	if (tblastop == D3DOP_PROCESSVERTICES) // Add to previous op
	{
		((LPD3DPROCESSVERTICES)((BYTE *)lasttb + sizeof(D3DINSTRUCTION)))->dwCount += vc;
	}
	else									// Make new op
	{
		lasttb = tb;
		tblastop = D3DOP_PROCESSVERTICES;
		OP_PROCESS_VERTICES(1, tb);
			PROCESSVERTICES_DATA(vertmode, startvert, vc, tb);
	}

  // Spew the triangle list op
	CheckFaceBufSize(sizeof(D3DTRIANGLE) * numtri + 128);

	if (fblastop == D3DOP_TRIANGLE)
	{
		((LPD3DINSTRUCTION)lastfb)->wCount += (WORD)numtri;
	}
	else 
	{
		if (QWORD_ALIGNED(fb))
			 OP_NOP(fb);
		lastfb = fb;
		fblastop = D3DOP_TRIANGLE;
		OP_TRIANGLE_LIST((WORD)numtri, fb);
	}

	LPD3DTRIANGLE t = (LPD3DTRIANGLE)fb;
	int vi = startvert; // Uses vert index if 'i' is NULL
	for (c = 0; c < numtri; c++, t++)
	{
		if (i)
		{
			t->v1 = *(i++) + startvert;
			t->v2 = *(i++) + startvert;
			t->v3 = *(i++) + startvert;
		}
		else
		{
			t->v1 = vi++;
			t->v2 = vi++;
			t->v3 = vi++;
		}
		t->wFlags = D3DTRIFLAG_EDGEENABLETRIANGLE;
	}
	fb = (char *)fb + (sizeof(D3DTRIANGLE) * numtri);

	return DD_OK;
}

