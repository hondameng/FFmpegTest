ifeq ($(APP_ABI), x86)
LIB_NAME_PLUS := x86
else
LIB_NAME_PLUS := armeabi
endif

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:= ffmpeg-prebuilt-$(LIB_NAME_PLUS)
LOCAL_SRC_FILES:= prebuilt/$(LIB_NAME_PLUS)/libffmpeg.so
include $(PREBUILT_SHARED_LIBRARY)



include $(CLEAR_VARS)

LOCAL_MODULE := libffmpegjni

ifeq ($(APP_ABI), x86)
TARGET_ARCH:=x86
TARGET_ARCH_ABI:=x86
else
LOCAL_ARM_MODE := arm
endif

LOCAL_SRC_FILES := myutils/utils.c \
                   ffmpegprocess_plus/GetRgbFremer.cpp \
                   ffmpegprocess_plus/RgbToYuv420P.cpp \
                   ffmpegprocess_plus/H264Encoder.cpp \
                   ffmpegprocess_plus/Mp4Muxer.cpp \
                   ffmpeg-jni.cpp

LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog -lz

LOCAL_SHARED_LIBRARIES:= ffmpeg-prebuilt-$(LIB_NAME_PLUS)

LOCAL_C_INCLUDES += -L$(SYSROOT)/usr/include
LOCAL_C_INCLUDES += $(LOCAL_PATH)/ffmpegprocess_plus/include

ifeq ($(APP_ABI), x86)
LOCAL_CFLAGS := -DUSE_X86_CONFIG  -std=c++11
else
LOCAL_CFLAGS := -DUSE_ARM_CONFIG  -std=c++11
endif

include $(BUILD_SHARED_LIBRARY)

