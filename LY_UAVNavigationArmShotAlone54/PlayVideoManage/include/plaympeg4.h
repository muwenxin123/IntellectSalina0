#ifndef _PLAYM4_H_
#define _PLAYM4_H_

#if defined( _WINDLL)
    #define PLAYM4_API  extern "C"__declspec(dllexport)
#else
    #define PLAYM4_API  extern "C" __declspec(dllimport)
#endif

#define PLAYM4_MAX_SUPPORTS 500
#define MIN_WAVE_COEF -100
#define MAX_WAVE_COEF 100

#define TIMER_1 1
#define TIMER_2 2

#define BUF_VIDEO_SRC               (1)
#define BUF_AUDIO_SRC               (2)
#define BUF_VIDEO_RENDER            (3)
#define BUF_AUDIO_RENDER            (4)
#define BUF_VIDEO_DECODED           (5)
#define BUF_AUDIO_DECODED           (6)
#define BUF_DISPLAY_NODE            (7)

#define  PLAYM4_NOERROR                            0
#define     PLAYM4_PARA_OVER                        1
#define  PLAYM4_ORDER_ERROR                        2
#define     PLAYM4_TIMER_ERROR                        3
#define  PLAYM4_DEC_VIDEO_ERROR                    4
#define  PLAYM4_DEC_AUDIO_ERROR                    5
#define     PLAYM4_ALLOC_MEMORY_ERROR                6
#define  PLAYM4_OPEN_FILE_ERROR                    7
#define  PLAYM4_CREATE_OBJ_ERROR                8
#define  PLAYM4_CREATE_DDRAW_ERROR                9
#define  PLAYM4_CREATE_OFFSCREEN_ERROR            10
#define  PLAYM4_BUF_OVER                        11
#define  PLAYM4_CREATE_SOUND_ERROR                12
#define     PLAYM4_SET_VOLUME_ERROR                13
#define  PLAYM4_SUPPORT_FILE_ONLY                14
#define  PLAYM4_SUPPORT_STREAM_ONLY                15
#define  PLAYM4_SYS_NOT_SUPPORT                    16
#define  PLAYM4_FILEHEADER_UNKNOWN                17
#define  PLAYM4_VERSION_INCORRECT                18
#define  PLAYM4_INIT_DECODER_ERROR                19
#define  PLAYM4_CHECK_FILE_ERROR                20
#define  PLAYM4_INIT_TIMER_ERROR                21
#define     PLAYM4_BLT_ERROR                        22
#define  PLAYM4_UPDATE_ERROR                    23
#define  PLAYM4_OPEN_FILE_ERROR_MULTI            24
#define  PLAYM4_OPEN_FILE_ERROR_VIDEO            25
#define  PLAYM4_JPEG_COMPRESS_ERROR                26
#define  PLAYM4_EXTRACT_NOT_SUPPORT                27
#define  PLAYM4_EXTRACT_DATA_ERROR                28
#define  PLAYM4_SECRET_KEY_ERROR                29
#define  PLAYM4_DECODE_KEYFRAME_ERROR            30
#define  PLAYM4_NEED_MORE_DATA                    31
#define  PLAYM4_INVALID_PORT                    32
#define  PLAYM4_NOT_FIND                        33
#define  PLAYM4_NEED_LARGER_BUFFER              34
#define  PLAYM4_FAIL_UNKNOWN                    99

#define PLAYM4_FEC_ERR_ENABLEFAIL                100
#define PLAYM4_FEC_ERR_NOTENABLE                101
#define PLAYM4_FEC_ERR_NOSUBPORT                102
#define PLAYM4_FEC_ERR_PARAMNOTINIT                103
#define PLAYM4_FEC_ERR_SUBPORTOVER                104
#define PLAYM4_FEC_ERR_EFFECTNOTSUPPORT            105
#define PLAYM4_FEC_ERR_INVALIDWND                106
#define PLAYM4_FEC_ERR_PTZOVERFLOW                107
#define PLAYM4_FEC_ERR_RADIUSINVALID            108
#define PLAYM4_FEC_ERR_UPDATENOTSUPPORT            109
#define PLAYM4_FEC_ERR_NOPLAYPORT                110
#define PLAYM4_FEC_ERR_PARAMVALID                111
#define PLAYM4_FEC_ERR_INVALIDPORT                112
#define PLAYM4_FEC_ERR_PTZZOOMOVER                113
#define PLAYM4_FEC_ERR_OVERMAXPORT                114
#define PLAYM4_FEC_ERR_ENABLED                  115
#define PLAYM4_FEC_ERR_D3DACCENOTENABLE            116

