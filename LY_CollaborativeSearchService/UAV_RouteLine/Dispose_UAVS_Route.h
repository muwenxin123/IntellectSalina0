#ifndef __DISPOSE_UAVS_ROUTE__
#define __DISPOSE_UAVS_ROUTE__

#include "UAV_Dispose_Route_Base.h"
#include "target_UAV_info.h"

class Dispose_UAVS_Route
{
public:
	void setTargetUAVInfos(vector<DISPOSE_ROUTE::target_UAVS_Info>& _targetUAVInfos);

	void setDispose_RouteFirst(DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST _route_first);

	void setHeight_safe_add(int _height_safe_add);

	void setSafeDis(int _safeDis);

	void setNoFlyZonePoints(std::vector<LLQPoints> _NoFlyZonePoints);

	bool routeLines();

	std::map<std::string, std::vector<casic_vector3d>> getRoutedLines();

	void clearRoutedLine();

private:

	void LineTransToZone(std::vector<casic_vector3d> linePoints, std::vector<LLQPoints>& noflyZones);

public:
	qnzkna::ZoneManage::CZoneBase takeOff_land_Area;
	qnzkna::ZoneManage::CZoneBase searchArea;
	std::vector<qnzkna::ZoneManage::CZoneBase> lineZoneVec;
	qnzkna::ZoneManage::CZoneBase minLineZone;
	qnzkna::ZoneManage::CZoneBase returnLineZone;

	std::vector<qnzkna::ZoneManage::CZoneBase> noFlyAreaVec;
	std::vector<LLQPoints> noFlyAreaPointVec;

private:
	vector<DISPOSE_ROUTE::target_UAVS_Info>	targetUAVInfos;
	DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST			route_first = DISPOSE_ROUTE::DISPOSE_ROUTE_FIRST_TIME;
	int							height_safe_add = 50;
	int safeDis					= 10;
	std::vector<LLQPoints> NoFlyZonePoints;

	std::map<int, std::vector<std::vector<casic_vector3d>>>	zones_used;
	std::map<std::string, std::vector<casic_vector3d>>		route_lines;

};

#endif
