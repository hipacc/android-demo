#include <jni.h>
#include <android/log.h>
#include <android/bitmap.h>
#include "hipacc_types.hpp"

#define LOG_TAG "libfilters"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

// Creates JNI function name
#define CREATE_NAME(NAME) Java_org_hipacc_demo_HIPAccFilters_ ## NAME

// Initializes dimensions and pixels pointer from bitmaps
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

// Deinitializes pixels pointer from bitmaps
void deinit(JNIEnv *env, jobject in, jobject out) {
  LOGI("Unlocking pixels\n");
  AndroidBitmap_unlockPixels(env, out);
  AndroidBitmap_unlockPixels(env, in);
}

// Creates wrapper function to call filter from JNI
#define CREATE_FUNCTION(NAME) \
extern int run ## NAME(int w, int h, uchar4 *in, uchar4 *out); \
 \
extern "C" { \
JNIEXPORT int JNICALL \
CREATE_NAME(run ## NAME)(JNIEnv *env, jobject thiz, jobject in, jobject out) { \
  LOGI("In native method\n"); \
 \
  int width; \
  int height; \
  void *pin; \
  void *pout; \
 \
  int ret = init(env, &width, &height, &pin, &pout, in, out); \
 \
  if (ret == 0) { \
    LOGI("Running " #NAME "\n"); \
    ret = run ## NAME(width, height, (uchar4*)pin, (uchar4*)pout); \
  } \
 \
  deinit(env, in, out); \
 \
  return ret; \
} \
}

// Creates Renderscript and Filterscript version of filter
#define CREATE_FILTER(NAME) \
    CREATE_FUNCTION(RS ## NAME) \
    CREATE_FUNCTION(FS ## NAME)

// Create filters
CREATE_FILTER(Blur)
CREATE_FILTER(Gaussian)
CREATE_FILTER(Laplace)
CREATE_FILTER(Sobel)
CREATE_FILTER(Harris)
