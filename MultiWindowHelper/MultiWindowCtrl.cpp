#include "stdafx.h"
#include "MultiWindowCtrl.h"
using namespace CWUi;

CMultiWindowCtrl CMultiWindowCtrl::m_self;



BOOL CMultiWindowCtrl::IsStarted()
{
    return m_bStarted;
}

BOOL CMultiWindowCtrl::Start()
{
    BOOL bRet = FALSE;
    CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_LOG];
    CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
    CToolbar * tbrLeft = (CToolbar *)g_ctrlMain[TBR_MAIN_LEFT];

    if ( m_bStarted )
    {
        edtShow->AddText( L"Already started\r\n" );
        bRet = TRUE;
    }
    else
    {
        CMultiWindowConfig::GetInstance()->Reload();

        vector<wstring> vecWindowNames;
        CMultiWindowConfig::GetInstance()->GetWindowNames( vecWindowNames );

        for ( size_t i = 0 ; i < vecWindowNames.size() ; i++ )
        {
            HWND hWnd = FindWindowW( NULL , vecWindowNames[i].c_str() );
            if ( hWnd != NULL )
            {
                edtShow->AddText( L"[%d] Found HWND %ws\r\n" , i , vecWindowNames[i].c_str() );
                WindowInfo info;
                info.wstrWindowName = vecWindowNames[i];
                info.hWnd = hWnd;
                info.lpExStyle = GetWindowLongPtrW( hWnd , GWL_EXSTYLE );
                SetWindowLongPtrW( hWnd , GWL_EXSTYLE , info.lpExStyle | WS_EX_LAYERED );

                //Get target window's icon
                HICON hIcon = (HICON)GetClassLongPtrW( hWnd , GCLP_HICON );
                HDC hDC = GetDC( tbrUp->Self() );
                HDC hMemDC = CreateCompatibleDC( hDC );
                HBITMAP hMemBmp = CreateCompatibleBitmap( hDC , tbrUp->IconWidth() , tbrUp->IconHeight() );
                HGDIOBJ hLastObj = SelectObject( hMemDC , hMemBmp );
                DrawIconEx( hMemDC , 0 , 0 , hIcon , tbrUp->IconWidth() , tbrUp->IconHeight() , 0 , NULL , DI_NORMAL );
                SelectObject( hMemDC , hLastObj );
                DeleteDC( hMemDC );
                ReleaseDC( NULL , hDC );
                DestroyIcon( hIcon );
                info.hBitmap = hMemBmp;
                m_vecWindows.push_back( info );

                INT nImgIndex = tbrUp->AddImage( info.hBitmap , RGB(255,255,255) );
                tbrUp->AddCheckButton( i , IDC_TBR_UP_WINDOWS+i , info.wstrWindowName.c_str() , nImgIndex , TRUE );
            }
        }


        edtShow->AddText( L"Started\r\n" );
        tbrLeft->SetButtonText( 0 , L"Stop" );
        m_bStarted = TRUE;
        bRet = TRUE;
    }

    return bRet;
}

BOOL CMultiWindowCtrl::Stop()
{
    BOOL bRet = FALSE;
    CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_LOG];
    CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
    CToolbar * tbrLeft = (CToolbar *)g_ctrlMain[TBR_MAIN_LEFT];

    if ( m_bStarted )
    {
        if ( m_vecWindows.size() > 0 )
        {
            for ( size_t i = 0 ; i < m_vecWindows.size() ; i++ )
            {
                tbrUp->DeleteButton( 1 );
            }
            m_vecWindows.clear();
        }

        edtShow->AddText( L"Stopped\r\n" );
        tbrLeft->SetButtonText( 0 , L"Start" );
        m_bStarted = FALSE;
        bRet = TRUE;
    }
    else
    {
        edtShow->AddText( L"Already stopped\r\n" );
        bRet = TRUE;
    }

    return bRet;
}

HWND CMultiWindowCtrl::GetWindow( INT aIndex )
{
    HWND hWnd = NULL;
    if ( 0 <= aIndex && aIndex < (INT)m_vecWindows.size() )
    {
        hWnd = m_vecWindows[aIndex].hWnd;
    }
    return hWnd;
}


BOOL CMultiWindowCtrl::DispatchMsgIfNeed( UINT aMsg , WPARAM aWParam , LPARAM aLParam )
{
    BOOL bRet = FALSE;

    if ( CMultiWindowCtrl::GetInstance()->IsStarted() &&
         CMultiWindowConfig::GetInstance()->ShouldDispatch( aMsg ) )
    {
        if ( CMultiWindowConfig::GetInstance()->IsAsync() )
        {
            for ( size_t i = 0 ; i < m_vecWindows.size() ; i++ )
            {
                PostMessageW( m_vecWindows[i].hWnd , aMsg , aWParam , aLParam );
            }
        }
        else
        {
            for ( size_t i = 0 ; i < m_vecWindows.size() ; i++ )
            {
                SendMessageW( m_vecWindows[i].hWnd , aMsg , aWParam , aLParam );
            }
        }
        
        bRet = TRUE;
    }

    return bRet;
}
