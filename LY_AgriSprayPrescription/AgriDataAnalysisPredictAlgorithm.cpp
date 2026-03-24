#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <QGeoCoordinate>
#include <QDebug>
#include "AgriDataAnalysisPredictAlgorithm.h"
#include <osgEarth\GeoMath>

using namespace AgriDataAnalysisPredictAlgorithm;

//AgriDataAnalysisPredictAlgorithm::Poi::Poi(double dLon, double dLat, double dAlt /*= 0.0*/)
//{
//	lon = dLon;
//	lat = dLat;
//	alt = dAlt;
//}
//AgriDataAnalysisPredictAlgorithm::AirInstructionPoi::AirInstructionPoi(double startLon, double endLon, double startLat, double endLat, double centerLon, double centerLat)
//{
//	startLon = startLon;
//	endLon = endLon;
//	startLat = startLat;
//	endLat = endLat;
//	centerLon = centerLon;
//	centerLat = centerLat;
//}
//double AgriDataAnalysisPredictAlgorithm::Poi::distanceTo(const Poi &other) const
//{
//	QGeoCoordinate pp1(lat, lon);
//	QGeoCoordinate pp2(other.lat, other.lon);
//	return pp1.distanceTo(pp2);
//}

#if 1

//// 计算路径长度
//double pathLength(const std::vector<Poi>& points, const std::vector<int>& order) {
//	double length = 0;
//	for (size_t i = 0; i < order.size() - 1; ++i) {
//		length += points[order[i]].distanceTo(points[order[i + 1]]);
//	}
//	length += points[order.back()].distanceTo(points[order[0]]);
//	return length;
//}
//
//// 2-opt 交换
//void twoOptSwap(std::vector<int>& order, int i, int k) {
//	std::reverse(order.begin() + i, order.begin() + k + 1);
//}
//
//// 2-opt 迭代优化
//void twoOpt(std::vector<Poi>& points, std::vector<int>& order) {
//	bool improved = true;
//	while (improved) {
//		improved = false;
//		for (size_t i = 1; i < order.size() - 1; ++i) {
//			for (size_t k = i + 1; k < order.size(); ++k) {
//				double oldDist = points[order[i - 1]].distanceTo(points[order[i]]) +
//					points[order[k]].distanceTo(points[order[(k + 1) % order.size()]]);
//				double newDist = points[order[i - 1]].distanceTo(points[order[k]]) +
//					points[order[i]].distanceTo(points[order[(k + 1) % order.size()]]);
//				if (newDist < oldDist) {
//					twoOptSwap(order, i, k);
//					improved = true;
//				}
//			}
//		}
//	}
//}
//
//// 主函数
//void AgriDataAnalysisPredictAlgorithm::zoneMinPath(const std::vector<Poi>& points, std::vector<Poi>& path) {
//	if (points.empty()) return;
//
//	int N = points.size();
//	std::vector<int> order(N);
//	for (int i = 0; i < N; ++i) order[i] = i;
//
//	// 固定随机种子，保证结果一致
//	std::srand(42);
//
//	// 初始路径：最近邻法
//	std::vector<bool> visited(N, false);
//	std::vector<int> greedy_path;
//	greedy_path.push_back(0);
//	visited[0] = true;
//
//	for (int i = 1; i < N; ++i) {
//		int last = greedy_path.back();
//		int next = -1;
//		double minDist = std::numeric_limits<double>::max();
//
//		for (int j = 0; j < N; ++j) {
//			if (!visited[j]) {
//				double d = points[last].distanceTo(points[j]);
//				if (d < minDist) {
//					minDist = d;
//					next = j;
//				}
//			}
//		}
//
//		greedy_path.push_back(next);
//		visited[next] = true;
//	}
//
//	order = greedy_path;
//
//	// 用 2-opt 优化路径
//	twoOpt(const_cast<std::vector<Poi>&>(points), order);
//
//	// 输出最优路径
//	path.clear();
//	for (int idx : order)
//		path.push_back(points[idx]);
//}

