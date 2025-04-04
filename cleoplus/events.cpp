#include "externs.h"

DECL_HOOKv(PedDamageResponse, CPedDamageResponseCalculator* self, CPed* victim, CPedDamageResponse* response, bool react)
{
    if(!response->m_bDamageCalculated)
    {
        auto xdata = GetExtData(victim);
        if(xdata)
        {
            xdata->lastDamageEntity = (CEntity*)self->m_pInflictor;
            xdata->lastDamageIntensity = self->m_fRawDamage;
            xdata->lastDamageWeapon = (int)self->m_eWeaponUsed;
            xdata->lastDamagePart = (int)self->m_eHitZone;
        }
        int ref = (*ms_pPedPool)->GetRef(victim);
        for (auto scriptEvent : scriptEvents[ScriptEventList::CharDamage]) scriptEvent->RunScriptEvent(ref);
    }
    PedDamageResponse(self, victim, response, react);
}

uintptr_t CarInflictDmg_BackTo;
extern "C" uintptr_t CarInflictDmg_Patch(CVehicle* self, int type, CEntity* damager, float damage)
{
    if(damage > 0)
    {
        auto xdata = GetExtData(self);
        if(xdata)
        {
            xdata->lastDamageEntity = damager;
            xdata->lastDamageIntensity = damage;
            xdata->lastDamageWeapon = type;
        }

        int ref = (*ms_pVehiclePool)->GetRef(self);
        for (auto scriptEvent : scriptEvents[ScriptEventList::CarWeaponDamage]) scriptEvent->RunScriptEvent(ref);
    }
    return CarInflictDmg_BackTo;
}
__attribute__((optnone)) __attribute__((naked)) void CarInflictDmg_Inject(void)
{
    asm volatile(
        "PUSH {R0-R11}\n"
        "LDR R0, [SP, #0x4 * 5]\n"
        "LDR R1, [SP, #0x4 * 8]\n"
        "LDR R2, [SP, #0x4 * 11]\n"
        "VMOV R3, S16\n"
        "BL CarInflictDmg_Patch\n"
        "MOV R12, R0\n"
        "POP {R0-R11}\n");
    asm volatile(
        "ADDW R4, R5, #0x4CC\n"
        "VLDR S18, [R4]\n"
        "VCMPE.F32 S18, #0.0\n"); // org
    asm volatile(
        "BX R12\n");
}

// --------------------------------------------------------------- //

