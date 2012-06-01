/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CGraphics.h
Author : Chad Vernon
URL : http://www.c-unit.com

Description : DirectX Graphics wrapper

Created :  08/26/2005
Modified : 12/01/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef CGRAPHICS_H
#define CGRAPHICS_H

#include "stdafx.h"

class CGraphics
{
public:
    CGraphics();
    ~CGraphics() { Release(); }
    BOOL Initialize( HWND hWnd, BOOL bWindowed );
    void Release();
    BOOL Reset();
    LPDIRECT3D9       GetD3D()          { return m_pD3D9; }
    LPDIRECT3DDEVICE9 GetDevice()       { return m_pDevice; }
    D3DDISPLAYMODE*   GetDisplayMode()  { return &m_displayMode; }
    BOOL Windowed;

private:
    BOOL BuildPresentParameters();
    HWND                    m_hWnd;
    HINSTANCE               m_hInstance;
    LPDIRECT3D9             m_pD3D9;
    LPDIRECT3DDEVICE9       m_pDevice;
    D3DCAPS9                m_caps;
    D3DPRESENT_PARAMETERS   m_pp;
    D3DDISPLAYMODE          m_displayMode;
};

#endif