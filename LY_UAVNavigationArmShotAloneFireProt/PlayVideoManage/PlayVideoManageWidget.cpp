#include "PlayVideoManageWidget.h"
#include "ui_PlayVideoManageWidget.h"

#include <QMouseEvent>
#include <QPainter>
#include <QSettings>
#include <QTextCodec>

#include <QDebug>

#if __DEBUG__LOGINFO__
#define logInfo(format, ...)
#else
#define logInfo(format, ...)

#endif

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libavutil/hwcontext.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

#define USE_VIDEO_HWDEVICE

#pragma comment(lib, "avcodec.lib")
#pragma comment(lib, "avformat.lib")
#pragma comment(lib, "avutil.lib")
#pragma comment(lib, "swresample.lib")
#pragma comment(lib, "swscale.lib")

#define SDL_MAIN_HANDLED
extern "C"
{
#include "SDL2/SDL_config.h"
#include "SDL2/SDL.h"
}

#ifdef _DEBUG
#pragma comment(lib, "SDL2d.lib")
#else
#pragma comment(lib, "SDL2.lib")
#endif

#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"

using namespace qnzkna::PlayVideoManage;

extern void SendGimbalMoveCenter(const std::string& strComponent, const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo& controlInfo);

static int RecvVideoThreadFun(void* lpParameter);

bool PlayVideoManageWidget::m_NET_DVR_Init = false;

