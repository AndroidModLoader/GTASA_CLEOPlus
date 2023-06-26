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
        int spriteIdForDisplay = (spriteId != -1) ? spriteId : RADAR_SPRITE_PROPERTYG;
        int spriteIdForLegend = (spriteId != -1) ? spriteId : RADAR_SPRITE_NONE;
        if(gMobileMenu->m_bDrawMenuMap)
        {
            
        }
        else
        {
            
        }

        float width = (blipScale * RsGlobal->maximumWidth) / 64.0f;
        float height = (blipScale * RsGlobal->maximumHeight) / 44.80f;

        logger->Info("In Draw 1 %f %f", width, height);
        if(DisplayThisBlip(spriteIdForDisplay, -99))
        {
            logger->Info("In Draw 2 %f %f", x, y);
            CRect rect;
            rect.left = x - width;
            rect.top = y - height;
            rect.right = width + x;
            rect.bottom = height + y;
            DrawSprite(sprite, rect, color);
            if (spriteIdForLegend != RADAR_SPRITE_NONE) AddBlipToLegendList(0, spriteIdForLegend);
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

uintptr_t DrawBlips_BackTo;
extern "C" void DrawBlips_Patch()
{
    if(blips.size() <= 0) return;

    for (CRadarBlipCLEO *blip : blips)
    {
        if (blip->entity)
        {
            CVector entityPos = blip->entity->GetPosition();
            blip->worldPos.x = entityPos.x;
            blip->worldPos.y = entityPos.y;
        }
        CVector2D radarPoint;
        CVector2D screenPos;
        TransformRealWorldPointToRadarSpace(radarPoint, blip->worldPos);
        float distance = LimitRadarPoint(radarPoint);
        if (!blip->shortDistance || distance <= 1.0f || gMobileMenu->m_bDrawMenuMap)
        {
            if (blip->HasBeenRevealed())
            {
                TransformRadarPointToScreenSpace(screenPos, radarPoint);
                blip->Draw(screenPos.x, screenPos.y);
            }
        }
    }
}
__attribute__((optnone)) __attribute__((naked)) void DrawBlips_Inject(void)
{
    asm volatile(
        "PUSH {R0-R11}\n"
        "BL DrawBlips_Patch\n");
    asm volatile(
        "MOV R12, %0\n"
        "POP {R0-R11}\n"
    :: "r" (DrawBlips_BackTo));
    asm volatile(
        "MOV R0, %0\n"
    :: "r" (gMobileMenu->m_bDrawMenuMap));
    asm volatile(
        "BX R12\n");
}
void RadarBlip_Patch()
{
    DrawBlips_BackTo = pGTASA + 0x43EA1E + 0x1;
    aml->Redirect(pGTASA + 0x43EA12 + 0x1, (uintptr_t)DrawBlips_Inject);
}

CLEO_Fn(ADD_CLEO_BLIP)
{
    int spriteId = cleo->ReadParam(handle)->i;
    unsigned int spriteIdUnsigned = (unsigned int)spriteId;
    CSprite2d *sprite = new CSprite2d();
    if (spriteId < 0 && spriteId > -10000)
    {
        sprite->m_pTexture = ScriptSprites[-spriteId].m_pTexture;
        spriteId = -1;
    }
    else
    {
        if (spriteIdUnsigned > 0x00010000)
        {
            sprite->m_pTexture = (RwTexture*)spriteIdUnsigned;
            spriteId = -1;
        }
        else
        {
            sprite->m_pTexture = RadarBlipSprites[spriteIdUnsigned].m_pTexture;
        }
    }
    float x = cleo->ReadParam(handle)->f;
    float y = cleo->ReadParam(handle)->f;
    int shortDistance = cleo->ReadParam(handle)->i;
    int r = cleo->ReadParam(handle)->i;
    int g = cleo->ReadParam(handle)->i;
    int b = cleo->ReadParam(handle)->i;
    int a = cleo->ReadParam(handle)->i;
    CRadarBlipCLEO *blip = new CRadarBlipCLEO(spriteId, sprite, x, y, shortDistance, r, g, b, a);
    blips.push_back(blip);
    cleo->GetPointerToScriptVar(handle)->i = (int)blip;
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