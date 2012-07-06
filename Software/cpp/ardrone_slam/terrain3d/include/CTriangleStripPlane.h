/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CGraphics.h
Author : Chad Vernon
URL : http://www.c-unit.com

Description : DirectX Graphics wrapper

Created :  08/26/2005
Modified : 12/01/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CTRIANGLESTRIPPLANE_H
#define CTRIANGLESTRIPPLANE_H

#include "stdafx.h"
#include "CustomVertex.h"

class CTriangleStripPlane
{
public:
    static void GeneratePositionNormalTextured( cuCustomVertex::PositionNormalTextured** ppVertices, int verticesAlongWidth, int verticesAlongLength );
    static void GeneratePositionTexturedWithHeight( cuCustomVertex::PositionTextured** ppVertices, int verticesAlongWidth, int verticesAlongLength, short* pHeight );
    static int GenerateIndices( int** ppIndices, int verticesAlongWidth, int verticesAlongLength );
    static int GenerateIndices( USHORT** ppIndices, int verticesAlongWidth, int verticesAlongLength );

	// NICK
	static void UpdatePositionTexturedWithHeight( cuCustomVertex::PositionTextured** ppVertices, int verticesAlongWidth, int verticesAlongLength, short* pHeight, int* roi );
};

#endif