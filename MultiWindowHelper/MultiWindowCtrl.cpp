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
    CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_SHOW];
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
                info.hInstance = (HINSTANCE)GetWindowLongW( hWnd , GWL_HINSTANCE );
                info.hBitmap = (HBITMAP)LoadImageW( info.hInstance , MAKEINTRESOURCE(IDI_APPLICATION) , IMAGE_BITMAP , 0 , 0 , LR_DEFAULTSIZE | LR_SHARED );
                m_vecWindows.push_back( info );

                INT nImgIndex = tbrUp->AddImage( info.hBitmap , i , RGB(255,255,255) );
                tbrUp->AddCheckButton( i , IDC_TBR_UP_EMPTY+i , NULL , nImgIndex , TRUE );
            }
        }


        edtShow->AddText( L"Started\r\n" );
        //tbrLeft->SetButtonInfo()->SetText( L"Stop" );
        m_bStarted = TRUE;
        bRet = TRUE;
    }

    return bRet;
}

BOOL CMultiWindowCtrl::Stop()
{
    BOOL bRet = FALSE;
    CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_SHOW];
    CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
    CToolbar * tbrLeft = (CToolbar *)g_ctrlMain[TBR_MAIN_LEFT];

    if ( m_bStarted )
    {
        if ( m_vecWindows.size() > 0 )
        {
            for ( size_t i = 0 ; i < m_vecWindows.size() ; i++ )
            {
                tbrUp->DeleteButton( i + 1 );
            }
            m_vecWindows.clear();
        }

        edtShow->AddText( L"Stopped\r\n" );
        //btnStart->SetText( L"Start" );
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

HWND CMultiWindowCtrl::GetWindow( size_t aIndex )
{
    HWND hWnd = NULL;
    if ( 0 <= aIndex && aIndex < m_vecWindows.size() - 1 )
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