LOCAL_PATH:= $(call my-dir)
svc_c_flags =   \
    -Wall -Wextra -Wunused-parameter -Wno-error=format-security 

include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := liblog libselinux
LOCAL_SRC_FILES := mysocket_service.c
LOCAL_CFLAGS += $(svc_c_flags)
LOCAL_MODULE := mysocket
LOCAL_MODULE_TAGS := optional
LOCAL_CLANG := true
include $(BUILD_EXECUTABLE)

