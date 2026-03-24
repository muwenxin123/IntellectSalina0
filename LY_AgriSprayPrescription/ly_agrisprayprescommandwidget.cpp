#include "ly_agrisprayprescommandwidget.h"
#include "ui_ly_agrisprayprescommandwidget.h"
#include "LY_AgriSprayPrescriptionActivator.h"
#include "LY_SprayPrescriptionUntil.h"
#include "qmath.h"
#include <QDebug>
#include <QDateTime>
#include "LY_AgriSprayPrescription.h"

#include <lycore.h>
#include "LY_AgriSprayPrescription.h"
#include "LY_AgriSprayPrescriptionInterface.h"
#include "DataManage/SystemInfo/SystemInternalInterface.h"
#include "LY_AgriSprayPrescriptionInterface.h"
#include "DataManage/IUAVRealTimeStateManageService.h"
#include "LY_AgriSprayPrescriptionInterface.h"
#include "CreateShape.h"
#include <QGeoCoordinate>

#include <osg/ComputeBoundsVisitor>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonOffset>
#include <osg/PrimitiveSet>
#include <osgAnimation/BasicAnimationManager>>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osg/LineWidth>
#include <osgFX/Scribe>
#include <osgFX/Outline>
#include <osg/LineWidth>
#include <osgManipulator/CommandManager>
#include <osgManipulator/Dragger>
#include <osg/NodeVisitor>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>

#define M_PI (3.14159265358979323846)
// 常量
const double R = 6371000.0; // 地球平均半径(米)


class PickModelHandler :public osgGA::GUIEventHandler
{
public:
	PickModelHandler::PickModelHandler() {}
	PickModelHandler::~PickModelHandler() {}

	virtual bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*> (&aa);
		if (!viewer)return false;

		switch (ea.getEventType())
		{
		case osgGA::GUIEventAdapter::PUSH: // 鼠标按下
		{
			// 创建一个线段交集检测函数
			osgUtil::LineSegmentIntersector::Intersections intersections;
			osg::ref_ptr<osg::Node> targetNode = new osg::Node();
			osg::ref_ptr<osg::Group> parent = new osg::Group();

			if (viewer->computeIntersections(ea.getX(), ea.getY(), intersections))
			{
				// 获取相交模型点的节点
				osgUtil::LineSegmentIntersector::Intersection intersection = *intersections.begin();
				osg::NodePath& nodePath = intersection.nodePath;
				//targetNode = nodePath.back();

				// 判定所属的分组
				for (auto & path : nodePath)
				{
					if (path->getName() == "UAVTransform")
					{
#if 1
						if (parent = dynamic_cast<osg::Group*> (path))
						{
							if (targetNode = parent->getChild(0))
							{
								osgFX::Outline* ot = dynamic_cast<osgFX::Outline*>(targetNode.get());
								if (!ot)
								{
									osg::ref_ptr<osgFX::Outline> outline = new osgFX::Outline();
									outline->setColor(osg::Vec4(1, 1, 0, 1));
									outline->setWidth(5);
									outline->addChild(targetNode);
									parent->replaceChild(targetNode, outline);
								}
								else
								{
									osg::ref_ptr<osg::Group> sidParent = dynamic_cast<osg::Group*> (targetNode.get());
									if (sidParent)
									{
										targetNode = sidParent->getChild(0);
										parent->replaceChild(ot, targetNode);
									}
								}
							}
						}
#else
						// 点击节点切换高亮
						parent = dynamic_cast<osg::Group*> (nodePath[nodePath.size() - 3]); // 当前选择节点的父节点
						const int childNum = parent->getNumChildren();
						for (int i = 0 ; i < childNum; i++)
						{
							osg::ref_ptr<osg::Node> child = parent->getChild(i);
							if (osg::ref_ptr<osg::Group> childGroup = dynamic_cast<osg::Group*>(child.get()))
							{
								targetNode = childGroup->getChild(0);
								if (targetNode)
								{
									osgFX::Outline* ot = dynamic_cast<osgFX::Outline*>(child.get());
									if (!ot)
									{
										osg::ref_ptr<osgFX::Outline> outline = new osgFX::Outline();
										outline->setColor(osg::Vec4(1, 1, 0, 1));
										outline->setWidth(5);
										outline->addChild(targetNode);
										childGroup->replaceChild(targetNode, outline);
									}
									else
									{
										osg::Node::ParentList parentList = ot->getParents();
										osg::Node::ParentList::iterator itr = parentList.begin();
										(*itr)->replaceChild(ot, targetNode);
									}
								}
							}
						}
#endif
						break;
					}
				}
			}
			break;
		}
		case osgGA::GUIEventAdapter::DRAG:
		case osgGA::GUIEventAdapter::RELEASE:
			break;
		default:
			break;
		}
		return false;
	}

private:
	std::map<std::string, ILabel *>					m_map3DVideo;
};


