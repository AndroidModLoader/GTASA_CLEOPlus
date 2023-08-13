#include <mod/amlmod.h>
#include "externs.h"

std::set<int> SpecialCharacterModelsUsed;

// Vars
CCamera* TheCamera;
tUsedObject* UsedObjectArray;
CBaseModelInfo** ms_modelInfoPtrs;
CCarGenerator* CarGeneratorArray;
uint16_t *OldWeatherType, *NewWeatherType, *ForcedWeatherType, *m_ItemToFlash;
float *Rain, *UnderWaterness, *m_fDNBalanceParam;
uint8_t *ms_nGameClockHours, *ms_nGameClockMinutes, *ScriptSpace;
RsGlobalType* RsGlobal;
uint32_t *gbCineyCamProcessedOnFrame, *m_FrameCounter, *ms_exitEnterState, *m_snTimeInMilliseconds, *ZonesRevealed;
bool *ms_bTakePhoto, *bDisplayHud, *ms_running, *m_aCheatsActive;
MobileMenu *gMobileMenu;
CPickup* aPickUps;
CEntity** pIgnoreEntity;
CSprite2d* RadarBlipSprites;
CSprite2d* ScriptSprites;
int32_t *OnAMissionFlag;
CPool<CPed, CCopPed> **ms_pPedPool;
CPool<CVehicle, CHeli> **ms_pVehiclePool;
CPool<CObject, CCutsceneObject> **ms_pObjectPool;
CScriptResourceManager *ScriptResourceManager;
CColModel *ms_colModelPed1;

// Funcs
CObject* (*CreateObject)(int);
float (*FindGroundZForCoord)(float, float);
float (*FindGroundZFor3DCoord)(float, float, float, bool*, CEntity**);
void (*ClearSpaceForMissionEntity)(CVector&, CEntity*);
void (*AddToWorld)(CEntity*);
int (*GetObjectRef)(CObject*);
int (*GetVehicleRef)(CVehicle*);
CVehicle* (*GetVehicleFromRef)(int);
CPed* (*GetPedFromRef)(int);
CObject* (*GetObjectFromRef)(int);
void (*UpdateCompareFlag)(CRunningScript*, uint8_t);
bool (*CalcScreenCoors)(RwV3d const&,RwV3d*,float *,float *,bool,bool);
int (*GetActualPickupIndex)(int);
bool (*ProcessLineOfSight)(CVector const&,CVector const&,CColPoint &,CEntity *&,bool,bool,bool,bool,bool,bool,bool,bool);
void (*QuaternionSet)(CQuaternion*, const RwMatrix& m);
void (*QuaternionMult)(const CQuaternion *a1, const CQuaternion *a2, const CQuaternion *a3);
void (*RtQuatRotate)(CQuaternion *, CVector const*, float, RwOpCombineType);
void (*RwMatrixRotate)(RwMatrix *, RwV3d const*, float, RwOpCombineType);
bool (*GetCurrentZoneLockedOrUnlocked)(float, float);
void (*TransformRealWorldPointToRadarSpace)(CVector2D &, CVector2D const&);
float (*LimitRadarPoint)(CVector2D &);
void (*TransformRadarPointToScreenSpace)(CVector2D &, CVector2D const&);
bool (*DisplayThisBlip)(int, char);
void (*AddBlipToLegendList)(uint8_t, int);
void (*DrawSprite)(CSprite2d*, CRect const&, CRGBA const&);
int (*GetUppercaseKey)(const char*);
CWeaponInfo* (*GetWeaponInfo)(int, char);
void (*SetClumpAlpha)(RpClump*, int);
float (*GetDistanceFromCentreOfMassToBaseOfModel)(CEntity*);
bool (*ProcessVerticalLine)(CVector const&,float,CColPoint &,CEntity *&,bool,bool,bool,bool,bool,bool,CStoredCollPoly *);
CPlayerPed* (*FindPlayerPed)(int);
CPlayerInfo* (*GetPlayerInfoForThisPlayerPed)(CPlayerPed*);
char (*GetWeaponSkill)(CPed*, eWeaponType);
void (*ObjectDamage)(CObject *,float,CVector *,CVector *,CEntity *,eWeaponType);
void (*AddToResourceManager)(CScriptResourceManager *,int,uint,CRunningScript *);
void (*RequestModel)(int,int);
void (*LoadAllRequestedModels)(bool);
void (*TimerSuspend)();
void (*TimerResume)();
CPedModelInfo* (*AddPedModel)(int id);
void (*SetColModel)(CBaseModelInfo *,CColModel *,bool);
void (*RequestSpecialModel)(int,char const*,int);

