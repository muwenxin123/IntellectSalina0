#include "IVideoRealTimePreviewActivator.h"

#include "Ly_VideoRealTimePreview.h"

IVideoRealTimePreviewActivator::IVideoRealTimePreviewActivator()
	: m_IRegistryImpl(nullptr)
	, m_pISensorVedioService(nullptr)
	, m_pIWeaponManageService(nullptr)
	, m_pISystemMaintenanceService(nullptr)
	, m_pITargetManageService(nullptr)
	, m_pINebulaTaskManageService(nullptr)
	, m_pISensorManageService(nullptr)
{

}

IVideoRealTimePreviewActivator::~IVideoRealTimePreviewActivator()
{
	m_IRegistryImpl = nullptr;
	m_pISensorVedioService = nullptr;
	m_pIWeaponManageService = nullptr;
	m_pISystemMaintenanceService = nullptr;
	m_pITargetManageService = nullptr;
	m_pINebulaTaskManageService = nullptr;
	m_pISensorManageService = nullptr;
}

IVideoRealTimePreviewActivator &IVideoRealTimePreviewActivator::Get()
{
	static IVideoRealTimePreviewActivator sIVideoRealTimePreviewActivator;
	return sIVideoRealTimePreviewActivator;
}

qnzkna::framework::IRegistry *IVideoRealTimePreviewActivator::getIRegistryImpl() const
{
	return m_IRegistryImpl;
}

qnzkna::SensorVedio::ISensorVedioService* IVideoRealTimePreviewActivator::getPISensorVedioService() const
{
	return m_pISensorVedioService;
}


qnzkna::WeaponManage::IWeaponManageService* IVideoRealTimePreviewActivator::getPWeaponManageService() const
{
	return m_pIWeaponManageService;
}

qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService* IVideoRealTimePreviewActivator::getPIUAVRealTimeStateManageService() const
{
	return m_pIUAVRealTimeStateManageService;
}

qnzkna::SystemMaintenance::ISystemMaintenanceService * IVideoRealTimePreviewActivator::getISystemMaintenanceService() const
{
	return m_pISystemMaintenanceService;
}

qnzkna::TargetManage::IZKLYTargetManageService* IVideoRealTimePreviewActivator::getPZKLYTargetManageService() const
{
	return m_pITargetManageService;
}

qnzkna::TaskManage::INebulaTaskManageService* IVideoRealTimePreviewActivator::getPTaskManageService() const
{
	return m_pINebulaTaskManageService;
}

qnzkna::SensorManage::ISensorManageService *IVideoRealTimePreviewActivator::getPISensorManageService() const
{
	return m_pISensorManageService;
}

void IVideoRealTimePreviewActivator::start(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}
	m_IRegistryImpl->addServiceListener(this); 
	m_IRegistryImpl->addMessageListener("MsgGimbalAttitude", this);
	m_IRegistryImpl->addMessageListener("MSG_FOCAL_CHANGED", this);
}

void IVideoRealTimePreviewActivator::stop(qnzkna::framework::IRegistry *const context)
{
	m_IRegistryImpl = context;
	if (m_IRegistryImpl == nullptr)
	{
		return;
	}
	m_IRegistryImpl->removeServiceListener(this);
	m_IRegistryImpl->removeMessageListener("MSG_FOCAL_CHANGED", this);
	m_IRegistryImpl->removeMessageListener("MsgGimbalAttitude", this);
}

bool IVideoRealTimePreviewActivator::addingService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getService() == nullptr)
	{
		return false;
	}

	if (service.getServiceName() == "Ly.ISensorVedioService")
	{
		m_pISensorVedioService = static_cast<qnzkna::SensorVedio::ISensorVedioService*>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IWeaponManageService")
	{
		m_pIWeaponManageService = static_cast<qnzkna::WeaponManage::IWeaponManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
	{
		m_pIUAVRealTimeStateManageService = static_cast<qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
	{
		m_pISystemMaintenanceService = static_cast<qnzkna::SystemMaintenance::ISystemMaintenanceService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.ITargetManageService")
	{
		m_pITargetManageService = static_cast<qnzkna::TargetManage::IZKLYTargetManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.ITaskManageService")
	{
		m_pINebulaTaskManageService = static_cast<qnzkna::TaskManage::INebulaTaskManageService *>(service.getService());
	}
	else if (service.getServiceName() == "Ly.ISensorManageService")
	{
		m_pISensorManageService = static_cast<qnzkna::SensorManage::ISensorManageService *>(service.getService());
		
		m_sensorID2ComponentIDMap.clear();
		qnzkna::SensorManage::SensorInfoMap map;
		m_pISensorManageService->GetSensorInfoList("Optoelectronic", &map);
		for (auto iter = map.begin(); iter != map.end(); iter++)
			m_sensorID2ComponentIDMap.insert(iter->first, (QString::fromStdString(iter->second.GetComponentID())).toLongLong());
	}

	return true;
}

bool IVideoRealTimePreviewActivator::removedService(const qnzkna::framework::ServiceReference &service)
{
	if (service.getServiceName() == "Ly.ISensorVedioService")
	{
		m_pISensorVedioService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IWeaponManageService")
	{
		m_pIWeaponManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.IUAVRealTimeStateManageService")
	{
		m_pIUAVRealTimeStateManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.ISystemMaintenanceService")
	{
		m_pISystemMaintenanceService = nullptr;
	}
	else if (service.getServiceName() == "Ly.ITargetManageService")
	{
		m_pITargetManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.ITaskManageService")
	{
		m_pINebulaTaskManageService = nullptr;
	}
	else if (service.getServiceName() == "Ly.ISensorManageService")
	{
		m_pISensorManageService = nullptr;
	}
	return true;
}

bool IVideoRealTimePreviewActivator::dealingMessage(const qnzkna::framework::MessageReference &message)
{
	const std::string &messageName = message.getMessageName();
	if (messageName == "MSG_FOCAL_CHANGED")
	{
		const char        *msg = message.getMessage();
		const unsigned int msgLength = message.getMessageLength();
		if ((nullptr == msg) || (msgLength != sizeof(int)))
		{
			return true;
		}
		int imageZoom =  *((const int* const)msg);
		Ly_VideoRealTimePreview::Get().onFocalZoomChanged(imageZoom);
	}
	else if (messageName == "MsgGimbalAttitude")
	{
		const char        *msg = message.getMessage();
		const unsigned int msgLength = message.getMessageLength();
		if ((nullptr == msg) || (msgLength != sizeof(XygsZC_02)))
		{
			return true;
		}

		const std::string &strSensorId = message.getMessageProperties("TargetID");
		if (strSensorId.empty())
		{
			return true;
		}

		const XygsZC_02 *const pXygsZC_02 = (const XygsZC_02 * const)msg;
		
		auto iter = m_sensorID2ComponentIDMap.find(strSensorId);
		if (iter != m_sensorID2ComponentIDMap.end())
		{
			Ly_VideoRealTimePreview::Get().CameraAttributeChanged(iter.value(), pXygsZC_02->yaw,
				pXygsZC_02->pitch, pXygsZC_02->roll);
		}
	}

	return true;
}

