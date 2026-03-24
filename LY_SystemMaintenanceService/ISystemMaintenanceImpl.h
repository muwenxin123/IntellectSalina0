#pragma once
#include "DataManage/ISystemMaintenanceService.h"
#include "ISystemMaintenanceActivator.h"
#include "QMutex"

class ISystemMaintenanceActivator;
namespace qnzkna
{
namespace SystemMaintenance
{

class ISystemMaintenanceImpl : public qnzkna::SystemMaintenance::ISystemMaintenanceService
{
public:
    ISystemMaintenanceImpl();
    ~ISystemMaintenanceImpl();
public:
    virtual bool GetNodeInfo(NODE_TYPE_UINT *nType, std::string *nodeId);
    virtual bool GetSeatInfo(qnzkna::SystemConfig::SystemSeat_Type *nType, unsigned int *seatId);
    virtual bool GetWeaponSystems(qnzkna::WeaponManage::WeaponSystemMap *pMap);
    virtual bool GetWeaponSystems(qnzkna::WeaponManage::WeaponSystem_Type eType, qnzkna::WeaponManage::WeaponSystemMap *pMap);
    virtual bool GetWeaponComponents(qnzkna::WeaponManage::MultiWeaponComponentMap *pMap);
    virtual bool GetWeaponComponents(qnzkna::WeaponManage::WeaponComponent_Type eType, qnzkna::WeaponManage::MultiWeaponComponentMap *pMap);
    virtual bool GetWeaponComponents_UAV(qnzkna::WeaponManage::MultiWeaponComponentMap *pMap);
    virtual bool GetWeaponSystem(const std::string &strWeaponSys, qnzkna::WeaponManage::WeaponSystem *pWeaponSys);
    virtual bool GetWeaponComponent(const std::string &strWeaponComponent, qnzkna::WeaponManage::WeaponComponent *pWeaponComponent);

    virtual bool GetWeaponComponentPlatforms(std::set<std::string> *pPlatformList);
public:
    mutable QMutex                m_mutex;
    ISystemMaintenanceActivator  *m_pActivator;
private:
    bool											InitWeaponSystems();
    bool											m_bInitData;
    qnzkna::WeaponManage::WeaponSystemMap			m_mapWeaponSystem;
    qnzkna::WeaponManage::MultiWeaponComponentMap	m_mapWeaponComponent;
};

}
}
