//
// Created by mhd on 2017/8/28.
//

#include "H264Encoder.h"

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
H264Encoder::H264Encoder(string input, string output, int width, int height, int frameN, int fps)
{
    initH264Encoder(input,output,width,height,frameN,fps);
    LOGI("H264Encoder()");
}

H264Encoder::~H264Encoder()
{
    release();
    LOGI("~H264Encoder()");
}

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
int H264Encoder::initH264Encoder(string input, string output, int width, int height, int frameN, int fps)
{
    pInFile = fopen(input.c_str(), "rb");   //Input raw YUV data

    if(pInFile == NULL)
    {
        LOGI("input file open file! \n");
        return -1;
    }

    frameNum=frameN;
    const char* out_file = output.c_str();

    av_register_all();
    pFormatCtx = avformat_alloc_context();
    pOutputFormat = av_guess_format(NULL, out_file, NULL);
    pFormatCtx->oformat = pOutputFormat;

    if (avio_open(&pFormatCtx->pb,out_file, AVIO_FLAG_READ_WRITE) < 0){
        LOGI("Failed to open output file! \n");
        return -1;
    }

    pVideoAVStream = avformat_new_stream(pFormatCtx, 0);
    if (pVideoAVStream==NULL){
        return -1;
    }

    pCodecCtx = pVideoAVStream->codec;
    pCodecCtx->codec_id = pOutputFormat->video_codec;
    pCodecCtx->codec_type = AVMEDIA_TYPE_VIDEO;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;
    pCodecCtx->width = width;
    pCodecCtx->height = height;
    pCodecCtx->bit_rate = width * height * fps * 0.25;   //0.25*width*height*fps
    pCodecCtx->gop_size=250;

    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = fps;

    //H264
    pCodecCtx->qmin = 10;
    pCodecCtx->qmax = 51;

    pCodecCtx->max_b_frames=3;

    AVDictionary *param = 0;

    //H.264
    if(pCodecCtx->codec_id == AV_CODEC_ID_H264) {
        av_dict_set(&param, "preset", "slow", 0);
        av_dict_set(&param, "tune", "zerolatency", 0);
    }
    //H.265
    if(pCodecCtx->codec_id == AV_CODEC_ID_H265){
        av_dict_set(&param, "preset", "ultrafast", 0);
        av_dict_set(&param, "tune", "zero-latency", 0);
    }

    //Show some Information
    av_dump_format(pFormatCtx, 0, out_file, 1);

    pCodec = avcodec_find_encoder(pCodecCtx->codec_id);
    if (!pCodec){
        LOGI("Can not find encoder! \n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec,&param) < 0){
        LOGI("Failed to open encoder! \n");
        return -1;
    }

    pFrame = av_frame_alloc();
    pictureSize = avpicture_get_size(pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);
    pPictureBuffer = (uint8_t *)av_malloc(pictureSize);
    avpicture_fill((AVPicture *)pFrame, pPictureBuffer, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height);

    //Write File Header
    avformat_write_header(pFormatCtx,NULL);

    av_new_packet(&packet,pictureSize);

    ySize = pCodecCtx->width * pCodecCtx->height;
}

/**
 * 执行编码
 */
int H264Encoder::encode()
{
    for (int i = 0; i<frameNum; i++){
        //Read raw YUV data
        if (fread(pPictureBuffer, 1, ySize*3/2, pInFile) <= 0){
            LOGI("Failed to read raw data! \n");
            return -1;
        }else if(feof(pInFile)){
            LOGI("H264Encoder::encode() feof! \n");
            break;
        }
        pFrame->data[0] = pPictureBuffer;              // Y
        pFrame->data[1] = pPictureBuffer+ ySize;      // U
        pFrame->data[2] = pPictureBuffer+ ySize*5/4;  // V
        //PTS
        //pFrame->pts=i;
        pFrame->pts=i*(pVideoAVStream->time_base.den)/((pVideoAVStream->time_base.num)*25);
        int got_picture=0;

        int ret = avcodec_encode_video2(pCodecCtx, &packet,pFrame, &got_picture);
        if(ret < 0){
            LOGI("Failed to encode! \n");
            return -1;
        }
        if (got_picture==1){
            LOGI("Succeed to encode frame: %5d\tsize:%5d\n",frameIndex,packet.size);
            frameIndex++;
            packet.stream_index = pVideoAVStream->index;
            ret = av_write_frame(pFormatCtx, &packet);
            av_free_packet(&packet);
        }
    }

    return 0;
}

/**
 * 释放
 * @return
 */
int H264Encoder::release()
{
    //Write file trailer
    av_write_trailer(pFormatCtx);

    //Clean
    if (pVideoAVStream){
        avcodec_close(pVideoAVStream->codec);
        av_free(pFrame);
        av_free(pPictureBuffer);
    }
    avio_close(pFormatCtx->pb);
    avformat_free_context(pFormatCtx);

    fclose(pInFile);
}
