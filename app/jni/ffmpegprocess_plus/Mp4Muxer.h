//
// Created by mhd on 2017/8/28.
//
#include <string>

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
};
#else
//Linux...
#ifdef __cplusplus

//解决FFmpeg在c++下编译问题
extern "C"
{
#ifdef __cplusplus
#define __STDC_CONSTANT_MACROS
#ifdef _STDINT_H
#undef _STDINT_H
#endif
# include <stdint.h>
#endif
}

extern "C"
{
#endif

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#ifdef __cplusplus
};
#endif
#endif


#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,"Mp4Muxer",__VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,"Mp4Muxer",__VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,"Mp4Muxer",__VA_ARGS__)


/*
 FIX: H.264 in some container format (FLV, MP4, MKV etc.) need
 "h264_mp4toannexb" bitstream filter (BSF)
 *Add SPS,PPS in front of IDR frame
 *Add start code ("0,0,0,1") in front of NALU
 H.264 in some container (MPEG2TS) don't need this BSF.
 */
//'1': Use H.264 Bitstream Filter
#define USE_H264BSF 1

/*
 FIX:AAC in some container format (FLV, MP4, MKV etc.) need
 "aac_adtstoasc" bitstream filter (BSF)
 */
//'1': Use AAC Bitstream Filter
#define USE_AACBSF 1

using namespace std;

#ifndef MP4MUXER_H
#define MP4MUXER_H


class Mp4Muxer
{
private:
    AVOutputFormat *pOutputFormat;
    AVFormatContext *pInFormatContextAudio;
    AVFormatContext *pInFormatContextVideo;
    AVFormatContext *pOutFormatContext;

    const char *inH264Path;
    const char *inAacPath;
    const char *outMp4Path;

    bool isInintOk;

    /**
     *初始化复合器
     * @param inputH264FileName 输入的H264路径
     * @param inputAacFileName 输入的AAC文件路径
     * @param outMP4FileName 输出的MP4路径
     * @return
     */
    int initMp4Muxer(const char* inputH264FileName,const char* inputAacFileName,const char* outMP4FileName );
    int release();
public:

    /**
     *初始化复合器
     * @param inputH264FileName 输入的H264路径
     * @param inputAacFileName 输入的AAC文件路径
     * @param outMP4FileName 输出的MP4路径
     * @return
     */
    Mp4Muxer(const char* inputH264FileName,const char* inputAacFileName,const char* outMP4FileName);

    ~Mp4Muxer();
    int muxer();
};


#endif //MP4MUXER_H
