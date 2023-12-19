#include "externs.h"

CPool<PedExtVars> *ms_pPedExtVarsPool = NULL;
CPool<VehicleExtVars> *ms_pVehicleExtVarsPool = NULL;
CPool<ObjectExtVars> *ms_pObjectExtVarsPool = NULL;

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

CLEO_Fn(INIT_EXTENDED_CHAR_VARS)
{
    char buf[100];
    bool bResult = false;
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(SET_EXTENDED_CHAR_VAR)
{
    char buf[100];
    bool bResult = false;
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(GET_EXTENDED_CHAR_VAR)
{
    char buf[100];
    bool bResult = false;
    CPed *ped = GetPedFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}

CLEO_Fn(INIT_EXTENDED_CAR_VARS)
{
    char buf[100];
    bool bResult = false;
    CVehicle *ped = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(SET_EXTENDED_CAR_VAR)
{
    char buf[100];
    bool bResult = false;
    CVehicle *ped = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(GET_EXTENDED_CAR_VAR)
{
    char buf[100];
    bool bResult = false;
    CVehicle *ped = GetVehicleFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}

CLEO_Fn(INIT_EXTENDED_OBJECT_VARS)
{
    char buf[100];
    bool bResult = false;
    CObject *ped = GetObjectFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(SET_EXTENDED_OBJECT_VAR)
{
    char buf[100];
    bool bResult = false;
    CObject *ped = GetObjectFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(GET_EXTENDED_OBJECT_VAR)
{
    char buf[100];
    bool bResult = false;
    CObject *ped = GetObjectFromRef(cleo->ReadParam(handle)->i);
    uint32_t findId = GetUppercaseKey(CLEO_ReadStringEx(handle, buf, sizeof(buf)));
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
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}