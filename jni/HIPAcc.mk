ifeq ($(HIPACC_CLEAN),1)
# Clean generated files
$(shell rm $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/* \
           $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/.checksums)
else
ifneq ($(HIPACC_SETUP_COMPLETE),1)


################################################################################
# Setup local C/C++ to use Renderscript from NDK
################################################################################
LOCAL_C_INCLUDES += $(SYSROOT_LINK)/usr/include/rs/cpp \
                    $(SYSROOT_LINK)/usr/include/rs
LOCAL_LDLIBS += -l$(SYSROOT_LINK)/usr/lib/rs/libcutils.so \
                -l$(SYSROOT_LINK)/usr/lib/rs/libRScpp_static.a
$(shell $(call host-mkdir, libs/$(TARGET_ARCH_ABI))) # must be created manually


################################################################################
# Setup HIPAcc
################################################################################
# Convert relative source path to absolute path
HIPACC_SRC_PATH := $(shell pwd)/$(LOCAL_PATH)/$(HIPACC_SRC_PATH)

# Search HIPAcc includes (relative to HIPAcc binary in PATH)
HIPACC_INCLUDES += $(subst hipacc,..,$(shell which hipacc))/include \
                   $(subst hipacc,..,$(shell which hipacc))/include/dsl


################################################################################
# Setup HIPAcc specific flags and sources to local C/C++
################################################################################
LOCAL_CPPFLAGS += -DRS_TARGET_API=$(HIPACC_RS_VERSION) -DEXCLUDE_IMPL
LOCAL_RENDERSCRIPT_FLAGS += -allow-rs-prefix -target-api $(HIPACC_RS_VERSION) \
                            $(addprefix -I,$(HIPACC_INCLUDES))
LOCAL_C_INCLUDES += $(HIPACC_INCLUDES) \
                    obj/local/$(TARGET_ARCH_ABI)/objs/$(LOCAL_MODULE)/$(HIPACC_GEN_PATH)
LOCAL_SRC_FILES += hipacc_runtime.cpp


# Mark setup complete
HIPACC_SETUP_COMPLETE := 1
endif # HIPACC_SETUP_COMPLETE


################################################################################
# Run HIPAcc
################################################################################
$(foreach SRC,$(HIPACC_SRC_FILES), \
  	$(shell cd $(LOCAL_PATH)/$(HIPACC_GEN_PATH); \
            MD5SUM=$$(echo $(LOCAL_CPPFLAGS) $(HIPACC_FLAGS) | \
                    cat $(HIPACC_SRC_PATH)/$(SRC) - | md5sum); \
            KEY=$(HIPACC_GEN_PREFIX)$(SRC); \
            if [ "$$KEY:$$MD5SUM" != "$$(grep $$KEY .checksums)" ]; then \
                hipacc $(HIPACC_FLAGS) -std=c++11 \
                        -I/usr/include \
                        -I$(shell clang -print-file-name=include) \
                        -I$(shell llvm-config --includedir) \
                        -I$(shell llvm-config --includedir)/c++/v1 \
                        $(addprefix -I,$(HIPACC_INCLUDES)) \
                        $(LOCAL_CPPFLAGS) -DHIPACC \
                        $(HIPACC_SRC_PATH)/$(SRC) \
                        -o $(HIPACC_GEN_PREFIX)$(SRC); \
                if [ "$$?" == "0" ]; then \
                    if grep -q $$KEY .checksums; then \
                        sed -i "/$$KEY:/c$$KEY:$$MD5SUM" .checksums; \
                    else \
                        echo "$$KEY:$$MD5SUM" >> .checksums; \
                    fi; \
                fi; \
            fi))


endif # HIPACC_CLEAN