osg::Geometry* createDirectLine(
	 osgEarth::GeoPoint& start,
	 osgEarth::GeoPoint& end)
{

	//osg::AutoTransform* at = new osg::AutoTransform;
	//at->setAutoScaleToScreen(true);  // 关键：启用屏幕空间自适应缩放

	osg::Billboard* billboard = new osg::Billboard;
	billboard->setMode(osg::Billboard::POINT_ROT_EYE);  // 始终朝向相机

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

void SystemFireProtControl_SendMsgToAll(const unsigned long long int    vehicle_id, const  int  value)
{
	if (vehicle_id == 0) {
		return;
	}

	qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl systemFireProtControl;
	systemFireProtControl.head.id = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SYSTEMINTERNALINTERFACEHEAD_ID_SYSTEMFIREPROTCONTROL;
	systemFireProtControl.head.time = QDateTime::currentMSecsSinceEpoch();
	systemFireProtControl.vehicle_id = vehicle_id;
	systemFireProtControl.fireProtType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtType_Spray;
	systemFireProtControl.fireProtOperType = qnzkna::DataManageSystemInternalInterface::SystemInternalInterface::SystemFireProtControl_FireProtOperType_FirePot_Spray_Count;
	systemFireProtControl.nValue = value;
	void *lParam = (void *)(&systemFireProtControl);
	if (lParam == nullptr) {
		return;
	}

	LY_AgriSprayPrescriptionInterface::Get().SendMsgToAll(IInterface::MSG_Command, 60001, lParam);
}


LY_AgriSprayPresCommandWidget::LY_AgriSprayPresCommandWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_AgriSprayPresCommandWidget)
	, m_playerTimer(new QTimer(this))
	, m_flyTimer(new QTimer(this))
	, currentIndex(0)
{
    ui->setupUi(this);
	setupUI();
	m_pIWeaponManageService = LY_AgriSprayPrescriptionActivator::Get().getPIWeaponManageService();
	
	m_pIZoneManageService = LY_AgriSprayPrescriptionActivator::Get().getPIZoneManageService();

 	m_strDataDir = tr2(LY_AgriSprayPrescription::Get().GetDataDir().data());

	localWeaponNameBindToId();
	InitWeaponCombobox();
	
	IEarth *IEarthPtr = IEarth::GetEarth();
	mapNode_ = (osgEarth::MapNode*)(IEarthPtr->GetPtr(IEarth::EPtrType_EarthMapNode));
	group_forD_ = new osg::Group();
	mapNode_->addChild(group_forD_);
	LoadIPIni();

	if (osgViewer::Viewer* viewer = (osgViewer::Viewer*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_OsgViewer)))
	{
		viewer->addEventHandler(new PickModelHandler);
	}
}
void LY_AgriSprayPresCommandWidget::LoadIPIni()
{
	m_UAVServerIPVec.clear();
	std::string strPath = "../data/conf/PlaySendToServer.ini";
	std::ifstream inFile(strPath);
	std::string oneLine;
	std::stringstream ss;
	QStringList nameList, ipList;
	while (getline(inFile, oneLine))
	{
		std::string name, ip, port;
		ss = std::stringstream(oneLine);
		ss >> name >> ip >> port;

		UAVServerIP stUAVServerIPVec;
		stUAVServerIPVec.strIP = QString::fromStdString(ip);
		stUAVServerIPVec.strName = QString::fromStdString(name);
		stUAVServerIPVec.nPort = QString::fromStdString(port).toInt();
		m_UAVServerIPVec.push_back(stUAVServerIPVec);
	}
}
void LY_AgriSprayPresCommandWidget::setupUI()
{

	//m_playerTimer->setTimerType(Qt::PreciseTimer);
	//m_playerTimer->setTimerType(Qt::PreciseTimer);


	ui->pysb_box->addItem(tr2("无人机"));
	ui->pysb_box->addItem(tr2("植保机"));

	ui->pyfs_box->addItem(tr2("点喷"));
	ui->pyfs_box->addItem(tr2("面喷"));

	//ui->pyls_box->addItem(tr2("5"));

	ui->pzlx_box->addItem(tr2("离心喷嘴"));
	ui->pzlx_box->addItem(tr2("压力喷嘴"));

	ui->pyzs_box->addItem(tr2("10000"));
	ui->pyls_box->addItem(tr2("60"));

	ui->zwgd_edit->setText("2");

	ui->pygd_box->addItem("5");

	// 查询当前激活农场
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Agri");
    m_currentFarm = ini.value("FarmID", "").toString();

	ui->spray_box->addItem(tr2("处方图规划"));
	ui->drop_slider->setMinimum(60);
	ui->drop_slider->setMaximum(200);
	ui->drop_edit->setText("60");
	ui->start_time->setDateTime(QDateTime::currentDateTime());
	ui->end_time->setDateTime(QDateTime::currentDateTime());

	ui->flight_distance_slider->setRange(0, 20);
	ui->boundary_slider->setRange(0, 20);
	ui->obstacles_slider->setRange(0, 20);

	ui->compute_box->addItem(tr2("算法1"));
	//ui->compute_box->addItem(tr2("算法2"));

	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("经度") << tr2("纬度") << tr2("喷药高度") << tr2("喷药幅宽") << tr2("喷药设备") << tr2("喷嘴类型") << tr2("喷药方式") << tr2("喷药开关") << tr2("转速") << tr2("流速") << tr2("雾滴大小") << tr2("喷嘴大小");
	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget->setRowCount(0);

	connect(ui->field_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_fieldBoxChanged(const int &)));
	connect(ui->spray_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_sprayBoxChanged(const int &)));
	connect(ui->spray_name_comBox, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_ComboBoxChanged(const int &)));
	connect(ui->compute_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_Compute_boxChanged(const int &)));
	connect(ui->route_diration_box, SIGNAL(currentIndexChanged(const int &)), this, SLOT(on_Route_diration_box(const int &)));
	connect(ui->drop_slider, &QSlider::valueChanged, this, &LY_AgriSprayPresCommandWidget::on_drop_valueChanged);
	//connect(ui->flight_direction_slider, &QSlider::valueChanged, this, &LY_AgriSprayPresCommandWidget::on_flight_direction_valueChanged);
	connect(ui->flight_distance_slider, &QSlider::valueChanged, this, &LY_AgriSprayPresCommandWidget::on_flight_distance_valueChanged);
	connect(ui->boundary_slider, &QSlider::valueChanged, this, &LY_AgriSprayPresCommandWidget::on_boundary_valueChanged);
	connect(ui->obstacles_slider, &QSlider::valueChanged, this, &LY_AgriSprayPresCommandWidget::on_obstacles_valueChanged);
	
	//connect(ui->field_tree_widget, &LY_AgriSprayTreeWidget::setupField, this, &LY_AgriSprayPresCommandWidget::addField);

	connect(ui->send_instruction, &QPushButton::clicked, this, &LY_AgriSprayPresCommandWidget::on_send_instruction);
	connect(ui->clearn_instruction, &QPushButton::clicked, this, &LY_AgriSprayPresCommandWidget::on_clearn_instruction);
	connect(ui->play_instruction, &QPushButton::clicked, this, &LY_AgriSprayPresCommandWidget::on_play_instruction);
	connect(ui->anlog_playback_btn, &QPushButton::clicked, this, &LY_AgriSprayPresCommandWidget::on_anlog_playback_btn);
	connect(ui->generation_task, &QPushButton::clicked, this, &LY_AgriSprayPresCommandWidget::on_generation_task);
	connect(ui->hide_route_line, &QPushButton::clicked, this, &LY_AgriSprayPresCommandWidget::on_hide_route_line);
	connect(ui->spay_control_btn, &QPushButton::clicked, this, &LY_AgriSprayPresCommandWidget::on_spay_control);
	connect(ui->start_task_btn, &QPushButton::clicked, this, &LY_AgriSprayPresCommandWidget::on_start_task_btn);

	

	loadWeather = new thread_loagWeather();
	loadWeather->working();
	connect(loadWeather, &thread_loagWeather::signal_weatherIsQueryed, this, &LY_AgriSprayPresCommandWidget::slot_isQueryed);

	//显隐气象条件对农药喷洒范围的影响
	connect(ui->checkBox, &QCheckBox::stateChanged, this, [&](int state_) {
		bool isShow = false;
		if (state_ == 0) {
			isShow = true;
		}
		for (const auto& zone : m_pInfluenceZoneVec) {
			m_pIZoneManageService->setCoverisShow(QString::fromLocal8Bit(zone.strID), isShow);
		}
	});

	ui->field_box->setCurrentIndex(0);
	ui->spray_box->setCurrentIndex(0);
}
void LY_AgriSprayPresCommandWidget::addItemData()
{

}
void LY_AgriSprayPresCommandWidget::on_fieldBoxChanged(const int &value)
{
	if (value < 0)
	{
		return;
	}
	ui->route_diration_box->clear();
	ui->route_diration_box->addItem(tr2("东西"));
	if (value == 0)
	{
		//ui->route_diration_box->addItem(tr2("东西"));
		//ui->route_diration_box->addItem(tr2("南北"));
	}
	else
	{
		//TODO:
		/*	ui->route_diration_box->addItem(tr2("左上"));
			ui->route_diration_box->addItem(tr2("左下"));
			ui->route_diration_box->addItem(tr2("右上"));
			ui->route_diration_box->addItem(tr2("右下"));*/
	}
	AgriFieldInfo fieldInfo = ui->field_box->currentData().value<AgriFieldInfo>();
	pescriptionVec.clear();
	pescriptionVec = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->GetSprayPrescription(fieldInfo.id);
	ui->spray_name_comBox->clear();

	int noNumber;
	ui->spray_name_comBox->addItem("");
	for (const auto &item : pescriptionVec) {
		ui->spray_name_comBox->addItem(item.map_name);
		noNumber++;
	}

	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = LY_AgriSprayPrescriptionActivator::Get().getPIZoneManageService()->GetAllZoneBase();

	// 清理所有高亮区域
	m_zoneDrawMap.clear();

	bool isCenter = false;
	m_boundPoings.clear();
	for (const auto &itrZone : zoneBaseVec)
	{
		if (0 == fieldInfo.area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			double curLon = itrZone.stRoundParameter.dLon;
			double curLat = itrZone.stRoundParameter.dLat;

			if (itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
				itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
				if (itrZone.stZonePointVec.size() > 0) {
					curLon = itrZone.stZonePointVec[0].dLon;
					curLat = itrZone.stZonePointVec[0].dLat;

					for (auto zone : itrZone.stZonePointVec)
					{
						QPointF pointF;
						pointF.setX(zone.dLon);
						pointF.setY(zone.dLat);
						m_boundPoings.push_back(pointF);
					}
				}
			}
			//TODO：
			//LY_SprayPrescriptionUntil::CalcPolygonArea(m_boundPoings, m_allArea);

			if (!isCenter)
			{
				//QString str = "CenterAt(" + QString::number(curLon, 'f', 8) + "," + QString::number(curLat, 'f', 8) + ")";
				//LY_AgriSprayPrescriptionInterface::Get().Script(str.toLocal8Bit());
				//isCenter = true;
			}

			// 添加高亮区域
			m_zoneDrawMap.insert(fieldInfo.area_id.toStdString(), itrZone);
		}
		QString obstacleStr = tr2("障碍");
		//m_obstaclePolygonPonigs.clear();
		//if (0 == obstacleStr.compare(QString::fromLocal8Bit(itrZone.strID)))
		//{
		//	double curLon = itrZone.stRoundParameter.dLon;
		//	double curLat = itrZone.stRoundParameter.dLat;

		//	if (itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
		//		itrZone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {
		//		if (itrZone.stZonePointVec.size() > 0) {
		//			curLon = itrZone.stZonePointVec[0].dLon;
		//			curLat = itrZone.stZonePointVec[0].dLat;

		//			for (auto zone : itrZone.stZonePointVec)
		//			{
		//				QPointF pointF;
		//				pointF.setX(zone.dLon);
		//				pointF.setY(zone.dLat);
		//				m_obstaclePolygonPonigs.push_back(pointF);
		//			}
		//		}
		//	}
		//	/*QPolygon polygon;
		//	for (const auto& point : m_obstaclePolygonPonigs) {
		//		polygon.append(QPoint(point.x(), point.y()));
		//	}
		//	m_obstaclePolygon = polygon;*/
		//	
		//}
	}
}
void LY_AgriSprayPresCommandWidget::zoneHighLight(QPainter &painter, IProj *pProj)
{
	if (m_spayMapshow == true)
	{
		m_zoneDrawMap.clear();
		return;
	}
	m_zoneDrawMap.iterate_query([&](const std::string& id, const qnzkna::ZoneManage::CZoneBase& zone) {

		if (zone.nZoneType == qnzkna::ZoneManage::CZoneBase::POLY ||
			zone.nZoneType == qnzkna::ZoneManage::CZoneBase::LINE) {

			

			QPen pen(Qt::white);
			pen.setWidth(5);
			painter.setPen(pen);
			painter.setBrush(Qt::NoBrush);
			QVector<QPoint> points;
			points.clear();
			for (auto & zonePoint : zone.stZonePointVec)
			{
				QPoint point1 = pProj->Jwd2Screen(zonePoint.dLon,
					zonePoint.dLat);
				points.push_back(point1);
			}
			QPolygon poly(points);
			painter.drawPolygon(poly);
		}
	});
}
void LY_AgriSprayPresCommandWidget::showEvent(QShowEvent *event)
{
	loadWeather->working();
	m_spayMapshow = true;
	isGeneratingCourse = false;
	vecFieldList.clear();
	LY_AgriSprayPrescriptionActivator::Get().getPIAgriFarmManagerService()->GetFieldInfoList(vecFieldList);

	ui->field_box->clear();
	ui->field_box->addItem("");
	for (auto& fieldInfo : vecFieldList)
	{
		// 匹配激活农场
		if (m_currentFarm == fieldInfo.farm_id)
		{
			ui->field_box->addItem(fieldInfo.name, QVariant::fromValue(fieldInfo));
		}
	}

	if (m_playerTimer == nullptr)
	{
		m_playerTimer = new QTimer(this);
	}
	m_flyTimer->setInterval(100);
	m_flyTimer->start();

	connect(m_playerTimer, &QTimer::timeout, this, &LY_AgriSprayPresCommandWidget::on_time_out);
	connect(m_flyTimer, &QTimer::timeout, this, &LY_AgriSprayPresCommandWidget::on_fly_time_out);
 }
 void LY_AgriSprayPresCommandWidget::hideEvent(QHideEvent *event)
{
	 QMutexLocker locker(&sMutex);
	 loadWeather->setEnd();
	 m_spayMapshow = false;
	 //isGeneratingCourse = false;

	 m_zoneDrawMap.clear();
	 //清除航点
	 m_gridPoints.clear();
	 //清除3D图
	 LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->clearSprayDiagramData();
	 LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->clearn3DflyData();

	 return;
	 //清除3D航线
	 del3DPathLine();
	 //清除等高线
	 Path3DLines.clear();
	 IEarth *pEarth = IEarth::GetEarth();
	 //清除飞机
	 if (m_playerTimer != nullptr)
	 {
		 m_playerTimer->stop();

		 if (pEarth == nullptr)
			 return;
		 auto itr2 = m_3DTakeOffMeshVec.begin();
		 for (; itr2 != m_3DTakeOffMeshVec.end(); itr2++)
		 {
			 IEarth *pEarth = IEarth::GetEarth();
			 pEarth->GetGraphics()->RemoveObject(*itr2);
			 *itr2 = nullptr;
			 delete *itr2;
		 }
		 m_3DTakeOffMeshVec.clear();

		 auto itr3 = m_map3DTextVec.begin();
		 for (; itr3 != m_map3DTextVec.end(); itr3++)
		 {
			 IEarth *pEarth = IEarth::GetEarth();
			 pEarth->GetGraphics()->RemoveObject(*itr3);
			 *itr3 = nullptr;
			 delete *itr3;
		 }
		 m_map3DTextVec.clear();


		 m_playerTimer->deleteLater();
		 m_playerTimer = nullptr;
	 }

	 osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	 if (mapNode) {
		 for (auto& line : m_allLines) {
			 mapNode->removeChild(line);
		 }
		 m_allLines.clear();
	 };

	 //清楚三菱锥
	 clearn3DCone();
	 //on_clearn_instruction();

}

