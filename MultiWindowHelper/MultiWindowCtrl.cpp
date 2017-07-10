#include "stdafx.h"
#include "MultiWindowCtrl.h"
using namespace CWUi;

CMultiWindowCtrl CMultiWindowCtrl::m_self;

#define RULE_NAME_OPENGL    L"OpenGL"

DWORD OpenGLScanCbk( IN DWORD aPid , IN VOID * aUserCtx , CHAR * aBuf , DWORD aBufSize )
{
    CMultiWindowCtrl * pCtrl = (CMultiWindowCtrl *)aUserCtx;
    CONST CMultiWindowCtrl::WindowInfo * pInfo;
    if ( pCtrl != NULL && pCtrl->GetWindowInfoByPid( aPid , &pInfo )  )
    {
        CopyMemory( pInfo->pBmpData , aBuf , min(aBufSize , pInfo->dwBmpSize * sizeof(pInfo->pBmpData[0])) );
    }
    return ERROR_SUCCESS;
}



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

    do 
    {
        if ( m_bStarted )
        {
            edtShow->AddText( L"Already started\r\n" );
            bRet = TRUE;
            break;
        }

        wstring wstrModDir;
        CWUtils::GetModuleDir( GetModuleHandleW(NULL) , wstrModDir );

        CMultiWindowConfig::GetInstance()->Reload();

        if ( CMultiWindowConfig::GetInstance()->GetDisplayMode() == MULTI_WINDOW_DISPLAY_MODE_GLREADPIXELS )
        {
            //Inject DLL to the target process for OpenGL mode
            if ( FALSE == m_DllInjectMgr.Init( (wstrModDir + L"Config.ini").c_str() ) )
            {
                edtShow->AddText( L"m_DllInjectMgr.Init() failed. GetLastError()=%lu\r\n" , GetLastError() );
                break;
            }
        }

        vector<wstring> vecWindowNames;
        CMultiWindowConfig::GetInstance()->GetWindowNames( vecWindowNames );
        for ( size_t i = 0 ; i < vecWindowNames.size() ; i++ )
        {
            HWND hWnd = FindWindowW( NULL , vecWindowNames[i].c_str() );
            if ( hWnd == NULL )
            {
                continue;
            }

            edtShow->AddText( L"[%d] Found HWND %ws\r\n" , i , vecWindowNames[i].c_str() );
            WindowInfo info;
            m_vecWindows.push_back( info );
            WindowInfo * pInfo = &(*(m_vecWindows.rbegin()));

            pInfo->wstrWindowName = vecWindowNames[i];
            pInfo->hWnd = hWnd;
            pInfo->lpExStyle = GetWindowLongPtrW( hWnd , GWL_EXSTYLE );
            SetWindowLongPtrW( hWnd , GWL_EXSTYLE , pInfo->lpExStyle | WS_EX_LAYERED );

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
            pInfo->hIcon = hMemBmp;

            if ( CMultiWindowConfig::GetInstance()->GetDisplayMode() == MULTI_WINDOW_DISPLAY_MODE_GLREADPIXELS )
            {
                //Prepare per-window buffer for OpenGL
                INT nScreenWidth = GetSystemMetrics( SM_CXSCREEN );
	            INT nScreenHeight = GetSystemMetrics( SM_CYSCREEN );
                pInfo->dwBmpSize = nScreenWidth * nScreenHeight * 4;
                pInfo->pBmpData = new (std::nothrow)UINT[pInfo->dwBmpSize];
                ZeroMemory( pInfo->pBmpData  , pInfo->dwBmpSize );

                //Do DLL injection
                HANDLE hProcess = NULL;
                CWUtils::DllInjectServerUserCfg cfgOpenGL;
                cfgOpenGL.wstrDllPath32 = wstrModDir + L"OpenGLHandler32.dll";
                #ifdef _WIN64
                    cfgOpenGL.wstrDllPath64 = wstrModDir + L"OpenGLHandler64.dll";
                #endif
                cfgOpenGL.pUserCtx = (VOID *)this;
                cfgOpenGL.InjectedCbk = NULL;
                cfgOpenGL.ScanCbk = OpenGLScanCbk;
                cfgOpenGL.UnInjectedCbk = NULL;
                do 
                {
                    if ( FALSE == m_DllInjectMgr.RegisterDllInject( RULE_NAME_OPENGL , &cfgOpenGL ) )
                    {
                        edtShow->AddText( L"m_DllInjectMgr.RegisterDllInject() failed. GetLastError()=%lu\r\n" , GetLastError() );
                        break;
                    }
                
                    if ( FALSE == m_DllInjectMgr.StartMonitor( RULE_NAME_OPENGL , FALSE ) )
                    {
                        edtShow->AddText( L"m_DllInjectMgr.StartMonitor() failed. GetLastError()=%lu\r\n" , GetLastError() );
                        break;
                    }

                    GetWindowThreadProcessId( hWnd , &pInfo->dwPid );
                    hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ , FALSE , pInfo->dwPid );
                    if ( NULL == hProcess )
                    {
                        edtShow->AddText( L"OpenProcess() failed. GetLastError()=%lu\r\n" , GetLastError() );
                        break;
                    }

                    WCHAR wzTmpPath[MAX_PATH] = {};
                    if ( 0 == GetModuleFileNameExW( hProcess , NULL , wzTmpPath , _countof(wzTmpPath) ) )
                    {
                        edtShow->AddText( L"GetModuleFileNameExW() failed. GetLastError()=%lu\r\n" , GetLastError() );
                        break;
                    }
                    pInfo->wstrProcPath = wzTmpPath;
                    if ( FALSE == m_DllInjectMgr.OnProcessCreateTerminate( TRUE , pInfo->dwPid , wzTmpPath ) )
                    {
                        edtShow->AddText( L"OnProcessCreateTerminate() failed. GetLastError()=%lu\r\n" , GetLastError() );
                        break;
                    }
                    if ( FALSE == m_DllInjectMgr.OnImageLoaded( pInfo->dwPid , L"kernel32.dll" ) )
                    {
                        edtShow->AddText( L"OnImageLoaded() failed. GetLastError()=%lu\r\n" , GetLastError() );
                        break;
                    }
                    pInfo->bDllInjected = TRUE;
                } while ( 0 );

                if ( NULL != hProcess )
                {
                    CloseHandle( hProcess );
                }
                
                if ( FALSE == pInfo->bDllInjected )
                {
                    edtShow->AddText( L"Skip %ws because DLL injection failed\r\n" , pInfo->wstrWindowName.c_str() );
                    m_vecWindows.pop_back();
                    continue;
                }
            }



            INT nImgIndex = tbrUp->AddImage( pInfo->hIcon , RGB(255,255,255) );
            tbrUp->AddCheckButton( i , IDC_TBR_UP_WINDOWS+i , pInfo->wstrWindowName.c_str() , nImgIndex , TRUE );
        }

        edtShow->AddText( L"Started\r\n" );
        tbrLeft->SetButtonText( 0 , L"Stop" );
        m_bStarted = TRUE;
        bRet = TRUE;
    } while ( 0 );
    
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

            m_DllInjectMgr.StopMonitor( RULE_NAME_OPENGL );
            m_DllInjectMgr.UnregisterDllInject( RULE_NAME_OPENGL );

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

