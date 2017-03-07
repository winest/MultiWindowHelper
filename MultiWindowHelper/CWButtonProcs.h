#pragma once

#include "Resource.h"
#include "CWButton.h"
#include "CWEdit.h"
#include "CWString.h"
#include "CWFile.h"
#include "CWIni.h"

extern BOOL g_bStarted;
extern HWND * g_hWnds;
extern std::vector<std::wstring> g_vecWindowNames;

BOOL BtnStartCommand( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs );
BOOL BtnCleanCommand( HWND aHWnd , WPARAM aWParam , LPARAM aLParam , VOID * aArgs );