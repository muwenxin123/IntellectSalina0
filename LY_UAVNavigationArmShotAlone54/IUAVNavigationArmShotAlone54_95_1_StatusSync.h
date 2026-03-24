#ifndef IUAVNAVIGATIONARMSHOTALONE54_95_1_STATUSSYNC_H
#define IUAVNAVIGATIONARMSHOTALONE54_95_1_STATUSSYNC_H

#include <QObject>
#include <QUdpSocket>
#include <QHostAddress>

class IUAVNavigationArmShotAlone54_95_1_StatusSync : public QObject
{
    Q_OBJECT
public:
    explicit IUAVNavigationArmShotAlone54_95_1_StatusSync(QObject *parent = nullptr);

    int sendMessage_95_1_StatusSync_control(const char * const pbuf, const int pbufSize);

    static bool UseUdpSocket_95_1_StatusSync();

signals:

public slots:


private:
    static bool                                                 m_UseUdpSocket_95_1_StatusSync;
    QString                                                     m_UdpHostInterfacename_95_1_StatusSync;
    QHostAddress                                                m_UdpHostAddressLocal_95_1_StatusSync;
    QHostAddress                                                m_UdpHostAddressRemote_95_1_StatusSync;
    int                                                         m_UdpHostAddressLocalPort_95_1_StatusSync;
    int                                                         m_UdpHostAddressRemotePort_95_1_StatusSync;
    QUdpSocket*                                                 m_udpSocket_95_1_StatusSync;
};

#endif // IUAVNAVIGATIONARMSHOTALONE54_95_1_STATUSSYNC_H
