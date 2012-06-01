/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CTriangleStripPlane.cpp
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Generates vertices and indices to create an indexed triangle strip plane

Created :  12/04/2005
Modified : 12/04/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "..\include\stdafx.h"
#include "..\include\CTriangleStripPlane.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Generates vertices with a position, normal, and texture coordinates to create an indexed 
triangle strip plane.
Parameters: 
[in/out] ppVertices - Pointer to an array to be filled up.
[in] verticesAlongWidth - Number of vertices along the width
[in] verticesAlongLength - Number of vertices along the length
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CTriangleStripPlane::GeneratePositionNormalTextured( cuCustomVertex::PositionNormalTextured** ppVertices, int verticesAlongWidth, int verticesAlongLength )
{
    SAFE_DELETE_ARRAY( *ppVertices );

	int verticesAlongLengthIndex = verticesAlongLength - 1;

    *ppVertices = new cuCustomVertex::PositionNormalTextured[verticesAlongLength * verticesAlongWidth];
    for ( int z = 0; z < verticesAlongLength; z++ )
    {
        for ( int x = 0; x < verticesAlongWidth; x++ )
        {
            float halfWidth = ((float)verticesAlongWidth - 1.0f) / 2.0f;
            float halfLength = ((float)verticesAlongLength - 1.0f) / 2.0f;
            (*ppVertices)[z * verticesAlongLength + x] = cuCustomVertex::PositionNormalTextured(
                (float)x - halfWidth, 0.0f, (float)z - halfLength,
                0.0f, 1.0f, 0.0f,
				(float)x / (verticesAlongWidth - 1),
				(float)(verticesAlongLengthIndex - z) / verticesAlongLengthIndex
                //(float)x / (verticesAlongWidth - 1), (float)z / (verticesAlongLength - 1)
            );
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Generates vertices with a position, normal, and texture coordinates to create an indexed 
triangle strip plane.
Parameters: 
[in/out] ppVertices - Pointer to an array to be filled up.
[in] verticesAlongWidth - Number of vertices along the width
[in] verticesAlongLength - Number of vertices along the length
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CTriangleStripPlane::GeneratePositionTexturedWithHeight( cuCustomVertex::PositionTextured** ppVertices, int verticesAlongWidth, int verticesAlongLength, short* pHeight )
{
    SAFE_DELETE_ARRAY( *ppVertices );

	int verticesAlongLengthIndex = verticesAlongLength - 1;

    *ppVertices = new cuCustomVertex::PositionTextured[verticesAlongLength * verticesAlongWidth];
    for ( int z = 0; z < verticesAlongLength; z++ )
    {
        for ( int x = 0; x < verticesAlongWidth; x++ )
        {
            float halfWidth = ((float)verticesAlongWidth - 1.0f) / 2.0f;
            float halfLength = ((float)verticesAlongLength - 1.0f) / 2.0f;
            (*ppVertices)[z * verticesAlongLength + x] = cuCustomVertex::PositionTextured(
                (float)x - halfWidth, 
                (float)pHeight[z * verticesAlongLength + x], 
                (float)z - halfLength,
				(float)x / (verticesAlongWidth - 1),
				(float)(verticesAlongLengthIndex - z) / verticesAlongLengthIndex
                //(float)x / (verticesAlongWidth - 1), (float)z / (verticesAlongLength - 1)
            );
        }
    }
}

void CTriangleStripPlane::UpdatePositionTexturedWithHeight( cuCustomVertex::PositionTextured** ppVertices, int verticesAlongWidth, int verticesAlongLength, short* pHeight, int* roi )
{
	int i;

	int verticesAlongLengthIndex = verticesAlongLength - 1;

    for ( int z = roi[0]; z <= roi[1]; z++ )
    {
        for ( int x = roi[2]; x <= roi[3]; x++ )
        {
            float halfWidth = ((float)verticesAlongWidth - 1.0f) / 2.0f;
            float halfLength = ((float)verticesAlongLength - 1.0f) / 2.0f;
			i = z * verticesAlongLength + x;

            (*ppVertices)[i].X = (float)x - halfWidth;
			(*ppVertices)[i].Y = (float)pHeight[z * verticesAlongLength + x];
			(*ppVertices)[i].Z = (float)z - halfLength;
			(*ppVertices)[i].Tu = (float)x / (verticesAlongWidth - 1);
			(*ppVertices)[i].Tv = (float)(verticesAlongLengthIndex - z) / verticesAlongLengthIndex;
			//(*ppVertices)[i].Tv = (float)z / (verticesAlongLength - 1);
        }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Generates 32-bit indices for an indexed triangle strip plane.
Parameters: 
[in/out] ppIndices - Array to be filled up.
[in] verticesAlongWidth - Number of vertices along the width
[in] verticesAlongLength - Number of vertices along the length
Returns: The number of indices
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int CTriangleStripPlane::GenerateIndices( int** ppIndices, int verticesAlongWidth, int verticesAlongLength )
{
    int numIndices = (verticesAlongWidth * 2) * (verticesAlongLength - 1) + (verticesAlongLength - 2);

    SAFE_DELETE_ARRAY( *ppIndices );
    *ppIndices = new int[numIndices];

    int index = 0;
    for ( int z = 0; z < verticesAlongLength - 1; z++ )
    {
        // Even rows move left to right, odd rows move right to left.
        if ( z % 2 == 0 )
        {
            // Even row
            int x;
            for ( x = 0; x < verticesAlongWidth; x++ )
            {
                (*ppIndices)[index++] = x + (z * verticesAlongWidth);
                (*ppIndices)[index++] = x + (z * verticesAlongWidth) + verticesAlongWidth;
            }
            // Insert degenerate vertex if this isn't the last row
            if ( z != verticesAlongLength - 2)
            {
                (*ppIndices)[index++] = --x + (z * verticesAlongWidth);
            }
        } 
        else
        {
            // Odd row
            int x;
            for ( x = verticesAlongWidth - 1; x >= 0; x-- )
            {
                (*ppIndices)[index++] = x + (z * verticesAlongWidth);
                (*ppIndices)[index++] = x + (z * verticesAlongWidth) + verticesAlongWidth;
            }
            // Insert degenerate vertex if this isn't the last row
            if ( z != verticesAlongLength - 2)
            {
                (*ppIndices)[index++] = ++x + (z * verticesAlongWidth);
            }
        }
    } 
    return numIndices;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Generates 16-bit indices for an indexed triangle strip plane.
Parameters: 
[in/out] pIndices - Array to be filled up.
[in] verticesAlongWidth - Number of vertices along the width
[in] verticesAlongLength - Number of vertices along the length
Returns: The number of indices
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int CTriangleStripPlane::GenerateIndices( USHORT** ppIndices, int verticesAlongWidth, int verticesAlongLength )
{
    int numIndices = (verticesAlongWidth * 2) * (verticesAlongLength - 1) + (verticesAlongLength - 2);

    SAFE_DELETE_ARRAY( *ppIndices );
    *ppIndices = new USHORT[numIndices];

    int index = 0;
    for ( int z = 0; z < verticesAlongLength - 1; z++ )
    {
        // Even rows move left to right, odd rows move right to left.
        if ( z % 2 == 0 )
        {
            // Even row
            int x;
            for ( x = 0; x < verticesAlongWidth; x++ )
            {
                (*ppIndices)[index++] = (USHORT)(x + (z * verticesAlongWidth));
                (*ppIndices)[index++] = (USHORT)(x + (z * verticesAlongWidth) + verticesAlongWidth);
            }
            // Insert degenerate vertex if this isn't the last row
            if ( z != verticesAlongLength - 2)
            {
                (*ppIndices)[index++] = (USHORT)(--x + (z * verticesAlongWidth));
            }
        } 
        else
        {
            // Odd row
            int x;
            for ( x = verticesAlongWidth - 1; x >= 0; x-- )
            {
                (*ppIndices)[index++] = (USHORT)(x + (z * verticesAlongWidth));
                (*ppIndices)[index++] = (USHORT)(x + (z * verticesAlongWidth) + verticesAlongWidth);
            }
            // Insert degenerate vertex if this isn't the last row
            if ( z != verticesAlongLength - 2)
            {
                (*ppIndices)[index++] = (USHORT)(++x + (z * verticesAlongWidth));
            }
        }
    } 
    return numIndices;
}