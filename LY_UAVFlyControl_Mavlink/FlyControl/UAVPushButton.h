#ifndef UAVPUSHBUTTON_H
#define UAVPUSHBUTTON_H

#include <QPainter>
#include <QPushButton>
#include <QString>
#include <QStyleOption>
#include <QVariant>

#include "DataManage/XYWeaponManage/XYWeaponManage.h"

class UAVPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit UAVPushButton(const unsigned long long weaponId, const unsigned long long int vehicleID, const QObject* vehicle,
                           const QString& strComponentName, const QString& strComponentID,
                           const QString& strProtocolType, const XYWeaponIDSetting& xyWeaponIDSetting, QWidget *parent = nullptr);
    ~UAVPushButton();

    unsigned long long getWeaponId() const;
    unsigned long long getVehicleID() const;

    const QObject *getVehicle() const;

    const QString& getStrComponentID() const;

    const QString &getStrProtocolType() const;

    const QString &getStrComponentName() const;

    const XYWeaponIDSetting& getXYWeaponIDSetting() const;

private:
    void UAVPushButtonClicked();

private:
    const unsigned long long int m_weaponId;
    const unsigned long long int m_vehicleID;
    const QObject               *m_vehicle;
    const QString                m_strComponentName;
    const QString                m_strComponentID;
    const QString                m_strProtocolType;
    const XYWeaponIDSetting      m_XYWeaponIDSetting;
};

#endif
