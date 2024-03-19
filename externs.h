#pragma once

#include <list>
#include <cctype>
#include <string>
#include <vector>
#include <set>
#define AML32
#include <mod/amlmod.h>
#include <mod/logger.h>
#include "GTASA_STRUCTS.h"
#include "cleo.h"

#define CLEO_RegisterOpcode(x, h) cleo->RegisterOpcode(x, h); cleo->RegisterOpcodeFunction(#h, h)
#define CLEO_Fn(h) void h (void *handle, uint32_t *ip, uint16_t opcode, const char *name)
#define RpClumpGetAnimBlendClumpData(clump) (*(CAnimBlendClumpData**)(((unsigned int)(clump) + ClumpOffset)))

extern uintptr_t pGTASA;
extern void* hGTASA;
extern cleo_ifs_t* cleo;

struct tUsedObject
{
    char szModelName[24];
    int  nModelIndex;
};

void ResolveExternals();

inline int GetPCOffset()
{
    switch(cleo->GetGameIdentifier())
    {
        case GTASA: return 20;
        case GTALCS: return 24;

        default: return 16;
    }
}
inline char* CLEO_ReadStringEx(void* handle, char* buf = NULL, size_t size = 0)
{
    uint8_t& byte = **(uint8_t**)((int)handle + GetPCOffset());
    if(byte <= 8) return NULL; // Not a string

    static char newBuf[128];
    if(!buf || size < 1) buf = (char*)newBuf;

    switch(byte)
    {
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
            return (char*)cleo->ReadParam(handle)->i;

        case 0x9:
            //cleo->ReadParam(handle); // Need to collect results before that
            byte += 1; // the same here
            return cleo->ReadString8byte(handle, buf, size) ? buf : NULL;

        case 0xA:
        case 0xB:
        case 0x10:
        case 0x11:
        {
            size = (size > 16) ? 16 : size;
            memcpy(buf, (char*)cleo->GetPointerToScriptVar(handle), size);
            buf[size-1] = 0;
            return buf;
        }

        default:
        {
            return cleo->ReadStringLong(handle, buf, size) ? buf : NULL;
        }
    }
    return buf;
}
inline void CLEO_WriteStringEx(void* handle, const char* buf)
{
    if(**(uint8_t**)((int)handle + GetPCOffset()) > 8)
    {
        char* dst = (char*)cleo->GetPointerToScriptVar(handle);
        memcpy(dst, buf, 15); dst[15] = 0;
    }
    else
    {
        char* dst = (char*)cleo->ReadParam(handle)->i;
        strcpy(dst, buf);
    }
}
inline void toupper(char *s)
{
    char *p = s;
    while(*p)
    {
        *p = toupper(*p);
        ++p;
    }
}

extern std::set<int> SpecialCharacterModelsUsed;
extern int g_nCurrentSaveSlot;

enum ScriptEventList
{
    SaveConfirmation, CharDelete, CharCreate, CarDelete, CarCreate, ObjectDelete, ObjectCreate, OnMenu, CharProcess, CarProcess, ObjectProcess,
    BuildingProcess, CharDamage, CarWeaponDamage, BulletImpact, BeforeGameProcess, AfterGameProcess, TOTAL_SCRIPT_EVENTS
};
class ScriptEvent
{
public:
    CRunningScript *script;
    int eventScriptIP;
    unsigned int varPointer[4];

public:
    void RunScriptEvent(int arg1 = 0, int arg2 = 0, int arg3 = 0, int arg4 = 0);
    static void AddEvent(void* handle, std::vector<ScriptEvent*> &scriptEventList, unsigned int args = 1);
    static void ClearForScriptLabelAndVar(std::vector<ScriptEvent*> &scriptEvents, CRunningScript* script, int label, unsigned int varPointer[4]);
    static void ClearAllForScript(CRunningScript *script);
    static void ClearAllScriptEvents();
};
extern std::vector<ScriptEvent*> scriptEvents[TOTAL_SCRIPT_EVENTS];


// Render Objects
class RenderObject;


// Extended Vars
class ExtendedVars
{
public:
    uint32_t id;
    void *data;
    ExtendedVars(ExtendedVars *) : id(0), data(NULL) {}
    ExtendedVars(uint32_t newId, uint32_t totalVars)
    {
        id = newId;
        data = malloc(totalVars * 4);
        memset(data, 0, (totalVars * 4));
    }
};
class PedExtVars
{
public:
    int activeTasks[32];
    CEntity *killTargetPed;
    CEntity *lastDamageEntity;
    int lastDamageWeapon;
    int lastDamagePart;
    float lastDamageIntensity;
    bool ignoreDamageAnims;

