#ifndef DATA_TYPE_H
#define DATA_TYPE_H
#define FRAME_HEAD_MAGIC 0x03211546
#define SYSTEM_SYNC_ID    2

#ifdef __LINUX__
typedef unsigned char            UCHAR;
typedef unsigned char*          PBYTE;
typedef char*                    LPTSTR;
typedef    unsigned short             USHORT;
typedef int                     HANDLE;
typedef unsigned long           ULONG;
typedef unsigned long           DWORD;
#endif

typedef struct tagFrameInfo{
    ULONG    SyncId;
    ULONG    Magic;
    USHORT    FrameType;
    ULONG    Length;
    ULONG    FrameNumber;
    UCHAR    Breakable;

    ULONG    PTS;
}TMFRAME_HEADER, *PTMFRAME_HEADER;

typedef enum {
   StandardNone                   = 0x80000000,
   StandardNTSC                   = 0x00000001,
   StandardPAL                    = 0x00000002,
   StandardSECAM                  = 0x00000004,
} VideoStandard_t;

typedef enum {
    PktError = 0,
    PktIFrames = 0x0001,
    PktPFrames = 0x0002,
    PktBBPFrames = 0x0004,
    PktAudioFrames = 0x0008,
    PktMotionDetection = 0x00010,
    PktDspStatus = 0x00020,
    PktOrigImage = 0x00040,
    PktSysHeader = 0x00080,
    PktBPFrames = 0x00100,
    PktSFrames = 0x00200,
    PktSubIFrames = 0x00400,
    PktSubPFrames = 0x00800,
    PktSubBBPFrames = 0x01000,
    PktSubSysHeader = 0x02000
}FrameType_t;

typedef struct tagVersion{
    ULONG DspVersion, DspBuildNum;
    ULONG DriverVersion, DriverBuildNum;
    ULONG SDKVersion, SDKBuildNum;
}VERSION_INFO, *PVERSION_INFO;

typedef enum {
    ENC_CIF_FORMAT = 0,
    ENC_QCIF_FORMAT = 1,
    ENC_2CIF_FORMAT = 2,
    ENC_4CIF_FORMAT = 3,
    ENC_QQCIF_FORMAT = 4,
    ENC_CIFQCIF_FORMAT =5,
    ENC_CIFQQCIF_FORMAT =6,
    ENC_DCIF_FORMAT =7,
    ENC_VGA_FORMAT=8
}PictureFormat_t;

typedef struct tagMotionData{
    PictureFormat_t PicFormat;
    ULONG HorizeBlocks;
    ULONG VerticalBlocks;
    ULONG BlockSize;
}MOTION_DATA_HEADER, *PMOTION_DATA_HEADER;

#define _OSD_BASE            0x9000
#define    _OSD_YEAR4            (_OSD_BASE+0)
#define _OSD_YEAR2            (_OSD_BASE+1)
#define _OSD_MONTH3            (_OSD_BASE+2)
#define _OSD_MONTH2            (_OSD_BASE+3)
#define _OSD_DAY            (_OSD_BASE+4)
