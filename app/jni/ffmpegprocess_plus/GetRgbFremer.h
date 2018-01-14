//
// Created by mhd on 2017/8/27.
//

#include <ctype.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <string>

#if HAVE_SYS_RESOURCE_H
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#elif HAVE_GETPROCESSTIMES
#include <windows.h>
#endif
#if HAVE_GETPROCESSMEMORYINFO
#include <windows.h>
#include <psapi.h>
#endif

#if HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#if HAVE_TERMIOS_H
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#elif HAVE_KBHIT
#include <conio.h>
#endif

#include <time.h>

#if HAVE_ISATTY
#if HAVE_IO_H
#include <io.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif

#if HAVE_ISATTY
#if HAVE_IO_H
#include <io.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#endif

#if HAVE_TERMIOS_H
#include <termios.h>
#elif HAVE_KBHIT
#include <conio.h>
#endif

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
#include "libavformat/avformat.h"
#include "libavfilter/avcodec.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersrc.h"
#include "libavfilter/buffersink.h"
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}


#include "LogUtil.h"

using namespace std;


#ifndef GETRGBFREMER_H
#define GETRGBFREMER_H

//这个是一个能从MP4解码视频帧为RGB格式的东西
class GetRgbFremer
{
private:
    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    struct SwsContext *pSwsContext;
    AVPacket packet;
    AVFrame frame;
    AVFrame *pFrameYUV;

    int videoStreamIndex;

    int openInputFile(string fileName);

public:
    uint8_t *pOutBuffer;       //输出的RGB buffer
    int frameIndex;           //当前取到第几帧
    int frameBufferSize;      //RGB帧buffer大小
    int width;
    int height;

    /**
     * 初始化解码
     * @param inputMp4 输入视频的路径
     * @param w 视频宽
     * @param h 视频高
     */
    int initGetRgbFrame(string inputMp4, int w, int h);

    /**
     * 从视频中获取一帧
     * @param data 帧数据
     */
    int getNextRgbFrame(uint8_t ** data);

    int uninitGetRgbFrame();

};


#endif //GETRGBFREMER_H
