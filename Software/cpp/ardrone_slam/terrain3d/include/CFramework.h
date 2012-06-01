/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

Title : CFramework.h
Author : Chad Vernon
URL : http://www.c-unit.com

Description : Main control point of the application.

Created :  08/26/2005
Modified : 12/03/2005

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef CFRAMEWORK_H
#define CFRAMEWORK_H

#include "stdafx.h"
#include "CGraphics.h"
#include "CInput.h"
#include "CTimer.h"

static DWORD WINAPI start_thread_input(void* Param);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Interface that the main game application must implement
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
class CBaseApp
{
public:
    virtual ~CBaseApp() {}
    virtual void Release() = 0;
    virtual void OnCreateDevice( LPDIRECT3DDEVICE9 pDevice ) = 0;
    virtual void OnResetDevice( LPDIRECT3DDEVICE9 pDevice ) = 0;
    virtual void OnLostDevice() = 0;
    virtual void OnDestroyDevice() = 0;
    virtual void OnUpdateFrame( LPDIRECT3DDEVICE9 pDevice, float elapsedTime ) = 0;
    virtual void OnRenderFrame( LPDIRECT3DDEVICE9 pDevice, float elapsedTime ) = 0;
    virtual void ProcessInput( long xDelta, long yDelta, long zDelta, BOOL* pMouseButtons, BOOL* pPressedKeys, float elapsedTime ) = 0;
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
Summary: Main framework class
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
class CFramework
{
public:
    CFramework( CBaseApp* pGameApp );
    ~CFramework() { Release(); }
    BOOL Initialize( char* title, HINSTANCE hInstance, int width, int height, BOOL windowed = TRUE );
    void Release();
    void ToggleFullscreen();
    void ToggleWireframe();
    static LRESULT CALLBACK StaticWndProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam );
    void LockKey( DWORD key );
    long GetXCursor();
    long GetYCursor();
    DWORD GetFillMode();
    int GetWidth();
    int GetHeight();

    void OnUpdateFrame();
    void OnRenderFrame();

	CGraphics*      m_pGraphics;
	CInputDevice    m_mouse;

private:
    LRESULT CALLBACK WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    void OnCreateDevice();
    void OnResetDevice();
    void OnLostDevice();
    void OnDestroyDevice();
    void Pause( BOOL rendering, BOOL timer );

    HWND            m_hWnd;
    HINSTANCE       m_hInstance;
    BOOL            m_active;
    int             m_windowWidth;
    int             m_windowHeight;
    int             m_fullscreenWidth;
    int             m_fullscreenHeight;
    WINDOWPLACEMENT m_wp;
    DWORD           m_fillMode;
    BOOL            m_renderingPaused;
    BOOL            m_timerPaused;
    int             m_renderingPauseCount;
    int             m_timerPauseCount;
    BOOL            m_closing;

    //CGraphics*      m_pGraphics;
    CBaseApp*       m_pGameApp;
    CTimer*         m_pTimer;
    //CInputDevice    m_mouse;
	//BOOL			m_mouseRightButtonPressed;
    CInputDevice    m_keyboard;
    LPDIRECTINPUT8  m_pDI;

	HANDLE				thread_input;
};

#endif