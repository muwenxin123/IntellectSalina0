#ifndef VIDEOHUDDIALOG_H
#define VIDEOHUDDIALOG_H

#include <QWidget>

#include "Protocol/XygsJL/Bag_XygsJLDefines.h"
#include "Protocol/DY_ZC_Control/DY_ZC_HUD_Control.h"

namespace Ui
{
class VideoHUDDialog;
}

class VideoHUDDialog : public QWidget
{
    Q_OBJECT

public:
    explicit VideoHUDDialog(const QString &currentComponentID, QWidget *parent = nullptr);
    ~VideoHUDDialog();

    void updateVideoHUDDialogTelemetryInformation(const telemetryInformation &telemetryInfo);
    void updateDY_ZC_HUD_ControlInfo(const qnzkna::DY_ZC_HUD_Control::DY_ZC_HUD_ControlInfo &dy_ZC_HUD_ControlInfo);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void changeEvent(QEvent *e) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void showEvent(QShowEvent *event) override;
    virtual void hideEvent(QHideEvent *event) override;

private:
    void chartDirectionPoor(QPainter &painter, const QPoint &center, const int moveX = 0);
    void charCrossHairLine(QPainter &painter, const QPoint &center);
    void chartHeightRule(QPainter &painter, const QRect &rect);
    void charLaunchAllowConditions(QPainter &painter, const QRect &rect);
    void setUAVAttitude(float topDirection, float roll, float pitch, double height, double speed);

    double CalLauchHeight();
    double CalLauchHeightNew();
    double CalLauchHeightNewByErFen(const double &dStartHeight, const double &dFinishHeight, const double &dDis);
    double CalLauchDis(const double &dHeight);

    int headDraw(double path, double depath, QPainter &painter);

    void targetDraw(int turn, int height, int cx, int cy, int stpx, int stpy, int stpz, int grounddt, double head, double dehead, QPainter &painter);

    void reticleDraw(int cenx, int ceny, int radius, double tarrange, double minrange, double roll, QPainter &painter);

    QString AMapPod54ControlStatus_ServoStatus_ToStr(const uint8_t servoStatus);

private:
    Ui::VideoHUDDialog										*ui;
    QString													m_currentComponentID;
    qnzkna::DY_ZC_HUD_Control::DY_ZC_HUD_ControlInfo		m_DY_ZC_HUD_ControlInfo;
    bool													m_DY_ZC_HUD_ControlInfo_Show;
    bool													m_DY_ZC_HUD_ControlInfo_Update;
    unsigned int											m_DY_ZC_HUD_ControlInfo_Times;
    float													m_topDirection;
    float													m_roll;
    float													m_pitch;
    double													m_height;
    double													m_speed;
    double													m_HPAHeight;
    int														m_fSpeed;
    double													m_fAngle;
    int														m_skySpeed;
    int														m_msec;
    double													m_dMaxDistance;
};

#endif
