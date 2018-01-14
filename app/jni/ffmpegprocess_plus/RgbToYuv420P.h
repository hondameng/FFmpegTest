//
// Created by mhd on 2017/8/27.
//

#include <math.h>
#include <string>

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
#include "libavutil/imgutils.h"
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "libswscale/swscale.h"
#include <libavformat/avformat.h>
}

#include "LogUtil.h"
#include "include/libavcodec/avcodec.h"
#include "include/libavutil/frame.h"
#include "include/libavutil/imgutils.h"
#include "include/libswscale/swscale.h"
#include "include/libavutil/opt.h"

using namespace std;

#ifndef RGBTOYUN420P_H
#define RGBTOYUN420P_H

//rgb yuv转换器
class RgbToYuv420P
{
private:
    AVCodec *pCodec;
    AVCodecContext *pCodecContext = NULL;
    struct SwsContext *pSwsContext;
    AVFrame *pPictureFrame;     //转换后的YUV帧

    FILE *pOutFile;
    int outbufSize;
    int width;
    int height;

public:
    RgbToYuv420P();

    ~RgbToYuv420P();

    /**
     * 初始化RGB YUV转换器
     * @param outfile 输出的YUV文件路径
     * @param w 视频宽度
     * @param h 视频高度
     * @return
     */
    int initRgbToYuv(const char *outfile, int w, int h);

    /**
     * 输入RGB数据帧
     * @param srcRgb RGB数据帧
     * @return
     */
    int pushRgbData(uint8_t *srcRgb);

    /**
     * 释放转换器
     */
    int uninitRgbToYuv();
};


#endif //RGBTOYUN420P_H