    union
    {
        unsigned int aiFlagsIntValue;
        struct
        {
            unsigned char bKillingSomething : 1;
            unsigned char bUsingGun : 1;
            unsigned char bFighting : 1;
            unsigned char bFallenOnGround : 1;
            unsigned char bEnteringAnyCar : 1;
            unsigned char bExitingAnyCar : 1;
            unsigned char bPlayingAnyPrimaryScriptAnimation : 1;
            unsigned char bPlayingAnySecondaryScriptAnimation : 1;
            unsigned char bRootTaskIsntImportant : 1;
        } aiFlags;
    };

    std::vector<RenderObject*> renderObjects;
    std::list<ExtendedVars*> extendedVarsList;
};
class VehicleExtVars
{
public:
    std::list<ExtendedVars*> extendedVarsList;
};
class ObjectExtVars
{
public:
    std::list<ExtendedVars*> extendedVarsList;
};
extern CPool<PedExtVars> *ms_pPedExtVarsPool;
extern CPool<VehicleExtVars> *ms_pVehicleExtVarsPool;
extern CPool<ObjectExtVars> *ms_pObjectExtVarsPool;


// Game vars
extern CCamera* TheCamera;
extern tUsedObject* UsedObjectArray;
extern CBaseModelInfo** ms_modelInfoPtrs;
extern CCarGenerator* CarGeneratorArray;
extern uint16_t *OldWeatherType, *NewWeatherType, *ForcedWeatherType, *m_ItemToFlash;
extern float *Rain, *UnderWaterness, *m_fDNBalanceParam;
extern uint8_t *ms_nGameClockHours, *ms_nGameClockMinutes, *ScriptSpace;
extern RsGlobalType* RsGlobal;
extern uint32_t *gbCineyCamProcessedOnFrame, *m_FrameCounter, *ms_exitEnterState, *m_snTimeInMilliseconds, *ZonesRevealed;
extern bool *ms_bTakePhoto, *bDisplayHud, *ms_running, *m_aCheatsActive;
extern MobileMenu *gMobileMenu;
extern CPickup* aPickUps;
extern CEntity** pIgnoreEntity;
extern CSprite2d* RadarBlipSprites;
extern CSprite2d* ScriptSprites;
extern int32_t *OnAMissionFlag;
extern CPool<CPed, CCopPed> **ms_pPedPool;
extern CPool<CVehicle, CHeli> **ms_pVehiclePool;
extern CPool<CObject, CCutsceneObject> **ms_pObjectPool;
extern CScriptResourceManager *ScriptResourceManager;
extern CColModel *ms_colModelPed1;
extern CDirectory **ms_pExtraObjectsDir;
extern CStreamingInfo *ms_aInfoForModel;
extern script_effect_struct *ScriptEffectSystemArray;
extern FxManager_c *g_fxMan;
extern bool *m_UserPause ,*m_CodePause;
extern int *ClumpOffset;
extern CMouseControllerState *NewMouseControllerState;
extern float *m_fMouseAccelHorzntl;

