#ifndef LY_AGRISPRAYPRESCOMMANDWIDGET_H
#define LY_AGRISPRAYPRESCOMMANDWIDGET_H

#include <QWidget>
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"
#include "iostream"
#include <QMetaType>
#include <QGeoCoordinate>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <vector>
#include <QPainter>
#include "IProj.h"
#include "QCString.h"
#include <osgEarth/ElevationQuery>
#include "map3d\IGraphics.h"
#include "vector3.h"
#include "TinyXML\tinyxml.h"
#include <map3d/EarthEvent.h>
#include <QTimer>
#include <QUuid>
#include <map>
#include <deque>
#include "AgriDataAnalysisPredictAlgorithm.h"
#include "Protocol/XygsJL/sharedFile.h"
#include "IWeaponManageService.h"
#include "IZoneManageService.h"

#include "ly_sprayprescriptionuntil.h"
#include "map3d/IEarth.h"
#include "thread_loagWeather.h"
#include "KrigingInterPolator.h"
#include "ly_taskheaderwidget.h"
#include "Zone\ZoneManagerInfo.h"
#include <QMutex>
#include "UAVProtocol.h"
#include "track.h"
#include <osg/BlendFunc>
#include <OpenThreads/ScopedLock>

using namespace AgriSprayPrescriptionDecl;
using namespace AgriDataAnalysisPredictAlgorithm;
namespace Ui {
	class LY_AgriSprayPresCommandWidget;
}


struct Uav3D
{
	//unsigned int Uav3DMarkTrackShowTimes;
	//ITrack		*pTrack;
	//ILabel		*pLabel;
	IUserMesh2	*pSightingLine;
	IUserMesh2	*pSightingLine2;
	IUserMesh2	*pBombTrack;
	IUserMesh2	*pBombTrack2;
	IUserMesh1	*pUavMarkTrack;
	//QColor       colorSightingLine;
	//std::string	strLabel;
};
typedef struct UAVServerIP
{
	QString				strName;
	QString             strIP;
	int                 nPort;
	UAVServerIP()
	{
		strName = "";
		strIP = "";
		nPort = 0;
	}
} UAVServerIP;

typedef std::vector<UAVServerIP>	UAVServerIPVec;

//#define UPLOADFLYLINE

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

	HeatMap3D  m_map3D;
	thread_loagWeather* loadWeather = nullptr;
	QMutex sMutex;

	QString m_currentFarm = nullptr;

	//是否显示指令状态
	bool isInstructionStatus = false;
	//区域边界
	std::vector<QPointF> m_boundPoings;
	//障碍多边形
	//std::vector<QPointF>m_obstaclePolygonPonigs;
	QPolygon m_obstaclePolygon;
	//航线生成的path
	std::vector<QPoint> m_paths;
	std::vector<Poi>m_flyPoints;
	//网格生成的map
	std::map<QPoint, Poi, QPointFCompare> m_pointMap;
	//是否生成指令航线
	bool isPlayOrderfilght = false;

	//二维网格点
	QVector<QPoint> m_gridPoints;

	//存储飞机飞行带颜色方格的经纬度
	std::map<QPoint, Poi, QPointFCompare> m_flyPointMap;
	//飞机飞行点的信息
	std::vector< AirInstructionPoi>airInstructionPoints;
	std::vector< AirInstructionPoi>tempAirInstructionPoints; // 测试数据
	//指令开关航点
	std::vector<std::vector<AirInstructionPoi>> InstructionFlyPointsVec;
	//盐田列表
	AgriFieldInfoVec vecFieldList;
	//处方图列表
	AgriSprayPrescriptionVec pescriptionVec;
	//处方图对象
	AgriSprayPrescriptionModel  prescriptionModel;
	//盐田对象
	AgriFieldInfoPtr		m_pFieldInfo;
	//是否要显示高亮
	//bool   m_showZoneHighLight = false;
	//盐田高亮map
	ThreadSafeMap<qnzkna::ZoneManage::CZoneBase>			m_zoneDrawMap;
	//处方图显隐
	bool m_spayMapshow = false;

	//分布图的点
	std::vector<Poi> m_origin_poiPaths;
	//田名
	QString          file_name;
	//模拟播放定时器
	QTimer         *m_playerTimer = nullptr;
	bool             m_isRunningPlay = false;
	//飞机飞行定时器
	QTimer         *m_flyTimer = nullptr;
	UAVProtocol		 m_UAVProtocol;
	UAVServerIPVec   m_UAVServerIPVec;
	QHostAddress      m_hostAddress;
	QString             strIP;
	int                 nPort;
	
	//航线
	std::vector<IRenderableObject*> Path3DLines_plat;
	//等高线  绘图相关
	std::vector<IUserMesh1*> Path3DLines;
	std::vector<ILabel*>                    m_3DTakeOffMeshVec;
	std::map<QUuid, std::string>				m_mapLabelPath;
	std::map<QUuid, ILabel *>			m_aeroplaneMap;
	//std::map<QUuid, ILabel *>			m_videoplaneMap;
	QCString      m_strDataDir;

	//三菱锥
	std::vector<Uav3D>                m_uav3DVec;
	std::vector<IUserMesh2*>	   discoverZone;

	//插入的点
	std::vector<Poi> new_poiPaths;

	//上传航线当前index
	int                                              currentIndex = 0;
	//上传航线相关
	qnzkna::WeaponManage::IWeaponManageService*								m_pIWeaponManageService;

	qnzkna::ZoneManage::IZoneManageService*									m_pIZoneManageService;

	std::map<std::string, std::string>										m_SystemSysNameAndID;

	//是否点击隐藏航线
	bool isGeneratingCourse     = false;
	//任务是否执行
	bool isTaskPlayIng = false;

	//TODO:
	//算法类型
	int compute_type = 0;
	//飞行方向
	int planWay;

	std::vector<qnzkna::ZoneManage::ZonePointVec> m_pInfluencePolygonVec;
	std::vector<qnzkna::ZoneManage::CZoneBase> m_pInfluenceZoneVec;

	osgEarth::MapNode *mapNode_ = nullptr;
	osg::Group *group_forD_ = nullptr;

	//飞机连接线
	std::vector<osg::ref_ptr<osg::Group>> m_allLines;

	//飞机标牌
	std::vector<IText *>                     m_map3DTextVec;
	// 模型变换节点
	osg::ref_ptr<osg::MatrixTransform> m_modelTransform;
	// 模型节点
	osg::ref_ptr<osg::Node> m_model;  

	osg::ref_ptr<osg::Geode> m_routeGeode;              // 航线节点
	osg::ref_ptr<osg::Geometry> m_routeGeometry;        // 航线几何体
	osg::ref_ptr<osg::Vec3dArray> m_routeVertices;      // 航线顶点数组
	std::vector<osg::Vec3d> m_flightPath;               // 存储飞行路径点
	osg::ref_ptr<osg::DrawArrays> m_routeDrawArrays;
	
	//无人机实时飞行是否到第一个点
	int m_isStart = false;
	//无人机状态
	QString  m_uavStatus = "";
	double m_allArea = 0.0;    //总面积
	double m_allValue = 0.0;   //总使用面积
	LY_TaskHeaderWidget *taskWidget = nullptr;


