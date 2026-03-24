
//!!!!!!!!
//!!!!!!!!注意：将本文件编码从utf-8改为ansi才能够使用vc2015编译通过，gcc可能不用更改
//!!!!!!!!


#include "LY_AgriSprayPrescription.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "LY_AgriSprayPrescriptionInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include <QCString.h>

//#include <QDb.h>
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"


#include "LY_AgriSprayPrescriptionInterface.h"

#include "LY_AgriSprayPrescriptionActivator.h"
#include "IFunctionInterface.h"



Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LY_AgriSprayPrescription:: LY_AgriSprayPrescription()
	:m_sprayPrescriptionWidget(nullptr)
	, m_commandWidget(nullptr)
	,m_taskHeaderWidget(nullptr)
	, m_uavFlyLineMarkWidget(nullptr)
	//, m_viewModel(nullptr)
{
	LY_AgriSprayPrescriptionActivator::Get();

}

LY_AgriSprayPrescription::~LY_AgriSprayPrescription()
{

}
void LY_AgriSprayPrescription::Reset()
{

}

void LY_AgriSprayPrescription::Startup()
{
    IFunctionInterface &ifi = LY_AgriSprayPrescriptionInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgriSprayPrescriptionActivator::Get().start(pIRegistry);
		}
	}
}

void LY_AgriSprayPrescription::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriSprayPrescriptionInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgriSprayPrescriptionActivator::Get().stop(pIRegistry);
		}
	}
}

bool LY_AgriSprayPrescription::OnNetRecv(int nLength, char *lpData)
{
	return false;
}
bool LY_AgriSprayPrescription::DrawGraph(QPainter &painter, IProj *pProj)
{

    return false;
}

bool LY_AgriSprayPrescription::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_sprayPrescriptionWidget)
	{

		m_sprayPrescriptionWidget->drawDynamic(painter, pProj);
		m_sprayPrescriptionWidget->draw2DGridInfo(painter, pProj);
		m_commandWidget->zoneHighLight(painter, pProj);
		m_commandWidget->draw2DPathLine(painter, pProj);
	}
    return false;
}

bool LY_AgriSprayPrescription::OnMouseButtonDown(QMouseEvent *e)
{

    return false;
}
void LY_AgriSprayPrescription::OnEarthMessage(int wParam, void *lParam)
{

	pos3d *p3d = (pos3d *)lParam;


	if (wParam == IEarthEventHandler::Earth_LBUTTONDOWN)
	{


	}
	else if (wParam == IEarthEventHandler::Earth_MOUSE_MOVE)
	{
		//qDebug() << "============---:" << p3d->longitude << p3d->latitude;

		IEarth *pEarth = IEarth::GetEarth();
		if (pEarth == nullptr)
			return;

		m_sprayPrescriptionWidget->draw3DGridInfo(p3d);

	}


}
bool LY_AgriSprayPrescription::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriSprayPrescription::OnCommand(int nID)
{
    switch (nID)
    {
    case 433386:{

		if (m_sprayPrescriptionWidget)
		{
		   m_sprayPrescriptionWidget->show();
		   m_sprayPrescriptionWidget ->updataCurActiveFarm();
		}
		if (m_commandWidget)
		{
			m_commandWidget->hide();
		}

	} break;

	case 433390: {

		if (m_commandWidget)
		{
			m_commandWidget->show();
			m_commandWidget->updataCurActiveFarm();
		}
		/*	if (m_uavFlyLineMarkWidget)
			{
				m_uavFlyLineMarkWidget->show();
			}*/
		if (m_sprayPrescriptionWidget)
		{
			m_sprayPrescriptionWidget->hide();
		}
	} break;
	case 50009:
	{

	}
	break;
	case 33389:
	{
		//视频窗口
		//		if (m_ShowVideoData)
		//		{
		//			if (m_ShowVideoData->isHidden())
		//			{
		//				m_ShowVideoData->show();
		//			}
		//			else
		//			{
		//				m_ShowVideoData->hide();
		//			}
		//		}
		//		//任务窗口
		//		if (m_ShowTaskDataInfo)
		//		{
		//			if (m_ShowTaskDataInfo->isHidden())
		//			{
		//				m_ShowTaskDataInfo->SetStyleSheetWidget();
		//				m_ShowTaskDataInfo->show();
		//			}
		//			else
		//			{
		//				m_ShowTaskDataInfo->hide();
		//			}
		//		}
		//		//天气窗口
		//		if (m_ShowWeatherDataInfo)
		//		{
		//			if (m_ShowWeatherDataInfo->isHidden())
		//			{
		//				m_ShowWeatherDataInfo->SetStyleSheetWidget();
		//				m_ShowWeatherDataInfo->show();
		//			}
		//			else
		//			{
		//				m_ShowVideoData->hide();
		//			}
		//		}

	}
	case 900300:
	{

	}
	break;
	case 900200:
	{

	}
	break;
	case 60001:
	{

	}
	break;
    default:
	{
		if (m_sprayPrescriptionWidget)
		{
			m_sprayPrescriptionWidget->hide();
		}
		if (m_commandWidget)
		{
			m_commandWidget->hide();
		}
		if (m_uavFlyLineMarkWidget)
		{
			m_uavFlyLineMarkWidget->hide();
		}
	}
        break;
    }
    return false;
}