void LY_AgriSprayPresCommandWidget::on_sprayBoxChanged(const int &value)
{
	for (int i = 0; i < ui->verticalLayout->count(); ++i) {
		QWidget *w = ui->verticalLayout->itemAt(2)->widget();
		if (w) {
		
			w->setVisible((value == 0) ? false:  true);
		}
	}

	ui->label->setText((value == 0) ? tr2("处方图名称") : tr2("规划名称"));

	for (int i = 0; i < ui->gridLayout_2->count(); ++i) {
		QWidget *w = ui->gridLayout_2->itemAt(0)->widget();
		if (w) {

			w->setVisible((value == 0) ? false : true);
		}
		QWidget *w1 = ui->gridLayout_2->itemAt(1)->widget();
		if (w) {

			w1->setVisible((value == 0) ? false : true);
		}
		QWidget *w2 = ui->gridLayout_2->itemAt(2)->widget();
		if (w) {

			w2->setVisible((value == 0) ? false : true);
		}
	}


}
void LY_AgriSprayPresCommandWidget::on_ComboBoxChanged(const int &index)
{
	on_clearn_instruction();
	AgriFieldInfo fieldInfo = ui->field_box->currentData().value<AgriFieldInfo>();
	for (const auto &item : pescriptionVec) {
		//ui->comboBox->addItem(item.map_name);
		if (item.map_name == ui->spray_name_comBox->currentText())
		{
			prescriptionModel = item;
			//处方图名称

			ui->edit_type->setText(LY_SprayPrescriptionUntil::CropTypeToString(static_cast<AgriSprayPrescriptionDecl::CropType>(item.crop_type)));
			ui->edit_pest_type->setText(LY_SprayPrescriptionUntil::PestCategoryToString(static_cast<PestCategory>(item.pest_category)));
			ui->edit_phase->setText(LY_SprayPrescriptionUntil::PestDiseaseToString(static_cast<PestDisease>(item.pest_disease)));
			ui->edit_width->setText(QString::number(item.pyfk));
			//ui->edit_width->setText(QString::number(item.pygd) + tr2("m"));

			//清除航线
			del3DPathLine();
			Path3DLines.clear();
			on_hide_route_line();


			LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->generateSprayPrescriptionDiagram(m_boundPoings, item.data_source_id, double(item.pyfk), item.map_level, false);

		

			break;
		}
	}
}
void LY_AgriSprayPresCommandWidget::on_spay_control()
{
	
	m_spayMapshow = !m_spayMapshow;
	//m_map3D.setImageShowOrHide_forDisPosition(m_spayMapshow);
	LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->showSprayDiagramData(!m_spayMapshow);
}
osgEarth::Symbology::Color LY_AgriSprayPresCommandWidget::hexToColor(const std::string& hexColor, float rgb_a/* = 1.0f*/)
{
	{
		unsigned int r, g, b;
		if (sscanf(hexColor.c_str(), "#%02X%02X%02X", &r, &g, &b) == 3)
		{
			return osgEarth::Symbology::Color(r / 255.0f, g / 255.0f, b / 255.0f, rgb_a); // 透明度默认为 1.0
		}
		return osgEarth::Symbology::Color(); // 失败返回默认颜色
	}
}

void LY_AgriSprayPresCommandWidget::on_anlog_playback_btn()
{

	LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->flyRouteSimulPlayback(ui->flight_height_edit->text().toDouble(),false);
	//return;
	//taskWidget = LY_AgriSprayPrescription::Get().GetTaskHeaderWidget();
	//taskWidget->setFieldArea("7.7");

	//for (int i = 0; i < prescriptionModel.leve_value.size(); i++)
	//{
	//	m_allValue += prescriptionModel.leve_value[i];
	//}
	//taskWidget->setWorkArea(QString::number(m_allValue, 'f', 1));
	//taskWidget->show();

	

#ifdef  UPLOADFLYLINE

	tempAirInstructionPoints = airInstructionPoints;
#else

#endif
	interpolateSegments();

	m_playerTimer->setInterval(1000); 
	m_playerTimer->start();

		std::map<int, double> windLevel2Speed;
		windLevel2Speed[0] = 0.01;
		windLevel2Speed[1] = 1;
		windLevel2Speed[2] = 2;
		windLevel2Speed[3] = 4;
		windLevel2Speed[4] = 6;
		windLevel2Speed[5] = 9;
		windLevel2Speed[6] = 11;
		windLevel2Speed[7] = 14;
		windLevel2Speed[8] = 18;
		windLevel2Speed[9] = 21;
		windLevel2Speed[10] = 25;
		windLevel2Speed[11] = 29;
		windLevel2Speed[12] = 33;
		windLevel2Speed[13] = 37;
		windLevel2Speed[14] = 42;

		double spsj = 4;
		double czsj = 4;
		double windLevel = ui->speed->text().toInt();
		QString windDir = ui->direction->text();
		windInfluencePesticideSprayingRange(windLevel2Speed[windLevel], windDir, spsj, czsj);
		create3DInfluencePolygon();
		createWindDirectionIcon(windDir);
}

void LY_AgriSprayPresCommandWidget::on_pushButton_clicked()
{
	LY_AgriSprayPrescriptionInterface::Get().SendMsgToAll(IInterface::MSG_Command, 50009, nullptr);
}

#ifdef UPLOADFLYLINE
void LY_AgriSprayPresCommandWidget::on_time_out()
{
	if (currentIndex >= tempAirInstructionPoints.size()) {
		//qDebug() << "所有POI处理完成，停止定时器";
		m_playerTimer->stop();
		currentIndex = 0;
		return;
	}
	QVariant  qv = ui->drone_box->itemData(ui->drone_box->currentIndex());
	QString strUAVID = qv.toString();
	const std::string &vehicleID = strUAVID.toStdString();
	for (auto IPItr : m_UAVServerIPVec)
	{
		std::string strName = IPItr.strName.toStdString();
		if (0 == strcmp(strName.c_str(), vehicleID.c_str()))
		{
			QHostAddress stQHostAddress;
			stQHostAddress.setAddress(IPItr.strIP);
			m_hostAddress = stQHostAddress;
			strIP = IPItr.strIP;
			nPort = IPItr.nPort;
			break;
		}
	}
    AirInstructionPoi &currentPoi = tempAirInstructionPoints[currentIndex];
	
	double h = ui->flight_height_edit->text().toDouble() * 1.0;


	if (currentPoi.point_status == POINT_STATUS_START)
	{
		double elevation = getElevation(currentPoi.startLon, currentPoi.centerLat);
		TelemetryInformation_t stTelemetryInformation_t;
		memset(&stTelemetryInformation_t, 0x00, sizeof(TelemetryInformation_t));
		stTelemetryInformation_t.lat = currentPoi.centerLat * 1e7;
		stTelemetryInformation_t.lng = currentPoi.startLon * 1e7;
		stTelemetryInformation_t.alt = elevation + h;
		stTelemetryInformation_t.relativeOriginHeigth = (elevation + h) * 100;
		m_UAVProtocol.SendCommandBytes(0x10, 0x10, reinterpret_cast<const uchar *>(&stTelemetryInformation_t), sizeof(stTelemetryInformation_t), m_hostAddress, nPort, nPort);
		
		//draw3DModel(currentPoi.startLon, currentPoi.centerLat, elevation + h);
		//draw3DFlyLabel(currentPoi.startLon, currentPoi.centerLat, elevation + h);
	}
	else if(currentPoi.point_status == POINT_STATUS_MIDDLE)
	{
		double elevation = getElevation(currentPoi.middleLon, currentPoi.centerLat);
		TelemetryInformation_t stTelemetryInformation_t;
		memset(&stTelemetryInformation_t, 0x00, sizeof(TelemetryInformation_t));
		stTelemetryInformation_t.lat = currentPoi.centerLat * 1e7;
		stTelemetryInformation_t.lng = currentPoi.middleLon * 1e7;
		stTelemetryInformation_t.alt = elevation + h;
		stTelemetryInformation_t.relativeOriginHeigth = (elevation + h) * 100;
		m_UAVProtocol.SendCommandBytes(0x10, 0x10, reinterpret_cast<const uchar *>(&stTelemetryInformation_t), sizeof(stTelemetryInformation_t), m_hostAddress, nPort, nPort);
		
		//draw3DModel(currentPoi.middleLon, currentPoi.centerLat, elevation + h);
		//draw3DFlyLabel(currentPoi.middleLon, currentPoi.centerLat, elevation + h);
	}
	else if (currentPoi.point_status == POINT_STATUS_END)
	{
		double elevation = getElevation(currentPoi.endLon, currentPoi.centerLat);
		TelemetryInformation_t stTelemetryInformation_t;
		memset(&stTelemetryInformation_t, 0x00, sizeof(TelemetryInformation_t));
		stTelemetryInformation_t.lat = currentPoi.centerLat * 1e7;
		stTelemetryInformation_t.lng = currentPoi.endLon * 1e7;
		stTelemetryInformation_t.alt = elevation + h;
		stTelemetryInformation_t.relativeOriginHeigth = (elevation + h) * 100;
		m_UAVProtocol.SendCommandBytes(0x10, 0x10, reinterpret_cast<const uchar *>(&stTelemetryInformation_t), sizeof(stTelemetryInformation_t), m_hostAddress, nPort, nPort);
		
		//draw3DModel(currentPoi.endLon, currentPoi.centerLat, elevation + h);
		//draw3DFlyLabel(currentPoi.endLon, currentPoi.centerLat, elevation + h);
	}
	currentIndex++; // 移动到下一个元素

}
#else
void LY_AgriSprayPresCommandWidget::on_time_out()
{

	if (currentIndex >= new_poiPaths.size()) {
		//qDebug() << "所有POI处理完成，停止定时器";
		m_playerTimer->stop();
		interpolateSegments();
		currentIndex = 0;

		return;
	}

	// 取出当前Poi并处理
	const Poi &currentPoi = new_poiPaths[currentIndex];
	
	QGeoCoordinate endP = QGeoCoordinate(currentPoi.lat, currentPoi.lon);
	if (currentIndex > 0)
	{
		double di = start.distanceTo(endP);
		flyingDiance += di;
		double process = QString::number(flyingDiance, 'f', 2).toDouble() / QString::number(allFlyLineditance, 'f', 2).toDouble();
	    workProcess = QString::number(process, 'f', 4).toDouble() * 100;
	}

	double elevation = getElevation(currentPoi.lon, currentPoi.lat);

	 double h = ui->flight_height_edit->text().toDouble() * 1.0;

	draw3DModel(currentPoi.lon, currentPoi.lat, elevation + h, 0);

	QString ID = tr2("编号:") + ui->drone_box->currentText();
	QString fly = tr2("类型:") + tr2("无人机");
	QString status = tr2("状态:") + tr2("飞行中");
	QString flySpeed = tr2("飞行速度:") + ui->flight_speed_edit->text();
	QString flyHeight = tr2("飞行高度:") + ui->flight_height_edit->text();
	QString workType = tr2("作业类型:") + tr2("喷药");
	QString workProcessStr = tr2("作业进度") + QString::number(workProcess) + "%";

	taskWidget->setTaskTypeName(tr2("喷药"));
	taskWidget->setTaskProcess(QString::number(workProcess));

	QString str = ID + "\n"
		+ fly + "\n" + status + "\n"
		 + flySpeed + "\n" + flyHeight + "\n"
		+ workType + "\n"
		+ workProcessStr;

	draw3DFlyLabel(currentPoi.lon, currentPoi.lat, elevation + h, str);

	casic_vector3d curPos(currentPoi.lon, currentPoi.lat, elevation + h);

	auto colorleve = [&](int level) {
		switch (level)
		{
		case 1:
			return "#ff8080";
		case 2:
			return "#ff4d4d";
		case 3:
			return "#ff0000";
		case 4:
			return "#cc0000";
		case 5:
			return "#800000";
		}
	};

	auto uavImageLevel = [&](int level) {
		switch (level)
		{
		case 0:
			return "Temp_Line/fly_1.png";
		case 1:
			return "Temp_Line/fly_1.png";
		case 2:
			return "Temp_Line/fly_2.png";
		case 3:
			return "Temp_Line/fly_3.png";
		case 4:
			return "Temp_Line/fly_4.png";
		case 5:
			return "Temp_Line/fly_5.png";
		}
	};
	if (isPlayOrderfilght)
	{
		//draw3DImage(currentPoi.lon, currentPoi.lat, elevation + h, uavImageLevel(currentPoi.alt));
		if (currentPoi.alt > 0)
		{
			QColor color(colorleve(currentPoi.alt));
			double spsj = 30 * qTan(20 / 2 * M_PI / 180) * 2;
			double czsj = 30 * qTan(30 / 2 * M_PI / 180) * 2;
			convert3DMap(curPos, 0, -90.0, 0, spsj, czsj, 15, color);
			
		}
		else
		{
			clearn3DCone();
		}
	}
	else
	{
		double spsj = 30 * qTan(20 / 2 * M_PI / 180) * 2;
		double czsj = 30 * qTan(30 / 2 * M_PI / 180) * 2;
		convert3DMap(curPos, 0, -90.0, 0, spsj, czsj, 15, Qt::red);
		//draw3DImage(currentPoi.lon, currentPoi.lat, elevation + h, uavImageLevel(0));
	}


	currentIndex++; // 移动到下一个元素
	start = endP;

}
#endif



