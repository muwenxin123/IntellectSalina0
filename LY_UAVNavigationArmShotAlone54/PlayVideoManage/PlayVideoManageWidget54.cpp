#include "PlayVideoManageWidget54.h"
#include "ui_PlayVideoManageWidget54.h"

#include <QMouseEvent>
#include <QPainter>
#include <QNetworkDatagram>
#include <QNetworkInterface>
#include <QSettings>
#include <QTextCodec>
#include <QDir>

#include "LY_UAVNavigationArmShotAlone54_global.h"

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
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

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

#include <QBuffer>
#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"
#include "IUAVNavigationArmShotAlone54Activator.h"

static const int  Video_Infrare_Top_Dist = 25;
static const int  Video_Infrare_Scale    = 15;

using namespace qnzkna::PlayVideoManage;

extern void SendGimbalMoveCenter(const std::string &strComponent, const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo &controlInfo);

int RecvVideoThreadFun54(void *lpParameter);

bool PlayVideoManageWidget54::m_NET_DVR_Init = false;
PlayVideoManageWidget54::PlayVideoManageWidget54(QWidget *parent) :
    IPlayVideoManage(parent),
    ui(new Ui::PlayVideoManageWidget54)
    , m_Mutex(QMutex::RecursionMode::NonRecursive)
    , m_WId(0)
    , m_ShowVideoLight(true)
    , m_WidgetUpdatesEnabled(false)
    , m_pShowVideoThread(nullptr)
    , m_pShowVideoThreadObject(nullptr)
    , m_pRecvVideoThread(nullptr)
    , m_pRecvVideoThreadObject(nullptr)
    , m_pRecvVideoThreadInfrared(nullptr)
    , m_pRecvVideoThreadObjectInfrared(nullptr)
    , m_VideoDevice("", "", "", "", "", "", "", "", "")
    , m_sdl_rendering(false)
    , m_sdl_render_times(0)
    , sdl_screen(nullptr)
    , m_Stop(false)
    , m_Pause(false)
    , m_strFileName("")
    , m_strFileNameInfrared("")
{

    QString videoMulticastInterfaceName("");
    QString VideoCodecName("");
    const QString strApplicationDirPath = QCoreApplication::applicationDirPath();
    QSettings settings(strApplicationDirPath + "/Yolo/" + "YoloConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    qDebug() << __FILE__ << __LINE__ << strApplicationDirPath << settings.allKeys() << settings.childKeys();
    settings.beginGroup("VideoStreamSetting");
    if (settings.contains("VideoMulticastInterfaceName") && settings.value("VideoMulticastInterfaceName").canConvert(QVariant::Type::String))
    {
        videoMulticastInterfaceName = settings.value("VideoMulticastInterfaceName", "").toString();
    }

    if (settings.contains("VideoCodecName") && settings.value("VideoCodecName").canConvert(QVariant::Type::String))
    {
        VideoCodecName = settings.value("VideoCodecName", "").toString();
    }
    settings.endGroup();

    m_VideoDeviceParam.setVideoDeviceMulticastInterfaceName(videoMulticastInterfaceName);
    m_VideoDeviceParam.SetVideoDeviceCodecName(VideoCodecName);

    bool videoInfraredEnable = true;
    settings.beginGroup("VideoInfraredEnabledConfig");
    if (settings.contains("VideoInfraredEnable"))
    {
        videoInfraredEnable = settings.value("VideoInfraredEnable", "true").toBool();
    }
    settings.endGroup();

    m_VideoDeviceParam.SetVideoDeviceInfraredEnable(videoInfraredEnable);

    uint8_t buffer [] =
    {
        0x00, 0x00, 0x01, 0x67, 0x64, 0x00, 0x2a, 0xad, 0x84, 0x01,
        0x0c, 0x20, 0x08, 0x61, 0x00, 0x43, 0x08, 0x02, 0x18, 0x40,
        0x10, 0xc2, 0x00, 0x84, 0x3b, 0x50, 0x3c, 0x01, 0x13, 0xf2,
        0xcd, 0xc0, 0x40, 0x40, 0x40, 0x80, 0x00, 0x00, 0x00, 0x01,
        0x68, 0xee, 0x3c, 0xb0, 0x00,
    };

    const QByteArray VideoDeviceCodecExtData(QByteArray((char *)buffer, sizeof(buffer)));
    m_VideoDeviceParam.SetVideoDeviceCodecExtData(VideoDeviceCodecExtData);
    m_VideoDeviceParam.SetVideoDeviceCodecParam(true);

    uint8_t bufferInfrared [] =
    {
        0x00, 0x00, 0x01, 0x67, 0x4d, 0x00, 0x1e, 0x96, 0x35, 0x41,
        0x40, 0x20, 0xd3, 0x70, 0x10, 0x10, 0x14, 0x00, 0x00, 0x1c,
        0x20, 0x00, 0x05, 0x7e, 0x42, 0x10, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x68, 0xee,
        0x31, 0xb2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    };

    const QByteArray VideoDeviceCodecExtDataInfrared(QByteArray((char *)bufferInfrared, sizeof(bufferInfrared)));
    m_VideoDeviceParam.SetVideoDeviceCodecExtDataInfrared(VideoDeviceCodecExtDataInfrared);
    m_VideoDeviceParam.SetVideoDeviceCodecParamInfrared(true);

    if (!m_NET_DVR_Init)
    {
        m_NET_DVR_Init = true;
    }

    ui->setupUi(this);

    setUpdatesEnabled(false);

    QMutexLocker locker(&m_Mutex);
    m_WId                             = 0;
    m_ShowVideoLight                  = true;
    m_pShowVideoThread                = nullptr;
    m_pShowVideoThreadObject          = nullptr;
    m_pRecvVideoThread                = nullptr;
    m_pRecvVideoThreadObject          = nullptr;
    m_pRecvVideoThreadInfrared        = nullptr;
    m_pRecvVideoThreadObjectInfrared  = nullptr;
    m_Stop                            = false;
    m_Pause                           = false;
    m_strFileName                     = "";
    m_strFileNameInfrared             = "";

    {
        bool showFlagVideoRealTimePreview = false;
        const QString settingsVideoRealTimePreviewFilePath = QApplication::applicationDirPath() + "/Data/conf/Ly_VideoRealTimePreview.ini";
        QSettings settingsVideoRealTimePreview(settingsVideoRealTimePreviewFilePath, QSettings::IniFormat);
        settingsVideoRealTimePreview.setIniCodec(QTextCodec::codecForName("utf-8"));
        settingsVideoRealTimePreview.beginGroup("Config");
        if (settingsVideoRealTimePreview.contains("ShowFlag"))
        {
            showFlagVideoRealTimePreview = settingsVideoRealTimePreview.value("ShowFlag", false).toBool();
        }
        settingsVideoRealTimePreview.endGroup();

		m_collection = showFlagVideoRealTimePreview;
    }

    av_register_all();

    avformat_network_init();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        logInfo("Couldn't initialize SDL: %s\n", SDL_GetError());
    }

    SDL_StopTextInput();

    for (Uint32 i = 0; i < 256; i++)
    {
        SDL_EventState(i, SDL_DISABLE);
    }

    atexit(SDL_Quit);
}

PlayVideoManageWidget54::~PlayVideoManageWidget54()
{
    StopVideoManageThread();

    if (m_pShowVideoThreadObject)
    {
        m_pShowVideoThreadObject->deleteLater();
    }

    if (m_pRecvVideoThreadObject)
    {
        m_pRecvVideoThreadObject->deleteLater();
    }

    if (m_pRecvVideoThreadObjectInfrared)
    {
        m_pRecvVideoThreadObjectInfrared->deleteLater();
    }

    if (this->sdl_screen != nullptr)
    {
        SDL_DestroyWindow(this->sdl_screen);
        this->sdl_screen = nullptr;
    }

    avformat_network_deinit();
    delete ui;
}

QPoint PlayVideoManageWidget54::VideoWidgetPos() const
{
    return pos();
}

QSize PlayVideoManageWidget54::VideoWidgetSize() const
{
    return size();
}

std::string PlayVideoManageWidget54::getStrFileName() const
{
    return m_strFileName;
}

void PlayVideoManageWidget54::setStrFileName(const std::string &strFileName)
{
    m_strFileName = strFileName;
}

volatile bool PlayVideoManageWidget54::getStop() const
{
    return m_Stop;
}

void PlayVideoManageWidget54::setStop(volatile bool Stop)
{
    m_Stop = Stop;
}

volatile bool PlayVideoManageWidget54::getPause() const
{
    return m_Pause;
}

void PlayVideoManageWidget54::setPause(volatile bool Pause)
{
    m_Pause = Pause;
}

void PlayVideoManageWidget54::Lock()
{
    m_Mutex.lock();
}

void PlayVideoManageWidget54::UnLock()
{
    m_Mutex.unlock();
}

