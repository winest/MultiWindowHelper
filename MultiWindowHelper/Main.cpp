#include "stdafx.h"
#include "Main.h"
#include <bitset>
#include <gl/gl.h>

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
HWND g_hWndMain = NULL;
HWND g_hWndCanvas = NULL;



ULONG BitRange( ULONG aNum , INT aIndexStart , INT aIndexEnd )
{
    ULONG mask = ( 1 << (aIndexEnd-aIndexStart+1) ) - 1;
    return ( aNum >> aIndexStart ) & mask;
}


INT GetTbrUpIndex()
{
    CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
    INT nRet = -1;
    for ( INT i = 0 ; i < tbrUp->ButtonCount() ; i++ )
    {
        if ( tbrUp->IsChecked(i) )
        {
            nRet = i;
            break;
        }
    }    
    return nRet;
}




VOID CALLBACK UpdateCanvas( HWND aHWnd , UINT aMsg , UINT_PTR aEvent , DWORD aTime )
{
    UNREFERENCED_PARAMETER( aMsg );
    UNREFERENCED_PARAMETER( aEvent );
    UNREFERENCED_PARAMETER( aTime );

    static INT nLastWidth = GetSystemMetrics(SM_CXSCREEN) , nLastHeight = GetSystemMetrics(SM_CYSCREEN);

    if ( CMultiWindowCtrl::GetInstance()->IsStarted() )
    {
		INT nIndex = GetTbrUpIndex();
        HWND hWndTarget = CMultiWindowCtrl::GetInstance()->GetWindow( nIndex - 1 );
        if ( hWndTarget != NULL )
        {
            RECT rectCanvasWnd , rectCanvasClient = { 0 , 0 };
            RECT rectTarget;
            GetWindowRect( aHWnd , &rectCanvasWnd );
            GetClientRect( aHWnd , &rectCanvasClient );
            GetWindowRect( hWndTarget , &rectTarget );
            INT nTargetWidth = rectTarget.right - rectTarget.left;
            INT nTargetHeight = rectTarget.bottom - rectTarget.top;
            rectCanvasClient.right = max( nTargetWidth , nLastWidth );// + ( ( rectCanvasWnd.right - rectCanvasWnd.left ) - rectCanvasClient.right );
            rectCanvasClient.bottom = max( nTargetHeight , nLastHeight );// + ( ( rectCanvasWnd.bottom - rectCanvasWnd.top ) - rectCanvasClient.bottom );
            nLastWidth = rectCanvasClient.right;
            nLastHeight = rectCanvasClient.bottom;
            InvalidateRect( aHWnd , &rectCanvasClient , TRUE );
        }
    }
}