void LY_AgriSprayPresCommandWidget::interpolateSegments()
{

	int nCalculatePoint = 1;
	new_poiPaths.clear();

	QGeoCoordinate QPoint1, QPoint2;
	auto itrPoint = m_origin_poiPaths.begin();
	for (; itrPoint != m_origin_poiPaths.end(); itrPoint++)
	{
		if ((itrPoint + 1) != m_origin_poiPaths.end())
		{
			double dLongStart(0), dLatStart(0), dLongEnd(0), dLatEnd(0), dHeightEnd(0), dHeight(0);
			dLongStart = itrPoint->lon;
			dLatStart = itrPoint->lat;
			dLongEnd = (itrPoint + 1)->lon;
			dLatEnd = (itrPoint + 1)->lat;
			QPoint1.setLongitude(dLongStart);
			QPoint1.setLatitude(dLatStart);
			QPoint2.setLongitude(dLongEnd);
			QPoint2.setLatitude(dLatEnd);

			double localdistance = QPoint1.distanceTo(QPoint2);

			double dbearing = QPoint1.azimuthTo(QPoint2);
			//步长
			double dDisOnce = ui->flight_speed_edit->text().toDouble() * 1.0;
			if (dDisOnce <= 0)
			{
				dDisOnce = 0.5;
			}
			double dNewLong(0), dNewLat(0);

			Poi stTelemetryInformation;
			memset(&stTelemetryInformation, 0x00, sizeof(stTelemetryInformation));
			stTelemetryInformation.lon = dLongStart;
			stTelemetryInformation.lat = dLatStart;
			stTelemetryInformation.alt = itrPoint->alt;

			new_poiPaths.push_back(stTelemetryInformation);

			nCalculatePoint++;

			double dlocaldistanceOnce = localdistance;
			double dLastDistance = localdistance;
			while (abs(localdistance - dDisOnce) > dDisOnce)
			{

				QPoint1 = QPoint1.atDistanceAndAzimuth(dDisOnce, dbearing);

				dLastDistance = localdistance;
				localdistance = QPoint1.distanceTo(QPoint2);

				dbearing = QPoint1.azimuthTo(QPoint2);

				Poi stTelemetryInformationTmp;
				memset(&stTelemetryInformationTmp, 0x00, sizeof(stTelemetryInformationTmp));
				stTelemetryInformationTmp.lon = QPoint1.longitude();
				stTelemetryInformationTmp.lat = QPoint1.latitude();
				//stTelemetryInformationTmp.alt = 0;
				new_poiPaths.push_back(stTelemetryInformationTmp);

				nCalculatePoint++;
				if (dLastDistance < localdistance)
				{
					break;
				}
			}
		}
		else
		{

			double dLongStart(0), dLatStart(0), dHeight(0);
			dLongStart = itrPoint->lon;
			dLatStart = itrPoint->lat;

			Poi stTelemetryInformation;
			memset(&stTelemetryInformation, 0x00, sizeof(stTelemetryInformation));
			stTelemetryInformation.lon = dLongStart;
			stTelemetryInformation.lat = dLatStart;
			stTelemetryInformation.alt = itrPoint->alt;
			new_poiPaths.push_back(stTelemetryInformation);
			nCalculatePoint++;

		}
	}
}

 
 void LY_AgriSprayPresCommandWidget::on_Compute_boxChanged(int value)
 {
	 compute_type = value;
 }
