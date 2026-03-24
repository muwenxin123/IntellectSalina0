#include "aStar.h"
#include <ctime>
#include "QtPositioning\QGeoCoordinate"

#define RADTODEG(r) ((r) * 57.29577951308232)
#define DEGTORAD(d)  ((d) * 0.017453292519943295)

#define MIN(a,b)  ((a) > (b) ? (b) : (a))
#define MAX(a,b)  ((a) > (b) ? (a) : (b))

#define PI       3.14159265358979323846
using namespace std;

casic_vector3d getLonLat_By_disangle(double &lon, double &lat, double &distance, double& angle)
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

	return newPosition;

}

static const double WGS84_A = 6378137.0;
static const double WGS84_E = 0.0818191908;
casic_vector3d lla2ecef(casic_vector3d pos)
{
	double lon = pos.x;
	double lat = pos.y;
	double alt = pos.z;

	double WGS84_f = 1 / 298.257223565;
	double WGS84_E2 = WGS84_f * (2 - WGS84_f);

	lon = DEGTORAD(lon);
	lat = DEGTORAD(lat);

	double N = WGS84_A / (sqrt(1 - WGS84_E2 * sin(lat) * sin(lat)));

	double x = (N + alt)*cos(lat) * cos(lon);
	double y = (N + alt)*cos(lat) * sin(lon);
	double z = (N * (1 - WGS84_f) * (1 - WGS84_f) + alt)*sin(lat);

	return casic_vector3d(x, y, z);
}
casic_vector3d ecef2lla(casic_vector3d pos)
{
	double x = pos.x;
	double y = pos.y;
	double z = pos.z;

	const double b = sqrt(WGS84_A * WGS84_A*(1 - WGS84_E * WGS84_E));
	const double ep = sqrt((WGS84_A * WGS84_A - b * b) / (b * b));
	const double p = hypot(x, y);
	const double th = atan2(WGS84_A * z, b * p);
	const double lon = atan2(y, x);
	const double lat = atan2((z + ep * ep * b * pow(sin(th), 3)), (p - WGS84_E * WGS84_E *WGS84_A * pow(cos(th), 3)));
	const double N = WGS84_A / sqrt(1 - WGS84_E * WGS84_E * sin(lat) * sin(lat));
	const double alt = p / cos(lat) - N;

	return casic_vector3d(RADTODEG(lon), RADTODEG(lat), alt);
}

AStar::PlanNode::PlanNode() {
}

AStar::PlanNode::PlanNode(double _x, double _y, double _xindex, double _yindex, double _gCost, double _hCost )
	: m_x(_x), m_y(_y), m_gCost(_gCost), m_hCost(_hCost),
	m_obstacle(false), m_backpoint(nullptr), m_isVisited(false), m_indexX(_xindex), m_indexY(_yindex){
}

AStar::PlanNode::~PlanNode() {
}

void AStar::PlanNode::setObstacle() {
	m_obstacle = true;
}

bool AStar::PlanNode::isObstacle() {
	return m_obstacle;
}

AStar::PlanNode& AStar::PlanNode::operator +=(const PlanNode& rhs) {
	m_x += rhs.m_x;
	m_y += rhs.m_y;
	m_indexX += rhs.m_indexX;
	m_indexY += rhs.m_indexY;
	m_gCost += rhs.m_gCost;
	m_hCost += rhs.m_hCost;
	return *this;
}

AStar::PlanNode& AStar::PlanNode::operator =(const PlanNode& rhs) {
	m_x = rhs.m_x;
	m_y = rhs.m_y;
	m_indexX = rhs.m_indexX;
	m_indexY = rhs.m_indexY;
	m_gCost = rhs.m_gCost;
	m_hCost = rhs.m_hCost;
	return *this;
}

bool AStar::PlanNode::operator ==(const PlanNode& rhs) {
	return  m_indexX == rhs.m_indexX && m_indexY == rhs.m_indexY;
}

double AStar::PlanNode::coordinateX() const {
	return m_x;
}

double AStar::PlanNode::coordinateY() const {
	return m_y;
}

void AStar::PlanNode::setBackpoint(std::shared_ptr<PlanNode> rhs) {
	m_backpoint = rhs;
}

std::shared_ptr<AStar::PlanNode> AStar::PlanNode::getBackpoint() {
	return m_backpoint;
}

bool AStar::PlanNode::isVisited() {
	return m_isVisited;
}

void AStar::PlanNode::setVisited() {
	m_isVisited = true;
}

double AStar::PlanNode::indexX() const
{
	return m_indexX;
}

double AStar::PlanNode::indexY() const
{
	return m_indexY;
}

void AStar::PlanNode::setindexX(double index)
{
	m_indexX = index;
}

void AStar::PlanNode::setindexY(double index)
{
	m_indexY = index;
}

void AStar::PlanNode::setHCost(double _hCost) {
	m_hCost = _hCost;
}

double AStar::PlanNode::getHCost() const {
	return m_hCost;
}