#define MAX_DISPLAY_WND 4

#define DISPLAY_NORMAL                0x00000001
#define DISPLAY_QUARTER                0x00000002
#define DISPLAY_YC_SCALE            0x00000004
#define DISPLAY_NOTEARING            0x00000008
#define MAX_DIS_FRAMES 50
#define MIN_DIS_FRAMES 1

#define BY_FRAMENUM  1
#define BY_FRAMETIME 2

#define SOURCE_BUF_MAX    1024*100000
#define SOURCE_BUF_MIN    1024*50

#define STREAME_REALTIME 0
#define STREAME_FILE     1

#define T_AUDIO16    101
#define T_AUDIO8    100
#define T_UYVY        1
#define T_YV12        3
#define T_RGB32        7

#define SUPPORT_DDRAW        1
#define SUPPORT_BLT         2
#define SUPPORT_BLTFOURCC   4
#define SUPPORT_BLTSHRINKX  8
#define SUPPORT_BLTSHRINKY  16
#define SUPPORT_BLTSTRETCHX 32
#define SUPPORT_BLTSTRETCHY 64
#define SUPPORT_SSE         128
#define SUPPORT_MMX            256

#define FOURCC_HKMI            0x484B4D49
#define SYSTEM_NULL            0x0
#define SYSTEM_HIK          0x1
#define SYSTEM_MPEG2_PS     0x2
#define SYSTEM_MPEG2_TS     0x3
#define SYSTEM_RTP          0x4
#define SYSTEM_RTPHIK       0x401

#define VIDEO_NULL          0x0
#define VIDEO_H264          0x1
#define VIDEO_MPEG4         0x3
#define VIDEO_MJPEG            0x4
#define VIDEO_AVC264        0x0100

#define AUDIO_NULL          0x0000
#define AUDIO_ADPCM         0x1000
#define AUDIO_MPEG          0x2000
#define AUDIO_AAC            0X2001
#define AUDIO_RAW_DATA8        0x7000
#define AUDIO_RAW_UDATA16    0x7001
#define AUDIO_G711_U        0x7110
#define AUDIO_G711_A        0x7111
#define AUDIO_G722_1        0x7221
#define AUDIO_G723_1        0x7231
#define AUDIO_G726_U        0x7260
#define AUDIO_G726_A        0x7261
#define AUDIO_G726_16       0x7262
#define AUDIO_G729          0x7290
#define AUDIO_AMR_NB        0x3000

#define SYNCDATA_VEH        1
#define SYNCDATA_IVS        2

#define    MOTION_FLOW_NONE            0
#define MOTION_FLOW_CPU                1
#define MOTION_FLOW_GPU                2

#define ENCRYPT_AES_3R_VIDEO     1
#define ENCRYPT_AES_10R_VIDEO    2
#define ENCRYPT_AES_3R_AUDIO     1
#define ENCRYPT_AES_10R_AUDIO    2

typedef struct
{
    long nFilePos;
    long nFrameNum;
    long nFrameTime;
    long nErrorFrameNum;
    SYSTEMTIME *pErrorTime;
    long nErrorLostFrameNum;
    long nErrorFrameSize;
} FRAME_POS, *PFRAME_POS;

typedef struct
{
    long nWidth;
    long nHeight;
    long nStamp;
    long nType;
    long nFrameRate;
    DWORD dwFrameNum;
} FRAME_INFO;

typedef struct
{
    char *pDataBuf;
    long  nSize;
    long  nFrameNum;
    BOOL  bIsAudio;
    long  nReserved;
} FRAME_TYPE;

typedef struct
{
    char *pDataBuf;
    long  nSize;
    long  nFrameNum;
    BOOL  bRsaRight;
    long  nReserved;
} WATERMARK_INFO;

typedef struct SYNCDATA_INFO
{
    DWORD dwDataType;
    DWORD dwDataLen;
    BYTE *pData;
} SYNCDATA_INFO;

