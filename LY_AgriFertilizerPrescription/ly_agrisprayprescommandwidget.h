#ifndef LY_AGRISPRAYPRESCOMMANDWIDGET_H
#define LY_AGRISPRAYPRESCOMMANDWIDGET_H

#include <QWidget>
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"


#include "iostream"
#include <QMetaType>
#include "AgrisprayComboBox.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <vector>
#include <QPainter>
#include "IProj.h"
#include "QCString.h"
#include <osgEarth/ElevationQuery>
#include "map3d\IGraphics.h"
#include "vector3.h"
#include <QTimer>
#include "TinyXML\tinyxml.h"
#include <map3d/EarthEvent.h>
#include <QTimer>
#include <QUuid>
#include <map>
#include "AgriDataAnalysisPredictAlgorithm.h"
#include "Protocol/XygsJL/sharedFile.h"
#include "IWeaponManageService.h"
#include "IZoneManageService.h"
#include "ly_agrifertilizepescviewmodel.h"
#include "map3d/IEarth.h"
#include "thread_loagWeather.h"
#include "KrigingInterPolator.h"
#include <osgEarthSymbology/Geometry>
#include "Zone\ZoneManagerInfo.h"
#include <QMutex>
#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"

using namespace AgriSprayPrescriptionDecl;
using namespace AgriDataAnalysisPredictAlgorithm;
//


namespace Ui {
class LY_AgriSprayPresCommandWidget;
}

class LY_AgriSprayPresCommandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LY_AgriSprayPresCommandWidget(QWidget *parent = nullptr);
    ~LY_AgriSprayPresCommandWidget();

	void updataCurActiveFarm();
	void draw2DPathLine(QPainter &painter, IProj *pProj);

	void zoneHighLight(QPainter &painter, IProj *pProj);


protected:
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:

	thread_loagWeather* loadWeather = nullptr;
	QString m_currentFarm = nullptr;
	//是否显示指令状态
	bool isInstructionStatus = false;
	//区域边界
	std::vector<QPointF> m_boundPoings;
	//航线生成的path
	std::vector<QPoint> m_paths;
	//网格生成的map
	std::map<QPoint, AgriSoilTestingFertilityPoint, QPointFCompare> m_pointMap;
	//是否生成指令航线
	bool isPlayOrderfilght = false;

	//二维网格点
	QVector<QPoint> m_gridPoints;

	//存储飞机飞行带颜色方格的经纬度
	std::map<QPoint, AgriSoilTestingFertilityPoint, QPointFCompare> m_flyPointMap;
	//飞机飞行点的信息
	std::vector<AirInstructionPoi>airInstructionPoints;
	//盐田列表
	AgriFieldInfoModelVec vecFieldList;
	//处方图列表
	FertilizerPrescriptionVec pescriptionVec;
	//处方图对象
	FertilizerPrescription  prescriptionModel;
	//盐田对象
	AgriFieldInfoPtr		m_pFieldInfo;
	//是否要显示高亮
	//bool   m_showZoneHighLight = false;
	//盐田高亮map
	ThreadSafeMap<qnzkna::ZoneManage::CZoneBase>			m_zoneDrawMap;
	//处方图显隐
	bool m_spayMapshow = false;

	//分布图的点
	std::vector<AgriSoilTestingFertilityPoint> m_origin_poiPaths;
	//田名
	QString          file_name;
	QString          m_currentID = nullptr;
	//模拟播放定时器
	QTimer         *m_playerTimer = nullptr;
	
	//航线
	std::vector<IRenderableObject*> Path3DLines_plat;
	//等高线  绘图相关
	std::vector<IUserMesh1*> Path3DLines;
	std::vector<ILabel*>                    m_3DTakeOffMeshVec;
	std::map<QUuid, std::string>				m_mapLabelPath;
	std::map<QUuid, ILabel *>			m_aeroplaneMap;
	QCString      m_strDataDir;

	//三菱锥
	std::vector<Uav3D>                m_uav3DVec;
	std::vector<IUserMesh2*>	   discoverZone;

	//插入的点
	std::vector<AgriSoilTestingFertilityPoint> new_poiPaths;

	//上传航线当前index
	int                                              currentIndex = 0;
	//上传航线相关
	qnzkna::WeaponManage::IWeaponManageService*								m_pIWeaponManageService;

	qnzkna::ZoneManage::IZoneManageService*									m_pIZoneManageService;

	std::map<std::string, std::string>										m_SystemSysNameAndID;

	//是否点击隐藏航线
	bool isGeneratingCourse     = false;

	//TODO:
	//算法类型
	int compute_type = 0;
	//飞行方向
	int planWay;

	std::vector<qnzkna::ZoneManage::ZonePointVec> m_pInfluencePolygonVec;
	std::vector<qnzkna::ZoneManage::CZoneBase> m_pInfluenceZoneVec;

	osgEarth::MapNode *mapNode_ = nullptr;
	osg::Group *group_forD_ = nullptr;

