#pragma once
#include "Resource.h"
#include "CWGeneralUtils.h"
#include "CWEdit.h"
#include "CWString.h"
#include "CWFile.h"
#include "CWIni.h"



class CMultiWindowConfig
{
    public :
        static CMultiWindowConfig * GetInstance() { return &m_self; }
    protected :
        CMultiWindowConfig() : m_bAsync(TRUE) {}
        virtual ~CMultiWindowConfig() 
        {
            m_vecWindowNames.clear();
            m_mapDispatchCfg.clear();
        }

    public :
        BOOL Reload();

        BOOL GetWindowNames( std::vector<std::wstring> & aWindowNames );
        BOOL IsAsync();
        BOOL ShouldDispatch( INT aMsgType );

    protected :
        static CMultiWindowConfig m_self;

    private :
        std::vector<std::wstring> m_vecWindowNames;
        BOOL m_bAsync;
        std::map<INT , BOOL> m_mapDispatchCfg;
};