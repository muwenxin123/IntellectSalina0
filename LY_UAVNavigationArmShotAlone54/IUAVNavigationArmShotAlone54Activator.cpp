#include "IUAVNavigationArmShotAlone54Activator.h"

#include "LY_UAVNavigationArmShotAlone54.h"
#include "PlayVideoManage/PlayYoloVideoStreamManageWidget.h"
#include "IUAVTargetTree.h"

IUAVNavigationArmShotAlone54Activator::IUAVNavigationArmShotAlone54Activator()
    : m_IRegistryImpl(nullptr)
    , m_pISystemMaintenanceService(nullptr)
    , m_pIPositionManageService(nullptr)
    , m_pISensorManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
	, m_pISensorVedioService(nullptr)
{

}

IUAVNavigationArmShotAlone54Activator::~IUAVNavigationArmShotAlone54Activator()
{
    m_IRegistryImpl                    = nullptr;
    m_pISystemMaintenanceService       = nullptr;
    m_pIPositionManageService          = nullptr;
    m_pISensorManageService            = nullptr;
    m_pIUAVRealTimeStateManageService  = nullptr;
	m_pISensorVedioService = nullptr;
}

IUAVNavigationArmShotAlone54Activator &IUAVNavigationArmShotAlone54Activator::Get()
{
    static IUAVNavigationArmShotAlone54Activator sIUAVNavigationArmShotAlone54Activator;
    return sIUAVNavigationArmShotAlone54Activator;
}

qnzkna::framework::IRegistry *IUAVNavigationArmShotAlone54Activator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *IUAVNavigationArmShotAlone54Activator::getPISystemMaintenanceService() const
{
    return m_pISystemMaintenanceService;
}

qnzkna::PositionManage::IPositionManageService *IUAVNavigationArmShotAlone54Activator::getPIPositionManageService() const
{
    return m_pIPositionManageService;
}

qnzkna::SensorManage::ISensorManageService *IUAVNavigationArmShotAlone54Activator::getPISensorManageService() const
{
    return m_pISensorManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *IUAVNavigationArmShotAlone54Activator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::TargetManage::IZKLYTargetManageService					 *IUAVNavigationArmShotAlone54Activator::getPITargetManageService() const
{
    return m_pITargetManageService;
}

qnzkna::SensorVedio::ISensorVedioService*  IUAVNavigationArmShotAlone54Activator::getPISensorVedioService() const
{
	return m_pISensorVedioService;
}

void IUAVNavigationArmShotAlone54Activator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->addServiceListener(this);

    m_IRegistryImpl->addMessageListener("MsgUAVNavigationArmShot54", this);
    m_IRegistryImpl->addMessageListener("MsgDYZCHUDControl", this);
    m_IRegistryImpl->addMessageListener("MsgPodAerialShot54", this);
    m_IRegistryImpl->addMessageListener("MsgZCTargetPosition", this);
	m_IRegistryImpl->addMessageListener("MSG_FOCAL_CHANGED", this);
}

void IUAVNavigationArmShotAlone54Activator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return ;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgUAVNavigationArmShot54", this);
    m_IRegistryImpl->removeMessageListener("MsgDYZCHUDControl", this);
    m_IRegistryImpl->removeMessageListener("MsgPodAerialShot54", this);
    m_IRegistryImpl->removeMessageListener("MsgZCTargetPosition", this);
	m_IRegistryImpl->removeMessageListener("MSG_FOCAL_CHANGED", this);
}

