#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include "QGeoCoordinate"
ITaskManagerActivator::ITaskManagerActivator()
    : m_IRegistryImpl(nullptr)
    , m_pIConfigProjectControlService(nullptr)
    , m_pISystemMaintenanceService(nullptr)
    , m_pITaskManageService(nullptr)
    , m_pIWeaponManageService(nullptr)
    , m_pIFormationManageService(nullptr)
    , m_pIUAVRealTimeStateManageService(nullptr)
    , m_pIMarkManageService(nullptr)
    , m_pICommandUnitManageService(nullptr)
    , m_pAmmunitionManageService(nullptr)
{

}

ITaskManagerActivator::~ITaskManagerActivator()
{
    m_IRegistryImpl = nullptr;
    m_pIConfigProjectControlService = nullptr;
    m_pISystemMaintenanceService = nullptr;
    m_pITaskManageService = nullptr;
    m_pIWeaponManageService = nullptr;
    m_pIFormationManageService = nullptr;
    m_pIUAVRealTimeStateManageService = nullptr;
    m_pIMarkManageService = nullptr;
    m_pICommandUnitManageService = nullptr;
    m_pAmmunitionManageService = nullptr;
}

ITaskManagerActivator &ITaskManagerActivator::Get()
{
    static ITaskManagerActivator sITaskManagerActivator;
    return sITaskManagerActivator;
}

qnzkna::framework::IRegistry *ITaskManagerActivator::getIRegistryImpl() const
{
    return m_IRegistryImpl;
}

