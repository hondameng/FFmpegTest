//
// Created by mhd on 2017/8/27.
//
#include <jni.h>
#include <string>
#include <stdexcept>
#include "ffmpegprocess_plus/GetRgbFremer.h"
#include "ffmpegprocess_plus/RgbToYuv420P.h"
#include "ffmpegprocess_plus/H264Encoder.h"
#include "ffmpegprocess_plus/Mp4Muxer.h"

using namespace std;

//从Mp4z中取RGB帧=======================================
//初始化解码器
extern "C" jlong
Java_cn_hongda_ffmpeg_library_FFGetRgbFramer_initGetRgbFrame(JNIEnv *env, jobject thiz,jstring inputMp4,jint width,jint height) {
    const char *pInputMp4 = env->GetStringUTFChars(inputMp4, 0);
    string inputStr(pInputMp4);
    GetRgbFremer *pGetRgbFremer = new GetRgbFremer();
    pGetRgbFremer->initGetRgbFrame(inputStr,width,height);
    return (jlong)(pGetRgbFremer);
}

//每次从MP4中取一帧的RGB数据
extern "C" jbyteArray
Java_cn_hongda_ffmpeg_library_FFGetRgbFramer_getNextRgbFrame(JNIEnv *env,jlong handle, jobject thiz) {
    GetRgbFremer * pGetRgbFremer = (GetRgbFremer *)handle;
    if(pGetRgbFremer == NULL)
    {
        LOGI("no GetRgbFremer instance");
        return NULL;
    }
    uint8_t *respone_byte ;
    pGetRgbFremer->getNextRgbFrame(&respone_byte);
    if(!respone_byte){
        LOGI("respone_byte is null");
        return NULL;
    }
    jbyteArray byteArray =env->NewByteArray(pGetRgbFremer->frameBufferSize);
    env->SetByteArrayRegion(byteArray,0,pGetRgbFremer->frameBufferSize, (jbyte *)respone_byte);
    return byteArray;
}

//释放解码器
extern "C" jint
Java_cn_hongda_ffmpeg_library_FFGetRgbFramer_unInitGetRgbFrame(JNIEnv *env, jobject thiz,jlong handle) {
    GetRgbFremer * pGetRgbFremer = (GetRgbFremer *)handle;
    if(pGetRgbFremer == NULL)
    {
        LOGI("no GetRgbFremer instance");
        return -1;
    }
    pGetRgbFremer->uninitGetRgbFrame();
    delete pGetRgbFremer;
    pGetRgbFremer = NULL;
    return 0;
}
//从Mp4z中取RGB帧========================================


//RGB帧转yuv420p并保存为文件======================================
//初始化RGB YUV转换器
extern "C" jlong
Java_cn_hongda_ffmpeg_library_FFRgbToYuv420p_initRgbToYuv420P(JNIEnv *env, jobject thiz , jstring outputpath,jint width ,jint height) {
    RgbToYuv420P * pRgbToYuv420P = new RgbToYuv420P();
    const char *pOutput = env->GetStringUTFChars( outputpath, 0);
    pRgbToYuv420P->initRgbToYuv(pOutput,width,height);
    return (jlong)pRgbToYuv420P;
}

//往转换器中输送RGB数据
extern "C" jint
Java_cn_hongda_ffmpeg_library_FFRgbToYuv420p_pushRgbData(JNIEnv *env, jobject thiz ,jlong handle, jbyteArray rgbJByte) {
    RgbToYuv420P * pRgbToYuv420P = (RgbToYuv420P*)handle;
    if(pRgbToYuv420P == NULL){
        LOGI("no RgbToYun420P instance");
        return -1;
    }
    jsize len  = env->GetArrayLength(rgbJByte);
    jbyte *jbarray = (jbyte *)malloc(len * sizeof(jbyte));
    if(jbarray == NULL)
    {
        return -1;
    }
    env->GetByteArrayRegion(rgbJByte,0,len,jbarray);
    uint8_t *data = (uint8_t *)jbarray;//uint8_t 就是byte
    if(data == NULL)
    {
        LOGI("data == NULL");
    }
    int ret = pRgbToYuv420P->pushRgbData(data);
    return ret;
}