#else
void AgriDataAnalysisPredictAlgorithm::zoneMinPath(const std::vector<Poi> &points, std::vector<Poi> &path)
{
	int n = points.size();

	std::vector<std::vector<int>> graph(n, std::vector<int>(n, 0));
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (i == j) {
				graph[i][j] = 0;
				continue;
			}
			auto p1 = points[i];
			auto p2 = points[j];
			double dis = p1.distanceTo(p2);
			graph[i][j] = dis;
		}
	}

	std::vector<std::vector<int>> dp(1 << n, std::vector<int>(n, INT_MAX));
	std::vector<std::vector<int>> parent(1 << n, std::vector<int>(n, -1));

	dp[1][0] = 0;

	for (int mask = 1; mask < (1 << n); mask++) {
		for (int u = 0; u < n; u++) {
			if (!(mask & (1 << u))) continue;

			for (int v = 0; v < n; v++) {
				if (mask & (1 << v)) continue;

				int new_mask = mask | (1 << v);
				if (dp[mask][u] != INT_MAX && graph[u][v] != INT_MAX) {
					if (dp[new_mask][v] > dp[mask][u] + graph[u][v]) {
						dp[new_mask][v] = dp[mask][u] + graph[u][v];
						parent[new_mask][v] = u;
					}
				}
			}
		}
	}

	int min_cost = INT_MAX;
	int last_node = -1;
	for (int u = 1; u < n; u++) {
		if (graph[u][0] != INT_MAX && dp[(1 << n) - 1][u] != INT_MAX) {
			if (min_cost > dp[(1 << n) - 1][u] + graph[u][0]) {
				min_cost = dp[(1 << n) - 1][u] + graph[u][0];
				last_node = u;
			}
		}
	}

	// vector<int> path;
	int mask = (1 << n) - 1;
	int cur_node = last_node;
	while (cur_node != -1) {
		path.push_back(points[cur_node]);
		int pre_node = parent[mask][cur_node];
		mask ^= (1 << cur_node);
		cur_node = pre_node;
	}
	reverse(path.begin(), path.end());
}
#endif

void AgriDataAnalysisPredictAlgorithm::generatePath
        (std::vector<QPoint> points,
        	std::vector<QPoint> &path,
        	std::vector<QPoint> &uploadPath)
{

	if (points.empty()) return;

	auto cmp1 = [](const QPoint& p1, const QPoint& p2)->bool {
		if (p1.y() == p2.y()) {
			return p1.x() < p2.x();
		}
		return p1.y() < p2.y();
	};

	auto cmp2 = [](const QPoint& p1, const QPoint& p2)->bool {
		if (p1.y() == p2.y()) {
			return p1.x() > p2.x();
		}
		return p1.y() < p2.y();
	};

	std::vector<QPoint> points1 = points;//从左往右
	std::vector<QPoint> points2 = points;//从右往左

	std::sort(points1.begin(), points1.end(), cmp1);
	std::sort(points2.begin(), points2.end(), cmp2);

	// 从第一个点开始
	path.push_back(QPoint(points1[0].x() - 1, points1[0].y()));
	path.push_back(points1[0]);
	uploadPath.push_back(QPoint(points1[0].x() - 1, points1[0].y()));
	uploadPath.push_back(points1[0]);

	bool isLeft = true;
	for (size_t i = 1; i < points1.size(); ++i) {
		int x1 = path.back().x();
		int y1 = path.back().y();
		int x2 = 0;
		int y2 = 0;
		if (isLeft) {
			x2 = points1[i].x();
			y2 = points1[i].y();
		}
		else {
			x2 = points2[i].x();
			y2 = points2[i].y();
		}

		if (y1 == y2 && x1 == x2) continue;

		if (y1 == y2) {
			path.push_back(QPoint(x2, y2));
		}
		else {
			int xt = 0;
			if (isLeft) {
				xt = x1 + 1;
				x2 = points2[i].x();
				y2 = points2[i].y();
				if (x2 > x1) {
					xt = x2 + 1;
				}
			}
			else {
				xt = x1 - 1;
				x2 = points1[i].x();
				y2 = points1[i].y();
				if (x2 < x1) {
					xt = x2 - 1;
				}
			}
			path.push_back(QPoint(xt, y1)); //当前行终点
			path.push_back(QPoint(xt, y2));// 垂直移动
			path.push_back(QPoint(x2, y2));// 目标点

			uploadPath.push_back(QPoint(xt, y1));// 当前行终点
			uploadPath.push_back(QPoint(xt, y2)); // 下一行起点

			isLeft = !isLeft;
			if (isLeft) {
				xt = x2 + 1;
			}
			else {
				xt = x2 - 1;
			}
			//添加方向调整点
			path.push_back(QPoint(xt, y2));
			//uploadPath.push_back(QPoint(xt, y2));
		}
	}

	int x1 = path.back().x();
	int y1 = path.back().y();
	if (isLeft) {
		path.push_back(QPoint(x1 + 1, y1));
	}
	else {
		path.push_back(QPoint(x1 - 1, y1));
	}

	auto lastPoint = path.rbegin();
	uploadPath.push_back(*lastPoint);

}


