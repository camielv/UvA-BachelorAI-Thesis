#pragma once

#include "stdafx.h"
#include "CFramework.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CWorldTransform.h"
#include "CUtility.h"
#include "CTriangleStripPlane.h"
#include "CustomVertex.h"
#include "CMesh.h"
#include "CTerrain.h"
#include "CCamera.h"
#include "CArrow.h"

class terrain3d : public CBaseApp
{
public:
	terrain3d(short* map, UINT w, UINT h, byte* texture, float* arrow, float* waypoint);
	~terrain3d();
	void SetFramework( CFramework* pFramework );
	BOOL Initialize();
	virtual void Release();
    virtual void OnCreateDevice( LPDIRECT3DDEVICE9 pDevice );
    virtual void OnResetDevice( LPDIRECT3DDEVICE9 pDevice );
    virtual void OnLostDevice();
    virtual void OnDestroyDevice();
    virtual void OnUpdateFrame( LPDIRECT3DDEVICE9 pDevice, float elapsedTime );
    virtual void OnRenderFrame( LPDIRECT3DDEVICE9 pDevice, float elapsedTime );
    virtual void ProcessInput( long xDelta, long yDelta, long zDelta, BOOL* pMouseButtons, BOOL* pPressedKeys, float elapsedTime );

	void render();
	void update_elevation_map(int* roi);
	void update_texture(int* roi);
	bool requires_render();

	bool				updated;

private:
    CFramework*         m_pFramework;
    CTerrain            m_terrain;
	CArrow				m_arrow;
    CCamera             m_camera;

	short*				elevation_map;
	UINT				elevation_map_w;
	UINT				elevation_map_h;
	byte*				texture;
	float*				arrow;
	float*				waypoint;
};