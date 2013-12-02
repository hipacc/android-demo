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
extern int runGaussian(int w, int h, uchar4* i, uchar4* o);

extern "C" {

int init(JNIEnv *env, int* width, int* height, void **ppin, void **ppout,
         jobject in, jobject out) {
  int ret = 0;
  AndroidBitmapInfo in_info;
  AndroidBitmapInfo out_info;

  if ((ret = AndroidBitmap_getInfo(env, in, &in_info)) < 0 ||
      (ret = AndroidBitmap_getInfo(env, out, &out_info)) < 0) {
    LOGE("AndroidBitmap_getInfo() failed ! error=%d", ret);
    return -1;
  }

  LOGI("Checking dimension\n");
  if (in_info.width != out_info.width ||
      in_info.height != out_info.height) {
    LOGE("Bitmap dimensions do not match!");
    return -1;
  }

  LOGI("Checking format\n");
  if (in_info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
    LOGE("Input bitmap format is not RGBA_8888!");
    return -1;
  }
  if (out_info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
    LOGE("Output bitmap format is not RGBA_8888!");
    return -1;
  }

  LOGI("Locking pixels\n");
  if ((ret = AndroidBitmap_lockPixels(env, in, ppin)) < 0 ||
      (ret = AndroidBitmap_lockPixels(env, out, ppout)) < 0) {
    LOGE("AndroidBitmap_lockPixels() failed ! error=%d", ret);
    return -1;
  }

  *width = in_info.width;
  *height = in_info.height;

  return ret;
}

void deinit(JNIEnv *env, jobject in, jobject out) {
  LOGI("Unlocking pixels\n");
  AndroidBitmap_unlockPixels(env, out);
  AndroidBitmap_unlockPixels(env, in);
}

JNIEXPORT int JNICALL
FUNCTION(runBlur)(JNIEnv *env, jobject thiz, jobject in, jobject out) {
  LOGI("In native method\n");

  int width;
  int height;
  void *pin;
  void *pout;

  int ret = init(env, &width, &height, &pin, &pout, in, out);

  if (ret == 0) {
    LOGI("Running blur\n");
    ret = runBlur(width, height, (uchar4*)pin, (uchar4*)pout);
  }

  deinit(env, in, out);

  return ret;
}

JNIEXPORT int JNICALL
FUNCTION(runGaussian)(JNIEnv *env, jobject thiz, jobject in, jobject out) {
  LOGI("In native method\n");

  int width;
  int height;
  void *pin;
  void *pout;

  int ret = init(env, &width, &height, &pin, &pout, in, out);

  if (ret == 0) {
    LOGI("Running gaussian\n");
    ret = runGaussian(width, height, (uchar4*)pin, (uchar4*)pout);
  }

  deinit(env, in, out);

  return ret;
}

}
