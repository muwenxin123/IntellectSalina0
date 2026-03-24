#include "LY_UAVNavigationArmShotAloneFireProt.h"
#include "LY_UAVNavigationArmShotAloneFireProtInterface.h"

#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IUAVNavigationArmShotAloneFireProtActivator.h"

#include "lyUI/LyWidget.h"

#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"

const QString& GetVideoStreamRemoteObjectPrefix()
{
    static QString s_VideoStreamRemoteObjectPrefix("RemoteObjectPrefix_DYFireProt_");
    return s_VideoStreamRemoteObjectPrefix;
}

QWidget *GetVideoStreamWidgetParentPWnd()
{
    QWidget* pWnd = LY_UAVNavigationArmShotAloneFireProt::Get().getPWnd();
    if(pWnd == nullptr){
        return nullptr;
    }
    return pWnd;
}

void SendTargetZCVideoTargetNoPosition(const std::string& strSensorId, const XygsZC_Mark_01& xygsZC_Target)
{

}

void SendGimbalMoveCenter(const std::string& strComponent, const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo& controlInfo)
{

}

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)

LY_UAVNavigationArmShotAloneFireProt::LY_UAVNavigationArmShotAloneFireProt()
    : m_pIUAVNavigationArmShotAloneFireProtDialogList()
    , m_pWnd(nullptr)
{
    IUAVNavigationArmShotAloneFireProtActivator::Get();

    m_pIUAVNavigationArmShotAloneFireProtDialogList.clear();
}

void LY_UAVNavigationArmShotAloneFireProt::Startup()
{
    IFunctionInterface &ifi = LY_UAVNavigationArmShotAloneFireProtInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
        qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
        if(pIRegistry != nullptr){
            IUAVNavigationArmShotAloneFireProtActivator::Get().start(pIRegistry);
        }
    }
}

void LY_UAVNavigationArmShotAloneFireProt::Shutdown()
{
    IFunctionInterface &ifi = LY_UAVNavigationArmShotAloneFireProtInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry*>()) {
        qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
        if(pIRegistry != nullptr){
            IUAVNavigationArmShotAloneFireProtActivator::Get().stop(pIRegistry);
        }
    }

    if(m_pIUAVNavigationArmShotAloneFireProtDialogList.size() > 0){
        for(auto*  pIUAVNavigationArmShotAloneFireProtDialog : m_pIUAVNavigationArmShotAloneFireProtDialogList)
        {
            if(pIUAVNavigationArmShotAloneFireProtDialog != nullptr){
                pIUAVNavigationArmShotAloneFireProtDialog->StopIUAVNavigationArmShotAloneFireProtVideoManage();
            }
        }
    }
}

void LY_UAVNavigationArmShotAloneFireProt::Reset()
{

}

bool LY_UAVNavigationArmShotAloneFireProt::DrawGraph( QPainter &painter, IProj *pProj )
{
    return false;
}

bool LY_UAVNavigationArmShotAloneFireProt::DrawDynamic( QPainter &painter, IProj *pProj )
{
    return false;
}

bool LY_UAVNavigationArmShotAloneFireProt::OnMouseButtonDown( QMouseEvent *e )
{
    bool bReturn = false;
    return bReturn;
}

bool LY_UAVNavigationArmShotAloneFireProt::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_UAVNavigationArmShotAloneFireProt::OnCommand(int wParam , void *lParam)
{
    switch(wParam)
    {
    case 43385:
    {

    }
        break;
    case 44777:
    {

    }
        break;
    case 44778:
    {

    }
        break;
    case 60001:
    {
        if(m_pIUAVNavigationArmShotAloneFireProtDialogList.size() > 0){
            for(auto*  pIUAVNavigationArmShotAloneFireProtDialog : m_pIUAVNavigationArmShotAloneFireProtDialogList)
            {
                if(pIUAVNavigationArmShotAloneFireProtDialog != nullptr){
                    pIUAVNavigationArmShotAloneFireProtDialog->OnCommand(wParam, lParam);
                }
            }
        }
    }
    default:
        break;
    }
    return false;
}

