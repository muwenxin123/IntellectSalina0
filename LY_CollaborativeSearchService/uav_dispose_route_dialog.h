#ifndef UAV_DISPOSE_ROUTE_DIALOG_H
#define UAV_DISPOSE_ROUTE_DIALOG_H

#include <QDialog>

#include "vector3.h"
#include "QCString.h"
#include "DataManage/Mark/Mark.h"
#include "ICollaborativeSearchActivator.h"
#include "UAV_RouteLine/UAV_Dispose_Route_Base.h"
#include "IProj.h"

#include "map3d/IEarth.h"
#include <map3d/IGraphics.h>
#include "CreateShape.h"

#include "UAV_RouteLine/Dispose_targetAndUAVS.h"

#define fireAddHei 40
#define ANGLE_PITCH_FOR_M3	-20
#define DIS_POD_FOR_M3	100
#define ANGLE_PITCH_FOR_M2	-90
#define DIS_POD_FOR_M2	100

const bool isDispose_targetAndUAVS = true;

enum DISPOSE_WAY
{
	DISPOSE_THROW = 0,
	DISPOSE_FIRE,
};

enum SELECTPOS
{
	SELECTPOS_NO = 0,
	SELECTPOS_UAV_Throw,
	SELECTPOS_TAR_Throw,
	SELECTPOS_UAV_Fire,
	SELECTPOS_TAR_Fire,
};

struct dispose_pos_way
{
	casic_vector3d	uavPos;
	casic_vector3d	targetPos;
	DISPOSE_WAY		dispose_way;
	int				disOrHei;
	int				FavDir;
};

namespace Ui {
class UAV_DisPose_Route_Dialog;
}

class UAV_DisPose_Route_Dialog : public QDialog, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT

public:
    explicit UAV_DisPose_Route_Dialog(QWidget *parent = nullptr);
    ~UAV_DisPose_Route_Dialog();

	virtual void Update(qnzkna::TaskManage::INebulaTaskManageService* sub);

public:

	void init();

	void init_targets();

	void init_uav_online();

	void drawPoint(QPainter & painter, IProj * pProj);

	void drawUAV_MarkPoisnts(QPainter & painter, IProj * pProj);

	void drawUAV_Fire_3Test(QPainter & painter, IProj * pProj);

	bool deal_TrackInfo(int nLength, char* lpData);

	void clearAll();

public:

	void init_forDisPose_TargetAndUAVS();

	void init_UAVLists_ForDispose();

	void init_TargetList_forDispose();

	void setUAVOriPos(casic_vector3d pos);

	void bindSignToSlot();

public slots:

void slot_uavIDChange_throw();
void slot_uavIDChange_fire();

void slot_targetIDChange_throw();
void slot_targetIDChange_fire();

void slot_changedThrowHei();
void slot_changedFireDis();

void slot_changedIsRoute_throw();
void slot_changedIsRoute_Fire();

void slot_route_auto();
void slot_route_up_forUAVS();

void slot_addMarkID(QString newMarkID);
void slot_delMarkID(QString delMarkID);

void slot_changeMode_forThrow();
void slot_changeMode_forFire();

void slot_areachoose_forUAVS();

private:

	bool CalculateTerrainVisibility(QGeoCoordinate npos, QGeoCoordinate tpos);

	int GetDayOfYear(int year, int month, int day);

	void GetSolarAngle(int year, int month, int day, int hour, int min, double sec, double lon, double lat, int timeZone,
		int N, double &AzimuthAngle, double &ZenithAngle);

	double calBen_Sun_Angle(casic_vector3d targetPos);

	void LineTransToZone(std::vector<casic_vector3d> linePoints, std::vector<LLQPoints>& noflyZones);

	std::vector<casic_vector3d> routeLine(dispose_pos_way uavRouteInfo, std::vector<casic_vector3d> noflyLine);

	double calSpeed_dan(double dis, double hei);

	IUserMesh2 *CreateParabola(float v, int h, float angle = 0.0);

	vector<IUserMesh2*> CreateDetectionZone(float angle_l, float angle_w, float dis_m);
public:

	void draw3DDropBoomArea(double dlatitude, double dlongitude, double dheight, double dradius);

	void draw3DAreaOnSurface(double dlongitude, double dlatitude, double dheight, double dradius, IUserMesh2* userMesh2);

	void draw3DPathLine( std::vector<casic_vector3d>& PointsVec, QColor pathColor, int lineWidth);
	void draw3DPathLine_M3(std::vector<casic_vector3d>& PointsVec, QColor pathColor, int lineWidth);
	void draw3DPathLine_plan(std::map<std::string, std::vector<casic_vector3d>> &PointsVec, QColor pathColor, int lineWidth = 2);

	void del3DPathLine();

	void del3DstdIUserMesh1s(std::vector<IUserMesh1*>& mesh1s);
	void del3DstdIUserMesh2s(std::vector<IUserMesh2*>& mesh2s);

	void draw3DFireBoom_For_M3(casic_vector3d firePos, QColor zoneColor, int ZoneSize);

	void draw3DFireBoom_For_M3_throw_Zone(casic_vector3d firePos, int fireDis, int fireHei, double angle, QColor zoneColor, int lineSize);

	void draw3DFireBoom_For_M3_throw(casic_vector3d firePos, double heading, double speed, QColor zoneColor, int zoneSize);

	void change3DFireBoom_For_M3_throw(casic_vector3d firePos, double heading, QColor zoneColor);

	void draw3DDiscoverZone_for_M3(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float dis_m, QColor zoneColor);
	void change3DDiscoverZone_For_M3(casic_vector3d firePos, double heading, double fuyang, double pianzhuan, QColor zoneColor);

	void draw3DDiscoverZone_for_M2(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, float dis_m, QColor zoneColor);
	void change3DDiscoverZone_For_M2(casic_vector3d curPos, double heading, double fuyang, double pianzhuan, QColor zoneColor);

