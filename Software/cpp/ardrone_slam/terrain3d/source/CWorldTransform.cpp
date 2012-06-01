/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CWorldTransform.cpp
Author : Chad Vernon
URL : http://www.c-unit.com

Description : World transform wrapper

Created :  09/05/2005
Modified : 12/03/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "..\include\stdafx.h"
#include "..\include\CWorldTransform.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Default Constructor
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CWorldTransform::CWorldTransform()
{
    Reset();
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Reset the matrices to default position.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CWorldTransform::Reset()
{
    D3DXMatrixIdentity( &m_translate );
    D3DXMatrixIdentity( &m_rotate );
    D3DXMatrixIdentity( &m_scale );
    D3DXMatrixIdentity( &m_transform );
    m_rotationX = m_rotationY = m_rotationZ = 0.0f;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Absolute translation
Parameters:
[in] x - X position
[in] y - Y position
[in] z - Z position
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CWorldTransform::TranslateAbs( float x, float y, float z )
{
    m_translate._41 = x;
    m_translate._42 = y;
    m_translate._43 = z;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Relative translation
Parameters:
[in] x - X amount
[in] y - Y amount
[in] z - Z amount
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CWorldTransform::TranslateRel( float x, float y, float z )
{
    m_translate._41 += x;
    m_translate._42 += y;
    m_translate._43 += z;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Absolute rotation
Parameters:
[in] x - X radians
[in] y - Y radians
[in] z - Z radians
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CWorldTransform::RotateAbs( float x, float y, float z )
{
    m_rotationX = x;
    m_rotationY = y;
    m_rotationZ = z;
    D3DXMatrixRotationYawPitchRoll( &m_rotate, y, x, z );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Relative rotation
Parameters:
[in] x - X radians
[in] y - Y radians
[in] z - Z radians
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CWorldTransform::RotateRel( float x, float y, float z )
{
    m_rotationX += x;
    m_rotationY += y;
    m_rotationZ += z;
    D3DXMatrixRotationYawPitchRoll( &m_rotate,  m_rotationY,  m_rotationX,  m_rotationZ );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Absolute scale.
Parameters:
[in] x - X size
[in] y - Y size
[in] z - Z size
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CWorldTransform::ScaleAbs( float x, float y, float z )
{
    m_scale._11 = x;
    m_scale._22 = y;
    m_scale._33 = z;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Relative scale.
Parameters:
[in] x - X size
[in] y - Y size
[in] z - Z size
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CWorldTransform::ScaleRel( float x, float y, float z )
{
    m_scale._11 += x;
    m_scale._22 += y;
    m_scale._33 += z;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Get the transformation matrix
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
D3DXMATRIX* CWorldTransform::GetTransform()
{
    m_transform = m_scale * m_rotate * m_translate; 
    return &m_transform;
}