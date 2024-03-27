
/* ---------------------------- 官方新SDK Demo 勿動！！！ --------------------------- */

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
#include "guidemt.h"

// add
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>

#define WIDTH 384
#define HEIGHT 288

int frameCallBack(guide_usb_frame_data_t *pVideoData);
int connectStatusCallBack(guide_usb_device_status_e deviceStatus);

guide_measure_external_param_t *measureExternalParam;
unsigned char *paramline = NULL;
float *pTemper = NULL;

int FrameNum = 0;

static const char *DstDir2Save = "/home/raspi2024/Desktop/IR640_data/"; //

void SaveRawImage(char *nImageName, unsigned char *ImageBuffer, unsigned int ImageBufferLen)
{
    //
}

int main(void)
{

    guide_usb_setloglevel(LOG_TEST);
    int ret = guide_usb_initial("/dev/video0");
    if (ret < 0)
    {
        printf("Initial fail:%d \n", ret);
        return -1;
    }

    guide_usb_setpalette(2);

    if (paramline == NULL)
    {
        paramline = (unsigned char *)malloc(WIDTH * 2);
    }

    if (pTemper == NULL)
    {
        pTemper = (float *)malloc(sizeof(float) * WIDTH * HEIGHT);
    }

    measureExternalParam = (guide_measure_external_param_t *)malloc(sizeof(guide_measure_external_param_t));
    measureExternalParam->emiss = 98;
    // measureExternalParam->distance = 50;
    measureExternalParam->relHum = 60;
    measureExternalParam->atmosphericTemper = 230;
    measureExternalParam->reflectedTemper = 230;
    measureExternalParam->modifyK = 100;
    measureExternalParam->modifyB = 0;

    guide_usb_device_info_t *deviceInfo = (guide_usb_device_info_t *)malloc(sizeof(guide_usb_device_info_t));
    deviceInfo->width = WIDTH;
    deviceInfo->height = HEIGHT;

    // deviceInfo->video_mode = Y16_PARAM;
    deviceInfo->video_mode = Y16_PARAM_YUV; //
    ret = guide_usb_openstream(deviceInfo, (OnFrameDataReceivedCB)frameCallBack, (OnDeviceConnectStatusCB)connectStatusCallBack);
    if (ret < 0)
    {
        printf("Open fail! %d \n", ret);
        return ret;
    }

    int count = 1;
    int i = 0;
    while (1)
    {
        usleep(1000);
        // sleep(1);
        // if(FrameNum > 1)
        //         break;
    }

    ret = guide_usb_closestream();
    printf("close usb return %d\n", ret);

    ret = guide_usb_exit();
    printf("exit return %d\n", ret);

    if (paramline != NULL)
    {
        free(paramline);
        paramline = NULL;
    }

    if (pTemper != NULL)
    {
        free(pTemper);
        pTemper = NULL;
    }

    return ret;
}

int connectStatusCallBack(guide_usb_device_status_e deviceStatus)
{
    if (deviceStatus == DEVICE_CONNECT_OK)
    {
        printf("VideoStream is Staring...\n");
    }
    else
    {
        printf("VideoStream is closing...\n");
    }
}

bool isFirstLoad = true;

int frameCallBack(guide_usb_frame_data_t *pVideoData)
{
    if (pVideoData->paramLine != NULL)
    {
        // 複製一段記憶體區塊的函式
        memcpy(paramline, pVideoData->paramLine, pVideoData->paramLine_length);

        /* int gear = guide_measure_setinternalparam(paramline);

        if(isFirstLoad)
        {
            guide_measure_loadtempercurve(1,gear,3);
            isFirstLoad = false;
        } */


        printf("parameline:  %02X %02X %02X\n", paramline[0], paramline[1], paramline[2]);
        //   printf("have come here  ---------------------\n");
    }

    printf("src Data:  %02X %02X %02X\n", pVideoData->frame_src_data[0], pVideoData->frame_src_data[1], pVideoData->frame_src_data[2]);

    int ret = 0;
    // guide_measure_convertgray2temper(0, 1, pVideoData->frame_src_data, paramline, 1, measureExternalParam, pTemper);
    guide_measure_convertgray2temper(1, 1, pVideoData->frame_src_data, paramline, 1, measureExternalParam, pTemper);
    printf("first pix temp-------------------------%.1f\n", pTemper[0]);


    // SaveRawImage("GaodeiR",(unsigned char *)pVideoData->frame_src_data,pVideoData->frame_src_data_length*2);
    //
    usleep(100);
    // printf("callback-------------------------\n");
    FrameNum++;
}
