#include "ly_sprayprescriptionuntil.h"
#include <QPolygonF>
#include "QCString.h"
#include <QUuid>
#include <osgEarth/GeoData>
#include <random>
#include <qmath.h>

QString LY_SprayPrescriptionUntil::CropTypeToString(CropType cropType)
{
    switch (cropType)
    {
    case CropType::CROP_TYPE_NONE: return tr2("全部");
    case CropType::CROP_TYPE_1: return  tr2("玉米");
    case CropType::CROP_TYPE_2: return  tr2("小麦");
    case CropType::CROP_TYPE_3: return  tr2("水稻");
    default: return  tr2("未知");
    }
}

QString LY_SprayPrescriptionUntil::DataCollectTypeToString(DataCollectType collectType)
{
    switch (collectType)
    {
    case DataCollectType::DATA_COLLECT_UNKNOW: return tr2("全部");
    case DataCollectType::DATA_COLLECT_ARTIFICIAL: return  tr2("人工现场调查");
    case DataCollectType::DATA_COLLECT_SENSOR: return  tr2("传感器图像识别");
    case DataCollectType::DATA_COLLECT_IMAGE: return  tr2("图片文件识别");
    case DataCollectType::DATA_COLLECT_IMPORT: return  tr2("数据文件导入");
    default: return  tr2("未知");
    }
}
QString LY_SprayPrescriptionUntil::PestDiseaseToString(PestDisease pestDisease)
{
	switch (pestDisease)
	{
	case PestDisease::PEST_DISEASE_NONE: return tr2("全部");
	case PestDisease::PEST_DISEASE_1: return  tr2("虫害");
	case PestDisease::PEST_DISEASE_2: return  tr2("病害");
	case PestDisease::PEST_DISEASE_3: return  tr2("病虫害");
	default: return  tr2("未知");
	}
}

QString LY_SprayPrescriptionUntil::PestLevelToString(PestLevel category)
{
	switch (category)
	{
	case PestLevel::PEST_LEVEL_NONE: return tr2("0-无害");
	case PestLevel::PEST_LEVEL_LEVEL_1: return  tr2("1-轻微");
	case PestLevel::PEST_LEVEL_LEVEL_2: return  tr2("2-轻度");
	case PestLevel::PEST_LEVEL_LEVEL_3: return  tr2("3-中度");
	case PestLevel::PEST_LEVEL_LEVEL_4: return  tr2("4-较严重");
	case PestLevel::PEST_LEVEL_LEVEL_5: return  tr2("5-严重");
	case PestLevel::PEST_LEVEL_LEVEL_6: return  tr2("6-非常严重");
	case PestLevel::PEST_LEVEL_LEVEL_7: return  tr2("7-极严重");
	case PestLevel::PEST_LEVEL_LEVEL_8: return  tr2("8-危险");
	case PestLevel::PEST_LEVEL_LEVEL_9: return  tr2("9-极危险");
	case PestLevel::PEST_LEVEL_LEVEL_10: return  tr2("10-灾难");
	default: return  tr2("未知");
	}
}

QString LY_SprayPrescriptionUntil::PestCategoryToString(PestCategory stage)
{
	switch (stage)
	{
	case PestCategory::PEST_CATEGORY_NONE: return tr2("全部");
	case PestCategory::PEST_CATEGORY_1: return  tr2("蚜虫");
	case PestCategory::PEST_CATEGORY_2: return  tr2("红蜘蛛");
	case PestCategory::PEST_CATEGORY_3: return  tr2("穗蚜");
	case PestCategory::PEST_CATEGORY_4: return  tr2("吸浆虫");
	default: return  tr2("未知");
	}
}

QString LY_SprayPrescriptionUntil::PestStageToString(PestStage stage)
{
	switch (stage)
	{
	case PestStage::PEST_STAGE_NONE: return tr2("未知");
	case PestStage::PEST_STAGE_1: return  tr2("卵");
	case PestStage::PEST_STAGE_2: return  tr2("幼虫");
	case PestStage::PEST_STAGE_3: return  tr2("成虫");
	default: return  tr2("未知");
	}
}
QString LY_SprayPrescriptionUntil::WorkStatueToString(WorkType stage)
{

	switch (stage)
	{
	case WorkType::WOKR_STAGE_NONE: return tr2("未知");
	case WorkType::WOKR_STAGE_1: return  tr2("无人机");
	case WorkType::WOKR_STAGE_2: return  tr2("无人机");
	case WorkType::WOKR_STAGE_3: return  tr2("无人机");
	default: return  tr2("未知");
	}
}


QString LY_SprayPrescriptionUntil::GenerateUuid()
{
	const QByteArray &ba = QUuid::createUuid().toRfc4122();
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
	return uid.toString(QUuid::WithoutBraces);
}
/* 指定多边形内生成n个随机点 */
std::vector<QPointF> LY_SprayPrescriptionUntil::GenerateRandomPointsInPolygon(const std::vector<QPointF>& polygon, int n)
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
bool LY_SprayPrescriptionUntil::JudgePointInZone(const QPointF& point, const std::vector<QPointF>& zonePoints)
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
bool LY_SprayPrescriptionUntil::CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area)
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
