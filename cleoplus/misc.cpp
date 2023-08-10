#include "externs.h"

CLEO_Fn(IS_ON_MISSION)
{
    
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
    
}
CLEO_Fn(IS_GAME_FIRST_START)
{
    
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
    
}
CLEO_Fn(RANDOM_PERCENT)
{
    
}
CLEO_Fn(GET_TRAILER_FROM_CAR)
{
    
}
CLEO_Fn(GET_CAR_FROM_TRAILER)
{
    
}
CLEO_Fn(GET_CAR_DUMMY_COORD)
{
    
}
CLEO_Fn(IS_CHEAT_ACTIVE)
{
    
}
CLEO_Fn(CHANGE_PLAYER_MONEY)
{
    
}
CLEO_Fn(CAR_HORN)
{
    
}
CLEO_Fn(GET_STRING_LENGTH)
{
    
}
CLEO_Fn(COPY_STRING)
{
    
}
CLEO_Fn(GET_CAR_ALARM)
{
    
}
CLEO_Fn(SET_CAR_ALARM)
{
    
}
CLEO_Fn(GET_CHAR_MAX_HEALTH)
{
    
}
CLEO_Fn(GET_CHAR_HEALTH_PERCENT)
{
    
}
CLEO_Fn(GET_CURRENT_CAMERA_MODE)
{
    
}
CLEO_Fn(GET_CAR_COLLISION_INTENSITY)
{
    
}
CLEO_Fn(GET_CAR_COLLISION_COORDINATES)
{
    
}
CLEO_Fn(STREAM_CUSTOM_SCRIPT_FROM_LABEL)
{
    
}
CLEO_Fn(GET_LAST_CREATED_CUSTOM_SCRIPT)
{
    
}
CLEO_Fn(GET_OBJECT_CENTRE_OF_MASS_TO_BASE_OF_MODEL)
{
    
}
CLEO_Fn(GET_MODEL_TYPE)
{
    
}
CLEO_Fn(IS_STRING_EQUAL)
{
    
}
CLEO_Fn(IS_STRING_COMMENT)
{
    
}
CLEO_Fn(DOES_CAR_HAVE_PART_NODE)
{
    
}
CLEO_Fn(GET_CURRENT_CHAR_WEAPONINFO)
{
    
}
CLEO_Fn(GET_WEAPONINFO)
{
    
}
CLEO_Fn(GET_WEAPONINFO_MODELS)
{
    
}
CLEO_Fn(GET_WEAPONINFO_FLAGS)
{
    
}
CLEO_Fn(GET_WEAPONINFO_ANIMGROUP)
{
    
}
CLEO_Fn(GET_WEAPONINFO_TOTAL_CLIP)
{
    
}
CLEO_Fn(GET_WEAPONINFO_FIRE_TYPE)
{
    
}
CLEO_Fn(GET_WEAPONINFO_SLOT)
{
    
}
CLEO_Fn(GET_CHAR_WEAPON_STATE)
{
    
}
CLEO_Fn(GET_CHAR_WEAPON_CLIP)
{
    
}
CLEO_Fn(GET_CHAR_COLLISION_SURFACE)
{
    
}
CLEO_Fn(GET_CHAR_COLLISION_LIGHTING)
{
    
}
CLEO_Fn(GET_CAR_COLLISION_SURFACE)
{
    
}
CLEO_Fn(GET_CAR_COLLISION_LIGHTING)
{
    
}
CLEO_Fn(IS_CHAR_REALLY_IN_AIR)
{
    
}
CLEO_Fn(IS_CAR_REALLY_IN_AIR)
{
    
}
CLEO_Fn(IS_OBJECT_REALLY_IN_AIR)
{
    
}
CLEO_Fn(SIMULATE_OBJECT_DAMAGE)
{
    
}
CLEO_Fn(REQUEST_PRIORITY_MODEL)
{
    
}
CLEO_Fn(LOAD_ALL_PRIORITY_MODELS_NOW)
{
    
}
CLEO_Fn(LOAD_SPECIAL_CHARACTER_FOR_ID)
{
    
}
CLEO_Fn(UNLOAD_SPECIAL_CHARACTER_FROM_ID)
{
    
}
CLEO_Fn(GET_MODEL_BY_NAME)
{
    
}
CLEO_Fn(IS_MODEL_AVAILABLE_BY_NAME)
{
    
}
CLEO_Fn(GET_MODEL_DOESNT_EXIST_IN_RANGE)
{
    
}
CLEO_Fn(REMOVE_ALL_UNUSED_MODELS)
{
    
}
CLEO_Fn(REMOVE_MODEL_IF_UNUSED)
{
    
}
CLEO_Fn(IS_CHAR_ON_FIRE)
{
    
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
    
}
CLEO_Fn(GET_CHAR_PEDSTATE)
{
    
}
CLEO_Fn(GET_CHAR_PROOFS)
{
    
}
CLEO_Fn(GET_CAR_PROOFS)
{
    
}
CLEO_Fn(GET_OBJECT_PROOFS)
{
    
}
CLEO_Fn(IS_CHAR_WEAPON_VISIBLE_SET)
{
    
}
CLEO_Fn(GET_CHAR_STAT_ID)
{
    
}
CLEO_Fn(GET_OFFSET_FROM_CAMERA_IN_WORLD_COORDS)
{
    
}
CLEO_Fn(GET_OFFSET_FROM_MATRIX_IN_WORLD_COORDS)
{
    
}
CLEO_Fn(SET_CAR_COORDINATES_SIMPLE)
{
    
}
CLEO_Fn(GET_CHAR_DAMAGE_LAST_FRAME)
{
    
}
CLEO_Fn(GET_CAR_WEAPON_DAMAGE_LAST_FRAME)
{
    
}
CLEO_Fn(IS_ON_SCRIPTED_CUTSCENE)
{
    
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