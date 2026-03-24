
//!!!!!!!!
//!!!!!!!!注意：将本文件编码从utf-8改为ansi才能够使用vc2015编译通过，gcc可能不用更改
//!!!!!!!!


#include "Ly_VideoRealTimePreview.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif
#include "Ly_VideoRealTimePreviewInterface.h"
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>
//#include <QDb.h>
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"

#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/ITrack.h>
#include <map3d/IAirLine.h>
#include <map3d/IGraphics.h>

#include <QThreadPool>
#include <QApplication>

#include "IFunctionInterface.h"

#include "IVideoRealTimePreviewActivator.h"
#include "lyUI/LyDockManager.h"
#include <LyDockWidget.h>
#include "QCString.h"
#include "EntityVedioData.h"
#include "Ly_VideoRealTimePreviewWidget.h"
#include "VideoRealTimePreviewGraphicWidget.h"
#include "VedioRealTimeConfigureDlg.h"
#include<GetHeight.h>

static CGetHeight *s_pCGetHeight = nullptr;

void VideoRealTime::VideoRealTimeConfig::Sync()
{
	QString filePath = QApplication::applicationDirPath() + "/Data/conf/Ly_VideoRealTimePreview.ini";
	QSettings settings(filePath, QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("Config");
	showFlag = settings.value("ShowFlag", true).toBool();
	viewLocation = (EViewLocation)settings.value("ViewLoaction", EViewLocation::ViewBottom).toInt();
	useSortByPosAndYawFlag = settings.value("UseSortByPosAndYawFlag", false).toBool();
	dYaw = settings.value("Yaw", 0).toDouble();
	dLon = settings.value("Lon", 0).toDouble();
	dLat = settings.value("Lat", 0).toDouble();
	useFriendTrackFlag = settings.value("UseFriendTrackFlag", false).toBool();
	lObserverIdentity = settings.value("ObserverIdentity", -1).toLongLong();
	detailOccupyValue = settings.value("DetailOccupyValue", 0.25).toDouble();
	settings.endGroup();
}

void VideoRealTime::VideoRealTimeConfig::Save()
{
	QString filePath = QApplication::applicationDirPath() + "/Data/conf/Ly_VideoRealTimePreview.ini";
	QSettings settings(filePath, QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("Config");
	settings.setValue("ShowFlag", showFlag);
	settings.setValue("ViewLoaction", (int)viewLocation);
	settings.setValue("UseSortByPosAndYawFlag", useSortByPosAndYawFlag);
	settings.setValue("Yaw", dYaw);
	settings.setValue("Lon", dLon);
	settings.setValue("Lat", dLat);
	settings.setValue("UseFriendTrackFlag", useFriendTrackFlag);
	settings.setValue("ObserverIdentity", lObserverIdentity);
	settings.setValue("DetailOccupyValue", detailOccupyValue);
	settings.endGroup();
}

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)
Ly_VideoRealTimePreview:: Ly_VideoRealTimePreview()
{
	if (nullptr == s_pCGetHeight)
	{
		QString dir = "../data/dem/90";

		QSettings settings("Data/conf/FlightTaskManageSettings.ini", QSettings::IniFormat);
		settings.beginGroup("FlightTaskManageEditLineSettings");
		if (settings.contains("FlightTaskManageEditLineElevationDir"))
		{
			QString flightTaskManageEditLineElevationDirTemp = settings.value("FlightTaskManageEditLineElevationDir", "").toString();
			if (!flightTaskManageEditLineElevationDirTemp.isEmpty())
			{
				if (QDir(flightTaskManageEditLineElevationDirTemp).exists())
				{
					dir = flightTaskManageEditLineElevationDirTemp;
				}
			}
		}
		settings.endGroup();
		s_pCGetHeight = new CGetHeight(dir.toStdString().c_str());
	}
}

void Ly_VideoRealTimePreview::Startup()
{
	IFunctionInterface &ifi = Ly_VideoRealTimePreviewInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			IVideoRealTimePreviewActivator::Get().start(pIRegistry);
		}
	}
}

void Ly_VideoRealTimePreview::Shutdown()
{
	if (nullptr != m_VideoRealTimePreviewWidget)
	{
		delete m_VideoRealTimePreviewWidget;
		m_VideoRealTimePreviewWidget = nullptr;
	}
	EntityDataManage::Get().ClearAll();

	IFunctionInterface &ifi = Ly_VideoRealTimePreviewInterface::Get().GetFunctionInterface();
	QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
	if (vt.canConvert<qnzkna::framework::IRegistry *>())
	{
		qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
		if (pIRegistry != nullptr)
		{
			IVideoRealTimePreviewActivator::Get().stop(pIRegistry);
		}
	}
}

void Ly_VideoRealTimePreview::Reset()
{

}