//释放RGB YUV转换器
extern "C" jint
Java_cn_hongda_ffmpeg_library_FFRgbToYuv420p_unInitRgbToYuv420P(JNIEnv *env, jobject thiz,jlong handle) {
    RgbToYuv420P * pRgbToYuv420P = (RgbToYuv420P*)handle;
    pRgbToYuv420P->uninitRgbToYuv();
    delete pRgbToYuv420P;
    pRgbToYuv420P = NULL;
    return 0;
}
//RGB帧转yuv420p并保存为文件=============================================


//H264编码 （yuv to h264）=============H264Encoder=============================================

//初始化H264编码器
extern "C" jlong
Java_cn_hongda_ffmpeg_library_H264Encoder_initH264Encoder(JNIEnv *env, jobject thiz,
                                                               jstring inputpath,
                                                               jstring outputpath,
                                                               jint width, jint height,
                                                               jint frameNum,
                                                               jint fps) {

    const char *pInputFile = env->GetStringUTFChars(inputpath, 0);
    const char *pOutputFile = env->GetStringUTFChars(outputpath, 0);
    H264Encoder * pH264Encoder = new H264Encoder(pInputFile, pOutputFile, width, height, frameNum,fps);
    env->ReleaseStringUTFChars(inputpath, pInputFile);
    env->ReleaseStringUTFChars(outputpath, pOutputFile);
    return (jlong) pH264Encoder;
}

//执行H264编码
extern "C" jint
Java_cn_hongda_ffmpeg_library_H264Encoder_encode(JNIEnv *env, jobject thiz, jlong handle)
{
    H264Encoder * pH264Encoder = (H264Encoder*) handle;

    try
    {
        pH264Encoder->encode();
    }
    catch (const runtime_error &e)
    {
        const char *err = e.what();
        LOGE("JNI exception in H264Encoder::encodeToH264: %s", err);
        return -1;
    }
}

//释放H264编码器
extern "C" jint
Java_cn_hongda_ffmpeg_library_H264Encoder_release(JNIEnv *env, jobject thiz, jlong handle)
{
    H264Encoder * pH264Encoder = (H264Encoder*) handle;
    delete pH264Encoder;
    pH264Encoder = NULL;
    return 0;
}
//H264编码 （yuv to h264）=====H264Encoder=====================================================


//AAC H264合成MP4=====================================================

//初始化MP4合成器
extern "C" jlong
Java_cn_hongda_ffmpeg_library_Mp4Muxer_initMp4Muxer(JNIEnv *env, jobject thiz,jstring inputH264,jstring inputAac,jstring outputMp4)
{
    const char *pInputH264 = env->GetStringUTFChars(inputH264, 0);
    const char *pInputAac = env->GetStringUTFChars(inputAac, 0);
    const char *pOutputMp4= env->GetStringUTFChars(outputMp4, 0);

    Mp4Muxer *pMp4Muxer = new Mp4Muxer(pInputH264,pInputAac,pOutputMp4);

    return (jlong)pMp4Muxer;
}

//执行MP4合成器
extern "C" jint
Java_cn_hongda_ffmpeg_library_Mp4Muxer_muxer(JNIEnv *env, jobject thiz,jlong handle)
{
    Mp4Muxer *pMp4Muxer = (Mp4Muxer*)handle;

    try
    {
        pMp4Muxer->muxer();
    }
    catch (const runtime_error &e)
    {
        const char *err = e.what();
        LOGE("JNI exception in Mp4Muxer::muxer: %s", err);
        return -1;
    }
    return 0;
}

//释放MP4合成器
extern "C" jint
Java_cn_hongda_ffmpeg_library_Mp4Muxer_release(JNIEnv *env, jobject thiz,jstring inputH264,jlong handle)
{
    Mp4Muxer *pMp4Muxer = (Mp4Muxer*)handle;
    delete pMp4Muxer;
    pMp4Muxer = NULL;
    return 0;
}
//AAC H264合成MP4=======================================================