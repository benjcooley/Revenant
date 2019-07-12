/*************************************************************
	SwScene.cpp
		3D Scene module wrapping Blue software renderer 
		for Cinematix Revenant

	Created in 98/10,  Young-Hyun Joo
**************************************************************/

#include <windows.h>
#include <ddraw.h>
#include <math.h>

#include "revenant.h"
#include "display.h"

#include "swscene.h"
#include "d3dmath.h"
#include "directdraw.h"

#include "3struct.h"
#include "3gcolort.h"
#include "3g4444t.h"
#include "3gdiv.h"
#include "3cameraf.h"

#include <vector>
#include <assert.h>

static C3DCamera*	_mnEngine = NULL;
static bool			_SwDevInited = false;

static bool			_bRGB555 = false;

static D3DMATRIX	_ViewMat, 
					_ProjMat, 
					_WorldMat;

static D3DMATRIX	_TransMat;
static bool			_TransMatValid = false;

static int			_scrL, _scrR, _scrT, _scrB;

LPDIRECTDRAWSURFACE	_ITexSurface = NULL;
DWORD				_hTexture = 0;
bool				_TexARGB = false;

DWORD				_cullMode = D3DCULL_NONE;
bool				_zWriteEnable = true;
bool				_zEnable = true;
DWORD				_textureMapBlend;
DWORD				_srcBlend;
DWORD				_destBlend;
//2>>>>>>>>>>>>>>>>>>>
D3DCLIPSTATUS		_clipStatus;
//2<<<<<<<<<<<<<<<<<<<

float _AmbientR = 0, _AmbientG = 0, _AmbientB = 0;

//------- for debugging ----------
static short*		_vidmem;
static short*		_zBuffer;
//--------------------------------

T3DSwScene::T3DSwScene()
{
	assert(!_SwDevInited);
	_mnEngine = NULL;
}

T3DSwScene::~T3DSwScene()
{
	if (_mnEngine) {
		delete _mnEngine;
		_mnEngine = NULL;
	}
	if (_ITexSurface) {
		_ITexSurface->Release();
	}
}

BOOL T3DSwScene::Initialize()
  // Sets up viewport, ambient light, etc. etc.
{
	_mnEngine = new C3DCamera();

	_bRGB555 = Display->BackBuffer()->BitsPerPixel() == 15;
	int colCount = _bRGB555 ? 32768 : 65536; 
	int colShift = _bRGB555 ? 15 : 16;
	int colMask = _bRGB555 ? 0xffff8000 : 0xffff0000;

	if (!_SwDevInited) {
		colortable.MakeTable(colCount);	
		divtable.MakeTable(colCount);
		argb4444table.MakeTable(colCount);

		_SwDevInited = true;
	}

	_mnEngine->SetDivTable( divtable.pm_divtable );
	_mnEngine->SetAlphaTable( colortable.pm_colortable, colMask, colShift );
	_mnEngine->Set4444Table( argb4444table.pm_table );

	return TRUE;
}

BOOL T3DSwScene::SetSize(int x, int y, int width, int height)
  // Sets the 3D Vieport size
{
	_scrL = x, _scrT = y, _scrR = x+width, _scrB = y+height;

	return TRUE;
}

BOOL T3DSwScene::InitializeMatrices()
  // Initialize view matrices
{
	return TRUE;
}

BOOL T3DSwScene::Close()
  // Closes scene, removes viewport, etc.
{
	return TRUE;
}

void T3DSwScene::SetCameraPos(RS3DPoint pos, int zdist)
  // Sets Camera Position
{
	D3DVECTOR v1;//, v2;
	
	D3DMATRIXClear(&_ViewMat);

	v1.x = D3DVAL(pos.x);
	v1.y = D3DVAL(pos.y);
	v1.z = D3DVAL(pos.z);
	D3DMATRIXMove(&_ViewMat, &v1); 

	D3DMATRIXRotateZ(&_ViewMat, D3DVAL(45.0 * TORADIAN));
	D3DMATRIXRotateX(&_ViewMat, D3DVAL(-(90.0 + CAMERAANGLE) * TORADIAN));

	v1.x = D3DVAL(0.0);
	v1.y = D3DVAL(0.0);
	v1.z = D3DVAL(-zdist);
	D3DMATRIXMove(&_ViewMat, &v1); 

	_TransMatValid = false;
}

