#include <QPoint>
#include <QMouseEvent>
#include <QScreen>
#include "LY_AgriFarmManager.h"
#include "LY_AgriFarmManagerInterface.h"
#include "AgriAddFarmDialog.h"
#include <QDebug>
#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"
#include <QTimer>
#include "IAgriFarmManagerActivator.h"
#include "QMainWindow"
#include "LyDockWidget.h"
Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

qnzkna::framework::IRegistry           *LY_AgriFarmManager::s_IRegistryImpl = nullptr;
qnzkna::AgriFarmManager::IAgriFarmManagerServiceImpl LY_AgriFarmManager::s_IAgriFarmManagerServiceImpl;

LY_AgriFarmManager::LY_AgriFarmManager() 
	:m_pLegendPopup(nullptr)
	,m_pAgriFieldInfoWidget(nullptr)
{
    IAgriFarmManagerActivator::Get();
}

void LY_AgriFarmManager::Startup()
{
    IFunctionInterface &ifi = LY_AgriFarmManagerInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriFarmManagerActivator::Get().start(pIRegistry);
			s_IRegistryImpl = pIRegistry;
        }
    }

	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IAgriFarmManagerService", &(s_IAgriFarmManagerServiceImpl)));
	}
}

void LY_AgriFarmManager::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriFarmManagerInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriFarmManagerActivator::Get().stop(pIRegistry);
        }
    }

	if (s_IRegistryImpl != nullptr)
	{
		s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IAgriFarmManagerService", &(s_IAgriFarmManagerServiceImpl)));
	}
}

void LY_AgriFarmManager::Reset()
{

}

bool LY_AgriFarmManager::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriFarmManager::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_pAgriFarmManagerDialog != NULL)
	{
		if (m_pAgriFarmManagerDialog->isVisible()) {
			m_pAgriFarmManagerDialog->ZoneHighLight_Tree(painter, pProj);
		}
	}
    return false;
}

bool LY_AgriFarmManager::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;

	QPoint pt = e->pos();

	if (e->button() == Qt::LeftButton && m_pAgriFarmManagerDialog)
	{
		auto dialog = m_pAgriFarmManagerDialog->getAddFarmDialog();
		if (dialog && dialog->isPositionPick())
		{
			double lon;
			double lat;
			m_pProj->Screen2Jwd(pt, lon, lat);
			dialog->updateTargetPoint(QPointF(lon, lat));
		}
	}
    return bReturn;
}

bool LY_AgriFarmManager::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}
bool LY_AgriFarmManager::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	case 433383:
	{
		if (m_pAgriFarmManagerDialog)
		{
			if (m_pAgriFarmManagerDialog->isHidden())
			{
				m_pAgriFarmManagerDialog->show();
			}
		}
		if (QGuiApplication::screens().count() < 2)
		{
			QVariant mapVariant = qApp->property("ly_map_lydialog");
			if (mapVariant.canConvert<LyDockWidget*>()) {
				LyDockWidget* castedObj = qvariant_cast<LyDockWidget*>(mapVariant);
				if (nullptr != castedObj)
				{
					castedObj->setVisible(false);
				}
			}
		}
	}
	break;
	case 33389:
	{
	}
	break;
	case 433391:
	{
		if (m_pAgriFarmManagerDialog)
		{
			m_pAgriFarmManagerDialog->hide();
		}
		if (m_pLegendPopup)
		{
			m_pLegendPopup->hide();
		}
		if (QGuiApplication::screens().count() < 2)
		{
			QVariant mapVariant = qApp->property("ly_map_lydialog");
			if (mapVariant.canConvert<LyDockWidget*>()) {
				LyDockWidget* castedObj = qvariant_cast<LyDockWidget*>(mapVariant);
				if (nullptr != castedObj)
				{
					castedObj->raise();
					castedObj->show();
				}
			}
		}
	}
	break;
	default:
	{
		if (m_pAgriFarmManagerDialog)
		{
			m_pAgriFarmManagerDialog->hide();
		}
		if (m_pLegendPopup)
		{
			m_pLegendPopup->hide();
		}
		if (QGuiApplication::screens().count() < 2)
		{
			QVariant mapVariant = qApp->property("ly_map_lydialog");
			if (mapVariant.canConvert<LyDockWidget*>()) {
				LyDockWidget* castedObj = qvariant_cast<LyDockWidget*>(mapVariant);
				if (nullptr != castedObj)
				{
					castedObj->setVisible(false);
				}
			}
		}
	}
		break;
	}
	return false;
}

bool LY_AgriFarmManager::OnCreate(int wParam, QWidget *pWgt)
{
	QSize screenSize = qApp->property("ly_screen_xy").toSize();
    switch (wParam)
    {
    case 0:
    {

    }
    break;
	case 1:
	{
	
// 		if (m_pLegendPopup == nullptr)
// 		{
// 			m_pLegendPopup = new AgriLegendPopup(pWgt);
// 			m_pLegendPopup->resize(QSize(544, 421));
// 			m_pLegendPopup->move(2846, -440);
// 		}
// 
// 		if (m_pAgriFieldInfoWidget == nullptr)
// 		{
// 			m_pAgriFieldInfoWidget = new LY_AgriFieldInfoWidget();
// 			m_pAgriFieldInfoWidget->resize(QSize(753, 2050));
// 			m_pAgriFieldInfoWidget->move(3087, -2112);
// 		}
	}
	break;
	case 1000:
    {
	
		if (m_pAgriFarmManagerDialog == nullptr)
		{
			// 初始化数据库数据
			m_pAgriFarmDatabaseHandler = new AgriFarmDatabaseHandler(pWgt);

			// 初始化界面数据
			m_pAgriFarmManagerDialog = new AgriFarmManagerDialog(pWgt);

			// 默认显示（2025/03/27）
			m_pAgriFarmManagerDialog->move(0, 80);
			m_pAgriFarmManagerDialog->resize(QSize(screenSize.width(), screenSize.height() - 80));
			m_pAgriFarmManagerDialog->show();
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

bool LY_AgriFarmManager::OnTimer(int wParam)
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

bool LY_AgriFarmManager::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

void LY_AgriFarmManager::SetScreenCenter(const double dLon, const double dLat)
{
    if(m_pProj){
        QString str = "CenterAt(" + QString::number(dLon,'f', 8) + "," + QString::number(dLat,'f', 8) + ")";
        LY_AgriFarmManagerInterface::Get().Script(str.toLocal8Bit());
    }
}
AgriLegendPopup * LY_AgriFarmManager::GetLegendPopup()
{
	return m_pLegendPopup;
}
LY_AgriFieldInfoWidget *LY_AgriFarmManager::GetAgriFieldInfoWidget()
{
	return m_pAgriFieldInfoWidget;
}