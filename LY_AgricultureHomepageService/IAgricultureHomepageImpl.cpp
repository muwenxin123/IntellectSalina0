
#include "IAgricultureHomepageImpl.h"
#include "LY_AgricultureHomepageService.h"
#include <QVariant>
#include <memory>
#include <QSettings>
#include <QSqlError>
#include "QSqlQuery"
#include <QDebug>
#include <QSqlError>
#include <QPainter>
#include "IProj.h"
#include "QCString.h"

#include <QFile>
#include "GLES3/gl3.h"
#include "qmath.h"
#include "CreateShape.h"

#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/GeoTransform>
#include <osgEarth/SpatialReference>

#include <osgFX/Scribe>
#include <osgFX/Outline>
#include <osg/LineWidth>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <osg/Node>
#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Texture2D>
#include <osg/MatrixTransform> 
#include <osg/NodeCallback> 
#include <osg/DrawPixels> 
#include <osg/ShapeDrawable>
#include <osg/GraphicsContext>
#include <osg/ComputeBoundsVisitor>
#include <osg/PositionAttitudeTransform>

#include <osgGA/TrackballManipulator>
#include <osgGA/StateSetManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/GraphicsWindow>
#include <osgViewer/ViewerEventHandlers> 
#include <osgViewer/CompositeViewer> 
#include <osgParticle/PrecipitationEffect> 

#include <osgManipulator/TranslateAxisDragger>
#include<osgManipulator/TabBoxDragger>
#include<osgManipulator/TrackballDragger>

#pragma comment(lib, "OpenThreadsd.lib")
#pragma comment(lib, "osgd.lib")
#pragma comment(lib, "osgDBd.lib")
#pragma comment(lib, "osgUtild.lib")
#pragma comment(lib, "osgGAd.lib")
#pragma comment(lib, "osgViewerd.lib")
#pragma comment(lib, "osgTextd.lib")
#pragma comment(lib, "osgSimd.lib")
#pragma comment(lib, "osgFXd.lib")

#include <osgManipulator/CommandManager>
#include <osgManipulator/Dragger>
#include <osg/NodeVisitor>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>

#include <map>
#include <unordered_map>

using namespace qnzkna::IAgricultureHomePage;

#define PI						3.1415926
#define RELATE_HEIGHT			2.0
#define PIANZHUAN				0.0

std::unordered_map<std::string, osg::ref_ptr<osg::MatrixTransform>>				m_mGrop;
std::unordered_map<std::string, SensorListModel>								m_mSensorMap;

osg::ref_ptr<osg::Group> groupSensor;

//标牌连接线
std::vector<osg::ref_ptr<osg::Group>> m_allLines;

//飞机标牌
std::vector<IText *>                     m_map3DTextVec;


#define M_PI (3.14159265358979323846)
// 常量
const double EARTH_RADIUS = 6371000.0;  // 地球半径，单位米
const double R = 6371000.0; // 地球平均半径(米)

// 创建线段几何体
osg::Geometry* createLineGeometry(const osg::Vec3d& start, const osg::Vec3d& end,
	const osg::Vec4& color, float width)
{
	// 创建顶点数组
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array;
	vertices->push_back(start);
	vertices->push_back(end);

	// 创建几何体
	osg::ref_ptr<osg::Geometry> geometry = new osg::Geometry;
	geometry->setVertexArray(vertices);

	// 设置绘制线段（不是折线）
	geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));

	// 设置颜色
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(color);
	geometry->setColorArray(colors, osg::Array::BIND_OVERALL);  // 所有顶点使用相同颜色

																// 设置线宽
	osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth(width);
	geometry->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

	return geometry.release();
}


osg::Geometry* createDirectLine(
	osgEarth::GeoPoint& start,
	osgEarth::GeoPoint& end)
{
	// 转换为世界坐标
	osg::Vec3d startWorld, endWorld;
	start.toWorld(startWorld);
	end.toWorld(endWorld);

	// 创建几何体
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::Vec3Array* vertices = new osg::Vec3Array();
	vertices->push_back(startWorld);
	vertices->push_back(endWorld);
	geom->setVertexArray(vertices);

	// 设置颜色
	osg::Vec4Array* colors = new osg::Vec4Array();
	colors->push_back(osg::Vec4(0.357f, 0.859f, 0.965f, 1.0f));
	geom->setColorArray(colors, osg::Array::BIND_OVERALL);

	// 设置线宽
	osg::LineWidth* linewidth = new osg::LineWidth();
	linewidth->setWidth(1.5f);
	geom->getOrCreateStateSet()->setAttributeAndModes(linewidth, osg::StateAttribute::ON);

	// 添加图元
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));

	//osg::Geode* geode = new osg::Geode();
	//geode->addDrawable(geom);

	osg::Billboard* billboard = new osg::Billboard;
	billboard->setMode(osg::Billboard::POINT_ROT_EYE);  // 始终朝向相机
	billboard->addDrawable(geom);

	return geom.release();
}


// 将角度转换为弧度
double toRadians(double degrees) {
	return degrees * M_PI / 180.0;
}

// 将弧度转换为角度
double toDegrees(double radians) {
	return radians * 180.0 / M_PI;
}

// 计算目标点的经纬度和高程
void calculateNewPosition(double lat0, double lon0, double h0, double distance,
	double azimuth, double elevation, double& lat1, double& lon1, double& h1) {
	// 转换为弧度
	double lat0_rad = toRadians(lat0);
	double lon0_rad = toRadians(lon0);
	double azimuth_rad = toRadians(azimuth);
	double elevation_rad = toRadians(elevation);

	// 计算起点在地心坐标系中的位置
	double x0 = (R + h0) * cos(lat0_rad) * cos(lon0_rad);
	double y0 = (R + h0) * cos(lat0_rad) * sin(lon0_rad);
	double z0 = (R + h0) * sin(lat0_rad);

	// 计算目标点相对起点在局部坐标系中的偏移
	double n = distance * cos(elevation_rad) * cos(azimuth_rad); // 北方向分量
	double e = distance * cos(elevation_rad) * sin(azimuth_rad); // 东方向分量
	double u = distance * sin(elevation_rad);                   // 垂直方向分量

																// 将局部坐标系偏移转换为地心坐标系
	double dx = -sin(lon0_rad) * e - cos(lon0_rad) * sin(lat0_rad) * n + cos(lon0_rad) * cos(lat0_rad) * u;
	double dy = cos(lon0_rad) * e - sin(lon0_rad) * sin(lat0_rad) * n + sin(lon0_rad) * cos(lat0_rad) * u;
	double dz = cos(lat0_rad) * n + sin(lat0_rad) * u;

	// 计算目标点在地心坐标系中的位置
	double x1 = x0 + dx;
	double y1 = y0 + dy;
	double z1 = z0 + dz;

	// 计算目标点的经纬度和高程
	double r = sqrt(x1*x1 + y1*y1 + z1*z1);
	lat1 = toDegrees(asin(z1 / r));
	lon1 = toDegrees(atan2(y1, x1));
	h1 = r - R;
}

void draw3DFlyLabel(std::string modelName, double lon, double lat, double alt)
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (!mapNode) return;

	// 删除所有线
	for (auto& line : m_allLines) {
		mapNode->removeChild(line);
	}
	m_allLines.clear();

	double distance = 2.5;
	double azimuth = 90.0;
	double elevation = 60.0;

	// 创建组节点并添加线段
	//osg::ref_ptr<osg::Group> lineGroup = new osg::Group;
	////lineGroup->addChild(line);

	//mapNode->addChild(lineGroup);
	//m_allLines.push_back(lineGroup);

	if (pEarth->GetGraphics() == nullptr || pEarth == nullptr) return;

	auto& itrMap = m_mSensorMap.find(modelName);
	if (itrMap == m_mSensorMap.end()) return;

	auto& sensorModel = itrMap->second;
	QString str = "";
	//std::string ID = u8"编号:";
	std::string type = u8"  类型:" + sensorModel.mointType.toStdString();
	str += "\n" + QString::fromStdString(type) + "\n";

	switch (static_cast<SensorType>(sensorModel.typeID))
	{
	case SensorType::SensorTypeType_Video:
	{
	}
	break;
	case SensorType::SensorTypeType_Soil:
	{
		std::string soli_tem = u8"  土壤温度: 24.6℃";
		std::string soli_hum = u8"  土壤湿度: 22.8%RH";
		std::string soli_ph = u8"  土壤PH: 5.1";
		std::string soli_ec = u8"  土壤EC: 141.0us/cm";
		std::string soli_n = u8"  氮: 30.0mg/kg";
		std::string soli_p = u8"  磷: 117.0mg/kg";
		std::string soli_k = u8"  钾: 110.0mg/kg";

		str += QString::fromStdString(soli_tem) + "\n"
			+ QString::fromStdString(soli_hum) + "\n"
			+ QString::fromStdString(soli_ph) + "\n"
			+ QString::fromStdString(soli_ec) + "\n"
			+ QString::fromStdString(soli_n) + "\n"
			+ QString::fromStdString(soli_p) + "\n"
			+ QString::fromStdString(soli_k) + "\n";

		distance = 5.0;
		azimuth = 90.0;
		elevation = 60.0;
	}
	break;
	case SensorType::SensorTypeType_Weather:
	{
		std::string soli_tem = u8"  温度: 29.1℃";
		std::string soli_hum = u8"  湿度: 32.8%RH";
		std::string soli_ph = u8"  风速: 2.1m/s";
		std::string soli_ec = u8"  风向: 135°";
		std::string soli_n = u8"  降水量: 2.1mm";
		std::string soli_p = u8"  太阳总辐射: 120.5W/m2";
		std::string soli_k = u8"  大气压力: 1005.2hpa";

		str += QString::fromStdString(soli_tem) + "\n"
			+ QString::fromStdString(soli_hum) + "\n"
			+ QString::fromStdString(soli_ph) + "\n"
			+ QString::fromStdString(soli_ec) + "\n"
			+ QString::fromStdString(soli_n) + "\n"
			+ QString::fromStdString(soli_p) + "\n"
			+ QString::fromStdString(soli_k) + "\n";

		distance = 5.0;
		azimuth = 90.0;
		elevation = 60.0;
	}
	break;
	case SensorType::SensorTypeType_PestMonitor:
	{
	}
	break;
	case SensorType::SensorTypeType_KillInsects:
	{
	}
	break;
	case SensorType::SensorTypeType_Valve:
	{
	}
	break;
	case SensorType::SensorTypeType_FarmCar:
	{
	}
	break;
	case SensorType::SensorTypeType_FlyStop:
	{
	}
	break;

	default:
		break;
	}

	double endLat, endLon, endHeight;
	calculateNewPosition(lat, lon, alt, distance, azimuth, elevation, endLat, endLon, endHeight);

	const osgEarth::SpatialReference* wgs84 = mapNode->getMapSRS()->getGeographicSRS();
	osgEarth::GeoPoint startPoint(wgs84, lon, lat, alt, osgEarth::ALTMODE_ABSOLUTE);
	osgEarth::GeoPoint endPoint(wgs84, endLon, endLat, endHeight, osgEarth::ALTMODE_ABSOLUTE);
	osg::Geometry* line = createDirectLine(startPoint, endPoint);

	IText* mshpText = pEarth->GetGraphics()->CreateText(str.toLocal8Bit(), endLon, endLat, endHeight, NULL, 12,
		TRGB(255, 255, 255, 255), TRGB(11, 25, 60, 128), TRGB(91, 219, 64, 255), 4);

	m_map3DTextVec.push_back(mshpText);
}


