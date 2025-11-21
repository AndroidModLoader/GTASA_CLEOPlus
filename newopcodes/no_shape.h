#pragma once

#include "externs.h"

#define MAX_NO_SHAPES 100
#define MAX_NO_VERTICES 20

struct NOShape
{
    NOShape() { memset(this, 0, sizeof(*this)); }

    RwTexture* m_pTexture;
    RwOpenGLVertex m_Vertices[MAX_NO_VERTICES];
    uint8_t m_nPrimType, m_nNumVertices, m_nAlphaEnable, m_nBlendSrc, m_nBlendDst;

    inline void Draw()
    {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, m_pTexture ? m_pTexture->raster : NULL);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)(int)m_nAlphaEnable);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)(int)m_nBlendSrc);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)(int)m_nBlendDst);
        RwIm2DRenderPrimitive(m_nPrimType, m_Vertices, m_nNumVertices);
    }
};

struct ShapeDrawer
{
    static inline NOShape m_Shapes[MAX_NO_SHAPES];
    static inline uint32_t m_nShapes = 0;
    static inline RwOpenGLVertex m_aStdVerts[MAX_NO_VERTICES];

    static inline bool DrawShapeThisFrame(uint8_t primType, uint8_t numVertices, uint8_t alphaEnable, uint8_t blendSrc, uint8_t blendDst, RwTexture *texture, RwOpenGLVertex *vertices)
    {
        if(m_nShapes >= MAX_NO_SHAPES) return false;

        NOShape& shape = m_Shapes[m_nShapes];
        shape.m_pTexture = texture;
        shape.m_nPrimType = primType;
        shape.m_nNumVertices = ((numVertices > MAX_NO_VERTICES) ? MAX_NO_VERTICES : numVertices);
        memcpy(shape.m_Vertices, vertices, shape.m_nNumVertices * sizeof(RwOpenGLVertex));
        shape.m_nAlphaEnable = alphaEnable;
        shape.m_nBlendSrc = blendSrc;

        ++m_nShapes;
        return true;
    }
    static inline void DrawAll()
    {
        for(int i = 0; i < m_nShapes; ++i) { m_Shapes[i].Draw(); }
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)true);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
        m_nShapes = 0;
    }
    static inline void SetupVertex(RwOpenGLVertex *vertex, RwV3d posn, float u, float v, float rhw, RwRGBA color)
    {
        vertex->pos = posn;
        vertex->texCoord.u = u;
        vertex->texCoord.v = v;
        vertex->rhw = rhw;
        vertex->rgba = color;
    }
    static inline void SetupVertex(RwOpenGLVertex *vertex, RwV3d posn, float u, float v, float rhw, CRGBA color)
    {
        vertex->pos = posn;
        vertex->texCoord.u = u;
        vertex->texCoord.v = v;
        vertex->rhw = rhw;
        vertex->rgba = RwRGBA(color.r, color.g, color.b, color.a);
    }
};