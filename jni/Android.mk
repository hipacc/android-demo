LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hipacc_filters

LOCAL_CPPFLAGS += -DSIZE_X=5 -DSIZE_Y=5
LOCAL_SRC_FILES := hipacc_filters.cpp
LOCAL_LDLIBS := -llog -ljnigraphics

LOCAL_ARM_MODE := arm

# Configure HIPAcc
HIPACC_RS_VERSION := 19
HIPACC_SRC_PATH := hipacc_src
HIPACC_GEN_PATH := hipacc_gen
HIPACC_FLAGS := -rs-package org.hipacc.example
#HIPACC_INCLUDES := ...
HIPACC_SRC_FILES := $(subst $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/,,\
                            $(wildcard $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/*.cpp))

# Run HIPAcc
include $(LOCAL_PATH)/HIPAcc.mk

include $(BUILD_SHARED_LIBRARY)
