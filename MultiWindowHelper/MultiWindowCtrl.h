#pragma once
#include "Resource.h"
#include <Windowsx.h>
#include <atomic>
#include <vector>
#include <bitset>
#include <Psapi.h>
#include "CWGeneralUtils.h"
#include "CWToolbar.h"
#include "CWEdit.h"
#include "CWDllInjectMgr.h"
#include "MultiWindowConfig.h"

class CMultiWindowCtrl
{
    public :
        typedef struct _WindowInfo
        {
            _WindowInfo() : hWnd(NULL) , hIcon(NULL) , bDllInjected(FALSE) , dwPid(0) , dwBmpSize(0) , pBmpData(NULL) {}
            ~_WindowInfo()
            {
                if ( hIcon != NULL )
                {
                    DeleteObject( hIcon );
                    hIcon = NULL;
                }
                if ( pBmpData != NULL )
                {
                    delete [] pBmpData;
                    pBmpData = NULL;
                }
            }
            std::wstring wstrWindowName;
            HWND hWnd;
            LONG_PTR lpExStyle;
            HBITMAP hIcon;

            BOOL bDllInjected;
            DWORD dwPid;
            std::wstring wstrProcPath;
            DWORD dwBmpSize;
            UINT * pBmpData;
        } WindowInfo;

    public :
        static CMultiWindowCtrl * GetInstance() { return &m_self; }
    protected :
        CMultiWindowCtrl(): m_bStarted(FALSE) {}
        virtual ~CMultiWindowCtrl() { m_vecWindows.clear(); }

    public :
        BOOL IsStarted();
        BOOL Start();
        BOOL Stop();

        HWND GetWindow( INT aIndex );
        BOOL GetWindowInfoByPid( DWORD aPid , CONST WindowInfo ** aInfo );
        BOOL DispatchMsgIfNeed( UINT aMsg , WPARAM aWParam , LPARAM aLParam );

    protected :
        static CMultiWindowCtrl m_self;

    private :
        std::atomic<BOOL> m_bStarted;
        std::vector<WindowInfo> m_vecWindows;
        CWUtils::CDllInjectMgr m_DllInjectMgr;
};