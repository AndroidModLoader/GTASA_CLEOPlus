LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE    := CLEOPlus
LOCAL_SRC_FILES := main.cpp externs.cpp mod/logger.cpp
LOCAL_SRC_FILES += cleoplus/nosave.cpp cleoplus/environment.cpp cleoplus/scriptents.cpp cleoplus/screen.cpp cleoplus/types.cpp cleoplus/memory.cpp cleoplus/pickup.cpp cleoplus/colpoint.cpp
LOCAL_CFLAGS += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++14
LOCAL_C_INCLUDES += ./include
LOCAL_LDLIBS += -llog
include $(BUILD_SHARED_LIBRARY)