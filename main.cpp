#include <mod/amlmod.h>
#include <mod/logger.h>

#include "cleo.h"
#include <GTASA_STRUCTS.h>

MYMOD(net.juniordjjr.rusjj.cleoplus, CLEOPlus, 1.0, JuniorDjjr & RusJJ)
NEEDGAME(com.rockstargames.gtasa)

// Savings
uintptr_t pGTASA;
void* hGTASA;
cleo_ifs_t* cleo;

// Game Vars


// int main!
extern "C" void OnModLoad()
{
    logger->SetTag("CLEOPlus");
    pGTASA = aml->GetLib("libGTASA.so");
    hGTASA = aml->GetLibHandle("libGTASA.so");
    cleo = (cleo_ifs_t*)GetInterface("CLEO");


}