void AgriDataAnalysisPredictAlgorithm::generateVerticalPath(std::vector<QPoint> points, std::vector<QPoint> &path)
{
	if (points.empty()) return;

	// 新排序规则：优先按x轴分层，y轴方向交替
	auto cmpNorth = [](const QPoint& p1, const QPoint& p2)->bool {
		if (p1.x() == p2.x()) {
			return p1.y() < p2.y(); // 北向：x相同时y升序（从南向北）
		}
		return p1.x() < p2.x();     // 优先处理西侧列
	};

	auto cmpSouth = [](const QPoint& p1, const QPoint& p2)->bool {
		if (p1.x() == p2.x()) {
			return p1.y() > p2.y(); // 南向：x相同时y降序（从北向南）
		}
		return p1.x() < p2.x();      // 优先处理西侧列
	};

	std::vector<QPoint> northSorted = points;
	std::vector<QPoint> southSorted = points;

	std::sort(northSorted.begin(), northSorted.end(), cmpNorth);
	std::sort(southSorted.begin(), southSorted.end(), cmpSouth);

	path.clear();
	path.push_back(northSorted[0]);  // 起点：最西侧列的最南端
	bool goingNorth = true;

	for (size_t i = 1; i < northSorted.size(); ++i) {
		const QPoint& current = path.back();
		const QPoint& next = goingNorth ? northSorted[i] : southSorted[i];

		// 同一列内直接连接
		if (current.x() == next.x()) {
			path.push_back(next);
		}
		// 跨列时需要过渡路径
		else {
			// 过渡点1：纵向移动到边界
			path.push_back(QPoint(current.x(), current.y() + (goingNorth ? 1 : -1)));
			// 过渡点2：横向移动到新列
			path.push_back(QPoint(next.x(), path.back().y()));
			// 过渡点3：纵向对齐到目标点
			path.push_back(QPoint(next.x(), next.y()));
			goingNorth = !goingNorth; // 切换方向
		}
	}
}

