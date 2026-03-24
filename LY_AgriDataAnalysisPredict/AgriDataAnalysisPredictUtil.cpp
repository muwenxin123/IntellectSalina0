#include <QUuid>
#include <QPolygonF>
#include <QPainterPath>
#include <random>
#include <osgEarth/GeoData>
#include <qmath.h>

#include "AgriDataAnalysisPredictUtil.h"
#include "QCString.h"


QString AgriDataAnalysisPredictUtil::CropTypeToString(CropType cropType)
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

QString AgriDataAnalysisPredictUtil::DataCollectTypeToString(DataCollectType collectType)
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

QString AgriDataAnalysisPredictUtil::PestDiseaseToString(PestDisease pestDisease)
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

QString AgriDataAnalysisPredictUtil::PestLevelToString(PestLevel category)
{
	switch (category) 
	{
	case PestLevel::PEST_LEVEL_NONE: return tr2("0-无害");
	case PestLevel::PEST_LEVEL_1: return  tr2("1-轻微");
	case PestLevel::PEST_LEVEL_2: return  tr2("2-轻度");
	case PestLevel::PEST_LEVEL_3: return  tr2("3-中度");
	case PestLevel::PEST_LEVEL_4: return  tr2("4-较严重");
	case PestLevel::PEST_LEVEL_5: return  tr2("5-严重");
	case PestLevel::PEST_LEVEL_6: return  tr2("6-非常严重");
	case PestLevel::PEST_LEVEL_7: return  tr2("7-极严重");
	case PestLevel::PEST_LEVEL_8: return  tr2("8-危险");
	case PestLevel::PEST_LEVEL_9: return  tr2("9-极危险");
	case PestLevel::PEST_LEVEL_10: return  tr2("10-灾难");
	default: return  tr2("未知");
	}
}

QString AgriDataAnalysisPredictUtil::PestCategoryToString(PestCategory stage)
{
	switch (stage) 
	{
	case PestCategory::PEST_CATEGORY_NONE: return tr2("全部");
	case PestCategory::PEST_CATEGORY_1: return  tr2("玉米黏虫");
	case PestCategory::PEST_CATEGORY_2: return  tr2("红蜘蛛");
	case PestCategory::PEST_CATEGORY_3: return  tr2("穗蚜");
	case PestCategory::PEST_CATEGORY_4: return  tr2("吸浆虫");
	case PestCategory::PEST_CATEGORY_5: return  tr2("蚜虫");
	case PestCategory::PEST_CATEGORY_6: return  tr2("棉铃虫");
	case PestCategory::PEST_CATEGORY_7: return  tr2("甜菜夜蛾");
	case PestCategory::PEST_CATEGORY_8: return  tr2("斜纹夜蛾");
	case PestCategory::PEST_CATEGORY_9: return  tr2("玉米螟");
	case PestCategory::PEST_CATEGORY_10: return  tr2("粘虫");
	case PestCategory::PEST_CATEGORY_11: return  tr2("草地贪夜蛾");
	case PestCategory::PEST_CATEGORY_12: return  tr2("二点委夜蛾");
	case PestCategory::PEST_CATEGORY_13: return  tr2("蓟马");
	case PestCategory::PEST_CATEGORY_14: return  tr2("地老虎");
	case PestCategory::PEST_CATEGORY_15: return  tr2("玉米向日葵粉蚧");
	case PestCategory::PEST_CATEGORY_16: return  tr2("金龟子");
	case PestCategory::PEST_CATEGORY_17: return  tr2("东亚飞蝗");
	case PestCategory::PEST_CATEGORY_18: return  tr2("中华蚱蜢");
	case PestCategory::PEST_CATEGORY_19: return  tr2("稻绿蝽");
	default: return  tr2("未知");
	}
}

QString AgriDataAnalysisPredictUtil::PestStageToString(PestStage stage)
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

QString AgriDataAnalysisPredictUtil::GridSizeToString(GridSize size)
{
	switch (size)
	{
	case GridSize::GRID_SIZE_1: return  tr2("1米");
	case GridSize::GRID_SIZE_2: return  tr2("2米");
	case GridSize::GRID_SIZE_3: return  tr2("3米");
	case GridSize::GRID_SIZE_4: return  tr2("4米");
	case GridSize::GRID_SIZE_5: return  tr2("5米");
	default: return  tr2("未知");
	}
}

QString AgriDataAnalysisPredictUtil::DiffusionLayerToString(DiffusionLayer layer)
{
	switch (layer)
	{
	case DiffusionLayer::DIFFUSION_LAYER_1: return  tr2("1层");
	case DiffusionLayer::DIFFUSION_LAYER_2: return  tr2("2层");
	case DiffusionLayer::DIFFUSION_LAYER_3: return  tr2("5层");
	case DiffusionLayer::DIFFUSION_LAYER_4: return  tr2("10层");
	default: return  tr2("未知");
	}
}

DiffusionLayer AgriDataAnalysisPredictUtil::GetDiffusionLayer(int layer)
{
	if (layer == 1){
		return DiffusionLayer::DIFFUSION_LAYER_1;
	}
	else if (layer == 2){
		return DiffusionLayer::DIFFUSION_LAYER_2;
	}
	else if (layer == 5){
		return DiffusionLayer::DIFFUSION_LAYER_3;
	}
	else if (layer == 10){
		return DiffusionLayer::DIFFUSION_LAYER_4;
	}
	return DiffusionLayer::DIFFUSION_LAYER_1;
}

