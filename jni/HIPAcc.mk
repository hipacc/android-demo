################################################################################
# Setup local C/C++ to use Renderscript from NDK
################################################################################

LOCAL_C_INCLUDES += $(SYSROOT_LINK)/usr/include/rs/cpp \
                    $(SYSROOT_LINK)/usr/include/rs
LOCAL_LDLIBS += -l$(SYSROOT_LINK)/usr/lib/rs/libcutils.so \
                -l$(SYSROOT_LINK)/usr/lib/rs/libRScpp_static.a


################################################################################
# Setup and run HIPAcc
################################################################################

# Convert relative source path to absolute path
HIPACC_SRC_PATH := $(shell pwd)/$(LOCAL_PATH)/$(HIPACC_SRC_PATH)

# Search HIPAcc includes (relative to HIPAcc binary in PATH)
HIPACC_INCLUDES += $(subst hipacc,..,$(shell which hipacc))/include \
                   $(subst hipacc,..,$(shell which hipacc))/include/dsl

ifeq ($(C),1)
  # Clean generated files
  HIPACC_CLEAN_RESULT := $(shell rm $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/*)
else
  # Call HIPAcc to generate Renderscript sources
  $(foreach SRC,$(HIPACC_SRC_FILES), \
  	HIPACC_RS_RESULT := $(shell cd $(LOCAL_PATH)/$(HIPACC_GEN_PATH); \
                                hipacc -emit-renderscript \
                                    -std=c++11 \
                                    $(HIPACC_FLAGS) \
                                    -I/usr/include \
                                    -I$(shell clang -print-file-name=include) \
                                    -I$(shell llvm-config --includedir) \
                                    -I$(shell llvm-config --includedir)/c++/v1 \
                                    $(addprefix -I,$(HIPACC_INCLUDES)) \
                                    $(LOCAL_CPPFLAGS) -DHIPACC \
                                    $(HIPACC_SRC_PATH)/$(SRC) \
                                    -o rs$(SRC));)

  # Call HIPAcc to generate Filterscript sources
  $(foreach SRC,$(HIPACC_SRC_FILES), \
  	HIPACC_FS_RESULT := $(shell cd $(LOCAL_PATH)/$(HIPACC_GEN_PATH); \
                                hipacc -emit-filterscript \
                                    -std=c++11 \
                                    $(HIPACC_FLAGS) \
                                    -I/usr/include \
                                    -I$(shell clang -print-file-name=include) \
                                    -I$(shell llvm-config --includedir) \
                                    -I$(shell llvm-config --includedir)/c++/v1 \
                                    $(addprefix -I,$(HIPACC_INCLUDES)) \
                                    $(LOCAL_CPPFLAGS) -DHIPACC \
                                    $(HIPACC_SRC_PATH)/$(SRC) \
                                    -o fs$(SRC); \
                                sed -i "1i#define FILTERSCRIPT" fs$(SRC));)
endif


################################################################################
# Setup local C/C++ to use HIPAcc specific flags and compile generated files
################################################################################

LOCAL_CPPFLAGS += -DRS_TARGET_API=$(HIPACC_RS_VERSION) -DEXCLUDE_IMPL
LOCAL_RENDERSCRIPT_FLAGS += -allow-rs-prefix -target-api $(HIPACC_RS_VERSION) \
                            $(addprefix -I,$(HIPACC_INCLUDES))
LOCAL_C_INCLUDES += $(HIPACC_INCLUDES) \
                    obj/local/armeabi/objs/$(LOCAL_MODULE)/$(HIPACC_GEN_PATH)
LOCAL_SRC_FILES += hipacc_runtime.cpp \
                   $(subst $(LOCAL_PATH)/,, \
                           $(wildcard $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,, \
                           $(wildcard $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/*.rs)) \
                   $(subst $(LOCAL_PATH)/,, \
                           $(wildcard $(LOCAL_PATH)/$(HIPACC_GEN_PATH)/*.fs))