bool patchesActive[TOTAL_SCRIPT_EVENTS] { false };
std::vector<ScriptEvent*> scriptEvents[TOTAL_SCRIPT_EVENTS];
void ScriptEvent::RunScriptEvent(int arg1, int arg2, int arg3, int arg4)
{
    CRunningScript *script = this->script;
    if (!script) return;
    unsigned char *currentIPbackup = script->PCPointer;
    int ip = this->eventScriptIP;
    uint8_t* ptr = (uint8_t*)cleo->GetRealLabelPointer(script, ip);
    if(ptr)
    {
        script->PCPointer = ptr;
    }
    else
    {
        if (ip >= 0)
            script->PCPointer = (unsigned char *)(ScriptSpace + ip);
        else
            script->PCPointer = (script->BaseAddressOfThisScript - ip);
    }
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
void ScriptEvent::AddEvent(void* handle, std::vector<ScriptEvent*> &scriptEventList, unsigned int args)
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
void ScriptEvent::ClearForScriptLabelAndVar(std::vector<ScriptEvent*> &scriptEvents, CRunningScript* script, int label, unsigned int varPointer[4])
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
void ScriptEvent::ClearAllForScript(CRunningScript *script)
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
void ScriptEvent::ClearAllScriptEvents()
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

// --------------------------------------------------------------- //

DECL_HOOKv(OnBulletImpact, CWeapon *self, CEntity *pEntity, CEntity *pHitEntity, CVector *pShotOrigin, CVector *pShotTarget, CColPoint *pColPoint, int nIncrementalHit)
{
    OnBulletImpact(self, pEntity, pHitEntity, pShotOrigin, pShotTarget, pColPoint, nIncrementalHit);
    for (auto scriptEvent : scriptEvents[ScriptEventList::BulletImpact]) scriptEvent->RunScriptEvent((int)pEntity, (int)pHitEntity, (int)self->m_nType, (int)pColPoint);
}
DECL_HOOKv(OnObjectInit, CObject* self)
{
    OnObjectInit(self);
    int ref = (*ms_pObjectPool)->GetRef(self);
    for (auto scriptEvent : scriptEvents[ScriptEventList::ObjectCreate]) scriptEvent->RunScriptEvent(ref);
}
DECL_HOOKv(CarAudioInit, void* self, CVehicle* vehicle) // returns 0/1 but unused, so we're saving some calls
{
    CarAudioInit(self, vehicle);
    int ref = (*ms_pVehiclePool)->GetRef(vehicle);
    for (auto scriptEvent : scriptEvents[ScriptEventList::CarCreate]) scriptEvent->RunScriptEvent(ref);
}
DECL_HOOKv(PedAudioInit, void* self, CPed* ped) // returns audio type but unused, so we're saving some calls
{
    PedAudioInit(self, ped);
    int ref = (*ms_pPedPool)->GetRef(ped);
    for (auto scriptEvent : scriptEvents[ScriptEventList::CharCreate]) scriptEvent->RunScriptEvent(ref);
}
DECL_HOOKv(PedDeletion, CPed* ped)
{
    int ref = (*ms_pPedPool)->GetRef(ped);
    for (auto scriptEvent : scriptEvents[ScriptEventList::CharDelete]) scriptEvent->RunScriptEvent(ref);
    PedDeletion(ped);
}
DECL_HOOKv(CarDeletion, CVehicle* vehicle)
{
    int ref = (*ms_pVehiclePool)->GetRef(vehicle);
    for (auto scriptEvent : scriptEvents[ScriptEventList::CarDelete]) scriptEvent->RunScriptEvent(ref);
    CarDeletion(vehicle);
}
DECL_HOOKv(ObjectDeletion, CObject* object)
{
    int ref = (*ms_pObjectPool)->GetRef(object);
    for (auto scriptEvent : scriptEvents[ScriptEventList::ObjectDelete]) scriptEvent->RunScriptEvent(ref);
    ObjectDeletion(object);
}
DECL_HOOKv(PedPreRendered, CPed* ped)
{
    int ref = (*ms_pPedPool)->GetRef(ped);
    for (auto scriptEvent : scriptEvents[ScriptEventList::CharProcess]) scriptEvent->RunScriptEvent(ref);
    PedPreRendered(ped);
}
DECL_HOOKv(CarPreRendered, CVehicle* vehicle)
{
    int ref = (*ms_pVehiclePool)->GetRef(vehicle);
    for (auto scriptEvent : scriptEvents[ScriptEventList::CarProcess]) scriptEvent->RunScriptEvent(ref);
    CarPreRendered(vehicle);
}
DECL_HOOKv(ObjectPreRendered, CObject* object)
{
    int ref = (*ms_pObjectPool)->GetRef(object);
    for (auto scriptEvent : scriptEvents[ScriptEventList::ObjectProcess]) scriptEvent->RunScriptEvent(ref);
    ObjectPreRendered(object);
}
DECL_HOOKv(BuildingPreRendered, CBuilding* building)
{
    // This one sends the entity pointer because there is no function that use refs of BUILDINGS
    for (auto scriptEvent : scriptEvents[ScriptEventList::BuildingProcess]) scriptEvent->RunScriptEvent((int)building);
    BuildingPreRendered(building);
}
DECL_HOOKv(AnimBuildingPreRendered, CBuilding* building)
{
    // This one sends the entity pointer because there is no function that use refs of BUILDINGS
    for (auto scriptEvent : scriptEvents[ScriptEventList::BuildingProcess]) scriptEvent->RunScriptEvent((int)building);
    AnimBuildingPreRendered(building);
}

// --------------------------------------------------------------- //

void HookBulletImpact()
{
    if(patchesActive[ScriptEventList::BulletImpact]) return;
    patchesActive[ScriptEventList::BulletImpact] = true;
    HOOKPLT(OnBulletImpact, pGTASA + 0x66E8DC);
}
void HookObjectInit()
{
    if(patchesActive[ScriptEventList::ObjectCreate]) return;
    patchesActive[ScriptEventList::ObjectCreate] = true;
    HOOKPLT(OnObjectInit, pGTASA + 0x67371C);
}
void HookCarAudioInit()
{
    if(patchesActive[ScriptEventList::CarCreate]) return;
    patchesActive[ScriptEventList::CarCreate] = true;
    HOOKPLT(CarAudioInit, pGTASA + 0x66F25C);
}
void HookPedAudioInit()
{
    if(patchesActive[ScriptEventList::CharCreate]) return;
    patchesActive[ScriptEventList::CharCreate] = true;
    HOOKPLT(PedAudioInit, pGTASA + 0x671E00);
}
void HookPedDelete()
{
    if(patchesActive[ScriptEventList::CharDelete]) return;
    patchesActive[ScriptEventList::CharDelete] = true;

    HOOKPLT(PedDeletion, pGTASA + 0x66F4FC);
    HOOKPLT(PedDeletion, pGTASA + 0x668BF0);
    HOOKPLT(PedDeletion, pGTASA + 0x668C68);
}
void HookCarDelete()
{
    if(patchesActive[ScriptEventList::CarDelete]) return;
    patchesActive[ScriptEventList::CarDelete] = true;

    HOOKPLT(CarDeletion, pGTASA + 0x66E220);
    HOOKPLT(CarDeletion, pGTASA + 0x67227C);
}
void HookObjectDelete()
{
    if(patchesActive[ScriptEventList::ObjectDelete]) return;
    patchesActive[ScriptEventList::ObjectDelete] = true;

    HOOKPLT(ObjectDeletion, pGTASA + 0x667E40);
    HOOKPLT(ObjectDeletion, pGTASA + 0x667EA8);
    HOOKPLT(ObjectDeletion, pGTASA + 0x6703A4);
}
void HookPedProcess()
{
    if(patchesActive[ScriptEventList::CharProcess]) return;
    patchesActive[ScriptEventList::CharProcess] = true;

    HOOKPLT(PedPreRendered, pGTASA + 0x668AEC);
    HOOKPLT(PedPreRendered, pGTASA + 0x668B60);
    HOOKPLT(PedPreRendered, pGTASA + 0x668C38);
    HOOKPLT(PedPreRendered, pGTASA + 0x668CB0);
    HOOKPLT(PedPreRendered, pGTASA + 0x6692D0);
}
void HookCarProcess()
{
    if(patchesActive[ScriptEventList::CarProcess]) return;
    patchesActive[ScriptEventList::CarProcess] = true;

    HOOKPLT(CarPreRendered, pGTASA + 0x66E268);
    HOOKPLT(CarPreRendered, pGTASA + 0x66F490);
}
void HookObjectProcess()
{
    if(patchesActive[ScriptEventList::ObjectProcess]) return;
    patchesActive[ScriptEventList::ObjectProcess] = true;

    HOOKPLT(ObjectPreRendered, pGTASA + 0x667EF0);
    HOOKPLT(ObjectPreRendered, pGTASA + 0x667F58);
}
void HookBuildingProcess()
{
    if(patchesActive[ScriptEventList::BuildingProcess]) return;
    patchesActive[ScriptEventList::BuildingProcess] = true;

    HOOKPLT(BuildingPreRendered, pGTASA + 0x662010);
    HOOKPLT(AnimBuildingPreRendered, pGTASA + 0x662074);
}
void Events_Patch()
{
    HOOKPLT(PedDamageResponse, pGTASA + 0x66F0EC);
    CarInflictDmg_BackTo = pGTASA + 0x584070 + 0x1;
    aml->Redirect(pGTASA + 0x584064 + 0x1, (uintptr_t)CarInflictDmg_Inject);
}

// --------------------------------------------------------------- //

CLEO_Fn(RETURN_SCRIPT_EVENT)
{
    cleoaddon->Interrupt(handle);
}
CLEO_Fn(SET_SCRIPT_EVENT_SAVE_CONFIRMATION)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::SaveConfirmation]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_DELETE)
{
    HookPedDelete();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CharDelete]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_CREATE)
{
    HookPedAudioInit();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CharCreate]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CAR_DELETE)
{
    HookCarDelete();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CarDelete]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CAR_CREATE)
{
    HookCarAudioInit();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CarCreate]);
}
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_DELETE)
{
    HookObjectDelete();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::ObjectDelete]);
}
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_CREATE)
{
    HookObjectInit();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::ObjectCreate]);
}
CLEO_Fn(SET_SCRIPT_EVENT_ON_MENU)
{
    // main.cpp
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::OnMenu]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_PROCESS)
{
    HookPedProcess();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CharProcess]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CAR_PROCESS)
{
    HookCarProcess();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CarProcess]);
}
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_PROCESS)
{
    HookObjectProcess();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::ObjectProcess]);
}
CLEO_Fn(SET_SCRIPT_EVENT_BUILDING_PROCESS)
{
    HookBuildingProcess();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::BuildingProcess]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_DAMAGE)
{
    // Events_Patch
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CharDamage]);
}
CLEO_Fn(SET_SCRIPT_EVENT_CAR_WEAPON_DAMAGE)
{
    // Events_Patch
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::CarWeaponDamage]);
}
CLEO_Fn(SET_SCRIPT_EVENT_BULLET_IMPACT)
{
    HookBulletImpact();
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::BulletImpact], 4);
}
CLEO_Fn(SET_SCRIPT_EVENT_BEFORE_GAME_PROCESS)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::BeforeGameProcess], 0);
}
CLEO_Fn(SET_SCRIPT_EVENT_AFTER_GAME_PROCESS)
{
    ScriptEvent::AddEvent(handle, scriptEvents[ScriptEventList::AfterGameProcess], 0);
}