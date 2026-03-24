#include "LocalLink.h"

#include <QtGlobal>
#include <QTimer>
#include <QList>
#include <QDebug>
#include <iostream>
#include <IUAVFlyControl_MavlinkActivator.h>

#include "QGC.h"
#include "QGCApplication.h"
#include "SettingsManager.h"

static const char* kZeroconfRegistration = "_qgroundcontrol._udp";

//--------------------------------------------------------------------------
//-- UDPConfiguration

LocalConfiguration::LocalConfiguration(const QString& name) : LinkConfiguration(name)
{

}

LocalConfiguration::LocalConfiguration(LocalConfiguration* source) : LinkConfiguration(source)
{
    copyFrom(source);
}

LocalConfiguration::~LocalConfiguration()
{

}

void LocalConfiguration::copyFrom(LinkConfiguration *source)
{
    LinkConfiguration::copyFrom(source);
}

void LocalConfiguration::saveSettings(QSettings& settings, const QString& root)
{
}

void LocalConfiguration::loadSettings(QSettings& settings, const QString& root)
{
}


LocalLink::LocalLink(SharedLinkConfigurationPtr& config)
    : LinkInterface     (config)
    , _running          (false)
    , _udpConfig        (qobject_cast<LocalConfiguration*>(config.get()))
    , _connectState     (false)
{
    if (!_udpConfig) {
        qWarning() << "Internal error";
    }
}

LocalLink::~LocalLink()
{
    disconnect();
    // Tell the thread to exit
    _running = false;
    // Clear client list
}

bool LocalLink::isConnected() const
{
    return _running && _connectState;
}

void LocalLink::run()
{
    _running = true;
    _connect();
}

bool LocalLink::dealingMessage(const char * const recvMsg, const unsigned int msgLength, const unsigned long long weaponId)
{
    if(recvMsg == nullptr || msgLength <= 0 || weaponId <= 0){
        return false;
    }

    emit bytesReceived(this,  weaponId, QByteArray(recvMsg, msgLength));

    return true;
}

void LocalLink::_writeBytes(unsigned long long weaponId, const QByteArray data)
{
    _writeDataGram(weaponId, data);
}

bool LocalLink::_connect()
{
    _connectState = true;
    return _connectState;
}

void LocalLink::_writeDataGram(unsigned long long int weaponId, const QByteArray data)
{
    //qDebug() << "UDP Out" << target->address << target->port;
    // lysd // if(_socket->writeDatagram(data, target->address, target->port) < 0) {
    // lysd //     qWarning() << "Error writing to" << target->address << target->port;
    // lysd // }

    if (data.size() > 0){
        IUAVFlyControl_MavlinkActivator::Get().sendMessage(data.constData(), data.size(), weaponId);
    }
}

void LocalLink::readBytes()
{
    //-- Send whatever is left
    // lysd // if (databuffer.size()) {
    // lysd //     emit bytesReceived(this, databuffer);
    // lysd // }
}

void LocalLink::disconnect(void)
{
    _running = false;

    _connectState = false;
}
