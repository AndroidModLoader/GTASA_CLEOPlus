#include "externs.h"

inline int GetFirstFreeCounterLine()
{
    for(int i = 0; i < 4; ++i)
    {
        if(OnscnTimer->CounterEntry[i].CounterIndex == 0) return i;
    }
    return -1;
}

CLEO_Fn(DISPLAY_ONSCREEN_TIMER_LOCAL)
{
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    bool countDown = cleo->ReadParam(handle)->i;
    OnscreenTimerAddClock(OnscnTimer, varptr, NULL, countDown);
}
CLEO_Fn(DISPLAY_ONSCREEN_TIMER_WITH_STRING_LOCAL)
{
    char str[10];
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    bool countDown = cleo->ReadParam(handle)->i;
    cleoaddon->ReadString(handle, str, sizeof(str));
    OnscreenTimerAddClock(OnscnTimer, varptr, str, countDown);
}
CLEO_Fn(DISPLAY_ONSCREEN_COUNTER_LOCAL)
{
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    int type = cleo->ReadParam(handle)->i;
    int line = GetFirstFreeCounterLine();
    if(line) OnscreenTimerAddCounter(OnscnTimer, varptr, type, NULL, line);
}
CLEO_Fn(DISPLAY_ONSCREEN_COUNTER_WITH_STRING_LOCAL)
{
    char str[10];
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    int type = cleo->ReadParam(handle)->i;
    cleoaddon->ReadString(handle, str, sizeof(str));
    int line = GetFirstFreeCounterLine();
    if(line) OnscreenTimerAddCounter(OnscnTimer, varptr, type, str, line);
}
CLEO_Fn(DISPLAY_TWO_ONSCREEN_COUNTERS_LOCAL)
{
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    uintptr_t varptr2 = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    int line = GetFirstFreeCounterLine();
    if(line)
    {
        OnscreenTimerAddCounterCounter(OnscnTimer, varptr, varptr2, NULL, line);
        OnscnTimer->CounterEntry[line].iColour_id = 3;
    }
}
CLEO_Fn(DISPLAY_TWO_ONSCREEN_COUNTERS_WITH_STRING_LOCAL)
{
    char str[10];
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    uintptr_t varptr2 = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    cleoaddon->ReadString(handle, str, sizeof(str));
    int line = GetFirstFreeCounterLine();
    if(line)
    {
        OnscreenTimerAddCounterCounter(OnscnTimer, varptr, varptr2, str, line);
        OnscnTimer->CounterEntry[line].iColour_id = 3;
    }
}
CLEO_Fn(CLEAR_ONSCREEN_TIMER_LOCAL)
{
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    OnscreenTimerClearClock(OnscnTimer, varptr);
}
CLEO_Fn(CLEAR_ONSCREEN_COUNTER_LOCAL)
{
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    OnscreenTimerClearCounter(OnscnTimer, varptr);
}
CLEO_Fn(SET_ONSCREEN_COUNTER_FLASH_WHEN_FIRST_DISPLAYED_LOCAL)
{
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    OnscreenTimerSetCounterFlashWhenFirstDisplayed(OnscnTimer, varptr, cleo->ReadParam(handle)->i);
}
CLEO_Fn(SET_TIMER_BEEP_COUNTDOWN_TIME_LOCAL)
{
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    int secs = cleo->ReadParam(handle)->i;
    OnscreenTimerSetClockBeepCountdownSecs(OnscnTimer, varptr, secs);
}
CLEO_Fn(SET_ONSCREEN_COUNTER_COLOUR_LOCAL)
{
    uintptr_t varptr = (uintptr_t)cleo->GetPointerToScriptVar(handle) - (uintptr_t)ScriptSpace;
    int clr = cleo->ReadParam(handle)->i;
    OnscreenTimerSetCounterColourID(OnscnTimer, varptr, clr);
}