package cn.hongda.ffmpeg.library;

import android.util.Log;

public class FFmpegNativeHelper {
	public FFmpegNativeHelper() {
	}
	
	static {
		System.loadLibrary("ffmpeg");
		System.loadLibrary("ffmpegjni");
	}

	// success 0, error 1
	public static String runCommand(String command) {
		if(command == null || command.length() == 0) {
            return "Command can`t be empty.";
        }

		String[] args = command.split(" ");
		for(int i = 0; i < args.length; i++) {
			Log.d("ffmpeg-jni", args[i]);
		}
		try {
			return ffmpeg_run(args);
		} catch (Exception e) {
			throw e;
		}
	}

	// Deprecated, no use anymore
    public static int init() {
        //return ffmpeg_init();
		return 0;
    }

	// Deprecated, no use anymore
    public static int uninit() {
        //return ffmpeg_uninit();
		return 0;
    }

	private static native int ffmpeg_init();
	private static native int ffmpeg_uninit();
	private static native String ffmpeg_run(String[] args);


    public static native  int mp4Filter(String inputMp4 , String inputFilter , String outputYuv);
    public static native  int ffmpegDemuxer(String input , String outputH264, String outputAudio);
    public static native  int getAacFromVideo(String input , String outputAudio);
    public static native  int getH264FromVideo(String input , String outputH264);
    public static native  int yuv2H264(String input , String output, int width, int height, int frameNum, int fps);
    public static native  int muxerToMp4(String inputH264 , String inputAac , String outputMp4);

	public static native  int mp4ToYuv(String inputMp4);

	//yuv
	public static native  int initGetYuvFrame(String inputMp4, int width , int height);
	public static native  byte[] getNextYuvFrame(int size);
    public static native  int unInitGetYuvFrame();

	//rgb
	public static native  int initGetRgbFrame(String inputMp4, int width , int height);
	public static native  byte[] getNextRgbFrame(int size);
    public static native  int unInitGetRgbFrame();


	//rgb to yuv
	public static native  int initRgbToYuv420P(String outputPath, int width , int height);
	public static native  int rgbToYuv420P(byte[] rgbByte);
	public static native  int unInitRgbToYuv420P();





}
