#include "externs.h"

bool g_bDisableCamControl = false;
CLEO_Fn(SET_CAMERA_CONTROL)
{
    g_bDisableCamControl = (cleo->ReadParam(handle)->i == 0);
}
CLEO_Fn(IS_AIM_BUTTON_PRESSED)
{
    
}
CLEO_Fn(SET_PLAYER_CONTROL_PAD)
{
    
}
CLEO_Fn(SET_PLAYER_CONTROL_PAD_MOVEMENT)
{
    
}
CLEO_Fn(IS_ANY_FIRE_BUTTON_PRESSED)
{
    
}
CLEO_Fn(IS_SELECT_MENU_JUST_PRESSED)
{
    
}
CLEO_Fn(GET_TIME_NOT_TOUCHING_PAD)
{
    
}