// Game funcs
extern CObject* (*CreateObject)(int);
extern float (*FindGroundZForCoord)(float, float);
extern float (*FindGroundZFor3DCoord)(float, float, float, bool*, CEntity**);
extern void (*ClearSpaceForMissionEntity)(CVector&, CEntity*);
extern void (*AddToWorld)(CEntity*);
extern int (*GetObjectRef)(CObject*);
extern int (*GetVehicleRef)(CVehicle*);
extern CVehicle* (*GetVehicleFromRef)(int);
extern CPed* (*GetPedFromRef)(int);
extern CObject* (*GetObjectFromRef)(int);
extern void (*UpdateCompareFlag)(CRunningScript*, uint8_t);
extern bool (*CalcScreenCoors)(RwV3d const&,RwV3d*,float *,float *,bool,bool);
extern int (*GetActualPickupIndex)(int);
extern bool (*ProcessLineOfSight)(CVector const&,CVector const&,CColPoint &,CEntity *&,bool,bool,bool,bool,bool,bool,bool,bool);
extern void (*QuaternionSet)(CQuaternion*, const RwMatrix& m);
extern void (*QuaternionMult)(const CQuaternion *a1, const CQuaternion *a2, const CQuaternion *a3);
extern void (*RtQuatRotate)(CQuaternion *, CVector const*, float, RwOpCombineType);
extern void (*RwMatrixRotate)(RwMatrix *, RwV3d const*, float, RwOpCombineType);
extern bool (*GetCurrentZoneLockedOrUnlocked)(float, float);
extern void (*TransformRealWorldPointToRadarSpace)(CVector2D &, CVector2D const&);
extern float (*LimitRadarPoint)(CVector2D &);
extern void (*TransformRadarPointToScreenSpace)(CVector2D &, CVector2D const&);
extern bool (*DisplayThisBlip)(int, char);
extern void (*AddBlipToLegendList)(uint8_t, int);
extern void (*DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);
extern int (*GetUppercaseKey)(const char*);
extern CWeaponInfo* (*GetWeaponInfo)(int, char);
extern void (*SetClumpAlpha)(RpClump*, int);
extern float (*GetDistanceFromCentreOfMassToBaseOfModel)(CEntity*);
extern bool (*ProcessVerticalLine)(CVector const&,float,CColPoint &,CEntity *&,bool,bool,bool,bool,bool,bool,CStoredCollPoly *);
extern CPlayerPed* (*FindPlayerPed)(int);
extern CPlayerInfo* (*GetPlayerInfoForThisPlayerPed)(CPlayerPed*);
extern char (*GetWeaponSkill)(CPed*, eWeaponType);
extern void (*ObjectDamage)(CObject *,float,CVector *,CVector *,CEntity *,eWeaponType);
extern void (*AddToResourceManager)(CScriptResourceManager *,int,uint,CRunningScript *);
extern bool (*RemoveFromResourceManager)(CScriptResourceManager *,int,uint,CRunningScript *);
extern void (*RequestModel)(int,int);
extern void (*LoadAllRequestedModels)(bool);
extern void (*TimerSuspend)();
extern void (*TimerResume)();
extern CPedModelInfo* (*AddPedModel)(int id);
extern void (*SetColModel)(CBaseModelInfo *, CColModel *, bool);
extern void (*RequestSpecialModel)(int,char const*,int);
extern void (*SetMissionDoesntRequireModel)(int);
extern void (*RemoveModel)(int);
extern CBaseModelInfo* (*GetModelInfoByName)(const char *, int *);
extern CDirectory::DirectoryInfo* (*FindItem)(CDirectory*,char const*,uint &,uint &);
extern bool (*IsObjectInCdImage)(int);
extern void (*RemoveAllUnusedModels)();
extern void (*RwV3dTransformPoint)(CVector&,CVector&,CMatrix&);
extern void (*TransformPoint)(RwV3d& point, const CSimpleTransform& placement, const RwV3d& vecPos);
extern void (*PassTime)(unsigned int);
extern void (*SetRwObjectAlpha)(CEntity*, int);
extern void (*SetWindowOpenFlag)(CVehicle*, uint8_t);
extern void (*ClearWindowOpenFlag)(CVehicle*, uint8_t);
extern int (*GetActualScriptThingIndex)(int, uint8_t);
extern void (*AddParticle)(FxSystem_c *, RwV3d *,RwV3d *,float,FxPrtMult_c *,float,float,float,uint8_t);
extern FxSystemBP_c* (*FindFxSystemBP)(FxManager_c *, const char *);
extern void (*ProcessScript)(CRunningScript*);
extern void (*UpdateRpHAnim)(CEntity*);
extern RpHAnimHierarchy* (*GetAnimHierarchyFromSkinClump)(RpClump*);
extern int (*RpHAnimIDGetIndex)(RpHAnimHierarchy*, int);
extern void (*Find3rdPersonCamTargetVector)(CCamera*, float, CVector, CVector&, CVector&);
extern CPad* (*GetPad)(int);
extern bool (*PadGetTarget)(CPad*, bool);
extern void (*CreateMatFromVec)(void* unused, CMatrix*, CVector*, CVector*);

