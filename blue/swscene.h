/*************************************************************
	SwScene.h
		Headers of 3D Scene module wrapping Blue software 
		renderer for Cinematix Revenant

	Created in 98/10,  Young-Hyun Joo
**************************************************************/

#ifndef __SW_3D_SCENE_H__
#define __SW_3D_SCENE_H__

#include <ddraw.h>
#include <d3d.h>

class T3DSwScene
{
  protected:
	BOOL initialized;								// 3DSwScene is initialized

  public:
    T3DSwScene();
	~T3DSwScene();

	/***************************************************
	 *
	 *  Replicas of T3DScene methods
	 *
	 *  - Each method is functionally equivalent to 
	 *    that of T3DScene.
	 *  - Typically, these are called by corresponding 
	 *    T3DScene methods in some ways.
	 *
	 ***************************************************/

	BOOL Initialize();
	BOOL SetSize(int x, int y, int width, int height);
	BOOL InitializeMatrices();
	BOOL Close();

	void SetCameraPos(RS3DPoint pos, int zdist);
	BOOL UpdateCamera();
	BOOL DrawScene();

	void GetClosestTextureFormat(LPDDSURFACEDESC srcsd, LPDDSURFACEDESC dstsd);

	HRESULT BeginScene();
	HRESULT EndScene();
	//2>>>>>>>>>>>>>>
	HRESULT SetClipStatus(LPD3DCLIPSTATUS cs);
	HRESULT GetClipStatus(LPD3DCLIPSTATUS cs);
	//2<<<<<<<<<<<<<<
	HRESULT SetRenderState(D3DRENDERSTATETYPE rs, DWORD data);
	HRESULT GetRenderState(D3DRENDERSTATETYPE rs, DWORD *data);
	HRESULT SetTransform(D3DTRANSFORMSTATETYPE ts, LPD3DMATRIX matrix);
	HRESULT GetTransform(D3DTRANSFORMSTATETYPE ts, LPD3DMATRIX matrix);
	HRESULT DrawIndexedPrimitive(
		D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, LPWORD i, DWORD ic, DWORD f);
	HRESULT DrawPrimitive(
		D3DPRIMITIVETYPE pt, D3DVERTEXTYPE vt, LPVOID v, DWORD vc, DWORD f);

	HRESULT BeginRender();
	HRESULT EndRender();

	/***************************************************
	 *
	 *  Additional methods of T3DSwScene
	 *
	 ***************************************************/
	
	// This method is called by T3DScene::BeginScene to set ambient light
	void SetAmbientLightColor(DWORD data);	

	// This method is called by T3DScene::SetRenderState to set active
	// texture handle & SURFACE INTERFACE
	void SetTexture( DWORD hTexture, LPDIRECTDRAWSURFACE surface );

	// These methods are called by T3DLight::LightOn/Off to 
	// activate/deactivate lights.
	void TurnOnLight( LPD3DLIGHT2 lightdata );
	void TurnOffLight( LPD3DLIGHT2 lightdata );
};

#endif
