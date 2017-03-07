#include "stdafx.h"
#include "MultiWindowHelper.h"
#include <bitset>

#include "CWEdit.h"
#include "CWButton.h"
#include "CWButtonProcs.h"

#include "CWGeneralUtils.h"

using namespace std;
using namespace CWUi;

#pragma comment( linker , "/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"" )

CControl * g_ctrlMain[CTRL_MAIN_COUNT];



ULONG BitRange( ULONG aNum , INT aIndexStart , INT aIndexEnd )
{
	ULONG mask = ( 1 << (aIndexEnd-aIndexStart+1) ) - 1;
	return ( aNum >> aIndexStart ) & mask;
}


LRESULT CALLBACK WndProc( HWND aHWnd , UINT aMsg , WPARAM aWParam , LPARAM aLParam )
{
    static HBRUSH hBrush;
    static HWND * hWnds;

    switch( aMsg )
    {
        case WM_CREATE :    //Receive when the window is created
        {
            HINSTANCE hInstance = ((LPCREATESTRUCT)aLParam)->hInstance;

            //Allocate memory for each control, don't initialize here since some messages will be sent
            for ( INT i = 0 ; i < CTRL_MAIN_COUNT ; i++ )
            {
                if ( i < EDT_MAIN_END )
                {
                    g_ctrlMain[i] = new CEdit();
                }
                else if ( i < BTN_MAIN_END )
                {
                    g_ctrlMain[i] = new CButton();
                }
                else
                {
                    CWUtils::ShowDebugMsg( L"Forget to allocate memory for the control" );
                }
            }

            WCHAR wzEdt[EDT_MAIN_COUNT][100] = { L"(Something)" };
            WCHAR wzBtn[BTN_MAIN_COUNT][100] = { L"Start" , L"Clean" };
            EventProc btnCommand[BTN_MAIN_COUNT] = { (EventProc)BtnStartCommand , (EventProc)BtnCleanCommand };

            //Initialize each control here
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
                ((CButton *)g_ctrlMain[BTN_MAIN_START + i])->Init( IDC_BTN_START + i , aHWnd , hInstance , wzBtn[i] );
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
            if ( g_hWnds != NULL )
            {
                for ( size_t i = 0 ; i < g_vecWindowNames.size() ; i++ )
                {
                    if ( g_hWnds[i] != NULL )
                    {
                        PostMessageW( g_hWnds[i] , aMsg , aWParam , aLParam );
                    }
                }
            }
            break;
        }
        case WM_SIZE :    //Receive when windows size is changed
        {
            INT nWidth = LOWORD( aLParam );
            INT nHeight = HIWORD( aLParam );

            for ( INT i = 0 ; i < EDT_MAIN_COUNT - 1 ; i++ )
            {
                g_ctrlMain[EDT_MAIN_START + i]->SetWidthHeight( nWidth / 6 , 20 );
            }
            for ( INT i = 0 ; i < BTN_MAIN_COUNT ; i++ )
            {
                g_ctrlMain[BTN_MAIN_START + i]->SetWidthHeight( nWidth / 6 , 20 );
            }

            
            g_ctrlMain[BTN_START]->SetPos( ( nWidth - g_ctrlMain[BTN_START]->Width() ) / 2 , 10 );
            g_ctrlMain[BTN_CLEAN]->SetPos( nWidth - 10 - g_ctrlMain[BTN_CLEAN]->Width() , g_ctrlMain[BTN_START]->PosY() );

            g_ctrlMain[EDT_SHOW]->SetWidthHeight( nWidth - 20 , nHeight - g_ctrlMain[BTN_CLEAN]->PosY() - g_ctrlMain[BTN_CLEAN]->Height() - 20 );
            g_ctrlMain[EDT_SHOW]->SetPos( 10 , g_ctrlMain[BTN_CLEAN]->PosY() + g_ctrlMain[BTN_CLEAN]->Height() + 10 );

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
        //case WM_RBUTTONDOWN :    //Receive when user click right mouse
        //{
        //    WCHAR wzFileName[MAX_PATH];
        //    HINSTANCE hInstance = GetModuleHandle( NULL );
        //
        //    GetModuleFileNameW( hInstance , wzFileName , MAX_PATH );
        //    MessageBoxW( aHWnd , wzFileName , L"This program is: " , MB_OK | MB_ICONINFORMATION );
        //    break;
        //}            
        case WM_CLOSE :    //Receive when user try to close the window
        {
            if ( g_hWnds != NULL )
            {
                delete [] g_hWnds;
                g_hWnds = NULL;
            }
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

    CONST WCHAR wzClassName[] = L"MyWindowClass";
    WNDCLASSEXW wndClass;
    wndClass.cbSize        = sizeof( WNDCLASSEX );
    wndClass.style         = 0;    //CS_HREDRAW | CS_VREDRAW
    wndClass.lpfnWndProc   = WndProc;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hInstance     = aHInstance;
    wndClass.hIcon         = LoadIcon( NULL , IDI_APPLICATION );
    wndClass.hCursor       = LoadCursor( NULL , IDC_ARROW );
    wndClass.hbrBackground = (HBRUSH) ( COLOR_WINDOW + 1 );
    wndClass.lpszMenuName  = NULL;
    wndClass.lpszClassName = wzClassName;
    wndClass.hIconSm       = LoadIcon( NULL , IDI_APPLICATION );

    if ( ! RegisterClassExW( &wndClass ) )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to register wndClass" );
        return -1;
    }

    HWND hWnd = CreateWindowExW( WS_EX_CLIENTEDGE , wzClassName , L"MultiWindowHelper by winest" , WS_OVERLAPPEDWINDOW ,
                                 CW_USEDEFAULT , CW_USEDEFAULT , 800  , 600 , NULL , NULL , aHInstance , NULL );
    if ( NULL == hWnd )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to create hWnd" );
        return 0;
    }

    UpdateWindow( hWnd );
    ShowWindow( hWnd, aCmdShow );

    MSG msg;
    while ( 0 < GetMessageW( &msg , NULL , 0 , 0 ) )
    {
        if ( ! IsDialogMessage( hWnd , &msg ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    return (INT)msg.wParam;
}