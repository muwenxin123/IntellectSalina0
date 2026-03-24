#ifndef WEAPONMARKASSIGNAIMINGSTRIKE_H
#define WEAPONMARKASSIGNAIMINGSTRIKE_H

#include <QObject>

namespace qnzkna
{
namespace MessageObjectManage
{

class WeaponMarkAssignAimingStrikeObject;

}
}

class WeaponMarkAssignAimingStrike : public QObject
{
    Q_OBJECT
public:
    explicit WeaponMarkAssignAimingStrike(QObject *parent = nullptr);

    void DY54_StartAutoAiming(const QString &strWeaponId, const QString &byMarkId);
    void DY54_StopAutoAiming(const QString &strWeaponId);

    void SendWeaponMarkAssign(const unsigned int nWeaponId, const char *const byMarkId, const bool bAssign);
    void SendWeaponMarkAiming(const unsigned int nWeaponId, const char *const byMarkId, const bool bAiming);
    void SendWeaponMarkStrike(const unsigned int nWeaponId, const char *const byMarkId, const unsigned char byAmmunitionType, const unsigned char byAmmunitionState, const float fAzimuth, const float fDistance);

    void RecvWeaponMarkAssignAimingStrike(const int nLength, const char *const lpData);

signals:

public slots:

private:
    qnzkna::MessageObjectManage::WeaponMarkAssignAimingStrikeObject *m_DY54CommandOperObject;
};

#endif
