#ifndef PLAYVIDEOMANAGEWIDGET54_H
#define PLAYVIDEOMANAGEWIDGET54_H

#include <QWidget>
#include <QMutex>
#include <QThread>
#include <QUdpSocket>
#include <QNetworkInterface>
#include <QLinkedList>
#include <QMutex>
#include <QTimer>
#include <QMouseEvent>

#include <SDL2/SDL_rect.h>

#include "IPlayVideoManage.h"

namespace Ui
{
class PlayVideoManageWidget54;
}

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Rect SDL_Rect;

struct AVFrame;
struct AVCodecContext;
struct AVCodecParserContext;
struct AVPacket;
struct SwsContext;

namespace qnzkna
{
namespace PlayVideoManage
{

class ShowVideoStreamThread;
class RecvVideoStreamThread;

class CVideoDeviceParam
{
public:
    CVideoDeviceParam()
        : m_VideoDeviceType54(false)
        , m_VideoDeviceInfraredEnable(false)
        , m_VideoWindowMove(true)
        , m_VideoDeviceMulticastInterfaceName("")
        , m_VideoDeviceCodecName("")
        , m_VideoDeviceCodecExtData()
        , m_VideoDeviceCodecParam(false)
        , m_VideoDeviceCodecExtDataInfrared()
        , m_VideoDeviceCodecParamInfrared(false)
    {
    }

    CVideoDeviceParam(const bool videoDeviceType54
                      , const bool videoDeviceInfraredEnable
                      , const bool VideoWindowMove
                      , const QString &videoDeviceMulticastInterfaceName
                      , const QString &videoDeviceCodecName
                      , const QByteArray &videoDeviceCodecExtData
                      , const bool videoDeviceCodecParam
                      , const QByteArray &videoDeviceCodecExtDataInfrared
                      , const bool videoDeviceCodecParamInfrared
                     )
        : m_VideoDeviceType54(videoDeviceType54)
        , m_VideoDeviceInfraredEnable(videoDeviceInfraredEnable)
        , m_VideoWindowMove(VideoWindowMove)
        , m_VideoDeviceMulticastInterfaceName(videoDeviceMulticastInterfaceName)
        , m_VideoDeviceCodecName(videoDeviceCodecName)
        , m_VideoDeviceCodecExtData(videoDeviceCodecExtData)
        , m_VideoDeviceCodecParam(videoDeviceCodecParam)
        , m_VideoDeviceCodecExtDataInfrared(videoDeviceCodecExtDataInfrared)
        , m_VideoDeviceCodecParamInfrared(videoDeviceCodecParamInfrared)
    {
    }

    CVideoDeviceParam(const CVideoDeviceParam &videoDevice)
        : m_VideoDeviceType54(videoDevice.GetVideoDeviceType54())
        , m_VideoDeviceInfraredEnable(videoDevice.GetVideoDeviceInfraredEnable())
        , m_VideoWindowMove(videoDevice.GetVideoWindowMove())
        , m_VideoDeviceMulticastInterfaceName(videoDevice.GetVideoDeviceMulticastInterfaceName())
        , m_VideoDeviceCodecName(videoDevice.GetVideoDeviceCodecName())
        , m_VideoDeviceCodecExtData(videoDevice.GetVideoDeviceCodecExtData())
        , m_VideoDeviceCodecParam(videoDevice.GetVideoDeviceCodecParam())
        , m_VideoDeviceCodecExtDataInfrared(videoDevice.GetVideoDeviceCodecExtDataInfrared())
        , m_VideoDeviceCodecParamInfrared(videoDevice.GetVideoDeviceCodecParamInfrared())
    {
    }

    ~CVideoDeviceParam()
    {
        m_VideoDeviceType54                     = false;
        m_VideoDeviceInfraredEnable             = false;
        m_VideoWindowMove                       = true;
        m_VideoDeviceMulticastInterfaceName     = "";
        m_VideoDeviceCodecName                  = "";
        m_VideoDeviceCodecExtData               = QByteArray();
        m_VideoDeviceCodecParam                 = false;
        m_VideoDeviceCodecExtDataInfrared       = QByteArray();
        m_VideoDeviceCodecParamInfrared         = false;
    }

    void SetVideoDeviceType54(const bool videoDeviceType54)
    {
        m_VideoDeviceType54 = videoDeviceType54;
    }
    bool GetVideoDeviceType54() const
    {
        return m_VideoDeviceType54;
    }

    void SetVideoDeviceInfraredEnable(const bool videoDeviceInfraredEnable)
    {
        m_VideoDeviceInfraredEnable = videoDeviceInfraredEnable;
    }
    bool GetVideoDeviceInfraredEnable() const
    {
        return m_VideoDeviceInfraredEnable;
    }

    void SetVideoWindowMove(const bool videoWindowMove)
    {
        m_VideoWindowMove = videoWindowMove;
    }
    bool GetVideoWindowMove() const
    {
        return m_VideoWindowMove;
    }

