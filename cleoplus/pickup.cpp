#include "externs.h"

CLEO_Fn(GET_PICKUP_THIS_COORD)
{
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    bool onlyAvailable = cleo->ReadParam(handle)->i;

    CPickup *pickup = nullptr;
    int pickupRefResult = 0;
    bool result = false;

    CVector coord = {x, y, z};

    for (unsigned int i = 0; i < 620; ++i) // MAX_NUM_PICKUPS is hardcoded now!
    {
        CPickup *pickup = &aPickUps[i];
        CVector pickupCoord = pickup->GetPosition();

        if (DistanceBetweenPoints(pickupCoord, coord) < 0.5f)
        {
            if (!onlyAvailable || pickup->m_nPickupType)
            {
                pickupRefResult = (i | (pickup->m_nReferenceIndex << 16));
                result = true;
                break;
            }
        }
    }
    cleo->GetPointerToScriptVar(handle)->i = pickupRefResult;
    cleoaddon->UpdateCompareFlag(handle, result);
}
CLEO_Fn(GET_PICKUP_MODEL)
{
    int pickupIndex = cleo->ReadParam(handle)->i;
    int actualIndex = GetActualPickupIndex(pickupIndex);
    int model = -1;
    bool result = false;
    if (actualIndex != -1)
    {
        CPickup *pickup = &aPickUps[actualIndex];
        model = pickup->m_nModelIndex;
        result = true;
    }
    cleo->GetPointerToScriptVar(handle)->i = model;
    cleoaddon->UpdateCompareFlag(handle, result);
}
CLEO_Fn(GET_PICKUP_POINTER)
{
    int pickupIndex = cleo->ReadParam(handle)->i;
    int actualIndex = GetActualPickupIndex(pickupIndex);
    CPickup *pickup = NULL;
    bool result = false;
    if (actualIndex != -1)
    {
        pickup = &aPickUps[actualIndex];
        result = true;
    }
    cleo->GetPointerToScriptVar(handle)->i = (int)pickup;
    cleoaddon->UpdateCompareFlag(handle, result);
}
CLEO_Fn(GET_PICKUP_TYPE)
{
    int pickupIndex = cleo->ReadParam(handle)->i;
    int actualIndex = GetActualPickupIndex(pickupIndex);
    int type = -1;
    bool result = false;
    if (actualIndex != -1)
    {
        CPickup *pickup = &aPickUps[actualIndex];
        type = pickup->m_nPickupType;
        result = true;
    }
    cleo->GetPointerToScriptVar(handle)->i = type;
    cleoaddon->UpdateCompareFlag(handle, result);
}