BOOL T3DSwScene::DrawScene()
  //  Draws all objects using Direct 3D
{
	return TRUE;
}

static std::vector<LPD3DLIGHT2> _pointLightList;
static std::vector<LPD3DLIGHT2> _dirLightList;

void T3DSwScene::TurnOnLight( LPD3DLIGHT2 lightdata )
{
	if (lightdata->dltType == D3DLIGHT_POINT) {

		std::vector<LPD3DLIGHT2>::iterator it;
		for (it = _pointLightList.begin(); it != _pointLightList.end(); it++) {
			if (lightdata == *it)
				return;
		}
		_pointLightList.push_back(lightdata);

	} else if (lightdata->dltType == D3DLIGHT_DIRECTIONAL) {

		std::vector<LPD3DLIGHT2>::iterator it;
		for (it = _dirLightList.begin(); it != _dirLightList.end(); it++) {
			if (lightdata == *it)
				return;
		}
		_dirLightList.push_back(lightdata);
	}
}

void T3DSwScene::TurnOffLight( LPD3DLIGHT2 lightdata )
{
	if (lightdata->dltType == D3DLIGHT_POINT) {

		std::vector<LPD3DLIGHT2>::iterator it;
		for (it = _pointLightList.begin(); it != _pointLightList.end(); it++) {
			if (lightdata == *it) {
				_pointLightList.erase(it);
				return;
			}
		}

	} else if (lightdata->dltType == D3DLIGHT_DIRECTIONAL) {

		std::vector<LPD3DLIGHT2>::iterator it;
		for (it = _dirLightList.begin(); it != _dirLightList.end(); it++) {
			if (lightdata == *it) {
				_dirLightList.erase(it);
				return;
			}
		}
	}
}

void T3DSwScene::GetClosestTextureFormat(LPDDSURFACEDESC srcsd, LPDDSURFACEDESC dstsd)
{
	memcpy(dstsd, srcsd, sizeof(DDSURFACEDESC));

	//2>>>>>>>>>>>>>>>>>>>>
	if ( srcsd->ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS ) {
		dstsd->ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_ALPHAPIXELS;
		dstsd->ddpfPixelFormat.dwRGBBitCount = 16;
		dstsd->ddpfPixelFormat.dwRBitMask = 0x0f00;
		dstsd->ddpfPixelFormat.dwGBitMask = 0x00f0;
		dstsd->ddpfPixelFormat.dwBBitMask = 0x000f;
		dstsd->ddpfPixelFormat.dwRGBAlphaBitMask = 0xf000;
	} else {
		if (_bRGB555) {
			dstsd->ddpfPixelFormat.dwFlags = DDPF_RGB;
			dstsd->ddpfPixelFormat.dwRGBBitCount = 16;
			dstsd->ddpfPixelFormat.dwRBitMask = 0x7c00;
			dstsd->ddpfPixelFormat.dwGBitMask = 0x03e0;
			dstsd->ddpfPixelFormat.dwBBitMask = 0x001f;
			dstsd->ddpfPixelFormat.dwRGBAlphaBitMask = 0;
		} else {
			dstsd->ddpfPixelFormat.dwFlags = DDPF_RGB;
			dstsd->ddpfPixelFormat.dwRGBBitCount = 16;
			dstsd->ddpfPixelFormat.dwRBitMask = 0xf800;
			dstsd->ddpfPixelFormat.dwGBitMask = 0x07e0;
			dstsd->ddpfPixelFormat.dwBBitMask = 0x001f;
			dstsd->ddpfPixelFormat.dwRGBAlphaBitMask = 0;
		}
	}
	//2<<<<<<<<<<<<<<<<<<<
}

// Duplicate D3D Functions (Can only be called between BeginRender() and EndRender())
HRESULT T3DSwScene::BeginScene()
{
	//2>>>>>>>>>>>>>>>
	_clipStatus.dwFlags = D3DCLIPSTATUS_EXTENTS2;
	_clipStatus.minx = _clipStatus.miny = +9999.0f;
	_clipStatus.maxx = _clipStatus.maxy = -9999.0f;
	_clipStatus.minz = 0;
	_clipStatus.maxz = 65535.0f;
	//2<<<<<<<<<<<<<<<

	return DD_OK;
}

HRESULT T3DSwScene::EndScene()
{
	return DD_OK;
}

