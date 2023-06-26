#include "externs.h"
#include <vector>

class CRadarBlipCLEO;
std::vector<CRadarBlipCLEO*> blips;

class CRadarBlipCLEO
{
public:
    CEntity *entity;
    CVector2D worldPos;
    CRGBA color;
    CSprite2d *sprite;
    int spriteId;
    float angle;
    bool shortDistance;
    CRadarBlipCLEO(int _spriteId, CSprite2d *_sprite, float x, float y, bool _shortDistance, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        entity = nullptr;
        spriteId = _spriteId;
        sprite = _sprite;
        worldPos.x = x;
        worldPos.y = y;
        shortDistance = _shortDistance;
        color.r = r;
        color.g = g;
        color.b = b;
        color.a = a;
        angle = 0.0f;
    }
    void Draw(float x, float y)
    {
        float blipScale = *(float*)(pGTASA + 0x43F990);
        if(gMobileMenu->m_bDrawMenuMap)
        {

        }
        else
        {
            
        }
    }
    inline bool HasBeenRevealed()
    {
        return (!gMobileMenu->m_bDrawMenuMap || *ZonesRevealed > 80 || GetCurrentZoneLockedOrUnlocked(worldPos.x, worldPos.y));
    }

    static void Delete()
    {
        for (CRadarBlipCLEO* blip : blips) delete blip;
        blips.clear();
    }
};


void RadarBlip_Patch()
{
    //aml->Redirect();
}

CLEO_Fn(ADD_CLEO_BLIP)
{

}
CLEO_Fn(REMOVE_CLEO_BLIP)
{
    CRadarBlipCLEO *blip = (CRadarBlipCLEO *)cleo->ReadParam(handle)->i;
    for (std::vector<CRadarBlipCLEO*>::iterator iter = blips.begin(); iter != blips.end(); ++iter)
    {
        if (*iter == blip)
        {
            delete blip;
            blips.erase(iter);
            break;
        }
    }
}