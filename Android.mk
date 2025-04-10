LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CPP_EXTENSION := .cpp .cc
LOCAL_MODULE        := CLEOPlus
LOCAL_SRC_FILES     := main.cpp externs.cpp txdloader.cpp mod/logger.cpp
LOCAL_SRC_FILES     += SimplexNoise/src/SimplexNoise.cpp
LOCAL_SRC_FILES     += cleoplus/nosave.cpp cleoplus/environment.cpp cleoplus/scriptents.cpp cleoplus/screen.cpp cleoplus/types.cpp cleoplus/memory.cpp
LOCAL_SRC_FILES     += cleoplus/pickup.cpp cleoplus/colpoint.cpp cleoplus/list.cpp cleoplus/audio.cpp cleoplus/input.cpp cleoplus/math.cpp cleoplus/radarblip.cpp
LOCAL_SRC_FILES     += cleoplus/extentvars.cpp cleoplus/drawing.cpp cleoplus/misc.cpp cleoplus/events.cpp cleoplus/bones.cpp cleoplus/specialmodels.cpp
LOCAL_SRC_FILES     += cleoplus/timer.cpp cleoplus/ai.cpp
LOCAL_CFLAGS        += -O2 -mfloat-abi=softfp -DNDEBUG -std=c++17
LOCAL_C_INCLUDES    += $(LOCAL_PATH)/SimplexNoise/src
LOCAL_LDLIBS        += -llog
include $(BUILD_SHARED_LIBRARY)