    void setVideoDeviceMulticastInterfaceName(const QString &videoDeviceMulticastInterfaceName)
    {
        m_VideoDeviceMulticastInterfaceName = videoDeviceMulticastInterfaceName;
    }
    const QString &GetVideoDeviceMulticastInterfaceName() const
    {
        return m_VideoDeviceMulticastInterfaceName;
    }

    void SetVideoDeviceCodecName(const QString &videoDeviceCodecName)
    {
        m_VideoDeviceCodecName = videoDeviceCodecName;
    }
    const QString &GetVideoDeviceCodecName() const
    {
        return m_VideoDeviceCodecName;
    }

    void SetVideoDeviceCodecExtData(const QByteArray &videoDeviceCodecExtData)
    {
        m_VideoDeviceCodecExtData = videoDeviceCodecExtData;
    }
    const QByteArray &GetVideoDeviceCodecExtData() const
    {
        return m_VideoDeviceCodecExtData;
    }

    void SetVideoDeviceCodecExtDataInfrared(const QByteArray &videoDeviceCodecExtDataInfrared)
    {
        m_VideoDeviceCodecExtDataInfrared = videoDeviceCodecExtDataInfrared;
    }
    const QByteArray &GetVideoDeviceCodecExtDataInfrared() const
    {
        return m_VideoDeviceCodecExtDataInfrared;
    }

    void SetVideoDeviceCodecParam(const bool videoDeviceCodecParam)
    {
        m_VideoDeviceCodecParam = videoDeviceCodecParam;
    }
    bool GetVideoDeviceCodecParam() const
    {
        return m_VideoDeviceCodecParam;
    }

    void SetVideoDeviceCodecParamInfrared(const bool videoDeviceCodecParamInfrared)
    {
        m_VideoDeviceCodecParamInfrared = videoDeviceCodecParamInfrared;
    }
    bool GetVideoDeviceCodecParamInfrared() const
    {
        return m_VideoDeviceCodecParamInfrared;
    }

private:
    bool        m_VideoDeviceType54;
    bool        m_VideoDeviceInfraredEnable;
    bool        m_VideoWindowMove;

    QString     m_VideoDeviceMulticastInterfaceName;
    QString     m_VideoDeviceCodecName;
    QByteArray  m_VideoDeviceCodecExtData;
    bool        m_VideoDeviceCodecParam;
    QByteArray  m_VideoDeviceCodecExtDataInfrared;
    bool        m_VideoDeviceCodecParamInfrared;
};

class PlayVideoManageWidget54: public IPlayVideoManage
{
    Q_OBJECT

public:
    explicit PlayVideoManageWidget54(QWidget *parent = nullptr);
    virtual ~PlayVideoManageWidget54() override;

    virtual QPoint VideoWidgetPos() const override;
    virtual QSize VideoWidgetSize() const override;
    virtual std::string getStrFileName() const override;
    virtual void setStrFileName(const std::string &strFileName) override;

    virtual volatile bool getStop() const override;
    virtual void setStop(volatile bool Stop) override;

    virtual volatile bool getPause() const override;
    virtual void setPause(volatile bool Pause) override;

    virtual void Lock() override;
    virtual void UnLock() override;

    WId WinId() const;
    virtual void Show() override;

    virtual void setVideoType(int videoType) override;

    virtual bool StartVideoManageObjectHost(const CVideoDevice &videoDevice) override;
    virtual bool StartVideoManageThread(const CVideoDevice &videoDevice) override;
    virtual bool StopVideoManageThread() override;
    virtual void StartVideoPlay() override;
    virtual void StopVideoPlay()  override;

    void SetWidgetUpdatesEnabled(bool update);
	void appendAvframe(AVFrame* avframe);

	bool canCollection();

protected:
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
    virtual void changeEvent(QEvent *e) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Ui::PlayVideoManageWidget54 *ui;
	bool					   m_collection = true;
	QMutex                     m_Mutex;
    WId                        m_WId;
    bool                       m_ShowVideoLight;
    bool                       m_WidgetUpdatesEnabled;
	QThread                   *m_pShowVideoThread;
    ShowVideoStreamThread     *m_pShowVideoThreadObject;
    QThread                   *m_pRecvVideoThread;
    RecvVideoStreamThread     *m_pRecvVideoThreadObject;
    QThread                   *m_pRecvVideoThreadInfrared;
    RecvVideoStreamThread     *m_pRecvVideoThreadObjectInfrared;

    CVideoDevice               m_VideoDevice;
    CVideoDeviceParam          m_VideoDeviceParam;
    static bool                m_NET_DVR_Init;

public:
    volatile bool              m_sdl_rendering;
    unsigned int               m_sdl_render_times;
    SDL_Window                *sdl_screen;
    volatile bool              m_Stop;
    volatile bool              m_Pause;
    std::string                m_strFileName;
    std::string                m_strFileNameInfrared;

