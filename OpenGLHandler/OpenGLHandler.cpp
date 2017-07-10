#include "stdafx.h"
#include <process.h>
#include <string>
#include <vector>
using namespace std;

#pragma warning( disable : 4127 )
#include "CWDllInjectCommonDef.h"
#include "CWDllInjectClient.h"

#include "CWGeneralUtils.h"
#include "CWString.h"
using namespace CWUtils;

#include "_GenerateTmh.h"
#include "OpenGLHandler.tmh"

#define KERNEL32_MODULE_NAME                 L"kernel32.dll"
#define FREELIBRARY_FUNCTION_NAME            "FreeLibrary"

volatile LONG g_lDllRefCnt = 0;


typedef struct _EnumWindowParam
{
    _EnumWindowParam() : dwMainTid(0) {}
    DWORD dwMainTid;
} EnumWindowParam;



#ifdef _MANAGED
    #pragma managed( push , off )
#endif

VOID UnInitialize();



HMODULE g_hModule = NULL;
HANDLE  g_hHookThread = NULL;

CDllInjectClient g_SmClient;

DWORD WINAPI RemoteHookHandler( VOID * pParam )
{
    UNREFERENCED_PARAMETER( pParam );

    LPTHREAD_START_ROUTINE pfnFreeLibrary = NULL;
    WCHAR wzSmName[MAX_PATH] = {};
    GenerateShareMemoryName( wzSmName , GetCurrentProcessId() );
    if ( g_SmClient.Connect( wzSmName , NULL ) )
    {
        pfnFreeLibrary = g_SmClient.GetFreeLibraryAddr();

        BOOL bStop = FALSE;
        HANDLE hEvtWaitStop[] = { g_SmClient.GetServerQuitEvt() , g_SmClient.GetClientQuitEvt() , g_SmClient.GetServerAliveEvt() };
        while ( ! bStop )
        {
            DWORD dwWaitStop = WaitForMultipleObjects( _countof(hEvtWaitStop) , hEvtWaitStop , FALSE , INFINITE );
            switch ( dwWaitStop )
            {
                case WAIT_OBJECT_0 :            //PER_SERVER_EVT_INDEX_STOP
                {
                    DbgOut( WARN , DBG_OPENGL_HANDLER , "PER_SERVER_EVT_INDEX_STOP event triggered" );
                    bStop = TRUE;
                    break;
                }
                case WAIT_OBJECT_0 + 1 :        //PER_CLIENT_EVT_INDEX_STOP
                {
                    DbgOut( WARN , DBG_OPENGL_HANDLER , "PER_CLIENT_EVT_INDEX_STOP event triggered" );
                    bStop = TRUE;
                    break;
                }
                case WAIT_OBJECT_0 + 2 :        //hDllInjectMgrAliveThread
                {
                    DbgOut( WARN , DBG_OPENGL_HANDLER , "CWDllInjectMgr leave event triggered" );
                    bStop = TRUE;
                    break;
                }
                default :
                {
                    DbgOut( ERRO , DBG_OPENGL_HANDLER , "WaitForMultipleObjects() return unexpected value 0x%08X. GetLastError()=%!WINERROR!" , dwWaitStop , GetLastError() );
                    bStop = TRUE;
                    break;
                }
            }
        }

        g_SmClient.Disconnect();
    }
    else
    {
        DbgOut( ERRO , DBG_OPENGL_HANDLER , "Connect() failed" );
    }

    //Be aware that some programs may hook on this function and therefore we may get an wrong address for FreeLibrary
    if ( NULL == pfnFreeLibrary )
    {
        DbgOut( ERRO , DBG_OPENGL_HANDLER , "Cannot get FreeLibrary's address from share memory. Try to use 0x%p" , pfnFreeLibrary );
        pfnFreeLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress( GetModuleHandleW(KERNEL32_MODULE_NAME) , FREELIBRARY_FUNCTION_NAME );
    }

    //Create a thread to leave itself
    if ( 0 < ATOMIC_READ(g_lDllRefCnt) && NULL != pfnFreeLibrary )
    {
        DWORD dwFreeTid = 0;
        HANDLE hFreeThread = CreateThread( NULL , 0 , pfnFreeLibrary , g_hModule , 0 , &dwFreeTid );
        DbgOut( WARN , DBG_OPENGL_HANDLER , "Create thread to FreeLibrary. dwFreeTid=0x%04X, hFreeThread=0x%p" , dwFreeTid , hFreeThread );
        CloseHandle( hFreeThread );
    }

    DbgOut( INFO , DBG_OPENGL_HANDLER , "RemoteHookHandler() Leave" );
    return 0;
}



void UnInitialize()
{
    if ( g_SmClient.IsConnected() )
    {
        g_SmClient.Disconnect();
    }
    DbgOut( INFO , DBG_OPENGL_HANDLER , "Waiting for the hooking thread terminated" );
    if ( WAIT_TIMEOUT == WaitForSingleObject( g_hHookThread , 0 ) ) //Just log whether it's alive to avoid loader lock issue in DllMain()
    {
        DbgOut( ERRO , DBG_OPENGL_HANDLER , "Failed to wait until the hooking thread terminated. Unload directly" );
    }
    else
    {
        DbgOut( INFO , DBG_OPENGL_HANDLER , "Successfully wait until the hooking thread terminated" );
    }

    CloseHandle( g_hHookThread );
};


BOOL
APIENTRY
DllMain( HMODULE aModule , DWORD aReason , LPVOID aReserved )
{
    UNREFERENCED_PARAMETER( aReserved );

    BOOL bRet = TRUE;
    switch ( aReason )
    {
        case DLL_PROCESS_ATTACH:
        {
            WPP_INIT_TRACING( L"OpenGLHandler" );
            DbgOut( VERB , DBG_OPENGL_HANDLER , "OpenGLHandler.dll DLL_PROCESS_ATTACH" );

            DisableThreadLibraryCalls( aModule );
            g_hModule = aModule;

            //Create hook thread
            DWORD dwTid;
            g_hHookThread = CreateThread( NULL , 0 , RemoteHookHandler , NULL , 0 , &dwTid );
            if ( NULL == g_hHookThread )
            {
                DbgOut( ERRO , DBG_OPENGL_HANDLER , "Failed to create remote hook thread. GetLastError()=%!WINERROR!" , GetLastError() );
                bRet = FALSE;
                break;
            }

            DbgOut( INFO , DBG_OPENGL_HANDLER , "Remote hook thread id 0x%04X created with handle 0x%p" , dwTid , g_hHookThread );
            ATOMIC_INC( g_lDllRefCnt );
            break;
        }   
        case DLL_THREAD_ATTACH:
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
        {
            DbgOut( VERB , DBG_OPENGL_HANDLER , "OpenGLHandler.dll DLL_PROCESS_DETACH" );
            ATOMIC_DEC( g_lDllRefCnt );

            UnInitialize();
            WPP_CLEANUP();
            break;
        }   
    }
    return TRUE;
}

#ifdef _MANAGED
    #pragma managed(pop)
#endif