// All of CLEO functions
CLEO_Fn(CREATE_OBJECT_NO_SAVE);
CLEO_Fn(SET_CAR_GENERATOR_NO_SAVE);
CLEO_Fn(GET_CURRENT_WEATHER);
CLEO_Fn(GET_NEXT_WEATHER);
CLEO_Fn(SET_NEXT_WEATHER);
CLEO_Fn(GET_RAIN_INTENSITY);
CLEO_Fn(SET_RAIN_INTENSITY);
CLEO_Fn(GET_CURRENT_HOUR);
CLEO_Fn(GET_CURRENT_MINUTE);
CLEO_Fn(GET_DAY_NIGHT_BALANCE);
CLEO_Fn(GET_UNDERWATERNESS);
CLEO_Fn(GET_FORCED_WEATHER);
CLEO_Fn(IS_CAR_SCRIPT_CONTROLLED);
CLEO_Fn(MARK_CAR_AS_NEEDED);
CLEO_Fn(IS_CHAR_SCRIPT_CONTROLLED);
CLEO_Fn(MARK_CHAR_AS_NEEDED);
CLEO_Fn(IS_OBJECT_SCRIPT_CONTROLLED);
CLEO_Fn(MARK_OBJECT_AS_NEEDED);
CLEO_Fn(GET_CURRENT_RESOLUTION);
CLEO_Fn(GET_FIXED_XY_ASPECT_RATIO);
CLEO_Fn(CONVERT_3D_TO_SCREEN_2D);
CLEO_Fn(IS_RADAR_VISIBLE);
CLEO_Fn(IS_HUD_VISIBLE);
CLEO_Fn(GET_FADE_ALPHA);
CLEO_Fn(IS_MOUSE_WHEEL_UP);
CLEO_Fn(IS_MOUSE_WHEEL_DOWN);
CLEO_Fn(GET_MOUSE_SENSIBILITY);
CLEO_Fn(IS_KEY_JUST_PRESSED);
CLEO_Fn(IS_BUTTON_JUST_PRESSED);
CLEO_Fn(SET_CAMERA_CONTROL);
CLEO_Fn(IS_AIM_BUTTON_PRESSED);
CLEO_Fn(SET_PLAYER_CONTROL_PAD);
CLEO_Fn(SET_PLAYER_CONTROL_PAD_MOVEMENT);
CLEO_Fn(IS_ANY_FIRE_BUTTON_PRESSED);
CLEO_Fn(IS_SELECT_MENU_JUST_PRESSED);
CLEO_Fn(GET_TIME_NOT_TOUCHING_PAD);
CLEO_Fn(RETURN_SCRIPT_EVENT);
CLEO_Fn(SET_SCRIPT_EVENT_SAVE_CONFIRMATION);
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_DELETE);
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_CREATE);
CLEO_Fn(SET_SCRIPT_EVENT_CAR_DELETE);
CLEO_Fn(SET_SCRIPT_EVENT_CAR_CREATE);
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_DELETE);
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_CREATE);
CLEO_Fn(SET_SCRIPT_EVENT_ON_MENU);
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_PROCESS);
CLEO_Fn(SET_SCRIPT_EVENT_CAR_PROCESS);
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_PROCESS);
CLEO_Fn(SET_SCRIPT_EVENT_BUILDING_PROCESS);
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_DAMAGE);
CLEO_Fn(SET_SCRIPT_EVENT_CAR_WEAPON_DAMAGE);
CLEO_Fn(SET_SCRIPT_EVENT_BULLET_IMPACT);
CLEO_Fn(SET_SCRIPT_EVENT_BEFORE_GAME_PROCESS);
CLEO_Fn(SET_SCRIPT_EVENT_AFTER_GAME_PROCESS);
CLEO_Fn(GET_VEHICLE_SUBCLASS);
CLEO_Fn(GET_ENTITY_TYPE);
CLEO_Fn(INIT_EXTENDED_CHAR_VARS);
CLEO_Fn(SET_EXTENDED_CHAR_VAR);
CLEO_Fn(GET_EXTENDED_CHAR_VAR);
CLEO_Fn(INIT_EXTENDED_CAR_VARS);
CLEO_Fn(SET_EXTENDED_CAR_VAR);
CLEO_Fn(GET_EXTENDED_CAR_VAR);
CLEO_Fn(INIT_EXTENDED_OBJECT_VARS);
CLEO_Fn(SET_EXTENDED_OBJECT_VAR);
CLEO_Fn(GET_EXTENDED_OBJECT_VAR);
CLEO_Fn(IS_ON_MISSION);
CLEO_Fn(IS_ON_SAMP);
CLEO_Fn(IS_ON_CUTSCENE);
CLEO_Fn(FIX_CHAR_GROUND_BRIGHTNESS_AND_FADE_IN);
CLEO_Fn(IS_WEAPON_FIRE_TYPE);
CLEO_Fn(GET_CURRENT_SAVE_SLOT);
CLEO_Fn(IS_GAME_FIRST_START);
CLEO_Fn(SET_CHAR_COORDINATES_SIMPLE);
CLEO_Fn(FRAME_MOD);
CLEO_Fn(RANDOM_PERCENT);
CLEO_Fn(GET_TRAILER_FROM_CAR);
CLEO_Fn(GET_CAR_FROM_TRAILER);
CLEO_Fn(GET_CAR_DUMMY_COORD);
CLEO_Fn(IS_CHEAT_ACTIVE);
CLEO_Fn(CHANGE_PLAYER_MONEY);
CLEO_Fn(CAR_HORN);
CLEO_Fn(GET_STRING_LENGTH);
CLEO_Fn(COPY_STRING);
CLEO_Fn(GET_CAR_ALARM);
CLEO_Fn(SET_CAR_ALARM);
CLEO_Fn(GET_CHAR_MAX_HEALTH);
CLEO_Fn(GET_CHAR_HEALTH_PERCENT);
CLEO_Fn(GET_CURRENT_CAMERA_MODE);
CLEO_Fn(GET_CAR_COLLISION_INTENSITY);
CLEO_Fn(GET_CAR_COLLISION_COORDINATES);
CLEO_Fn(STREAM_CUSTOM_SCRIPT_FROM_LABEL);
CLEO_Fn(GET_LAST_CREATED_CUSTOM_SCRIPT);
CLEO_Fn(GET_OBJECT_CENTRE_OF_MASS_TO_BASE_OF_MODEL);
CLEO_Fn(GET_MODEL_TYPE);
CLEO_Fn(IS_STRING_EQUAL);
CLEO_Fn(IS_STRING_COMMENT);
CLEO_Fn(DOES_CAR_HAVE_PART_NODE);
CLEO_Fn(GET_CURRENT_CHAR_WEAPONINFO);
CLEO_Fn(GET_WEAPONINFO);
CLEO_Fn(GET_WEAPONINFO_MODELS);
CLEO_Fn(GET_WEAPONINFO_FLAGS);
CLEO_Fn(GET_WEAPONINFO_ANIMGROUP);
CLEO_Fn(GET_WEAPONINFO_TOTAL_CLIP);
CLEO_Fn(GET_WEAPONINFO_FIRE_TYPE);
CLEO_Fn(GET_WEAPONINFO_SLOT);
CLEO_Fn(GET_CHAR_WEAPON_STATE);
CLEO_Fn(GET_CHAR_WEAPON_CLIP);
CLEO_Fn(GET_CHAR_COLLISION_SURFACE);
CLEO_Fn(GET_CHAR_COLLISION_LIGHTING);
CLEO_Fn(GET_CAR_COLLISION_SURFACE);
CLEO_Fn(GET_CAR_COLLISION_LIGHTING);
CLEO_Fn(IS_CHAR_REALLY_IN_AIR);
CLEO_Fn(IS_CAR_REALLY_IN_AIR);
CLEO_Fn(IS_OBJECT_REALLY_IN_AIR);
CLEO_Fn(SIMULATE_OBJECT_DAMAGE);
CLEO_Fn(REQUEST_PRIORITY_MODEL);
CLEO_Fn(LOAD_ALL_PRIORITY_MODELS_NOW);
CLEO_Fn(LOAD_SPECIAL_CHARACTER_FOR_ID);
CLEO_Fn(UNLOAD_SPECIAL_CHARACTER_FROM_ID);
CLEO_Fn(GET_MODEL_BY_NAME);
CLEO_Fn(IS_MODEL_AVAILABLE_BY_NAME);
CLEO_Fn(GET_MODEL_DOESNT_EXIST_IN_RANGE);
CLEO_Fn(REMOVE_ALL_UNUSED_MODELS);
CLEO_Fn(REMOVE_MODEL_IF_UNUSED);
CLEO_Fn(IS_CHAR_ON_FIRE);
CLEO_Fn(GET_CLOSEST_COP_NEAR_CHAR);
CLEO_Fn(GET_CLOSEST_COP_NEAR_POS);
CLEO_Fn(GET_ANY_CHAR_NO_SAVE_RECURSIVE);
CLEO_Fn(GET_ANY_CAR_NO_SAVE_RECURSIVE);
CLEO_Fn(GET_ANY_OBJECT_NO_SAVE_RECURSIVE);
CLEO_Fn(SET_CHAR_ARRESTED);
CLEO_Fn(GET_CHAR_PEDSTATE);
CLEO_Fn(GET_CHAR_PROOFS);
CLEO_Fn(GET_CAR_PROOFS);
CLEO_Fn(GET_OBJECT_PROOFS);
CLEO_Fn(IS_CHAR_WEAPON_VISIBLE_SET);
CLEO_Fn(GET_CHAR_STAT_ID);
CLEO_Fn(GET_OFFSET_FROM_CAMERA_IN_WORLD_COORDS);
CLEO_Fn(GET_OFFSET_FROM_MATRIX_IN_WORLD_COORDS);
CLEO_Fn(SET_CAR_COORDINATES_SIMPLE);
CLEO_Fn(GET_CHAR_DAMAGE_LAST_FRAME);
CLEO_Fn(GET_CAR_WEAPON_DAMAGE_LAST_FRAME);
CLEO_Fn(IS_ON_SCRIPTED_CUTSCENE);
CLEO_Fn(GET_MODEL_PED_TYPE_AND_STAT);
CLEO_Fn(PASS_TIME);
CLEO_Fn(GENERATE_RANDOM_INT_IN_RANGE_WITH_SEED);
CLEO_Fn(GENERATE_RANDOM_FLOAT_IN_RANGE_WITH_SEED);
CLEO_Fn(LOCATE_CAMERA_DISTANCE_TO_COORDINATES);
CLEO_Fn(GET_FX_SYSTEM_POINTER);
CLEO_Fn(ADD_FX_SYSTEM_PARTICLE);
CLEO_Fn(IS_FX_SYSTEM_AVAILABLE_WITH_NAME);
CLEO_Fn(SET_STRING_UPPER);
CLEO_Fn(SET_STRING_LOWER);
CLEO_Fn(STRING_FIND);
CLEO_Fn(CUT_STRING_AT);
CLEO_Fn(IS_STRING_CHARACTER_AT);
CLEO_Fn(GET_CHAR_RANDOM_SEED);
CLEO_Fn(GET_CAR_RANDOM_SEED);
CLEO_Fn(GET_OBJECT_RANDOM_SEED);
CLEO_Fn(GET_CHAR_MOVE_STATE);
CLEO_Fn(DONT_DELETE_CHAR_UNTIL_TIME);
CLEO_Fn(DONT_DELETE_CAR_UNTIL_TIME);
CLEO_Fn(GET_TIME_CHAR_IS_DEAD);
CLEO_Fn(GET_TIME_CAR_IS_DEAD);
CLEO_Fn(SET_CHAR_IGNORE_DAMAGE_ANIMS);
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_CHAR);
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_CAR);
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_OBJECT);
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_OBJECT);
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_CAR);
CLEO_Fn(LOCATE_OBJECT_DISTANCE_TO_OBJECT);
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_COORDINATES);
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_COORDINATES);
CLEO_Fn(LOCATE_OBJECT_DISTANCE_TO_COORDINATES);
CLEO_Fn(LOCATE_ENTITY_DISTANCE_TO_ENTITY);
CLEO_Fn(GET_ENTITY_COORDINATES);
CLEO_Fn(GET_ENTITY_HEADING);
CLEO_Fn(IS_CAR_OWNED_BY_PLAYER);
CLEO_Fn(SET_CAR_OWNED_BY_PLAYER);
CLEO_Fn(GET_MODEL_INFO);
CLEO_Fn(GET_CAR_ANIMGROUP);
CLEO_Fn(GET_CHAR_FEAR);
CLEO_Fn(IS_CAR_CONVERTIBLE);
CLEO_Fn(GET_CAR_VALUE);
CLEO_Fn(GET_CAR_PEDALS);
CLEO_Fn(GET_LOADED_LIBRARY);
CLEO_Fn(RETURN_TIMES);
CLEO_Fn(GET_THIRD_PERSON_CAMERA_TARGET);
CLEO_Fn(GET_DISTANCE_MULTIPLIER);
CLEO_Fn(GET_ACTIVE_CAMERA_ROTATION);
CLEO_Fn(GET_CLOSEST_WATER_DISTANCE);
CLEO_Fn(GET_CAMERA_STRUCT);
CLEO_Fn(GET_CAMERA_ROTATION_INPUT_VALUES);
CLEO_Fn(SET_CAMERA_ROTATION_INPUT_VALUES);
CLEO_Fn(SET_ON_MISSION);
CLEO_Fn(GET_MODEL_NAME_POINTER);
CLEO_Fn(SET_CAR_MODEL_ALPHA);
CLEO_Fn(SET_CHAR_MODEL_ALPHA);
CLEO_Fn(SET_OBJECT_MODEL_ALPHA);
CLEO_Fn(GET_LOCAL_TIME);
CLEO_Fn(SET_SCRIPT_VAR);
CLEO_Fn(GET_SCRIPT_VAR);
CLEO_Fn(SET_CAR_DOOR_WINDOW_STATE);
CLEO_Fn(DRAW_TEXTURE_PLUS);
CLEO_Fn(GET_TEXTURE_FROM_SPRITE);
CLEO_Fn(DRAW_STRING);
CLEO_Fn(DRAW_STRING_EXT);
CLEO_Fn(QUAT_SLERP);
CLEO_Fn(SET_MATRIX_ROTATION_FROM_QUAT);
CLEO_Fn(SET_QUAT_FROM_MATRIX);
CLEO_Fn(ROTATE_QUAT_ON_AXIS);
CLEO_Fn(GET_NORMALISED_QUAT);
CLEO_Fn(MULTIPLY_QUATS);
CLEO_Fn(INITIALISE_QUAT);
CLEO_Fn(GET_QUAT_ELEMENTS);
CLEO_Fn(ROTATE_MATRIX_ON_AXIS);
CLEO_Fn(GET_MATRIX_X_ANGLE);
CLEO_Fn(GET_MATRIX_Y_ANGLE);
CLEO_Fn(GET_MATRIX_Z_ANGLE);
CLEO_Fn(EASE);
CLEO_Fn(GET_ANGLE_FROM_TWO_COORDS);
CLEO_Fn(PERLIN_NOISE);
CLEO_Fn(PERLIN_NOISE_FRACTAL);
CLEO_Fn(GET_COORD_FROM_ANGLED_DISTANCE);
CLEO_Fn(PERLIN_NOISE_FRACTAL_2D);
CLEO_Fn(PERLIN_NOISE_FRACTAL_3D);
CLEO_Fn(CLAMP_FLOAT);
CLEO_Fn(CLAMP_INT);
CLEO_Fn(CONVERT_DIRECTION_TO_QUAT);
CLEO_Fn(LERP);
CLEO_Fn(SET_MATRIX_LOOK_DIRECTION);
CLEO_Fn(GET_AUDIO_SFX_VOLUME);
CLEO_Fn(GET_AUDIO_RADIO_VOLUME);
CLEO_Fn(GET_AUDIOSTREAM_INTERNAL);
CLEO_Fn(WRITE_STRUCT_OFFSET);
CLEO_Fn(READ_STRUCT_OFFSET);
CLEO_Fn(WRITE_STRUCT_OFFSET);
CLEO_Fn(COPY_MEMORY);
CLEO_Fn(WRITE_STRUCT_PARAM);
CLEO_Fn(READ_STRUCT_PARAM);
CLEO_Fn(MAKE_NOP);
CLEO_Fn(READ_STRUCT_OFFSET_MULTI);
CLEO_Fn(WRITE_STRUCT_OFFSET_MULTI);
CLEO_Fn(ADD_CLEO_BLIP);
CLEO_Fn(REMOVE_CLEO_BLIP);
CLEO_Fn(CREATE_RENDER_OBJECT_TO_CHAR_BONE);
CLEO_Fn(CREATE_RENDER_OBJECT_TO_CHAR_BONE_FROM_SPECIAL);
CLEO_Fn(DELETE_RENDER_OBJECT);
CLEO_Fn(SET_RENDER_OBJECT_AUTO_HIDE);
CLEO_Fn(SET_RENDER_OBJECT_VISIBLE);
CLEO_Fn(SET_RENDER_OBJECT_POSITION);
CLEO_Fn(SET_RENDER_OBJECT_ROTATION);
CLEO_Fn(SET_RENDER_OBJECT_SCALE);
CLEO_Fn(SET_RENDER_OBJECT_DISTORTION);
CLEO_Fn(GET_PICKUP_THIS_COORD);
CLEO_Fn(GET_PICKUP_MODEL);
CLEO_Fn(GET_PICKUP_POINTER);
CLEO_Fn(GET_PICKUP_TYPE);
CLEO_Fn(DISPLAY_ONSCREEN_TIMER_LOCAL);
CLEO_Fn(DISPLAY_ONSCREEN_TIMER_WITH_STRING_LOCAL);
CLEO_Fn(DISPLAY_ONSCREEN_COUNTER_LOCAL);
CLEO_Fn(DISPLAY_ONSCREEN_COUNTER_WITH_STRING_LOCAL);
CLEO_Fn(DISPLAY_TWO_ONSCREEN_COUNTERS_LOCAL);
CLEO_Fn(DISPLAY_TWO_ONSCREEN_COUNTERS_WITH_STRING_LOCAL);
CLEO_Fn(CLEAR_ONSCREEN_TIMER_LOCAL);
CLEO_Fn(CLEAR_ONSCREEN_COUNTER_LOCAL);
CLEO_Fn(SET_ONSCREEN_COUNTER_FLASH_WHEN_FIRST_DISPLAYED_LOCAL);
CLEO_Fn(SET_TIMER_BEEP_COUNTDOWN_TIME_LOCAL);
CLEO_Fn(SET_ONSCREEN_COUNTER_COLOUR_LOCAL);
CLEO_Fn(IS_CHAR_DOING_TASK_ID);
CLEO_Fn(GET_CHAR_TASK_POINTER_BY_ID);
CLEO_Fn(GET_CHAR_KILL_TARGET_CHAR);
CLEO_Fn(IS_CHAR_USING_GUN);
CLEO_Fn(IS_CHAR_FIGHTING);
CLEO_Fn(IS_CHAR_FALLEN_ON_GROUND);
CLEO_Fn(IS_CHAR_ENTERING_ANY_CAR);
CLEO_Fn(IS_CHAR_EXITING_ANY_CAR);
CLEO_Fn(IS_CHAR_PLAYING_ANY_SCRIPT_ANIMATION);
CLEO_Fn(IS_CHAR_DOING_ANY_IMPORTANT_TASK);
CLEO_Fn(CLEAR_CHAR_PRIMARY_TASKS);
CLEO_Fn(CLEAR_CHAR_SECONDARY_TASKS);
CLEO_Fn(GET_CHAR_SIMPLEST_ACTIVE_TASK);
CLEO_Fn(GET_COLLISION_BETWEEN_POINTS);
CLEO_Fn(GET_COLPOINT_NORMAL_VECTOR);
CLEO_Fn(GET_COLPOINT_SURFACE);
CLEO_Fn(GET_COLPOINT_DEPTH);
CLEO_Fn(GET_COLPOINT_LIGHTING);
CLEO_Fn(GET_COLPOINT_COORDINATES);
CLEO_Fn(CREATE_LIST);
CLEO_Fn(DELETE_LIST);
CLEO_Fn(LIST_ADD);
CLEO_Fn(LIST_REMOVE_VALUE);
CLEO_Fn(LIST_REMOVE_INDEX);
CLEO_Fn(GET_LIST_SIZE);
CLEO_Fn(GET_LIST_VALUE_BY_INDEX);
CLEO_Fn(RESET_LIST);
CLEO_Fn(GET_LIST_STRING_VALUE_BY_INDEX);
CLEO_Fn(LIST_ADD_STRING);
CLEO_Fn(LIST_REMOVE_STRING_VALUE);
CLEO_Fn(LIST_REMOVE_INDEX_RANGE);
CLEO_Fn(REVERSE_LIST);
CLEO_Fn(REPLACE_LIST_VALUE_BY_INDEX);
CLEO_Fn(REPLACE_LIST_STRING_VALUE_BY_INDEX);
CLEO_Fn(INSERT_LIST_VALUE_BY_INDEX);
CLEO_Fn(INSERT_LIST_STRING_VALUE_BY_INDEX);
CLEO_Fn(GET_CHAR_BONE_MATRIX);
CLEO_Fn(GET_CHAR_BONE);
CLEO_Fn(GET_BONE_OFFSET_VECTOR);
CLEO_Fn(GET_BONE_QUAT);
CLEO_Fn(LOAD_SPECIAL_MODEL);
CLEO_Fn(REMOVE_SPECIAL_MODEL);
CLEO_Fn(GET_SPECIAL_MODEL_DATA);

