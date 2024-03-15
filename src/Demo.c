
/* ----------------------------- 官方Demo 勿動！！！！！！ ---------------------------- */

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


#define WIDTH 384
#define HEIGHT 288

int frameCallBack(guide_usb_frame_data_t *pVideoData);
int connectStatusCallBack(guide_usb_device_status_e deviceStatus);

int main(void)
{
    guide_usb_setloglevel(LOG_TEST);
    int ret = guide_usb_initial("/dev/video0");
    if(ret < 0)
    {
        printf("Initial fail:%d \n",ret);
        return -1;
    }

    guide_usb_setpalette(2);

    guide_usb_device_info_t* deviceInfo = (guide_usb_device_info_t*)malloc(sizeof (guide_usb_device_info_t));
    deviceInfo->width = WIDTH;
    deviceInfo->height = HEIGHT;
    deviceInfo->video_mode = Y16_PARAM_YUV;

    ret = guide_usb_openstream(deviceInfo,(OnFrameDataReceivedCB)frameCallBack,(OnDeviceConnectStatusCB)connectStatusCallBack);
    if(ret < 0)
    {
        printf("Open fail! %d \n",ret);
        return ret;
    }

    int count = 60000;
    while(count--)
    {
      usleep(10);
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

int frameCallBack(guide_usb_frame_data_t *pVideoData)
{
  printf("frameCallBack\n");
}
