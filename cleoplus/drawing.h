#include <externs.h>

enum DrawEvent { BeforeDrawing, AfterDrawing, BeforeHud, AfterHud, BeforeRadar, AfterRadar, BeforeRadarOverlay, AfterRadarOverlay, BeforeBlips, AfterBlips, AfterFade, TOTAL_DRAW_EVENT };

struct CLEOTexture;
extern std::vector<CLEOTexture*> m_aCLEOTexs[TOTAL_DRAW_EVENT];

struct CLEOTexture
{
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
};