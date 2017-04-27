#include "stdafx.h"
#include "CWButtonProcs.h"
using namespace std;
using namespace CWUi;


BOOL g_bStarted = FALSE;
HWND * g_hWnds = NULL;
std::vector<std::wstring> g_vecWindowNames;

BOOL BtnStartCommand( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs )
{
    UNREFERENCED_PARAMETER( aLParam );
    UNREFERENCED_PARAMETER( aArgs );

    BOOL bRet = FALSE;
    switch ( HIWORD(aWParam) )
    {
        case BN_CLICKED:
        {
            if ( CMultiWindowCtrl::GetInstance()->IsStarted() )
            {
                bRet = CMultiWindowCtrl::GetInstance()->Stop();
            }
            else
            {
                bRet = CMultiWindowCtrl::GetInstance()->Start();
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return bRet;
}



BOOL BtnCleanCommand( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs )
{
    UNREFERENCED_PARAMETER( aHWnd );
    UNREFERENCED_PARAMETER( aLParam );
    UNREFERENCED_PARAMETER( aArgs );

    switch ( HIWORD(aWParam) )
    {
        case BN_CLICKED:
        {
            CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_SHOW];
            edtShow->Clean();
            return TRUE;
        }
        default:
        {
            break;
        }
    }
    return FALSE;
}