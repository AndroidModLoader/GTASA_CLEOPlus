#include "externs.h"

bool g_bDisableCamControl = false;
bool g_bDisablePadControl[3] = {false};
bool g_bDisablePadControlMovement[3] = {false};

CLEO_Fn(IS_MOUSE_WHEEL_UP)
{
    cleoaddon->UpdateCompareFlag(handle, NewMouseControllerState->m_bWheelMovedUp);
}
CLEO_Fn(IS_MOUSE_WHEEL_DOWN)
{
    cleoaddon->UpdateCompareFlag(handle, NewMouseControllerState->m_bWheelMovedDown);
}
CLEO_Fn(GET_MOUSE_SENSIBILITY)
{
    cleo->GetPointerToScriptVar(handle)->f = *m_fMouseAccelHorzntl;
}
CLEO_Fn(IS_KEY_JUST_PRESSED)
{
    
}
CLEO_Fn(IS_BUTTON_JUST_PRESSED)
{
    
}
CLEO_Fn(SET_CAMERA_CONTROL)
{
    g_bDisableCamControl = (cleo->ReadParam(handle)->i == 0);
}
CLEO_Fn(IS_AIM_BUTTON_PRESSED)
{
    int pad = cleo->ReadParam(handle)->i;
    cleoaddon->UpdateCompareFlag(handle, PadGetTarget(GetPad(pad), true));
}
CLEO_Fn(SET_PLAYER_CONTROL_PAD)
{
    int pad = cleo->ReadParam(handle)->i;
    g_bDisablePadControl[pad] = (cleo->ReadParam(handle)->i == 0);
}
CLEO_Fn(SET_PLAYER_CONTROL_PAD_MOVEMENT)
{
    int pad = cleo->ReadParam(handle)->i;
    g_bDisablePadControlMovement[pad] = (cleo->ReadParam(handle)->i == 0);
}
CLEO_Fn(IS_ANY_FIRE_BUTTON_PRESSED)
{
    int pad = cleo->ReadParam(handle)->i;
    cleoaddon->UpdateCompareFlag(handle, GetPadState(NULL, pad, 4));
}
CLEO_Fn(IS_SELECT_MENU_JUST_PRESSED)
{
    // We dont have that menu on Android
    cleoaddon->UpdateCompareFlag(handle, false);
}
CLEO_Fn(GET_TIME_NOT_TOUCHING_PAD)
{
    int pad = cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = (*m_snTimeInMilliseconds - GetPad(pad)->LastTimeTouched);
}