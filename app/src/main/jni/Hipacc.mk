ifeq ($(HIPACC_CLEAN),1)
# Clean generated files
$(shell $(call host-rm, $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/* \
                        $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/.checksums))
else
ifneq ($(HIPACC_SETUP_COMPLETE),1)


################################################################################
# Setup local C/C++ to use Renderscript from NDK
################################################################################
LOCAL_C_INCLUDES += $(RENDERSCRIPT_PLATFORM_HEADER)/cpp \
                    $(RENDERSCRIPT_PLATFORM_HEADER)
LOCAL_LDFLAGS    += -L$(RENDERSCRIPT_TOOLCHAIN_PREBUILT_ROOT)/platform/$(TARGET_ARCH)
LOCAL_STATIC_LIBRARIES := RScpp_static
$(shell $(call host-mkdir, libs/$(TARGET_ARCH_ABI))) # must be created manually


################################################################################
# Setup Hipacc
################################################################################

# Search Hipacc includes (relative to Hipacc binary in PATH)
HIPACC_PATH     := $(subst bin/hipacc,,$(shell which hipacc))
HIPACC_INCLUDES += $(HIPACC_PATH)/include/dsl

# Create directory for generated sources
$(shell $(call host-mkdir, $(LOCAL_PATH)/$(HIPACC_GEN_PATH)))


################################################################################
# Setup Hipacc specific flags and sources to local C/C++
################################################################################
LOCAL_RENDERSCRIPT_FLAGS += -allow-rs-prefix -Wno-unused-variable -Wno-unused-function \
                            $(addprefix -I,$(HIPACC_INCLUDES))
LOCAL_C_INCLUDES += $(HIPACC_INCLUDES) $(HIPACC_PATH)/include \
                    $(TARGET_OBJS)/$(LOCAL_MODULE)/$(HIPACC_GEN_PATH)
LOCAL_SRC_FILES += hipacc_runtime.cpp

################################################################################
# Setup operating system specific tools
################################################################################
MD5 := md5sum
SED := sed -i
ifeq ($(HOST_OS),darwin)
    MD5 := md5 -r
    SED := sed -i ''
endif
ifeq ($(HOST_OS),linux)
    SHELL=/bin/bash
endif


# Mark setup complete
HIPACC_SETUP_COMPLETE := 1
endif # HIPACC_SETUP_COMPLETE


################################################################################
# Run Hipacc
################################################################################
$(foreach SRC,$(HIPACC_SRC_FILES), \
    $(shell cd $(LOCAL_PATH)/$(HIPACC_GEN_PATH); \
            MD5SUM=$$(echo $(HIPACC_APP_FLAGS) $(HIPACC_FLAGS) | \
                    cat $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/$(SRC) - | $(MD5)); \
            KEY=$(HIPACC_GEN_PREFIX)$(SRC); \
            if [ ! -e .checksums ] || \
               [ ! -e $(HIPACC_GEN_PREFIX)$(SRC) ] || \
               [ "$$KEY:$$MD5SUM" != "$$(grep $$KEY .checksums)" ]; then \
                hipacc $(HIPACC_FLAGS) $(HIPACC_APP_FLAGS) -DHIPACC \
                        -std=c++11 -stdlib=libc++ -nostdinc++ \
                        -I$(HIPACC_PATH)/include/c++/v1 \
                        -I$(HIPACC_PATH)/include/clang \
                        $(addprefix -I,$(HIPACC_INCLUDES)) \
                        $(LOCAL_PATH)/$(HIPACC_SRC_PATH)/$(SRC) \
                        -o $(HIPACC_GEN_PREFIX)$(SRC); \
                if [ "$$?" == "0" ]; then \
                    if grep -q $$KEY .checksums; then \
                        $(SED) "s/$$KEY:.*$$/$$KEY:$$MD5SUM/" .checksums; \
                    else \
                        echo "$$KEY:$$MD5SUM" >> .checksums; \
                    fi; \
                fi; \
            fi))


endif # HIPACC_CLEAN
