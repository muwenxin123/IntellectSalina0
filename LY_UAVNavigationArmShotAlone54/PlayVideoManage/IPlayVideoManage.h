#ifndef IPLAYVIDEOMANAGE_H
#define IPLAYVIDEOMANAGE_H

#include <QWidget>
#include <string>

class QImage;
namespace qnzkna
{
namespace PlayVideoManage
{

class CVideoDevice
{
public:
    CVideoDevice(const std::string &videoComponentID, const std::string &videoDeviceType, const std::string &videoDeviceURL, const std::string &videoDeviceURLInfrared, const std::string &videoDevicePort,
                 const std::string &videoDeviceChannel, const std::string &videoDeviceUserName, const std::string &videoDevicePassWord, const std::string &videoType_54)
    {
        m_VideoComponentID          = videoComponentID;
        m_VideoDeviceType           = videoDeviceType;
        m_VideoDeviceURL            = videoDeviceURL;
        m_VideoDeviceURLInfrared    = videoDeviceURLInfrared;
        m_VideoDevicePort           = videoDevicePort;
        m_VideoDeviceChannel        = videoDeviceChannel;
        m_VideoDeviceUserName       = videoDeviceUserName;
        m_VideoDevicePassWord       = videoDevicePassWord;
        m_VideoType_54              = videoType_54;
    }

    CVideoDevice(const CVideoDevice &videoDevice)
    {
        m_VideoComponentID           = videoDevice.GetVideoComponentID();
        m_VideoDeviceType            = videoDevice.GetVideoDeviceType();
        m_VideoDeviceURL             = videoDevice.GetVideoDeviceURL();
        m_VideoDeviceURLInfrared     = videoDevice.GetVideoDeviceURLInfrared();
        m_VideoDevicePort            = videoDevice.GetVideoDevicePort();
        m_VideoDeviceChannel         = videoDevice.GetVideoDeviceChannel();
        m_VideoDeviceUserName        = videoDevice.GetVideoDeviceUserName();
        m_VideoDevicePassWord        = videoDevice.GetVideoDevicePassWord();
        m_VideoType_54               = videoDevice.GetVideoType_54();
    }

    ~CVideoDevice()
    {
        m_VideoComponentID            = "";
        m_VideoDeviceType             = "";
        m_VideoDeviceURL              = "";
        m_VideoDeviceURLInfrared      = "";
        m_VideoDevicePort             = "8000";
        m_VideoDeviceChannel          = "0";
        m_VideoDeviceUserName         = "";
        m_VideoDevicePassWord         = "";
        m_VideoType_54                = "";
    }

    void SetVideoDeviceType(const std::string &videoDeviceType)
    {
        m_VideoDeviceType = videoDeviceType;
    }
    const std::string &GetVideoDeviceType() const
    {
        return m_VideoDeviceType;
    }

    void SetVideoDeviceURL(const std::string &videoDeviceURL)
    {
        m_VideoDeviceURL = videoDeviceURL;
    }
    const std::string &GetVideoDeviceURL() const
    {
        return m_VideoDeviceURL;
    }

    void SetVideoDeviceURLInfrared(const std::string &videoDeviceURLInfrared)
    {
        m_VideoDeviceURLInfrared = videoDeviceURLInfrared;
    }
    const std::string &GetVideoDeviceURLInfrared() const
    {
        return m_VideoDeviceURLInfrared;
    }

    void SetVideoDevicePort(const std::string &videoDevicePort)
    {
        m_VideoDevicePort = videoDevicePort;
    }
    const std::string &GetVideoDevicePort() const
    {
        return m_VideoDevicePort;
    }

    void SetVideoDeviceChannel(const std::string &videoDeviceChannel)
    {
        m_VideoDeviceChannel = videoDeviceChannel;
    }
    const std::string &GetVideoDeviceChannel() const
    {
        return m_VideoDeviceChannel;
    }

    void SetVideoDeviceUserName(const std::string &videoDeviceUserName)
    {
        m_VideoDeviceUserName = videoDeviceUserName;
    }
    const std::string &GetVideoDeviceUserName() const
    {
        return m_VideoDeviceUserName;
    }

    void SetVideoDevicePassWord(const std::string &videoDevicePassWord)
    {
        m_VideoDevicePassWord = videoDevicePassWord;
    }
    const std::string &GetVideoDevicePassWord() const
    {
        return m_VideoDevicePassWord;
    }

    void SetVideoComponentID(const std::string &VideoComponentID)
    {
        m_VideoComponentID = VideoComponentID;
    }

    const std::string &GetVideoComponentID() const
    {
        return m_VideoComponentID;
    }

    const std::string &GetVideoType_54() const
    {
        return m_VideoType_54;
    }

private:
    std::string m_VideoComponentID;
    std::string m_VideoDeviceType;
    std::string m_VideoDeviceURL;
    std::string m_VideoDeviceURLInfrared;
    std::string m_VideoDevicePort;
    std::string m_VideoDeviceChannel;
    std::string m_VideoDeviceUserName;
    std::string m_VideoDevicePassWord;
    std::string m_VideoType_54;
};

class IPlayVideoManage : public QWidget
{
    Q_OBJECT

public:
    explicit IPlayVideoManage(QWidget *parent = nullptr): QWidget(parent) {}
    virtual ~IPlayVideoManage() {}

    virtual QPoint VideoWidgetPos() const = 0;
    virtual QSize VideoWidgetSize() const = 0;
    virtual std::string getStrFileName() const = 0;
    virtual void setStrFileName(const std::string &strFileName) = 0;

    virtual volatile bool getStop() const = 0;
    virtual void setStop(volatile bool Stop) = 0;

    virtual volatile bool getPause() const = 0;
    virtual void setPause(volatile bool Pause) = 0;

    virtual void Lock() = 0;
    virtual void UnLock() = 0;

    virtual void Show() = 0;

    virtual void setVideoType(int videoType) { }

    virtual bool StartVideoManageObjectHost(const CVideoDevice &videoDevice) = 0;
    virtual bool StartVideoManageThread(const CVideoDevice &videoDevice) = 0;
    virtual bool StopVideoManageThread() = 0;
    virtual void StartVideoPlay() = 0;
    virtual void StopVideoPlay()  = 0;
};

}
}

#endif
