//
// Created by mhd on 2017/8/27.
//

#include "GetRgbFremer.h"

int GetRgbFremer::openInputFile(string fileName)
{
    int ret;
    AVCodec *dec;

    if ((ret = avformat_open_input(&pFormatCtx, fileName.c_str(), NULL, NULL)) < 0)
    {
        LOGI("Cannot open input file\n");
        return ret;
    }

    if ((ret = avformat_find_stream_info(pFormatCtx, NULL)) < 0)
    {
        LOGI("Cannot find stream information\n");
        return ret;
    }

    /* select the video stream */
    ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec, 0);
    if (ret < 0)
    {
        LOGI("Cannot find a video stream in the input file\n");
        return ret;
    }
    videoStreamIndex = ret;
    pCodecCtx = pFormatCtx->streams[videoStreamIndex]->codec;

    /* init the video decoder */
    if ((ret = avcodec_open2(pCodecCtx, dec, NULL)) < 0)
    {
        LOGI("Cannot open video decoder\n");
        return ret;
    }

    return 0;
}

/**
 * 初始化解码
 * @param inputMp4 输入视频的路径
 * @param w 视频宽
 * @param h 视频高
 */
int GetRgbFremer::initGetRgbFrame(string inputmp4, int w, int h)
{
    width = w;
    height = h;
    frameIndex = 0;
    int ret;
    av_register_all();
    avcodec_register_all();

    if ((ret = openInputFile(inputmp4)) < 0)
    {
        LOGI( "input file null\n");
        uninitGetRgbFrame();
        return -1;
    }

    pFrameYUV = avcodec_alloc_frame();


    frameBufferSize = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    pOutBuffer = (uint8_t *)malloc(frameBufferSize);
    avpicture_fill((AVPicture *)pFrameYUV, pOutBuffer, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);

    pSwsContext = sws_getContext(pCodecCtx->width,
                                 pCodecCtx->height,
                                 pCodecCtx->pix_fmt,
                                 pCodecCtx->width,
                                 pCodecCtx->height,
                                 PIX_FMT_RGB24,
                                 SWS_BICUBIC,
                                 NULL, NULL, NULL);
    return 0;
}

/**
 * 从视频中获取一帧
 * @param data 帧数据
 */
int GetRgbFremer::getNextRgbFrame(uint8_t ** data)
{
    int gotFrame;
    int ret;

    while (1){
        ret = av_read_frame(pFormatCtx, &packet);
        if (ret< 0)
            return -1;

        if (packet.stream_index == videoStreamIndex) {
            avcodec_get_frame_defaults(&frame);
            gotFrame = 0;
            ret = avcodec_decode_video2(pCodecCtx, &frame, &gotFrame, &packet);
            if (ret < 0) {
                LOGI( "Error decoding video\n");
                return -1;
            }

            if (gotFrame) {
                sws_scale(
                        pSwsContext,
                        (const uint8_t* const*)((&frame)->data),
                        (&frame)->linesize, 0,
                        pCodecCtx->height,
                        pFrameYUV->data,
                        pFrameYUV->linesize);

                frameIndex ++;
//                LOGI("frameIndex:%d",frameIndex);
                *data = pOutBuffer;
                return 0;

            } else{
                continue;
            }
        } else{
            continue;
        }
    }
}

int GetRgbFremer::uninitGetRgbFrame()
{
    frameIndex = 0;
    int ret;
    if (pCodecCtx)
        avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);

    if(pFrameYUV){
        avformat_free_context(pFormatCtx);
    }
    if (ret < 0 && ret != AVERROR_EOF) {
        char buf[1024];
        av_strerror(ret, buf, sizeof(buf));
        LOGI("Error occurred: %s\n", buf);
        return -1;
    }

    av_free_packet(&packet);
}