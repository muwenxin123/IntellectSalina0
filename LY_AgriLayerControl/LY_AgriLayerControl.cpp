#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <osgEarth/MapNode>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarth/Viewpoint>
#include <osgViewer/Viewer>
#include <osg/Math>

#include <QDebug>

#include "LY_AgriLayerControl.h"
#include "LY_AgriLayerControlInterface.h"

#include "IProj.h"
#include "IFunctionInterface.h"
#include "lyUI/LyDockManager.h"

#include "IAgriLayerControlActivator.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)


LY_AgriLayerControl::LY_AgriLayerControl()
{
    IAgriRestfulManagerActivator::Get();
}

void LY_AgriLayerControl::Startup()
{
    IFunctionInterface &ifi = LY_AgriLayerControlInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriRestfulManagerActivator::Get().start(pIRegistry);
        }
    }
}

void LY_AgriLayerControl::Shutdown()
{
    IFunctionInterface &ifi = LY_AgriLayerControlInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IAgriRestfulManagerActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_AgriLayerControl::Reset()
{

}

bool LY_AgriLayerControl::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_AgriLayerControl::DrawDynamic(QPainter &painter, IProj *pProj)
{
	if (m_pOeLayerControl_Dialog)
	{
		m_pOeLayerControl_Dialog->drawDynamic(painter, pProj);
	}
    return true;
}

bool LY_AgriLayerControl::OnMouseButtonDown(QMouseEvent *e)
{
	return false;
}

bool LY_AgriLayerControl::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LY_AgriLayerControl::OnCommand(int nID, void *lParam )
{
	switch (nID)
	{
	case 33370:
	{
		if (m_pOeLayerControl_Dialog)
		{
			if (m_pOeLayerControl_Dialog->isHidden())
			{
				if (osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode)))
				{
					m_pOeLayerControl_Dialog->setMapNode(mapNode);
				}
				m_pOeLayerControl_Dialog->show();
			}
			else
			{
				m_pOeLayerControl_Dialog->hide();
			}
		}

		if (osgViewer::Viewer* viewer = (osgViewer::Viewer*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_OsgViewer)))
		{
			osgGA::CameraManipulator* cameraMain = viewer->getCameraManipulator();
			osgEarth::Util::EarthManipulator* manipulator = dynamic_cast<osgEarth::Util::EarthManipulator*>(cameraMain);
			if (manipulator)
			{
				osgEarth::Viewpoint vp = manipulator->getViewpoint();

				double lon = vp.focalPoint().get().x();  // 经度
				double lat = vp.focalPoint().get().y();  // 纬度
				double alt = vp.focalPoint().get().z();  // 海拔高度

				double heading = vp.getHeading();     // 方位角（度）
				double pitch = vp.getPitch();         // 俯仰角（度）
				double range = vp.getRange();         // 距离（米）

				qDebug() << "当前视角：\n";
				qDebug() << "经度: " << lon << "\n";
				qDebug() << "纬度: " << lat << "\n";
				qDebug() << "高度: " << alt << "\n";
				qDebug() << "方位角: " << heading << "\n";
				qDebug() << "俯仰角: " << pitch << "\n";
				qDebug() << "距离: " << range << "\n";
			}
			else 
			{
				osg::Vec3d eye, center, up;
				viewer->getCamera()->getViewMatrixAsLookAt(eye, center, up);

				//std::cout << "Eye Position: " << eye.x() << ", " << eye.y() << ", " << eye.z() << std::endl;
				//std::cout << "Center Position: " << center.x() << ", " << center.y() << ", " << center.z() << std::endl;
				//std::cout << "Up Vector: " << up.x() << ", " << up.y() << ", " << up.z() << std::endl;

				// 假设 mapSRS 是你的地图坐标系（通常是 WGS84 地理坐标系）
				const osgEarth::SpatialReference* mapSRS = osgEarth::SpatialReference::get("wgs84");

				double lon, lat, height;
				mapSRS->getEllipsoid()->convertXYZToLatLongHeight(
					eye.x(), eye.y(), eye.z(), lat, lon, height);

				// 弧度转度
				lon = osg::RadiansToDegrees(lon);
				lat = osg::RadiansToDegrees(lat);

				qDebug() << u8"经度: " << lon << u8" 纬度: " << lat << u8" 高度: " << height;

				// 计算视线方向
				osg::Vec3d lookVec = center - eye;
				lookVec.normalize();

				double heading = osg::RadiansToDegrees(atan2(lookVec.x(), lookVec.y()));
				if (heading < 0.0) heading += 360.0;  // 保证范围 0~360°
				qDebug() << u8"方位角（heading）: " << heading << u8"°";

				double horizontalLength = sqrt(lookVec.x() * lookVec.x() + lookVec.y() * lookVec.y());
				double pitch = osg::RadiansToDegrees(atan2(-lookVec.z(), horizontalLength));
				qDebug() << u8"俯仰角（pitch）: " << pitch << u8"°";

				// 计算相机距离（range）
				double range = (center - eye).length();
				qDebug() << u8"距离（range）: " << range;
			}
		}
	}
	break;
	default:
	break;
	}
	return false;
}

bool LY_AgriLayerControl::OnCreate(int wParam, QWidget *pWgt)
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
	case 1000:
	{
		if (m_pOeLayerControl_Dialog == nullptr)
		{
			m_pOeLayerControl_Dialog = new oeLayerControl_Dialog();
			m_pOeLayerControl_Dialog->move(0, -755);
			m_pOeLayerControl_Dialog->hide();
			m_pOeLayerControl_Dialog->OnCreate();
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

bool LY_AgriLayerControl::OnTimer(int wParam)
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

bool LY_AgriLayerControl::OnNetRecv(int nLength, char *lpData)
{
    return false;
}

void LY_AgriLayerControl::OnEarthMessage(int wParam, void *lParam)
{
	return;
}

bool LY_AgriLayerControl::On2d3dChanged(int wParam)
{
	return false;
}

void LY_AgriLayerControl::SetScreenCenter(double dLon, double dLat)
{
	return;
}
