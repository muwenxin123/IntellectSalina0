#ifndef YOLOVIDEOSTREAMMANAGECOMMONINTERFACE_H
#define YOLOVIDEOSTREAMMANAGECOMMONINTERFACE_H

#include <QObject>
#include <QTimer>
#include "rep_YoloVideoStreamManageCommonInterface_source.h"

class YoloVideoStreamManageCommonInterface : public YoloVideoStreamManageCommonInterfaceSimpleSource
{
    Q_OBJECT

public:
    explicit YoloVideoStreamManageCommonInterface(QObject *parent = nullptr);
    virtual ~YoloVideoStreamManageCommonInterface() {}

signals:
    void video_widow_show_recv(const bool show);
    void video_selectrect_recv(const QByteArray videostreamtargetinfo);

public slots:
    virtual void video_widow_show(const bool show) override;
    virtual void video_selectrect(const QByteArray videostreamtargetinfo) override;
    virtual void server_slot(bool clientState) override;

private:
    QTimer       m_StateChangeTimer;
};

#endif
