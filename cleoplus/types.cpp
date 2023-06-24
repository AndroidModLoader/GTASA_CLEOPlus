#include "externs.h"

CLEO_Fn(GET_VEHICLE_SUBCLASS)
{
    int ref = cleo->ReadParam(handle)->i;
    CVehicle* veh = GetVehicleFromRef(ref);
    cleo->GetPointerToScriptVar(handle)->i = veh->m_nVehicleSubType;
}
CLEO_Fn(GET_ENTITY_TYPE)
{
    CEntity* ent = (CEntity*)cleo->ReadParam(handle)->i;
    // Why in the world it is not as simple as this: return ent->m_nType ?
    switch(ent->m_nType)
    {
        default:
        {
            cleo->GetPointerToScriptVar(handle)->i = 0;
            break;
        }
        case ENTITY_TYPE_BUILDING:
        {
            cleo->GetPointerToScriptVar(handle)->i = 1;
            break;
        }
        case ENTITY_TYPE_OBJECT:
        {
            cleo->GetPointerToScriptVar(handle)->i = 2;
            break;
        }
        case ENTITY_TYPE_PED:
        {
            cleo->GetPointerToScriptVar(handle)->i = 3;
            break;
        }
        case ENTITY_TYPE_VEHICLE:
        {
            cleo->GetPointerToScriptVar(handle)->i = 4;
            break;
        }
    }
}