PlayVideoManageWidget::PlayVideoManageWidget(QWidget *parent)
    : IPlayVideoManage(parent)
    , ui(new Ui::PlayVideoManageWidget)
    , m_Mutex(QMutex::RecursionMode::NonRecursive)
    , m_WId(0)
    , m_pRecvVideoThread(nullptr)
    , m_RealPlay(nullptr)
    , m_VideoDevice("", "", "", "", "", "", "", "")
    , sdl_screen(nullptr)
    , m_sdl_rendering(false)
    , m_Stop(false)
    , m_Pause(false)
    , m_strFileName("")
    , m_hwdevice_video_use(false)
    , m_hwdevice_type_name("")
    , m_hwdevice_pix_fmt_name("")
{

    QSettings settings("../data/conf/VideoStream.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup("VIDEO_HWDEVICE_SETTING");

    if(settings.contains("Video_HWDevice_hw_device_video_use") && settings.value("Video_HWDevice_hw_device_video_use").canConvert(QVariant::Type::Bool)){
        m_hwdevice_video_use = settings.value("Video_HWDevice_hw_device_video_use", "").toBool();
    }

    if(settings.contains("Video_HWDevice_hw_device_type_name") && settings.value("Video_HWDevice_hw_device_type_name").canConvert(QVariant::Type::String)){
        const QString& hwdevice_type_name = settings.value("Video_HWDevice_hw_device_type_name", "").toString();
        if(!hwdevice_type_name.isEmpty()){
            m_hwdevice_type_name = hwdevice_type_name.toStdString();
        }
    }

    if(settings.contains("Video_HWDevice_hw_device_pix_fmt_name") && settings.value("Video_HWDevice_hw_device_pix_fmt_name").canConvert(QVariant::Type::String)){
        const QString& hwdevice_pix_fmt_name = settings.value("Video_HWDevice_hw_device_pix_fmt_name", "").toString();
        if(!hwdevice_pix_fmt_name.isEmpty()){
            m_hwdevice_pix_fmt_name = hwdevice_pix_fmt_name.toStdString();
        }
    }

    settings.endGroup();

    if(!m_NET_DVR_Init)
    {
        m_NET_DVR_Init = true;
        NET_DVR_Init();
    }

    ui->setupUi(this);

    setUpdatesEnabled(false);

    QMutexLocker locker(&m_Mutex);
    m_WId              = 0;
    m_pRecvVideoThread = nullptr;
    m_Stop             = false;
    m_Pause            = false;
    m_strFileName      = "";

    av_register_all();

    avformat_network_init();

    if (SDL_Init(SDL_INIT_VIDEO) < 0 )
    {
        logInfo("Couldn't initialize SDL: %s\n", SDL_GetError());
    }

    atexit(SDL_Quit);
}

PlayVideoManageWidget::~PlayVideoManageWidget()
{
    StopVideoManageThread();

    avformat_network_deinit();

    NET_DVR_Cleanup();
    delete ui;
}

QPoint PlayVideoManageWidget::VideoWidgetPos() const
{
    return pos();
}

QSize PlayVideoManageWidget::VideoWidgetSize() const
{
    return size();
}

std::string PlayVideoManageWidget::getStrFileName() const
{
    return m_strFileName;
}

void PlayVideoManageWidget::setStrFileName(const std::string &strFileName)
{
    m_strFileName = strFileName;
}

volatile bool PlayVideoManageWidget::getStop() const
{
    return m_Stop;
}

void PlayVideoManageWidget::setStop(volatile bool Stop)
{
    m_Stop = Stop;
}

volatile bool PlayVideoManageWidget::getPause() const
{
    return m_Pause;
}

void PlayVideoManageWidget::setPause(volatile bool Pause)
{
    m_Pause = Pause;
}

void PlayVideoManageWidget::Lock()
{
    m_Mutex.lock();
}

void PlayVideoManageWidget::UnLock()
{
    m_Mutex.unlock();
}

WId PlayVideoManageWidget::WinId() const
{
    return m_WId;
}

void PlayVideoManageWidget::Show()
{

    QMetaObject::invokeMethod(this, "show", Qt::QueuedConnection);
}

bool PlayVideoManageWidget::StartVideoManageObjectHost(const CVideoDevice &videoDevice)
{
    m_VideoDevice = videoDevice;
    return true;
}

bool PlayVideoManageWidget::StartVideoManageThread(const CVideoDevice& videoDevice)
{
    m_VideoDevice = videoDevice;
    m_strFileName = videoDevice.GetVideoDeviceURL();
    if (m_strFileName.empty()) {
        return false;
    }

    setAttribute(Qt::WA_WState_Created);
    setAttribute(Qt::WA_PaintOnScreen);
    winId();

    m_WId = effectiveWinId();

    bool bResult = false;
    if (videoDevice.GetVideoDeviceType().empty() || videoDevice.GetVideoDeviceURL().empty()) {
        return false;
    }

    if (videoDevice.GetVideoDeviceType() == "hk") {

        m_RealPlay.SetPlayWnd((HWND)m_WId);
        if (m_RealPlay.Login(videoDevice.GetVideoDeviceURL(), videoDevice.GetVideoDeviceUserName(), videoDevice.GetVideoDevicePassWord(), strtoul(videoDevice.GetVideoDevicePort().c_str(), nullptr, 10))) {
            m_RealPlay.DbPlayChannel(strtol(videoDevice.GetVideoDeviceChannel().c_str(), nullptr, 10));
        }
    }
    else if (videoDevice.GetVideoDeviceType() == "rtsp") {

        m_Stop  = true;
        m_Pause = false;

        if (m_pRecvVideoThread != nullptr) {
            StopVideoManageThread();
        }

        m_Stop  = false;
        m_Pause = false;

        if(m_pRecvVideoThread == nullptr){
            if(sdl_screen == nullptr){

                WId hwnd = WinId();
                Show();
                Lock();
                sdl_screen = SDL_CreateWindowFrom((void*)hwnd);
                UnLock();
            }

            m_pRecvVideoThread = new RecvVideoThread(this);
            if(m_pRecvVideoThread != nullptr){
                connect(m_pRecvVideoThread, &QThread::finished, this, [this]()
                {

                    delete this->m_pRecvVideoThread;
                    this->m_pRecvVideoThread = nullptr;

                });
                m_pRecvVideoThread->start();
            }
        }
        bResult = (m_pRecvVideoThread == nullptr);
    }

    if (bResult) {
        logInfo("²¥·ÅÊ§°Ü");
    }
    return bResult;
}

bool PlayVideoManageWidget::StopVideoManageThread()
{
    bool bResult = false;

    if (m_VideoDevice.GetVideoDeviceType() == "hk") {

        if (m_RealPlay.GetPlayHandle() >= 0) {
            m_RealPlay.StopPlay();
        }
        bResult = true;
    }else if (m_VideoDevice.GetVideoDeviceType() == "rtsp") {
        m_Stop  = true;
        m_Pause = true;

        if (m_pRecvVideoThread != nullptr)
        {
            m_pRecvVideoThread->quit();
            m_pRecvVideoThread->wait();

            if(sdl_screen != nullptr){
                Lock();
                SDL_DestroyWindow(sdl_screen);
                UnLock();
                sdl_screen = nullptr;
            }

            bResult = true;
        }
    }

    if (!bResult) {
        logInfo("²¥·ÅÍ£Ö¹! %s", m_VideoDevice.GetVideoDeviceURL().c_str());
    }
    else {
        logInfo("²¥·ÅÍ£Ö¹Ê§°Ü! %s", m_VideoDevice.GetVideoDeviceURL().c_str());
    }

    return bResult;
}

void PlayVideoManageWidget::StartVideoPlay()
{
    StartVideoManageThread(m_VideoDevice);
}

void PlayVideoManageWidget::StopVideoPlay()
{
    StopVideoManageThread();
}

void PlayVideoManageWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event != nullptr){
        std::string strComponent = m_VideoDevice.GetVideoComponentID();
        if(!strComponent.empty()){
            bool ok = false;
            long long int componentID = QString::fromStdString(strComponent).toLongLong(&ok);
            if(ok){
                qnzkna::DY_ZC_Control::DY_ZC_ControlInfo controlInfo;
                controlInfo.componentID = componentID;
                controlInfo.oper        = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_MoveCenter;
                controlInfo.operData    = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_None;

                controlInfo.x           = event->x();
                controlInfo.y           = event->y();

                controlInfo.width       = this->width();
                controlInfo.heigth      = this->height();

                SendGimbalMoveCenter(strComponent, controlInfo);
            }
        }
    }
}

