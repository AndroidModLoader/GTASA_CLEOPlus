#include "externs.h"

#include <string>
#include <vector>
#include <list>

class ScriptList
{
public:
    int type;
    uintptr_t listPointer;

    ScriptList()
    {
        type = 0;
        listPointer = 0;
    }

    ~ScriptList()
    {
        if (listPointer)
        {
            if (type == 0) // int
            {
                std::list<int>* l = (std::list<int>*)listPointer;
                l->clear(); // useful?
                delete l;
            }
            else if (type == 1) // float
            {
                std::list<float>* l = (std::list<float>*)listPointer;
                l->clear();
                delete l;
            }
            else if (type == 2) // string
            {
                std::list<std::string>* l = (std::list<std::string>*)listPointer;
                l->clear();
                delete l;
            }
        }
    }
};
std::vector<ScriptList*> scriptLists;
inline void ClearScriptLists()
{
    for (auto& scriptList : scriptLists) delete scriptList;
    scriptLists.clear();
}

CLEO_Fn(CREATE_LIST)
{
    int type = cleo->ReadParam(handle)->i;
    ScriptList *scriptList = new ScriptList();
    scriptList->type = type;
    if (type == 0) // int
    {
        std::list<int> *l = new std::list<int>();
        scriptList->listPointer = (int)l;
    }
    else if (type == 1) // float
    {
        std::list<float> *l = new std::list<float>();
        scriptList->listPointer = (int)l;
    }
    else if (type == 2) // string
    {
        std::list<std::string> *l = new std::list<std::string>();
        scriptList->listPointer = (int)l;
    }
    else
    {
        logger->Error("CREATE_LIST failed: list type unknown %d", scriptList->type);
    }
    scriptLists.push_back(scriptList);
    cleo->GetPointerToScriptVar(handle)->i = (int)scriptList;
}
CLEO_Fn(DELETE_LIST)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    if (scriptList) delete scriptList;
}
CLEO_Fn(LIST_ADD)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    if (scriptList->type == 0) // int
    {
        int value = cleo->ReadParam(handle)->i;
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        l->push_back(value);
    }
    else if (scriptList->type == 1) // float 
    {
        float value = cleo->ReadParam(handle)->f;
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        l->push_back(value);
    }
    else
    {
        logger->Error("LIST_ADD failed: list type unknown %d", scriptList->type);
        cleo->ReadParam(handle); // FallBack
    }
}
CLEO_Fn(LIST_REMOVE_VALUE)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    if (scriptList->type == 0) // int
    {
        int value = cleo->ReadParam(handle)->i;
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        l->remove(value);
    }
    else if (scriptList->type == 1) // float
    {
        float value = cleo->ReadParam(handle)->f;
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        l->remove(value);
    }
    else
    {
        logger->Error("LIST_REMOVE_VALUE failed: list type unknown %d", scriptList->type);
        cleo->ReadParam(handle); // FallBack
    }
}
CLEO_Fn(LIST_REMOVE_INDEX)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    unsigned int index = cleo->ReadParam(handle)->i;
    if (scriptList->type == 0) // int
    {
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        if (index < l->size())
        {
            std::list<int>::iterator it;
            it = l->begin();
            advance(it, index);
            l->erase(it);
        }
    }
    else if (scriptList->type == 1) // float
    {
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        if (index < l->size())
        {
            std::list<float>::iterator it;
            it = l->begin();
            advance(it, index);
            l->erase(it);
        }
    }
    else if (scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        if (index < l->size())
        {
            std::list<std::string>::iterator it;
            it = l->begin();
            advance(it, index);
            l->erase(it);
        }
    }
    else
    {
        logger->Error("LIST_REMOVE_INDEX failed: list type unknown %d", scriptList->type);
    }
}
CLEO_Fn(GET_LIST_SIZE)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    int size = 0;
    if (scriptList)
    {
        if (scriptList->type == 0) // int
        {
            std::list<int> *l = (std::list<int>*)scriptList->listPointer;
            size = l->size();
        }
        else if (scriptList->type == 1) // float
        {
            std::list<float> *l = (std::list<float>*)scriptList->listPointer;
            size = l->size();
        }
        else if (scriptList->type == 2) // string
        {
            std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
            size = l->size();
        }
        else
        {
            logger->Error("GET_LIST_SIZE failed: list type unknown %d", scriptList->type);
        }
    }
    cleo->GetPointerToScriptVar(handle)->i = size;
}
CLEO_Fn(GET_LIST_VALUE_BY_INDEX)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    unsigned int index = cleo->ReadParam(handle)->i;
    if (scriptList->type == 0) // int
    {
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        std::list<int>::iterator it;
        it = l->begin();
        if (index >= l->size())
        {
            cleo->GetPointerToScriptVar(handle)->i = 0;
        }
        else
        {
            advance(it, index);
            cleo->GetPointerToScriptVar(handle)->i = *it;
        }
    }
    else if (scriptList->type == 1) // float
    {
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        std::list<float>::iterator it;
        it = l->begin();
        if (index >= l->size())
        {
            cleo->GetPointerToScriptVar(handle)->i = 0;
        }
        else
        {
            advance(it, index);
            cleo->GetPointerToScriptVar(handle)->f = *it;
        }
    }
    else
    {
        logger->Error("GET_LIST_VALUE_BY_INDEX failed: list type unknown %d", scriptList->type);
        cleo->ReadParam(handle); // FallBack
    }
}
CLEO_Fn(RESET_LIST)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    if (scriptList->type == 0) // int
    {
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        l->clear();
    }
    else if (scriptList->type == 1) // float
    {
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        l->clear();
    }
    else if (scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        l->clear();
    }
    else
    {
        logger->Error("RESET_LIST failed: list type unknown %d", scriptList->type);
    }
}
CLEO_Fn(GET_LIST_STRING_VALUE_BY_INDEX)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    unsigned int index = cleo->ReadParam(handle)->i;
    if (scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        std::list<std::string>::iterator it;
        it = l->begin();
        if (index >= l->size())
        {
            CLEO_WriteStringEx(handle, "");
        }
        else
        {
            advance(it, index);
            std::string str = *it;
            CLEO_WriteStringEx(handle, &str[0]);
        }
    }
    else
    {
        logger->Error("GET_LIST_VALUE_BY_INDEX failed: list type unknown %d", scriptList->type);
        CLEO_WriteStringEx(handle, "");//fallback
    }
}
CLEO_Fn(LIST_ADD_STRING)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    char value[128]; CLEO_ReadStringEx(handle, value, sizeof(value));
    if(scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        l->push_back(value);
    }
    else
    {
        logger->Error("LIST_ADD_STRING failed: list type unknown %d", scriptList->type);
    }
}
CLEO_Fn(LIST_REMOVE_STRING_VALUE)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    char value[128]; CLEO_ReadStringEx(handle, value, sizeof(value));
    if (scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        l->remove(value);
    }
    else
    {
        logger->Error("LIST_REMOVE_STRING_VALUE failed: list type unknown %d", scriptList->type);
    }
}
CLEO_Fn(LIST_REMOVE_INDEX_RANGE)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    int index = cleo->ReadParam(handle)->i;
    int indexEnd = cleo->ReadParam(handle)->i;
    if (scriptList->type == 0) // int
    {
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        std::list<int>::iterator it, itEnd;
        it = l->begin();
        advance(it, index);
        itEnd = l->begin();
        advance(itEnd, indexEnd);
        l->erase(it, itEnd);
    }
    else if (scriptList->type == 1) // float
    {
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        std::list<float>::iterator it, itEnd;
        it = l->begin();
        advance(it, index);
        itEnd = l->begin();
        advance(itEnd, indexEnd);
        l->erase(it, itEnd);
    }
    else if (scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        std::list<std::string>::iterator it, itEnd;
        it = l->begin();
        advance(it, index);
        itEnd = l->begin();
        advance(itEnd, indexEnd);
        l->erase(it, itEnd);
    }
    else
    {
        logger->Error("LIST_REMOVE_INDEX_RANGE failed: list type unknown %d", scriptList->type);
    }
}
CLEO_Fn(REVERSE_LIST)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    if (scriptList->type == 0) // int
    {
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        l->reverse();
    }
    else if (scriptList->type == 1) // float
    {
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        l->reverse();
    }
    else if (scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        l->reverse();
    }
    else
    {
        logger->Error("REVERSE_LIST failed: list type unknown %d", scriptList->type);
    }
}
CLEO_Fn(REPLACE_LIST_VALUE_BY_INDEX)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    unsigned int index = cleo->ReadParam(handle)->u;
    if (scriptList->type == 0) // int
    {
        int value = cleo->ReadParam(handle)->i;
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        std::list<int>::iterator it = l->begin();
        if (index >= l->size())
        {
            // hello
        }
        else
        {
            std::advance(it, index);
            it = l->erase(it);
            l->insert(it, value);
        }
    }
    else if (scriptList->type == 1) // float
    {
        float value = cleo->ReadParam(handle)->f;
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        std::list<float>::iterator it = l->begin();
        if (index >= l->size())
        {
            // hello
        }
        else
        {
            std::advance(it, index);
            it = l->erase(it);
            l->insert(it, value);
        }
    }
    else
    {
        logger->Error("REPLACE_LIST_VALUE_BY_INDEX failed: list type unknown %d", scriptList->type);
        cleo->ReadParam(handle); // FallBack
    }
}
CLEO_Fn(REPLACE_LIST_STRING_VALUE_BY_INDEX)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    unsigned int index = cleo->ReadParam(handle)->u;
    char value[128];
    CLEO_ReadStringEx(handle, value, sizeof(value));
    if (scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        std::list<std::string>::iterator it = l->begin();
        if (index >= l->size())
        {
            // hello
        }
        else
        {
            std::advance(it, index);
            it = l->erase(it);
            l->insert(it, value);
        }
    }
    else
    {
        logger->Error("REPLACE_LIST_STRING_VALUE_BY_INDEX failed: list type unknown %d", scriptList->type);
        cleo->ReadParam(handle); // FallBack
    }
}
CLEO_Fn(INSERT_LIST_VALUE_BY_INDEX)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    unsigned int index = cleo->ReadParam(handle)->u;
    if (scriptList->type == 0) // int
    {
        int value = cleo->ReadParam(handle)->i;
        std::list<int> *l = (std::list<int>*)scriptList->listPointer;
        std::list<int>::iterator it = l->begin();
        if (index >= l->size())
        {
            // hello
        }
        else
        {
            std::advance(it, index);
            l->insert(it, value);
        }
    }
    else if (scriptList->type == 1) // float
    {
        float value = cleo->ReadParam(handle)->f;
        std::list<float> *l = (std::list<float>*)scriptList->listPointer;
        std::list<float>::iterator it = l->begin();
        if (index >= l->size())
        {
            // hello
        }
        else
        {
            std::advance(it, index);
            l->insert(it, value);
        }
    }
    else
    {
        logger->Error("INSERT_LIST_VALUE_BY_INDEX failed: list type unknown %d", scriptList->type);
        cleo->ReadParam(handle); // FallBack
    }
}
CLEO_Fn(INSERT_LIST_STRING_VALUE_BY_INDEX)
{
    ScriptList *scriptList = (ScriptList*)cleo->ReadParam(handle)->i;
    unsigned int index = cleo->ReadParam(handle)->u;
    char value[128];
    CLEO_ReadStringEx(handle, value, sizeof(value));
    if (scriptList->type == 2) // string
    {
        std::list<std::string> *l = (std::list<std::string>*)scriptList->listPointer;
        std::list<std::string>::iterator it = l->begin();
        if (index >= l->size())
        {
            // hello
        }
        else
        {
            std::advance(it, index);
            l->insert(it, value);
        }
    }
    else
    {
        logger->Error("INSERT_LIST_STRING_VALUE_BY_INDEX failed: list type unknown %d", scriptList->type);
    }
}