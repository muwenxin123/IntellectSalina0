#ifndef ROUTEPLANNINGDIALOG_H
#define ROUTEPLANNINGDIALOG_H

#include <QDialog>
#include "vector3.h"
#include "vector2.h"
#include <QGeoCoordinate>
#include "IProj.h"
#include "AreaSearchAlgorithm.h"
#include "CreateShape.h"
#include "map3d/IEarth.h"
#include "map3d/IGraphics.h"

#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/Common/TypeDefine_Node.h"
#include <QTimer>
#include "Protocol/XygsJL/sharedFile.h"
#include "RoutePlanningDisplayDialog.h"

#include "ICollaborativeSearchActivator.h"

#include "map3d/IEarth.h"
#include <map3d/IGraphics.h>

#ifndef PI
#define PI       3.14159265358979323846
#endif

typedef std::vector<QPointF> LLQPoints;
typedef std::pair<int, casic_vector3d> IndexPos;
class CPolygonCone;

class Vector3D {
public:
	Vector3D() {}
	Vector3D(double _x, double _y, double _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	Vector3D(const Vector3D& vec)
	{
		x = vec.x;
		y = vec.y;
		z = vec.z;
	}

	void setX(double _x) { x = _x; }
	double getX() { return x; }
	void setY(double _y) { y = _y; }
	double getY() { return y; }
	void setZ(double _z) { z = _z; }
	double getZ() { return z; }
	double getDis() {
		return std::sqrt(x * x + y * y + z * z);
	}
private:
	double x;
	double y;
	double z;
};

class Vector2D {
public:
	Vector2D() {
		x = 0.0;
		y = 0.0;
	}
	Vector2D(double _x, double _y)
	{
		x = _x;
		y = _y;
	}

	Vector2D(const Vector2D& vec)
	{
		x = vec.x;
		y = vec.y;
	}

	Vector2D operator+ (Vector2D a)
	{
		return Vector2D(a.getX() + this->getX(), a.getY() + this->getY());
	}
	void operator= (Vector2D a)
	{
		this->x = a.getX();
		this->y = a.getY();
	}

	void setX(double _x) { x = _x; }
	double getX() { return x; }
	void setY(double _y) { y = _y; }
	double getY() { return y; }
	double getDis() {
		return std::sqrt(x * x + y * y );
	}
	double getDis(Vector2D a) {
		return std::sqrt((a.getX() - x)*(a.getX() - x) + (a.getY() - y)*(a.getY() - y));
	}
private:
	double x;
	double y;
};

class IProj;

namespace Ui {
class RoutePlanningDialog;
}

class RoutePlanningDialog : public QWidget, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT

public:
    explicit RoutePlanningDialog(QWidget *parent = nullptr);
    ~RoutePlanningDialog();

	void init();

	void autoPlanning();

	virtual void Update(qnzkna::TaskManage::INebulaTaskManageService* sub);

	void UpdateActionData();

	double getGridLength(double height,double frame,double focal,double heading_ratio);

	double getGridWidth(double height, double frame, double focal, double side_ratio);

	double calcolateMaxHeight();

	double calcolateGrid();

	std::vector<casic_vector2d> calcTerrianObstacle(casic_vector2d start, casic_vector2d goal);

	void setAllObstacle(std::vector<std::vector<casic_vector2d>> no_flyAreaList, casic_vector2d start, casic_vector2d goal);

	void stagingPathPlanning(int uid, casic_vector3d start, casic_vector3d goal,double safedis, std::map<int, std::vector<casic_vector3d>>& stagePointPlanMap);

	void drawMarkRect(QPainter& painter, IProj* pProj);

	void initDialog();

	void setMarkPloy();

	QString radian2dms(double radian);

	void calcolateGridLinePoints();

	void sortGridSide(casic_vector3d leftUp, casic_vector3d rightDown, casic_vector3d startPoint);

	void drawGrid(QPainter &painter, IProj *pProj);

	void addNoFlyZone(QPointF point);

	void drawNoFlayZone(QPainter &painter, IProj *pProj);

	bool is_point_in_zone(const QPointF point, const std::vector<QPointF> polyPoints);

	void sparsePath(std::map<int, std::vector<casic_vector3d>>& AreaSearchPointMap);

	void drawStartPoint(QPainter& painter, IProj *pProj);

	void transToObstacleArea(const std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> GradMap);

	void drawObstacleArea(QPainter &painter, IProj *pProj);

	void drawObstacleZone(QPainter &painter, IProj *pProj);

	std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> createGridsNode();

	void setpbtnText();

	void addKeyAreaPoint(QPointF point);

	void finishaddKeyAreaPoint();

	void setKeyAreaXyHcost();

	void setKeyAreaNoTrans();

	void delKeyArea(QPointF mousePoint);

	void drawKeyAreaSelectPoly(QPainter& painter, IProj *pProj);

	void drawKeyArea(QPainter& painter, IProj *pProj);

	void drawCoverCube(QPainter& painter, IProj *pProj);

	void drawSelectPoint(QPainter& painter, IProj* pProj);

	void draw3DCoverCube();