static int decode_interrupt_cb(void *tryTimes)
{
    if (tryTimes == nullptr) {
        return 0;
    }
    time_t now = ::time(nullptr);
    if ((now - (*((time_t*)tryTimes)) > 30)) {
        return 1;
    }
    return 0;
}

#ifdef USE_VIDEO_HWDEVICE

struct HWData_AVCodecContext_Private{
    bool&            video_hwdevice_init_pri;
    AVPixelFormat&   hw_pix_fmt_pri;
    AVBufferRef**    hw_device_ctx_pri;
    AVBufferRef**    hw_frames_ctx_pri;
    void (*hwaccel_uninit)(AVCodecContext *s);

    HWData_AVCodecContext_Private(bool& video_hwdevice_init_pri, AVPixelFormat& hw_pix_fmt_pri, AVBufferRef** hw_device_ctx_pri, AVBufferRef** hw_frames_ctx_pri)
        : video_hwdevice_init_pri(video_hwdevice_init_pri)
        , hw_pix_fmt_pri(hw_pix_fmt_pri)
        , hw_device_ctx_pri(hw_device_ctx_pri)
        , hw_frames_ctx_pri(hw_frames_ctx_pri)
        , hwaccel_uninit(nullptr)
    {}
};

static enum AVPixelFormat find_fmt_by_hw_type_fun(const enum AVHWDeviceType type)
{
    enum AVPixelFormat fmt;

    switch (type) {
    case AV_HWDEVICE_TYPE_CUDA:
        fmt = AV_PIX_FMT_CUDA;
        break;
    case AV_HWDEVICE_TYPE_VAAPI:
        fmt = AV_PIX_FMT_VAAPI;
        break;
    case AV_HWDEVICE_TYPE_DXVA2:
        fmt = AV_PIX_FMT_DXVA2_VLD;
        break;
    case AV_HWDEVICE_TYPE_D3D11VA:
        fmt = AV_PIX_FMT_D3D11;
        break;
    case AV_HWDEVICE_TYPE_VDPAU:
        fmt = AV_PIX_FMT_VDPAU;
        break;
    case AV_HWDEVICE_TYPE_VIDEOTOOLBOX:
        fmt = AV_PIX_FMT_VIDEOTOOLBOX;
        break;
    default:
        fmt = AV_PIX_FMT_NONE;
        break;
    }

    return fmt;
}

static enum AVPixelFormat get_hw_format_fun(AVCodecContext *ctx, const enum AVPixelFormat *pix_fmts)
{
    if(ctx == nullptr){
        return AV_PIX_FMT_NONE;
    }

    struct HWData_AVCodecContext_Private* hw_HWData_AVCodecContext_Private = (struct HWData_AVCodecContext_Private*)(ctx->opaque);

    const enum AVPixelFormat hw_pix_fmt = hw_HWData_AVCodecContext_Private->hw_pix_fmt_pri;

    const enum AVPixelFormat *p;

    for (p = pix_fmts; *p != -1; p++) {
        if (*p == hw_pix_fmt){
            return *p;
        }
    }

    hw_HWData_AVCodecContext_Private->video_hwdevice_init_pri = false;
    hw_HWData_AVCodecContext_Private->hw_pix_fmt_pri          = AVPixelFormat::AV_PIX_FMT_NONE;

    fprintf(stderr, "Failed to get HW surface format.\n");
    return AV_PIX_FMT_NONE;
}

#endif

