#ifndef UAVPLANEATTITUDE_H
#define UAVPLANEATTITUDE_H

#include <QWidget>
#include <QPainter>
#include <QPoint>
#include <QBrush>
#include <QPen>

#include "Protocol/XygsJL/Bag_XygsJLDefines.h"

namespace Ui
{
class UAVPlaneAttitude;
}

class UAVPlaneAttitude : public QWidget
{
    Q_OBJECT

public:
    explicit UAVPlaneAttitude(QWidget *parent = nullptr);
    ~UAVPlaneAttitude();

    void updateTelemetryInformation(const QString &strWeaponComponentName, const QString &strWeaponComponentID, const telemetryInformation &telemetryInfo);
    void updateAttitude();

private:
    void setAttitude(float pitch, float roll, float yaw, double height, float speed);
    void pitch_angle(QPainter &painter);
    void rollMedian(QPainter &painter);

protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    Ui::UAVPlaneAttitude *ui;
    QString               m_strWeaponComponentName;
    QString               m_strWeaponComponentID;
    float                 m_Pitch;
    float                 m_Roll;
    float                 m_Yaw;
    double                m_Height;
    float                 m_Speed;
    bool                  m_show;
};

#endif
