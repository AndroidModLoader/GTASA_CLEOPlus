#pragma once

#include <mod/amlmod.h>
#include <mod/logger.h>
#include "GTASA_STRUCTS.h"
#include "cleo.h"

#define CLEO_RegisterOpcode(x, h) cleo->RegisterOpcode(x, h); cleo->RegisterOpcodeFunction(#h, h)
#define CLEO_Fn(h) void h (void *handle, uint32_t *ip, uint16_t opcode, const char *name)

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
inline char* CLEO_ReadStringEx(void* handle, char* buf, size_t size)
{
    uint8_t byte = **(uint8_t**)((int)handle + GetPCOffset());
    if(byte <= 8) return NULL; // Not a string

    static char newBuf[128];
    if(!buf || size < 1) buf = (char*)newBuf;

    switch(byte)
    {
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

// Game vars
extern CCamera* TheCamera;
extern tUsedObject* UsedObjectArray;
extern CBaseModelInfo** ms_modelInfoPtrs;
extern CCarGenerator* CarGeneratorArray;
extern uint16_t *OldWeatherType, *NewWeatherType, *ForcedWeatherType, *m_ItemToFlash;
extern float *Rain, *UnderWaterness, *m_fDNBalanceParam;
extern uint8_t *ms_nGameClockHours, *ms_nGameClockMinutes;
extern RsGlobalType* RsGlobal;
extern uint32_t *gbCineyCamProcessedOnFrame, *m_FrameCounter, *ms_exitEnterState, *m_snTimeInMilliseconds;
extern bool *ms_bTakePhoto, *bDisplayHud;
extern MobileMenu *gMobileMenu;
extern CPickup* aPickUps;
extern CEntity** pIgnoreEntity;

// Game funcs
extern CObject* (*CreateObject)(int);
extern float (*FindGroundZForCoord)(float, float);
extern float (*FindGroundZFor3DCoord)(float, float, float, bool*, CEntity**);
extern void (*ClearSpaceForMissionEntity)(CVector&, CEntity*);
extern void (*AddToWorld)(CEntity*);
extern int (*GetObjectRef)(CObject*);
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

// All of CLEO functions
CLEO_Fn(CREATE_OBJECT_NO_SAVE); // 0xE01=7,create_object_no_save %1o% at %2d% %3d% %4d% offset %5d% ground %6d% to %7d%
CLEO_Fn(SET_CAR_GENERATOR_NO_SAVE); // 0xE02=1,set_car_generator %1d% no_save
CLEO_Fn(GET_CURRENT_WEATHER);
CLEO_Fn(GET_NEXT_WEATHER); // 0xE04=1,get_next_weather_to %1d%
CLEO_Fn(SET_NEXT_WEATHER); // 0xE05=1,set_next_weather_to %1d%
CLEO_Fn(GET_RAIN_INTENSITY); // 0xE06=1,get_rain_intensity %1d%
CLEO_Fn(SET_RAIN_INTENSITY); // 0xE07=1,set_rain_intensity %1d%
CLEO_Fn(GET_CURRENT_HOUR); // 0xE40=1,get_current_hour %1d%
CLEO_Fn(GET_CURRENT_MINUTE); // 0xE41=1,get_current_minute %1d%
CLEO_Fn(GET_DAY_NIGHT_BALANCE); // 0xE6C=1,get_day_night_balance %1d%
CLEO_Fn(GET_UNDERWATERNESS); // 0xE6D=1,get_underwaterness %1d%
CLEO_Fn(GET_FORCED_WEATHER); // 0xEB0=1,get_forced_weather %1d%
CLEO_Fn(IS_CAR_SCRIPT_CONTROLLED); // 0xE08=1,is_car_script_controlled %1d%
CLEO_Fn(MARK_CAR_AS_NEEDED); // 0xE09=1,mark_car_as_needed %1d%
CLEO_Fn(IS_CHAR_SCRIPT_CONTROLLED); // 0xE0A=1,is_char_script_controlled %1d%
CLEO_Fn(MARK_CHAR_AS_NEEDED); // 0xE0B=1,mark_char_as_needed %1d%
CLEO_Fn(IS_OBJECT_SCRIPT_CONTROLLED); // 0xE0C=1,is_object_script_controlled %1d%
CLEO_Fn(MARK_OBJECT_AS_NEEDED); // 0xE0D=1,mark_object_as_needed %1d%
CLEO_Fn(GET_CURRENT_RESOLUTION); // 0xE0E=2,get_current_resolution_to %1d% %2d%
CLEO_Fn(GET_FIXED_XY_ASPECT_RATIO); // 0xE0F=4,get_fixed_xy_aspect_ratio %1d% %2d% to %3d% %4d%
CLEO_Fn(CONVERT_3D_TO_SCREEN_2D); // 0xE3F=9,convert_3d_to_screen_2d %1d% %2d% %3d% checkNearClip %4d% checkFarClip %5d% store_2d_to %6d% %7d% size_to %8d% %9d%
CLEO_Fn(IS_RADAR_VISIBLE); // 0xEB8=0,is_radar_visible
CLEO_Fn(IS_HUD_VISIBLE); // 0xEB9=0,is_hud_visible
CLEO_Fn(GET_FADE_ALPHA); // 0xEC7=1,get_fade_alpha %1d%
CLEO_Fn(IS_MOUSE_WHEEL_UP); // 0xE10=0,is_mouse_wheel_up
CLEO_Fn(IS_MOUSE_WHEEL_DOWN); // 0xE11=0,is_mouse_wheel_down
CLEO_Fn(GET_MOUSE_SENSIBILITY); // 0xE23=1,get_mouse_sensibility_to %1d%
CLEO_Fn(IS_KEY_JUST_PRESSED); // 0xE3D=1,is_key_just_pressed %1d%
CLEO_Fn(IS_BUTTON_JUST_PRESSED); // 0xE3E=2,is_button_just_pressed %1d% button %2d%
CLEO_Fn(SET_CAMERA_CONTROL); // 0xE60=2,set_camera_control %1d% %2d% 
CLEO_Fn(IS_AIM_BUTTON_PRESSED); // 0xE67=1,is_aim_button_pressed %1d%
CLEO_Fn(SET_PLAYER_CONTROL_PAD); // 0xE68=2,set_player_control_pad %1d% %2d% 
CLEO_Fn(SET_PLAYER_CONTROL_PAD_MOVEMENT); // 0xE69=2,set_player_control_pad_movement %1d% %2d% 
CLEO_Fn(IS_ANY_FIRE_BUTTON_PRESSED); // 0xE8D=1,is_any_fire_button_pressed %1d%
CLEO_Fn(IS_SELECT_MENU_JUST_PRESSED); // 0xE6E=0,is_select_menu_just_pressed
CLEO_Fn(RETURN_SCRIPT_EVENT); // 0xED0=0,return_script_event
CLEO_Fn(SET_SCRIPT_EVENT_SAVE_CONFIRMATION); // 0xED1=3,set_script_event_save_confirmation %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_DELETE); // 0xED2=3,set_script_event_char_delete %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_CREATE); // 0xED3=3,set_script_event_char_create %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_CAR_DELETE); // 0xED4=3,set_script_event_car_delete %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_CAR_CREATE); // 0xED5=3,set_script_event_car_create %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_DELETE); // 0xED6=3,set_script_event_object_delete %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_CREATE); // 0xED7=3,set_script_event_object_create %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_ON_MENU); // 0xED8=3,set_script_event_on_menu %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_PROCESS); // 0xEDA=3,set_script_event_char_process %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_CAR_PROCESS); // 0xEDB=3,set_script_event_car_process %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_OBJECT_PROCESS); // 0xEDC=3,set_script_event_object_process %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_BUILDING_PROCESS); // 0xEDD=3,set_script_event_building_process %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_CHAR_DAMAGE); // 0xEDE=3,set_script_event_char_damage %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_CAR_WEAPON_DAMAGE); // 0xEDF=3,set_script_event_car_weapon_damage %1d% label %2p% args %3d%
CLEO_Fn(SET_SCRIPT_EVENT_BULLET_IMPACT); // 0xEE0=6,set_script_event_bullet_impact %1d% label %2p% args %3d% %4d% %5d% %6d%
CLEO_Fn(GET_VEHICLE_SUBCLASS); // 0xE12=2,get_vehicle %1d% subclass_to %2d%
CLEO_Fn(GET_ENTITY_TYPE); // 0xE13=2,get_entity %1d% type_to %2d%
CLEO_Fn(INIT_EXTENDED_CHAR_VARS); // 0xE14=3,init_extended_char_vars %1d% id %2d% new_vars %3d%
CLEO_Fn(SET_EXTENDED_CHAR_VAR); // 0xE15=4,set_extended_char_var %1d% id %2d% var %3d% value %4d%
CLEO_Fn(GET_EXTENDED_CHAR_VAR); // 0xE16=4,get_extended_char_var %1d% id %2d% var %3d% to %4d%
CLEO_Fn(INIT_EXTENDED_CAR_VARS); // 0xE17=3,init_extended_car_vars %1d% id %2d% new_vars %3d%
CLEO_Fn(SET_EXTENDED_CAR_VAR); // 0xE18=4,set_extended_car_var %1d% id %2d% var %3d% value %4d%
CLEO_Fn(GET_EXTENDED_CAR_VAR); // 0xE19=4,get_extended_car_var %1d% id %2d% var %3d% to %4d%
CLEO_Fn(INIT_EXTENDED_OBJECT_VARS); // 0xE1A=3,init_extended_object_vars %1d% id %2d% new_vars %3d%
CLEO_Fn(SET_EXTENDED_OBJECT_VAR); // 0xE1B=4,set_extended_object_var %1d% id %2d% var %3d% value %4d%
CLEO_Fn(GET_EXTENDED_OBJECT_VAR); // 0xE1C=4,get_extended_object_var %1d% id %2d% var %3d% to %4d%
CLEO_Fn(IS_ON_MISSION); // 0xE1D=0,is_on_mission
CLEO_Fn(IS_ON_SAMP); // 0xE20=0,is_on_samp
CLEO_Fn(IS_ON_CUTSCENE); // 0xE25=0,is_on_cutscene
CLEO_Fn(FIX_CHAR_GROUND_BRIGHTNESS_AND_FADE_IN); // 0xE24=4,fix_char %1d% ground %2d% brightness %3d% and_fade_in %4d%
CLEO_Fn(IS_WEAPON_FIRE_TYPE); // 0xE26=2,is_weapon %1d% fire_type %2d%
CLEO_Fn(GET_CURRENT_SAVE_SLOT); // 0xE2C=1,get_current_save_slot %1d%
CLEO_Fn(IS_GAME_FIRST_START); // 0xE2D=0,is_game_first_start
CLEO_Fn(SET_CHAR_COORDINATES_SIMPLE); // 0xE32=4,set_char_coordinates_simple %1d% coord %2d% %3d% %4d%
CLEO_Fn(FRAME_MOD); // 0xE45=1,frame_mod %1d%
CLEO_Fn(RANDOM_PERCENT); // 0xE4D=1,random_percent %1d%
CLEO_Fn(GET_TRAILER_FROM_CAR); // 0xE59=2,get_trailer_from_car %1d% trailer %2d%
CLEO_Fn(GET_CAR_FROM_TRAILER); // 0xE5A=2,get_car_from_trailer %1d% store_to %2d%
CLEO_Fn(GET_CAR_DUMMY_COORD); // 0xE5B=7,get_car_dummy_coord %1d% dummy %2d% world_coords %3d% invert_x %4d% store_to %5d% %6d% %7d% // same as NewOpcodes but this is adapted to VehFuncs
CLEO_Fn(IS_CHEAT_ACTIVE); // 0xE5D=1,is_cheat_active %1d%
CLEO_Fn(CHANGE_PLAYER_MONEY); // 0xE5E=3,change_player_money %1d% mode %2d% value %3d%
CLEO_Fn(CAR_HORN); // 0xE5F=1,car_horn %1d%
CLEO_Fn(GET_STRING_LENGTH); // 0xD4C=2,%2d% = string %1s% length // keep NewOpcodes order
CLEO_Fn(COPY_STRING); // 0xD4D=2,copy_string %1s% to %2d%
CLEO_Fn(GET_CAR_ALARM); // 0xE00=2,get_car_alarm %1d% mode_to %2d%
CLEO_Fn(SET_CAR_ALARM); // 0xE61=2,set_car_alarm %1d% mode %2d%
CLEO_Fn(GET_CHAR_MAX_HEALTH); // 0xD39=2,%2d% = get_char_max_health %1d% // keep NewOpcodes order
CLEO_Fn(GET_CHAR_HEALTH_PERCENT); // 0xE5C=2,get_player_health_percent %1d% store_to %2d%
CLEO_Fn(GET_CURRENT_CAMERA_MODE); // 0xE64=1,get_camera_mode %1d%
CLEO_Fn(GET_CAR_COLLISION_INTENSITY); // 0xE65=2,get_car_collision_intensity %1d% store_to %2d%
CLEO_Fn(GET_CAR_COLLISION_COORDINATES); // 0xE66=4,get_car_collision_coordinates %1d% store_to %2d% %3d% %4d%
CLEO_Fn(STREAM_CUSTOM_SCRIPT_FROM_LABEL); // 0xE6F=-1,stream_custom_script_from_label %1p%
CLEO_Fn(GET_LAST_CREATED_CUSTOM_SCRIPT); // 0xE70=1,get_last_created_custom_script %1d%
CLEO_Fn(GET_OBJECT_CENTRE_OF_MASS_TO_BASE_OF_MODEL); // 0xE71=2,get_object_centre_of_mass_to_base_of_model %1d% %2d%
CLEO_Fn(GET_MODEL_TYPE); // 0xE7F=2,get_model_type %1d% store_to %2d%
CLEO_Fn(IS_STRING_EQUAL); // 0xE80=5,is_string_equal %1s% %2s% max_size %3d% case_sensitive %4d% ignore_charactere %5s%
CLEO_Fn(IS_STRING_COMMENT); // 0xE81=1,is_string_comment %1s%
CLEO_Fn(DOES_CAR_HAVE_PART_NODE); // 0xE82=2,does_car_have_part_node %1d% %2d%
CLEO_Fn(GET_CURRENT_CHAR_WEAPONINFO); // 0xE83=2,get_current_char_weaponinfo %1d% store_to %2d%
CLEO_Fn(GET_WEAPONINFO); // 0xE84=3,get_weaponinfo %1d% skill %2d% store_to %3d%
CLEO_Fn(GET_WEAPONINFO_MODELS); // 0xE85=3,get_weaponinfo_models %1d% store_to %2d% %3d%
CLEO_Fn(GET_WEAPONINFO_FLAGS); // 0xE86=2,get_weaponinfo_flags %1d% store_to %2d%
CLEO_Fn(GET_WEAPONINFO_ANIMGROUP); // 0xE87=2,get_weaponinfo_animgroup %1d% store_to %2d%
CLEO_Fn(GET_WEAPONINFO_TOTAL_CLIP); // 0xE88=2,get_weaponinfo_total_clip %1d% store_to %2d%
CLEO_Fn(GET_WEAPONINFO_FIRE_TYPE); // 0xE89=2,get_weaponinfo_fire_type %1d% store_to %2d%
CLEO_Fn(GET_WEAPONINFO_SLOT); // 0xE8A=2,get_weaponinfo_slot %1d% store_to %2d%
CLEO_Fn(GET_CHAR_WEAPON_STATE); // 0xE8B=2,get_char_weapon_state %1d% store_to %2d%
CLEO_Fn(GET_CHAR_WEAPON_CLIP); // 0xE8C=2,get_char_weapon_clip %1d% store_to %2d%
CLEO_Fn(GET_CHAR_COLLISION_SURFACE); // 0xE8E=2,get_char_collision_surface %1d% store_to %2d%
CLEO_Fn(GET_CHAR_COLLISION_LIGHTING); // 0xE8F=2,get_char_collision_lighting %1d% store_to %2d%
CLEO_Fn(GET_CAR_COLLISION_SURFACE); // 0xE90=2,get_car_collision_surface %1d% store_to %2d%
CLEO_Fn(GET_CAR_COLLISION_LIGHTING); // 0xE91=2,get_car_collision_lighting %1d% store_to %2d%
CLEO_Fn(IS_CHAR_REALLY_IN_AIR); // 0xE92=1,is_char_really_in_air %1d%
CLEO_Fn(IS_CAR_REALLY_IN_AIR); // 0xE93=1,is_car_really_in_air %1d%
CLEO_Fn(IS_OBJECT_REALLY_IN_AIR); // 0xE94=1,is_object_really_in_air %1d%
CLEO_Fn(SIMULATE_OBJECT_DAMAGE); // 0xE95=3,simulate_object_damage %1d% damage %2d% type %3d%
CLEO_Fn(REQUEST_PRIORITY_MODEL); // 0xE98=1,request_priority_model %1d%
CLEO_Fn(LOAD_ALL_PRIORITY_MODELS_NOW); // 0xE99=0,load_all_priority_models_now
CLEO_Fn(LOAD_SPECIAL_CHARACTER_FOR_ID); // 0xE9A=2,load_special_character_for_id %1d% name %2d%
CLEO_Fn(UNLOAD_SPECIAL_CHARACTER_FROM_ID); // 0xE9B=1,unload_special_character_from_id %1d%
CLEO_Fn(GET_MODEL_BY_NAME); //  0E9C=1,get_model_by_name %1d% store_id %2d%
CLEO_Fn(IS_MODEL_AVAILABLE_BY_NAME); // 0xE9D=1,get_model_available_by_name %1d%
CLEO_Fn(GET_MODEL_DOESNT_EXIST_IN_RANGE); // 0xE9E=3,get_model_available_by_name %1d% to %2d% store_to %3d%
CLEO_Fn(REMOVE_ALL_UNUSED_MODELS); // 0xE9F=0,remove_all_unused_models
CLEO_Fn(REMOVE_MODEL_IF_UNUSED); // 0xEA3=1,remove_model_if_unused %1d%
CLEO_Fn(IS_CHAR_ON_FIRE); // 0xEA4=1,is_char_on_fire %1d%
CLEO_Fn(GET_CLOSEST_COP_NEAR_CHAR); // 0xEA5=7,get_closest_cop_near_char %1d% radius %2d% alive %3d% in_car %4d% on_foot %5d% seen_in_front %6d% store_to %7d%
CLEO_Fn(GET_CLOSEST_COP_NEAR_POS); // 0xEA6=8,get_closest_cop_near_char %1d% %2d% %3d% radius %4d% alive %5d% in_car %6d% on_foot %7d% store_to %8d%
CLEO_Fn(GET_ANY_CHAR_NO_SAVE_RECURSIVE); // 0xEA7=3,get_any_char_no_save_recursive %1d% progress_to %2d% char_to %3d%
CLEO_Fn(GET_ANY_CAR_NO_SAVE_RECURSIVE); // 0xEA8=3,get_any_car_no_save_recursive %1d% progress_to %2d% car_to %3d%
CLEO_Fn(GET_ANY_OBJECT_NO_SAVE_RECURSIVE); // 0xEA9=3,get_any_object_no_save_recursive %1d% progress_to %2d% object_to %3d%
CLEO_Fn(SET_CHAR_ARRESTED); // 0xEAA=1,set_char_arrested %1d%
CLEO_Fn(GET_CHAR_PEDSTATE); // 0xEAB=2,get_char_pedstate %1d% store_to %2d%
CLEO_Fn(GET_CHAR_PROOFS); // 0xEAC=6,get_char_proofs %1d% bullet %2d% fire %3d% explosion %4d% collision %5d% melee %6d%
CLEO_Fn(GET_CAR_PROOFS); // 0xEAD=6,get_car_proofs %1d% bullet %2d% fire %3d% explosion %4d% collision %5d% melee %6d%
CLEO_Fn(GET_OBJECT_PROOFS); // 0xEAE=6,get_object_proofs %1d% bullet %2d% fire %3d% explosion %4d% collision %5d% melee %6d%
CLEO_Fn(IS_CHAR_WEAPON_VISIBLE_SET); // 0xEAF=1,is_char_weapon_visible_set %1d%
CLEO_Fn(GET_CHAR_STAT_ID); // 0xEB1=2,get_char_stat_id %1d% store_to %2d%
CLEO_Fn(GET_OFFSET_FROM_CAMERA_IN_WORLD_COORDS); // 0xEB2=6,get_offset_from_camera_in_world_coords %1d% %2d% %3d% store_to %4d% %5d% %6d%
CLEO_Fn(GET_OFFSET_FROM_MATRIX_IN_WORLD_COORDS); // 0xD0A=7,store_coords_to %5d% %6d% %7d% from_matrix %1d% with_offsets %2d% %3d% %4d% // keep NewOpcodes order
CLEO_Fn(SET_CAR_COORDINATES_SIMPLE); // 0xEB4=4,set_car_coordinates_simple %1d% position %2d% %3d% %4d%
CLEO_Fn(GET_CHAR_DAMAGE_LAST_FRAME); // 0xEB5=5,get_char_damage_last_frame %1d% damager %2d% type %3d% part %4d% intensity %5d%
CLEO_Fn(GET_CAR_WEAPON_DAMAGE_LAST_FRAME); // 0xEB6=4,get_car_weapon_damage_last_frame %1d% char %2d% type %3d% intensity %4d%
CLEO_Fn(IS_ON_SCRIPTED_CUTSCENE); // 0xEB7=0,is_on_scripted_cutscene
CLEO_Fn(GET_MODEL_PED_TYPE_AND_STAT); // 0xEBA=3,get_model_ped_type_and_stat %1d% store_to %2d% %3d%
CLEO_Fn(PASS_TIME); // 0xEBB=1,pass_time %1d%
CLEO_Fn(GENERATE_RANDOM_INT_IN_RANGE_WITH_SEED); // 0xEBC=4,generate_random_int_in_range_with_seed %1d% min %2d% max %3d% store_to %4d%
CLEO_Fn(GENERATE_RANDOM_FLOAT_IN_RANGE_WITH_SEED); // 0xEBD=4,generate_random_float_in_range_with_seed %1d% min %2d% max %3d% store_to %4d%
CLEO_Fn(LOCATE_CAMERA_DISTANCE_TO_COORDINATES); // 0xEBE=4,locate_camera_distance_to_coordinates %1d% %2d% %3d% radius %4d%
CLEO_Fn(GET_FX_SYSTEM_POINTER); // 0xEBF=2,get_fx_system_pointer %1d% store_to %2d%
CLEO_Fn(ADD_FX_SYSTEM_PARTICLE); // 0xEC0=14,add_fx_system_particle %1d% coord %2d% %3d% %4d% vel %5d% %6d% %7d% size %8d% brightness %9d% rgba %10d% %11d% %12d% %13d% lastFactor %14d%
CLEO_Fn(IS_FX_SYSTEM_AVAILABLE_WITH_NAME); // 0xEC1=1,is_fx_system_available_with_name %1s%
CLEO_Fn(SET_STRING_UPPER); // 0xEC2=1,set_string_upper %1s%
CLEO_Fn(SET_STRING_LOWER); // 0xEC3=1,set_string_lower %1s%
CLEO_Fn(STRING_FIND); // 0xEC4=4,string_find %1d% %2s% %3s% store_to %4d%
CLEO_Fn(CUT_STRING_AT); // 0xEC5=2,cut_string_at %1d% %2d%
CLEO_Fn(IS_STRING_CHARACTER_AT); // 0xEC6=3,is_string_character_at %1d% character %2d% index %2d%
CLEO_Fn(GET_CHAR_RANDOM_SEED); // 0xEC8=2,get_char_random_seed %1d% store_to %2d%
CLEO_Fn(GET_CAR_RANDOM_SEED); // 0xEC9=2,get_car_random_seed %1d% store_to %2d%
CLEO_Fn(GET_OBJECT_RANDOM_SEED); // 0xECA=2,get_object_random_seed %1d% store_to %2d%
CLEO_Fn(GET_CHAR_MOVE_STATE); // 0xECB=2,get_char_move_state %1d% store_to %2d%
CLEO_Fn(DONT_DELETE_CHAR_UNTIL_TIME); // 0xECC=2,dont_delete_char_until_time %1d% %2d%
CLEO_Fn(DONT_DELETE_CAR_UNTIL_TIME); // 0xECD=2,dont_delete_car_until_time %1d% %2d%
CLEO_Fn(GET_TIME_CHAR_IS_DEAD); // 0xECE=2,get_time_char_is_dead %1d% store_to %2d%
CLEO_Fn(GET_TIME_CAR_IS_DEAD); // 0xECF=2,get_time_car_is_dead %1d% store_to %2d%
CLEO_Fn(SET_CHAR_IGNORE_DAMAGE_ANIMS); // 0xED9=2,set_char_ignore_damage_anims %1d% %2d%
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_CHAR); // 0xEE4=3,locate_char_distance_to_char %1d% char %2d% radius %3d%
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_CAR); // 0xEE5=3,locate_char_distance_to_car %1d% car %2d% radius %3d%
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_OBJECT); // 0xEE6=3,locate_char_distance_to_object %1d% object %2d% radius %3d%
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_OBJECT); // 0xEE7=3,locate_car_distance_to_object %1d% object %2d% radius %3d%
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_CAR); // 0xEE8=3,locate_car_distance_to_car %1d% car %2d% radius %3d%
CLEO_Fn(LOCATE_OBJECT_DISTANCE_TO_OBJECT); // 0xEE9=3,locate_object_distance_to_object %1d% object %2d% radius %3d%
CLEO_Fn(LOCATE_CHAR_DISTANCE_TO_COORDINATES); // 0xEEA=5,locate_char_distance_to_coordinates %1d% pos %2d% %3d% %4d% radius %5d%
CLEO_Fn(LOCATE_CAR_DISTANCE_TO_COORDINATES); // 0xEEB=5,locate_car_distance_to_coordinates %1d% pos %2d% %3d% %4d% radius %5d%
CLEO_Fn(LOCATE_OBJECT_DISTANCE_TO_COORDINATES); // 0xEEC=5,locate_object_distance_to_coordinates %1d% pos %2d% %3d% %4d% radius %5d%
CLEO_Fn(LOCATE_ENTITY_DISTANCE_TO_ENTITY); // 0xEED=5,locate_object_distance_to_coordinates %1d% pos %2d% %3d% %4d% radius %5d%
CLEO_Fn(GET_ENTITY_COORDINATES); // 0xEEE=4,get_entity_coordinates %1d% store_to %2d% %3d% %4d%
CLEO_Fn(GET_ENTITY_HEADING); // 0xEEF=2,get_entity_heading %1d% store_to %2d%
CLEO_Fn(IS_CAR_OWNED_BY_PLAYER); // 0xEF5=1,is_car_owned_by_player %1d%
CLEO_Fn(SET_CAR_OWNED_BY_PLAYER); // 0xEF6=2,set_car_owned_by_player %1d% %2d%
CLEO_Fn(GET_MODEL_INFO); // 0xEF8=2,get_model_info %1d% store_to %2d%
CLEO_Fn(GET_CAR_ANIMGROUP); // 0xEF9=2,get_car_animgroup %1d% store_to %2d%
CLEO_Fn(GET_CHAR_FEAR); // 0xEFA=2,get_char_fear %1d% store_to %2d%
CLEO_Fn(IS_CAR_CONVERTIBLE); // 0xEFB=1,is_car_convertible %1d%
CLEO_Fn(GET_CAR_VALUE); // 0xEFC=2,get_car_value %1d% store_to %2d%
CLEO_Fn(GET_CAR_PEDALS); // 0xEFD=3,get_car_pedals %1d% gas_to %2d% break_to %3d% 
CLEO_Fn(GET_LOADED_LIBRARY); // 0xEFE=2,get_loaded_library %1d% store_to %2d%
CLEO_Fn(SET_CAR_MODEL_ALPHA); // 0xD0F=2,set_car_model_alpha %1d% alpha %2d%
CLEO_Fn(SET_CHAR_MODEL_ALPHA); // 0xD10=2,set_char_model_alpha %1d% alpha %2d%
CLEO_Fn(SET_OBJECT_MODEL_ALPHA); // 0xD11=2,set_object_model_alpha %1d% alpha %2d%
CLEO_Fn(GET_LOCAL_TIME); // 0xD2D=8,get_local_time_year_to %1d% month_to %2d% day_of_week_to %3d% day_to %4d% hour_to %5d% minute_to %6d% second_to %7d% milliseconds_to %8d%
CLEO_Fn(SET_SCRIPT_VAR); // 0xD2E=3,set_script %1d% var %2d% value %3d%
CLEO_Fn(GET_SCRIPT_VAR); // 0xD2F=3,%3d% = get_script %1d% var %2d% // keep NewOpcodes order
CLEO_Fn(SET_CAR_DOOR_WINDOW_STATE); // 0xD33=3,set_car %1d% door %2d% window_state %3d%
CLEO_Fn(DRAW_TEXTURE_PLUS); // 0xE1E=15,draw_texture_plus %1d% event %2d% pos %3d% %4d% size %5d% %6d% angle %7d% depth %8d% fix_aspect_ratio %9d% maskTrisCount %10d% maskTrisArray %11d% rgba %12d% %13d% %14d% %15d% 
CLEO_Fn(GET_TEXTURE_FROM_SPRITE); // 0xE3C=2,get_texture_from_sprite %1d% store_to %2d%
CLEO_Fn(DRAW_STRING); // 0xE62=8,print %1s% event %2d% at %3d% %4d% scale %5d% %6d% fixAR %7d% style %8d%
CLEO_Fn(DRAW_STRING_EXT); // 0xE63=27,print %1s% event %2d% at %3d% %4d% scale %5d% %6d% fixAR %7d% style %8d% prop %9d% align %10d% wrap %11d% justify %12d% color %13d% %14d% %15d% %16d% outline %17d% shadow %18d% dropColor %19d% %20d% %21d% %22d% background %23d% backColor %24d% %25d% %26d% %27d%
CLEO_Fn(QUAT_SLERP); // 0xD1E=4,quat_slerp %1d% to %2d% lambda %3d% result %4d%
CLEO_Fn(SET_MATRIX_ROTATION_FROM_QUAT); // 0xD16=2,set_matrix_rotation_from_quat %1d% rotation_from_quat %2d%
CLEO_Fn(SET_QUAT_FROM_MATRIX); // 0xD17=2,set_quat_from_matrix %1d% to_quat %2d%
CLEO_Fn(ROTATE_QUAT_ON_AXIS); // 0xD18=6,rotate_quat_on_axis %1d% axis_vector %2d% %3d% %4d% angle %5d% combine_op %6d%
CLEO_Fn(GET_NORMALISED_QUAT); // 0xD19=2,get_normalised_quat %1d% to_quat %2d%
CLEO_Fn(MULTIPLY_QUATS); // 0xD1A=3,multiply_quats %3d% = quat %1d% * quat %2d% // keep NewOpcodes order
CLEO_Fn(INITIALISE_QUAT); // 0xD24=5,set_quat %1d% elements %2d% %3d% %4d% %5d%
CLEO_Fn(GET_QUAT_ELEMENTS); // 0xD29=5,get_quat %1d% elements_to %2d% %3d% %4d% %5d%
CLEO_Fn(ROTATE_MATRIX_ON_AXIS); // 0xD01=6,rotate_matrix %1d% on_axis %2d% %3d% %4d% angle %5d% combine_op %6d%
CLEO_Fn(GET_MATRIX_X_ANGLE); // 0xD02=2,%2d% = matrix %1d% x_angle // keep NewOpcodes order
CLEO_Fn(GET_MATRIX_Y_ANGLE); // 0xD03=2,%2d% = matrix %1d% y_angle // keep NewOpcodes order
CLEO_Fn(GET_MATRIX_Z_ANGLE); // 0xD04=2,%2d% = matrix %1d% z_angle // keep NewOpcodes order
CLEO_Fn(EASE); // 0xE1F=4,ease %1d% mode %2d% way %3d% to %4d%
CLEO_Fn(GET_ANGLE_FROM_TWO_COORDS); // 0xE27=5,get_angle_from_two_coords %1d% %2d% and %3d% %4d% to %5d%
CLEO_Fn(PERLIN_NOISE); // 0xE03=2,perlin_noise %1d% store_to %2d%
CLEO_Fn(PERLIN_NOISE_FRACTAL); // 0xE29=7,perlin_noise %1d% octaves %2d% frequency %3d% amplitude %4d% lacunarity %5d% persistence %6d% store_to %7d%
CLEO_Fn(GET_COORD_FROM_ANGLED_DISTANCE); // 0xEF0=6,get_coord_from_angled_distance %1d% %2d% angle %3d% dist %4d% store_to %5d% %6d%
CLEO_Fn(PERLIN_NOISE_FRACTAL_2D); // 0xEF1=8,perlin_noise_fractal_2d x %1d% y %2d% octaves %3d% frequency %4d% amplitude %5d% lacunarity %6d% persistence %7d% store_to %8d%
CLEO_Fn(PERLIN_NOISE_FRACTAL_3D); // 0xEF2=9,perlin_noise_fractal_3d x %1d% y %2d% z %3d% octaves %4d% frequency %5d% amplitude %6d% lacunarity %7d% persistence %8d% store_to %9d%
CLEO_Fn(CLAMP_FLOAT); // 0xEF4=4,clamp_float %1d% min %2d% max %3d% store_to %4d%
CLEO_Fn(CLAMP_INT); // 0xEF7=4,clamp_int %1d% min %2d% max %3d% store_to %4d%
CLEO_Fn(CONVERT_DIRECTION_TO_QUAT); // 0xEB3=4,convert_direction_to_quat %1d% dir %2d% %3d% %4d%
CLEO_Fn(LERP); // 0xEF3=4,lerp %1d% %2d% %3d% store_to %4d%
CLEO_Fn(GET_AUDIO_SFX_VOLUME); // 0xE21=1,get_audio_sfx_volume %1d%
CLEO_Fn(GET_AUDIO_RADIO_VOLUME); // 0xE22=1,get_audio_radio_volume %1d%
CLEO_Fn(GET_AUDIOSTREAM_INTERNAL); // 0xE3B=2,get_audiostream_internal %1d% store_to %2d%
CLEO_Fn(WRITE_STRUCT_OFFSET); // 0xE28=4,write_struct %1d% offset %2d% size %3d% value %4d%
CLEO_Fn(READ_STRUCT_OFFSET); // 0xD4E=4,%4d% = read_struct %1d% offset %2d% size %3d% // keep newOpcodes ordering for SB
CLEO_Fn(WRITE_STRUCT_OFFSET); // // newOpcodes version of 0E28, the original one doesn't work, this is a fall off
CLEO_Fn(COPY_MEMORY); // 0xD27=3,copy_memory %1d% to %2d% size %3d%
CLEO_Fn(WRITE_STRUCT_PARAM); // 0xD37=3,write_struct_param %1d% param %2d% value %3d%
CLEO_Fn(READ_STRUCT_PARAM); // 0xD38=3,%3d% = read_struct_param %1d% param %2d% // keep newOpcodes ordering for SB
CLEO_Fn(MAKE_NOP); // 0xE6A=2,make_nop %1d% size %2d%
CLEO_Fn(READ_STRUCT_OFFSET_MULTI); // 0xEE2=-1,read_struct_offset_multi %1d% offset %2d% total %3d% size %4d%
CLEO_Fn(WRITE_STRUCT_OFFSET_MULTI); // 0xEE3=-1,write_struct_offset_multi %1d% offset %2d% count %3d% size %4d%
CLEO_Fn(ADD_CLEO_BLIP); // 0xE2A=9,add_cleo_blip %1d% position %2d% %3d% is_short %4d% RGBA %5d% %6d% %7d% %8d% store_to %9d%
CLEO_Fn(REMOVE_CLEO_BLIP); // 0xE2B=1,remove_cleo_blip %1d%
CLEO_Fn(CREATE_RENDER_OBJECT_TO_CHAR_BONE); // 0xE2E=10,create_render_object_to_char_bone %1d% model %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d% store_to %10d%
CLEO_Fn(CREATE_RENDER_OBJECT_TO_CHAR_BONE_FROM_SPECIAL); // 0F02=10,create_render_object_to_char_bone_from_special %1d% special_model %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d% scale %10d% %11d% %12d% store_to %13d%
CLEO_Fn(DELETE_RENDER_OBJECT); // 0xE2F=1,delete_render_object %1d%
CLEO_Fn(SET_RENDER_OBJECT_AUTO_HIDE); // 0xE30=4,set_render_object_auto_hide %1d% dead %2d% weapon %3d% car %4d%
CLEO_Fn(SET_RENDER_OBJECT_VISIBLE); // 0xE31=2,set_render_object_visible %1d% %2d%
CLEO_Fn(SET_RENDER_OBJECT_POSITION); // 0xE35=4,set_render_object_position %1d% %2d% %3d% %4d%
CLEO_Fn(SET_RENDER_OBJECT_ROTATION); // 0xE36=4,set_render_object_rotation %1d% %2d% %3d% %4d%
CLEO_Fn(SET_RENDER_OBJECT_SCALE); // 0xE37=4,set_render_object_scale %1d% %2d% %3d% %4d%
CLEO_Fn(SET_RENDER_OBJECT_DISTORTION); // 0xE3A=5,set_render_object_distortion %1d% %2d% %3d% %4d% %5d%
CLEO_Fn(GET_PICKUP_THIS_COORD); // 0xE33=5,get_pickup_this_coord %1d% %2d% %3d% only_available %4d% store_to %5d%
CLEO_Fn(GET_PICKUP_MODEL); // 0xE34=2,get_pickup_model %1d% %2d%
CLEO_Fn(GET_PICKUP_POINTER); // 0xE38=2,get_pickup_pointer %1d% store_to %2d%
CLEO_Fn(GET_PICKUP_TYPE); // 0xE39=2,get_pickup_type %1d% store_to %2d%
CLEO_Fn(DISPLAY_ONSCREEN_TIMER_LOCAL); // 0xE4E=2,display_onscreen_timer_local %1d% direction %2d%
CLEO_Fn(DISPLAY_ONSCREEN_TIMER_WITH_STRING_LOCAL); // 0xE4F=3,display_onscreen_timer_with_string_local %1d% direction %2d% GXT %3d%
CLEO_Fn(DISPLAY_ONSCREEN_COUNTER_LOCAL); // 0xE50=2,display_onscreen_counter_local %1d% direction %2d%
CLEO_Fn(DISPLAY_ONSCREEN_COUNTER_WITH_STRING_LOCAL); // 0xE51=3,display_onscreen_counter_with_string_local %1d% direction %2d% GXT %3d%
CLEO_Fn(DISPLAY_TWO_ONSCREEN_COUNTERS_LOCAL); // 0xE52=2,display_two_onscreen_counters_local %1d% max_value %2d%
CLEO_Fn(DISPLAY_TWO_ONSCREEN_COUNTERS_WITH_STRING_LOCAL); // 0xE53=3,display_two_onscreen_counters_with_string_local %1d% max_value %2d% GXT %3d%
CLEO_Fn(CLEAR_ONSCREEN_TIMER_LOCAL); // 0xE54=1,clear_onscreen_timer_local %1d%
CLEO_Fn(CLEAR_ONSCREEN_COUNTER_LOCAL); // 0xE55=1,clear_onscreen_counter_local %1d%
CLEO_Fn(SET_ONSCREEN_COUNTER_FLASH_WHEN_FIRST_DISPLAYED_LOCAL); // 0xE56=2,set_onscreen_counter_flash_when_first_displayed_local %1d% flash %2d%
CLEO_Fn(SET_TIMER_BEEP_COUNTDOWN_TIME_LOCAL); // 0xE57=2,set_timer_beep_countdown_time_local %1d% secs %2d%
CLEO_Fn(SET_ONSCREEN_COUNTER_COLOUR_LOCAL); // 0xE58=2,set_onscreen_counter_colour_local %1d% color %2d%
CLEO_Fn(IS_CHAR_DOING_TASK_ID); // 0xE42=2,is_char_doing_task_id %1d% %2d%
CLEO_Fn(GET_CHAR_TASK_POINTER_BY_ID); // 0xE43=3,get_char_task_pointer_by_id %1d% %2d% store_to %3d%
CLEO_Fn(GET_CHAR_KILL_TARGET_CHAR); // 0xE44=2,get_char_kill_target_char %1d% store_to %2d%
CLEO_Fn(IS_CHAR_USING_GUN); // 0xE46=1,is_char_using_gun %1d%
CLEO_Fn(IS_CHAR_FIGHTING); // 0xE47=1,is_char_fighting %1d%
CLEO_Fn(IS_CHAR_FALLEN_ON_GROUND); // 0xE48=1,is_char_fallen_on_ground %1d%
CLEO_Fn(IS_CHAR_ENTERING_ANY_CAR); // 0xE49=1,is_char_entering_any_car %1d%
CLEO_Fn(IS_CHAR_EXITING_ANY_CAR); // 0xE4A=1,is_char_exiting_any_car %1d%
CLEO_Fn(IS_CHAR_PLAYING_ANY_SCRIPT_ANIMATION); // 0xE4B=2,is_char_playing_any_script_animation %1d% include_anims %2d%
CLEO_Fn(IS_CHAR_DOING_ANY_IMPORTANT_TASK); // 0xE4C=2,is_char_doing_any_important_task %1d% include_anims %2d%
CLEO_Fn(CLEAR_CHAR_PRIMARY_TASKS); // 0xE96=1,clear_char_primary_tasks %1d%
CLEO_Fn(CLEAR_CHAR_SECONDARY_TASKS); // 0xE97=1,clear_char_secondary_tasks %1d%
CLEO_Fn(GET_CHAR_SIMPLEST_ACTIVE_TASK); // 0xEFF=3,get_char_simplest_active_task %1d% id_to %2d% pointer_to %3d%
CLEO_Fn(GET_COLLISION_BETWEEN_POINTS); // 0xD3A=20,get_collision_between_points %1d% %2d% %3d% and %4d% %5d% %6d% flags %7d% %8d% %9d% %10d% %11d% %12d% %13d% %14d% ignore_entity %15d% store_point_to %17d% %18d% %19d% entity_to %20d% colpoint_data_to %16d% // keep NewOpcodes order
CLEO_Fn(GET_COLPOINT_NORMAL_VECTOR); // 0xD3B=4,get_colpoint_normal_vector %1d% store_to %2d% %3d% %4d%
CLEO_Fn(GET_COLPOINT_SURFACE); // 0xD3C=2,get_colpoint_surface %1d% store_to %2d%
CLEO_Fn(GET_COLPOINT_DEPTH); // 0xD3E=2,get_colpoint_depth %1d% store_to %2d%
CLEO_Fn(GET_COLPOINT_LIGHTING); // 0xE6B=2,get_colpoint_lighting %1d% from_night %2d% store_to %3d%
CLEO_Fn(GET_COLPOINT_COORDINATES); // 0xEE1=4,get_colpoint_coordinates %1d% store_to %2d% %3d% %4d%
CLEO_Fn(CREATE_LIST); // 0xE72=2,create_list %1d% store_to %2d%
CLEO_Fn(DELETE_LIST); // 0xE73=1,delete_list %1d%
CLEO_Fn(LIST_ADD); // 0xE74=2,list_add %1d% value %2d%
CLEO_Fn(LIST_REMOVE_VALUE); // 0xE75=2,list_remove_value %1d% value %2d%
CLEO_Fn(LIST_REMOVE_INDEX); // 0xE76=2,list_remove_index %1d% index %2d%
CLEO_Fn(GET_LIST_SIZE); // 0xE77=2,get_list_size %1d% size %2d%
CLEO_Fn(GET_LIST_VALUE_BY_INDEX); // 0xE78=3,get_list_value_by_index %1d% index %2d% store_to %3d%
CLEO_Fn(RESET_LIST); // 0xE79=1,reset_list %1d%
CLEO_Fn(GET_LIST_STRING_VALUE_BY_INDEX); // 0xE7A=3,get_list_string_value_by_index %1d% index %2d% store_to %3d%
CLEO_Fn(LIST_ADD_STRING); // 0xE7B=2,list_add_string %1d% value %2d%
CLEO_Fn(LIST_REMOVE_STRING_VALUE); // 0xE7C=2,list_remove_string_value %1d% value %2d%
CLEO_Fn(LIST_REMOVE_INDEX_RANGE); // 0xE7D=3,list_remove_index %1d% start %2d% end %3d%
CLEO_Fn(REVERSE_LIST); // 0xE7E=1,reverse_list %1d%