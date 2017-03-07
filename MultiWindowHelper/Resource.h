#pragma once

#include "CWControl.h"

#define IDC_LAB_INPUT       0x1000

#define IDC_EDT_INPUT       0x2000
#define IDC_EDT_SHOW        0x2001

#define IDC_BTN_START       0x3000
#define IDC_BTN_CLEAN       0x3001

#define IDC_CHK_HELLO       0x3500

#define IDC_RAD_HELLO       0x3600

#define IDC_COMBO_HELLO     0x4000

#define IDC_LSV_HELLO       0x5000

#define IDC_TBR_UP          0x6000

typedef enum _CTRL_MAIN
{
    EDT_MAIN_START ,
    EDT_SHOW = EDT_MAIN_START ,
    EDT_MAIN_END ,
    EDT_MAIN_COUNT = EDT_MAIN_END - EDT_MAIN_START ,

    BTN_MAIN_START = EDT_MAIN_END ,
    BTN_START = BTN_MAIN_START ,
    BTN_CLEAN ,
    BTN_MAIN_END ,
    BTN_MAIN_COUNT = BTN_MAIN_END - BTN_MAIN_START ,

    CTRL_MAIN_COUNT = BTN_MAIN_END
} CTRL_MAIN;

extern CWUi::CControl * g_ctrlMain[CTRL_MAIN_COUNT];