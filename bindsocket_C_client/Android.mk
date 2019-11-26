LOCAL_PATH:= $(call my-dir)
svc_c_flags =   \
    -Wall -Wextra -Wunused-parameter \

include $(CLEAR_VARS)
LOCAL_SHARED_LIBRARIES := liblog libselinux
LOCAL_SRC_FILES := bindsocket.c
LOCAL_CFLAGS += $(svc_c_flags)
LOCAL_MODULE := bindsocket
LOCAL_MODULE_TAGS := optional
LOCAL_CLANG := true
include $(BUILD_EXECUTABLE)

