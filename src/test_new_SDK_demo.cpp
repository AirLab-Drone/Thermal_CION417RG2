#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <string>

extern "C"{
#include "guidemt.h"
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
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <libudev.h>
}


#define WIDTH 384
#define HEIGHT 288

/* --------------------------------- 熱像儀info -------------------------------- */
const std::string vendorId = "04b4";
const std::string productId = "f9f9";
const std::string deviceDescription = "Cypress Semiconductor Corp. GuideCamera";


/* ---------------------------------- 函式宣告 ---------------------------------- */
int frameCallBack(guide_usb_frame_data_t *pVideoData);
int connectStatusCallBack(guide_usb_device_status_e deviceStatus);
cv::Mat convertYUV422ToBGR(const short* yuv422Data);
cv::Mat convertRGBToMat(const unsigned char* rgbData);
cv::Mat convertY16ToGray(const short* y16Data);
std::string Find_Thermal_Device();
bool comparePaths(const std::string& path1, const std::string& path2);

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
    float* pTemper;                     // 浮點數陣列指標
    int pTemper_length;                 // 浮點數陣列長度

    // 構造函式
    FrameData() : frame_width(0), frame_height(0),
                  frame_rgb_data(nullptr), frame_rgb_data_length(0),
                  frame_src_data(nullptr), frame_src_data_length(0),
                  frame_yuv_data(nullptr), frame_yuv_data_length(0),
                  paramLine(nullptr), paramLine_length(0),
                  pTemper(nullptr), pTemper_length(0) {}

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
        if (pTemper != nullptr) {
            delete[] pTemper;
        }
    }
};


FrameData frameData; // 存放熱像儀的輸出資料
guide_measure_external_param_t *measureExternalParam; //熱像儀參數設定
unsigned char *paramline = NULL;



int main(void)
{
    int ret=0;
    guide_usb_setloglevel(LOG_INFO);

    std::string device_path = Find_Thermal_Device();

    if (!device_path.empty()){
        std::cout << "Found Device Path: " << device_path << std::endl;
        ret = guide_usb_initial(device_path.c_str());

        if(ret < 0)
        {   
            std::cerr << "Initial fail:" << ret << std::endl;
            return -1;
        }

        guide_usb_setpalette(8);


        if (paramline == NULL)
        {
            paramline = (unsigned char *)malloc(WIDTH * 2);
        }

        measureExternalParam = (guide_measure_external_param_t *)malloc(sizeof(guide_measure_external_param_t));
        measureExternalParam->emiss = 98;
        // measureExternalParam->distance = 50;
        measureExternalParam->relHum = 60;
        measureExternalParam->atmosphericTemper = 230;
        measureExternalParam->reflectedTemper = 230;
        measureExternalParam->modifyK = 100;
        measureExternalParam->modifyB = 0;



    /* ------------------------------- video mode ------------------------------- */

        // X16 = 0,                         //X16
        // X16_PARAM = 1,                   //X16+参数行
        // Y16= 2,							//Y16
        // Y16_PARAM = 3,					//Y16+参数行
        // YUV =4,							//YUV
        // YUV_PARAM = 5,				    //YUV+参数行
        // Y16_YUV = 6,						//Y16+YUV
        // Y16_PARAM_YUV = 7				//Y16+参数行+YUV



        guide_usb_device_info_t* deviceInfo = (guide_usb_device_info_t*)malloc(sizeof (guide_usb_device_info_t));
        deviceInfo->width = WIDTH;
        deviceInfo->height = HEIGHT;
        deviceInfo->video_mode = Y16_PARAM_YUV;

        ret = guide_usb_openstream(deviceInfo, (OnFrameDataReceivedCB)frameCallBack, (OnDeviceConnectStatusCB)connectStatusCallBack);
        if(ret < 0)
        {
            std::cerr << "Open fail!" << ret << std::endl;
            delete deviceInfo;
            return ret;
        }

        while(!exitLoop)
        {   
            // usleep(10);
        }

        ret = guide_usb_closestream();
        std::cout << "close usb return" << ret << std::endl;

        ret = guide_usb_exit();
        std::cout << "exit return" << ret << std::endl; 


        if (paramline != NULL)
        {
            free(paramline);
            paramline = NULL;
        }

        delete deviceInfo;
        return ret;
    }
    return ret;
}

int connectStatusCallBack(guide_usb_device_status_e deviceStatus)
{
    if(deviceStatus == DEVICE_CONNECT_OK)
    {
        std::cout << "VideoStream is Staring..." << std::endl;
    }
    else
    {
        std::cout << "VideoStream is Closing..." << std::endl;
    }
}





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
    frameData.pTemper = (float *)malloc(sizeof(float) * WIDTH * HEIGHT);


    if (frameData.paramLine != NULL) {
        memcpy(paramline, frameData.paramLine, frameData.paramLine_length);
    }


    // std::cout << frameData.frame_rgb_data_length << std::endl;

    // std::cout << typeid(frameData.paramLine).name() << std::endl;


    // for (int i = 0; i < frameData.paramLine_length; ++i) {
    //     std::cout << frameData.paramLine[i] << ", ";
    // }
    // std::cout << std::endl;


    guide_measure_convertgray2temper(1, 1, frameData.frame_src_data, frameData.paramLine,
        WIDTH*HEIGHT, measureExternalParam, frameData.pTemper);

    std::cout << "first pix temp: " << frameData.pTemper[0] <<
