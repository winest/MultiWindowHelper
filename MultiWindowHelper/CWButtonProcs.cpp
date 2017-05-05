#include "stdafx.h"
#include "CWButtonProcs.h"
using namespace std;
using namespace CWUi;



BOOL BtnCleanCommand( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs )
{
    UNREFERENCED_PARAMETER( aHWnd );
    UNREFERENCED_PARAMETER( aLParam );
    UNREFERENCED_PARAMETER( aArgs );

    switch ( HIWORD(aWParam) )
    {
        case BN_CLICKED:
        {
            CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_LOG];
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