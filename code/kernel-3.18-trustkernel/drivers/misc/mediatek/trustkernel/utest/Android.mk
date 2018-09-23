LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_C_INCLUDES := $(LOCAL_PATH)/ \

LOCAL_MODULE     := utest
LOCAL_SRC_FILES  := \
	utest.c\

LOCAL_LDFLAGS := -llog 

LOCAL_SHARED_LIBRARIES := \

include $(BUILD_EXECUTABLE)