void RadarBlip_Patch();
void Misc_Patch();
void Events_Patch();

extern bool g_bForceInterrupt;
extern bool g_bDisableCamControl;
extern bool g_bDisablePadControl[3];
extern bool g_bDisablePadControlMovement[3];


inline PedExtVars* GetExtData(CPed* ped)
{
    if(!ms_pPedExtVarsPool)
    {
        if(!*ms_pPedPool) return NULL;
        auto size = (*ms_pPedPool)->m_nSize;
        ms_pPedExtVarsPool = new CPool<PedExtVars>(size, "PedExtVars");
        ms_pPedExtVarsPool->m_nFirstFree = size;
        for(int i = 0; i < size; ++i) ms_pPedExtVarsPool->m_byteMap[i].bEmpty = false;
    }
    return ms_pPedExtVarsPool->GetAt((*ms_pPedPool)->GetIndex(ped));
}
inline VehicleExtVars* GetExtData(CVehicle* ped)
{
    if(!ms_pVehicleExtVarsPool)
    {
        if(!*ms_pVehiclePool) return NULL;
        auto size = (*ms_pVehiclePool)->m_nSize;
        ms_pVehicleExtVarsPool = new CPool<VehicleExtVars>(size, "VehicleExtVars");
        ms_pVehicleExtVarsPool->m_nFirstFree = size;
        for(int i = 0; i < size; ++i) ms_pVehicleExtVarsPool->m_byteMap[i].bEmpty = false;
    }
    return ms_pVehicleExtVarsPool->GetAt((*ms_pVehiclePool)->GetIndex(ped));
}
inline ObjectExtVars* GetExtData(CObject* ped)
{
    if(!ms_pObjectExtVarsPool)
    {
        if(!*ms_pObjectPool) return NULL;
        auto size = (*ms_pObjectPool)->m_nSize;
        ms_pObjectExtVarsPool = new CPool<ObjectExtVars>(size, "ObjectExtVars");
        ms_pObjectExtVarsPool->m_nFirstFree = size;
        for(int i = 0; i < size; ++i) ms_pObjectExtVarsPool->m_byteMap[i].bEmpty = false;
    }
    return ms_pObjectExtVarsPool->GetAt((*ms_pObjectPool)->GetIndex(ped));
}
inline ExtendedVars *FindExtendedVarsFromId(std::list<ExtendedVars*> extendedVarsList, uint32_t findId)
{
    for (ExtendedVars* extendedVars : extendedVarsList)
    {
        if (extendedVars->id == findId) return extendedVars;
    }
    return NULL;
}