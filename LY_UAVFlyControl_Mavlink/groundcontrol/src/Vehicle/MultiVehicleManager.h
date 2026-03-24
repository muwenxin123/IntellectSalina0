#ifndef MULTIVEHICLEMANAGER_H
#define MULTIVEHICLEMANAGER_H

#include <QObject>
#include <QGeoCoordinate>
#include <QTimer>

#include "QGCMAVLink.h"
#include "QGCLoggingCategory.h"

class FirmwarePluginManager;
class FollowMe;
class JoystickManager;
class QGCApplication;
class MAVLinkProtocol;
class Vehicle;
class LinkInterface;

Q_DECLARE_LOGGING_CATEGORY(MultiVehicleManagerLog)



class MultiVehicleManager : public QObject
{
    Q_OBJECT

public:
    MultiVehicleManager(QGCApplication* app);


    class WeaponVehicle_ID
    {
    public:
        WeaponVehicle_ID( unsigned long long int  weaponId, int vehicleId)
            : _weaponId(weaponId)
            , _vehicleId(vehicleId)
            , _weaponId_vehicleId(QString::number(weaponId) + "_" + QString::number(vehicleId) )
        {}

        // QMap's key type must provide operator<()
        bool operator < (const MultiVehicleManager::WeaponVehicle_ID &other) const
        {
            return _weaponId_vehicleId < other._weaponId_vehicleId;
//            return ( (this->_weaponId < other._weaponId)
//                     || (
//                         (this->_weaponId == other._weaponId) &&
//                         (this->_vehicleId < other._vehicleId)
//                         )
//                     );

//            if (this->_weaponId == other._weaponId && this->_vehicleId == other._vehicleId){
//                return false;
//            }

//            return true;

//            if (this->_weaponId != other._weaponId)
//                return this->_weaponId < other._weaponId;
//            return this->_vehicleId < other._vehicleId;
        }

//        friend bool operator < (const MultiVehicleManager::WeaponVehicle_ID &other1, const MultiVehicleManager::WeaponVehicle_ID &other2)
//        {
//            if (other1._weaponId != other2._weaponId)
//                return other1._weaponId < other2._weaponId;
//            return other1._vehicleId < other2._vehicleId;
//        }

    private:
        const QString                _weaponId_vehicleId;
        const unsigned long long int _weaponId;
        const int                    _vehicleId;
    };


    Q_INVOKABLE void        saveSetting (const QString &key, const QString& value);
    Q_INVOKABLE QString     loadSetting (const QString &key, const QString& defaultValue);

    Q_PROPERTY(bool                 gcsHeartBeatEnabled             READ gcsHeartbeatEnabled            WRITE setGcsHeartbeatEnabled    NOTIFY gcsHeartBeatEnabledChanged)
    Q_PROPERTY(QGeoCoordinate       lastKnownLocation               READ lastKnownLocation                                              NOTIFY lastKnownLocationChanged) //< Current vehicles last know location

    // Methods

    Q_INVOKABLE Vehicle* getVehicleById(int vehicleId);
    Q_INVOKABLE Vehicle* getVehicleByWeaponIdAndId(unsigned long long int  weaponId, int vehicleId);


    void addVehicles(LinkInterface* link, unsigned long long int  weaponId, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType);
    void removeVehicles(unsigned long long int  weaponId, int vehicleId);
    QList<Vehicle*>* vehicles(void) { return &_vehicles; }

    bool gcsHeartbeatEnabled(void) const { return _gcsHeartbeatEnabled; }
    void setGcsHeartbeatEnabled(bool gcsHeartBeatEnabled);


    // Override from QGCTool
    virtual void setToolbox();

    QGeoCoordinate lastKnownLocation    () { return _lastKnownLocation; }


signals:
    void vehicleAdded                   (Vehicle* vehicle);
    void vehicleRemoved                 (Vehicle* vehicle);
    void activeVehicleAvailableChanged  (bool activeVehicleAvailable);
    void parameterReadyVehicleAvailableChanged(bool parameterReadyVehicleAvailable);
    void activeVehicleChanged           (Vehicle* activeVehicle);
    void gcsHeartBeatEnabledChanged     (bool gcsHeartBeatEnabled);
    void lastKnownLocationChanged       ();
#ifndef DOXYGEN_SKIP
    void _deleteVehiclePhase2Signal     (void);
#endif

private slots:
    void _deleteVehiclePhase1           (Vehicle* vehicle);
    void _deleteVehiclePhase2           (void);
    // lysd // void _setActiveVehiclePhase2        (void);
    void _vehicleParametersReadyChanged (bool parametersReady);
    void _sendGCSHeartbeat              (void);
    void _vehicleHeartbeatInfo          (LinkInterface* link, unsigned long long int  weaponId, int vehicleId, int componentId, int vehicleFirmwareType, int vehicleType);
    void _requestProtocolVersion        (unsigned version);
    void _coordinateChanged             (QGeoCoordinate coordinate);

private:
    bool _vehicleExists(int vehicleId);
    QMap<WeaponVehicle_ID, Vehicle *>* weapon_vehicles() { return &_vehicleId_vehicles; }

    QList<Vehicle*> _vehiclesBeingDeleted;          ///< List of Vehicles being deleted in queued phases
    // lysd // Vehicle*        _vehicleBeingSetActive;         ///< Vehicle being set active in queued phases

    QList<int>  _ignoreVehicleIds;          ///< List of vehicle id for which we ignore further communication

    QList<Vehicle*>  _vehicles;
    QMap<WeaponVehicle_ID, Vehicle*> _vehicleId_vehicles;

    FirmwarePluginManager*      _firmwarePluginManager;
    MAVLinkProtocol*            _mavlinkProtocol;
    QGeoCoordinate              _lastKnownLocation;

    QTimer              _gcsHeartbeatTimer;             ///< Timer to emit heartbeats
    bool                _gcsHeartbeatEnabled;           ///< Enabled/disable heartbeat emission
    static const int    _gcsHeartbeatRateMSecs = 1000;  ///< Heartbeat rate
    static const char*  _gcsHeartbeatEnabledKey;

    friend class MAVLinkProtocol;
};


#endif // MULTIVEHICLEMANAGER_H