LRESULT CALLBACK CanvasProc( HWND aHWnd , UINT aMsg , WPARAM aWParam , LPARAM aLParam )
{    
    static UINT_PTR pTimerId = NULL;

    switch( aMsg )
    {
        case WM_CREATE :    //Receive when the window is created
        {
            //HINSTANCE hInstance = ((LPCREATESTRUCT)aLParam)->hInstance;
            //SetWindowLong( aHWnd , GWL_USERDATA , (LONG)&treeList );
            break;
        }    
        case WM_SIZE :    //Receive when windows size is changed
        {
            //INT nClientWidth = LOWORD( aLParam );
            //INT nClientHeight = HIWORD( aLParam );
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
            //    ((CEdit *)g_ctrlMain[EDT_LOG])->AddText( wzBuf );
            //}
            if ( CMultiWindowCtrl::GetInstance()->DispatchMsgIfNeed(aMsg,aWParam,aLParam) )
            {
                return 0;   //Handled
            }
            break;
        }
        case WM_PAINT :    //Receive when something need to be painted
        {
            if ( CMultiWindowCtrl::GetInstance()->IsStarted() )
            {
                INT nIndex = GetTbrUpIndex();
                HWND hWndTarget = CMultiWindowCtrl::GetInstance()->GetWindow( nIndex - 1 );
                if ( hWndTarget != NULL )
                {
                    PAINTSTRUCT paint;
                    HDC hdcCanvas = BeginPaint( aHWnd , &paint );
                    HDC hdcTarget = GetDC( hWndTarget );
                    RECT rect;

                    
                    //PIXELFORMATDESCRIPTOR pfd = { 
                    //    sizeof(PIXELFORMATDESCRIPTOR), 1, 
                    //    PFD_SUPPORT_GDI | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, PFD_TYPE_RGBA, 
                    //    24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 }; 
                    //INT nPixelFormat = ChoosePixelFormat( hdcTarget , &pfd );
                    
                    //if ( GetDeviceCaps( hdcTarget , RASTERCAPS )  )

                    BOOL bDraw = TRUE;
                    BOOL bNeedToMinimize = FALSE;
                    ANIMATIONINFO aniInfo = {};
                    INT nLastMinAni = 0;
                    if ( IsIconic( hWndTarget ) )
                    {
                        if ( CMultiWindowConfig::GetInstance()->IsRetrieveEvenMinimized() == FALSE )
                        {
                            bDraw = FALSE;
                        }
                        else
                        {
                            aniInfo.cbSize = sizeof(ANIMATIONINFO);
                            SystemParametersInfoW( SPI_GETANIMATION , sizeof(ANIMATIONINFO) , &aniInfo , 0 );
                            nLastMinAni = aniInfo.iMinAnimate;
                            if ( aniInfo.iMinAnimate )
                            {
                                aniInfo.iMinAnimate = 0;
                                SystemParametersInfoW( SPI_SETANIMATION , sizeof(ANIMATIONINFO) , &aniInfo , 0 );
                            }
                            SetLayeredWindowAttributes( hWndTarget , 0 , 0 ,  LWA_ALPHA );
                            ShowWindow( hWndTarget , SW_SHOWNOACTIVATE );
                            bNeedToMinimize = TRUE;
                        }
                    }
                    if ( bDraw )
                    {
                        switch ( CMultiWindowConfig::GetInstance()->GetDisplayMode() )
                        {
                            case MULTI_WINDOW_DISPLAY_MODE_BITBLT :
                            {
                                GetClientRect( hWndTarget , &rect );
                                BitBlt( hdcCanvas , 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , hdcTarget , 0 , 0 , SRCCOPY );
                                break;
                            }
                            case MULTI_WINDOW_DISPLAY_MODE_PRINTWINDOW :
                            {
                                PrintWindow( hWndTarget , hdcCanvas , 0 );
                                break;
                            }
                            case MULTI_WINDOW_DISPLAY_MODE_GLREADPIXELS :
                            {
                                HDC hdcMemCanvas = CreateCompatibleDC( hdcCanvas );
                                HDC hdcMemTarget = CreateCompatibleDC( hdcTarget );
                                GetClientRect( hWndTarget , &rect );
                                INT nWidth = rect.right - rect.left;
                                INT nHeight = rect.bottom - rect.top;

                                BITMAPINFO bitmapInfo = {};
                                bitmapInfo.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
                                bitmapInfo.bmiHeader.biPlanes      = 1;
                                bitmapInfo.bmiHeader.biBitCount    = 32;
                                bitmapInfo.bmiHeader.biCompression = BI_RGB;
                                bitmapInfo.bmiHeader.biWidth       = nWidth;
                                bitmapInfo.bmiHeader.biHeight      = nHeight;
                                bitmapInfo.bmiHeader.biSizeImage   = nWidth * nHeight * 4; // Size 4, assuming RGBA from OpenGL

                                VOID * bmBits = NULL;
                                HBITMAP memBM = CreateDIBSection( NULL , &bitmapInfo , DIB_RGB_COLORS , &bmBits , NULL, 0 );
                                HGDIOBJ objLast = SelectObject( hdcMemCanvas , memBM );
                                //HGDIOBJ obj        = SelectObject( hdcMemCanvas , memBM );
                                glReadPixels( 0 , 0 , nWidth , nHeight , GL_BGRA_EXT , GL_UNSIGNED_BYTE , bmBits );
                                
                                //HGDIOBJ prevBitmap = SelectObject( hdcMemTarget , memBM );
                                BitBlt( hdcCanvas , 0 , 0 , nWidth , nHeight , hdcMemCanvas , 0 , 0 , SRCCOPY );
                                SelectObject( hdcMemTarget , objLast );
                                DeleteObject( memBM );
                                DeleteDC( hdcMemTarget );   
                                DeleteDC( hdcMemCanvas );

                                //HBITMAP hBitmap = CreateCompatibleBitmap( hdcTarget , rect.right - rect.left , rect.bottom - rect.top );
                                //glReadPixels( 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , GL_RGB , GL_UNSIGNED_BYTE , )
                                //HGDIOBJ objLast = SelectObject( hdcMemTarget , hBitmap );
                                //BitBlt( hdcCanvas , 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , hdcTarget , 0 , 0 , SRCCOPY | CAPTUREBLT );
                                //SelectObject( hdcMemTarget , objLast );
                                //DeleteObject( hBitmap );
                                //DeleteDC( hdcMemTarget );
                                break;
                            }
                            default :
                            {
                                DrawTextW( hdcCanvas , L"Unknown DisplayMode" , -1 , &rect , DT_CENTER | DT_VCENTER | DT_END_ELLIPSIS | DT_WORDBREAK );
                                CWUtils::ShowDebugMsg( L"Unknown DisplayMode" );
                                break;
                            }
                        }
                    }
                    if ( bNeedToMinimize )
                    {
                        ShowWindow( hWndTarget , SW_MINIMIZE );
                        SetLayeredWindowAttributes( hWndTarget , 0 , 255 ,  LWA_ALPHA );
                        if ( nLastMinAni )
                        {
                            aniInfo.iMinAnimate = nLastMinAni;
                            SystemParametersInfoW( SPI_SETANIMATION , sizeof(ANIMATIONINFO) , &aniInfo , 0 );
                        }
                    }

                    //HDC hdcTarget = GetWindowDC( hWndTarget );
                    //PrintWindow( hWndTarget , hdcCanvas , 0 );
                    //RECT rect;
                    //GetWindowRect( hWndTarget , &rect );
                    //BitBlt( hdcCanvas , 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , hdcTarget , 0 , 0 , SRCCOPY | CAPTUREBLT );

                    //HDC hdcTarget = GetDC( NULL );
                    //RECT rect;
                    //GetClientRect( hWndTarget , &rect );
                    //POINT ptTarget = { 0 , 0 };
                    //ClientToScreen( hWndTarget , &ptTarget );
                    //BitBlt( hdcCanvas , 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , hdcTarget , ptTarget.x , ptTarget.y , SRCCOPY | CAPTUREBLT );

                    //HDC hdcTarget = GetDC( hWndTarget );
                    //HDC hdcMemTarget = CreateCompatibleDC( hdcTarget );
                    //RECT rect;
                    //GetClientRect( hWndTarget , &rect );
                    //HBITMAP hBitmap = CreateCompatibleBitmap( hdcTarget , rect.right - rect.left , rect.bottom - rect.top );
                    //HGDIOBJ objLast = SelectObject( hdcMemTarget , hBitmap );
                    //BitBlt( hdcCanvas , 0 , 0 , rect.right - rect.left , rect.bottom - rect.top , hdcTarget , 0 , 0 , SRCCOPY | CAPTUREBLT );
                    //SelectObject( hdcMemTarget , objLast );
                    //DeleteObject( hBitmap );
                    //DeleteDC( hdcMemTarget );

                    ReleaseDC( hWndTarget , hdcTarget );
                    EndPaint( aHWnd , &paint );
                }
            }
            break;
        }        
        case WM_SHOWWINDOW :
        {
            CEdit * edtLog = (CEdit *)g_ctrlMain[EDT_LOG];

            if ( aWParam == TRUE )
            {
                edtLog->AddText( L"WM_SHOWWINDOW TRUE()\r\n" );
                pTimerId = SetTimer( aHWnd , NULL , (UINT)(1000.0 / CMultiWindowConfig::GetInstance()->GetFps()) , UpdateCanvas );
            }
            else
            {
                edtLog->AddText( L"WM_SHOWWINDOW FALSE()\r\n" );
                KillTimer( aHWnd , pTimerId );
            }
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
    static HWND g_hWndMain = aHWnd;

    switch( aMsg )
    {
        case WM_CREATE :    //Receive when the window is created
        {
            HINSTANCE hInstance = ((LPCREATESTRUCT)aLParam)->hInstance;
            hBrush = CreateSolidBrush( GetSysColor(COLOR_WINDOW) );

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

            WCHAR wzTbr[TBR_MAIN_COUNT][100];
            EventProc notifyTbr[TBR_MAIN_COUNT] = { (EventProc)TbrUpNotify , (EventProc)TbrLeftNotify };

            WCHAR wzTbrUp[TBR_MAIN_UP_COUNT][100] = { L"Log" };
            HBITMAP bmpTbrUp[TBR_MAIN_UP_COUNT];
            WCHAR wzBmpTbrUp[TBR_MAIN_UP_COUNT][100] = { L"Pictures\\Log.bmp" };
            for ( INT i = 0 ; i < TBR_MAIN_UP_COUNT ; i++ )
            {
                bmpTbrUp[i] = (HBITMAP)LoadImageW( NULL , wzBmpTbrUp[i] , IMAGE_BITMAP , 0 , 0 , LR_LOADFROMFILE | LR_DEFAULTSIZE );
            }

            WCHAR wzTbrLeft[TBR_MAIN_LEFT_COUNT][100] = { L"Start" , L"About" };
            HBITMAP bmpTbrLeft[TBR_MAIN_LEFT_COUNT];
            WCHAR wzBmpTbrLeft[TBR_MAIN_LEFT_COUNT][100] = { L"Pictures\\StartStop.bmp" ,  L"Pictures\\About.bmp" };
            for ( INT i = 0 ; i < TBR_MAIN_LEFT_COUNT ; i++ )
            {
                bmpTbrLeft[i] = (HBITMAP)LoadImageW( NULL , wzBmpTbrLeft[i] , IMAGE_BITMAP , 0 , 0 , LR_LOADFROMFILE | LR_DEFAULTSIZE );
            }


            
            WCHAR wzBtn[BTN_MAIN_COUNT][100] = { L"Clean" };
            WCHAR wzEdt[EDT_MAIN_COUNT][100] = { L"Log:\r\n" };
            EventProc btnCommand[BTN_MAIN_COUNT] = { (EventProc)BtnCleanCommand };

            //Initialize each control here
            for ( INT i = 0 ; i < TBR_MAIN_COUNT ; i++ )
            {
                if ( TBR_MAIN_START + i == TBR_MAIN_UP )
                {
                    ((CToolbar *)g_ctrlMain[TBR_MAIN_START + i])->Init( IDC_TBR_UP , aHWnd , hInstance , wzTbr[i] );
                }
                else if ( TBR_MAIN_START + i == TBR_MAIN_LEFT )
                {
                    ((CToolbar *)g_ctrlMain[TBR_MAIN_START + i])->Init( IDC_TBR_LEFT , aHWnd , hInstance , wzTbr[i] , 0 , 0 , 2 , 0 , 0 , 0 , 0 , CW_TBR_DIR_VERTICAL );
                }
                else
                {
                    CWUtils::ShowDebugMsg( L"Some toolbar isn't initialize" );
                }
                g_ctrlMain[TBR_MAIN_START + i]->OnNotify = notifyTbr[i];
            }
            for ( INT i = 0 ; i < TBR_MAIN_UP_COUNT ; i++ )
            {
                INT imgIndex = ((CToolbar *)g_ctrlMain[TBR_MAIN_UP])->AddImage( bmpTbrUp[i] , RGB(255,255,255) );
                ((CToolbar *)g_ctrlMain[TBR_MAIN_UP])->AddCheckButton( i , IDC_TBR_UP_LOG+i , wzTbrUp[i] , imgIndex , TRUE );
                DeleteObject( bmpTbrUp[i] );    //Since image is now copied to internal
            }
            for ( INT i = 0 ; i < TBR_MAIN_LEFT_COUNT ; i++ )
            {
                INT imgIndex = ((CToolbar *)g_ctrlMain[TBR_MAIN_LEFT])->AddImage( bmpTbrLeft[i] , RGB(255,255,255) );
                ((CToolbar *)g_ctrlMain[TBR_MAIN_LEFT])->AddButton( i , IDC_TBR_LEFT_START_STOP+i , wzTbrLeft[i] , imgIndex );
                DeleteObject( bmpTbrLeft[i] );  //Since image is now copied to internal
            }
            
            for ( INT i = 0 ; i < EDT_MAIN_COUNT ; i++ )
            {
                if ( EDT_LOG != ( EDT_MAIN_START + i ) )
                {
                    ((CEdit *)g_ctrlMain[EDT_MAIN_START + i])->Init( IDC_EDT_LOG + i , aHWnd , hInstance , wzEdt[i] );
                }
                else
                {
                    ((CEdit *)g_ctrlMain[EDT_MAIN_START + i])->Init( IDC_EDT_LOG + i , aHWnd , hInstance , wzEdt[i] ,
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

            
            
            //Set initial state
            ((CToolbar *)g_ctrlMain[TBR_MAIN_UP])->SetCheck( 0 , TRUE );
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
                    {
                        break;
                    }
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
                    {
                        break;
                    }
                }
            }
            break;
        }
        case WM_SIZE :    //Receive when windows size is changed
        {
            INT nWidth = LOWORD( aLParam );
            INT nHeight = HIWORD( aLParam );
            CEdit * edtLog = (CEdit *)g_ctrlMain[EDT_LOG];

            CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
            CToolbar * tbrLeft = (CToolbar *)g_ctrlMain[TBR_MAIN_LEFT];

            tbrLeft->SetWidthHeight( tbrLeft->ButtonWidth() , nHeight );
            tbrLeft->SetPos( 0 , 0 );

            tbrUp->SetWidthHeight( nWidth - tbrLeft->PosX() - tbrLeft->Width() , tbrUp->ButtonHeight() );
            tbrUp->SetPos( tbrLeft->PosX() + tbrLeft->Width() , 0 );
        
            for ( INT i = 0 ; i < EDT_MAIN_COUNT - 1 ; i++ )
            {
                g_ctrlMain[EDT_MAIN_START + i]->SetWidthHeight( nWidth / 6 , 20 );
            }
            for ( INT i = 0 ; i < BTN_MAIN_COUNT ; i++ )
            {
                g_ctrlMain[BTN_MAIN_START + i]->SetWidthHeight( nWidth / 6 , 20 );
            }
        
            g_ctrlMain[BTN_CLEAN]->SetPos( nWidth - 10 - g_ctrlMain[BTN_CLEAN]->Width() , tbrUp->PosY() + tbrUp->Height() + 10 );
        
            g_ctrlMain[EDT_LOG]->SetWidthHeight( nWidth - tbrLeft->PosX() - tbrLeft->Width() - 20 , nHeight - g_ctrlMain[BTN_CLEAN]->PosY() - g_ctrlMain[BTN_CLEAN]->Height() - 20 );
            g_ctrlMain[EDT_LOG]->SetPos( tbrLeft->PosX() + tbrLeft->Width() + 10 , g_ctrlMain[BTN_CLEAN]->PosY() + g_ctrlMain[BTN_CLEAN]->Height() + 10 );
        
            if ( GetTbrUpIndex() > 0 )
            {
                INT nCanvasPosX = tbrLeft->PosX() + tbrLeft->Width() + 10;
                INT nCanvasPosY = tbrUp->PosY() + tbrUp->Height() + 10;
                MoveWindow( g_hWndCanvas , nCanvasPosX , nCanvasPosY , nWidth - nCanvasPosX - 10 , nHeight - nCanvasPosY - 10 , TRUE );
                ShowWindow( g_hWndCanvas , SW_SHOW );
                edtLog->AddText( L"Resize to %dx%d\r\n" , nWidth - nCanvasPosX - 10 , nHeight - nCanvasPosY - 10 );
            }
            else
            {
                ShowWindow( g_hWndCanvas , SW_HIDE );
                edtLog->AddText( L"Hiding canvas\r\n" );
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
    wndMainClass.style         = 0;    //CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS
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
    wndCanvasClass.hCursor       = LoadCursorW( NULL , IDC_ARROW );
    wndCanvasClass.hbrBackground = (HBRUSH) ( COLOR_WINDOW + 1 );
    wndCanvasClass.lpszMenuName  = NULL;
    wndCanvasClass.lpszClassName = wzCanvasClassName;
    wndCanvasClass.hIconSm       = NULL;
    if ( ! RegisterClassEx( &wndCanvasClass ) )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to register wndCanvasClass" );
        return -1;
    }

    g_hWndMain = CreateWindowExW( WS_EX_CLIENTEDGE , wzMainClassName , L"MultiWindowHelper by winest" , WS_OVERLAPPEDWINDOW ,
                                     CW_USEDEFAULT , CW_USEDEFAULT , 800+64+10+10  , 450+64+10+20+10+10 , NULL , NULL , aHInstance , NULL );
    if ( NULL == g_hWndMain )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to create g_hWndMain" );
        return 0;
    }
    //Create canvas area
    g_hWndCanvas = CreateWindowEx( WS_EX_CLIENTEDGE | WS_EX_COMPOSITED , wzCanvasClassName , L"Canvas" ,
                                   WS_CHILD | WS_VISIBLE , 0 , 0 , 0  , 0 , g_hWndMain , NULL , aHInstance , NULL );
    if ( g_hWndCanvas == NULL )
    {
        CWUtils::ShowDebugMsg( L"Error: failed to create g_hWndCanvas" );
        return -1;
    }

    UpdateWindow( g_hWndMain );
    ShowWindow( g_hWndMain, aCmdShow );

    MSG msg;
    while ( 0 < GetMessageW( &msg , NULL , 0 , 0 ) )
    {
        //if ( ! IsDialogMessage( g_hWndMain , &msg ) && ! IsDialogMessage( hWndCanvas , &msg ) )
        {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
    }

    return (INT)msg.wParam;
}