void AStar::PlanNode::setG(double _gCost) {
	m_gCost = _gCost;
}

double AStar::PlanNode::getGCost() const {
	return m_gCost;
}

void AStar::PlanNode::setTotalCost() {

	m_totalCost = m_gCost + m_hCost;

}

double AStar::PlanNode::getTotalCost() const {
	return m_gCost + m_hCost;
}

AStar::AStar() {
	srand(time(nullptr));
}

AStar::~AStar() {

}

void AStar::init(int mapSize, int gridSize, double Height, double safedis,
	const casic_vector3d& start, const casic_vector3d& goal) {
	m_mapSize = mapSize;
	m_gridSize = gridSize;
	m_Height = Height;

	casic_vector3d Pos_S;
	casic_vector3d Pos_G;

	Pos_S = lla2ecef(start);
	Pos_G = lla2ecef(goal);

	QGeoCoordinate point1(start.y,start.x);
	QGeoCoordinate point2(goal.y, goal.x);
	double ddis = point1.distanceTo(point2);

	double  dis_step = safedis;
	m_safeDis = safedis;
	m_lonPointNum = mapSize / (dis_step) + 1;

	m_latPointNum = gridSize / (dis_step) + 1;

	double lat1, lon1, alt, lat2, lon2;
	lon1 = start.x;
	lat1 = start.y;
	lon2 = goal.x;
	lat2 = goal.y;

	m_startNode = PlanNode(lon1, lat1, 0, 0);
	m_goalNode = PlanNode(lon2, lat2, m_lonPointNum-1, m_latPointNum-1);

	if (GridMap.size() != 0) {
		int i, j;
		int xindex_start, yindex_start, xindex_goal, yindex_goal;
		double cost_start = std::pow(2, 30);
		double cost_goal = std::pow(2, 30);
		for (i = 0; i < GridMap.size(); i++) {
			std::vector<std::shared_ptr<PlanNode>> tmp;
			for (j = 0; j < GridMap[i].size(); j++) {
				double lon = GridMap[i][j][0];
				double lat = GridMap[i][j][1];
				double index_x = GridMap[i][j][2];
				double index_y = GridMap[i][j][3];
				std::shared_ptr<PlanNode> tmpNode = std::make_shared<PlanNode>(lon, lat, index_x, index_y);
				QGeoCoordinate point(lat, lon);
				tmpNode->setHCost(point.distanceTo(point2));
				if ((GridMap[i][j][4] - 0) > 0.001) tmpNode->setObstacle();
				tmp.push_back(tmpNode);
				if (point.distanceTo(point1) < cost_start) {
					xindex_start = index_x;
					yindex_start = index_y;
					cost_start = point.distanceTo(point1);
				}
				if (point.distanceTo(point2) < cost_goal) {
					xindex_goal = index_x;
					yindex_goal = index_y;
					cost_goal = point.distanceTo(point2);
				}
			}
			m_roadmap.push_back(tmp);
		}
		m_startNode = PlanNode(lon1, lat1, xindex_start, yindex_start);
		m_goalNode = PlanNode(lon2, lat2, xindex_goal, yindex_goal);
		m_lonPointNum = i;
		m_latPointNum = j;

		return;
	}

	double lon_current = m_startNode.coordinateX();
	double lat_current = m_startNode.coordinateY();

	double delta_lon = (m_goalNode.coordinateX() - m_startNode.coordinateX()) / m_lonPointNum;
	double delta_lat = (m_goalNode.coordinateY() - m_startNode.coordinateY()) / m_latPointNum;

	for (int i = 0; i < m_lonPointNum; ++i) {
		vector<shared_ptr<PlanNode>> tmp;
		for (int j = 0; j < m_latPointNum; ++j) {

			casic_vector2d pos_lonlat = casic_vector2d(m_startNode.coordinateX() + delta_lon * i, m_startNode.coordinateY() + delta_lat * j);
			shared_ptr<PlanNode> tmpNode = make_shared<PlanNode>(pos_lonlat.x, pos_lonlat.y,i,j);

			QGeoCoordinate point(pos_lonlat.y, pos_lonlat.x);
			tmpNode->setHCost(point.distanceTo(point2));

			tmp.push_back(tmpNode);
			lon_current = pos_lonlat.x;
			lat_current = pos_lonlat.y;
		}
		m_roadmap.push_back(tmp);
	}

	defineObstacle(Height);

}

void AStar::defineObstacle(double Height) {

	for (int i = 0; i < m_lonPointNum; ++i)
	{
		for (int j = 0; j < m_latPointNum; ++j)
		{
			double lon = m_roadmap[i][j]->coordinateX();
			double lat = m_roadmap[i][j]->coordinateY();
			double alt = 0,lat2 = 0,lon2 = 0,alt2 = 0;

		}

	}

	if (!ObstacleUAVs.empty())
	{
		vector<shared_ptr<PlanNode>> tmp;
		for (auto pos : ObstacleUAVs)
		{
			for (int i = 0; i < m_lonPointNum; ++i) {
				for (int j = 0; j < m_latPointNum; ++j) {
					double lon = m_roadmap[i][j]->coordinateX();
					double lat = m_roadmap[i][j]->coordinateY();
					QGeoCoordinate pointS(lat, lon);
					QGeoCoordinate pointG(pos.y, pos.x);
					if ((lon == (double)pos.x && lat == (double)pos.y) || pointS.distanceTo(pointG) < m_safeDis)
					{
						m_roadmap[i][j]->setObstacle();
					}
				}
			}

		}
	}

	if (NoFlyZonePoints.size() > 0) {
		for (int i = 0; i < m_roadmap.size(); i++) {
			for (int j = 0; j < m_roadmap[i].size(); j++) {
				double lon = m_roadmap[i][j]->coordinateX();
				double lat = m_roadmap[i][j]->coordinateY();
				QPointF point = QPointF(lon, lat);
				bool is_inside = false;
				for (auto area : NoFlyZonePoints) {
					if (is_point_in_zone(point, area)) {
						is_inside = true;
						break;
					}
				}

				if (is_inside) {
					m_roadmap[i][j]->setObstacle();
				}
			}
		}
	}

}

void AStar::pathPlan(std::vector<casic_vector3d> &PointsVec) {

	casic_vector3d pos;

	auto cmp = [](shared_ptr<PlanNode> left, shared_ptr<PlanNode> right) {
		return left->getTotalCost() < right->getTotalCost();
	};

	priority_queue<shared_ptr<PlanNode>,
		vector<shared_ptr<PlanNode>>, decltype(cmp)> O(cmp);

	if (m_roadmap.empty() || m_roadmap[0].empty())
	{
		return;
	}
	shared_ptr<PlanNode> tmp = m_roadmap[m_startNode.indexX()][m_startNode.indexY()];
	O.push(tmp);

	std::vector<PlanNode> motion = {
		PlanNode(0, 0, -1, 0, 1),
		PlanNode(0, 0, 0, -1, 1),
		PlanNode(0, 0, 1, 0, 1),
		PlanNode(0, 0, 0, 1, 1),
		PlanNode(0, 0, -1, -1, std::sqrt(2)),
		PlanNode(0, 0, -1, 1, std::sqrt(2)),
		PlanNode(0, 0, 1, -1, std::sqrt(2)),
		PlanNode(0, 0, 1, 1, std::sqrt(2))
	};

	shared_ptr<PlanNode> currNode = m_roadmap[m_goalNode.indexX()][m_goalNode.indexY()];

	while (!O.empty()) {

		shared_ptr<PlanNode> nBest = O.top();
		O.pop();

		nBest->setVisited();

		if (*nBest == m_goalNode) {
			is_found = true;
			break;
		}

		for (int i = 0; i < motion.size(); i++) {
			PlanNode tmpNode = motion[i];
			tmpNode += *nBest;
			int xxx = tmpNode.indexX();
			int yyy = tmpNode.indexY();
			if (tmpNode.indexX() < 0 || tmpNode.indexY() < 0 || tmpNode.indexX() >= m_lonPointNum || tmpNode.indexY() >= m_latPointNum)
			{
				continue;
			}
			shared_ptr<PlanNode> tmpNodePointer =
				m_roadmap[tmpNode.indexX()][tmpNode.indexY()];

			if (!verifyNode(*tmpNodePointer) || tmpNodePointer->isObstacle()) {
				continue;
			}
			if (!tmpNodePointer->isVisited()) {

				O.push(tmpNodePointer);
				m_countvec.push_back(tmpNodePointer);

				tmpNodePointer->setVisited();
				tmpNodePointer->setBackpoint(nBest);
				m_point = tmpNodePointer;

			}
		}
	}

	tmp = currNode;
	shared_ptr<PlanNode> pre_node = nullptr;
	double pre_angle = 0;
	int count = 0;
	while (!(tmp->indexX()== 0 && tmp->indexY() == 0)) {

		pos = casic_vector3d(tmp->coordinateX(), tmp->coordinateY(), m_Height);

		PointsVec.push_back(pos);

		if (pre_node != nullptr) {
			QGeoCoordinate p1(tmp->coordinateY(), tmp->coordinateX());
			QGeoCoordinate p2(pre_node->coordinateY(), pre_node->coordinateX());

			double straight_time = p1.distanceTo(p2) / straight_speed;
			double angular_time = std::fabs(p1.azimuthTo(p2) - pre_angle) / angular_speed;
			fly_time += (straight_time + angular_time);
			pre_angle = p1.azimuthTo(p2);
		}

		if (tmp->getBackpoint() == nullptr)
		{
			break;
		}
		pre_node = tmp;
		tmp = tmp->getBackpoint();

		count++;

	}

}

bool AStar::is_point_in_zone(const QPointF point, const std::vector<QPointF> ZonePoints)
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