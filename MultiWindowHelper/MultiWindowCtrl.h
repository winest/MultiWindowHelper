#pragma once
#include "Resource.h"
#include <atomic>
#include <vector>
#include "CWGeneralUtils.h"
#include "CWToolbar.h"
#include "CWEdit.h"
#include "MultiWindowConfig.h"

class CMultiWindowCtrl
{
    protected :
        typedef struct _WindowInfo
        {
            ~_WindowInfo()
            {
                if ( hBitmap != NULL )
                {
                    DeleteObject( hBitmap );
                    hBitmap = NULL;
                }
            }
            std::wstring wstrWindowName;
            HWND hWnd;
            LONG_PTR lpExStyle;
            HBITMAP hBitmap;
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
        BOOL DispatchMsgIfNeed( UINT aMsg , WPARAM aWParam , LPARAM aLParam );

    protected :
        static CMultiWindowCtrl m_self;

    private :
        std::atomic<BOOL> m_bStarted;
        std::vector<WindowInfo> m_vecWindows;
};