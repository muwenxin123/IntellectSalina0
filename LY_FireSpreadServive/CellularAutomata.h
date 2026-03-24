#pragma once

#include <QMap>
#include <QString>
#include <QVector>
#include <QQueue>
#include <QPointF>
#include "QPainter"

using namespace std;

typedef enum {
	fire = 1,
	fired = 2,
	dead = 3,
	newfire = 4
} stateCell;

struct InfoCell {
	stateCell state;
	QPointF position;
	double firedArea;

	InfoCell() {
		state = stateCell::dead; firedArea = 0;
	}
	InfoCell(stateCell a, QPointF b, double c) :
		state(a), position(b), firedArea(c){}
};

struct FirstFireInfo {
	double lon;
	double lat;
	double l;
	double w;
	FirstFireInfo(){}
	FirstFireInfo(double a, double b, double c, double d):
		lon(a), lat(b), l(c), w(d) {}

};

class CellularAutomata
{
public:
	CellularAutomata();
	~CellularAutomata();

	void setMapSize(double l, double w);
	void setInitFireInfo(double lon, double lat, double l, double w);
	void setSpreadTime(int time_sec);
	void setTimeStep(int time_sec);
	QQueue<QVector<QPointF>> spreadFire(const QVector<int> &weatherConditionVec);

private:
	void initMap();

public:
	QQueue<QVector<QPointF>> outQueue;

private:
	FirstFireInfo fFireInfo;
	int fireSpreadTime;
	QVector<QVector<InfoCell>> fireMap;
	int mapRowCount;
	int mapColCount;
	QVector<QVector<double>> FiredArea;
	double gridWidth = 20;
	const int dx[8] = { -1, 1, 0, 0, -1, -1, 1, 1 };
	const int dy[8] = { 0, 0, -1, 1, -1, 1, -1, 1 };
	QMap<int, double> KwMap;
	QMap<QString, int> dirMap;
	int timeStep = 600;
};

