#include "stdafx.h"
#include "Main.h"
#include <bitset>

#include "CWToolbar.h"
#include "CWEdit.h"
#include "CWButton.h"
#include "CWToolbarProcs.h"
#include "CWButtonProcs.h"

#include "CWGeneralUtils.h"

#include "MultiWindowConfig.h"
#include "MultiWindowCtrl.h"

using namespace std;
using namespace CWUi;

//#pragma comment( linker , "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" )
CControl * g_ctrlMain[CTRL_MAIN_COUNT];
HWND g_hWndCanvas = NULL;



ULONG BitRange( ULONG aNum , INT aIndexStart , INT aIndexEnd )
{
    ULONG mask = ( 1 << (aIndexEnd-aIndexStart+1) ) - 1;
    return ( aNum >> aIndexStart ) & mask;
}


INT GetTbrUpIndex()
{
    CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
    int i;
    for ( i = 0 ; i < tbrUp->ButtonCount() ; i++ )
    {
        if ( tbrUp->IsChecked(i) )
        {
            return i;
        }
    }    
    return -1;
}




VOID CALLBACK UpdateCanvas( HWND aHWnd , UINT aMsg , UINT_PTR aEvent , DWORD aTime )
{
    if ( CMultiWindowCtrl::GetInstance()->IsStarted() )
    {
        HWND hWndTarget = CMultiWindowCtrl::GetInstance()->GetWindow( 0 );
        if ( hWndTarget != NULL )
        {
            PAINTSTRUCT ps;
            HDC hdcSrc = BeginPaint( aHWnd , &ps );
            HDC hdcDst = GetDC( hWndTarget );

            RECT rect;
            GetWindowRect( hWndTarget , &rect );
            BitBlt( hdcSrc , 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , hdcDst , 0 , 0 , SRCCOPY );
            EndPaint( aHWnd , &ps );
        }
    }
}


LRESULT CALLBACK CanvasProc( HWND aHWnd , UINT aMsg , WPARAM aWParam , LPARAM aLParam )
{    
    switch( aMsg )
    {
        case WM_CREATE :    //Receive when the window is created
        {
            HINSTANCE hInstance = ((LPCREATESTRUCT)aLParam)->hInstance;
            SetTimer( aHWnd , NULL , 1000 , UpdateCanvas );

            //SetWindowLong( aHWnd , GWL_USERDATA , (LONG)&treeList );
            break;
        }    
        case WM_SIZE :    //Receive when windows size is changed
        {
            INT nClientWidth = LOWORD( aLParam );
            INT nClientHeight = HIWORD( aLParam );
            break;
        }
        case WM_KEYDOWN :
        case WM_KEYUP :
        case WM_SYSKEYDOWN :
        case WM_SYSKEYUP :
        case WM_CHAR :
        case WM_LBUTTONDOWN :
        case WM_LBUTTONUP :
        case WM_LBUTTONDBLCLK :
        case WM_RBUTTONDOWN :
        case WM_RBUTTONUP :
        case WM_RBUTTONDBLCLK :
        case WM_MOUSEMOVE :
        {
            //if ( aMsg == WM_KEYDOWN || aMsg == WM_KEYUP )
            //{
            //    WCHAR wzBuf[4096];
            //    bitset<sizeof(ULONG)*8> repeat( (int)BitRange( (ULONG)aLParam , 0 , 15 ) );
            //    bitset<sizeof(ULONG)*8> scan( (int)BitRange( (ULONG)aLParam , 16 , 23 ) );
            //    bitset<sizeof(ULONG)*8> extend( (int)BitRange( (ULONG)aLParam , 24 , 24 ) );
            //    bitset<sizeof(ULONG)*8> reserved( (int)BitRange( (ULONG)aLParam , 25 , 28 ) );
            //    bitset<sizeof(ULONG)*8> context( (int)BitRange( (ULONG)aLParam , 29 , 29 ) );
            //    bitset<sizeof(ULONG)*8> preState( (int)BitRange( (ULONG)aLParam , 30 , 30 ) );
            //    bitset<sizeof(ULONG)*8> transition( (int)BitRange( (ULONG)aLParam , 31 , 31 ) );            
            //    _snwprintf_s( wzBuf , _TRUNCATE , 
            //                    L"%ws: Virtual Code: %lu, Repeat Count: %lu, Scan Code: %lu, Extend: %lu, Reserved: %lu\r\n"
            //                    L"\tContext Code: %lu, Previous State: %lu, Transition State: %lu\r\n" , 
            //                    ( aMsg == WM_KEYDOWN ) ? L"WM_KEYDOWN" : L"WM_KEYUP" ,
            //                    aWParam , repeat.to_ulong() , scan.to_ulong() , extend.to_ulong() , reserved.to_ulong() ,
            //                    context.to_ulong() , preState.to_ulong() , transition.to_ulong() );
            //    ((CEdit *)g_ctrlMain[EDT_SHOW])->AddText( wzBuf );
            //}
            if ( CMultiWindowCtrl::GetInstance()->DispatchMsgIfNeed(aMsg,aWParam,aLParam) )
            {
                return 0;   //Handled
            }
            break;
        }
        case WM_PAINT :    //Receive when something need to be painted
        {
            //if ( CMultiWindowCtrl::GetInstance()->IsStarted() )
            //{
            //    HWND hWndTarget = CMultiWindowCtrl::GetInstance()->GetWindow( 0 );
            //    if ( hWndTarget != NULL )
            //    {
            //        PAINTSTRUCT ps;
            //        HDC hdcSrc = BeginPaint( aHWnd , &ps );
            //        HDC hdcDst = GetDC( hWndTarget );
            //
            //        RECT rect;
            //        GetWindowRect( hWndTarget , &rect );
            //        BitBlt( hdcSrc , 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , hdcDst , 0 , 0 , SRCCOPY );
            //        EndPaint( aHWnd , &ps );
            //    }
            //}
            break;
        }        
        case WM_CLOSE :    //Receive when user try to close the window
        {
            DestroyWindow( aHWnd );
            break;
        }
        case WM_DESTROY :
        {
            PostQuitMessage( 0 );
            break;
        }
        default :
        {
            break;
        }
    }
    return DefWindowProc( aHWnd , aMsg , aWParam , aLParam );
}













