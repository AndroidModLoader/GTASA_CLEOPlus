#include <mod/amlmod.h>
#include <mod/logger.h>
#include "externs.h"

MYMOD(net.juniordjjr.rusjj.cleoplus, CLEOPlus, 1.0.1, JuniorDjjr & RusJJ)
NEEDGAME(com.rockstargames.gtasa)

// Savings
uintptr_t pGTASA;
void* hGTASA;
cleo_ifs_t* cleo = nullptr;
cleo_addon_ifs_t* cleoaddon = nullptr;

// Some opcodes changes compared to PC version:
// CHANGE_PLAYER_MONEY has 2 additional modes:
// - 3 = dont take money less than 0
// - 4 = dont take money if they are negative

// Hooks
bool pausedLastFrame = true;

DECL_HOOKv(OnGameProcess)
{
    if(!*m_UserPause && !*m_CodePause) pausedLastFrame = false;
    for (auto scriptEvent : scriptEvents[ScriptEventList::BeforeGameProcess]) scriptEvent->RunScriptEvent();
    OnGameProcess();
    for (auto scriptEvent : scriptEvents[ScriptEventList::AfterGameProcess]) scriptEvent->RunScriptEvent();
}
DECL_HOOKv(DoGameSpecificStuffBeforeSave)
{
    g_nCurrentSaveSlot = gMobileMenu->m_nGenericGameStorageSlot;
    for (auto scriptEvent : scriptEvents[ScriptEventList::SaveConfirmation]) scriptEvent->RunScriptEvent(g_nCurrentSaveSlot + 1);
    DoGameSpecificStuffBeforeSave();
}
DECL_HOOKv(MobileMenuRender, void* self)
{
    MobileMenuRender(self);
    for (auto scriptEvent : scriptEvents[ScriptEventList::OnMenu]) scriptEvent->RunScriptEvent(!pausedLastFrame);
    if (!pausedLastFrame) pausedLastFrame = true;
}
DECL_HOOKv(ComputePedDamageResponse, void* self, CPed* ped, uintptr_t response, bool bSpeak)
{
    if(!*(bool*)(response + 10))
    {
        int pedref = (*ms_pPedPool)->GetRef(ped);
        for (auto scriptEvent : scriptEvents[ScriptEventList::CharDamage]) scriptEvent->RunScriptEvent(pedref);
    }
    ComputePedDamageResponse(self, ped, response, bSpeak);
}

