#include "AreaSearchAlgorithm.h"
#include "LY_CollaborativeSearchService.h"
#include <QGeoCoordinate>
#include <stack>
#include "RoutePlanningDialog.h"
#include "QThread"
#include "Dijkstra.h"

#include <iostream>
#include "gridbasesweepplanner.h"
#include "LyMessageBox.h"

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif
#define PI       3.14159265358979323846

AreaSearchAlgorithm::PlanNode::PlanNode() {
}

AreaSearchAlgorithm::PlanNode::PlanNode(double _x, double _y, double _xindex, double _yindex, double _gCost, double _hCost)
	: m_x(_x), m_y(_y), m_gCost(_gCost), m_hCost(_hCost),
	m_obstacle(false), m_backpoint(nullptr), m_isVisited(false), m_indexX(_xindex), m_indexY(_yindex) {
}

AreaSearchAlgorithm::PlanNode::~PlanNode() {
}

void AreaSearchAlgorithm::PlanNode::setObstacle() {
	m_obstacle = true;
}

bool AreaSearchAlgorithm::PlanNode::isObstacle() {
	return m_obstacle;
}

AreaSearchAlgorithm::PlanNode& AreaSearchAlgorithm::PlanNode::operator +=(const PlanNode& rhs) {
	m_x += rhs.m_x;
	m_y += rhs.m_y;
	m_indexX += rhs.m_indexX;
	m_indexY += rhs.m_indexY;
	m_gCost += rhs.m_gCost;
	m_hCost += rhs.m_hCost;
	return *this;
}

AreaSearchAlgorithm::PlanNode& AreaSearchAlgorithm::PlanNode::operator =(const PlanNode& rhs) {
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	m_indexX = rhs.m_indexX;
	m_indexY = rhs.m_indexY;
	m_gCost = rhs.m_gCost;
	m_hCost = rhs.m_hCost;
	m_areaAttribute = rhs.m_areaAttribute;
	return *this;
}

bool AreaSearchAlgorithm::PlanNode::operator ==(const PlanNode& rhs) {
	return m_x == rhs.m_x && m_y == rhs.m_y && m_indexX == rhs.m_indexX && m_indexY == rhs.m_indexY;
}

double AreaSearchAlgorithm::PlanNode::coordinateX() const {
	return m_x;
}

double AreaSearchAlgorithm::PlanNode::coordinateY() const {
	return m_y;
}

void AreaSearchAlgorithm::PlanNode::setBackpoint(std::shared_ptr<PlanNode> rhs) {
	m_backpoint = rhs;
}

std::shared_ptr<AreaSearchAlgorithm::PlanNode> AreaSearchAlgorithm::PlanNode::getBackpoint() {
	return m_backpoint;
}

bool AreaSearchAlgorithm::PlanNode::isVisited() {
	return m_isVisited;
}

void AreaSearchAlgorithm::PlanNode::setVisited() {
	m_isVisited = true;
}

double AreaSearchAlgorithm::PlanNode::indexX() const
{
	return m_indexX;
}

double AreaSearchAlgorithm::PlanNode::indexY() const
{
	return m_indexY;
}

void AreaSearchAlgorithm::PlanNode::setindexX(double index)
{
	m_indexX = index;
}

void AreaSearchAlgorithm::PlanNode::setindexY(double index)
{
	m_indexY = index;
}

void AreaSearchAlgorithm::PlanNode::setAreaAttribute(int areaType)
{
	m_areaAttribute = areaType;
}

int AreaSearchAlgorithm::PlanNode::getAreaAttribute()
{
	return m_areaAttribute;
}

void AreaSearchAlgorithm::PlanNode::setHCost(double _hCost) {
	m_hCost = _hCost;
}

double AreaSearchAlgorithm::PlanNode::getHCost() const {
	return m_hCost;
}

void AreaSearchAlgorithm::PlanNode::setG(double _gCost) {
	m_gCost = _gCost;
}

double AreaSearchAlgorithm::PlanNode::getGCost() const {
	return m_gCost;
}

void AreaSearchAlgorithm::PlanNode::setTotalCost() {

	m_totalCost = m_gCost + m_hCost;

}

double AreaSearchAlgorithm::PlanNode::getTotalCost() const {
	return m_gCost + m_hCost;
}

AreaSearchAlgorithm::AreaSearchAlgorithm()
{
	stagingPoint = casic_vector3d(0,0,0);
	currTakeOffPoint = casic_vector3d(0, 0, 0);
}

AreaSearchAlgorithm::~AreaSearchAlgorithm()
{
}

casic_vector3d AreaSearchAlgorithm::CalcPosByDis_Angle(double & lon, double & lat, double distance, double & angle)
{
	casic_vector3d newPosition(0.0, 0.0, 0.0);

	double ea = 6378137;
	double eb = 6356725;

	double geoLon = lon * (180 / PI);
	double geoLat = lat * (180 / PI);

	double DLon = distance * sin(angle * (PI / 180));
	double DLat = distance * cos(angle * (PI / 180));

	double latLong = 2 * PI * ea * cos(lat );
	double LonLong = 2 * PI * eb;

	double newGeoLon = geoLon + DLon / latLong * 360;
	double newGeoLat = geoLat + DLat / LonLong * 360;

	newPosition.x = newGeoLon * (PI / 180);
	newPosition.y = newGeoLat * (PI / 180);

	short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(newPosition.x, newPosition.y);
	if (elevationTemp<-10000)
	{
		elevationTemp = 0;
	}
	newPosition.z = elevationTemp;

	return newPosition;
}

casic_vector3d AreaSearchAlgorithm::getStagingPoint(casic_vector2d leftup, casic_vector2d rightdown, casic_vector2d takeoffPoint,double cruiseHeight)
{
	auto girdPoint = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->gridSidePoint;

	QGeoCoordinate min1(girdPoint[0][1], girdPoint[0][0]);
	QGeoCoordinate min2(girdPoint[1][1], girdPoint[1][0]);
	QGeoCoordinate min1Min(girdPoint[3][1], girdPoint[3][0]);
	QGeoCoordinate min2Min(girdPoint[2][1], girdPoint[2][0]);

	double min1Dismin2 = min1.distanceTo(min2);
	double min1DisMin = min1.distanceTo(min1Min);

	int min1Anglemin2 = min1.azimuthTo(min2);
	double min1AngleMin = min1.azimuthTo(min1Min);
	double AngleLine = (min1AngleMin + min1Anglemin2) / 2;

	QGeoCoordinate tmp = min1.atDistanceAndAzimuth(min1Dismin2 / 2, min1Anglemin2);
	QGeoCoordinate point0(takeoffPoint.y, takeoffPoint.x);
	double angle = tmp.azimuthTo(point0);
	double angleSide = min1Anglemin2;

	double gridDis = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->gridwidth;

	stagingPoint = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->stagingPoint;

	if (stagingPoint.x == 0 && stagingPoint.y == 0)
	{
		QGeoCoordinate temppoint = tmp.atDistanceAndAzimuth(_safeDistance * 4, angle);
		stagingPoint = casic_vector3d(temppoint.longitude(), temppoint.latitude(), cruiseHeight);
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->stagingPoint = stagingPoint;
		return stagingPoint;
	}
	QGeoCoordinate stagpoint(stagingPoint.y, stagingPoint.x);
	QGeoCoordinate temppoint = stagpoint.atDistanceAndAzimuth(_safeDistance * 4, angleSide);
	stagingPoint = casic_vector3d(temppoint.longitude(), temppoint.latitude(), cruiseHeight);
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->stagingPoint = stagingPoint;
	return stagingPoint;
}

casic_vector3d AreaSearchAlgorithm::getTakeOffPoint(casic_vector2d leftup, casic_vector2d rightdown, casic_vector2d takeoffPoint, double cruiseHeight)
{
	currTakeOffPoint = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->currTakeOffPoint;
	if (currTakeOffPoint.x == 0 && currTakeOffPoint.y == 0)
	{
		currTakeOffPoint = takeoffPoint;
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->currTakeOffPoint = currTakeOffPoint;
		return currTakeOffPoint;
	}
	std::vector<QGeoCoordinate> sidePoints;
	sidePoints.clear();
	QGeoCoordinate startPoint(currTakeOffPoint.y, currTakeOffPoint.x);

	double lon1 = leftup[0];
	double lon2 = rightdown[0];
	double lat1 = leftup[1];
	double lat2 = rightdown[1];
	QPointF jwdUp(lon1 > lon2 ? lon2 : lon1, lat1 > lat2 ? lat1 : lat2);
	QPointF jwdDown(lon1 > lon2 ? lon1 : lon2, lat1 > lat2 ? lat2 : lat1);

	QGeoCoordinate pointUpLeft(jwdUp.y(), jwdUp.x());
	QGeoCoordinate pointUpRight(jwdUp.y(), jwdDown.x());
	QGeoCoordinate pointDownLeft(jwdDown.y(), jwdUp.x());
	QGeoCoordinate pointDownRight(jwdDown.y(), jwdDown.x());
	QGeoCoordinate pointStart(takeoffPoint[1], takeoffPoint[0]);
	sidePoints.push_back(pointUpLeft);
	sidePoints.push_back(pointUpRight);
	sidePoints.push_back(pointDownLeft);
	sidePoints.push_back(pointDownRight);

	casic_vector2d centerpoint(0,0);
	for (auto pos : sidePoints)
	{
		centerpoint += casic_vector2d(pos.longitude(),pos.latitude());
	}
	centerpoint = centerpoint / 4;
	QGeoCoordinate centerpos(centerpoint.y,centerpoint.x);

	std::map<double, QGeoCoordinate> startDisPoint;
	startDisPoint.clear();
	for (int i = 0; i < sidePoints.size(); i++)
	{

		double dis = pointStart.distanceTo(sidePoints[i]);
		startDisPoint[dis] = sidePoints[i];
	}

	auto sidePointItor = startDisPoint.begin();

	QGeoCoordinate min1 = sidePointItor->second;
	sidePointItor++;
	QGeoCoordinate min2 = sidePointItor->second;
	sidePointItor++;

	casic_vector2d _min1 = casic_vector2d(min1.longitude(), min1.latitude());
	casic_vector2d _min2 = casic_vector2d(min2.longitude(), min2.latitude());
	casic_vector2d _mincenter = (_min1 + _min2) / 2;
	QGeoCoordinate mincenter(_mincenter.y, _mincenter.x);

	std::map<double, QGeoCoordinate> min1DisPoint;
	min1DisPoint.clear();
	while (sidePointItor != startDisPoint.end())
	{
		double dis = min1.distanceTo(sidePointItor->second);
		min1DisPoint[dis] = sidePointItor->second;
		sidePointItor++;
	}

	auto min1DisItor = min1DisPoint.begin();
	QGeoCoordinate min1Min = min1DisItor->second;
	min1DisItor++;
	QGeoCoordinate min2Min = min1DisItor->second;

	double min1Anglemin2 = min1.azimuthTo(min2);
	double min1Angletp = min1.azimuthTo(startPoint);

	double AngleLine = centerpos.azimuthTo(mincenter);
	QGeoCoordinate temppoint = startPoint.atDistanceAndAzimuth(_safeDistance, AngleLine);
	currTakeOffPoint = casic_vector3d(temppoint.longitude(), temppoint.latitude(), cruiseHeight);
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->currTakeOffPoint = currTakeOffPoint;
	return currTakeOffPoint;
}

std::vector<casic_vector2d> AreaSearchAlgorithm::calcTerrianObstacle(casic_vector2d start, casic_vector2d goal,double cruiseHeight)
{
	std::vector<casic_vector2d>obstacleVec;

	casic_vector2d leftdown = casic_vector2d(start.x, goal.y);
	casic_vector2d rightup = casic_vector2d(goal.x, start.y);

	QGeoCoordinate point1(start.y,start.x);
	QGeoCoordinate point3(leftdown.y,leftdown.x);
	QGeoCoordinate point4(rightup.y,rightup.x);
	QGeoCoordinate point2(goal.y,goal.x);

	double mapSize = point1.distanceTo(point4);
	double gridSize = point1.distanceTo(point3);

	double ddis = point1.distanceTo(point2);

	double  dis_step = ddis / 40.0;
	double m_lonPointNum = mapSize / (dis_step)+1;
	double m_latPointNum = gridSize / (dis_step)+1;

	double delta_lon = (goal.x - start.x) / m_lonPointNum;
	double delta_lat = (goal.y - start.y) / m_latPointNum;

	for (int i = 0; i < m_lonPointNum; ++i)
	{
		std::vector<std::vector<casic_vector2d>> tmp;
		for (int j = 0; j < m_latPointNum; ++j)
		{
			casic_vector2d pos_lonlat = new casic_vector2d(start.x + delta_lon * i, start.y + delta_lat * j);
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(pos_lonlat.x, pos_lonlat.y);
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			double m_alt = elevationTemp;

			if (m_alt >= cruiseHeight)
			{
				obstacleVec.push_back(casic_vector2d(pos_lonlat.x, pos_lonlat.y));
			}
		}

	}
	return obstacleVec;
}

void AreaSearchAlgorithm::stagingPathPlanning(int uid, casic_vector3d start, casic_vector3d goal, double safeDis, std::vector<double>& fly_param, std::vector<casic_vector3d>& stagePointPlanMap)
{
	auto sg1 = casic_vector3d(start.x, goal.y, goal.z);
	auto sg2 = casic_vector3d(goal.x, start.y, goal.z);
	QGeoCoordinate point1(start.y, start.x);
	QGeoCoordinate point2(sg1.y, sg1.x);
	QGeoCoordinate point3(goal.y, goal.x);
	QGeoCoordinate point4(sg2.y, sg2.x);

	double length_ = point1.distanceTo(point2);
	double width_ = point1.distanceTo(point4);
	double alt_ = goal.z;

	AStar astar;
	astar.ObstacleUAVs = ObstacleVec1;
	astar.NoFlyZonePoints = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
	astar.setFlyParam(fly_param[0], fly_param[1]);
	std::vector<casic_vector3d>posVec;
	astar.init(length_, width_, alt_, safeDis, start, goal);
	astar.pathPlan(posVec);

	fly_param.push_back(astar.getFlyTime());

	for (auto pos_itor = posVec.rbegin(); pos_itor != posVec.rend(); ++pos_itor)
	{
		ObstacleVec1.push_back(*pos_itor);
		stagePointPlanMap.push_back(*pos_itor);
	}
	if (std::find(posVec.begin(), posVec.end(), goal) == posVec.end())
	{
		ObstacleVec1.push_back(goal);
		stagePointPlanMap.push_back(goal);
	}
}

void AreaSearchAlgorithm::firstPointPathPlanning(int uid, casic_vector3d start, casic_vector3d goal, double safedis, std::vector<double>& fly_param, std::vector<casic_vector3d>& stagePointPlanMap)
{
	auto sg1 = casic_vector3d(start.x, goal.y, goal.z);
	auto sg2 = casic_vector3d(goal.x, start.y, goal.z);
	QGeoCoordinate point1(start.y, start.x);
	QGeoCoordinate point2(sg1.y, sg1.x);
	QGeoCoordinate point3(goal.y, goal.x);
	QGeoCoordinate point4(sg2.y, sg2.x);

	double length_ = point1.distanceTo(point2);
	double width_ = point1.distanceTo(point4);
	double alt_ = goal.z;

	AStar astar;
	astar.ObstacleUAVs = ObstacleVecf1;
	astar.NoFlyZonePoints = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
	astar.setFlyParam(fly_param[0], fly_param[1]);
	std::vector<casic_vector3d>posVec;
	astar.init(length_, width_, alt_, safedis, start, goal);
	astar.pathPlan(posVec);

	fly_param.push_back(astar.getFlyTime());

	for (auto pos_itor = posVec.rbegin(); pos_itor != posVec.rend(); ++pos_itor)
	{
		ObstacleVecf1.push_back(*pos_itor);
		stagePointPlanMap.push_back(*pos_itor);
	}
	if (std::find(posVec.begin(), posVec.end(), goal) == posVec.end())
	{
		ObstacleVecf1.push_back(goal);
		stagePointPlanMap.push_back(goal);
	}
}

bool AreaSearchAlgorithm::is_point_in_zone(const QPointF point,const std::vector<QPointF> ZonePoints)
{
	int count = ZonePoints.size();
	int nCross = 0;
	for (int i = 0; i < count; i++)
	{
		QPointF p1 = ZonePoints[i];
		QPointF p2 = ZonePoints[(i + 1) % count];

		if (p1.y() == p2.y())
			continue;

		if (point.y() < min(p1.y(), p2.y()))
			continue;

		if (point.y() >= max(p1.y(), p2.y()))
			continue;

		double x = (point.y() - p1.y()) * (p2.x() - p1.x()) / (p2.y() - p1.y()) + p1.x();

		if (x > point.x())
			nCross++;
	}

	return (nCross % 2 == 1);
}

bool AreaSearchAlgorithm::is_point_in_zones(const QPointF point, const std::vector<LLQPoints> Zones)
{
	bool isReturn = false;
	for (int i = 0; i < Zones.size(); i++)
	{
		auto zonePoints = Zones[i];
		isReturn = is_point_in_zone(point, zonePoints);
		if (isReturn)
		{
			return isReturn;
		}
	}
	return isReturn;

}

double AreaSearchAlgorithm::calculateCross(casic_vector3d p1, casic_vector3d p2, casic_vector3d p3)
{
	double x1 = p2.x - p1.x;
	double y1 = p2.y - p1.y;
	double x2 = p3.x - p1.x;
	double y2 = p3.y - p1.y;
	return x1 * y2 - x2 * y1;
}