LRESULT CALLBACK WndProc( HWND aHWnd , UINT aMsg , WPARAM aWParam , LPARAM aLParam )
{
    static HBRUSH hBrush;
    static HWND hWndMain = aHWnd;

    switch( aMsg )
    {
        case WM_CREATE :    //Receive when the window is created
        {
            HINSTANCE hInstance = ((LPCREATESTRUCT)aLParam)->hInstance;

            //Allocate memory for each control, don't initialize here since some messages will be sent
            for ( INT i = 0 ; i < CTRL_MAIN_COUNT ; i++ )
            {
                if ( i < TBR_MAIN_END )
                {
                    g_ctrlMain[i] = new CToolbar();
                }
                else if ( i < BTN_MAIN_END )
                {
                    g_ctrlMain[i] = new CButton();
                }
                else if ( i < EDT_MAIN_END )
                {
                    g_ctrlMain[i] = new CEdit();
                }
                else
                {
                    CWUtils::ShowDebugMsg( L"Forget to allocate memory for the control" );
                }
            }

            WCHAR wzTbr[TBR_MAIN_COUNT][100] = { L"Up" , L"Left" };
            WCHAR wzBtn[BTN_MAIN_COUNT][100] = { L"Clean" };
            WCHAR wzEdt[EDT_MAIN_COUNT][100] = { L"Log:\r\n" };
            EventProc notifyTbr[TBR_MAIN_COUNT] = { (EventProc)TbrUpNotify , (EventProc)TbrLeftNotify };
            EventProc btnCommand[BTN_MAIN_COUNT] = { (EventProc)BtnCleanCommand };

            WCHAR wzTbrUp[TBR_MAIN_UP_COUNT][100] = { L"Empty" };
            HBITMAP bmpTbrUp[TBR_MAIN_UP_COUNT];
            WCHAR wzBmpTbrUp[TBR_MAIN_UP_COUNT][100] = { L"Pictures\\Empty.bmp" };
            for ( INT i = 0 ; i < TBR_MAIN_UP_COUNT ; i++ )
            {
                bmpTbrUp[i] = (HBITMAP)LoadImage( NULL , wzBmpTbrUp[i] , IMAGE_BITMAP , 0 , 0 , LR_LOADFROMFILE | LR_DEFAULTSIZE );
            }

            WCHAR wzTbrLeft[TBR_MAIN_LEFT_COUNT][100] = { L"Start/Stop" , L"Log" };
            HBITMAP bmpTbrLeft[TBR_MAIN_LEFT_COUNT];
            WCHAR wzBmpTbrLeft[TBR_MAIN_LEFT_COUNT][100] = { L"Pictures\\StartStop.bmp" ,  L"Pictures\\Log.bmp" };
            for ( INT i = 0 ; i < TBR_MAIN_LEFT_COUNT ; i++ )
            {
                bmpTbrLeft[i] = (HBITMAP)LoadImage( NULL , wzBmpTbrLeft[i] , IMAGE_BITMAP , 0 , 0 , LR_LOADFROMFILE | LR_DEFAULTSIZE );
            }


            //Initialize each control here
            for ( INT i = 0 ; i < TBR_MAIN_COUNT ; i++ )
            {
                if ( TBR_MAIN_START + i == TBR_MAIN_UP )
                {
                    ((CToolbar *)g_ctrlMain[TBR_MAIN_START + i])->Init( IDC_TBR_UP + i , aHWnd , hInstance , wzTbr[i] , 0 , 0 , 0 , 0 , CW_TBR_DIR_HORIZONTAL );
                }
                else if ( TBR_MAIN_START + i == TBR_MAIN_LEFT )
                {
                    ((CToolbar *)g_ctrlMain[TBR_MAIN_START + i])->Init( IDC_TBR_LEFT + i , aHWnd , hInstance , wzTbr[i] , 0 , 0 , 0 , 0 , CW_TBR_DIR_VERTICAL );
                }
                else
                {
                    CWUtils::ShowDebugMsg( L"Some toolbar isn't initialize" );
                }
                g_ctrlMain[TBR_MAIN_START + i]->OnNotify = notifyTbr[i];
            }
            for ( INT i = 0 ; i < TBR_MAIN_UP_COUNT ; i++ )
            {
                INT imgIndex = ((CToolbar *)g_ctrlMain[TBR_MAIN_UP])->AddImage( bmpTbrUp[i] , i , RGB(255,255,255) );
                ((CToolbar *)g_ctrlMain[TBR_MAIN_UP])->AddCheckButton( i , IDC_TBR_UP_EMPTY+i , wzTbrUp[i] , imgIndex , TRUE );
            }
            for ( INT i = 0 ; i < TBR_MAIN_LEFT_COUNT ; i++ )
            {
                INT imgIndex = ((CToolbar *)g_ctrlMain[TBR_MAIN_LEFT])->AddImage( bmpTbrLeft[i] , i , RGB(255,255,255) );
                ((CToolbar *)g_ctrlMain[TBR_MAIN_LEFT])->AddButton( i , IDC_TBR_LEFT_START_STOP+i , wzTbrLeft[i] , imgIndex );
            }
            
            for ( INT i = 0 ; i < EDT_MAIN_COUNT ; i++ )
            {
                if ( EDT_SHOW != ( EDT_MAIN_START + i ) )
                {
                    ((CEdit *)g_ctrlMain[EDT_MAIN_START + i])->Init( IDC_EDT_INPUT + i , aHWnd , hInstance , wzEdt[i] );
                }
                else
                {
                    ((CEdit *)g_ctrlMain[EDT_MAIN_START + i])->Init( IDC_EDT_INPUT + i , aHWnd , hInstance , wzEdt[i] ,
                                                                     0 , 0 , 0 , 0 , WS_CHILD | WS_VISIBLE | WS_BORDER | WS_DISABLED |
                                                                     ES_LEFT | ES_MULTILINE | ES_READONLY | 
                                                                     WS_HSCROLL | WS_VSCROLL | ES_AUTOHSCROLL | ES_AUTOVSCROLL );
                    ((CEdit *)g_ctrlMain[EDT_MAIN_START + i])->SetMaxLength( -1 );
                }
            }

            for ( INT i = 0 ; i < BTN_MAIN_COUNT ; i++ )
            {
                ((CButton *)g_ctrlMain[BTN_MAIN_START + i])->Init( IDC_BTN_CLEAN + i , aHWnd , hInstance , wzBtn[i] );
                g_ctrlMain[BTN_MAIN_START + i]->OnCommand = btnCommand[i];
            }

            hBrush = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
            break;
        }
        case WM_COMMAND :    //Receive when some commands is sent
        {
            WORD wCtrlId = LOWORD( aWParam );
            for ( INT i = 0 ; i < CTRL_MAIN_COUNT ; i++ )
            {
                if ( ( g_ctrlMain[i]->OnCommand != NULL ) && ( wCtrlId == g_ctrlMain[i]->Id() ) )
                {
                    if ( g_ctrlMain[i]->OnCommand( aHWnd , aWParam , aLParam , NULL ) )
                        break;
                    
                }
            }
            break;
        }
        case WM_NOTIFY :    //Receive when user do something
        {
            HWND hwndFrom = ((NMHDR *)aLParam)->hwndFrom;
            for ( INT i = 0 ; i < CTRL_MAIN_COUNT ; i++ )
            {
                if ( ( g_ctrlMain[i]->OnNotify != NULL ) && ( hwndFrom == g_ctrlMain[i]->Self() ) )
                {
                    if ( g_ctrlMain[i]->OnNotify( aHWnd , aWParam , aLParam , NULL ) )
                        break;
                }
            }
            break;
        }
        case WM_SIZE :    //Receive when windows size is changed
        {
            INT nWidth = LOWORD( aLParam );
            INT nHeight = HIWORD( aLParam );

            CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
            CToolbar * tbrLeft = (CToolbar *)g_ctrlMain[TBR_MAIN_LEFT];

            tbrLeft->SetWidthHeight( tbrLeft->ButtonWidth() , nHeight );
            tbrLeft->SetPos( 0 , 0 );

            tbrUp->SetWidthHeight( nWidth - tbrLeft->PosX() - tbrLeft->Width() - 10 , tbrUp->ButtonHeight() );
            tbrUp->SetPos( tbrLeft->PosX() + tbrLeft->Width() +10 , 0 );
        
            for ( INT i = 0 ; i < EDT_MAIN_COUNT - 1 ; i++ )
            {
                g_ctrlMain[EDT_MAIN_START + i]->SetWidthHeight( nWidth / 6 , 20 );
            }
            for ( INT i = 0 ; i < BTN_MAIN_COUNT ; i++ )
            {
                g_ctrlMain[BTN_MAIN_START + i]->SetWidthHeight( nWidth / 6 , 20 );
            }
        
            g_ctrlMain[BTN_CLEAN]->SetPos( nWidth - 10 - g_ctrlMain[BTN_CLEAN]->Width() , tbrUp->PosY() + tbrUp->Height() + 10 );
        
            g_ctrlMain[EDT_SHOW]->SetWidthHeight( nWidth - tbrLeft->PosX() - tbrLeft->Width() - 20 , nHeight - g_ctrlMain[BTN_CLEAN]->PosY() - g_ctrlMain[BTN_CLEAN]->Height() - 20 );
            g_ctrlMain[EDT_SHOW]->SetPos( tbrLeft->PosX() + tbrLeft->Width() + 10 , g_ctrlMain[BTN_CLEAN]->PosY() + g_ctrlMain[BTN_CLEAN]->Height() + 10 );
        
            if ( GetTbrUpIndex() > 0 )
            {
                INT nCanvasPosX = tbrLeft->PosX() + tbrLeft->Width() + 10;
                INT nCanvasPosY = tbrUp->PosY() + tbrUp->Height() + 10;
                MoveWindow( g_hWndCanvas , nCanvasPosX , nCanvasPosY , nWidth - nCanvasPosX - 10 , nHeight - nCanvasPosY - 10 , TRUE );
                ShowWindow( g_hWndCanvas , SW_SHOW );
            }
            else
            {
                ShowWindow( g_hWndCanvas , SW_MINIMIZE );
            }
            break;
        }
        case WM_PAINT :    //Receive when something need to be painted
        {
            break;
        }
        case WM_CTLCOLORSTATIC :    //Receive when drawing static(label) or edit
        {
            //Paint background color to window's color
            //LONG lId = GetWindowLong( (HWND)aLParam , GWL_ID );
            //
            //if ( g_ctrlMain[LAB_MAIN_START]->Id() <= lId && lId < g_ctrlMain[LAB_MAIN_END]->Id() )
            //{
            //    SetBkColor( (HDC)aWParam , GetSysColor(COLOR_WINDOW) );
            //    return (LRESULT)hBrush;
            //}
            break;
        }
        case WM_SYSCOLORCHANGE :    //Receive when a change is made to a system color setting
        {
            DeleteObject( hBrush );
            hBrush = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );
            return 0 ;
        }         
        case WM_CLOSE :    //Receive when user try to close the window
        {
            CMultiWindowCtrl::GetInstance()->Stop();
            DeleteObject( hBrush );
            DestroyWindow( aHWnd );
            break;
        }
        case WM_DESTROY :
        {
            PostQuitMessage( 0 );
            break;
        }
        default :
        {
            break;
        }
    }
    return DefWindowProc( aHWnd , aMsg , aWParam , aLParam );
}




