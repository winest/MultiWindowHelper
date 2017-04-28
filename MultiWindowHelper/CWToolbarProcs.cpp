#include "stdafx.h"
#include "CWToolbarProcs.h"
using namespace std;
using namespace CWUi;


BOOL TbrUpNotify( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs )
{
    UNREFERENCED_PARAMETER( aLParam );
    UNREFERENCED_PARAMETER( aArgs );

    BOOL bRet = FALSE;
    switch ( ((NMHDR *)aLParam)->code )
    {
        case NM_CLICK:
        {
            CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_SHOW];
            CToolbar * tbrUp = (CToolbar *)g_ctrlMain[TBR_MAIN_UP];
			DWORD id = ((NMMOUSE *)aLParam)->dwItemSpec;
			INT nIndex = tbrUp->GetIndexById( id );
			
			if ( nIndex == TBR_WINDOW_EMPTY )
            {
                //Resize Canvas
            }

			bRet = TRUE;
            break;
        }
        default:
        {
            break;
        }
    }
    return bRet;
}



BOOL TbrLeftNotify( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs )
{
    UNREFERENCED_PARAMETER( aLParam );
    UNREFERENCED_PARAMETER( aArgs );

    BOOL bRet = FALSE;
    switch ( ((NMHDR *)aLParam)->code )
    {
        case NM_CLICK:
        {
            CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_SHOW];
            CToolbar * tbrLeft = (CToolbar *)g_ctrlMain[TBR_MAIN_LEFT];
			DWORD id = ((NMMOUSE *)aLParam)->dwItemSpec;
			INT nIndex = tbrLeft->GetIndexById( id );
			
            if ( nIndex == TBR_START_STOP )
            {
                if ( CMultiWindowCtrl::GetInstance()->IsStarted() )
                {
                    bRet = CMultiWindowCtrl::GetInstance()->Stop();
                }
                else
                {
                    bRet = CMultiWindowCtrl::GetInstance()->Start();
                }
            }
			else if ( nIndex == TBR_LOG )
            {
                //Resize Canvas
            }
            else
            {
                edtShow->AddText( L"Unknown nIndex=%d\r\n" , nIndex );
            }

			bRet = TRUE;
            break;
        }
        default:
        {
            break;
        }
    }
    return bRet;
}