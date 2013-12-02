LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hipacc_filters

HIPACC_INCLUDES := $(subst hipacc,..,$(shell which hipacc))/include \
                   $(subst hipacc,..,$(shell which hipacc))/include/dsl

LOCAL_C_INCLUDES := $(SYSROOT_LINK)/usr/include/rs/cpp \
                    $(SYSROOT_LINK)/usr/include/rs \
                    $(HIPACC_INCLUDES) \
                    obj/local/armeabi/objs/$(LOCAL_MODULE)/hipacc_gen

LOCAL_CPPFLAGS += -DRS_TARGET_API=19 -DSIZE_X=5 -DSIZE_Y=5 -DEXCLUDE_IMPL

HIPACC_SRC := $(subst $(LOCAL_PATH)/hipacc_src/,,\
                      $(wildcard $(LOCAL_PATH)/hipacc_src/*.cpp))

ifeq ($(C),1)
  HIPACC_RESULT := $(shell rm $(LOCAL_PATH)/hipacc_gen/*)
else
  $(foreach SRC,$(HIPACC_SRC), \
  	HIPACC_RESULT := $(shell cd $(LOCAL_PATH)/hipacc_gen; \
                           hipacc -emit-renderscript \
                               -rs-package org.hipacc.example \
                               -std=c++11 \
                               -I/usr/include \
                               -I$(shell clang -print-file-name=include) \
                               -I$(shell llvm-config --includedir) \
                               -I$(shell llvm-config --includedir)/c++/v1 \
                               $(addprefix -I,$(HIPACC_INCLUDES)) \
                               $(LOCAL_CPPFLAGS) -DHIPACC \
                               ../hipacc_src/$(SRC) -o $(SRC));)
endif

LOCAL_SRC_FILES := hipacc_runtime.cpp \
                   hipacc_filters.cpp \
                   $(subst $(LOCAL_PATH)/,, \
                   	       $(wildcard $(LOCAL_PATH)/hipacc_gen/*.cpp)) \
                   $(subst $(LOCAL_PATH)/,, \
                   	       $(wildcard $(LOCAL_PATH)/hipacc_gen/*.rs)) \
                   $(subst $(LOCAL_PATH)/,, \
                           $(wildcard $(LOCAL_PATH)/hipacc_gen/*.fs))

LOCAL_LDLIBS := -llog -ljnigraphics \
                -l$(SYSROOT_LINK)/usr/lib/rs/libcutils.so \
                -l$(SYSROOT_LINK)/usr/lib/rs/libRScpp_static.a

LOCAL_RENDERSCRIPT_FLAGS := -allow-rs-prefix -target-api 19 \
                            $(addprefix -I,$(HIPACC_INCLUDES))

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)