//2>>>>>>>>>>>>>>>>>>>>>
HRESULT T3DSwScene::SetClipStatus(LPD3DCLIPSTATUS cs)
{
	_clipStatus = *cs;
	return DD_OK;
}

HRESULT T3DSwScene::GetClipStatus(LPD3DCLIPSTATUS cs)
{
	*cs = _clipStatus;
	return DD_OK;
}
//2<<<<<<<<<<<<<<<<<<<<<

void T3DSwScene::SetTexture( DWORD hTexture, LPDIRECTDRAWSURFACE surface )
{
	_hTexture = hTexture;

	if ( _ITexSurface != NULL ) _ITexSurface->Release();
	_ITexSurface = surface;
	if ( _ITexSurface != NULL ) _ITexSurface->AddRef();

	DDPIXELFORMAT pf;
	pf.dwSize = sizeof(DDPIXELFORMAT);
	if ( _ITexSurface != NULL)
	{
	  _ITexSurface->GetPixelFormat( &pf );
	  _TexARGB = pf.dwRGBAlphaBitMask != 0;
	}
}

HRESULT T3DSwScene::SetRenderState(D3DRENDERSTATETYPE rs, DWORD data)
{
	switch (rs) {
		case D3DRENDERSTATE_ZENABLE:
			_zEnable = data != 0;
			break;

		case D3DRENDERSTATE_SHADEMODE:
			break;

		case D3DRENDERSTATE_DITHERENABLE:
			break;

		case D3DRENDERSTATE_ALPHABLENDENABLE:
			break;

		case D3DRENDERSTATE_TEXTUREMAPBLEND:
			_textureMapBlend = data;
			break;

		case D3DRENDERSTATE_SRCBLEND:
			_srcBlend = data;
			break;

		case D3DRENDERSTATE_DESTBLEND:
			_destBlend = data;
			break;

		case D3DRENDERSTATE_ZWRITEENABLE:
			_zWriteEnable = data != 0;
			break;

		case D3DRENDERSTATE_CULLMODE:
			_cullMode = data;
			break;

		case D3DRENDERSTATE_FILLMODE:
			break;
		
		default:
			break;
	}

	return DD_OK;
}

HRESULT T3DSwScene::GetRenderState(D3DRENDERSTATETYPE rs, DWORD *data)
{
	switch (rs) {
		case D3DRENDERSTATE_TEXTUREHANDLE:
			*data = _hTexture;
			break;

		case D3DRENDERSTATE_ZENABLE:
			*data = _zEnable;
			break;

		case D3DRENDERSTATE_SHADEMODE:
			break;

		case D3DRENDERSTATE_DITHERENABLE:
			break;

		case D3DRENDERSTATE_ALPHABLENDENABLE:
			break;

		case D3DRENDERSTATE_TEXTUREMAPBLEND:
			*data = _textureMapBlend;
			break;

		case D3DRENDERSTATE_SRCBLEND:
			*data = _srcBlend;
			break;

		case D3DRENDERSTATE_DESTBLEND:
			*data = _destBlend;
			break;

		case D3DRENDERSTATE_ZWRITEENABLE:
			*data = _zWriteEnable;
			break;

		case D3DRENDERSTATE_CULLMODE:
			*data = _cullMode;
			break;

		case D3DRENDERSTATE_FILLMODE:
			break;

		default: break;
	}
	return DD_OK;
}

void T3DSwScene::SetAmbientLightColor(DWORD data)
{
	_AmbientR = (( data & 0xff0000 ) >> 16) / 255.0f;
	_AmbientG = (( data & 0x00ff00 ) >> 8) / 255.0f;
	_AmbientB = ( data & 0x0000ff ) / 255.0f;
}

HRESULT T3DSwScene::SetTransform(D3DTRANSFORMSTATETYPE ts, LPD3DMATRIX matrix)
{
	if (ts == D3DTRANSFORMSTATE_VIEW)
		_ViewMat = *matrix;
	else if (ts == D3DTRANSFORMSTATE_PROJECTION) {
		_ProjMat = *matrix;
		_ProjMat._33 *= 65536.0f;
		_ProjMat._11 *= 65536.0f * _ProjMat._33;
		_ProjMat._22 *= 65536.0f * _ProjMat._33;
	} else if (ts == D3DTRANSFORMSTATE_WORLD)
		_WorldMat = *matrix;

	_TransMatValid = false;

	return DD_OK;
}

