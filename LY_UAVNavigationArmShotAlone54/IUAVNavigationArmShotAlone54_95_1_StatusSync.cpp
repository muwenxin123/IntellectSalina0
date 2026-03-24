#include "IUAVNavigationArmShotAlone54_95_1_StatusSync.h"

#include "LY_UAVNavigationArmShotAlone54.h"

#include <QSettings>
#include <QNetworkInterface>
#include <QNetworkDatagram>



bool IUAVNavigationArmShotAlone54_95_1_StatusSync::m_UseUdpSocket_95_1_StatusSync = (false);

IUAVNavigationArmShotAlone54_95_1_StatusSync::IUAVNavigationArmShotAlone54_95_1_StatusSync(QObject *parent)
    : QObject(parent)
    , m_UdpHostInterfacename_95_1_StatusSync("")
    , m_UdpHostAddressLocal_95_1_StatusSync("")
    , m_UdpHostAddressRemote_95_1_StatusSync("")
    , m_UdpHostAddressLocalPort_95_1_StatusSync(0)
    , m_UdpHostAddressRemotePort_95_1_StatusSync(0)
    , m_udpSocket_95_1_StatusSync(nullptr)
{
    {
        QSettings settings("Data/Conf/UAVNavigationArmShotConfig.ini", QSettings::IniFormat);
        settings.setIniCodec(QTextCodec::codecForName("utf-8"));
        settings.beginGroup("Multicast_95_1_StatusSync");
        if (settings.contains("UseUdpSocket_95_1_StatusSync") && settings.value("UseUdpSocket_95_1_StatusSync").canConvert(QVariant::Type::Bool))
        {
            m_UseUdpSocket_95_1_StatusSync = settings.value("UseUdpSocket_95_1_StatusSync", "false").toBool();
        }

        if (settings.contains("NetworkInterface") && settings.value("NetworkInterface").canConvert(QVariant::Type::String))
        {
            const QString strNetwork = settings.value("NetworkInterface", "\xe4\xbb\xa5\xe5\xa4\xaa\xe7\xbd\x91").toString();
            if (!strNetwork.isEmpty())
            {
                m_UdpHostInterfacename_95_1_StatusSync = strNetwork;
            }
        }

        if (settings.contains("HostAddressLocal") && settings.value("HostAddressLocal").canConvert(QVariant::Type::String))
        {
            QString strHost = settings.value("HostAddressLocal", "225.1.1.5").toString();
            if (!QHostAddress(strHost).isNull())
            {
                m_UdpHostAddressLocal_95_1_StatusSync = QHostAddress(strHost);
            }
        }


        if (settings.contains("HostAddressLocalPort") && settings.value("HostAddressLocalPort").canConvert(QVariant::Type::UInt))
        {
            bool ok = false;
            unsigned int port = settings.value("HostAddressLocalPort", "19990").toUInt(&ok);
            if (ok)
            {
                m_UdpHostAddressLocalPort_95_1_StatusSync = port;
            }
        }

        if (settings.contains("HostAddressRemote") && settings.value("HostAddressRemote").canConvert(QVariant::Type::String))
        {
            QString strHost = settings.value("HostAddressRemote", "225.1.1.5").toString();
            if (!QHostAddress(strHost).isNull())
            {
                m_UdpHostAddressRemote_95_1_StatusSync = QHostAddress(strHost);
            }
        }


        if (settings.contains("HostAddressRemotePort") && settings.value("HostAddressRemotePort").canConvert(QVariant::Type::UInt))
        {
            bool ok = false;
            unsigned int port = settings.value("HostAddressRemotePort", "19990").toUInt(&ok);
            if (ok)
            {
                m_UdpHostAddressRemotePort_95_1_StatusSync = port;
            }
        }

        settings.endGroup();
    }

//    qDebug() << m_UseUdpSocket_95_1_StatusSync
//             << m_UdpHostInterfacename_95_1_StatusSync
//             << m_UdpHostAddressLocal_95_1_StatusSync
//             << m_UdpHostAddressRemote_95_1_StatusSync
//             << m_UdpHostAddressLocalPort_95_1_StatusSync
//             << m_UdpHostAddressRemotePort_95_1_StatusSync ;

    if( m_UseUdpSocket_95_1_StatusSync && ( !m_UdpHostAddressLocal_95_1_StatusSync.isNull() ) && ( !m_UdpHostAddressRemote_95_1_StatusSync.isNull() ) ){
        m_udpSocket_95_1_StatusSync = new QUdpSocket(this);

        m_udpSocket_95_1_StatusSync->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);

        bool result        = false;
        bool joinMulticast = false;

        if (m_UdpHostAddressRemote_95_1_StatusSync.isMulticast())
        {
            result = m_udpSocket_95_1_StatusSync->bind(QHostAddress(QHostAddress::SpecialAddress::AnyIPv4), m_UdpHostAddressLocalPort_95_1_StatusSync, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        }
        else
        {
            result = m_udpSocket_95_1_StatusSync->bind(m_UdpHostAddressLocal_95_1_StatusSync, m_UdpHostAddressLocalPort_95_1_StatusSync, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        }


        QString strInterfacename("");
        for (const QNetworkInterface &networkInterfaceTemp : QNetworkInterface::allInterfaces())
        {
            if (networkInterfaceTemp.humanReadableName() == m_UdpHostInterfacename_95_1_StatusSync)
            {
                strInterfacename = networkInterfaceTemp.name();
                break;
            }
        }

        const auto &networkInterfaceSelect = QNetworkInterface::interfaceFromName(strInterfacename);

        if (networkInterfaceSelect.addressEntries().size() > 0)
        {
            m_udpSocket_95_1_StatusSync->setMulticastInterface(networkInterfaceSelect);

            if (m_UdpHostAddressRemote_95_1_StatusSync.isMulticast())
            {
                joinMulticast = m_udpSocket_95_1_StatusSync->joinMulticastGroup(m_UdpHostAddressRemote_95_1_StatusSync, networkInterfaceSelect);
            }
        }

        connect(m_udpSocket_95_1_StatusSync, &QUdpSocket::readyRead, this, [this]() {
            while (m_udpSocket_95_1_StatusSync->hasPendingDatagrams()) {
                const QNetworkDatagram& datagram = m_udpSocket_95_1_StatusSync->receiveDatagram();

                const QByteArray &data = datagram.data();

                if(data.size() != sizeof(qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_CommandStatusSync)){
                    return ;
                }

                const qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_CommandStatusSync* const xygsDY95_1_SBUS_CommandStatusSync = ( const qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_CommandStatusSync* const )(data.constData());
                const qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command*           const xygsDY95_1_SBUS_Command           = &(xygsDY95_1_SBUS_CommandStatusSync->command);

                if(xygsDY95_1_SBUS_CommandStatusSync != nullptr && (xygsDY95_1_SBUS_Command->head == XygsDY95_1_SBUS_KZMSG_HEAD)){

                    const auto verify     = qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command_Verify((((unsigned char *)(xygsDY95_1_SBUS_Command)) + 1), sizeof (qnzkna::ProtocolXygsDY::XygsDY95_1::FireControl_XygsDY95_1_SBUS_Command) - 2 );
                    if(xygsDY95_1_SBUS_Command->verify != verify){
                        return ;
                    }

                    qDebug() << m_UseUdpSocket_95_1_StatusSync
                             << m_UdpHostInterfacename_95_1_StatusSync
                             << m_UdpHostAddressLocal_95_1_StatusSync
                             << m_UdpHostAddressRemote_95_1_StatusSync
                             << m_UdpHostAddressLocalPort_95_1_StatusSync
                             << m_UdpHostAddressRemotePort_95_1_StatusSync
                             << datagram.senderAddress()
                             ;

                    const unsigned long long int componentID = xygsDY95_1_SBUS_CommandStatusSync->componentID;
                    if (componentID <= 0)
                    {
                        return;
                    }

                    if (LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog() != nullptr)
                    {
                        std::list<IUAVNavigationArmShotAlone54Dialog *>::const_iterator iter = LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog()->begin();
                        for (; iter != LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog()->end(); ++iter)
                        {
                            if (*iter != nullptr &&  (QString((*iter)->getCurrentComponentID()).toULongLong() == componentID))
                            {
                                (*iter)->Update_XygsDY95_1_SBUS_CommandStatusSyn(xygsDY95_1_SBUS_CommandStatusSync);
                                break;
                            }
                        }
                    }
                }
            }
        });
    }
}

int IUAVNavigationArmShotAlone54_95_1_StatusSync::sendMessage_95_1_StatusSync_control(const char * const pbuf, const int pbufSize)
{
    if( m_UseUdpSocket_95_1_StatusSync && ( !m_UdpHostAddressLocal_95_1_StatusSync.isNull() ) && ( !m_UdpHostAddressRemote_95_1_StatusSync.isNull() ) ) {
        // UDP发送
        if (nullptr == m_udpSocket_95_1_StatusSync || pbufSize <= 0 || pbuf == nullptr) {
            return 0;
        }
        qint64 byteSent = m_udpSocket_95_1_StatusSync->writeDatagram(pbuf, pbufSize, (m_UdpHostAddressRemote_95_1_StatusSync), m_UdpHostAddressRemotePort_95_1_StatusSync);
        return byteSent;
    }
    return 0;
}

bool IUAVNavigationArmShotAlone54_95_1_StatusSync::UseUdpSocket_95_1_StatusSync()
{
    QSettings settings("Data/Conf/UAVNavigationArmShotConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup("Multicast_95_1_StatusSync");
    if (settings.contains("UseUdpSocket_95_1_StatusSync") && settings.value("UseUdpSocket_95_1_StatusSync").canConvert(QVariant::Type::Bool))
    {
        m_UseUdpSocket_95_1_StatusSync = settings.value("UseUdpSocket_95_1_StatusSync", "false").toBool();
    }
    settings.endGroup();

    return m_UseUdpSocket_95_1_StatusSync;
}
