#include "LinkConfiguration.h"

#include "LinkInterface.h"
#include "LocalLink.h"

#ifndef NO_SERIAL_LINK
// lysd // #include "SerialLink.h"
#endif
// lysd // #include "UDPLink.h"
// lysd // // lysd // #include "TCPLink.h"
// lysd // #include "LogReplayLink.h"
#ifdef QGC_ENABLE_BLUETOOTH
#include "BluetoothLink.h"
#endif
#ifdef QT_DEBUG
// lysd // #include "MockLink.h"
#endif

#define LINK_SETTING_ROOT "LinkConfigurations"

LinkConfiguration::LinkConfiguration(const QString& name)
    : _name         (name)
    , _dynamic      (false)
    , _autoConnect  (false)
    , _highLatency  (false)
{

}

LinkConfiguration::LinkConfiguration(LinkConfiguration* copy)
{
    _link       = copy->_link;
    _name       = copy->name();
    _dynamic    = copy->isDynamic();
    _autoConnect= copy->isAutoConnect();
    _highLatency= copy->isHighLatency();
    Q_ASSERT(!_name.isEmpty());
}

void LinkConfiguration::copyFrom(LinkConfiguration* source)
{
    Q_ASSERT(source != nullptr);
    _link       = source->_link;
    _name       = source->name();
    _dynamic    = source->isDynamic();
    _autoConnect= source->isAutoConnect();
    _highLatency= source->isHighLatency();
}

/*!
  Where the settings are saved
  @return The root path of the setting.
*/
const QString LinkConfiguration::settingsRoot()
{
    return QString(LINK_SETTING_ROOT);
}

/*!
  Configuration Factory
  @return A new instance of the given type
*/
LinkConfiguration* LinkConfiguration::createSettings(int type, const QString& name)
{
    LinkConfiguration* config = nullptr;
    switch(type) {
// lysd // #ifndef NO_SERIAL_LINK
// lysd //         case LinkConfiguration::TypeSerial:
// lysd //             config = new SerialConfiguration(name);
// lysd //             break;
// lysd // #endif
// lysd //         case LinkConfiguration::TypeUdp:
// lysd //             config = new UDPConfiguration(name);
// lysd //             break;
// lysd //         case LinkConfiguration::TypeTcp:
// lysd //             config = new TCPConfiguration(name);
// lysd //             break;
// lysd // #ifdef QGC_ENABLE_BLUETOOTH
// lysd //     case LinkConfiguration::TypeBluetooth:
// lysd //         config = new BluetoothConfiguration(name);
// lysd //         break;
// lysd // #endif
// lysd //         case LinkConfiguration::TypeLogReplay:
// lysd //             config = new LogReplayLinkConfiguration(name);
// lysd //             break;
// lysd // #ifdef QT_DEBUG
// lysd //         case LinkConfiguration::TypeMock:
// lysd //             config = new MockConfiguration(name);
// lysd //             break;
// lysd // #endif
            case LinkConfiguration::TypeLocal:
                config = new LocalConfiguration(name);
                break;
    }
    return config;
}

/*!
  Duplicate link settings
  @return A new copy of the given settings instance
*/
LinkConfiguration* LinkConfiguration::duplicateSettings(LinkConfiguration* source)
{
    LinkConfiguration* dupe = nullptr;
    switch(source->type()) {
// lysd // #ifndef NO_SERIAL_LINK
// lysd //         case TypeSerial:
// lysd //             dupe = new SerialConfiguration(qobject_cast<SerialConfiguration*>(source));
// lysd //             break;
// lysd // #endif
// lysd //         case TypeUdp:
// lysd //             dupe = new UDPConfiguration(qobject_cast<UDPConfiguration*>(source));
// lysd //             break;
// lysd //         case TypeTcp:
// lysd //             dupe = new TCPConfiguration(qobject_cast<TCPConfiguration*>(source));
// lysd //             break;
// lysd // #ifdef QGC_ENABLE_BLUETOOTH
// lysd //         case TypeBluetooth:
// lysd //             dupe = new BluetoothConfiguration(qobject_cast<BluetoothConfiguration*>(source));
// lysd //             break;
// lysd // #endif
// lysd //         case TypeLogReplay:
// lysd //             dupe = new LogReplayLinkConfiguration(qobject_cast<LogReplayLinkConfiguration*>(source));
// lysd //             break;
// lysd // #ifdef QT_DEBUG
// lysd //         case TypeMock:
// lysd //             dupe = new MockConfiguration(qobject_cast<MockConfiguration*>(source));
// lysd //             break;
// lysd // #endif
// lysd //         case TypeLast:
// lysd //             break;
             case TypeLocal:
                 dupe = new LocalConfiguration(qobject_cast<LocalConfiguration*>(source));
                 break;
    }
    return dupe;
}

void LinkConfiguration::setName(const QString name)
{
    _name = name;
    emit nameChanged(name);
}

void LinkConfiguration::setLink(std::shared_ptr<LinkInterface> link)
{
    _link = link;
    emit linkChanged();
}