HRESULT T3DSwScene::GetTransform(D3DTRANSFORMSTATETYPE ts, LPD3DMATRIX matrix)
{
	if (ts == D3DTRANSFORMSTATE_VIEW)
		*matrix = _ViewMat;
	else if (ts == D3DTRANSFORMSTATE_PROJECTION) {
		*matrix = _ProjMat;
		matrix->_11 /= 65536.0f * matrix->_33;
		matrix->_22 /= 65536.0f * matrix->_33;
		matrix->_33 /= 65536.0f;
	} else if (ts == D3DTRANSFORMSTATE_WORLD)
		*matrix = _WorldMat;

	return DD_OK;
}

inline void UpdateTransMat()
{
	D3DMATRIX t;
	MultiplyD3DMATRIX( &t, &_WorldMat, &_ViewMat );
	MultiplyD3DMATRIX( &_TransMat, &t, &_ProjMat );
	
	_TransMatValid = true;
}

inline void Transform(const D3DMATRIX& mat, const D3DVERTEX& v, D3DLVERTEX& res)
{
	res.x = v.x * mat._11 + v.y * mat._21 + v.z * mat._31 + mat._41;
	res.y = v.x * mat._12 + v.y * mat._22 + v.z * mat._32 + mat._42;
	res.z = v.x * mat._13 + v.y * mat._23 + v.z * mat._33 + mat._43;
}

inline void Illuminate(const D3DMATRIX& worldmat, const D3DVERTEX& v, D3DLVERTEX& res )
{
	float sumR = _AmbientR, sumG = _AmbientG, sumB = _AmbientB;
	std::vector<LPD3DLIGHT2>::iterator it;

	float wnx = v.nx * worldmat._11 + v.ny * worldmat._21 + v.nz * worldmat._31;
	float wny = v.nx * worldmat._12 + v.ny * worldmat._22 + v.nz * worldmat._32;
	float wnz = v.nx * worldmat._13 + v.ny * worldmat._23 + v.nz * worldmat._33;

	float len = sqrt(wnx*wnx+wny*wny+wnz*wnz);
	wnx /= len, wny /= len, wnz /= len;

	for (it = _dirLightList.begin(); it != _dirLightList.end(); it++) {

		float vx = -(*it)->dvDirection.x, 
			  vy = -(*it)->dvDirection.y, 
			  vz = -(*it)->dvDirection.z;

		float f = wnx*vx + wny*vy + wnz*vz;
		if (f <= 0.0f) continue;

		assert(f < 1.1f);

		sumR += f * (*it)->dcvColor.r;
		sumG += f * (*it)->dcvColor.g;
		sumB += f * (*it)->dcvColor.b;
	}

	float wx = v.x * worldmat._11 + v.y * worldmat._21 + v.z * worldmat._31 + worldmat._41;
	float wy = v.x * worldmat._12 + v.y * worldmat._22 + v.z * worldmat._32 + worldmat._42;
	float wz = v.x * worldmat._13 + v.y * worldmat._23 + v.z * worldmat._33 + worldmat._43;

	for (it = _pointLightList.begin(); it != _pointLightList.end(); it++) {

		float vx = (*it)->dvPosition.x - wx, 
			  vy = (*it)->dvPosition.y - wy, 
			  vz = (*it)->dvPosition.z - wz;

		float d = float( sqrt( vx*vx + vy*vy + vz*vz ) );

		if ( d > (*it)->dvRange ) continue;

		float f = wnx*vx + wny*vy + wnz*vz;
		if (f <= 0.0f) continue;

		f /= d;
		sumR += f * (*it)->dcvColor.r;
		sumG += f * (*it)->dcvColor.g;
		sumB += f * (*it)->dcvColor.b;
	}

	if (_bRGB555) {
		res.color = (sumR>1.0f ? 31<<10 : int(sumR*31.0f) << 10) 
			      | (sumG>1.0f ? 31<<5	: int(sumG*31.0f) << 5) 
			      | (sumB>1.0f ? 31		: int(sumB*31.0f)); 
	} else {
		res.color = (sumR>1.0f ? 31<<11 : int(sumR*31.0f) << 11)
			      | (sumG>1.0f ? 31<<6	: int(sumG*31.0f) << 6)
			      | (sumB>1.0f ? 31		: int(sumB*31.0f));
	}
}