class PickModelHandler :public osgGA::GUIEventHandler
{
public:
	PickModelHandler::PickModelHandler() :_activeDragger(0) {
		_pointer = new osgManipulator::PointerInfo;
	}

	PickModelHandler::~PickModelHandler() {}

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*> (&aa);
		if (!viewer)return false;

		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::PUSH: // 鼠标按下
		{
			_pointer->reset();

			// 创建一个线段交集检测函数
			osgUtil::LineSegmentIntersector::Intersections intersections;
			osg::ref_ptr<osg::Node> targetNode = new osg::Node();
			osg::ref_ptr<osg::Group> parent = new osg::Group();

			if (viewer->computeIntersections(ea.getX(), ea.getY(), intersections))
			{
				{
					_pointer->setCamera(viewer->getCamera());
					_pointer->setMousePosition(ea.getX(), ea.getY());

					osgUtil::LineSegmentIntersector::Intersections::iterator hitr;//线段访问器
					for (hitr = intersections.begin(); hitr != intersections.end(); ++hitr) {
						_pointer->addIntersection(hitr->nodePath, hitr->getLocalIntersectPoint());//添加线段信息
					}
					osg::NodePath::iterator itr;
					for (itr = _pointer->_hitList.front().first.begin();
						itr != _pointer->_hitList.front().first.end();
						++itr) {
						osgManipulator::Dragger* dragger = dynamic_cast<osgManipulator::Dragger*>(*itr);//获取输入参数 Dragger
						if (dragger) {
							dragger->handle((*_pointer), ea, aa);
							_activeDragger = dragger;
							break;
						}
					}
				}

				// 获取相交模型点的节点
				osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
				osg::NodePath& nodePath = intersection.nodePath;
				targetNode = nodePath.back();

				// 判定所属的分组
				for (auto & path : nodePath)
				{
					if (path->getName() == "groupSensor")
					{
						// 点击节点切换高亮
						parent = dynamic_cast<osg::Group*> (nodePath[nodePath.size() - 2]); // 当前选择节点的父节点
						osgFX::Outline* ot = dynamic_cast<osgFX::Outline*>(parent.get());
						//若ot不存在（未高亮） (node->parent)=>(node->outline->parent)
						if (!ot)
						{
							osg::ref_ptr<osgFX::Outline> outline = new osgFX::Outline();
							outline->setColor(osg::Vec4(1, 1, 0, 1));
							outline->setWidth(5);
							outline->addChild(targetNode);
							parent->replaceChild(targetNode, outline);

							if (IEarth *pEarth = IEarth::GetEarth())
							{
#if 1
								osg::Vec3d worldPos = intersection.getWorldIntersectPoint();
								osgEarth::SpatialReference* mapSRS = osgEarth::SpatialReference::get("wgs84");
								// 输出经纬度（地理坐标）
								osgEarth::GeoPoint mapPoint;
								mapPoint.fromWorld(mapSRS, worldPos);

								//const char * rtspPath = "rtsp://192.168.1.30/videodevice";
								//ILabel * video = pEarth->GetGraphics()->CreateLabel("", "rtsp://192.168.1.30/videodevice", mapPoint.x(), mapPoint.y(), mapPoint.z() + 3.00, 400 * 0.5, 300 * 0.5, 0, NULL, 22, TRGB(128, 255, 255, 255), 0);
								//m_map3DVideo.insert(std::make_pair(targetNode->getName(), video));

								//QString strDataDir = tr2(LY_AgricultureHomepageService::Get().GetDataDir().data());
								//QString strPath = strDataDir + "Temp_Line/monitor_background.png";

								//ILabel * label = pEarth->GetGraphics()->CreateLabel("", strPath.toStdString().c_str(), mapPoint.x(), mapPoint.y(), mapPoint.z() + 3.1, 400 * 0.5, 300 * 0.5, 0, NULL, 22, TRGB(128, 255, 255, 255), 100);
								//m_map3DVideo.insert(std::make_pair(targetNode->getName() + "_label", label));

#else
								auto &targetPosXYZ = osg::Vec3d((targetNode->getBound().center()* osg::computeLocalToWorld(targetNode->getParentalNodePaths()[0])).x(),
									(targetNode->getBound().center()* osg::computeLocalToWorld(targetNode->getParentalNodePaths()[0])).y(),
									(targetNode->getBound().center()* osg::computeLocalToWorld(targetNode->getParentalNodePaths()[0])).z());
								pEarth->GetGraphics()->CreateLabel("video", "rtsp://192.168.1.30/videodevice", targetPosXYZ.x(), targetPosXYZ.y(), targetPosXYZ.z() + 3.00, 100, 100, 0);
#endif

								auto& modelNode = nodePath[nodePath.size() - 4];
								if (modelNode)
								{
									draw3DFlyLabel(modelNode->getName(), mapPoint.x(), mapPoint.y(), mapPoint.z() + 1.0);
								}
							}
						}
						//若ot存在（高亮）找出当前outline的父节点（node->outline->*itr）=>(node->*itr)
						else
						{
							osg::Node::ParentList parentList = ot->getParents();
							osg::Node::ParentList::iterator itr = parentList.begin();
							(*itr)->replaceChild(ot, targetNode);

							auto& itrMap = m_map3DVideo.find(targetNode->getName());
							if (itrMap != m_map3DVideo.end()) {
								IEarth *pEarth = IEarth::GetEarth();
								pEarth->GetGraphics()->RemoveObject(itrMap->second);
								m_map3DVideo.erase(itrMap);
							}

							auto& itrMap2 = m_map3DVideo.find(targetNode->getName() + "_label");
							if (itrMap2 != m_map3DVideo.end()) {
								IEarth *pEarth = IEarth::GetEarth();
								pEarth->GetGraphics()->RemoveObject(itrMap2->second);
								m_map3DVideo.erase(itrMap2);
							}

							// 清理标牌
							auto itr2 = m_map3DTextVec.begin();
							for (; itr2 != m_map3DTextVec.end(); itr2++)
							{
								IEarth *pEarth = IEarth::GetEarth();
								pEarth->GetGraphics()->RemoveObject(*itr2);
								*itr2 = nullptr;
								delete *itr2;
							}
							m_map3DTextVec.clear();
						}

						break;
					}
				}
			}
			else
			{
				std::cout << "select nothing!" << std::endl;
			}
			break;
		}
		case osgGA::GUIEventAdapter::DRAG:
		case osgGA::GUIEventAdapter::RELEASE: {
			if (_activeDragger) {
				_pointer->_hitIter = _pointer->_hitList.begin();
				_pointer->setCamera(viewer->getCamera());
				_pointer->setMousePosition(ea.getX(), ea.getY());
				_activeDragger->handle((*_pointer), ea, aa);
			}
			if (ea.getEventType() == osgGA::GUIEventAdapter::RELEASE) {
				_activeDragger = NULL;
				_pointer->reset();
			}
			break;
		}
		default:
			break;
		}
		return false;
	}

private:
	std::map<std::string, ILabel *>					m_map3DVideo;
	osgManipulator::Dragger*						_activeDragger;
	osgManipulator::PointerInfo*					_pointer;//拖拽器 输入的信息集合 Dragger类种的handler参数
};

IAgricultureHomepageImpl::IAgricultureHomepageImpl()
{
	InitDataBase();
	QString fileName = LOCAL_PATH_MOINTOR;
	QByteArray byteArray = fileName.toUtf8();
	char* charStr = byteArray.data(); // 获取char*指针
	tinyXmlDoc = new TiXmlDocument(charStr);
	tinyXmlDoc->LoadFile(TIXML_ENCODING_LEGACY);
	clearLocalFile();
}

void IAgricultureHomepageImpl::InitDataBase()
{
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Database");
	QString strDbName = "Agri"; // TODO: 配置项
	QString strDbIP = ini.value("SvrIP", "192.168.1.115").toString();
	QString strDbUser = ini.value("User", "root").toString();
	QString strDbPass = ini.value("Password", "123456").toString();
	QString strDbDriver = ini.value("Driver", "QMYSQL").toString();

	db = QSqlDatabase::addDatabase(strDbDriver);
	db.setHostName(strDbIP);
	db.setPort(3306);
	db.setDatabaseName(strDbName);
	db.setUserName(strDbUser);
	db.setPassword(strDbPass);
	isOpen = db.open();
}
AgriFarmInfoVector IAgricultureHomepageImpl::GetFarmInfoList() const
{
	AgriFarmInfoVector 	farmInfoPtrVec;
	if (!isOpen)
	{
		return farmInfoPtrVec;
	}
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	farmInfoPtrVec.clear();

	QString strSql = "SELECT `id`,  `name`, `category`, `description`, `lon`, `lat`";
	QString strGeneralSql = strSql + " FROM " + "agri_farm";

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			QString str4 = query.value(3).toString();
			QString str5 = query.value(4).toString();
			QString str6 = query.value(5).toString();

			AgriFarmInfoModel pInfo;
			pInfo.id = str1;
			pInfo.name = str2;
			pInfo.category = str3;
			pInfo.description = str4;
			pInfo.lon = str5.toDouble();
			pInfo.lat = str6.toDouble();

			farmInfoPtrVec.push_back(pInfo);
		}
	}

	return farmInfoPtrVec;
}

AgriAgriSensorGroupModelVec IAgricultureHomepageImpl::GetSensorGroupList(QString &farm_id)
{
	AgriAgriSensorGroupModelVec vec;
	if (!isOpen)
	{
		return vec;
	}

	QMutexLocker locker(&sMutex);

	QSqlQuery query;
	QString strSql = "SELECT `id`, `farm_id`, `name`";
	QString strGeneralSql = strSql + " FROM " + "agri_mointor_group" + " WHERE farm_id = \"" + farm_id + "\";";

	m_group_list.clear();

	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			AgriSensorGroupModel pInfo;
			pInfo.id = str1;
			pInfo.farm_id = str2;
			pInfo.name = str3;
			vec.push_back(pInfo);
		}
	}
	m_group_list = vec;
	return vec;
}

