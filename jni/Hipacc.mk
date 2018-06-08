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
LOCAL_LDLIBS     += -lRScpp_static
#LOCAL_CFLAGS     += -std=c++11 -Wall -Wextra
$(shell $(call host-mkdir, libs/$(TARGET_ARCH_ABI))) # must be created manually


################################################################################
# Setup Hipacc
################################################################################
# Convert relative source path to absolute path
HIPACC_SRC_PATH := $(shell pwd)/$(LOCAL_PATH)/$(HIPACC_SRC_PATH)

# Search Hipacc includes (relative to Hipacc binary in PATH)
HIPACC_INCLUDES += $(subst bin/hipacc,include,$(shell which hipacc)) \
                   $(subst bin/hipacc,include/dsl,$(shell which hipacc))

# Create directory for generated sources
$(shell $(call host-mkdir, $(LOCAL_PATH)/$(HIPACC_GEN_PATH)))


################################################################################
# Setup Hipacc specific flags and sources to local C/C++
################################################################################
LOCAL_CPPFLAGS += -D__HIPACC_RS_STANDALONE_HPP__
LOCAL_RENDERSCRIPT_FLAGS += -allow-rs-prefix -Wno-unused-variable -Wno-unused-function \
                            $(addprefix -I,$(HIPACC_INCLUDES))
LOCAL_C_INCLUDES += $(HIPACC_INCLUDES) \
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
            MD5SUM=$$(echo $(LOCAL_CPPFLAGS) $(HIPACC_FLAGS) | \
                    cat $(HIPACC_SRC_PATH)/$(SRC) - | $(MD5)); \
            KEY=$(HIPACC_GEN_PREFIX)$(SRC); \
            if [ ! -e .checksums ] || \
               [ ! -e $(HIPACC_GEN_PREFIX)$(SRC) ] || \
               [ "$$KEY:$$MD5SUM" != "$$(grep $$KEY .checksums)" ]; then \
                hipacc $(HIPACC_FLAGS) -std=c++11 -stdlib=libc++ \
                        -I$(shell llvm-config --includedir) \
                        -I$(shell llvm-config --includedir)/c++/v1 \
                        -I$(shell clang -print-file-name=include) \
                        $(addprefix -I,$(HIPACC_INCLUDES)) \
                        $(LOCAL_CPPFLAGS) -DHIPACC \
                        $(HIPACC_SRC_PATH)/$(SRC) \
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
