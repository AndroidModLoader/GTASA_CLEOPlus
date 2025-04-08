#include "drawing.h"

std::vector<CLEOTexture*> m_aCLEOTexs[TOTAL_DRAW_EVENT];

void ClearAllCLEOTextures()
{
    CLEOTexture::ClearAll();
}

CLEO_Fn(DRAW_TEXTURE_PLUS)
{
    RwTexture* tex = (RwTexture*)cleo->ReadParam(handle)->i;
    int ev = cleo->ReadParam(handle)->i;
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    float sx = cleo->ReadParam(handle)->f;
    float sy = cleo->ReadParam(handle)->f;
    float angle = cleo->ReadParam(handle)->f;
    float depth = cleo->ReadParam(handle)->f;
    bool arFix = cleo->ReadParam(handle)->i;
    int trisCount = cleo->ReadParam(handle)->i;
    float* trisArr = (float*)cleo->ReadParam(handle)->i;
    int r = cleo->ReadParam(handle)->i;
    int g = cleo->ReadParam(handle)->i;
    int b = cleo->ReadParam(handle)->i;
    int a = cleo->ReadParam(handle)->i;

    if(arFix)
    {
        FixAspectRatio(&sx, &sy);
    }

    float cx = (float)RsGlobal->maximumWidth / 640.0f;
    float cy = (float)RsGlobal->maximumHeight / 448.0f;

    CLEOTexture* newTex = CLEOTexture::New(ev);
    if(!tex)
    {
        cleoaddon->UpdateCompareFlag(handle, false);
        return;
    }

    newTex->m_pTexture = tex;
    newTex->m_vecCornerA.x = cx * (x - 0.5f * sx);
    newTex->m_vecCornerA.y = cy * (y - 0.5f * sy);
    newTex->m_vecCornerB.x = cx * (x + 0.5f * sx);
    newTex->m_vecCornerB.y = cy * (y + 0.5f * sy);
    newTex->m_fDepth = depth;
    newTex->m_Color = CRGBA(r, g, b, a);
    newTex->m_nMaskTrisCount = trisCount;

    float* maskTris = trisArr;
    if(trisCount > 0 && trisArr)
    {
        trisCount *= 2;
        maskTris = new float[trisCount];

        int i = -1;
        while(i < trisCount)
        {
            maskTris[i] = cx * trisArr[i];
            ++i;
            
            maskTris[i] = cy * trisArr[i];
            ++i;
        }
    }

    if(trisCount <= 0)
    {
        angle += 180.0f;
        while(angle > 360) angle -= 360.0f;
    }

    newTex->m_pMaskTris = maskTris;
    newTex->m_fAngle = (angle / 180.0f) * M_PI;
    cleoaddon->UpdateCompareFlag(handle, true);
}
CLEO_Fn(GET_TEXTURE_FROM_SPRITE)
{
    int id = cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = (int)( cleoaddon->GetScriptTextureByID(handle, id) );
}
CLEO_Fn(DRAW_STRING)
{
    
}
CLEO_Fn(DRAW_STRING_EXT)
{
    
}