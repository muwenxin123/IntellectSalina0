#include "LY_AgriSPrayPrescriptionUntil.h"
#include <QPolygonF>
#include "QCString.h"
#include <QUuid>
#include <osgEarth/GeoData>
#include <random>
#include <qmath.h>

// 将一维 int 向量转换为逗号分隔的字符串
QString LY_AgriSPrayPrescriptionUntil::vectorToString(const std::vector<int>& vec) {
	QString result;
	for (size_t i = 0; i < vec.size(); ++i) {
		if (i > 0) {
			result += ",";
		}
		result += QString::number(vec[i]);
	}
	return result;
}

// 将二维 int 向量转换为字符串，子向量用分号分隔
QString LY_AgriSPrayPrescriptionUntil::vector2DToString(const std::vector<std::vector<int>>& vec2D) {
	QString result;
	for (size_t i = 0; i < vec2D.size(); ++i) {
		if (i > 0) {
			result += ";";
		}
		result += vectorToString(vec2D[i]);
	}
	return result;
}

// 将一维 double 向量转换为逗号分隔的字符串
QString LY_AgriSPrayPrescriptionUntil::vectorDoubleToString(const std::vector<double>& vec) {
	QString result;
	for (size_t i = 0; i < vec.size(); ++i) {
		if (i > 0) {
			result += ",";
		}
		result += QString::number(vec[i]);
	}
	return result;
}

// 将逗号分隔的字符串转换为一维 int 向量
std::vector<int> LY_AgriSPrayPrescriptionUntil::stringToVector(const QString& str) {
	std::vector<int> vec;
	QStringList list = str.split(',');
	for (const QString& item : list) {
		if (!item.isEmpty()) {
			vec.push_back(item.toInt());
		}
	}
	return vec;
}

// 将字符串转换为二维 int 向量
std::vector<std::vector<int>> LY_AgriSPrayPrescriptionUntil::stringToVector2D(const QString& str) {
	std::vector<std::vector<int>> vec2D;
	QStringList subLists = str.split(';');
	for (const QString& subList : subLists) {
		if (!subList.isEmpty()) {
			vec2D.push_back(stringToVector(subList));
		}
	}
	return vec2D;
}

// 将逗号分隔的字符串转换为一维 double 向量
std::vector<double> LY_AgriSPrayPrescriptionUntil::stringToVectorDouble(const QString& str) {
	std::vector<double> vec;
	QStringList list = str.split(',');
	for (const QString& item : list) {
		if (!item.isEmpty()) {
			vec.push_back(item.toDouble());
		}
	}
	return vec;
}
osgEarth::Symbology::Color LY_AgriSPrayPrescriptionUntil::hexToColor(const std::string& hexColor, float rgb_a /*= 1.0f*/)
{
	{
		unsigned int r, g, b;
		if (sscanf(hexColor.c_str(), "#%02X%02X%02X", &r, &g, &b) == 3)
		{
			return osgEarth::Symbology::Color(r / 255.0f, g / 255.0f, b / 255.0f, rgb_a); // 透明度默认为 1.0
		}
		return osgEarth::Symbology::Color(); // 失败返回默认颜色
	}
}
double LY_AgriSPrayPrescriptionUntil::getElevation(double lon, double lat)
{
	//根据经纬度获取高度
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	osgEarth::ElevationQuery eq(mapNode->getMap());
	osgEarth::GeoPoint point(mapNode->getMap()->getSRS(), lon, lat);

	double elevation = 0.0;
	if (eq.getElevation(point, elevation)) {
		//qDebug() << "Elevation at (" << currentPoi.lat << ", " << currentPoi.lon
		//	<< ") is " << elevation << " meters.";
		return elevation;
	}
	else {
		elevation = 150;
	}
	return elevation;
}
// 将角度转换为弧度
double LY_AgriSPrayPrescriptionUntil::toRadians(double degrees) {
	return degrees * M_PI / 180.0;
}

// 将弧度转换为角度
double LY_AgriSPrayPrescriptionUntil::toDegrees(double radians) {
	return radians * 180.0 / M_PI;
}

// 计算目标点的经纬度和高程
void LY_AgriSPrayPrescriptionUntil::calculateNewPosition(double lat0, double lon0, double h0, double distance,
	double azimuth, double elevation, double& lat1, double& lon1, double& h1) {
	// 转换为弧度
	double lat0_rad = toRadians(lat0);
	double lon0_rad = toRadians(lon0);
	double azimuth_rad = toRadians(azimuth);
	double elevation_rad = toRadians(elevation);

	// 计算起点在地心坐标系中的位置
	double x0 = (R + h0) * cos(lat0_rad) * cos(lon0_rad);
	double y0 = (R + h0) * cos(lat0_rad) * sin(lon0_rad);
	double z0 = (R + h0) * sin(lat0_rad);

	// 计算目标点相对起点在局部坐标系中的偏移
	double n = distance * cos(elevation_rad) * cos(azimuth_rad); // 北方向分量
	double e = distance * cos(elevation_rad) * sin(azimuth_rad); // 东方向分量
	double u = distance * sin(elevation_rad);                   // 垂直方向分量

																// 将局部坐标系偏移转换为地心坐标系
	double dx = -sin(lon0_rad) * e - cos(lon0_rad) * sin(lat0_rad) * n + cos(lon0_rad) * cos(lat0_rad) * u;
	double dy = cos(lon0_rad) * e - sin(lon0_rad) * sin(lat0_rad) * n + sin(lon0_rad) * cos(lat0_rad) * u;
	double dz = cos(lat0_rad) * n + sin(lat0_rad) * u;

	// 计算目标点在地心坐标系中的位置
	double x1 = x0 + dx;
	double y1 = y0 + dy;
	double z1 = z0 + dz;

	// 计算目标点的经纬度和高程
	double r = sqrt(x1*x1 + y1*y1 + z1*z1);
	lat1 = toDegrees(asin(z1 / r));
	lon1 = toDegrees(atan2(y1, x1));
	h1 = r - R;
}

