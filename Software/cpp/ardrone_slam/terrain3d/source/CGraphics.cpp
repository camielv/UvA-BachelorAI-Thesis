/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CGraphics.cpp
Author : Chad Vernon
URL : http://www.c-unit.com

Description : DirectX Graphics wrapper

Created :  08/26/2005
Modified : 12/01/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "..\include\stdafx.h"
#include "..\include\CGraphics.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Default constructor
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
CGraphics::CGraphics()
{
    m_pD3D9 = NULL; 
    m_pDevice = NULL; 
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Cleans up resources
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CGraphics::Release()
{
    SAFE_RELEASE( m_pDevice );
    SAFE_RELEASE( m_pD3D9 );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Initializes Direct3D.
Parameters:
[in] hWnd - Handle to the window
[in] windowed - TRUE for windowed mode, FALSE for fullscreen mode
Returns: TRUE on success. FALSE on failure
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CGraphics::Initialize( HWND hWnd, BOOL windowed )
{
    Windowed = windowed;
    m_hWnd = hWnd;
    SAFE_RELEASE( m_pD3D9 );
    SAFE_RELEASE( m_pDevice );
    m_pD3D9 = Direct3DCreate9( D3D_SDK_VERSION );
    if ( !m_pD3D9 )
    {
        SHOWERROR( "Direct3DCreate9() - Failed", __FILE__, __LINE__ );
        return FALSE;
    }

    m_pD3D9->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &m_displayMode );
    m_pD3D9->GetDeviceCaps( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_caps );

    // Check for hardware T&L
    DWORD vertexProcessing = 0;
    if ( m_caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
    {
        vertexProcessing = D3DCREATE_HARDWARE_VERTEXPROCESSING;
		// Check for pure device
		if ( m_caps.DevCaps & D3DDEVCAPS_PUREDEVICE )
		{
			vertexProcessing |= D3DCREATE_PUREDEVICE;
		}
    }
    else
    {
        vertexProcessing = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
    }

    if ( !BuildPresentParameters() )
	{
        SAFE_RELEASE( m_pD3D9 );
        return FALSE;
	}

    HRESULT hresult = 0;
    // Create the device
    hresult = m_pD3D9->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_hWnd, vertexProcessing, &m_pp, &m_pDevice );
    if ( FAILED( hresult ) )
    {
        SAFE_RELEASE( m_pD3D9 );
        SHOWERROR( "CreateDevice() - Failed", __FILE__, __LINE__ );
        return FALSE;
    }
    return TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Builds the D3DPRESENT_PARAMETERS structure.
Returns: TRUE on success. FALSE on failure
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CGraphics::BuildPresentParameters()
{
    ZeroMemory( &m_pp, sizeof(m_pp) );
	D3DFORMAT adapterFormat = (Windowed) ? m_displayMode.Format : D3DFMT_X8R8G8B8;
    if ( SUCCEEDED( m_pD3D9->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24S8 ) ) )
    {
        m_pp.AutoDepthStencilFormat = D3DFMT_D24S8;
    }
    else if ( SUCCEEDED( m_pD3D9->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D24X8 ) ) )
    {
        m_pp.AutoDepthStencilFormat = D3DFMT_D24X8;
    }
    else if ( SUCCEEDED( m_pD3D9->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, adapterFormat, D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
    {
        m_pp.AutoDepthStencilFormat = D3DFMT_D16;
    }
	else
	{
        SHOWERROR( "Unable to find valid depth format", __FILE__, __LINE__ );
		return FALSE;
	}

    m_pp.BackBufferWidth            = (Windowed) ? 0 : m_displayMode.Width;
    m_pp.BackBufferHeight           = (Windowed) ? 0 : m_displayMode.Height;
    m_pp.BackBufferFormat           = adapterFormat;
    m_pp.BackBufferCount            = 1;
    m_pp.MultiSampleType            = D3DMULTISAMPLE_NONE;
    m_pp.MultiSampleQuality         = 0;
    m_pp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
    m_pp.hDeviceWindow              = m_hWnd;
    m_pp.Windowed                   = Windowed;
    m_pp.EnableAutoDepthStencil     = TRUE; 
	m_pp.FullScreen_RefreshRateInHz = (Windowed) ? 0 : m_displayMode.RefreshRate;
    m_pp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

	return TRUE;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Resets the device
Returns: TRUE on success. FALSE on failure
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
BOOL CGraphics::Reset()
{
    if ( m_pDevice )
    {
        if ( !BuildPresentParameters() )
        {
            return FALSE;
        }
        m_pDevice->Reset( &m_pp );
    }

    return TRUE;
}