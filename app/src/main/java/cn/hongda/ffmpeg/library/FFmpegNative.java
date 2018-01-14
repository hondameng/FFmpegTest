package cn.hongda.ffmpeg.library;

public class FFmpegNative {

	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("ffmpegjni");
	}

	public static native  int yuv2H264(String input , String output, int width, int height, int frameNum, int fps);
	public static native  int muxerToMp4(String inputH264 , String inputAac , String outputMp4);


	//rgb
	public static native  long initGetRgbFrame(String inputMp4, int width , int height);
	public static native  byte[] getNextRgbFrame(long handle);
	public static native  int unInitGetRgbFrame(long handle);

	//rgb to yuv
	public static native  long initRgbToYuv420P(String outputPath, int width , int height);
	public static native  int pushRgbData(long handle,byte[] rgbByte);
	public static native  int unInitRgbToYuv420P(long handle);

}