bool AgriDataAnalysisPredictAlgorithm::createGridGeometryMap(const std::vector<QPointF>& boundPoings, 
	const std::vector<double>& lons, 
	const std::vector<double>& lats, 
	const std::vector<double>& levels,
	double spacing,
	std::vector<QPoint>& points,
	std::map<QPoint, Poi, QPointFCompare>& pointMap)
{
	if (boundPoings.size() <= 2 || lons.size() <= 2 || lats.size() <= 2 || (std::fabs(spacing) <= 1e-9) || lons.size() != lats.size())
	{
		return false;
	}

	double maxLon = *(std::max_element(lons.begin(), lons.end()));
	double minLon = *(std::min_element(lons.begin(), lons.end()));
	double maxLat = *(std::max_element(lats.begin(), lats.end()));
	double minLat = *(std::min_element(lats.begin(), lats.end()));

	for (int i = 0; i < boundPoings.size(); i++)
	{
		maxLon = maxLon > boundPoings[i].x() ? maxLon : boundPoings[i].x();
		minLon = minLon < boundPoings[i].x() ? minLon : boundPoings[i].x();
		maxLat = maxLat > boundPoings[i].y() ? maxLat : boundPoings[i].y();
		minLat = minLat < boundPoings[i].y() ? minLat : boundPoings[i].y();
	}
	//往外扩一层
	//minLon -= spacing * 10;
	//maxLon += spacing * 10;
	//minLat -= spacing * 10;
	//maxLat += spacing * 10;
	// TODO: 修改旋转角度
	double angle = osg::DegreesToRadians(10.0);

	QGeoCoordinate cenGeo((maxLat + minLat) / 2, (maxLon + minLon) / 2);
	QGeoCoordinate x1Geo(minLat, minLon);
	QGeoCoordinate x2Geo(maxLat, minLon);
	QGeoCoordinate x3Geo(maxLat, maxLon);
	double angle_1To3 = x1Geo.azimuthTo(x3Geo);
	int dis = cenGeo.distanceTo(x1Geo) + cenGeo.distanceTo(x2Geo);

	QGeoCoordinate tempGeo = cenGeo.atDistanceAndAzimuth(dis, 180 + osg::RadiansToDegrees(angle) + angle_1To3);

	size_t rows = static_cast<size_t>((maxLat - minLat) / spacing) + 1;
	size_t cols = static_cast<size_t>((maxLon - minLon) / spacing) + 1;


	if (/*false*/angle != 0.0)
	{
		rows *= 3;
		cols *= 3;
	}

	std::vector<std::vector<double>> grid(rows, std::vector<double>(cols, 0.0));

	if (angle != 0.0)
	{
		minLon = tempGeo.longitude();
		minLat = tempGeo.latitude();
	}

	double disSpacing = spacing * 110000.0;


	for (size_t x = 0; x < cols; ++x)
	{
		for (size_t y = 0; y < rows; ++y)
		{
			//double centerLon = (2.0 * i + 1.0) * spacing / 2.0 + minLon;
			//double centerLat = (2.0 * j + 1.0) * spacing / 2.0 + minLat;
			//const QPointF center(centerLon, centerLat);

			double CenLonR, CenLatR;
			double bearingRad = atan(((y * 1.0) + 0.5) / ((x * 1.0) + 0.5)) + angle;
			double Tempdistance1 = sqrt(((x * 1.0) + 0.5) * ((x * 1.0) + 0.5) + ((y * 1.0) + 0.5) * ((y * 1.0) + 0.5)) * disSpacing;
			//destination()函数内的单位都为弧度制，需要从度°转为弧度
			osgEarth::GeoMath::destination(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
				bearingRad, Tempdistance1, CenLatR, CenLonR);

			double centerLon = osg::RadiansToDegrees(CenLonR);
			double centerlat = osg::RadiansToDegrees(CenLatR);
			const QPoint gridKey(x, y);
			pointMap[gridKey] = Poi(centerLon, centerlat);

		}
	}
	

	for (size_t idx = 0; idx < lons.size() && idx < lats.size() && idx < levels.size(); ++idx)
	{

		double tempLon = 0.0;
		double tempLat = 0.0;
		int y = 0;
		int x = 0;


		double realDistance = osgEarth::GeoMath::distance(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
			osg::DegreesToRadians(lats[idx]), osg::DegreesToRadians(lons[idx]));
		double realBearing = osgEarth::GeoMath::bearing(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
			osg::DegreesToRadians(lats[idx]), osg::DegreesToRadians(lons[idx]));
		double disBearingRad = realBearing - angle;
		y = int((sin(disBearingRad) * realDistance) / disSpacing);
		x = int((cos(disBearingRad) * realDistance) / disSpacing);

	/*	if (x >= 0 && x < static_cast<int>(cols) &&
			y >= 0 && y < static_cast<int>(rows))
		{*/
			const QPoint gridKey(x, y);
			points.push_back(gridKey);
			pointMap[gridKey] = Poi(lons[idx], lats[idx], levels[idx]); // 更新为实际坐标
		//}
	}

	return true;
}

