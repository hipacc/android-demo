LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := filters

LOCAL_CPPFLAGS += -DSIZE_X=5 -DSIZE_Y=5
LOCAL_SRC_FILES := filters.cpp
LOCAL_LDLIBS := -llog -ljnigraphics

LOCAL_ARM_MODE := arm


################################################################################
# Configure HIPAcc
################################################################################
HIPACC_RS_VERSION := 19
HIPACC_SRC_PATH := hipacc_src
HIPACC_GEN_PATH := hipacc_gen
#HIPACC_INCLUDES := ...
HIPACC_SRC_FILES := $(subst $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/,,\
                            $(wildcard $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/*.cpp))


################################################################################
# Run HIPAcc for Renderscript
################################################################################
HIPACC_FLAGS := -emit-renderscript -rs-package org.hipacc.demo
HIPACC_SRC_PREFIX := rs
include $(LOCAL_PATH)/HIPAcc.mk


################################################################################
# Run HIPAcc for Filterscript
################################################################################
HIPACC_FLAGS := -emit-filterscript -rs-package org.hipacc.demo
HIPACC_SRC_PREFIX := fs
include $(LOCAL_PATH)/HIPAcc.mk


################################################################################
# Append generated sources
################################################################################
LOCAL_SRC_FILES += $(subst $(LOCAL_PATH)/,, \
                           $(wildcard $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,, \
                           $(wildcard $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/*.rs)) \
                   $(subst $(LOCAL_PATH)/,, \
                           $(wildcard $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/*.fs))

# Prepend define FILTERSCRIPT in generated Filterscript sources
$(foreach SRC,$(HIPACC_SRC_FILES), \
	$(shell sed -i "1i#define FILTERSCRIPT" \
		$(LOCAL_PATH)/$(HIPACC_GEN_PATH)/fs$(SRC));)


include $(BUILD_SHARED_LIBRARY)