bool AreaSearchAlgorithm::is_lineA_intersect_lineB(casic_vector3d Apoint1, casic_vector3d Apoint2, casic_vector3d Bpoint1, casic_vector3d Bpoint2)
{
	double E = 0.0000005;

	double maxAx = max(Apoint1.x, Apoint2.x);
	double minBx = min(Bpoint1.x, Bpoint2.x);
	if (max(Apoint1.x, Apoint2.x) >= min(Bpoint1.x, Bpoint2.x) &&
		max(Bpoint1.x, Bpoint2.x) >= min(Apoint1.x, Apoint2.x) &&
		max(Apoint1.y, Apoint2.y) >= min(Bpoint1.y, Bpoint2.y) &&
		max(Bpoint1.y, Bpoint2.y) >= min(Apoint1.y, Apoint2.y))
	{

		auto a1 = calculateCross(Apoint1, Apoint2, Bpoint1);
		auto a2 = calculateCross(Apoint1, Apoint2, Bpoint2);
		auto a3 = calculateCross(Bpoint1, Bpoint2, Apoint1);
		auto a4 = calculateCross(Bpoint1, Bpoint2, Apoint2);

		if (calculateCross(Apoint1, Apoint2, Bpoint1) * calculateCross(Apoint1, Apoint2, Bpoint2) <= 0 &&
			calculateCross(Bpoint1, Bpoint2, Apoint1) * calculateCross(Bpoint1, Bpoint2, Apoint2) <= 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

bool AreaSearchAlgorithm::is_lineIntersectZone(casic_vector3d pointA, casic_vector3d pointB, std::vector<QPointF> points_Zone)
{
	bool isIntersect = false;

	int count_zone = points_Zone.size();
	for (int i = 0; i < count_zone; i++)
	{
		casic_vector3d tmp = casic_vector3d(points_Zone[i % count_zone].x(), points_Zone[i % count_zone].y(), 0);
		casic_vector3d tmp1 = casic_vector3d(points_Zone[(i + 1) % count_zone].x(),
											 points_Zone[(i + 1) % count_zone].y(), 0);
		if (is_lineA_intersect_lineB(pointA, pointB, tmp, tmp1))
		{
			return true;
		}
	}
	return isIntersect;
}

int AreaSearchAlgorithm::num_lineIntersectZone(casic_vector3d pointA, casic_vector3d pointB, std::vector<QPointF> points_Zone, std::vector<QPointF> &points_List)
{
	int pCount = 0;

	int count_zone = points_Zone.size();
	for (int i = 0; i < count_zone; i++)
	{
		casic_vector3d tmp = casic_vector3d(points_Zone[i].x(), points_Zone[i].y(), 0);
		casic_vector3d tmp1 = casic_vector3d(points_Zone[(i + 1)].x(),points_Zone[(i + 1)].y(), 0);
		if (is_lineA_intersect_lineB(pointA, pointB, tmp, tmp1))
		{
			std::pair<double, double>mpair = getlineIntersectPoint(pointA, pointB, tmp, tmp1);
			double dis =  calDisP2Line(tmp, tmp1, casic_vector3d(mpair.first, mpair.second,0));
			points_List.push_back(QPointF(mpair.first,mpair.second));
			pCount++;
		}
	}
	return pCount;
}

std::pair<double, double> AreaSearchAlgorithm::getlineIntersectPoint(casic_vector3d pointA, casic_vector3d pointB, casic_vector3d pointC, casic_vector3d pointD)
{
	double A1, B1, C1 = 0;
	double A2, B2, C2 = 0;

	A1 = -(pointB.y - pointA.y);
	B1 = pointB.x - pointA.x;
	C1 = (pointB.y - pointA.y)*pointA.x - (pointB.x - pointA.x)*pointA.y;

	A2 = -(pointD.y - pointC.y);
	B2 = pointD.x - pointC.x;
	C2 = (pointD.y - pointC.y)*pointC.x - (pointD.x - pointC.x)*pointC.y;

	double X = (B2*C1 - B1*C2) / (A2*B1 - A1*B2);
	double Y = (A2*C1 - A1*C2) / (A1*B2 - A2*B1);

	return std::make_pair(X, Y);
}

bool AreaSearchAlgorithm::is_lineToAllaZones(casic_vector3d pointA, casic_vector3d pointB, std::vector<LLQPoints> zonelist)
{
	bool is_interect = false;
	for (auto zone : zonelist)
	{
		if (is_lineIntersectZone(pointA, pointB, zone))
		{
			return true;
		}
	}
	return is_interect;
}

double AreaSearchAlgorithm::calDisP2Line(casic_vector3d linePoint1, casic_vector3d linePoint2, casic_vector3d point)
{
	double Dis;
	double lineSlope = (linePoint2[1] - linePoint1[1]) / (linePoint2[0] - linePoint1[0]);
	double pointSlope = 1.0 / lineSlope;

	double interPointx = (lineSlope * linePoint2[0] - linePoint2[1] - pointSlope * point[0] + point[1])
		/ (lineSlope - pointSlope);
	double interPointy = pointSlope * (interPointx - point[0]) + point[1];

	QGeoCoordinate oldPoint(point[1], point[0]);
	QGeoCoordinate interPoint(interPointy, interPointx);
	Dis = oldPoint.distanceTo(interPoint);

	return Dis;
}

double AreaSearchAlgorithm::calDisP2Line1(casic_vector3d linePoint1, casic_vector3d linePoint2, casic_vector3d point)
{
	double delta = 10000000;
	linePoint1 = linePoint1 * delta;
	linePoint2 = linePoint2 *delta;
	point = point * delta;

	double A, B, C = 1;
	A = (linePoint2.y - linePoint1.y);
	B = linePoint1.x - linePoint2.x;

	C = linePoint2.x * linePoint1.y - linePoint1.x*linePoint2.y;
	double dis = abs(A * point.x + B * point.y + C) / sqrt(A * A + B * B);
	return dis;
}

bool AreaSearchAlgorithm::isInterObstacle(casic_vector3d point1, casic_vector3d point2, vector<casic_vector2d> obstacleArea)
{
	bool r = false;
	double Dis = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->gridwidth;

	QGeoCoordinate p1(point1[1], point1[0]);
	QGeoCoordinate p2(point2[1], point2[0]);
	double angle = p1.azimuthTo(p2);
	double dis = Dis / 2;
	QGeoCoordinate lineArea1 = p1.atDistanceAndAzimuth(dis, angle - 90);
	QGeoCoordinate lineArea2 = p1.atDistanceAndAzimuth(dis, angle + 90);
	QGeoCoordinate lineArea3 = p2.atDistanceAndAzimuth(dis, angle + 90);
	QGeoCoordinate lineArea4 = p2.atDistanceAndAzimuth(dis, angle - 90);
	QPointF pf1(lineArea1.longitude(), lineArea1.latitude());
	QPointF pf2(lineArea2.longitude(), lineArea2.latitude());
	QPointF pf3(lineArea3.longitude(), lineArea3.latitude());
	QPointF pf4(lineArea4.longitude(), lineArea4.latitude());

	vector<QPointF> area;
	area.clear();
	area.push_back(pf1);
	area.push_back(pf2);
	area.push_back(pf3);
	area.push_back(pf4);

	for (auto item : obstacleArea)
	{
		QPointF point(item[0], item[1]);
		r = is_point_in_zone(point, area);
		if (r)
		{
			return r;
		}
	}

	return r;
}

bool AreaSearchAlgorithm::isInterObstacle(casic_vector3d point1, casic_vector3d point2, vector<casic_vector2d> obstacleArea, double gridwidth)
{
	bool r = false;
	double Dis = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->gridwidth;

	QGeoCoordinate p1(point1[1], point1[0]);
	QGeoCoordinate p2(point2[1], point2[0]);
	double angle = p1.azimuthTo(p2);
	double dis = Dis / 2;
	QGeoCoordinate lineArea1 = p1.atDistanceAndAzimuth(dis, angle - 90);
	QGeoCoordinate lineArea2 = p1.atDistanceAndAzimuth(dis, angle + 90);
	QGeoCoordinate lineArea3 = p2.atDistanceAndAzimuth(dis, angle + 90);
	QGeoCoordinate lineArea4 = p2.atDistanceAndAzimuth(dis, angle - 90);
	QPointF pf1(lineArea1.longitude(), lineArea1.latitude());
	QPointF pf2(lineArea2.longitude(), lineArea2.latitude());
	QPointF pf3(lineArea3.longitude(), lineArea3.latitude());
	QPointF pf4(lineArea4.longitude(), lineArea4.latitude());

	vector<QPointF> area;
	area.clear();
	area.push_back(pf1);
	area.push_back(pf2);
	area.push_back(pf3);
	area.push_back(pf4);

	for (auto item : obstacleArea)
	{
		QPointF point(item[0], item[1]);

		casic_vector3d pos = casic_vector3d(item[0], item[1], 0);
		double dis = calDisP2Line(point1, point2, item);
		if (dis < gridwidth / 2 * sqrt(2))
		{
			r = true;
		}
		if (r)
		{
			return r;
		}
	}

	return r;
}

void AreaSearchAlgorithm::AreaSerachPlan(casic_vector3d leftup, casic_vector3d rightdown,
	casic_vector3d currPos, double Height, double gridWidth, double speed, double angularSpeed, double flyTime, double coverRate,
	double safeDis, std::map<int, std::vector<casic_vector3d>> &PointsVec)
{

	double _maxFlyTime = 0;
	allStagingPoint.clear();
	_coverAreaVec.clear();
	auto noflyZonePointVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
	auto obstacleArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->obstacleArea;
	stagingPoint = casic_vector3d(0.0, 0.0, 0.0);
	PointsVec.clear();
	_safeDistance = safeDis;
	std::vector<std::vector<shared_ptr<PlanNode>>> m_roadmap;
	m_roadmap = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->GridPointMap;
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->calcolateGridLinePoints();

	double step_dis = gridWidth;
	if (m_roadmap.size() == 0 || m_roadmap[0].size() == 0)
	{
		return;
	}
	int lonNum = m_roadmap.size();
	int latNum = m_roadmap[0].size();

	Gridsmap = m_roadmap;

	double Xindex = m_roadmap.size();
	double Yindex = m_roadmap[0].size();
	PlanNode m_startNode = PlanNode(m_roadmap[0][0]->coordinateX(), m_roadmap[0][0]->coordinateY(), 0, 0);

	if (m_roadmap.empty())
	{
		return;
	}

	std::vector<casic_vector3d>ObstacleUAVs;
	std::vector<casic_vector3d>_visitedPoints;
	std::vector<std::vector<shared_ptr<PlanNode>>>_planNodes;
	std::vector<casic_vector3d>_onePlanevisitedPoints;
	casic_vector3d pos;

	std::vector<PlanNode> motion = {
		PlanNode(0, 0,  1,  0,  1, 0),
		PlanNode(0, 0,  0,  1,  1, 0),
		PlanNode(0, 0,  1, -1,  std::sqrt(2),0),
		PlanNode(0, 0,  1,  1,  std::sqrt(2),0),
		PlanNode(0, 0,  0, -1,  1, 0),
		PlanNode(0, 0, -1,  0,  1, 0),
		PlanNode(0, 0, -1, -1,  std::sqrt(2),0),
		PlanNode(0, 0, -1,  1,  std::sqrt(2),0)

	};

	int Planindex = 1;
	while (_visitedPoints.size() < m_roadmap.size()* m_roadmap[0].size()*coverRate)
	{

		auto cmp = [](shared_ptr<PlanNode> left, shared_ptr<PlanNode> right) {

			return left->getTotalCost() > right->getTotalCost();
		};

		std::priority_queue<shared_ptr<PlanNode>,
			std::vector<shared_ptr<PlanNode>>, decltype(cmp)> O(cmp);
		std::stack<shared_ptr<PlanNode>>O1;

		double flytime = 0;

		if (!ObstacleUAVs.empty())
		{
			vector<shared_ptr<PlanNode>> tmp;
			for (auto pos : ObstacleUAVs)
			{
				for (int i = 0; i < lonNum; ++i) {
					vector<shared_ptr<PlanNode>> tmp;
					for (int j = 0; j < latNum; ++j) {
						double lon = m_roadmap[i][j]->coordinateX();
						double lat = m_roadmap[i][j]->coordinateY();
						if (lon == (double)pos.x && lat == (double)pos.y)
						{
							m_roadmap[i][j]->setVisited();
						}
					}
				}
			}
		}

		int startIndex = 0;
		int startjIndex = 0;

		shared_ptr<PlanNode> tmp = m_roadmap[startIndex][startjIndex];
		bool isReturn = false;
		while (startjIndex < m_roadmap[0].size())
		{
			startIndex = 0;
			while (startIndex < m_roadmap.size())
			{
				tmp = m_roadmap[startIndex][startjIndex];
				if (!tmp->isVisited() && !tmp->isObstacle())
				{
					isReturn = true;
					break;
				}
				startIndex++;
			}
			if (isReturn)
			{
				break;
			}
			startjIndex++;
		}

		int roadIndex = Planindex;
		O.push(tmp);
		O1.push(tmp);

		std::vector<casic_vector3d> pointList;
		std::vector<casic_vector3d> stagingPointList;
		casic_vector3d m_collPos = getStagingPoint(casic_vector2d(leftup.x,leftup.y), casic_vector2d(rightdown.x,rightdown.y),
			casic_vector2d(currPos.x,currPos.y), Height);
		allStagingPoint.push_back(m_collPos);
		casic_vector3d m_takeoffPos = getTakeOffPoint(casic_vector2d(leftup.x, leftup.y), casic_vector2d(rightdown.x, rightdown.y),
			casic_vector2d(currPos.x, currPos.y), Height);
		std::vector<double> fly_param;
		fly_param.push_back(speed);
		fly_param.push_back(angularSpeed);
		stagingPathPlanning(Planindex, m_takeoffPos, m_collPos,safeDis, fly_param, pointList);
		stagingPointList = pointList;

		PlanNode m_collNode = PlanNode(m_collPos.x, m_collPos.y, 0, 0,0,0);
		casic_vector3d tmpPo = casic_vector3d(tmp->coordinateX(), tmp->coordinateY(), Height);

		if (is_lineToAllaZones(m_collPos, tmpPo, noflyZonePointVec))
		{
			O1.pop();
			int Xindex = tmp->indexX();
			int Yindex = tmp->indexY();
			shared_ptr<PlanNode> tmp1 = m_roadmap[0][0];
			bool isFindPoint = false;
			for (int i = 0, Iindex = Yindex; i < m_roadmap[Xindex].size();i++)
			{
				for (int j = 0, Jindex = Xindex + 1; j < m_roadmap.size(); j++)
				{
					if (!(m_roadmap[Jindex][Iindex]->isVisited()) && !(m_roadmap[Jindex][Iindex]->isObstacle()))
					{
						tmp1 = m_roadmap[Jindex][Iindex];
						tmpPo = casic_vector3d(tmp1->coordinateX(), tmp1->coordinateY(), Height);
						if (!is_lineToAllaZones(m_collPos, tmpPo, noflyZonePointVec))
						{
							i = m_roadmap[Iindex].size();
							isFindPoint = true;
							break;
						}
					}
					Jindex = (Jindex + 1) % m_roadmap.size();
				}
				Iindex = (Iindex + 1) % m_roadmap[Xindex].size();
			}
			if (isFindPoint)
			{
				O1.push(tmp1);
			}
		}
		if (O1.empty())
		{

			double visitNum = _visitedPoints.size();
			double allNum = m_roadmap.size()* m_roadmap[0].size();
			double rate = visitNum / allNum;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = rate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < flytime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = flytime;
			}
			return;
		}
		tmp = O1.top();
		if (tmp->isVisited())
		{
			double visitNum = _visitedPoints.size();
			double allNum = m_roadmap.size()* m_roadmap[0].size();
			double rate = visitNum / allNum;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = rate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < flytime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = flytime;
			}
			return;
		}
		firstPointPathPlanning(Planindex, m_collPos, tmpPo, safeDis, fly_param, pointList);
		for (int i = stagingPointList.size(); i < pointList.size();i++)
		{
			auto pt = pointList[i];
			if (std::find(stagingPointList.begin(), stagingPointList.end(), pt) == stagingPointList.end())
			{
				stagingPointList.push_back(pt);
			}
		}
		pointList.push_back(tmpPo);
		std::vector<casic_vector3d>edpoints = getEndPoints(tmpPo, gridWidth);
		std::pair<casic_vector3d, std::vector<casic_vector3d>>_pair = std::make_pair(pos, edpoints);
		_coverAreaVec.push_back(edpoints);
		if (Planindex == 1)
		{
			_coverAreaVec.push_back(edpoints);
		}

		QGeoCoordinate pos0(currPos.y, currPos.x);
		QGeoCoordinate pos1(m_collNode.coordinateY(), m_collNode.coordinateX());
		QGeoCoordinate pos2(tmp->coordinateY(), tmp->coordinateX());
		double stagedis = pos0.distanceTo(pos1);
		double colldis = pos1.distanceTo(pos2);
		flytime += 2*(stagedis / speed);
		flytime += 2*(colldis / speed);

		std::vector<shared_ptr<PlanNode>>_onePoints;
		PlanNode nlast;
		double AngularPass = 0;

		QGeoCoordinate lastPoint = pos1;

		shared_ptr<PlanNode>lastPlanNode = m_roadmap[m_startNode.indexX()][m_startNode.indexY()];
		shared_ptr<PlanNode>minNode = m_roadmap[m_startNode.indexX()][m_startNode.indexY()];

		int calCount = 0;
		while (flytime <= flyTime && _onePlanevisitedPoints.size() <= m_roadmap.size()* m_roadmap[0].size()* coverRate)
		{
			if (O1.empty())
			{
				shared_ptr<PlanNode> tmp1 = m_roadmap[0][0];
				for (int i = 0; i < m_roadmap.size();i++)
				{
					for (int j = 0; j < m_roadmap[i].size(); j++)
					{
						if (m_roadmap[i][j]->isVisited() ==  false)
						{
							tmp1 = m_roadmap[i][j];
							i = m_roadmap.size();
							break;
						}
					}
				}

				O1.push(tmp1);
				roadIndex++;
			}

			shared_ptr<PlanNode> nBest = O1.top();

			if (_onePoints.size() == 0)
			{

				nlast = m_collNode;
			}
			QGeoCoordinate nBestQGeo = QGeoCoordinate(nBest->coordinateY(), nBest->coordinateX());

			casic_vector3d posnBest = casic_vector3d(nBest->coordinateX(), nBest->coordinateY(),Height);
			O1.pop();

			nBest->setVisited();

			_onePlanevisitedPoints.push_back(casic_vector3d(nBest->coordinateX(), nBest->coordinateY(), Height));
			int index = 0;
			std::map<double, shared_ptr<PlanNode>> minPoint_map;
			minPoint_map.clear();
			for (int i = 0; i < motion.size(); i++)
			{
				index++;
				PlanNode tmpNode = motion[i];
				nBest->setG(0.0);
				tmpNode += *nBest;
				int xxx = tmpNode.indexX();
                int yyy = tmpNode.indexY();
				if (tmpNode.indexX() < 0 || tmpNode.indexY() < 0 || tmpNode.indexX() >= lonNum || tmpNode.indexY() >= latNum)
				{
					continue;
				}

				shared_ptr<PlanNode> tmpNodePointer =
					m_roadmap[tmpNode.indexX()][tmpNode.indexY()];

				casic_vector3d tmpNodePos = casic_vector3d(tmpNodePointer->coordinateX(), tmpNodePointer->coordinateY(), Height);
				if (is_lineToAllaZones(posnBest, tmpNodePos, noflyZonePointVec))
				{
					_visitedPoints.push_back(tmpNodePos);
					continue;
				}

				if (tmpNodePointer->isObstacle() || tmpNodePointer->isVisited()) {
					continue;
				}

				if (isInterObstacle(posnBest, tmpNodePos, obstacleArea,gridWidth))
				{
					continue;
				}

				QGeoCoordinate tmpPoint(tmpNodePointer->coordinateY(), tmpNodePointer->coordinateX());
				double lastAngle = lastPoint.azimuthTo(nBestQGeo);
				double nowAngle = nBestQGeo.azimuthTo(tmpPoint);

				double disAngle = fabs(nowAngle - lastAngle);
				if (disAngle > 180)
				{
					double maxAngle = max(lastAngle, nowAngle);
					double minAngle = min(lastAngle, nowAngle);
					minAngle += 360.0;
					disAngle = fabs(minAngle - maxAngle);
				}

				double Gcost = tmpNode.getGCost() + disAngle / 180.0;
				tmpNodePointer->setG(Gcost);
				int hcost = Gridsmap[tmpNode.indexX()][tmpNode.indexY()]->getHCost();
				Gcost -= hcost;
				minPoint_map[Gcost] = tmpNodePointer;

			}
			lastPoint = nBestQGeo;

			if (minPoint_map.size() == 0)
			{
				QPoint nowPointXY(nBest->indexX(), nBest->indexY());

				std::vector<QPoint> nextPoints = getNextPointFromImpasse(nowPointXY, m_roadmap);

				if (nextPoints.size() == 0)
				{
					break;
				}

				std::vector<QPoint> nextPathPoints;
				nextPathPoints.clear();

				QPoint nextPointXY;
				shared_ptr<PlanNode> nextPlanNode = nullptr;
				QGeoCoordinate nextPointGeo;
				casic_vector3d nextPointLLA;
				bool connected = false;
				for (int nextIndex = 0; nextIndex < nextPoints.size(); nextIndex++)
				{
					nextPointXY = nextPoints[nextIndex];
					nextPlanNode = m_roadmap[nextPointXY.x()][nextPointXY.y()];
					nextPointGeo = QGeoCoordinate(nextPlanNode->coordinateY(), nextPlanNode->coordinateX());
					nextPointLLA = casic_vector3d(nextPlanNode->coordinateX(), nextPlanNode->coordinateY(), Height);

					bool isInterset = is_lineToAllaZones(posnBest, nextPointLLA, noflyZonePointVec);
					bool isIntersetObs = isInterObstacle(posnBest, nextPointLLA, obstacleArea);
					if (isInterset == false && isIntersetObs == false)
					{
						nextPlanNode->setBackpoint(nBest);
						double dis = nextPointGeo.distanceTo(nBestQGeo);
						flytime += 2 * (dis / speed);
						pointList.push_back(nextPointLLA);
						nextPlanNode->setVisited();
						O1.push(nextPlanNode);
						connected = true;
						break;
					}
				}
				if (connected == true)
				{
					continue;
				}
				else
				{

					for (int nextIndex = 0; nextIndex < nextPoints.size(); nextIndex++)
					{

						nextPointXY = nextPoints[nextIndex];
						nextPlanNode = m_roadmap[nextPointXY.x()][nextPointXY.y()];
						nextPointGeo = QGeoCoordinate(nextPlanNode->coordinateY(), nextPlanNode->coordinateX());
						nextPointLLA = casic_vector3d(nextPlanNode->coordinateX(), nextPlanNode->coordinateY(), Height);

						std::map<shared_ptr<PlanNode>, int> V2PlanNode;
						V2PlanNode.clear();
						std::vector<std::vector<int>> graph;
						graph.clear();

						createMGraph(m_roadmap, 0, V2PlanNode, graph);
						MGraph* mGraph = new MGraph;
						auto iBeginItor = V2PlanNode.find(nBest);
						auto iEndItor = V2PlanNode.find(nextPlanNode);
						int iBegin;
						int iEnd;
						if (iBeginItor != V2PlanNode.end() && iEndItor != V2PlanNode.end())
						{
							iBegin = iBeginItor->second;
							iEnd = iEndItor->second;
						}
						else
						{
							break;
						}
						std::vector<int> stackVertices = mGraph->getShortPath(graph, iBegin, iEnd);

						int stackSize = stackVertices.size();
						if (stackSize <= 0)
						{
							continue;
						}

						casic_vector3d lastPoint = posnBest;
						bool isConnect = true;
						for (int index = stackSize - 1; index >= 0; index--)
						{
							int k = stackVertices[index];
							auto V2planNodeItor = V2PlanNode.begin();
							while (V2planNodeItor->second != k)
							{
								V2planNodeItor++;
							}
							auto planNodeItor = V2planNodeItor->first;
							casic_vector3d nextPoint(planNodeItor->coordinateX(), planNodeItor->coordinateY(), Height);
							bool isInterNoFly = is_lineToAllaZones(lastPoint, nextPoint, noflyZonePointVec);
							bool isInterOb = isInterObstacle(lastPoint, nextPoint, obstacleArea);
							if (isInterNoFly == false && isInterOb == false)
							{
								lastPoint = nextPoint;
								nextPathPoints.push_back(QPoint(planNodeItor->indexX(), planNodeItor->indexY()));
								lastPoint = nextPoint;
							}
							else
							{
								isConnect = false;
								nextPathPoints.clear();
								break;
							}
						}
						if (isConnect == false)
						{
							continue;
						}
						else
						{
							break;
						}
					}
				}

				if (nextPathPoints.size() > 0)
				{
					auto lastNode = nBest;
					for (int i = 0; i < nextPathPoints.size(); i++)
					{
						QPoint nextPathPoint = nextPathPoints[i];
						auto nextNode = Gridsmap[nextPathPoint.x()][nextPathPoint.y()];
						casic_vector3d nextPlanLLA(nextNode->coordinateX(), nextNode->coordinateY(), 0);
						nextNode->setBackpoint(lastPlanNode);
						nextNode->setVisited();
						lastNode = nextNode;
						pointList.push_back(nextPlanLLA);
					}
					nextPlanNode->setBackpoint(lastNode);
					O1.push(nextPlanNode);
					pointList.push_back(nextPointLLA);
					continue;
				}
				else if (connected == true)
				{
					continue;
				}
				else
				{
					break;
				}
			}

			auto nextPointer = minPoint_map.begin()->second;

			nextPointer->setBackpoint(nBest);
			flytime += 2 * (nextPointer->getGCost() * step_dis / speed);

			casic_vector3d nextPos(nextPointer->coordinateX(), nextPointer->coordinateY(), Height);

			pointList.push_back(nextPos);

			nextPointer->setVisited();
			O1.push(nextPointer);
			lastPlanNode = nextPointer;
			calCount++;
		}

		tmp = lastPlanNode;
		int count = 0;
		std::vector<casic_vector3d>tmpsearchList;
		casic_vector3d firstPoint = casic_vector3d(0,0,0);
		while (!(tmp->indexX() == 0 && tmp->indexY() == 0)) {

			pos = new  casic_vector3d(tmp->coordinateX(), tmp->coordinateY(), Height);
			pointList.push_back(pos);

			tmpsearchList.push_back(pos);
			_visitedPoints.push_back(pos);
			ObstacleUAVs.push_back(pos);
			if (tmp->getBackpoint() == nullptr)
			{
				firstPoint = pos;
				break;
			}
			tmp = tmp->getBackpoint();
			count++;
		}

		if (_maxFlyTime < flytime)
		{
			_maxFlyTime = flytime;
		}

		for (auto pos : tmpsearchList)
		{
			std::vector<casic_vector3d>edpoints = getEndPoints(pos, gridWidth);
			std::pair<casic_vector3d, std::vector<casic_vector3d>>_pair = std::make_pair(pos, edpoints);
			_coverAreaVec.push_back(edpoints);
		}

		for (auto pos_itor = stagingPointList.rbegin(); pos_itor != stagingPointList.rend(); ++pos_itor)
		{
			pointList.emplace_back(*pos_itor);
		}

		auto imtor = pointList.rbegin();
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(imtor->x, imtor->y);
		if (elevationTemp < -10000)
		{
			elevationTemp = 0;
		}
		imtor->z = elevationTemp;

		std::pair<int, std::vector<casic_vector3d>>mpair = std::make_pair(Planindex, pointList);
		PointsVec.insert(mpair);
		Planindex++;
		if (_onePlanevisitedPoints.size() >= m_roadmap.size()* m_roadmap[0].size()
			|| _visitedPoints.size() > m_roadmap.size()* m_roadmap[0].size())
		{

			double visitNum = _visitedPoints.size();
			double allNum = m_roadmap.size()* m_roadmap[0].size();
			double rate = visitNum / allNum;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = rate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < flytime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = flytime;
			}
			return;
		}
		if (Planindex > LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->uavNum)
		{

			double visitNum = _visitedPoints.size();
			double allNum = m_roadmap.size()* m_roadmap[0].size();
			double rate = visitNum / allNum;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = rate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < flytime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = flytime;
			}
			return;
		}
	}
	double visitNum = _visitedPoints.size();
	double allNum = m_roadmap.size()* m_roadmap[0].size();
	double rate = visitNum / allNum;
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = rate;
	double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
	if (maxflyTime < _maxFlyTime)
	{
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = _maxFlyTime;
	}

	sparsePath(PointsVec);
}