bool IAgricultureHomepageImpl::AddSensorGroup(AgriSensorGroupModel &fieldInfo)
{
	if (!isOpen)
	{
		return false;
	}
	QMutexLocker locker(&sMutex);
	bool isSuc = false;
	for (auto &model : m_group_list) {
		if (model.name == fieldInfo.name)
		{
			isSuc = true;
			break;
		}
	}
	if (!isSuc)
	{
		QString strSql("INSERT INTO agri_mointor_group (`id`, `farm_id`, `name`) VALUES (");
		strSql += "'" + fieldInfo.id + "',";
		strSql += "'" + fieldInfo.farm_id + "',";
		strSql += "'" + fieldInfo.name + "'";
		strSql += ")";
		QSqlQuery query;
		bool success = query.exec(strSql);
		if (!success) {
			QSqlError error = query.lastError();
			qDebug() << "SQL error:" << error.text();
			qDebug() << "Database error code:" << error.nativeErrorCode();
		}
		return success;
	}
	return isSuc;
}
 bool  IAgricultureHomepageImpl::DeleteSensorGroup()
{
	 if (!isOpen)
	 {
		 return false;
	 }
	 QMutexLocker locker(&sMutex);
	 QString strSql("DELETE FROM agri_mointor_group");
	 QSqlQuery query;
	 bool success = query.exec(strSql);
	 return success;
}
AgriFieldInfoModelVec IAgricultureHomepageImpl::GetFieldInfoList(QString &farm_id) 
{
	AgriFieldInfoModelVec fieldInfoPtrVec;
	if (!isOpen)
	{
		return fieldInfoPtrVec;
	}
	QMutexLocker locker(&sMutex);

	QSqlQuery query;

	QString strSql = "SELECT `id`, `farm_id`, `area_id`, `name`, `category`, `description`, `crop`, `crop_variety`, `plant_standard`, `sow_model`";
	QString strGeneralSql = strSql + " FROM " + "agri_farm_field" + " WHERE farm_id = \"" + farm_id + "\";";

	// TODO:
	//m_sensorListMap.clear();
	m_origin_sensor_list_map.clear();
	bool success = query.exec(strGeneralSql);
	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			QString str4 = query.value(3).toString();
			QString str5 = query.value(4).toString();
			QString str6 = query.value(5).toString();
			QString str7 = query.value(6).toString();
			QString str8 = query.value(7).toString();
			QString str9 = query.value(8).toString();
			QString str10 = query.value(9).toString();

			AgriFieldInfoModel pInfo;
			pInfo.id = str1;
			pInfo.farm_id = str2;
			pInfo.area_id = str3;
			pInfo.name = str4;
			pInfo.category = str5;
			pInfo.description = str6;
			pInfo.crop = str7.toInt();
			pInfo.crop_variety = str8.toInt();
			pInfo.plant_standard = str9.toInt();
			pInfo.sow_model = str10.toInt();

			fieldInfoPtrVec.push_back(pInfo);
		}
	}
	return fieldInfoPtrVec;
}
SensorTypeModelVec IAgricultureHomepageImpl::GetSensorTypeList(QString &field_id) 
{
	SensorTypeModelVec vec;
	if (!isOpen)
	{
		return vec;
	}
	 QMutexLocker locker(&sMutex);

	 QSqlQuery query;

	 QString strSql = "SELECT `id`, `name`, `field_id`, `type`";
	 QString strGeneralSql = strSql + " FROM `agri_monitor_list`" + " WHERE field_id = \"" + field_id + "\";";

	
	 vec.clear();
	 bool success = query.exec(strGeneralSql);
	 if (success)
	 {
	 	while (query.next())
	 	{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			QString str4 = query.value(3).toString();

			SensorTypeModel model;
			model.id = str1;
			model.name = str2;
			model.field_id = str3;
			model.type = str4.toInt();
			vec.push_back(model);
	 	}
	 }
	 return vec;
}
 bool IAgricultureHomepageImpl::AddSensorType(SensorTypeModel &model) 
 {
	 if (!isOpen)
	 {
		 return false;
	 }
	 QMutexLocker locker(&sMutex);
	 QSqlQuery query;
	 QString strSql("INSERT INTO agri_monitor_list (`id`, `name`, `field_id`, `type`) VALUES (");
	 strSql += "'" + model.id + "',";
	 strSql += "'" + model.name + "',";
	 strSql += "'" + model.field_id + "',";
	 strSql += "'" + QString::number(model.type) + "'";
	 strSql += ")";

	 bool success = query.exec(strSql);
	 if (!success) {
		 QSqlError error = query.lastError();
		 qDebug() << "SQL error:" << error.text();
		 qDebug() << "Database error code:" << error.nativeErrorCode();
	 }

	 return success;

 }
 bool IAgricultureHomepageImpl::DeleteSensorType(SensorTypeModel &model) 
 {
	 if (!isOpen)
	 {
		 return false;
	 }
	 QMutexLocker locker(&sMutex);
	 QString strSql("DELETE FROM agri_monitor_list where id = '");
	 strSql += model.id + "'";
	 QSqlQuery query;
	 bool success = query.exec(strSql);
	 if (success)
	 {
		 //locker.unlock();
		 //SensorListModelVec vec = m_sensorListMap[model.id];
		 //for (auto &model : vec) {
			// UpdateSensor(model);
		 //};
	 }
	 return success;
 }
  SensorListModelVec IAgricultureHomepageImpl::GetSensorListWithFieldID(QString &FieldID)
 {
	 SensorListModelVec vec;
	 if (!isOpen)
	 {
		 return vec;
	 }
	 QMutexLocker locker(&sMutex);
	 QSqlQuery query;
	 QString strSql = "SELECT `id`, `mointName`, `mointID`, `mointType`, `mointModenlName`, `localHeight`, `longitude`, `latitude`, `url`,`yj`,`fw`,`spsj`,`czsj`,`typeID`,`screenX`,`screenY`,`sensor_type_id`,`origin_sensor_type_id`,`origin_longitude`,`origin_latitude`,`field_id`";
	 QString strGeneralSql = strSql + " FROM `agri_monitor_sub_list`" + " WHERE field_id = \"" + FieldID + "\";";

	 bool success = query.exec(strGeneralSql);
	 vec.clear();
	 if (success)
	 {

		 while (query.next())
		 {
			 QString str1 = query.value(0).toString();
			 QString str2 = query.value(1).toString();
			 QString str3 = query.value(2).toString();
			 QString str4 = query.value(3).toString();
			 QString str5 = query.value(4).toString();
			 QString str6 = query.value(5).toString();
			 QString str7 = query.value(6).toString();
			 QString str8 = query.value(7).toString();
			 QString str9 = query.value(8).toString();
			 QString str10 = query.value(9).toString();
			 QString str11 = query.value(10).toString();
			 QString str12 = query.value(11).toString();
			 QString str13 = query.value(12).toString();
			 QString str14 = query.value(13).toString();
			 QString str15 = query.value(14).toString();
			 QString str16 = query.value(15).toString();
			 QString str17 = query.value(16).toString();
			 QString str18 = query.value(17).toString();
			 QString str19 = query.value(18).toString();
			 QString str20 = query.value(19).toString();
			 QString str21 = query.value(20).toString();

			 SensorListModel m;
			 m.id = str1.toInt();
			 m.mointName = str2;
			 m.mointID = str3;
			 m.mointType = str4;
			 m.mointModenlName = str5;
			 m.localHeight = str6.toDouble();
			 m.longitude = str7.toDouble();
			 m.latitude = str8.toDouble();
			 m.requsetUrl = str9;
			 m.yj = str10.toInt();
			 m.fw = str11.toInt();
			 m.spsj = str12.toInt();
			 m.czsj = str13.toInt();
			 m.typeID = str14.toInt();
			 //m.screenX = str15.toDouble();
			 //m.screenY = str16.toDouble();
			 m.sensor_type_id = str17;
			 m.origin_sensor_type_id = str18;
			 m.origin_longitude = str19.toDouble();
			 m.origin_latitude = str20.toDouble();
			 m.field_id = str21;

			 vec.push_back(m);
		 }
	 }

	 return vec;
 }
 SensorListModelVec IAgricultureHomepageImpl::GetSensorList(QString &sensorTypeID) 
 {
	 SensorListModelVec vec;
	 if (!isOpen)
	 {
		 return vec;
	 }
	 QMutexLocker locker(&sMutex);
	 QSqlQuery query;
	 QString strSql = "SELECT `id`, `mointName`, `mointID`, `mointType`, `mointModenlName`, `localHeight`, `longitude`, `latitude`, `url`,`yj`,`fw`,`spsj`,`czsj`,`typeID`,`screenX`,`screenY`,`sensor_type_id`,`origin_sensor_type_id`,`origin_longitude`,`origin_latitude`,`field_id`";
	 QString strGeneralSql = strSql + " FROM `agri_monitor_sub_list`" +" WHERE sensor_type_id = \"" + sensorTypeID + "\";";

	 bool success = query.exec(strGeneralSql);

	
	 vec.clear();
	 //m_map3DImage.clear();
	// m_sensorListVec.clear();
	 if (success)
	 {

		 while (query.next())
		 {
			 QString str1 = query.value(0).toString();
			 QString str2 = query.value(1).toString();
			 QString str3 = query.value(2).toString();
			 QString str4 = query.value(3).toString();
			 QString str5 = query.value(4).toString();
			 QString str6 = query.value(5).toString();
			 QString str7 = query.value(6).toString();
			 QString str8 = query.value(7).toString();
			 QString str9 = query.value(8).toString();
			 QString str10 = query.value(9).toString();
			 QString str11 = query.value(10).toString();
			 QString str12 = query.value(11).toString();
			 QString str13 = query.value(12).toString();
			 QString str14 = query.value(13).toString();
			 QString str15 = query.value(14).toString();
			 QString str16 = query.value(15).toString();
			 QString str17 = query.value(16).toString();
			 QString str18 = query.value(17).toString();
			 QString str19 = query.value(18).toString();
			 QString str20 = query.value(19).toString();
			 QString str21 = query.value(20).toString();

			 SensorListModel m;
			 m.id = str1.toInt();
			 m.mointName = str2;
			 m.mointID = str3;
			 m.mointType = str4;
			 m.mointModenlName = str5;
			 m.localHeight = str6.toDouble();
			 m.longitude = str7.toDouble();
			 m.latitude = str8.toDouble();
			 m.requsetUrl = str9;
			 m.yj = str10.toInt();
			 m.fw = str11.toInt();
			 m.spsj = str12.toInt();
			 m.czsj = str13.toInt();
			 m.typeID = str14.toInt();
			 //m.screenX = str15.toDouble();
			 //m.screenY = str16.toDouble();
			 m.sensor_type_id = str17;
			 m.origin_sensor_type_id = str18;
			 m.origin_longitude = str19.toDouble();
			 m.origin_latitude = str20.toDouble();
			 m.field_id = str21;

			 vec.push_back(m);
		 }
	 }
	 //writeStructLocalFile(vec);
	 m_sensorListMap[sensorTypeID] = vec;
	 m_origin_sensor_list_map[sensorTypeID] = vec;

	 if (vec.size() == 0)
	 {
		 return vec;
	 }
	 for (auto &model : vec) {
		 addStructLocalFile(model);
		 clearSingle3DMointerImg(model);
	 }
	 for (auto &model : vec) {
		 clearn3DMap(model);
		// clearnMointer3dText(model);
	 }

	
	 for (auto &model : vec) {
		
		 if (model.sensor_type_id == "unknow")
		 {
			 continue;
		 }
		 draw3DMointorImg(model);
		 //drawMointer3dText(model);
		 if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Video)
		 {
			 casic_vector3d curPos(model.longitude, model.latitude, model.localHeight + RELATE_HEIGHT);

			 double spsj = 30 * qTan(model.spsj / 2 * M_PI / 180) * 2;
			 double czsj = 30 * qTan(model.czsj / 2 * M_PI / 180) * 2;
			 convert3DMap(curPos, model.fw, model.yj, PIANZHUAN, spsj, czsj, 15, QColor("#88FFFF"), model);
		 }

	 }

	 return vec;
  }
 bool IAgricultureHomepageImpl::AddSensor(SensorListModel &model) 
 {
	 if (!isOpen)
	 {
		 return false;
	 }
	 QMutexLocker locker(&sMutex);
	 QString strSql("INSERT INTO agri_monitor_sub_list (`mointName`, `mointID`, `mointType`, `mointModenlName`, `localHeight`, `longitude`, `latitude`,`url`,`yj`,`fw`,`spsj`,`czsj`,`typeID`,`screenX`,`screenY`,`sensor_type_id`,`origin_sensor_type_id`,`origin_longitude`,`origin_latitude`,`field_id`) VALUES (");
	 //strSql += "'" + QString::number(listModel.nID) + "',";
	 strSql += "'" + model.mointName + "',";
	 strSql += "'" + model.mointID + "',";
	 strSql += "'" + model.mointType + "',";
	 strSql += "'" + model.mointModenlName + "',";
	 strSql += "'" + QString::number(model.localHeight, 'f', 2) + "',";
	 strSql += "'" + QString::number(model.longitude, 'f', 7) + "',";
	 strSql += "'" + QString::number(model.latitude, 'f', 7) + "',";
	 strSql += "'" + model.requsetUrl + "',";
	 strSql += "'" + QString::number(model.yj) + "',";
	 strSql += "'" + QString::number(model.fw) + "',";
	 strSql += "'" + QString::number(model.spsj) + "',";
	 strSql += "'" + QString::number(model.czsj) + "',";
	 strSql += "'" + QString::number(model.typeID) + "',";
	 strSql += "'" + QString::number(model.screenX, 'f', 2) + "',";
	 strSql += "'" + QString::number(model.screenY, 'f', 2) + "',";
	 strSql += "'" + model.sensor_type_id + "',";
	 strSql += "'" + model.origin_sensor_type_id + "',";
	 strSql += "'" + QString::number(model.origin_longitude, 'f', 7) + "',";
	 strSql += "'" + QString::number(model.origin_latitude, 'f', 7) + "',";
	 strSql += "'" + model.field_id + "'";
	 strSql += ")";
	 QSqlQuery query;
	 bool success = query.exec(strSql);
	 if (!success) {
		 QSqlError error = query.lastError();
		 qDebug() << "SQL error:" << error.text();
		 qDebug() << "Database error code:" << error.nativeErrorCode();
	 }
	 else
	 {
		 addStructLocalFile(model);
		

		 auto it1 = m_origin_sensor_list_map.find(model.sensor_type_id);
		 if (it1 != m_origin_sensor_list_map.end())
		 {
			 it1->second.push_back(model);
		 }
		 else
		 {
			 std::vector<SensorListModel> temp;
			 temp.push_back(model);
			 m_origin_sensor_list_map.insert(std::make_pair(model.sensor_type_id, temp));
		 }

		 auto it = m_sensorListMap.find(model.sensor_type_id);
		 if (it != m_sensorListMap.end())
		 {
			 it->second.push_back(model);
		 }
		 else
		 {
			 std::vector<SensorListModel> temp;
			 temp.push_back(model);
			 m_sensorListMap.insert(std::make_pair(model.sensor_type_id, temp));
		 }

		 //clearSingle3DMointerImg(model);
		 draw3DMointorImg(model);
		// clearnMointer3dText(model);
		// drawMointer3dText(model);
		 casic_vector3d curPos(model.longitude, model.latitude, model.localHeight + RELATE_HEIGHT);
		 if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Video)
		 {
			 clearn3DMap(model);
			 double spsj = 30 * qTan(model.spsj / 2 * M_PI / 180) * 2;
			 double czsj = 30 * qTan(model.czsj / 2 * M_PI / 180) * 2;
			 convert3DMap(curPos, model.fw, model.yj, PIANZHUAN, spsj, czsj, 15, QColor("#88FFFF"), model);
		 }

	 }
	 return success;
}
 std::vector<QString> IAgricultureHomepageImpl::GetSensorWithField(QString &field_id)
 {
	 std::vector<QString> vec;
	 if (!isOpen)
	 {
		 return vec;
	 }
	 QMutexLocker locker(&sMutex);

	 QSqlQuery query;

	 QString strSql = "SELECT`sensor_id`";
	 QString strGeneralSql = strSql + " FROM `agri_relation_field_sensor`" + " WHERE field_id = \"" + field_id + "\";";

	
	 vec.clear();
	 bool success = query.exec(strGeneralSql);
	 if (success)
	 {
		 while (query.next())
		 {
			 QString str1 = query.value(0).toString();
			 vec.push_back(str1);
		 }
	 }
	 return vec;
 }
 bool IAgricultureHomepageImpl::AddSensorWithField(QString &field_id, QString &mointID)
 {
	 if (!isOpen)
	 {
		 return false;
	 }
	 QMutexLocker locker(&sMutex);
	 QString strSql("INSERT INTO agri_relation_field_sensor (`field_id`, `sensor_id`) VALUES (");
	 //strSql += "'" + QString::number(listModel.nID) + "',";
	 strSql += "'" + field_id + "',";
	 strSql += "'" + mointID + "'";
	 strSql += ")";
	 QSqlQuery query;
	 bool success = query.exec(strSql);
	 if (!success) {
		 QSqlError error = query.lastError();
		 qDebug() << "SQL error:" << error.text();
		 qDebug() << "Database error code:" << error.nativeErrorCode();
	 }
	 return success;
 }
 bool IAgricultureHomepageImpl::DeleteSensorWithField(QString &mointID)
 {
	 if (!isOpen)
	 {
		 return false;
	 }
	 QMutexLocker locker(&sMutex);
	 QString strSql("DELETE FROM agri_relation_field_sensor where sensor_id = '");
	 strSql += mointID + "'";
	 QSqlQuery query;
	 bool success = query.exec(strSql);
	 return success;
 }
 bool IAgricultureHomepageImpl::DeleteSensor(SensorListModel &model) 
 {
	 if (!isOpen)
	 {
		 return false;
	 }
	 QMutexLocker locker(&sMutex);
	 QString strSql("DELETE FROM agri_monitor_sub_list where mointID = '");
	 strSql += model.mointID + "'";
	 QSqlQuery query;
	 bool success = query.exec(strSql);
	 if (success)
	 {
		 deleteStructLocalFile(model);

		 QString mointID = model.mointID;

		 SensorListModelVec sensorList = m_sensorListMap[model.sensor_type_id];

		 sensorList.erase(
			 std::remove_if(sensorList.begin(), sensorList.end(),
				 [&mointID](const auto& s) { return s.mointID == mointID; }),
			 sensorList.end()
		 );

		 
		 m_sensorListMap[model.sensor_type_id] = sensorList;

		 clearSingle3DMointerImg(model);
		// clearnMointer3dText(model);
		 clearn3DMap(model);
	 }

	 return success;
}
bool IAgricultureHomepageImpl::UpdateSensor(SensorListModel &model)
 {
	if (!isOpen)
	{
		return false;
	}
	QMutexLocker locker(&sMutex);
	QString strSql("UPDATE agri_monitor_sub_list set ");
	strSql += "`mointName` = '" + model.mointName + "',";
	//strSql += "`mointID` = '" + QString::number(listModel.mointID) + "',";
	strSql += "`mointType` = '" + model.mointType + "',";
	strSql += "`sensor_type_id` = '" + model.sensor_type_id + "',";
	strSql += "`mointModenlName` = '" + model.mointModenlName + "',";
	strSql += "`localHeight` = '" + QString::number(model.localHeight, 'f', 2) + "',";
	strSql += "`longitude` = '" + QString::number(model.longitude, 'f', 7) + "',";
	strSql += "`latitude` = '" + QString::number(model.latitude, 'f', 7) + "',";
	strSql += "`origin_longitude` = '" + QString::number(model.origin_longitude, 'f', 7) + "',";
	strSql += "`origin_latitude` = '" + QString::number(model.origin_latitude, 'f', 7) + "',";
	strSql += "`origin_sensor_type_id` = '" + model.origin_sensor_type_id + "',";
	strSql += "`url` = '" + model.requsetUrl + "',";
	strSql += "`yj` = '" + QString::number(model.yj) + "',";
	strSql += "`fw` = '" + QString::number(model.fw) + "',";
	strSql += "`spsj` = '" + QString::number(model.spsj) + "',";
	strSql += "`czsj` = '" + QString::number(model.czsj) + "',";
	//strSql += "`screenX` = '" + QString::number(model.screenX, 'f', 2) + "',";
	//strSql += "`screenY` = '" + QString::number(model.screenY, 'f', 2) + "'";
	strSql += "`sensor_type_id` = '" + model.sensor_type_id + "'";
	strSql += "where `mointID` = '" + model.mointID + "'";

	/*
	strSql += "where `MBBH` = '" + QString::fromStdString(data.GetID()) + "'";*/
	QSqlQuery query;
	bool success = query.exec(strSql);
	if (!success) {
		QSqlError error = query.lastError();
		qDebug() << "SQL error:" << error.text();
		qDebug() << "Database error code:" << error.nativeErrorCode();
	}
	else
	{
		//clearAll3DMointerImg();
		if (model.sensor_type_id == "unknow")
		{
			clearSingle3DMointerImg(model);
			clearn3DMap(model);
			//clearnMointer3dText(model);
			auto it = m_sensorListMap.find(model.sensor_type_id);
			if (it != m_sensorListMap.end())
			{
				it->second.push_back(model);
			}
			else
			{
				std::vector<SensorListModel> temp;
				temp.push_back(model);
				m_sensorListMap.insert(std::make_pair(model.sensor_type_id, temp));
			}

			auto it1 = m_origin_sensor_list_map.find(model.sensor_type_id);
			if (it1 != m_origin_sensor_list_map.end())
			{
				it1->second.push_back(model);
			}
			else
			{
				std::vector<SensorListModel> temp;
				temp.push_back(model);
				m_origin_sensor_list_map.insert(std::make_pair(model.sensor_type_id, temp));
			}


			return success;
		}
		SensorListModelVec sensorList = m_sensorListMap[model.sensor_type_id];

		auto replace_it = std::find_if(sensorList.begin(), sensorList.end(),
			[&model](const auto& s) { return s.mointID == model.mointID; });
		if (replace_it != sensorList.end()) {
			*replace_it = model;
		}

		m_sensorListMap[model.sensor_type_id] = sensorList;
		m_origin_sensor_list_map[model.sensor_type_id] = sensorList;

		clearSingle3DMointerImg(model);
		//clearnMointer3dText(model);
		clearn3DMap(model);
	/*	SensorListModel *localSensorModel = readStructLocalFile(model);
		if (localSensorModel->isShow)
		{

		}*/
		draw3DMointorImg(model);
		draw3DSensorModel(model);
		//drawMointer3dText(model);
		casic_vector3d curPos(model.longitude, model.latitude, model.localHeight + RELATE_HEIGHT);
		if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Video)
		{
			double spsj = 30 * qTan(model.spsj / 2 * M_PI / 180) * 2;
			double czsj = 30 * qTan(model.czsj / 2 * M_PI / 180) * 2;
			convert3DMap(curPos, model.fw, model.yj, PIANZHUAN, spsj, czsj, 15, QColor("#88FFFF"), model);
		}

	}
	return success;
 }
