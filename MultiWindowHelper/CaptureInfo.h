#pragma once
#include <windef.h>

typedef struct _CaptureInfo
{
    BOOL bStart;
    HWND hWnd;
    DOUBLE fFps;
} CaptureInfo;