bool LY_AgriSprayPrescription::OnCreate(int wParam, QWidget *pWidget)
{


    switch (wParam)
    {
	case 1000:
	{
	
	/*	if (m_viewModel == nullptr)
		{
			m_viewModel = new LY_SprayPrescriptionViewModel();
		}*/
		if (m_sprayPrescriptionWidget == nullptr)
		{
			m_sprayPrescriptionWidget = new LY_AgriSprayPresHomeWidget(pWidget);
			m_sprayPrescriptionWidget->hide();
			m_sprayPrescriptionWidget->move(0, 60);
			m_sprayPrescriptionWidget->resize(QSize(1920, 1020));
		}

		if (m_commandWidget == nullptr)
		{
			m_commandWidget = new LY_AgriSprayPresCommandWidget(pWidget);
			m_commandWidget->hide();
			m_commandWidget->move(0, 60);
			m_commandWidget->resize(QSize(1920, 1020));
		}
	}
	break;

	case 0:
	{

	}
	break;
    case 1:   
    {
		if (m_taskHeaderWidget == nullptr)
		{
			m_taskHeaderWidget = new LY_TaskHeaderWidget();
			m_taskHeaderWidget->resize(QSize(1607, 240));
			m_taskHeaderWidget->move((3840 - 1607) / 2, -2150);
			m_taskHeaderWidget->hide();
		}
		if (m_uavFlyLineMarkWidget == nullptr)
		{
			m_uavFlyLineMarkWidget = new LY_UAVFlyLineMark();
			//m_uavFlyLineMarkWidget->resize(QSize(250, 180));
			m_uavFlyLineMarkWidget->move(800, -200);
			m_uavFlyLineMarkWidget->hide();
		}
		//视频窗口
		//if (m_ShowVideoData == nullptr)
		//{
		//	m_ShowVideoData = new LY_ShowVideoData(pWidget);
		//	m_ShowVideoData->hide();
		//}
		////任务信息窗口
		//if (m_ShowTaskDataInfo == nullptr)
		//{
		//	m_ShowTaskDataInfo = new LY_ShowTaskDataInfo(pWidget);
		//	m_ShowTaskDataInfo->hide();
		//}
		////天气信息窗口
		//if (m_ShowWeatherDataInfo == nullptr)
		//{
		//	m_ShowWeatherDataInfo = new LY_ShowWeatherDataInfo(pWidget);
		//	m_ShowWeatherDataInfo->hide();
		//}

    }
        break;
	case 2:   //上
	{

	}
	break;
	case 3:   //下
	{

	}
	break;
	case 4:
		break;
	}
    return false;
}

bool LY_AgriSprayPrescription::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1://200ms
        LY_AgriSprayPrescriptionInterface::Get().Refresh(1);
        break;
    case 2://1s
        break;
    case 3://60s
        break;
    }
    return true;
}
//LY_SprayPrescriptionViewModel *LY_AgriSprayPrescription::GetViewModel()
//{
//	return m_viewModel;
//}
LY_AgriSprayPresHomeWidget *LY_AgriSprayPrescription::GetSprayPresHomeWidget() const
{
	return m_sprayPrescriptionWidget;
}
LY_AgriSprayPresCommandWidget *LY_AgriSprayPrescription::GetSprayPresCommandWidget() const
{
	return m_commandWidget;
}
LY_TaskHeaderWidget *LY_AgriSprayPrescription::GetTaskHeaderWidget() const
{
	return m_taskHeaderWidget;
}
LY_UAVFlyLineMark *LY_AgriSprayPrescription::GetUavFlyLineMarkWidget() const
{
	return m_uavFlyLineMarkWidget;
}
