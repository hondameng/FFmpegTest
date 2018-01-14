#include "utils.h"

typedef struct BITMAPFILEHEADER
{
    u_int16_t bfType;
    u_int32_t bfSize;
    u_int16_t bfReserved1;
    u_int16_t bfReserved2;
    u_int32_t bfOffBits;
}BITMAPFILEHEADER;

typedef struct BITMAPINFOHEADER
{
    u_int32_t biSize;
    u_int32_t biWidth;
    u_int32_t biHeight;
    u_int16_t biPlanes;
    u_int16_t biBitCount;
    u_int32_t biCompression;
    u_int32_t biSizeImage;
    u_int32_t biXPelsPerMeter;
    u_int32_t biYPelsPerMeter;
    u_int32_t biClrUsed;
    u_int32_t biClrImportant;
}BITMAPINFOHEADER;

void saveFrame(uint8_t *pRGBBuffer, int iFrame, int width, int height)
{
    BITMAPFILEHEADER bmpheader;
    BITMAPINFOHEADER bmpinfo;
    FILE *fp;

    unsigned int uiTmp, uiTmp2;
    unsigned char *ucTmp = NULL;
    unsigned char ucRGB;
    int i;

    uiTmp = (width*3+3)/4*4*height;
    uiTmp2 = width*height*3;

    char szFilename[1024];
    sprintf(szFilename, "/mnt/sdcard/1/test%d.bmp",  iFrame);//图片名字为视频名+号码
    fp=fopen(szFilename, "wb");
    if(fp==NULL)
            return;

    //文件标识"BM"（即0x4D42）表示位图
    bmpheader.bfType = 0x4D42;
    //保留。设置为0
    bmpheader.bfReserved1 = 0;
    //保留。设置为0
    bmpheader.bfReserved2 = 0;
    //从文件开始到位图数据的偏移量（单位：字节）
    bmpheader.bfOffBits = sizeof(bmpheader) + sizeof(bmpinfo);
    //整个文件的大小（单位：字节）
    bmpheader.bfSize = bmpheader.bfOffBits + uiTmp;

    //信息头长度（单位：字节）。典型值为28
    bmpinfo.biSize = 0x28;
    //位图宽度（单位：像素）
    bmpinfo.biWidth = width;
    //位图高度（单位：像素）。若其为正，表示倒向的位图。若为负，表示正向的位图
    bmpinfo.biHeight = height;
    //位图的面数（为1）
    bmpinfo.biPlanes = 1;
    //每个像素的位数
    bmpinfo.biBitCount = 24;
    //压缩说明。0(BI_RGB)表示不压缩
    bmpinfo.biCompression = 0;
    //用字节数表示的位图数据的大小（为4的位数）
    bmpinfo.biSizeImage = uiTmp;
    //水平分辨率（单位：像素/米）
    bmpinfo.biXPelsPerMeter = 0;
    //垂直分辨率（单位：像素/米）
    bmpinfo.biYPelsPerMeter = 0;
    //位图使用的颜色数
    bmpinfo.biClrUsed = 0;
    //重要的颜色数
    bmpinfo.biClrImportant = 0;

    fwrite(&bmpheader,sizeof(bmpheader),1,fp);
    fwrite(&bmpinfo,sizeof(bmpinfo),1,fp);

    //把图像数据倒置
    uint8_t tmp[width*3];//临时数据
    i = 0;
    for(; i < height/2; i++)
    {
        memcpy(tmp, &(pRGBBuffer[width*i*3]), width*3);
        memcpy(&(pRGBBuffer[width*i*3]), &(pRGBBuffer[width*(height-1-i)*3]), width*3);
        memcpy(&(pRGBBuffer[width*(height-1-i)*3]), tmp, width*3);
    }

    fwrite(pRGBBuffer,width*height*3,1,fp);
    fclose(fp);

    /*struct jpeg_compress_struct jcs;

    // 声明错误处理器，并赋值给jcs.err域
    struct jpeg_error_mgr jem;
    jcs.err = jpeg_std_error(&jem);

    jpeg_create_compress(&jcs);

    char szFilename[1024];
    sprintf(szFilename, "test%d.jpg", iFrame);//图片名字为视频名+号码
    FILE *fp = fopen(szFilename, "wb");
    if(fp == NULL)
        return;

    jpeg_stdio_dest(&jcs, fp);

    jcs.image_width = width;    // 为图的宽和高，单位为像素
    jcs.image_height = height;
    jcs.input_components = 3;   // 在此为1,表示灰度图， 如果是彩色位图，则为3
    jcs.in_color_space = JCS_RGB; //JCS_GRAYSCALE表示灰度图，JCS_RGB表示彩色图像

    jpeg_set_defaults(&jcs);
    jpeg_set_quality (&jcs, 80, true);

    jpeg_start_compress(&jcs, TRUE);

    JSAMPROW row_pointer[1];   // 一行位图
    int row_stride = jcs.image_width * 3;//每一行的字节数,如果不是索引图,此处需要乘以3

    // 对每一行进行压缩
    while (jcs.next_scanline < jcs.image_height) {
        row_pointer[0] = &(pRGBBuffer[jcs.next_scanline * row_stride]);
        jpeg_write_scanlines(&jcs, row_pointer, 1);
    }

    jpeg_finish_compress(&jcs);
    jpeg_destroy_compress(&jcs);

    fclose(fp);
     */
}