#include "externs.h"

int timesGameRestarted = 0;

CLEO_Fn(IS_ON_MISSION)
{
    UpdateCompareFlag((CRunningScript*)handle, *OnAMissionFlag && *(ScriptSpace + *OnAMissionFlag));
}
CLEO_Fn(IS_ON_SAMP)
{
    UpdateCompareFlag((CRunningScript*)handle, (
        aml->GetLibHandle("libsamp.so") != NULL || aml->GetLibHandle("libvoice.so") != NULL || aml->GetLibHandle("libAlyn_SAMPMOBILE.so") != NULL || aml->HasMod("net.rusjj.resamp")
    ));
}
CLEO_Fn(IS_ON_CUTSCENE)
{
    UpdateCompareFlag((CRunningScript*)handle, *ms_running);
}
CLEO_Fn(FIX_CHAR_GROUND_BRIGHTNESS_AND_FADE_IN)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    int ground = cleo->ReadParam(handle)->i;
    int brightness = cleo->ReadParam(handle)->i;
    int fade = cleo->ReadParam(handle)->i;

    if(!ped) return;

    if (fade && ped->m_pRwClump) SetClumpAlpha(ped->m_pRwClump, 0);

    CVector *pedPos = NULL;
    if(ground)
    {
        pedPos = &ped->GetPosition();

        bool foundGround;
        CEntity *foundEntity;
        float foundZ = FindGroundZFor3DCoord(pedPos->x, pedPos->y, pedPos->z, &foundGround, &foundEntity);
        if (foundGround)
        {
            pedPos->z = GetDistanceFromCentreOfMassToBaseOfModel(ped) + foundZ;
            ped->Teleport(*pedPos);
        }
    }

    if(brightness)
    {
        if (!pedPos) pedPos = &ped->GetPosition();

        CEntity *outEntity;
        CColPoint outColPoint;
        if (ProcessVerticalLine(*pedPos, -1000.0, outColPoint, outEntity, 1, 0, 0, 0, 0, 0, NULL))
        {
            ped->m_fContactSurfaceBrightness =
                      outColPoint.m_nLightingB.day
                    * 0.033333334f
                    * (1.0f - *m_fDNBalanceParam)
                    + outColPoint.m_nLightingB.night
                    * 0.033333334f
                    * *m_fDNBalanceParam;
        }
    }
}
CLEO_Fn(IS_WEAPON_FIRE_TYPE)
{
    eWeaponType weaponType = (eWeaponType)cleo->ReadParam(handle)->i;
    int fireType = cleo->ReadParam(handle)->i;

    bool ok = false;
    auto weaponInfo = GetWeaponInfo(weaponType, 1);
    if (!weaponInfo) GetWeaponInfo(weaponType, 0);
    if (weaponInfo) ok = weaponInfo->m_nWeaponFire == fireType;
    UpdateCompareFlag((CRunningScript*)handle, ok);
}
CLEO_Fn(GET_CURRENT_SAVE_SLOT)
{
    cleo->GetPointerToScriptVar(handle)->i = gMobileMenu->m_nGenericGameStorageSlot;
}
CLEO_Fn(IS_GAME_FIRST_START)
{
    UpdateCompareFlag((CRunningScript*)handle, timesGameRestarted == 0);
}
CLEO_Fn(SET_CHAR_COORDINATES_SIMPLE)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    ped->m_matrix->pos.x = cleo->ReadParam(handle)->f;
    ped->m_matrix->pos.y = cleo->ReadParam(handle)->f;
    ped->m_matrix->pos.z = cleo->ReadParam(handle)->f;
}
CLEO_Fn(FRAME_MOD)
{
    int frame = cleo->ReadParam(handle)->i;
    UpdateCompareFlag((CRunningScript*)handle, *m_FrameCounter % frame == 0);
}
CLEO_Fn(RANDOM_PERCENT)
{
    int percent = cleo->ReadParam(handle)->i;
    int random = rand() % 101;
    UpdateCompareFlag((CRunningScript*)handle, random <= percent);
}
CLEO_Fn(GET_TRAILER_FROM_CAR)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    int trailerRef = -1;
    if(vehicle)
    {
        CVehicle* trailer = vehicle->m_pVehicleBeingTowed;
        if(trailer) trailerRef = GetVehicleRef(trailer);
    }
    cleo->GetPointerToScriptVar(handle)->i = trailerRef;
    UpdateCompareFlag((CRunningScript*)handle, trailerRef != -1);
}
CLEO_Fn(GET_CAR_FROM_TRAILER)
{
    CVehicle *trailer = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    int vehicleRef = -1;
    if(trailer)
    {
        CVehicle* vehicle = trailer->m_pTowingVehicle;
        if(vehicle) vehicleRef = GetVehicleRef(vehicle);
    }
    cleo->GetPointerToScriptVar(handle)->i = vehicleRef;
    UpdateCompareFlag((CRunningScript*)handle, vehicleRef != -1);
}
CLEO_Fn(GET_CAR_DUMMY_COORD)
{
    
}
CLEO_Fn(IS_CHEAT_ACTIVE)
{
    int i = cleo->ReadParam(handle)->i;
    UpdateCompareFlag((CRunningScript*)handle, m_aCheatsActive[i]);
}
CLEO_Fn(CHANGE_PLAYER_MONEY)
{
    int playerNum = cleo->ReadParam(handle)->i;
    int mode = cleo->ReadParam(handle)->i;
    int value = cleo->ReadParam(handle)->i;
    CPlayerPed* player = FindPlayerPed(playerNum);

    CPlayerInfo* info;
    if(player && (info = GetPlayerInfoForThisPlayerPed(player)))
    {
        switch(mode)
        {
            case 0:
                info->m_nMoney = value;
                info->m_nDisplayMoney = value;
                break;

            case 1:
                info->m_nMoney += value;
                break;

            case 2:
                info->m_nMoney -= value;
                break;

            case 3:
                info->m_nMoney -= value;
                if(info->m_nMoney < 0) info->m_nMoney = 0;
                break;

            case 4:
                if(info->m_nMoney > 0) info->m_nMoney -= value;
                break;
        }
    }
}
CLEO_Fn(CAR_HORN)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    if(!vehicle) return;

    vehicle->m_cHorn = 1;
    vehicle->PlayCarHorn();
}
CLEO_Fn(GET_STRING_LENGTH)
{
    char buf[128];
    CLEO_ReadStringEx(handle, buf, sizeof(buf));
    int len = strnlen(buf, 128);
    cleo->GetPointerToScriptVar(handle)->i = len;
    UpdateCompareFlag((CRunningScript*)handle, len > 0);
}
CLEO_Fn(COPY_STRING)
{
    
}
CLEO_Fn(GET_CAR_ALARM)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    int mode = -1;
    if(vehicle)
    {
        if(vehicle->CarAlarmState == 0) mode = 0;
        else mode = 1 + (vehicle->CarAlarmState != 0xFFFF);
    }
    cleo->GetPointerToScriptVar(handle)->i = mode;
}
CLEO_Fn(SET_CAR_ALARM)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    int mode = cleo->ReadParam(handle)->i;
    if(vehicle)
    {
        switch(mode)
        {
            case 0: vehicle->CarAlarmState = 0; return;
            case 1: vehicle->CarAlarmState = 0xFFFF; return;
            case 2: vehicle->CarAlarmState = 0xFFFE; return;
        }
    }
}
CLEO_Fn(GET_CHAR_MAX_HEALTH)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    float maxHealth = 0.0f;
    if(ped->m_nPedType <= PED_TYPE_PLAYER2)
    {
        maxHealth = (float)(GetPlayerInfoForThisPlayerPed((CPlayerPed*)ped)->m_nMaxHealth);
    }
    else
    {
        maxHealth = ped->m_fMaxHealth;
    }
    cleo->GetPointerToScriptVar(handle)->f = maxHealth;
}
CLEO_Fn(GET_CHAR_HEALTH_PERCENT)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    float maxHealth = 0.0f;
    if(ped->m_nPedType <= PED_TYPE_PLAYER2)
    {
        maxHealth = (float)(GetPlayerInfoForThisPlayerPed((CPlayerPed*)ped)->m_nMaxHealth);
    }
    else
    {
        maxHealth = ped->m_fMaxHealth;
    }
    float curHealth = ped->m_fHealth;
    cleo->GetPointerToScriptVar(handle)->f = (curHealth / maxHealth) * 100.0f;
}
CLEO_Fn(GET_CURRENT_CAMERA_MODE)
{
    cleo->GetPointerToScriptVar(handle)->i = TheCamera->m_apCams[TheCamera->m_nCurrentActiveCam].Mode;
}
CLEO_Fn(GET_CAR_COLLISION_INTENSITY)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    float intensity = 0.0f;
    if(vehicle) intensity = vehicle->m_fDamageIntensity;
    cleo->GetPointerToScriptVar(handle)->f = intensity;
    UpdateCompareFlag((CRunningScript*)handle, intensity > 0);
}
CLEO_Fn(GET_CAR_COLLISION_COORDINATES)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    CVector pos;
    if(vehicle) pos = vehicle->m_vecLastCollisionPosn;
    cleo->GetPointerToScriptVar(handle)->f = pos.x;
    cleo->GetPointerToScriptVar(handle)->f = pos.y;
    cleo->GetPointerToScriptVar(handle)->f = pos.z;
}
CLEO_Fn(STREAM_CUSTOM_SCRIPT_FROM_LABEL)
{
    
}
CLEO_Fn(GET_LAST_CREATED_CUSTOM_SCRIPT)
{
    
}
CLEO_Fn(GET_OBJECT_CENTRE_OF_MASS_TO_BASE_OF_MODEL)
{
    CObject *object = GetObjectFromRef(cleo->ReadParam(handle)->i);
    float dist = 999999.0f;
    if(object) dist = GetDistanceFromCentreOfMassToBaseOfModel(object);
    cleo->GetPointerToScriptVar(handle)->f = dist;
    UpdateCompareFlag((CRunningScript*)handle, dist != 999999.0f);
}
CLEO_Fn(GET_MODEL_TYPE)
{
    int modelId = cleo->ReadParam(handle)->i;
    int type = -1;
    if(modelId >= 0)
    {
        CBaseModelInfo* modelInfo = ms_modelInfoPtrs[modelId];
        if(modelInfo) type = modelInfo->GetModelType();
    }
    cleo->GetPointerToScriptVar(handle)->i = type;
}
CLEO_Fn(IS_STRING_EQUAL)
{
    bool bResult = true;
    char stringA[128], stringB[128], ignoreChar[2];
    CLEO_ReadStringEx(handle, stringA, sizeof(stringA));
    CLEO_ReadStringEx(handle, stringB, sizeof(stringB));
    int maxSize = cleo->ReadParam(handle)->i;
    int caseSensitive = cleo->ReadParam(handle)->i;
    CLEO_ReadStringEx(handle, ignoreChar, sizeof(ignoreChar));

    if(!caseSensitive)
    {
        toupper(stringA);
        toupper(stringB);
    }

    int i = 0;
    while (i < maxSize && stringA[i] != 0 && stringB[i] != 0)
    {
        if (stringA[i] != ignoreChar[0] && stringB[i] != ignoreChar[0])
        {
            if (stringA[i] != stringB[i])
            {
                bResult = false;
                break;
            }
        }
        ++i;
    }
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(IS_STRING_COMMENT)
{
    bool bResult = false;
    char buf[128];
    CLEO_ReadStringEx(handle, buf, sizeof(buf));

    unsigned int i = 0;
    while (buf[i] == ' ' && i <= 127) ++i;
    if (buf[i] == '#' || buf[i] == ';' || (buf[i] == '/' && buf[i + 1] == '/')) bResult = true;
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(DOES_CAR_HAVE_PART_NODE)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    int nodeId = cleo->ReadParam(handle)->i;
    bool bResult = false;
    if(vehicle) bResult = ((CAutomobile*)vehicle)->m_CarNodes[nodeId] != NULL;
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(GET_CURRENT_CHAR_WEAPONINFO)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    eWeaponType weaponType = ped->m_Weapons[ped->m_byteCurrentWeaponSlot].m_nType;
    CWeaponInfo *weaponInfo = GetWeaponInfo(weaponType, GetWeaponSkill(ped, weaponType));
    cleo->GetPointerToScriptVar(handle)->i = (int)weaponInfo;
    UpdateCompareFlag((CRunningScript*)handle, weaponInfo != NULL);
}
CLEO_Fn(GET_WEAPONINFO)
{
    int weaponId = cleo->ReadParam(handle)->i;
    int skill = cleo->ReadParam(handle)->i;
    CWeaponInfo *weaponInfo = GetWeaponInfo((eWeaponType)weaponId, skill);
    cleo->GetPointerToScriptVar(handle)->i = (int)weaponInfo;
    UpdateCompareFlag((CRunningScript*)handle, weaponInfo != NULL);
}
CLEO_Fn(GET_WEAPONINFO_MODELS)
{
    CWeaponInfo *weaponInfo = (CWeaponInfo*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = weaponInfo->m_nModelId1;
    cleo->GetPointerToScriptVar(handle)->i = weaponInfo->m_nModelId2;
}
CLEO_Fn(GET_WEAPONINFO_FLAGS)
{
    CWeaponInfo *weaponInfo = (CWeaponInfo*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = weaponInfo->m_nFlags;
}
CLEO_Fn(GET_WEAPONINFO_ANIMGROUP)
{
    CWeaponInfo *weaponInfo = (CWeaponInfo*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = weaponInfo->m_eAnimGroup;
}
CLEO_Fn(GET_WEAPONINFO_TOTAL_CLIP)
{
    CWeaponInfo *weaponInfo = (CWeaponInfo*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = weaponInfo->m_nAmmoClip;
}
CLEO_Fn(GET_WEAPONINFO_FIRE_TYPE)
{
    CWeaponInfo *weaponInfo = (CWeaponInfo*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = weaponInfo->m_nWeaponFire;
}
CLEO_Fn(GET_WEAPONINFO_SLOT)
{
    CWeaponInfo *weaponInfo = (CWeaponInfo*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = weaponInfo->m_nSlot;
}
CLEO_Fn(GET_CHAR_WEAPON_STATE)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ped->m_Weapons[ped->m_byteCurrentWeaponSlot].m_nState;
}
CLEO_Fn(GET_CHAR_WEAPON_CLIP)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ped->m_Weapons[ped->m_byteCurrentWeaponSlot].m_nAmmoInClip;
}
CLEO_Fn(GET_CHAR_COLLISION_SURFACE)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ped->m_nContactSurface;
}
CLEO_Fn(GET_CHAR_COLLISION_LIGHTING)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->f = ped->m_fContactSurfaceBrightness;
}
CLEO_Fn(GET_CAR_COLLISION_SURFACE)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->f = vehicle->m_fContactSurfaceBrightness;
}
CLEO_Fn(GET_CAR_COLLISION_LIGHTING)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = vehicle->m_nContactSurface;
}
CLEO_Fn(IS_CHAR_REALLY_IN_AIR)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    UpdateCompareFlag((CRunningScript*)handle, !ped->physicalFlags.bOnSolidSurface && !ped->physicalFlags.bSubmergedInWater);
}
CLEO_Fn(IS_CAR_REALLY_IN_AIR)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    UpdateCompareFlag((CRunningScript*)handle, !vehicle->physicalFlags.bOnSolidSurface && !vehicle->physicalFlags.bSubmergedInWater);
}
CLEO_Fn(IS_OBJECT_REALLY_IN_AIR)
{
    CObject *object = GetObjectFromRef(cleo->ReadParam(handle)->i);
    UpdateCompareFlag((CRunningScript*)handle, !object->physicalFlags.bOnSolidSurface && !object->physicalFlags.bSubmergedInWater);
}
CLEO_Fn(SIMULATE_OBJECT_DAMAGE)
{
    CObject *object = GetObjectFromRef(cleo->ReadParam(handle)->i);
    float damage = cleo->ReadParam(handle)->f;
    int type = cleo->ReadParam(handle)->i;
    ObjectDamage(object, damage, NULL, NULL, NULL, (eWeaponType)type);
}
CLEO_Fn(REQUEST_PRIORITY_MODEL)
{
    int model = cleo->ReadParam(handle)->i;
    RequestModel(model, eStreamingFlags::STREAMING_KEEP_IN_MEMORY | eStreamingFlags::STREAMING_MISSION_REQUIRED | eStreamingFlags::STREAMING_PRIORITY_REQUEST);
    AddToResourceManager(ScriptResourceManager, model, 2, (CRunningScript*)handle);
}
CLEO_Fn(LOAD_ALL_PRIORITY_MODELS_NOW)
{
    TimerSuspend();
    LoadAllRequestedModels(false);
    TimerResume();
}
CLEO_Fn(LOAD_SPECIAL_CHARACTER_FOR_ID)
{
    int id = cleo->ReadParam(handle)->i;
    CBaseModelInfo* modelInfo = ms_modelInfoPtrs[id];
    if(!modelInfo)
    {
        CPedModelInfo* pedModelInfo = AddPedModel(id);
        SetColModel(pedModelInfo, ms_colModelPed1, false);
        CPedModelInfo* basePedModelInfo = (CPedModelInfo*)ms_modelInfoPtrs[290];
        pedModelInfo->m_defaultPedType = ePedType::PED_TYPE_CIVMALE;
        pedModelInfo->m_defaultPedStats = ePedStats::PEDSTAT_STD_MISSION;
        if(basePedModelInfo)
        {
            pedModelInfo->m_motionAnimGroup = basePedModelInfo->m_motionAnimGroup;
            pedModelInfo->m_drivesWhichCars = basePedModelInfo->m_drivesWhichCars;
            pedModelInfo->m_flags = basePedModelInfo->m_flags;
            pedModelInfo->m_radio1 = basePedModelInfo->m_radio1;
            pedModelInfo->m_radio2 = basePedModelInfo->m_radio2;
            pedModelInfo->m_Race = basePedModelInfo->m_Race;
            pedModelInfo->m_AudioPedType = basePedModelInfo->m_AudioPedType;
            pedModelInfo->m_FirstVoice = basePedModelInfo->m_FirstVoice;
            pedModelInfo->m_LastVoice = basePedModelInfo->m_LastVoice;
            pedModelInfo->m_NextVoice = basePedModelInfo->m_NextVoice;
        }
    }
    char buf[128];
    CLEO_ReadStringEx(handle, buf, sizeof(buf));
    RequestSpecialModel(id, buf, eStreamingFlags::STREAMING_KEEP_IN_MEMORY | eStreamingFlags::STREAMING_MISSION_REQUIRED);
    SpecialCharacterModelsUsed.insert(id);
    AddToResourceManager(ScriptResourceManager, id, 2, (CRunningScript*)handle);
}
CLEO_Fn(UNLOAD_SPECIAL_CHARACTER_FROM_ID)
{
    int id = cleo->ReadParam(handle)->i;
    if(RemoveFromResourceManager(ScriptResourceManager, id, 2, (CRunningScript*)handle))
    {
        SetMissionDoesntRequireModel(id);
    }
    CBaseModelInfo *baseModelInfo = ms_modelInfoPtrs[id];
    if(baseModelInfo && baseModelInfo->m_nRefCount == 0) RemoveModel(id);
}
CLEO_Fn(GET_MODEL_BY_NAME)
{
    char buf[128];
    CLEO_ReadStringEx(handle, buf, sizeof(buf));
    int id = -1;
    CBaseModelInfo *baseModelInfo = GetModelInfoByName(name, &id);
    cleo->GetPointerToScriptVar(handle)->i = id;
    UpdateCompareFlag((CRunningScript*)handle, id > -1 && baseModelInfo);
}
CLEO_Fn(IS_MODEL_AVAILABLE_BY_NAME)
{
    char buf[128];
    CLEO_ReadStringEx(handle, buf, sizeof(buf));
    unsigned int offset, size;
    CDirectory::DirectoryInfo* dir = FindItem(*ms_pExtraObjectsDir, (const char*)name, offset, size);
    UpdateCompareFlag((CRunningScript*)handle, dir != NULL);
}
CLEO_Fn(GET_MODEL_DOESNT_EXIST_IN_RANGE)
{
    bool bResult = false;
    int start = cleo->ReadParam(handle)->i;
    int end = cleo->ReadParam(handle)->i;
    int id;

    for (id = start; id <= end; ++id)
    {
        if(!IsObjectInCdImage(id) || (ms_aInfoForModel[id].m_status != 1 && SpecialCharacterModelsUsed.find(id) != SpecialCharacterModelsUsed.end()))
        {
            bResult = true;
            break;
        }
    }
    if (!bResult) id = -1;
    cleo->GetPointerToScriptVar(handle)->i = id;
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(REMOVE_ALL_UNUSED_MODELS)
{
    RemoveAllUnusedModels();
}
CLEO_Fn(REMOVE_MODEL_IF_UNUSED)
{
    int id = cleo->ReadParam(handle)->i;
    CBaseModelInfo *baseModelInfo = ms_modelInfoPtrs[id];
    if(baseModelInfo && baseModelInfo->m_nRefCount == 0) RemoveModel(id);
}
CLEO_Fn(IS_CHAR_ON_FIRE)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    UpdateCompareFlag((CRunningScript*)handle, ped && ped->m_pFire != NULL);
}
CLEO_Fn(GET_CLOSEST_COP_NEAR_CHAR)
{
    
}
CLEO_Fn(GET_CLOSEST_COP_NEAR_POS)
{
    
}
CLEO_Fn(GET_ANY_CHAR_NO_SAVE_RECURSIVE)
{
    
}
CLEO_Fn(GET_ANY_CAR_NO_SAVE_RECURSIVE)
{
    
}
CLEO_Fn(GET_ANY_OBJECT_NO_SAVE_RECURSIVE)
{
    
}
CLEO_Fn(SET_CHAR_ARRESTED)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    ped->m_nPedState = ePedState::PEDSTATE_ARRESTED;
}
CLEO_Fn(GET_CHAR_PEDSTATE)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ped->m_nPedState;
}
CLEO_Fn(GET_CHAR_PROOFS)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ped->physicalFlags.bBulletProof;
    cleo->GetPointerToScriptVar(handle)->i = ped->physicalFlags.bFireProof;
    cleo->GetPointerToScriptVar(handle)->i = ped->physicalFlags.bExplosionProof;
    cleo->GetPointerToScriptVar(handle)->i = ped->physicalFlags.bCollisionProof;
    cleo->GetPointerToScriptVar(handle)->i = ped->physicalFlags.bMeleeProof;
}
CLEO_Fn(GET_CAR_PROOFS)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = vehicle->physicalFlags.bBulletProof;
    cleo->GetPointerToScriptVar(handle)->i = vehicle->physicalFlags.bFireProof;
    cleo->GetPointerToScriptVar(handle)->i = vehicle->physicalFlags.bExplosionProof;
    cleo->GetPointerToScriptVar(handle)->i = vehicle->physicalFlags.bCollisionProof;
    cleo->GetPointerToScriptVar(handle)->i = vehicle->physicalFlags.bMeleeProof;
}
CLEO_Fn(GET_OBJECT_PROOFS)
{
    CObject *object = GetObjectFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = object->physicalFlags.bBulletProof;
    cleo->GetPointerToScriptVar(handle)->i = object->physicalFlags.bFireProof;
    cleo->GetPointerToScriptVar(handle)->i = object->physicalFlags.bExplosionProof;
    cleo->GetPointerToScriptVar(handle)->i = object->physicalFlags.bCollisionProof;
    cleo->GetPointerToScriptVar(handle)->i = object->physicalFlags.bMeleeProof;
}
CLEO_Fn(IS_CHAR_WEAPON_VISIBLE_SET)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    CPlayerData* data = ped->m_pPlayerData;
    UpdateCompareFlag((CRunningScript*)handle, data && data->m_bRenderWeapon);
}
CLEO_Fn(GET_CHAR_STAT_ID)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = (int)ped->m_pPedStat;
}
CLEO_Fn(GET_OFFSET_FROM_CAMERA_IN_WORLD_COORDS)
{
    CVector offset, posReturn;
    offset.x = cleo->ReadParam(handle)->f;
    offset.y = cleo->ReadParam(handle)->f;
    offset.z = cleo->ReadParam(handle)->f;

    RwV3dTransformPoint(posReturn, offset, TheCamera->m_matCameraMatrix);

    cleo->GetPointerToScriptVar(handle)->f = posReturn.x;
    cleo->GetPointerToScriptVar(handle)->f = posReturn.y;
    cleo->GetPointerToScriptVar(handle)->f = posReturn.z;
}
CLEO_Fn(GET_OFFSET_FROM_MATRIX_IN_WORLD_COORDS)
{
    CMatrix* matrix = (CMatrix*)cleo->ReadParam(handle)->i;

    CVector offset, posReturn;
    offset.x = cleo->ReadParam(handle)->f;
    offset.y = cleo->ReadParam(handle)->f;
    offset.z = cleo->ReadParam(handle)->f;

    RwV3dTransformPoint(posReturn, offset, *matrix);

    cleo->GetPointerToScriptVar(handle)->f = posReturn.x;
    cleo->GetPointerToScriptVar(handle)->f = posReturn.y;
    cleo->GetPointerToScriptVar(handle)->f = posReturn.z;
}
CLEO_Fn(SET_CAR_COORDINATES_SIMPLE)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    vehicle->m_matrix->pos.x = cleo->ReadParam(handle)->f;
    vehicle->m_matrix->pos.y = cleo->ReadParam(handle)->f;
    vehicle->m_matrix->pos.z = cleo->ReadParam(handle)->f;
}
CLEO_Fn(GET_CHAR_DAMAGE_LAST_FRAME)
{
    
}
CLEO_Fn(GET_CAR_WEAPON_DAMAGE_LAST_FRAME)
{
    
}
CLEO_Fn(IS_ON_SCRIPTED_CUTSCENE)
{
    UpdateCompareFlag((CRunningScript*)handle, TheCamera->m_WideScreenOn);
}
CLEO_Fn(GET_MODEL_PED_TYPE_AND_STAT)
{
    
}
CLEO_Fn(PASS_TIME)
{
    
}
CLEO_Fn(GENERATE_RANDOM_INT_IN_RANGE_WITH_SEED)
{
    
}
CLEO_Fn(GENERATE_RANDOM_FLOAT_IN_RANGE_WITH_SEED)
{
    
}
CLEO_Fn(LOCATE_CAMERA_DISTANCE_TO_COORDINATES)
{
    
}
CLEO_Fn(GET_FX_SYSTEM_POINTER)
{
    
}
CLEO_Fn(ADD_FX_SYSTEM_PARTICLE)
{
    
}
CLEO_Fn(IS_FX_SYSTEM_AVAILABLE_WITH_NAME)
{
    
}
CLEO_Fn(SET_STRING_UPPER)
{
    
}
CLEO_Fn(SET_STRING_LOWER)
{
    
}
CLEO_Fn(STRING_FIND)
{
    
}
CLEO_Fn(CUT_STRING_AT)
{
    
}
CLEO_Fn(IS_STRING_CHARACTER_AT)
{
    
}
CLEO_Fn(GET_CHAR_RANDOM_SEED)
{
    
}
CLEO_Fn(GET_CAR_RANDOM_SEED)
{
    
}
CLEO_Fn(GET_OBJECT_RANDOM_SEED)
{
    
}
CLEO_Fn(GET_CHAR_MOVE_STATE)
{
    
}
CLEO_Fn(DONT_DELETE_CHAR_UNTIL_TIME)
{
    
}
CLEO_Fn(DONT_DELETE_CAR_UNTIL_TIME)
{
    
}
CLEO_Fn(GET_TIME_CHAR_IS_DEAD)
{
    
}
CLEO_Fn(GET_TIME_CAR_IS_DEAD)
{
    
}
CLEO_Fn(SET_CHAR_IGNORE_DAMAGE_ANIMS)
{
    
}
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_CHAR)
{
    
}
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_CAR)
{
    
}
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_OBJECT)
{
    
}
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_OBJECT)
{
    
}
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_CAR)
{
    
}
CLEO_Fn(LOCATE_OBJECT_DISTANCE_TO_OBJECT)
{
    
}
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_COORDINATES)
{
    
}
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_COORDINATES)
{
    
}
CLEO_Fn(LOCATE_OBJECT_DISTANCE_TO_COORDINATES)
{
    
}
CLEO_Fn(LOCATE_ENTITY_DISTANCE_TO_ENTITY)
{
    
}
CLEO_Fn(GET_ENTITY_COORDINATES)
{
    
}
CLEO_Fn(GET_ENTITY_HEADING)
{
    
}
CLEO_Fn(IS_CAR_OWNED_BY_PLAYER)
{
    
}
CLEO_Fn(SET_CAR_OWNED_BY_PLAYER)
{
    
}
CLEO_Fn(GET_MODEL_INFO)
{
    
}
CLEO_Fn(GET_CAR_ANIMGROUP)
{
    
}
CLEO_Fn(GET_CHAR_FEAR)
{
    
}
CLEO_Fn(IS_CAR_CONVERTIBLE)
{
    
}
CLEO_Fn(GET_CAR_VALUE)
{
    
}
CLEO_Fn(GET_CAR_PEDALS)
{
    
}
CLEO_Fn(GET_LOADED_LIBRARY)
{
    
}
CLEO_Fn(SET_CAR_MODEL_ALPHA)
{
    
}
CLEO_Fn(SET_CHAR_MODEL_ALPHA)
{
    
}
CLEO_Fn(SET_OBJECT_MODEL_ALPHA)
{
    
}
CLEO_Fn(GET_LOCAL_TIME)
{
    
}
CLEO_Fn(SET_SCRIPT_VAR)
{
    
}
CLEO_Fn(GET_SCRIPT_VAR)
{
    
}
CLEO_Fn(SET_CAR_DOOR_WINDOW_STATE)
{
    
}

DECL_HOOKv(InitialiseWhenRestarting)
{
    InitialiseWhenRestarting();

    ++timesGameRestarted;
}
void Misc_Patch()
{
    HOOKPLT(InitialiseWhenRestarting, pGTASA + 0x672014);
}