#pragma once

#include "CWControl.h"

#define IDC_EDT_LOG         0x2000

#define IDC_BTN_CLEAN       0x3000

#define IDC_TBR_UP          0x6000
#define IDC_TBR_LEFT        0x6001

#define IDC_TBR_UP_LOG      0x6100
#define IDC_TBR_UP_WINDOWS  0x6101

#define IDC_TBR_LEFT_START_STOP     0x6200
#define IDC_TBR_LEFT_ABOUT          0x6201

typedef enum _CTRL_MAIN
{
    TBR_MAIN_START = 0 ,
	TBR_MAIN_UP = TBR_MAIN_START ,
	TBR_MAIN_LEFT ,
	TBR_MAIN_END ,
	TBR_MAIN_COUNT  = TBR_MAIN_END - TBR_MAIN_START ,

    BTN_MAIN_START = TBR_MAIN_END ,
    BTN_CLEAN = BTN_MAIN_START ,
    BTN_MAIN_END ,
    BTN_MAIN_COUNT = BTN_MAIN_END - BTN_MAIN_START ,

    EDT_MAIN_START = BTN_MAIN_END ,
    EDT_LOG = EDT_MAIN_START ,
    EDT_MAIN_END ,
    EDT_MAIN_COUNT = EDT_MAIN_END - EDT_MAIN_START ,

    CTRL_MAIN_COUNT = EDT_MAIN_END
} CTRL_MAIN;


typedef enum _TBR_MAIN_UP_CTRL
{
	TBR_WINDOW_START = 0 ,
	TBR_WINDOW_LOG = TBR_WINDOW_START ,
	TBR_WINDOW_END ,
	TBR_WINDOW_COUNT = TBR_WINDOW_END - TBR_WINDOW_START ,

	TBR_MAIN_UP_COUNT = TBR_WINDOW_COUNT
} TBR_MAIN_UP_CTRL;

typedef enum _TBR_MAIN_LEFT_CTRL
{
	TBR_CHOICE_START = 0 ,
	TBR_START_STOP = TBR_CHOICE_START ,
    TBR_ABOUT ,
	TBR_CHOICE_END ,
	TBR_CHOICE_COUNT = TBR_CHOICE_END - TBR_CHOICE_START ,

	TBR_MAIN_LEFT_COUNT = TBR_CHOICE_END
} TBR_MAIN_LEFT_CTRL;

extern CWUi::CControl * g_ctrlMain[CTRL_MAIN_COUNT];