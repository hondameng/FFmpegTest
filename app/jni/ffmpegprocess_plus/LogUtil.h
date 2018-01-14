//
// Created by mhd on 2017/8/27.
//

#ifndef LOGUTIL_H
#define LOGUTIL_H

#include <android/log.h>

#define LOG_TAG "mhd_ffmpeg"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#endif //LOGUTIL_H
