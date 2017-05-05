#include "stdafx.h"
#include "MultiWindowConfig.h"
using namespace CWUi;

CMultiWindowConfig CMultiWindowConfig::m_self;



struct
{
    INT nType;
    WCHAR * wzText;
} g_DispatchConfig[] =
{
    NUM_TEXT_PAIRW( WM_KEYDOWN ) ,
    NUM_TEXT_PAIRW( WM_KEYUP ) ,
    NUM_TEXT_PAIRW( WM_SYSKEYDOWN ) ,
    NUM_TEXT_PAIRW( WM_SYSKEYUP ) ,
    NUM_TEXT_PAIRW( WM_CHAR ) ,
    NUM_TEXT_PAIRW( WM_LBUTTONDOWN ) ,
    NUM_TEXT_PAIRW( WM_LBUTTONUP ) ,
    NUM_TEXT_PAIRW( WM_LBUTTONDBLCLK ) ,
    NUM_TEXT_PAIRW( WM_RBUTTONDOWN ) ,
    NUM_TEXT_PAIRW( WM_RBUTTONUP ) ,
    NUM_TEXT_PAIRW( WM_RBUTTONDBLCLK ) ,
    NUM_TEXT_PAIRW( WM_MOUSEMOVE )
};



BOOL CMultiWindowConfig::Reload()
{
    BOOL bRet = FALSE;
    CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_LOG];

    do 
    {
        wstring wstrConfigPath;
        CWUtils::RelativeToFullPath( L"Config.ini" , wstrConfigPath );
        map<std::wstring,std::wstring> mapIniGeneral;
        if ( FALSE == CWUtils::GetIniSectionValues( wstrConfigPath.c_str() , L"General" , mapIniGeneral ) )
        {
            edtShow->AddText( L"%ws not found\r\n" , wstrConfigPath.c_str() );
            break;
        }
        
        //WindowNames
        map<std::wstring,std::wstring>::iterator it = mapIniGeneral.find( L"WindowNames" );
        if ( it == mapIniGeneral.end() )
        {
            edtShow->AddText( L"WindowNames key not found\r\n" );
            break;
        }
        else
        {
            edtShow->AddText( L"WindowNames: %ws\r\n" , it->second.c_str() );
        }
        CWUtils::SplitStringW( it->second , m_vecWindowNames , L";" );
        if ( m_vecWindowNames.size() == 0 )
        {
            edtShow->AddText( L"WindowNames value is empty\r\n" );
            break;
        }

        //DisplayMode
        m_nDisplayMode = (MULTI_WINDOW_DISPLAY_MODE)GetPrivateProfileIntW( L"General" , L"DisplayMode" , (INT)MULTI_WINDOW_DISPLAY_MODE_BITBLT , wstrConfigPath.c_str() );
        m_nDisplayMode = max( MULTI_WINDOW_DISPLAY_MODE_BITBLT , min( m_nDisplayMode , MULTI_WINDOW_DISPLAY_MODE_GLREADPIXELS ) );

        //Frame Per Second
        it = mapIniGeneral.find( L"FPS" );
        if ( it == mapIniGeneral.end() )
        {
            edtShow->AddText( L"FPS key not found\r\n" );
            it->second = L"30";
        }
        else
        {
            edtShow->AddText( L"FPS: %ws\r\n" , it->second.c_str() );
        }
        m_fFps = wcstod( it->second.c_str() , NULL );
        m_fFps = max( FLT_MIN , min( m_fFps , 1000.0 ) );

        //Asynchronous
        m_bAsync = GetPrivateProfileIntW( L"General" , L"Asynchronous" , 1 , wstrConfigPath.c_str() ) ? TRUE : FALSE;

        //RetrieveEvenMinimized
        m_bRetrieveEvenMinimized = GetPrivateProfileIntW( L"General" , L"RetrieveEvenMinimized" , 0 , wstrConfigPath.c_str() ) ? TRUE : FALSE;

        //WM_*
        for ( size_t i = 0 ; i < _countof(g_DispatchConfig) ; i++ )
        {
            m_mapDispatchCfg[g_DispatchConfig[i].nType] = GetPrivateProfileIntW( L"General" , g_DispatchConfig[i].wzText , 1 , wstrConfigPath.c_str() ) ? TRUE : FALSE;
            edtShow->AddText( L"%ws=%d\r\n" , g_DispatchConfig[i].wzText , m_mapDispatchCfg[g_DispatchConfig[i].nType] );
        }

        bRet = TRUE;
    } while ( 0 );
    
    return bRet;
}



BOOL CMultiWindowConfig::GetWindowNames( std::vector<std::wstring> & aWindowNames )
{
    aWindowNames = m_vecWindowNames;
    return TRUE;
}

MULTI_WINDOW_DISPLAY_MODE CMultiWindowConfig::GetDisplayMode()
{
    return m_nDisplayMode;
}

DOUBLE CMultiWindowConfig::GetFps()
{
    return m_fFps;
}

BOOL CMultiWindowConfig::IsAsync()
{
    return m_bAsync;
}

BOOL CMultiWindowConfig::IsRetrieveEvenMinimized()
{
    return m_bRetrieveEvenMinimized;
}

BOOL CMultiWindowConfig::ShouldDispatch( INT aMsgType )
{
    map<INT , BOOL>::iterator it = m_mapDispatchCfg.find( aMsgType );
    if ( it == m_mapDispatchCfg.end() )
    {
        return FALSE;
    }
    else
    {
        return it->second;
    }
}