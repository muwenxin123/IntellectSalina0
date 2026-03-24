#include "CellularAutomata.h"
#include "qmath.h"
#include <QThread>
#include <QCString.h>
#include <QGeoCoordinate>
#include <set>

#define PI 3.1415926

struct Point {
	Point() {};
	Point(int px, int py) { x = px; y = py; };
	int x, y;
	bool operator<(const Point &p) const {
		return x < p.x || (x == p.x && y < p.y);
	}
};

// 使用 Bresenham 直线算法计算边界上的点
std::set<Point> getBoundaryPoints(const vector<Point> &polygon) {
	std::set<Point> boundary;
	int n = polygon.size();

	for (int i = 0; i < n; i++) {
		Point p1 = polygon[i], p2 = polygon[(i + 1) % n];

		int dx = abs(p2.x - p1.x), dy = abs(p2.y - p1.y);
		int sx = (p1.x < p2.x) ? 1 : (p1.x > p2.x) ? -1 : 0;
		int sy = (p1.y < p2.y) ? 1 : (p1.y > p2.y) ? -1 : 0;

		int err = dx - dy, e2;
		int x = p1.x, y = p1.y;

		while (true) {
			boundary.insert({ x, y });
			if (x == p2.x && y == p2.y) break;
			e2 = 2 * err;
			if (e2 > -dy) { err -= dy; x += sx; }
			if (e2 < dx) { err += dx; y += sy; }
		}
	}

	return boundary;
}

// 扫描线填充算法
void fillPolygon(const std::vector<Point> &polygon, std::vector<std::vector<double>>& outDltVec, double value)
{
	std::set<Point> boundary = getBoundaryPoints(polygon);
	std::set<Point> filled = boundary;

	int minY = polygon[0].y, maxY = polygon[0].y;
	for (const auto &p : polygon) {
		minY = std::min(minY, p.y);
		maxY = std::max(maxY, p.y);
	}

	for (int y = minY; y <= maxY; y++) {
		std::vector<int> intersections;
		for (const auto &p : boundary) {
			if (p.y == y) intersections.push_back(p.x);
		}

		sort(intersections.begin(), intersections.end());
		for (size_t i = 0; i < intersections.size(); i += 2) {
			if (i + 1 >= intersections.size()) break;
			for (int x = intersections[i]; x <= intersections[i + 1]; x++) {
				outDltVec[x][y] = std::max(outDltVec[x][y], value);
			}
		}
	}
}

CellularAutomata::CellularAutomata()
{
	KwMap[1] = 1.2;
	KwMap[2] = 1.4;
	KwMap[3] = 1.7;
	KwMap[4] = 2.0;
	KwMap[5] = 2.4;
	KwMap[6] = 2.9;
	KwMap[7] = 3.3;
	KwMap[8] = 4.1;
	KwMap[9] = 5.1;
	KwMap[10] = 6.0;
	KwMap[11] = 7.3;
	KwMap[12] = 8.5;
}

CellularAutomata::~CellularAutomata()
{
}


void CellularAutomata::setGridSize(double grid_w)
{
	gridWidth = grid_w;
}

void CellularAutomata::setMapSize(double l, double w)
{
	mapRowCount = w / gridWidth;
	mapColCount = l / gridWidth;
	InfoCell initInfoCell;
	QVector<QVector<InfoCell>> temp1(mapRowCount, QVector<InfoCell>(mapColCount, initInfoCell));
	fireMap = temp1;
}

void CellularAutomata::setInitFireInfo(double lon, double lat, double l, double w)
{
	fFireInfo = FirstFireInfo(lon, lat, l, w);
}

void CellularAutomata::setSpreadTime(int time_sec)
{
	fireSpreadTime = time_sec;
}

void CellularAutomata::setTimeStep(int time_sec)
{
	timeStep = time_sec;
}

