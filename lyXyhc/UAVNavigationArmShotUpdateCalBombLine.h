#ifndef UAVNAVIGATIONARMSHOTUPDATECALBOMBLINE_H
#define UAVNAVIGATIONARMSHOTUPDATECALBOMBLINE_H

#include <QObject>
#include <QTimer>
#include <QThread>
#include <QDebug>

class UAVNavigationArmShotUpdateCalBombLine : public QObject
{
    Q_OBJECT
public:
    explicit UAVNavigationArmShotUpdateCalBombLine(QObject *parent = nullptr);

signals:

private slots:
    void UpdateMark_80mm();

private:
    double HeadAngle(const double angle) const;

private:
    QTimer       m_TimerUpdateMark_80mm;
};

#endif