qnzkna::SystemConfig::IConfigProjectControlService *ITaskManagerActivator::getPIConfigProjectControlService() const
{
    return m_pIConfigProjectControlService;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService *ITaskManagerActivator::getISystemMaintenanceService() const
{
    return m_pISystemMaintenanceService;
}

qnzkna::ZoneManage::IZoneManageService * ITaskManagerActivator::getIZoneManageService() const
{
	return m_pIZoneManageService;
}

qnzkna::WeaponManage::IWeaponManageService *ITaskManagerActivator::getPWeaponManageService() const
{
    return m_pIWeaponManageService;
}

qnzkna::TaskManage::INebulaTaskManageService *ITaskManagerActivator::getPTaskManageService() const
{
    return m_pITaskManageService;
}

qnzkna::UAVFormManage::IFormationManageService *ITaskManagerActivator::getPFormationManageService() const
{
    return m_pIFormationManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *ITaskManagerActivator::getPIUAVRealTimeStateManageService() const
{
    return m_pIUAVRealTimeStateManageService;
}

qnzkna::MarkManage::IMarkManageService *ITaskManagerActivator::getPIMarkManageService() const
{
    return m_pIMarkManageService;
}

qnzkna::CCommandUnitManage::ICommandUnitManageService *ITaskManagerActivator::getPICommandUnitManageService() const
{
    return m_pICommandUnitManageService;
}

qnzkna::AmmunitionManage::IAmmunitionManageService *ITaskManagerActivator::getPAmmunitionManageService() const
{
    return m_pAmmunitionManageService;
}

bool ITaskManagerActivator::sendMessage(const char *const sendMsg, const unsigned int length, const std::string &currentComponentID)
{
    return true;
}

void ITaskManagerActivator::start(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return;
    }

    m_IRegistryImpl->addServiceListener(this);

    m_IRegistryImpl->addMessageListener("MsgUAVParameterState", this);
    m_IRegistryImpl->addMessageListener("MsgStrikeSchemeList", this);
    m_IRegistryImpl->addMessageListener("MsgMarkAmmoID", this);
    m_IRegistryImpl->addMessageListener("MsgOnlyMarkCreateSend", this);
    m_IRegistryImpl->addMessageListener("MsgPlatAmmoID", this);
    m_IRegistryImpl->addMessageListener("MsgOnlyLinepointSend", this);
    m_IRegistryImpl->addMessageListener("TaskDataEdit", this);
	m_IRegistryImpl->addMessageListener("DeviationVoyageRoute", this);
	m_IRegistryImpl->addMessageListener("DistanceVoyageRoute", this);
}

void ITaskManagerActivator::stop(qnzkna::framework::IRegistry *const context)
{
    m_IRegistryImpl = context;
    if (m_IRegistryImpl == nullptr)
    {
        return;
    }

    m_IRegistryImpl->removeServiceListener(this);

    m_IRegistryImpl->removeMessageListener("MsgUAVParameterState", this);
    m_IRegistryImpl->removeMessageListener("MsgStrikeSchemeList", this);
    m_IRegistryImpl->removeMessageListener("MsgMarkAmmoID", this);
    m_IRegistryImpl->removeMessageListener("MsgOnlyMarkCreateSend", this);
    m_IRegistryImpl->removeMessageListener("MsgPlatAmmoID", this);
    m_IRegistryImpl->removeMessageListener("MsgOnlyLinepointSend", this);
    m_IRegistryImpl->removeMessageListener("TaskDataEdit", this);
	m_IRegistryImpl->removeMessageListener("DeviationVoyageRoute", this);
	m_IRegistryImpl->removeMessageListener("DistanceVoyageRoute", this);
}

bool ITaskManagerActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getService() == nullptr)
    {
        return false;
    }
    else if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = static_cast<qnzkna::SystemConfig::IConfigProjectControlService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pISystemMaintenanceService = static_cast<qnzkna::SystemMaintenance::ISystemMaintenanceService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pITaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IFormManageService")
    {
        m_pIFormationManageService = static_cast<qnzkna::UAVFormManage::IFormationManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IMarkManageService")
    {
        m_pIMarkManageService = static_cast<qnzkna::MarkManage::IMarkManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.ICommandUnitManageService")
    {
        m_pICommandUnitManageService = static_cast<qnzkna::CCommandUnitManage::ICommandUnitManageService *>(service.getService());
    }
    else if (service.getServiceName() == "Ly.IAmmunitionManageService")
    {
        m_pAmmunitionManageService = static_cast<qnzkna::AmmunitionManage::IAmmunitionManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = static_cast<qnzkna::ZoneManage::IZoneManageService *>(service.getService());
	}

    return true;
}

bool ITaskManagerActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
    if (service.getServiceName() == "Ly.IConfigProjectControlService")
    {
        m_pIConfigProjectControlService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
    {
        m_pISystemMaintenanceService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ITaskManageService")
    {
        m_pITaskManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IWeaponManageService")
    {
        m_pIWeaponManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IFormManageService")
    {
        m_pIFormationManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
    {
        m_pIUAVRealTimeStateManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IMarkManageService")
    {
        m_pIMarkManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.ICommandUnitManageService")
    {
        m_pICommandUnitManageService = nullptr;
    }
    else if (service.getServiceName() == "Ly.IAmmunitionManageService")
    {
        m_pAmmunitionManageService = nullptr;
    }
	else if (service.getServiceName() == "Ly.IZoneManageService")
	{
		m_pIZoneManageService = nullptr;
	}
    return true;
}

bool ITaskManagerActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
    const std::string &messageName = message.getMessageName();
    if (messageName == "MsgUAVParameterState")
    {
        const char        *msg = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();

        if ((nullptr == msg) || (msgLength <= 0))
        {
            return true;
        }

        const std::string &strWeaponId = message.getMessageProperties("TargetID");

        if (strWeaponId.empty())
        {
            return true;
        }

        if (LY_TaskManage::Get().GetDlgEditLine() != nullptr)
        {
            LY_TaskManage::Get().GetDlgEditLine()->DealUAVParameterInfo(QString::fromStdString(strWeaponId), QByteArray(msg, msgLength));
        }
    }
    else if (messageName == "MsgStrikeSchemeList")
    {
        const std::string &strGenerate = message.getMessageProperties("OpenGenerate");
        const std::string &strMarkID = message.getMessageProperties("TargetID");
        const std::string &strDamageRequest = message.getMessageProperties("DamageRequest");
        const std::string &strSchemeID = message.getMessageProperties("SchemeID");
        if (strGenerate.empty())
        {
            return true;
        }
        if (strGenerate == "1")
        {
            LY_TaskManage::Get().GetDlgMissionList()->b_openTask = true;
            LY_TaskManage::Get().GetDlgMissionEdit()->SchemeUpdateMark(strMarkID, strDamageRequest);
            LY_TaskManage::Get().GetDlgMissionEdit()->m_sSchemeID = strSchemeID;

        }
    }
    else if (messageName == "MsgMarkAmmoID")
    {
        const std::string &strUpdateInfo = message.getMessageProperties("UpdateInfo");
        if (strUpdateInfo.empty())
        {
            return true;
        }
        if (strUpdateInfo == "1")
        {
            LY_TaskManage::Get().GetDlgMissionPlatformLoadList()->b_UpdateAmmo = true;
        }
    }
    else if (messageName == "MsgOnlyMarkCreateSend")
    {
        const char        *msg = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();

        if ((nullptr == msg) || (msgLength <= 0))
        {
            return true;
        }
        const std::string &strMark = message.getMessageProperties("targetid");
        const std::string &strLon = message.getMessageProperties("startLon");
        const std::string &strLat = message.getMessageProperties("startLat");
        const std::string &strAlt = message.getMessageProperties("startAlt");
        const std::string &strLineName = message.getMessageProperties("routename");
        double dlat = atof(strLat.c_str());
        double dlon = atof(strLon.c_str());
        double dalt = atof(strAlt.c_str());
        QGeoCoordinate point(dlat, dlon, dalt);
        LY_TaskManage::Get().GetDlgMissionEdit()->AcceptOnlyMarkInfo(msg, strMark, strLineName, point);

    }
    else if (messageName == "MsgPlatAmmoID")
    {
        const std::string &strUpdatePlatAmmoList = message.getMessageProperties("UpdatePlatAmmoList");
        if (strUpdatePlatAmmoList.empty())
        {
            return true;
        }
        if (strUpdatePlatAmmoList == "1" &&  LY_TaskManage::Get().GetDlgMissionPlatformList())
        {
            LY_TaskManage::Get().GetDlgMissionPlatformList()->b_UpdatePlatAmmo = true;
        }
    }
    else if (messageName == "MsgOnlyLinepointSend")
    {
        const char *msg = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength <= 0))
        {
            return true;
        }
        const std::string &strSize = message.getMessageProperties("routepointNum");
        LY_TaskManage::Get().GetDlgMissionEdit()->AcceptOnlyLineInfo(msg, strSize);

    }
    else if (messageName == "TaskDataEdit")
    {
        const char *msg = message.getMessage();
        const unsigned int msgLength = message.getMessageLength();
        if ((nullptr == msg) || (msgLength <= 0))
        {
            return true;
        }
        memcpy(&TaskDataEdit_pMsg03, msg, sizeof(TeleMsg_FB_03));
        if (LY_TaskManage::Get().GetDlgMissionEdit() != nullptr)
        {
            emit LY_TaskManage::Get().GetDlgMissionEdit()->DlgMissionEditShow();
        }

    }
	else if (messageName == "DeviationVoyageRoute")
	{
		const char* msg = message.getMessage();
		if (nullptr == msg)
			return true;
		const std::string& strMsg = message.getMessageProperties("UAVName");
		LY_TaskManage::Get().GetDlgPlay()->m_VecUAVName.push_back(strMsg);
		emit LY_TaskManage::Get().GetDlgPlay()->DLGPlayDeviationVoyage();
	}
	else if (messageName == "DistanceVoyageRoute_editHeight")
	{
		const char* msg = message.getMessage();
		if (nullptr == msg)
			return true;
		const std::string& strMsg = message.getMessageProperties("UAVName");
		const std::string& strMsg1 = message.getMessageProperties("UAVName1");
		LY_TaskManage::Get().GetDlgPlay()->m_VecUAVName.push_back(strMsg);
		LY_TaskManage::Get().GetDlgPlay()->m_VecUAVName.push_back(strMsg1);
		emit LY_TaskManage::Get().GetDlgPlay()->DLGPlayDeviationVoyage();
	}
    return true;
}

