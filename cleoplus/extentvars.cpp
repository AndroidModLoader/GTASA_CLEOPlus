#include "externs.h"

CPool<PedExtVars> *ms_pPedExtVarsPool = NULL;
CPool<VehicleExtVars> *ms_pVehicleExtVarsPool = NULL;
CPool<ObjectExtVars> *ms_pObjectExtVarsPool = NULL;

CLEO_Fn(INIT_EXTENDED_CHAR_VARS)
{
    char buf[100];
    bool bResult = false;
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int totalVars = cleo->ReadParam(handle)->i;
    if(totalVars > 0)
    {
        auto xdata = *GetExtData(ped);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            free(extVars->data);
            xdata.extendedVarsList.remove(extVars);
            delete extVars;
        }
        xdata.extendedVarsList.push_back(new ExtendedVars(findId, totalVars));
        bResult = true;
    }
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(SET_EXTENDED_CHAR_VAR)
{
    char buf[100];
    bool bResult = false;
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int varId = cleo->ReadParam(handle)->i - 1;
    int varValue = cleo->ReadParam(handle)->i;
    if(varId >= 0)
    {
        auto xdata = *GetExtData(ped);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            ((int*)(extVars->data))[varId] = varValue;
            bResult = true;
        }
    }
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(GET_EXTENDED_CHAR_VAR)
{
    char buf[100];
    bool bResult = false;
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int varId = cleo->ReadParam(handle)->i - 1;
    int varValue = 0;
    if(varId >= 0)
    {
        auto xdata = *GetExtData(ped);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            varValue = ((int*)(extVars->data))[varId];
            bResult = true;
        }
    }
    cleo->GetPointerToScriptVar(handle)->i = varValue;
    cleoaddon->UpdateCompareFlag(handle, bResult);
}

CLEO_Fn(INIT_EXTENDED_CAR_VARS)
{
    char buf[100];
    bool bResult = false;
    CVehicle *veh = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int totalVars = cleo->ReadParam(handle)->i;
    if(totalVars > 0)
    {
        auto xdata = *GetExtData(veh);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            free(extVars->data);
            xdata.extendedVarsList.remove(extVars);
            delete extVars;
        }
        xdata.extendedVarsList.push_back(new ExtendedVars(findId, totalVars));
        bResult = true;
    }
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(SET_EXTENDED_CAR_VAR)
{
    char buf[100];
    bool bResult = false;
    CVehicle *veh = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int varId = cleo->ReadParam(handle)->i - 1;
    int varValue = cleo->ReadParam(handle)->i;
    if(varId >= 0)
    {
        auto xdata = *GetExtData(veh);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            ((int*)(extVars->data))[varId] = varValue;
            bResult = true;
        }
    }
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(GET_EXTENDED_CAR_VAR)
{
    char buf[100];
    bool bResult = false;
    CVehicle *veh = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int varId = cleo->ReadParam(handle)->i - 1;
    int varValue = 0;
    if(varId >= 0)
    {
        auto xdata = *GetExtData(veh);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            varValue = ((int*)(extVars->data))[varId];
            bResult = true;
        }
    }
    cleo->GetPointerToScriptVar(handle)->i = varValue;
    cleoaddon->UpdateCompareFlag(handle, bResult);
}

CLEO_Fn(INIT_EXTENDED_OBJECT_VARS)
{
    char buf[100];
    bool bResult = false;
    CObject *obj = GetObjectFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int totalVars = cleo->ReadParam(handle)->i;
    if(totalVars > 0)
    {
        auto xdata = *GetExtData(obj);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            free(extVars->data);
            xdata.extendedVarsList.remove(extVars);
            delete extVars;
        }
        xdata.extendedVarsList.push_back(new ExtendedVars(findId, totalVars));
        bResult = true;
    }
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(SET_EXTENDED_OBJECT_VAR)
{
    char buf[100];
    bool bResult = false;
    CObject *obj = GetObjectFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int varId = cleo->ReadParam(handle)->i - 1;
    int varValue = cleo->ReadParam(handle)->i;
    if(varId >= 0)
    {
        auto xdata = *GetExtData(obj);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            ((int*)(extVars->data))[varId] = varValue;
            bResult = true;
        }
    }
    cleoaddon->UpdateCompareFlag(handle, bResult);
}
CLEO_Fn(GET_EXTENDED_OBJECT_VAR)
{
    char buf[100];
    bool bResult = false;
    CObject *obj = GetObjectFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(cleoaddon->ReadString(handle, buf, sizeof(buf)));
    int varId = cleo->ReadParam(handle)->i - 1;
    int varValue = 0;
    if(varId >= 0)
    {
        auto xdata = *GetExtData(obj);
        if (findId == 2949658545) // "AUTO"
            findId = (uint32_t)handle;

        ExtendedVars *extVars = FindExtendedVarsFromId(xdata.extendedVarsList, findId);
        if (extVars)
        {
            varValue = ((int*)(extVars->data))[varId];
            bResult = true;
        }
    }
    cleo->GetPointerToScriptVar(handle)->i = varValue;
    cleoaddon->UpdateCompareFlag(handle, bResult);
}