void AreaSearchAlgorithm::KmeansAreaSerachPlan(casic_vector3d leftup, casic_vector3d rightdown, casic_vector3d currPos,
	double Height, double gridWidth, double speed, double angularSpeed, double upSpeed, double flyTime,
	double coverRate, double safeDis, int areaType, int areaPointsNum, std::map<int, std::vector<casic_vector3d>>& PointsVec,
	int planWay, std::map<int, std::vector<casic_vector3d>> &startReturnPointMap)
{
	allStagingPoint.clear();
	_coverAreaVec.clear();
	auto noflyZonePointVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
	auto obstacleArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->obstacleArea;
	stagingPoint = casic_vector3d(0.0, 0.0, 0.0);
	PointsVec.clear();
	_safeDistance = safeDis;
	std::vector<std::vector<shared_ptr<PlanNode>>> m_roadmap;
	m_roadmap = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->GridPointMap;
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->calcolateGridLinePoints();

	double step_dis = gridWidth;
	if (m_roadmap.size() == 0 || m_roadmap[0].size() == 0)
	{
		return;
	}
	int lonNum = m_roadmap.size();
	int latNum = m_roadmap[0].size();

	Gridsmap = m_roadmap;

	double Xindex = m_roadmap.size();
	double Yindex = m_roadmap[0].size();
	PlanNode m_startNode = PlanNode(m_roadmap[0][0]->coordinateX(), m_roadmap[0][0]->coordinateY(), 0, 0);

	if (m_roadmap.empty())
	{
		return;
	}

	std::vector<casic_vector3d>ObstacleUAVs;
	std::vector<casic_vector3d>_visitedPoints;
	std::vector<std::vector<shared_ptr<PlanNode>>>_planNodes;
	std::vector<casic_vector3d>_onePlanevisitedPoints;
	casic_vector3d pos;

	std::vector<PlanNode> motion = {
		PlanNode(0, 0,  1,  0,  1, 0),
        PlanNode(0, 0,  0,  1,  1, 0),
		PlanNode(0, 0,  1, -1,  std::sqrt(2),0),
		PlanNode(0, 0,  1,  1,  std::sqrt(2),0),
        PlanNode(0, 0, -1,  0,  1, 0),
		PlanNode(0, 0,  0, -1,  1, 0),
        PlanNode(0, 0, -1,  1,  std::sqrt(2),0),
        PlanNode(0, 0, -1, -1,  std::sqrt(2),0)
	};

	for (int i = 0; i < m_roadmap.size();i++)
	{
		for (int j = 0; j < m_roadmap[i].size(); j++)
		{
			 auto tmp = m_roadmap[i][j];
			 QPointF pointf(tmp->coordinateX(),tmp->coordinateY());
             casic_vector3d tmmPos(tmp->coordinateX(), tmp->coordinateY(), Height);
			 if (tmp->getAreaAttribute() == areaType)
			 {
				 if (tmp->isObstacle() || is_point_in_zones(pointf, noflyZonePointVec))
				 {
					 _visitedPoints.push_back(tmmPos);
				 }
			 }

		}
	}

	int Planindex = 1;
	while (_visitedPoints.size() < areaPointsNum*coverRate)
	{

		auto cmp = [](shared_ptr<PlanNode> left, shared_ptr<PlanNode> right) {

			return left->getTotalCost() > right->getTotalCost();
		};

		std::priority_queue<shared_ptr<PlanNode>,
			std::vector<shared_ptr<PlanNode>>, decltype(cmp)> O(cmp);
		std::stack<shared_ptr<PlanNode>>O1;

		double flytime = 0;

		if (!ObstacleUAVs.empty())
		{
			vector<shared_ptr<PlanNode>> tmp;
			for (auto pos : ObstacleUAVs)
			{
				for (int i = 0; i < lonNum; ++i) {
					vector<shared_ptr<PlanNode>> tmp;
					for (int j = 0; j < latNum; ++j) {
						double lon = m_roadmap[i][j]->coordinateX();
						double lat = m_roadmap[i][j]->coordinateY();
						if (lon == (double)pos.x && lat == (double)pos.y)
						{
							m_roadmap[i][j]->setVisited();
						}
					}
				}
			}
		}

		int startIndex = 0;
		int startjIndex = 0;

		shared_ptr<PlanNode> tmp = m_roadmap[startIndex][startjIndex];
		bool isReturn = false;
		while (startjIndex < m_roadmap[0].size())
		{
			startIndex = 0;
			while (startIndex < m_roadmap.size())
			{
				tmp = m_roadmap[startIndex][startjIndex];
				if (!tmp->isVisited() && !tmp->isObstacle() && tmp->getAreaAttribute() == areaType)
				{
					isReturn = true;
					break;
				}
				startIndex++;
			}
			if (isReturn)
			{
				break;
			}
			startjIndex++;
		}

		int roadIndex = Planindex;
		O.push(tmp);
		O1.push(tmp);

		std::vector<casic_vector3d> pointList;
		std::vector<casic_vector3d> startReturnPointVec;
		std::vector<casic_vector3d> stagingPointList;
		std::vector<casic_vector3d> takeoffPointList;
		casic_vector3d m_collPos = getStagingPoint(casic_vector2d(leftup.x, leftup.y), casic_vector2d(rightdown.x, rightdown.y),
			casic_vector2d(currPos.x, currPos.y), Height);

		casic_vector3d m_takeoffPos = getTakeOffPoint(casic_vector2d(leftup.x, leftup.y), casic_vector2d(rightdown.x, rightdown.y),
			casic_vector2d(currPos.x, currPos.y), Height);
        allStagingPoint.push_back(currPos);
		std::vector<double> fly_param;
		fly_param.push_back(speed);
		fly_param.push_back(angularSpeed);

		stagingPointList = pointList;
		takeoffPointList = pointList;

		int stagingPathCount = pointList.size();

        PlanNode m_takeoffNode = PlanNode(m_takeoffPos.x, m_takeoffPos.y, 0, 0, 0, 0);
		casic_vector3d tmpPo = casic_vector3d(tmp->coordinateX(), tmp->coordinateY(), Height);

        if (is_lineToAllaZones(currPos, tmpPo, noflyZonePointVec))
		{
			O1.pop();
			int Xindex = tmp->indexX();
			int Yindex = tmp->indexY();
			shared_ptr<PlanNode> tmp1 = m_roadmap[0][0];
			bool isFindPoint = false;
			for (int i = 0, Iindex = Yindex; i < m_roadmap[Xindex].size();i++)
			{
				for (int j = 0, Jindex = (Xindex + 1) % m_roadmap.size(); j < m_roadmap.size(); j++)
				{
					if (!(m_roadmap[Jindex][Iindex]->isVisited()) && !(m_roadmap[Jindex][Iindex]->isObstacle()) && m_roadmap[Jindex][Iindex]->getAreaAttribute() == areaType)
					{
						tmp1 = m_roadmap[Jindex][Iindex];
						tmpPo = casic_vector3d(tmp1->coordinateX(), tmp1->coordinateY(), Height);
                        if (!is_lineToAllaZones(currPos, tmpPo, noflyZonePointVec))
						{
							i = m_roadmap[0].size();
							isFindPoint = true;
							break;
						}
					}
					Jindex = (Jindex + 1) % m_roadmap.size();
				}
				Iindex = (Iindex + 1) % m_roadmap[Xindex].size();
			}
			if (isFindPoint)
			{
				O1.push(tmp1);
			}
        }
		if (O1.empty())
		{

			double coverRate = _visitedPoints.size();
			double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < flytime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = flytime;
			}
			return;
		}
		tmp = O1.top();
		if (tmp->isVisited())
		{
			double coverRate = _visitedPoints.size();
			double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < flytime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = flytime;
			}
			return;
		}

		pointList.push_back(currPos);
        firstPointPathPlanning(Planindex, currPos, tmpPo, safeDis, fly_param, pointList);
		startReturnPointVec.push_back(tmpPo);

		stagingPointList = pointList;
		auto miter = pointList.rbegin();
		if (miter->x != tmpPo.x || miter->y != tmpPo.y)
		{
			pointList.push_back(tmpPo);
		}

		std::vector<casic_vector3d>edpoints = getEndPoints(tmpPo, gridWidth);
		std::pair<casic_vector3d, std::vector<casic_vector3d>>_pair = std::make_pair(pos, edpoints);
		_coverAreaVec.push_back(edpoints);
		if (Planindex == 0)
		{
			_coverAreaVec.push_back(edpoints);
		}

		QGeoCoordinate pos0(currPos.y, currPos.x);
        QGeoCoordinate pos1(m_takeoffNode.coordinateY(), m_takeoffNode.coordinateX());
		QGeoCoordinate pos2(tmp->coordinateY(), tmp->coordinateX());

		flytime += 2 * fly_param[2];

		std::vector<shared_ptr<PlanNode>>_onePoints;
		PlanNode nlast;
		double AngularPass = 0;

		QGeoCoordinate lastPoint = pos1;

		shared_ptr<PlanNode>lastPlanNode = m_roadmap[m_startNode.indexX()][m_startNode.indexY()];
		shared_ptr<PlanNode>minNode = m_roadmap[m_startNode.indexX()][m_startNode.indexY()];

		casic_vector3d startPos = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->curpos;
		QGeoCoordinate StartPosGeo(startPos.y, startPos.x);
		int calCount = 0;
		double returnFlyTime = 0;

        queue<int> premotion12;

		while (flytime <= flyTime  && _onePlanevisitedPoints.size() <= areaPointsNum* coverRate)
		{
			if (O1.empty())
			{
				shared_ptr<PlanNode> tmp1 = m_roadmap[0][0];
				for (int i = 0; i < m_roadmap.size();i++)
				{
					for (int j = 0; j < m_roadmap[i].size(); j++)
					{
						if (m_roadmap[i][j]->isVisited() == false)
						{
							tmp1 = m_roadmap[i][j];
							i = m_roadmap.size();
							break;
						}
					}
				}

				O1.push(tmp1);
				roadIndex++;
			}

			shared_ptr<PlanNode> nBest = O1.top();

			if (_onePoints.size() == 0)
			{

                nlast = m_takeoffNode;
			}
			QGeoCoordinate nBestQGeo = QGeoCoordinate(nBest->coordinateY(), nBest->coordinateX());

			casic_vector3d posnBest = casic_vector3d(nBest->coordinateX(), nBest->coordinateY(), Height);
			O1.pop();

			nBest->setVisited();

			_onePlanevisitedPoints.push_back(casic_vector3d(nBest->coordinateX(), nBest->coordinateY(), Height));
			int index = 0;
            std::map<int, shared_ptr<PlanNode>> minPoint_map;
            std::map<int, int> motion_map;
			minPoint_map.clear();
            bool flag = false;
			for (int i = 0; i < motion.size(); i++)
			{
                flag = false;

				index++;

                int curi = i;

                if(planWay == 0 && premotion12.size() == 2){
                    int pre1 = premotion12.front();
                    premotion12.pop();
                    int pre2 = premotion12.front();
                    if(pre1 != pre2){
                        i = (pre1 + 4) % 8;
                    }
                    flag = true;
                }

				if (planWay == 1) {
					flag = false;
				}

				PlanNode tmpNode = motion[i];
				nBest->setG(0.0);
				tmpNode += *nBest;
				int xxx = tmpNode.indexX();
				int yyy = tmpNode.indexY();
				if (tmpNode.indexX() < 0 || tmpNode.indexY() < 0 || tmpNode.indexX() >= lonNum || tmpNode.indexY() >= latNum)
				{
                    if(flag){
                        i = curi - 1;
                    }
					continue;
				}

				shared_ptr<PlanNode> tmpNodePointer =
					m_roadmap[tmpNode.indexX()][tmpNode.indexY()];

				if (tmpNodePointer->getAreaAttribute() != areaType)
				{
                    if(flag){
                        i = curi - 1;
                    }
					continue;
				}

				casic_vector3d tmpNodePos = casic_vector3d(tmpNodePointer->coordinateX(), tmpNodePointer->coordinateY(), Height);
				if (is_lineToAllaZones(posnBest, tmpNodePos, noflyZonePointVec))
				{
					if (std::find(_visitedPoints.begin(), _visitedPoints.end(), tmpNodePos) == _visitedPoints.end())
					{
						_visitedPoints.push_back(tmpNodePos);
					}
                    if(flag){
                        i = curi - 1;
                    }
					continue;
				}

                if (tmpNodePointer->isObstacle() || tmpNodePointer->isVisited())
                {
                    if(flag){
                        i = curi - 1;
                    }
					continue;
				}

				if (isInterObstacle(posnBest, tmpNodePos, obstacleArea, gridWidth))
				{
                    if(flag){
                        i = curi - 1;
                    }
					continue;
				}

				QGeoCoordinate tmpPoint(tmpNodePointer->coordinateY(), tmpNodePointer->coordinateX());
				double lastAngle = lastPoint.azimuthTo(nBestQGeo);
				double nowAngle = nBestQGeo.azimuthTo(tmpPoint);

				double disAngle = fabs(nowAngle - lastAngle);

				if (disAngle > 180)
				{
					double maxAngle = max(lastAngle, nowAngle);
					double minAngle = min(lastAngle, nowAngle);
					minAngle += 360.0;
					disAngle = fabs(minAngle - maxAngle);
                }

                double Gcost = tmpNode.getGCost() + disAngle / 180.0;
				tmpNodePointer->setG(Gcost);
				int hcost = Gridsmap[tmpNode.indexX()][tmpNode.indexY()]->getHCost();
				Gcost -= hcost;
                minPoint_map[(int)(Gcost * 100)] = tmpNodePointer;
                motion_map[(int)(Gcost * 100)] = i;
			}
			lastPoint = nBestQGeo;

			if (minPoint_map.size() == 0)
			{
				QPoint nowPointXY(nBest->indexX(), nBest->indexY());

				std::vector<QPoint> nextPoints = getNextPointFromImpasse(nowPointXY, m_roadmap);

				if (nextPoints.size() == 0)
				{
					break;
				}

				std::vector<QPoint> nextPathPoints;
				nextPathPoints.clear();

				QPoint nextPointXY;
				shared_ptr<PlanNode> nextPlanNode = nullptr;
				QGeoCoordinate nextPointGeo;
				casic_vector3d nextPointLLA;
				bool connected = false;
				for (int nextIndex = 0; nextIndex < nextPoints.size(); nextIndex++)
				{
					nextPointXY = nextPoints[nextIndex];
					nextPlanNode = m_roadmap[nextPointXY.x()][nextPointXY.y()];
					nextPointGeo = QGeoCoordinate(nextPlanNode->coordinateY(), nextPlanNode->coordinateX());
					nextPointLLA = casic_vector3d(nextPlanNode->coordinateX(), nextPlanNode->coordinateY(), Height);

					bool isInterset = is_lineToAllaZones(posnBest, nextPointLLA, noflyZonePointVec);
					bool isIntersetObs = isInterObstacle(posnBest, nextPointLLA, obstacleArea);
					if (isInterset == false && isIntersetObs == false)
					{
						nextPlanNode->setBackpoint(nBest);
						double dis = nextPointGeo.distanceTo(nBestQGeo);
						flytime += 2 * (dis / speed);
						pointList.push_back(nextPointLLA);
						_visitedPoints.push_back(nextPointLLA);
						nextPlanNode->setVisited();
						O1.push(nextPlanNode);
						connected = true;
						break;
					}
				}
				if (connected == true)
				{
					continue;
				}
				else
				{
					for (int nextIndex = 0; nextIndex < nextPoints.size(); nextIndex++)
					{

						nextPointXY = nextPoints[nextIndex];
						nextPlanNode = m_roadmap[nextPointXY.x()][nextPointXY.y()];
						nextPointGeo = QGeoCoordinate(nextPlanNode->coordinateY(), nextPlanNode->coordinateX());
						nextPointLLA = casic_vector3d(nextPlanNode->coordinateX(), nextPlanNode->coordinateY(), Height);

						std::map<shared_ptr<PlanNode>, int> V2PlanNode;
						V2PlanNode.clear();
						std::vector<std::vector<int>> graph;
						graph.clear();

						createMGraph(m_roadmap, areaType, V2PlanNode, graph);
						MGraph* mGraph = new MGraph;
						auto iBeginItor = V2PlanNode.find(nBest);
						auto iEndItor = V2PlanNode.find(nextPlanNode);
						int iBegin;
						int iEnd;
						if (iBeginItor != V2PlanNode.end() && iEndItor != V2PlanNode.end())
						{
							iBegin = iBeginItor->second;
							iEnd = iEndItor->second;
						}
						else
						{
							break;
						}
						std::vector<int> stackVertices = mGraph->getShortPath(graph, iBegin, iEnd);

						int stackSize = stackVertices.size();
						if (stackSize <= 0)
						{
							continue;
						}

						casic_vector3d lastPoint = posnBest;
						bool isConnect = true;
						for (int index = stackSize - 1; index >= 0; index--)
						{
							int k = stackVertices[index];
							auto V2planNodeItor = V2PlanNode.begin();
							while (V2planNodeItor->second != k)
							{
								V2planNodeItor++;
							}
							auto planNodeItor = V2planNodeItor->first;
							casic_vector3d nextPoint(planNodeItor->coordinateX(), planNodeItor->coordinateY(), Height);
							bool isInterNoFly = is_lineToAllaZones(lastPoint, nextPoint, noflyZonePointVec);
							bool isInterOb = isInterObstacle(lastPoint, nextPoint, obstacleArea);
							if (isInterNoFly == false && isInterOb == false)
							{
								lastPoint = nextPoint;
								nextPathPoints.push_back(QPoint(planNodeItor->indexX(), planNodeItor->indexY()));
								lastPoint = nextPoint;
							}
							else
							{
								isConnect = false;
								nextPathPoints.clear();
								break;
							}
						}
						if (isConnect == false)
						{
							continue;
						}
						else
						{
							break;
						}
					}
				}

				if (nextPathPoints.size() > 0)
				{
					auto lastNode = nBest;
					for (int i = 0; i < nextPathPoints.size(); i++)
					{
						QPoint nextPathPoint = nextPathPoints[i];
						auto nextNode = Gridsmap[nextPathPoint.x()][nextPathPoint.y()];
						casic_vector3d nextPlanLLA(nextNode->coordinateX(), nextNode->coordinateY(), 0);
						nextNode->setBackpoint(lastPlanNode);
						nextNode->setVisited();
						lastNode = nextNode;
						pointList.push_back(nextPlanLLA);
					}
					nextPlanNode->setBackpoint(lastNode);
					O1.push(nextPlanNode);
					pointList.push_back(nextPointLLA);
					nextPlanNode->setVisited();
					continue;
				}
				else if (connected == true)
				{
					continue;
				}
				else
				{
					break;
				}
			}

			auto nextPointer = minPoint_map.begin()->second;
            int curmotion = motion_map[minPoint_map.begin()->first];
            premotion12.push(curmotion);

			nextPointer->setBackpoint(nBest);
			flytime += 2 * (nextPointer->getGCost() * step_dis / speed);

			casic_vector3d nextPos(nextPointer->coordinateX(), nextPointer->coordinateY(), Height);
			QGeoCoordinate nextPosGeo(nextPos.y, nextPos.x);
			pointList.push_back(nextPos);
			_visitedPoints.push_back(nextPos);

			nextPointer->setVisited();
			O1.push(nextPointer);
			lastPlanNode = nextPointer;
			double returnDis = nextPosGeo.distanceTo(StartPosGeo);
			returnFlyTime = returnDis / speed;
			calCount++;
		}

		startReturnPointVec.push_back(pointList[pointList.size() - 1]);

		double totalTime = flytime;
		int way = 1;

		if (way == 1) {
			std::vector<casic_vector3d> searchPointList = pointList;
			QGeoCoordinate leftup_x(lastPlanNode->coordinateY(), lastPlanNode->coordinateX());
			QGeoCoordinate rightup(lastPlanNode->coordinateY(), currPos.x);
			QGeoCoordinate rightdown(currPos.y, currPos.x);
			QGeoCoordinate leftdown(currPos.y, lastPlanNode->coordinateX());
			double mapLength = leftup_x.distanceTo(rightup);
			double mapWidth = leftup_x.distanceTo(leftdown);
			double angleLehgth = leftup_x.azimuthTo(rightup);
			double angleWidth = leftup_x.azimuthTo(leftdown);
			if (fabs(angleWidth - 0) < 1 && fabs(angleLehgth - 270) < 1)
				angleWidth += 360.0;
			if (fabs(angleLehgth - 0) < 1 && fabs(angleWidth - 270) < 1)
				angleLehgth += 360.0;
			double angleLine = (angleLehgth + angleWidth) / 2;
			double temp[] = { mapLength, mapWidth , angleLehgth , angleWidth , angleLine };
			std::vector<double> gridMapParams(temp, temp + sizeof(temp) / sizeof(temp[0]));
            short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(currPos.x, currPos.y);
            if (elevationTemp < -10000)
            {
                elevationTemp = 0;
            }
            double m_alt = elevationTemp;

            int height_x = Height + Planindex * safeDis * 2;

			allGridsMaps[Planindex] = createGridsNode(Planindex, height_x, speed, gridMapParams, leftup_x);

			addNoflyToObstacleArea(Planindex, height_x, noflyZonePointVec);

			auto start_x = casic_vector3d(lastPlanNode->coordinateX(), lastPlanNode->coordinateY(), height_x);
			returnPathPlanning(Planindex, start_x, currPos, safeDis, fly_param, searchPointList);
            if (fly_param[3] + (height_x / upSpeed) < totalTime / 2 && (fly_param[4] - 0.0) > 0.001) {
                totalTime = totalTime / 2 + fly_param[3] + (height_x / upSpeed);
				pointList = searchPointList;
			}
			else {
				int len = pointList.size();
				for (int i = len - 1; i >= 0; i--) {
					pointList.push_back(pointList[i]);
				}
			}

		}
		if (way == 2) {

			tmp = lastPlanNode;
			int count = 0;
			std::vector<casic_vector3d>tmpsearchList;
			casic_vector3d firstPoint = casic_vector3d(0, 0, 0);

			pointList.push_back(*pointList.rbegin());
			auto itor = pointList.rbegin();
			itor->z = Height + (Planindex + 1) * 10;

			casic_vector3d sPoint(m_takeoffPos.x, m_takeoffPos.y, Height + (Planindex + 1) * 10);
			pointList.push_back(sPoint);

			pointList.push_back(casic_vector3d(m_takeoffPos.x, m_takeoffPos.y, Height));

			int co = pointList.size();
			int reCount = co - stagingPathCount;
			for (co = co - 2; co >= stagingPathCount; co--)
			{
				pos = pointList[co];

				tmpsearchList.emplace_back(pos);

				if (std::find(stagingPointList.begin(), stagingPointList.end(), pos) == stagingPointList.end())
				{
					_visitedPoints.push_back(pos);
				}
				ObstacleUAVs.emplace_back(pos);
				count++;
			}

			_visitedPoints.push_back(*stagingPointList.rbegin());

			for (auto pos : tmpsearchList)
			{
				std::vector<casic_vector3d>edpoints = getEndPoints(pos, gridWidth);
				std::pair<casic_vector3d, std::vector<casic_vector3d>>_pair = std::make_pair(pos, edpoints);
				_coverAreaVec.push_back(edpoints);
			}

			pointList.push_back(*pointList.rbegin());
			auto imtor = pointList.rbegin();
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(imtor->x, imtor->y);
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			imtor->z = elevationTemp;
		}

		std::pair<int, std::vector<casic_vector3d>>mpair = std::make_pair(Planindex + 1, pointList);
		PointsVec.insert(mpair);
		std::pair<int, std::vector<casic_vector3d>>mpair2 = std::make_pair(Planindex + 1, startReturnPointVec);
		startReturnPointMap.insert(mpair2);
		Planindex++;
		if (_onePlanevisitedPoints.size() >= areaPointsNum
			|| _visitedPoints.size() > areaPointsNum)
		{

			double coverRate = _visitedPoints.size();
			double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < totalTime)
			{

				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = totalTime;
			}
			return;
		}
		if (Planindex > 1)
		{

			double coverRate = _visitedPoints.size();
			double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < totalTime)
			{

				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = totalTime;
			}
			return;
		}
	}
	double coverRate1 = _visitedPoints.size();
	double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate1;

	sparsePath(PointsVec, startReturnPointMap);
}

void AreaSearchAlgorithm::SpySideAreaSerachPlan(casic_vector3d leftup, casic_vector3d rightdown, casic_vector3d currPos, double Height,
	double gridWidth, double speed, double angularSpeed, double flyTime, double coverRate, double safeDis, int minAngle, int maxAngle,
	int podTurnSpeed, int uavNum, std::map<int, std::vector<casic_vector3d>>& PointsVec)
{
	PointsVec.clear();
	double _maxFlyTime = 0;
	allStagingPoint.clear();
	_coverAreaVec.clear();
	auto noflyZonePointVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
	auto obstacleArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->obstacleArea;
	stagingPoint = casic_vector3d(0.0, 0.0, 0.0);
	PointsVec.clear();
	_safeDistance = safeDis;
	std::vector<std::vector<shared_ptr<PlanNode>>> m_roadmap;
	m_roadmap = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->GridPointMap;
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->calcolateGridLinePoints();
	QGeoCoordinate startGeoPos(currPos[1], currPos[0], currPos[2]);
	double step_dis = gridWidth;
	if (m_roadmap.size() == 0 || m_roadmap[0].size() == 0)
	{
		return;
	}
	int lonNum = m_roadmap.size();
	int latNum = m_roadmap[0].size();
	Gridsmap = m_roadmap;
	int minDis = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->calSpyDis(Height, minAngle);
	int maxDis = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->calSpyDis(Height, maxAngle);
	std::vector<casic_vector3d> nearlyLenSide = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->getNearlyLenSide();
	QGeoCoordinate min1(nearlyLenSide[0][1], nearlyLenSide[0][0], nearlyLenSide[0][2]);
	QGeoCoordinate min2(nearlyLenSide[1][1], nearlyLenSide[1][0], nearlyLenSide[1][2]);
	QGeoCoordinate min1toMin(nearlyLenSide[2][1], nearlyLenSide[2][0], nearlyLenSide[2][2]);
	QGeoCoordinate min2toMin(nearlyLenSide[3][1], nearlyLenSide[3][0], nearlyLenSide[3][2]);

	double lenDir = min1.azimuthTo(min2);
	double shortDir = min1.azimuthTo(min1toMin);

	int targetShort = min1.distanceTo(min1toMin);
	int targetLen = min1.distanceTo(min2);

	int podTurnTime = fabs(minAngle - maxAngle) / podTurnSpeed * 2;

	double spyDir = lenDir;

	double nextSpyDir = shortDir;
	QGeoCoordinate sideMin1 = min1.atDistanceAndAzimuth(minDis + gridWidth / 2, nextSpyDir - 180);

	QGeoCoordinate firstPos = sideMin1.atDistanceAndAzimuth(gridWidth / 2, spyDir);
	QGeoCoordinate nowGeoPos = firstPos;
	QGeoCoordinate nextGeoPos = nowGeoPos;

	double spyNum = min1.distanceTo(min1toMin) / (maxDis - minDis) + 1;
	double spyIndex = 1;

	int spyPosNum = m_roadmap.size() > m_roadmap[0].size() ? m_roadmap.size() : m_roadmap[0].size();
	int spyPosIndex = 0;

	double transTime = gridWidth * 1.0 / speed + podTurnTime;

	for (int uavIndex = 0; uavIndex < uavNum; uavIndex++)
	{
		bool timeIsEnough = true;
		bool isCovered = false;
		std::vector<casic_vector3d> pathPoss;
		pathPoss.clear();
		pathPoss.push_back(currPos);

		if (spyIndex > spyNum)
		{
			return;
		}
		int flyTime_residue = flyTime - (startGeoPos.distanceTo(nowGeoPos) / speed) - podTurnTime;
		if (flyTime_residue - startGeoPos.distanceTo(nowGeoPos) / speed > 10)
		{
			spyPosIndex++;
			pathPoss.push_back(casic_vector3d(nowGeoPos.longitude(), nowGeoPos.latitude(), Height));
		}
		else
		{
			return;
		}
		while (spyIndex <= spyNum)
		{
			while (spyPosIndex < spyPosNum)
			{
				nextGeoPos = nowGeoPos.atDistanceAndAzimuth(gridWidth, spyDir);
				if (spyPosIndex == 1 && spyDir == lenDir)
				{
					firstPos = nowGeoPos;
				}
				else if (spyPosIndex == spyPosNum - 1 && spyDir == lenDir - 180)
				{
					firstPos = nextGeoPos;
				}
				if (flyTime_residue - transTime - startGeoPos.distanceTo(nextGeoPos) / speed > 10)
				{
					flyTime_residue = flyTime_residue - transTime;
					nowGeoPos = nextGeoPos;
					spyPosIndex++;
					pathPoss.push_back(casic_vector3d(nextGeoPos.longitude(), nextGeoPos.latitude(), Height));

				}
				else
				{
					nowGeoPos = nextGeoPos;
					timeIsEnough = false;

					break;
				}
			}

			if (spyPosIndex >= spyPosNum)
			{
				spyIndex++;
				if (spyIndex > spyNum)
				{
					isCovered = true;
					break;
				}
				spyPosIndex = 0;
			}
			if (timeIsEnough)
			{

				nextGeoPos = nowGeoPos.atDistanceAndAzimuth(maxDis - minDis, nextSpyDir);
				if (flyTime_residue - (nextGeoPos.distanceTo(nowGeoPos) + nextGeoPos.distanceTo(startGeoPos)) / speed - podTurnTime > 10)
				{
					flyTime_residue = flyTime_residue - podTurnTime - nextGeoPos.distanceTo(nowGeoPos) / speed;
					nowGeoPos = nextGeoPos;
					spyDir = spyDir - 180;
					spyPosIndex++;

					pathPoss.push_back(casic_vector3d(nowGeoPos.longitude(), nowGeoPos.latitude(), Height));
				}
				else
				{
					nowGeoPos = firstPos.atDistanceAndAzimuth(maxDis - minDis, nextSpyDir);

					spyDir = lenDir;

					break;
				}
			}
			else
			{
				if (spyIndex > spyNum )
				{
					isCovered = true;

				}
				break;
			}
		}
		pathPoss.push_back(currPos);
		PointsVec[uavIndex] = pathPoss;
		if (isCovered)
			break;
	}
}