printf("first pix temp-------------------------%.1f\n", pTemper[0]);
    
    cv::Mat rgbImage = convertRGBToMat(frameData.frame_rgb_data);
    cv::Mat Y16_img = convertY16ToGray(frameData.frame_src_data);
    cv::Mat YUVImage = convertYUV422ToBGR(frameData.frame_yuv_data);

    // cv::imshow("Thermal Image", YUVImage);

    cv::imshow("RGB Image", rgbImage);
    cv::imshow("Y16 Image", Y16_img);
    cv::imshow("YUV Image", YUVImage);




    int key = cv::waitKey(1);
        if (key == 27) { // ESC 鍵的 ASCII 碼為 27
            exitLoop = true;
        }


}


cv::Mat convertYUV422ToBGR(const short* yuv422Data) {
    cv::Mat yuvImage(HEIGHT, WIDTH, CV_8UC2, (void*)yuv422Data);
    cv::Mat bgrImage;
    cv::cvtColor(yuvImage, bgrImage, cv::COLOR_YUV2BGR_YUYV); // 使用OpenCV函數將YUV轉換為BGR
    return bgrImage;
}

cv::Mat convertRGBToMat(const unsigned char* rgbData) {
    cv::Mat rgbImage(HEIGHT, WIDTH, CV_8UC3, (void*)rgbData);
    return rgbImage.clone(); // 返回複製的影像以避免內存問題
}


cv::Mat convertY16ToGray(const short* y16Data) {
    cv::Mat grayImage(HEIGHT, WIDTH, CV_16UC1, (void*)y16Data);
    // Convert to 8-bit grayscale image
    cv::Mat gray8Bit;
    grayImage.convertTo(gray8Bit, CV_8U, 255.0 / 65535.0); // Scale to 0-255
    return gray8Bit;
}


std::string Find_Thermal_Device() {

    struct udev *udev = udev_new();
    if (!udev) {
        std::cerr << "无法初始化 udev" << std::endl;
        return "";
    }

    struct udev_enumerate *enumerate = udev_enumerate_new(udev);
    if (!enumerate) {
        std::cerr << "无法创建 udev 枚举器" << std::endl;
        udev_unref(udev);
        return "";
    }

    // 添加匹配规则：使用设备的厂商 ID、产品 ID 和描述信息
    udev_enumerate_add_match_property(enumerate, "ID_VENDOR_ID", vendorId.c_str());
    udev_enumerate_add_match_property(enumerate, "ID_MODEL_ID", productId.c_str());
    udev_enumerate_add_match_property(enumerate, "ID_MODEL", deviceDescription.c_str());
    udev_enumerate_scan_devices(enumerate);

    struct udev_list_entry *devices = udev_enumerate_get_list_entry(enumerate);
    struct udev_list_entry *entry;

    std::string maxVideoPath = "";

    udev_list_entry_foreach(entry, devices) {
        const char *path = udev_list_entry_get_name(entry);
        struct udev_device *device = udev_device_new_from_syspath(udev, path);

        const char *deviceName = udev_device_get_sysname(device);
        if (deviceName) {
            const char *devicePath = udev_device_get_devnode(device);
            if (devicePath && std::string(deviceName).find("video") != std::string::npos) {
                std::cout << "设备名称: " << deviceName << std::endl;
                std::cout << "设备路径: " << devicePath << std::endl;
                if (maxVideoPath.empty() || comparePaths(devicePath, maxVideoPath)) {
                    maxVideoPath = devicePath;
                }
            }
        }
        udev_device_unref(device);
    }

    if (!maxVideoPath.empty()) {
        udev_enumerate_unref(enumerate);
        udev_unref(udev);
        return maxVideoPath;
    }

    std::cout << "未找到 Cypress Semiconductor Corp. GuideCamera 的视频设备" << std::endl;

    udev_enumerate_unref(enumerate);
    udev_unref(udev);

    return "";
}


bool comparePaths(const std::string& path1, const std::string& path2) {
    size_t pos1 = path1.find_last_of("/");
    size_t pos2 = path2.find_last_of("/");
    if (pos1 == std::string::npos || pos2 == std::string::npos) {
        return false;
    }
    std::string path1Suffix = path1.substr(pos1 + 1);
    std::string path2Suffix = path2.substr(pos2 + 1);
    size_t numPos1 = path1Suffix.find("video");
    size_t numPos2 = path2Suffix.find("video");
    if (numPos1 == std::string::npos || numPos2 == std::string::npos) {
        return false;
    }
    int num1 = std::stoi(path1Suffix.substr(numPos1 + 5));
    int num2 = std::stoi(path2Suffix.substr(numPos2 + 5));
    return num1 < num2;
}