bool IUAVNavigationArmShotAlone54Activator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }

    if (service.getServiceName() == "Ly.IMessageObjectManageServiceImpl")
    {
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pISystemMaintenanceService = static_cast<qnzkna::SystemMaintenance::ISystemMaintenanceService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IPositionManageService")
    {
        m_pIPositionManageService = static_cast<qnzkna::PositionManage::IPositionManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ISensorManageService")
    {
        m_pISensorManageService = static_cast<qnzkna::SensorManage::ISensorManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ITargetManageService")
    {
        m_pITargetManageService = static_cast<qnzkna::TargetManage::IZKLYTargetManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *>(service.getService());
    }
	else if (service.getServiceName() == "Ly.ISensorVedioService")
	{
		m_pISensorVedioService = static_cast<qnzkna::SensorVedio::ISensorVedioService*>(service.getService());
	}
    if (m_Component_Property_Component54Map.size() == 0)
    {
        InitWeaponInfoList();
    }

    return true;
}

bool IUAVNavigationArmShotAlone54Activator::removedService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getServiceName() == "Ly.IMessageObjectManageServiceImpl")
    {
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pISystemMaintenanceService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IPositionManageService")
    {
        m_pIPositionManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ISensorManageService")
    {
        m_pISensorManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ITargetManageService")
    {
        m_pITargetManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = nullptr;
    }
	else if (service.getServiceName() == "Ly.ISensorVedioService")
	{
		m_pISensorVedioService = nullptr;
	}
    return true;
}

bool IUAVNavigationArmShotAlone54Activator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();
    if (messageName == "MsgUAVNavigationArmShot54")
    {
        const char        *msg       = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || ((msgLength != XYGSDY54KZSTATUSMSG_LENGTH) && (msgLength != XYGSDY54KZSTATUSFEEDBACKMSG_LENGTH)))
        {
            return true;
        }

        const std::string &strSensorId     = message.getMessageProperties("TargetID");
        if (strSensorId.empty())
        {
            return true;
        }

        if (LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog() != nullptr)
        {
            std::list<IUAVNavigationArmShotAlone54Dialog *>::const_iterator iter = LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog()->begin();
            for (; iter != LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog()->end(); ++iter)
            {
                if (*iter != nullptr && ((*iter)->getCurrentSensorInfo() == strSensorId))
                {
                    (*iter)->on_Message_Recv(strSensorId, QByteArray(msg, msgLength));
                    break;
                }
            }
        }
    }
    else if (messageName == "MsgDYZCHUDControl")
    {
        const char        *msg       = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength <= 0))
        {
            return true;
        }

        const std::string &strTargetID     = message.getMessageProperties("TargetID");
        if (strTargetID.empty())
        {
            return true;
        }

        if (LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog() != nullptr)
        {
            std::list<IUAVNavigationArmShotAlone54Dialog *>::const_iterator iter = LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog()->begin();
            for (; iter != LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog()->end(); ++iter)
            {
                if (*iter != nullptr && ((*iter)->getCurrentComponentInfo() == strTargetID))
                {
                    (*iter)->on_DY_ZC_HUD_ControlInfo(strTargetID, QByteArray(msg, msgLength));
                    break;
                }
            }
        }
    }
    else if (messageName == "MsgPodAerialShot54")
    {
        const char        *msg = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength != AMAPPOD54CONTROLSTATUS_LENGTH))
        {
            return true;
        }

        const std::string &strSensorId = message.getMessageProperties("TargetID");
        if (strSensorId.empty())
        {
            return true;
        }

        const AMapPod54ControlStatus *const aMapPod54ControlStatus = (const AMapPod54ControlStatus *)msg;

        SetPod54ControlStatusInfo(QString::fromStdString(strSensorId), *aMapPod54ControlStatus);
    }
	else if (messageName == "MSG_FOCAL_CHANGED")
	{
		const char        *msg = message.getMessage();
		const unsigned int msgLength = message.getMessageLength();
		if ((nullptr == msg) || (msgLength != sizeof(int)))
		{
			return true;
		}
		int imageZoom = *((const int* const)msg);
		LY_UAVNavigationArmShotAlone54::Get().onFocalZoomChanged(imageZoom);
		//Ly_VideoRealTimePreview::Get().onFocalZoomChanged(imageZoom);

	}
	else if (messageName == "MsgZCTargetPosition")
    {
        const char        *msg = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength != sizeof(XygsZC_Mark_01)))
        {
            return true;
        }

        const std::string &strSensorID = message.getMessageProperties("TargetID");
        if (strSensorID.empty())
        {
            return true;
        }

        const std::string &strComponentID = message.getMessageProperties("WeaponID");
        if (strComponentID.empty())
        {
            return true;
        }
        qDebug() << "WeaponID:" << QString::fromStdString(strComponentID);

        XygsZC_Mark_01 *const pXygsZC_Mark_01 = (XygsZC_Mark_01 * const)msg;
        if (pXygsZC_Mark_01->target_lon == 0 ||  pXygsZC_Mark_01->target_lat == 0)
        {
            return true;
        }

        if (pXygsZC_Mark_01)
        {
            auto widgetList = LY_UAVNavigationArmShotAlone54::Get().getPIUAVNavigationArmShotAlone54Dialog();
            for (auto wdg : *widgetList)
            {
                if (wdg->getCurrentComponentInfo() == strComponentID)
                {
                    qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget* tpWdj = (qnzkna::PlayVideoManage::PlayYoloVideoStreamManageWidget*)wdg->getPlayVideoManageWidget();
                    if (tpWdj == nullptr || tpWdj->timerLaser == nullptr) {
                        continue;
                    }
                    if (tpWdj && tpWdj->timerLaser)
                    {
                        tpWdj->timerLaser->stop();
                    }
                    const auto &mpt = tpWdj->bIslserMap.find((QString::fromStdString(strComponentID)).toLongLong());
                    if (mpt != tpWdj->bIslserMap.end())
                    {
                        if (mpt->second == true)
                        {
                            const auto &mmtr = tpWdj->m_laserTargetMap.find(QString::fromStdString(wdg->getCurrentComponentInfo()).toLongLong());
                            if (mmtr != tpWdj->m_laserTargetMap.end())
                            {
                                mmtr->second.push_back(*pXygsZC_Mark_01);
                            }
                        }
                    }

                }
            }
            XygsZC_Mark_01 xygsZC_Target;

            getPITargetManageService()->GetCurrUAVRectID(pXygsZC_Mark_01->componentID, xygsZC_Target);
            pXygsZC_Mark_01->targetID = xygsZC_Target.targetID;

            LY_UAVNavigationArmShotAlone54::Get().GetUAVTargetTree()->m_AchieveTarget_P.push_back(*pXygsZC_Mark_01);
        }
    }
    return true;
}

