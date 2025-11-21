#include <externs.h>

inline bool IsTaskImportantForPed(CPed* ped, int type)
{
    if(type == TASK_COMPLEX_WANDER                      || type == TASK_SIMPLE_CAR_DRIVE                        ||
       type == TASK_COMPLEX_CAR_DRIVE                   || type == TASK_COMPLEX_CAR_DRIVE_WANDER                ||
       type == TASK_SIMPLE_NAMED_ANIM                   || type == TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING ||
       type == TASK_COMPLEX_GANG_LEADER                 || type == TASK_COMPLEX_GANG_FOLLOWER                   ||
       type == TASK_COMPLEX_GOTO_VEHICLE_AND_LEAN       || type == TASK_COMPLEX_SEEK_ENTITY                     ||
       type == TASK_SIMPLE_PLAYER_ON_FOOT               || type == TASK_SIMPLE_PLAYER_IN_CAR                    ||
       type == TASK_SIMPLE_STAND_STILL                  || type == TASK_SIMPLE_SCRATCH_HEAD                     ||
       type == TASK_SIMPLE_DO_HAND_SIGNAL               || type == TASK_COMPLEX_BE_IN_COUPLE                    ||
       type == TASK_SIMPLE_SUNBATHE                     || type == TASK_COMPLEX_SUNBATHE                        ||
       type == TASK_COMPLEX_WALK_ROUND_CAR              || type == TASK_COMPLEX_WALK_ROUND_FIRE                 ||
       type == TASK_COMPLEX_WALK_ROUND_OBJECT           || type == TASK_COMPLEX_WALK_ROUND_SHOP                 ||
       type == TASK_COMPLEX_WALK_ROUND_BUILDING_ATTEMPT || type == TASK_COMPLEX_PARTNER)
    {
        return false;
    }

    if(ped && ped->m_nMoveState <= PEDMOVE_WALK)
    {
        if(type == TASK_COMPLEX_FLEE_ANY_MEANS || type == TASK_COMPLEX_FLEE_ENTITY      ||
           type == TASK_COMPLEX_FLEE_POINT     || type == TASK_COMPLEX_SMART_FLEE_POINT ||
           type == TASK_COMPLEX_SMART_FLEE_ENTITY)
        {
           return false;
        }
    }

    return true;
}

void PedExtVars::GrabTasks(CPed* ped)
{
    aiFlagsIntValue = 0; // fast clean-up
    if(!ped->m_pIntelligence) return;
    CTaskManager* taskMan = &ped->m_pIntelligence->m_TaskMgr;

    int idx = 0, type;
    CTask* task = NULL;
    for(int i = 0; i < TASK_PRIMARY_MAX; ++i)
    {
        task = taskMan->m_aPrimaryTasks[i];
        while(task)
        {
            type = (int)task->GetTaskType();
            if(idx < PedExtVars::MAX_TASKS_CACHE) activeTasks[idx++] = type;
            GrabFlags(task, type, false, idx, ped);
            task = task->GetSubTask();
        }
    }
    for(int i = 0; i < TASK_SECONDARY_MAX; ++i)
    {
        task = taskMan->m_aSecondaryTasks[i];
        while(task)
        {
            type = (int)task->GetTaskType();
            if(idx < PedExtVars::MAX_TASKS_CACHE) activeTasks[idx++] = type;
            GrabFlags(task, type, true, idx, ped);
            task = task->GetSubTask();
        }
    }

    for(int i = idx; i < PedExtVars::MAX_TASKS_CACHE; ++i)
    {
        activeTasks[i] = -1;
    }
}
void PedExtVars::GrabFlags(CTask* task, int type, bool secondaryTask, int idx, CPed* ped)
{
    uintptr_t pedTarget_TaskOffset = 0;
    switch(type)
    {
        case TASK_COMPLEX_ARREST_PED:
        case TASK_COMPLEX_KILL_PED_ON_FOOT:
            pedTarget_TaskOffset = 0x10;
            break;
            
        case TASK_SIMPLE_ARREST_PED:
        case TASK_SIMPLE_KILL_PED_WITH_CAR:
        case TASK_SIMPLE_HURT_PED_WITH_CAR:
            pedTarget_TaskOffset = 0x08;
            break;

        case TASK_KILL_PED_FROM_BOAT:
        case TASK_COMPLEX_KILL_CRIMINAL:
        case TASK_COMPLEX_HIT_PED_WITH_CAR:
            pedTarget_TaskOffset = 0x0C;
            break;

        case TASK_COMPLEX_FALL_AND_GET_UP:
        case TASK_COMPLEX_FALL_AND_STAY_DOWN:
            aiFlags.bFallenOnGround = true;
            break;

        case TASK_SIMPLE_USE_GUN:
            aiFlags.bUsingGun = true;
            break;

        case TASK_SIMPLE_FIGHT:
            aiFlags.bFighting = true;
            break;

        case TASK_COMPLEX_ENTER_CAR_AS_DRIVER:
        case TASK_COMPLEX_ENTER_CAR_AS_PASSENGER:
            aiFlags.bEnteringAnyCar = true;
            enterExitTargetCar = *(CVehicle**)((uintptr_t)task + 0x0C);
            break;

        case TASK_COMPLEX_LEAVE_ANY_CAR:
            aiFlags.bExitingAnyCar = true;
            enterExitTargetCar = ped->m_pVehicle;
            break;

        case TASK_COMPLEX_LEAVE_CAR:
        case TASK_COMPLEX_LEAVE_BOAT:
            aiFlags.bExitingAnyCar = true;
            enterExitTargetCar = *(CVehicle**)((uintptr_t)task + 0x0C);
            break;

        case TASK_SIMPLE_NAMED_ANIM:
            if(secondaryTask)
            {
                aiFlags.bPlayingAnySecondaryScriptAnimation = true;
            }
            else
            {
                aiFlags.bPlayingAnyPrimaryScriptAnimation = true;
            }
            break;
    }

    if(idx == 1 && !aiFlags.bKillingSomething)
    {
        if(type == TASK_COMPLEX_BE_IN_GROUP)
        {
            CTask* subtask = task->GetSubTask();
            if(subtask)
            {
                aiFlags.bRootTaskIsntImportant = !IsTaskImportantForPed(ped, type);
            }
        }
        else
        {
            aiFlags.bRootTaskIsntImportant = !IsTaskImportantForPed(ped, type);
        }
    }

    if(pedTarget_TaskOffset)
    {
        aiFlags.bKillingSomething = true; // this is not KILLING
        killTargetPed = *(CEntity**)((uintptr_t)task + pedTarget_TaskOffset);
    }
    else
    {
        killTargetPed = NULL;
    }
}

