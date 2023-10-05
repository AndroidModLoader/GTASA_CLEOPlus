#include "externs.h"

CLEO_Fn(GET_CHAR_BONE_MATRIX)
{
    CPed* ped = (*ms_pPedPool)->GetAtRef(cleo->ReadParam(handle)->i);
    int bone = cleo->ReadParam(handle)->i;

    if(!ped->m_pRwObject)
    {
        cleo->GetPointerToScriptVar(handle)->i = 0;
        UpdateCompareFlag((CRunningScript*)handle, false);
        return;
    }

    RwMatrix* matrix = NULL;
    UpdateRpHAnim(ped);
    RpHAnimHierarchy* hierarchy = GetAnimHierarchyFromSkinClump(ped->m_pRwClump);
    if (!hierarchy)
    {
        matrix = (RwMatrix*)ped->GetMatrix();
    }
    else
    {
        matrix = &hierarchy->pMatrixArray[RpHAnimIDGetIndex(hierarchy, bone)];
    }

    cleo->GetPointerToScriptVar(handle)->i = (int)matrix;
    UpdateCompareFlag((CRunningScript*)handle, matrix != NULL);
}
CLEO_Fn(GET_CHAR_BONE)
{
    CPed* ped = (*ms_pPedPool)->GetAtRef(cleo->ReadParam(handle)->i);
    int boneId = cleo->ReadParam(handle)->i;

    auto bone = &RpClumpGetAnimBlendClumpData(ped->m_pRwClump)->m_pFrameDataArray[boneId];
    cleo->GetPointerToScriptVar(handle)->i = (int)bone;
    UpdateCompareFlag((CRunningScript*)handle, bone != NULL);
}
CLEO_Fn(GET_BONE_OFFSET_VECTOR)
{
    AnimBlendFrameData* bone = (AnimBlendFrameData*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = (int)(&bone->bonePosition);
}
CLEO_Fn(GET_BONE_QUAT)
{
    AnimBlendFrameData* bone = (AnimBlendFrameData*)cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = (int)(&bone->pStdKeyFrame->q);
}