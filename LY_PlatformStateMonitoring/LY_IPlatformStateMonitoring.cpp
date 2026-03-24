#include "LY_IPlatformStateMonitoring.h"
#include "LY_IPlatformStateMonitoringInterface.h"

#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IPlatformStateMonitoringActivator.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LY_IPlatformStateMonitoring::LY_IPlatformStateMonitoring()
    : m_pUAVPlaneStatesDialog(nullptr),
      m_pUAVPlatformStateDialog(nullptr),
      m_pUAVPlanRealTimeAlititudeDialog(nullptr),
      m_pUAVPlanSystemState(nullptr)
{
    IPlatformStateMonitoringActivator::Get();
}

void LY_IPlatformStateMonitoring::Startup()
{
    IFunctionInterface &ifi = LY_IPlatformStateMonitoringInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IPlatformStateMonitoringActivator::Get().start(pIRegistry);
        }
    }
}

void LY_IPlatformStateMonitoring::Shutdown()
{
    IFunctionInterface &ifi = LY_IPlatformStateMonitoringInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IPlatformStateMonitoringActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_IPlatformStateMonitoring::Reset()
{

}

bool LY_IPlatformStateMonitoring::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_IPlatformStateMonitoring::DrawDynamic(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_IPlatformStateMonitoring::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    return bReturn;
}

bool LY_IPlatformStateMonitoring::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_IPlatformStateMonitoring::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	case 900200:
	{
		if (m_pUAVPlanSystemState != nullptr)
		{
			auto param1 = (EventInfo*)(lParam);
			m_pUAVPlanSystemState->UpdateMissionStates(*param1);
		}
	}
	break;
	case 900300:
	{
		if (m_pUAVPlanSystemState != nullptr)
		{
			auto param1 = (EventInfo*)(lParam);
			m_pUAVPlanSystemState->UpdateMissionStates(*param1);
		}
	}
	break;
	case 433387:
	{
		if (m_pUAVPlanSystemState != nullptr)
		{
			//m_pUAVPlanSystemState->show();
			m_pUAVPlanSystemState->hide();
		}
	}
	break;
	case 60001:
	{

	}
	break;
	default:
		if (m_pUAVPlanSystemState != nullptr)
		{
			m_pUAVPlanSystemState->hide();
		}
		break;
	}
	return false;
	return false;
}

bool LY_IPlatformStateMonitoring::OnCreate(int wParam, QWidget *pTab)
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
    switch (wParam)
    {
    case 0:
    {

    }
    break;
    case 1000:
    {

#ifdef QWT_CONFIG_ENABLE_DEFINES

        {
            LyDockWidget *pUAVPlanRealTimeAlititudeDialog = LyDockManager::instance()->dockWidget("UAVPlanRealTimeAlititudeDialog");
            if(pUAVPlanRealTimeAlititudeDialog == nullptr)
            {
                pUAVPlanRealTimeAlititudeDialog = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, QStringLiteral("UAV Altitude Line Chart"),"UAVPlanRealTimeAlititudeDialog");
                LyDockManager::instance()->embedToolDockWidget(pUAVPlanRealTimeAlititudeDialog,LyDockDef::Right);

            }
            if(m_pUAVPlanRealTimeAlititudeDialog == nullptr)
            {
                m_pUAVPlanRealTimeAlititudeDialog = new UAVPlanRealTimeAlititudeDialog();
                pUAVPlanRealTimeAlititudeDialog->setCentralWidget(m_pUAVPlanRealTimeAlititudeDialog);
                pUAVPlanRealTimeAlititudeDialog->setWindowTitle(UAVPlanRealTimeAlititudeDialog::tr("UAV Altitude Line Chart"));
                LyDockWidget::connect(m_pUAVPlanRealTimeAlititudeDialog, &UAVPlanRealTimeAlititudeDialog::windowTitleChanged, pUAVPlanRealTimeAlititudeDialog, &LyDockWidget::setWindowTitle);

            }
        }
#endif

        {
            //LyDockWidget *pUAVPlanSystemState = LyDockManager::instance()->dockWidget("UAVPlanSystemState");
            //if(pUAVPlanSystemState == nullptr)
            //{
            //    pUAVPlanSystemState = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, UAVPlanSystemState::tr("UAV System State Information"), "UAVPlanSystemState");
            //    LyDockManager::instance()->embedToolDockWidget(pUAVPlanSystemState,LyDockDef::Right);

            //}
            if(m_pUAVPlanSystemState == nullptr)
            {
                m_pUAVPlanSystemState = new UAVPlanSystemState(pTab);
				m_pUAVPlanSystemState->move(QPoint(0, screenSize.height() - 380));
				m_pUAVPlanSystemState->setFixedSize(QSize(screenSize.width() - 760, 380 /*1160, 1020*/));
                //pUAVPlanSystemState->setCentralWidget(m_pUAVPlanSystemState);
                m_pUAVPlanSystemState->InitAllState();
				m_pUAVPlanSystemState->CreateTree();
				m_pUAVPlanSystemState->InitUAVEventStates();
				m_pUAVPlanSystemState->hide();
                //pUAVPlanSystemState->setWindowTitle(UAVPlanSystemState::tr("UAV System State Information"));
                //LyDockWidget::connect(m_pUAVPlanSystemState, &UAVPlanSystemState::windowTitleChanged, pUAVPlanSystemState, &LyDockWidget::setWindowTitle);
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
    }
    return false;
}

bool LY_IPlatformStateMonitoring::OnTimer(int wParam)
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

bool LY_IPlatformStateMonitoring::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

UAVPlaneStatesDialog *LY_IPlatformStateMonitoring::getPUAVPlaneStatesDialog() const
{
    return m_pUAVPlaneStatesDialog;
}

UAVPlatformStateDialog *LY_IPlatformStateMonitoring::getPUAVPlatformStateDialog() const
{
    return m_pUAVPlatformStateDialog;
}

UAVPlanRealTimeAlititudeDialog *LY_IPlatformStateMonitoring::getPUAVPlanRealTimeAlititudeDialog() const
{
    return m_pUAVPlanRealTimeAlititudeDialog;
}

UAVPlanSystemState *LY_IPlatformStateMonitoring::getPUAVPlanSystemState() const
{
    return m_pUAVPlanSystemState;
}

UAVPlaneStare_ForFire * LY_IPlatformStateMonitoring::getPUAVPlanStare_ForFire() const
{
	return m_pUAVPlaneStare_ForFire;
}

void LY_IPlatformStateMonitoring::SetScreenCenter(const double dLon, const double dLat)
{
    if(m_pProj){
        QString str = "CenterAt(" + QString::number(dLon,'f', 8) + "," + QString::number(dLat,'f', 8) + ")";
        LY_IPlatformStateMonitoringInterface::Get().Script(str.toLocal8Bit());
    }
}