// int main
void ResolveExternals()
{
// Vars
    SET_TO(TheCamera, aml->GetSym(hGTASA, "TheCamera"));
    SET_TO(UsedObjectArray, aml->GetSym(hGTASA, "_ZN11CTheScripts15UsedObjectArrayE"));
    SET_TO(ms_modelInfoPtrs, aml->GetSym(hGTASA, "_ZN10CModelInfo16ms_modelInfoPtrsE"));
    SET_TO(CarGeneratorArray, aml->GetSym(hGTASA, "_ZN17CTheCarGenerators17CarGeneratorArrayE"));
    SET_TO(OldWeatherType, aml->GetSym(hGTASA, "_ZN8CWeather14OldWeatherTypeE"));
    SET_TO(NewWeatherType, aml->GetSym(hGTASA, "_ZN8CWeather14NewWeatherTypeE"));
    SET_TO(ForcedWeatherType, aml->GetSym(hGTASA, "_ZN8CWeather17ForcedWeatherTypeE"));
    SET_TO(m_ItemToFlash, aml->GetSym(hGTASA, "_ZN4CHud13m_ItemToFlashE"));
    SET_TO(Rain, aml->GetSym(hGTASA, "_ZN8CWeather4RainE"));
    SET_TO(UnderWaterness, aml->GetSym(hGTASA, "_ZN8CWeather14UnderWaternessE"));
    SET_TO(m_fDNBalanceParam, aml->GetSym(hGTASA, "_ZN25CCustomBuildingDNPipeline17m_fDNBalanceParamE"));
    SET_TO(ms_nGameClockHours, aml->GetSym(hGTASA, "_ZN6CClock18ms_nGameClockHoursE"));
    SET_TO(ms_nGameClockMinutes, aml->GetSym(hGTASA, "_ZN6CClock20ms_nGameClockMinutesE"));
    SET_TO(RsGlobal, aml->GetSym(hGTASA, "RsGlobal"));
    SET_TO(gbCineyCamProcessedOnFrame, aml->GetSym(hGTASA, "gbCineyCamProcessedOnFrame"));
    SET_TO(m_FrameCounter, aml->GetSym(hGTASA, "_ZN6CTimer14m_FrameCounterE"));
    SET_TO(ms_exitEnterState, aml->GetSym(hGTASA, "_ZN17CEntryExitManager17ms_exitEnterStateE"));
    SET_TO(m_snTimeInMilliseconds, aml->GetSym(hGTASA, "_ZN6CTimer22m_snTimeInMillisecondsE"));
    SET_TO(ms_bTakePhoto, aml->GetSym(hGTASA, "_ZN7CWeapon13ms_bTakePhotoE"));
    SET_TO(bDisplayHud, aml->GetSym(hGTASA, "_ZN11CTheScripts11bDisplayHudE"));
    SET_TO(gMobileMenu, aml->GetSym(hGTASA, "gMobileMenu"));
    SET_TO(aPickUps, *(uintptr_t*)(pGTASA + 0x678BF8));
    SET_TO(pIgnoreEntity, aml->GetSym(hGTASA, "_ZN6CWorld13pIgnoreEntityE"));
    SET_TO(ZonesRevealed, aml->GetSym(hGTASA, "_ZN9CTheZones13ZonesRevealedE"));
    SET_TO(RadarBlipSprites, aml->GetSym(hGTASA, "_ZN6CRadar16RadarBlipSpritesE"));
    SET_TO(ScriptSprites, aml->GetSym(hGTASA, "_ZN11CTheScripts13ScriptSpritesE"));
    SET_TO(OnAMissionFlag, aml->GetSym(hGTASA, "_ZN11CTheScripts14OnAMissionFlagE"));
    SET_TO(ScriptSpace, aml->GetSym(hGTASA, "_ZN11CTheScripts11ScriptSpaceE"));
    SET_TO(ms_running, aml->GetSym(hGTASA, "_ZN12CCutsceneMgr10ms_runningE"));
    SET_TO(m_aCheatsActive, aml->GetSym(hGTASA, "_ZN6CCheat15m_aCheatsActiveE"));
    SET_TO(ms_pPedPool, aml->GetSym(hGTASA, "_ZN6CPools11ms_pPedPoolE"));
    SET_TO(ms_pVehiclePool, aml->GetSym(hGTASA, "_ZN6CPools15ms_pVehiclePoolE"));
    SET_TO(ms_pObjectPool, aml->GetSym(hGTASA, "_ZN6CPools14ms_pObjectPoolE"));
    SET_TO(ScriptResourceManager, aml->GetSym(hGTASA, "_ZN11CTheScripts21ScriptResourceManagerE"));
    SET_TO(ms_colModelPed1, aml->GetSym(hGTASA, "_ZN14CTempColModels15ms_colModelPed1E"));

// Funcs
    SET_TO(CreateObject, aml->GetSym(hGTASA, "_ZN7CObject6CreateEib"));
    SET_TO(FindGroundZForCoord, aml->GetSym(hGTASA, "_ZN6CWorld19FindGroundZForCoordEff"));
    SET_TO(FindGroundZFor3DCoord, aml->GetSym(hGTASA, "_ZN6CWorld21FindGroundZFor3DCoordEfffPbPP7CEntity"));
    SET_TO(ClearSpaceForMissionEntity, aml->GetSym(hGTASA, "_ZN11CTheScripts26ClearSpaceForMissionEntityERK7CVectorP7CEntity"));
    SET_TO(AddToWorld, aml->GetSym(hGTASA, "_ZN6CWorld3AddEP7CEntity"));
    SET_TO(GetObjectRef, aml->GetSym(hGTASA, "_ZN6CPools12GetObjectRefEP7CObject"));
    SET_TO(GetVehicleRef, aml->GetSym(hGTASA, "_ZN6CPools13GetVehicleRefEP8CVehicle"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));
    SET_TO(GetPedFromRef, aml->GetSym(hGTASA, "_ZN6CPools6GetPedEi"));
    SET_TO(GetObjectFromRef, aml->GetSym(hGTASA, "_ZN6CPools9GetObjectEi"));
    SET_TO(UpdateCompareFlag, aml->GetSym(hGTASA, "_ZN14CRunningScript17UpdateCompareFlagEh"));
    SET_TO(CalcScreenCoors, aml->GetSym(hGTASA, "_ZN7CSprite15CalcScreenCoorsERK5RwV3dPS0_PfS4_bb"));
    SET_TO(GetActualPickupIndex, aml->GetSym(hGTASA, "_ZN8CPickups20GetActualPickupIndexEi"));
    SET_TO(ProcessLineOfSight, aml->GetSym(hGTASA, "_ZN6CWorld18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntitybbbbbbbb"));
    SET_TO(QuaternionSet, aml->GetSym(hGTASA, "_ZN11CQuaternion3SetERK11RwMatrixTag"));
    SET_TO(QuaternionMult, aml->GetSym(hGTASA, "_ZN11CQuaternion8MultiplyERKS_S1_"));
    SET_TO(RtQuatRotate, aml->GetSym(hGTASA, "_Z12RtQuatRotateP6RtQuatPK5RwV3df15RwOpCombineType"));
    SET_TO(RwMatrixRotate, aml->GetSym(hGTASA, "_Z14RwMatrixRotateP11RwMatrixTagPK5RwV3df15RwOpCombineType"));
    SET_TO(GetCurrentZoneLockedOrUnlocked, aml->GetSym(hGTASA, "_ZN9CTheZones30GetCurrentZoneLockedOrUnlockedEff"));
    SET_TO(TransformRealWorldPointToRadarSpace, aml->GetSym(hGTASA, "_ZN6CRadar35TransformRealWorldPointToRadarSpaceER9CVector2DRKS0_"));
    SET_TO(LimitRadarPoint, aml->GetSym(hGTASA, "_ZN6CRadar15LimitRadarPointER9CVector2D"));
    SET_TO(TransformRadarPointToScreenSpace, aml->GetSym(hGTASA, "_ZN6CRadar32TransformRadarPointToScreenSpaceER9CVector2DRKS0_"));
    SET_TO(DisplayThisBlip, aml->GetSym(hGTASA, "_ZN6CRadar15DisplayThisBlipEia"));
    SET_TO(AddBlipToLegendList, aml->GetSym(hGTASA, "_ZN6CRadar19AddBlipToLegendListEhi"));
    SET_TO(DrawSprite, aml->GetSym(hGTASA, "_ZN9CSprite2d4DrawERK5CRectRK5CRGBA"));
    SET_TO(GetUppercaseKey, aml->GetSym(hGTASA, "_ZN7CKeyGen15GetUppercaseKeyEPKc"));
    SET_TO(GetWeaponInfo, aml->GetSym(hGTASA, "_ZN11CWeaponInfo13GetWeaponInfoE11eWeaponTypea"));
    SET_TO(SetClumpAlpha, aml->GetSym(hGTASA, "_ZN18CVisibilityPlugins13SetClumpAlphaEP7RpClumpi"));
    SET_TO(GetDistanceFromCentreOfMassToBaseOfModel, aml->GetSym(hGTASA, "_ZN7CEntity40GetDistanceFromCentreOfMassToBaseOfModelEv"));
    SET_TO(ProcessVerticalLine, aml->GetSym(hGTASA, "_ZN6CWorld19ProcessVerticalLineERK7CVectorfR9CColPointRP7CEntitybbbbbbP15CStoredCollPoly"));
    SET_TO(FindPlayerPed, aml->GetSym(hGTASA, "_Z13FindPlayerPedi"));
    SET_TO(GetPlayerInfoForThisPlayerPed, aml->GetSym(hGTASA, "_ZN10CPlayerPed29GetPlayerInfoForThisPlayerPedEv"));
    SET_TO(GetWeaponSkill, aml->GetSym(hGTASA, "_ZN4CPed14GetWeaponSkillE11eWeaponType"));
    SET_TO(ObjectDamage, aml->GetSym(hGTASA, "_ZN7CObject12ObjectDamageEfP7CVectorS1_P7CEntity11eWeaponType"));
    SET_TO(AddToResourceManager, aml->GetSym(hGTASA, "_ZN22CScriptResourceManager20AddToResourceManagerEijP14CRunningScript"));
    SET_TO(RequestModel, aml->GetSym(hGTASA, "_ZN10CStreaming12RequestModelEii"));
    SET_TO(LoadAllRequestedModels, aml->GetSym(hGTASA, "_ZN10CStreaming22LoadAllRequestedModelsEb"));
    SET_TO(TimerSuspend, aml->GetSym(hGTASA, "_ZN6CTimer7SuspendEv"));
    SET_TO(TimerResume, aml->GetSym(hGTASA, "_ZN6CTimer6ResumeEv"));
    SET_TO(AddPedModel, aml->GetSym(hGTASA, "_ZN10CModelInfo11AddPedModelEi"));
    SET_TO(SetColModel, aml->GetSym(hGTASA, "_ZN14CBaseModelInfo11SetColModelEP9CColModelb"));
    SET_TO(RequestSpecialModel, aml->GetSym(hGTASA, "_ZN10CStreaming19RequestSpecialModelEiPKci"));
}