WId PlayVideoManageWidget54::WinId() const
{
    return m_WId;
}

void PlayVideoManageWidget54::Show()
{
    QMetaObject::invokeMethod(this, "show", Qt::QueuedConnection);
}

void PlayVideoManageWidget54::setVideoType(int videoType)
{
    if (videoType == 2)
    {
        m_ShowVideoLight = true;
    }
    else if (videoType == 1)
    {
        m_ShowVideoLight = false;
    }
    else
    {
        m_ShowVideoLight = true;
    }
}

bool PlayVideoManageWidget54::StartVideoManageObjectHost(const CVideoDevice &videoDevice)
{
    m_VideoDevice = videoDevice;
    return true;
}

bool PlayVideoManageWidget54::StartVideoManageThread(const CVideoDevice &videoDevice)
{
    if (m_pRecvVideoThread != nullptr && m_pRecvVideoThreadInfrared != nullptr)
    {
        return true;
    }

    m_VideoDevice = videoDevice;
    m_strFileName = videoDevice.GetVideoDeviceURL();
    if (m_strFileName.empty())
    {
        return false;
    }

    m_strFileNameInfrared = videoDevice.GetVideoDeviceURLInfrared();

    m_VideoDeviceParam.SetVideoDeviceType54((videoDevice.GetVideoType_54() != "0"));

    setAttribute(Qt::WA_WState_Created);
    setAttribute(Qt::WA_PaintOnScreen);
    winId();

    m_WId = effectiveWinId();

    bool bResult = false;
    if (videoDevice.GetVideoDeviceType().empty() || videoDevice.GetVideoDeviceURL().empty())
    {
        return false;
    }

    if (videoDevice.GetVideoDeviceType() == "rtsp54")
    {

        m_Stop  = true;
        m_Pause = false;

        if (m_pRecvVideoThread != nullptr || m_pRecvVideoThreadInfrared != nullptr)
        {
            StopVideoManageThread();
        }

        m_Stop  = false;
        m_Pause = false;

        if (m_pShowVideoThread == nullptr)
        {

            if (this->sdl_screen != nullptr)
            {
                SDL_DestroyWindow(this->sdl_screen);
                this->sdl_screen = nullptr;
            }

            if (sdl_screen == nullptr)
            {
                WId hwnd = WinId();
                Show();
                m_sdl_rendering = true;
                Lock();
                sdl_screen = SDL_CreateWindowFrom((void *)hwnd);
                UnLock();
                m_sdl_rendering = false;
            }

            m_pShowVideoThread = new QThread();
            m_pShowVideoThreadObject = new ShowVideoStreamThread(this, nullptr);
            if (m_pShowVideoThread != nullptr && m_pShowVideoThreadObject != nullptr)
            {
                m_pShowVideoThreadObject->moveToThread(m_pShowVideoThread);
                connect(m_pShowVideoThread, &QThread::finished, this, [this]()
                {
                    delete this->m_pShowVideoThread;
                    this->m_pShowVideoThread = nullptr;

                    if (this->m_pShowVideoThreadObject != nullptr)
                    {
                        delete this->m_pShowVideoThreadObject;
                        this->m_pShowVideoThreadObject = nullptr;
                    }

                });

                connect(m_pShowVideoThread, &QThread::finished, m_pShowVideoThreadObject, &ShowVideoStreamThread::video_doStop_Sig);
                connect(m_pShowVideoThread, &QThread::started, m_pShowVideoThreadObject, &ShowVideoStreamThread::video_doWork_Sig);

                m_pShowVideoThread->start();
            }
        }

        if (m_pRecvVideoThread == nullptr)
        {
            m_pRecvVideoThreadObject = new RecvVideoStreamThread(QString::fromStdString(m_strFileName), m_VideoDeviceParam, false, nullptr);
            m_pRecvVideoThread = new QThread();
            if (m_pRecvVideoThread != nullptr && m_pRecvVideoThreadObject != nullptr)
            {
                m_pRecvVideoThreadObject->moveToThread(m_pRecvVideoThread);
                connect(m_pRecvVideoThread, &QThread::finished, this, [this]()
                {
                    delete this->m_pRecvVideoThread;
                    this->m_pRecvVideoThread = nullptr;

                    if (this->m_pRecvVideoThreadObject != nullptr)
                    {
                        delete this->m_pRecvVideoThreadObject;
                        this->m_pRecvVideoThreadObject = nullptr;
                    }
                });
                connect(m_pRecvVideoThread, &QThread::finished, m_pRecvVideoThreadObject, &RecvVideoStreamThread::video_doStop_Sig);

                connect(m_pRecvVideoThread, &QThread::started, m_pRecvVideoThreadObject, &RecvVideoStreamThread::video_doWork_Sig);

                m_pRecvVideoThread->start();
            }
        }

        if (!m_strFileNameInfrared.empty() && m_pRecvVideoThreadInfrared == nullptr)
        {
            m_pRecvVideoThreadObjectInfrared = new RecvVideoStreamThread(QString::fromStdString(m_strFileNameInfrared), m_VideoDeviceParam, true, nullptr);
            m_pRecvVideoThreadInfrared = new QThread();
            if (m_pRecvVideoThreadInfrared != nullptr && m_pRecvVideoThreadObjectInfrared != nullptr)
            {
                m_pRecvVideoThreadObjectInfrared->moveToThread(m_pRecvVideoThreadInfrared);
                connect(m_pRecvVideoThreadInfrared, &QThread::finished, this, [this]()
                {
                    delete this->m_pRecvVideoThreadInfrared;
                    this->m_pRecvVideoThreadInfrared = nullptr;

                    if (this->m_pRecvVideoThreadObjectInfrared != nullptr)
                    {
                        delete this->m_pRecvVideoThreadObjectInfrared;
                        this->m_pRecvVideoThreadObjectInfrared = nullptr;
                    }
                });
                connect(m_pRecvVideoThreadInfrared, &QThread::finished, m_pRecvVideoThreadObjectInfrared, &RecvVideoStreamThread::video_doStop_Sig);

                connect(m_pRecvVideoThreadInfrared, &QThread::started, m_pRecvVideoThreadObjectInfrared, &RecvVideoStreamThread::video_doWork_Sig);

                m_pRecvVideoThreadInfrared->start();
            }
        }

        bResult = (m_pRecvVideoThread == nullptr);
    }

    if (bResult)
    {
        logInfo("²¥·ÅÊ§°Ü");
    }
    return bResult;
}

bool PlayVideoManageWidget54::StopVideoManageThread()
{
    bool bResult = false;

    if (m_VideoDevice.GetVideoDeviceType() == "rtsp54")
    {
        m_Stop  = true;
        m_Pause = true;

        if (m_pShowVideoThread != nullptr)
        {
            m_pShowVideoThread->quit();
            m_pShowVideoThread->wait();
            m_pShowVideoThread->deleteLater();
            bResult = true;
        }

        if (m_pRecvVideoThread != nullptr)
        {
            m_pRecvVideoThread->quit();
            m_pRecvVideoThread->wait();
            m_pRecvVideoThread->deleteLater();

            bResult = true;
        }

        if (m_pRecvVideoThreadInfrared != nullptr)
        {
            m_pRecvVideoThreadInfrared->quit();
            m_pRecvVideoThreadInfrared->wait();
            m_pRecvVideoThreadInfrared->deleteLater();

            bResult = true;
        }
    }

    if (!bResult)
    {
        logInfo("²¥·ÅÍ£Ö¹! %s", m_VideoDevice.GetVideoDeviceURL().c_str());
    }
    else
    {
        logInfo("²¥·ÅÍ£Ö¹Ê§°Ü! %s", m_VideoDevice.GetVideoDeviceURL().c_str());
    }

    return bResult;
}

void PlayVideoManageWidget54::StartVideoPlay()
{
    StartVideoManageThread(m_VideoDevice);
}

void PlayVideoManageWidget54::StopVideoPlay()
{
    StopVideoManageThread();
}

void PlayVideoManageWidget54::SetWidgetUpdatesEnabled(bool update)
{
    if (m_WidgetUpdatesEnabled != update)
    {
        m_WidgetUpdatesEnabled = update;
        setUpdatesEnabled(update);
    }
}
void PlayVideoManageWidget54::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void PlayVideoManageWidget54::paintEvent(QPaintEvent *event)
{
    if (m_sdl_rendering)
    {
        return ;
    }
    Lock();
    IPlayVideoManage::paintEvent(event);
    UnLock();
}