void AreaSearchAlgorithm::sparsePath(std::map<int, std::vector<casic_vector3d>>& AreaSearchPointMap, std::map<int, std::vector<casic_vector3d>> &searchStartReturnPointMap)
{
	double angleDiffMax = 1;

	auto PathItor = AreaSearchPointMap.begin();
	for (PathItor; PathItor != AreaSearchPointMap.end(); PathItor++)
	{
		int planIndex = PathItor->first;
		std::vector<casic_vector3d> startreturnpoint;
		if(searchStartReturnPointMap.size() > 0)
			startreturnpoint = searchStartReturnPointMap[planIndex];
		std::vector<casic_vector3d>* tempPath = &(PathItor->second);
		if(tempPath->size() <= 3)
			continue;
		auto pointItor = tempPath->begin();
		QGeoCoordinate point1(pointItor->y, pointItor->x, pointItor->z);
		pointItor++;

		auto point2Itor = pointItor;
		int mindex = 0;
		QGeoCoordinate point2(pointItor->y, pointItor->x, pointItor->z);
		pointItor++;
		for (pointItor; pointItor != tempPath->end(); )
		{
			bool is_startreturnPoint = false;
			for (auto& poi : startreturnpoint) {
				if (std::fabs(point2Itor->x - poi.x) <= 1e-6 && std::fabs(point2Itor->y - poi.y) <= 1e-6)
					is_startreturnPoint = true;
			}

			QGeoCoordinate point3(pointItor->y, pointItor->x, pointItor->z);

			casic_vector3d nowPoint = *pointItor;
			bool isEqual = false;
			for (int stagIndex = 0; stagIndex < allStagingPoint.size(); stagIndex++)
			{
				QGeoCoordinate newPointGeo(nowPoint.y, nowPoint.x);
				QGeoCoordinate stagingPoint(allStagingPoint[stagIndex].y, allStagingPoint[stagIndex].x);
				double dis = newPointGeo.distanceTo(stagingPoint);
				if (nowPoint.x == allStagingPoint[stagIndex].x && nowPoint.y == allStagingPoint[stagIndex].y)
				{
					point1 = point2;
					point2 = point3;
					point2Itor = pointItor;
					pointItor++;
					isEqual = true;
					break;
				}
			}
			if (isEqual)
			{
				continue;
			}

			if (mindex >= tempPath->size() - 6)
			{
				pointItor++;
				continue;
			}

			double angle1_2 = point1.azimuthTo(point2);
			double angle2_3 = point2.azimuthTo(point3);
			double angleDiff = fabs(angle1_2 - angle2_3);
			if (angleDiff < angleDiffMax)
			{
				if(!is_startreturnPoint)
					pointItor = tempPath->erase(point2Itor);
				point2 = point3;
				point2Itor = pointItor;
				pointItor++;
			}
			else
			{
				point1 = point2;
				point2 = point3;
				point2Itor = pointItor;
				pointItor++;
			}

			mindex++;
		}
	}
}

double AreaSearchAlgorithm::calAngle(casic_vector2d point1, casic_vector2d centerPoint, casic_vector2d point3)
{
	double theta = atan2(point1[0] - centerPoint[0], point1[1] - centerPoint[1]) -
				   atan2(point3[0] - centerPoint[0], point3[1] - centerPoint[1]);
	if (theta > PI)
	{
		theta -= 2 * PI;
	}
	if (theta < -PI)
	{
		theta += 2 * PI;
	}
	theta = abs(theta * 180.0 / PI);
	return theta;
}

std::vector<casic_vector3d> AreaSearchAlgorithm::getEndPoints(casic_vector3d pos, double gridWidth)
{
	std::vector<casic_vector3d>tmpVec;
	QGeoCoordinate mpos(pos.y, pos.x);
	casic_vector3d tmppos;
	QGeoCoordinate tmp = mpos.atDistanceAndAzimuth(gridWidth*sqrt(2) / 2, 45);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	tmp = mpos.atDistanceAndAzimuth(gridWidth*sqrt(2) / 2, 135);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	tmp = mpos.atDistanceAndAzimuth(gridWidth*sqrt(2) / 2, 225);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	tmp = mpos.atDistanceAndAzimuth(gridWidth*sqrt(2) / 2, 315);
	tmppos = casic_vector3d(tmp.longitude(), tmp.latitude(), tmp.altitude());
	tmpVec.push_back(tmppos);
	return tmpVec;
}

std::vector<QPoint> AreaSearchAlgorithm::getNextPointFromImpasse(QPoint nowPoint, std::vector<std::vector<shared_ptr<PlanNode>>> GridMap)
{
	std::vector<QPoint> nextPoints;
	nextPoints.clear();
	QPoint nextPoint;
	auto noflyAreas = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
	auto obstacleArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->obstacleArea;

	int nowXindex = nowPoint.x();
	int nowYindex = nowPoint.y();
	if (nowXindex >= GridMap.size() )
	{
		nowXindex = 0;
	}
	shared_ptr<PlanNode> nowGridPoint = GridMap[nowXindex][nowYindex];
	casic_vector3d nowPointLLA = casic_vector3d(nowGridPoint->coordinateX(), nowGridPoint->coordinateY(), 0);
	int areaLable = nowGridPoint->getAreaAttribute();

	bool isReturn = true;
	int cycleIndex = 1;

	while (isReturn)
	{
		int startX = nowXindex - cycleIndex;
		int stopX = nowXindex + cycleIndex;
		int startY = nowYindex - cycleIndex;
		int stopY = nowYindex + cycleIndex;
		if (cycleIndex > max(GridMap.size(), GridMap[0].size()))
		{
			return nextPoints;
		}

		for (int x1 = startX; x1 < stopX; x1++)
		{
			if (x1 < GridMap.size() && x1 >= 0)
			{
				if (startY < GridMap[x1].size() && startY >= 0)
				{
					auto tmpPoint = GridMap[x1][startY];
					QPointF tmpQPoint(tmpPoint->coordinateX(), tmpPoint->coordinateY());
					bool isInNofly = is_point_in_zones(tmpQPoint, noflyAreas);
					if (tmpPoint->getAreaAttribute() == areaLable && !tmpPoint->isVisited() && !isInNofly && !tmpPoint->isObstacle())
					{
						nextPoint.setX(x1);
						nextPoint.setY(startY);

						casic_vector3d tmpPointLLA(tmpPoint->coordinateX(), tmpPoint->coordinateY(), 0);
						bool isInternet = is_lineToAllaZones(nowPointLLA, tmpPointLLA, noflyAreas);
						bool isInternetObs = isInterObstacle(nowPointLLA, tmpPointLLA, obstacleArea);
						if (isInternet == false && isInternetObs == false)
						{
							nextPoints.clear();
							nextPoints.push_back(nextPoint);
							return nextPoints;
						}
						else
						{
							nextPoints.push_back(nextPoint);
						}
					}
				}
			}
		}

		for (int y1 = startY; y1 < stopY; y1++)
		{
			if (y1 < GridMap[0].size() && y1 >= 0)
			{
				if (stopX < GridMap.size() && stopX >= 0)
				{
					auto tmpPoint = GridMap[stopX][y1];
					QPointF tmpQPoint(tmpPoint->coordinateX(), tmpPoint->coordinateY());
					bool isInNofly = is_point_in_zones(tmpQPoint, noflyAreas);
					if (tmpPoint->getAreaAttribute() == areaLable && !tmpPoint->isVisited() && !isInNofly && !tmpPoint->isObstacle())
					{
						nextPoint.setX(stopX);
						nextPoint.setY(y1);

						casic_vector3d tmpPointLLA(tmpPoint->coordinateX(), tmpPoint->coordinateY(), 0);
						bool isInternet = is_lineToAllaZones(nowPointLLA, tmpPointLLA, noflyAreas);
						bool isInternetObs = isInterObstacle(nowPointLLA, tmpPointLLA, obstacleArea);
						if (isInternet == false && isInternetObs == false)
						{
							nextPoints.clear();
							nextPoints.push_back(nextPoint);
							return nextPoints;
						}
						else
						{
							nextPoints.push_back(nextPoint);
						}
					}
				}
			}
		}

		for (int x2 = stopX; x2 > startX; x2--)
		{
			if (x2 < GridMap.size() && x2 >= 0)
			{
				if (stopY < GridMap[x2].size() && stopY >= 0)
				{
					auto tmpPoint = GridMap[x2][stopY];
					QPointF tmpQPoint(tmpPoint->coordinateX(), tmpPoint->coordinateY());
					bool isInNofly = is_point_in_zones(tmpQPoint, noflyAreas);
					if (tmpPoint->getAreaAttribute() == areaLable && !tmpPoint->isVisited() && !isInNofly && !tmpPoint->isObstacle())
					{
						nextPoint.setX(x2);
						nextPoint.setY(stopY);

						casic_vector3d tmpPointLLA(tmpPoint->coordinateX(), tmpPoint->coordinateY(), 0);
						bool isInternet = is_lineToAllaZones(nowPointLLA, tmpPointLLA, noflyAreas);
						bool isInternetObs = isInterObstacle(nowPointLLA, tmpPointLLA, obstacleArea);
						if (isInternet == false && isInternetObs == false)
						{
							nextPoints.clear();
							nextPoints.push_back(nextPoint);
							return nextPoints;
						}
						else
						{
							nextPoints.push_back(nextPoint);
						}
					}
				}
			}
		}

		for (int y2 = stopY; y2 > startY; y2--)
		{
			if (y2 < GridMap[0].size() && y2 >= 0)
			{
				if (startX < GridMap.size() && startX >= 0)
				{
					auto tmpPoint = GridMap[startX][y2];
					QPointF tmpQPoint(tmpPoint->coordinateX(), tmpPoint->coordinateY());
					bool isInNofly = is_point_in_zones(tmpQPoint, noflyAreas);
					if (tmpPoint->getAreaAttribute() == areaLable && !tmpPoint->isVisited() && !isInNofly && !tmpPoint->isObstacle())
					{
						nextPoint.setX(startX);
						nextPoint.setY(y2);

						casic_vector3d tmpPointLLA(tmpPoint->coordinateX(), tmpPoint->coordinateY(), 0);
						bool isInternet = is_lineToAllaZones(nowPointLLA, tmpPointLLA, noflyAreas);
						bool isInternetObs = isInterObstacle(nowPointLLA, tmpPointLLA, obstacleArea);
						if (isInternet == false && isInternetObs == false)
						{
							nextPoints.clear();
							nextPoints.push_back(nextPoint);
							return nextPoints;
						}
						else
						{
							nextPoints.push_back(nextPoint);
						}
					}
				}
			}
		}

		cycleIndex++;
	}
	return nextPoints;
}

void AreaSearchAlgorithm::createMGraph(const std::vector<std::vector<shared_ptr<PlanNode>>> m_roadMap, int areaLable, std::map<shared_ptr<PlanNode>, int>& V2PlanNode, std::vector<std::vector<int>>& mGraph)
{
	if (m_roadMap.empty())
	{
		return;
	}

	auto noflyArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
	auto ObstacleArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->obstacleArea;

	int planNodeCount = 0;
	for (int xIndex = 0; xIndex < m_roadMap.size(); xIndex++)
	{
		for (int yIndex = 0; yIndex < m_roadMap[xIndex].size(); yIndex++)
		{
			auto tmpPlanNode = m_roadMap[xIndex][yIndex];
			QPointF tmpPointLLA(tmpPlanNode->coordinateX(), tmpPlanNode->coordinateY());
			bool isInNoFly = is_point_in_zones(tmpPointLLA, noflyArea);
			if (tmpPlanNode->getAreaAttribute() == areaLable && isInNoFly == false && !tmpPlanNode->isObstacle())
			{
				V2PlanNode[tmpPlanNode] = planNodeCount;
				planNodeCount++;
			}
		}
	}

	mGraph.resize(planNodeCount);
	for (int i = 0; i < planNodeCount; i++)
	{
		mGraph[i].resize(planNodeCount);
	}

	auto planNodeItor1 = V2PlanNode.begin();
	while (planNodeItor1 != V2PlanNode.end())
	{
		auto planNode1 = planNodeItor1->first;
		int index1 = planNodeItor1->second;
		int xIndex1 = planNode1->indexX();
		int yIndex1 = planNode1->indexY();
		casic_vector3d point1(planNode1->coordinateX(), planNode1->coordinateY(), 0);

		auto planNodeItor2 = planNodeItor1;
		while (planNodeItor2 != V2PlanNode.end())
		{
			if (planNodeItor2 == planNodeItor1)
			{
				mGraph[index1][index1] = 0;
				planNodeItor2++;
				continue;
			}
			auto planNode2 = planNodeItor2->first;
			int index2 = planNodeItor2->second;
			int xIndex2 = planNode2->indexX();
			int yIndex2 = planNode2->indexY();
			casic_vector3d point2(planNode2->coordinateX(), planNode2->coordinateY(), 0);

			int dis = fabs(xIndex1 - xIndex2) + fabs(yIndex1 - yIndex2);
			if (dis == 0)
			{
				mGraph[index1][index2] = 0;
				mGraph[index2][index1] = 0;
			}
			else if (dis == 1)
			{
				if (is_lineToAllaZones(point1, point2, noflyArea) || isInterObstacle(point1, point2, ObstacleArea))
				{
					mGraph[index1][index2] = -1;
					mGraph[index2][index1] = -1;
				}
				else
				{
					mGraph[index1][index2] = 1;
					mGraph[index2][index1] = 1;
				}
			}
			else
			{
				mGraph[index1][index2] = -1;
				mGraph[index2][index1] = -1;
			}

			planNodeItor2++;
		}
		planNodeItor1++;
	}
}

