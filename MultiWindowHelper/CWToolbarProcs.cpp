#include "stdafx.h"
#include "CWToolbarProcs.h"
using namespace std;
using namespace CWUi;


BOOL TbrUpNotify( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs )
{
    UNREFERENCED_PARAMETER( aHWnd );
    UNREFERENCED_PARAMETER( aWParam );
    UNREFERENCED_PARAMETER( aLParam );
    UNREFERENCED_PARAMETER( aArgs );

    CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_LOG];
    switch ( ((NMHDR *)aLParam)->code )
    {
        case NM_CLICK:
        {
            CButton * btnClean = (CButton *)g_ctrlMain[BTN_CLEAN];
            CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
            CToolbar * tbrLeft = (CToolbar *)g_ctrlMain[TBR_MAIN_LEFT];
			DWORD id = ((NMMOUSE *)aLParam)->dwItemSpec;
			INT nIndex = tbrUp->GetIndexById( id );
			edtShow->AddText( L"Tab clicked. nIndex=%d\r\n" , nIndex );

			if ( nIndex == TBR_WINDOW_LOG )
            {
                ShowWindow( g_hWndCanvas , SW_HIDE );
                
                ShowWindow( btnClean->Self() , SW_SHOW );
                ShowWindow( edtShow->Self() , SW_SHOW );
            }
            else if ( TBR_WINDOW_LOG < nIndex && nIndex < TBR_WINDOW_LOG + tbrUp->ButtonCount() )
            {
                ShowWindow( btnClean->Self() , SW_HIDE );
                ShowWindow( edtShow->Self() , SW_HIDE );

                RECT rectMain;
                GetClientRect( g_hWndMain , &rectMain );
                INT nWidth = rectMain.right - rectMain.left;
                INT nHeight = rectMain.bottom - rectMain.top;
                INT nCanvasPosX = tbrLeft->PosX() + tbrLeft->Width() + 10;
                INT nCanvasPosY = tbrUp->PosY() + tbrUp->Height() + 10;
                MoveWindow( g_hWndCanvas , nCanvasPosX , nCanvasPosY , nWidth - nCanvasPosX - 10 , nHeight - nCanvasPosY - 10 , TRUE );
                ShowWindow( g_hWndCanvas , SW_SHOW );
            }
            else
            {
                edtShow->AddText( L"Unknown nIndex=%d\r\n" , nIndex );
            }

            break;
        }
        default:
        {
            break;
        }
    }

    return TRUE;
}



BOOL TbrLeftNotify( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs )
{
    UNREFERENCED_PARAMETER( aWParam );
    UNREFERENCED_PARAMETER( aLParam );
    UNREFERENCED_PARAMETER( aArgs );

    switch ( ((NMHDR *)aLParam)->code )
    {
        case NM_CLICK:
        {
            CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_LOG];
            CToolbar * tbrLeft = (CToolbar *)g_ctrlMain[TBR_MAIN_LEFT];
			DWORD id = ((NMMOUSE *)aLParam)->dwItemSpec;
			INT nIndex = tbrLeft->GetIndexById( id );
			
            if ( nIndex == TBR_START_STOP )
            {
                if ( CMultiWindowCtrl::GetInstance()->IsStarted() )
                {
                    CMultiWindowCtrl::GetInstance()->Stop();
                }
                else
                {
                    if ( FALSE == CMultiWindowCtrl::GetInstance()->Start() )
                    {
                        edtShow->AddText( L"Start() failed\r\n" );
                    }
                }
            }
			else if ( nIndex == TBR_ABOUT )
            {
                MessageBoxW( aHWnd , L"Created by winest" , L"About" , MB_OK );
            }
            else
            {
                edtShow->AddText( L"Unknown nIndex=%d\r\n" , nIndex );
            }

            break;
        }
        default:
        {
            break;
        }
    }

    return TRUE;
}