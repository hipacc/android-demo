LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := filters

HIPACC_APP_FLAGS := -DSIZE_X=5 -DSIZE_Y=5 -D__HIPACC_RS_STANDALONE_HPP__
LOCAL_CPPFLAGS   += -std=c++11 -Wall -Wextra $(HIPACC_APP_FLAGS)
LOCAL_SRC_FILES  := filters.cpp
LOCAL_LDLIBS     := -llog -ljnigraphics

################################################################################
# Configure Hipacc
################################################################################
$(call clear-vars, HIPACC_SETUP_COMPLETE)
HIPACC_SRC_PATH := hipacc_src
HIPACC_GEN_PATH := hipacc_gen
HIPACC_INCLUDES := $(LOCAL_PATH)
HIPACC_SRC_FILES := $(subst $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/,,\
                            $(wildcard $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/*.cpp))


################################################################################
# Run Hipacc for Renderscript
################################################################################
HIPACC_GEN_PREFIX := rs
HIPACC_FLAGS := -emit-renderscript -target Midgard -rs-package org.hipacc.demo
include $(LOCAL_PATH)/Hipacc.mk


################################################################################
# Run Hipacc for Filterscript
################################################################################
HIPACC_GEN_PREFIX := fs
HIPACC_FLAGS := -emit-filterscript -target Midgard -rs-package org.hipacc.demo
include $(LOCAL_PATH)/Hipacc.mk


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

$(call import-module,android/renderscript)