bool IAgricultureHomepageImpl::CheckSampleSensor(SensorListModel &model)
{
	if (!isOpen)
	{
		return false;
	}
	SensorListModelVec m_sensonrListVec = m_sensorListMap[model.sensor_type_id];
	for (auto m : m_sensonrListVec) {
		if (m.mointID == model.mointID || m.mointName == model.mointName)
		{
			return true;
		}
	}
	m_sensonrListVec.push_back(model);
	return false;
}
bool IAgricultureHomepageImpl::AddPestHarmSqlData(FarmPestPhotoInfication &listModel)
{
	if (!isOpen)
	{
		return false;
	}
	QMutexLocker locker(&sMutex);
	QSqlQuery query;
	query.prepare("INSERT INTO agri_pest_photo_indefication (photo_id, recode_time, localHeight, longitude, latitude,  "
		"pest_type, pest_stage, pest_number, recode_range, pest_grade, "
		"recognize_image, mointId, original_image) "
		"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

	query.addBindValue(listModel.photo_id);
	query.addBindValue(listModel.recode_time);
	query.addBindValue(listModel.localHeight);
	query.addBindValue(listModel.longitude);
	query.addBindValue(listModel.latitude);
	query.addBindValue(listModel.pest_type);
	query.addBindValue(listModel.pest_stage);
	query.addBindValue(listModel.pest_number);
	query.addBindValue(listModel.recode_range);
	query.addBindValue(listModel.pest_grade);
	query.addBindValue(listModel.recongzine_byteArray);
	query.addBindValue(listModel.mointId);
	query.addBindValue(listModel.origain_byteArray);

	bool success = query.exec();
	if (!success) {
		//qDebug() << "Insert error: " << query.lastError().text();
		return false;
	}
	return success;
}
PestPhotoInficationVec IAgricultureHomepageImpl::GetPestHarmData(const QString &timeStr)
{
	PestPhotoInficationVec farmPestVec;
	if (!isOpen)
	{
		return farmPestVec;
	}

	QMutexLocker locker(&sMutex);
	QSqlQuery query;
	QString strSql = "SELECT `id`, `photo_id`, `recode_time`, `localHeight`, `longitude`, `latitude`, `pest_type`, `pest_stage`, `pest_number`,`recode_range`,`pest_grade`,`recognize_image`,`mointId`";
	QString strGeneralSql = strSql + " FROM `agri_pest_photo_indefication`" + " where recode_time = ";
	strGeneralSql += "'" + timeStr + "'";

	bool success = query.exec(strGeneralSql);
	if (success)
	{

		while (query.next())
		{
			QString str1 = query.value(0).toString();
			QString str2 = query.value(1).toString();
			QString str3 = query.value(2).toString();
			QString str4 = query.value(3).toString();
			QString str5 = query.value(4).toString();
			QString str6 = query.value(5).toString();
			QString str7 = query.value(6).toString();
			QString str8 = query.value(7).toString();
			QString str9 = query.value(8).toString();
			QString str10 = query.value(9).toString();
			QString str11 = query.value(10).toString();
			QByteArray str12 = query.value(11).toByteArray();
			QString str13 = query.value(12).toString();

			FarmPestPhotoInfication m;
			m.nID = str1.toInt();
			m.photo_id = str2;
			m.recode_time = str3;
			m.pest_type = str4;
			m.pest_stage = str5;
			m.localHeight = str6.toDouble();
			m.longitude = str7.toDouble();
			m.latitude = str8.toDouble();
			m.pest_number = str9.toInt();
			m.recode_range = str10.toInt();
			m.pest_grade = str11;
			m.recongzine_byteArray = str12;
			m.mointId = str13;

			farmPestVec.push_back(m);

		}
	}
	return farmPestVec;

}
std::vector<QString> IAgricultureHomepageImpl::GetPestHarmTime()
{
	std::vector<QString>tempVec;
	if (!isOpen)
	{
		return tempVec;
	}
	

	QMutexLocker locker(&sMutex);
	QSqlQuery query;
	QString strSql = "SELECT `recode_time`";
	QString strGeneralSql = strSql + " FROM `agri_pest_photo_indefication`";

	bool success = query.exec(strGeneralSql);

	if (success)
	{
		while (query.next())
		{
			QString str1 = query.value(0).toString();
			tempVec.push_back(str1);
		}
	}
	return tempVec;
}
std::map<QString, SensorListModelVec> IAgricultureHomepageImpl::GetSensorListMap()
{
	return m_origin_sensor_list_map;
}
 std::map<QString, SensorListModelVec> IAgricultureHomepageImpl::GetSensorMap()
{
	 return m_sensorListMap;
}
 void IAgricultureHomepageImpl::showSensor(bool isShow)
{
	 for (auto it: m_image3DVec)
	 {
		 //it->SetTagState(isShow);
		 if (it != nullptr)
		 {
			 it->SetVisible(isShow);
		 }
		
	 }


}
 void IAgricultureHomepageImpl::clearn3DMap(SensorListModel &model)
 {
	 IEarth *pEarth = IEarth::GetEarth();
	 if (pEarth == nullptr)
		 return;

	 ILabel * label = m_map3DImage[model.mointID];
	 if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Video)
	 {
		 std::vector<IUserMesh2*>	 discoverZone = cone3DpointMap[model.mointID];
		 auto itr1 = discoverZone.begin();
		 for (; itr1 != discoverZone.end(); itr1++)
		 {
			 IEarth *pEarth = IEarth::GetEarth();
			 pEarth->GetGraphics()->RemoveObject(*itr1);
			 *itr1 = nullptr;
			 delete *itr1;
		 }
		 discoverZone.clear();
	 }
 }

 void IAgricultureHomepageImpl::showBrand(bool isShow)
{
	 //TODO:
	/* SensorListModelVec m_sensorListVec;
	 for (auto it = m_sensorListMap.begin(); it != m_sensorListMap.end(); it++)
	 {
		 for (auto model : it->second) {
			 m_sensorListVec.push_back(model);
		 }
	 }
	 clearnMointer3dText();
	 if (isShow)
	 {
		 drawMointer3dText(m_sensorListModel);
	 }*/
}

 void IAgricultureHomepageImpl::showView(bool isShow)
{
	 SensorListModelVec m_sensorListVec;
	 for (auto it = m_sensorListMap.begin(); it != m_sensorListMap.end(); it++)
	 {
		 for (auto model : it->second) {
			 m_sensorListVec.push_back(model);
		 }
	 }
	 for (auto &model : m_sensorListVec)
	 {
		 clearn3DMap(model);
	 }
	 if (isShow)
	 {
		 for (auto &model : m_sensorListVec) {

			 if (model.sensor_type_id == "unknow")
			 {
				 continue;
			 }
			 draw3DMointorImg(model);

			 if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Video)
			 {
				 casic_vector3d curPos(model.longitude, model.latitude, model.localHeight + RELATE_HEIGHT);

				 double spsj = 30 * qTan(model.spsj / 2 * M_PI / 180) * 2;
				 double czsj = 30 * qTan(model.czsj / 2 * M_PI / 180) * 2;
				 convert3DMap(curPos, model.fw, model.yj, PIANZHUAN, spsj, czsj, 15, QColor("#88FFFF"), model);
			 }

		 }
	 }
}
 void IAgricultureHomepageImpl::drawMointer3dText(SensorListModel &model)
 {
	 QMutexLocker locker(&sMutex);
	 IEarth *pEarth = IEarth::GetEarth();
	 if (pEarth == nullptr) return;

	 clearnMointer3dText();
	 m_sensorListModel = model;

	 if (pEarth->GetGraphics() == nullptr || pEarth == nullptr) return;

	 // TODO：切换标牌显隐，通过病虫害分析切回此界面发生崩溃。2025/04/27
	 IText* mshpText = pEarth->GetGraphics()->CreateText(model.mointName.toLocal8Bit(), model.longitude, model.latitude, model.localHeight + 10.00, NULL, 18,
		 TRGB(128, 255, 255, 255), TRGB(128, 128, 128, 128), TRGB(128, 128, 0, 128), 7);

	 m_map3DTextVec.push_back(mshpText);
 }

 //删除标牌
 void IAgricultureHomepageImpl::clearnMointer3dText()
 {
	 IEarth *pEarth = IEarth::GetEarth();
	 if (pEarth == nullptr)
		 return;
	 auto itr2 = m_map3DTextVec.begin();
	 for (; itr2 != m_map3DTextVec.end(); itr2++)
	 {
		 IEarth *pEarth = IEarth::GetEarth();
		 pEarth->GetGraphics()->RemoveObject(*itr2);
		 *itr2 = nullptr;
		 delete *itr2;
	 }
	 m_map3DTextVec.clear();


	 //IText* pai = m_map3DText[model.mointID];
	 //if (pai != nullptr)
	 //{
		//  IEarth *pEarth = IEarth::GetEarth();
		//  pEarth->GetGraphics()->RemoveObject(pai);
		//  pai = nullptr;
		//  delete pai;
	 //}
	
 }
