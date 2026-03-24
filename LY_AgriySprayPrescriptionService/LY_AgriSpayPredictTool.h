#ifndef LY_AGRISPAYPREDICTTOOL_H
#define LY_AGRISPAYPREDICTTOOL_H

#include <QObject>
#include <QTimer>
#include "AgriDataAnalysisPredictAlgorithm.h"
#include "LY_AgriSPrayPrescriptionUntil.h"
#include "IWeaponManageService.h"
#include "IZoneManageService.h"
#include "map3d/IEarth.h"
#include "map3d\IGraphics.h"
#include "Zone\ZoneManagerInfo.h"
#include <QMutex>
#include "track.h"
#include <osg/BlendFunc>
#include <OpenThreads/ScopedLock>
#include "QCString.h"
#include <osgEarth/ElevationQuery>
#include "vector3.h"
#include <map3d/EarthEvent.h>
#include <QUuid>
#include <map>
#include <deque>
#include <QGeoCoordinate>
#include <QVector>
#include <QPoint>

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

class LY_AgriSpayPredictTool : public QObject
{
    //Q_OBJECT

public:
    explicit LY_AgriSpayPredictTool(QObject *parent = nullptr);
    ~LY_AgriSpayPredictTool();

	QString m_flyID = "";
	double m_flySpeed = 0.0;
	double m_flyHeight = 0.0;

	//true 肥力图 false 处方图
	bool m_isFerili= false;

	//设置总航线距离
	void setFlyDisance(double disance);

	void setOriginPoints(const std::vector<Poi> &points);
	//绘制3d航线
	void draw3DPathLine(std::vector<Poi> points, QColor pathColor, int lineWidth, double flyHeight);
	//飞行路径插点
	void interpolateSegments(double flySpeed, std::vector<Poi> m_origin_poiPaths);
	//模拟播放飞行
	//flyHeight 飞行高度， isLitering 是否巡飞
	void startFlying(double flyHeight, bool isLitering = false);
	//指令播放
	void startInstructionFlying(bool isPlayOrderfilght);
	//清理三维数据
	void clear3DLineAndModel();
	//隐藏/显示 飞机航线
	void setupFlylineStatus(bool isShow);


	UavStatus getFlyStatus();
private:

	void startFlying(QString flyID1, double flySpeed1, double flyHeight1, bool isPlayOrderfilght);
	//绘制3d飞机标牌
	void draw3DFlyLabel(double lon, double lat, double alt, QString str);
	//飞机连接线
	osg::Geometry* createDirectLine(osgEarth::GeoPoint& start,osgEarth::GeoPoint& end);
	//绘制三角锥
	void convert3DMap(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float angle_l, float angle_w, float dis_m, QColor zoneColor);
	//绘制飞机模型
	void draw3DModel(double lon, double lat, double alt, double jiaodu);
	// 创建航线绘制系统
	void createFlightRouteSystem(osgEarth::MapNode* mapNode);
	// 更新飞行航线
	void updateFlightRoute(const osg::Vec3d& newPosition);
	//清理航线
	void delete3DflyPathLine();
	//清理三菱锥
	void clearn3DCone();
	//清理模型
	void clearn3DModel();

private:

	QTimer *m_playerTimer = nullptr;
	int currentIndex = 0;
	//是否巡飞
	bool m_isLitering = false;
	//是否指令播放
	bool m_isPlayOrderfilght = false;

	std::vector<Poi> m_origin_points;
	UavStatus m_usvStatus;
	bool m_first = false;
	bool m_isend = false;
	std::vector<Poi> m_poiPaths;
	//航线
	std::vector<IRenderableObject*> Path3DLines_plat;
	//等高线  绘3d航线相关
	std::vector<IUserMesh1*> Path3DLines;
	//飞机连接线
	std::vector<osg::ref_ptr<osg::Group>> m_allLines;
	//飞机标牌
	std::vector<IText *>                     m_map3DTextVec;
	//三菱锥
	std::vector<Uav3D>                m_uav3DVec;
	std::vector<IUserMesh2*>	   discoverZone;
	//二维网格点
	//QVector<QPoint> m_gridPoints;
	//插入的点
	std::vector<Poi> new_poiPaths;

	// 模型变换节点
	osg::ref_ptr<osg::MatrixTransform> m_modelTransform;
	// 模型节点
	osg::ref_ptr<osg::Node> m_model;
	osg::Geometry* m_line;
	osg::ref_ptr<osg::Group> m_lineGroup; //连接线
	osg::ref_ptr<osg::Geode> m_routeGeode;              // 航线节点
	osg::ref_ptr<osg::Geometry> m_routeGeometry;        // 航线几何体
	osg::ref_ptr<osg::Vec3dArray> m_routeVertices;      // 航线顶点数组
	std::vector<osg::Vec3d> m_flightPath;               // 存储飞行路径点
	osg::ref_ptr<osg::DrawArrays> m_routeDrawArrays;


	//飞机标牌参数
	QGeoCoordinate startPoint;
	QGeoCoordinate end;
	double workProcess = 0.0;
	double allFlyLineditance = 0.0;
	//std::string flyStatus  = "";
	double flyingDiance = 0.0;
	int m_time = 0;
	int temp_temp = 0;

private slots:
	void on_time_out();

};

#endif // LY_AGRISPAYPREDICTTOOL_H
