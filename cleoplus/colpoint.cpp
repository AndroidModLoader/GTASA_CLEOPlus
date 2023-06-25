#include "externs.h"

CLEO_Fn(GET_COLLISION_BETWEEN_POINTS)
{
    bool bResult = false;
    CVector origin, target, outPoint; unsigned int flag[8]; CEntity *colEntity, *ignoringEntity; CColPoint *colPoint;

    origin.x = cleo->ReadParam(handle)->f;
    origin.y = cleo->ReadParam(handle)->f;
    origin.z = cleo->ReadParam(handle)->f;
    target.x = cleo->ReadParam(handle)->f;
    target.y = cleo->ReadParam(handle)->f;
    target.z = cleo->ReadParam(handle)->f;
    flag[0] =  cleo->ReadParam(handle)->i;
    flag[1] =  cleo->ReadParam(handle)->i;
    flag[2] =  cleo->ReadParam(handle)->i;
    flag[3] =  cleo->ReadParam(handle)->i;
    flag[4] =  cleo->ReadParam(handle)->i;
    flag[5] =  cleo->ReadParam(handle)->i;
    flag[6] =  cleo->ReadParam(handle)->i;
    flag[7] =  cleo->ReadParam(handle)->i;
    ignoringEntity = (CEntity*)cleo->ReadParam(handle)->i;
    colPoint = (CColPoint*)cleo->ReadParam(handle)->i;

    // So isn't required to send the pointer
    bool deleteColPointAfter = false;
    if ((uintptr_t)colPoint <= 0x00A00000)
    {
        colPoint = new CColPoint();
        deleteColPointAfter = true;
    }

    *pIgnoreEntity = ignoringEntity;
    bResult = ProcessLineOfSight(origin, target, *colPoint, colEntity, flag[0], flag[1], flag[2], flag[3], flag[4], flag[5], flag[6], flag[7]);
    *pIgnoreEntity = nullptr;

    if (bResult)
    {
        cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecPoint.x;
        cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecPoint.y;
        cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecPoint.z;
        cleo->GetPointerToScriptVar(handle)->i = (int)colEntity;
    }
    else
    {
        cleo->GetPointerToScriptVar(handle)->f = 0.0f;
        cleo->GetPointerToScriptVar(handle)->f = 0.0f;
        cleo->GetPointerToScriptVar(handle)->f = 0.0f;
        cleo->GetPointerToScriptVar(handle)->i = 0;
    }
    if(deleteColPointAfter) delete colPoint;
    UpdateCompareFlag((CRunningScript*)handle, bResult);
}
CLEO_Fn(GET_COLPOINT_NORMAL_VECTOR)
{
    CColPoint *colPoint = (CColPoint*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecNormal.x;
    cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecNormal.y;
    cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecNormal.z;
}
CLEO_Fn(GET_COLPOINT_SURFACE)
{
    CColPoint *colPoint = (CColPoint*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = colPoint->m_nSurfaceTypeB;
}
CLEO_Fn(GET_COLPOINT_DEPTH)
{
    CColPoint *colPoint = (CColPoint*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->f = colPoint->m_fDepth;
}
CLEO_Fn(GET_COLPOINT_LIGHTING)
{
    CColPoint *colPoint = (CColPoint*)cleo->ReadParam(handle)->i;
    bool night = cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = night ? colPoint->m_nLightingB.night : colPoint->m_nLightingB.day;
}
CLEO_Fn(GET_COLPOINT_COORDINATES)
{
    CColPoint *colPoint = (CColPoint*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecPoint.x;
    cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecPoint.y;
    cleo->GetPointerToScriptVar(handle)->f = colPoint->m_vecPoint.z;
}