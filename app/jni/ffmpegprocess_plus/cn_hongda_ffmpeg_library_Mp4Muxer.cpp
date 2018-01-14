//
// Created by mhd on 2017/9/6.
//

#include <jni.h>
#include <android/log.h>
#include <string>
#include "LogUtil.h"

static const std::string className = "cn/hongda/opengl/opengles/MyGLView";

struct fields_t {
    jfieldID context;
};

static struct fields_t gFields;




static const JNINativeMethod methods[] =
        {
        };

JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JNIEnv *env = NULL;

    if (vm->GetEnv((void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        LOGE("GetEnv failed!");
        return JNI_VERSION_1_4;
    }

    jclass clazz;

    if ((clazz = env->FindClass(className.c_str())) == NULL)
    {
        LOGE("can't find class");
    }

    if (env->RegisterNatives(clazz, methods, sizeof(methods)/sizeof(methods[0])) != JNI_OK)
    {
        LOGE("can't register methods");
    }

    return JNI_VERSION_1_4;
}
