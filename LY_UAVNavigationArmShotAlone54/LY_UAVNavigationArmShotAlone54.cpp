#include "LY_UAVNavigationArmShotAlone54.h"
#include "LY_UAVNavigationArmShotAlone54Interface.h"

#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IUAVNavigationArmShotAlone54Activator.h"
#include "IUAVNavigationArmShotAlone54_ALL_Dialog.h"
#include "IUAVNavigationArmShotAlone54_95_1_StatusSync.h"

#include "lyUI/LyWidget.h"

#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"
#include "IUAVTargetTree.h"
#include "netheader.h"

#include "UAVArmShotAloneListDialog.h"

const QString &GetVideoStreamRemoteObjectPrefix()
{
    static QString s_VideoStreamRemoteObjectPrefix("RemoteObjectPrefix_DY54_");
    return s_VideoStreamRemoteObjectPrefix;
}

QWidget *GetVideoStreamWidgetParentPWnd()
{
    QWidget *pWnd = LY_UAVNavigationArmShotAlone54::Get().getPWnd();
    if (pWnd == nullptr)
    {
        return nullptr;
    }
    return pWnd;
}

void SendTargetZCVideoTargetNoPosition(const std::string &strSensorId, const XygsZC_Mark_01 &xygsZC_Target)
{
    if (IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl() != nullptr)
    {
        qnzkna::framework::MessageReferenceProps props;
        props.put("TargetType",   std::to_string(NODE_TYPE_SENSOR));
        props.put("TargetID",     strSensorId);
        props.put("WeaponID",     "");

        XygsZC_Mark_01 sXygsZC_Mark_01 = xygsZC_Target;

        IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
            new qnzkna::framework::MessageReference("MsgZCVideoTargetNoPosition", (char *)&sXygsZC_Mark_01, sizeof(sXygsZC_Mark_01), props));
    }

}

void SendGimbalDYZCControl(const std::string &strComponent, const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo &controlInfo)
{
    if (IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl() != nullptr)
    {
        qnzkna::framework::MessageReferenceProps props;
        props.put("TargetID", strComponent);

        IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
            new qnzkna::framework::MessageReference("MsgDYZCControl", (const char *)&controlInfo, sizeof(controlInfo), props));
    }
}

void SendGimbalMoveCenter(const std::string &strComponent, const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo &controlInfo)
{
    if (IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl() != nullptr)
    {
        qnzkna::framework::MessageReferenceProps props;
        props.put("TargetID",     strComponent);

        IUAVNavigationArmShotAlone54Activator::Get().getIRegistryImpl()->messageNotify(
            new qnzkna::framework::MessageReference("MsgDYZCControl", (const char *)&controlInfo, sizeof(controlInfo), props));
    }
}

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LY_UAVNavigationArmShotAlone54::LY_UAVNavigationArmShotAlone54()
    : m_pIUAVNavigationArmShotAlone54DialogList()
    , m_pIUAVNavigationArmShotAlone54_ALL_Dialog(nullptr)
    , m_pIUAVNavigationArmShotAlone54_95_1_StatusSync(nullptr)
	, m_pIUAVArmSHotAloneListDialog(nullptr)
    , m_UseUdpSocket_95_1_StatusSync(false)
    , m_pWnd(nullptr)
    , m_p2DView(nullptr)
{
    IUAVNavigationArmShotAlone54Activator::Get();

    m_pIUAVNavigationArmShotAlone54DialogList.clear();
}

void LY_UAVNavigationArmShotAlone54::Startup()
{
    IFunctionInterface &ifi = LY_UAVNavigationArmShotAlone54Interface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IUAVNavigationArmShotAlone54Activator::Get().start(pIRegistry);
        }
    }
}

void LY_UAVNavigationArmShotAlone54::Shutdown()
{
    IFunctionInterface &ifi = LY_UAVNavigationArmShotAlone54Interface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IUAVNavigationArmShotAlone54Activator::Get().stop(pIRegistry);
        }
    }
}

void LY_UAVNavigationArmShotAlone54::Reset()
{

}

