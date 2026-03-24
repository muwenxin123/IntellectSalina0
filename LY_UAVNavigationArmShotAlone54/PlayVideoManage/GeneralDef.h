#ifndef GENERALDEF_H
#define GENERALDEF_H

#include "./include/HCNetSDK.h"

#define MAX_CHANS      24
#define MAX_DEVICE     10
#define MAX_OUTPUTS	   16
#define MAX_PREVIEWS		512

#define MAX_CRUISE_SEQ  32
#define MAX_CRUISE_POINT 32
#define MAX_CRUISE_PRESET 128
#define MAX_CRUISE_TIME 255
#define MAX_CRUISE_SPEED 15

#define TREE_ALL_T	 0
#define DEVICETYPE 	 1
#define CHANNELTYPE	 2

#define DEMO_FULL_WIDTH 1024
#define DEMO_FULL_HEIGHT 768
#define OUTPUT_INTERVAL 4

#define CYCLE_PREVIEW_TIMER 2

typedef struct STRU_CHANNEL_INFO
{
    char    chChanName[40];
    int		iChanIndex;
    int		iPicResolution;
    int		iPicQuality;
    char	chAccessChanIP[16];
    BOOL    bEnable;
    NET_DVR_DECODERCFG_V30  struDecodercfg;
    STRU_CHANNEL_INFO()
    {
        chChanName[0]       = '\0';
        iChanIndex          = -1;
        iPicResolution		= 0;
        iPicQuality			= 2;
        chAccessChanIP[0]	= '\0';
        bEnable             = FALSE;
    }
} CHANNEL_INFO, *pCHANNEL_INFO;

typedef struct STRU_DEVICE_INFO
{

    LONG    lLoginID;
    int		iDeviceChanNum;
    int		iStartChan;
    int 	iIPChanNum;
    int 	iIPStartChan;
    int		iEnableChanNum;
    BOOL    bIPRet;

    CHANNEL_INFO struChanInfo[MAX_CHANNUM_V30];

    STRU_DEVICE_INFO()
    {
        lLoginID = -1;
        iDeviceChanNum      =  -1;
        iStartChan          = 0;
        iIPChanNum          = 0;
        iEnableChanNum      = -1;
        bIPRet              = FALSE;
    }
} LOCAL_DEVICE_INFO, *pLOCAL_DEVICE_INFO;

#endif