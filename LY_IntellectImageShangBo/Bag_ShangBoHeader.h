#ifndef BAG_SHANGBOHEADER_H
#define BAG_SHANGBOHEADER_H

#include <stdint.h>

#pragma pack(push)
#pragma pack(1)

namespace ShangBoIntellectMsg {

enum ControlInfo
{
    SHANGBOCONTROLMSG_EMPTYORDER                  = 0x00,
    SHANGBOCONTROLMSG_VISIBLERAY1                 = 0x01,
    SHANGBOCONTROLMSG_VISIBLERAY2                 = 0x02,
    SHANGBOCONTROLMSG_INFRARED1                   = 0x03,
    SHANGBOCONTROLMSG_INFRARED2                   = 0x04,
    SHANGBOCONTROLMSG_IMAGEENHANCE_ON             = 0x05,
    SHANGBOCONTROLMSG_IMAGEENHANCE_OFF            = 0x06,
    SHANGBOCONTROLMSG_TARGETDISTINGGUISH_ON       = 0x07,
    SHANGBOCONTROLMSG_TARGETDISTINGGUISH_OFF      = 0x08,
    SHANGBOCONTROLMSG_STORAGE_ON                  = 0x09,
    SHANGBOCONTROLMSG_STORAGE_OFF                 = 0x0A,
    SHANGBOCONTROLMSG_TRACKPOINT                  = 0x0D,
    SHANGBOCONTROLMSG_TRACKSTOP                   = 0x0E,
    SHANGBOCONTROLMSG_ALLAUTOMATICLOCK            = 0x0F,
    SHANGBOCONTROLMSG_HALFAUTOMATICLOCK           = 0x10,
    SHANGBOCONTROLMSG_INFRAREDWHITEHOT            = 0x11,
    SHANGBOCONTROLMSG_INFRAREDBLACKHOT            = 0x12,
    SHANGBOCONTROLMSG_TRACKALG_ADAPTION           = 0x13,
    SHANGBOCONTROLMSG_TRACKALG_PERSON             = 0x14,
    SHANGBOCONTROLMSG_TRACKALG_CAR                = 0x15,
    SHANGBOCONTROLMSG_TRACKALG_BUILD              = 0x16,
    SHANGBOCONTROLMSG_HOLDERSERRCH                = 0x24,
    SHANGBOCONTROLMSG_ZOOMINSTRUCT                = 0x25,
    SHANGBOCONTROLMSG_FRAMEANGLE                  = 0x26,
    SHANGBOCONTROLMSG_DYNAME_ON                   = 0x27,
    SHANGBOCONTROLMSG_DYNAME_OFF                  = 0x28,
    SHANGBOCONTROLMSG_FOLLOW_ON                   = 0x29,
    SHANGBOCONTROLMSG_FOLLOW_POSITION             = 0x2A,
    SHANGBOCONTROLMSG_REGRESSIONCONTRE            = 0x2B,
    SHANGBOCONTROLMSG_GYRODRIFTING_RESTRAIN       = 0x2C,
    SHANGBOCONTROLMSG_LASERRANGING_ON             = 0x2D,
    SHANGBOCONTROLMSG_LASERRANGING_OFF            = 0x2E,
    SHANGBOCONTROLMSG_ELECTRICLOCK                = 0x30,
    SHANGBOCONTROLMSG_ELECTRICLOCK_REMOVE         = 0x31,
    SHANGBOCONTROLMSG_GYROCALIBRATION             = 0x39,
    SHANGBOCONTROLMSG_NUMBERGUIDE                 = 0x3A,
    SHANGBOCONTROLMSG_ASSIGNGESTUREANGLE          = 0x3B,
    SHANGBOCONTROLMSG_FLYANDPODCALIBRATIONPOSE    = 0x3C,
    SHANGBOCONTROLMSG_IMAGEPOWERCONTROL           = 0x4A,
    SHANGBOCONTROLMSG_PSEUDOCOLOR                 = 0x50,
    SHANGBOCONTROLMSG_OSDSHOW_ON                  = 0x51,
    SHANGBOCONTROLMSG_OSDSHOW_OFF                 = 0x52,
    SHANGBOCONTROLMSG_LOWLIGHTMODE_OFF            = 0x55,
    SHANGBOCONTROLMSG_LOWLIGHTMODE_ON             = 0x56,
    SHANGBOCONTROLMSG_NUMBERZOOM_ON               = 0x58,
    SHANGBOCONTROLMSG_NUMBERZOOM_OFF              = 0x59,
    SHANGBOCONTROLMSG_ASSIGNZOOM                  = 0x5A,
    SHANGBOCONTROLMSG_PHOTOGRAGH                  = 0x5B,
    SHANGBOCONTROLMSG_FOCUSINGMODEL               = 0x5C,
    SHANGBOCONTROLMSG_FOCUSINGPOSITION            = 0x5D,
    SHANGBOCONTROLMSG_SAVEFLASHPARAM              = 0xA0,
    SHANGBOCONTROLMSG_LIFTCONTROL                 = 0xB0,
    SHANGBOCONTROLMSG_SETCURRENTANGLEZERO         = 0xB1
};

typedef struct __ControlGeneral
{
    int16_t             paramX;
    int16_t             paramY;
    unsigned char       framesParam;
    int8_t              zoomSpeed;
    char                keepBit[6];
}ControlGeneral;

typedef struct __ControlNumberguide
{
    unsigned char   numberGuideState;
    int32_t         targetLat;
    int32_t         targetLon;
    int16_t         targetHightPriced;
    unsigned char   keepBit;
}ControlNumberguide;

#define SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_FIRST_HEAD  0xEB
#define SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_SECOND_HEAD 0x90
#define SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_LENGTH      sizeof(ShangBoIntellectMsg::ShangBoControlMsg)
#define SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_HEAD_LENGTH 0x02

#define SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_HEAD_LENGTH   0x02
#define SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_LENGTH_LENGTH 0x01
#define SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_CRC_LENGTH    0x01

#define SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_EXT_LENGTH (SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_HEAD_LENGTH + SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_LENGTH_LENGTH + SHANGBOINTELLECTMSG_SHANGBOCONTROLMSG_NET_CRC_LENGTH)

typedef struct __ShangBoControlMsg
{
    unsigned char synchro1;
    unsigned char synchro2;
    unsigned char controlInfo;
    union
    {
        ControlGeneral     controlGeneral;
        ControlNumberguide controlNumberguide;
    }generalCrl;
    unsigned char verifySum;
}ShangBoControlMsg;

static inline unsigned char CRCCheck_ShangBo(const unsigned char* buffer, const unsigned int length)
{
    unsigned int crcCheck = 0;
    for(unsigned int i = 0; i < length; i++)
    {
        crcCheck += buffer[i];
    }
    return (crcCheck & 0xFF);
}

#define SHANGBOINTELLECTMSG_SHANGBOCONTROLSTATUSMSG_FIRST_HEAD  0xEE
#define SHANGBOINTELLECTMSG_SHANGBOCONTROLSTATUSMSG_SECOND_HEAD 0x16
#define SHANGBOINTELLECTMSG_SHANGBOCONTROLSTATUSMSG_LENGTH      0x20
typedef struct __ShangBoControlStatusMsg
{
    unsigned char synchro1;
    unsigned char synchro2;

    unsigned char status1;

    unsigned char status2;

    unsigned char zoomMultiple;

    unsigned char status3;

    signed short int targetLossX;
    signed short int targetLossY;

    signed short int roll;
    signed short int pitch;
    signed short int yaw;

    unsigned short int reserve1;
    unsigned short int reserve2;

    signed short int   rollAngularSpeed;
    signed short int   pitchAngularSpeed;
    signed short int   yawAngularSpeed;

    unsigned short int laserDistance;

    unsigned char     selfResults ;

    unsigned short int reserve3;
    unsigned char      verifySum;
}ShangBoControlStatusMsg;

}

#pragma pack(pop)
#endif
