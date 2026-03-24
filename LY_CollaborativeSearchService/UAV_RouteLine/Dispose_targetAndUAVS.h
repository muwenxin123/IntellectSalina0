#ifndef __DISPOSE_TARGETANDUAVS__
#define __DISPOSE_TARGETANDUAVS__

#include <vector>
#include <string>
#include <QObject>

#include "target_UAV_info.h"
#include "ICollaborativeSearchActivator.h"
#include "Dispose_UAVS_Route.h"
#include "map3d/IGraphics.h"
#include "map3d/IEarth.h"

typedef std::vector<IUserMesh1 *>	IUserMesh1Vec;
typedef std::vector<IUserMesh2 *>	IUserMesh2Vec;
typedef std::vector<ILabel *>		ILabelVec;

const std::string SeneorType = "FireControlFireProt";
const std::string DisposeType_Key = "µ¯Ò©ÀàÐÍFireProt";
const std::string DisposeType_Throw = "Ïû·À¹Þ";
const std::string DisposeType_Fire = "Ïû·À»ð¼ýµ¯";

class Dispose_targetAndUAVS_process : public QObject
{
    Q_OBJECT
public:
	Dispose_targetAndUAVS_process();

	void init();

	void addMarkType_used(ESystemMarkType _markType);
	void delMarkType_used(ESystemMarkType _markType);
	std::vector<ESystemMarkType>::iterator findMarkType_used(ESystemMarkType _markType);
	CSystemMarkInfoVec getMarks_All();

	void updateMarkList();

	void addOrUpdateMark(std::string markID);

	void loadUAVS_online();

	void loadUAVIDS_fromXml();

	std::vector<std::string> getUAVS_online();

	void addOrUpdateUAVforMark(std::string markID, std::string uavID);
	void delUAVforMark(std::string markID, std::string uavID);
	void delUAVForAllMark(std::string uavID);

	void addOrUpdateUAVforMark_unique(std::string markID, std::string uavID);

	void allocationUavFire();

	void routeLines();

	bool routeLinesUptoUAV();

	std::map<std::string, std::vector<casic_vector3d>> getRoutedLines();

	void setDispose_RouteFirst(DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST _route_first);

	void setHeight_safe_add(int _height_safe_add);

	void setSafeDis(int _safeDis);

	void setNoFlyZonePoints(std::vector<LLQPoints> _NoFlyZonePoints);

	void setTargetUAVInfos(vector<DISPOSE_ROUTE::target_UAVS_Info>& _targetUAVInfos);

	void setUAVStartPos_handle(std::string uavID, casic_vector3d pos);

	void changeUAV_throwHeiOrFireDis(std::string uavID, int disOrHei);

	Dispose_UAVS_Route* getDispos_Uav_Route();
private:

	void removeMeshs(IEarth* pEarth, std::vector<IUserMesh1*>& meshs);
	void removeMeshs(IEarth* pEarth, std::vector<IUserMesh2*>& meshs);
	void removeMeshs(IEarth* pEarth, std::vector<ILabel*>& meshs);

	void create3DLine(IEarth * pEarth, std::vector<casic_vector3d>& PointsVec, std::vector<IUserMesh1*> & Mesh1s, IUserMesh2Vec & Mesh2s,
		ILabelVec & labels, std::vector<int> lineIndexs, std::vector<QColor> lineColors, int lineWidth);
	void create3DLine(IEarth* pEarth, std::vector<casic_vector3d>& PointsVec, IUserMesh1Vec& Mesh1s, IUserMesh2Vec& Mesh2s, ILabelVec& labels,
		QColor lineColor, int lineWidth);

	void create3DLine(IEarth* pEarth, std::vector<casic_vector3d>&PointVec, IUserMesh1Vec& Mesh1s, QColor lineColor, int lineWidth);

	void create3DLine_height(IEarth* pEarth, std::vector<casic_vector3d>&PointVec, IUserMesh1Vec& Mesh1s, IUserMesh2Vec& Mesh2s, int lineWidth, QColor lineColor = QColor(255, 255, 255, 100));

public:
	std::vector<std::string> getUAVIDS_throw();
	std::vector<std::string> getUAVIDS_Fire();

signals:
	void signal_addaMarkID(QString markID);
	void signal_delaMarkID(QString markID);
private:

	void loadMarksFormDb();

	std::vector<int> sortUAVbyAngle(const DISPOSE_ROUTE::targetInfo& pTargetInfo, const std::vector<DISPOSE_ROUTE::UAVInfo>& pUAVInfos, const std::vector<int>& uavIndexs);

	DISPOSE_ROUTE::DISPOSE_WAY getUAVType(std::string uavID);

	void updateUav2Mission();

private:
	CSystemMarkInfoVec					Marks_All;
	std::vector<std::string>			Marks_AllID;
	std::vector<ESystemMarkType>		MarkTypes_used;

	std::vector<std::string>			uavIDS;
	std::vector<std::string>			uavIDS_Xml;
	std::vector<std::string>			uavIDS_Throw;
	std::vector<std::string>			uavIDS_Fire;
	std::map<std::string, casic_vector3d> uavIDSPos;

	std::map<int, int>  fireInfo;

	std::vector<DISPOSE_ROUTE::target_UAVS_Info>		target_Match_UAVS;

	Dispose_UAVS_Route							mDispose_UAVS_Route;

	std::map<std::string, CNeubulaMission> UAVID2Mission;

	IUserMesh1Vec Mesh1Vec_line;
	IUserMesh2Vec Mesh2Vec_line;
	ILabelVec	 labelVec_line;
};

#endif
