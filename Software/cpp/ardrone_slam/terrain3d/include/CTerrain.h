/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CTerrain.h
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Terrain class

Created :  08/11/2005
Modified : 12/05/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CTERRAIN_H
#define CTERRAIN_H

#include "stdafx.h"
#include "CWorldTransform.h"
#include "CTriangleStripPlane.h"
#include "CUtility.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CustomVertex.h"

class CTerrain : public CWorldTransform
{
public:

    CTerrain();
    ~CTerrain() { Release(); }

    BOOL Initialize( LPDIRECT3DDEVICE9 pDevice, short* map, UINT w, UINT h, byte* texture );
    void Render( LPDIRECT3DDEVICE9 pDevice);
    void Release();
	void update_elevation_map( LPDIRECT3DDEVICE9 pDevice, short* map, UINT w, UINT h, int* roi);
	void update_texture(byte* texture, int* roi);

private:
    CVertexBuffer m_vb;
    CIndexBuffer m_ib;
    LPDIRECT3DTEXTURE9 m_pTexture;
    short* m_pHeight;
    UINT m_numVertices;
    UINT m_numIndices;

	cuCustomVertex::PositionTextured* pVertices;
};

#endif