//2>>>>>>>>>>>>>>>>
void UpdateExtents( D3DCLIPSTATUS& clipStatus, const D3DLVERTEX* tvtxs, int tvtxCount )
{
	float _scrCX = (_scrL+_scrR)/2;
	float _scrCY = (_scrT+_scrB)/2;

	for (; tvtxCount--; tvtxs++) {
		float x = tvtxs->x + _scrCX;
		float y = -tvtxs->y + _scrCY;
		if (x < clipStatus.minx)
			clipStatus.minx = x;
		if (x > clipStatus.maxx)
			clipStatus.maxx = x;
		if (y < clipStatus.miny)
			clipStatus.miny = y;
		if (y > clipStatus.maxy)
			clipStatus.maxy = y;
	}

	clipStatus.minx = max(min(clipStatus.minx, _scrR), _scrL);
	clipStatus.maxx = max(min(clipStatus.maxx, _scrR), _scrL);
	clipStatus.miny = max(min(clipStatus.miny, _scrB), _scrT);
	clipStatus.maxy = max(min(clipStatus.maxy, _scrB), _scrT);
}
//2<<<<<<<<<<<<<<<<

static inline float _abs(float a) { return a > 0 ? a : -a; }

static inline int _max3( int a, int b, int c ) {
	if (a>b) return a>c ? a: c;
	else return b>c ? b: c;
}

static std::vector<D3DLVERTEX> _ptv;

inline int _log2( int pitch ) {
	switch (pitch) {
		case 4:
			return 1;
		case 8:
			return 2;
		case 16:
			return 3;
		case 32:
			return 4;
		case 64:
			return 5;
		case 128:
			return 6;
		case 256:
			return 7;
		case 512:
			return 8;
		default:
			return 0;
	}
}

