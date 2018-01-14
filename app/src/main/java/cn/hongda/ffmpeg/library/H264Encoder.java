package cn.hongda.ffmpeg.library;


/**
 * yuv编码成h264
 */
public class H264Encoder {
    private long mHandle = -1;

    public H264Encoder(String inputpath, String outputpath, int width, int height, int frameNum, int fps){
        mHandle = initH264Encoder(inputpath,outputpath,width,height,frameNum,fps);
    }

    public void encode(){
        encode(mHandle);
    }

    public void release(){
        release(mHandle);
    }



    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpegjni");
    }



    private static final native long initH264Encoder(String inputpath, String outputpath, int width, int height, int frameNum, int fps);

    private static final native long encode(long handle);

    private static final native long release(long handle);

}
