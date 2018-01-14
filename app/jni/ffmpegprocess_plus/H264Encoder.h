//
// Created by mhd on 2017/8/28.
//
// Yuv420p编码为H264
//

#include <stdio.h>
#include <string>

#define __STDC_CONSTANT_MACROS

#ifdef _WIN32
//Windows
extern "C"
{
#include "libavutil/opt.h"
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

#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>

#ifdef __cplusplus
};
#endif
#endif

#include "LogUtil.h"

#ifndef H264ENCODER_H
#define H264ENCODER_H

using namespace std;

//H264编码器，
class H264Encoder
{
private:
    AVFormatContext *pFormatCtx;
    AVOutputFormat *pOutputFormat; //输出的format
    AVStream *pVideoAVStream;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVPacket packet;
    uint8_t *pPictureBuffer;
    AVFrame *pFrame;
    int pictureSize;
    int ySize;                      //Y数据大小
    int frameIndex;                 //当前编码帧的索引
    int frameNum;

    FILE *pInFile;   //输入的YUV

    /**
     *初始化编码器
     * @param input 输入的YUV路径
     * @param output 输出的H264路径
     * @param width 视频宽
     * @param height 视频高
     * @param frameNum 要解码的帧数量
     * @param fps 帧率
     * @return
     */
    int initH264Encoder(string input, string output, int width, int height, int frameNum, int fps);

    int release();

public:
    /**
     *初始化编码器
     * @param input 输入的YUV路径
     * @param output 输出的H264路径
     * @param width 视频宽
     * @param height 视频高
     * @param frameNum 要解码的帧数量
     * @param fps 帧率
     * @return
     */
    H264Encoder(string input, string output, int width, int height, int frameNum, int fps);
    ~H264Encoder();

    /**
     * 执行编码
     */
    int encode();

};


#endif //H264ENCODER_H
