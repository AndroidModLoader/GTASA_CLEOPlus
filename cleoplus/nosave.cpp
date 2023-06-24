#include "externs.h"

CLEO_Fn(CREATE_OBJECT_NO_SAVE)
{
    int mi = cleo->ReadParam(handle)->i;
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    int offset = cleo->ReadParam(handle)->i;
    int fixGround = cleo->ReadParam(handle)->i;

    if (mi < 0) mi = UsedObjectArray[-mi].nModelIndex;

    CBaseModelInfo* pModel = ms_modelInfoPtrs[mi];
    pModel->m_nAlpha = -1;

    CObject* object = CreateObject(mi);
    object->m_nObjectType = eObjectType::OBJECT_MISSION2;

    if (z <= -100.0) z = FindGroundZForCoord(x, y);
    else if (fixGround)
    {
        bool foundGround;
        CEntity *foundEntity;
        float foundZ = FindGroundZFor3DCoord(x, y, z, &foundGround, &foundEntity);
        if(foundGround) z = foundZ;
    }

    if (offset) z -= pModel->m_pColModel->m_boxBound.m_vecMin.z;
    object->Teleport(x, y, z);

    if(pModel->AsLodAtomicModelInfoPtr() != NULL)
    {
        // SetupBigBuilding()
        object->m_bUsesCollision = false;
        object->m_bIsBIGBuilding = true;
        object->m_bStreamingDontDelete = true;
        pModel->bDoWeOwnTheColModel = true;
    }

    CVector vecPos{x, y, z};
    ClearSpaceForMissionEntity(vecPos, object);
    AddToWorld(object);

    cleo->GetPointerToScriptVar(handle)->i = GetObjectRef(object);
}
CLEO_Fn(SET_CAR_GENERATOR_NO_SAVE)
{
    int ref = cleo->ReadParam(handle)->i;
    CarGeneratorArray[ref].m_level = (char)0xFF;
}