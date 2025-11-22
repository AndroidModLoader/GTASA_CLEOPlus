#pragma once

#include <externs.h>

enum DrawEvent { BeforeDrawing, AfterDrawing, BeforeHud, AfterHud, BeforeRadar, AfterRadar, BeforeRadarOverlay, AfterRadarOverlay, BeforeBlips, AfterBlips, AfterFade, TOTAL_DRAW_EVENT };

struct CLEOTexture;
extern std::vector<CLEOTexture*> m_aCLEOTexs[TOTAL_DRAW_EVENT];

struct CLEOTexture
{
    static inline float m_fTexCoords[8] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };
    static inline float m_fVertices[8];

    RwTexture* m_pTexture = NULL;
    CVector2D m_vecCornerA, m_vecCornerB;
    float m_fAngle = 0, m_fDepth = 0;
    CRGBA m_Color;
    int m_nMaskTrisCount = 0;
    float* m_pMaskTris = NULL;

    static inline CLEOTexture* New(int ev)
    {
        if(ev < BeforeDrawing || ev >= TOTAL_DRAW_EVENT)
        {
            return NULL;
        }
        CLEOTexture* ret = new CLEOTexture;
        m_aCLEOTexs[ev].push_back(ret);
        return ret;
    }

    ~CLEOTexture()
    {
        if(m_pMaskTris) delete[] m_pMaskTris;
    }
    inline void Draw()
    {
        RwRenderStateSet(rwRENDERSTATEFOGENABLE, (void*)false);
        RwRenderStateSet(rwRENDERSTATESRCBLEND, (void*)rwBLENDSRCALPHA);
        RwRenderStateSet(rwRENDERSTATEDESTBLEND, (void*)rwBLENDINVSRCALPHA);
        RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, (void*)rwFILTERLINEAR);
        RwRenderStateSet(rwRENDERSTATESHADEMODE, (void*)rwSHADEMODEFLAT);
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)true);
        RwRenderStateSet(rwRENDERSTATETEXTURERASTER, m_pTexture ? m_pTexture->raster : NULL);
        if(m_nMaskTrisCount > 0)
        {
            if(m_pMaskTris)
            {
                RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)false);
                RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)false);
                RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
                RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONALWAYS); // not workin on mobile btw

                SetMaskVertices(m_nMaskTrisCount, m_pMaskTris, *NearScreenZ + m_fDepth);
                RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, maVertices, m_nMaskTrisCount);
            }
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)false);
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSCLAMP);
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, (void*)rwALPHATESTFUNCTIONGREATER); // not workin on mobile btw
        }
        else
        {
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSMIRROR); // might be typo?
        }
        RwRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE, (void*)false);

        float v8 = (m_vecCornerB.x + m_vecCornerA.x) * 0.5f;
        float v9 = (m_vecCornerB.y + m_vecCornerA.y) * 0.5f;
        float v10 = v8 - m_vecCornerA.x;
        float v11 = v9 - m_vecCornerA.y;
        float v12 = cosf(m_fAngle);
        float v13 = sinf(m_fAngle);
        float v14 = v12 * v11;
        float v15 = v13 * v10;
        float v16 = v12 * v10;
        float v17 = v13 * v11;
        float v18 = -v16;

        m_fVertices[7] = v15 + v14 + v9;
        m_fVertices[6] = v16 - v17 + v8;
        m_fVertices[5] = v14 - v15 + v9;
        m_fVertices[4] = v18 - v17 + v8;
        m_fVertices[3] = v15 - v14 + v9;
        m_fVertices[2] = v17 + v16 + v8;
        m_fVertices[1] = -v15 - v14 + v9;
        m_fVertices[0] = v18 + v17 + v8;

        if(m_nMaskTrisCount > 0)
        {
            float bak = *NearScreenZ;
            *NearScreenZ += m_fDepth;
            SetVertices4(4, m_fVertices, m_fTexCoords, m_Color);
            *NearScreenZ = bak;
            
            RwIm2DRenderPrimitive(rwPRIMTYPETRISTRIP, maVertices, 4);
        }
        else
        {
            SetVertices8(m_fVertices[0], m_fVertices[1], m_fVertices[2], m_fVertices[3],
                         m_fVertices[4], m_fVertices[5], m_fVertices[6], m_fVertices[7],
                         m_Color, m_Color, m_Color, m_Color);
            RwIm2DRenderPrimitive(rwPRIMTYPETRIFAN, maVertices, 4);
        }
    }

    static inline void Clear(int ev)
    {
        if(ev >= BeforeDrawing && ev < TOTAL_DRAW_EVENT)
        {
            for(CLEOTexture* tex : m_aCLEOTexs[ev])
            {
                delete tex;
            }
            m_aCLEOTexs[ev].clear();
        }
    }
    static inline void ClearAll()
    {
        for(int i = 0; i < TOTAL_DRAW_EVENT; ++i)
        {
            Clear(i);
        }
    }
    static inline void DrawAll(int ev)
    {
        if(ev >= BeforeDrawing && ev < TOTAL_DRAW_EVENT)
        {
            for(CLEOTexture* tex : m_aCLEOTexs[ev])
            {
                tex->Draw();
            }
            Clear(ev);
        }
    }
};