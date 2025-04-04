#include "externs.h"

CSpecialModel* CSpecialModel::New(const char* dff, const char* txd)
{
    CSpecialModel* smdl = NULL;

    // TXD part is simplified
    RwStream* stream = RwStreamOpen(2, 1, (void*)dff);
    if(stream)
    {
        if(RwStreamFindChunk(stream, 0x10, NULL, NULL))
        {
            RpClump* clump = RpClumpStreamRead(stream);
            if(clump)
            {
                RpAtomic* atomic = GetFirstAtomic(clump);
                if(atomic)
                {
                    smdl = new CSpecialModel();
                    smdl->m_pClump = clump;
                    smdl->m_pAtomic = atomic;
                }
            }
        }
        RwStreamClose(stream, NULL);
    }
    // TXD part is simplified

    return smdl;
}
void CSpecialModel::Destroy()
{
    if(--m_nRefs <= 0)
    {
        RpAtomicDestroy(m_pAtomic);
        RpClumpDestroy(m_pClump);
        if(m_nTxdIdx != -1) TxdStoreRemoveRef(m_nTxdIdx);
        delete this;
    }
}

CLEO_Fn(LOAD_SPECIAL_MODEL)
{
    char dff[0xFF], txd[0xFF];
    cleoaddon->ReadString(handle, dff, sizeof(dff));
    cleoaddon->ReadString(handle, txd, sizeof(txd));

    snprintf(dff, sizeof(dff), "%s.dff", dff);
    snprintf(txd, sizeof(txd), "%s.txd", txd);

    CSpecialModel* smdl = CSpecialModel::New(dff, txd);
    cleo->GetPointerToScriptVar(handle)->i = (int)smdl;
    cleoaddon->UpdateCompareFlag(handle, smdl != NULL);
}
CLEO_Fn(REMOVE_SPECIAL_MODEL)
{
    CSpecialModel* smdl = (CSpecialModel*)cleo->ReadParam(handle)->i;
    if(smdl) smdl->Destroy();
}
CLEO_Fn(GET_SPECIAL_MODEL_DATA)
{
    CSpecialModel* smdl = (CSpecialModel*)cleo->ReadParam(handle)->i;
    if(smdl)
    {
        cleo->GetPointerToScriptVar(handle)->i = (int)smdl->m_pClump;
        cleo->GetPointerToScriptVar(handle)->i = (int)smdl->m_pAtomic;
        cleo->GetPointerToScriptVar(handle)->i = smdl->m_nTxdIdx;
    }
    else
    {
        cleo->GetPointerToScriptVar(handle)->i = 0;
        cleo->GetPointerToScriptVar(handle)->i = 0;
        cleo->GetPointerToScriptVar(handle)->i = -1;
    }
}