	void draw3DSightViewCube(double dLon,double dLat,double dHeight);

	void draw3DTakfOffLandArea(double dLon, double dLat, double dHeight);

	void draw3DDropBoomArea(double dlatitude, double dlongitude, double dheight, double dradius);

	void draw3DFireLine(double dLon, double dLat, double dHeight, double heading, double speed);
	std::vector<casic_vector3d> generateLinePoints(double dLon,double dLat,double dHeight,double heading,double speed);

	void draw3DPathLine(std::map<int, std::vector<casic_vector3d>> &PointsVec, QColor pathColor, int lineWidth = 2);

	void del3DPathLine();

	void draw3DMarkArea();

	void draw3DGrid();

	void draw3DNoFlyArea();

	void drawCoverArea();

	void delCoverArea();

	void draw3DObstacleArea();

	void drawKeyArea3D();

	void drawKmeansArea();

	void deleteKmeansArea();

	std::vector<std::vector<double>> getClusterPoints(int clusterNum);

	std::vector<std::vector<casic_vector3d>> getClusterAreas(int clusterNum);

	int getClusterCount();

	double getFlyTimeCoef(double &suggestFlyTime);

	casic_vector2d getMaxPlanNodeIndex(std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>oneAreaPlanNode);

	casic_vector2d getMinPlanNodeIndex(std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>oneAreaPlanNode);

	std::vector<casic_vector3d> getEndPoints(casic_vector3d pos, double gridWidth);

	void CalculatePoint();

	void setTimeShow();

	void calSelectedSerialPoint();

	void delSelectedPoint(int pathIndex, IndexPos index_pos);

	void deleAllSelectedPoint();

	int getIndexInPath(int pathIndex, IndexPos indexpos);

	void drawNewAddPoint(QPainter& painter, IProj* pProj);

	void newPointsAssem2Path(int pathIndex, std::vector<casic_vector3d>& path, std::vector<casic_vector3d> newPoints);

	casic_vector3d newPointTransGridPos(casic_vector3d newPoint);
protected:
		virtual void changeEvent(QEvent *event);
		virtual void showEvent(QShowEvent *event);
		virtual void hideEvent(QHideEvent *event);
public slots:
    void on_pbtnPlanning_clicked();

	void on_pbtnKmeans_clicked();

	void on_push_GetArea_clicked();

	void on_pushAddNoFly_clicked();

	void on_pushDelNoFly_clicked();

	void on_pushSaveNoFly_clicked();

	void on_pushImportNoFly_clicked();

	void on_pushStartPoint_clicked();

	void on_cbximportanceLevel_Changed(const QString &);

	void on_pushAddImpArea_clicked();

	void on_pushDelImpArea_clicked();

	void on_pushSaveImpArea_clicked();

	void on_pushImportImpArea_clicked();

	void on_pushClearAll();

	void on_pbthDisplay_clicked();

	void on_timeOut();

	void LoadIPIni();

	void drawcoverslot();

	void on_pushArticModify_clicked();
	//加载按钮
	void on_pbtnLoadMission_Clicked();

	void on_pbtnRouteUpLoad_clicked();

public:
	bool			 m_show;
	casic_vector3d   curpos ;
	double			 cruiseHeight = 100;
	double           cruiseSpeed = 15;
	double           flyTime = 2000;
	double           angularSpeed = 150;
	double			 upSpeed = 1;
	double           safeDistance = 5;
    double           uavNum = 1;

	double			 pic_Length			=  36;
	double           pic_width			=  24;
	double           focal_dis			=  26;
	double           heading_overlap	=  0.7;
	double           side_overlap		=  0.5;
	double           GSD_max			=  0.4;
	double           p_Length			=  1920;

	casic_vector3d   leftup;
	casic_vector3d   rightdowm;
	double           coverRate = 0.8;

	int				 KeyAreaNum = 1;

	double           gridwidth = 10;

	double           areaLength = 0;
	double           areaWidth = 0;
	double           realCoverRate = 0;
	double           remainFlyTime = 0;
	double           cameraGridWidth = 0;

	bool		     isGetUp = false;
	bool			 isAddNoFly = false;
	bool		     isDelNoFly = false;
	bool			 isPushStartPoint = false;
	bool			 isAddKeyArea = false;
	bool			 isDelKeyArea = false;

	QPointF		     targetLeftUp;
	QPointF			 targetRightDown;
	std::vector<QPointF> gridLinesPoints;

	std::vector<QPointF> oneNoFlyZonePoint;
	std::vector<LLQPoints> NoFlyZonePoints;
	std::vector<LLQPoints> ObstacleAreaPoints;

	std::vector<QPoint> ObstactAreaXY;
	std::vector<casic_vector2d> obstacleArea;

	std::vector<casic_vector3d> gridSidePoint;

	std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> GridPointMap;

	std::vector<std::vector<double>>kmeansSamples;
	std::vector<int>kmeansLabels;
	std::vector<std::vector<int>> FloodFill_grid;
	std::vector<std::pair<std::vector<std::vector<double>>, int>>AreaGrids;
	std::vector<std::vector<std::pair<std::vector<std::vector<double>>, int>>>AllGrids;