private:

	void setupUI();

	void addItemData();

	QMutex sMutex;

	AgriSoilFertilityDistributeMapPtrVec  m_soilDistributeMapVec;
	AgriSoilFertilityDistributeMapPtr        m_currentDistributeMap = nullptr;

	AgriSoilTestingFertilityPointPtrVec   outSoilFertilityPointVec;

	//添加飞机指令的点
	void addFlyPoint();
	//添加任务摘要
	void addTaskSummary();
	//根据索引获取经纬度
	AgriSoilTestingFertilityPoint getCoordinateWithPoint(QPoint &point);

	//带颜色等级方格的索引
	std::vector<QPoint> tempPoints;

	osgEarth::Symbology::Color hexToColor(const std::string& hexColor, float rgb_a = 1.0f);
	//画航线
	void draw3DPathLine(std::vector<AgriSoilTestingFertilityPoint> points, QColor pathColor, int lineWidth);
	//删除航线
	void del3DPathLine();
	//插入点
	void interpolateSegments();
	//绘制飞机
	void draw3DImage(double lon, double lat, double alt);
	//清除三菱锥
	void clearn3DCone();
	//画三菱锥
	void convert3DMap(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float angle_l, float angle_w, float dis_m, QColor zoneColor);
	std::vector<IUserMesh2*>createDetectionZone(float angle_l, float angle_w, float dis_m);
	//查询高程
	double getElevation(double lon, double lat);

	LY_AgriFertilizePescViewModel *m_viewModel = nullptr;
	//上传航线相关
	void localWeaponNameBindToId();

	void InitWeaponCombobox();

	void AddLineToServer(const std::vector<AgriSoilTestingFertilityPoint>& LinePoints);


	void windInfluencePesticideSprayingRange(double windSpeed, const QString& windDir, double grid_l, double grid_w);
	void create3DInfluencePolygon();
	void createWindDirectionIcon(const QString& windDir);


private slots :

//请求网络
	void slot_isQueryed();

    void on_fieldBoxChanged(const int &value);
	void on_sprayBoxChanged(const int &value);
	void on_drop_valueChanged(int value);//航线方向
	//算法选择
	void on_Compute_boxChanged(int value);
	void on_flight_distance_valueChanged(int value);//航线距离
	void on_boundary_valueChanged(int value);//边界安全距离
	void on_obstacles_valueChanged(int value);//障碍安全距离
	void on_soilMap_boxChanged(const int &);
	//处方图box
	void spray_name_comBoxChange(const int &);
	
	void addField(QString &text, QString & field_id, QString &id);
	//隐藏处方图
	void on_spay_control();
	//生成航线
	void on_generation_task();
	//隐藏航线
	void on_hide_route_line();
	//清除指令
	void on_clearn_instruction();
	//生成指令
	void on_send_instruction();
	//航线模拟播放
	void on_anlog_playback_btn();
	//航线上传
	void on_pushButton_clicked();
	//指令模拟播放
	void on_play_instruction();

	void on_time_out();

private:
    Ui::LY_AgriSprayPresCommandWidget *ui;
};

#endif // LY_AGRISPRAYPRESCOMMANDWIDGET_H
