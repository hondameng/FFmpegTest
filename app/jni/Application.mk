# $Id: Application.mk 212 2015-05-15 10:22:36Z oparviai $
#
# Build library bilaries for all supported architectures
#

APP_ABI := armeabi-v7a
APP_OPTIM := release
APP_STL := stlport_static
#APP_STL := gnustl_shared  #shared_ptr用到
APP_CPPFLAGS := -fexceptions # -D SOUNDTOUCH_DISABLE_X86_OPTIMIZATIONS

NDK_TOOLCHAIN_VERSION := 4.8
APP_CPPFLAGS += -std=c++0x
APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti