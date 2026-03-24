
//!!!!!!!!
//!!!!!!!!注意：将本文件编码从utf-8改为ansi才能够使用vc2015编译通过，gcc可能不用更改
//!!!!!!!!


#include "LY_AgricultureHomepage.h"
#include <QPainter>
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include <osgEarth/MapNode>
#include "LY_AgricultureHomepageInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
#include <QCString.h>

//#include <QDb.h>
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"


#include "LY_AgricultureHomepageInterface.h"

#include "LY_AgricultureHomepageActivator.h"
#include "IFunctionInterface.h"

#include "TinyXML\tinyxml.h"

#include <iostream>
#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <QPointer>
#include <QDebug>
#include <qstring.h>
#include "LY_AgricultureHomepageActivator.h"
#include "KrigingInterPolator.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)



LY_AgricultureHomepage:: LY_AgricultureHomepage()
: m_moudleManagerWidget(nullptr)
, m_viewModel(nullptr)
, m_addMonitorWidget(nullptr)
, m_homePageWidget(nullptr)
, m_harmManagerWidget(nullptr)
, m_ffmpegUntil(nullptr)
{
	LY_AgricultureHomepageActivator::Get();

}

LY_AgricultureHomepage::~LY_AgricultureHomepage()

{

}
void LY_AgricultureHomepage::Reset()
{

}

void LY_AgricultureHomepage::Startup()
{
    IFunctionInterface &ifi = LY_AgricultureHomepageInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgricultureHomepageActivator::Get().start(pIRegistry);
		}
	}
}

void LY_AgricultureHomepage::Shutdown()
{
    IFunctionInterface &ifi = LY_AgricultureHomepageInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			LY_AgricultureHomepageActivator::Get().stop(pIRegistry);
		}
	}
}

bool LY_AgricultureHomepage::OnNetRecv(int nLength, char *lpData)
{
	return false;
}
bool LY_AgricultureHomepage::DrawGraph(QPainter &painter, IProj *pProj)
{


    return true;
}

bool LY_AgricultureHomepage::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_moudleManagerWidget != nullptr)
	{
		m_moudleManagerWidget->DrawDynamic(painter, pProj);
	}

	return true;
}
void LY_AgricultureHomepage::OnEarthMessage(int wParam, void *lParam)
{
	if (lParam == nullptr)
	{
		return;
	}
	pos3d *p3d = (pos3d *)lParam;
	m_moudleManagerWidget->draw3dText();
	if (wParam == IEarthEventHandler::Earth_LBUTTONDOWN)
	{
		m_viewModel->m_clickPointVec.clear();
		if (m_viewModel == nullptr || p3d == nullptr)
		{
			return;
		}
		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
		double height_t = 0.0;
		mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), p3d->longitude, p3d->latitude, &height_t);
		m_viewModel->lon = p3d->longitude;
		m_viewModel->lat = p3d->latitude;
		m_viewModel->altitude = height_t;
		m_viewModel->screenX = p3d->x;
		m_viewModel->screenY = p3d->y;

		ZonePoint zoneTmepPoint;
		zoneTmepPoint.longitude = p3d->longitude;
		zoneTmepPoint.latitude = p3d->latitude;
		zoneTmepPoint.x = p3d->x;
		zoneTmepPoint.y = p3d->y;
		zoneTmepPoint.altitude = height_t;

		m_viewModel->m_clickPointVec.push_back(zoneTmepPoint);

		m_viewModel->click3DPoint();
		
		//m_viewModel->convert3DCube(p3d->longitude, p3d->latitude, height_t);
	}


}
bool LY_AgricultureHomepage::OnMouseButtonDown(QMouseEvent *e)
{
	bool bReturn = false;
	QPoint pt = e->pos();

	if (e->button() == Qt::LeftButton )
	{

		double lon;
		double lat;
		m_pProj->Screen2Jwd(pt, lon, lat);
		QPointF llPoint(lon, lat);
		if (m_viewModel == nullptr)
		{
			return false;
		}
		m_viewModel->lon = lon;
		m_viewModel->lat = lat;

	}
    return false;
}

