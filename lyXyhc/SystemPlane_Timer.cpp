#include "SystemPlane_Timer.h"

#define TIMEOUT_TIME 1000 * 10

SystemPlane_Timer::SystemPlane_Timer(QObject *parent)
    : QObject(parent)
{
}

SystemPlane_Timer::SystemPlane_Timer(std::string planeName)
{
    systemName = planeName;
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]()
    {
        emit sendSystemName(systemName);
    });
    timer->start(TIMEOUT_TIME);
}

SystemPlane_Timer::~SystemPlane_Timer()
{
    delete timer;
    systemName.clear();
}

void SystemPlane_Timer::restartQtimer()
{
    timer->start(TIMEOUT_TIME);
}

std::string SystemPlane_Timer::getSystemName()
{
    return systemName;
}