bool LY_UAVNavigationArmShotAlone54::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_UAVNavigationArmShotAlone54::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_UAVNavigationArmShotAlone54::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_UAVNavigationArmShotAlone54::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_UAVNavigationArmShotAlone54::OnCommand(int nID, void *lParam)
{
    switch (nID)
    {
    case 433387:
    {
		if (m_pIUAVArmSHotAloneListDialog)
		{
			m_pIUAVArmSHotAloneListDialog->show();
		}
    }
    break;
	case 60001:
	{
		break;
	}
	break;
	case 900200:
	{
		break;
	}
	default:
	{
		if (m_pIUAVArmSHotAloneListDialog)
		{
			m_pIUAVArmSHotAloneListDialog->hide();
		}
	}
	break;
    }
    return false;
}

bool LY_UAVNavigationArmShotAlone54::OnCreate(int wParam, QWidget *pTab)
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
    switch (wParam)
    {
    case 0:
    {
        m_pWnd = pTab;
    }
    break;
    case 1000:
    {
		m_pWnd = pTab;

        qnzkna::SystemMaintenance::ISystemMaintenanceService   *pSystemMaintenanceService      =  IUAVNavigationArmShotAlone54Activator::Get().getPISystemMaintenanceService();
        qnzkna::SensorManage::ISensorManageService             *pISensorManageService          =  IUAVNavigationArmShotAlone54Activator::Get().getPISensorManageService();
        if (pSystemMaintenanceService != nullptr && pISensorManageService != nullptr)
        {
            m_UseUdpSocket_95_1_StatusSync = IUAVNavigationArmShotAlone54_95_1_StatusSync::UseUdpSocket_95_1_StatusSync();

            std::set<std::string> Platforms;
            pSystemMaintenanceService->GetWeaponComponentPlatforms(&Platforms);
            for (const auto &itrPlatform : Platforms)
            {
                std::set<std::string>	ChildComponents;
                std::string strPlatform = itrPlatform;
                bool bGetChild = true;
                if (bGetChild)
                {
                    qnzkna::SensorManage::SensorInfoMap tmpSensorInfoFireControlMap;
                    qnzkna::SensorManage::SensorInfoMap tmpSensorInfoOptoelectronicMap;
                    qnzkna::SensorManage::SensorInfoMap tmpSensorInfoMap;
                    bool bGetSensor = pISensorManageService->GetSensorInfosInWeaponComponent(strPlatform, &tmpSensorInfoMap);
                    if (bGetSensor)
                    {
                        qnzkna::SensorManage::SensorInfoMap::iterator itrSensor = tmpSensorInfoMap.begin();
                        for (; itrSensor != tmpSensorInfoMap.end(); itrSensor++)
                        {
                            qnzkna::SensorManage::CSensorInfo &tmpSensorInfo = itrSensor->second;
                            std::string DetectType("");
                            tmpSensorInfo.GetSensorParam(std::string("Detect"), DetectType);
                            if (("FireControl54" == tmpSensorInfo.GetSensorType() || "FireControl38mm" == tmpSensorInfo.GetSensorType() || "FireControl95-1" == tmpSensorInfo.GetSensorType() || "FireControl95-1-Wolf" == tmpSensorInfo.GetSensorType() || "FireControl38mmPWM" == tmpSensorInfo.GetSensorType() || "FireControlFireProt" == tmpSensorInfo.GetSensorType()) && DetectType != "1")
                            {
                                tmpSensorInfoFireControlMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                            }

                            std::string typeOptoelectronic("");
                            tmpSensorInfo.GetSensorParam("Type", typeOptoelectronic);
                            if ("Optoelectronic" == tmpSensorInfo.GetSensorType() && (typeOptoelectronic == "gaode54"))
                            {
                                tmpSensorInfoOptoelectronicMap.insert(std::make_pair(tmpSensorInfo.GetSensorID(), tmpSensorInfo));
                            }
                        }
                    }

                    if (tmpSensorInfoFireControlMap.size() > 0)
                    {
                        qnzkna::WeaponManage::WeaponComponent weaponComponent;
                        bool bGetPlatform = pSystemMaintenanceService->GetWeaponComponent(strPlatform, &weaponComponent);
                        if (bGetPlatform)
                        {
                            const QString strComponentName = QString::fromStdString(weaponComponent.getComponentName());
                            const QString strComponentID   = QString::fromStdString(weaponComponent.getComponentID());
                            const QString dockWidgetId = "IUAVNavigationArmShotAlone54Dialog:" + strComponentID;
                            //const QString windowTitile = IUAVNavigationArmShotAlone54Dialog::tr("Fire Control 54:") + strComponentName;
                            QString windowTitile = IUAVNavigationArmShotAlone54Dialog::tr("Fire Control 54:") + strComponentName;


                            IUAVNavigationArmShotAlone54Dialog *pIUAVNavigationArmShotAlone54Dialog = new IUAVNavigationArmShotAlone54Dialog(strComponentID, strComponentName);
                            if (pIUAVNavigationArmShotAlone54Dialog->initSensorAndBomb(tmpSensorInfoFireControlMap))
                            {
                                pIUAVNavigationArmShotAlone54Dialog->initSensorAndOptoelectronic(tmpSensorInfoOptoelectronicMap);
                                if (!pIUAVNavigationArmShotAlone54Dialog->getTitleInfo().isEmpty()) {
                                    if (pIUAVNavigationArmShotAlone54Dialog->getTitleInfo() == " ") {
                                        windowTitile = strComponentName;
                                    }
                                    else
                                    {
                                        windowTitile = strComponentName + (":") + (pIUAVNavigationArmShotAlone54Dialog->getTitleInfo());
                                    }
                                }
                                LyDockWidget *pDock = LyDockManager::instance()->dockWidget(dockWidgetId);
                                if (pDock == nullptr)
                                {
                                    pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, windowTitile, dockWidgetId);
                                    LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
                                }
                                pDock->setCentralWidget(pIUAVNavigationArmShotAlone54Dialog);
                                pDock->setWindowTitle(windowTitile);
                                pIUAVNavigationArmShotAlone54Dialog->setWindowTitle(windowTitile);
                                m_pIUAVNavigationArmShotAlone54DialogList.push_back(pIUAVNavigationArmShotAlone54Dialog);

                                LyDockWidget::connect(pIUAVNavigationArmShotAlone54Dialog, &IUAVNavigationArmShotAlone54Dialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);
                            }
                            else
                            {
                                delete pIUAVNavigationArmShotAlone54Dialog;
                                pIUAVNavigationArmShotAlone54Dialog = nullptr;
                            }
                        }
                    }
                }
            }

			if (m_pIUAVArmSHotAloneListDialog == nullptr)
			{
				m_pIUAVArmSHotAloneListDialog = new UAVArmShotAloneListDialog(pTab);

				if (m_pIUAVNavigationArmShotAlone54DialogList.size() > 0)
				{
					m_pIUAVArmSHotAloneListDialog->InitDialog(m_pIUAVNavigationArmShotAlone54DialogList);
				}

				m_pIUAVArmSHotAloneListDialog->move(QPoint(0, 80));
				m_pIUAVArmSHotAloneListDialog->resize(QSize(screenSize.width() - 760,screenSize.height() - 440/*1160, 640*/));
				m_pIUAVArmSHotAloneListDialog->hide();
			}


            // 一键 加锁 解锁 击发
            if(m_pIUAVNavigationArmShotAlone54DialogList.size() > 0)
            {
                IUAVNavigationArmShotAlone54_ALL_Dialog *pIUAVNavigationArmShotAlone54_ALL_Dialog= new IUAVNavigationArmShotAlone54_ALL_Dialog();
                if (pIUAVNavigationArmShotAlone54_ALL_Dialog->initSensorAndBombList(m_pIUAVNavigationArmShotAlone54DialogList))
                {
                    const QString& windowTitile = "\xe4\xb8\x80\xe9\x94\xae \xe5\x8a\xa0\xe9\x94\x81 \xe8\xa7\xa3\xe9\x94\x81 \xe5\x87\xbb\xe5\x8f\x91";

                    const QString dockWidgetId = "IUAVNavigationArmShotAlone54_ALL_Dialog";
                    LyDockWidget *pDock = LyDockManager::instance()->dockWidget(dockWidgetId);
                    if (pDock == nullptr)
                    {
                        pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, windowTitile, dockWidgetId);
                        LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Right);
                    }
                    pDock->setCentralWidget(pIUAVNavigationArmShotAlone54_ALL_Dialog);
                    pDock->setWindowTitle(windowTitile);
                    pIUAVNavigationArmShotAlone54_ALL_Dialog->setWindowTitle(windowTitile);
                    LyDockWidget::connect(pIUAVNavigationArmShotAlone54_ALL_Dialog, &IUAVNavigationArmShotAlone54Dialog::windowTitleChanged, pDock, &LyDockWidget::setWindowTitle);

                    m_pIUAVNavigationArmShotAlone54_ALL_Dialog = pIUAVNavigationArmShotAlone54_ALL_Dialog;
                }

				//将pIUAVNavigationArmShotAlone54_ALL_Dialog 放到 m_pIUAVArmSHotAloneListDialog列表中

                if(m_UseUdpSocket_95_1_StatusSync && m_pIUAVNavigationArmShotAlone54_95_1_StatusSync == nullptr){
                    m_pIUAVNavigationArmShotAlone54_95_1_StatusSync = new IUAVNavigationArmShotAlone54_95_1_StatusSync();
                }
            }
        }
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
    case 4:
        break;
	case 6:
		m_p2DView = pTab;
		break;
	//case 1000:
	//{
	//	//if (m_pIUAVArmSHotAloneListDialog == nullptr)
	//	//{
	//	//	m_pIUAVArmSHotAloneListDialog = new UAVArmShotAloneListDialog(pTab);
	//	//	m_pIUAVArmSHotAloneListDialog->move(QPoint(0, 60));
	//	//	m_pIUAVArmSHotAloneListDialog->resize(QSize(1160, 640));
	//	//	m_pIUAVArmSHotAloneListDialog->hide();
	//	//}
	//}
    }
    return false;
}

