#include "ISystemMaintenanceImpl.h"

namespace qnzkna
{
namespace SystemMaintenance
{

ISystemMaintenanceImpl::ISystemMaintenanceImpl()
{
    m_bInitData = false;
    m_pActivator = nullptr;
}

ISystemMaintenanceImpl::~ISystemMaintenanceImpl()
{
}

bool ISystemMaintenanceImpl::InitWeaponSystems()
{
    m_mapWeaponSystem.clear();
    m_mapWeaponComponent.clear();
    if (m_pActivator)
    {
        qnzkna::SystemConfig::IConfigProjectControlService		*pProjectService = m_pActivator->getConfigProjectControlService();
        qnzkna::WeaponManage::IWeaponManageService				*pWeaponManageService = m_pActivator->getWeaponManageService();
        qnzkna::CCommandUnitManage::ICommandUnitManageService	*pCommandUnitService = m_pActivator->getCommandUnitManageService();
        if (pProjectService && pWeaponManageService && pCommandUnitService)
        {
            std::string strNodeID = pProjectService->systemSeatInfo().SystemNodeId();
            StringSet weaponSysList;
            if (pCommandUnitService->GetCommandUnitWeaponList(strNodeID, &weaponSysList))
            {
                for (const auto &itr : weaponSysList)
                {
                    qnzkna::WeaponManage::WeaponSystem weaponSys;
                    if (pWeaponManageService->GetWeaponSystem(&weaponSys, itr))
                    {
                        m_mapWeaponSystem.insert(std::make_pair(itr, weaponSys));
                        for (const auto &itrComponent : weaponSys.getMapWeaponComponent())
                        {
                            m_mapWeaponComponent.insert(std::make_pair(itrComponent.second.getComponentID(), itrComponent.second));
                        }
                    }
                }
            }
            return true;
        }
    }
    return false;
}

bool ISystemMaintenanceImpl::GetNodeInfo(NODE_TYPE_UINT *nType, std::string *nodeId)
{
    if (nullptr == nType || nullptr == nodeId)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (m_pActivator && m_pActivator->getConfigProjectControlService())
    {
        *nType = m_pActivator->getConfigProjectControlService()->systemSeatInfo().SystemNodeType();
        *nodeId = m_pActivator->getConfigProjectControlService()->systemSeatInfo().SystemNodeId();
        return true;
    }
    return false;
}

bool ISystemMaintenanceImpl::GetSeatInfo(qnzkna::SystemConfig::SystemSeat_Type *nType, unsigned int *seatId)
{
    if (nullptr == nType || nullptr == seatId)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (m_pActivator && m_pActivator->getConfigProjectControlService())
    {
        *nType = m_pActivator->getConfigProjectControlService()->systemSeatInfo().SeatType();
        *seatId = m_pActivator->getConfigProjectControlService()->systemSeatInfo().SeatId();
        return true;
    }
    return false;
}

bool ISystemMaintenanceImpl::GetWeaponSystems(qnzkna::WeaponManage::WeaponSystemMap *pMap)
{
    if (nullptr == pMap)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (false == m_bInitData)
    {
        m_bInitData = InitWeaponSystems();
    }
    if (m_bInitData)
    {
        *pMap = m_mapWeaponSystem;
        return true;
    }
    return false;
}

bool ISystemMaintenanceImpl::GetWeaponSystems(qnzkna::WeaponManage::WeaponSystem_Type eType, qnzkna::WeaponManage::WeaponSystemMap *pMap)
{
    if (nullptr == pMap)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (false == m_bInitData)
    {
        m_bInitData = InitWeaponSystems();
    }
    for (const auto &itr : m_mapWeaponSystem)
    {
        if (itr.second.getSystemType() == eType)
        {
            pMap->insert(std::make_pair(itr.first, itr.second));
        }
    }
    return true;
}

bool ISystemMaintenanceImpl::GetWeaponComponents(qnzkna::WeaponManage::MultiWeaponComponentMap *pMap)
{
    if (nullptr == pMap)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (false == m_bInitData)
    {
        m_bInitData = InitWeaponSystems();
    }
    if (m_bInitData)
    {
        *pMap = m_mapWeaponComponent;
        return true;
    }
    return true;
}

bool ISystemMaintenanceImpl::GetWeaponComponents(qnzkna::WeaponManage::WeaponComponent_Type eType, qnzkna::WeaponManage::MultiWeaponComponentMap *pMap)
{
    if (nullptr == pMap)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (false == m_bInitData)
    {
        m_bInitData = InitWeaponSystems();
    }
    if (m_bInitData)
    {
        for (const auto &itr : m_mapWeaponComponent)
        {
            if (itr.second.getPWeaponModel()->getComponentType() == eType)
            {
                pMap->insert(std::make_pair(itr.first, itr.second));
            }
        }
        return true;
    }
    return false;
}

bool ISystemMaintenanceImpl::GetWeaponComponents_UAV(qnzkna::WeaponManage::MultiWeaponComponentMap *pMap)
{
    if (nullptr == pMap)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (false == m_bInitData)
    {
        m_bInitData = InitWeaponSystems();
    }
    if (m_bInitData)
    {
        for (const auto &itr : m_mapWeaponComponent)
        {
            if (itr.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type_RotorUAV			||
                    itr.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type_HelicopterUAV	||
                    itr.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type_FixedWingUAV		||
                    itr.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type_SpinWingUAV		||
                    itr.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type_PrarafoilUAV		||
                    itr.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type_FlappingWingUAV  ||
                    itr.second.getPWeaponModel()->getComponentType() == qnzkna::WeaponManage::WeaponComponent_Type_UAS)
            {
                pMap->insert(std::make_pair(itr.first, itr.second));
            }
        }
        return true;
    }
    return false;

}

bool ISystemMaintenanceImpl::GetWeaponSystem(const std::string &strWeaponSys, qnzkna::WeaponManage::WeaponSystem *pWeaponSys)
{
    if (nullptr == pWeaponSys)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (false == m_bInitData)
    {
        m_bInitData = InitWeaponSystems();
    }
    if (m_bInitData)
    {
        auto itrMap = m_mapWeaponSystem.find(strWeaponSys);
        if (itrMap != m_mapWeaponSystem.end())
        {
            *pWeaponSys = itrMap->second;
            return true;
        }
    }
    return false;
}

bool ISystemMaintenanceImpl::GetWeaponComponent(const std::string &strWeaponComponent, qnzkna::WeaponManage::WeaponComponent *pWeaponComponent)
{
    if (nullptr == pWeaponComponent)
    {
        return false;
    }
    QMutexLocker locker(&m_mutex);
    if (false == m_bInitData)
    {
        m_bInitData = InitWeaponSystems();
    }
    auto itr = m_mapWeaponComponent.find(strWeaponComponent);
    if (itr != m_mapWeaponComponent.end())
    {
        *pWeaponComponent = itr->second;
        return true;
    }
    return false;
}

bool ISystemMaintenanceImpl::GetWeaponComponentPlatforms(std::set<std::string> *pPlatformList)
{
    if (nullptr == pPlatformList)
    {
        return false;
    }
    pPlatformList->clear();
    QMutexLocker locker(&m_mutex);
    if (false == m_bInitData)
    {
        m_bInitData = InitWeaponSystems();
    }
    for (auto &itr : m_mapWeaponComponent)
    {
        if (itr.second.IsPlatformComponent())
        {
            pPlatformList->insert(itr.second.getComponentID());
        }
    }
    return true;
}

}
}