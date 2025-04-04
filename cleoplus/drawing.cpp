#include "externs.h"

CLEO_Fn(DRAW_TEXTURE_PLUS)
{
    
}
CLEO_Fn(GET_TEXTURE_FROM_SPRITE)
{
    int id = cleo->ReadParam(handle)->i;
    cleo->GetPointerToScriptVar(handle)->i = (int)( ScriptSprites[id].m_pTexture ); // TODO: GetScriptTextureById
}
CLEO_Fn(DRAW_STRING)
{
    
}
CLEO_Fn(DRAW_STRING_EXT)
{
    
}