#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include "hipacc_types.hpp"

#define LOG_TAG "libhipacc_filters"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#define GLUENAME HIPAccFilters
#define PACKAGE org_hipacc_example
#define FUNCTION(NAME) Java_org_hipacc_example_HIPAccFilters_ ## NAME

extern int runBlur(int w, int h, uchar4* i, uchar4* o);

extern "C" {

JNIEXPORT int JNICALL
FUNCTION(runBlur)(JNIEnv *env, jobject thiz, int w, int h, jobject i, jobject o) {
	LOGI("In native method\n");

  int ret;
  void *in;
  void *out;
	AndroidBitmapInfo in_info;
	AndroidBitmapInfo out_info;

  if ((ret = AndroidBitmap_getInfo(env, i, &in_info)) < 0 ||
      (ret = AndroidBitmap_getInfo(env, o, &out_info)) < 0) {
    LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
    return 1;
  }

  LOGI("Checking format\n");
  if (in_info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
    LOGE("Input bitmap format is not RGBA_8888 !");
    return 1;
  }
  if (out_info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
    LOGE("Output bitmap format is not RGBA_8888 !");
    return 1;
  }

  LOGI("Locking pixels\n");
  if ((ret = AndroidBitmap_lockPixels(env, i, &in)) < 0 ||
      (ret = AndroidBitmap_lockPixels(env, o, &out)) < 0) {
    LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    return 1;
  }

	LOGI("Running blur\n");
  ret = runBlur(w, h, (uchar4*)in, (uchar4*)out);

	LOGI("Unlocking pixels\n");
  AndroidBitmap_unlockPixels(env, o);
  AndroidBitmap_unlockPixels(env, i);

  return ret;
}

}