void IAgricultureHomepageImpl::draw3DMointorImg(SensorListModel &model)
{
	if (model.sensor_type_id == "unknow")
	{
		return;
	}
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	QColor col(255, 0, 0, 255);

	char * imgStr;

	switch (static_cast<SensorType>(model.typeID))
	{
	case SensorType::SensorTypeType_Video:
	{
		imgStr = "Temp_Line/surveillance_camera_3D.png";
	}
	break;

	case SensorType::SensorTypeType_Soil:
	{
		imgStr = "Temp_Line/soil_moisture_3D.png";
	}
	break;

	case SensorType::SensorTypeType_Weather:
	{
		imgStr = "Temp_Line/weather_station_3D.png";
	}
	break;

	case SensorType::SensorTypeType_PestMonitor:
	{
		imgStr = "Temp_Line/pest_detection_3D.png";
	}
	break;

	case SensorType::SensorTypeType_KillInsects:
	{
		imgStr = "Temp_Line/drive_pest_3D.png";
	}
	break;

	case SensorType::SensorTypeType_Valve:
	{
		imgStr = "Temp_Line/irrigation_valve_3D.png";
	}
	break;

	case SensorType::SensorTypeType_FarmCar:
	{
		imgStr = "Temp_Line/entry_point_3D.png";
	}
	break;
	case SensorType::SensorTypeType_FlyStop:
	{
		imgStr = "Temp_Line/dji_airport_3D.png";
	}
	break;

	default:
		break;
	}

	if (m_strDataDir == NULL)
	{
		m_strDataDir = tr2(LY_AgricultureHomepageService::Get().GetDataDir().data());
	}
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(QUuid::createUuid().toRfc4122(), 16));
	QString strLable = "";
	//if (m_map3DImage.find(id) == m_map3DImage.end())
	//{
		std::string strImg;
		auto it = m_mapLabelPath.find(uid);
		if (it == m_mapLabelPath.end())
		{
			QString strPath = QObject::tr(m_strDataDir);
			strPath += QLatin1String(imgStr);
			QImage img = QImage(strPath);
			strPath = QObject::tr(m_strDataDir);
			strPath += QLatin1String("/temp/");
			strPath += uid.toString(QUuid::WithoutBraces);
			strPath += QLatin1String(".png");
			QFile::remove(strPath);
			bool bresult = img.save(strPath);
			strImg = tr3(strPath).data();
			m_mapLabelPath[uid] = strImg;
		}
		else
		{
			strImg = it->second;
		}
		ILabel *pLabel = nullptr;
		if (static_cast<SensorType>(model.typeID) == SensorType::SensorTypeType_Video)
		{
			pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(),
				model.longitude, model.latitude, model.localHeight + RELATE_HEIGHT + 5.0, 40, 80, 0);
		}
		else
		{
			pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(),
				model.longitude, model.latitude, model.localHeight + 5.0, 40, 80, 0);
		}
		if (pLabel != nullptr)
		{
			m_image3DVec.push_back(pLabel);
			m_map3DImage[model.mointID] = pLabel;
		}
	//}
		
	//qDebug() << "**********==:" << m_map3DImage;
}