bool AgriDataAnalysisPredictAlgorithm::createVerticalGridGeometryMap(const std::vector<QPointF>& boundPoings, const std::vector<double>& lons, const std::vector<double>& lats, double spacing, std::vector<QPoint>& points, std::map<QPoint, Poi, QPointFCompare>& pointMap)
{
	if (boundPoings.size() <= 2 || lons.size() <= 2 || lats.size() <= 2 || (std::fabs(spacing) <= 1e-9))
	{
		return false;
	}

	double maxLon = *(std::max_element(lons.begin(), lons.end()));
	double minLon = *(std::min_element(lons.begin(), lons.end()));
	double maxLat = *(std::max_element(lats.begin(), lats.end()));
	double minLat = *(std::min_element(lats.begin(), lats.end()));

	for (int i = 0; i < boundPoings.size(); i++)
	{
		maxLon = maxLon > boundPoings[i].x() ? maxLon : boundPoings[i].x();
		minLon = minLon < boundPoings[i].x() ? minLon : boundPoings[i].x();
		maxLat = maxLat > boundPoings[i].y() ? maxLat : boundPoings[i].y();
		minLat = minLat < boundPoings[i].y() ? minLat : boundPoings[i].y();
	}

	size_t rows = static_cast<size_t>((maxLat - minLat) / spacing) + 1;
	size_t cols = static_cast<size_t>((maxLon - minLon) / spacing) + 1;

	for (size_t i = 0; i < cols; ++i)
	{
		for (size_t j = 0; j < rows; ++j)
		{
			double centerLon = (2.0 * i + 1.0) * spacing / 2.0 + minLon;
			double centerLat = (2.0 * j + 1.0) * spacing / 2.0 + minLat;
			pointMap[QPoint(i, j)] = Poi(centerLon, centerLat);
		}
	}

	// 映射采样点到网格（左上角坐标系）
	points.clear();
	for (size_t idx = 0; idx < lons.size() && idx < lats.size(); ++idx) {
		// 关键修改点：y坐标计算反向
		int x = static_cast<int>((lons[idx] - minLon) / spacing);
		int y = static_cast<int>((maxLat - lats[idx]) / spacing); // 纬度越高y越小

		if (x >= 0 && x < static_cast<int>(cols) &&
			y >= 0 && y < static_cast<int>(rows))
		{
			points.emplace_back(QPoint(x, y));
			pointMap[QPoint(x, y)] = Poi(lons[idx], lats[idx]);
		}
	}

	return true;
}