HRESULT T3DSwScene::DrawIndexedPrimitive(
	D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, LPWORD i, DWORD ic, DWORD f)
{
	if (!_TransMatValid) UpdateTransMat();

	_ptv.resize(vc);
	D3DVERTEX* pv = reinterpret_cast<D3DVERTEX*>(v);

	// transform and lighting
	unsigned int k;
	for (k = 0; k < vc; k++) {
		Transform( _TransMat, pv[k], _ptv[k] );
		Illuminate( _WorldMat, pv[k], _ptv[k] );
	}

	//2>>>>>>>>>>>>>>>>>>>>
	UpdateExtents( _clipStatus, &_ptv.front(), vc );
	//2<<<<<<<<<<<<<<<<<<<<

	// rasterizing
	if (_hTexture == NULL) {
		// no texture - lighting only

		for (k = 0; k < ic; k+=3) {
			unsigned int i1 = i[k], i2 = i[k+1], i3 = i[k+2];

			if (_abs(_ptv[i1].x-_ptv[i2].x)>640 || _abs(_ptv[i1].x-_ptv[i3].x)>640 
				|| _abs(_ptv[i1].y-_ptv[i2].y)>480 || _abs(_ptv[i1].y-_ptv[i3].y)>480) 
				continue;

			if ((_ptv[i2].x - _ptv[i1].x)*(_ptv[i3].y - _ptv[i1].y)	
				> (_ptv[i3].x - _ptv[i1].x)*(_ptv[i2].y - _ptv[i1].y)) continue;

			sAGU_GORAUD arg;
			
			arg.v1.x = _ptv[i1].x, arg.v1.y = -_ptv[i1].y;
			arg.v2.x = _ptv[i2].x, arg.v2.y = -_ptv[i2].y;
			arg.v3.x = _ptv[i3].x, arg.v3.y = -_ptv[i3].y;

			arg.v1.c = _ptv[i1].color;
			arg.v2.c = _ptv[i2].color;
			arg.v3.c = _ptv[i3].color;

			_mnEngine->DrawGoraudZbuffer( arg, _max3(_ptv[i1].z,_ptv[i2].z,_ptv[i3].z) );
		}
		return DD_OK;
	}

	// texture mapping

	DDSURFACEDESC ddsd;
	ddsd.dwSize = sizeof(ddsd);

	TRY_D3D(_ITexSurface->Lock( NULL, &ddsd, NULL, NULL ));

	int texLogPitch = _log2(ddsd.lPitch);
	float texWidth = ddsd.dwWidth, texHeight = ddsd.dwHeight;

	if (_TexARGB) {
		// ARGB4444 texture format - no lighting 

		for (k = 0; k < ic; k+=3) {
			unsigned int i1 = i[k], i2 = i[k+1], i3 = i[k+2];

			if ( _abs(_ptv[i1].x-_ptv[i2].x)>640 || _abs(_ptv[i1].x-_ptv[i3].x)>640 
				|| _abs(_ptv[i1].y-_ptv[i2].y)>480 || _abs(_ptv[i1].y-_ptv[i3].y)>480 ) 
			{
				continue;
			}

			if ((_ptv[i2].x - _ptv[i1].x)*(_ptv[i3].y - _ptv[i1].y)	
				> (_ptv[i3].x - _ptv[i1].x)*(_ptv[i2].y - _ptv[i1].y)) continue;

			sAGU_TEXTURE arg;

			arg.v1.x = _ptv[i1].x, arg.v1.y = -_ptv[i1].y;
			arg.v2.x = _ptv[i2].x, arg.v2.y = -_ptv[i2].y;
			arg.v3.x = _ptv[i3].x, arg.v3.y = -_ptv[i3].y;

			arg.v1.u = pv[i1].tu * texWidth, arg.v1.v = pv[i1].tv * texHeight;
			arg.v2.u = pv[i2].tu * texWidth, arg.v2.v = pv[i2].tv * texHeight;
			arg.v3.u = pv[i3].tu * texWidth, arg.v3.v = pv[i3].tv * texHeight;

			if (_zEnable) {
				if (_zWriteEnable) 
					_mnEngine->DrawTextureZbuffer4444( 
						arg, (short*)ddsd.lpSurface, 
						texLogPitch, ddsd.dwWidth-1, ddsd.dwHeight-1,
						(_ptv[i1].z + _ptv[i2].z + _ptv[i3].z)/3, 
						_max3(_ptv[i1].z,_ptv[i2].z,_ptv[i3].z) 
					);
				else
					_mnEngine->DrawTextureZbuffer4444WOff( 
						arg, (short*)ddsd.lpSurface, 
						texLogPitch, ddsd.dwWidth-1, ddsd.dwHeight-1,
						(_ptv[i1].z + _ptv[i2].z + _ptv[i3].z)/3
					);
			} else {
				if (_zWriteEnable) 
					_mnEngine->DrawTextureZbuffer4444ROff( 
						arg, (short*)ddsd.lpSurface, 
						texLogPitch, ddsd.dwWidth-1, ddsd.dwHeight-1,
						_max3(_ptv[i1].z,_ptv[i2].z,_ptv[i3].z) 
					);
				else
					_mnEngine->DrawTexture4444( 
						arg, (short*)ddsd.lpSurface, 
						texLogPitch, ddsd.dwWidth-1, ddsd.dwHeight-1
					);
			}
		}

	} else {
		// RGB565 texture - modulation with lighting

		for (k = 0; k < ic; k+=3) {
			unsigned int i1 = i[k], i2 = i[k+1], i3 = i[k+2];

			if ((_ptv[i2].x - _ptv[i1].x)*(_ptv[i3].y - _ptv[i1].y)	
				> (_ptv[i3].x - _ptv[i1].x)*(_ptv[i2].y - _ptv[i1].y)) continue;

			if (_abs(_ptv[i1].x-_ptv[i2].x)>640 || _abs(_ptv[i1].x-_ptv[i3].x)>640 
				|| _abs(_ptv[i1].y-_ptv[i2].y)>480 || _abs(_ptv[i1].y-_ptv[i3].y)>480) 
			{
				continue;
			}

			sAGU_TEXTUREANDGORAUD arg;

			arg.v1.x = _ptv[i1].x, arg.v1.y = -_ptv[i1].y;
			arg.v2.x = _ptv[i2].x, arg.v2.y = -_ptv[i2].y;
			arg.v3.x = _ptv[i3].x, arg.v3.y = -_ptv[i3].y;

			arg.v1.c = _ptv[i1].color;
			arg.v2.c = _ptv[i2].color;
			arg.v3.c = _ptv[i3].color;

			arg.v1.u = pv[i1].tu * texWidth, arg.v1.v = pv[i1].tv * texHeight;
			arg.v2.u = pv[i2].tu * texWidth, arg.v2.v = pv[i2].tv * texHeight;
			arg.v3.u = pv[i3].tu * texWidth, arg.v3.v = pv[i3].tv * texHeight;

			if (_zEnable) {
				if (_zWriteEnable)
					_mnEngine->DrawTextureAndModulationGoraudZbuffer( 
						arg, (short*)ddsd.lpSurface, 
						texLogPitch, ddsd.dwWidth-1, ddsd.dwHeight-1,
						_max3(_ptv[i1].z,_ptv[i2].z,_ptv[i3].z) 
					);
				else
					_mnEngine->DrawTextureAndModulationGoraudZbufferWOff( 
						arg, (short*)ddsd.lpSurface, 
						texLogPitch, ddsd.dwWidth-1, ddsd.dwHeight-1,
						_max3(_ptv[i1].z,_ptv[i2].z,_ptv[i3].z) 
					);
			} else {
				if (_zWriteEnable)
					_mnEngine->DrawTextureAndModulationGoraudZbufferROff( 
						arg, (short*)ddsd.lpSurface, 
						texLogPitch, ddsd.dwWidth-1, ddsd.dwHeight-1,
						_max3(_ptv[i1].z,_ptv[i2].z,_ptv[i3].z) 
					);
				else
					_mnEngine->DrawTextureAndModulationGoraud( 
						arg, (short*)ddsd.lpSurface, 
						texLogPitch, ddsd.dwWidth-1, ddsd.dwHeight-1
					);
			}
		}
	}

	TRY_D3D(_ITexSurface->Unlock(NULL));

	return DD_OK;
}