#ifndef _HIK_MEDIAINFO_FLAG_
#define _HIK_MEDIAINFO_FLAG_
typedef struct _HIK_MEDIAINFO_
{
    unsigned int    media_fourcc;
    unsigned short  media_version;
    unsigned short  device_id;

    unsigned short  system_format;
    unsigned short  video_format;

    unsigned short  audio_format;
    unsigned char   audio_channels;
    unsigned char   audio_bits_per_sample;
    unsigned int    audio_samplesrate;
    unsigned int    audio_bitrate;

    unsigned int    reserved[4];
} HIK_MEDIAINFO;
#endif

typedef struct
{
    long nPort;
    char *pBuf;
    long nBufLen;
    long nWidth;
    long nHeight;
    long nStamp;
    long nType;
    long nUser;
} DISPLAY_INFO;

typedef struct
{
    long nPort;
    char *pVideoBuf;
    long nVideoBufLen;
    char *pPriBuf;
    long nPriBufLen;
    long nWidth;
    long nHeight;
    long nStamp;
    long nType;
    long nUser;
} DISPLAY_INFOEX;

typedef struct PLAYM4_SYSTEM_TIME
{
    DWORD dwYear;
    DWORD dwMon;
    DWORD dwDay;
    DWORD dwHour;
    DWORD dwMin;
    DWORD dwSec;
    DWORD dwMs;
} PLAYM4_SYSTEM_TIME;

typedef struct
{
    long nVideoEncryptType;
    long nAudioEncryptType;
    long nSetSecretKey;
} ENCRYPT_INFO;