QString LY_AgriSPrayPrescriptionUntil::GenerateUuid()
{
	const QByteArray &ba = QUuid::createUuid().toRfc4122();
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
	return uid.toString(QUuid::WithoutBraces);
}
/* 指定多边形内生成n个随机点 */
std::vector<QPointF> LY_AgriSPrayPrescriptionUntil::GenerateRandomPointsInPolygon(const std::vector<QPointF>& polygon, int n)
{
	std::vector<QPointF> points;
	if (polygon.size() < 3) return points;  // 至少需要 3 个点构成封闭区域

	QPolygonF qPolygon(QVector<QPointF>::fromStdVector(polygon));

	// 获取多边形的边界框
	qreal minX = polygon[0].x(), maxX = polygon[0].x();
	qreal minY = polygon[0].y(), maxY = polygon[0].y();
	for (const auto& p : polygon) {
		minX = std::min(minX, p.x());
		maxX = std::max(maxX, p.x());
		minY = std::min(minY, p.y());
		maxY = std::max(maxY, p.y());
	}

	// 随机数生成器
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<qreal> distX(minX, maxX);
	std::uniform_real_distribution<qreal> distY(minY, maxY);

	// 生成 n 个随机点
	while (points.size() < static_cast<size_t>(n)) {
		QPointF randomPoint(distX(gen), distY(gen));
		if (qPolygon.containsPoint(randomPoint, Qt::OddEvenFill)) {
			points.push_back(randomPoint);
		}
	}

	return points;
}
bool LY_AgriSPrayPrescriptionUntil::JudgePointInZone(const QPointF& point, const std::vector<QPointF>& zonePoints)
{
    bool isInZone = false;

    if (zonePoints.size() > 0) {
        int count = zonePoints.size();
        int nCross = 0;
        for (int i = 0; i < count; i++)
        {
            QPointF p1 = zonePoints[i];
            QPointF p2 = zonePoints[(i + 1) % count];

            if (p1.y() == p2.y())
                continue;

            if (point.y() < std::min(p1.y(), p2.y()))
                continue;

            if (point.y() >= std::max(p1.y(), p2.y()))
                continue;

            double x = (point.y() - p1.y()) * (p2.x() - p1.x()) / (p2.y() - p1.y()) + p1.x();

            if (x > point.x())
                nCross++;
        }

        isInZone = (nCross % 2 == 1);
    }

    return isInZone;
}
const double EARTH_RADIUS = 6378137.0; // 地球半径（单位：米）
bool LY_AgriSPrayPrescriptionUntil::CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area)
{
	// 至少需要3个点形成多边形
	int n = zonePoints.size();
	if (n < 3) return false;

	QPointF origin = zonePoints[0];
	std::vector<osg::Vec3d> utmPoints;
	for (const auto& point : zonePoints)
	{
		osg::Vec3d tmpPoint;

		double dLon = (point.x() - origin.x()) * M_PI / 180.0;
		double dLat = (point.y() - origin.y()) * M_PI / 180.0;

		tmpPoint.x() = EARTH_RADIUS * dLat; // Y 方向
		tmpPoint.y() = EARTH_RADIUS * dLon * cos(origin.y() * M_PI / 180.0); // X 方向

		utmPoints.emplace_back(tmpPoint);
	}

	// 使用 Shoelace 公式计算面积
	double sum = 0.0;
	for (size_t i = 0; i < utmPoints.size(); ++i)
	{
		size_t next = (i + 1) % utmPoints.size();
		sum += (utmPoints[i].x() * utmPoints[next].y()) - (utmPoints[next].x() * utmPoints[i].y());
	}

	area = std::abs(sum) * 0.5;
	return true;
}
std::vector<IUserMesh2*> LY_AgriSPrayPrescriptionUntil::createDetectionZone(float angle_l, float angle_w, float dis_m)
{
	std::vector<IUserMesh2*> reUserMesh2s;
	reUserMesh2s.clear();
	IUserMesh2 *pUM = nullptr;
	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth != nullptr)
	{
		pUM = pEarth->GetGraphics()->CreateUserMesh2();
	}

	int angle_w_count = 1;
	int angle_l_count = 1;

	if (angle_w_count == 0)
		angle_w_count = 1;
	if (angle_l_count == 0)
		angle_l_count = 1;

	float angle_w_dis = angle_w / angle_w_count;
	float angle_l_dis = angle_l / angle_l_count;

	std::vector<unsigned short> pIndex_up;
	std::vector<unsigned short> pIndex_down;
	std::vector<unsigned short> pIndex_left;
	std::vector<unsigned short> pIndex_r;
	std::vector<std::vector<unsigned short>> pIndex_cir;
	pIndex_up.clear();
	pIndex_down.clear();
	pIndex_left.clear();
	pIndex_r.clear();
	pIndex_cir.clear();

	pIndex_up.push_back(0);
	pIndex_down.push_back(0);
	pIndex_left.push_back(0);
	pIndex_r.push_back(0);

	VERTEX *pVerter = (VERTEX*)malloc(((angle_l_count + 1) * (angle_w_count + 1) + 1) * sizeof(VERTEX));
	if (pVerter != nullptr)
	{
		int pVIndex = 0;
		float angle_w_cur = -(angle_w / 2);
		pVerter[pVIndex].x = 0;
		pVerter[pVIndex].y = 0;
		pVerter[pVIndex].z = 0;
		pVIndex++;
		const double PI = 3.14159265358979323846;
		for (int angle_w_index = 0; angle_w_index <= angle_w_count; angle_w_index++)
		{
			double angle_w_cur_pi = angle_w_cur*PI / 180;
			angle_w_cur += angle_w_dis;
			double z = dis_m * sin(angle_w_cur_pi);
			double dis_z = dis_m * cos(angle_w_cur_pi);
			float angle_l_cur = -(angle_l / 2);

			for (int angle_l_index = 0; angle_l_index <= angle_l_count; angle_l_index++)
			{
				double angle_l_cur_pi = angle_l_cur * PI / 180;
				angle_l_cur += angle_l_dis;
				double x = dis_z * sin(angle_l_cur_pi);
				double y = dis_z * cos(angle_l_cur_pi);

				pVerter[pVIndex].x = x;
				pVerter[pVIndex].y = y;
				pVerter[pVIndex].z = z;

				if (angle_w_index == 0)
				{
					pIndex_up.push_back(pVIndex);
				}
				else if (angle_w_index == angle_w_count)
				{
					pIndex_down.push_back(pVIndex);
				}

				if (angle_l_index == 0)
				{
					pIndex_left.push_back(pVIndex);
				}
				else if (angle_l_index == angle_l_count)
				{
					pIndex_r.push_back(pVIndex);
				}

				pVIndex++;
			}
		}

		int startIndex = 1;
		int index_added = angle_l_count + 1;
		for (int wIndex = 0; wIndex < angle_w_count; wIndex++)
		{
			int startWIndex = startIndex + wIndex * index_added;
			std::vector<unsigned short> tempPindex_cir;
			tempPindex_cir.clear();
			tempPindex_cir.push_back(startWIndex);
			tempPindex_cir.push_back(startWIndex + index_added);
			for (int lIndex = 1; lIndex < index_added; lIndex++)
			{
				tempPindex_cir.push_back(startWIndex + lIndex);
				tempPindex_cir.push_back(startWIndex + lIndex + index_added);
			}
			pIndex_cir.push_back(tempPindex_cir);
		}

		pUM->SetPoints(pVIndex, pVerter);
		pUM->SetIndexs(pIndex_up.size(), &pIndex_up[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_down.size(), &pIndex_down[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_left.size(), &pIndex_left[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_r.size(), &pIndex_r[0], IUserMesh::USERMESH_POLYGON, false);
		for (int iindex = 0; iindex < pIndex_cir.size(); iindex++)
		{
			if (iindex != pIndex_cir.size() - 1)
			{
				pUM->SetIndexs(pIndex_cir[iindex].size(), &pIndex_cir[iindex][0], IUserMesh::USERMESH_QUAD_STRIP, false);
			}
			else
			{
				pUM->SetIndexs(pIndex_cir[iindex].size(), &pIndex_cir[iindex][0], IUserMesh::USERMESH_QUAD_STRIP);
			}
		}

		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_FILL);
		pUM->SetState(IUserMesh::LINEWIDTH, 3);
		//pUM->SetState(GL_DEPTH_TEST, 1);
		reUserMesh2s.push_back(pUM);

		pUM = pEarth->GetGraphics()->CreateUserMesh2();
		pUM->SetPoints(pVIndex, pVerter);
		pUM->SetIndexs(pIndex_up.size(), &pIndex_up[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_down.size(), &pIndex_down[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_left.size(), &pIndex_left[0], IUserMesh::USERMESH_POLYGON, false);
		pUM->SetIndexs(pIndex_r.size(), &pIndex_r[0], IUserMesh::USERMESH_POLYGON);
		free(pVerter);
		pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
		pUM->SetState(IUserMesh::LINEWIDTH, 2);
		pUM->SetState(IUserMesh::LIGHT, 1);
		//pUM->SetState(GL_DEPTH_TEST, 1);
		reUserMesh2s.push_back(pUM);

	}
	return reUserMesh2s;
}