std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> AreaSearchAlgorithm::createGridsNode(int uid, double height, double cruiseSpeed,
	const std::vector<double>& gridMapParams, const QGeoCoordinate& gridMapLeftup)
{
	std::vector<std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>>> GridsMap;
	GridsMap.clear();
	allObstacleArea[uid].clear();

	if (gridMapParams.size() < 5) return GridsMap;

	double gridWidth = cruiseSpeed;
	double mapLength = gridMapParams[0], mapWidth = gridMapParams[1];
	double angleLength = gridMapParams[2], angleWidth = gridMapParams[3], angleLine = gridMapParams[4];

	int i = 0;
	for (int lp = 0; lp < mapLength; lp += gridWidth) {
		QGeoCoordinate pointToLeftup = gridMapLeftup.atDistanceAndAzimuth(lp, angleLength);
		std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>> pointsLine;
		pointsLine.clear();
		int j = 0;
		for (int wp = 0; wp < mapWidth; wp += gridWidth) {
			QGeoCoordinate pointRightdown = pointToLeftup.atDistanceAndAzimuth(wp, angleWidth);
			QGeoCoordinate gridCenterPoint = pointRightdown.atDistanceAndAzimuth(gridWidth * sqrt(2) / 2,
				angleLine);
			auto m_node = std::make_shared<AreaSearchAlgorithm::PlanNode>(gridCenterPoint.longitude(), gridCenterPoint.latitude(), i, j);
			m_node->setAreaAttribute(0);
			pointsLine.push_back(m_node);

			double elevationTemp = (double)LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(gridCenterPoint.longitude(), gridCenterPoint.latitude());
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			if (elevationTemp > height) {
				allObstacleArea[uid].push_back(casic_vector3d(gridCenterPoint.longitude(), gridCenterPoint.latitude(), height));
				pointsLine[j]->setObstacle();
			}
			j++;
		}
		GridsMap.push_back(pointsLine);
		i++;
	}

	return GridsMap;
}

void AreaSearchAlgorithm::addNoflyToObstacleArea(int uid, double height, const std::vector<LLQPoints>& noFlyZonePoints)
{
	for (int i = 0; i < allGridsMaps[uid].size(); i++) {
		for (int j = 0; j < allGridsMaps[uid][i].size(); j++) {
			double lon = allGridsMaps[uid][i][j]->coordinateX();
			double lat = allGridsMaps[uid][i][j]->coordinateY();
			QPointF point = QPointF(lon, lat);
			bool is_inside = false;
			for (auto area : noFlyZonePoints) {
				if (is_point_in_zone(point, area)) {
					is_inside = true;
					break;
				}
			}

			if (is_inside) {
				allObstacleArea[uid].push_back(casic_vector3d(lon, lat, height));
				allGridsMaps[uid][i][j]->setObstacle();
			}
		}
	}
}

void AreaSearchAlgorithm::returnPathPlanning(int uid, casic_vector3d start, casic_vector3d goal, double safeDis, std::vector<double>& fly_param, std::vector<casic_vector3d>& stagePointPlanMap)
{
	auto sg1 = casic_vector3d(start.x, goal.y, goal.z);
	auto sg2 = casic_vector3d(goal.x, start.y, goal.z);
	QGeoCoordinate point1(start.y, start.x);
	QGeoCoordinate point2(sg1.y, sg1.x);
	QGeoCoordinate point3(goal.y, goal.x);
	QGeoCoordinate point4(sg2.y, sg2.x);

	double length_ = point1.distanceTo(point2);
	double width_ = point1.distanceTo(point4);
	double alt_ = start.z;

	std::vector<std::vector<std::vector<double>>> gridMap;
	for (int i = 0; i < allGridsMaps[uid].size(); i++) {
		std::vector<std::vector<double>> tmp;
		for (int j = 0; j < allGridsMaps[uid][i].size(); j++) {
			double lon = allGridsMaps[uid][i][j]->coordinateX();
			double lat = allGridsMaps[uid][i][j]->coordinateY();
			double index_x = allGridsMaps[uid][i][j]->indexX();
			double index_y = allGridsMaps[uid][i][j]->indexY();
			double is_ibstacle = 0;
			if (allGridsMaps[uid][i][j]->isObstacle()) is_ibstacle = 1;
			double tarray[] = { lon, lat , index_x , index_y , is_ibstacle };
			std::vector<double> tmpNode(tarray, tarray + sizeof(tarray) / sizeof(tarray[0]));
			tmp.push_back(tmpNode);
		}
		gridMap.push_back(tmp);
	}

	AStar astar;
	astar.ObstacleUAVs = allObstacleArea[uid];
	astar.GridMap = gridMap;
	astar.setFlyParam(fly_param[0], fly_param[1]);
	std::vector<casic_vector3d>posVec;
	astar.init(length_, width_, alt_, safeDis, start, goal);
	astar.pathPlan(posVec);

	fly_param.push_back(astar.getFlyTime());
	fly_param.push_back(astar.getGoalisFound() ? 1 : 0);

	for (auto pos_itor = posVec.rbegin(); pos_itor != posVec.rend(); ++pos_itor) {
		allObstacleArea[uid].push_back(*pos_itor);
		stagePointPlanMap.push_back(*pos_itor);
	}
	if (std::find(posVec.begin(), posVec.end(), goal) == posVec.end()) {
		allObstacleArea[uid].push_back(goal);
		stagePointPlanMap.push_back(goal);
	}
}