bool LY_UAVNavigationArmShotAloneFireProt::OnCreate( int wParam, QWidget *pTab )
{
    switch(wParam)
    {
    case 0:
    {

    }
        break;
    case 1:
    {
        qnzkna::SystemConfig::IConfigProjectControlService*  pIConfigProjectControlService  =  IUAVNavigationArmShotAloneFireProtActivator::Get().getPIConfigProjectControlService();
        qnzkna::WeaponManage::IWeaponManageService*          pIWeaponManageService          =  IUAVNavigationArmShotAloneFireProtActivator::Get().getPIWeaponManageService();
        qnzkna::PositionManage::IPositionManageService*      pIPositionManageService        =  IUAVNavigationArmShotAloneFireProtActivator::Get().getPIPositionManageService();
        qnzkna::SensorManage::ISensorManageService*          pISensorManageService          =  IUAVNavigationArmShotAloneFireProtActivator::Get().getPISensorManageService();

        if(pIConfigProjectControlService != nullptr && pIWeaponManageService != nullptr && pIPositionManageService != nullptr && pISensorManageService != nullptr){

            qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
            if(pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap)){
                for(const auto& weaponSystemIter : weaponSystemMap){
                    const auto& mapWeaponComponent = weaponSystemIter.second.getMapWeaponComponent();
                    for(const auto& mapWeaponComponentIter : mapWeaponComponent){
                        const qnzkna::WeaponManage::WeaponComponent& weaponComponent = (mapWeaponComponentIter.second);
                        qnzkna::WeaponManage::WeaponComponent_Type componentType = weaponComponent.getPWeaponModel()->getComponentType();

                        if ((qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_RotorUAV != componentType)
                                && (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_C2VEHICLE != componentType)
                                && (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_TSER != componentType)
                                && (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_TSATV != componentType)
                                && (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_VCCS != componentType)
                                && (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_ACS != componentType)
                                && (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_CTP != componentType)
                                && (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_MSAR != componentType)
                                && (qnzkna::WeaponManage::WeaponComponent_Type::WeaponComponent_Type_UTWRCS != componentType)) {
                            continue;
                        }

                        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoFireControlMap;
                        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoOptoelectronicMap;
                        qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
                        bool bGetSensor = pISensorManageService->GetSensorInfosInWeaponComponent(weaponComponent.getComponentID(), &tmpSensorInfoMap);
                        if (bGetSensor) {
                            qnzkna::SensorManage::SensorInfoMap::iterator itrSensor = tmpSensorInfoMap.begin();
                            for (; itrSensor != tmpSensorInfoMap.end(); itrSensor++) {
                                qnzkna::SensorManage::CSensorInfo& tmpSensorInfo = itrSensor->second;
                                std::string DetectType("");
                                tmpSensorInfo.GetSensorParam(std::string("Detect"), DetectType);
                                if ("FireControlFireProt" == tmpSensorInfo.GetSensorType() && DetectType != "1") {
                                    tmpSensorInfoFireControlMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                                }

                                std::string typeOptoelectronic("");
                                tmpSensorInfo.GetSensorParam("Type", typeOptoelectronic);
                                if ("Optoelectronic" == tmpSensorInfo.GetSensorType() && (typeOptoelectronic == "FireProt")) {
                                    tmpSensorInfoOptoelectronicMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                                }
                            }
                        }

                        if (tmpSensorInfoFireControlMap.size() > 0) {
                            bool bGetPlatform = bGetSensor;
                            if (bGetPlatform) {
                                const QString strComponentName = QString::fromStdString(weaponComponent.getComponentName());
                                const QString strComponentID   = QString::fromStdString(weaponComponent.getComponentID());
                                const QString dockWidgetId = "IUAVNavigationArmShotAloneFireProtDialog:" + strComponentID;

                                const QString windowTitile = strComponentName + ":\xe6\xb6\x88\xe9\x98\xb2\xe5\xbc\xb9\xe6\x8e\xa7\xe5\x88\xb6";

                                IUAVNavigationArmShotAloneFireProtDialog* pIUAVNavigationArmShotAloneFireProtDialog = new IUAVNavigationArmShotAloneFireProtDialog(strComponentID, strComponentName);
                                if (pIUAVNavigationArmShotAloneFireProtDialog->initSensorAndBomb(tmpSensorInfoFireControlMap)) {
                                    pIUAVNavigationArmShotAloneFireProtDialog->initSensorAndOptoelectronic(tmpSensorInfoOptoelectronicMap);
                                    LyDockWidget *pDock = LyDockManager::instance()->dockWidget(dockWidgetId);
                                    if (pDock == nullptr)
                                    {
                                        pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, windowTitile, dockWidgetId);
                                        LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
                                    }
                                    pDock->setCentralWidget(pIUAVNavigationArmShotAloneFireProtDialog);
                                    pDock->setWindowTitle(windowTitile);
                                    pIUAVNavigationArmShotAloneFireProtDialog->setWindowTitle(windowTitile);
                                    if(!pIUAVNavigationArmShotAloneFireProtDialog->GetWindowTitle().isEmpty()){
                                        pDock->setWindowTitle(pIUAVNavigationArmShotAloneFireProtDialog->GetWindowTitle());
                                    }
                                    m_pIUAVNavigationArmShotAloneFireProtDialogList.push_back(pIUAVNavigationArmShotAloneFireProtDialog);

                                    LyDockWidget::connect(pIUAVNavigationArmShotAloneFireProtDialog, &IUAVNavigationArmShotAloneFireProtDialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);
                                }
                                else {
                                    delete pIUAVNavigationArmShotAloneFireProtDialog;
                                    pIUAVNavigationArmShotAloneFireProtDialog = nullptr;
                                }
                            }
                        }
                    }
                }
            }
        }

#if 0
        qnzkna::SystemMaintenance::ISystemMaintenanceService*   pSystemMaintenanceService      =  IUAVNavigationArmShotAloneFireProtActivator::Get().getPISystemMaintenanceService();
        qnzkna::SensorManage::ISensorManageService*             pISensorManageService          =  IUAVNavigationArmShotAloneFireProtActivator::Get().getPISensorManageService();
        if(pSystemMaintenanceService != nullptr && pISensorManageService != nullptr) {
            std::set<std::string> Platforms;
            pSystemMaintenanceService->GetWeaponComponentPlatforms(&Platforms);
            for (const auto& itrPlatform : Platforms) {
                std::set<std::string>	ChildComponents;
                std::string strPlatform = itrPlatform;
                bool bGetChild = true;
                if (bGetChild) {
                    qnzkna::SensorManage::SensorInfoMap tmpSensorInfoFireControlMap;
                    qnzkna::SensorManage::SensorInfoMap tmpSensorInfoOptoelectronicMap;
                    qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
                    bool bGetSensor = pISensorManageService->GetSensorInfosInWeaponComponent(strPlatform, &tmpSensorInfoMap);
                    if (bGetSensor) {
                        qnzkna::SensorManage::SensorInfoMap::iterator itrSensor = tmpSensorInfoMap.begin();
                        for (; itrSensor != tmpSensorInfoMap.end(); itrSensor++) {
                            qnzkna::SensorManage::CSensorInfo& tmpSensorInfo = itrSensor->second;
                            std::string DetectType("");
                            tmpSensorInfo.GetSensorParam(std::string("Detect"), DetectType);
                            if ("FireControlFireProt" == tmpSensorInfo.GetSensorType() && DetectType != "1") {
                                tmpSensorInfoFireControlMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                            }

                            std::string typeOptoelectronic("");
                            tmpSensorInfo.GetSensorParam("Type", typeOptoelectronic);
                            if ("Optoelectronic" == tmpSensorInfo.GetSensorType() && (typeOptoelectronic == "gaodeFireProt")) {
                                tmpSensorInfoOptoelectronicMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                            }
                        }
                    }

                    if (tmpSensorInfoFireControlMap.size() > 0) {
                        qnzkna::WeaponManage::WeaponComponent weaponComponent;
                        bool bGetPlatform = pSystemMaintenanceService->GetWeaponComponent(strPlatform, &weaponComponent);
                        if (bGetPlatform) {
                            const QString strComponentName = QString::fromStdString(weaponComponent.getComponentName());
                            const QString strComponentID   = QString::fromStdString(weaponComponent.getComponentID());
                            const QString dockWidgetId = "IUAVNavigationArmShotAloneFireProtDialog:" + strComponentID;
                            const QString windowTitile = IUAVNavigationArmShotAloneFireProtDialog::tr("Fire Control FireProt:") + strComponentName;

                            IUAVNavigationArmShotAloneFireProtDialog* pIUAVNavigationArmShotAloneFireProtDialog = new IUAVNavigationArmShotAloneFireProtDialog(strComponentID, strComponentName);
                            if (pIUAVNavigationArmShotAloneFireProtDialog->initSensorAndBomb(tmpSensorInfoFireControlMap)) {
                                pIUAVNavigationArmShotAloneFireProtDialog->initSensorAndOptoelectronic(tmpSensorInfoOptoelectronicMap);
                                LyDockWidget *pDock = LyDockManager::instance()->dockWidget(dockWidgetId);
                                if (pDock == nullptr)
                                {
                                    pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, windowTitile, dockWidgetId);
                                    LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
                                }
                                pDock->setCentralWidget(pIUAVNavigationArmShotAloneFireProtDialog);
                                pDock->setWindowTitle(windowTitile);
                                pIUAVNavigationArmShotAloneFireProtDialog->setWindowTitle(windowTitile);
                                m_pIUAVNavigationArmShotAloneFireProtDialogList.push_back(pIUAVNavigationArmShotAloneFireProtDialog);

                                LyDockWidget::connect(pIUAVNavigationArmShotAloneFireProtDialog, &IUAVNavigationArmShotAloneFireProtDialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);
                            }
                            else {
                                delete pIUAVNavigationArmShotAloneFireProtDialog;
                                pIUAVNavigationArmShotAloneFireProtDialog = nullptr;
                            }
                        }
                    }
                }
            }
        }
#endif
    }
        break;
    case 2:
    {

    }
        break;
    case 3:
    {

    }
        break;
    case 4: break;
    }
    return false;
}

bool LY_UAVNavigationArmShotAloneFireProt::OnTimer( int wParam )
{
    switch (wParam)
    {
    case 1:
        break;
    case 2:
        break;
    case 3:
        break;
    }

    return false;
}

bool LY_UAVNavigationArmShotAloneFireProt::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

const std::list<IUAVNavigationArmShotAloneFireProtDialog*> *LY_UAVNavigationArmShotAloneFireProt::getPIUAVNavigationArmShotAloneFireProtDialog() const
{
    return &m_pIUAVNavigationArmShotAloneFireProtDialogList;
}

