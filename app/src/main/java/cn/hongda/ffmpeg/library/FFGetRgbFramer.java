package cn.hongda.ffmpeg.library;


/**
 * 从MP4文件中取RGB帧
 */
public class FFGetRgbFramer {
    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpegjni");
    }

    //rgb
    private static native  long initGetRgbFrame(String inputMp4, int width , int height);
    private static native  byte[] getNextRgbFrame(long handle);
    private static native  int unInitGetRgbFrame(long handle);

    private long handle;

    public  FFGetRgbFramer(String inputMp4, int width , int height){
        handle = initGetRgbFrame(inputMp4,width,height);
    }

    public byte[] getNextRgbFrame(){
        byte[] data = getNextRgbFrame(handle);
        return data;
    }
    public int release(){
        unInitGetRgbFrame(handle);
        return 0;
    }
}
