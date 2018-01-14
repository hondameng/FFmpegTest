package cn.hongda.ffmpeg.library;


/**
 * H264、AAC合成MP4
 */
public class Mp4Muxer {
    private long mHandle = -1;

    public Mp4Muxer(String inputH264, String inputAac, String outputMp4){
        mHandle = initMp4Muxer(inputH264,inputAac,outputMp4);
    }

    public void muxer(){
        muxer(mHandle);
    }

    public void release(){
        release(mHandle);
    }



    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpegjni");
    }

    private static final native long initMp4Muxer(String inputH264, String inputAac, String outputMp4);

    private static final native long muxer(long handle);

    private static final native long release(long handle);
}