public slots:
void slot_selectUAV();
void slot_selectUAVPos();
void slot_selectMark();
void slot_selectMarkPos();
void slot_selectDan();
void slot_putin_windDir();
void slot_putin_windDir1();
void slot_enterWindDir();
void slot_putin_disOrHei();

void slot_route();

void slot_SelectPos_State_Change();

void slot_selectUAVPos_A2();
void slot_selectUAVPos_A3();

void slot_selectMark_A2();
void slot_selectMark_A3();

void slot_selectMarkPos_A2();
void slot_selectMarkPos_A3();

void slot_isRouteUAV_A2();
void slot_isRouteUAV_A3();

void slot_double_route();

void slot_route_Self();

void slot_routPath_up();

private:
	bool  isUseWind = false;
	int   wind_dir = 0;

	DISPOSE_WAY  dispose_way = DISPOSE_THROW;
	int   lineEdit_in_Value;

	int  UAVID_selected = 0;
	std::string  targetID_selected;
	int			targetIndex;

	casic_vector3d uavPos;
	casic_vector3d targetPos;

	std::vector<SystemMarkInfo> targetInfoVec;

	std::vector<casic_vector3d> searchPoints;

	std::vector<casic_vector3d> routeLine_forA2A3[2];

	std::string UAVIDs[2] = { "110302", "110303" };

	QString selectMarkNames[2] = { " ", "" };

	int selectedRouteUAV[2] = { 0, 0 };

	std::map<int, int>  fireInfo;

	std::vector<casic_vector3d> testPoints;

	std::vector<IUserMesh2*> dropBoomAreaVec;
	std::vector<IText*> dropBoomAreaTextVec;

	bool m_draw3DFirst = true;
	std::vector<IUserMesh1*> Path3DLines;
	std::vector<IRenderableObject*> Path3DLines_plat;
	std::vector<IUserMesh2*> Path3DLines_surface;
	std::vector<ILabel*>     Path3DLines_label;

	std::vector<IUserMesh1*> path3DLines_M3;
	std::vector<IUserMesh2*> Path3DLines_surface_M3;
	std::vector<ILabel*>     Path3DLines_label_M3;

	std::vector<IUserMesh1*>				Boom_fire_ForM3;
	IUserMesh2*							Boom_fire_ForM3_throw = NULL;
	IUserMesh2*							Boom_fire_forM3_throw_onUAV = nullptr;
	bool								Boom_fire_ForM3_throw_isShow = true;
	bool								Boom_fire_ForM2_throw_isShow = true;

	vector<IUserMesh2*>						discoverZone_for_M3;
	vector<IUserMesh2*>						discoverZone_for_M2;

	QCString								m_strDataDir;
	QString									m_strIconPatch;
	QString									m_strIconPatch_tou;
	QString									m_strIconPatch_fashe;

	int ActivedActionID;
	CNeubulaAction currAction;
	std::vector<std::string> m_MarkIDVec;
	std::string currActionArea;
	std::string currActionIntelligence;
	CNeubulaMissionVec actionMissionVec;

	CRelationMissionResourceVec currMissionPlatVec;

	CNebulaMissionPlatformLoadVec   currMissionLoadVec;

	std::string   fireMissionIDS[2] = { " ", "" };
	CNeubulaMission fireMissions[2];

public:
	SELECTPOS			selectPos_For = SELECTPOS_NO;

	dispose_pos_way routInfo_forA2A3[2];

	qnzkna::ZoneManage::CZoneBase m_takeoffarea;
	qnzkna::ZoneManage::CZoneBase m_enterLine;
	qnzkna::ZoneManage::CZoneBase m_outLine;

	QString strTakeOffArea;
	QString strEnterLine;
	QString strOutLine;
	QString strWorkArea;

private:
	std::map<std::string, std::string> MarkName2ID;

public:
	qnzkna::ZoneManage::CZoneBase takeOff_land_Area;
	qnzkna::ZoneManage::CZoneBase searchArea;
	std::vector<QPointF> searchZonePoints;

private:
    Ui::UAV_DisPose_Route_Dialog *ui;

	Dispose_targetAndUAVS_process disposeTargetAndUAVS_process;
};

#endif