BOOL CMultiWindowCtrl::GetWindowInfoByPid( DWORD aPid , CONST WindowInfo ** aInfo )
{
    for ( size_t i = 0 ; i < m_vecWindows.size() ; i++ )
    {
        if ( m_vecWindows[i].dwPid == aPid )
        {
            *aInfo = &m_vecWindows[i];
            return TRUE;
        }
    }
    return FALSE;
}


BOOL CMultiWindowCtrl::DispatchMsgIfNeed( UINT aMsg , WPARAM aWParam , LPARAM aLParam )
{
    BOOL bRet = FALSE;

    if ( CMultiWindowCtrl::GetInstance()->IsStarted() &&
         CMultiWindowConfig::GetInstance()->ShouldDispatch( aMsg ) )
    {
        //if ( aMsg == WM_LBUTTONDOWN || aMsg == WM_LBUTTONUP || aMsg == WM_LBUTTONDBLCLK || 
        //     aMsg == WM_RBUTTONDOWN || aMsg == WM_RBUTTONUP || aMsg == WM_RBUTTONDBLCLK )
        //{
        //    WCHAR wzBuf[4096] = {};
        //    wstring wstrBuf;
        //    struct { UINT uId; CONST WCHAR * wzId; } aryMsg[] = 
        //    {
        //        NUM_TEXT_PAIRW(WM_LBUTTONDOWN) , 
        //        NUM_TEXT_PAIRW(WM_LBUTTONUP) , 
        //        NUM_TEXT_PAIRW(WM_LBUTTONDBLCLK) , 
        //        NUM_TEXT_PAIRW(WM_RBUTTONDOWN) , 
        //        NUM_TEXT_PAIRW(WM_RBUTTONUP) , 
        //        NUM_TEXT_PAIRW(WM_RBUTTONDBLCLK) 
        //    };
        //    for ( size_t i = 0 ; i < _countof(aryMsg) ; i++ )
        //    {
        //        if ( aMsg == aryMsg[i].uId )
        //        {
        //            wstrBuf.append( aryMsg[i].wzId );
        //            break;
        //        }
        //    }
        //    wstrBuf.push_back( L':' );
        //
        //    struct { UINT uId; CONST WCHAR * wzId; } aryWParam[] = 
        //    {
        //        NUM_TEXT_PAIRW(MK_LBUTTON) , 
        //        NUM_TEXT_PAIRW(MK_MBUTTON) ,
        //        NUM_TEXT_PAIRW(MK_RBUTTON) , 
        //        NUM_TEXT_PAIRW(MK_CONTROL) , 
        //        NUM_TEXT_PAIRW(MK_SHIFT)
        //    };
        //    for ( size_t i = 0 ; i < _countof(aryWParam) ; i++ )
        //    {
        //        if ( aWParam == aryWParam[i].uId )
        //        {
        //            if ( 0 < wstrBuf.length() )
        //            {
        //                wstrBuf.push_back( L'|' );
        //            }
        //            wstrBuf.append( aryWParam[i].wzId );
        //        }
        //    }
        //    wstrBuf.push_back( L':' );
        //
        //    _snwprintf_s( wzBuf , _TRUNCATE , L"(%d,%d)\r\n" , GET_X_LPARAM(aLParam) , GET_Y_LPARAM(aLParam) );
        //    wstrBuf.append( wzBuf );
        //    ((CEdit *)g_ctrlMain[EDT_LOG])->AddText( wstrBuf.c_str() );
        //}
        //if ( aMsg == WM_KEYDOWN || aMsg == WM_KEYUP )
        //{
        //    WCHAR wzBuf[4096] = {};
        //    bitset<sizeof(ULONG)*8> repeat( (int)CWUtils::BitRange( (ULONG)aLParam , 0 , 15 ) );
        //    bitset<sizeof(ULONG)*8> scan( (int)CWUtils::BitRange( (ULONG)aLParam , 16 , 23 ) );
        //    bitset<sizeof(ULONG)*8> extend( (int)CWUtils::BitRange( (ULONG)aLParam , 24 , 24 ) );
        //    bitset<sizeof(ULONG)*8> reserved( (int)CWUtils::BitRange( (ULONG)aLParam , 25 , 28 ) );
        //    bitset<sizeof(ULONG)*8> context( (int)CWUtils::BitRange( (ULONG)aLParam , 29 , 29 ) );
        //    bitset<sizeof(ULONG)*8> preState( (int)CWUtils::BitRange( (ULONG)aLParam , 30 , 30 ) );
        //    bitset<sizeof(ULONG)*8> transition( (int)CWUtils::BitRange( (ULONG)aLParam , 31 , 31 ) );            
        //    _snwprintf_s( wzBuf , _TRUNCATE , 
        //                  L"%ws: Virtual Code: %lu, Repeat Count: %lu, Scan Code: %lu, Extend: %lu, Reserved: %lu\r\n"
        //                  L"\tContext Code: %lu, Previous State: %lu, Transition State: %lu\r\n" , 
        //                  ( aMsg == WM_KEYDOWN ) ? L"WM_KEYDOWN" : L"WM_KEYUP" ,
        //                  aWParam , repeat.to_ulong() , scan.to_ulong() , extend.to_ulong() , reserved.to_ulong() ,
        //                  context.to_ulong() , preState.to_ulong() , transition.to_ulong() );
        //    ((CEdit *)g_ctrlMain[EDT_LOG])->AddText( wzBuf );
        //}
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



