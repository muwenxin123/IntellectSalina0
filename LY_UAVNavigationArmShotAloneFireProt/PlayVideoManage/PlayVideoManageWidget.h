#ifndef PLAYVIDEOMANAGEWIDGET_H
#define PLAYVIDEOMANAGEWIDGET_H

#include <QWidget>
#include <QMutex>
#include <QThread>

#include "IPlayVideoManage.h"
#include "RealPlay.h"

namespace Ui {
class PlayVideoManageWidget;
}

typedef struct SDL_Window SDL_Window;

namespace qnzkna { namespace PlayVideoManage
{

class PlayVideoManageWidget: public IPlayVideoManage
{
    Q_OBJECT

public:
    explicit PlayVideoManageWidget(QWidget *parent = nullptr);
    virtual ~PlayVideoManageWidget();

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

    virtual bool StartVideoManageObjectHost(const CVideoDevice& videoDevice) override;
    virtual bool StartVideoManageThread(const CVideoDevice& videoDevice) override;
    virtual bool StopVideoManageThread() override;
    virtual void StartVideoPlay() override;
    virtual void StopVideoPlay()  override;

protected:

    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;

private:
    Ui::PlayVideoManageWidget *ui;
    QMutex                     m_Mutex;
    WId                        m_WId;
    QThread*                   m_pRecvVideoThread;
    CRealPlay                  m_RealPlay;
    CVideoDevice               m_VideoDevice;
    static bool                m_NET_DVR_Init;

public:
    SDL_Window*                sdl_screen;
    volatile bool              m_sdl_rendering;
    volatile bool              m_Stop;
    volatile bool              m_Pause;
    std::string                m_strFileName;
    bool                       m_hwdevice_video_use;
    std::string                m_hwdevice_type_name;
    std::string                m_hwdevice_pix_fmt_name;
};

class RecvVideoThread : public QThread
{
    Q_OBJECT
public:
    RecvVideoThread(PlayVideoManageWidget* pPlayVideoManageWidget):m_pPlayVideoManageWidget(pPlayVideoManageWidget) { }
    virtual ~RecvVideoThread(){}

protected:
    virtual void run() override;

private:
    PlayVideoManageWidget* m_pPlayVideoManageWidget;
};

}
}

#endif
