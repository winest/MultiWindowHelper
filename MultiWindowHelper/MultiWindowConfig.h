#pragma once
#include "Resource.h"
#include "CWGeneralUtils.h"
#include "CWEdit.h"
#include "CWString.h"
#include "CWFile.h"
#include "CWIni.h"

typedef enum _MULTI_WINDOW_DISPLAY_MODE
{
    MULTI_WINDOW_DISPLAY_MODE_BITBLT = 0 ,
    MULTI_WINDOW_DISPLAY_MODE_PRINTWINDOW ,
    MULTI_WINDOW_DISPLAY_MODE_GLREADPIXELS ,
    MULTI_WINDOW_DISPLAY_MODE_COUNT
} MULTI_WINDOW_DISPLAY_MODE;

class CMultiWindowConfig
{
    public :
        static CMultiWindowConfig * GetInstance() { return &m_self; }
    protected :
        CMultiWindowConfig() : m_nDisplayMode(MULTI_WINDOW_DISPLAY_MODE_BITBLT) ,  m_fFps(30.0) , m_bAsync(TRUE) , m_bRetrieveEvenMinimized(FALSE) {}
        virtual ~CMultiWindowConfig() 
        {
            m_vecWindowNames.clear();
            m_mapDispatchCfg.clear();
        }

    public :
        BOOL Reload();

        BOOL GetWindowNames( std::vector<std::wstring> & aWindowNames );

        MULTI_WINDOW_DISPLAY_MODE GetDisplayMode();
        DOUBLE GetFps();
        BOOL IsAsync();
        BOOL IsRetrieveEvenMinimized();
        BOOL ShouldDispatch( INT aMsgType );

    protected :
        static CMultiWindowConfig m_self;

    private :
        std::vector<std::wstring> m_vecWindowNames;
        MULTI_WINDOW_DISPLAY_MODE m_nDisplayMode;
        DOUBLE m_fFps;
        BOOL m_bAsync;
        BOOL m_bRetrieveEvenMinimized;
        std::map<INT , BOOL> m_mapDispatchCfg;
};