bool IUAVNavigationArmShotAlone54Activator::sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentSensorID, const std::string &currentComponentID)
{
    if (sendMsg == nullptr || length <= 0)
    {
        return false;
    }
    qnzkna::SystemMaintenance::ISystemMaintenanceService *pSystemMaintenanceService = getPISystemMaintenanceService();
    if (pSystemMaintenanceService == nullptr)
    {
        return false;
    }
    qnzkna::NodeManage::NODE_TYPE_UINT nLocalType = NODE_TYPE_CMDUNIT;
    std::string                        strLocalID;
    const qnzkna::NodeManage::NODE_TYPE_UINT nTargetType = NODE_TYPE_SENSOR;
    pSystemMaintenanceService->GetNodeInfo(&nLocalType, &strLocalID);

    qnzkna::framework::MessageReferenceProps props;
    props.put("SourceType", std::to_string(nLocalType));
    props.put("SourceID", strLocalID);
    props.put("TargetType", std::to_string(nTargetType));
    props.put("TargetID", currentSensorID);
    props.put("ComponentID", currentComponentID);
    if (getIRegistryImpl() == nullptr)
    {
        return false;
    }
    getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVLoad54Send", sendMsg, length, props));
    return true;
}

void IUAVNavigationArmShotAlone54Activator::InitWeaponInfoList()
{
    if (!m_pISystemMaintenanceService || !m_pISensorManageService)
    {
        return;
    }
    std::set<std::string>  strPlatformList;
    m_pISystemMaintenanceService->GetWeaponComponentPlatforms(&strPlatformList);
    for (const auto &itrPlatform : strPlatformList)
    {
        std::string strPlatformId = itrPlatform;
        qnzkna::WeaponManage::WeaponComponent weaponInfo;
        if (!m_pISystemMaintenanceService->GetWeaponComponent(strPlatformId, &weaponInfo))
        {
            continue;
        }
        std::string strSysId;
        unsigned int nIndex = 0;
        if (!qnzkna::WeaponManage::WeaponSystem::DecodeWeaponComponentID(strSysId, nIndex, strPlatformId))
        {
            continue;
        }
        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoOptoelectronicMap;
        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
        bool bGetSensor = m_pISensorManageService->GetSensorInfosInWeaponComponent(strPlatformId, &tmpSensorInfoMap);
        if (bGetSensor)
        {
            qnzkna::SensorManage::SensorInfoMap::iterator itrSensor = tmpSensorInfoMap.begin();
            for (; itrSensor != tmpSensorInfoMap.end(); itrSensor++)
            {
                qnzkna::SensorManage::CSensorInfo &tmpSensorInfo = itrSensor->second;
                std::string typeOptoelectronic("");
                tmpSensorInfo.GetSensorParam("Type", typeOptoelectronic);
                std::transform(typeOptoelectronic.begin(), typeOptoelectronic.end(), typeOptoelectronic.begin(), ::tolower);
                if ("Optoelectronic" == tmpSensorInfo.GetSensorType() && (typeOptoelectronic == "gaode54"))
                {
                    tmpSensorInfoOptoelectronicMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                }
            }
        }

        for (const auto &tmpSensorInfoOptoelectronic : tmpSensorInfoOptoelectronicMap)
        {
            Component_Property_Component54 component_Property_Component54;
            component_Property_Component54.sysID = QString::fromStdString(strSysId).toULong();
            component_Property_Component54.component_ComponentSysID = strSysId;
            component_Property_Component54.component_ComponentName = weaponInfo.getComponentName();
            component_Property_Component54.component_ComponentID = weaponInfo.getComponentID();
            component_Property_Component54.component_SensorName = tmpSensorInfoOptoelectronic.second.GetSensorName();
            component_Property_Component54.component_SensorID = tmpSensorInfoOptoelectronic.second.GetSensorID();
            m_Component_Property_Component54Map[QString::fromStdString(tmpSensorInfoOptoelectronic.second.GetSensorID())] = component_Property_Component54;
        }
    }
}

