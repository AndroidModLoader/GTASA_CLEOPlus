#include "externs.h"

inline void FixAspectRatio(float *x, float *y)
{
    float resX = (float)RsGlobal->maximumWidth;
    float resY = (float)RsGlobal->maximumHeight;
    resY *= 1.33333333f;
    resX /= resY;
    
    *x /= resX;
    *y /= 1.07142857f;
}

CLEO_Fn(GET_CURRENT_RESOLUTION)
{
    cleo->GetPointerToScriptVar(handle)->i = RsGlobal->maximumWidth;
    cleo->GetPointerToScriptVar(handle)->i = RsGlobal->maximumHeight;
}
CLEO_Fn(GET_FIXED_XY_ASPECT_RATIO)
{
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    FixAspectRatio(&x, &y);
    cleo->GetPointerToScriptVar(handle)->f = x;
    cleo->GetPointerToScriptVar(handle)->f = y;
}
CLEO_Fn(CONVERT_3D_TO_SCREEN_2D)
{
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float z = cleo->ReadParam(handle)->f;
    bool checkNearClip = cleo->ReadParam(handle)->i != 0;
    bool checkFarClip = cleo->ReadParam(handle)->i != 0;

    RwV3d pos3D{ x, y, z },screenPos2D;
    float sizeX, sizeY;

    bool result = CalcScreenCoors(pos3D, &screenPos2D, &sizeX, &sizeY, checkFarClip, checkNearClip);
    x = (screenPos2D.x / RsGlobal->maximumWidth) * 640.0f;
    y = (screenPos2D.y / RsGlobal->maximumHeight) * 448.0f;
    sizeX = (sizeX / RsGlobal->maximumWidth) * 8.0f;
    sizeY = (sizeY / RsGlobal->maximumHeight) * 8.0f;

    cleo->GetPointerToScriptVar(handle)->f = x;
    cleo->GetPointerToScriptVar(handle)->f = y;
    cleo->GetPointerToScriptVar(handle)->f = sizeX;
    cleo->GetPointerToScriptVar(handle)->f = sizeY;

    UpdateCompareFlag((CRunningScript*)handle, result);
}
CLEO_Fn(IS_RADAR_VISIBLE) // Pretty inacurate tho...
{
    UpdateCompareFlag((CRunningScript*)handle, 
        *gbCineyCamProcessedOnFrame != *m_FrameCounter && !*ms_bTakePhoto && (!gMobileMenu->m_nScreensCount || !gMobileMenu->m_pTopScreen) && *ms_exitEnterState > 2 && (*m_ItemToFlash != 8 || *m_snTimeInMilliseconds % 350 > 150)
    );
}
CLEO_Fn(IS_HUD_VISIBLE)
{
    UpdateCompareFlag((CRunningScript*)handle, 
        *bDisplayHud && *gbCineyCamProcessedOnFrame != *m_FrameCounter && !*ms_bTakePhoto && (!gMobileMenu->m_nScreensCount || !gMobileMenu->m_pTopScreen)
    );
}
CLEO_Fn(GET_FADE_ALPHA)
{
    cleo->GetPointerToScriptVar(handle)->f = TheCamera->m_fFloatingFade;
    UpdateCompareFlag((CRunningScript*)handle, TheCamera->m_fFloatingFade > 0);
}