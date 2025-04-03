#include "externs.h"

int timesGameRestarted = 0;
const int maxDffFiles = 50000;
char modelNames[maxDffFiles][32];

inline void TransformFromObjectSpace(CEntity* self, CVector& outPos, const CVector& offset)
{
    if(self->m_matrix)
    {
        outPos = *self->m_matrix * offset;
        return;
    }
    TransformPoint((RwV3d&)outPos, self->m_placement, (RwV3d&)offset);
}
inline CVector TransformFromObjectSpace(CEntity* ent, const CVector& offset)
{
    auto result = CVector();
    if (ent->m_matrix)
    {
        result = *ent->m_matrix * offset;
        return result;
    }

    TransformPoint((RwV3d&)result, ent->m_placement, (RwV3d&)offset);
    return result;
}

CLEO_Fn(IS_ON_MISSION)
{
    cleoaddon->UpdateCompareFlag(handle, *OnAMissionFlag && *(ScriptSpace + *OnAMissionFlag));
}
CLEO_Fn(IS_ON_SAMP)
{
    cleoaddon->UpdateCompareFlag(handle, (
        aml->GetLibHandle("libsamp.so") != NULL || aml->GetLibHandle("libvoice.so") != NULL || aml->GetLibHandle("libAlyn_SAMPMOBILE.so") != NULL || aml->HasMod("net.rusjj.resamp")
    ));
}
CLEO_Fn(IS_ON_CUTSCENE)
{
    cleoaddon->UpdateCompareFlag(handle, *ms_running);
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
    cleoaddon->UpdateCompareFlag(handle, ok);
}
CLEO_Fn(GET_CURRENT_SAVE_SLOT)
{
    cleo->GetPointerToScriptVar(handle)->i = g_nCurrentSaveSlot;
}
CLEO_Fn(IS_GAME_FIRST_START)
{
    cleoaddon->UpdateCompareFlag(handle, timesGameRestarted == 0);
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
    cleoaddon->UpdateCompareFlag(handle, *m_FrameCounter % frame == 0);
}
CLEO_Fn(RANDOM_PERCENT)
{
    int percent = cleo->ReadParam(handle)->i;
    int random = rand() % 101;
    cleoaddon->UpdateCompareFlag(handle, random <= percent);
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
    cleoaddon->UpdateCompareFlag(handle, trailerRef != -1);
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
    cleoaddon->UpdateCompareFlag(handle, vehicleRef != -1);
}
CLEO_Fn(GET_CAR_DUMMY_COORD)
{
    bool bResult = false;
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    int dummyId = cleo->ReadParam(handle)->i;
    int worldCoords = cleo->ReadParam(handle)->i;
    int invertX = cleo->ReadParam(handle)->i;

    CVector posn = { 0.0f, 0.0f, 0.0f };

    if(0)
    {
        // CVector *vector = vehFuncs_Ext_GetVehicleDummyPosAdapted(vehicle, dummyId);
        // if (!vector) goto GET_CAR_DUMMY_COORD_OFFSET_getByModelInfo;
        // posn = *vector;
    }
    else
    {
      GET_CAR_DUMMY_COORD_OFFSET_getByModelInfo:
        CVehicleModelInfo *vehModelInfo = (CVehicleModelInfo *)(ms_modelInfoPtrs[vehicle->m_nModelIndex]);
        if (vehModelInfo) posn = vehModelInfo->m_VehicleStructure->m_positions[dummyId];
    }

    if (posn.x != 0.0f && posn.y != 0.0f && posn.z != 0.0f) bResult = true;
    if (invertX) posn.x = -posn.x;
    if (worldCoords) posn = TransformFromObjectSpace(vehicle, posn);

    cleo->GetPointerToScriptVar(handle)->f = posn.x;
    cleo->GetPointerToScriptVar(handle)->f = posn.y;
    cleo->GetPointerToScriptVar(handle)->f = posn.z;
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(IS_CHEAT_ACTIVE)
{
    int i = cleo->ReadParam(handle)->i;
    cleoaddon->UpdateCompareFlag(handle, m_aCheatsActive[i]);
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
    char buf[0xFF];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    int len = strnlen(buf, sizeof(buf));
    cleo->GetPointerToScriptVar(handle)->i = len;
    cleoaddon->UpdateCompareFlag(handle, len > 0);
}
CLEO_Fn(COPY_STRING)
{
    char buf[0xFF];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    cleoaddon->WriteString(handle, buf);
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
    cleoaddon->UpdateCompareFlag(handle, intensity > 0);
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
    cleoaddon->UpdateCompareFlag(handle, dist != 999999.0f);
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
    char stringA[0xFF], stringB[0xFF], ignoreChar[2];
    cleoaddon->ReadString(handle, stringA, sizeof(stringA));
    cleoaddon->ReadString(handle, stringB, sizeof(stringB));
    int maxSize = cleo->ReadParam(handle)->i;
    int caseSensitive = cleo->ReadParam(handle)->i;
    cleoaddon->ReadString(handle, ignoreChar, sizeof(ignoreChar));

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
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(IS_STRING_COMMENT)
{
    bool bResult = false;
    char buf[0xFF];
    cleoaddon->ReadString(handle, buf, sizeof(buf));

    unsigned int i = 0;
    while (buf[i] == ' ' && i <= 127) ++i;
    if (buf[i] == '#' || buf[i] == ';' || (buf[i] == '/' && buf[i + 1] == '/')) bResult = true;
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(DOES_CAR_HAVE_PART_NODE)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    int nodeId = cleo->ReadParam(handle)->i;
    bool bResult = false;
    if(vehicle) bResult = ((CAutomobile*)vehicle)->m_CarNodes[nodeId] != NULL;
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(GET_CURRENT_CHAR_WEAPONINFO)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    eWeaponType weaponType = ped->m_Weapons[ped->m_byteCurrentWeaponSlot].m_nType;
    CWeaponInfo *weaponInfo = GetWeaponInfo(weaponType, GetWeaponSkill(ped, weaponType));
    cleo->GetPointerToScriptVar(handle)->i = (int)weaponInfo;
    cleoaddon->UpdateCompareFlag(handle, weaponInfo != NULL);
}
CLEO_Fn(GET_WEAPONINFO)
{
    int weaponId = cleo->ReadParam(handle)->i;
    int skill = cleo->ReadParam(handle)->i;
    CWeaponInfo *weaponInfo = GetWeaponInfo((eWeaponType)weaponId, skill);
    cleo->GetPointerToScriptVar(handle)->i = (int)weaponInfo;
    cleoaddon->UpdateCompareFlag(handle, weaponInfo != NULL);
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
    cleoaddon->UpdateCompareFlag(handle, !ped->physicalFlags.bOnSolidSurface && !ped->physicalFlags.bSubmergedInWater);
}
CLEO_Fn(IS_CAR_REALLY_IN_AIR)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    cleoaddon->UpdateCompareFlag(handle, !vehicle->physicalFlags.bOnSolidSurface && !vehicle->physicalFlags.bSubmergedInWater);
}
CLEO_Fn(IS_OBJECT_REALLY_IN_AIR)
{
    CObject *object = GetObjectFromRef(cleo->ReadParam(handle)->i);
    cleoaddon->UpdateCompareFlag(handle, !object->physicalFlags.bOnSolidSurface && !object->physicalFlags.bSubmergedInWater);
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
    char buf[0xFF];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
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
    char buf[0xFF];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    int id = -1;
    CBaseModelInfo *baseModelInfo = GetModelInfoByName(buf, &id);
    cleo->GetPointerToScriptVar(handle)->i = id;
    cleoaddon->UpdateCompareFlag(handle, id > -1 && baseModelInfo);
}
CLEO_Fn(IS_MODEL_AVAILABLE_BY_NAME)
{
    char buf[0xFF];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    unsigned int offset, size;
    CDirectory::DirectoryInfo* dir = FindItem(*ms_pExtraObjectsDir, (const char*)buf, offset, size);
    cleoaddon->UpdateCompareFlag(handle, dir != NULL);
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
    cleoaddon->UpdateCompareFlag(handle, bResult);
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
    cleoaddon->UpdateCompareFlag(handle, ped && ped->m_pFire != NULL);
}
CLEO_Fn(GET_CLOSEST_COP_NEAR_CHAR)
{
    CPed *closestPed = NULL, *obj;
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    float radius = cleo->ReadParam(handle)->f;
    bool alive = cleo->ReadParam(handle)->i;
    bool inCar = cleo->ReadParam(handle)->i;
    bool onFoot = cleo->ReadParam(handle)->i;
    bool seenInFront = cleo->ReadParam(handle)->i;

    float radiusSqr = radius * radius; // small opt.

    CVector pos = ped->GetPosition();
    auto size = (*ms_pPedPool)->m_nSize;
    for(int i = 0; i < size; ++i)
    {
        obj = (*ms_pPedPool)->GetAt(i);
        if(obj && obj != ped)
        {
            if(obj->IsInAnyVehicle())
            {
                if(onFoot && !inCar) continue;
            }
            else
            {
                if(!onFoot && inCar) continue;
            }

            if(!alive || obj->m_fHealth > 0.0f)
            {
                if(radius >= 1000.0f || (pos - obj->GetPosition()).MagnitudeSqr() <= radiusSqr)
                {
                    if(obj->m_nPedType == ePedType::PED_TYPE_COP || (obj->m_nModelIndex >= 280 && obj->m_nModelIndex <= 288))
                    {
                        if(!seenInFront || OurPedCanSeeThisEntity(obj, ped, false))
                        {
                            // Update closest one
                            if(closestPed)
                            {
                                if((closestPed->GetPosition() - pos).MagnitudeSqr() < (obj->GetPosition() - pos).MagnitudeSqr())
                                {
                                    continue;
                                }
                            }
                            closestPed = obj;
                        }
                    }
                }
            }
        }
    }

    if(closestPed)
    {
        cleo->GetPointerToScriptVar(handle)->i = GetPedRef(closestPed);
        cleoaddon->UpdateCompareFlag(handle, false);
    }
    else
    {
        cleo->GetPointerToScriptVar(handle)->i = -1;
        cleoaddon->UpdateCompareFlag(handle, false);
    }
}
CLEO_Fn(GET_CLOSEST_COP_NEAR_POS)
{
    CPed *closestPed = NULL, *obj;
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    float radius = cleo->ReadParam(handle)->f;
    bool alive = cleo->ReadParam(handle)->i;
    bool inCar = cleo->ReadParam(handle)->i;
    bool onFoot = cleo->ReadParam(handle)->i;

    float radiusSqr = radius * radius; // small opt.

    CVector pos(x, y, z);
    auto size = (*ms_pPedPool)->m_nSize;
    for(int i = 0; i < size; ++i)
    {
        obj = (*ms_pPedPool)->GetAt(i);
        if(obj)
        {
            if(obj->IsInAnyVehicle())
            {
                if(onFoot && !inCar) continue;
            }
            else
            {
                if(!onFoot && inCar) continue;
            }

            if(!alive || obj->m_fHealth > 0.0f)
            {
                if(radius >= 1000.0f || (pos - obj->GetPosition()).MagnitudeSqr() <= radiusSqr)
                {
                    if(obj->m_nPedType == ePedType::PED_TYPE_COP || (obj->m_nModelIndex >= 280 && obj->m_nModelIndex <= 288))
                    {
                        // Update closest one
                        if(closestPed)
                        {
                            if((closestPed->GetPosition() - pos).MagnitudeSqr() < (obj->GetPosition() - pos).MagnitudeSqr())
                            {
                                continue;
                            }
                        }
                        closestPed = obj;
                    }
                }
            }
        }
    }

    if(closestPed)
    {
        cleo->GetPointerToScriptVar(handle)->i = GetPedRef(closestPed);
        cleoaddon->UpdateCompareFlag(handle, false);
    }
    else
    {
        cleo->GetPointerToScriptVar(handle)->i = -1;
        cleoaddon->UpdateCompareFlag(handle, false);
    }
}
CLEO_Fn(GET_ANY_CHAR_NO_SAVE_RECURSIVE)
{
    CPed* obj = NULL, *it;
    bool found = false;
    int startidx = cleo->ReadParam(handle)->i, result = -1;

    auto size = (*ms_pPedPool)->m_nSize;
    for(int i = startidx; i < size; ++i)
    {
        it = (*ms_pPedPool)->GetAt(i);
        if(it)
        {
            startidx = i + 1;
            found = true;
            result = GetPedRef(it);
            break;
        }
    }

    cleo->GetPointerToScriptVar(handle)->i = startidx;
    cleo->GetPointerToScriptVar(handle)->i = result;
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(GET_ANY_CAR_NO_SAVE_RECURSIVE)
{
    CVehicle* obj = NULL, *it;
    bool found = false;
    int startidx = cleo->ReadParam(handle)->i, result = -1;

    auto size = (*ms_pVehiclePool)->m_nSize;
    for(int i = startidx; i < size; ++i)
    {
        it = (*ms_pVehiclePool)->GetAt(i);
        if(it)
        {
            startidx = i + 1;
            found = true;
            result = GetVehicleRef(it);
            break;
        }
    }

    cleo->GetPointerToScriptVar(handle)->i = startidx;
    cleo->GetPointerToScriptVar(handle)->i = result;
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(GET_ANY_OBJECT_NO_SAVE_RECURSIVE)
{
    CObject* obj = NULL, *it;
    bool found = false;
    int startidx = cleo->ReadParam(handle)->i, result = -1;

    auto size = (*ms_pObjectPool)->m_nSize;
    for(int i = startidx; i < size; ++i)
    {
        it = (*ms_pObjectPool)->GetAt(i);
        if(it)
        {
            startidx = i + 1;
            found = true;
            result = GetObjectRef(it);
            break;
        }
    }

    cleo->GetPointerToScriptVar(handle)->i = startidx;
    cleo->GetPointerToScriptVar(handle)->i = result;
    cleoaddon->UpdateCompareFlag(handle, found);
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
    cleoaddon->UpdateCompareFlag(handle, data && data->m_bRenderWeapon);
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
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);

    int ret = -1, type = -1, part = -1;
    float damage = 0.0f;

    if(ped)
    {
        auto xdata = GetExtData(ped);
        if(xdata)
        {
            if(xdata->lastDamageEntity)
            {
                ret = GetPedRef((CPed*)xdata->lastDamageEntity);
                type = xdata->lastDamageWeapon;
                part = xdata->lastDamagePart;
                damage = xdata->lastDamageIntensity;
            }
        }
    }

    cleo->GetPointerToScriptVar(handle)->i = ret;
    cleo->GetPointerToScriptVar(handle)->i = type;
    cleo->GetPointerToScriptVar(handle)->i = part;
    cleo->GetPointerToScriptVar(handle)->f = damage;
    cleoaddon->UpdateCompareFlag(handle, damage > 0);
}
CLEO_Fn(GET_CAR_WEAPON_DAMAGE_LAST_FRAME)
{
    CVehicle* veh = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    int ret = -1, type = -1;
    float damage = 0.0f;

    if(veh)
    {
        auto xdata = GetExtData(veh);
        if(xdata)
        {
            if(xdata->lastDamageEntity)
            {
                ret = GetPedRef((CPed*)xdata->lastDamageEntity);
            }
            type = xdata->lastDamageWeapon;
            damage = xdata->lastDamageIntensity;
        }
    }

    cleo->GetPointerToScriptVar(handle)->i = ret;
    cleo->GetPointerToScriptVar(handle)->i = type;
    cleo->GetPointerToScriptVar(handle)->f = damage;
    cleoaddon->UpdateCompareFlag(handle, damage > 0);
}
CLEO_Fn(IS_ON_SCRIPTED_CUTSCENE)
{
    cleoaddon->UpdateCompareFlag(handle, TheCamera->m_WideScreenOn);
}
CLEO_Fn(GET_MODEL_PED_TYPE_AND_STAT)
{
    bool bResult = false;
    int model = cleo->ReadParam(handle)->i;
    int type = -1, stat = -1;
    CPedModelInfo *pedModelInfo = (CPedModelInfo*)ms_modelInfoPtrs[model];
    if (pedModelInfo)
    {
        bResult = true;
        type = pedModelInfo->m_defaultPedType;
        stat = pedModelInfo->m_defaultPedStats;
    }
    cleo->GetPointerToScriptVar(handle)->i = type;
    cleo->GetPointerToScriptVar(handle)->i = stat;
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(PASS_TIME)
{
    int mins = cleo->ReadParam(handle)->i;
    PassTime(mins);
}
CLEO_Fn(GENERATE_RANDOM_INT_IN_RANGE_WITH_SEED)
{
    int seed = cleo->ReadParam(handle)->i;
    int min = cleo->ReadParam(handle)->i;
    int max = cleo->ReadParam(handle)->i - 1;

    unsigned int resetSeed = rand() / 2;
    srand(seed);
    cleo->GetPointerToScriptVar(handle)->i = rand() % max + min;
    srand((time(NULL) / 2) + resetSeed);
}
CLEO_Fn(GENERATE_RANDOM_FLOAT_IN_RANGE_WITH_SEED)
{
    int seed = cleo->ReadParam(handle)->i;
    float min = cleo->ReadParam(handle)->f;
    float max = cleo->ReadParam(handle)->f;

    unsigned int resetSeed = rand() / 2;
    srand(seed);
    cleo->GetPointerToScriptVar(handle)->f = fmaf((float)rand() / (float)RAND_MAX, max - min, min);
    srand((time(NULL) / 2) + resetSeed);
}
CLEO_Fn(LOCATE_CAMERA_DISTANCE_TO_COORDINATES)
{
    bool bResult = false;
    CVector pos;
    pos.x = cleo->ReadParam(handle)->f;
    pos.y = cleo->ReadParam(handle)->f;
    pos.z = cleo->ReadParam(handle)->f;
    float radius = cleo->ReadParam(handle)->f;
    if ((TheCamera->m_apCams[TheCamera->m_nCurrentActiveCam].Source - pos).MagnitudeSqr() <= radius * radius) bResult = true;
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(GET_FX_SYSTEM_POINTER)
{
    int hndl = cleo->ReadParam(handle)->i;
    if(hndl >= 0)
    {
        hndl = GetActualScriptThingIndex(hndl, 1);
        if(hndl >= 0)
        {
            cleo->GetPointerToScriptVar(handle)->i = (int)(ScriptEffectSystemArray[hndl].pFXSystem);
            cleoaddon->UpdateCompareFlag(handle, true);
            return;
        }
    }
    cleo->GetPointerToScriptVar(handle)->i = 0;
    cleoaddon->UpdateCompareFlag(handle, false);
}
CLEO_Fn(ADD_FX_SYSTEM_PARTICLE)
{
    int hndl = cleo->ReadParam(handle)->i;

    RwV3d pos;
    pos.x = cleo->ReadParam(handle)->f;
    pos.y = cleo->ReadParam(handle)->f;
    pos.z = cleo->ReadParam(handle)->f;

    RwV3d vel;
    vel.x = cleo->ReadParam(handle)->f;
    vel.y = cleo->ReadParam(handle)->f;
    vel.z = cleo->ReadParam(handle)->f;

    float size = cleo->ReadParam(handle)->f;

    float brightness = cleo->ReadParam(handle)->f;

    float r = cleo->ReadParam(handle)->f;
    float g = cleo->ReadParam(handle)->f;
    float b = cleo->ReadParam(handle)->f;
    float a = cleo->ReadParam(handle)->f;

    float lastFactor = cleo->ReadParam(handle)->f;

    if(hndl >= 0)
    {
        hndl = GetActualScriptThingIndex(hndl, 1);
        if(hndl >= 0)
        {
            FxSystem_c *fx = ScriptEffectSystemArray[hndl].pFXSystem;
            FxPrtMult_c *prtMult = new FxPrtMult_c(r, g, b, a, size, 0.0, lastFactor);
            AddParticle(fx, &pos, &vel, 0.0f, prtMult, -1.0f, brightness, 0.0f, 0);
        }
    }
}
CLEO_Fn(IS_FX_SYSTEM_AVAILABLE_WITH_NAME)
{
    char buf[0xFF];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    cleoaddon->UpdateCompareFlag(handle, FindFxSystemBP(g_fxMan, buf) != NULL);
}
CLEO_Fn(SET_STRING_UPPER)
{
    char* str = (char*)cleo->ReadParam(handle)->i;
    int i = -1;
    while(str[++i]) str[i] = std::toupper(str[i]);
}
CLEO_Fn(SET_STRING_LOWER)
{
    char* str = (char*)cleo->ReadParam(handle)->i;
    int i = -1;
    while(str[++i]) str[i] = std::tolower(str[i]);
}
CLEO_Fn(STRING_FIND)
{
    int mode = cleo->ReadParam(handle)->i;
    char buf[0xFF];
    cleoaddon->ReadString(handle, buf, sizeof(buf)); std::string str = buf;
    cleoaddon->ReadString(handle, buf, sizeof(buf)); std::string strFind = buf;
    
    size_t found = std::string::npos;
    if (mode == 0) found = str.find(strFind);
    else if (mode == 1) found = str.rfind(strFind);

    if (found != std::string::npos)
    {
        cleo->GetPointerToScriptVar(handle)->i = found;
        cleoaddon->UpdateCompareFlag(handle, true);
    }
    else
    {
        cleo->GetPointerToScriptVar(handle)->i = -1;
        cleoaddon->UpdateCompareFlag(handle, false);
    }
}
CLEO_Fn(CUT_STRING_AT)
{
    char* str = (char*)cleo->ReadParam(handle)->i;
    str[cleo->ReadParam(handle)->i] = 0;
}
CLEO_Fn(IS_STRING_CHARACTER_AT)
{
    char str[0xFF], cha[4];
    cleoaddon->ReadString(handle, str, sizeof(str));
    cleoaddon->ReadString(handle, cha, sizeof(cha));
    int index = cleo->ReadParam(handle)->i;
    cleoaddon->UpdateCompareFlag(handle, (str[index] == cha[0]));
}
CLEO_Fn(GET_CHAR_RANDOM_SEED)
{
    CEntity *ent = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ent->m_nRandomSeed;
}
CLEO_Fn(GET_CAR_RANDOM_SEED)
{
    CEntity *ent = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ent->m_nRandomSeed;
}
CLEO_Fn(GET_OBJECT_RANDOM_SEED)
{
    CEntity *ent = GetObjectFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ent->m_nRandomSeed;
}
CLEO_Fn(GET_CHAR_MOVE_STATE)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ped->m_nMoveState;
}
CLEO_Fn(DONT_DELETE_CHAR_UNTIL_TIME)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    unsigned int time = cleo->ReadParam(handle)->i + *m_snTimeInMilliseconds;
    if(ped->m_nTimeTillWeNeedThisPed < time) ped->m_nTimeTillWeNeedThisPed = time;
}
CLEO_Fn(DONT_DELETE_CAR_UNTIL_TIME)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    unsigned int time = cleo->ReadParam(handle)->i + *m_snTimeInMilliseconds;
    if(vehicle->DontUseSmallerRemovalRange < time) vehicle->DontUseSmallerRemovalRange = time;
}
CLEO_Fn(GET_TIME_CHAR_IS_DEAD)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    if(ped->IsDead()) cleo->GetPointerToScriptVar(handle)->i = ped->m_nTimeOfDeath - *m_snTimeInMilliseconds;
    else cleo->GetPointerToScriptVar(handle)->i = -1;
}
CLEO_Fn(GET_TIME_CAR_IS_DEAD)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    if(vehicle->m_fHealth <= 0.0f) cleo->GetPointerToScriptVar(handle)->i = vehicle->m_nTimeOfDeath - *m_snTimeInMilliseconds;
    else cleo->GetPointerToScriptVar(handle)->i = -1;
}
CLEO_Fn(SET_CHAR_IGNORE_DAMAGE_ANIMS)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    PedExtVars* data = GetExtData(ped);

    bool set = (cleo->ReadParam(handle)->i == 1);
    if(data)
    {
        data->ignoreDamageAnims = set;
    }
}
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_CHAR)
{
    CEntity *ent1 = GetPedFromRef(cleo->ReadParam(handle)->i);
    CEntity *ent2 = GetPedFromRef(cleo->ReadParam(handle)->i);
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - ent2->GetPosition()).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_CAR)
{
    CEntity *ent1 = GetPedFromRef(cleo->ReadParam(handle)->i);
    CEntity *ent2 = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - ent2->GetPosition()).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_OBJECT)
{
    CEntity *ent1 = GetPedFromRef(cleo->ReadParam(handle)->i);
    CEntity *ent2 = GetObjectFromRef(cleo->ReadParam(handle)->i);
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - ent2->GetPosition()).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_OBJECT)
{
    CEntity *ent1 = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    CEntity *ent2 = GetObjectFromRef(cleo->ReadParam(handle)->i);
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - ent2->GetPosition()).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_CAR)
{
    CEntity *ent1 = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    CEntity *ent2 = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - ent2->GetPosition()).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_OBJECT_DISTANCE_TO_OBJECT)
{
    CEntity *ent1 = GetObjectFromRef(cleo->ReadParam(handle)->i);
    CEntity *ent2 = GetObjectFromRef(cleo->ReadParam(handle)->i);
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - ent2->GetPosition()).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_COORDINATES)
{
    CEntity *ent1 = GetPedFromRef(cleo->ReadParam(handle)->i);
    CVector pos;
    pos.x = cleo->ReadParam(handle)->f;
    pos.y = cleo->ReadParam(handle)->f;
    pos.z = cleo->ReadParam(handle)->f;
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - pos).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_COORDINATES)
{
    CEntity *ent1 = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    CVector pos;
    pos.x = cleo->ReadParam(handle)->f;
    pos.y = cleo->ReadParam(handle)->f;
    pos.z = cleo->ReadParam(handle)->f;
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - pos).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_OBJECT_DISTANCE_TO_COORDINATES)
{
    CEntity *ent1 = GetObjectFromRef(cleo->ReadParam(handle)->i);
    CVector pos;
    pos.x = cleo->ReadParam(handle)->f;
    pos.y = cleo->ReadParam(handle)->f;
    pos.z = cleo->ReadParam(handle)->f;
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - pos).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(LOCATE_ENTITY_DISTANCE_TO_ENTITY)
{
    CEntity *ent1 = (CEntity*)(cleo->ReadParam(handle)->i);
    CVector pos;
    pos.x = cleo->ReadParam(handle)->f;
    pos.y = cleo->ReadParam(handle)->f;
    pos.z = cleo->ReadParam(handle)->f;
    float radius = cleo->ReadParam(handle)->f;

    cleoaddon->UpdateCompareFlag(handle, (ent1->GetPosition() - pos).MagnitudeSqr() <= radius * radius);
}
CLEO_Fn(GET_ENTITY_COORDINATES)
{
    CEntity *ent = (CEntity*)(cleo->ReadParam(handle)->i);
    CVector& pos = ent->GetPosition();
    cleo->GetPointerToScriptVar(handle)->f = pos.x;
    cleo->GetPointerToScriptVar(handle)->f = pos.y;
    cleo->GetPointerToScriptVar(handle)->f = pos.z;
}
CLEO_Fn(GET_ENTITY_HEADING)
{
    CEntity *ent = (CEntity*)(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->f = ent->GetHeading();
}
CLEO_Fn(IS_CAR_OWNED_BY_PLAYER)
{
    CVehicle *vehicle = (CVehicle*)(cleo->ReadParam(handle)->i);
    cleoaddon->UpdateCompareFlag(handle, vehicle->vehicleFlags.bHasBeenOwnedByPlayer);
}
CLEO_Fn(SET_CAR_OWNED_BY_PLAYER)
{
    CVehicle *vehicle = (CVehicle*)(cleo->ReadParam(handle)->i);
    vehicle->vehicleFlags.bHasBeenOwnedByPlayer = (cleo->ReadParam(handle)->i != 0);
}
CLEO_Fn(GET_MODEL_INFO)
{
    int model = cleo->ReadParam(handle)->i;
    CBaseModelInfo* modelInfo = NULL;
    if(model >= 0) modelInfo = ms_modelInfoPtrs[model];
    cleo->GetPointerToScriptVar(handle)->i = (int)modelInfo;
    cleoaddon->UpdateCompareFlag(handle, modelInfo != NULL);
}
CLEO_Fn(GET_CAR_ANIMGROUP)
{
    CVehicle *vehicle = (CVehicle*)(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = vehicle->m_pHandling->AnimGroup;
}
CLEO_Fn(GET_CHAR_FEAR)
{
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = ped->m_pPedStat->m_nFear;
}
CLEO_Fn(IS_CAR_CONVERTIBLE)
{
    CVehicle *vehicle = (CVehicle*)(cleo->ReadParam(handle)->i);
    cleoaddon->UpdateCompareFlag(handle, vehicle->m_pHandling->m_bConvertible);
}
CLEO_Fn(GET_CAR_VALUE)
{
    CVehicle *vehicle = (CVehicle*)(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->i = vehicle->m_pHandling->nMonetaryValue;
}
CLEO_Fn(GET_CAR_PEDALS)
{
    CVehicle *vehicle = (CVehicle*)(cleo->ReadParam(handle)->i);
    cleo->GetPointerToScriptVar(handle)->f = vehicle->m_fGasPedal;
    cleo->GetPointerToScriptVar(handle)->f = vehicle->m_fBrakePedal;
}
CLEO_Fn(GET_LOADED_LIBRARY)
{
    char buf[64];
    cleoaddon->ReadString(handle, buf, sizeof(buf));
    void* library = dlopen(buf, RTLD_LAZY);
    cleo->GetPointerToScriptVar(handle)->i = (int)library;
    cleoaddon->UpdateCompareFlag(handle, library != NULL);
}
CLEO_Fn(RETURN_TIMES)
{
    int times = cleo->ReadParam(handle)->i;
    CRunningScript* script = (CRunningScript*)handle;
    int curSp = (int)script->StackDepth - times;
    if(curSp < 0) curSp = 0;
    script->StackDepth = curSp;
    script->PCPointer = script->PCStack[curSp];
}
CLEO_Fn(GET_THIRD_PERSON_CAMERA_TARGET)
{
    CVector sourcePos;
	CVector cameraPos = { 0.0f, 0.0f, 0.0f };
	CVector pointPos =  { 0.0f, 0.0f, 0.0f };
    float range = cleo->ReadParam(handle)->f;
	sourcePos.x = cleo->ReadParam(handle)->f;
	sourcePos.y = cleo->ReadParam(handle)->f;
	sourcePos.z = cleo->ReadParam(handle)->f;

    Find3rdPersonCamTargetVector(TheCamera, range, sourcePos, cameraPos, pointPos);

    cleo->GetPointerToScriptVar(handle)->f = cameraPos.x;
    cleo->GetPointerToScriptVar(handle)->f = cameraPos.y;
    cleo->GetPointerToScriptVar(handle)->f = cameraPos.z;

    cleo->GetPointerToScriptVar(handle)->f = pointPos.x;
    cleo->GetPointerToScriptVar(handle)->f = pointPos.y;
    cleo->GetPointerToScriptVar(handle)->f = pointPos.z;
}
CLEO_Fn(GET_DISTANCE_MULTIPLIER)
{
    cleo->GetPointerToScriptVar(handle)->f = TheCamera->m_fLODDistMultiplier;
    cleo->GetPointerToScriptVar(handle)->f = TheCamera->m_fGenerationDistMultiplier;
}
CLEO_Fn(GET_ACTIVE_CAMERA_ROTATION)
{
    CMatrix* matrix = TheCamera->GetMatrix();
    if(!matrix)
    {
        cleo->GetPointerToScriptVar(handle)->f = 0.0f;
        cleo->GetPointerToScriptVar(handle)->f = 0.0f;
        cleo->GetPointerToScriptVar(handle)->f = 0.0f;
        return;
    }

    float x = matrix->right.x;
    float y = matrix->right.y;
    float z = matrix->right.z;
    float angleX = GetATanOfXY(z, sqrtf(x * x + y * y)) * -57.295776f - 270.0f;
    float angleZ = GetATanOfXY(x, y) * -57.295776f - 180.0f;

    x = matrix->up.x;
    y = matrix->up.y;
    z = matrix->up.z;
    float angleY = GetATanOfXY(z, sqrtf(x * x + y * y)) * -57.295776f - 270.0f;

    while (angleX < 0.0f) angleX += 360.0f;
    while (angleY < 0.0f) angleY += 360.0f;
    while (angleZ < 0.0f) angleZ += 360.0f;

    cleo->GetPointerToScriptVar(handle)->f = angleX;
    cleo->GetPointerToScriptVar(handle)->f = angleY;
    cleo->GetPointerToScriptVar(handle)->f = angleZ;
}
CLEO_Fn(GET_CLOSEST_WATER_DISTANCE)
{
    cleo->GetPointerToScriptVar(handle)->f = TheCamera->DistanceToWater;
    cleo->GetPointerToScriptVar(handle)->f = TheCamera->HeightOfNearestWater;
}
CLEO_Fn(GET_CAMERA_STRUCT)
{
    cleo->GetPointerToScriptVar(handle)->i = (int)TheCamera;
    cleo->GetPointerToScriptVar(handle)->i = (int)&TheCamera->m_apCams[TheCamera->m_nCurrentActiveCam];
}
CLEO_Fn(GET_CAMERA_ROTATION_INPUT_VALUES)
{
    CCam* cam = &TheCamera->m_apCams[TheCamera->m_nCurrentActiveCam];
    cleo->GetPointerToScriptVar(handle)->f = cam->Beta * -57.295776f;
    cleo->GetPointerToScriptVar(handle)->f = cam->Alpha * 57.295776f;
}
CLEO_Fn(SET_CAMERA_ROTATION_INPUT_VALUES)
{
    CCam* cam = &TheCamera->m_apCams[TheCamera->m_nCurrentActiveCam];
    cam->Beta = cleo->ReadParam(handle)->f * -0.01745329252f;
    cam->Alpha = cleo->ReadParam(handle)->f * 0.01745329252f;
}
CLEO_Fn(SET_ON_MISSION)
{
    *(ScriptSpace + *OnAMissionFlag) = cleo->ReadParam(handle)->i;
}
CLEO_Fn(GET_MODEL_NAME_POINTER)
{
    int id = cleo->ReadParam(handle)->i;
    if(id >= 0 && id < maxDffFiles)
    {
        cleo->GetPointerToScriptVar(handle)->i = (int)&modelNames[id][0];
    }
    else
    {
        cleo->GetPointerToScriptVar(handle)->i = 0;
    }
}
CLEO_Fn(SET_CAR_MODEL_ALPHA)
{
    CEntity *ent = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    if (ent->m_pRwObject)
    {
        SetRwObjectAlpha(ent, cleo->ReadParam(handle)->i);
        cleoaddon->UpdateCompareFlag(handle, true);
    }
    else
    {
        cleoaddon->UpdateCompareFlag(handle, false);
    }
}
CLEO_Fn(SET_CHAR_MODEL_ALPHA)
{
    CEntity *ent = GetPedFromRef(cleo->ReadParam(handle)->i);
    if (ent->m_pRwObject)
    {
        SetRwObjectAlpha(ent, cleo->ReadParam(handle)->i);
        cleoaddon->UpdateCompareFlag(handle, true);
    }
    else
    {
        cleoaddon->UpdateCompareFlag(handle, false);
    }
}
CLEO_Fn(SET_OBJECT_MODEL_ALPHA)
{
    CEntity *ent = GetObjectFromRef(cleo->ReadParam(handle)->i);
    if (ent->m_pRwObject)
    {
        SetRwObjectAlpha(ent, cleo->ReadParam(handle)->i);
        cleoaddon->UpdateCompareFlag(handle, true);
    }
    else
    {
        cleoaddon->UpdateCompareFlag(handle, false);
    }
}
CLEO_Fn(GET_LOCAL_TIME)
{
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    cleo->GetPointerToScriptVar(handle)->i = 1900 + tm.tm_year;
    cleo->GetPointerToScriptVar(handle)->i = 1 + tm.tm_mon;
    cleo->GetPointerToScriptVar(handle)->i = tm.tm_wday;
    cleo->GetPointerToScriptVar(handle)->i = tm.tm_mday;
    cleo->GetPointerToScriptVar(handle)->i = tm.tm_hour;
    cleo->GetPointerToScriptVar(handle)->i = tm.tm_min;
    cleo->GetPointerToScriptVar(handle)->i = tm.tm_sec;

    struct timeval tv;
    gettimeofday(&tv, NULL);
    cleo->GetPointerToScriptVar(handle)->i = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}
CLEO_Fn(SET_SCRIPT_VAR)
{
    CRunningScript* script = (CRunningScript*)cleo->ReadParam(handle)->i;
    int varnum = cleo->ReadParam(handle)->i;
    int val = cleo->ReadParam(handle)->i; // not only integer, any value
    script->Locals[varnum] = val;
}
CLEO_Fn(GET_SCRIPT_VAR)
{
    CRunningScript* script = (CRunningScript*)cleo->ReadParam(handle)->i;
    int varnum = cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = script->Locals[varnum]; // not only integer, any value
}
CLEO_Fn(SET_CAR_DOOR_WINDOW_STATE)
{
    CVehicle *vehicle = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    unsigned int door = cleo->ReadParam(handle)->u;
    unsigned int state = cleo->ReadParam(handle)->u;
    if(state)
    {
        SetWindowOpenFlag(vehicle, door);
    }
    else
    {
        ClearWindowOpenFlag(vehicle, door);
    }
}

DECL_HOOKv(InitialiseWhenRestarting)
{
    InitialiseWhenRestarting();

    ++timesGameRestarted;
}
DECL_HOOKv(MIAccel_LoadCdDir, void* gModelInfoAccelerator, CBaseModelInfo **mi, int* id, char* dffname)
{
    MIAccel_LoadCdDir(gModelInfoAccelerator, mi, id, dffname);
    if(mi && *mi)
    {
        int len = strnlen(dffname, 32);
        if(len > 0) strncpy(modelNames[*id], dffname, 32);
    }
}
void Misc_Patch()
{
    HOOKPLT(InitialiseWhenRestarting, pGTASA + 0x672014);
    HOOKPLT(MIAccel_LoadCdDir, pGTASA + 0x6730D0);
}