// int main!
extern "C" void OnModLoad()
{
    logger->SetTag("CLEOPlus");
    pGTASA =    aml->GetLib("libGTASA.so");
    hGTASA =    aml->GetLibHandle("libGTASA.so");
    cleo =      (cleo_ifs_t*)GetInterface("CLEO");
    cleoaddon = (cleo_addon_ifs_t*)GetInterface("CLEOAddon");

    ResolveExternals();

    // Hooks
    HOOKPLT(OnGameProcess,                 pGTASA + 0x66FE58);
    HOOKPLT(DoGameSpecificStuffBeforeSave, pGTASA + 0x66EC5C);
    HOOKPLT(MobileMenuRender,              pGTASA + 0x674254);
    HOOKPLT(ComputePedDamageResponse,      pGTASA + 0x66F0EC);

    // NoSave
    CLEO_RegisterOpcode(0x0E01, CREATE_OBJECT_NO_SAVE); // 0E01=7,create_object_no_save %1o% at %2d% %3d% %4d% offset %5d% ground %6d% to %7d%
    CLEO_RegisterOpcode(0x0E02, SET_CAR_GENERATOR_NO_SAVE); // 0E02=1,set_car_generator_no_save %1d%

    // Environment
    CLEO_RegisterOpcode(0x0D59, GET_CURRENT_WEATHER);
    CLEO_RegisterOpcode(0x0E04, GET_NEXT_WEATHER); // 0E04=1,get_next_weather_to %1d%
    CLEO_RegisterOpcode(0x0E05, SET_NEXT_WEATHER); // 0E05=1,set_next_weather_to %1d%
    CLEO_RegisterOpcode(0x0E06, GET_RAIN_INTENSITY); // 0E06=1,get_rain_intensity %1d%
    CLEO_RegisterOpcode(0x0E07, SET_RAIN_INTENSITY); // 0E07=1,set_rain_intensity %1d%
    CLEO_RegisterOpcode(0x0E40, GET_CURRENT_HOUR); // 0E40=1,get_current_hour %1d%
    CLEO_RegisterOpcode(0x0E41, GET_CURRENT_MINUTE); // 0E41=1,get_current_minute %1d%
    CLEO_RegisterOpcode(0x0E6C, GET_DAY_NIGHT_BALANCE); // 0E6C=1,get_day_night_balance %1d%
    CLEO_RegisterOpcode(0x0E6D, GET_UNDERWATERNESS); // 0E6D=1,get_underwaterness %1d%
    CLEO_RegisterOpcode(0x0EB0, GET_FORCED_WEATHER); // 0EB0=1,get_forced_weather %1d%

    // ScriptEntities
    CLEO_RegisterOpcode(0x0E08, IS_CAR_SCRIPT_CONTROLLED); // 0E08=1,is_car_script_controlled %1d%
    CLEO_RegisterOpcode(0x0E09, MARK_CAR_AS_NEEDED); // 0E09=1,mark_car_as_needed %1d%
    CLEO_RegisterOpcode(0x0E0A, IS_CHAR_SCRIPT_CONTROLLED); // 0E0A=1,is_char_script_controlled %1d%
    CLEO_RegisterOpcode(0x0E0B, MARK_CHAR_AS_NEEDED); // 0E0B=1,mark_char_as_needed %1d%
    CLEO_RegisterOpcode(0x0E0C, IS_OBJECT_SCRIPT_CONTROLLED); // 0E0C=1,is_object_script_controlled %1d%
    CLEO_RegisterOpcode(0x0E0D, MARK_OBJECT_AS_NEEDED); // 0E0D=1,mark_object_as_needed %1d%

    // Screen
    CLEO_RegisterOpcode(0x0E0E, GET_CURRENT_RESOLUTION); // 0E0E=2,get_current_resolution_to %1d% %2d%
    CLEO_RegisterOpcode(0x0E0F, GET_FIXED_XY_ASPECT_RATIO); // 0E0F=4,get_fixed_xy_aspect_ratio %1d% %2d% to %3d% %4d%
    CLEO_RegisterOpcode(0x0E3F, CONVERT_3D_TO_SCREEN_2D); // 0E3F=9,convert_3d_to_screen_2d %1d% %2d% %3d% checkNearClip %4d% checkFarClip %5d% store_2d_to %6d% %7d% size_to %8d% %9d%
    CLEO_RegisterOpcode(0x0EB8, IS_RADAR_VISIBLE); // 0EB8=0,is_radar_visible
    CLEO_RegisterOpcode(0x0EB9, IS_HUD_VISIBLE); // 0EB9=0,is_hud_visible
    CLEO_RegisterOpcode(0x0EC7, GET_FADE_ALPHA); // 0EC7=1,get_fade_alpha %1d%

    // Input
    CLEO_RegisterOpcode(0x0E10, IS_MOUSE_WHEEL_UP); // 0E10=0,is_mouse_wheel_up
    CLEO_RegisterOpcode(0x0E11, IS_MOUSE_WHEEL_DOWN); // 0E11=0,is_mouse_wheel_down 
    CLEO_RegisterOpcode(0x0E23, GET_MOUSE_SENSIBILITY); // 0E23=1,get_mouse_sensibility_to %1d%
    CLEO_RegisterOpcode(0x0E3D, IS_KEY_JUST_PRESSED); // 0E3D=1,is_key_just_pressed %1d%
    CLEO_RegisterOpcode(0x0E3E, IS_BUTTON_JUST_PRESSED); // 0E3E=2,is_button_just_pressed %1d% button %2d%
    CLEO_RegisterOpcode(0x0E60, SET_CAMERA_CONTROL); // 0E60=1,set_camera_control %1d%
    CLEO_RegisterOpcode(0x0E67, IS_AIM_BUTTON_PRESSED); // 0E67=1,is_aim_button_pressed %1d%
    CLEO_RegisterOpcode(0x0E68, SET_PLAYER_CONTROL_PAD); // 0E68=2,set_player_control_pad %1d% %2d% 
    CLEO_RegisterOpcode(0x0E69, SET_PLAYER_CONTROL_PAD_MOVEMENT); // 0E69=2,set_player_control_pad_movement %1d% %2d% 
    CLEO_RegisterOpcode(0x0E8D, IS_ANY_FIRE_BUTTON_PRESSED); // 0E8D=1,is_any_fire_button_pressed %1d%
    CLEO_RegisterOpcode(0x0E6E, IS_SELECT_MENU_JUST_PRESSED); // 0E6E=0,is_select_menu_just_pressed
    CLEO_RegisterOpcode(0x0F13, GET_TIME_NOT_TOUCHING_PAD); // 0F13=2,get_time_not_touching_pad %1d% store_to %2d%

    // Events
    Events_Patch();
    CLEO_RegisterOpcode(0x0ED0, RETURN_SCRIPT_EVENT); // 0ED0=0,return_script_event
    CLEO_RegisterOpcode(0x0ED1, SET_SCRIPT_EVENT_SAVE_CONFIRMATION); // 0ED1=3,set_script_event_save_confirmation %1d% label %2p% var_slot %3d%
    CLEO_RegisterOpcode(0x0ED2, SET_SCRIPT_EVENT_CHAR_DELETE); // 0ED2=3,set_script_event_char_delete %1d% label %2p% var_char %3d%
    CLEO_RegisterOpcode(0x0ED3, SET_SCRIPT_EVENT_CHAR_CREATE); // 0ED3=3,set_script_event_char_create %1d% label %2p% var_char %3d%
    CLEO_RegisterOpcode(0x0ED4, SET_SCRIPT_EVENT_CAR_DELETE); // 0ED4=3,set_script_event_car_delete %1d% label %2p% var_car %3d%
    CLEO_RegisterOpcode(0x0ED5, SET_SCRIPT_EVENT_CAR_CREATE); // 0ED5=3,set_script_event_car_create %1d% label %2p% var_car %3d%
    CLEO_RegisterOpcode(0x0ED6, SET_SCRIPT_EVENT_OBJECT_DELETE); // 0ED6=3,set_script_event_object_delete %1d% label %2p% var_object %3d%
    CLEO_RegisterOpcode(0x0ED7, SET_SCRIPT_EVENT_OBJECT_CREATE); // 0ED7=3,set_script_event_object_create %1d% label %2p% var_object %3d%
    CLEO_RegisterOpcode(0x0ED8, SET_SCRIPT_EVENT_ON_MENU); // 0ED8=3,set_script_event_on_menu %1d% label %2p% var_just_paused %3d%
    CLEO_RegisterOpcode(0x0EDA, SET_SCRIPT_EVENT_CHAR_PROCESS); // 0EDA=3,set_script_event_char_process %1d% label %2p% var_char %3d%
    CLEO_RegisterOpcode(0x0EDB, SET_SCRIPT_EVENT_CAR_PROCESS); // 0EDB=3,set_script_event_car_process %1d% label %2p% var_car %3d%
    CLEO_RegisterOpcode(0x0EDC, SET_SCRIPT_EVENT_OBJECT_PROCESS); // 0EDC=3,set_script_event_object_process %1d% label %2p% var_object %3d%
    CLEO_RegisterOpcode(0x0EDD, SET_SCRIPT_EVENT_BUILDING_PROCESS); // 0EDD=3,set_script_event_building_process %1d% label %2p% var_building %3d%
    CLEO_RegisterOpcode(0x0EDE, SET_SCRIPT_EVENT_CHAR_DAMAGE); // 0EDE=3,set_script_event_char_damage %1d% label %2p% var_char %3d%
    CLEO_RegisterOpcode(0x0EDF, SET_SCRIPT_EVENT_CAR_WEAPON_DAMAGE); // 0EDF=3,set_script_event_car_weapon_damage %1d% label %2p% var_car %3d%
    CLEO_RegisterOpcode(0x0EE0, SET_SCRIPT_EVENT_BULLET_IMPACT); // 0EE0=6,set_script_event_bullet_impact %1d% label %2p% var_owner %3d% var_victim %4d% var_weapon %5d% var_colpoint %6d%
    CLEO_RegisterOpcode(0x0F0B, SET_SCRIPT_EVENT_BEFORE_GAME_PROCESS); // 0F0B=2,set_script_event_before_game_process %1d% label %2p%
    CLEO_RegisterOpcode(0x0F0C, SET_SCRIPT_EVENT_AFTER_GAME_PROCESS); // 0F0C=2,set_script_event_after_game_process %1d% label %2p%

    // Types
    CLEO_RegisterOpcode(0x0E12, GET_VEHICLE_SUBCLASS); // 0E12=2,get_vehicle %1d% subclass_to %2d%
    CLEO_RegisterOpcode(0x0E13, GET_ENTITY_TYPE); // 0E13=2,get_entity %1d% type_to %2d%

    // Extended
    //CHAR
    CLEO_RegisterOpcode(0x0E14, INIT_EXTENDED_CHAR_VARS); // 0E14=3,init_extended_char_vars %1d% id %2d% new_vars %3d%
    CLEO_RegisterOpcode(0x0E15, SET_EXTENDED_CHAR_VAR); // 0E15=4,set_extended_char_var %1d% id %2d% var %3d% value %4d%
    CLEO_RegisterOpcode(0x0E16, GET_EXTENDED_CHAR_VAR); // 0E16=4,get_extended_char_var %1d% id %2d% var %3d% to %4d%

    //CAR
    CLEO_RegisterOpcode(0x0E17, INIT_EXTENDED_CAR_VARS); // 0E17=3,init_extended_car_vars %1d% id %2d% new_vars %3d%
    CLEO_RegisterOpcode(0x0E18, SET_EXTENDED_CAR_VAR); // 0E18=4,set_extended_car_var %1d% id %2d% var %3d% value %4d%
    CLEO_RegisterOpcode(0x0E19, GET_EXTENDED_CAR_VAR); // 0E19=4,get_extended_car_var %1d% id %2d% var %3d% to %4d%

    //OBJECT
    CLEO_RegisterOpcode(0x0E1A, INIT_EXTENDED_OBJECT_VARS); // 0E1A=3,init_extended_object_vars %1d% id %2d% new_vars %3d%
    CLEO_RegisterOpcode(0x0E1B, SET_EXTENDED_OBJECT_VAR); // 0E1B=4,set_extended_object_var %1d% id %2d% var %3d% value %4d%
    CLEO_RegisterOpcode(0x0E1C, GET_EXTENDED_OBJECT_VAR); // 0E1C=4,get_extended_object_var %1d% id %2d% var %3d% to %4d%

    // Misc
    Misc_Patch();
    CLEO_RegisterOpcode(0x0E1D, IS_ON_MISSION); // 0E1D=0,is_on_mission
    CLEO_RegisterOpcode(0x0E20, IS_ON_SAMP); // 0E20=0,is_on_samp
    CLEO_RegisterOpcode(0x0E25, IS_ON_CUTSCENE); // 0E25=0,is_on_cutscene
    CLEO_RegisterOpcode(0x0E24, FIX_CHAR_GROUND_BRIGHTNESS_AND_FADE_IN); // 0E24=4,fix_char %1d% ground %2d% brightness %3d% and_fade_in %4d%
    CLEO_RegisterOpcode(0x0E26, IS_WEAPON_FIRE_TYPE); // 0E26=2,is_weapon %1d% fire_type %2d%
    CLEO_RegisterOpcode(0x0E2C, GET_CURRENT_SAVE_SLOT); // 0E2C=1,get_current_save_slot %1d%
    CLEO_RegisterOpcode(0x0E2D, IS_GAME_FIRST_START); // 0E2D=0,is_game_first_start
    CLEO_RegisterOpcode(0x0E32, SET_CHAR_COORDINATES_SIMPLE); // 0E32=4,set_char_coordinates_simple %1d% coord %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0E45, FRAME_MOD); // 0E45=1,frame_mod %1d%
    CLEO_RegisterOpcode(0x0E4D, RANDOM_PERCENT); // 0E4D=1,random_percent %1d%
    CLEO_RegisterOpcode(0x0E59, GET_TRAILER_FROM_CAR); // 0E59=2,get_trailer_from_car %1d% trailer %2d%
    CLEO_RegisterOpcode(0x0E5A, GET_CAR_FROM_TRAILER); // 0E5A=2,get_car_from_trailer %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E5B, GET_CAR_DUMMY_COORD); // 0E5B=7,get_car_dummy_coord %1d% dummy %2d% world_coords %3d% invert_x %4d% store_to %5d% %6d% %7d% // same as NewOpcodes but this is adapted to VehFuncs
    CLEO_RegisterOpcode(0x0E5D, IS_CHEAT_ACTIVE); // 0E5D=1,is_cheat_active %1d%
    CLEO_RegisterOpcode(0x0E5E, CHANGE_PLAYER_MONEY); // 0E5E=3,change_player_money %1d% mode %2d% value %3d%
    CLEO_RegisterOpcode(0x0E5F, CAR_HORN); // 0E5F=1,car_horn %1d%
    CLEO_RegisterOpcode(0x0D4C, GET_STRING_LENGTH); // 0D4C=2,%2d% = string %1s% length // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0D4D, COPY_STRING); // 0D4D=2,copy_string %1s% to %2d%
    CLEO_RegisterOpcode(0x0E00, GET_CAR_ALARM); // 0E00=2,get_car_alarm %1d% mode_to %2d%
    CLEO_RegisterOpcode(0x0E61, SET_CAR_ALARM); // 0E61=2,set_car_alarm %1d% mode %2d%
    CLEO_RegisterOpcode(0x0D39, GET_CHAR_MAX_HEALTH); // 0D39=2,%2d% = get_char_max_health %1d% // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0E5C, GET_CHAR_HEALTH_PERCENT); // 0E5C=2,get_player_health_percent %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E64, GET_CURRENT_CAMERA_MODE); // 0E64=1,get_camera_mode %1d%
    CLEO_RegisterOpcode(0x0E65, GET_CAR_COLLISION_INTENSITY); // 0E65=2,get_car_collision_intensity %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E66, GET_CAR_COLLISION_COORDINATES); // 0E66=4,get_car_collision_coordinates %1d% store_to %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0E6F, STREAM_CUSTOM_SCRIPT_FROM_LABEL); // 0E6F=-1,stream_custom_script_from_label %1p%
    CLEO_RegisterOpcode(0x0E70, GET_LAST_CREATED_CUSTOM_SCRIPT); // 0E70=1,get_last_created_custom_script %1d%
    CLEO_RegisterOpcode(0x0E71, GET_OBJECT_CENTRE_OF_MASS_TO_BASE_OF_MODEL); // 0E71=2,get_object_centre_of_mass_to_base_of_model %1d% %2d%
    CLEO_RegisterOpcode(0x0E7F, GET_MODEL_TYPE); // 0E7F=2,get_model_type %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E80, IS_STRING_EQUAL); // 0E80=5,is_string_equal %1s% %2s% max_size %3d% case_sensitive %4d% ignore_charactere %5s%
    CLEO_RegisterOpcode(0x0E81, IS_STRING_COMMENT); // 0E81=1,is_string_comment %1s%
    CLEO_RegisterOpcode(0x0E82, DOES_CAR_HAVE_PART_NODE); // 0E82=2,does_car_have_part_node %1d% %2d%
    CLEO_RegisterOpcode(0x0E83, GET_CURRENT_CHAR_WEAPONINFO); // 0E83=2,get_current_char_weaponinfo %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E84, GET_WEAPONINFO); // 0E84=3,get_weaponinfo %1d% skill %2d% store_to %3d%
    CLEO_RegisterOpcode(0x0E85, GET_WEAPONINFO_MODELS); // 0E85=3,get_weaponinfo_models %1d% store_to %2d% %3d%
    CLEO_RegisterOpcode(0x0E86, GET_WEAPONINFO_FLAGS); // 0E86=2,get_weaponinfo_flags %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E87, GET_WEAPONINFO_ANIMGROUP); // 0E87=2,get_weaponinfo_animgroup %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E88, GET_WEAPONINFO_TOTAL_CLIP); // 0E88=2,get_weaponinfo_total_clip %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E89, GET_WEAPONINFO_FIRE_TYPE); // 0E89=2,get_weaponinfo_fire_type %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E8A, GET_WEAPONINFO_SLOT); // 0E8A=2,get_weaponinfo_slot %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E8B, GET_CHAR_WEAPON_STATE); // 0E8B=2,get_char_weapon_state %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E8C, GET_CHAR_WEAPON_CLIP); // 0E8C=2,get_char_weapon_clip %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E8E, GET_CHAR_COLLISION_SURFACE); // 0E8E=2,get_char_collision_surface %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E8F, GET_CHAR_COLLISION_LIGHTING); // 0E8F=2,get_char_collision_lighting %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E90, GET_CAR_COLLISION_SURFACE); // 0E90=2,get_car_collision_surface %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E91, GET_CAR_COLLISION_LIGHTING); // 0E91=2,get_car_collision_lighting %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E92, IS_CHAR_REALLY_IN_AIR); // 0E92=1,is_char_really_in_air %1d%
    CLEO_RegisterOpcode(0x0E93, IS_CAR_REALLY_IN_AIR); // 0E93=1,is_car_really_in_air %1d%
    CLEO_RegisterOpcode(0x0E94, IS_OBJECT_REALLY_IN_AIR); // 0E94=1,is_object_really_in_air %1d%
    CLEO_RegisterOpcode(0x0E95, SIMULATE_OBJECT_DAMAGE); // 0E95=3,simulate_object_damage %1d% damage %2d% type %3d%
    CLEO_RegisterOpcode(0x0E98, REQUEST_PRIORITY_MODEL); // 0E98=1,request_priority_model %1d%
    CLEO_RegisterOpcode(0x0E99, LOAD_ALL_PRIORITY_MODELS_NOW); // 0E99=0,load_all_priority_models_now
    CLEO_RegisterOpcode(0x0E9A, LOAD_SPECIAL_CHARACTER_FOR_ID); // 0E9A=2,load_special_character_for_id %1d% name %2d%
    CLEO_RegisterOpcode(0x0E9B, UNLOAD_SPECIAL_CHARACTER_FROM_ID); // 0E9B=1,unload_special_character_from_id %1d%
    CLEO_RegisterOpcode(0x0E9C, GET_MODEL_BY_NAME); //  0E9C=2,get_model_by_name %1d% store_id %2d%
    CLEO_RegisterOpcode(0x0E9D, IS_MODEL_AVAILABLE_BY_NAME); // 0E9D=1,is_model_available_by_name %1d%
    CLEO_RegisterOpcode(0x0E9E, GET_MODEL_DOESNT_EXIST_IN_RANGE); // 0E9E=3,get_model_doesnt_exist_in_range %1d% to %2d% store_to %3d%
    CLEO_RegisterOpcode(0x0E9F, REMOVE_ALL_UNUSED_MODELS); // 0E9F=0,remove_all_unused_models
    CLEO_RegisterOpcode(0x0EA3, REMOVE_MODEL_IF_UNUSED); // 0EA3=1,remove_model_if_unused %1d%
    CLEO_RegisterOpcode(0x0EA4, IS_CHAR_ON_FIRE); // 0EA4=1,is_char_on_fire %1d%
    CLEO_RegisterOpcode(0x0EA5, GET_CLOSEST_COP_NEAR_CHAR); // 0EA5=7,get_closest_cop_near_char %1d% radius %2d% alive %3d% in_car %4d% on_foot %5d% seen_in_front %6d% store_to %7d%
    CLEO_RegisterOpcode(0x0EA6, GET_CLOSEST_COP_NEAR_POS); // 0EA6=8,get_closest_cop_near_char %1d% %2d% %3d% radius %4d% alive %5d% in_car %6d% on_foot %7d% store_to %8d%
    CLEO_RegisterOpcode(0x0EA7, GET_ANY_CHAR_NO_SAVE_RECURSIVE); // 0EA7=3,get_any_char_no_save_recursive %1d% progress_to %2d% char_to %3d%
    CLEO_RegisterOpcode(0x0EA8, GET_ANY_CAR_NO_SAVE_RECURSIVE); // 0EA8=3,get_any_car_no_save_recursive %1d% progress_to %2d% car_to %3d%
    CLEO_RegisterOpcode(0x0EA9, GET_ANY_OBJECT_NO_SAVE_RECURSIVE); // 0EA9=3,get_any_object_no_save_recursive %1d% progress_to %2d% object_to %3d%
    CLEO_RegisterOpcode(0x0EAA, SET_CHAR_ARRESTED); // 0EAA=1,set_char_arrested %1d%
    CLEO_RegisterOpcode(0x0EAB, GET_CHAR_PEDSTATE); // 0EAB=2,get_char_pedstate %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EAC, GET_CHAR_PROOFS); // 0EAC=6,get_char_proofs %1d% bullet %2d% fire %3d% explosion %4d% collision %5d% melee %6d%
    CLEO_RegisterOpcode(0x0EAD, GET_CAR_PROOFS); // 0EAD=6,get_car_proofs %1d% bullet %2d% fire %3d% explosion %4d% collision %5d% melee %6d%
    CLEO_RegisterOpcode(0x0EAE, GET_OBJECT_PROOFS); // 0EAE=6,get_object_proofs %1d% bullet %2d% fire %3d% explosion %4d% collision %5d% melee %6d%
    CLEO_RegisterOpcode(0x0EAF, IS_CHAR_WEAPON_VISIBLE_SET); // 0EAF=1,is_char_weapon_visible_set %1d%
    CLEO_RegisterOpcode(0x0EB1, GET_CHAR_STAT_ID); // 0EB1=2,get_char_stat_id %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EB2, GET_OFFSET_FROM_CAMERA_IN_WORLD_COORDS); // 0EB2=6,get_offset_from_camera_in_world_coords %1d% %2d% %3d% store_to %4d% %5d% %6d%
    CLEO_RegisterOpcode(0x0D0A, GET_OFFSET_FROM_MATRIX_IN_WORLD_COORDS); // 0D0A=7,store_coords_to %5d% %6d% %7d% from_matrix %1d% with_offsets %2d% %3d% %4d% // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0EB4, SET_CAR_COORDINATES_SIMPLE); // 0EB4=4,set_car_coordinates_simple %1d% position %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0EB5, GET_CHAR_DAMAGE_LAST_FRAME); // 0EB5=5,get_char_damage_last_frame %1d% damager %2d% type %3d% part %4d% intensity %5d%
    CLEO_RegisterOpcode(0x0EB6, GET_CAR_WEAPON_DAMAGE_LAST_FRAME); // 0EB6=4,get_car_weapon_damage_last_frame %1d% char %2d% type %3d% intensity %4d%
    CLEO_RegisterOpcode(0x0EB7, IS_ON_SCRIPTED_CUTSCENE); // 0EB7=0,is_on_scripted_cutscene
    CLEO_RegisterOpcode(0x0EBA, GET_MODEL_PED_TYPE_AND_STAT); // 0EBA=3,get_model_ped_type_and_stat %1d% store_to %2d% %3d%
    CLEO_RegisterOpcode(0x0EBB, PASS_TIME); // 0EBB=1,pass_time %1d%
    CLEO_RegisterOpcode(0x0EBC, GENERATE_RANDOM_INT_IN_RANGE_WITH_SEED); // 0EBC=4,generate_random_int_in_range_with_seed %1d% min %2d% max %3d% store_to %4d%
    CLEO_RegisterOpcode(0x0EBD, GENERATE_RANDOM_FLOAT_IN_RANGE_WITH_SEED); // 0EBD=4,generate_random_float_in_range_with_seed %1d% min %2d% max %3d% store_to %4d%
    CLEO_RegisterOpcode(0x0EBE, LOCATE_CAMERA_DISTANCE_TO_COORDINATES); // 0EBE=4,locate_camera_distance_to_coordinates %1d% %2d% %3d% radius %4d%
    CLEO_RegisterOpcode(0x0EBF, GET_FX_SYSTEM_POINTER); // 0EBF=2,get_fx_system_pointer %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EC0, ADD_FX_SYSTEM_PARTICLE); // 0EC0=14,add_fx_system_particle %1d% coord %2d% %3d% %4d% vel %5d% %6d% %7d% size %8d% brightness %9d% rgba %10d% %11d% %12d% %13d% lastFactor %14d%
    CLEO_RegisterOpcode(0x0EC1, IS_FX_SYSTEM_AVAILABLE_WITH_NAME); // 0EC1=1,is_fx_system_available_with_name %1s%
    CLEO_RegisterOpcode(0x0EC2, SET_STRING_UPPER); // 0EC2=1,set_string_upper %1s%
    CLEO_RegisterOpcode(0x0EC3, SET_STRING_LOWER); // 0EC3=1,set_string_lower %1s%
    CLEO_RegisterOpcode(0x0EC4, STRING_FIND); // 0EC4=4,string_find %1d% %2s% %3s% store_to %4d%
    CLEO_RegisterOpcode(0x0EC5, CUT_STRING_AT); // 0EC5=2,cut_string_at %1d% %2d%
    CLEO_RegisterOpcode(0x0EC6, IS_STRING_CHARACTER_AT); // 0EC6=3,is_string_character_at %1s% character %2s% index %3s%
    CLEO_RegisterOpcode(0x0EC8, GET_CHAR_RANDOM_SEED); // 0EC8=2,get_char_random_seed %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EC9, GET_CAR_RANDOM_SEED); // 0EC9=2,get_car_random_seed %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0ECA, GET_OBJECT_RANDOM_SEED); // 0ECA=2,get_object_random_seed %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0ECB, GET_CHAR_MOVE_STATE); // 0ECB=2,get_char_move_state %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0ECC, DONT_DELETE_CHAR_UNTIL_TIME); // 0ECC=2,dont_delete_char_until_time %1d% %2d%
    CLEO_RegisterOpcode(0x0ECD, DONT_DELETE_CAR_UNTIL_TIME); // 0ECD=2,dont_delete_car_until_time %1d% %2d%
    CLEO_RegisterOpcode(0x0ECE, GET_TIME_CHAR_IS_DEAD); // 0ECE=2,get_time_char_is_dead %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0ECF, GET_TIME_CAR_IS_DEAD); // 0ECF=2,get_time_car_is_dead %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0ED9, SET_CHAR_IGNORE_DAMAGE_ANIMS); // 0ED9=2,set_char_ignore_damage_anims %1d% %2d%
    CLEO_RegisterOpcode(0x0EE4, LOCATE_CHAR_DISTANCE_TO_CHAR); // 0EE4=3,locate_char_distance_to_char %1d% char %2d% radius %3d%
    CLEO_RegisterOpcode(0x0EE5, LOCATE_CHAR_DISTANCE_TO_CAR); // 0EE5=3,locate_char_distance_to_car %1d% car %2d% radius %3d%
    CLEO_RegisterOpcode(0x0EE6, LOCATE_CHAR_DISTANCE_TO_OBJECT); // 0EE6=3,locate_char_distance_to_object %1d% object %2d% radius %3d%
    CLEO_RegisterOpcode(0x0EE7, LOCATE_CAR_DISTANCE_TO_OBJECT); // 0EE7=3,locate_car_distance_to_object %1d% object %2d% radius %3d%
    CLEO_RegisterOpcode(0x0EE8, LOCATE_CAR_DISTANCE_TO_CAR); // 0EE8=3,locate_car_distance_to_car %1d% car %2d% radius %3d%
    CLEO_RegisterOpcode(0x0EE9, LOCATE_OBJECT_DISTANCE_TO_OBJECT); // 0EE9=3,locate_object_distance_to_object %1d% object %2d% radius %3d%
    CLEO_RegisterOpcode(0x0EEA, LOCATE_CHAR_DISTANCE_TO_COORDINATES); // 0EEA=5,locate_char_distance_to_coordinates %1d% pos %2d% %3d% %4d% radius %5d%
    CLEO_RegisterOpcode(0x0EEB, LOCATE_CAR_DISTANCE_TO_COORDINATES); // 0EEB=5,locate_car_distance_to_coordinates %1d% pos %2d% %3d% %4d% radius %5d%
    CLEO_RegisterOpcode(0x0EEC, LOCATE_OBJECT_DISTANCE_TO_COORDINATES); // 0EEC=5,locate_object_distance_to_coordinates %1d% pos %2d% %3d% %4d% radius %5d%
    CLEO_RegisterOpcode(0x0EED, LOCATE_ENTITY_DISTANCE_TO_ENTITY); // 0EED=3,locate_entity_distance_to_entity %1d% entityB %2d% radius %3d%
    CLEO_RegisterOpcode(0x0EEE, GET_ENTITY_COORDINATES); // 0EEE=4,get_entity_coordinates %1d% store_to %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0EEF, GET_ENTITY_HEADING); // 0EEF=2,get_entity_heading %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EF5, IS_CAR_OWNED_BY_PLAYER); // 0EF5=1,is_car_owned_by_player %1d%
    CLEO_RegisterOpcode(0x0EF6, SET_CAR_OWNED_BY_PLAYER); // 0EF6=2,set_car_owned_by_player %1d% %2d%
    CLEO_RegisterOpcode(0x0EF8, GET_MODEL_INFO); // 0EF8=2,get_model_info %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EF9, GET_CAR_ANIMGROUP); // 0EF9=2,get_car_animgroup %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EFA, GET_CHAR_FEAR); // 0EFA=2,get_char_fear %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EFB, IS_CAR_CONVERTIBLE); // 0EFB=1,is_car_convertible %1d%
    CLEO_RegisterOpcode(0x0EFC, GET_CAR_VALUE); // 0EFC=2,get_car_value %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0EFD, GET_CAR_PEDALS); // 0EFD=3,get_car_pedals %1d% gas_to %2d% break_to %3d% 
    CLEO_RegisterOpcode(0x0EFE, GET_LOADED_LIBRARY); // 0EFE=2,get_loaded_library %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0F0A, RETURN_TIMES); // 0F0A=1,return_times %1d%
    CLEO_RegisterOpcode(0x0F0E, GET_THIRD_PERSON_CAMERA_TARGET); // 0F0E=10,get_third_person_camera_target %1d% from %2d% %3d% %4d% start_to %5d% %6d% %7d% end_to %8d% %9d% %10d%
    CLEO_RegisterOpcode(0x0F0F, GET_DISTANCE_MULTIPLIER); // 0F0F=2,get_distance_multiplier %1d% %2d%
    CLEO_RegisterOpcode(0x0F10, GET_ACTIVE_CAMERA_ROTATION); // 0F10=3,get_active_camera_rotation %1d% %2d% %3d%
    CLEO_RegisterOpcode(0x0F11, GET_CLOSEST_WATER_DISTANCE); // 0F11=2,get_closest_water_distance %1d% %2d%
    CLEO_RegisterOpcode(0x0F12, GET_CAMERA_STRUCT); // 0F12=2,get_camera_struct %1d% %2d%
    CLEO_RegisterOpcode(0x0F14, GET_CAMERA_ROTATION_INPUT_VALUES); // 0F14=2,get_camera_rotation_input_values %1d% %2d%
    CLEO_RegisterOpcode(0x0F15, SET_CAMERA_ROTATION_INPUT_VALUES); // 0F15=2,set_camera_rotation_input_values %1d% %2d%
    CLEO_RegisterOpcode(0x0F16, SET_ON_MISSION); // 0F16=1,set_on_mission %1d%
    CLEO_RegisterOpcode(0x0F17, GET_MODEL_NAME_POINTER); // 0F17=2,get_model_name_pointer %1d% to %2s%
    CLEO_RegisterOpcode(0x0D0F, SET_CAR_MODEL_ALPHA); // 0D0F=2,set_car_model_alpha %1d% alpha %2d%
    CLEO_RegisterOpcode(0x0D10, SET_CHAR_MODEL_ALPHA); // 0D10=2,set_char_model_alpha %1d% alpha %2d%
    CLEO_RegisterOpcode(0x0D11, SET_OBJECT_MODEL_ALPHA); // 0D11=2,set_object_model_alpha %1d% alpha %2d%
    CLEO_RegisterOpcode(0x0D2D, GET_LOCAL_TIME); // 0D2D=8,get_local_time_year_to %1d% month_to %2d% day_of_week_to %3d% day_to %4d% hour_to %5d% minute_to %6d% second_to %7d% milliseconds_to %8d%
    CLEO_RegisterOpcode(0x0D2E, SET_SCRIPT_VAR); // 0D2E=3,set_script %1d% var %2d% value %3d%
    CLEO_RegisterOpcode(0x0D2F, GET_SCRIPT_VAR); // 0D2F=3,%3d% = get_script %1d% var %2d% // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0D33, SET_CAR_DOOR_WINDOW_STATE); // 0D33=3,set_car %1d% door %2d% window_state %3d%

    // Drawing
    CLEO_RegisterOpcode(0x0E1E, DRAW_TEXTURE_PLUS); // 0E1E=15,draw_texture_plus %1d% event %2d% pos %3d% %4d% size %5d% %6d% angle %7d% depth %8d% fix_aspect_ratio %9d% maskTrisCount %10d% maskTrisArray %11d% rgba %12d% %13d% %14d% %15d% 
    CLEO_RegisterOpcode(0x0E3C, GET_TEXTURE_FROM_SPRITE); // 0E3C=2,get_texture_from_sprite %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E62, DRAW_STRING); // 0E62=8,print %1s% event %2d% at %3d% %4d% scale %5d% %6d% fixAR %7d% style %8d%
    CLEO_RegisterOpcode(0x0E63, DRAW_STRING_EXT); // 0E63=27,print %1s% event %2d% at %3d% %4d% scale %5d% %6d% fixAR %7d% style %8d% prop %9d% align %10d% wrap %11d% justify %12d% color %13d% %14d% %15d% %16d% outline %17d% shadow %18d% dropColor %19d% %20d% %21d% %22d% background %23d% backColor %24d% %25d% %26d% %27d%

    // Math
    CLEO_RegisterOpcode(0x0D1E, QUAT_SLERP); // 0D1E=4,quat_slerp %1d% to %2d% lambda %3d% result %4d%
    CLEO_RegisterOpcode(0x0D16, SET_MATRIX_ROTATION_FROM_QUAT); // 0D16=2,set_matrix_rotation_from_quat %1d% rotation_from_quat %2d%
    CLEO_RegisterOpcode(0x0D17, SET_QUAT_FROM_MATRIX); // 0D17=2,set_quat_from_matrix %1d% to_quat %2d%
    CLEO_RegisterOpcode(0x0D18, ROTATE_QUAT_ON_AXIS); // 0D18=6,rotate_quat_on_axis %1d% axis_vector %2d% %3d% %4d% angle %5d% combine_op %6d%
    CLEO_RegisterOpcode(0x0D19, GET_NORMALISED_QUAT); // 0D19=2,get_normalised_quat %1d% to_quat %2d%
    CLEO_RegisterOpcode(0x0D1A, MULTIPLY_QUATS); // 0D1A=3,multiply_quats %3d% = quat %1d% * quat %2d% // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0D24, INITIALISE_QUAT); // 0D24=5,set_quat %1d% elements %2d% %3d% %4d% %5d%
    CLEO_RegisterOpcode(0x0D29, GET_QUAT_ELEMENTS); // 0D29=5,get_quat %1d% elements_to %2d% %3d% %4d% %5d%
    CLEO_RegisterOpcode(0x0D01, ROTATE_MATRIX_ON_AXIS); // 0D01=6,rotate_matrix %1d% on_axis %2d% %3d% %4d% angle %5d% combine_op %6d%
    CLEO_RegisterOpcode(0x0D02, GET_MATRIX_X_ANGLE); // 0D02=2,%2d% = matrix %1d% x_angle // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0D03, GET_MATRIX_Y_ANGLE); // 0D03=2,%2d% = matrix %1d% y_angle // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0D04, GET_MATRIX_Z_ANGLE); // 0D04=2,%2d% = matrix %1d% z_angle // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0E1F, EASE); // 0E1F=4,ease %1d% mode %2d% way %3d% to %4d%
    CLEO_RegisterOpcode(0x0E27, GET_ANGLE_FROM_TWO_COORDS); // 0E27=5,get_angle_from_two_coords %1d% %2d% and %3d% %4d% to %5d%
    CLEO_RegisterOpcode(0x0E03, PERLIN_NOISE); // 0E03=2,perlin_noise %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E29, PERLIN_NOISE_FRACTAL); // 0E29=7,perlin_noise %1d% octaves %2d% frequency %3d% amplitude %4d% lacunarity %5d% persistence %6d% store_to %7d%
    CLEO_RegisterOpcode(0x0EF0, GET_COORD_FROM_ANGLED_DISTANCE); // 0EF0=6,get_coord_from_angled_distance %1d% %2d% angle %3d% dist %4d% store_to %5d% %6d%
    CLEO_RegisterOpcode(0x0EF1, PERLIN_NOISE_FRACTAL_2D); // 0EF1=8,perlin_noise_fractal_2d x %1d% y %2d% octaves %3d% frequency %4d% amplitude %5d% lacunarity %6d% persistence %7d% store_to %8d%
    CLEO_RegisterOpcode(0x0EF2, PERLIN_NOISE_FRACTAL_3D); // 0EF2=9,perlin_noise_fractal_3d x %1d% y %2d% z %3d% octaves %4d% frequency %5d% amplitude %6d% lacunarity %7d% persistence %8d% store_to %9d%
    CLEO_RegisterOpcode(0x0EF4, CLAMP_FLOAT); // 0EF4=4,clamp_float %1d% min %2d% max %3d% store_to %4d%
    CLEO_RegisterOpcode(0x0EF7, CLAMP_INT); // 0EF7=4,clamp_int %1d% min %2d% max %3d% store_to %4d%
    CLEO_RegisterOpcode(0x0EB3, CONVERT_DIRECTION_TO_QUAT); // 0EB3=4,convert_direction_to_quat %1d% dir %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0EF3, LERP); // 0EF3=4,lerp %1d% %2d% %3d% store_to %4d%
    CLEO_RegisterOpcode(0x0F0D, SET_MATRIX_LOOK_DIRECTION); // 0F0D=7,set_matrix_look_direction %1d% origin %2d% %3d% %4d% dir %5d% %6d% %7d%

    // Audio
    CLEO_RegisterOpcode(0x0E21, GET_AUDIO_SFX_VOLUME); // 0E21=1,get_audio_sfx_volume %1d%
    CLEO_RegisterOpcode(0x0E22, GET_AUDIO_RADIO_VOLUME); // 0E22=1,get_audio_radio_volume %1d%
    CLEO_RegisterOpcode(0x0E3B, GET_AUDIOSTREAM_INTERNAL); // 0E3B=2,get_audiostream_internal %1d% store_to %2d%

    // Memory
    CLEO_RegisterOpcode(0x0E28, WRITE_STRUCT_OFFSET); // 0E28=4,write_struct %1d% offset %2d% size %3d% value %4d%
    CLEO_RegisterOpcode(0x0D4E, READ_STRUCT_OFFSET); // 0D4E=4,%4d% = read_struct %1d% offset %2d% size %3d% // keep newOpcodes ordering for SB
    CLEO_RegisterOpcode(0x0D4F, WRITE_STRUCT_OFFSET); // // newOpcodes version of 0E28, the original one doesn't work, this is a fall off
    CLEO_RegisterOpcode(0x0D27, COPY_MEMORY); // 0D27=3,copy_memory %1d% to %2d% size %3d%
    CLEO_RegisterOpcode(0x0D37, WRITE_STRUCT_PARAM); // 0D37=3,write_struct_param %1d% param %2d% value %3d%
    CLEO_RegisterOpcode(0x0D38, READ_STRUCT_PARAM); // 0D38=3,%3d% = read_struct_param %1d% param %2d% // keep newOpcodes ordering for SB
    CLEO_RegisterOpcode(0x0E6A, MAKE_NOP); // 0E6A=2,make_nop %1d% size %2d%
    CLEO_RegisterOpcode(0x0EE2, READ_STRUCT_OFFSET_MULTI); // 0EE2=-1,read_struct_offset_multi %1d% offset %2d% total %3d% size %4d%
    CLEO_RegisterOpcode(0x0EE3, WRITE_STRUCT_OFFSET_MULTI); // 0EE3=-1,write_struct_offset_multi %1d% offset %2d% count %3d% size %4d%
    
    // Radar blips
    RadarBlip_Patch();
    CLEO_RegisterOpcode(0x0E2A, ADD_CLEO_BLIP); // 0E2A=9,add_cleo_blip %1d% position %2d% %3d% is_short %4d% RGBA %5d% %6d% %7d% %8d% store_to %9d%
    CLEO_RegisterOpcode(0x0E2B, REMOVE_CLEO_BLIP); // 0E2B=1,remove_cleo_blip %1d%
    //CLEO_RegisterOpcode(0x, CLEO_BLIP_FOLLOW_ENTITY); // 0000=2,cleo_blip %1d% follow_entity %2d% // custom opcode, no opcode yet

    // Render object
    /*CLEO_RegisterOpcode(0x0E2E, CREATE_RENDER_OBJECT_TO_CHAR_BONE); // 0E2E=10,create_render_object_to_char_bone %1d% model %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d% store_to %10d%
    CLEO_RegisterOpcode(0x0F02, CREATE_RENDER_OBJECT_TO_CHAR_BONE_FROM_SPECIAL); // 0F02=10,create_render_object_to_char_bone_from_special %1d% special_model %2d% bone %3d% offset %4d% %5d% %6d% rotation %7d% %8d% %9d% scale %10d% %11d% %12d% store_to %13d%
    CLEO_RegisterOpcode(0x0F03, CREATE_RENDER_OBJECT_TO_OBJECT); // 0F03=9,create_render_object_to_object %1d% model %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d% store_to %9d%
    CLEO_RegisterOpcode(0x0F04, CREATE_RENDER_OBJECT_TO_OBJECT_FROM_SPECIAL); // 0F04=9,create_render_object_to_object_from_special %1d% special_model %2d% offset %3d% %4d% %5d% rotation %6d% %7d% %8d% store_to %9d%
    CLEO_RegisterOpcode(0x0E2F, DELETE_RENDER_OBJECT); // 0E2F=1,delete_render_object %1d%
    CLEO_RegisterOpcode(0x0E30, SET_RENDER_OBJECT_AUTO_HIDE); // 0E30=4,set_render_object_auto_hide %1d% dead %2d% weapon %3d% car %4d%
    CLEO_RegisterOpcode(0x0E31, SET_RENDER_OBJECT_VISIBLE); // 0E31=2,set_render_object_visible %1d% %2d%
    CLEO_RegisterOpcode(0x0E35, SET_RENDER_OBJECT_POSITION); // 0E35=4,set_render_object_position %1d% %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0E36, SET_RENDER_OBJECT_ROTATION); // 0E36=4,set_render_object_rotation %1d% %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0E37, SET_RENDER_OBJECT_SCALE); // 0E37=4,set_render_object_scale %1d% %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0E3A, SET_RENDER_OBJECT_DISTORTION); // 0E3A=5,set_render_object_distortion %1d% %2d% %3d% %4d% %5d%
    */
    // Pickup
    CLEO_RegisterOpcode(0x0E33, GET_PICKUP_THIS_COORD); // 0E33=5,get_pickup_this_coord %1d% %2d% %3d% only_available %4d% store_to %5d%
    CLEO_RegisterOpcode(0x0E34, GET_PICKUP_MODEL); // 0E34=2,get_pickup_model %1d% %2d%
    CLEO_RegisterOpcode(0x0E38, GET_PICKUP_POINTER); // 0E38=2,get_pickup_pointer %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E39, GET_PICKUP_TYPE); // 0E39=2,get_pickup_type %1d% store_to %2d%
    
    // Timer
    /*CLEO_RegisterOpcode(0x0E4E, DISPLAY_ONSCREEN_TIMER_LOCAL); // 0E4E=2,display_onscreen_timer_local %1d% direction %2d%
    CLEO_RegisterOpcode(0x0E4F, DISPLAY_ONSCREEN_TIMER_WITH_STRING_LOCAL); // 0E4F=3,display_onscreen_timer_with_string_local %1d% direction %2d% GXT %3d%
    CLEO_RegisterOpcode(0x0E50, DISPLAY_ONSCREEN_COUNTER_LOCAL); // 0E50=2,display_onscreen_counter_local %1d% direction %2d%
    CLEO_RegisterOpcode(0x0E51, DISPLAY_ONSCREEN_COUNTER_WITH_STRING_LOCAL); // 0E51=3,display_onscreen_counter_with_string_local %1d% direction %2d% GXT %3d%
    CLEO_RegisterOpcode(0x0E52, DISPLAY_TWO_ONSCREEN_COUNTERS_LOCAL); // 0E52=2,display_two_onscreen_counters_local %1d% max_value %2d%
    CLEO_RegisterOpcode(0x0E53, DISPLAY_TWO_ONSCREEN_COUNTERS_WITH_STRING_LOCAL); // 0E53=3,display_two_onscreen_counters_with_string_local %1d% max_value %2d% GXT %3d%
    CLEO_RegisterOpcode(0x0E54, CLEAR_ONSCREEN_TIMER_LOCAL); // 0E54=1,clear_onscreen_timer_local %1d%
    CLEO_RegisterOpcode(0x0E55, CLEAR_ONSCREEN_COUNTER_LOCAL); // 0E55=1,clear_onscreen_counter_local %1d%
    CLEO_RegisterOpcode(0x0E56, SET_ONSCREEN_COUNTER_FLASH_WHEN_FIRST_DISPLAYED_LOCAL); // 0E56=2,set_onscreen_counter_flash_when_first_displayed_local %1d% flash %2d%
    CLEO_RegisterOpcode(0x0E57, SET_TIMER_BEEP_COUNTDOWN_TIME_LOCAL); // 0E57=2,set_timer_beep_countdown_time_local %1d% secs %2d%
    CLEO_RegisterOpcode(0x0E58, SET_ONSCREEN_COUNTER_COLOUR_LOCAL); // 0E58=2,set_onscreen_counter_colour_local %1d% color %2d%

    // Intelligence
    CLEO_RegisterOpcode(0x0E42, IS_CHAR_DOING_TASK_ID); // 0E42=2,is_char_doing_task_id %1d% %2d%
    CLEO_RegisterOpcode(0x0E43, GET_CHAR_TASK_POINTER_BY_ID); // 0E43=3,get_char_task_pointer_by_id %1d% %2d% store_to %3d%
    CLEO_RegisterOpcode(0x0E44, GET_CHAR_KILL_TARGET_CHAR); // 0E44=2,get_char_kill_target_char %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E46, IS_CHAR_USING_GUN); // 0E46=1,is_char_using_gun %1d%
    CLEO_RegisterOpcode(0x0E47, IS_CHAR_FIGHTING); // 0E47=1,is_char_fighting %1d%
    CLEO_RegisterOpcode(0x0E48, IS_CHAR_FALLEN_ON_GROUND); // 0E48=1,is_char_fallen_on_ground %1d%
    CLEO_RegisterOpcode(0x0E49, IS_CHAR_ENTERING_ANY_CAR); // 0E49=1,is_char_entering_any_car %1d%
    CLEO_RegisterOpcode(0x0E4A, IS_CHAR_EXITING_ANY_CAR); // 0E4A=1,is_char_exiting_any_car %1d%
    CLEO_RegisterOpcode(0x0E4B, IS_CHAR_PLAYING_ANY_SCRIPT_ANIMATION); // 0E4B=2,is_char_playing_any_script_animation %1d% include_anims %2d%
    CLEO_RegisterOpcode(0x0E4C, IS_CHAR_DOING_ANY_IMPORTANT_TASK); // 0E4C=2,is_char_doing_any_important_task %1d% include_anims %2d%
    CLEO_RegisterOpcode(0x0E96, CLEAR_CHAR_PRIMARY_TASKS); // 0E96=1,clear_char_primary_tasks %1d%
    CLEO_RegisterOpcode(0x0E97, CLEAR_CHAR_SECONDARY_TASKS); // 0E97=1,clear_char_secondary_tasks %1d%
    CLEO_RegisterOpcode(0x0EFF, GET_CHAR_SIMPLEST_ACTIVE_TASK); // 0EFF=3,get_char_simplest_active_task %1d% id_to %2d% pointer_to %3d%
*/
    // Colpoint
    CLEO_RegisterOpcode(0x0D3A, GET_COLLISION_BETWEEN_POINTS); // 0D3A=20,get_collision_between_points %1d% %2d% %3d% and %4d% %5d% %6d% flags %7d% %8d% %9d% %10d% %11d% %12d% %13d% %14d% ignore_entity %15d% store_point_to %17d% %18d% %19d% entity_to %20d% colpoint_data_to %16d% // keep NewOpcodes order
    CLEO_RegisterOpcode(0x0D3B, GET_COLPOINT_NORMAL_VECTOR); // 0D3B=4,get_colpoint_normal_vector %1d% store_to %2d% %3d% %4d%
    CLEO_RegisterOpcode(0x0D3C, GET_COLPOINT_SURFACE); // 0D3C=2,get_colpoint_surface %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0D3E, GET_COLPOINT_DEPTH); // 0D3E=2,get_colpoint_depth %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E6B, GET_COLPOINT_LIGHTING); // 0E6B=3,get_colpoint_lighting %1d% from_night %2d% store_to %3d%
    CLEO_RegisterOpcode(0x0EE1, GET_COLPOINT_COORDINATES); // 0EE1=4,get_colpoint_coordinates %1d% store_to %2d% %3d% %4d%

    // List
    CLEO_RegisterOpcode(0x0E72, CREATE_LIST); // 0E72=2,create_list %1d% store_to %2d%
    CLEO_RegisterOpcode(0x0E73, DELETE_LIST); // 0E73=1,delete_list %1d%
    CLEO_RegisterOpcode(0x0E74, LIST_ADD); // 0E74=2,list_add %1d% value %2d%
    CLEO_RegisterOpcode(0x0E75, LIST_REMOVE_VALUE); // 0E75=2,list_remove_value %1d% value %2d%
    CLEO_RegisterOpcode(0x0E76, LIST_REMOVE_INDEX); // 0E76=2,list_remove_index %1d% index %2d%
    CLEO_RegisterOpcode(0x0E77, GET_LIST_SIZE); // 0E77=2,get_list_size %1d% size %2d%
    CLEO_RegisterOpcode(0x0E78, GET_LIST_VALUE_BY_INDEX); // 0E78=3,get_list_value_by_index %1d% index %2d% store_to %3d%
    CLEO_RegisterOpcode(0x0E79, RESET_LIST); // 0E79=1,reset_list %1d%
    CLEO_RegisterOpcode(0x0E7A, GET_LIST_STRING_VALUE_BY_INDEX); // 0E7A=3,get_list_string_value_by_index %1d% index %2d% store_to %3d%
    CLEO_RegisterOpcode(0x0E7B, LIST_ADD_STRING); // 0E7B=2,list_add_string %1d% value %2d%
    CLEO_RegisterOpcode(0x0E7C, LIST_REMOVE_STRING_VALUE); // 0E7C=2,list_remove_string_value %1d% value %2d%
    CLEO_RegisterOpcode(0x0E7D, LIST_REMOVE_INDEX_RANGE); // 0E7D=3,list_remove_index %1d% start %2d% end %3d%
    CLEO_RegisterOpcode(0x0E7E, REVERSE_LIST); // 0E7E=1,reverse_list %1d%
    CLEO_RegisterOpcode(0x0F06, REPLACE_LIST_VALUE_BY_INDEX); // 0F06=3,replace_list_value_by_index %1d% index %2d% value %3d%
    CLEO_RegisterOpcode(0x0F07, REPLACE_LIST_STRING_VALUE_BY_INDEX); // 0F07=3,replace_list_string_value_by_index %1d% index %2d% value %3d%
    CLEO_RegisterOpcode(0x0F08, INSERT_LIST_VALUE_BY_INDEX); // 0F08=3,insert_list_value_by_index %1d% index %2d% value %3d%
    CLEO_RegisterOpcode(0x0F09, INSERT_LIST_STRING_VALUE_BY_INDEX); // 0F09=3,insert_list_string_value_by_index %1d% index %2d% value %3d%

    // Bones
    CLEO_RegisterOpcode(0x0D0B, GET_CHAR_BONE_MATRIX); // 0D0B=3,get_actor %1d% bone %2d% matrix_to %3d% // IF and SET    
    CLEO_RegisterOpcode(0x0D30, GET_CHAR_BONE); // 0D30=3,%3d% = actor %1d% bone %2d% // IF and SET  
    CLEO_RegisterOpcode(0x0D31, GET_BONE_OFFSET_VECTOR); // 0D31=2,%2d% = bone %1d% offset_vector     
    CLEO_RegisterOpcode(0x0D32, GET_BONE_QUAT); // 0D32=2,%2d% = bone %1d% quat

    // Special Models
    CLEO_RegisterOpcode(0x0F00, LOAD_SPECIAL_MODEL); // 0F00=3,load_special_model_dff %1s% txd %2s% store_to %3d%
    CLEO_RegisterOpcode(0x0F01, REMOVE_SPECIAL_MODEL); // 0F01=1,remove_special_model %1d%
    CLEO_RegisterOpcode(0x0F05, GET_SPECIAL_MODEL_DATA); // 0F05=4,get_special_model_data %1d% clump_to %2d% atomic_to %3d% txd_index_to %4d%
}