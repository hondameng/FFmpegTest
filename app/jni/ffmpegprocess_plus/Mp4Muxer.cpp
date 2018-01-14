//
// Created by mhd on 2017/8/28.
//

#include "Mp4Muxer.h"

/**
 *初始化复合器
 * @param inputH264FileName 输入的H264路径
 * @param inputAacFileName 输入的AAC文件路径
 * @param outMP4FileName 输出的MP4路径
 * @return
 */
Mp4Muxer::Mp4Muxer(const char* inputH264FileName, const char* inputAacFileName, const char* outMP4FileName)
{
    LOGI("Mp4Muxer()");
    initMp4Muxer(inputH264FileName,inputAacFileName,outMP4FileName);
}

Mp4Muxer::~Mp4Muxer()
{
    release();
    LOGI("~Mp4Muxer()");
}

/**
 *初始化复合器
 * @param inputH264FileName 输入的H264路径
 * @param inputAacFileName 输入的AAC文件路径
 * @param outMP4FileName 输出的MP4路径
 * @return
 */
int Mp4Muxer::initMp4Muxer(const char* inputH264FileName, const char* inputAacFileName, const char* outMP4FileName)
{
    isInintOk = false;
    avcodec_register_all();
    av_register_all();
    int ret;

    inAacPath = inputAacFileName;
    inH264Path = inputH264FileName;
    outMp4Path = outMP4FileName;

    pInFormatContextVideo =NULL;
    pInFormatContextAudio =NULL;
    pOutFormatContext = NULL;
    pOutputFormat = NULL;

    //Input
    if ((ret = avformat_open_input(&pInFormatContextAudio,inAacPath,NULL,NULL)) <0) {
        LOGI( "Could not open input file.");
        return ret;
    }
    if ((ret = avformat_find_stream_info(pInFormatContextAudio,0)) <0) {
        LOGI( "Failed to retrieve input audio stream information");
        return ret;
    }

    if ((ret = avformat_open_input(&pInFormatContextVideo, inH264Path,NULL,NULL)) <0) {
        LOGI( "Could not open input file:%d\n",ret);
        return ret;
    }
    if ((ret = avformat_find_stream_info(pInFormatContextVideo,0)) <0) {
        LOGI( "Failed to retrieve input video stream information");
        return ret;
    }

    LOGI("===========Input Information==========\n");
    av_dump_format(pInFormatContextVideo,0, inH264Path,0);
    av_dump_format(pInFormatContextAudio,0, inAacPath,0);
    LOGI("======================================\n");
    //Output
    avformat_alloc_output_context2(&pOutFormatContext,NULL,NULL, outMp4Path);
    if (!pOutFormatContext) {
        LOGI( "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        return ret;
    }
    pOutputFormat = pOutFormatContext->oformat;
    isInintOk = true;
    return 0;
}

/**
 * 执行合成
 */
int Mp4Muxer::muxer()
{
    if(!isInintOk)
    {
        LOGE( "haven't init ok \n");
        return -1;
    }
    int ret;
    int videoindex_v=-1;
    int videoindex_out=-1;
    int audioindex_a=-1;
    int audioindex_out=-1;
    int frame_index=0;
    int64_t cur_pts_v=0,cur_pts_a=0;
    AVPacket pkt;

    for (int i =0; i < pInFormatContextVideo->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        if(pInFormatContextVideo->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO){
            AVStream *in_stream = pInFormatContextVideo->streams[i];
            AVStream *out_stream = avformat_new_stream(pOutFormatContext, in_stream->codec->codec);
            videoindex_v=i;
            if (!out_stream) {
                LOGI( "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                return -1;
            }
            videoindex_out=out_stream->index;
            //Copy the settings of AVCodecContext

//            ret =av_dict_set(&out_stream->metadata,"rotate","90",0);//设置旋转角度

            if (avcodec_copy_context(out_stream->codec, in_stream->codec) <0) {
                LOGI("Failed to copy context from input to output stream codec context\n");
                return -1;
            }
            out_stream->codec->codec_tag =0;
            if (pOutFormatContext->oformat->flags &AVFMT_GLOBALHEADER)
                out_stream->codec->flags |=CODEC_FLAG_GLOBAL_HEADER;
            break;
        }
    }

    for (int i =0; i < pInFormatContextAudio->nb_streams; i++) {
        //Create output AVStream according to input AVStream
        if(pInFormatContextAudio->streams[i]->codec->codec_type==AVMEDIA_TYPE_AUDIO){
            AVStream *in_stream = pInFormatContextAudio->streams[i];
            AVStream *out_stream =avformat_new_stream(pOutFormatContext, in_stream->codec->codec);
            audioindex_a=i;
            if (!out_stream) {
                LOGI( "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                return -1;
            }
            audioindex_out=out_stream->index;
            //Copy the settings of AVCodecContext
            if (avcodec_copy_context(out_stream->codec, in_stream->codec) <0) {
                LOGI( "Failed to copy context from input to output stream codec context\n");
                return -1;
            }
            out_stream->codec->codec_tag =0;
            if (pOutFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
                out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;

            break;
        }
    }

    LOGI("==========Output Information==========\n");
    av_dump_format(pOutFormatContext,0, outMp4Path,1);
    LOGI("======================================\n");
    //Open output file
    if (!(pOutputFormat->flags & AVFMT_NOFILE)) {
        if (avio_open(&pOutFormatContext->pb, outMp4Path, AVIO_FLAG_WRITE) <0) {
            LOGI("Could not open output file '%s'", outMp4Path);
            return -1;
        }
    }
    //Write file header
    if (avformat_write_header(pOutFormatContext,NULL) < 0) {
        LOGI( "Error occurred when opening output file\n");
        return -1;
    }


    //FIX
#if USE_H264BSF
    AVBitStreamFilterContext* h264bsfc =  av_bitstream_filter_init("h264_mp4toannexb");
#endif
#if USE_AACBSF
    AVBitStreamFilterContext* aacbsfc =  av_bitstream_filter_init("aac_adtstoasc");
#endif

    while (1) {
        AVFormatContext *ifmt_ctx;
        int stream_index=0;
        AVStream *in_stream, *out_stream;

        //Get an AVPacket
        if(av_compare_ts(cur_pts_v,pInFormatContextVideo->streams[videoindex_v]->time_base,cur_pts_a,pInFormatContextAudio->streams[audioindex_a]->time_base) <=0){
            ifmt_ctx=pInFormatContextVideo;
            stream_index=videoindex_out;

            if(av_read_frame(ifmt_ctx, &pkt) >=0){
                do{
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = pOutFormatContext->streams[stream_index];

                    if(pkt.stream_index==videoindex_v){
                        //FIX£∫No PTS (Example: Raw H.264)
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }

                        cur_pts_v=pkt.pts;
                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >=0);
            }else{
                break;
            }
        }else{
            ifmt_ctx=pInFormatContextAudio;
            stream_index=audioindex_out;
            if(av_read_frame(ifmt_ctx, &pkt) >=0){
                do{
                    in_stream  = ifmt_ctx->streams[pkt.stream_index];
                    out_stream = pOutFormatContext->streams[stream_index];

                    if(pkt.stream_index==audioindex_a){

                        //FIX£∫No PTS
                        //Simple Write PTS
                        if(pkt.pts==AV_NOPTS_VALUE){
                            //Write PTS
                            AVRational time_base1=in_stream->time_base;
                            //Duration between 2 frames (us)
                            int64_t calc_duration=(double)AV_TIME_BASE/av_q2d(in_stream->r_frame_rate);
                            //Parameters
                            pkt.pts=(double)(frame_index*calc_duration)/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            pkt.dts=pkt.pts;
                            pkt.duration=(double)calc_duration/(double)(av_q2d(time_base1)*AV_TIME_BASE);
                            frame_index++;
                        }
                        cur_pts_a=pkt.pts;

                        break;
                    }
                }while(av_read_frame(ifmt_ctx, &pkt) >=0);
            }else{
                break;
            }

        }

        //FIX:Bitstream Filter
#if USE_H264BSF
        av_bitstream_filter_filter(h264bsfc, in_stream->codec,NULL, &pkt.data, &pkt.size, pkt.data, pkt.size,0);
#endif
#if USE_AACBSF
        av_bitstream_filter_filter(aacbsfc, out_stream->codec,NULL, &pkt.data, &pkt.size, pkt.data, pkt.size,0);
#endif


        //Convert PTS/DTS
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));

        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index=stream_index;

        LOGI("Write 1 Packet. size:%5d\tpts:%lld\n",pkt.size,pkt.pts);
        //Write
        if (av_interleaved_write_frame(pOutFormatContext, &pkt) <0) {
            LOGI( "Error muxing packet\n");
            break;
        }
        av_free_packet(&pkt);

    }
    //Write file trailer
    av_write_trailer(pOutFormatContext);

#if USE_H264BSF
    av_bitstream_filter_close(h264bsfc);
#endif
#if USE_AACBSF
    av_bitstream_filter_close(aacbsfc);
#endif

    LOGI("======muxer mp4 success =====!\n");
    return 0;
}

int Mp4Muxer::release()
{
    avformat_close_input(&pInFormatContextVideo);
    avformat_close_input(&pInFormatContextAudio);
    if (pOutFormatContext && !(pOutputFormat->flags & AVFMT_NOFILE))
        avio_close(pOutFormatContext->pb);
    avformat_free_context(pOutFormatContext);
}