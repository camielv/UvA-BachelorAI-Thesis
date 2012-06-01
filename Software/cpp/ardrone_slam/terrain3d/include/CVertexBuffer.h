/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CVertexBuffer.h
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Vertex buffer wrapper

Created :  08/11/2005
Modified : 12/01/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef CVERTEXBUFFER_H
#define CVERTEXBUFFER_H

#include "stdafx.h"
#include "CIndexBuffer.h"

class CVertexBuffer
{
public:
    CVertexBuffer();
    ~CVertexBuffer() { Release(); }

    BOOL CreateBuffer( LPDIRECT3DDEVICE9 pDevice, UINT numVertices, DWORD FVF, UINT vertexSize, 
        BOOL dynamic = FALSE );
    void Release();
    BOOL SetData( UINT numVertices, void *pVertices, DWORD flags = D3DLOCK_DISCARD, int verticesAlongLength = 0, int* roi = NULL);
    void SetIndexBuffer( CIndexBuffer* pIB );
    void Render( LPDIRECT3DDEVICE9 pDevice, UINT numPrimitives, D3DPRIMITIVETYPE primitiveType );

private:
    LPDIRECT3DVERTEXBUFFER9 m_pVB;
    CIndexBuffer*           m_pIB;
    UINT                    m_numVertices;
    UINT                    m_vertexSize;
    DWORD                   m_FVF;
};

#endif