CLEO_Fn(IS_CHAR_DOING_TASK_ID)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    int type = cleo->ReadParam(handle)->i;
    bool found = false;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata) found = xdata->HasTask(type);
    }
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(GET_CHAR_TASK_POINTER_BY_ID)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    int taskId = cleo->ReadParam(handle)->i;

    if(ped->m_pIntelligence)
    {
        CTask* task;
        CTaskManager& taskMan = ped->m_pIntelligence->m_TaskMgr;
        for(int i = 0; i < 5; ++i)
        {
            task = taskMan.m_aPrimaryTasks[i];
            while(task)
            {
                if(task->GetTaskType() == (eTaskType)taskId)
                {
                    cleo->GetPointerToScriptVar(handle)->i = (int)task;
                    cleoaddon->UpdateCompareFlag(handle, true);
                    return;
                }
                task = task->GetSubTask();
            }
        }
        for(int i = 0; i < 6; ++i)
        {
            task = taskMan.m_aSecondaryTasks[i];
            while(task)
            {
                if(task->GetTaskType() == (eTaskType)taskId)
                {
                    cleo->GetPointerToScriptVar(handle)->i = (int)task;
                    cleoaddon->UpdateCompareFlag(handle, true);
                    return;
                }
                task = task->GetSubTask();
            }
        }
    }

    cleo->GetPointerToScriptVar(handle)->i = 0;
    cleoaddon->UpdateCompareFlag(handle, false);
}
CLEO_Fn(GET_CHAR_KILL_TARGET_CHAR)
{    
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    int ref = -1;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata && xdata->killTargetPed && xdata->killTargetPed->m_nType == ENTITY_TYPE_PED)
        {
            ref = GetPedRef((CPed*)xdata->killTargetPed);
        }
    }
    cleo->GetPointerToScriptVar(handle)->i = ref;
    cleoaddon->UpdateCompareFlag(handle, ref != -1);
}
CLEO_Fn(IS_CHAR_USING_GUN)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    bool found = false;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata) found = xdata->aiFlags.bUsingGun;
    }
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(IS_CHAR_FIGHTING)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    bool found = false;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata) found = xdata->aiFlags.bFighting;
    }
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(IS_CHAR_FALLEN_ON_GROUND)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    bool found = false;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata) found = xdata->aiFlags.bFallenOnGround;
    }
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(IS_CHAR_ENTERING_ANY_CAR)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    bool found = false;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata) found = xdata->aiFlags.bEnteringAnyCar;
    }
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(IS_CHAR_EXITING_ANY_CAR)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    bool found = false;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata) found = xdata->aiFlags.bExitingAnyCar;
    }
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(IS_CHAR_PLAYING_ANY_SCRIPT_ANIMATION)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    int mode = cleo->ReadParam(handle)->i;
    bool found = true;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata) found = xdata->HasAnimFlags(mode);
    }
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(IS_CHAR_DOING_ANY_IMPORTANT_TASK)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    int mode = cleo->ReadParam(handle)->i;
    bool found = true;
    if(ped)
    {
        PedExtVars* xdata = GetExtData(ped);
        if(xdata && xdata->aiFlags.bRootTaskIsntImportant && !xdata->aiFlags.bKillingSomething)
        {
            found = xdata->HasAnimFlags(mode);
        }
    }
    cleoaddon->UpdateCompareFlag(handle, found);
}
CLEO_Fn(CLEAR_CHAR_PRIMARY_TASKS)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    if(ped && ped->m_pIntelligence)
    {
        ClearTasks(ped->m_pIntelligence, true, false);
    }
}
CLEO_Fn(CLEAR_CHAR_SECONDARY_TASKS)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    if(ped && ped->m_pIntelligence)
    {
        ClearTasks(ped->m_pIntelligence, false, true);
    }
}
CLEO_Fn(GET_CHAR_SIMPLEST_ACTIVE_TASK)
{
    CPed* ped = (CPed*)cleo->ReadParam(handle)->i;
    CTask* task = NULL;
    if(ped && ped->m_pIntelligence)
    {
        task = GetSimplestActiveTask(&ped->m_pIntelligence->m_TaskMgr);
    }

    if(task)
    {
        cleo->GetPointerToScriptVar(handle)->i = (int)task->GetTaskType();
        cleo->GetPointerToScriptVar(handle)->i = (int)task;
    }
    else
    {
        cleo->GetPointerToScriptVar(handle)->i = -1;
        cleo->GetPointerToScriptVar(handle)->i = -1;
    }
    cleoaddon->UpdateCompareFlag(handle, task != NULL);
}