/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CTerrain.cpp
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Terrain class

Created :  08/11/2005
Modified : 12/05/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "..\include\stdafx.h"
#include "..\include\CTerrain.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Default constructor.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CTerrain::CTerrain()
{
    m_pTexture = NULL;
    m_numVertices = m_numIndices = 0;

	pVertices = NULL;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Create a new Terrain object.
Parameters:
[in] pDevice - D3D Device
[in] rawFile - Name of the height map file
[in] terrainTexture - Texture file name
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CTerrain::Initialize( LPDIRECT3DDEVICE9 pDevice, short* map, UINT w, UINT h, byte* texture )
{
    Release();
  
    // Generate vertices
	m_numVertices = w * h;

    CTriangleStripPlane::GeneratePositionTexturedWithHeight( &pVertices, w, h, map );
    m_vb.CreateBuffer( pDevice, m_numVertices, D3DFVF_XYZ | D3DFVF_TEX1, sizeof( cuCustomVertex::PositionTextured ) );
    m_vb.SetData( m_numVertices, pVertices, 0, h);

    // Generate indices
    int* pIndices = NULL;
    m_numIndices = CTriangleStripPlane::GenerateIndices( &pIndices, w, h );
    m_ib.CreateBuffer( pDevice, m_numIndices, D3DFMT_INDEX32 );
    m_ib.SetData( m_numIndices, pIndices, 0 );
    m_vb.SetIndexBuffer( &m_ib );

	if ( FAILED( D3DXCreateTexture( pDevice, 4096, 4096, 1, D3DUSAGE_DYNAMIC /*| D3DUSAGE_WRITEONLY*/, D3DFMT_R8G8B8, D3DPOOL_DEFAULT, &m_pTexture ) ) )
    {
        SHOWERROR( "Unable to create terrain texture.", __FILE__, __LINE__ );
        return FALSE;
    }

	// init texture
	D3DLOCKED_RECT m_rectLocked;
	m_pTexture->LockRect(0,&m_rectLocked,NULL,0);
	byte* pByte = (byte*) m_rectLocked.pBits;
	memcpy_s(pByte, 4096 * 4096 * 4, texture, 4096 * 4096 * 4);
	m_pTexture->UnlockRect(0);

    return TRUE;
}

void CTerrain::update_elevation_map( LPDIRECT3DDEVICE9 pDevice, short* map, UINT w, UINT h, int* roi)
{
    // Generate vertices
	m_numVertices = w * h;

    CTriangleStripPlane::UpdatePositionTexturedWithHeight( &pVertices, w, h, map, roi );
    m_vb.SetData( m_numVertices, pVertices, 0, h, roi );

	// no need to update indices
}

void CTerrain::update_texture(byte* texture, int* roi)
{
	D3DLOCKED_RECT m_rectLocked;

	int w = roi[1] - roi[0];
	int h = roi[3] - roi[2];

	RECT r = {roi[0], roi[2], roi[1], roi[3]};

	m_pTexture->LockRect(0,&m_rectLocked,&r,0);

	byte* pByte = (byte*) m_rectLocked.pBits;

	for (int line = 0; line < h; line++)
		memcpy((char*) &pByte[(line * 4096 * 4)], (char*) &texture[((r.top + line) * 4096 * 4) + (r.left * 4)], w * 4);

	m_pTexture->UnlockRect(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Renders the terrain.
Parameters:
[in] pDevice - D3D Device
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CTerrain::Render( LPDIRECT3DDEVICE9 pDevice )
{
    pDevice->SetTransform( D3DTS_WORLD, GetTransform() );
    pDevice->SetTexture( 0, m_pTexture );
    m_vb.Render( pDevice, m_numIndices - 2, D3DPT_TRIANGLESTRIP );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Release resources
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CTerrain::Release()
{
    SAFE_RELEASE( m_pTexture );
    m_vb.Release();
    m_ib.Release();
}