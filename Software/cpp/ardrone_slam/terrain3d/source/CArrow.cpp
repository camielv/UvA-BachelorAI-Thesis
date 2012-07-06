/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CTerrain.cpp
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Terrain class

Created :  08/11/2005
Modified : 12/05/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "..\include\stdafx.h"
#include "..\include\CArrow.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Default constructor.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CArrow::CArrow()
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Create a new Terrain object.
Parameters:
[in] pDevice - D3D Device
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CArrow::Initialize( LPDIRECT3DDEVICE9 pDevice, float* pos )
{
	this->pos = pos;


    Release();

	UINT nr_vertices = 18;

    if ( FAILED( pDevice->CreateVertexBuffer( nr_vertices * sizeof(Vertex), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &m_vb, NULL ) ) )
    {
        SHOWERROR( "CreateVertexBuffer failed.", __FILE__, __LINE__ );
        return FALSE;
    }

	Vertex *pVertices = NULL;

    // Lock the vertex buffer
    if ( FAILED( m_vb->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
    {
        SHOWERROR( "IDirect3DVertexBuffer9::Lock failed.", __FILE__, __LINE__ );
        return FALSE;
    }


	D3DCOLOR dark = D3DCOLOR_COLORVALUE( 0.4, 0.0, 0.0, 1.0 );
	D3DCOLOR normal = D3DCOLOR_COLORVALUE( 0.6, 0.0, 0.0, 1.0 );
	D3DCOLOR bright = D3DCOLOR_COLORVALUE( 1.0, 0.0, 0.0, 1.0 );

    pVertices[0].x = -0.5f;
    pVertices[0].y = -0.5f;
    pVertices[0].z =  0.0f;
    pVertices[0].color = dark;

    pVertices[1].x = -0.5f;
    pVertices[1].y =  0.5f;
    pVertices[1].z =  0.0f;
    pVertices[1].color = dark;

    pVertices[2].x =  0.5f;
    pVertices[2].y = -0.5f;
    pVertices[2].z =  0.0f;
    pVertices[2].color = dark;


    pVertices[3].x = -0.5f;
    pVertices[3].y =  0.5f;
    pVertices[3].z =  0.0f;
    pVertices[3].color = dark;

    pVertices[4].x =  0.5f;
    pVertices[4].y =  0.5f;
    pVertices[4].z =  0.0f;
    pVertices[4].color = dark;

    pVertices[5].x =  0.5f;
    pVertices[5].y = -0.5f;
    pVertices[5].z =  0.0f;
    pVertices[5].color = dark;


    pVertices[6].x = -0.5f;
    pVertices[6].y =  0.5f;
    pVertices[6].z =  0.0f;
    pVertices[6].color = normal;

    pVertices[7].x = -0.5f;
    pVertices[7].y = -0.5f;
    pVertices[7].z =  0.0f;
    pVertices[7].color = normal;

    pVertices[8].x =  0.0f;
    pVertices[8].y =  0.0f;
    pVertices[8].z =  4.0f;
    pVertices[8].color = bright;


    pVertices[9].x =  0.5f;
    pVertices[9].y =  0.5f;
    pVertices[9].z =  0.0f;
    pVertices[9].color = normal;

    pVertices[10].x = -0.5f;
    pVertices[10].y =  0.5f;
    pVertices[10].z =  0.0f;
    pVertices[10].color = normal;

    pVertices[11].x =  0.0f;
    pVertices[11].y =  0.0f;
    pVertices[11].z =  4.0f;
    pVertices[11].color = bright;


    pVertices[12].x =  0.5f;
    pVertices[12].y = -0.5f;
    pVertices[12].z =  0.0f;
    pVertices[12].color = normal;

    pVertices[13].x =  0.5f;
    pVertices[13].y =  0.5f;
    pVertices[13].z =  0.0f;
    pVertices[13].color = normal;

    pVertices[14].x =  0.0f;
    pVertices[14].y =  0.0f;
    pVertices[14].z =  4.0f;
    pVertices[14].color = bright;


    pVertices[15].x = -0.5f;
    pVertices[15].y = -0.5f;
    pVertices[15].z =  0.0f;
    pVertices[15].color = normal;

    pVertices[16].x =  0.5f;
    pVertices[16].y = -0.5f;
    pVertices[16].z =  0.0f;
    pVertices[16].color = normal;

    pVertices[17].x =  0.0f;
    pVertices[17].y =  0.0f;
    pVertices[17].z =  4.0f;
    pVertices[17].color = bright;

	m_vb->Unlock();


    return TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Renders the terrain.
Parameters:
[in] pDevice - D3D Device
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CArrow::Render( LPDIRECT3DDEVICE9 pDevice )
{
	SetZPosition(pos[0] * 0.5f);
	SetXPosition(pos[1] * 0.5f);
	SetYPosition(pos[2] * 0.01f);

	RotateAbs(-pos[4], pos[5], -pos[3]);

	pDevice->SetTransform( D3DTS_WORLD, GetTransform() );

	pDevice->SetStreamSource( 0, m_vb, 0, sizeof(Vertex) );
	pDevice->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE );
	pDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 6 );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
Summary: Release resources
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CArrow::Release()
{
	//SAFE_RELEASE( m_vb );
}