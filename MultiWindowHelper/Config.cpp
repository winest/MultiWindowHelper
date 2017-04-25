#include "stdafx.h"
#include "Config.h"

CMultiWindowConfig CMultiWindowConfig::m_self;





BOOL CMultiWindowConfig::LoadConfig()
{
    CEdit * edtShow = (CEdit *)g_ctrlMain[EDT_SHOW];

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

    vector<wstring> vecWindowNames;
    CWUtils::SplitStringW( it->second , vecWindowNames , L";" );
    if ( vecWindowNames.size() == 0 )
    {
        edtShow->AddText( L"WindowNames value is empty\r\n" );
        return TRUE;
    }
}