static int RecvVideoThreadFun(void* lpParameter)
{
    int result = -1;
    PlayVideoManageWidget* playVideoManage = (PlayVideoManageWidget*)lpParameter;
    if (playVideoManage == nullptr) {
        return result;
    }

    volatile bool& sdl_rendering = playVideoManage->m_sdl_rendering;

    sdl_rendering = false;

    playVideoManage->Lock();
    const bool fileNameEmpty                 = playVideoManage->m_strFileName.empty();
    playVideoManage->UnLock();
    if(fileNameEmpty){
        return result;
    }

    char fileName[1024 + 1]               = { 0 };
    playVideoManage->Lock();
    strncpy(fileName, playVideoManage->m_strFileName.c_str(),                 1024);
    playVideoManage->UnLock();

#ifdef USE_VIDEO_HWDEVICE
    playVideoManage->Lock();
    char hwdevice_type_name[64 + 1]          = { 0 };
    char hwdevice_pix_fmt_name[64 + 1]       = { 0 };
    const bool hwdevice_video_use            = playVideoManage->m_hwdevice_video_use;
    const bool hwdevice_type_nameEmpty       = playVideoManage->m_hwdevice_type_name.empty();
    const bool hwdevice_pix_fmt_nameEmpty    = playVideoManage->m_hwdevice_pix_fmt_name.empty();

    strncpy(hwdevice_type_name,   playVideoManage->m_hwdevice_type_name.c_str(),       64);
    strncpy(hwdevice_pix_fmt_name,   playVideoManage->m_hwdevice_pix_fmt_name.c_str(),    64);
    playVideoManage->UnLock();
#endif

    bool udpUrl =
            ( (fileName[0] == 'U' || fileName[0] == 'u' )
            && (fileName[0] == 'D' || fileName[0] == 'd' )
            && (fileName[0] == 'P' || fileName[0] == 'p' )
            );

    bool localFile = false;

    std::string::size_type pos(std::string(fileName).find("://"));
    if (pos == std::string::npos || pos < 3) {
        localFile = true;
    }

    AVFormatContext*          p_fmt_ctx           = nullptr;
    AVCodecContext*           p_codec_ctx         = nullptr;
    AVCodecParameters*        p_codec_par         = nullptr;
    AVCodec*                  p_codec             = nullptr;
    AVFrame*                  p_frm_raw           = nullptr;
    AVFrame*                  p_frm_yuv           = nullptr;
    AVPacket*                 p_packet            = nullptr;
    struct SwsContext*        sws_ctx             = nullptr;
    int                       buf_size            = 0;
    uint8_t*                  buffer              = nullptr;
    int                       i                   = 0;
    int                       v_idx               = -1;
    int                       ret                 = -1;
    SDL_Window*               screen              = nullptr;
    SDL_Renderer*             sdl_renderer        = nullptr;
    int                       sdl_TextureWidth    = 0;
    int                       sdl_TextureHeight   = 0;
    SDL_Texture*              sdl_texture         = nullptr;
    SDL_Rect                  sdl_rect            = {0, 0, 0, 0};
    Uint32                    sdl_format          = SDL_PIXELFORMAT_IYUV;
    AVDictionary*             opts                = nullptr;
    enum AVPixelFormat        src_pix_fmt         = AV_PIX_FMT_YUV420P;
    enum AVPixelFormat        dst_pix_fmt         = AV_PIX_FMT_YUV420P;
#ifdef USE_VIDEO_HWDEVICE

    AVBufferRef*              hw_device_ctx       = nullptr;
    AVPixelFormat             hw_pix_fmt          = AVPixelFormat::AV_PIX_FMT_NONE;
    AVFrame*                  p_hw_sw_frame       = nullptr;
    const bool                video_hwdevice_use  = hwdevice_video_use;
    bool                      video_hwdevice_init = false;
#endif

    av_dict_set(&opts, "bufsize",     "14745600000",    0);
    av_dict_set(&opts, "buffer_size", "14745600000",    0);
    av_dict_set(&opts, "max_delay",   "400",         0);
    av_dict_set(&opts, "stimeout",    "50000000",    0);
    av_dict_set(&opts, "buffer_size", "2147480000",    0);
    av_dict_set(&opts, "tune",        "zerolatency", 0);
    av_dict_set(&opts, "preset",      "slow",        0);

    time_t tryTimes = ::time(nullptr);

    if (playVideoManage->m_Stop) {
        goto End_Stream;
    }

    p_fmt_ctx = avformat_alloc_context();
    if (p_fmt_ctx == nullptr) {
        logInfo("IPServer :%s av_malloc could not init, pFormatCtx is NULL\n", fileName);
        goto End_Stream;
    }

    logInfo("IPServer :%s \n", fileName);
    p_fmt_ctx->interrupt_callback.callback = decode_interrupt_cb;
    p_fmt_ctx->interrupt_callback.opaque   = &tryTimes;

    ret = avformat_open_input(&p_fmt_ctx, fileName, nullptr, &opts);
    if (ret != 0)
    {
        avformat_close_input(&p_fmt_ctx);
        char errorStr[AV_ERROR_MAX_STRING_SIZE] = { 0 };
        logInfo("IPServer :%s could not open : %s, Line:%d\n", fileName, av_make_error_string(errorStr, AV_ERROR_MAX_STRING_SIZE, ret), __LINE__);
        goto End_Stream;
    }

    tryTimes = ::time(nullptr);

    if (p_fmt_ctx == nullptr) {
        goto End_Stream;
    }

    p_fmt_ctx->interrupt_callback.callback = decode_interrupt_cb;
    p_fmt_ctx->interrupt_callback.opaque   = &tryTimes;

    ret = avformat_find_stream_info(p_fmt_ctx, nullptr);
    if (ret < 0)
    {
        logInfo("avformat_find_stream_info() failed\n");
        goto End_Stream;
    }

    v_idx = -1;
    for (i = 0; i < p_fmt_ctx->nb_streams; i++)
    {
        if (p_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            v_idx = i;
            logInfo("Find a video stream, index %d\n", v_idx);
            break;
        }
    }
    if (v_idx == -1)
    {
        logInfo("Cann't find a video stream\n");
        goto End_Stream;
    }

    p_codec_par = p_fmt_ctx->streams[v_idx]->codecpar;
    if (p_codec_par == nullptr) {
        logInfo("IPServer :%s pCodecCtx not found\n", fileName);
        goto End_Stream;
    }

    p_codec = avcodec_find_decoder(p_codec_par->codec_id);
    if (p_codec == nullptr)
    {
        logInfo("Cann't find codec!\n");
        goto End_Stream;
    }

    p_codec_ctx = avcodec_alloc_context3(p_codec);
    if (p_codec_ctx == nullptr)
    {
        logInfo("Cann't find p_codec_ctx!\n");
        goto End_Stream;
    }

    ret = avcodec_parameters_to_context(p_codec_ctx, p_codec_par);
    if (ret < 0)
    {
        logInfo("avcodec_parameters_to_context() failed %d\n", ret);
        goto End_Stream;
    }

    if (p_codec_ctx->width <= 0 || p_codec_ctx->height <= 0) {
        logInfo("IPServer :%s pCodecCtx not found video stream, width:%d, height:%d\n", fileName, p_codec_ctx->width, p_codec_ctx->height);
        goto End_Stream;
    }

#ifdef USE_VIDEO_HWDEVICE

    struct HWData_AVCodecContext_Private hw_HWData_AVCodecContext_Private(video_hwdevice_init, hw_pix_fmt, &hw_device_ctx, &p_codec_ctx->hw_frames_ctx);

    if(video_hwdevice_use){
        AVHWDeviceType hw_device_type      = AVHWDeviceType::AV_HWDEVICE_TYPE_NONE;
        AVHWDeviceType hw_device_type_find = AVHWDeviceType::AV_HWDEVICE_TYPE_NONE;

        if(!hwdevice_type_nameEmpty){
            hw_device_type_find = av_hwdevice_find_type_by_name(hwdevice_type_name);
            if(hw_device_type_find != AVHWDeviceType::AV_HWDEVICE_TYPE_NONE){
                hw_device_type = hw_device_type_find;
            }
        }

        if(!hwdevice_pix_fmt_nameEmpty){
            const AVPixelFormat hw_pix_fmt_find = av_get_pix_fmt(hwdevice_pix_fmt_name);
            if(hw_pix_fmt_find != AVPixelFormat::AV_PIX_FMT_NONE){
                hw_pix_fmt = hw_pix_fmt_find;
            }
        }

        if(hw_device_type == AVHWDeviceType::AV_HWDEVICE_TYPE_NONE){
            hw_device_type = AVHWDeviceType::AV_HWDEVICE_TYPE_D3D11VA;
            while((hw_device_type_find = av_hwdevice_iterate_types(hw_device_type_find)) != AVHWDeviceType::AV_HWDEVICE_TYPE_NONE){
                if(hw_device_type_find == hw_device_type){
                    hw_device_type = hw_device_type_find;
                    break;
                }
            }
        }

        if(hw_device_type_find != AVHWDeviceType::AV_HWDEVICE_TYPE_NONE){

            if(hw_pix_fmt == AVPixelFormat::AV_PIX_FMT_NONE){
                hw_pix_fmt = find_fmt_by_hw_type_fun(hw_device_type);
            }

            p_codec_ctx->opaque        = &hw_HWData_AVCodecContext_Private;
            p_codec_ctx->get_format    = get_hw_format_fun;
            if (hw_pix_fmt != AVPixelFormat::AV_PIX_FMT_NONE) {
                ret = av_hwdevice_ctx_create(&hw_device_ctx, hw_device_type, nullptr, nullptr, 0);
                if(ret == 0){
                    p_codec_ctx->opaque        = &hw_HWData_AVCodecContext_Private;
                    p_codec_ctx->hw_device_ctx = av_buffer_ref(hw_device_ctx);
                    p_hw_sw_frame              = av_frame_alloc();

                    AVHWFramesConstraints * constraints= av_hwdevice_get_hwframe_constraints(p_codec_ctx->hw_device_ctx, nullptr);
                    if(constraints){

                        for (int i = 0; constraints->valid_hw_formats[i] != AV_PIX_FMT_NONE; i++) {
                            const AVPixFmtDescriptor * desc = av_pix_fmt_desc_get(constraints->valid_hw_formats[i]);
                            if(desc && desc->name){

                            }
                        }

                        for (int i = 0; constraints->valid_sw_formats[i] != AV_PIX_FMT_NONE; i++) {
                            const AVPixFmtDescriptor * desc = av_pix_fmt_desc_get(constraints->valid_sw_formats[i]);
                             if(desc && desc->name){

                             }
                        }
                    }
                }
            }

            if(p_hw_sw_frame != nullptr && p_codec_ctx->hw_device_ctx != nullptr){
                video_hwdevice_init = true;
            }
        }
    }

#endif

    ret = avcodec_open2(p_codec_ctx, p_codec, nullptr);
    if (ret < 0)
    {
        logInfo("avcodec_open2() failed %d\n", ret);
        goto End_Stream;
    }

    src_pix_fmt = p_codec_ctx->pix_fmt;
    dst_pix_fmt = p_codec_ctx->pix_fmt;

    if(src_pix_fmt == AVPixelFormat::AV_PIX_FMT_YUV420P) {
        sdl_format = SDL_PIXELFORMAT_IYUV;
        dst_pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
    }else if(src_pix_fmt == AVPixelFormat::AV_PIX_FMT_NV12) {
        sdl_format = SDL_PIXELFORMAT_NV12;
        dst_pix_fmt = AVPixelFormat::AV_PIX_FMT_NV12;
    }else{
        sdl_format = SDL_PIXELFORMAT_IYUV;
        dst_pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
    }

    p_frm_raw = av_frame_alloc();
    p_frm_yuv = av_frame_alloc();

    buf_size = av_image_get_buffer_size(dst_pix_fmt,
                                        p_codec_ctx->width,
                                        p_codec_ctx->height,
                                        1
                                        );

    buffer = (uint8_t *)av_malloc(buf_size);

    {
        av_image_fill_arrays(p_frm_yuv->data,
                             p_frm_yuv->linesize,
                             buffer,
                             dst_pix_fmt,
                             p_codec_ctx->width,
                             p_codec_ctx->height,
                             1
                             );
    }

    src_pix_fmt = p_codec_ctx->pix_fmt;
    sws_ctx = sws_getContext(p_codec_ctx->width,
                             p_codec_ctx->height,
                             src_pix_fmt,
                             p_codec_ctx->width,
                             p_codec_ctx->height,
                             dst_pix_fmt,
                             SWS_BICUBIC,
                             nullptr,
                             nullptr,
                             nullptr
                             );
    if (sws_ctx == nullptr)
    {
        logInfo("sws_getContext() sws_ctx failed %d\n", ret);
        goto End_Stream;
    }

    playVideoManage->Lock();
    screen = playVideoManage->sdl_screen;

    playVideoManage->UnLock();

    if ( screen == nullptr ) {
        fprintf(stderr, "Couldn't set 640x480x8 video mode: %s\n", SDL_GetError());
        goto End_Stream;
    }
    if (screen == nullptr)
    {
        logInfo("SDL_CreateWindow() failed: %s\n", SDL_GetError());
        goto End_Stream;
    }

    SDL_ShowWindow(screen);
    playVideoManage->Lock();

    sdl_renderer = SDL_CreateRenderer(screen, -1, SDL_RENDERER_ACCELERATED);

    sdl_texture = SDL_CreateTexture(sdl_renderer,
                                    sdl_format,
                                    SDL_TEXTUREACCESS_STREAMING,
                                    p_codec_ctx->width,
                                    p_codec_ctx->height);
    sdl_TextureWidth  = p_codec_ctx->width;
    sdl_TextureHeight = p_codec_ctx->height;

    playVideoManage->UnLock();

    sdl_rect.x = 0;
    sdl_rect.y = 0;
    sdl_rect.w = p_codec_ctx->width;
    sdl_rect.h = p_codec_ctx->height;

    p_packet = (AVPacket *)av_malloc(sizeof(AVPacket));

#ifdef USE_VIDEO_HWDEVICE

    if(!video_hwdevice_use || hw_pix_fmt == AVPixelFormat::AV_PIX_FMT_NONE || p_hw_sw_frame == nullptr || p_codec_ctx->hw_device_ctx == nullptr){

        video_hwdevice_init = false;
    }

#endif

    int ctx_width = p_codec_ctx->width, ctx_height = p_codec_ctx->height;

    result = 0;
    int newInput = 0;
    int got_picture = 0;

    while (!playVideoManage->m_Stop)
    {
        if (playVideoManage->m_Pause) {

            QThread::msleep(100);
            logInfo("playVideoManage->m_Pause %d\n", playVideoManage->m_Pause);
            continue;
        }

        if (av_read_frame(p_fmt_ctx, p_packet) < 0) {
            char errorStr[AV_ERROR_MAX_STRING_SIZE] = { 0 };
            logInfo("avcodec_receive_frame() failed :%s, Line:%d\n", av_make_error_string(errorStr, AV_ERROR_MAX_STRING_SIZE, ret), __LINE__);
            av_packet_unref(p_packet);
            break;
        }
        tryTimes = ::time(nullptr);

        if (p_packet->stream_index == v_idx)
        {

            ret = avcodec_send_packet(p_codec_ctx, p_packet);
            if (ret < 0 && ret != AVERROR_EOF) {
                break;
            }

            if (ret != 0) {
                logInfo("avcodec_send_packet() failed %d\n", ret);
                av_packet_unref(p_packet);
                break;
            }

            do{
                ret = avcodec_receive_frame(p_codec_ctx, p_frm_raw);
                if (ret != 0)
                {
                    if(ret == AVERROR_EOF){
                        break;
                    }

                    char errorStr[AV_ERROR_MAX_STRING_SIZE] = { 0 };
                    logInfo("avcodec_receive_frame() failed :%s,  AVERROR(EAGAIN) == ret %d, Line:%d\n", av_make_error_string(errorStr, AV_ERROR_MAX_STRING_SIZE, ret), AVERROR(EAGAIN) == ret, __LINE__);

                    av_packet_unref(p_packet);
                    newInput++;
                    if (AVERROR(EAGAIN) == ret && newInput < 10) {

                    }
                    else {

                    }
                    break;
                }
                newInput = 0;

                AVFrame * p_frame_raw = p_frm_raw;

#ifdef USE_VIDEO_HWDEVICE
                if(video_hwdevice_init){
                    if (p_frm_raw->format == hw_pix_fmt && p_frm_raw->hw_frames_ctx != nullptr) {

                        if ((ret = av_hwframe_transfer_data(p_hw_sw_frame, p_frm_raw, AVHWFrameTransferDirection::AV_HWFRAME_TRANSFER_DIRECTION_FROM)) < 0) {
                            logInfo("Error transferring the data to system memory\n");
                            break;
                        }

                        p_frame_raw = p_hw_sw_frame;
                    } else {
                        p_frame_raw = p_frm_raw;
                    }
                }
#endif

                if ( src_pix_fmt != p_frame_raw->format || ctx_width != p_frame_raw->width || ctx_height != p_frame_raw->height) {

                    src_pix_fmt = (AVPixelFormat)p_frame_raw->format;
                    dst_pix_fmt = src_pix_fmt;

                    if(src_pix_fmt == AVPixelFormat::AV_PIX_FMT_YUV420P) {
                        sdl_format = SDL_PIXELFORMAT_IYUV;
                        dst_pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
                    }else if(src_pix_fmt == AVPixelFormat::AV_PIX_FMT_NV12) {
                        sdl_format = SDL_PIXELFORMAT_NV12;
                        dst_pix_fmt = AVPixelFormat::AV_PIX_FMT_NV12;
                    }else{
                        sdl_format = SDL_PIXELFORMAT_IYUV;
                        dst_pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
                    }

                    av_free(buffer);
                    buffer = nullptr;

                    sws_freeContext(sws_ctx);
                    sws_ctx = nullptr;

                    sws_ctx = sws_getContext(p_frame_raw->width,
                                             p_frame_raw->height,
                                             src_pix_fmt,
                                             p_frame_raw->width,
                                             p_frame_raw->height,
                                             dst_pix_fmt,
                                             SWS_BICUBIC,
                                             nullptr,
                                             nullptr,
                                             nullptr
                                             );
                    if (sws_ctx == nullptr) {
                        logInfo("IPServer :%s sws_getCachedContext could not init\n", fileName);
                        break;
                    }

                    buf_size = av_image_get_buffer_size(dst_pix_fmt,
                                                        p_frame_raw->width,
                                                        p_frame_raw->height,
                                                        1
                                                        );

                    buffer = (uint8_t *)av_malloc(buf_size);
                    if (buffer == nullptr) {
                        logInfo("IPServer :%s av_malloc could not init\n", fileName);
                        break;
                    }

#ifdef USE_VIDEO_HWDEVICE
                    if(dst_pix_fmt == src_pix_fmt && video_hwdevice_init && p_frm_raw->format == hw_pix_fmt){
                        av_image_fill_arrays(p_frame_raw->data,
                                             p_frame_raw->linesize,
                                             buffer,
                                             dst_pix_fmt,
                                             p_frame_raw->width,
                                             p_frame_raw->height,
                                             1
                                             );
                    } else
#endif
                    {
                        av_image_fill_arrays(p_frm_yuv->data,
                                             p_frm_yuv->linesize,
                                             buffer,
                                             dst_pix_fmt,
                                             p_frame_raw->width,
                                             p_frame_raw->height,
                                             1
                                             );

                    }

                    ctx_width  = p_frame_raw->width;
                    ctx_height = p_frame_raw->height;

                    if (sdl_texture != nullptr) {
                        SDL_DestroyTexture(sdl_texture);
                        sdl_texture = nullptr;
                    }

                    sdl_texture = SDL_CreateTexture(sdl_renderer,
                                                    sdl_format,
                                                    SDL_TEXTUREACCESS_STREAMING,
                                                    p_frame_raw->width,
                                                    p_frame_raw->height);
                    sdl_TextureWidth  = p_frame_raw->width;
                    sdl_TextureHeight = p_frame_raw->height;
                }

                if(src_pix_fmt == AVPixelFormat::AV_PIX_FMT_YUV420P && dst_pix_fmt == AVPixelFormat::AV_PIX_FMT_YUV420P ) {
                    got_picture = true;
                }
#ifdef USE_VIDEO_HWDEVICE
                else if (dst_pix_fmt == src_pix_fmt && video_hwdevice_init && p_frm_raw->format == hw_pix_fmt)
                {
                    got_picture = true;
                }
#endif
                else if (dst_pix_fmt == src_pix_fmt)
                {
                    got_picture = true;
                    av_image_copy_to_buffer(buffer,
                                            buf_size,
                                            p_frame_raw->data,
                                            p_frame_raw->linesize,
                                            dst_pix_fmt,
                                            p_frame_raw->width,
                                            p_frame_raw->height,
                                            1
                                         );
                } else
                {
                    got_picture = sws_scale(sws_ctx,
                                            (const uint8_t *const *)p_frame_raw->data,
                                            p_frame_raw->linesize,
                                            0,
                                            p_frame_raw->height,
                                            p_frm_yuv->data,
                                            p_frm_yuv->linesize
                                            );
                }

                if(got_picture > 0){
                    if (p_frame_raw->width != sdl_TextureWidth || p_frame_raw->height != sdl_TextureHeight) {
                        if (sdl_texture != nullptr) {
                            SDL_DestroyTexture(sdl_texture);
                            sdl_texture = nullptr;
                        }

                        sdl_texture = SDL_CreateTexture(sdl_renderer,
                                                        sdl_format,
                                                        SDL_TEXTUREACCESS_STREAMING,
                                                        p_frame_raw->width,
                                                        p_frame_raw->height);
                        sdl_TextureWidth  = p_frame_raw->width;
                        sdl_TextureHeight = p_frame_raw->height;
                    }

                    playVideoManage->Lock();

                    sdl_rendering = true;

                    if( sdl_format == SDL_PIXELFORMAT_IYUV){
                        SDL_UpdateYUVTexture(sdl_texture, nullptr,
                                             p_frame_raw->data[0], p_frame_raw->linesize[0],
                                             p_frame_raw->data[1], p_frame_raw->linesize[1],
                                             p_frame_raw->data[2], p_frame_raw->linesize[2]);
                    } else if (sdl_format == SDL_PIXELFORMAT_NV12 ){
                        SDL_UpdateTexture(sdl_texture, nullptr, buffer, p_frame_raw->width);
                    } else {
                        SDL_UpdateTexture(sdl_texture, nullptr, buffer, p_frame_raw->width);
                    }

                    SDL_RenderClear(sdl_renderer);

                    SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, nullptr);

                    SDL_RenderPresent(sdl_renderer);

                    sdl_rendering = false;

                    playVideoManage->UnLock();

                }

                if (localFile) {

                    QThread::msleep(40);
                }

            }while(0);
        }
        av_packet_unref(p_packet);
        av_init_packet(p_packet);
    }

