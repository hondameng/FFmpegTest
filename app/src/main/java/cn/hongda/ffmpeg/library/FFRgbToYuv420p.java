package cn.hongda.ffmpeg.library;

/**
 * RGB转换YUV并保存为文件
 */
public class FFRgbToYuv420p {

    private long mHandle;

    public FFRgbToYuv420p(String outputPath, int width , int height){
        mHandle = initRgbToYuv420P(outputPath,width,height);
    }

    public void pushRgbData(byte[] data){
        pushRgbData(mHandle,data);
    }

    public void release(){
        unInitRgbToYuv420P(mHandle);
    }




    static {
        System.loadLibrary("ffmpeg");
        System.loadLibrary("ffmpegjni");
    }

    private static native  int initRgbToYuv420P(String outputPath, int width , int height);
    private static native  int pushRgbData(long handle,byte[] rgbByte);
    private static native  int unInitRgbToYuv420P(long handle);

}
