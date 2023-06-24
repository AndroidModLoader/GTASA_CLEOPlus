#include "externs.h"

CLEO_Fn(WRITE_STRUCT_OFFSET)
{
    unsigned int struc = cleo->ReadParam(handle)->i;
    unsigned int offset = cleo->ReadParam(handle)->i;
    unsigned int size = cleo->ReadParam(handle)->i;
    unsigned int value;

    CRunningScript* script = (CRunningScript*)handle;
    if (*script->PCPointer > 8) // is string (CLEO_GetOperandType)
    {
        *(char **)(struc + offset) = (char *)cleo->ReadParam(handle)->i;
    }
    else
    {
        value = cleo->ReadParam(handle)->i;
        memcpy((void *)(struc + offset), &value, size);
    }
}
CLEO_Fn(READ_STRUCT_OFFSET)
{
    unsigned int struc = cleo->ReadParam(handle)->i;
    unsigned int offset = cleo->ReadParam(handle)->i;
    unsigned int size = cleo->ReadParam(handle)->i;
    unsigned int result = 0;
    
    CRunningScript* script = (CRunningScript*)handle;
    if (*script->PCPointer > 8) // is string (CLEO_GetOperandType)
    {
        cleo->GetPointerToScriptVar(handle)->i = (int)((char *)(struc + offset));
    }
    else
    {
        memcpy(&result, (void *)(struc + offset), size);
        cleo->GetPointerToScriptVar(handle)->i = result;
    }
}
CLEO_Fn(COPY_MEMORY)
{
    unsigned int from = cleo->ReadParam(handle)->i;
    unsigned int to = cleo->ReadParam(handle)->i;
    unsigned int size = cleo->ReadParam(handle)->i;
    memcpy((void*)to, (void*)from, size);
}
CLEO_Fn(WRITE_STRUCT_PARAM)
{
    unsigned int *struc = (unsigned int *)cleo->ReadParam(handle)->i;
    int param = cleo->ReadParam(handle)->i;
    int value = cleo->ReadParam(handle)->i;
    memcpy(&struc[param], &value, 4);
}
CLEO_Fn(READ_STRUCT_PARAM)
{
    unsigned int *struc = (unsigned int *)cleo->ReadParam(handle)->i;
    int param = cleo->ReadParam(handle)->i;
    int value;
    memcpy(&value, &struc[param], 4);
    cleo->GetPointerToScriptVar(handle)->i = value;
}
CLEO_Fn(MAKE_NOP)
{
    unsigned int address = cleo->ReadParam(handle)->i;
    unsigned int size = cleo->ReadParam(handle)->i;
    aml->PlaceNOP(address, size);
}
CLEO_Fn(READ_STRUCT_OFFSET_MULTI)
{
    CRunningScript* script = (CRunningScript*)handle;
    unsigned int struc = cleo->ReadParam(handle)->i;
    unsigned int offset = cleo->ReadParam(handle)->i;
    unsigned int count = cleo->ReadParam(handle)->i;
    unsigned int size = cleo->ReadParam(handle)->i;

    struc += offset;

    for (unsigned int i = 0; i < (count * size); i += size)
    {
        cleo_ifs_t::data_t *pointer = cleo->ReadParam(handle);
        if (pointer != nullptr)
        {
            pointer->i = 0;
            memcpy(pointer, (void*)(struc + i), size);
        }
    }
    while(*script->PCPointer) cleo->ReadParam(handle);
    ++script->PCPointer;
}
CLEO_Fn(WRITE_STRUCT_OFFSET_MULTI)
{
    CRunningScript* script = (CRunningScript*)handle;
    unsigned int struc = cleo->ReadParam(handle)->i;
    unsigned int offset = cleo->ReadParam(handle)->i;
    unsigned int count = cleo->ReadParam(handle)->i;
    unsigned int size = cleo->ReadParam(handle)->i;
    unsigned int value;

    struc += offset;

    for (unsigned int i = 0; i < (count * size); i += size)
    {
        value = cleo->ReadParam(handle)->i;
        memcpy((void *)(struc + i), &value, size);
    }
    while(*script->PCPointer) cleo->ReadParam(handle);
    ++script->PCPointer;
}