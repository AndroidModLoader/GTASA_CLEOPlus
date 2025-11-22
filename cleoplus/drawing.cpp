#include "drawing.h"

#include <newopcodes/no_shape.h>

std::vector<CLEOTexture*> m_aCLEOTexs[TOTAL_DRAW_EVENT];

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
CLEO_Fn(DRAW_SHAPE) // newOpcodes
{
    unsigned int type, numVerts, vertexAlpha, srcBlend, dstBlend, unused;
    RwTexture* texture;
    RwOpenGLVertex* pVerts;

    type = cleo->ReadParam(handle)->u;
    texture = (RwTexture*)cleo->ReadParam(handle)->u;
    numVerts = cleo->ReadParam(handle)->u;
    pVerts = (RwOpenGLVertex*)cleo->ReadParam(handle)->u;
    vertexAlpha = cleo->ReadParam(handle)->u;
    srcBlend = cleo->ReadParam(handle)->u;
    dstBlend = cleo->ReadParam(handle)->u;
    unused = cleo->ReadParam(handle)->u;

    // MOBILE ADDITION (FIX???)
    if(( uint32_t)pVerts <= 1)
    {
        pVerts = ShapeDrawer::m_aStdVerts;
    }
    // MOBILE ADDITION END (FIX???)

    cleoaddon->UpdateCompareFlag(handle, ShapeDrawer::DrawShapeThisFrame(type, numVerts, vertexAlpha, srcBlend, dstBlend, texture, pVerts));
}
CLEO_Fn(SETUP_SHAPE_VERTEX) // newOpcodes
{
    RwV3d posn;
    RwRGBA color;
    float rhw, u, v;
    unsigned int vertex, invX, invY;

    RwOpenGLVertex* pVerts = (RwOpenGLVertex*)cleo->ReadParam(handle)->u;
    vertex = cleo->ReadParam(handle)->u;
    posn.x = cleo->ReadParam(handle)->f;
    posn.y = cleo->ReadParam(handle)->f;
    posn.z = cleo->ReadParam(handle)->f;
    rhw = cleo->ReadParam(handle)->f;
    color.red = cleo->ReadParam(handle)->u;
    color.green = cleo->ReadParam(handle)->u;
    color.blue = cleo->ReadParam(handle)->u;
    color.alpha = cleo->ReadParam(handle)->u;
    u = cleo->ReadParam(handle)->f;
    v = cleo->ReadParam(handle)->f;
    invX = cleo->ReadParam(handle)->u;
    invY = cleo->ReadParam(handle)->u;

    if(invX)
    {
        u = 1.0f - u;
        posn.x = RsGlobal->maximumWidth - posn.x;
    }
    if(invY)
    {
        v = 1.0f - v;
        posn.y = RsGlobal->maximumHeight - posn.y;
    }

    if((uint32_t)pVerts <= 1)
    {
        ShapeDrawer::SetupVertex(&ShapeDrawer::m_aStdVerts[vertex-1], posn, u, v, rhw, color);
    }
    else
    {
        ShapeDrawer::SetupVertex(&pVerts[vertex-1], posn, u, v, rhw, color);
    }
}
CLEO_Fn(ROTATE_SHAPE_VERTICES) // newOpcodes
{
    float x, y, xc, yc, a, _sin, _cos;
    unsigned int vertices, numVerts;

    vertices = cleo->ReadParam(handle)->u;
    numVerts = cleo->ReadParam(handle)->u;
    xc = cleo->ReadParam(handle)->f;
    yc = cleo->ReadParam(handle)->f;
    a = (M_PI / 180.0f) * cleo->ReadParam(handle)->f;

    RwOpenGLVertex* pVerts = ((vertices <= 1) ? ShapeDrawer::m_aStdVerts : (RwOpenGLVertex*)vertices);

    _sin = sinf(a);
    _cos = cosf(a);

    if(numVerts > MAX_NO_VERTICES)
    {
        numVerts = MAX_NO_VERTICES;
    }
    while(numVerts)
    {
        float x = pVerts->pos.x;
        float y = pVerts->pos.y;

        pVerts->pos.x = xc + (x - xc) * _cos + (y - yc) * _sin;
        pVerts->pos.y = yc - (x - xc) * _sin + (y - yc) * _cos;

        --numVerts;
        ++pVerts;
    }
}