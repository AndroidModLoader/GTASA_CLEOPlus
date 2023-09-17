#include "externs.h"

class ScriptEvent;
std::vector<ScriptEvent*> scriptEvents[TOTAL_SCRIPT_EVENTS];
class ScriptEvent
{
public:
    CRunningScript *script;
    int eventScriptIP;
    unsigned int varPointer[4];

public:
    void RunScriptEvent(int arg1, int arg2, int arg3, int arg4)
    {
        CRunningScript *script = this->script;
        if (!script) return;

        unsigned char *currentIPbackup = script->PCPointer;

        int ip = this->eventScriptIP;
        if (ip >= 0)
            script->PCPointer = (unsigned char *)(ScriptSpace + ip);
        else
            script->PCPointer = (script->BaseAddressOfThisScript - ip);

        int wakeTimeBackup = this->script->ActivateTime;
        script->ActivateTime = 0;

        if (this->varPointer[0] > 0x00010000) *(uint32_t*)varPointer[0] = arg1;
        if (this->varPointer[1] > 0x00010000) *(uint32_t*)varPointer[1] = arg2;
        if (this->varPointer[2] > 0x00010000) *(uint32_t*)varPointer[2] = arg3;
        if (this->varPointer[3] > 0x00010000) *(uint32_t*)varPointer[3] = arg4;

        ProcessScript(script);

        script->PCPointer = currentIPbackup;
        script->ActivateTime = wakeTimeBackup;
    }
    void RunScriptEvent(int arg1)
    {
        ScriptEvent::RunScriptEvent(arg1, 0, 0, 0);
    }

    static void AddEvent(void* handle, std::vector<ScriptEvent*> &scriptEventList, unsigned int args = 1)
    {
        int toggle = cleo->ReadParam(handle)->i;
        int label = cleo->ReadParam(handle)->i;

        if(toggle)
        {
            ScriptEvent *eventStruct = new ScriptEvent();
            eventStruct->script = (CRunningScript*)handle;
            eventStruct->eventScriptIP = label;

            for (unsigned int i = 0; i < 4; ++i)
            {
                uintptr_t pointer = 0;
                if (i < args)
                {
                    pointer = (uintptr_t)(cleo->GetPointerToScriptVar(handle));
                }
                eventStruct->varPointer[i] = pointer;
            }
            scriptEventList.push_back(eventStruct);
        }
        else
        {
            uintptr_t vars[4];
            for (unsigned int i = 0; i < 4; ++i)
            {
                uintptr_t pointer = 0;
                if (i < args)
                {
                    pointer = (uintptr_t)(cleo->GetPointerToScriptVar(handle));
                }
                vars[i] = pointer;
            }
            ScriptEvent::ClearForScriptLabelAndVar(scriptEventList, (CRunningScript*)handle, label, vars);
        }
    }
    static void ClearForScriptLabelAndVar(std::vector<ScriptEvent*> &scriptEvents, CRunningScript* script, int label, unsigned int varPointer[4])
    {
        auto end = scriptEvents.end();
        for (std::vector<ScriptEvent*>::iterator it = scriptEvents.begin(); it != end;)
        {
            ScriptEvent *scriptEvent = (ScriptEvent*)*it;
            if (scriptEvent->script == script && scriptEvent->eventScriptIP == label)
            {
                bool ok = true;
                for (unsigned int i = 0; i < 4; ++i)
                {
                    if (scriptEvent->varPointer[i] == 0) break;
                    if (scriptEvent->varPointer[i] != varPointer[i])
                    {
                        ok = false;
                        break;
                    }
                }
                if (ok)
                {
                    it = scriptEvents.erase(it);
                    delete scriptEvent;
                }
            }
            else ++it;
        }
    }
    static void ClearAllForScript(CRunningScript *script)
    {
        for (unsigned int i = 0; i < TOTAL_SCRIPT_EVENTS; ++i)
        {
            auto end = scriptEvents[i].end();
            for (std::vector<ScriptEvent*>::iterator it = scriptEvents[i].begin(); it != end;)
            {
                ScriptEvent *scriptEvent = (ScriptEvent*)*it;
                if (scriptEvent->script == script)
                {
                    it = scriptEvents[i].erase(it);
                    delete scriptEvent;
                }
                else ++it;
            }
        }
    }
    static void ClearAllScriptEvents()
    {
        for (unsigned int i = 0; i < TOTAL_SCRIPT_EVENTS; ++i)
        {
            for (auto& scriptEvent : scriptEvents[i]) delete scriptEvent;
        }
        for (unsigned int i = 0; i < TOTAL_SCRIPT_EVENTS; ++i)
        {
            scriptEvents[i].clear();
        }
    }
};

CLEO_Fn(RETURN_SCRIPT_EVENT)
{
    g_bForceInterrupt = true;
}
CLEO_Fn(SET_SCRIPT_EVENT_SAVE_CONFIRMATION)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::SaveConfirmation]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_DELETE)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CharDelete]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_CREATE)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CharCreate]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CAR_DELETE)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CarDelete]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CAR_CREATE)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CarCreate]);
}
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_DELETE)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::ObjectDelete]);
}
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_CREATE)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::ObjectCreate]);
}
CLEO_Fn(SET_SCRIPT_EVENT_ON_MENU)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::OnMenu]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_PROCESS)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CharProcess]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CAR_PROCESS)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CarProcess]);
}
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_PROCESS)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::ObjectProcess]);
}
CLEO_Fn(SET_SCRIPT_EVENT_BUILDING_PROCESS)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::BuildingProcess]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_DAMAGE)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CharDamage]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CAR_WEAPON_DAMAGE)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CarWeaponDamage]);
}
void HookBulletImpact();
CLEO_Fn(SET_SCRIPT_EVENT_BULLET_IMPACT)
{
    HookBulletImpact();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::BulletImpact], 4);
}

// --------------------------------------------------------------- //

DECL_HOOKv(GameSave)
{
    g_nCurrentSaveSlot = gMobileMenu->m_nGenericGameStorageSlot;
    for (auto scriptEvent : scriptEvents[ScriptEventList::SaveConfirmation]) scriptEvent->RunScriptEvent(g_nCurrentSaveSlot + 1);
    GameSave();
}
DECL_HOOKv(OnBulletImpact, CWeapon *self, CEntity *pEntity, CEntity *pHitEntity, CVector *pShotOrigin, CVector *pShotTarget, CColPoint *pColPoint, int nIncrementalHit)
{
    OnBulletImpact(self, pEntity, pHitEntity, pShotOrigin, pShotTarget, pColPoint, nIncrementalHit);
    for (auto scriptEvent : scriptEvents[ScriptEventList::BulletImpact]) scriptEvent->RunScriptEvent((int)pEntity, (int)pHitEntity, (int)self->m_nType, (int)pColPoint);
}
void HookBulletImpact()
{
    static bool bHooked = false;
    if(bHooked) return;
    bHooked = true;
    HOOKPLT(OnBulletImpact, pGTASA + 0x66E8DC);
}
void Events_Patch()
{
    HOOKPLT(GameSave, pGTASA + 0x66EC5C);
}