void LY_AgriSprayPresCommandWidget::addField(QString &text, QString & field_id)
{
	file_name = text;
}
void LY_AgriSprayPresCommandWidget::on_Route_diration_box(const int &value)
{
	planWay = value;
}
void LY_AgriSprayPresCommandWidget::on_send_instruction()
{
	addTaskSummary();
	//addFlyPoint();
	airInstructionPoints = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->generateInstructionPoint(ui->edit_width->text().toDouble());
	isInstructionStatus = true;
	
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);
	int index = 1;
	ui->tableWidget->setRowCount(airInstructionPoints.size());
	int row = 0;

	LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->DeleteTaskInstruction();
	for (const auto &poi : airInstructionPoints)
	{
		ui->tableWidget->insertRow(row);

		
		QTableWidgetItem *item1 = new QTableWidgetItem(QString::number(index));
		QTableWidgetItem *item2;
		AgriTaskInstruction  Instruction;
		switch (poi.point_status)
		{
		case POINT_STATUS_START:
		{
			item2 = new QTableWidgetItem(QString::number(poi.startLon, 'f', 7));
			Instruction.lon = QString::number(poi.startLon, 'f', 7).toDouble();
		}
		break;
		case POINT_STATUS_MIDDLE:
		{
			item2 = new QTableWidgetItem(QString::number(poi.middleLon, 'f', 7));
			Instruction.lon = QString::number(poi.middleLon, 'f', 7).toDouble();
		}
		break;
		case POINT_STATUS_END:
		{
			item2 = new QTableWidgetItem(QString::number(poi.endLon, 'f', 7));
			Instruction.lon = QString::number(poi.endLon, 'f', 7).toDouble();
		}
		break;
		default:
			break;
		}



		QTableWidgetItem *item3 = new QTableWidgetItem(QString::number(poi.centerLat, 'f', 7));
		QTableWidgetItem *item4 = new QTableWidgetItem(ui->flight_height_edit->text());
		QTableWidgetItem *item5 = new QTableWidgetItem(ui->edit_width->text());
		QTableWidgetItem *item6 = new QTableWidgetItem(ui->drone_box->currentText());
		QTableWidgetItem *item7 = new QTableWidgetItem(ui->pzlx_box->currentText());
		QTableWidgetItem *item8 = new QTableWidgetItem(ui->pyfs_box->currentText());
		QTableWidgetItem *item9 = new QTableWidgetItem(poi.isOpen ? tr2("打开") : tr2("关闭"));
		double value = QString::number(poi.openValue, 'f', 2).toDouble();
		double speed = ui->pyzs_box->currentText().toDouble() * value;
		QTableWidgetItem *item10 = new QTableWidgetItem(QString::number(value * speed, 'f', 2));
		double ls = ui->pyls_box->currentText().toDouble() * value;
		QTableWidgetItem *item11 = new QTableWidgetItem(QString::number(value * ls, 'f', 2));
		double wd = ui->drop_slider->value() * value;
		QTableWidgetItem *item12 = new QTableWidgetItem(QString::number(value * wd, 'f', 2));
		QTableWidgetItem *item13 = new QTableWidgetItem(QString::number(value , 'f', 2));

		ui->tableWidget->setItem(row, 0, item1);
		ui->tableWidget->setItem(row, 1, item2);
		ui->tableWidget->setItem(row, 2, item3);
		ui->tableWidget->setItem(row, 3, item4);
		ui->tableWidget->setItem(row, 4, item5);
		ui->tableWidget->setItem(row, 5, item6);
		ui->tableWidget->setItem(row, 6, item7);
		ui->tableWidget->setItem(row, 7, item8);
		ui->tableWidget->setItem(row, 8, item9);
		ui->tableWidget->setItem(row, 9, item10);
		ui->tableWidget->setItem(row, 10, item11);
		ui->tableWidget->setItem(row, 11, item12);
		ui->tableWidget->setItem(row, 12, item13);

		ui->tableWidget->item(row, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		if (item2 != nullptr)
		{
			ui->tableWidget->item(row, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		ui->tableWidget->item(row, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(row, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(row, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(row, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(row, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(row, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(row, 8)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(row, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		//ui->tableWidget->item(row, 11)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		//ui->tableWidget->item(row, 12)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		//ui->tableWidget->item(row, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		Instruction.spray_guid = prescriptionModel.guid;
		Instruction.point_status = poi.point_status;
		Instruction.lat = QString::number(poi.centerLat, 'f', 7).toDouble();
		Instruction.pygd = ui->flight_height_edit->text().toDouble();
		Instruction.pyfk = ui->edit_width->text().toDouble();
		Instruction.pysb = ui->drone_box->currentText();
		Instruction.pzlx = ui->pzlx_box->currentText();
		Instruction.pyfs = ui->pyfs_box->currentText();
		Instruction.pykg = poi.isOpen ? tr2("打开") : tr2("关闭");
		Instruction.rote_speed = value * speed;
		Instruction.flow_speed = value * ls;
		Instruction.droplet_size = value * wd;
		Instruction.penzui_size = QString::number(poi.openValue, 'f', 1).toDouble() * 100;
		LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->AddTaskInstruction(Instruction);

		index++;
		row++;
	}

}
void LY_AgriSprayPresCommandWidget::on_clearn_instruction()
{
	isInstructionStatus = false;
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);
	airInstructionPoints.clear();
	LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->DeleteTaskInstruction();
}
void LY_AgriSprayPresCommandWidget::on_play_instruction()
{
		
	isPlayOrderfilght = !isPlayOrderfilght;
	LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->playInstructionFlying(isPlayOrderfilght);
}
void LY_AgriSprayPresCommandWidget::on_start_task_btn()
{
	isTaskPlayIng = true;
}
void LY_AgriSprayPresCommandWidget::flowFlyDiance(double lon, double lat)
{
	if (m_origin_poiPaths.size() == 0) return;
	Poi firstPoint = m_origin_poiPaths[0];
	QGeoCoordinate planningStartP = QGeoCoordinate(firstPoint.lat, firstPoint.lon);
	QGeoCoordinate endP = QGeoCoordinate(lat, lon);

	double disance = endP.distanceTo(planningStartP);
	if (disance <= 5.0)
	{
		m_isStart = true;
	}
	if (!m_isStart) return;

	double di = start.distanceTo(endP);
	flyingDiance += di;

	qDebug() << "fly--disanc-:" << disance << flyingDiance;
	start = endP;

	double process = QString::number(flyingDiance, 'f', 2).toDouble() / QString::number(allFlyLineditance, 'f', 2).toDouble();
	workProcess = QString::number(process, 'f', 4).toDouble() * 100;
	
}
void LY_AgriSprayPresCommandWidget::clearFlightRoute()
{
    m_flightPath.clear();
    if (m_routeVertices) m_routeVertices->clear();
    if (m_routeGeometry) {
        m_routeGeometry->removePrimitiveSet(0, m_routeGeometry->getNumPrimitiveSets());
        m_routeGeometry->addPrimitiveSet(new osg::DrawArrays(
            osg::PrimitiveSet::LINE_STRIP, 0, 0));
    }
}
void LY_AgriSprayPresCommandWidget::setRouteVisibility(bool visible)
{
	if (m_routeGeode) {
		// 直接设置节点掩码来控制显隐
		m_routeGeode->setNodeMask(visible ? 0xFFFFFFFF : 0x0);
	}
	
}
void LY_AgriSprayPresCommandWidget::on_fly_time_out()
{

	QVariant  qv = ui->drone_box->itemData(ui->drone_box->currentIndex());
	QString strUAVID = qv.toString();
	qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t uavInfoTemp;
	bool isSuc = LY_AgriSprayPrescriptionActivator::Get().getPIUAVRealTimeStateManageService()->GetDefaultUAVRealTimeState(strUAVID.toStdString(), uavInfoTemp);
	if (isSuc)
	{
		double  lon = uavInfoTemp.telemetryInfo.position.lng - 0.000015/* - 0.1501885*/;
		double  lat = uavInfoTemp.telemetryInfo.position.lat/* + 0.2510522*/;
		double alt = getElevation(uavInfoTemp.telemetryInfo.position.lng, uavInfoTemp.telemetryInfo.position.lat) + 10;
		//double alt = uavInfoTemp.telemetryInfo.gpsHeigth * 10 + 18;
		//if (uavInfoTemp.telemetryInfo.powervoltage > 10.0)//电压
		{
			draw3DModel(lon, lat, alt, (uavInfoTemp.telemetryInfo.angleofRudder + 140.0));
		}
		
		flowFlyDiance(lon, lat);

		if (uavInfoTemp.strflightMode == "Auto" || uavInfoTemp.strflightMode == "RTL")
		{
			m_uavStatus = tr2("飞行中");
		}
		else if (uavInfoTemp.strflightMode == "Disarm")
		{
			m_uavStatus = tr2("已降落");
		}
		else
		{
			m_uavStatus = tr2("待飞行");
		}

#if 0
		QString ID = tr2("编号:") + ui->drone_box->currentText();
		QString fly = tr2("类型:") + tr2("无人机");
		QString status = tr2("状态:") + m_uavStatus;
		QString workType = tr2("作业类型:") + tr2("喷药");

		QString flySpeed = tr2("飞行速度:") + QString::number(uavInfoTemp.telemetryInfo.groundVelocity, 'f', 2) + "m/s";
		QString flyHeight = tr2("飞行高度:") + QString::number(uavInfoTemp.telemetryInfo.relativeOriginHeigth, 'f', 2) + "m";
		QString workProcessStr = tr2("作业进度") + QString::number(workProcess) + "%";

		QString str = ID + "\n"+ fly + "\n"
			+ status + "\n" + flySpeed + "\n" + flyHeight + "\n"
			+ workType + "\n"
			+ workProcessStr;

#else
		QString ID = tr2("编号:") + ui->drone_box->currentText();
		QString fly = tr2("类型:") + tr2("拖拉机");
		QString status = tr2("状态:") + tr2("作业中");
		QString flySpeed = tr2("作业速度:") + QString::number(uavInfoTemp.telemetryInfo.groundVelocity, 'f', 2) + "m/s";
		QString flyHeight = tr2("作业高度:") + QString::number(uavInfoTemp.telemetryInfo.gpsHeigth * 10, 'f', 2) + "m";
		//QString workProcessStr = tr2("作业进度") + QString::number(workProcess) + "%";
		QString str = ID + "\n" + fly + "\n"
			+ status + "\n" + flySpeed + "\n" + flyHeight;
#endif

		draw3DFlyLabel(lon, lat, alt, str);

		casic_vector3d curPos(lon, lat, alt);
		auto calculatedSpray = [&](double value) {
			static const std::unordered_map<double, QString> ratioMap = {
				{ 0.2, "#ff8080" },
				{ 0.4,"#ff4d4d" },
				{ 0.6, "#ff0000" },
				{ 0.8, "#cc0000" },
				{ 1.0, "#800000" }
			};

			// 查找输入数字对应的比例值
			auto it = ratioMap.find(value);
			if (it != ratioMap.end()) {
				return it->second;
			}
			return QString();
		};
		QGeoCoordinate uavstartP = QGeoCoordinate(lat, lon);


		for (auto it = airInstructionPoints.rbegin(); it != airInstructionPoints.rend(); ) {

			QGeoCoordinate startP = QGeoCoordinate(it->centerLat, it->startLon);
			QGeoCoordinate middleP = QGeoCoordinate(it->centerLat, it->middleLon);
			QGeoCoordinate endP = QGeoCoordinate(it->centerLat, it->endLon);

			double startDistance = uavstartP.distanceTo(startP);
			double middleDistance = uavstartP.distanceTo(middleP);
			double endDistance = uavstartP.distanceTo(endP);
			double value = QString::number(it->openValue, 'f', 1).toDouble();
	
			//clearn3DCone();
			if (startDistance <= 1.0 || middleDistance <= 1.0)
			{
				
			    it = decltype(it)(airInstructionPoints.erase(std::next(it).base()));
				SystemFireProtControl_SendMsgToAll(strUAVID.toInt(), value * 100);

				if (isTaskPlayIng)
				{
					QColor color(calculatedSpray(value));
					double spsj = 30 * qTan(20 / 2 * M_PI / 180) * 2;
					double czsj = 30 * qTan(30 / 2 * M_PI / 180) * 2;
					convert3DMap(curPos, 0, -90.0, 0, spsj, czsj, 15, color);
				}

			//	qDebug() << "===**==: start" << value * 100;

			}
			else if (endDistance <= 1.0)
			{
			//	qDebug() << "===**==: end" << value * 100;
				it = decltype(it)(airInstructionPoints.erase(std::next(it).base()));
				SystemFireProtControl_SendMsgToAll(strUAVID.toInt(), 0);
				clearn3DCone();
			}
			else
			{
				++it;
			}
		}
	}
}

void LY_AgriSprayPresCommandWidget::on_drop_valueChanged(int value)
{
	ui->drop_edit->setText(QString::number(value) + "um");
}
void LY_AgriSprayPresCommandWidget::on_hide_route_line()
{
	isGeneratingCourse = !isGeneratingCourse;
	LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->setFlylineStatus(isGeneratingCourse);
	//setRouteVisibility(isGeneratingCourse);
	//del3DPathLine();
	//Path3DLines.clear();
	//IEarth *pEarth = IEarth::GetEarth();
	////清除飞机
	//if (m_playerTimer != nullptr )
	//{
	//	m_playerTimer->stop();
	//
	//	if (pEarth == nullptr)
	//		return;
	//	auto itr2 = m_3DTakeOffMeshVec.begin();
	//	for (; itr2 != m_3DTakeOffMeshVec.end(); itr2++)
	//	{
	//		IEarth *pEarth = IEarth::GetEarth();
	//		pEarth->GetGraphics()->RemoveObject(*itr2);
	//		*itr2 = nullptr;
	//		delete *itr2;
	//	}
	//	m_3DTakeOffMeshVec.clear();

	//	auto itr3 = m_map3DTextVec.begin();
	//	for (; itr3 != m_map3DTextVec.end(); itr3++)
	//	{
	//		IEarth *pEarth = IEarth::GetEarth();
	//		pEarth->GetGraphics()->RemoveObject(*itr3);
	//		*itr3 = nullptr;
	//		delete *itr3;
	//	}
	//	m_map3DTextVec.clear();
	//}
	// osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	// if (mapNode) {
	//	 for (auto& line : m_allLines) {
	//		 mapNode->removeChild(line);
	//	 }
	//	 m_allLines.clear();
	// };
	////清楚三菱锥
	//clearn3DCone();
	//
}
void LY_AgriSprayPresCommandWidget::on_generation_task()
{
	double flyDIsance = 0.0;
	m_flyPoints.clear();
	std::vector<Poi>flyPoints;
	LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->generateSprayDiagramFlyLine(
		m_boundPoings, prescriptionModel.data_source_id, prescriptionModel.pyfk, ui->flight_height_edit->text().toDouble(),false, flyPoints, flyDIsance);
	m_flyPoints = flyPoints;
	ui->flight_point->setText(QString::number(flyPoints.size()));
	ui->flight_distance->setText(QString::number(flyDIsance, 'f', 2) + "m");
	//上传航线
	AddLineToServer(flyPoints);

}
void LY_AgriSprayPresCommandWidget::addTaskSummary()
{
	ui->work_address->setText(tr2("十三陵绿都盐田"));
	QDateTime currentTime = QDateTime::currentDateTime();
	QString timeString = currentTime.toString("yyyy-MM-dd hh:mm:ss");
	ui->work_time->setText(timeString);
	ui->work_people->setText(ui->edit_person->text());
	ui->work_type->setText(tr2("无人机"));
	ui->work_grow->setText(ui->edit_grow->text());
	ui->work_goal->setText(tr2("害虫"));
	ui->flight_weath->setText(QString::number(50));

	double allValue = 0;
	for (int i =  0; i < prescriptionModel.leve_value.size(); i++)
	{
		allValue += prescriptionModel.leve_value[i];
	}
	ui->work_all_area->setText(QString::number(allValue, 'f', 3));
}
void LY_AgriSprayPresCommandWidget::addFlyPoint()
{
	if (tempPoints.size() <= 0 || m_flyPointMap.size() <= 0 )
	{
		return;
	}
	airInstructionPoints.clear();
	double spaceing = prescriptionModel.pyfk * 1.0 *  (1.0 / 110000.0);
	//key：方格的y  vector:方格x的数组
	std::map<int, std::vector<int>> tempMap;
	for (int i = 0; i < tempPoints.size(); i++)
	{
		auto it = tempMap.find(tempPoints[i].ry());
		if (it != tempMap.end())
		{
			it->second.push_back(tempPoints[i].rx());
		}
		else
		{
			std::vector<int> temp;
			temp.push_back(tempPoints[i].rx());
			tempMap.insert(std::make_pair(tempPoints[i].ry(), temp));
		}
	}
	//存储方格的方向  y: 方向  
	std::map<int, bool> dirctionMap;
	bool isLeft = true; //从左往右
	int y = 0;
	for (int i = 0; i < tempPoints.size(); i++)
	{
		if (y != 0)
		{
			if (y != tempPoints[i].ry()) {
				isLeft = !isLeft;
				y = tempPoints[i].ry();
			}
		}
		else
		{
			y = tempPoints[i].ry();
		}
		dirctionMap[tempPoints[i].ry()] = isLeft;
	}
	
	auto calculatedSpray = [&](int value) {
		static const std::unordered_map<int, double> ratioMap = {
			{ 1, 0.2 },
			{ 2, 0.4 },
			{ 3, 0.6 },
			{ 4, 0.8 },
			{ 5, 1.0 }
		};

		// 查找输入数字对应的比例值
		auto it = ratioMap.find(value);
		if (it != ratioMap.end()) {
			return it->second;
		}
		return 0.0;
	};

	for (const auto &map : tempMap)
	{
		auto dirction = dirctionMap.find(map.first);
		if (dirction != dirctionMap.end())
		{
			std::vector<std::vector<int>> groups = m_map3D.groupConsecutiveNumbers(map.second, dirction->second);

			std::vector<AirInstructionPoi> InstructionPoiVec;
			InstructionPoiVec.clear();

			for (const auto& group : groups) {

				double centerLat;
				double endLon;
				double mindleLon;

				int index = 0;
				Poi tempP;// 记录当前方格的等级
				tempP.alt = 0.0;

				
				for (const auto& subGroup : group)
				{

					QPoint startPath(subGroup, map.first);
					Poi p = getCoordinateWithPoint(startPath);
					centerLat = p.lat;

					AirInstructionPoi airPoi;
					airPoi.centerLat = centerLat;

					double lon;
					if (dirction->second)
					{
						lon = p.lon - spaceing / 2;
					}
					else
					{
						lon = p.lon + spaceing / 2;
					}
					if (index == 0)
					{
						airPoi.startLon = lon;
						airPoi.height = p.alt;
						airPoi.openValue = calculatedSpray(p.alt);
						airPoi.point_status = POINT_STATUS_START;
						airPoi.isOpen = true;
					}
					else
					{
						airPoi.point_status = POINT_STATUS_MIDDLE;
						airPoi.isOpen = true;
						
						if (tempP.alt != p.alt) //不同等级  调整飞机喷洒农药的量
						{
							airPoi.openValue = calculatedSpray(p.alt);
							airPoi.middleLon = lon;
							airPoi.height = p.alt;
						}
						else
						{
							continue;
						}
						
					}
					tempP = p;
					index++;
					airInstructionPoints.push_back(airPoi);
					InstructionPoiVec.push_back(airPoi);
				}
				QPoint endPtah(group.back(), map.first);
				Poi p = getCoordinateWithPoint(endPtah);
				AgriSprayPrescriptionDecl::AirInstructionPoi airPoi;
				airPoi.point_status = POINT_STATUS_END;
				airPoi.isOpen = false;
				if (dirction->second)
				{
					airPoi.endLon = p.lon + spaceing / 2;
				}
				else
				{
					airPoi.endLon = p.lon - spaceing / 2;
				}
				airPoi.centerLat = centerLat;
				airInstructionPoints.push_back(airPoi);
				InstructionPoiVec.push_back(airPoi);
			}
		}
	}

}
Poi LY_AgriSprayPresCommandWidget::getCoordinateWithPoint(QPoint &point)
{
	auto it = m_flyPointMap.find(point);
	if (it != m_flyPointMap.end()) {
		Poi p = it->second;
		return p;
	}
}
void LY_AgriSprayPresCommandWidget::on_flight_distance_valueChanged(int value)
{

}
void LY_AgriSprayPresCommandWidget::on_boundary_valueChanged(int value)
{
	ui->boundary_value->setText(QString::number(value) + "m");
}
void LY_AgriSprayPresCommandWidget::on_obstacles_valueChanged(int value)
{
	
	ui->lineEdit->setText(QString::number(value) + "m");
}

//TODO：
void LY_AgriSprayPresCommandWidget::updataCurActiveFarm()
{
	//ui->field_tree_widget->init();
	if (m_pFieldInfo != nullptr && m_pFieldInfo->id != nullptr)
	{
		AgriSprayPrescriptionVec vec = LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->GetSprayPrescription(m_pFieldInfo->id);
		for (const auto &item : vec) {
			if (item.field_id == m_pFieldInfo->id)
			{
				prescriptionModel = item;
				break;
			}
		}
	}
}
LY_AgriSprayPresCommandWidget::~LY_AgriSprayPresCommandWidget()
{
    delete ui;


}
double LY_AgriSprayPresCommandWidget::getElevation(double lon, double lat)
{
	//根据经纬度获取高度
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	osgEarth::ElevationQuery eq(mapNode->getMap());
	osgEarth::GeoPoint point(mapNode->getMap()->getSRS(), lon, lat);

	double elevation = 0.0;
	if (eq.getElevation(point, elevation)) {
		//qDebug() << "Elevation at (" << currentPoi.lat << ", " << currentPoi.lon
		//	<< ") is " << elevation << " meters.";
		return elevation;
	}
	else {
		elevation = 150;
	}
	return elevation;
}

void LY_AgriSprayPresCommandWidget::localWeaponNameBindToId()
{
	if (m_pIWeaponManageService == nullptr)
	{
		return;
	}

	m_SystemSysNameAndID.clear();
	qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
	m_pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap);
	for (auto sysItr : weaponSystemMap)
	{
		if (sysItr.second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_UAV)
		{
			auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
			for (auto weapon : *weaponComPonentMap)
			{
				m_SystemSysNameAndID.insert(std::make_pair(weapon.second.getComponentID(), weapon.second.getComponentName()));
			}
		}
	}
}

void LY_AgriSprayPresCommandWidget::InitWeaponCombobox()
{
	ui->drone_box->clear();

	if (m_SystemSysNameAndID.size() == 0)
	{
		return;
	}

	int nNo = 0;
	for (auto itr : m_SystemSysNameAndID)
	{
		ui->drone_box->insertItem(nNo, QString::fromStdString(itr.second));
		ui->drone_box->setItemData(nNo, QString::fromStdString(itr.first));
		nNo++;
	}
}

void LY_AgriSprayPresCommandWidget::AddLineToServer(const std::vector<Poi>& LinePoints)
{
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	CAirTrafficLine stLine;
	stLine.nID = m_pIZoneManageService->GetAirTrafficLineMaxID();
	QString strTmp = tr2("HX") + QString::number(stLine.nID);
	strncpy(stLine.strName, strTmp.toLocal8Bit(), MAX_AIR_LEN_1);
	if (ui->drone_box->currentIndex() == -1)
	{
		return;
	}
	QVariant  qv = ui->drone_box->itemData(ui->drone_box->currentIndex());
	QString strUAVID = qv.toString();
	strncpy(stLine.strUAVID, strUAVID.toLocal8Bit(), MAX_AIR_LEN_1);

	int nHeight = ui->flight_height_edit->text().toInt();
	int nSpeed = ui->flight_speed_edit->text().toInt();
	int nPontID = m_pIZoneManageService->GetAirTrafficLinePointMaxID();

	int nCount = 0;
	for (auto itr :LinePoints)
	{
		CAirTrafficLinePoint stPoint;
		stPoint.nNo = nPontID + nCount;
		stPoint.nLineID = stLine.nID;
		stPoint.dLon = itr.lon;
		stPoint.dLat = itr.lat;
		stPoint.dAlt = nHeight;
		stPoint.dSpeed = nSpeed;
		stPoint.dHoverTime = 0 ;

		stLine.LinePointVec.push_back(stPoint);
		nCount++;
	}

	m_pIZoneManageService->AddAirTrafficLine(stLine);
}
void LY_AgriSprayPresCommandWidget::draw2DPathLine(QPainter &painter, IProj *pProj)
{

	QMutexLocker locker(&sMutex);
	if (prescriptionModel.data_source_id.length() > 0)
	{
		
		m_gridPoints.clear();

		//for (const auto &path : m_paths) {
		//	auto it = m_pointMap.find(path);
		//	if (it != m_pointMap.end()) {
		//		QPoint point1 = pProj->Jwd2Screen(it->second.lon,
		//			it->second.lat);
		//		m_gridPoints.push_back(point1);
		//	}
		//}
		for (const auto &it : m_flyPoints)
		{
			QPoint point1 = pProj->Jwd2Screen(it.lon,
				it.lat);
			m_gridPoints.push_back(point1);
		}

		//绘制二维处方图
		if (m_spayMapshow)
		{
			//m_map3D.create2DGridDrawing(painter, pProj);
			LY_AgriSprayPrescriptionActivator::Get().getPIAgriSprayPrescriptionService()->create2DSprayDiagram(painter, pProj);
		}

		if (isGeneratingCourse)
		{
			QVector<QPoint> startPoints;
			QVector<QPoint> middlePoints;
			QVector<QPoint> endPoints;
			QPen pen(Qt::blue);
			pen.setWidth(3);
			painter.setPen(pen);

			for (int i = 0; i < m_gridPoints.size() - 1; ++i) {
				painter.drawLine(m_gridPoints[i], m_gridPoints[i + 1]);
			}

			// 绘制航线点标记
			painter.setPen(Qt::white);
			painter.setBrush(Qt::white);
			for (const QPointF &point : m_gridPoints) {
				painter.drawEllipse(point, 2, 2);
			}
			if (isInstructionStatus)
			{
				//开始的点
				painter.setPen(Qt::yellow);
				painter.setBrush(Qt::yellow);

				for (int i = 0; i < airInstructionPoints.size(); i++)
				{
					AirInstructionPoi it = airInstructionPoints[i];
					QPoint point1 = pProj->Jwd2Screen(it.startLon,
						it.centerLat);
					startPoints.push_back(point1);
				}
				for (const QPointF &point : startPoints) {
					//painter.drawEllipse(point, 2, 2);
					painter.drawText(point, tr2("开"));
					painter.drawEllipse(point, 2, 2);
				}

				//中间的点
				painter.setPen(Qt::white);
				painter.setBrush(Qt::white);

				for (int i = 0; i < airInstructionPoints.size(); i++)
				{
					AirInstructionPoi it = airInstructionPoints[i];
					QPoint point1 = pProj->Jwd2Screen(it.middleLon,
						it.centerLat);
					middlePoints.push_back(point1);
				}
				for (const QPointF &point : middlePoints) {
					//painter.drawEllipse(point, 2, 2);
					painter.drawEllipse(point, 2, 2);
					painter.drawText(point, tr2("调整"));
				}
				//结束的点
				painter.setPen(Qt::red);
				painter.setBrush(Qt::red);

				for (int i = 0; i < airInstructionPoints.size(); i++)
				{
					AirInstructionPoi it = airInstructionPoints[i];
					QPoint point1 = pProj->Jwd2Screen(it.endLon,
						it.centerLat);
					endPoints.push_back(point1);
				}
				for (const QPointF &point : endPoints) {
					//painter.drawEllipse(point, 2, 2);
					painter.drawText(point, tr2("关"));
					painter.drawEllipse(point, 2, 2);
				}
			}
		}
	}
}
//写入文件
void LY_AgriSprayPresCommandWidget::writeQPointsToFile(const QString& filePath, const QList<Poi>& points) {
	QFile file(filePath);
	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		QTextStream out(&file);
		for (const Poi& point : points) {
			out << QString::number(point.lon, 'f', 7) << " " << QString::number(point.lat, 'f', 7) << "\n"; // 简洁格式
		}
		file.close();
	}
}

void LY_AgriSprayPresCommandWidget::windInfluencePesticideSprayingRange(double windSpeed, const QString& windDir, double grid_l, double grid_w)
{
	std::map<QString, int> dirMap;
	dirMap[tr2("东")] = 0;
	dirMap[tr2("南")] = 1;
	dirMap[tr2("西")] = 2;
	dirMap[tr2("北")] = 3;
	dirMap[tr2("东南")] = 4;
	dirMap[tr2("东北")] = 5;
	dirMap[tr2("西南")] = 6;
	dirMap[tr2("西北")] = 7;

	if (dirMap.find(windDir) == dirMap.end()) {
		return;
	}

	double windv_v = windSpeed;
	int dirIndex = dirMap[windDir];
	if (dirIndex > 3) {
		windv_v = windSpeed / 1.414;
	}

	double h = ui->flight_height_edit->text().toInt();//无人机高度
	double t = std::sqrt(2 * h / 10);//农药喷洒下落时间
	double v_l = grid_l / 2 / t;//在无风状态下农药喷洒单方向的最大速度 水平
	double v_w = grid_w / 2 / t;//在无风状态下农药喷洒单方向的最大速度 垂直

	double m = 1e-3;//水滴的质量 kg

	//以风的方向为正方向
	//当风速为0时，物体有风阻
	double Fw_l = 0.5*1.225*v_l*v_l*0.47*3.14*1e-4;
	double Fw_w = 0.5*1.225*v_w*v_w*0.47*3.14*1e-4;
	//水平
	//double F1 = 0.5*1.225*(windv_v + v_l)*(windv_v + v_l)*0.47*3.14*1e-4;
	double F1 = 0.5*1.225*windv_v*windv_v*0.47*3.14*1e-4;
	double a1 = F1 / m;
	//边界逆风的位移
	double headwindOffset_l = -v_l*t + 0.5*a1*t*t;

	//double F2 = 0.5*1.225*(windv_v - v_l)*(windv_v - v_l)*0.47*3.14*1e-4;
	double F2 = 0.5*1.225*windv_v*windv_v*0.47*3.14*1e-4;
	double a2 = F2 / m;
	//边界顺风的位移
	double tailwindOffset_l = v_l*t + 0.5*a2*t*t;

	//垂直
	//double F3 = 0.5*1.225*(windv_v + v_w)*(windv_v + v_w)*0.47*3.14*1e-4;
	double F3 = 0.5*1.225*windv_v*windv_v*0.47*3.14*1e-4;
	double a3 = F3 / m;
	//边界逆风的位移
	double headwindOffset_w = -v_w*t + 0.5*a3*t*t;

	//double F4 = 0.5*1.225*(windv_v - v_w)*(windv_v - v_w)*0.47*3.14*1e-4;
	double F4 = 0.5*1.225*windv_v*windv_v*0.47*3.14*1e-4;
	double a4 = F4 / m;
	//边界顺风的位移
	double tailwindOffset_w = v_w*t + 0.5*a4*t*t;

	//偏移量：左负右正，上正下负
	double OffsetN = grid_w / 2;//北
	double OffsetS = grid_w / 2 * (-1);//南
	double OffsetW = grid_l / 2 * (-1);//西
	double OffsetE = grid_l / 2;//东
	if (dirIndex == 0 || dirIndex == 4 || dirIndex == 5) {//东风影响
		OffsetE = (-1)*headwindOffset_l;
		OffsetW = (-1)*tailwindOffset_l;
	}
	if (dirIndex == 1 || dirIndex == 4 || dirIndex == 6) {//南风影响
		OffsetS = headwindOffset_w;
		OffsetN = tailwindOffset_w;
	}
	if (dirIndex == 2 || dirIndex == 6 || dirIndex == 7) {//西风影响
		OffsetW = headwindOffset_l;
		OffsetE = tailwindOffset_l;
	}
	if (dirIndex == 3 || dirIndex == 5 || dirIndex == 7) {//北风影响
		OffsetN = (-1)*headwindOffset_w;
		OffsetS = (-1)*tailwindOffset_w;
	}

	m_pInfluencePolygonVec.clear();
	QGeoCoordinate leftCenterPoi;
	QGeoCoordinate rightCenterPoi;
	double minLon = 0, maxLon = 0;
	int index = 0;
	for (const auto& curRect : airInstructionPoints) {
		if (curRect.middleLon > 0) {
			continue;
		}
		else {
			if (index == 0) {
				minLon = curRect.startLon;
				index++;
				continue;
			}
			else {
				maxLon = std::max(minLon, curRect.endLon);
				minLon = std::min(minLon, curRect.endLon);
				leftCenterPoi = QGeoCoordinate(curRect.centerLat, minLon);
				rightCenterPoi = QGeoCoordinate(curRect.centerLat, maxLon);
				index = 0;
			}
		}

		double lcDistoCen = std::fabs(OffsetW);
		double tcDistoCen = std::fabs(OffsetN);
		double rcDistoCen = std::fabs(OffsetE);

		int angle1 = 0, angle2 = 0, angle3 = 0;
		if (OffsetW < 0) {
			angle1 = 270;
		}
		else {
			angle1 = 90;
		}
		if (OffsetN < 0) {
			angle2 = 180;
		}
		else {
			angle2 = 0;
		}
		if (OffsetE < 0) {
			angle3 = 270;
		}
		else {
			angle3 = 90;
		}
		auto lc = leftCenterPoi.atDistanceAndAzimuth(lcDistoCen, angle1);
		auto lt = lc.atDistanceAndAzimuth(tcDistoCen, angle2);//区域左上角
		
		auto rc = rightCenterPoi.atDistanceAndAzimuth(rcDistoCen, angle3);
		auto rt = rc.atDistanceAndAzimuth(tcDistoCen, angle2);//区域右上角

		double chang = lt.distanceTo(rt);
		double kuan = std::fabs(OffsetN - OffsetS);

		auto ld = lt.atDistanceAndAzimuth(kuan, 180);
		auto rd = rt.atDistanceAndAzimuth(kuan, 180);
		qnzkna::ZoneManage::ZonePointVec pv;
		pv.push_back(qnzkna::ZoneManage::ZonePoint(lt.longitude(), lt.latitude()));
		pv.push_back(qnzkna::ZoneManage::ZonePoint(rt.longitude(), rt.latitude()));
		pv.push_back(qnzkna::ZoneManage::ZonePoint(rd.longitude(), rd.latitude()));
		pv.push_back(qnzkna::ZoneManage::ZonePoint(ld.longitude(), ld.latitude()));
		m_pInfluencePolygonVec.push_back(pv);
	}
}

void LY_AgriSprayPresCommandWidget::create3DInfluencePolygon()
{
	for (const auto& zone : m_pInfluenceZoneVec) {
		m_pIZoneManageService->RemoveCoverZoneBase(QString::fromLocal8Bit(zone.strID));
	}

	qnzkna::ZoneManage::CZoneBase zone;
	zone.nZoneType = qnzkna::ZoneManage::CZoneBase::ZoneBaseType::POLY;
	zone.nProperty = 0;
	zone.nLineType = 1;
	zone.nFillType = 1;
	QColor col(180, 100, 0, 200);
	unsigned int uiV = (col.blue() & 0xFF) << 16 | (col.green() & 0xFF) << 8 | (col.red() & 0xFF);
	zone.nLineColor = uiV;
	zone.nAboveAlt = 8000;
	zone.nBottomhAlt = 6000;

	int num = 0;
	m_pInfluenceZoneVec.clear();
	for (const auto& polygon_t : m_pInfluencePolygonVec) {
		QString tempID = tr2("影响") + QString::number(++num);
		strcpy(zone.strID, tempID.toLocal8Bit().data());
		zone.stZonePointVec.clear();
		zone.stZonePointVec = polygon_t;
		m_pInfluenceZoneVec.push_back(zone);
		bool success = m_pIZoneManageService->AddCoverZoneBase(zone);
	}

}

void LY_AgriSprayPresCommandWidget::createWindDirectionIcon(const QString& windDir)
{
	std::map<QString, int> dirMap;
	dirMap[tr2("西")] = 0;
	dirMap[tr2("西北")] = 1;
	dirMap[tr2("北")] = 2;
	dirMap[tr2("东北")] = 3;
	dirMap[tr2("东")] = 4;
	dirMap[tr2("东南")] = 5;
	dirMap[tr2("南")] = 6;
	dirMap[tr2("西南")] = 7;
	int dirIndex = dirMap[windDir];

	while (group_forD_->getNumChildren()) {
		group_forD_->removeChild(group_forD_->getChild(0));
	}

	for (const auto& polygon_t : m_pInfluencePolygonVec) {
		if (polygon_t.size() < 4) continue;
		
		osg::ref_ptr<osg::Image> image = osgDB::readImageFile("../data/images/windDir.png");
		if (image) {
			image->flipVertical();
		}
		osgEarth::Annotation::ImageOverlay *imageOverlay = new osgEarth::Annotation::ImageOverlay(mapNode_, image);
		imageOverlay->setImage(image);
		//imageOverlay->setBounds(osgEarth::Annotation::Bounds(polygon_t[1].dLon, polygon_t[1].dLat, polygon_t[1].dLon + 4.0/110000, polygon_t[2].dLat));//位置
		imageOverlay->setBoundsAndRotation(osgEarth::Annotation::Bounds(polygon_t[1].dLon + 1.0 / 110000, polygon_t[1].dLat - 1.0 / 110000, polygon_t[1].dLon + 4.0 / 110000, polygon_t[2].dLat + 0.5 / 110000), dirIndex * 45);

		group_forD_->addChild(imageOverlay);
	}
}

void LY_AgriSprayPresCommandWidget::draw3DPathLine(std::vector<Poi> points, QColor pathColor, int lineWidth)
{

	del3DPathLine();
	Path3DLines.clear();

	IEarth* pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
	{
		return;
	}

	CCreateShape cs;

	std::vector<double> pdLon;
	pdLon.clear();
	std::vector<double> pdLat;
	pdLat.clear();
	std::vector<double> pdHeight;
	pdHeight.clear();
	for (int i = 0; i <points.size(); i++)
	{
		Poi point = points.at(i);
		pdLon.push_back(point.lon);
		pdLat.push_back(point.lat);
		double elevation = getElevation(point.lon, point.lat) + ui->flight_height_edit->text().toDouble() * 1.0;
		pdHeight.push_back(elevation);

		double pdLon1[2] = { point.lon ,point.lon };
		double pdLat1[2] = { point.lat, point.lat };
		double pdHeight1[2] = { elevation ,0 };
		IUserMesh1	*pUM1 = nullptr;
		CCreateShape cs1;
		SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
		if (pShp1 != nullptr)
		{
			pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
			if (pUM1 != nullptr)
			{
				QColor col(255, 255, 255, 100);
				pUM1->SetColor(col.rgba());
				pUM1->SetState(IUserMesh::LINESTIPPLE, 4);
				pUM1->SetState(IUserMesh::LINEWIDTH, 4);
				pUM1->SetState(GL_DEPTH_TEST, 1);
				Path3DLines.push_back(pUM1);
			}
		}
	}

	IUserMesh1 *pUM = nullptr;
	SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), pdHeight.size(),
		100, false, nullptr, pdHeight.data());
	if (pShp != nullptr || pEarth->GetGraphics() != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
		if (pUM != nullptr)
		{
			pUM->SetColor(pathColor.rgb());
			pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
			pUM->SetState(IUserMesh::LINEWIDTH, lineWidth);

			Path3DLines_plat.push_back(pUM);
		}
	}
}
void LY_AgriSprayPresCommandWidget::del3DPathLine()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth ==  nullptr)
	{
		return;
	}
	for (int index = 0; index < Path3DLines.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines[index]);
	}
	for (int index = 0; index < Path3DLines_plat.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines_plat[index]);
	}

	return;
}

