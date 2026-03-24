#ifndef UAVPUSHBUTTON_H
#define UAVPUSHBUTTON_H

#include <QPainter>
#include <QPushButton>
#include <QString>
#include <QStyleOption>
#include <QVariant>


class UAVPushButton : public QPushButton
{
    Q_OBJECT

public:
    explicit UAVPushButton(const QString& weaponId, const QString& vehicleID, const QObject* vehicle,
                           const QString& strComponentName, const QString& strComponentID,
                           const QString& strProtocolType, QWidget *parent = nullptr);
    ~UAVPushButton();

    const QString& getWeaponId() const;

    const QString& getVehicleID() const;

    const QObject *getVehicle() const;

    const QString& getStrComponentID() const;

    const QString &getStrProtocolType() const;

    const QString &getStrComponentName() const;


private:
    void UAVPushButtonClicked();

private:
    const QString                m_weaponId;
    const QString                m_vehicleID;
    const QObject               *m_vehicle;
    const QString                m_strComponentName;
    const QString                m_strComponentID;
    const QString                m_strProtocolType;
};

#endif