	int clusterNum = 0;

	std::vector<QPointF> oneKeyAreaPoints;
	std::vector<LLQPoints> KeyAreaVec;
	std::map<int, std::pair<bool, int>> KeyAreas;
	std::map<int, std::vector<std::vector<double>>> keyXYPoint;

	std::vector<casic_vector3d> gridsidePoints;
	std::vector<IRenderableObject*> Path3DLines;

	IUserMesh1 *pMarkArea = nullptr;
	std::vector<IUserMesh1 *> m_3DMarkAreaVec;
	std::vector<IText*> m_3DMarkAreaTextVec;

	std::vector<IUserMesh1*> Grid3DLines;

	std::vector<IUserMesh1*> NoFlyArea3D;
	std::vector<IText*>NoFlyArea3DTextVec;

	std::vector<IUserMesh1*> coverArea3D;

	std::vector<IUserMesh1*> obstacleArea3D;

	std::vector<IUserMesh1*> keyArea3D;

	std::vector<IUserMesh1*> kmeansArea3D;

	std::vector<IUserMesh1*> coverCube3D;

	std::vector<IUserMesh1*> cover3DConeVec;
	 ;
	 std::vector<CPolygonCone*>m_CPolygonConeVec;

	std::vector<IUserMesh1*> takeoffLandVec;
	std::vector<IText*>takeoffLandTextVec;
	double takeoffLandGridWidth = 20;

	std::vector<IUserMesh2*> dropBoomAreaVec;
	std::vector<IText*> dropBoomAreaTextVec;

	IUserMesh2 *FireLinetrack = nullptr;

	casic_vector3d lastPos;

	casic_vector3d         stagingPoint;
	casic_vector3d         currTakeOffPoint;

	std::vector<casic_vector3d>ObstacleVec1;
	std::vector<casic_vector3d>ObstacleVec2;

	std::map<int, std::vector<casic_vector3d>>stagingPointList;
	std::map<int, std::vector<casic_vector3d>>rrtPlanPointList;

	std::map<int, std::vector<casic_vector3d>>AreaSearchPointMap;
	std::map<int, std::vector<casic_vector3d>>searchStartReturnPointMap;

	std::vector<std::vector<casic_vector2d>> allObstacleVec;

	std::map<casic_vector3d, std::vector<casic_vector3d>> _CoveredArea;
	std::vector<std::vector<casic_vector3d>>_CoveredAreaVec;

	CCalculatePointVec										m_vecCalculatePoint;
	unsigned int											m_vecCalculateSumTime;
	unsigned int											m_vecCalculateMaxNum;
	UAVServerIPVec											m_UAVServerIPVec;
	bool                                                    b_isStart = false;
	int                                                     m_nPlaySpeed = 1;
	bool                                                    b_startDraw = false;

	bool isModifyPoint = false;
	std::map<int, std::vector<std::pair<IndexPos, IndexPos>>> selectedFBPoints;
	std::map<int, std::vector<std::vector<IndexPos>>> selectedSerialPoints;
	std::vector<casic_vector3d> newAddPoints;
	std::vector<QPointF> pointSelects;
	QPointF selectFirstPoint;
	QPointF selectSecPoint;

	int ActivedActionID;
	CNeubulaAction currAction;
	std::vector<std::string> m_MarkIDVec;
	std::string currActionArea;
	std::string currActionIntelligence;
	CNeubulaMissionVec actionMissionVec;
	CNeubulaMission    currMission;

	CRelationMissionResourceVec currMissionPlatVec;

	CNebulaMissionPlatformLoadVec   currMissionLoadVec;

	QMap<QString, QString>									m_WeaponComponentMap;

private:
	int minAngle = -90;
	int maxAngle = -90;
	std::vector<casic_vector3d>      nearlyLenSide;
	IProj *m_pProj;
	bool m_pAutoPlanning = false;
	bool m_draw3DFirst = true;

	bool m_pClickabled = false;

public:

	inline double calSpyDis(double uavHei, double spyAngle) { return uavHei * tan((spyAngle + 90) / (180 / PI)); }
	inline std::vector<casic_vector3d> getNearlyLenSide() { return nearlyLenSide; }

public slots:
	void minAngleText_isChanged();
	void maxAngleText_isChanged();

public:
	RoutePlanningDisplayDialog* m_routePlanningPlayDialog;
	QTimer													*m_PlayTimer;
	double													 m_nPlayTime;
	UAVProtocol		  m_UAVProtocol;
	unsigned int	  _rate = 1;
	bool			  isStopClicked = false;

	qnzkna::TaskManage::INebulaTaskManageService*			m_pITaskManageService = nullptr;
	qnzkna::ZoneManage::IZoneManageService *m_pIZoneManageService;
	std::vector<casic_vector3d> flyRoadPointsVec;
	QString m_takeoffArea;
	QString m_enterLine;
	QString m_outLine;
	QString m_workArea;

	QStringList ZoneIDList;
private:
    Ui::RoutePlanningDialog *ui;

};

#endif
