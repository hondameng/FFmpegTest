//
// Created by mhd on 2017/8/27.
//

#include "RgbToYuv420P.h"

RgbToYuv420P::RgbToYuv420P()
{
    LOGI("RgbToYuv420P()");
}

RgbToYuv420P::~RgbToYuv420P()
{
    LOGI("~RgbToYuv420P()");
}

/**
 * 初始化RGB YUV转换器
 * @param outfile 输出的YUV文件路径
 * @param w 视频宽度
 * @param h 视频高度
 * @return
 */
int RgbToYuv420P::initRgbToYuv(const char *outFilePath, int w, int h)
{
    width = w;
    height = h;

    // 找到 mpeg1 video encoder
    pCodec = avcodec_find_encoder(CODEC_ID_H264);
    if (!pCodec)
    {
        fprintf(stderr, "pCodec not found\n");
        return -1;
    }

    pCodecContext = avcodec_alloc_context3(pCodec);
    pPictureFrame = avcodec_alloc_frame(); //这里分配了帧

    pCodecContext->bit_rate = 400000 * 2;
    pCodecContext->width = width;
    pCodecContext->height = height;
    pCodecContext->time_base.num = 1;
    pCodecContext->time_base.den = 24;
    pCodecContext->gop_size = 10;
    pCodecContext->max_b_frames = 1;
    pCodecContext->pix_fmt = PIX_FMT_YUV420P;

    pCodecContext->qmin = 1;
    pCodecContext->qmax = 5;


// codec_id == CODEC_ID_H264
    av_opt_set(pCodecContext->priv_data, "preset", "slow", 0);

    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0)
    {
        LOGI( "could not open pCodec\n");
        return -1;
    }

    pOutFile = fopen(outFilePath, "wb");
    if (!pOutFile)
    {
        LOGI("could not open %s\n", outFilePath);
        return -1;
    }

    //为输出buffer申请空间
    outbufSize = 100000 + 12 * pCodecContext->width * pCodecContext->height;

    pSwsContext = sws_getContext(pCodecContext->width, pCodecContext->height, PIX_FMT_RGB24,
                         pCodecContext->width, pCodecContext->height, PIX_FMT_YUV420P, 2, NULL,
                         NULL, NULL);
    return 0;
}

/**
 * 输入RGB数据帧
 * @param pSrcRgb RGB数据帧
 * @return
 */
int RgbToYuv420P::pushRgbData(uint8_t *pSrcRgb)
{
    av_image_alloc(pPictureFrame->data, pPictureFrame->linesize, pCodecContext->width, pCodecContext->height, pCodecContext->pix_fmt, 1);

    uint8_t *rgb_src[3]= {pSrcRgb, NULL, NULL};
    int rgb_stride[3]={3*pCodecContext->width, 0, 0};

    sws_scale(pSwsContext, rgb_src, rgb_stride, 0, pCodecContext->height, pPictureFrame->data, pPictureFrame->linesize);

    fwrite(pPictureFrame->data[0],(pCodecContext->width)*(pCodecContext->height),1,pOutFile);
    fwrite(pPictureFrame->data[1],(pCodecContext->width)*(pCodecContext->height)/4,1,pOutFile);
    fwrite(pPictureFrame->data[2],(pCodecContext->width)*(pCodecContext->height)/4,1,pOutFile);
    free(pSrcRgb);
    av_freep(&pPictureFrame->data[0]);
    return 0;
}

/**
 * 释放转换器
 */
int RgbToYuv420P::uninitRgbToYuv()
{
    av_free(pPictureFrame);
    fclose(pOutFile);
    return 0;
}