//绘制飞机模型
void LY_AgriSprayPresCommandWidget::draw3DModel(double lon, double lat, double alt, double jiaodu)
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr) return;

	osgEarth::MapNode* mapNode = static_cast<osgEarth::MapNode*>(
		IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (!mapNode) return;

	// ==== 模型初始化（首次调用时执行）====
	if (!m_modelTransform)
	{
		std::string modelNameStr = "newFlyModel";
		m_model = osgDB::readNodeFile("../data/maps_beijing_changping_shisanling/OSGB/" + modelNameStr + ".ive");
		if (!m_model) {
			qDebug() << "Failed to load model!";
			return;
		}

		// 动画设置
		osgAnimation::BasicAnimationManager* animManager =
			dynamic_cast<osgAnimation::BasicAnimationManager*>(m_model->getUpdateCallback());
		if (animManager && !animManager->getAnimationList().empty()) {
			animManager->playAnimation(animManager->getAnimationList()[0], 1.0, 0.0);
		}

		// 创建变换节点
		m_modelTransform = new osg::MatrixTransform;
		m_modelTransform->setName("UAVTransform");
		m_modelTransform->addChild(m_model);
		createFlightRouteSystem(mapNode);

		// ==== 关键设置：确保模型显示在顶层 ====
		osg::StateSet* ss = m_modelTransform->getOrCreateStateSet();
		// 1. 设置渲染顺序为最高优先级（值越大越后渲染）
		ss->setRenderBinDetails(100, "RenderBin");
		// 2. 禁用深度测试（避免被地形遮挡）
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		//ss->setAttributeAndModes(new osg::Depth(osg::Depth::LESS, 0, 1, false));
		ss->setAttribute(new osg::PolygonOffset(1.0f, 1.0f));  // 深度偏移参数
		ss->setAttributeAndModes(new osg::Depth(osg::Depth::LEQUAL));

		mapNode->addChild(m_modelTransform);
	}

	// ==== 实时更新位置 ====
	// 1. 计算模型中心偏移（仅需计算一次）
	static osg::Vec3d modelCenterOffset = [&]() {
		osg::ComputeBoundsVisitor cbv;
		m_model->accept(cbv);
		return cbv.getBoundingBox().center();
	}();

	// 2. 计算世界坐标变换
	osg::Matrixd posMatrix;
	osg::EllipsoidModel* em = static_cast<osg::EllipsoidModel*>(
		IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
	em->computeLocalToWorldTransformFromLatLongHeight(
		osg::inDegrees(lat), osg::inDegrees(lon), alt, posMatrix);

	osg::Vec3d currentPosition = posMatrix.getTrans();

	// 3. 构建变换矩阵
	const double scale = 2.0 * 0.0001;
	osg::Matrixd matrix =
		osg::Matrixd::translate(-modelCenterOffset) *   // 中心点校正
		osg::Matrixd::rotate(osg::inDegrees(jiaodu), osg::Z_AXIS) * // 旋转
		osg::Matrixd::scale(scale, scale, scale) *      // 缩放
		posMatrix;                                      // 地理定位

	// 4. 更新变换节点
	m_modelTransform->setMatrix(matrix);

	updateFlightRoute(currentPosition);
}
// 创建航线绘制系统
void LY_AgriSprayPresCommandWidget::createFlightRouteSystem(osgEarth::MapNode* mapNode)
{
	// 创建航线几何体
	m_routeGeometry = new osg::Geometry;
	m_routeVertices = new osg::Vec3dArray;
	m_routeGeometry->setVertexArray(m_routeVertices);

	// 设置颜色（RGBA：红色带半透明）
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 0.7f)); // 主航线颜色
	m_routeGeometry->setColorArray(colors, osg::Array::BIND_OVERALL);

	// 设置绘制方式为连续线段
	m_routeDrawArrays = new osg::DrawArrays(osg::PrimitiveSet::LINE_STRIP, 0, 0);
	m_routeGeometry->addPrimitiveSet(m_routeDrawArrays);

	// 启用VBO以获得最佳性能
	m_routeGeometry->setUseDisplayList(false);
	m_routeGeometry->setUseVertexBufferObjects(true);
	
	// 创建航线节点
	m_routeGeode = new osg::Geode;
	m_routeGeode->addDrawable(m_routeGeometry);

	// 设置线宽
	osg::LineWidth* lineWidth = new osg::LineWidth(3.0f);

	// 设置航线渲染状态
	osg::StateSet* ss = m_routeGeode->getOrCreateStateSet();
	ss->setRenderBinDetails(101, "RenderBin"); // 在模型下方一层
	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	// 启用混合（透明效果）
	ss->setMode(GL_BLEND, osg::StateAttribute::ON);
	ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	ss->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	//设置深度偏移（确保在飞机上方）
	ss->setAttribute(new osg::PolygonOffset(-1.0f, -1.0f));
	//禁用深度写入（确保航线不会相互遮挡）
	ss->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS));
	// 禁用深度测试（确保航线始终可见）
	ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(lineWidth);
	// 添加到场景
	mapNode->addChild(m_routeGeode);
}
// 更新飞行航线
void LY_AgriSprayPresCommandWidget::updateFlightRoute(const osg::Vec3d& newPosition)
{
	// 1. 添加新位置
	m_flightPath.push_back(newPosition);

	// 2. 限制路径长度
	//if (m_flightPath.size() > MAX_POINTS) {
	//	m_flightPath.pop_front(); // 使用deque更高效
	//}

	// 3. 直接更新顶点数组（避免清除重建）
	m_routeVertices->resize(m_flightPath.size());
	for (size_t i = 0; i < m_flightPath.size(); ++i) {
		(*m_routeVertices)[i] = m_flightPath[i];
	}

	// 4. 标记顶点数组已修改
	m_routeVertices->dirty();

	// 5. 更新PrimitiveSet（如果顶点数量变化）
	if (m_routeDrawArrays && m_routeDrawArrays->getCount() != m_routeVertices->size()) {
		m_routeGeometry->removePrimitiveSet(0);
		m_routeDrawArrays = new osg::DrawArrays(
			osg::PrimitiveSet::LINE_STRIP, 0, m_routeVertices->size());
		m_routeGeometry->addPrimitiveSet(m_routeDrawArrays);
	}

	// 6. 更新包围盒
	m_routeGeometry->dirtyBound();

	// 7. 对于VBO，通知需要更新
	m_routeGeometry->dirtyDisplayList();
}