private:

	void setupUI();
	void LoadIPIni();
	void addItemData();
	
	//添加飞机指令的点
	void addFlyPoint();
	//添加任务摘要
	void addTaskSummary();
	//根据索引获取经纬度
	Poi getCoordinateWithPoint(QPoint &point);

	//带颜色等级方格的索引
	std::vector<QPoint> tempPoints;

	osgEarth::Symbology::Color hexToColor(const std::string& hexColor, float rgb_a = 1.0f);
	//画航线
	void draw3DPathLine(std::vector<Poi> points, QColor pathColor, int lineWidth);
	//删除航线
	void del3DPathLine();
	//插入点
	void interpolateSegments();
	//绘制飞机
	void draw3DImage(double lon, double lat, double alt, QString uavImgStr);
	//绘制飞机标牌
	void draw3DFlyLabel(double lon, double lat, double alt, QString str);
	//绘制飞机模型
	void draw3DModel(double lon, double lat, double alt, double jiaodu);
	// 创建航线绘制系统
	void createFlightRouteSystem(osgEarth::MapNode* mapNode);
	// 更新飞行航线
	void updateFlightRoute(const osg::Vec3d& newPosition);
	//计算已飞航线
	void flowFlyDiance(double lon, double lat);
	//清除已飞航线
	void clearFlightRoute();
	//已飞航线显隐
	void setRouteVisibility(bool visible);
	//清除三菱锥
	void clearn3DCone();
	//画三菱锥
	void convert3DMap(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float angle_l, float angle_w, float dis_m, QColor zoneColor);
	std::vector<IUserMesh2*>createDetectionZone(float angle_l, float angle_w, float dis_m);
	//查询高程
	double getElevation(double lon, double lat);

	//上传航线相关
	void localWeaponNameBindToId();

	void InitWeaponCombobox();
	//上传航线到服务
	void AddLineToServer(const std::vector<Poi>& LinePoints);

	//临时测试 写入文件
	void writeQPointsToFile(const QString& filePath, const QList<Poi>& points);

	void windInfluencePesticideSprayingRange(double windSpeed, const QString& windDir, double grid_l, double grid_w);
	void create3DInfluencePolygon();
	void createWindDirectionIcon(const QString& windDir);

private slots :

//请求网络
	void slot_isQueryed();

    void on_fieldBoxChanged(const int &value);
	//喷药规则
	void on_sprayBoxChanged(const int &value);
	void on_drop_valueChanged(int value);//航线方向
	//算法
	void on_Compute_boxChanged(int value);
	void on_flight_distance_valueChanged(int value);//航线距离
	void on_boundary_valueChanged(int value);//边界安全距离
	void on_obstacles_valueChanged(int value);//障碍安全距离

	//选择方向
	void on_Route_diration_box(const int &value);
	//处方图box
	void on_ComboBoxChanged(const int &index);
	
	void addField(QString &text, QString & field_id);
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
	//任务执行
	void on_start_task_btn();

	//飞机实时数据
	void on_fly_time_out();
	//模拟播放定时器
	void on_time_out();

private:
    Ui::LY_AgriSprayPresCommandWidget *ui;
	ILabel *pVedioLabel = nullptr;
	//飞机标牌参数
	QGeoCoordinate start;
	QGeoCoordinate end;
	double workProcess = 0.0;
	double allFlyLineditance = 0.0;
	//std::string flyStatus  = "";
	double flyingDiance = 0.0;


};

#endif // LY_AGRISPRAYPRESCOMMANDWIDGET_H
