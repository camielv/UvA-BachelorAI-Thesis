/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CTerrain.h
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Terrain class

Created :  08/11/2005
Modified : 12/05/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CARROW_H
#define CARROW_H

#include "stdafx.h"
#include "CWorldTransform.h"

struct Vertex
{
    float x, y, z;
    DWORD color;
};

class CArrow : public CWorldTransform
{
public:
    CArrow();
    ~CArrow() { Release(); }

    BOOL Initialize( LPDIRECT3DDEVICE9 pDevice, float* pos );
    void Render( LPDIRECT3DDEVICE9 pDevice);
    void Release();

private:
    LPDIRECT3DVERTEXBUFFER9 m_vb;
	float* pos;
};

#endif