void PlayVideoManageWidget54::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event != nullptr && m_ShowVideoLight)
    {
        std::string strComponent = m_VideoDevice.GetVideoComponentID();
        if (!strComponent.empty())
        {
            bool ok = false;
            long long int componentID = QString::fromStdString(strComponent).toLongLong(&ok);
            if (ok)
            {
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

void qnzkna::PlayVideoManage::PlayVideoManageWidget54::appendAvframe(AVFrame* avframe)
{
	auto pSensor = IUAVNavigationArmShotAlone54Activator::Get().getPISensorVedioService();
	if (nullptr != pSensor)
	{
		pSensor->AddPixMap(std::atoi(m_VideoDevice.GetVideoComponentID().c_str()), avframe);
	}
	else
	{
		av_frame_free(&avframe);
	}
}

bool qnzkna::PlayVideoManage::PlayVideoManageWidget54::canCollection()
{
	return m_collection;
}

NetworkWorkerThread::NetworkWorkerThread(const multicast_configure_t &multicast_configure, const bool videoType_54, QObject *parent)
    : QThread(parent)
    , _init(false)
    , _videoType_54(videoType_54)
    , _multicast_configure_t(multicast_configure)
    , _udpSocket(nullptr)
{
}

NetworkWorkerThread::~NetworkWorkerThread()
{
    if (_udpSocket)
    {
        _udpSocket->close();
        _udpSocket->deleteLater();

        _udpSocket = nullptr;

        _init = false;
    }
}

void NetworkWorkerThread::run()
{
    if (!_udpSocket)
    {

        const auto &networkInterface = QNetworkInterface::interfaceFromName(_multicast_configure_t.interfacename);

        if (networkInterface.addressEntries().size() <= 0)
        {
            return ;
        }

        _udpSocket = new QUdpSocket();
        if (!_udpSocket)
        {
            return ;
        }

        _udpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);

        bool result        = false;
        bool joinMulticast = false;

        if (_multicast_configure_t.addressrecv.isMulticast())
        {
            result = _udpSocket->bind(QHostAddress(QHostAddress::SpecialAddress::AnyIPv4), _multicast_configure_t.portrecv, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        }
        else
        {
            result = _udpSocket->bind(_multicast_configure_t.addressrecv, _multicast_configure_t.portrecv, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        }

        _udpSocket->setMulticastInterface(networkInterface);

        if (_multicast_configure_t.addressrecv.isMulticast())
        {
            joinMulticast = _udpSocket->joinMulticastGroup(_multicast_configure_t.addressrecv, networkInterface);
        }

        if(_videoType_54){
            connect(_udpSocket, &QUdpSocket::readyRead, [this]()
            {
                while (_udpSocket && _udpSocket->hasPendingDatagrams())
                {
                    const QNetworkDatagram &datagram = _udpSocket->receiveDatagram();

                    if (_multicast_configure_t.addressrecv != datagram.destinationAddress())
                    {
                        continue;
                    }

                    const QByteArray &data = datagram.data();

                    QByteArray dataVideoRecvStream;
                    for (int i = 0; (i + 256) <= data.size(); i += 256)
                    {
                        if (data.at(i) != 0x47)
                        {
                            break;
                        }
                        int start =  i + 1;
                        dataVideoRecvStream.append(QByteArray((data.constData() + start), (256 - 1)));
                    }

                    if (dataVideoRecvStream.size() > 0)
                    {
                        QMutexLocker locker(&_VideoRecvBufferMutex);
                        if (_VideoRecvBufferList.size() > 2000)
                        {
                            _VideoRecvBufferList.removeFirst();
                        }
                        _VideoRecvBufferList.append(dataVideoRecvStream);
                        locker.unlock();
                    }
                }
            });
        }
        else
        {
            connect(_udpSocket, &QUdpSocket::readyRead, [this]()
            {
                while (_udpSocket && _udpSocket->hasPendingDatagrams())
                {
                    const QNetworkDatagram &datagram = _udpSocket->receiveDatagram();

                    if (_multicast_configure_t.addressrecv != datagram.destinationAddress())
                    {
                        continue;
                    }

                    const QByteArray &data = datagram.data();

                    if (data.size() > 0)
                    {
                        QMutexLocker locker(&_VideoRecvBufferMutex);
                        if (_VideoRecvBufferList.size() > 2000)
                        {
                            _VideoRecvBufferList.removeFirst();
                        }
                        _VideoRecvBufferList.append(data);
                        locker.unlock();
                    }
                }
            });
        }


        connect(this, &QThread::finished, [this]()
        {
            qDebug() << __FILE__ << __LINE__
                     << _multicast_configure_t.interfacename
                     << _multicast_configure_t.addressrecv
                     << _multicast_configure_t.portrecv
                     << _multicast_configure_t.addresssend
                     << _multicast_configure_t.portsend
                     ;
            if (_udpSocket)
            {
                _udpSocket->close();
                _udpSocket->deleteLater();

                _udpSocket = nullptr;

                _init = false;
            }
        });

        _init = true;
    }
    QThread::run();
}

void NetworkWorkerThread::GetMatVideoRecvBufferList(QLinkedList<const QByteArray> &videoRecvBufferList)
{
    QMutexLocker locker(&_VideoRecvBufferMutex);
    videoRecvBufferList = std::move(_VideoRecvBufferList);
    _VideoRecvBufferList.clear();
    locker.unlock();
}

bool NetworkWorkerThread::init() const
{
    return _init;
}

RecvVideoStreamThread::RecvVideoStreamThread(const QString &strUrlName, const CVideoDeviceParam &videoDeviceParam, bool urlNameDeviceType54Infrared, QObject *parent)
    : QObject(parent)
    , m_TimerOpencvRecvWindow(this)
    , m_UrlName(strUrlName)
    , m_UrlNameDeviceType54(false)
    , m_UrlNameDeviceType54Infrared(urlNameDeviceType54Infrared)
    , m_MulticastNetworkWorkerThread(nullptr)
    , m_AVFrame(nullptr)
    , m_AVCodecContext(nullptr)
    , m_AVCodecParserContext(nullptr)
    , m_AVFrameDest_Sws(nullptr)
    , m_SwsContext(nullptr)
    , m_Width_Sws(0)
    , m_Height_Sws(0)
    , m_Format_Sws(-1)
    , m_Buffer_Size_Sws(0)
    , m_Buffer_Sws(nullptr)
    , m_VideoDeviceParam(videoDeviceParam)
{
    connect(this, &RecvVideoStreamThread::video_doWork_Sig, this, &RecvVideoStreamThread::video_doWork);
    connect(this, &RecvVideoStreamThread::video_doStop_Sig, this, &RecvVideoStreamThread::video_stop);
}

RecvVideoStreamThread::~RecvVideoStreamThread()
{
    video_stop();

    if (m_MulticastNetworkWorkerThread)
    {
        m_MulticastNetworkWorkerThread->quit();
        m_MulticastNetworkWorkerThread->wait(60);
        m_MulticastNetworkWorkerThread->deleteLater();
    }

    if (m_AVCodecContext)
    {
        avcodec_close(m_AVCodecContext);
        avcodec_free_context(&m_AVCodecContext);
        m_AVCodecContext = nullptr;
    }

    if (m_AVCodecParserContext)
    {
        av_parser_close(m_AVCodecParserContext);
        m_AVCodecParserContext = nullptr;
    }

    if (m_AVFrame)
    {
        av_frame_free(&m_AVFrame);
        m_AVFrame = nullptr;
    }

    if (m_AVFrameDest_Sws)
    {
        av_frame_free(&m_AVFrameDest_Sws);
        m_AVFrameDest_Sws = nullptr;
    }

    if (m_SwsContext != nullptr)
    {
        sws_freeContext(m_SwsContext);
        m_SwsContext = nullptr;
    }

    if (m_Buffer_Sws != nullptr)
    {
        av_free(m_Buffer_Sws);
        m_Buffer_Sws = nullptr;
    }

    QMutexLocker locker(&m_VideoRecvStreamMutex);
    if (m_VideoRecvStreamList.size() > 0)
    {

        QLinkedList<RecvVideoStreamThread::VideoRecvFrame>::iterator messageReferenceFrameFreeIter;
        for (messageReferenceFrameFreeIter = m_VideoRecvStreamList.begin(); messageReferenceFrameFreeIter != m_VideoRecvStreamList.end(); ++messageReferenceFrameFreeIter)
        {
            av_frame_free(&(messageReferenceFrameFreeIter->frame));
        }

        m_VideoRecvStreamList.clear();
    }
    locker.unlock();
}

void RecvVideoStreamThread::GetMatVideoRecvStreamList(QLinkedList<VideoRecvFrame> &videoRecvStreamList)
{
    QMutexLocker locker(&m_VideoRecvStreamMutex);
    if (m_VideoRecvStreamList.size() > 0)
    {
        videoRecvStreamList = std::move(m_VideoRecvStreamList);
    }
    locker.unlock();
}

void RecvVideoStreamThread::video_doWork()
{

    if (m_UrlName.isEmpty())
    {
        return ;
    }

    m_UrlNameDeviceType54 = true;

    qDebug() << "UrlNameDeviceType54 " << m_UrlNameDeviceType54;
    if (m_UrlNameDeviceType54)
    {

        const QString    &videoDeviceMulticastInterfaceName = m_VideoDeviceParam.GetVideoDeviceMulticastInterfaceName();
        const QString    &videoDeviceCodecName              = m_VideoDeviceParam.GetVideoDeviceCodecName();
        const QByteArray &videoDeviceCodecExtData           = (m_UrlNameDeviceType54Infrared ? m_VideoDeviceParam.GetVideoDeviceCodecExtDataInfrared() : m_VideoDeviceParam.GetVideoDeviceCodecExtData());
        const bool       &videoDeviceCodecParam             = (m_UrlNameDeviceType54Infrared ? m_VideoDeviceParam.GetVideoDeviceCodecParamInfrared() : m_VideoDeviceParam.GetVideoDeviceCodecParam());

        if (m_AVFrame == nullptr)
        {
            av_register_all();

            m_AVFrame = av_frame_alloc();
            if (m_AVFrame == nullptr)
            {
                qDebug() << "Failed to allocate frame.";
                return ;
            }

            QString findVideoCodecName("");
            for (AVCodec *p_codec = av_codec_next(nullptr) ; p_codec != nullptr; p_codec = av_codec_next(p_codec))
            {
                if (p_codec)
                {

                    if (p_codec->name && videoDeviceCodecName == QString(p_codec->name))
                    {
                        findVideoCodecName = videoDeviceCodecName;
                    }

                    if (p_codec->pix_fmts)
                    {
                        const enum AVPixelFormat *p;
                        for (p = p_codec->pix_fmts; *p != -1; p++)
                        {
                        }
                    }

                    if (p_codec->priv_class)
                    {
                    }

                    if (p_codec->profiles)
                    {
                    }

                }

            }

            const AVCodec *p_codec = nullptr;

            if (!findVideoCodecName.isEmpty())
            {
                p_codec = avcodec_find_decoder_by_name(findVideoCodecName.toStdString().c_str());
            }

            if (p_codec == nullptr)
            {
                p_codec = avcodec_find_decoder(AV_CODEC_ID_H264);
            }

            if (p_codec == nullptr)
            {
                qDebug() << __FILE__ << __LINE__ << " Failed to find H.264 decoder.";
                return ;
            }

            AVCodecContext *p_codec_ctx = avcodec_alloc_context3(p_codec);
            if (p_codec_ctx == nullptr)
            {
                qDebug() << "Failed to allocate H.264 coder context.";
                return ;
            }

            AVCodecParameters *pAVCodecParameters = nullptr;
            if (videoDeviceCodecParam && findVideoCodecName.isEmpty())
            {
                pAVCodecParameters = avcodec_parameters_alloc();
                uint8_t *buffer      = nullptr;
                int      buffer_size = 0;
                if (!videoDeviceCodecExtData.isEmpty())
                {
                    buffer = (uint8_t *)av_mallocz(videoDeviceCodecExtData.length() + AV_INPUT_BUFFER_PADDING_SIZE);
                    if (buffer != nullptr)
                    {
                        buffer_size = videoDeviceCodecExtData.length();
                        memcpy(buffer, videoDeviceCodecExtData.data(), buffer_size);
                    }
                }

                {
                    AVCodecParameters *par = pAVCodecParameters;
                    par->codec_type = AVMediaType::AVMEDIA_TYPE_VIDEO;
                    par->codec_id   = AVCodecID::AV_CODEC_ID_H264;
                    par->codec_tag  = 27;

                    par->bit_rate                 = 0;
                    par->bits_per_coded_sample    = 0;
                    par->bits_per_raw_sample      = 8;
                    par->block_align              = 0;
                    par->channel_layout           = 0;
                    par->channels                 = 0;
                    par->chroma_location          = AVChromaLocation::AVCHROMA_LOC_LEFT;
                    par->codec_id                 = AVCodecID::AV_CODEC_ID_H264;
                    par->codec_tag                = 0;
                    par->codec_type               = AVMediaType::AVMEDIA_TYPE_VIDEO;
                    par->color_primaries          = AVColorPrimaries::AVCOL_PRI_BT709;
                    par->color_range              = AVColorRange::AVCOL_RANGE_JPEG;
                    par->color_space              = AVColorSpace::AVCOL_SPC_BT709;
                    par->color_trc                = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
                    par->extradata                = buffer;
                    par->extradata_size           = buffer_size;
                    par->field_order              = AVFieldOrder::AV_FIELD_PROGRESSIVE;
                    par->format                   = 12;
                    par->frame_size               = 0;
                    par->height                   = 1080;
                    par->initial_padding          = 0;
                    par->level                    = 42;
                    par->profile                  = 100;
                    par->sample_aspect_ratio.den  = 0;
                    par->sample_aspect_ratio.den  = 1;
                    par->sample_aspect_ratio.num  = 0;
                    par->sample_rate              = 0;
                    par->seek_preroll             = 0;
                    par->trailing_padding         = 0;
                    par->video_delay              = 0;
                    par->width                    = 1920;

                    if (m_UrlNameDeviceType54Infrared)
                    {
                        par->bit_rate                 = 0;
                        par->bits_per_coded_sample    = 0;
                        par->bits_per_raw_sample      = 8;
                        par->block_align              = 0;
                        par->channel_layout           = 0;
                        par->channels                 = 0;
                        par->chroma_location          = AVChromaLocation::AVCHROMA_LOC_LEFT;
                        par->codec_id                 = AVCodecID::AV_CODEC_ID_H264;
                        par->codec_tag                = 0;
                        par->codec_type               = AVMediaType::AVMEDIA_TYPE_VIDEO;
                        par->color_primaries          = AVColorPrimaries::AVCOL_PRI_BT709;
                        par->color_range              = AVColorRange::AVCOL_RANGE_JPEG;
                        par->color_space              = AVColorSpace::AVCOL_SPC_BT709;
                        par->color_trc                = AVColorTransferCharacteristic::AVCOL_TRC_BT709;
                        par->extradata                = buffer;
                        par->extradata_size           = buffer_size;
                        par->field_order              = AVFieldOrder::AV_FIELD_PROGRESSIVE;
                        par->format                   = 12;
                        par->frame_size               = 0;
                        par->height                   = 512;
                        par->initial_padding          = 0;
                        par->level                    = 30;
                        par->profile                  = 77;
                        par->sample_aspect_ratio.den  = 0;
                        par->sample_aspect_ratio.den  = 1;
                        par->sample_aspect_ratio.num  = 0;
                        par->sample_rate              = 0;
                        par->seek_preroll             = 0;
                        par->trailing_padding         = 0;
                        par->video_delay              = 0;
                        par->width                    = 640;
                    }
                }

                {
                    AVCodecContext *codec = p_codec_ctx;
                    const AVCodecParameters *par = pAVCodecParameters;
                    codec->codec_type = par->codec_type;
                    codec->codec_id   = par->codec_id;
                    codec->codec_tag  = par->codec_tag;

                    codec->bit_rate              = par->bit_rate;
                    codec->bits_per_coded_sample = par->bits_per_coded_sample;
                    codec->bits_per_raw_sample   = par->bits_per_raw_sample;
                    codec->profile               = par->profile;
                    codec->level                 = par->level;

                    switch (par->codec_type)
                    {
                    case AVMEDIA_TYPE_VIDEO:
                        codec->pix_fmt                = (AVPixelFormat)par->format;
                        codec->width                  = par->width;
                        codec->height                 = par->height;
                        codec->field_order            = par->field_order;
                        codec->color_range            = par->color_range;
                        codec->color_primaries        = par->color_primaries;
                        codec->color_trc              = par->color_trc;
                        codec->colorspace             = par->color_space;
                        codec->chroma_sample_location = par->chroma_location;
                        codec->sample_aspect_ratio    = par->sample_aspect_ratio;
                        codec->has_b_frames           = par->video_delay;
                        break;
                    case AVMEDIA_TYPE_AUDIO:
                        codec->sample_fmt       = (AVSampleFormat)par->format;
                        codec->channel_layout   = par->channel_layout;
                        codec->channels         = par->channels;
                        codec->sample_rate      = par->sample_rate;
                        codec->block_align      = par->block_align;
                        codec->frame_size       = par->frame_size;
                        codec->delay            =
                            codec->initial_padding  = par->initial_padding;
                        codec->trailing_padding = par->trailing_padding;
                        codec->seek_preroll     = par->seek_preroll;
                        break;
                    case AVMEDIA_TYPE_SUBTITLE:
                        codec->width  = par->width;
                        codec->height = par->height;
                        break;
                    }

                    if (par->extradata)
                    {
                        av_freep(&codec->extradata);
                        codec->extradata = (uint8_t *)av_mallocz(par->extradata_size + AV_INPUT_BUFFER_PADDING_SIZE);
                        if (codec->extradata != nullptr)
                        {
                            memcpy(codec->extradata, par->extradata, par->extradata_size);
                            codec->extradata_size = par->extradata_size;
                        }
                    }
                }
            }

            p_codec_ctx->flags        |= AV_CODEC_FLAG_LOW_DELAY;
            p_codec_ctx->flags2       |= AV_CODEC_FLAG2_FAST;

            if (videoDeviceCodecParam && pAVCodecParameters != nullptr && findVideoCodecName.isEmpty())
            {
                avcodec_parameters_to_context(p_codec_ctx, pAVCodecParameters);

                if (pAVCodecParameters->extradata != nullptr)
                {
                    av_freep(&pAVCodecParameters->extradata);
                    pAVCodecParameters->extradata      = nullptr;
                    pAVCodecParameters->extradata_size = 0;
                }

                avcodec_parameters_free(&pAVCodecParameters);
                pAVCodecParameters = nullptr;
            }

            AVCodecParserContext *pCodecParserCtx = av_parser_init(p_codec->id);
            if (pCodecParserCtx == nullptr)
            {
                qDebug() << "Failed to allocate H.264 coder parser context.";
                return ;
            }

            int ret = avcodec_open2(p_codec_ctx, p_codec, nullptr);
            if (ret < 0)
            {
                qDebug() << "Failed to open H.264 decoder.";
                avcodec_free_context(&p_codec_ctx);
                return ;
            }

            av_log_set_level(AV_LOG_FATAL);

            m_AVCodecContext       = p_codec_ctx;

            m_AVCodecParserContext = pCodecParserCtx;
        }

        NetworkWorkerThread::multicast_configure_t multi;
        multi.interfacename = videoDeviceMulticastInterfaceName;
        if (!videoDeviceMulticastInterfaceName.isEmpty())
        {
            for (const QNetworkInterface &networkInterface : QNetworkInterface::allInterfaces())
            {
                if (networkInterface.humanReadableName() == videoDeviceMulticastInterfaceName)
                {
                    multi.interfacename = networkInterface.name();
                    break;
                }
            }
        }
        else
        {
            for (const QNetworkInterface &networkInterface : QNetworkInterface::allInterfaces())
            {
                if (networkInterface.flags().testFlag(QNetworkInterface::CanMulticast) && networkInterface.flags().testFlag(QNetworkInterface::IsRunning) && networkInterface.flags().testFlag(QNetworkInterface::IsUp) && !networkInterface.flags().testFlag(QNetworkInterface::IsLoopBack))
                {
                    multi.interfacename = networkInterface.name();
                    break;
                }
            }
        }

        const QUrl url(m_UrlName);
        if (url.isValid())
        {
            multi.addressrecv   = QHostAddress(url.host());
            multi.portrecv      = url.port();
            multi.addresssend   = QHostAddress("");
            multi.portsend      = 0;

            m_MulticastNetworkWorkerThread = new NetworkWorkerThread(multi, m_VideoDeviceParam.GetVideoDeviceType54());
            if (m_MulticastNetworkWorkerThread)
            {
                m_MulticastNetworkWorkerThread->start();
            }
        }

        QThread::sleep(1);
    }

    qDebug() << __FILE__ << __LINE__ << m_UrlName << QThread::currentThreadId();

    connect(&m_TimerOpencvRecvWindow, &QTimer::timeout, this, &RecvVideoStreamThread::RecvStream);

    if (!m_UrlName.isEmpty())
    {
        video_start();
    }
}

void RecvVideoStreamThread::video_start()
{
    qDebug() << __FILE__ << __LINE__ << " " << m_UrlName << " " << m_UrlNameDeviceType54 << QThread::currentThreadId();
    if (m_UrlName.isEmpty())
    {
        return ;
    }

    if (!m_TimerOpencvRecvWindow.isActive())
    {
        int videoTimeout = 16;
        m_TimerOpencvRecvWindow.start(videoTimeout);

    }

}

void RecvVideoStreamThread::video_stop()
{
    if (m_TimerOpencvRecvWindow.isActive())
    {
        m_TimerOpencvRecvWindow.stop();
    }
}

void RecvVideoStreamThread::video_exit()
{
    qDebug() << __FILE__ << __LINE__ << "video_exit";
    video_stop();

    if (this->thread())
    {
        this->thread()->quit();
        this->thread()->exit();
    }
}

void RecvVideoStreamThread::Decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt)
{
    int ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret == AVERROR_EOF)
    {
        avcodec_flush_buffers(dec_ctx);

        return ;
    }
    else if (ret < 0)
    {

        return ;
    }

    while (ret == 0)
    {
        ret = avcodec_receive_frame(dec_ctx, frame);

        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        {
            return ;
        }
        else if (ret < 0)
        {
            return ;
        }

        if (ret == 0 && frame->format != AVPixelFormat::AV_PIX_FMT_NONE)
        {
            if (frame->format == AVPixelFormat::AV_PIX_FMT_YUV420P || frame->format == AVPixelFormat::AV_PIX_FMT_YUVJ420P)
            {
                AVFrame *dst = av_frame_alloc();
                av_frame_move_ref(dst, m_AVFrame);

                QMutexLocker locker(&m_VideoRecvStreamMutex);
                if (m_VideoRecvStreamList.size() > 60 )
                {
                    AVFrame *temp = m_VideoRecvStreamList.first().frame;
                    m_VideoRecvStreamList.removeFirst();
                    av_frame_unref(temp);
                    av_frame_free(&temp);
                }
                m_VideoRecvStreamList.append(VideoRecvFrame(dst, dst->width, dst->height, SDL_PIXELFORMAT_IYUV));
                locker.unlock();

            }
            else
            {

                if (m_Format_Sws != frame->format || m_Width_Sws != frame->width || m_Height_Sws != frame->height)
                {

                    m_Format_Sws = frame->format;
                    m_Width_Sws  = frame->width;
                    m_Height_Sws = frame->height;

                    if (m_SwsContext != nullptr)
                    {
                        sws_freeContext(m_SwsContext);
                        m_SwsContext = nullptr;
                    }

                    const enum AVPixelFormat dst_pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;

                    m_SwsContext = sws_getContext(frame->width,
                                                  frame->height,
                                                  (AVPixelFormat)frame->format,
                                                  frame->width,
                                                  frame->height,
                                                  dst_pix_fmt,
                                                  SWS_FAST_BILINEAR,
                                                  nullptr,
                                                  nullptr,
                                                  nullptr
                                                 );

                    if (m_Buffer_Sws != nullptr)
                    {
                        av_free(m_Buffer_Sws);
                        m_Buffer_Sws = nullptr;
                    }

                    m_Buffer_Size_Sws = av_image_get_buffer_size(dst_pix_fmt,
                                                                 frame->width,
                                                                 frame->height,
                                                                 1
                                                                );

                    m_Buffer_Sws = (uint8_t *)av_malloc(m_Buffer_Size_Sws);

                    if (m_AVFrameDest_Sws)
                    {
                        av_frame_free(&m_AVFrameDest_Sws);
                        m_AVFrameDest_Sws = nullptr;
                    }

                    m_AVFrameDest_Sws = av_frame_alloc();

                    av_image_fill_arrays(m_AVFrameDest_Sws->data,
                                         m_AVFrameDest_Sws->linesize,
                                         m_Buffer_Sws,
                                         dst_pix_fmt,
                                         frame->width,
                                         frame->height,
                                         1
                                        );
                }

                if (m_SwsContext != nullptr && m_Buffer_Sws != nullptr && m_AVFrameDest_Sws != nullptr)
                {

                    AVFrame *dst = av_frame_alloc();

                    if (dst != nullptr)
                    {
                        const enum AVPixelFormat dst_pix_fmt = AVPixelFormat::AV_PIX_FMT_YUV420P;
                        av_image_alloc(dst->data, dst->linesize, frame->width, frame->height, dst_pix_fmt, 1);
                        int got_picture = sws_scale(m_SwsContext,
                                                    (const uint8_t *const *)frame->data,
                                                    frame->linesize,
                                                    0,
                                                    frame->height,
                                                    dst->data,
                                                    dst->linesize
                                                   );
                        if (got_picture > 0)
                        {
                            QMutexLocker locker(&m_VideoRecvStreamMutex);
                            if (m_VideoRecvStreamList.size() > 60 )
                            {
                                AVFrame *temp = m_VideoRecvStreamList.first().frame;
                                m_VideoRecvStreamList.removeFirst();
                                av_frame_unref(temp);
                                av_frame_free(&temp);
                            }
                            m_VideoRecvStreamList.append(VideoRecvFrame(dst, dst->width, dst->height, SDL_PIXELFORMAT_IYUV));
                            locker.unlock();
                        }
                        else
                        {
                            av_frame_free(&dst);
                        }
                    }
                }
            }
        }
    }
}

void RecvVideoStreamThread::RecvStream()
{
    if (!m_UrlNameDeviceType54)
    {
        return ;
    }

    if (m_MulticastNetworkWorkerThread && m_AVFrame && m_AVCodecContext && m_AVCodecParserContext)
    {

        QLinkedList<const QByteArray> listMessage;
        listMessage.clear();
        m_MulticastNetworkWorkerThread->GetMatVideoRecvBufferList(listMessage);

        if (listMessage.size() <= 0)
        {
            return;
        }

        AVPacket out_pkt;
        QLinkedList<const QByteArray>::const_iterator messageReferenceIter;
        for (messageReferenceIter = listMessage.constBegin(); messageReferenceIter != listMessage.constEnd(); ++messageReferenceIter)
        {
            if (messageReferenceIter->size() <= 0)
            {
                continue ;
            }

            const uint8_t *in_data = (uint8_t *)(messageReferenceIter->data());
            int in_len             = messageReferenceIter->size();

            int len = 0;
            while (in_len > 0)
            {

                av_init_packet(&out_pkt);
                len = av_parser_parse2(m_AVCodecParserContext, m_AVCodecContext,
                                       &out_pkt.data, &out_pkt.size,
                                       in_data, in_len,
                                       AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
                if (len < 0)
                {
                    break;
                }
                in_data += len;
                in_len  -= len ;

                if (out_pkt.size > 0 && out_pkt.stream_index == AVMediaType::AVMEDIA_TYPE_VIDEO)
                {
                    Decode(m_AVCodecContext, m_AVFrame, &out_pkt);
                    av_packet_unref(&out_pkt);
                }
            }
            av_packet_unref(&out_pkt);
        }
    }
}

ShowVideoStreamThread::ShowVideoStreamThread(PlayVideoManageWidget54 *widget, QObject *parent)
    : QObject(parent)
    , m_TimerOpencvShowWindow(this)
    , m_WidgetVideo(widget)
    , m_UrlNameDeviceType54(false)
{
    connect(this, &ShowVideoStreamThread::video_doWork_Sig, this, &ShowVideoStreamThread::video_doWork);
    connect(this, &ShowVideoStreamThread::video_doStop_Sig, this, &ShowVideoStreamThread::video_stop);
    qDebug() << __FILE__ << __LINE__ << widget << QThread::currentThreadId();
}

ShowVideoStreamThread::~ShowVideoStreamThread()
{
    video_stop();

    m_WidgetVideo->m_sdl_rendering = true;
    if (sdl_texture != nullptr && m_WidgetVideo != nullptr)
    {
        m_WidgetVideo->Lock();
        SDL_DestroyTexture(sdl_texture);
        sdl_texture = nullptr;
        m_WidgetVideo->UnLock();
    }

    if (sdl_renderer != nullptr && m_WidgetVideo != nullptr)
    {
        m_WidgetVideo->Lock();
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = nullptr;
        m_WidgetVideo->UnLock();
    }
    m_WidgetVideo->Lock();
    if (sdl_texture != nullptr)
    {
        SDL_DestroyTexture(sdl_texture);
        sdl_texture = nullptr;
    }

    if (sdl_textureInfrared != nullptr)
    {
        SDL_DestroyTexture(sdl_textureInfrared);
        sdl_textureInfrared = nullptr;
    }

    if (sdl_renderer != nullptr && m_WidgetVideo != nullptr)
    {
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = nullptr;
    }
    m_WidgetVideo->UnLock();
    m_WidgetVideo->m_sdl_rendering = false;

    if (m_WidgetVideo && m_WidgetVideo->m_pRecvVideoThreadObject)
    {
        QLinkedList<RecvVideoStreamThread::VideoRecvFrame> listMessage;
        m_WidgetVideo->m_pRecvVideoThreadObject->GetMatVideoRecvStreamList(listMessage);

        QLinkedList<RecvVideoStreamThread::VideoRecvFrame> listMessageInfrared;
        if (m_WidgetVideo->m_pRecvVideoThreadObjectInfrared)
        {
            m_WidgetVideo->m_pRecvVideoThreadObjectInfrared->GetMatVideoRecvStreamList(listMessageInfrared);
        }

        QLinkedList<RecvVideoStreamThread::VideoRecvFrame>::iterator messageReferenceFrameFreeIter;
        for (messageReferenceFrameFreeIter = listMessage.begin(); messageReferenceFrameFreeIter != listMessage.end(); ++messageReferenceFrameFreeIter)
        {
            av_frame_free(&(messageReferenceFrameFreeIter->frame));
        }

        listMessage.clear();

        QLinkedList<RecvVideoStreamThread::VideoRecvFrame>::iterator messageReferenceFrameFreeIterInfrared;
        for (messageReferenceFrameFreeIterInfrared = listMessageInfrared.begin(); messageReferenceFrameFreeIterInfrared != listMessageInfrared.end(); ++messageReferenceFrameFreeIterInfrared)
        {
            av_frame_free(&(messageReferenceFrameFreeIterInfrared->frame));
        }
        listMessageInfrared.clear();
    }

    if (m_VideoRecvFrame != nullptr)
    {
        av_frame_free(&(m_VideoRecvFrame));
        m_VideoRecvFrame = nullptr;
    }

    if (m_VideoRecvFrameInfrared != nullptr)
    {
        av_frame_free(&(m_VideoRecvFrameInfrared));
        m_VideoRecvFrameInfrared = nullptr;
    }
}

void ShowVideoStreamThread::video_doWork()
{
    if (m_WidgetVideo == nullptr)
    {
        return ;
    }

    m_UrlNameDeviceType54 = true;

    qDebug() << "UrlNameDeviceType54 " << m_UrlNameDeviceType54;

    sdl_screen = m_WidgetVideo->sdl_screen;
    if (m_WidgetVideo == nullptr)
    {
        return ;
    }

    m_WidgetVideo->m_sdl_rendering = true;
    m_WidgetVideo->Lock();

    sdl_TextureWidth                  = 1920;
    sdl_TextureHeight                 = 1080;

    sdl_rect.x                        = 0;
    sdl_rect.y                        = 0;
    sdl_rect.w                        = sdl_TextureWidth;
    sdl_rect.h                        = sdl_TextureHeight;

    sdl_rect_Min.x                    = sdl_rect.x;
    sdl_rect_Min.y                    = sdl_rect.y + Video_Infrare_Top_Dist;
    sdl_rect_Min.w                    = sdl_rect.w / Video_Infrare_Scale;
    sdl_rect_Min.h                    = sdl_rect.h / Video_Infrare_Scale;

    m_VideoRecvFrame                  = nullptr;
    m_VideoRecvFrame_Width            = 0;
    m_VideoRecvFrame_Height           = 0;
    m_VideoRecvFrame_Format           = 0;

    m_VideoRecvFrameInfrared          = nullptr;
    m_VideoRecvFrameInfrared_Width    = sdl_TextureWidth;
    m_VideoRecvFrameInfrared_Height   = sdl_TextureHeight;
    m_VideoRecvFrameInfrared_Format   = 0;

    sdl_TextureWidth          = 0;
    sdl_TextureHeight         = 0;

    m_WidgetVideo->UnLock();
    m_WidgetVideo->m_sdl_rendering = false;

    qDebug() << __FILE__ << __LINE__ << m_WidgetVideo << QThread::currentThreadId();

    connect(&m_TimerOpencvShowWindow, &QTimer::timeout, this, &ShowVideoStreamThread::ShowVideoRecvStream);

    if (m_WidgetVideo != nullptr)
    {
        video_start();
    }
}

void ShowVideoStreamThread::video_start()
{
    m_captureImageCnt = 0;
    qDebug() << __FILE__ << __LINE__ << "video_start()" << m_WidgetVideo << " " << m_UrlNameDeviceType54 << QThread::currentThreadId();
    if (m_WidgetVideo == nullptr)
    {
        return ;
    }

    if (!m_TimerOpencvShowWindow.isActive())
    {
        const int videoTimeout = 16;
        m_TimerOpencvShowWindow.start(videoTimeout);

    }

}

void ShowVideoStreamThread::video_stop()
{
    if (m_TimerOpencvShowWindow.isActive())
    {
        m_TimerOpencvShowWindow.stop();
    }
}

void ShowVideoStreamThread::video_exit()
{
    qDebug() << __FILE__ << __LINE__ << "video_exit";
    video_stop();

    if (this->thread())
    {
        this->thread()->quit();
        this->thread()->exit();
    }

}

void ShowVideoStreamThread::ShowVideoRecvStream()
{
    if (!m_UrlNameDeviceType54)
    {
        return ;
    }

    if (m_WidgetVideo && m_WidgetVideo->m_pRecvVideoThreadObject && sdl_screen)
    {
        QLinkedList<RecvVideoStreamThread::VideoRecvFrame> listMessage;
        m_WidgetVideo->m_pRecvVideoThreadObject->GetMatVideoRecvStreamList(listMessage);

        QLinkedList<RecvVideoStreamThread::VideoRecvFrame> listMessageInfrared;
        if (m_WidgetVideo->m_pRecvVideoThreadObjectInfrared)
        {
            m_WidgetVideo->m_pRecvVideoThreadObjectInfrared->GetMatVideoRecvStreamList(listMessageInfrared);
        }

        if (listMessage.size() <= 0 && listMessageInfrared.size() <= 0)
        {
            if (m_WidgetVideo->m_sdl_render_times < 1000)
            {
                m_WidgetVideo->m_sdl_render_times++;

                if (m_WidgetVideo->m_sdl_render_times < 100)
                {
                    if (m_WidgetVideo->m_sdl_render_times == 99)
                    {
                        if (m_VideoRecvFrameInfrared == nullptr && sdl_textureInfrared == nullptr)
                        {
                            m_WidgetVideo->SetWidgetUpdatesEnabled(true);
                        }
                    }
                    else if (m_WidgetVideo->m_sdl_render_times < 10)
                    {
                        m_WidgetVideo->SetWidgetUpdatesEnabled(false);
                    }
                }

                if (m_WidgetVideo->m_sdl_render_times == 999)
                {
                    const int videoTimeout = 500;
                    m_TimerOpencvShowWindow.start(videoTimeout);
                }
            }
        }
        else
        {
            if (m_WidgetVideo->m_sdl_render_times >= 900)
            {
                m_WidgetVideo->m_sdl_render_times = 0;
                m_TimerOpencvShowWindow.stop();
                const int videoTimeout = 20;
                m_TimerOpencvShowWindow.start(videoTimeout);
            }
        }

        const bool showVideoLight = m_WidgetVideo->m_ShowVideoLight;

        if (listMessageInfrared.size() > 0)
        {
            if (m_VideoRecvFrameInfrared != nullptr)
            {
                av_frame_free(&(m_VideoRecvFrameInfrared));
                m_VideoRecvFrameInfrared = nullptr;
            }

            const RecvVideoStreamThread::VideoRecvFrame &videoRecvFrameTemp = listMessageInfrared.back();

            m_VideoRecvFrameInfrared        = videoRecvFrameTemp.frame;
            m_VideoRecvFrameInfrared_Width  = videoRecvFrameTemp.width;
            m_VideoRecvFrameInfrared_Height = videoRecvFrameTemp.height;
            m_VideoRecvFrameInfrared_Format = videoRecvFrameTemp.format;

            listMessageInfrared.pop_back();
        }

        m_WidgetVideo->m_sdl_rendering = true;
        QLinkedList<RecvVideoStreamThread::VideoRecvFrame>::const_iterator messageReferenceIter = listMessage.constBegin();

        for (; messageReferenceIter != listMessage.constEnd(); messageReferenceIter++)
        {
            if (messageReferenceIter->frame == nullptr
                    || messageReferenceIter->format < 0
                    || messageReferenceIter->width <= 0
                    || messageReferenceIter->height <= 0
               )
            {
                continue ;
            }

            if ((sdl_texture == nullptr) || ((sdl_format == messageReferenceIter->format) &&
                                             (
                                                 sdl_TextureWidthInfrared != m_VideoRecvFrameInfrared_Width || sdl_TextureHeightInfrared != m_VideoRecvFrameInfrared_Height ||
                                                 sdl_TextureWidth != messageReferenceIter->width || sdl_TextureHeight != messageReferenceIter->height
                                             )))
            {

                m_WidgetVideo->Lock();

                if (sdl_texture != nullptr)
                {
                    SDL_DestroyTexture(sdl_texture);
                    sdl_texture = nullptr;
                }

                if (sdl_textureInfrared != nullptr)
                {
                    SDL_DestroyTexture(sdl_textureInfrared);
                    sdl_textureInfrared = nullptr;
                }

                if (sdl_renderer != nullptr && m_WidgetVideo != nullptr)
                {
                    SDL_DestroyRenderer(sdl_renderer);
                    sdl_renderer = nullptr;
                }

                sdl_renderer = SDL_CreateRenderer(sdl_screen, -1, SDL_RENDERER_ACCELERATED);

                sdl_TextureWidth  = messageReferenceIter->width;
                sdl_TextureHeight = messageReferenceIter->height;

                if (sdl_TextureWidth > 0 && sdl_TextureHeight > 0)
                {
                    sdl_texture = SDL_CreateTexture(sdl_renderer,
                                                    sdl_format,
                                                    SDL_TEXTUREACCESS_STREAMING,
                                                    sdl_TextureWidth,
                                                    sdl_TextureHeight);
                }

                sdl_TextureWidthInfrared  = m_VideoRecvFrameInfrared_Width;
                sdl_TextureHeightInfrared = m_VideoRecvFrameInfrared_Height;
                if (sdl_TextureWidthInfrared > 0 && sdl_TextureHeightInfrared > 0)
                {
                    sdl_textureInfrared = SDL_CreateTexture(sdl_renderer,
                                                            sdl_format,
                                                            SDL_TEXTUREACCESS_STREAMING,
                                                            sdl_TextureWidthInfrared,
                                                            sdl_TextureHeightInfrared);
                }

                m_WidgetVideo->UnLock();

                sdl_rect.x        = 0;
                sdl_rect.y        = 0;
                sdl_rect.w        = sdl_TextureWidth;
                sdl_rect.h        = sdl_TextureHeight;

                sdl_rect_Min.x    = sdl_rect.x;
                sdl_rect_Min.y    = sdl_rect.y        + Video_Infrare_Top_Dist;
                sdl_rect_Min.w    = sdl_TextureWidth  / Video_Infrare_Scale;
                sdl_rect_Min.h    = sdl_TextureHeight / Video_Infrare_Scale;

                sdl_TextureWidthInfrared  = m_VideoRecvFrameInfrared_Width;
                sdl_TextureHeightInfrared = m_VideoRecvFrameInfrared_Height;
            }

            m_WidgetVideo->m_sdl_render_times = 0;
            m_WidgetVideo->Lock();

            SDL_RenderClear(sdl_renderer);
            if (showVideoLight && sdl_texture != nullptr)
            {
                if (sdl_format == SDL_PIXELFORMAT_IYUV)
                {
                    SDL_UpdateYUVTexture(sdl_texture, nullptr,
                                         messageReferenceIter->frame->data[0], messageReferenceIter->frame->linesize[0],
                                         messageReferenceIter->frame->data[1], messageReferenceIter->frame->linesize[1],
                                         messageReferenceIter->frame->data[2], messageReferenceIter->frame->linesize[2]);
                }
                SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, nullptr);

                if (m_VideoRecvFrameInfrared != nullptr && sdl_textureInfrared != nullptr)
                {
                    if (m_VideoRecvFrameInfrared_Format == SDL_PIXELFORMAT_IYUV)
                    {
                        SDL_UpdateYUVTexture(sdl_textureInfrared, nullptr,
                                             m_VideoRecvFrameInfrared->data[0], m_VideoRecvFrameInfrared->linesize[0],
                                             m_VideoRecvFrameInfrared->data[1], m_VideoRecvFrameInfrared->linesize[1],
                                             m_VideoRecvFrameInfrared->data[2], m_VideoRecvFrameInfrared->linesize[2]);
                    }
                    SDL_RenderCopy(sdl_renderer, sdl_textureInfrared, nullptr, &sdl_rect_Min);
                }
            }
            else if (sdl_texture != nullptr)
            {
                if (m_VideoRecvFrameInfrared != nullptr && sdl_textureInfrared != nullptr)
                {
                    if (m_VideoRecvFrameInfrared_Format == SDL_PIXELFORMAT_IYUV)
                    {
                        SDL_UpdateYUVTexture(sdl_textureInfrared, nullptr,
                                             m_VideoRecvFrameInfrared->data[0], m_VideoRecvFrameInfrared->linesize[0],
                                             m_VideoRecvFrameInfrared->data[1], m_VideoRecvFrameInfrared->linesize[1],
                                             m_VideoRecvFrameInfrared->data[2], m_VideoRecvFrameInfrared->linesize[2]);
                    }
                    SDL_RenderCopy(sdl_renderer, sdl_textureInfrared, nullptr, nullptr);
                }

                if (sdl_format == SDL_PIXELFORMAT_IYUV)
                {
                    SDL_UpdateYUVTexture(sdl_texture, nullptr,
                                         messageReferenceIter->frame->data[0], messageReferenceIter->frame->linesize[0],
                                         messageReferenceIter->frame->data[1], messageReferenceIter->frame->linesize[1],
                                         messageReferenceIter->frame->data[2], messageReferenceIter->frame->linesize[2]);
                }
                SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, &sdl_rect_Min);
            }

#ifdef  USE_ALL_IMAGE
            if (m_WidgetVideo->canCollection() && nullptr != messageReferenceIter->frame)
#else
            if (m_WidgetVideo->canCollection() && nullptr != messageReferenceIter->frame && (++m_captureImageCnt) % 4 == 0)
#endif
            {
                m_captureImageCnt = 0;

                auto avFrame = messageReferenceIter->frame;
				if (nullptr != avFrame && avFrame->data[0])
				{
					AVFrame* p_frmav_tmp = av_frame_clone(avFrame);
					if (nullptr != p_frmav_tmp)
					{
						m_WidgetVideo->appendAvframe(p_frmav_tmp);
					}
				}
				m_captureImageCnt++;
            }
            SDL_RenderPresent(sdl_renderer);
            m_WidgetVideo->UnLock();
        }

        if (listMessage.size() > 0)
        {
            if (m_VideoRecvFrame != nullptr)
            {
                av_frame_free(&(m_VideoRecvFrame));
                m_VideoRecvFrame = nullptr;
            }

            const RecvVideoStreamThread::VideoRecvFrame &videoRecvFrameTemp = listMessage.back();
            m_VideoRecvFrame        = videoRecvFrameTemp.frame;
            m_VideoRecvFrame_Width  = videoRecvFrameTemp.width;
            m_VideoRecvFrame_Height = videoRecvFrameTemp.height;
            m_VideoRecvFrame_Format = videoRecvFrameTemp.format;

            listMessage.pop_back();
        }
        else if (m_VideoRecvFrameInfrared != nullptr)
        {
            if ((sdl_texture == nullptr && m_VideoRecvFrame_Width > 0 && m_VideoRecvFrame_Height > 0) ||
                    (sdl_textureInfrared == nullptr && m_VideoRecvFrameInfrared_Width > 0 && m_VideoRecvFrameInfrared_Height > 0) ||
                    ((sdl_format == m_VideoRecvFrameInfrared_Format) &&
                     (
                         sdl_TextureWidthInfrared != m_VideoRecvFrameInfrared_Width || sdl_TextureHeightInfrared != m_VideoRecvFrameInfrared_Height ||
                         sdl_TextureWidth != m_VideoRecvFrame_Width || sdl_TextureHeight != m_VideoRecvFrame_Height
                     )))
            {

                m_WidgetVideo->Lock();

                if (sdl_texture != nullptr)
                {
                    SDL_DestroyTexture(sdl_texture);
                    sdl_texture = nullptr;
                }

                if (sdl_textureInfrared != nullptr)
                {
                    SDL_DestroyTexture(sdl_textureInfrared);
                    sdl_textureInfrared = nullptr;
                }

                if (sdl_renderer != nullptr && m_WidgetVideo != nullptr)
                {
                    SDL_DestroyRenderer(sdl_renderer);
                    sdl_renderer = nullptr;
                }

                sdl_renderer = SDL_CreateRenderer(sdl_screen, -1, SDL_RENDERER_ACCELERATED);

                sdl_TextureWidth  = m_VideoRecvFrame_Width;
                sdl_TextureHeight = m_VideoRecvFrame_Height;

                if (sdl_TextureWidth > 0 && sdl_TextureHeight > 0)
                {
                    sdl_texture = SDL_CreateTexture(sdl_renderer,
                                                    sdl_format,
                                                    SDL_TEXTUREACCESS_STREAMING,
                                                    sdl_TextureWidth,
                                                    sdl_TextureHeight);
                }

                sdl_TextureWidthInfrared  = m_VideoRecvFrameInfrared_Width;
                sdl_TextureHeightInfrared = m_VideoRecvFrameInfrared_Height;
                if (sdl_TextureWidthInfrared > 0 && sdl_TextureHeightInfrared > 0)
                {
                    sdl_textureInfrared = SDL_CreateTexture(sdl_renderer,
                                                            sdl_format,
                                                            SDL_TEXTUREACCESS_STREAMING,
                                                            sdl_TextureWidthInfrared,
                                                            sdl_TextureHeightInfrared);
                }

                m_WidgetVideo->UnLock();

                sdl_rect.x        = 0;
                sdl_rect.y        = 0;
                sdl_rect.w        = sdl_TextureWidth;
                sdl_rect.h        = sdl_TextureHeight;

                if (sdl_TextureWidth > 0 && sdl_TextureHeight > 0)
                {
                    sdl_rect_Min.x    = sdl_rect.x;
                    sdl_rect_Min.y    = sdl_rect.y        + Video_Infrare_Top_Dist;
                    sdl_rect_Min.w    = sdl_TextureWidth  / Video_Infrare_Scale;
                    sdl_rect_Min.h    = sdl_TextureHeight / Video_Infrare_Scale;
                }
            }

            m_WidgetVideo->Lock();

            SDL_RenderClear(sdl_renderer);
            if (showVideoLight)
            {
                if (m_VideoRecvFrame != nullptr && sdl_texture != nullptr)
                {
                    if (m_VideoRecvFrame_Format == SDL_PIXELFORMAT_IYUV)
                    {
                        SDL_UpdateYUVTexture(sdl_texture, nullptr,
                                             m_VideoRecvFrame->data[0], m_VideoRecvFrame->linesize[0],
                                             m_VideoRecvFrame->data[1], m_VideoRecvFrame->linesize[1],
                                             m_VideoRecvFrame->data[2], m_VideoRecvFrame->linesize[2]);
                    }
                    SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, nullptr);
                }

                if (m_VideoRecvFrameInfrared != nullptr && sdl_textureInfrared != nullptr)
                {
                    if (m_VideoRecvFrameInfrared_Format == SDL_PIXELFORMAT_IYUV)
                    {
                        SDL_UpdateYUVTexture(sdl_textureInfrared, nullptr,
                                             m_VideoRecvFrameInfrared->data[0], m_VideoRecvFrameInfrared->linesize[0],
                                             m_VideoRecvFrameInfrared->data[1], m_VideoRecvFrameInfrared->linesize[1],
                                             m_VideoRecvFrameInfrared->data[2], m_VideoRecvFrameInfrared->linesize[2]);
                    }
                    SDL_RenderCopy(sdl_renderer, sdl_textureInfrared, nullptr, &sdl_rect_Min);
                }
            }
            else
            {
                if (m_VideoRecvFrameInfrared != nullptr && sdl_textureInfrared != nullptr)
                {
                    if (m_VideoRecvFrameInfrared_Format == SDL_PIXELFORMAT_IYUV)
                    {
                        SDL_UpdateYUVTexture(sdl_textureInfrared, nullptr,
                                             m_VideoRecvFrameInfrared->data[0], m_VideoRecvFrameInfrared->linesize[0],
                                             m_VideoRecvFrameInfrared->data[1], m_VideoRecvFrameInfrared->linesize[1],
                                             m_VideoRecvFrameInfrared->data[2], m_VideoRecvFrameInfrared->linesize[2]);
                    }
                    SDL_RenderCopy(sdl_renderer, sdl_textureInfrared, nullptr, nullptr);
                }

                if (m_VideoRecvFrame != nullptr && sdl_texture != nullptr)
                {
                    if (m_VideoRecvFrame_Format == SDL_PIXELFORMAT_IYUV)
                    {
                        SDL_UpdateYUVTexture(sdl_texture, nullptr,
                                             m_VideoRecvFrame->data[0], m_VideoRecvFrame->linesize[0],
                                             m_VideoRecvFrame->data[1], m_VideoRecvFrame->linesize[1],
                                             m_VideoRecvFrame->data[2], m_VideoRecvFrame->linesize[2]);
                    }
                    SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, &sdl_rect_Min);
                }
            }

            SDL_RenderPresent(sdl_renderer);
            m_WidgetVideo->UnLock();
        }
        m_WidgetVideo->m_sdl_rendering = false;

        if (m_WidgetVideo->m_sdl_render_times < 100)
        {
            m_WidgetVideo->m_sdl_render_times++;
            if (m_WidgetVideo->m_sdl_render_times == 99)
            {
                if (m_VideoRecvFrameInfrared == nullptr && sdl_textureInfrared == nullptr)
                {
                    m_WidgetVideo->SetWidgetUpdatesEnabled(true);
                }
            }
            else if (m_WidgetVideo->m_sdl_render_times < 10)
            {
                m_WidgetVideo->SetWidgetUpdatesEnabled(false);
            }
        }

        QLinkedList<RecvVideoStreamThread::VideoRecvFrame>::iterator messageReferenceFrameFreeIter;
        for (messageReferenceFrameFreeIter = listMessage.begin(); messageReferenceFrameFreeIter != listMessage.end(); ++messageReferenceFrameFreeIter)
        {
            av_frame_free(&(messageReferenceFrameFreeIter->frame));
        }
        listMessage.clear();

        QLinkedList<RecvVideoStreamThread::VideoRecvFrame>::iterator messageReferenceFrameFreeIterInfrared;
        for (messageReferenceFrameFreeIterInfrared = listMessageInfrared.begin(); messageReferenceFrameFreeIterInfrared != listMessageInfrared.end(); ++messageReferenceFrameFreeIterInfrared)
        {
            av_frame_free(&(messageReferenceFrameFreeIterInfrared->frame));
        }
        listMessageInfrared.clear();
    }

}
