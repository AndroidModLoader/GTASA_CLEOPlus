#include <mod/amlmod.h>
#include "externs.h"

// Vars
CCamera* TheCamera;
tUsedObject* UsedObjectArray;
CBaseModelInfo** ms_modelInfoPtrs;
CCarGenerator* CarGeneratorArray;
uint16_t *OldWeatherType, *NewWeatherType, *ForcedWeatherType, *m_ItemToFlash;
float *Rain, *UnderWaterness, *m_fDNBalanceParam;
uint8_t *ms_nGameClockHours, *ms_nGameClockMinutes;
RsGlobalType* RsGlobal;
uint32_t *gbCineyCamProcessedOnFrame, *m_FrameCounter, *ms_exitEnterState, *m_snTimeInMilliseconds;
bool *ms_bTakePhoto, *bDisplayHud;
MobileMenu *gMobileMenu;
CPickup* aPickUps;
CEntity** pIgnoreEntity;

// Funcs
CObject* (*CreateObject)(int);
float (*FindGroundZForCoord)(float, float);
float (*FindGroundZFor3DCoord)(float, float, float, bool*, CEntity**);
void (*ClearSpaceForMissionEntity)(CVector&, CEntity*);
void (*AddToWorld)(CEntity*);
int (*GetObjectRef)(CObject*);
CVehicle* (*GetVehicleFromRef)(int);
CPed* (*GetPedFromRef)(int);
CObject* (*GetObjectFromRef)(int);
void (*UpdateCompareFlag)(CRunningScript*, uint8_t);
bool (*CalcScreenCoors)(RwV3d const&,RwV3d*,float *,float *,bool,bool);
int (*GetActualPickupIndex)(int);
bool (*ProcessLineOfSight)(CVector const&,CVector const&,CColPoint &,CEntity *&,bool,bool,bool,bool,bool,bool,bool,bool);

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

// Funcs
    SET_TO(CreateObject, aml->GetSym(hGTASA, "_ZN7CObject6CreateEib"));
    SET_TO(FindGroundZForCoord, aml->GetSym(hGTASA, "_ZN6CWorld19FindGroundZForCoordEff"));
    SET_TO(FindGroundZFor3DCoord, aml->GetSym(hGTASA, "_ZN6CWorld21FindGroundZFor3DCoordEfffPbPP7CEntity"));
    SET_TO(ClearSpaceForMissionEntity, aml->GetSym(hGTASA, "_ZN11CTheScripts26ClearSpaceForMissionEntityERK7CVectorP7CEntity"));
    SET_TO(AddToWorld, aml->GetSym(hGTASA, "_ZN6CWorld3AddEP7CEntity"));
    SET_TO(GetObjectRef, aml->GetSym(hGTASA, "_ZN6CPools12GetObjectRefEP7CObject"));
    SET_TO(GetVehicleFromRef, aml->GetSym(hGTASA, "_ZN6CPools10GetVehicleEi"));
    SET_TO(GetPedFromRef, aml->GetSym(hGTASA, "_ZN6CPools6GetPedEi"));
    SET_TO(GetObjectFromRef, aml->GetSym(hGTASA, "_ZN6CPools9GetObjectEi"));
    SET_TO(UpdateCompareFlag, aml->GetSym(hGTASA, "_ZN14CRunningScript17UpdateCompareFlagEh"));
    SET_TO(CalcScreenCoors, aml->GetSym(hGTASA, "_ZN7CSprite15CalcScreenCoorsERK5RwV3dPS0_PfS4_bb"));
    SET_TO(GetActualPickupIndex, aml->GetSym(hGTASA, "_ZN8CPickups20GetActualPickupIndexEi"));
    SET_TO(ProcessLineOfSight, aml->GetSym(hGTASA, "_ZN6CWorld18ProcessLineOfSightERK7CVectorS2_R9CColPointRP7CEntitybbbbbbbb"));
}