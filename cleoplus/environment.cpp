#include "externs.h"

CLEO_Fn(GET_CURRENT_WEATHER)
{
    cleo->GetPointerToScriptVar(handle)->i = *OldWeatherType;
}
CLEO_Fn(GET_NEXT_WEATHER)
{
    cleo->GetPointerToScriptVar(handle)->i = *NewWeatherType;
}
CLEO_Fn(SET_NEXT_WEATHER)
{
    int newWeather = cleo->ReadParam(handle)->i;
    *NewWeatherType = newWeather;
}
CLEO_Fn(GET_RAIN_INTENSITY)
{
    cleo->GetPointerToScriptVar(handle)->f = *Rain;
}
CLEO_Fn(SET_RAIN_INTENSITY)
{
    float intens = cleo->ReadParam(handle)->f;
    *Rain = intens;
}
CLEO_Fn(GET_CURRENT_HOUR)
{
    cleo->GetPointerToScriptVar(handle)->i = *ms_nGameClockHours;
}
CLEO_Fn(GET_CURRENT_MINUTE)
{
    cleo->GetPointerToScriptVar(handle)->i = *ms_nGameClockMinutes;
}
CLEO_Fn(GET_DAY_NIGHT_BALANCE)
{
    cleo->GetPointerToScriptVar(handle)->f = *m_fDNBalanceParam;
}
CLEO_Fn(GET_UNDERWATERNESS)
{
    cleo->GetPointerToScriptVar(handle)->f = *UnderWaterness;
}
CLEO_Fn(GET_FORCED_WEATHER)
{
    cleo->GetPointerToScriptVar(handle)->i = *ForcedWeatherType;
}