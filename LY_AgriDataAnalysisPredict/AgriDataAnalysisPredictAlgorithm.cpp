#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <QGeoCoordinate>

#include "AgriDataAnalysisPredictAlgorithm.h"

using namespace AgriDataAnalysisPredictAlgorithm;

AgriDataAnalysisPredictAlgorithm::Poi::Poi(double dLon, double dLat, double dAlt /*= 0.0*/)
{
	lon = dLon;
	lat = dLat;
	alt = dAlt;
}

double AgriDataAnalysisPredictAlgorithm::Poi::distanceTo(const Poi &other) const
{
	QGeoCoordinate pp1(lat, lon);
	QGeoCoordinate pp2(other.lat, other.lon);
	return pp1.distanceTo(pp2);
}

#if 1

// 计算路径长度
double pathLength(const std::vector<Poi>& points, const std::vector<int>& order) {
	double length = 0;
	for (size_t i = 0; i < order.size() - 1; ++i) {
		length += points[order[i]].distanceTo(points[order[i + 1]]);
	}
	length += points[order.back()].distanceTo(points[order[0]]);
	return length;
}

// 2-opt 交换
void twoOptSwap(std::vector<int>& order, int i, int k) {
	std::reverse(order.begin() + i, order.begin() + k + 1);
}

// 2-opt 迭代优化
void twoOpt(std::vector<Poi>& points, std::vector<int>& order) {
	bool improved = true;
	while (improved) {
		improved = false;
		for (size_t i = 1; i < order.size() - 1; ++i) {
			for (size_t k = i + 1; k < order.size(); ++k) {
				double oldDist = points[order[i - 1]].distanceTo(points[order[i]]) +
					points[order[k]].distanceTo(points[order[(k + 1) % order.size()]]);
				double newDist = points[order[i - 1]].distanceTo(points[order[k]]) +
					points[order[i]].distanceTo(points[order[(k + 1) % order.size()]]);
				if (newDist < oldDist) {
					twoOptSwap(order, i, k);
					improved = true;
				}
			}
		}
	}
}

// 主函数
void AgriDataAnalysisPredictAlgorithm::zoneMinPath(const std::vector<Poi>& points, std::vector<Poi>& path) {
	if (points.empty()) return;

	int N = points.size();
	std::vector<int> order(N);
	for (int i = 0; i < N; ++i) order[i] = i;

	// 固定随机种子，保证结果一致
	std::srand(42);

	// 初始路径：最近邻法
	std::vector<bool> visited(N, false);
	std::vector<int> greedy_path;
	greedy_path.push_back(0);
	visited[0] = true;

	for (int i = 1; i < N; ++i) {
		int last = greedy_path.back();
		int next = -1;
		double minDist = std::numeric_limits<double>::max();

		for (int j = 0; j < N; ++j) {
			if (!visited[j]) {
				double d = points[last].distanceTo(points[j]);
				if (d < minDist) {
					minDist = d;
					next = j;
				}
			}
		}

		greedy_path.push_back(next);
		visited[next] = true;
	}

	order = greedy_path;

	// 用 2-opt 优化路径
	twoOpt(const_cast<std::vector<Poi>&>(points), order);

	// 输出最优路径
	path.clear();
	for (int idx : order)
		path.push_back(points[idx]);
}

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

void AgriDataAnalysisPredictAlgorithm::generatePath(std::vector<QPoint> points, std::vector<QPoint> &path)
{
	if (points.empty()) return;

	auto cmp = [](const QPoint& p1, const QPoint& p2)->bool {
		return p1.y() < p2.y();
	};

	std::sort(points.begin(), points.end(), cmp);

	// 从第一个点开始
	path.push_back(points[0]);

	for (size_t i = 1; i < points.size(); ++i) {
		int x1 = path.back().x();
		int y1 = path.back().y();
		int x2 = points[i].x();
		int y2 = points[i].y();

		bool isshuiping = false;
		bool ischuizhi = false;
		// 先水平移动，再垂直移动
		if (x1 != x2) {
			path.push_back(QPoint(x2, y1));
			isshuiping = true;
		}
		if (y1 != y2) {
			path.push_back(QPoint(x2, y2));
			ischuizhi = true;
		}

		if (!ischuizhi) {
			if (x2 == x1) continue;
			int x3 = (x2 - x1) / std::abs(x2 - x1) + x2;
			path.push_back(QPoint(x3, y1));
		}
		else {
			if (y2 == y1) continue;
			int y3 = (y2 - y1) / std::abs(y2 - y1) + y2;
			path.push_back(QPoint(x2, y3));
		}
	}
}

bool AgriDataAnalysisPredictAlgorithm::createGridGeometryMap(const std::vector<QPointF>& boundPoings, const std::vector<double>& lons, const std::vector<double>& lats, double spacing, std::vector<QPoint>& points, std::map<QPoint, Poi, QPointFCompare>& pointMap)
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

	for (size_t idx = 0; idx < lons.size() && idx < lats.size(); ++idx)
	{
		int x = (lons[idx] - minLon) / spacing;
		int y = (lats[idx] - minLat) / spacing;

		for (int i = x; i <= x; ++i)
		{
			for (int j = y; j <= y; ++j)
			{
				if (i >= 0 && i < cols && j >= 0 && j < rows)
				{
					if (abs(x - i) <= 0 && abs(y - j) <= 0)
					{
						points.emplace_back(QPoint(i, j));
						pointMap[QPoint(i, j)] = Poi(lons[idx], lats[idx]);
					}
				}
			}
		}
	}

	return true;
}

bool AgriDataAnalysisPredictAlgorithm::createGridPointDiffuse(const std::vector<QPointF>& boundPoings, const std::vector<double>& lons, const std::vector<double>& lats, const std::vector<double>& values, double spacing, int difCount, std::vector<Poi>& difPoints)
{
	if (boundPoings.size() <= 2 || lons.size() <= 2 || lats.size() <= 2 || (spacing <= 1e-9) || (difCount < 1))
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

	auto transferLevel = [&](int level)->int {
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
			}
		}
	}

	return true;
}
