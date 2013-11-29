LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hipacc_filters

HIPACC_INCLUDES := $(subst hipacc,..,$(shell which hipacc))/include \
                   $(subst hipacc,..,$(shell which hipacc))/include/dsl

LOCAL_C_INCLUDES := $(SYSROOT_LINK)/usr/include/rs/cpp \
                    $(SYSROOT_LINK)/usr/include/rs \
                    obj/local/armeabi/objs/$(LOCAL_MODULE)/hipacc_gen \
                    $(HIPACC_INCLUDES)

LOCAL_CPPFLAGS += -DRS_TARGET_API=19 -DSIZE_X=5 -DSIZE_Y=5

ifeq ($(CLEAN),true)
  HIPACC_RESULT := $(shell rm $(LOCAL_PATH)/hipacc_gen/*)
else
  HIPACC_RESULT := $(shell cd $(LOCAL_PATH)/hipacc_gen; \
                           hipacc -emit-renderscript -std=c++11 \
                               -I/usr/include \
                               -I$(shell clang -print-file-name=include) \
                               -I$(shell llvm-config --includedir) \
                               -I$(shell llvm-config --includedir)/c++/v1 \
                               $(addprefix -I,$(HIPACC_INCLUDES)) \
                               $(LOCAL_CPPFLAGS) -DHIPACC \
                               ../hipacc_src/blur.cpp -o blur.cc)
endif

LOCAL_SRC_FILES := hipacc_filters.cpp \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/hipacc_gen/*.cc)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/hipacc_gen/*.rs)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/hipacc_gen/*.fs))

LOCAL_LDLIBS := -llog -ljnigraphics \
                -l$(SYSROOT_LINK)/usr/lib/rs/libcutils.so \
                -l$(SYSROOT_LINK)/usr/lib/rs/libRScpp_static.a

LOCAL_RENDERSCRIPT_FLAGS := -allow-rs-prefix -target-api 19 \
                            $(addprefix -I,$(HIPACC_INCLUDES))

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)
