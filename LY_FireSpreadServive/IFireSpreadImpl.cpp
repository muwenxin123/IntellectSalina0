#include "IFireSpreadImpl.h"
#include "LY_FireSpreadServive.h"
#include "QGeoCoordinate"

#define PI			3.1415926
#define NPrecision	10000
using namespace qnzkna::FireSpread;

IFireSpreadImpl::IFireSpreadImpl(void)
{
	m_CellularAutomata = new CellularAutomata;
}

IFireSpreadImpl::~IFireSpreadImpl(void)
{

}

void qnzkna::FireSpread::IFireSpreadImpl::getFireStateMapQueue()
{

}

void qnzkna::FireSpread::IFireSpreadImpl::setFireStateMapSize(double l, double w)
{
	if (m_CellularAutomata) {
		m_CellularAutomata->setMapSize(l, w);
	}
}

void qnzkna::FireSpread::IFireSpreadImpl::setInitFireInfo(double lon, double lat, double l, double w)
{
	if (m_CellularAutomata) {
		m_CellularAutomata->setInitFireInfo(lon, lat, l, w);
	}
}

void qnzkna::FireSpread::IFireSpreadImpl::setSpreadTime(int time_sec)
{
	if (m_CellularAutomata) {
		m_CellularAutomata->setSpreadTime(time_sec);
	}
}

void qnzkna::FireSpread::IFireSpreadImpl::setTimeStep(int time_sec)
{
	if (m_CellularAutomata) {
		m_CellularAutomata->setTimeStep(time_sec);
	}
}

QQueue<QVector<QPointF>> qnzkna::FireSpread::IFireSpreadImpl::spreadFire(const QVector<int> &weatherConditionVec)
{
	QQueue<QVector<QPointF>> out;
	if (m_CellularAutomata) {
		out = m_CellularAutomata->spreadFire(weatherConditionVec);
	}
	return out;
}

void qnzkna::FireSpread::IFireSpreadImpl::DrawDynamic(QPainter *painter)
{
	painter->save();
	painter->setRenderHint(QPainter::Antialiasing, true);

	int queueSize = m_FireSpreadQueue.size();
	if (queueSize > 0) {
		QBrush brush;
		brush.setStyle(Qt::BrushStyle::SolidPattern);

		painter->setBrush(brush);
		painter->setPen(Qt::NoPen);
		int tou = 255;
		int red = 255;
		int green = 0;
		QPolygonF prePolygon;
		for (auto iter = m_FireSpreadQueue.begin(); iter != m_FireSpreadQueue.end(); iter++) {
			brush.setColor(QColor(red, green, 0, tou));
			tou -= (255 - 20) / queueSize;
			red -= (255 - 100) / queueSize;
			green += 150 / queueSize;
			painter->setBrush(brush);
			auto poiVec = *iter;
			QVector<QPointF> points;
			for (const auto& poi : poiVec) {
				double x1, y1;
				float l1, b1;
				LY_FireSpreadServive::Get().L_X(poi.x(), poi.y(), &x1, &y1);
				LY_FireSpreadServive::Get().X_S(x1, y1, &l1, &b1);
				points.push_back(QPointF(l1, b1));
			}

			QPolygonF polygon(points);
			QPolygonF subPolygon = polygon.subtracted(prePolygon);

			painter->drawPolygon(subPolygon);

			prePolygon = polygon;
		}
		double lon = 120.9602501, lat = 23.7910968;
		QGeoCoordinate cenPoi(lat, lon);
		QGeoCoordinate ltPoi = cenPoi.atDistanceAndAzimuth(250, 307);
		double x1, y1;
		float l1, b1;
		LY_FireSpreadServive::Get().L_X(ltPoi.longitude(), ltPoi.latitude(), &x1, &y1);
		LY_FireSpreadServive::Get().X_S(x1, y1, &l1, &b1);
		double x2, y2;
		float l2, b2;
		LY_FireSpreadServive::Get().L_X(cenPoi.longitude(), cenPoi.latitude(), &x2, &y2);
		LY_FireSpreadServive::Get().X_S(x2, y2, &l2, &b2);
		double chang = std::sqrt((l1 - l2)*(l1 - l2) + (b1 - b2)*(b1 - b2)) * 4.0 / 5 * 2;
		double kuan = std::sqrt((l1 - l2)*(l1 - l2) + (b1 - b2)*(b1 - b2)) * 3.0 / 5 * 2;
		brush.setColor(QColor(255, 0, 0));
		painter->setBrush(brush);
		QPen curPen;
		curPen.setColor(QColor(Qt::yellow));
		painter->setPen(curPen);

	}

	painter->restore();
}

void qnzkna::FireSpread::IFireSpreadImpl::setFireSpreadQueue(QQueue<QVector<QPointF>> FireSpreadQueue)
{
	m_FireSpreadQueue = FireSpreadQueue;
}

void qnzkna::FireSpread::IFireSpreadImpl::clearFireSpreadQueue()
{
	m_FireSpreadQueue.clear();
}

