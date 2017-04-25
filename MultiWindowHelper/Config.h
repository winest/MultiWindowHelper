#pragma once
#include "Resource.h"
#include "CWEdit.h"
#include "CWString.h"
#include "CWIni.h"


typedef enum HOOK_TYPE
{
    HOOK_WM_KEYDOWN ,
    HOOK_WM_KEYUP ,
    HOOK_WM_SYSKEYDOWN ,
    HOOK_WM_SYSKEYUP ,
    HOOK_WM_CHAR ,
    HOOK_WM_LBUTTONDOWN ,
    HOOK_WM_LBUTTONUP ,
    HOOK_WM_LBUTTONDBLCLK ,
    HOOK_WM_RBUTTONDOWN ,
    HOOK_WM_RBUTTONUP ,
    HOOK_WM_RBUTTONDBLCLK ,
    HOOK_WM_MOUSEMOVE
};

class CMultiWindowConfig
{
    public :
        CMultiWindowConfig * GetInstance() { return &m_self; }
    protected :
        CMultiWindowConfig() {}
        virtual ~CMultiWindowConfig() { m_mapHookConfig.clear(); }

    public :
        BOOL LoadConfig();
        INT GetWindowCount();
        BOOL GetWindows( vector<HWND> & aHwnd );
        BOOL IsHookEnabled( HOOK_TYPE aHookType );

    protected :
        static CMultiWindowConfig m_self;

    private :
        std::vector<HWND> m_vecWindows;
        std::map<HOOK_TYPE , BOOL> m_mapHookConfig;
};