void AreaSearchAlgorithm::PolygonAreaSerachPlan(casic_vector3d currPos, double Height, double gridWidth, double speed, double angularSpeed, double upSpeed, double flyTime, double coverRate, double safeDis, int areaType, int areaPointsNum, std::map<int, std::vector<casic_vector3d>> &PointsVec, int planWay, std::map<int, std::vector<casic_vector3d>> &startReturnPointMap)
{

	searchArea = qnzkna::ZoneManage::CZoneBase();
	ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(tr2("搜索区"), searchArea);
	if (searchArea.nZoneType == 4)
	{
		qnzkna::ZoneManage::ZonePointVec pointlist;
		ICollaborativeSearchActivator::Get().getPIZoneManageService()->TransformRectangleToPoints(tr2("搜索区"), pointlist);
		searchArea.stZonePointVec = pointlist;
	}

	takeOff_land_Area= qnzkna::ZoneManage::CZoneBase();
	ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(tr2("起降区"), takeOff_land_Area);
	if (takeOff_land_Area.nZoneType == 4)
	{
		qnzkna::ZoneManage::ZonePointVec pointlist;
		ICollaborativeSearchActivator::Get().getPIZoneManageService()->TransformRectangleToPoints(tr2("起降区"), pointlist);
		takeOff_land_Area.stZonePointVec = pointlist;
	}

	QStringList lineZoneList;

	ICollaborativeSearchActivator::Get().getPIZoneManageService()->FindLineByZones(tr2("起降区"), tr2("搜索区"), lineZoneList);

	std::vector<qnzkna::ZoneManage::CZoneBase> lineZoneVec;
	qnzkna::ZoneManage::CZoneBase minLineZone;
	qnzkna::ZoneManage::CZoneBase returnLineZone;
	for (auto zone : lineZoneList)
	{
		qnzkna::ZoneManage::CZoneBase tempZone;
		ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(zone, tempZone);
		lineZoneVec.push_back(tempZone);
		if (minLineZone.nPointCount == 0)
		{
			minLineZone = tempZone;
		}
		else if (tempZone.stZonePointVec.size() < minLineZone.stZonePointVec.size())
		{
			minLineZone = tempZone;
		}
	}

	for (auto zonelt : lineZoneVec)
	{
		if (tr2(zonelt.strID) == tr2(minLineZone.strID))
		{
			continue;
		}
		returnLineZone = zonelt;
	}

	double direction = 0;
	double k, b = 0;
	int maxDis = 0;
	std::vector<QPointF>ZonePoints;
	if (searchArea.stZonePointVec.empty())
	{
		return;
	}
	for (int i = 0; i < searchArea.stZonePointVec.size() - 1; i++)
	{
		QGeoCoordinate start(searchArea.stZonePointVec[i].dLat, searchArea.stZonePointVec[i].dLon);
		QGeoCoordinate next(searchArea.stZonePointVec[i+1].dLat, searchArea.stZonePointVec[i+1].dLon);
		ZonePoints.push_back(QPointF(start.longitude(),start.latitude()));
		if (i >= searchArea.stZonePointVec.size() - 2)
		{
			ZonePoints.push_back(QPointF(next.longitude(), next.latitude()));
		}
		double currdist = start.distanceTo(next);
		double ag = start.azimuthTo(next);
		if (maxDis < currdist)
		{
			maxDis = currdist;
			if (abs(start.longitude() - next.longitude()) < 0.000001)
			{
				b = start.longitude();
			}
			else
			{
				k = (start.latitude() - next.latitude()) / (start.longitude() - next.longitude());
				b = start.latitude() - k * start.longitude();
			}
			direction = start.azimuthTo(next);
		}
	}

	QPointF takeoff_pos(takeOff_land_Area.stZonePointVec[0].dLon, takeOff_land_Area.stZonePointVec[0].dLat);
	QPointF minPos;
	int pdis = INT_MAX;
	for (auto tpos : searchArea.stZonePointVec)
	{
		QGeoCoordinate p1(tpos.dLat, tpos.dLon);
		QGeoCoordinate p2(takeoff_pos.y(), takeoff_pos.x());
		if (p1.distanceTo(p2) < pdis)
		{
			pdis = p1.distanceTo(p2);
			minPos = QPointF(tpos.dLon, tpos.dLat);
		}
	}

	pdis = INT_MAX;
	QPointF stagingPos;
	std::vector<casic_vector3d> firstpointList;

	for (auto point : minLineZone.stZonePointVec)
	{
		QGeoCoordinate p1(point.dLat, point.dLon);
		QGeoCoordinate p0(minPos.y(), minPos.x());
		QPointF pf(point.dLon, point.dLat);
		double nd = p0.distanceTo(p1);
		if (p0.distanceTo(p1) < pdis && !is_point_in_zone(pf, ZonePoints))
		{
			pdis = p0.distanceTo(p1);
			stagingPos = pf;
			firstpointList.push_back(casic_vector3d(pf.x(), pf.y(),0));
		}
	}

	std::vector<std::vector<shared_ptr<PlanNode>>> m_roadmap;
	std::vector<std::vector<casic_vector2d>> m_mapPoints;
	_coverAreaVec.clear();
	auto noflyZonePointVec = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->NoFlyZonePoints;
	auto obstacleArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->obstacleArea;
	stagingPoint = casic_vector3d(0.0, 0.0, 0.0);
	PointsVec.clear();
	_safeDistance = safeDis;

	QGeoCoordinate geoPoint0(minPos.y(), minPos.x());
	QGeoCoordinate geoPoint1 = geoPoint0.atDistanceAndAzimuth(2 * maxDis, direction - 180);
	QGeoCoordinate geoPoint2 = geoPoint0.atDistanceAndAzimuth(2 * maxDis, direction);
	QGeoCoordinate GeoTakeOffPos(takeoff_pos.y(), takeoff_pos.x());

	casic_vector3d linePoint1(geoPoint1.longitude(), geoPoint1.latitude(),0);
	casic_vector3d linePoint2(geoPoint2.longitude(), geoPoint2.latitude(), 0);

	double referDirection = GeoTakeOffPos.azimuthTo(geoPoint0);
	double moveDirection = referDirection - abs(direction - 90) < referDirection - abs(direction + 90) ? (direction - 90) : (direction + 90);

	QPointF farPos;
	int fDis = 0;
	for (auto tpos : searchArea.stZonePointVec)
	{
		casic_vector3d pos(tpos.dLon, tpos.dLat,0);
		double currDis = calDisP2Line(linePoint1, linePoint2, pos);
		if (currDis > fDis)
		{
			fDis = currDis;
			farPos = QPointF(tpos.dLon, tpos.dLat);
		}
	}

	QGeoCoordinate GeoFarPos(farPos.y(), farPos.x());
	casic_vector3d far_csc(farPos.x(), farPos.y(), 0);
	casic_vector3d min_csc(minPos.x(), minPos.y(), 0);
	double max_far2Line = calDisP2Line(linePoint1, linePoint2, far_csc);
	std::vector<casic_vector2d>ppp;

	int index_i = 0;
	int line_count = 0;
	while (calDisP2Line(linePoint1, linePoint2, far_csc) >= gridWidth && max_far2Line >= calDisP2Line(linePoint1, linePoint2, min_csc))
	{
		double dmos = calDisP2Line(linePoint1, linePoint2, far_csc);
		double dins = calDisP2Line(linePoint1, linePoint2, min_csc);
		double ang = geoPoint0.azimuthTo(GeoFarPos);
		std::vector<QPointF>IntersectPointList;
		std::vector<shared_ptr<AreaSearchAlgorithm::PlanNode>> pointsMin1toMin;
		if (index_i == 0)
		{

			geoPoint0 = geoPoint0.atDistanceAndAzimuth(gridWidth / 2, moveDirection);
			geoPoint1 = geoPoint0.atDistanceAndAzimuth(5*maxDis, direction - 180);
			geoPoint2 = geoPoint0.atDistanceAndAzimuth(5*maxDis, direction);
			linePoint1 = casic_vector3d(geoPoint1.longitude(), geoPoint1.latitude(), 0);
			linePoint2 = casic_vector3d(geoPoint2.longitude(), geoPoint2.latitude(), 0);

			double lon = geoPoint0.longitude();
			double lat = geoPoint0.latitude();
			if (2 == num_lineIntersectZone(casic_vector3d(geoPoint1.longitude(), geoPoint1.latitude(), 0), casic_vector3d(geoPoint2.longitude(), geoPoint2.latitude(), 0), ZonePoints, IntersectPointList))
			{
				QGeoCoordinate pm1(IntersectPointList[0].y(), IntersectPointList[0].x());
				QGeoCoordinate pm2(IntersectPointList[1].y(), IntersectPointList[1].x());
				double dis = pm1.distanceTo(pm2);
				double azmuth = pm1.azimuthTo(pm2);
				if (dis > gridWidth)
				{
					line_count = dis / gridWidth;
					QGeoCoordinate gridCenterPoint;
					for (int index_j = 0; index_j < line_count; index_j++)
					{
						if (index_j == 0)
						{
							gridCenterPoint = pm1.atDistanceAndAzimuth(gridWidth / 2, azmuth);
							auto m_node = std::make_shared<AreaSearchAlgorithm::PlanNode>(gridCenterPoint.longitude(), gridCenterPoint.latitude(), index_i, index_j);
							pointsMin1toMin.push_back(m_node);
							ppp.push_back(casic_vector2d(gridCenterPoint.longitude(), gridCenterPoint.latitude()));
						}
						else
						{
							gridCenterPoint = gridCenterPoint.atDistanceAndAzimuth(gridWidth, azmuth);
							auto m_node = std::make_shared<AreaSearchAlgorithm::PlanNode>(gridCenterPoint.longitude(), gridCenterPoint.latitude(), index_i, index_j);
							pointsMin1toMin.push_back(m_node);
							ppp.push_back(casic_vector2d(gridCenterPoint.longitude(), gridCenterPoint.latitude()));
						}
						index_i++;
					}
				}
			}
			else
			{
				continue;
			}
		}
		else
		{
			geoPoint0 = geoPoint0.atDistanceAndAzimuth(gridWidth, moveDirection);
			geoPoint1 = geoPoint0.atDistanceAndAzimuth(2 * maxDis, direction - 180);
			geoPoint2 = geoPoint0.atDistanceAndAzimuth(2 * maxDis, direction);
			linePoint1 = casic_vector3d(geoPoint1.longitude(), geoPoint1.latitude(), 0);
			linePoint2 = casic_vector3d(geoPoint2.longitude(), geoPoint2.latitude(), 0);
			if (2 == num_lineIntersectZone(casic_vector3d(geoPoint1.longitude(), geoPoint1.latitude(), 0), casic_vector3d(geoPoint2.longitude(), geoPoint2.latitude(), 0), ZonePoints, IntersectPointList))
			{
				QGeoCoordinate pm1(IntersectPointList[0].y(), IntersectPointList[0].x());
				QGeoCoordinate pm2(IntersectPointList[1].y(), IntersectPointList[1].x());
				double dis = pm1.distanceTo(pm2);
				double azmuth = pm1.azimuthTo(pm2);
				if (dis > gridWidth)
				{

					QGeoCoordinate gridCenterPoint;
					for (int index_j = 0; index_j < line_count; index_j++)
					{
						if (index_j == 0)
						{
							gridCenterPoint = pm1.atDistanceAndAzimuth(gridWidth / 2, azmuth);
							auto m_node = std::make_shared<AreaSearchAlgorithm::PlanNode>(gridCenterPoint.longitude(), gridCenterPoint.latitude(), index_i, index_j);
							pointsMin1toMin.push_back(m_node);
							ppp.push_back(casic_vector2d(gridCenterPoint.longitude(), gridCenterPoint.latitude()));
						}
						else
						{
							gridCenterPoint = gridCenterPoint.atDistanceAndAzimuth(gridWidth, azmuth);
							auto m_node = std::make_shared<AreaSearchAlgorithm::PlanNode>(gridCenterPoint.longitude(), gridCenterPoint.latitude(), index_i, index_j);
							pointsMin1toMin.push_back(m_node);
							ppp.push_back(casic_vector2d(gridCenterPoint.longitude(), gridCenterPoint.latitude()));
						}
					}
					index_i++;
				}
			}

		}
		if (!pointsMin1toMin.empty())
		{
			m_roadmap.push_back(pointsMin1toMin);
			m_mapPoints.push_back(ppp);
		}
	}
	m_PolygonGridMap = m_roadmap;
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->obstacleArea = ppp;

	if (m_roadmap.size() == 0 || m_roadmap[0].size() == 0)
	{
		return;
	}

	double step_dis = gridWidth;
	int lonNum = m_roadmap.size();
	int latNum = m_roadmap[0].size();
	PlanNode m_startNode = PlanNode(m_roadmap[0][0]->coordinateX(), m_roadmap[0][0]->coordinateY(), 0, 0);
	if (m_roadmap.empty())
	{
		return;
	}

	std::vector<casic_vector3d>ObstacleUAVs;
	std::vector<casic_vector3d>_visitedPoints;
	std::vector<std::vector<shared_ptr<PlanNode>>>_planNodes;
	std::vector<casic_vector3d>_onePlanevisitedPoints;
	casic_vector3d pos;

	std::vector<PlanNode> motion = {
		PlanNode(0, 0,  1,  0,  1, 0),
		PlanNode(0, 0,  0,  1,  1, 0),
		PlanNode(0, 0,  1, -1,  std::sqrt(2),0),
		PlanNode(0, 0,  1,  1,  std::sqrt(2),0),
		PlanNode(0, 0, -1,  0,  1, 0),
		PlanNode(0, 0,  0, -1,  1, 0),
		PlanNode(0, 0, -1,  1,  std::sqrt(2),0),
		PlanNode(0, 0, -1, -1,  std::sqrt(2),0)
	};

	for (int i = 0; i < m_roadmap.size(); i++)
	{
		for (int j = 0; j < m_roadmap[i].size(); j++)
		{
			auto tmp = m_roadmap[i][j];
			QPointF pointf(tmp->coordinateX(), tmp->coordinateY());
			casic_vector3d tmmPos(tmp->coordinateX(), tmp->coordinateY(), Height);
			if (tmp->getAreaAttribute() == areaType)
			{
				if (tmp->isObstacle() || is_point_in_zones(pointf, noflyZonePointVec))
				{
					_visitedPoints.push_back(tmmPos);
				}
			}
		}
	}

	int Planindex = 1;
	while (_visitedPoints.size() < areaPointsNum*coverRate)
	{

		auto cmp = [](shared_ptr<PlanNode> left, shared_ptr<PlanNode> right) {

			return left->getTotalCost() > right->getTotalCost();
		};

		std::priority_queue<shared_ptr<PlanNode>,
			std::vector<shared_ptr<PlanNode>>, decltype(cmp)> O(cmp);
		std::stack<shared_ptr<PlanNode>>O1;

		double flytime = 0;

		if (!ObstacleUAVs.empty())
		{
			vector<shared_ptr<PlanNode>> tmp;
			for (auto pos : ObstacleUAVs)
			{
				for (int i = 0; i < lonNum; ++i) {
					vector<shared_ptr<PlanNode>> tmp;
					for (int j = 0; j < latNum; ++j) {
						double lon = m_roadmap[i][j]->coordinateX();
						double lat = m_roadmap[i][j]->coordinateY();
						if (lon == (double)pos.x && lat == (double)pos.y)
						{
							m_roadmap[i][j]->setVisited();
						}
					}
				}
			}
		}

		int startIndex = 0;
		int startjIndex = 0;

		shared_ptr<PlanNode> tmp = m_roadmap[startIndex][startjIndex];
		bool isReturn = false;
		while (startjIndex < m_roadmap[0].size())
		{
			startIndex = 0;
			while (startIndex < m_roadmap.size())
			{
				tmp = m_roadmap[startIndex][startjIndex];
				if (!tmp->isVisited() && !tmp->isObstacle() && tmp->getAreaAttribute() == areaType)
				{
					isReturn = true;
					break;
				}
				startIndex++;
			}
			if (isReturn)
			{
				break;
			}
			startjIndex++;
		}

		int roadIndex = Planindex;
		O.push(tmp);
		O1.push(tmp);

		std::vector<casic_vector3d> pointList;
		std::vector<casic_vector3d> startReturnPointVec;
		std::vector<casic_vector3d> stagingPointList;
		std::vector<casic_vector3d> takeoffPointList;
		casic_vector3d m_collPos = casic_vector3d(stagingPos.x(), stagingPos.y(), 0);

		casic_vector3d m_takeoffPos = casic_vector3d(takeoff_pos.x(), takeoff_pos.y(), 0);

		std::vector<double> fly_param;
		fly_param.push_back(speed);
		fly_param.push_back(angularSpeed);

		stagingPointList = pointList;
		takeoffPointList = pointList;

		int stagingPathCount = pointList.size();

		PlanNode m_takeoffNode = PlanNode(m_takeoffPos.x, m_takeoffPos.y, 0, 0, 0, 0);
		casic_vector3d tmpPo = casic_vector3d(tmp->coordinateX(), tmp->coordinateY(), Height);

		if (is_lineToAllaZones(currPos, tmpPo, noflyZonePointVec))
		{
			O1.pop();
			int Xindex = tmp->indexX();
			int Yindex = tmp->indexY();
			shared_ptr<PlanNode> tmp1 = m_roadmap[0][0];
			bool isFindPoint = false;
			for (int i = 0, Iindex = Yindex; i < m_roadmap[Xindex].size(); i++)
			{
				for (int j = 0, Jindex = (Xindex + 1) % m_roadmap.size(); j < m_roadmap.size(); j++)
				{
					if (!(m_roadmap[Jindex][Iindex]->isVisited()) && !(m_roadmap[Jindex][Iindex]->isObstacle()) && m_roadmap[Jindex][Iindex]->getAreaAttribute() == areaType)
					{
						tmp1 = m_roadmap[Jindex][Iindex];
						tmpPo = casic_vector3d(tmp1->coordinateX(), tmp1->coordinateY(), Height);
						if (!is_lineToAllaZones(currPos, tmpPo, noflyZonePointVec))
						{
							i = m_roadmap[0].size();
							isFindPoint = true;
							break;
						}
					}
					Jindex = (Jindex + 1) % m_roadmap.size();
				}
				Iindex = (Iindex + 1) % m_roadmap[Xindex].size();
			}
			if (isFindPoint)
			{
				O1.push(tmp1);
			}
		}
		if (O1.empty())
		{

			double coverRate = _visitedPoints.size();
			double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < flytime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = flytime;
			}
			return;
		}
		tmp = O1.top();
		if (tmp->isVisited())
		{
			double coverRate = _visitedPoints.size();
			double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < flytime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = flytime;
			}
			return;
		}

		pointList = firstpointList;

		for (auto ps : minLineZone.stZonePointVec)
		{
			pointList.push_back(casic_vector3d(ps.dLon,ps.dLat,0));
		}

		stagingPointList = pointList;
		auto miter = pointList.rbegin();
		if (miter->x != tmpPo.x || miter->y != tmpPo.y)
		{
			pointList.push_back(tmpPo);
		}

		std::vector<casic_vector3d>edpoints = getEndPoints(tmpPo, gridWidth);
		std::pair<casic_vector3d, std::vector<casic_vector3d>>_pair = std::make_pair(pos, edpoints);
		_coverAreaVec.push_back(edpoints);
		if (Planindex == 0)
		{
			_coverAreaVec.push_back(edpoints);
		}

		QGeoCoordinate pos0(currPos.y, currPos.x);
		QGeoCoordinate pos1(m_takeoffNode.coordinateY(), m_takeoffNode.coordinateX());
		QGeoCoordinate pos2(tmp->coordinateY(), tmp->coordinateX());

		flytime += 2 * fly_param[2];

		std::vector<shared_ptr<PlanNode>>_onePoints;
		PlanNode nlast;
		double AngularPass = 0;

		QGeoCoordinate lastPoint = pos1;

		shared_ptr<PlanNode>lastPlanNode = m_roadmap[m_startNode.indexX()][m_startNode.indexY()];
		shared_ptr<PlanNode>minNode = m_roadmap[m_startNode.indexX()][m_startNode.indexY()];

		casic_vector3d startPos = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->curpos;
		QGeoCoordinate StartPosGeo(startPos.y, startPos.x);
		int calCount = 0;
		double returnFlyTime = 0;

		queue<int> premotion12;

		while (flytime <= flyTime  && _onePlanevisitedPoints.size() <= areaPointsNum* coverRate)
		{
			if (O1.empty())
			{
				shared_ptr<PlanNode> tmp1 = m_roadmap[0][0];
				for (int i = 0; i < m_roadmap.size(); i++)
				{
					for (int j = 0; j < m_roadmap[i].size(); j++)
					{
						if (m_roadmap[i][j]->isVisited() == false)
						{
							tmp1 = m_roadmap[i][j];
							i = m_roadmap.size();
							break;
						}
					}
				}

				O1.push(tmp1);
				roadIndex++;
			}

			shared_ptr<PlanNode> nBest = O1.top();

			if (_onePoints.size() == 0)
			{

				nlast = m_takeoffNode;
			}
			QGeoCoordinate nBestQGeo = QGeoCoordinate(nBest->coordinateY(), nBest->coordinateX());

			casic_vector3d posnBest = casic_vector3d(nBest->coordinateX(), nBest->coordinateY(), Height);
			O1.pop();

			nBest->setVisited();

			_onePlanevisitedPoints.push_back(casic_vector3d(nBest->coordinateX(), nBest->coordinateY(), Height));
			int index = 0;
			std::map<int, shared_ptr<PlanNode>> minPoint_map;
			std::map<int, int> motion_map;
			minPoint_map.clear();
			bool flag = false;
			for (int i = 0; i < motion.size(); i++)
			{
				flag = false;

				index++;

				int curi = i;

				if (planWay == 0 && premotion12.size() == 2) {
					int pre1 = premotion12.front();
					premotion12.pop();
					int pre2 = premotion12.front();
					if (pre1 != pre2) {
						i = (pre1 + 4) % 8;
					}
					flag = true;
				}

				if (planWay == 1) {
					flag = false;
				}

				PlanNode tmpNode = motion[i];
				nBest->setG(0.0);
				tmpNode += *nBest;
				int xxx = tmpNode.indexX();
				int yyy = tmpNode.indexY();
				if (tmpNode.indexX() < 0 || tmpNode.indexY() < 0 || tmpNode.indexX() >= lonNum || tmpNode.indexY() >= latNum)
				{
					if (flag) {
						i = curi - 1;
					}
					continue;
				}

				shared_ptr<PlanNode> tmpNodePointer =
					m_roadmap[tmpNode.indexX()][tmpNode.indexY()];

				if (tmpNodePointer->getAreaAttribute() != areaType)
				{
					if (flag) {
						i = curi - 1;
					}
					continue;
				}

				casic_vector3d tmpNodePos = casic_vector3d(tmpNodePointer->coordinateX(), tmpNodePointer->coordinateY(), Height);
				if (is_lineToAllaZones(posnBest, tmpNodePos, noflyZonePointVec))
				{
					if (std::find(_visitedPoints.begin(), _visitedPoints.end(), tmpNodePos) == _visitedPoints.end())
					{
						_visitedPoints.push_back(tmpNodePos);
					}
					if (flag) {
						i = curi - 1;
					}
					continue;
				}

				if (tmpNodePointer->isObstacle() || tmpNodePointer->isVisited())
				{
					if (flag) {
						i = curi - 1;
					}
					continue;
				}

				if (isInterObstacle(posnBest, tmpNodePos, obstacleArea, gridWidth))
				{
					if (flag) {
						i = curi - 1;
					}
					continue;
				}

				QGeoCoordinate tmpPoint(tmpNodePointer->coordinateY(), tmpNodePointer->coordinateX());
				double lastAngle = lastPoint.azimuthTo(nBestQGeo);
				double nowAngle = nBestQGeo.azimuthTo(tmpPoint);

				double disAngle = fabs(nowAngle - lastAngle);

				if (disAngle > 180)
				{
					double maxAngle = max(lastAngle, nowAngle);
					double minAngle = min(lastAngle, nowAngle);
					minAngle += 360.0;
					disAngle = fabs(minAngle - maxAngle);
				}

				double Gcost = tmpNode.getGCost() + disAngle / 180.0;
				tmpNodePointer->setG(Gcost);

				int hcost = m_roadmap[tmpNode.indexX()][tmpNode.indexY()]->getHCost();
				Gcost -= hcost;
				minPoint_map[(int)(Gcost * 100)] = tmpNodePointer;
				motion_map[(int)(Gcost * 100)] = i;
			}
			lastPoint = nBestQGeo;

			if (minPoint_map.size() == 0)
			{
				QPoint nowPointXY(nBest->indexX(), nBest->indexY());

				std::vector<QPoint> nextPoints = getNextPointFromImpasse(nowPointXY, m_roadmap);

				if (nextPoints.size() == 0)
				{
					break;
				}

				std::vector<QPoint> nextPathPoints;
				nextPathPoints.clear();

				QPoint nextPointXY;
				shared_ptr<PlanNode> nextPlanNode = nullptr;
				QGeoCoordinate nextPointGeo;
				casic_vector3d nextPointLLA;
				bool connected = false;
				for (int nextIndex = 0; nextIndex < nextPoints.size(); nextIndex++)
				{
					nextPointXY = nextPoints[nextIndex];
					nextPlanNode = m_roadmap[nextPointXY.x()][nextPointXY.y()];
					nextPointGeo = QGeoCoordinate(nextPlanNode->coordinateY(), nextPlanNode->coordinateX());
					nextPointLLA = casic_vector3d(nextPlanNode->coordinateX(), nextPlanNode->coordinateY(), Height);

					bool isInterset = is_lineToAllaZones(posnBest, nextPointLLA, noflyZonePointVec);
					bool isIntersetObs = isInterObstacle(posnBest, nextPointLLA, obstacleArea);
					if (isInterset == false && isIntersetObs == false)
					{
						nextPlanNode->setBackpoint(nBest);
						double dis = nextPointGeo.distanceTo(nBestQGeo);
						flytime += 2 * (dis / speed);
						pointList.push_back(nextPointLLA);
						_visitedPoints.push_back(nextPointLLA);
						nextPlanNode->setVisited();
						O1.push(nextPlanNode);
						connected = true;
						break;
					}
				}
				if (connected == true)
				{
					continue;
				}
				else
				{
					for (int nextIndex = 0; nextIndex < nextPoints.size(); nextIndex++)
					{

						nextPointXY = nextPoints[nextIndex];
						nextPlanNode = m_roadmap[nextPointXY.x()][nextPointXY.y()];
						nextPointGeo = QGeoCoordinate(nextPlanNode->coordinateY(), nextPlanNode->coordinateX());
						nextPointLLA = casic_vector3d(nextPlanNode->coordinateX(), nextPlanNode->coordinateY(), Height);

						std::map<shared_ptr<PlanNode>, int> V2PlanNode;
						V2PlanNode.clear();
						std::vector<std::vector<int>> graph;
						graph.clear();

						createMGraph(m_roadmap, areaType, V2PlanNode, graph);
						MGraph* mGraph = new MGraph;
						auto iBeginItor = V2PlanNode.find(nBest);
						auto iEndItor = V2PlanNode.find(nextPlanNode);
						int iBegin;
						int iEnd;
						if (iBeginItor != V2PlanNode.end() && iEndItor != V2PlanNode.end())
						{
							iBegin = iBeginItor->second;
							iEnd = iEndItor->second;
						}
						else
						{
							break;
						}
						std::vector<int> stackVertices = mGraph->getShortPath(graph, iBegin, iEnd);

						int stackSize = stackVertices.size();
						if (stackSize <= 0)
						{
							continue;
						}

						casic_vector3d lastPoint = posnBest;
						bool isConnect = true;
						for (int index = stackSize - 1; index >= 0; index--)
						{
							int k = stackVertices[index];
							auto V2planNodeItor = V2PlanNode.begin();
							while (V2planNodeItor->second != k)
							{
								V2planNodeItor++;
							}
							auto planNodeItor = V2planNodeItor->first;
							casic_vector3d nextPoint(planNodeItor->coordinateX(), planNodeItor->coordinateY(), Height);
							bool isInterNoFly = is_lineToAllaZones(lastPoint, nextPoint, noflyZonePointVec);
							bool isInterOb = isInterObstacle(lastPoint, nextPoint, obstacleArea);
							if (isInterNoFly == false && isInterOb == false)
							{
								lastPoint = nextPoint;
								nextPathPoints.push_back(QPoint(planNodeItor->indexX(), planNodeItor->indexY()));
								lastPoint = nextPoint;
							}
							else
							{
								isConnect = false;
								nextPathPoints.clear();
								break;
							}
						}
						if (isConnect == false)
						{
							continue;
						}
						else
						{
							break;
						}
					}
				}

				if (nextPathPoints.size() > 0)
				{
					auto lastNode = nBest;
					for (int i = 0; i < nextPathPoints.size(); i++)
					{
						QPoint nextPathPoint = nextPathPoints[i];
						auto nextNode = Gridsmap[nextPathPoint.x()][nextPathPoint.y()];
						casic_vector3d nextPlanLLA(nextNode->coordinateX(), nextNode->coordinateY(), 0);
						nextNode->setBackpoint(lastPlanNode);
						nextNode->setVisited();
						lastNode = nextNode;
						pointList.push_back(nextPlanLLA);
					}
					nextPlanNode->setBackpoint(lastNode);
					O1.push(nextPlanNode);
					pointList.push_back(nextPointLLA);
					nextPlanNode->setVisited();
					continue;
				}
				else if (connected == true)
				{
					continue;
				}
				else
				{
					break;
				}
			}

			auto nextPointer = minPoint_map.begin()->second;
			int curmotion = motion_map[minPoint_map.begin()->first];
			premotion12.push(curmotion);

			nextPointer->setBackpoint(nBest);
			flytime += 2 * (nextPointer->getGCost() * step_dis / speed);

			casic_vector3d nextPos(nextPointer->coordinateX(), nextPointer->coordinateY(), Height);
			QGeoCoordinate nextPosGeo(nextPos.y, nextPos.x);
			pointList.push_back(nextPos);
			_visitedPoints.push_back(nextPos);

			nextPointer->setVisited();
			O1.push(nextPointer);
			lastPlanNode = nextPointer;
			double returnDis = nextPosGeo.distanceTo(StartPosGeo);
			returnFlyTime = returnDis / speed;
			calCount++;
		}

		startReturnPointVec.push_back(pointList[pointList.size() - 1]);

		double totalTime = flytime;
		int way = 1;

		if (way == 1) {
			std::vector<casic_vector3d> searchPointList = pointList;
			QGeoCoordinate leftup_x(lastPlanNode->coordinateY(), lastPlanNode->coordinateX());
			QGeoCoordinate rightup(lastPlanNode->coordinateY(), currPos.x);
			QGeoCoordinate rightdown(currPos.y, currPos.x);
			QGeoCoordinate leftdown(currPos.y, lastPlanNode->coordinateX());
			double mapLength = leftup_x.distanceTo(rightup);
			double mapWidth = leftup_x.distanceTo(leftdown);
			double angleLehgth = leftup_x.azimuthTo(rightup);
			double angleWidth = leftup_x.azimuthTo(leftdown);
			if (fabs(angleWidth - 0) < 1 && fabs(angleLehgth - 270) < 1)
				angleWidth += 360.0;
			if (fabs(angleLehgth - 0) < 1 && fabs(angleWidth - 270) < 1)
				angleLehgth += 360.0;
			double angleLine = (angleLehgth + angleWidth) / 2;
			double temp[] = { mapLength, mapWidth , angleLehgth , angleWidth , angleLine };
			std::vector<double> gridMapParams(temp, temp + sizeof(temp) / sizeof(temp[0]));
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(currPos.x, currPos.y);
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			double m_alt = elevationTemp;

			int height_x = Height + Planindex * safeDis * 2;

			addNoflyToObstacleArea(Planindex, height_x, noflyZonePointVec);

		}
		if (way == 2) {

			tmp = lastPlanNode;
			int count = 0;
			std::vector<casic_vector3d>tmpsearchList;
			casic_vector3d firstPoint = casic_vector3d(0, 0, 0);

			pointList.push_back(*pointList.rbegin());
			auto itor = pointList.rbegin();
			itor->z = Height + (Planindex + 1) * 10;

			casic_vector3d sPoint(m_takeoffPos.x, m_takeoffPos.y, Height + (Planindex + 1) * 10);
			pointList.push_back(sPoint);

			pointList.push_back(casic_vector3d(m_takeoffPos.x, m_takeoffPos.y, Height));

			int co = pointList.size();
			int reCount = co - stagingPathCount;
			for (co = co - 2; co >= stagingPathCount; co--)
			{
				pos = pointList[co];

				tmpsearchList.emplace_back(pos);

				if (std::find(stagingPointList.begin(), stagingPointList.end(), pos) == stagingPointList.end())
				{
					_visitedPoints.push_back(pos);
				}
				ObstacleUAVs.emplace_back(pos);
				count++;
			}

			_visitedPoints.push_back(*stagingPointList.rbegin());

			for (auto pos : tmpsearchList)
			{
				std::vector<casic_vector3d>edpoints = getEndPoints(pos, gridWidth);
				std::pair<casic_vector3d, std::vector<casic_vector3d>>_pair = std::make_pair(pos, edpoints);
				_coverAreaVec.push_back(edpoints);
			}

			pointList.push_back(*pointList.rbegin());
			auto imtor = pointList.rbegin();
			short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight(imtor->x, imtor->y);
			if (elevationTemp < -10000)
			{
				elevationTemp = 0;
			}
			imtor->z = elevationTemp;
		}

		for (auto pt = returnLineZone.stZonePointVec.rbegin(); pt != returnLineZone.stZonePointVec.rend(); pt++)
		{
			pointList.push_back(casic_vector3d(pt->dLon, pt->dLat,0));
		}
		std::pair<int, std::vector<casic_vector3d>>mpair = std::make_pair(Planindex + 1, pointList);
		PointsVec.insert(mpair);
		std::pair<int, std::vector<casic_vector3d>>mpair2 = std::make_pair(Planindex + 1, startReturnPointVec);
		startReturnPointMap.insert(mpair2);
		Planindex++;
		if (_onePlanevisitedPoints.size() >= areaPointsNum
			|| _visitedPoints.size() > areaPointsNum)
		{
			double coverRate = _visitedPoints.size();
			double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < totalTime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = totalTime;
			}
			return;
		}
		if (Planindex > 1)
		{
			double coverRate = _visitedPoints.size();
			double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
			LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate;
			double maxflyTime = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime;
			if (maxflyTime < totalTime)
			{
				LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->remainFlyTime = totalTime;
			}
			return;
		}
	}
	double coverRate1 = _visitedPoints.size();
	double allRate = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate;
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->realCoverRate = allRate + coverRate1;

	sparsePath(PointsVec, startReturnPointMap);

}