void LY_AgriSprayPresCommandWidget::draw3DImage(double lon, double lat, double alt, QString  uavImgStr)
{
	
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	auto itr2 = m_3DTakeOffMeshVec.begin();
	for (; itr2 != m_3DTakeOffMeshVec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_3DTakeOffMeshVec.clear();
	
	
	QColor col(255, 0, 0, 255);

	//char * imgStr = "Temp_Line/uav.png";
	const char * imgStr = uavImgStr.toStdString().c_str();
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(QUuid::createUuid().toRfc4122(), 16));
	QString strLable = "";
	if (m_aeroplaneMap.find(uid) == m_aeroplaneMap.end())
	{
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

		ILabel *pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(),
			lon, lat, alt, 100, 100, 0);
		if (pLabel != nullptr)
		{
			m_3DTakeOffMeshVec.push_back(pLabel);
			m_aeroplaneMap[uid] = pLabel;
		}
	
		
	/*	if (pVedioLabel == nullptr)
		{
			pVedioLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), "rtsp://192.168.1.30/videodevice",
				lon, lat, alt+5.0, 50, 50, 0);
		}

		if (pVedioLabel != nullptr)
		{
			pVedioLabel->SetPosition(lon, lat, alt + 5.0);*/
			//m_3DTakeOffMeshVec.push_back(VideopLabel);
			//m_videoplaneMap[uid] = VideopLabel;
	//	}

	}
}
void LY_AgriSprayPresCommandWidget::draw3DFlyLabel(double lon, double lat, double alt, QString str)
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

	double distance = 10.0;
	double azimuth = 135.0; 
	double elevation = 30.0;  

	double endLat, endLon, endHeight;
	calculateNewPosition(lat, lon, alt, distance, azimuth, elevation, endLat, endLon, endHeight);

	const osgEarth::SpatialReference* wgs84 = mapNode->getMapSRS()->getGeographicSRS();
	osgEarth::GeoPoint startPoint(wgs84, lon, lat, alt, osgEarth::ALTMODE_ABSOLUTE);
	osgEarth::GeoPoint endPoint(wgs84, endLon, endLat, endHeight, osgEarth::ALTMODE_ABSOLUTE);
	osg::Geometry* line = createDirectLine(startPoint, endPoint);

	// 创建组节点并添加线段
	osg::ref_ptr<osg::Group> lineGroup = new osg::Group;
	lineGroup->addChild(line);

	mapNode->addChild(lineGroup);
	m_allLines.push_back(lineGroup);

	auto itr2 = m_map3DTextVec.begin();
	for (; itr2 != m_map3DTextVec.end(); itr2++)
	{
		IEarth *pEarth = IEarth::GetEarth();
		pEarth->GetGraphics()->RemoveObject(*itr2);
		*itr2 = nullptr;
		delete *itr2;
	}
	m_map3DTextVec.clear();

	if (pEarth->GetGraphics() == nullptr || pEarth == nullptr) return;

	IText* mshpText = pEarth->GetGraphics()->CreateText(str.toLocal8Bit(), endLon, endLat, endHeight, NULL, 12,
		TRGB(255, 255, 255, 255), TRGB(11, 25, 60, 128), TRGB(91, 219, 64, 255), 4);

	m_map3DTextVec.push_back(mshpText);
}

