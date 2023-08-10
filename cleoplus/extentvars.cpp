#include "externs.h"


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

// Yes, we are doing writing in paddings of the memory structs!
DECL_HOOKv(PedConstruct, CPed* self, unsigned int a1)
{
    PedConstruct(self, a1);
    self->ppad14 = 0;
}
DECL_HOOKv(PedDestruct, CPed* self)
{
    self->ppad14 = 0;
    PedDestruct(self);
}
DECL_HOOKv(VehicleConstruct, CVehicle* self, uint8_t a1)
{
    VehicleConstruct(self, a1);
    self->vehfiller6_1 = 0;
}
DECL_HOOKv(VehicleDestruct, CVehicle* self)
{
    self->vehfiller6_1 = 0;
    VehicleDestruct(self);
}
DECL_HOOKv(ObjectInit, CObject* self)
{
    ObjectInit(self);
    self->opad2 = 0;
}
DECL_HOOKv(ObjectDestruct, CObject* self)
{
    self->opad2 = 0;
    ObjectDestruct(self);
}
void ExtEntVars_Patch()
{
    //HOOK(PedConstruct, aml->GetSym(hGTASA, "_ZN4CPedC2Ej"));
    //HOOK(PedDestruct, aml->GetSym(hGTASA, "_ZN4CPedD2Ev"));

    //HOOK(VehicleConstruct, aml->GetSym(hGTASA, "_ZN8CVehicleC2Eh"));
    //HOOK(VehicleDestruct, aml->GetSym(hGTASA, "_ZN8CVehicleD2Ev"));

    //HOOK(ObjectInit, aml->GetSym(hGTASA, "_ZN7CObject4InitEv"));
    //HOOK(ObjectDestruct, aml->GetSym(hGTASA, "_ZN7CObjectD0Ev"));
}