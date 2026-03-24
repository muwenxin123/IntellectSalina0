#ifndef IAGRILAYERCONTROLACTIVATOR_H_
#define IAGRILAYERCONTROLACTIVATOR_H_
 
#include "ServiceFramework/IBundleActivator.h"
#include "ServiceFramework/IServiceInterfaceTracker.h"
#include "ServiceFramework/IServiceInterface.h"
#include "ServiceFramework/IMessageInterfaceTracker.h"

#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/IPositionManageService.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "DataManage/TextToSpeechManage/ITextToSpeechManageService.h"
#include "DataManage/IZoneManageService.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IAgriFarmManagerService.h"

class IAgriRestfulManagerActivator
	: public qnzkna::framework::IBundleActivator

	, public qnzkna::framework::IServiceInterfaceTracker
{
public:
	IAgriRestfulManagerActivator();
	virtual ~IAgriRestfulManagerActivator();

	static IAgriRestfulManagerActivator &Get();

	qnzkna::framework::IRegistry                                   *getIRegistryImpl() const;
	qnzkna::SystemConfig::IConfigProjectControlService             *getPIConfigProjectControlService() const;
	qnzkna::WeaponManage::IWeaponManageService                     *getPIWeaponManageService() const;
	qnzkna::PositionManage::IPositionManageService                 *getPIPositionManageService() const;
	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *getPIUAVRealTimeStateManageService() const;
	qnzkna::TextToSpeechManage::ITextToSpeechManageService         *getPITextToSpeechManageService() const;
	qnzkna::ZoneManage::IZoneManageService							 *getPIZoneManageService() const;
	qnzkna::TaskManage::INebulaTaskManageService					*getPTaskManageService() const;
	qnzkna::AgriFarmManager::IAgriFarmManagerService				*getPIAgriFarmManagerService() const;
public:
	virtual void start(qnzkna::framework::IRegistry *const context);
	virtual void stop(qnzkna::framework::IRegistry *const context);

	virtual bool addingService(const qnzkna::framework::ServiceReference &service);
	virtual bool removedService(const qnzkna::framework::ServiceReference &service);
	virtual bool dealingMessage(const qnzkna::framework::MessageReference &message);

private:
	qnzkna::framework::IRegistry*                                        m_IRegistryImpl;
	qnzkna::SystemConfig::IConfigProjectControlService*                  m_pIConfigProjectControlService;
	qnzkna::WeaponManage::IWeaponManageService*                          m_pIWeaponManageService;
	qnzkna::PositionManage::IPositionManageService*                      m_pIPositionManageService;
	qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService*      m_pIUAVRealTimeStateManageService;
	qnzkna::TextToSpeechManage::ITextToSpeechManageService*              m_pITextToSpeechManageService;
	qnzkna::ZoneManage::IZoneManageService*								 m_pIZoneManageService;
	qnzkna::TaskManage::INebulaTaskManageService*			             m_pITaskManageService;
	qnzkna::AgriFarmManager::IAgriFarmManagerService*			         m_pIAgriFarmManagerService;
};

#define	ZoneManageService()				IAgriRestfulManagerActivator::Get().getPIZoneManageService()
#define	AgriFarmManagerService()		IAgriRestfulManagerActivator::Get().getPIAgriFarmManagerService()
#endif  // IAgriLayerControlActivator_H