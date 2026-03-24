#ifndef _SYSTEMPLANE_TIMER__
#define _SYSTEMPLANE_TIMER__

#include <QObject>
#include <QTimer>
#include <string>

class SystemPlane_Timer : public QObject
{
    Q_OBJECT

public:
    SystemPlane_Timer(QObject *parent);
    SystemPlane_Timer(std::string planeName);
    ~SystemPlane_Timer();

    void restartQtimer();
    std::string getSystemName();

signals:
    void sendSystemName(std::string);

private:
    QTimer  *timer;
    std::string  systemName;
};
#endif