void IUAVNavigationArmShotAlone54Activator::SetPod54ControlStatusInfo(const QString &strID, const AMapPod54ControlStatus &stData)
{
    QMutexLocker locker(&sMutex);

    auto itr = m_Component_Property_Component54Map.find(strID);
    if (itr != m_Component_Property_Component54Map.end())
    {
        auto &m_Component_Property_Component54 = m_Component_Property_Component54Map[strID];
        m_Component_Property_Component54.aMapPod54ControlStatus = stData;
    }
}

AMapPod54ControlStatus IUAVNavigationArmShotAlone54Activator::GetPod54ControlStatusInfo(const QString strID)
{
    QMutexLocker locker(&sMutex);

    auto itr = m_Component_Property_Component54Map.find(strID);
    if (itr != m_Component_Property_Component54Map.end())
    {
        return m_Component_Property_Component54Map[strID].aMapPod54ControlStatus;
    }
}

AMapPod54ControlStatus IUAVNavigationArmShotAlone54Activator::GetPod54ControlStatusInfoByUAVID(const QString strID)
{
    QMutexLocker locker(&sMutex);
    AMapPod54ControlStatus stData;
    for (auto itr : m_Component_Property_Component54Map)
    {
        if (strID.compare(QString::fromStdString(itr.component_ComponentID)) == 0)
        {
            return itr.aMapPod54ControlStatus;
        }
    }
    return stData;
}