void CellularAutomata::initMap()
{
	QGeoCoordinate cenPoi(fFireInfo.lat, fFireInfo.lon);
	double angel = std::atan(mapColCount / mapRowCount * 1.0) * 180 / PI;
	double dis = std::sqrt((mapRowCount*gridWidth / 2)*(mapRowCount*gridWidth / 2) +
		(mapColCount*gridWidth / 2)*(mapColCount*gridWidth / 2));
	QGeoCoordinate firstPoi = cenPoi.atDistanceAndAzimuth(dis, 360 - angel);
	fireMap[0][0].position = QPointF(firstPoi.longitude(), firstPoi.latitude());

	for (int i = 1; i < mapRowCount; i++) {
		QGeoCoordinate poi = firstPoi.atDistanceAndAzimuth(i*gridWidth, 180);
		fireMap[i][0].position = QPointF(poi.longitude(), poi.latitude());
	}

	for (int i = 0; i < mapRowCount; i++) {
		QGeoCoordinate firstP(fireMap[i][0].position.y(), fireMap[i][0].position.x());
		for (int j = 1; j < mapColCount; j++) {
			QGeoCoordinate poi = firstP.atDistanceAndAzimuth(j*gridWidth, 90);
			fireMap[i][j].position = QPointF(poi.longitude(), poi.latitude());
		}
	}

	int coln = fFireInfo.l / gridWidth;
	int rown = fFireInfo.w / gridWidth;
	int cenX = mapRowCount / 2;
	int cenY = mapColCount / 2;
	int ltX = cenX - rown / 2;
	int ltY = cenY - coln / 2;

	for (int i = ltX; i < ltX + rown; i++) {
		for (int j = ltY; j < ltY + coln; j++) {
			fireMap[i][j].state = stateCell::fire;
			fireMap[i][j].firedArea = gridWidth * gridWidth * 1.0;
		}
	}
}

bool CellularAutomata::spreadFire(const QVector<int> &weatherConditionVec, QQueue<QVector<QPointF>>& outQueue)
{
	if (weatherConditionVec.size() < 4)
		return false;

	double T = weatherConditionVec[0];
	double V = 1;
	double h = weatherConditionVec[1];
	double R0 = (0.03*T + 0.05*V + 0.01*h - 0.3) / 60;
	double Ks = 1.0;
	int idx = dx[weatherConditionVec[2]];
	int idy = dy[weatherConditionVec[2]];
	double Kw = KwMap[weatherConditionVec[3]];
	double pd = 0;
	int zs = 3.533*std::pow(std::tan(pd), 1.2);
	double Kpd = std::pow(10, zs);
	double R = R0 * Ks * Kw * Kpd;

	setMapSize(R*fireSpreadTime * 2 + fFireInfo.l*3, R*fireSpreadTime * 2 + fFireInfo.l*3);
	initMap();

	int num = fireSpreadTime / timeStep;
	while (num--) {
		for (int i = 0; i < mapRowCount; i++) {
			for (int j = 0; j < mapColCount; j++) {
				if (fireMap[i][j].state == stateCell::fire) {
					double addFireArea = 0;
					for (int k = 0; k < 8; k++) {
						int newX = i + dx[k];
						int newY = j + dy[k];

						Kw = KwMap[weatherConditionVec[3]];

						if (dx[k] == idx) {
							if (idy == 0) {
								if(dy[k] == 0) Kw = KwMap[weatherConditionVec[3]];
								else Kw = Kw - weatherConditionVec[3] * 0.1;
							}
							else {
								if (dy[k] == idy) Kw = KwMap[weatherConditionVec[3]];
								else if (dy[k] == -1 * idy) Kw = Kw - weatherConditionVec[3] * 0.2;
								else if (dy[k] == 0) Kw = Kw - weatherConditionVec[3] * 0.1;
							}
						}
						else if (dy[k] == idy) {
							if (idx == 0) {
								if (dx[k] == 0) Kw = KwMap[weatherConditionVec[3]];
								else Kw = Kw - weatherConditionVec[3] * 0.1;
							}
							else {
								if (dx[k] == idx) Kw = KwMap[weatherConditionVec[3]];
								else if (dx[k] == -1 * idx) Kw = Kw - weatherConditionVec[3] * 0.2;
								else if (dx[k] == 0) Kw = Kw - weatherConditionVec[3] * 0.1;
							}
						}
						else if ((dx[k]+idx)==0 && (dy[k]+idy)==0) {
							Kw = 1 / Kw;
						}
						else {
							Kw = 1.0;
						}
						R = R0 * Ks * Kw * Kpd;

						double dis = R * timeStep;
						if (newX >= 0 && newX < mapRowCount && newY >= 0 && newY < mapColCount &&
							dis * gridWidth + fireMap[i][j].firedArea >= gridWidth * gridWidth &&
							fireMap[newX][newY].state == stateCell::dead) {
							fireMap[newX][newY].state = stateCell::newfire;

							double tempArea = dis * gridWidth + fireMap[i][j].firedArea - gridWidth * gridWidth;
							if (tempArea > gridWidth * gridWidth * 1.0) {
								fireMap[newX][newY].firedArea = gridWidth * gridWidth;
								int tempX = newX + dx[k];
								int tempY = newY + dy[k];
								tempArea -= (gridWidth * gridWidth);
								while (tempArea > 0 && tempX >=0 && tempX < mapRowCount && tempY >= 0 && tempY < mapColCount &&
									fireMap[tempX][tempY].state == stateCell::dead) {
									fireMap[tempX][tempY].state = stateCell::newfire;
									if (tempArea - gridWidth * gridWidth > 0) {
										fireMap[tempX][tempY].firedArea = gridWidth * gridWidth;
									}
									else {
										fireMap[tempX][tempY].firedArea = tempArea;
									}

									tempArea -= (gridWidth * gridWidth);
									tempX += dx[k];
									tempY += dy[k];
								}
							}
							else {
								fireMap[newX][newY].firedArea = tempArea;
							}
						}

						addFireArea += dis * gridWidth;

					}
					fireMap[i][j].state = stateCell::fired;
					fireMap[i][j].firedArea += addFireArea;
					fireMap[i][j].firedArea = std::min(fireMap[i][j].firedArea, gridWidth * gridWidth * 1.0);
				}
			}
		}

		for (int i = 0; i < mapRowCount; i++) {
			for (int j = 0; j < mapColCount; j++) {
				if (fireMap[i][j].state == stateCell::newfire ||
					fireMap[i][j].state == stateCell::fired) {
					fireMap[i][j].state = stateCell::fire;
				}
			}
		}
		QVector<QPointF> leftPointsVec;
		for (int i = 0; i < mapRowCount; i++) {
			for (int j = 0; j < mapColCount; j++) {
				if (fireMap[i][j].state == stateCell::fire) {
					leftPointsVec.push_back(fireMap[i][j].position);
					break;
				}
			}
		}
		QVector<QPointF> rightPointsVec;
		for (int i = mapRowCount - 1; i >= 0; i--) {
			for (int j = mapColCount - 1; j >= 0; j--) {
				if (fireMap[i][j].state == stateCell::fire && !leftPointsVec.contains(fireMap[i][j].position)) {
					leftPointsVec.push_back(fireMap[i][j].position);
					break;
				}
			}
		}

		outQueue.push_back(leftPointsVec);
	}

	return true;
}