QString AgriDataAnalysisPredictUtil::GeneTypeToString(GenerateType geneType)
{
	switch (geneType)
	{
	case GenerateType::GENERATE_EQUAL: return  tr2("等距扩散");
	case GenerateType::GENERATE_DYNAMIC: return  tr2("动态蔓延");
	default: return  tr2("未知");
	}
}

QString AgriDataAnalysisPredictUtil::WindDirectToString(WindDirection windDirect)
{
	switch (windDirect)
	{
	case WindDirection::EAST: return  tr2("东");
	case WindDirection::SOUTH: return  tr2("南");
	case WindDirection::WEST: return  tr2("西");
	case WindDirection::NORTH: return  tr2("北");
	case WindDirection::EAST_SOUTH: return  tr2("东南");
	case WindDirection::EAST_NORTH: return  tr2("东北");
	case WindDirection::WEST_SOUTH: return  tr2("西南");
	case WindDirection::WEST_NORTH: return  tr2("西北");
	default: return  tr2("未知");
	}
}

QString AgriDataAnalysisPredictUtil::GenerateUuid()
{
	const QByteArray &ba = QUuid::createUuid().toRfc4122();
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
	return uid.toString(QUuid::WithoutBraces);
}

bool AgriDataAnalysisPredictUtil::CalcPolygonCenterPoint(const std::vector<QPointF>& zonePoints, QPointF& point)
{
	int n = zonePoints.size();
	if (n < 3) {
		//std::cerr << "错误：多边形至少需要 3 个点！" << std::endl;
		return false;
	}

	double area = 0.0;
	double Cx = 0.0, Cy = 0.0;

	for (int i = 0; i < n; i++) {
		int j = (i + 1) % n;  // 下一个点，首尾相连
		double cross = zonePoints[i].x() * zonePoints[j].y() - zonePoints[j].x() * zonePoints[i].y();
		area += cross;
		Cx += (zonePoints[i].x() + zonePoints[j].x()) * cross;
		Cy += (zonePoints[i].y() + zonePoints[j].y()) * cross;
	}

	area *= 0.5;
	if (area == 0) {
		//std::cerr << "错误：多边形面积为 0，可能是退化情况！" << std::endl;
		return false;
	}

	Cx /= (6.0 * area);
	Cy /= (6.0 * area);

	point.setX(Cx);
	point.setY(Cy);

	return true;
}

bool AgriDataAnalysisPredictUtil::JudgePointInZone_OddEven(const QPointF& point, const std::vector<QPointF>& zonePoints)
{
	QPolygonF qPolygon = QPolygonF(QVector<QPointF>::fromStdVector(zonePoints));
	return qPolygon.containsPoint(point, Qt::OddEvenFill);  // 使用奇偶规则
}

bool AgriDataAnalysisPredictUtil::JudgePointInZone_Ray(const QPointF& point, const std::vector<QPointF>& zonePoints)
{
	QPointF p = point;

	int count = 0;
	int n = zonePoints.size();
	for (int i = 0; i < n; i++) {
		QPointF p1 = zonePoints[i];
		QPointF p2 = zonePoints[(i + 1) % n];

		// 确保p1在p2之下（y较小）
		if (p1.y() > p2.y()) std::swap(p1, p2);

		// 检查是否在水平线上
		if (p.y() == p1.y() || p.y() == p2.y()) {
			p.ry() += 0.0001; // 处理特殊情况，微调y坐标
		}

		// 如果点在p1和p2的y范围内，并且在射线的左侧，则交点计数+1
		if (p.y() > p1.y() && p.y() <= p2.y() &&
			(p.x() < (p2.x() - p1.x()) * (p.y() - p1.y()) / (p2.y() - p1.y()) + p1.x())) {
			count++;
		}
	}
	return count % 2 == 1;  // 奇数次相交则在多边形内
}

bool AgriDataAnalysisPredictUtil::JudgePointInZone_Complex(const QPointF& point, const std::vector<QPointF>& zonePoints)
{
	QPainterPath path;
	if (zonePoints.empty()) return false;

	path.moveTo(zonePoints[0]);
	for (size_t i = 1; i < zonePoints.size(); ++i) {
		path.lineTo(zonePoints[i]);
	}
	path.closeSubpath();  // 形成封闭区域

	return path.contains(point);
}

std::vector<QPointF> AgriDataAnalysisPredictUtil::GenerateRandomPointsInPolygon(const std::vector<QPointF>& polygon, int n)
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

// 近似转换为米的比例（适用于小区域）
const double EARTH_RADIUS = 6378137.0; // 地球半径（单位：米）

bool AgriDataAnalysisPredictUtil::CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area)
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

double AgriDataAnalysisPredictUtil::metersToLatitudeDegrees(double meters/* = 1.0*/)
{
	return (meters / (PI * R_POLAR)) * 180.0;
}

double AgriDataAnalysisPredictUtil::metersToLongitudeDegrees(double latitude, double meters/* = 1.0*/)
{
	double latRad = latitude * PI / 180.0;  // 转换为弧度
	double radiusAtLatitude = R_EQUATOR * cos(latRad);
	return (meters / (PI * radiusAtLatitude)) * 180.0;
}

// **计算交集**
QPolygonF AgriDataAnalysisPredictUtil::calculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2)
{
	QPolygonF intersectionPolygon;

	intersectionPolygon = polygon1.intersected(polygon2);
	if (!intersectionPolygon.isEmpty()) {
		// qDebug() << "交集区域点数：" << intersectionPolygon.size();
	}
	else {
		// qDebug() << "无交集";
	}
	return intersectionPolygon;
}