void AreaSearchAlgorithm::PolygonAreaSerachPlan1(casic_vector3d currPos, double Height, double gridWidth, double speed, double angularSpeed, double upSpeed, double flyTime, double coverRate, double safeDis, int areaType, int areaPointsNum, std::map<int, std::vector<casic_vector3d>> &PointsVec, int planWay, std::map<int, std::vector<casic_vector3d>> &startReturnPointMap)
{
	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_takeoffArea.isEmpty() ||
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_workArea.isEmpty())
	{
		LyMessageBox::warning(nullptr, tr2("警告"), tr2("未成功获取到起降区或作业区信息"), QMessageBox::Ok);
	}

	searchArea = qnzkna::ZoneManage::CZoneBase();
	QString strWorkArea;
	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_workArea.isEmpty() == false)
	{
		strWorkArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_workArea;
	}
	else
	{
		strWorkArea = tr2("搜索区");
	}
	QString strTakeOffArea;
	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_takeoffArea.isEmpty() == false)
	{
		strTakeOffArea = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_takeoffArea;
	}
	else
	{
		strTakeOffArea = tr2("起降区");
	}

	ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(strWorkArea, searchArea);
	if (searchArea.nZoneType == 4)
	{
		qnzkna::ZoneManage::ZonePointVec pointlist;

		ICollaborativeSearchActivator::Get().getPIZoneManageService()->TransformRectangleToPoints(strWorkArea, pointlist);
		searchArea.stZonePointVec = pointlist;
	}

	takeOff_land_Area = qnzkna::ZoneManage::CZoneBase();

	ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(strTakeOffArea, takeOff_land_Area);
	if (takeOff_land_Area.nZoneType == 4)
	{
		qnzkna::ZoneManage::ZonePointVec pointlist;
		ICollaborativeSearchActivator::Get().getPIZoneManageService()->TransformRectangleToPoints(strTakeOffArea, pointlist);
		takeOff_land_Area.stZonePointVec = pointlist;
	}

	QStringList lineZoneList;

	//ICollaborativeSearchActivator::Get().getPIZoneManageService()->FindLineByZones(tr2("起降区"), tr2("搜索区"), lineZoneList);
	ICollaborativeSearchActivator::Get().getPIZoneManageService()->FindLineByZones(takeOff_land_Area.strID, tr2("搜索区"), lineZoneList);

	std::vector<qnzkna::ZoneManage::CZoneBase> lineZoneVec;
	qnzkna::ZoneManage::CZoneBase minLineZone;
	qnzkna::ZoneManage::CZoneBase returnLineZone;
	for (auto zone : lineZoneList)
	{
		qnzkna::ZoneManage::CZoneBase tempZone;
		ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(zone, tempZone);
		lineZoneVec.push_back(tempZone);
		if (minLineZone.nPointCount == 0)
		{
			minLineZone = tempZone;
		}
		else if (tempZone.stZonePointVec.size() < minLineZone.stZonePointVec.size())
		{
			minLineZone = tempZone;
		}
	}

	for (auto zonelt : lineZoneVec)
	{
		if (tr2(zonelt.strID) == tr2(minLineZone.strID))
		{
			continue;
		}
		returnLineZone = zonelt;
	}

	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_enterLine.isEmpty() == false)
	{
		ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(LY_CollaborativeSearchService::
			Get().m_RoutePlanningDialog->m_enterLine, minLineZone);
	}

	if (LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_outLine.isEmpty() == false)
	{
		ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetZoneBase(LY_CollaborativeSearchService::
			Get().m_RoutePlanningDialog->m_outLine, returnLineZone);
	}

	double direction = 0;
	double k, b = 0;
	int maxDis = 0;
	double minDis = INT_MAX;
	std::vector<QPointF>ZonePoints;
	if (searchArea.stZonePointVec.empty())
	{
		return;
	}
	for (int i = 0; i < searchArea.stZonePointVec.size() - 1; i++)
	{
		QGeoCoordinate start(searchArea.stZonePointVec[i].dLat, searchArea.stZonePointVec[i].dLon);
		QGeoCoordinate next(searchArea.stZonePointVec[i + 1].dLat, searchArea.stZonePointVec[i + 1].dLon);
		ZonePoints.push_back(QPointF(start.longitude(), start.latitude()));
		if (i >= searchArea.stZonePointVec.size() - 2)
		{
			ZonePoints.push_back(QPointF(next.longitude(), next.latitude()));
		}
		double currdist = start.distanceTo(next);
		double ag = start.azimuthTo(next);
		if (maxDis < currdist)
		{
			maxDis = currdist;
			if (abs(start.longitude() - next.longitude()) < 0.000001)
			{
				b = start.longitude();
			}
			else
			{
				k = (start.latitude() - next.latitude()) / (start.longitude() - next.longitude());
				b = start.latitude() - k * start.longitude();
			}
			direction = start.azimuthTo(next);
		}
		if (minDis > currdist)
		{
			minDis = currdist;
		}
	}

	QPointF lineNearPos;
	QPointF areaNearPos;
	QPointF areaFarPos;
	QGeoCoordinate areaNearGeo;
	int xdis = INT_MAX;
	int idis = 0;
	for (int i = 0; i < searchArea.stZonePointVec.size() - 1; i++)
	{
		QGeoCoordinate areaGeo(searchArea.stZonePointVec[i].dLat, searchArea.stZonePointVec[i].dLon);
		for (auto point : minLineZone.stZonePointVec)
		{
			QGeoCoordinate lineGeo(point.dLat, point.dLon);
			double tpDIs = lineGeo.distanceTo(areaGeo);
			if (tpDIs < xdis)
			{
				xdis = tpDIs;
				lineNearPos = QPointF(point.dLon, point.dLat);
				areaNearPos = QPointF(areaGeo.longitude(), areaGeo.latitude());
				areaNearGeo = areaGeo;
			}
		}
	}

	for (int i = 0; i < searchArea.stZonePointVec.size() - 1; i++)
	{
		QGeoCoordinate areaGeo(searchArea.stZonePointVec[i].dLat, searchArea.stZonePointVec[i].dLon);
		double dis = areaGeo.distanceTo(areaNearGeo);
		if (dis > idis)
		{
			idis = dis;
			areaFarPos = QPointF(areaGeo.longitude(), areaGeo.latitude());
		}
	}

	std::vector<casic_vector3d> pointList;
	std::map<int, std::vector<casic_vector3d>>startReturnPointMap1;
	std::vector<casic_vector3d> startList;
	allStagingPoint.clear();

	double lineHeight = (minLineZone.nBottomhAlt + minLineZone.nAboveAlt) / 2;
	for (auto ps : minLineZone.stZonePointVec)
	{
		pointList.push_back(casic_vector3d(ps.dLon, ps.dLat, lineHeight));
		startList.push_back(casic_vector3d(ps.dLon, ps.dLat, lineHeight));
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->flyRoadPointsVec.push_back(casic_vector3d(ps.dLon, ps.dLat, 0));
	}

	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->ZoneIDList.append(tr2(takeOff_land_Area.strID));
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->ZoneIDList.append(tr2(minLineZone.strID));
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->ZoneIDList.append(tr2(searchArea.strID));
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->ZoneIDList.append(tr2(returnLineZone.strID));

	int Planindex = 0;

	vector<double> ox;
	vector<double> oy;
	for (auto pos = searchArea.stZonePointVec.rbegin(); pos != searchArea.stZonePointVec.rend(); pos++)
	{

		double lon_xy = 0;
		double lat_xy = 0;
		LY_CollaborativeSearchService::Get().GetProj()->jwxy(pos->dLon, pos->dLat, &lon_xy, &lat_xy);
		ox.push_back(lon_xy);
		oy.push_back(lat_xy);
	}

	std::vector<std::vector<QPointF>>foxy;
	foxy.clear();
	int zoneType = qnzkna::ZoneManage::CZoneBase::ZoneProperty::JF;
	qnzkna::ZoneManage::CZoneBaseVec AllZoneVec = ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetAllZoneBase();
	for (auto zone : AllZoneVec)
	{
		if (zone.nProperty == zoneType)
		{
			noFlyAreaVec.push_back(zone);
		}
	}
	for (auto nofly : noFlyAreaVec)
	{
		std::vector<QPointF> tempVec;
		for (auto point : nofly.stZonePointVec)
		{
			double lon_xy = 0;
			double lat_xy = 0;
			LY_CollaborativeSearchService::Get().GetProj()->jwxy(point.dLon, point.dLat, &lon_xy, &lat_xy);
			tempVec.push_back(QPointF(lon_xy, lat_xy));
		}
		foxy.push_back(tempVec);
	}

	int lineNum = minDis / gridWidth;
	double resolution = gridWidth / 2;

	QGeoCoordinate areaFarGeo(areaFarPos.y(), areaFarPos.x());
	int moveDirection1, moveDirection2;
	if (planWay == 0)
	{
		double directi = areaNearGeo.azimuthTo(areaFarGeo);
		if (directi >= 0 && directi <= 90)
		{
			moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
			moveDirection2 = (int)UAVPlanner::SweepDirection::DOWN;
		}
		else if (directi > 90 && directi <= 180)
		{
			moveDirection1 = (int)UAVPlanner::MovingDirection::LEFT;
			moveDirection2 = (int)UAVPlanner::SweepDirection::DOWN;
		}
		else if (directi > 180 && directi <= 270)
		{
			moveDirection1 = (int)UAVPlanner::MovingDirection::LEFT;
			moveDirection2 = (int)UAVPlanner::SweepDirection::UP;
		}
		else if (directi > 270 && directi <= 360)
		{
			moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
			moveDirection2 = (int)UAVPlanner::SweepDirection::UP;
		}
	}
	else if (planWay == 1)
	{
		moveDirection1 = (int)UAVPlanner::MovingDirection::LEFT;
		moveDirection2 = (int)UAVPlanner::SweepDirection::UP;
	}
	else if (planWay == 2)
	{
		moveDirection1 = (int)UAVPlanner::MovingDirection::LEFT;
		moveDirection2 = (int)UAVPlanner::SweepDirection::DOWN;
	}
	else if (planWay == 3)
	{
		moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
		moveDirection2 = (int)UAVPlanner::SweepDirection::UP;
	}
	else {
		moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
		moveDirection2 = (int)UAVPlanner::SweepDirection::DOWN;
	}

	UAVPlanner::GridBaseSweepPlanner planner;
	vector<double> rx, ry;
	planner.planning(ox, oy, foxy, resolution, moveDirection1, moveDirection2, rx, ry);

	for (int i = 0; i < rx.size(); i++)
	{

		double lon_xy = 0;
		double lat_xy = 0;
		LY_CollaborativeSearchService::Get().GetProj()->xyjw(rx[i], ry[i], &lon_xy, &lat_xy);
		short int elevationTemp = LY_CollaborativeSearchService::Get().s_CGetHeight.GetHeight
		(lon_xy, lat_xy);
		if (elevationTemp < -10000)
		{
			elevationTemp = 0;
		}

		double height = LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->cruiseHeight;
		pointList.push_back(casic_vector3d(lon_xy, lat_xy, height));
	}

	std::vector<casic_vector3d>returnPosList;
	lineHeight = (returnLineZone.nBottomhAlt + returnLineZone.nAboveAlt) / 2;
	for (auto pt = returnLineZone.stZonePointVec.rbegin(); pt != returnLineZone.stZonePointVec.rend(); pt++)
	{
		pointList.push_back(casic_vector3d(pt->dLon, pt->dLat, lineHeight));
		startList.push_back(casic_vector3d(pt->dLon, pt->dLat, lineHeight));
		LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->flyRoadPointsVec.push_back(casic_vector3d(pt->dLon, pt->dLat, 0));
	}
	startReturnPointMap1.insert(std::make_pair(0, startList));
	std::pair<int, std::vector<casic_vector3d>>mpair = std::make_pair(Planindex, pointList);
	PointsVec.insert(mpair);

	//sparsePath(PointsVec, startReturnPointMap1);
}