VideoRealTime::VideoRealTimeConfig* Ly_VideoRealTimePreview::getConfig()
{
	return &m_config;
}

QMutex& Ly_VideoRealTimePreview::getConfigMutex()
{
	return m_configMutex;
}

long long Ly_VideoRealTimePreview::GetCurComponentID()
{
	long long curComponent = -1;
	{
		QMutexLocker locker(&(getConfigMutex()));
		curComponent = getConfig()->lObserverIdentity;
	}
	return curComponent;
}

void Ly_VideoRealTimePreview::NotifyConfigureChanged()
{
	if (m_VideoRealTimePreviewWidget != NULL)
	{
		m_VideoRealTimePreviewWidget->ConfigureChanged();
	}
}

double Ly_VideoRealTimePreview::GetHeight(double lon, double lat)
{
	double height = 0;
	if (s_pCGetHeight)
	{
		height = s_pCGetHeight->GetHeight(lon, lat);
	}

	return height;
}

void Ly_VideoRealTimePreview::onFocalZoomChanged(int imageZoom)
{
	if (m_VideoRealTimePreviewWidget != NULL)
		m_VideoRealTimePreviewWidget->FocalZoomChanged(imageZoom);
}

void Ly_VideoRealTimePreview::CameraAttributeChanged(long long componentID, double yawValue, double pitchValue, double rollValue)
{
	auto pSensorService = IVideoRealTimePreviewActivator::Get().getPISensorManageService();
	if (nullptr == pSensorService)
		return;

	if (componentID == GetCurComponentID() && m_VideoRealTimePreviewWidget != nullptr)
	{
		m_VideoRealTimePreviewWidget->OberverCameraAttributeChanged(
			componentID, yawValue, pitchValue, rollValue);
	}

	auto pWEntity = EntityDataManage::Get().FindEntity(componentID);
	auto pEntity = pWEntity.lock();
	if (nullptr != pEntity)
	{
		pEntity->SetCameraInfo(yawValue, pitchValue, rollValue);
	}
}

bool Ly_VideoRealTimePreview::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool Ly_VideoRealTimePreview::DrawDynamic(QPainter &painter, IProj *pProj)
{
	return false;
}

bool Ly_VideoRealTimePreview::OnMouseButtonDown(QMouseEvent *e)
{
	return false;
}

bool Ly_VideoRealTimePreview::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool Ly_VideoRealTimePreview::OnCommand(int nID)
{
    switch (nID)
    {
   
    default:
        break;
    }
    return false;
}

bool Ly_VideoRealTimePreview::OnCreate(int wParam, QWidget *pWidget)
{
	switch (wParam)
	{
	case 5:
	{
		m_p3DView = pWidget;
	}
	break;
	case 6:
	{
		m_p2DView = pWidget;
		if (m_VideoRealTimePreviewWidget == NULL)
		{
#ifdef USE_GRAPHICS_VIEW
			m_VideoRealTimePreviewWidget = new VideoRealTimePreviewGraphicWidget(m_p2DView);
#else
			m_VideoRealTimePreviewWidget = new Ly_VideoRealTimePreviewWidget(m_p2DView);
#endif // USE_GRAPHICS_VIEW
			m_VideoRealTimePreviewWidget->Init();
		}
	}
	break;
	case 7:
	{
		m_VideoRealTimePreviewWidget->setVisiableDependsWgt(pWidget);
	}
		break;
	case 0:
	{
		m_config.Sync();

		m_pWnd = pWidget;
		m_pDlg = new VedioRealTimeConfigureDlg(pWidget);
		m_pDlg->Init();
		m_pDlg->hide();
	}
	break;
	case 50024:
	{
		//if (nullptr != m_pDlg)
		//{
		//	if (m_pDlg->isVisible())
		//	{
		//		m_pDlg->hide();
		//	}
		//	else {
		//		m_pDlg->show();
		//	}
		//}
	}
	break;
	}
    return false;
}

bool Ly_VideoRealTimePreview::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1://200ms
        Ly_VideoRealTimePreviewInterface::Get().Refresh(1);
        break;
    case 2://1s
        break;
    case 3://60s
        break;
    }
    return true;
}

void Ly_VideoRealTimePreview::On2d3dChanged(int wParamID)
{
	if (nullptr == m_VideoRealTimePreviewWidget
		|| nullptr == m_p2DView
		|| nullptr == m_p3DView)
		return;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth->IsVisible())
	{
		m_VideoRealTimePreviewWidget->setMapWidget(m_p3DView);
		m_VideoRealTimePreviewWidget->Init();
	}
	else
	{
		m_VideoRealTimePreviewWidget->setMapWidget(m_p2DView);
		m_VideoRealTimePreviewWidget->Init();
	}
}

