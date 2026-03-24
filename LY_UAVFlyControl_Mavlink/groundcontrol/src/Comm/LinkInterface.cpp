#include "LinkInterface.h"

#include "QGCLoggingCategory.h"

QGC_LOGGING_CATEGORY(LinkInterfaceLog, "LinkInterfaceLog")

// The LinkManager is only forward declared in the header, so the static_assert is here instead.
// lysd // static_assert(LinkManager::invalidMavlinkChannel() == std::numeric_limits<uint8_t>::max(), "update LinkInterface::_mavlinkChannel");

LinkInterface::LinkInterface(SharedLinkConfigurationPtr& config, bool isPX4Flow)
    : QObject   (nullptr)
    , _config   (config)
    , _isPX4Flow(isPX4Flow)
{
    // lysd // QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    // lysd // qRegisterMetaType<LinkInterface*>("LinkInterface*");

    // This will cause the writeBytes calls to end up on the thread of the link
#ifdef QGC_APPLICATION_MAVLINK_VEHICLE_CONTROL_SEND_COMMAN_DMESSAGE
    QObject::connect(this, &LinkInterface::_invokeWriteBytes, this, &LinkInterface::_writeBytes);
#endif
}

LinkInterface::~LinkInterface()
{
    if (_vehicleReferenceCount != 0) {
        qCWarning(LinkInterfaceLog) << "~LinkInterface still have vehicle references:" << _vehicleReferenceCount;
    }
    _config.reset();
}

// lysd // uint8_t LinkInterface::mavlinkChannel(void) const
// lysd // {
// lysd //     if (!mavlinkChannelIsSet()) {
// lysd //         qCWarning(LinkInterfaceLog) << "mavlinkChannel isSet() == false";
// lysd //     }
// lysd //     return _mavlinkChannel;
// lysd // }

// lysd // bool LinkInterface::mavlinkChannelIsSet(void) const
// lysd // {
// lysd //     return (LinkManager::invalidMavlinkChannel() != _mavlinkChannel);
// lysd // }
// lysd //
// lysd // bool LinkInterface::_allocateMavlinkChannel()
// lysd // {
// lysd //     // should only be called by the LinkManager during setup
// lysd //     Q_ASSERT(!mavlinkChannelIsSet());
// lysd //     if (mavlinkChannelIsSet()) {
// lysd //         qCWarning(LinkInterfaceLog) << "_allocateMavlinkChannel already have " << _mavlinkChannel;
// lysd //         return true;
// lysd //     }
// lysd //
// lysd //     auto mgr = MavlinkApp()->toolbox()->linkManager();
// lysd //     _mavlinkChannel = mgr->allocateMavlinkChannel();
// lysd //     if (!mavlinkChannelIsSet()) {
// lysd //         qCWarning(LinkInterfaceLog) << "_allocateMavlinkChannel failed";
// lysd //         return false;
// lysd //     }
// lysd //     qCDebug(LinkInterfaceLog) << "_allocateMavlinkChannel" << _mavlinkChannel;
// lysd //     return true;
// lysd // }
// lysd //
// lysd // void LinkInterface::_freeMavlinkChannel()
// lysd // {
// lysd //     qCDebug(LinkInterfaceLog) << "_freeMavlinkChannel" << _mavlinkChannel;
// lysd //     if (LinkManager::invalidMavlinkChannel() == _mavlinkChannel) {
// lysd //         return;
// lysd //     }
// lysd //
// lysd //     auto mgr = MavlinkApp()->toolbox()->linkManager();
// lysd //     mgr->freeMavlinkChannel(_mavlinkChannel);
// lysd //     _mavlinkChannel = LinkManager::invalidMavlinkChannel();
// lysd // }

void LinkInterface::writeBytesThreadSafe(unsigned long long int weaponId, const char *bytes, int length)
{
    emit _invokeWriteBytes(weaponId, QByteArray(bytes, length));
}

void LinkInterface::addVehicleReference(void)
{
    _vehicleReferenceCount++;
}

void LinkInterface::removeVehicleReference(void)
{
    if (_vehicleReferenceCount != 0) {
        _vehicleReferenceCount--;
        if (_vehicleReferenceCount == 0) {
            disconnect();
        }
    } else {
        qCWarning(LinkInterfaceLog) << "removeVehicleReference called with no vehicle references";
    }
}

void LinkInterface::run()
{

}

void LinkInterface::_connectionRemoved(void)
{
    if (_vehicleReferenceCount == 0) {
        // Since there are no vehicles on the link we can disconnect it right now
        disconnect();
    } else {
        // If there are still vehicles on this link we allow communication lost to trigger and don't automatically disconect until all the vehicles go away
    }
}

#ifdef UNITTEST_BUILD
#include "MockLink.h"
bool LinkInterface::isMockLink(void)
{
    return dynamic_cast<MockLink*>(this);
}
#endif