// 射线法判断点是否在多边形内
bool AgriDataAnalysisPredictAlgorithm::isPointInPolygon(const QPointF &point, const std::vector<QPointF> &polygon) {
	bool inside = false;
	int n = polygon.size();
	if (n < 3) return false; // 至少需要3个点构成多边形

	for (int i = 0, j = n - 1; i < n; j = i++) {
		const QPointF &p1 = polygon[j];
		const QPointF &p2 = polygon[i];

		// 检查点Y坐标是否在边的Y范围内
		if (((p1.y() > point.y()) != (p2.y() > point.y())) &&
			(point.x() < (p2.x() - p1.x()) * (point.y() - p1.y()) / (p2.y() - p1.y()) + p1.x())) {
			inside = !inside;
		}
	}
	return inside;
}
bool AgriDataAnalysisPredictAlgorithm::createGridPointDiffuse(const std::vector<QPointF>& boundPoings, 
	const std::vector<double>& lons, 
	const std::vector<double>& lats,
	const std::vector<double>& values,
	double spacing,
	int difCount, 
	std::vector<Poi>& difPoints)
{
	if (boundPoings.size() <= 2 || lons.size() <= 0 || lats.size() <= 0 || (spacing <= 1e-9) || (difCount < 1))
	{
		return false;
	}

	double maxLon = *(std::max_element(lons.begin(), lons.end()));
	double minLon = *(std::min_element(lons.begin(), lons.end()));
	double maxLat = *(std::max_element(lats.begin(), lats.end()));
	double minLat = *(std::min_element(lats.begin(), lats.end()));

	for (int i = 0; i < boundPoings.size(); i++)
	{
		maxLon = maxLon > boundPoings[i].x() ? maxLon : boundPoings[i].x();
		minLon = minLon < boundPoings[i].x() ? minLon : boundPoings[i].x();
		maxLat = maxLat > boundPoings[i].y() ? maxLat : boundPoings[i].y();
		minLat = minLat < boundPoings[i].y() ? minLat : boundPoings[i].y();
	}

	size_t rows = static_cast<size_t>((maxLat - minLat) / spacing) + 1;
	size_t cols = static_cast<size_t>((maxLon - minLon) / spacing) + 1;

	std::vector<std::vector<double>> grid(cols, std::vector<double>(rows, 0.0));

	auto transferLevel = [&]( int level)->int {
		return (level - 1) / (10 / difCount) + 1;
	};

	int layer = difCount;
	for (size_t idx = 0; idx < lons.size() && idx < lats.size() && idx < values.size(); ++idx) {
		int x = (lons[idx] - minLon) / spacing;
		int y = (lats[idx] - minLat) / spacing;
		double value = transferLevel(values[idx]);

		for (int i = x - layer; i <= x + layer; ++i) {
			for (int j = y - layer; j <= y + layer; ++j) {
				if (i >= 0 && i < cols && j >= 0 && j < rows)
				{
					double tmpCalcVal = value - abs(x - i) - abs(y - j);
					grid[i][j] = std::max(grid[i][j], tmpCalcVal);
				}
			}
		}
	}
	//AirInstructionPoi lastAirInsPoi;
	for (size_t i = 0; i < cols; ++i)
	{
		for (size_t j = 0; j < rows; ++j)
		{
			if (grid[i][j] > 0)
			{
				double centerLon = (2 * i + 1) * spacing / 2 + minLon;
				double centerLat = (2 * j + 1) * spacing / 2 + minLat;
				double centerAlt = grid[i][j];
				difPoints.emplace_back(Poi(centerLon, centerLat, centerAlt));


				//qDebug() << "======--------***:" << airInfPoints.size();

			}
		}
	}

	return true;
}
void AgriDataAnalysisPredictAlgorithm::planning(std::vector<double>& ox, std::vector<double>& oy,
	std::vector<std::vector<QPointF>>foxy,
	std::vector<Poi> stZonePointVec,
	QPointF areaFarPos,
	double resolution,
	std::vector<double>& rx,
	std::vector<double>& ry,
	int planWay)
{

	QGeoCoordinate areaNearGeo;
	int xdis = INT_MAX;
	int idis = 0;
	for (int i = 0; i < stZonePointVec.size() - 1; i++)
	{
		QGeoCoordinate areaGeo(stZonePointVec[i].lat, stZonePointVec[i].lon);
		areaNearGeo = areaGeo;
		//for (auto point : minLineZone.stZonePointVec)
		//{
		//	QGeoCoordinate lineGeo(point.dLat, point.dLon);
		//	double tpDIs = lineGeo.distanceTo(areaGeo);
		//	if (tpDIs < xdis)
		//	{
		//		xdis = tpDIs;
		//		lineNearPos = QPointF(point.dLon, point.dLat);
		//		areaNearPos = QPointF(areaGeo.longitude(), areaGeo.latitude());
		//		areaNearGeo = areaGeo;
		//	}
		//}
	}

	QGeoCoordinate areaFarGeo(areaFarPos.y(), areaFarPos.x());
	int moveDirection1, moveDirection2;
	if (planWay == 0)
	{
	/*	double directi = areaNearGeo.azimuthTo(areaFarGeo);
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
		{*/
		moveDirection1 = -1;
		moveDirection2 = 1;
		//}
		//else if (directi > 270 && directi <= 360)
		//{
		//	moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
		//	moveDirection2 = (int)UAVPlanner::SweepDirection::UP;
		//}
	}
	else if (planWay == 1)
	{
		moveDirection1 = -1;
		moveDirection2 = -1;
	}
	else if (planWay == 2)
	{
		moveDirection1 = 1;
		moveDirection2 = 1;
	}
	else if (planWay == 3)
	{
		moveDirection1 = 1;
		moveDirection2 = -1;
	}
	else {
		moveDirection1 = (int)UAVPlanner::MovingDirection::RIGHT;
		moveDirection2 = (int)UAVPlanner::SweepDirection::DOWN;
	}

	UAVPlanner::GridBaseSweepPlanner planner;
	planner.planning(ox, oy, foxy, resolution, moveDirection1, moveDirection2, rx, ry);


}
