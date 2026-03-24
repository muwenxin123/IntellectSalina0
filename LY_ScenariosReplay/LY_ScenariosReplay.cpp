#include "lY_ScenariosReplay.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "lY_ScenariosReplayInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include <QDb.h>
#include "map3d/IEarth.h"
#include "map3d/EarthEvent.h"
#include <map3d/IEarth.h>
#include <map3d/ITrack.h>
#include <map3d/IGraphics.h>

#include "ScenarioReplayDialog.h"
#include "IFunctionInterface.h"

#include "IScenariosReplayActivator.h"
#include "lyUI/LyDockManager.h"
#include <LyDockWidget.h>
#include "QCString.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry*)
LY_ScenariosReplay::LY_ScenariosReplay()
	//:m_pScenarioReplayWidgetDialog(nullptr)
{
    m_pScenarioReplayDialog = nullptr;
    IScenariosReplayActivator::Get();

	m_pIScenariosReplayThread = new IScenariosReplayThread(m_pWnd);
}

void LY_ScenariosReplay::Startup()
{
	IFunctionInterface &ifi = LY_ScenariosReplayInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>())
	{
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr)
		{
			IScenariosReplayActivator::Get().start(pIRegistry);
		}
	}
}

void LY_ScenariosReplay::Shutdown()
{
	IFunctionInterface &ifi = LY_ScenariosReplayInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry*>())
	{
		qnzkna::framework::IRegistry* pIRegistry = vt.value<qnzkna::framework::IRegistry*>();
		if (pIRegistry != nullptr)
		{
			IScenariosReplayActivator::Get().stop(pIRegistry);
		}
	}
}

void LY_ScenariosReplay::Reset()
{

}

bool LY_ScenariosReplay::DrawGraph( QPainter &painter, IProj *pProj )
{

    return false;
}

bool LY_ScenariosReplay::DrawDynamic( QPainter &painter, IProj *pProj )
{

    return false;
}

bool LY_ScenariosReplay::OnMouseButtonDown( QMouseEvent *e )
{
    bool bReturn = false;
    QPoint pt = e->pos();
    switch(e->button())
    {
    case Qt::LeftButton:
    {

    }
        break;
    case Qt::RightButton:
    {

    }
        break;
    case Qt::MiddleButton:
    {

    }
        break;
    }

    return bReturn;
}

bool LY_ScenariosReplay::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_ScenariosReplay::OnCommand( int nID )
{
    switch(nID)
    {
	case 58001:
	{

	}
        break;
    case 44777:
    {
        QMessageBox::information(NULL, QObject::tr("Sample Title"), QObject::tr("Sample title 44777!"));
    }
        return true;
    case 44778:
    {

    }
        break;
    default:
        break;
    }
    return false;
}

bool LY_ScenariosReplay::OnCreate( int wParam, QTabWidget *pTab )
{
    switch(wParam)
    {
    case 1:
    {
		LyDockWidget *pScenarioReplayDialog = LyDockManager::instance()->dockWidget("ScenarioReplayDialog");

		//if (m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog == nullptr && m_pUAVFlyManage_MavlinkTaskManageDialog == nullptr) {

		//	m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog = new LyDialog(tr2("º½ÏßÉÏ´«"), pTab);
		//	m_pUAVFlyManage_MavlinkTaskManageDialog = new UAVFlyManage_MavlinkTaskManageDialog(m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog);
		//	if (m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog != nullptr && m_pUAVFlyManage_MavlinkTaskManageDialog != nullptr) {

		//		m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->setResizable(true);
		//		m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->resize(m_pUAVFlyManage_MavlinkTaskManageDialog->size());
		//		QVBoxLayout* verticalLayout = new QVBoxLayout();
		//		if (verticalLayout) {
		//			verticalLayout->addWidget(m_pUAVFlyManage_MavlinkTaskManageDialog);
		//			m_pUAVFlyManage_MavlinkTaskManageLyWidgetDialog->setLayout(verticalLayout);
		//		}
		//	}
		//}


		if (pScenarioReplayDialog == nullptr)
		{
			pScenarioReplayDialog = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, QObject::tr("Data Replay"), "ScenarioReplayDialog");
			LyDockManager::instance()->embedToolDockWidget(pScenarioReplayDialog, LyDockDef::Bottom);
		}
		if (m_pScenarioReplayDialog == nullptr)
		{
			m_pScenarioReplayDialog = new ScenarioReplayDialog(pTab);
			pScenarioReplayDialog->setCentralWidget(m_pScenarioReplayDialog);
			pScenarioReplayDialog->setWindowTitle(QObject::tr("Data Replay"));
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

bool LY_ScenariosReplay::OnTimer( int wParam )
{
    switch (wParam)
    {
    case 1:
        LY_ScenariosReplayInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}
