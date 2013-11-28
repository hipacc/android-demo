LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := hipacc_filters

LOCAL_C_INCLUDES := $(SYSROOT_LINK)/usr/include/rs/cpp \
                    $(SYSROOT_LINK)/usr/include/rs \
                    obj/local/armeabi/objs/$(LOCAL_MODULE) \
                    /scratch-local/usr/include
										#$(HIPACC_INCLUDE)

LOCAL_CPPFLAGS += -DRS_TARGET_API=19 -DSIZE_X=5 -DSIZE_Y=5

HIPACC_RESULT := $(shell hipacc -emit-renderscript -std=c++11 \
                             -I$(shell clang -print-file-name=include) \
                             -I$(shell llvm-config --includedir) \
                             -I$(shell llvm-config --includedir)/c++/v1 \
                             -I/usr/include \
                             -I/scratch-local/usr/include/dsl \
                             $(LOCAL_CPPFLAGS) -DHIPACC \
                             jni/hipacc_src/blur.cpp -o jni/blur.cc)

LOCAL_SRC_FILES := hipacc_filters.cpp \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.cc)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.rs)) \
                   $(subst $(LOCAL_PATH)/,,$(wildcard $(LOCAL_PATH)/*.fs))

LOCAL_LDLIBS := -llog -ljnigraphics \
                -l$(SYSROOT_LINK)/usr/lib/rs/libcutils.so \
                -l$(SYSROOT_LINK)/usr/lib/rs/libRScpp_static.a

LOCAL_RENDERSCRIPT_FLAGS := -allow-rs-prefix -target-api 19 \
                            -I/scratch-local/usr/include
                            #-I$(HIPACC_INCLUDE)

LOCAL_ARM_MODE := arm

include $(BUILD_SHARED_LIBRARY)