bool LY_AgricultureHomepage::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgricultureHomepage::OnCommand(int nID)
{
    switch (nID)
    {

	//case 433382:
	//{
	//	if (m_homePageWidget != nullptr)
	//	{
	//		m_homePageWidget->show();
	//	}
	//}
	//break;
	case 433389:
	{
		if (m_harmManagerWidget != nullptr)
		{
			m_harmManagerWidget->show();

		}
		m_moudleManagerWidget->hide();
	}
	break;

	case 433384:
	{
		if (m_moudleManagerWidget != nullptr)
		{
			m_moudleManagerWidget->show();
			
		}
		m_harmManagerWidget->hide();
	}
	break;

	default:
	{

		m_moudleManagerWidget->hide();
		m_harmManagerWidget->hide();
		
	}
        break;
    }
    return false;
}

bool LY_AgricultureHomepage::OnCreate(int wParam, QWidget *pWidget)
{


    switch (wParam)
    {
	case 0:
	{


	}
	break;
    case 1:   
    {

    }
        break;
	case 1000:   //上
	{
		if (m_viewModel == nullptr)
		{
			m_viewModel = new LY_MointorViewModel();
		}
		if (m_ffmpegUntil == nullptr)
		{
			m_ffmpegUntil = new FFmpegUntil();
		}
		if (m_moudleManagerWidget == nullptr)
		{
			m_moudleManagerWidget = new LY_MoudleManageWidget(pWidget);
			m_moudleManagerWidget->hide();
		}

		if (m_harmManagerWidget == nullptr)
		{
			m_harmManagerWidget = new LY_HarmManagerWidget(pWidget);
			m_moudleManagerWidget->updateData();
			m_harmManagerWidget->hide();
		}
		//if (m_homePageWidget == nullptr)
		//{
		//	m_homePageWidget = new LY_AgriculHomePageWidget(pWidget);
		//	m_homePageWidget->hide();
		//}

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

bool LY_AgricultureHomepage::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1://200ms
        LY_AgricultureHomepageInterface::Get().Refresh(1);
        break;
    case 2://1s
        break;
    case 3://60s
        break;
    }
    return true;
}

LY_MoudleManageWidget * LY_AgricultureHomepage::GetMoudleManagerWidget() const
{
	return m_moudleManagerWidget;
}
LY_MointorViewModel *LY_AgricultureHomepage::GetViewModel() const
{
	return m_viewModel;
}
LY_MointerAddWidget *LY_AgricultureHomepage::GetAddMonitorWidget() const
{
	return m_addMonitorWidget;
}
LY_AgriculHomePageWidget *LY_AgricultureHomepage::GetHomePageWidget() const
{
	return m_homePageWidget;
}
LY_HarmManagerWidget *LY_AgricultureHomepage::GetHarmManangerWidget() const
{
	return m_harmManagerWidget;
}
FFmpegUntil *LY_AgricultureHomepage::GetFFmpegUntil() const
{
	return m_ffmpegUntil;
}
void LY_AgricultureHomepage::setupWidgetLocal(QWidget *tempWidget, const char * _value, const char * _element)
{
	TiXmlDocument docElement;
	std::string filename = "../data/layoutConfig.xml";
	if (!docElement.LoadFile(filename.c_str()))
	{
		qDebug() << "open xml fail";
		return;
	}
	TiXmlElement* pRootEle = docElement.RootElement();
	if (!pRootEle)
	{
		return;
	}
	TiXmlElement* pRecordListEle = pRootEle->FirstChildElement(_element);
	if (!pRecordListEle)
	{
		return;
	}
	TiXmlElement* pRecordEle = pRecordListEle->FirstChildElement(_value);
	while (pRecordEle) {
		const char* _x = pRecordEle->Attribute("x");
		const char* _y = pRecordEle->Attribute("y");
		const char* _height = pRecordEle->Attribute("height");
		const char* _width = pRecordEle->Attribute("width");
		const char* _hidden = pRecordEle->Attribute("hidden");
		if (_x && _y && _height && _width && tempWidget != nullptr)
		{
			tempWidget->move(QPoint(std::atoi(_x), std::atoi(_y)));
			tempWidget->setFixedSize(std::atoi(_width), std::atoi(_height));
			std::string str(_hidden);
			if (str == "show")
			{
				tempWidget->show();
			}
			else {
				tempWidget->hide();
			}
		}
		pRecordEle = pRecordEle->NextSiblingElement(_value);
	}

}