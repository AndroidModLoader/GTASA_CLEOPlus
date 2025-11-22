#pragma once

#include "externs.h"

#define MAX_NO_SHAPES 100
#define MAX_NO_VERTICES 20
#define MAX_NO_SPRITES 200

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

struct NOSprite
{
    NOSprite() { memset(this, 0, sizeof(*this)); }

    RwTexture* m_pTexture;
    CRect m_Rect;
    float m_fAngle;
    CRGBA m_Color[4];

    inline void RotateVerts(CVector2D *rect, unsigned int numVerts, float x, float y, float angle)
    {
        float xold, yold;
        float _cos = cosf(angle);
        float _sin = sinf(angle);
        for(int i = 0; i < numVerts; ++i)
        {
            xold = rect[i].x;
            yold = rect[i].y;
            rect[i].x = x + (xold - x) * _cos + (yold - y) * _sin;
            rect[i].y = y - (xold - x) * _sin + (yold - y) * _cos;
        }
    }

    inline void Draw()
    {
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, m_pTexture ? m_pTexture->raster : NULL);
        if(m_fAngle != 0.0f)
        {
            CVector2D verts[4];

            verts[0].x = m_Rect.left; verts[0].y = m_Rect.bottom;
            verts[1].x = m_Rect.right; verts[1].y = m_Rect.bottom;
            verts[2].x = m_Rect.left; verts[2].y = m_Rect.top;
            verts[3].x = m_Rect.right; verts[3].y = m_Rect.top;
            RotateVerts(verts, 4, m_Rect.left + 0.5f * (m_Rect.right - m_Rect.left), m_Rect.top + 
                        0.5f * (m_Rect.bottom - m_Rect.top), m_fAngle);
            DrawAnyRect(m_Rect.left, m_Rect.bottom, m_Rect.right, m_Rect.top, m_Rect.left, m_Rect.top,
                        m_Rect.right, m_Rect.top, m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
        }
        else
        {
            DrawAnyRect(m_Rect.left, m_Rect.bottom, m_Rect.right, m_Rect.top, m_Rect.left, m_Rect.top,
                        m_Rect.right, m_Rect.top, m_Color[0], m_Color[1], m_Color[2], m_Color[3]);
        }
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

struct SpriteDrawer
{
    static inline NOSprite m_Sprites[MAX_NO_SPRITES];
    static inline uint32_t m_nSprites = 0;

    static inline bool DrawSpriteThisFrame(RwTexture *texture, CRect& rect, CRGBA& color1, CRGBA& color2, CRGBA& color3, CRGBA& color4, float angle)
    {
        if(m_nSprites >= MAX_NO_SPRITES) return false;

        NOSprite& sprite = m_Sprites[m_nSprites];
        sprite.m_pTexture = texture;
        sprite.m_Rect = rect;
        sprite.m_Color[0] = color1;
        sprite.m_Color[1] = color2;
        sprite.m_Color[2] = color3;
        sprite.m_Color[3] = color4;
        sprite.m_fAngle = angle;

        ++m_nSprites;
        return true;
    }
    static inline void DrawAll()
    {
        for(int i = 0; i < m_nSprites; ++i) { m_Sprites[i].Draw(); }
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, NULL);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)true);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
        m_nSprites = 0;
    }
};