extern "C"
{
#include <stdio.h>
#include <sys/types.h>
#include "guideusbcamera.h"
#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include "sys/time.h"
#include "time.h"
#include <pthread.h>
#include <stdbool.h>
#include <fcntl.h>
}

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#define WIDTH 384
#define HEIGHT 288


/* ---------------------------------- 函式宣告 ---------------------------------- */
int frameCallBack(guide_usb_frame_data_t *pVideoData);
int connectStatusCallBack(guide_usb_device_status_e deviceStatus);
// cv::Mat convertYUV422ToBGR(const short* yuv422Data);
// cv::Mat convertRGBToMat(const unsigned char* rgbData);
// cv::Mat convertY16ToGray(const short* y16Data);

bool exitLoop = false;


struct FrameData {
    int frame_width;                    // 圖像寬度
    int frame_height;                   // 圖像高度
    unsigned char* frame_rgb_data;      // RGB 數據
    int frame_rgb_data_length;          // RGB 數據長度
    short* frame_src_data;              // 原始數據 Y16
    int frame_src_data_length;          // 原始數據長度
    short* frame_yuv_data;              // YUV422 數據
    int frame_yuv_data_length;          // YUV422 數據長度
    short* paramLine;                   // 參數行
    int paramLine_length;               // 參數行長度

    // 構造函式
    FrameData() : frame_width(0), frame_height(0),
                  frame_rgb_data(nullptr), frame_rgb_data_length(0),
                  frame_src_data(nullptr), frame_src_data_length(0),
                  frame_yuv_data(nullptr), frame_yuv_data_length(0),
                  paramLine(nullptr), paramLine_length(0) {}

    // 解構函式
    ~FrameData() {
        // 釋放記憶體
        if (frame_rgb_data != nullptr) {
            delete[] frame_rgb_data;
        }
        if (frame_src_data != nullptr) {
            delete[] frame_src_data;
        }
        if (frame_yuv_data != nullptr) {
            delete[] frame_yuv_data;
        }
        if (paramLine != nullptr) {
            delete[] paramLine;
        }
    }
};


FrameData frameData;


int main(void)
{

    guide_usb_setloglevel(LOG_INFO);
    int ret = guide_usb_initial("/dev/video0");
    if(ret < 0)
    {
        printf("Initial fail:%d \n",ret);
        return -1;
    }

    guide_usb_setpalette(5);

    guide_usb_device_info_t* deviceInfo = (guide_usb_device_info_t*)malloc(sizeof (guide_usb_device_info_t));
    deviceInfo->width = WIDTH;
    deviceInfo->height = HEIGHT;
    deviceInfo->video_mode = Y16_PARAM_YUV;

    ret = guide_usb_openstream(deviceInfo, (OnFrameDataReceivedCB)frameCallBack, (OnDeviceConnectStatusCB)connectStatusCallBack);
    if(ret < 0)
    {
        printf("Open fail! %d \n",ret);
        return ret;
    }

    while(!exitLoop)
    {   
        // usleep(10);
    }

    ret = guide_usb_closestream();
    printf("close usb return %d\n",ret);

    ret = guide_usb_exit();
    printf("exit return %d\n",ret);

    return ret;
}

int connectStatusCallBack(guide_usb_device_status_e deviceStatus)
{
    if(deviceStatus == DEVICE_CONNECT_OK)
    {
        printf("VideoStream is Staring...\n");
    }
    else
    {
        printf("VideoStream is closing...\n");
    }
}


/* ---------------------------------- 數據類型 ---------------------------------- */

// int frame_width;                 //图像宽度
// int frame_height;                //图像高度
// unsigned char* frame_rgb_data;   //rgb 数据
// int frame_rgb_data_length;       //rgb 数据长度
// short* frame_src_data;           //原始数据 y16
// int frame_src_data_length;       //原始数据长度
// short* frame_yuv_data;           //yuv422 数据
// int frame_yuv_data_length;       //yuv422 数据长度
// short* paramLine;                //参数行
// int paramLine_length;            //参数行长度


/* ------------------------------------------------------------------------------ */




int frameCallBack(guide_usb_frame_data_t *pVideoData)
{

    frameData.frame_width = pVideoData->frame_width;
    frameData.frame_height = pVideoData->frame_height;
    frameData.frame_rgb_data = pVideoData->frame_rgb_data;
    frameData.frame_rgb_data_length = pVideoData->frame_rgb_data_length;
    frameData.frame_src_data = pVideoData->frame_src_data;
    frameData.frame_src_data_length = pVideoData->frame_src_data_length;
    frameData.frame_yuv_data = pVideoData->frame_yuv_data;
    frameData.frame_yuv_data_length = pVideoData->frame_yuv_data_length;
    frameData.paramLine = pVideoData->paramLine;
    frameData.paramLine_length = pVideoData->paramLine_length;


    // std::cout << frameData.frame_rgb_data_length << std::endl;

    // std::cout << typeid(frameData.paramLine).name() << std::endl;


    // for (int i = 0; i < frameData.paramLine_length; ++i) {
    //     std::cout << frameData.paramLine[i] << ", ";
    // }
    // std::cout << std::endl;

    
    cv::Mat rgbImage = convertRGBToMat(frameData.frame_rgb_data);
    cv::Mat Y16_img = convertY16ToGray(frameData.frame_src_data);
    cv::Mat YUVImage = convertYUV422ToBGR(frameData.frame_yuv_data);



    cv::imshow("RGB Image", rgbImage);
    cv::imshow("Y16 Image", Y16_img);
    cv::imshow("YUV Image", YUVImage);




    int key = cv::waitKey(1);
        if (key == 27) { // ESC 鍵的 ASCII 碼為 27
            exitLoop = true;
        }


}


// cv::Mat convertYUV422ToBGR(const short* yuv422Data) {
//     cv::Mat yuvImage(HEIGHT, WIDTH, CV_8UC2, (void*)yuv422Data);
//     cv::Mat bgrImage;
//     cv::cvtColor(yuvImage, bgrImage, cv::COLOR_YUV2BGR_YUYV); // 使用OpenCV函數將YUV轉換為BGR
//     return bgrImage;
// }

// cv::Mat convertRGBToMat(const unsigned char* rgbData) {
//     cv::Mat rgbImage(HEIGHT, WIDTH, CV_8UC3, (void*)rgbData);
//     return rgbImage.clone(); // 返回複製的影像以避免內存問題
// }


// cv::Mat convertY16ToGray(const short* y16Data) {
//     cv::Mat grayImage(HEIGHT, WIDTH, CV_16UC1, (void*)y16Data);
//     // Convert to 8-bit grayscale image
//     cv::Mat gray8Bit;
//     grayImage.convertTo(gray8Bit, CV_8U2, 255.0 / 65535.0); // Scale to 0-255
//     return gray8Bit;
// }