PLAYM4_API BOOL __stdcall  PlayM4_InitDDraw(HWND hWnd);
PLAYM4_API BOOL __stdcall PlayM4_RealeseDDraw();
PLAYM4_API BOOL __stdcall PlayM4_OpenFile(LONG nPort, LPSTR sFileName);
PLAYM4_API BOOL __stdcall PlayM4_CloseFile(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_Play(LONG nPort, HWND hWnd);
PLAYM4_API BOOL __stdcall PlayM4_Stop(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_Pause(LONG nPort, DWORD nPause);
PLAYM4_API BOOL __stdcall PlayM4_Fast(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_Slow(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_OneByOne(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetPlayPos(LONG nPort, float fRelativePos);
PLAYM4_API float __stdcall PlayM4_GetPlayPos(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetFileEndMsg(LONG nPort, HWND hWnd, UINT nMsg);
PLAYM4_API BOOL __stdcall PlayM4_SetVolume(LONG nPort, WORD nVolume);
PLAYM4_API BOOL __stdcall PlayM4_StopSound();
PLAYM4_API BOOL __stdcall PlayM4_PlaySound(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_OpenStream(LONG nPort, PBYTE pFileHeadBuf, DWORD nSize, DWORD nBufPoolSize);
PLAYM4_API BOOL __stdcall PlayM4_InputData(LONG nPort, PBYTE pBuf, DWORD nSize);
PLAYM4_API BOOL __stdcall PlayM4_CloseStream(LONG nPort);
PLAYM4_API int  __stdcall  PlayM4_GetCaps();
PLAYM4_API DWORD __stdcall PlayM4_GetFileTime(LONG nPort);
PLAYM4_API DWORD __stdcall PlayM4_GetPlayedTime(LONG nPort);
PLAYM4_API DWORD __stdcall PlayM4_GetPlayedFrames(LONG nPort);

PLAYM4_API BOOL __stdcall    PlayM4_SetDecCallBack(LONG nPort, void (CALLBACK *DecCBFun)(long nPort, char *pBuf, long nSize, FRAME_INFO *pFrameInfo, long nReserved1, long nReserved2));
PLAYM4_API BOOL __stdcall    PlayM4_SetDisplayCallBack(LONG nPort, void (CALLBACK *DisplayCBFun)(long nPort, char *pBuf, long nSize, long nWidth, long nHeight, long nStamp, long nType, long nReserved));
PLAYM4_API BOOL __stdcall    PlayM4_ConvertToBmpFile(char *pBuf, long nSize, long nWidth, long nHeight, long nType, char *sFileName);
PLAYM4_API DWORD __stdcall    PlayM4_GetFileTotalFrames(LONG nPort);
PLAYM4_API DWORD __stdcall    PlayM4_GetCurrentFrameRate(LONG nPort);
PLAYM4_API DWORD __stdcall    PlayM4_GetPlayedTimeEx(LONG nPort);
PLAYM4_API BOOL __stdcall    PlayM4_SetPlayedTimeEx(LONG nPort, DWORD nTime);
PLAYM4_API DWORD __stdcall    PlayM4_GetCurrentFrameNum(LONG nPort);
PLAYM4_API BOOL __stdcall    PlayM4_SetStreamOpenMode(LONG nPort, DWORD nMode);
PLAYM4_API DWORD __stdcall    PlayM4_GetFileHeadLength();
PLAYM4_API DWORD __stdcall    PlayM4_GetSdkVersion();
PLAYM4_API DWORD __stdcall  PlayM4_GetLastError(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_RefreshPlay(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetOverlayMode(LONG nPort, BOOL bOverlay, COLORREF colorKey);
PLAYM4_API BOOL __stdcall PlayM4_GetPictureSize(LONG nPort, LONG *pWidth, LONG *pHeight);
PLAYM4_API BOOL __stdcall PlayM4_SetPicQuality(LONG nPort, BOOL bHighQuality);
PLAYM4_API BOOL __stdcall PlayM4_PlaySoundShare(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_StopSoundShare(LONG nPort);
PLAYM4_API LONG __stdcall PlayM4_GetStreamOpenMode(LONG nPort);
PLAYM4_API LONG __stdcall PlayM4_GetOverlayMode(LONG nPort);
PLAYM4_API COLORREF __stdcall PlayM4_GetColorKey(LONG nPort);
PLAYM4_API WORD __stdcall PlayM4_GetVolume(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_GetPictureQuality(LONG nPort, BOOL *bHighQuality);
PLAYM4_API DWORD __stdcall PlayM4_GetSourceBufferRemain(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_ResetSourceBuffer(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetSourceBufCallBack(LONG nPort, DWORD nThreShold, void (CALLBACK *SourceBufCallBack)(long nPort, DWORD nBufSize, DWORD dwUser, void *pResvered), DWORD dwUser, void *pReserved);
PLAYM4_API BOOL __stdcall PlayM4_ResetSourceBufFlag(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetDisplayBuf(LONG nPort, DWORD nNum);
PLAYM4_API DWORD __stdcall PlayM4_GetDisplayBuf(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_OneByOneBack(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetFileRefCallBack(LONG nPort, void (__stdcall *pFileRefDone)(DWORD nPort, DWORD nUser), DWORD nUser);
PLAYM4_API BOOL __stdcall PlayM4_SetCurrentFrameNum(LONG nPort, DWORD nFrameNum);
PLAYM4_API BOOL __stdcall PlayM4_GetKeyFramePos(LONG nPort, DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
PLAYM4_API BOOL __stdcall PlayM4_GetNextKeyFramePos(LONG nPort, DWORD nValue, DWORD nType, PFRAME_POS pFramePos);
#if (WINVER >= 0x0400)
    PLAYM4_API BOOL __stdcall PlayM4_InitDDrawDevice();
    PLAYM4_API void __stdcall PlayM4_ReleaseDDrawDevice();
    PLAYM4_API DWORD __stdcall PlayM4_GetDDrawDeviceTotalNums();
    PLAYM4_API BOOL __stdcall PlayM4_SetDDrawDevice(LONG nPort, DWORD nDeviceNum);
    PLAYM4_API int  __stdcall  PlayM4_GetCapsEx(DWORD nDDrawDeviceNum);
#endif
PLAYM4_API BOOL __stdcall PlayM4_ThrowBFrameNum(LONG nPort, DWORD nNum);
PLAYM4_API BOOL __stdcall PlayM4_SetDisplayType(LONG nPort, LONG nType);
PLAYM4_API long __stdcall PlayM4_GetDisplayType(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetDecCBStream(LONG nPort, DWORD nStream);
PLAYM4_API BOOL __stdcall PlayM4_SetDisplayRegion(LONG nPort, DWORD nRegionNum, RECT *pSrcRect, HWND hDestWnd, BOOL bEnable);
PLAYM4_API BOOL __stdcall PlayM4_RefreshPlayEx(LONG nPort, DWORD nRegionNum);
#if (WINVER >= 0x0400)
    PLAYM4_API BOOL __stdcall PlayM4_SetDDrawDeviceEx(LONG nPort, DWORD nRegionNum, DWORD nDeviceNum);
#endif

PLAYM4_API BOOL __stdcall PlayM4_GetRefValue(LONG nPort, BYTE *pBuffer, DWORD *pSize);
PLAYM4_API BOOL __stdcall PlayM4_SetRefValue(LONG nPort, BYTE *pBuffer, DWORD nSize);
PLAYM4_API BOOL __stdcall PlayM4_OpenStreamEx(LONG nPort, PBYTE pFileHeadBuf, DWORD nSize, DWORD nBufPoolSize);
PLAYM4_API BOOL __stdcall PlayM4_CloseStreamEx(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_InputVideoData(LONG nPort, PBYTE pBuf, DWORD nSize);
PLAYM4_API BOOL __stdcall PlayM4_InputAudioData(LONG nPort, PBYTE pBuf, DWORD nSize);
PLAYM4_API BOOL __stdcall PlayM4_RegisterDrawFun(LONG nPort, void (CALLBACK *DrawFun)(long nPort, HDC hDc, LONG nUser), LONG nUser);
PLAYM4_API BOOL __stdcall PlayM4_RigisterDrawFun(LONG nPort, void (CALLBACK *DrawFun)(long nPort, HDC hDc, LONG nUser), LONG nUser);
PLAYM4_API BOOL __stdcall PlayM4_SetTimerType(LONG nPort, DWORD nTimerType, DWORD nReserved);
PLAYM4_API BOOL __stdcall PlayM4_GetTimerType(LONG nPort, DWORD *pTimerType, DWORD *pReserved);
PLAYM4_API BOOL __stdcall PlayM4_ResetBuffer(LONG nPort, DWORD nBufType);
PLAYM4_API DWORD __stdcall PlayM4_GetBufferValue(LONG nPort, DWORD nBufType);

PLAYM4_API BOOL __stdcall PlayM4_AdjustWaveAudio(LONG nPort, LONG nCoefficient);
PLAYM4_API BOOL __stdcall PlayM4_SetVerifyCallBack(LONG nPort, DWORD nBeginTime, DWORD nEndTime, void (__stdcall *funVerify)(long nPort, FRAME_POS *pFilePos, DWORD bIsVideo, DWORD nUser),  DWORD  nUser);
PLAYM4_API BOOL __stdcall PlayM4_SetAudioCallBack(LONG nPort, void (__stdcall *funAudio)(long nPort, char *pAudioBuf, long nSize, long nStamp, long nType, long nUser), long nUser);
PLAYM4_API BOOL __stdcall PlayM4_SetEncTypeChangeCallBack(LONG nPort, void(CALLBACK *funEncChange)(long nPort, long nUser), long nUser);
PLAYM4_API BOOL __stdcall PlayM4_SetColor(LONG nPort, DWORD nRegionNum, int nBrightness, int nContrast, int nSaturation, int nHue);
PLAYM4_API BOOL __stdcall PlayM4_GetColor(LONG nPort, DWORD nRegionNum, int *pBrightness, int *pContrast, int *pSaturation, int *pHue);
PLAYM4_API BOOL __stdcall PlayM4_SetEncChangeMsg(LONG nPort, HWND hWnd, UINT nMsg);
PLAYM4_API BOOL __stdcall PlayM4_GetOriginalFrameCallBack(LONG nPort, BOOL bIsChange, BOOL bNormalSpeed, long nStartFrameNum, long nStartStamp, long nFileHeader, void(CALLBACK *funGetOrignalFrame)(long nPort, FRAME_TYPE *frameType, long nUser), long nUser);
PLAYM4_API BOOL __stdcall PlayM4_GetFileSpecialAttr(LONG nPort, DWORD *pTimeStamp, DWORD *pFileNum, DWORD *pReserved);
PLAYM4_API DWORD __stdcall PlayM4_GetSpecialData(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetCheckWatermarkCallBack(LONG nPort, void(CALLBACK *funCheckWatermark)(long nPort, WATERMARK_INFO *pWatermarkInfo, DWORD nUser), DWORD nUser);
PLAYM4_API BOOL __stdcall PlayM4_SetImageSharpen(LONG nPort, DWORD nLevel);
PLAYM4_API BOOL __stdcall PlayM4_SetDecodeFrameType(LONG nPort, DWORD nFrameType);
PLAYM4_API BOOL __stdcall PlayM4_SetPlayMode(LONG nPort, BOOL bNormal);
PLAYM4_API BOOL __stdcall PlayM4_SetOverlayFlipMode(LONG nPort, BOOL bTrue);

PLAYM4_API BOOL __stdcall PlayM4_ConvertToJpegFile(char *pBuf, long nSize, long nWidth, long nHeight, long nType, char *sFileName);
PLAYM4_API BOOL __stdcall PlayM4_SetJpegQuality(long nQuality);
PLAYM4_API BOOL __stdcall PlayM4_SetDeflash(LONG nPort, BOOL bDefalsh);
PLAYM4_API BOOL __stdcall PlayM4_CheckDiscontinuousFrameNum(LONG nPort, BOOL bCheck);
PLAYM4_API BOOL __stdcall PlayM4_GetBMP(LONG nPort, PBYTE pBitmap, DWORD nBufSize, DWORD *pBmpSize);
PLAYM4_API BOOL __stdcall PlayM4_GetJPEG(LONG nPort, PBYTE pJpeg, DWORD nBufSize, DWORD *pJpegSize);
PLAYM4_API BOOL __stdcall PlayM4_SetDecCallBackMend(LONG nPort, void (CALLBACK *DecCBFun)(long nPort, char *pBuf, long nSize, FRAME_INFO *pFrameInfo, long nUser, long nReserved2), long nUser);
PLAYM4_API BOOL __stdcall PlayM4_SetSecretKey(LONG nPort, LONG lKeyType, char *pSecretKey, LONG lKeyLen);

PLAYM4_API BOOL __stdcall PlayM4_SetFileEndCallback(LONG nPort, void(CALLBACK *FileEndCallback)(long nPort, void *pUser), void *pUser);

PLAYM4_API BOOL __stdcall PlayM4_GetPort(LONG *nPort);
PLAYM4_API BOOL __stdcall PlayM4_FreePort(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_SetDisplayCallBackEx(LONG nPort, void (CALLBACK *DisplayCBFun)(DISPLAY_INFO *pstDisplayInfo), long nUser);
PLAYM4_API BOOL __stdcall PlayM4_SkipErrorData(LONG nPort, BOOL bSkip);
PLAYM4_API BOOL __stdcall PlayM4_SetDecCallBackExMend(LONG nPort, void (CALLBACK *DecCBFun)(long nPort, char *pBuf, long nSize, FRAME_INFO *pFrameInfo,
                                                      long nUser, long nReserved2), char *pDest, long nDestSize, long nUser);
PLAYM4_API BOOL __stdcall PlayM4_ReversePlay(LONG nPort);
PLAYM4_API BOOL __stdcall PlayM4_GetSystemTime(LONG nPort, PLAYM4_SYSTEM_TIME *pstSystemTime);

#ifndef PLAYM4_SESSION_INFO_TAG
#define PLAYM4_SESSION_INFO_TAG
#define PLAYM4_PROTOCOL_RTSP    1
#define PLAYM4_SESSION_INFO_SDP 1

typedef struct _PLAYM4_SESSION_INFO_
{
    int            nSessionInfoType;
    int            nSessionInfoLen;
    unsigned char *pSessionInfoData;

} PLAYM4_SESSION_INFO;
#endif

PLAYM4_API BOOL __stdcall PlayM4_OpenStreamAdvanced(LONG nPort, int nProtocolType, PLAYM4_SESSION_INFO *pstSessionInfo, DWORD nBufPoolSize);

#define R_ANGLE_0   -1
#define R_ANGLE_L90  0
#define R_ANGLE_R90  1
#define R_ANGLE_180  2

PLAYM4_API BOOL __stdcall PlayM4_SetRotateAngle(LONG nPort, DWORD nRegionNum, DWORD dwType);

#ifndef PLAYM4_ADDITION_INFO_TAG
#define PLAYM4_ADDITION_INFO_TAG
typedef struct _PLAYM4_ADDITION_INFO_
{
    BYTE   *pData;
    DWORD   dwDatalen;
    DWORD    dwDataType;
    DWORD    dwTimeStamp;
} PLAYM4_ADDITION_INFO;
#endif

PLAYM4_API BOOL __stdcall PlayM4_SetSycGroup(LONG nPort, DWORD dwGroupIndex);
PLAYM4_API BOOL __stdcall PlayM4_SetSycStartTime(LONG nPort, PLAYM4_SYSTEM_TIME *pstSystemTime);

#ifndef FISH_EYE_TAG
#define FISH_EYE_TAG

typedef enum tagFECPlaceType
{
    FEC_PLACE_WALL    = 0x1,
    FEC_PLACE_FLOOR   = 0x2,
    FEC_PLACE_CEILING = 0x3,

} FECPLACETYPE;

typedef enum tagFECCorrectType
{
    FEC_CORRECT_PTZ = 0x100,
    FEC_CORRECT_180 = 0x200,
    FEC_CORRECT_360 = 0x300,
    FEC_CORRECT_LAT = 0x400

} FECCORRECTTYPE;

typedef struct tagCycleParam
{
    float    fRadiusLeft;
    float    fRadiusRight;
    float   fRadiusTop;
    float   fRadiusBottom;

} CYCLEPARAM;

typedef struct tagPTZParam
{
    float fPTZPositionX;
    float fPTZPositionY;

} PTZPARAM;

#define         FEC_UPDATE_RADIUS             0x1
#define         FEC_UPDATE_PTZZOOM             0x2
#define         FEC_UPDATE_WIDESCANOFFSET     0x4
#define         FEC_UPDATE_PTZPARAM             0x8

typedef struct tagFECParam
{

    unsigned int     nUpDateType;

    unsigned int    nPlaceAndCorrect;

    PTZPARAM        stPTZParam;

    CYCLEPARAM        stCycleParam;

    float            fZoom;

    float            fWideScanOffset;

    int                nResver[16];

} FISHEYEPARAM;

typedef void (__stdcall *FISHEYE_CallBack)(void *pUser, unsigned int  nSubPort, unsigned int nCBType, void *hDC,   unsigned int nWidth, unsigned int nHeight);

#endif
PLAYM4_API BOOL __stdcall PlayM4_FEC_Enable(LONG nPort);

PLAYM4_API BOOL __stdcall PlayM4_FEC_Disable(LONG nPort);

PLAYM4_API BOOL  __stdcall PlayM4_FEC_GetPort(LONG nPort, unsigned int *nSubPort, FECPLACETYPE emPlaceType, FECCORRECTTYPE emCorrectType);

PLAYM4_API BOOL __stdcall PlayM4_FEC_DelPort(LONG nPort, unsigned int nSubPort);

PLAYM4_API BOOL __stdcall PlayM4_FEC_SetParam(LONG nPort, unsigned int nSubPort, FISHEYEPARAM *pPara);

PLAYM4_API BOOL __stdcall PlayM4_FEC_GetParam(LONG nPort, unsigned int nSubPort, FISHEYEPARAM *pPara);

PLAYM4_API BOOL __stdcall PlayM4_FEC_SetWnd(LONG nPort, unsigned int nSubPort, void *hWnd);

PLAYM4_API BOOL __stdcall PlayM4_FEC_SetCallBack(LONG nPort, unsigned int nSubPort, FISHEYE_CallBack cbFunc, void *pUser);

PLAYM4_API BOOL __stdcall PlayM4_MotionFlow(LONG nPort, DWORD dwAdjustType);

#ifndef PLAYM4_HIKVIE_TAG
#define PLAYM4_HIKVIE_TAG

typedef struct _PLAYM4_VIE_DYNPARAM_
{
    int moduFlag;
    int brightVal;
    int contrastVal;
    int colorVal;
    int toneScale;
    int toneGain;
    int toneOffset;
    int toneColor;
    int dehazeLevel;
    int dehazeTrans;
    int dehazeBright;
    int denoiseLevel;
    int usmAmount;
    int usmRadius;
    int usmThreshold;
    int deblockLevel;
    int lensWarp;
    int lensZoom;
} PLAYM4_VIE_PARACONFIG;

typedef enum _PLAYM4_VIE_MODULES
{
    PLAYM4_VIE_MODU_ADJ      = 0x00000001,
    PLAYM4_VIE_MODU_EHAN     = 0x00000002,
    PLAYM4_VIE_MODU_DEHAZE   = 0x00000004,
    PLAYM4_VIE_MODU_DENOISE  = 0x00000008,
    PLAYM4_VIE_MODU_SHARPEN  = 0x00000010,
    PLAYM4_VIE_MODU_DEBLOCK  = 0x00000020,
    PLAYM4_VIE_MODU_CRB      = 0x00000040,
    PLAYM4_VIE_MODU_LENS     = 0x00000080,
} PLAYM4_VIE_MODULES;
#endif

PLAYM4_API BOOL __stdcall PlayM4_VIE_SetModuConfig(LONG lPort, int nModuFlag, BOOL bEnable);

PLAYM4_API BOOL __stdcall PlayM4_VIE_SetRegion(LONG lPort, LONG lRegNum, RECT *pRect);

PLAYM4_API BOOL __stdcall PlayM4_VIE_GetModuConfig(LONG lPort, int *pdwModuFlag);

PLAYM4_API BOOL __stdcall PlayM4_VIE_SetParaConfig(LONG lPort, PLAYM4_VIE_PARACONFIG *pParaConfig);

PLAYM4_API BOOL __stdcall PlayM4_VIE_GetParaConfig(LONG lPort, PLAYM4_VIE_PARACONFIG *pParaConfig);

PLAYM4_API BOOL __stdcall PlayM4_SyncToAudio(LONG nPort, BOOL bSyncToAudio);

typedef enum _PLAYM4_PRIDATA_RENDER
{
    PLAYM4_RENDER_ANA_INTEL_DATA   = 0x00000001,
    PLAYM4_RENDER_MD               = 0x00000002,
    PLAYM4_RENDER_ADD_POS          = 0x00000004,
    PLAYM4_RENDER_ADD_PIC          = 0x00000008,
    PLAYM4_RENDER_FIRE_DETCET      = 0x00000010,
    PLAYM4_RENDER_TEM              = 0x00000020
} PLAYM4_PRIDATA_RENDER;

typedef enum _PLAYM4_FIRE_ALARM
{
    PLAYM4_FIRE_FRAME_DIS           = 0x00000001,
    PLAYM4_FIRE_MAX_TEMP            = 0x00000002,
    PLAYM4_FIRE_MAX_TEMP_POSITION   = 0x00000004,
    PLAYM4_FIRE_DISTANCE            = 0x00000008,
} PLAYM4_FIRE_ALARM;

typedef enum _PLAYM4_TEM_FLAG
{
    PLAYM4_TEM_REGION_BOX           = 0x00000001,
    PLAYM4_TEM_REGION_LINE          = 0x00000002,
    PLAYM4_TEM_REGION_POINT         = 0x00000004,
} PLAYM4_TEM_FLAG;

typedef struct TI_PTZ_INFO_
{
    unsigned short dwDefVer;
    unsigned short dwLength;
    DWORD          dwP;
    DWORD          dwT;
    DWORD          dwZ;
    BYTE        chFSMState;
    BYTE           bClearFocusState;
    BYTE        reserved[6];
} PTZ_INFO;

PLAYM4_API BOOL __stdcall PlayM4_RenderPrivateData(LONG nPort, int nIntelType, BOOL bTrue);

PLAYM4_API BOOL __stdcall PlayM4_RenderPrivateDataEx(LONG nPort, int nIntelType, int nSubType, BOOL bTrue);

PLAYM4_API BOOL __stdcall PlayM4_SetEncryptTypeCallBack(LONG nPort, DWORD nType,
                                                        void (CALLBACK *EncryptTypeCBFun)(long nPort, ENCRYPT_INFO *pEncryptInfo, long nUser, long nReserved2), long nUser);
PLAYM4_API BOOL __stdcall PlayM4_GetStreamAdditionalInfo(LONG nPort, LONG lType, BYTE *pInfo, LONG *plLen);

#endif
