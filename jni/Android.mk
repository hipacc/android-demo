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
$(call clear-vars, HIPACC_SETUP_COMPLETE)
HIPACC_SRC_PATH := hipacc_src
HIPACC_GEN_PATH := hipacc_gen
HIPACC_INCLUDES := $(shell pwd)/$(LOCAL_PATH)
HIPACC_SRC_FILES := $(subst $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/,,\
                            $(wildcard $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/*.cpp))


################################################################################
# Run HIPAcc for Renderscript
################################################################################
HIPACC_GEN_PREFIX := rs
HIPACC_FLAGS := -emit-renderscript -rs-package org.hipacc.demo
include $(LOCAL_PATH)/HIPAcc.mk


################################################################################
# Run HIPAcc for Filterscript
################################################################################
HIPACC_GEN_PREFIX := fs
HIPACC_FLAGS := -emit-filterscript -rs-package org.hipacc.demo
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
	$(shell if test -e $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/fs$(SRC) && \
               ! grep -q "^#define FILTERSCRIPT" \
	                $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/fs$(SRC); then \
	            $(SED) '1 s|^|#define FILTERSCRIPT\'$$'\n|' \
	                    $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/fs$(SRC); \
	        fi))


include $(BUILD_SHARED_LIBRARY)


################################################################################
# Add prebuilts for Renderscript Support (legacy support API>=11)
################################################################################
include $(CLEAR_VARS)

LOCAL_MODULE := libRSSupport
LOCAL_SRC_FILES := $(SYSROOT_LINK)/usr/lib/rs/libRSSupport.so

include $(PREBUILT_SHARED_LIBRARY)