INT WINAPI wWinMain( HINSTANCE aHInstance , HINSTANCE aHPrevInstance , LPWSTR aCmdLine , INT aCmdShow )
{
    UNREFERENCED_PARAMETER( aHPrevInstance );
    UNREFERENCED_PARAMETER( aCmdLine );

    CONST WCHAR wzMainClassName[] = L"MainClass" , wzCanvasClassName[] = L"CanvasClass";
    WNDCLASSEXW wndMainClass , wndCanvasClass;

    //Register MainClass
    wndMainClass.cbSize        = sizeof( WNDCLASSEX );
    wndMainClass.style         = 0;    //CS_HREDRAW | CS_VREDRAW
    wndMainClass.lpfnWndProc   = WndProc;
    wndMainClass.cbClsExtra    = 0;
    wndMainClass.cbWndExtra    = 0;
    wndMainClass.hInstance     = aHInstance;
    wndMainClass.hIcon         = LoadIconW( NULL , IDI_APPLICATION );
    wndMainClass.hCursor       = LoadCursorW( NULL , IDC_ARROW );
    wndMainClass.hbrBackground = (HBRUSH) ( COLOR_WINDOW + 1 );
    wndMainClass.lpszMenuName  = NULL;
    wndMainClass.lpszClassName = wzMainClassName;
    wndMainClass.hIconSm       = LoadIconW( NULL , IDI_APPLICATION );
    if ( ! RegisterClassExW( &wndMainClass ) )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to register wndMainClass" );
        return -1;
    }

    //Register CanvasClass
    wndCanvasClass.cbSize        = sizeof( WNDCLASSEX );
    wndCanvasClass.style         = CS_DBLCLKS;
    wndCanvasClass.lpfnWndProc   = CanvasProc;
    wndCanvasClass.cbClsExtra    = 0;
    wndCanvasClass.cbWndExtra    = 0;
    wndCanvasClass.hInstance     = aHInstance;
    wndCanvasClass.hIcon         = NULL;
    wndCanvasClass.hCursor       = LoadCursor( NULL , IDC_CROSS );
    wndCanvasClass.hbrBackground = (HBRUSH) ( COLOR_WINDOW + 1 );
    wndCanvasClass.lpszMenuName  = NULL;
    wndCanvasClass.lpszClassName = wzCanvasClassName;
    wndCanvasClass.hIconSm       = NULL;
    if ( ! RegisterClassEx( &wndCanvasClass ) )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to register wndCanvasClass" );
        return -1;
    }

    HWND hWndMain = CreateWindowExW( WS_EX_CLIENTEDGE , wzMainClassName , L"MultiWindowHelper by winest" , WS_OVERLAPPEDWINDOW ,
                                 CW_USEDEFAULT , CW_USEDEFAULT , 800+10+64+10+10+10+10  , 450+10+64+10+10+32+10 , NULL , NULL , aHInstance , NULL );
    if ( NULL == hWndMain )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to create hWndMain" );
        return 0;
    }
    //Create canvas area
    g_hWndCanvas = CreateWindowEx( WS_EX_CLIENTEDGE | WS_EX_COMPOSITED , wzCanvasClassName , L"Canvas" ,
                                   WS_CHILD | WS_VISIBLE , 0 , 0 , 0  , 0 , hWndMain , NULL , aHInstance , NULL );
    if ( g_hWndCanvas == NULL )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to create g_hWndCanvas" );
        return -1;
    }

    UpdateWindow( hWndMain );
    ShowWindow( hWndMain, aCmdShow );

    MSG msg;
    while ( 0 < GetMessageW( &msg , NULL , 0 , 0 ) )
    {
        //if ( ! IsDialogMessage( hWndMain , &msg ) && ! IsDialogMessage( hWndCanvas , &msg ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    return (INT)msg.wParam;
}