    friend class ShowVideoStreamThread;
};

class NetworkWorkerThread : public QThread
{
    Q_OBJECT

public:
    typedef struct __multicast_configure__
    {
        QString      interfacename;
        QHostAddress addressrecv;
        quint16      portrecv;
        QHostAddress addresssend;
        quint16      portsend;
    } multicast_configure_t;

    explicit NetworkWorkerThread(const multicast_configure_t &multicast_configure, const bool videoType_54, QObject *parent = nullptr);
    virtual ~NetworkWorkerThread() override;

    bool init() const;

    void GetMatVideoRecvBufferList(QLinkedList<const QByteArray> &videoRecvBufferList);

protected:
    virtual void run() override;

signals:

private:
    bool                            _init;
    bool                            _videoType_54;
    const multicast_configure_t     _multicast_configure_t;
    QUdpSocket                     *_udpSocket;
    QMutex                          _VideoRecvBufferMutex;
    QLinkedList<const QByteArray>   _VideoRecvBufferList;
};

class RecvVideoStreamThread : public QObject
{
    Q_OBJECT

public: RecvVideoStreamThread(const QString &strUrlName, const CVideoDeviceParam &videoDeviceParam, bool urlNameDeviceType54Infrared = false, QObject *parent = nullptr);
    virtual ~RecvVideoStreamThread() override;

    struct VideoRecvFrame
    {
        AVFrame      *frame;
        const int     width;
        const int     height;
        const Uint32  format;
        VideoRecvFrame(AVFrame *frame, const int width, const int height, const Uint32 format)
            : frame(frame)
            , width(width)
            , height(height)
            , format(format)
        {}
    };

    void GetMatVideoRecvStreamList(QLinkedList<VideoRecvFrame> &videoRecvStreamList);

signals:
    void video_doWork_Sig();
    void video_doStop_Sig();

public slots:
    void video_doWork();
    void video_start();
    void video_stop();
    void video_exit();

    void RecvStream();

private:
    void Decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt);

private:
    QTimer                                               m_TimerOpencvRecvWindow;
    QString                                              m_UrlName;
    bool                                                 m_UrlNameDeviceType54;
    bool                                                 m_UrlNameDeviceType54Infrared ;
    NetworkWorkerThread                                 *m_MulticastNetworkWorkerThread;
    struct AVFrame                                      *m_AVFrame;
    struct AVCodecContext                               *m_AVCodecContext;
    struct AVCodecParserContext                         *m_AVCodecParserContext;
    struct AVFrame                                      *m_AVFrameDest_Sws;
    struct SwsContext                                   *m_SwsContext;
    int                                                  m_Width_Sws;
    int                                                  m_Height_Sws;
    int                                                  m_Format_Sws;
    int                                                  m_Buffer_Size_Sws;
    uint8_t                                             *m_Buffer_Sws;
    QMutex                                               m_VideoRecvStreamMutex;
    QLinkedList<VideoRecvFrame>                          m_VideoRecvStreamList;
    const CVideoDeviceParam                             &m_VideoDeviceParam;
};

class ShowVideoStreamThread : public QObject
{
    Q_OBJECT

public: ShowVideoStreamThread(PlayVideoManageWidget54 *widget = nullptr, QObject *parent = nullptr);
    virtual ~ShowVideoStreamThread() override;

signals:
    void video_doWork_Sig();
    void video_doStop_Sig();

public slots:
    void video_doWork();
    void video_start();
    void video_stop();
    void video_exit();

    void ShowVideoRecvStream();

private:
    QTimer                           m_TimerOpencvShowWindow;
    PlayVideoManageWidget54         *m_WidgetVideo;
    bool                             m_UrlNameDeviceType54;
    QMutex                           m_VideoRecvStreamMutex;

    AVFrame                         *m_VideoRecvFrame                  = nullptr;
    int                              m_VideoRecvFrame_Width            = 0;
    int                              m_VideoRecvFrame_Height           = 0;
    Uint32                           m_VideoRecvFrame_Format           = 0;

    AVFrame                         *m_VideoRecvFrameInfrared          = nullptr;
    int                              m_VideoRecvFrameInfrared_Width    = 0;
    int                              m_VideoRecvFrameInfrared_Height   = 0;
    Uint32                           m_VideoRecvFrameInfrared_Format   = 0;

    SDL_Window                      *sdl_screen                        = nullptr;
    SDL_Renderer                    *sdl_renderer                      = nullptr;
    int                              sdl_TextureWidth                  = 0;
    int                              sdl_TextureHeight                 = 0;
    int                              sdl_TextureWidthInfrared          = 0;
    int                              sdl_TextureHeightInfrared         = 0;
    SDL_Texture                     *sdl_texture                       = nullptr;
    SDL_Texture                     *sdl_textureInfrared               = nullptr;
    SDL_Rect                         sdl_rect                          = {0, 0, 0, 0};
    SDL_Rect                         sdl_rect_Min                      = {0, 0, 0, 0};
    Uint32                           sdl_format                        = SDL_PIXELFORMAT_IYUV;
	int							  m_captureImageCnt = 0;
};

}
}
#endif
