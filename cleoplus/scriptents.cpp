#include "externs.h"

CLEO_Fn(IS_CAR_SCRIPT_CONTROLLED)
{
    int ref = cleo->ReadParam(handle)->i;
    CVehicle* veh = GetVehicleFromRef(ref);
    cleoaddon->UpdateCompareFlag(handle, (veh != NULL && veh->m_nCreateBy == eVehicleCreatedBy::MISSION_VEHICLE));
}
CLEO_Fn(MARK_CAR_AS_NEEDED)
{
    int ref = cleo->ReadParam(handle)->i;
    CVehicle* veh = GetVehicleFromRef(ref);
    veh->m_nCreateBy = eVehicleCreatedBy::MISSION_VEHICLE;
}
CLEO_Fn(IS_CHAR_SCRIPT_CONTROLLED)
{
    int ref = cleo->ReadParam(handle)->i;
    CPed* ped = GetPedFromRef(ref);
    cleoaddon->UpdateCompareFlag(handle, (ped != NULL && ped->m_byteCreatedBy == ePedCreatedBy::PED_MISSION));
}
CLEO_Fn(MARK_CHAR_AS_NEEDED)
{
    int ref = cleo->ReadParam(handle)->i;
    CPed* ped = GetPedFromRef(ref);
    ped->m_byteCreatedBy = ePedCreatedBy::PED_MISSION;
}
CLEO_Fn(IS_OBJECT_SCRIPT_CONTROLLED)
{
    int ref = cleo->ReadParam(handle)->i;
    CObject* object = GetObjectFromRef(ref);
    cleoaddon->UpdateCompareFlag(handle, (object != NULL && (object->m_nObjectType == eObjectType::OBJECT_MISSION || object->m_nObjectType == eObjectType::OBJECT_MISSION2)));
}
CLEO_Fn(MARK_OBJECT_AS_NEEDED)
{
    int ref = cleo->ReadParam(handle)->i;
    CObject* object = GetObjectFromRef(ref);

    CRunningScript* script = (CRunningScript*)handle;
    object->m_nObjectType = (script->bIsThisAStreamedScript || script->ScriptBrainType != 0) ? eObjectType::OBJECT_MISSION2 : eObjectType::OBJECT_MISSION;
}