void LY_AgriSprayPresCommandWidget::clearn3DCone()
{
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr)
		return;
	if (discoverZone.size() > 0)
	{
		auto itr2 = discoverZone.begin();
		for (; itr2 != discoverZone.end(); itr2++)
		{
			pEarth->GetGraphics()->RemoveObject(*itr2);
			*itr2 = nullptr;
			delete *itr2;
		}
		m_uav3DVec.clear();
		discoverZone.clear();
	}
}
void LY_AgriSprayPresCommandWidget::convert3DMap(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float angle_l, float angle_w, float dis_m, QColor zoneColor)
{
	if (heading > 360)
		heading -= 360;

	clearn3DCone();

	// TODO: 修改为设备真实视场
	discoverZone = createDetectionZone(angle_l, angle_w, dis_m);
	
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

	m_uav3DVec.push_back(uav3D);
}
std::vector<IUserMesh2*> LY_AgriSprayPresCommandWidget::createDetectionZone(float angle_l, float angle_w, float dis_m)
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
		const double PI = 3.14159265358979323846;
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
		//pUM->SetState(GL_DEPTH_TEST, 1);
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
		//pUM->SetState(GL_DEPTH_TEST, 1);
		reUserMesh2s.push_back(pUM);

	}
	return reUserMesh2s;
}
void LY_AgriSprayPresCommandWidget::slot_isQueryed()
{
	weatherData w = loadWeather->getCurWeather();
	ui->humtiy->setText(QString::number(w.humidity));
	ui->temp->setText(QString::number(w.teamperature));
	ui->speed->setText(QString::number(w.maxWindPower));
	ui->direction->setText(QString::fromStdString(w.winddirection));
}

