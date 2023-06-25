#include "externs.h"

#define SETITEM_SA_SFX_VOLUME               11
#define SETITEM_SA_RADIO_VOLUME             12

inline float GetSettingVal(unsigned char set)
{
    uintptr_t settingsArray = *(uintptr_t*)(pGTASA + 0x679A40);
    return *(int32_t*)(settingsArray + 0x20 * set + 0x8) * 0.01f;
}

CLEO_Fn(GET_AUDIO_SFX_VOLUME)
{
    cleo->GetPointerToScriptVar(handle)->f = GetSettingVal(SETITEM_SA_SFX_VOLUME);
}
CLEO_Fn(GET_AUDIO_RADIO_VOLUME)
{
    cleo->GetPointerToScriptVar(handle)->f = GetSettingVal(SETITEM_SA_RADIO_VOLUME);
}
CLEO_Fn(GET_AUDIOSTREAM_INTERNAL) // useless?
{
    int* stream = (int*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = *stream;
}