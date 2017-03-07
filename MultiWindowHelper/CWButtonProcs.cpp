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


    switch ( HIWORD(aWParam) )
    {
        case BN_CLICKED:
        {
            CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_SHOW];
            CButton * btnStart = (CButton *)g_ctrlMain[BTN_START];
            if ( g_bStarted )
            {
                btnStart->SetText( L"Start" );

                if ( g_hWnds != NULL )
                {
                    delete [] g_hWnds;
                    g_hWnds = NULL;
                }
            }
            else
            {
                wstring wstrConfigPath;
                CWUtils::RelativeToFullPath( L"Config.ini" , wstrConfigPath );
                map<std::wstring,std::wstring> mapIniGeneral;
                if ( FALSE == CWUtils::GetIniSectionValues( wstrConfigPath.c_str() , L"General" , mapIniGeneral ) )
                {
                    edtShow->AddText( L"Config.ini not found\r\n" );
                    return TRUE;
                }
                map<std::wstring,std::wstring>::iterator it = mapIniGeneral.find( L"WindowNames" );
                if ( it == mapIniGeneral.end() )
                {
                    edtShow->AddText( L"WindowNames key not found\r\n" );
                    return TRUE;
                }
                CWUtils::SplitStringW( it->second , g_vecWindowNames , L";" );
                if ( g_vecWindowNames.size() == 0 )
                {
                    edtShow->AddText( L"WindowNames value is empty\r\n" );
                    return TRUE;
                }

                g_hWnds = new (std::nothrow) HWND[g_vecWindowNames.size()];
                ZeroMemory( g_hWnds , sizeof(HWND) * g_vecWindowNames.size() );
                for ( size_t i = 0 ; i < g_vecWindowNames.size() ; i++ )
                {
                    g_hWnds[i] = FindWindowW( NULL , g_vecWindowNames[i].c_str() );
                    if ( g_hWnds[i] != NULL )
                    {
			            edtShow->AddText( L"[%d] Found HWND %ws\r\n" , i , g_vecWindowNames[i].c_str() );
                    }
                }

                btnStart->SetText( L"Stop" );
            }

            g_bStarted = ( g_bStarted ) ? FALSE : TRUE;
            return TRUE;
        }
        default:
        {
            break;
        }
    }
    return FALSE;
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