End_Stream:

    if (sdl_texture != nullptr) {
        playVideoManage->Lock();
        SDL_DestroyTexture(sdl_texture);
        playVideoManage->UnLock();
        sdl_texture = nullptr;
    }

    if (sdl_renderer != nullptr) {
        playVideoManage->Lock();
        SDL_DestroyRenderer(sdl_renderer);
        playVideoManage->UnLock();
        sdl_renderer = nullptr;
    }

    if (screen != nullptr) {
        playVideoManage->Lock();

        playVideoManage->UnLock();

    }

    if (p_packet != nullptr) {
        av_free(p_packet);
    }

    if (sws_ctx != nullptr) {
        sws_freeContext(sws_ctx);
    }

    if (buffer != nullptr) {
        av_free(buffer);
    }

    if (p_frm_yuv != nullptr) {
        av_frame_free(&p_frm_yuv);
    }

    if (p_frm_raw != nullptr) {
        av_frame_free(&p_frm_raw);
    }

    if (p_codec_ctx != nullptr) {
        avcodec_close(p_codec_ctx);
        avcodec_free_context(&p_codec_ctx);
    }

    if (p_fmt_ctx != nullptr) {
        avformat_close_input(&p_fmt_ctx);
    }

#ifdef USE_VIDEO_HWDEVICE
    if(hw_device_ctx != nullptr){
        av_buffer_unref(&hw_device_ctx);
    }

    if (p_hw_sw_frame != nullptr) {
        av_frame_free(&p_hw_sw_frame);
    }
#endif

    return result;
}

void RecvVideoThread::run()
{
    if(m_pPlayVideoManageWidget != nullptr)
    {
        m_pPlayVideoManageWidget->Lock();
        bool fileNameEmpty = m_pPlayVideoManageWidget->m_strFileName.empty();
        m_pPlayVideoManageWidget->UnLock();
        if(fileNameEmpty){
            return ;
        }

        for(; (m_pPlayVideoManageWidget && (!m_pPlayVideoManageWidget->m_Stop)); ){

            RecvVideoThreadFun(m_pPlayVideoManageWidget);

            for(int i = 0 ; i < 10 && (!m_pPlayVideoManageWidget->m_Stop); i++){
                if(!m_pPlayVideoManageWidget->m_Stop){
                    QThread::msleep(1000);
                }else{
                    break;
                }
            }
        }
    }
}