bool LY_UAVNavigationArmShotAlone54::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1: //200ms
        break;
    case 2:
        break;
    case 3:
        break;
    }

    return false;
}

bool LY_UAVNavigationArmShotAlone54::OnNetRecv(int nLength, char *lpData)
{
    if (nLength <= sizeof(NetHeader))
    {
        return false;
    }


    NetHeader* pNetHeader = (NetHeader*)(lpData);

    if (pNetHeader->ucType == NETHEADER_UCTYPE_DY38MM_CONTROL) {
        if (getPIUAVNavigationArmShotAlone54Dialog() != nullptr)
        {
            std::list<IUAVNavigationArmShotAlone54Dialog *>::const_iterator iter = getPIUAVNavigationArmShotAlone54Dialog()->begin();
            for (; iter != getPIUAVNavigationArmShotAlone54Dialog()->end(); ++iter)
            {
                if (*iter != nullptr &&  (QString::fromStdString((*iter)->getCurrentSensorInfo()).toULongLong() == pNetHeader->unNodeID))
                {
                    (*iter)->Recv_netheader_uctype_dy38mm_control(lpData, nLength);
                    break;
                }
            }
        }
    }

    return false;
}

const std::list<IUAVNavigationArmShotAlone54Dialog *> *LY_UAVNavigationArmShotAlone54::getPIUAVNavigationArmShotAlone54Dialog() const
{
    return &m_pIUAVNavigationArmShotAlone54DialogList;
}

bool LY_UAVNavigationArmShotAlone54::getUseUdpSocket_95_1_StatusSync() const
{
    return m_UseUdpSocket_95_1_StatusSync;
}

IUAVNavigationArmShotAlone54_95_1_StatusSync *LY_UAVNavigationArmShotAlone54::getPIUAVNavigationArmShotAlone54_95_1_StatusSync() const
{
    return m_pIUAVNavigationArmShotAlone54_95_1_StatusSync;
}

int LY_UAVNavigationArmShotAlone54::sendMessage_95_1_StatusSync_control(const char * const pbuf, const int pbufSize)
{
    if(m_UseUdpSocket_95_1_StatusSync && m_pIUAVNavigationArmShotAlone54_95_1_StatusSync != nullptr){
        return m_pIUAVNavigationArmShotAlone54_95_1_StatusSync->sendMessage_95_1_StatusSync_control(pbuf, pbufSize);
    }

    return 0;
}

void LY_UAVNavigationArmShotAlone54::onFocalZoomChanged(int mFocal)
{
	currFocal = mFocal;
}