HRESULT T3DSwScene::DrawPrimitive(
	D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, DWORD f)
{
	if (!_TransMatValid) UpdateTransMat();

	_ptv.resize(vc);
	D3DVERTEX* pv = reinterpret_cast<D3DVERTEX*>(v);

	unsigned int k;
	for (k = 0; k < vc; k++) {
		Transform( _TransMat, pv[k], _ptv[k] );
		Illuminate( _WorldMat, pv[k], _ptv[k] );
	}

	//2>>>>>>>>>>>>>>>>>>>>
	UpdateExtents( _clipStatus, &_ptv.front(), vc );
	//2<<<<<<<<<<<<<<<<<<<<

	sAGU_GORAUD arg;
	for (k = 0; k < vc; k+=3) {
		unsigned int i1 = k, i2 = k+1, i3 = k+2;

		arg.v1.x = _ptv[i1].x, arg.v1.y = -_ptv[i1].y;
		arg.v2.x = _ptv[i2].x, arg.v2.y = -_ptv[i2].y;
		arg.v3.x = _ptv[i3].x, arg.v3.y = -_ptv[i3].y;

		if (_abs(_ptv[i1].x-_ptv[i2].x)>640 || _abs(_ptv[i1].x-_ptv[i3].x)>640 
			|| _abs(_ptv[i1].y-_ptv[i2].y)>480 || _abs(_ptv[i1].y-_ptv[i3].y)>480) 
			continue;
		
		arg.v1.c = _ptv[i1].color;
		arg.v2.c = _ptv[i2].color;
		arg.v3.c = _ptv[i3].color;

		_mnEngine->DrawGoraudZbuffer( arg, (_ptv[i1].z+_ptv[i2].z+_ptv[i3].z)/3 );
	}

	return DD_OK;
}

HRESULT T3DSwScene::BeginRender()
{
	PTSurface backBuffer = Display->BackBuffer();
	PTSurface zBuffer = Display->GetZBuffer();

	_vidmem = reinterpret_cast<short*>(backBuffer->Lock());
	_zBuffer = reinterpret_cast<short*>(zBuffer->Lock());

	_mnEngine->SetVideoBuffer( _vidmem, _zBuffer ); 
	_mnEngine->SetStartEnd( _scrL, _scrR-1, _scrT, _scrB-1, backBuffer->Stride() );
	_mnEngine->SetScreenCenter( (_scrL+_scrR)/2, (_scrT+_scrB)/2 );

	return DD_OK;
}

HRESULT T3DSwScene::EndRender()
{
	PTSurface backBuffer = Display->BackBuffer();
	PTSurface zBuffer = Display->GetZBuffer();
	backBuffer->Unlock();
	zBuffer->Unlock();

	return DD_OK;
}