bool CellularAutomata::spreadFire(const QVector<int> &weatherConditionVec, std::vector<std::vector<double>>& outDltVec)
{
	if (weatherConditionVec.size() < 4)
		return false;

	QQueue<std::vector<Point>> pointsQueue;

	double T = weatherConditionVec[0];
	double V = 1;
	double h = weatherConditionVec[1];
	double R0 = (0.03*T + 0.05*V + 0.01*h - 0.3) / 60;
	double Ks = 1.0;
	int idx = dx[weatherConditionVec[2]];
	int idy = dy[weatherConditionVec[2]];
	double Kw = KwMap[weatherConditionVec[3]];
	double pd = 0;
	int zs = 3.533*std::pow(std::tan(pd), 1.2);
	double Kpd = std::pow(10, zs);
	double R = R0 * Ks * Kw * Kpd;

	setMapSize(R*fireSpreadTime * 2 + fFireInfo.l * 3, R*fireSpreadTime * 2 + fFireInfo.l * 3);
	initMap();
	outDltVec.clear();
	outDltVec = std::vector<std::vector<double>>(mapRowCount, std::vector<double>(mapColCount, 0.0));

	int num = fireSpreadTime / timeStep;
	while (num--) {
		for (int i = 0; i < mapRowCount; i++) {
			for (int j = 0; j < mapColCount; j++) {
				if (fireMap[i][j].state == stateCell::fire) {
					double addFireArea = 0;
					for (int k = 0; k < 8; k++) {
						int newX = i + dx[k];
						int newY = j + dy[k];

						Kw = KwMap[weatherConditionVec[3]];

						if (dx[k] == idx) {
							if (idy == 0) {
								if (dy[k] == 0) Kw = KwMap[weatherConditionVec[3]];
								else Kw = Kw - weatherConditionVec[3] * 0.1;
							}
							else {
								if (dy[k] == idy) Kw = KwMap[weatherConditionVec[3]];
								else if (dy[k] == -1 * idy) Kw = Kw - weatherConditionVec[3] * 0.2;
								else if (dy[k] == 0) Kw = Kw - weatherConditionVec[3] * 0.1;
							}
						}
						else if (dy[k] == idy) {
							if (idx == 0) {
								if (dx[k] == 0) Kw = KwMap[weatherConditionVec[3]];
								else Kw = Kw - weatherConditionVec[3] * 0.1;
							}
							else {
								if (dx[k] == idx) Kw = KwMap[weatherConditionVec[3]];
								else if (dx[k] == -1 * idx) Kw = Kw - weatherConditionVec[3] * 0.2;
								else if (dx[k] == 0) Kw = Kw - weatherConditionVec[3] * 0.1;
							}
						}
						else if ((dx[k] + idx) == 0 && (dy[k] + idy) == 0) {
							Kw = 1 / Kw;
						}
						else {
							Kw = 1.0;
						}
						R = R0 * Ks * Kw * Kpd;

						double dis = R * timeStep;
						if (newX >= 0 && newX < mapRowCount && newY >= 0 && newY < mapColCount &&
							dis * gridWidth + fireMap[i][j].firedArea >= gridWidth * gridWidth &&
							fireMap[newX][newY].state == stateCell::dead) {
							fireMap[newX][newY].state = stateCell::newfire;

							double tempArea = dis * gridWidth + fireMap[i][j].firedArea - gridWidth * gridWidth;
							if (tempArea > gridWidth * gridWidth * 1.0) {
								fireMap[newX][newY].firedArea = gridWidth * gridWidth;
								int tempX = newX + dx[k];
								int tempY = newY + dy[k];
								tempArea -= (gridWidth * gridWidth);
								while (tempArea > 0 && tempX >= 0 && tempX < mapRowCount && tempY >= 0 && tempY < mapColCount &&
									fireMap[tempX][tempY].state == stateCell::dead) {
									fireMap[tempX][tempY].state = stateCell::newfire;
									if (tempArea - gridWidth * gridWidth > 0) {
										fireMap[tempX][tempY].firedArea = gridWidth * gridWidth;
									}
									else {
										fireMap[tempX][tempY].firedArea = tempArea;
									}

									tempArea -= (gridWidth * gridWidth);
									tempX += dx[k];
									tempY += dy[k];
								}
							}
							else {
								fireMap[newX][newY].firedArea = tempArea;
							}
						}

						addFireArea += dis * gridWidth;

					}
					fireMap[i][j].state = stateCell::fired;
					fireMap[i][j].firedArea += addFireArea;
					fireMap[i][j].firedArea = std::min(fireMap[i][j].firedArea, gridWidth * gridWidth * 1.0);
				}
			}
		}

		for (int i = 0; i < mapRowCount; i++) {
			for (int j = 0; j < mapColCount; j++) {
				if (fireMap[i][j].state == stateCell::newfire || 
					fireMap[i][j].state == stateCell::fired) {
					fireMap[i][j].state = stateCell::fire;
				}
			}
		}

		std::vector<Point> leftPointsVec;
		for (int i = 0; i < mapRowCount; i++) {
			for (int j = 0; j < mapColCount; j++) {
				if (fireMap[i][j].state == stateCell::fire) {
					leftPointsVec.push_back(Point(i,j));
					break;
				}
			}
		}

		for (int i = mapRowCount - 1; i >= 0; i--) {
			for (int j = mapColCount - 1; j >= 0; j--) {
				if (fireMap[i][j].state == stateCell::fire) {
					auto it = std::find_if(leftPointsVec.begin(), leftPointsVec.end(), [&](const Point& poi) { return (poi.x == i) && poi.y == j; });
					if (it == leftPointsVec.end()) {
						leftPointsVec.push_back(Point(i, j));
						break;
					}
				}
			}
		}

		pointsQueue.push_back(leftPointsVec);
	}

	// 填充
	for (int i = 0; i < pointsQueue.size(); ++i) {
		fillPolygon(pointsQueue.at(i), outDltVec, pointsQueue.size() - i);
	}

	geneLayer = pointsQueue.size();

	return true;
}

int CellularAutomata::spreadLayer()
{
	return geneLayer;
}