void IAgricultureHomepageImpl::clearSingle3DMointerImg(SensorListModel &model)
{
	if (m_map3DImage.size() == 0)
	{
		return;
	}

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	m_mapLabelPath.clear();
	ILabel * label = m_map3DImage[model.mointID];
	if (label != nullptr)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(label);
		label = nullptr;
		delete label;
	}
}
void IAgricultureHomepageImpl::clearAll3DMointerImg()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	auto itr2 = m_image3DVec.begin();
	for (; itr2 != m_image3DVec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_image3DVec.clear();

}
void IAgricultureHomepageImpl::convert3DMap(casic_vector3d curPos,
	double heading, double fuyang,
	double pianzhuan, float angle_l,
	float angle_w, float dis_m, QColor zoneColor, SensorListModel model)
{

	if (model.sensor_type_id == "unknow")
	{
		return;
	}
	if (heading > 360)
		heading -= 360;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	std::vector<IUserMesh2*>	 discoverZone;

	// TODO: 修改为设备真实视场
	discoverZone = createDetectionZone(angle_l, angle_w, dis_m + 100);

	cone3DpointMap[model.mointID] = discoverZone;
	Uav3D uav3D;
	if (discoverZone.size() == 2)
	{
		uav3D.pSightingLine = discoverZone.at(0);
		uav3D.pSightingLine2 = discoverZone.at(1);
	}
	if (uav3D.pSightingLine != nullptr && uav3D.pSightingLine2 != nullptr)
	{
		uav3D.pSightingLine->SetPosition(curPos.x, curPos.y,
			curPos.z);
		uav3D.pSightingLine->SetYawPitchRoll(heading, fuyang, pianzhuan);
		uav3D.pSightingLine2->SetPosition(curPos.x, curPos.y,
			curPos.z);
		uav3D.pSightingLine2->SetYawPitchRoll(heading, fuyang, pianzhuan);

		QColor colorTrack(zoneColor);
		colorTrack.setAlpha(255);
		QColor colorTemp;
		colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());

		colorTrack.setAlpha(40);
		colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());
		uav3D.pSightingLine->SetColor(colorTemp.rgba());
		colorTrack.setAlpha(80);
		colorTemp.setRgb(colorTrack.blue(), colorTrack.green(), colorTrack.red(), colorTrack.alpha());
		uav3D.pSightingLine2->SetColor(colorTemp.rgba());
	}
	coneUav3DpointMap[model.mointID] = uav3D;
}
std::vector<IUserMesh2*> IAgricultureHomepageImpl::createDetectionZone(float angle_l, float angle_w, float dis_m)
{
	std::vector<IUserMesh2*> reUserMesh2s;
	reUserMesh2s.clear();
	IUserMesh2 *pUM = nullptr;
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh2();
	}

	int angle_w_count = 1;
	int angle_l_count = 1;

	if (angle_w_count == 0)
		angle_w_count = 1;
	if (angle_l_count == 0)
		angle_l_count = 1;

	float angle_w_dis = angle_w / angle_w_count;
	float angle_l_dis = angle_l / angle_l_count;

	std::vector<unsigned short> pIndex_up;
	std::vector<unsigned short> pIndex_down;
	std::vector<unsigned short> pIndex_left;
	std::vector<unsigned short> pIndex_r;
	std::vector<std::vector<unsigned short>> pIndex_cir;
	pIndex_up.clear();
	pIndex_down.clear();
	pIndex_left.clear();
	pIndex_r.clear();
	pIndex_cir.clear();

	pIndex_up.push_back(0);
	pIndex_down.push_back(0);
	pIndex_left.push_back(0);
	pIndex_r.push_back(0);

	VERTEX *pVerter = (VERTEX*)malloc(((angle_l_count + 1) * (angle_w_count + 1) + 1) * sizeof(VERTEX));
	if (pVerter != nullptr)
	{
		int pVIndex = 0;
		float angle_w_cur = -(angle_w / 2);
		pVerter[pVIndex].x = 0;
		pVerter[pVIndex].y = 0;
		pVerter[pVIndex].z = 0;
		pVIndex++;
		for (int angle_w_index = 0; angle_w_index <= angle_w_count; angle_w_index++)
		{
			double angle_w_cur_pi = angle_w_cur*PI / 180;
			angle_w_cur += angle_w_dis;
			double z = dis_m * sin(angle_w_cur_pi);
			double dis_z = dis_m * cos(angle_w_cur_pi);
			float angle_l_cur = -(angle_l / 2);

			for (int angle_l_index = 0; angle_l_index <= angle_l_count; angle_l_index++)
			{
				double angle_l_cur_pi = angle_l_cur * PI / 180;
				angle_l_cur += angle_l_dis;
				double x = dis_z * sin(angle_l_cur_pi);
				double y = dis_z * cos(angle_l_cur_pi);

				pVerter[pVIndex].x = x;
				pVerter[pVIndex].y = y;
				pVerter[pVIndex].z = z;

				if (angle_w_index == 0)
				{
					pIndex_up.push_back(pVIndex);
				}
				else if (angle_w_index == angle_w_count)
				{
					pIndex_down.push_back(pVIndex);
				}

				if (angle_l_index == 0)
				{
					pIndex_left.push_back(pVIndex);
				}
				else if (angle_l_index == angle_l_count)
				{
					pIndex_r.push_back(pVIndex);
				}

				pVIndex++;
			}
		}

		int startIndex = 1;
		int index_added = angle_l_count + 1;
		for (int wIndex = 0; wIndex < angle_w_count; wIndex++)
		{
			int startWIndex = startIndex + wIndex * index_added;
			std::vector<unsigned short> tempPindex_cir;
			tempPindex_cir.clear();
			tempPindex_cir.push_back(startWIndex);
			tempPindex_cir.push_back(startWIndex + index_added);
			for (int lIndex = 1; lIndex < index_added; lIndex++)
			{
				tempPindex_cir.push_back(startWIndex + lIndex);
				tempPindex_cir.push_back(startWIndex + lIndex + index_added);
			}
			pIndex_cir.push_back(tempPindex_cir);
		}

		pUM->SetPoints(pVIndex, pVerter);
		pUM->SetIndexs(pIndex_up.size(), &pIndex_up[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_down.size(), &pIndex_down[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_left.size(), &pIndex_left[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_r.size(), &pIndex_r[0], IUserMesh::USERMESH_POLYGON, false);
		for (int iindex = 0; iindex < pIndex_cir.size(); iindex++)
		{
			if (iindex != pIndex_cir.size() - 1)
			{
				pUM->SetIndexs(pIndex_cir[iindex].size(), &pIndex_cir[iindex][0], IUserMesh::USERMESH_QUAD_STRIP, false);
			}
			else
			{
				pUM->SetIndexs(pIndex_cir[iindex].size(), &pIndex_cir[iindex][0], IUserMesh::USERMESH_QUAD_STRIP);
			}
		}

		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
		pUM->SetState(IUserMesh::LINEWIDTH, 3);
		pUM->SetState(GL_DEPTH_TEST, 1);
		reUserMesh2s.push_back(pUM);

		pUM = pEarth->GetGraphics()->CreateUserMesh2();
		pUM->SetPoints(pVIndex, pVerter);
		pUM->SetIndexs(pIndex_up.size(), &pIndex_up[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_down.size(), &pIndex_down[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_left.size(), &pIndex_left[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_r.size(), &pIndex_r[0], IUserMesh::USERMESH_POLYGON);
		free(pVerter);
		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
		pUM->SetState(IUserMesh::LINEWIDTH, 2);
		pUM->SetState(IUserMesh::LIGHT, 1);
		pUM->SetState(GL_DEPTH_TEST, 1);
		reUserMesh2s.push_back(pUM);

	}
	return reUserMesh2s;
}

void IAgricultureHomepageImpl::convert3DCube(double lon, double lat, double  local_height)
{

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}
	IUserMesh2 * pUM1 = pEarth->GetGraphics()->CreateUserMesh2();
	IGraphics *pGrp = pEarth->GetGraphics();
	SHPObject *pShp;
	CCreateShape cs;
	double dX[5], dY[5];
	double x1 = 10 / 2;
	double y1 = 10 / 2;
	double x2 = 10;
	double y2 = 10 / 2;
	/*if (zone.stRoundParameter.nCornerRadius == 0)
	{*/
	dX[4] = -x1;
	dY[4] = -y1;
	dX[0] = -x1;
	dY[0] = -y1;
	dX[3] = x1;
	dY[3] = -y1;
	dX[2] = x1;
	dY[2] = y1;
	dX[1] = -x1;
	dY[1] = y1;
	//}
	//else
	//{
	//	dX[4] = 0;
	//	dY[4] = -y1;
	//	dX[0] = 0;
	//	dY[0] = -y1;
	//	dX[3] = x2;
	//	dY[3] = -y1;
	//	dX[2] = x2;
	//	dY[2] = y1;
	//	dX[1] = 0;
	//	dY[1] = y1;
	//}
	pShp = cs.CreatePolygon(dX, dY, 5);
	if (pShp == nullptr)
	{
		return;
	}
	double dM = 10;
	SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &local_height, &dM);
	pUM1 = pGrp->CreateUserMesh2(pShp1);

	pUM1->SetColor(RGB(255, 0, 0));
	pUM1->HeighLightEdge();
	pUM1->SetPosition(lon, lat, 0);
	pUM1->SetYawPitchRoll(40, 0, 0);
	//pUM->SetState(GL_DEPTH_TEST, 1);

	SHPDestroyObject(pShp1);
}


void IAgricultureHomepageImpl::writeStructLocalFile(std::vector<SensorListModel> tempVec){

	if (tempVec.size() == 0)
	{
		return;
	}
	TiXmlDocument doc;
	TiXmlDeclaration* tinyXmlDeclare = new TiXmlDeclaration("1.0", "utf-8", "");	// 声明头部格式
	doc.LinkEndChild(tinyXmlDeclare);
	// Create the root element
	TiXmlElement* root = new TiXmlElement("MonitorList");
	doc.LinkEndChild(root);

	for (auto monitor : tempVec)
	{
		TiXmlElement* monitorElement = new TiXmlElement("Monitor");
		root->LinkEndChild(monitorElement);
		monitorElement->SetAttribute("mointName", monitor.mointName.toStdString().c_str());
		monitorElement->SetAttribute("mointID", monitor.mointID.toStdString().c_str());
		monitorElement->SetAttribute("isShow", monitor.isShow);
		monitorElement->SetAttribute("isStatus", monitor.isStatus);
		monitorElement->SetAttribute("isMap", monitor.isMap);
	}
	QString fileName = LOCAL_PATH_MOINTOR;
	QByteArray byteArray = fileName.toUtf8();
	char* charStr = byteArray.data(); // 获取char*指针

	if (!doc.SaveFile(charStr)) {
		qDebug() << "保存成功";
	}
	else {
		qDebug() << "保存失败";
	}
}
SensorListModel *IAgricultureHomepageImpl::readStructLocalFile(SensorListModel &model)
{

	if (tinyXmlDoc == NULL)
	{
		return nullptr;
	}
	// 读取文档声明信息(也就是xml的头部信息：<?xml version="1.0" encoding="utf-8" ?>)
	if (tinyXmlDoc->FirstChild() == NULL)
	{
		return nullptr;
	}
	TiXmlDeclaration *pDeclar = tinyXmlDoc->FirstChild()->ToDeclaration();
	if (pDeclar == NULL) {
		return nullptr;
	}

	// 得到文件根节点
	TiXmlElement *Library = new TiXmlElement("MonitorList");
	Library = tinyXmlDoc->RootElement();
	TiXmlElement* pItem = Library->FirstChildElement("Monitor");
	if (pItem == NULL) {
		return nullptr;
	}



	for (; pItem != NULL; pItem = pItem->NextSiblingElement("Monitor")) {

		const char* mointName = pItem->Attribute("mointName");
		const char* mointID = pItem->Attribute("mointID");
		const char* isShow = pItem->Attribute("isShow");
		const char* isStatus = pItem->Attribute("isStatus");
		const char* isMap = pItem->Attribute("isMap");
		SensorListModel m;
		m.mointName = QString(mointName);
		m.mointID = std::stoi(mointID);
		m.isShow = (strcmp(isShow, "true") == 0) || (strcmp(isShow, "1") == 0);
		m.isStatus = std::stoi(isStatus);
		m.isMap = std::stoi(isMap);


		if (m.mointID == model.mointID)
		{
			return &m;
		}
	}
	delete tinyXmlDoc;
}



void IAgricultureHomepageImpl::editStructLocalFile(SensorListModel &m)
{
	QString fileName = LOCAL_PATH_MOINTOR;
	QByteArray byteArray = fileName.toUtf8();
	char* charStr = byteArray.data(); // 获取char*指针
	TiXmlDocument* tinyXmlDoc = new TiXmlDocument(charStr);
	tinyXmlDoc->LoadFile(TIXML_ENCODING_LEGACY);

	// 读取文档声明信息(也就是xml的头部信息：<?xml version="1.0" encoding="utf-8" ?>)
	TiXmlDeclaration *pDeclar = tinyXmlDoc->FirstChild()->ToDeclaration();
	if (pDeclar != NULL) {
		printf("头部信息： version is %s , encoding is %s\n", pDeclar->Version(), pDeclar->Encoding());
	}

	// 得到文件根节点
	TiXmlElement *Library = new TiXmlElement("MonitorList");
	Library = tinyXmlDoc->RootElement();
	TiXmlElement* pItem = Library->FirstChildElement("Monitor");

	for (; pItem != NULL; pItem = pItem->NextSiblingElement("Monitor")) {

		if (strcmp(pItem->Attribute("mointID"), m.mointID.toStdString().c_str()) == 0) {
			pItem->SetAttribute("mointName", m.mointName.toStdString().c_str());
			pItem->SetAttribute("isShow", std::to_string(m.isShow).c_str());
			pItem->SetAttribute("isStatus", std::to_string(m.isStatus).c_str());
			pItem->SetAttribute("isMap", std::to_string(m.isMap).c_str());
		}
	}
	clearSingle3DMointerImg(m);
	//clearnMointer3dText(m);
	if (static_cast<SensorType>(m.typeID) == SensorType::SensorTypeType_Video)
	{
		clearn3DMap(m);
	}
	


	if (m.isShow)
	{
		draw3DMointorImg(m);
	}

	//if (m.isStatus)
	//{
	//	drawMointer3dText(m);
	//}
	if (static_cast<SensorType>(m.typeID) == SensorType::SensorTypeType_Video)
	{
		if (m.isMap)
		{
			casic_vector3d curPos(m.longitude, m.latitude, m.localHeight + RELATE_HEIGHT);

			double spsj = 30 * qTan(m.spsj / 2 * M_PI / 180) * 2;
			double czsj = 30 * qTan(m.czsj / 2 * M_PI / 180) * 2;
			convert3DMap(curPos, m.fw, m.yj, PIANZHUAN, spsj, czsj, 15, QColor("#88FFFF"), m);
		}

	}
	



	if (!tinyXmlDoc->SaveFile(charStr)) {
		qDebug() << "保存成功";
	}
	else {
		qDebug() << "保存失败";
	}
	delete tinyXmlDoc;
}
void IAgricultureHomepageImpl::deleteStructLocalFile(SensorListModel &m)
{
	QString fileName = LOCAL_PATH_MOINTOR;
	QByteArray byteArray = fileName.toUtf8();
	char* charStr = byteArray.data(); // 获取char*指针
	TiXmlDocument* tinyXmlDoc = new TiXmlDocument(charStr);
	tinyXmlDoc->LoadFile(TIXML_ENCODING_LEGACY);


	// 读取文档声明信息(也就是xml的头部信息：<?xml version="1.0" encoding="utf-8" ?>)
	TiXmlDeclaration *pDeclar = tinyXmlDoc->FirstChild()->ToDeclaration();
	if (pDeclar != NULL) {
		printf("头部信息： version is %s , encoding is %s\n", pDeclar->Version(), pDeclar->Encoding());
	}

	// 得到文件根节点
	TiXmlElement *Library = new TiXmlElement("MonitorList");
	Library = tinyXmlDoc->RootElement();
	TiXmlElement* pItem = Library->FirstChildElement("Monitor");

	for (; pItem != NULL; ) {

		if (strcmp(pItem->Attribute("mointID"), m.mointID.toStdString().c_str()) == 0) {

			TiXmlElement* temporary = pItem->NextSiblingElement("Book1");
			Library->RemoveChild(pItem->ToElement());
			pItem = temporary;
		}
		else {
			pItem = pItem->NextSiblingElement("Monitor");
		}
	}
	if (!tinyXmlDoc->SaveFile(charStr)) {
		qDebug() << "保存成功";
	}
	else {
		qDebug() << "保存失败";
	}
	delete tinyXmlDoc;
}
void IAgricultureHomepageImpl::addStructLocalFile(SensorListModel &m)
{
	QString fileName = LOCAL_PATH_MOINTOR;
	QByteArray byteArray = fileName.toUtf8();
	char* charStr = byteArray.data();
	TiXmlDocument* tinyXmlDoc = new TiXmlDocument(charStr);
	bool fileLoaded = tinyXmlDoc->LoadFile(TIXML_ENCODING_LEGACY);

	// 如果文件不存在或加载失败，创建新的XML文档
	if (!fileLoaded)
	{
		// 创建声明
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
		tinyXmlDoc->LinkEndChild(decl);

		// 创建根节点
		TiXmlElement* root = new TiXmlElement("MonitorList");
		tinyXmlDoc->LinkEndChild(root);
	}

	// 获取根节点
	TiXmlElement* library = tinyXmlDoc->RootElement();
	if (library == nullptr)
	{
		library = new TiXmlElement("MonitorList");
		tinyXmlDoc->LinkEndChild(library);
	}

	// 检查是否已存在相同mointID的节点
	bool alreadyExists = false;
	TiXmlElement* pItem = library->FirstChildElement("Monitor");
	for (; pItem != nullptr; pItem = pItem->NextSiblingElement("Monitor"))
	{
		if (strcmp(pItem->Attribute("mointID"), m.mointID.toStdString().c_str()) == 0)
		{
			alreadyExists = true;
			break;
		}
	}

	if (!alreadyExists)
	{
		// 创建新的Monitor节点
		TiXmlElement* newMonitor = new TiXmlElement("Monitor");

		// 设置属性
		newMonitor->SetAttribute("mointID", m.mointID.toStdString().c_str());
		newMonitor->SetAttribute("mointName", m.mointName.toStdString().c_str());
		newMonitor->SetAttribute("isShow", std::to_string(m.isShow).c_str());
		newMonitor->SetAttribute("isStatus", std::to_string(m.isStatus).c_str());
		newMonitor->SetAttribute("isMap", std::to_string(m.isMap).c_str());

		// 将新节点添加到根节点
		library->LinkEndChild(newMonitor);

		// 保存文件
		if (tinyXmlDoc->SaveFile(charStr)) {
			qDebug() << "添加成功并保存";
		}
		else {
			qDebug() << "添加失败";
		}
	}
	else
	{
		//qDebug() << "mointID已存在，不重复添加";
	}

	delete tinyXmlDoc;
}

bool IAgricultureHomepageImpl::clearLocalFile()
{
	QString fileName = LOCAL_PATH_MOINTOR;
	QByteArray byteArray = fileName.toUtf8();
	char* charStr = byteArray.data();
	TiXmlDocument* tinyXmlDoc = new TiXmlDocument(charStr);
	bool fileLoaded = tinyXmlDoc->LoadFile(TIXML_ENCODING_LEGACY);

	// 如果文件不存在或加载失败，仍然创建一个空结构
	if (!fileLoaded)
	{
		// 创建声明
		TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
		tinyXmlDoc->LinkEndChild(decl);

		// 创建根节点
		TiXmlElement* root = new TiXmlElement("MonitorList");
		tinyXmlDoc->LinkEndChild(root);
	}

	// 获取根节点
	TiXmlElement* library = tinyXmlDoc->RootElement();
	if (library == nullptr)
	{
		library = new TiXmlElement("MonitorList");
		tinyXmlDoc->LinkEndChild(library);
	}

	// 清空所有子节点（保留根节点）
	TiXmlNode* child;
	while ((child = library->FirstChild()) != nullptr)
	{
		library->RemoveChild(child);
	}

	// 保存文件
	bool saveSuccess = tinyXmlDoc->SaveFile(charStr);
	delete tinyXmlDoc;

	if (saveSuccess)
	{
		qDebug() << "文件内容已清空";
		return true;
	}
	else
	{
		qDebug() << "清空文件失败";
		return false;
	}
}

qnzkna::IAgricultureHomePage::IAgricultureHomepageImpl::~IAgricultureHomepageImpl(void)
{
// 	if (osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode)))
// 	{
// 		for (auto it = m_mGrop.begin(); it != m_mGrop.end(); it++) {
// 			mapNode->removeChild(it->second);
// 		}
// 	}
	m_mGrop.clear();
}

void qnzkna::IAgricultureHomePage::IAgricultureHomepageImpl::OnCreate()
{
	groupSensor = new osg::Group;
	groupSensor->setName("groupSensor");

	if (osgViewer::Viewer* viewer = (osgViewer::Viewer*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_OsgViewer)))
	{
		viewer->addEventHandler(new PickModelHandler);
	}
}

void qnzkna::IAgricultureHomePage::IAgricultureHomepageImpl::showSensorModel(bool isShow)
{
	SensorListModelVec m_sensorListVec;
	for (auto it = m_sensorListMap.begin(); it != m_sensorListMap.end(); it++)
	{
		for (auto model : it->second) {
			m_sensorListVec.push_back(model);
		}
	}

	clearAll3DSensorModel();
	if (isShow)
	{
		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
		if (!mapNode) return;

		mapNode->removeChild(groupSensor.get());
		for (SensorListModel &model : m_sensorListVec) {
			draw3DSensorModel(model);
		}
		mapNode->addChild(groupSensor.get());
	}
}

void qnzkna::IAgricultureHomePage::IAgricultureHomepageImpl::draw3DSensorModel(SensorListModel &sensorModel)
{
	if (sensorModel.sensor_type_id == "unknow") return;

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)return;

	auto& itrMap = m_mGrop.find(sensorModel.mointID.toStdString());
	if (itrMap != m_mGrop.end()) {
		groupSensor->removeChild(itrMap->second);
		m_mGrop.erase(itrMap);
	}

	auto& itrMap2 = m_mSensorMap.find(sensorModel.mointID.toStdString());
	if (itrMap2 == m_mSensorMap.end()) {
		m_mSensorMap.insert(std::make_pair(sensorModel.mointID.toStdString(), sensorModel));
	}

	std::string modelNameStr;

	switch (static_cast<SensorType>(sensorModel.typeID))
	{
	case SensorType::SensorTypeType_Video:
	{
		modelNameStr = "Camera";
	}
	break;

	case SensorType::SensorTypeType_Soil:
	{
		modelNameStr = "Soil";
	}
	break;

	case SensorType::SensorTypeType_Weather:
	{
		modelNameStr = "environment";
	}
	break;

	case SensorType::SensorTypeType_PestMonitor:
	{
		modelNameStr = "Insect";
	}
	break;

	case SensorType::SensorTypeType_KillInsects:
	{
		modelNameStr = "Sound";
	}
	break;

	case SensorType::SensorTypeType_Valve:
	{
		modelNameStr = "";
	}
	break;

	case SensorType::SensorTypeType_FarmCar:
	{
		modelNameStr = "";
	}
	break;
	case SensorType::SensorTypeType_FlyStop:
	{
		modelNameStr = "FlyStop";
	}
	break;

	default:
		break;
	}

	if (modelNameStr.empty()) return;

	osg::ref_ptr<osg::Node> model = osgDB::readNodeFile("../data/maps_beijing_changping_shisanling/OSGB/" + modelNameStr + ".osgb");
	if (!model){
		osg::notify(osg::FATAL) << "模型加载失败！" << std::endl;
		return;
	}

	model->setName(sensorModel.mointID.toStdString());

	osg::Matrix posMatrix;
	osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
	em->computeLocalToWorldTransformFromLatLongHeight(
		osg::inDegrees(sensorModel.latitude), osg::inDegrees(sensorModel.longitude), sensorModel.localHeight, posMatrix);

	double scale = 0.02 * 1;
	if (modelNameStr == "FlyStop") {
		scale = 1 * 1;
	}
	
	osg::Matrix scaleMatrix = osg::Matrix::scale(scale, scale, scale);
	osg::Matrix rotateMatrix = osg::Matrix::rotate(osg::inDegrees(80.0), osg::Z_AXIS);
	osg::Matrix finalMatrix = rotateMatrix * scaleMatrix * posMatrix;

	osg::ref_ptr<osg::MatrixTransform> transform = new osg::MatrixTransform;
	transform->setMatrix(finalMatrix);
	transform->addChild(model.get());

	transform->setName(sensorModel.mointID.toStdString());

	// 关闭模型光照
	//model->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

#if 0

	osg::ref_ptr<osgManipulator::Selection> selection = new osgManipulator::Selection;
	selection->addChild(transform);

	scale = transform->getBound().radius() * 1.6;

	// 盒式拖拽器 （由六个平面拖拽器构成，可在各个面上进行缩放、平移）
	//osgManipulator::TabBoxDragger* tad = new osgManipulator::TabBoxDragger();
	//tad->setupDefaultGeometry();//使用默认的图形，即一个立方体
	//tad->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(transform->getBound().center()));
	//tad->addTransformUpdating(selection);//Dragger要改变的对象。
	//tad->setHandleEvents(true);

	// 旋转拖拽器
	osgManipulator::TrackballDragger* dragger = new osgManipulator::TrackballDragger();
	dragger->setupDefaultGeometry();
	dragger->setMatrix(osg::Matrix::scale(scale*0.5, scale*0.5, scale*0.5)*osg::Matrix::translate(transform->getBound().center()));
	dragger->addTransformUpdating(selection);
	dragger->setHandleEvents(true);

	// 三维平移拖拽器（可在三个方向上对模型进行拖拽）
	osgManipulator::TranslateAxisDragger * move3d = new osgManipulator::TranslateAxisDragger();
	move3d->setupDefaultGeometry();
	move3d->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(transform->getBound().center()));
	move3d->addTransformUpdating(selection);
	move3d->setHandleEvents(true);

	//// 旋转圆柱拖拽器
	//osgManipulator::RotateCylinderDragger *rote = new osgManipulator::RotateCylinderDragger();
	//rote->setupDefaultGeometry();
	//rote->setMatrix(osg::Matrix::scale(scale, scale, scale)*osg::Matrix::translate(transform->getBound().center()));
	//rote->addTransformUpdating(selection);
	//rote->setHandleEvents(true);

	// CommandManager管理命令，可用于撤销和重做操作
	osg::ref_ptr<osgManipulator::CommandManager> manager = new osgManipulator::CommandManager;
	manager->connect(*dragger, *selection);

	groupSensor->addChild(selection);
	//groupSensor->addChild(tad);
	groupSensor->addChild(dragger);
	groupSensor->addChild(move3d);
	//groupSensor->addChild(rote);

#else

	groupSensor->addChild(transform);

#endif

	m_mGrop.insert(std::make_pair(sensorModel.mointID.toStdString(), transform));
}

void qnzkna::IAgricultureHomePage::IAgricultureHomepageImpl::clearAll3DSensorModel()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)return;

	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (!mapNode) return;

	for (auto itrMap = m_mGrop.begin(); itrMap != m_mGrop.end(); itrMap++) {
		int num = mapNode->getNumChildren();
		groupSensor->removeChild(itrMap->second);
		m_mGrop.erase(itrMap);
	}
}

