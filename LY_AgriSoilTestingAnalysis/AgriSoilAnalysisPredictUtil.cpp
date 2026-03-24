#include <random>
#include <qmath.h>
#include <QUuid>
#include <QDebug>
#include <QPolygonF>
#include <QPainterPath>
#include <osgEarth/GeoData>

#include "AgriSoilAnalysisPredictUtil.h"
#include "QCString.h"
#include "clipper/clipper.hpp"

using namespace ClipperLib;


QString AgriSoilAnalysisPredictUtil::CropTypeToString(CropType cropType)
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

QString AgriSoilAnalysisPredictUtil::DataCollectTypeToString(DataCollectType collectType)
{
	switch (collectType)
	{
	case DataCollectType::DATA_COLLECT_UNKNOW: return tr2("未知");
	case DataCollectType::DATA_COLLECT_ARTIFICIAL: return  tr2("人工采集");
	case DataCollectType::DATA_COLLECT_SENSOR: return  tr2("传感器采集");
	case DataCollectType::DATA_COLLECT_NDVI: return  tr2("植被指数反演");
	case DataCollectType::DATA_COLLECT_MIX: return  tr2("混合采集");
	default: return  tr2("未知");
	}
}


QString AgriSoilAnalysisPredictUtil::DiffusionLayerToString(DiffusionLayer layer)
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

DiffusionLayer AgriSoilAnalysisPredictUtil::GetDiffusionLayer(int layer)
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


QString AgriSoilAnalysisPredictUtil::NitrogenTypeToString(NitrogenType eleNType)
{
	switch (eleNType)
	{
	case NitrogenType::QUAN_DAN: return  tr2("全氮");
	case NitrogenType::YOU_XIAO_DAN: return  tr2("有效氮");
	case NitrogenType::SU_XIAO_DAN: return  tr2("速效氮");
	case NitrogenType::AN_TAI_DAN: return  tr2("氨态氮");
	case NitrogenType::XIAO_TAI_DAN: return  tr2("硝态氮");
	case NitrogenType::SHUI_JIE_DAN: return  tr2("水解氮");
	case NitrogenType::JIAN_JIE_DAN: return  tr2("碱解氮");
	case NitrogenType::XIAN_AN_TAI_DAN: return  tr2("酰胺态氮");
	default: return  tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::PhosphorusTypeToString(PhosphorusType elePType)
{
	switch (elePType)
	{
	case PhosphorusType::QUAN_LIN: return  tr2("全磷");
	case PhosphorusType::YOU_XIAO_LIN: return  tr2("有效磷");
	case PhosphorusType::SU_XIAO_LIN: return  tr2("速效磷");
	case PhosphorusType::HUAN_XIAO_LIN: return  tr2("缓效磷");
	default: return  tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::PotassiumTypeToString(PotassiumType eleKType)
{
	switch (eleKType)
	{
	case PotassiumType::QUAN_JIA: return  tr2("全钾");
	case PotassiumType::YOU_XIAO_JIA: return  tr2("有效钾");
	case PotassiumType::SU_XIAO_JIA: return  tr2("速效钾");
	case PotassiumType::HUAN_XIAO_JIA: return  tr2("缓效钾");
	default: return  tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::SoilTypeToString(SoilType soilType)
{
	switch (soilType)
	{
	case SoilType::TIE_LV_TU:           return tr2("铁铝土");
	case SoilType::LIN_RONG_TU:         return tr2("淋溶土");
	case SoilType::BAN_LIN_RONG_TU:     return tr2("半淋溶土");
	case SoilType::GAI_TU_CENG:         return tr2("钙土层");
	case SoilType::GAN_HAN_TU:          return tr2("干旱土");
	case SoilType::MO_TU:               return tr2("漠土");
	case SoilType::CHU_YU_TU:           return tr2("初育土");
	case SoilType::BAN_SHUI_CHENG_TU:   return tr2("半水成土");
	case SoilType::SHUI_CHENG_TU:       return tr2("水成土");
	case SoilType::YAN_JIAN_TU:         return tr2("盐碱土");
	case SoilType::REN_WEI_TU:          return tr2("人为土");
	case SoilType::GAO_SHAN_TU:         return tr2("高山土");
	case SoilType::UNKNOWN:					   
	default:                            return tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::SoilColorToString(SoilColor soilColor)
{
	switch (soilColor)
	{
	case SoilColor::COLOR_HEI:         return tr2("黑");
	case SoilColor::COLOR_ANLI:        return tr2("暗栗");
	case SoilColor::COLOR_ANZONG:      return tr2("暗棕");
	case SoilColor::COLOR_ANHUI:       return tr2("暗灰");
	case SoilColor::COLOR_LI:          return tr2("栗");
	case SoilColor::COLOR_ZONG:        return tr2("棕");
	case SoilColor::COLOR_HUI:         return tr2("灰");
	case SoilColor::COLOR_HONGZONG:    return tr2("红棕");
	case SoilColor::COLOR_HUANGZONG:   return tr2("黄棕");
	case SoilColor::COLOR_QIANZONG:    return tr2("浅棕");
	case SoilColor::COLOR_HONG:        return tr2("红");
	case SoilColor::COLOR_CHENG:       return tr2("橙");
	case SoilColor::COLOR_HUANG:       return tr2("黄");
	case SoilColor::COLOR_QIANHUANG:   return tr2("浅黄");
	case SoilColor::COLOR_BAI:         return tr2("白");
	case SoilColor::COLOR_UNKNOWN:
	default:                           return tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::SoilTextureToString(SoilTexture soilTexture)
{
	switch (soilTexture)
	{
	case SoilTexture::SHA_TU:                      return tr2("砂土");
	case SoilTexture::RANG_ZHI_SHA_TU:             return tr2("壤质砂土");
	case SoilTexture::SHA_ZHI_RANG_TU:             return tr2("砂质壤土");
	case SoilTexture::RANG_TU:                     return tr2("壤土");
	case SoilTexture::FEN_SHA_ZHI_RANG_TU:         return tr2("粉砂质壤土");
	case SoilTexture::SHA_ZHI_NIAN_RANG_TU:        return tr2("砂质黏壤土");
	case SoilTexture::NIAN_RANG_TU:                return tr2("黏壤土");
	case SoilTexture::FEN_SHA_ZHI_NIAN_RANG_TU:    return tr2("粉砂质黏壤土");
	case SoilTexture::SHA_ZHI_NIAN_TU:             return tr2("砂质黏土");
	case SoilTexture::RANG_ZHI_NIAN_TU:            return tr2("壤质黏土");
	case SoilTexture::FEN_SHA_ZHI_NIAN_TU:         return tr2("粉砂质黏土");
	case SoilTexture::NIAN_TU:                     return tr2("黏土");
	case SoilTexture::ZHONG_NIAN_TU:               return tr2("重黏土");
	case SoilTexture::UNKNOWN:
	default:                                       return tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::DistributeMapTypeToString(DistributeMapType disMapType)
{
	switch (disMapType)
	{
	case DistributeMapType::ELE_NUTRIENT: return  tr2("养分元素图");
	case DistributeMapType::ELE_MINOR: return  tr2("微量元素图");
	case DistributeMapType::ELE_HEAVY_METAL: return  tr2("土壤污染图");
	case DistributeMapType::MOIS: return  tr2("物理特征图");
	case DistributeMapType::SOIL_FERTILITY: return  tr2("肥力分布图");
	default: return  tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::SampleTypeToString(SampleType sampleType)
{
	switch (sampleType)
	{
	case SampleType::CELL_SAMPLING: return  tr2("单元采样");
	case SampleType::POINT_SAMPLING: return  tr2("点采样");
	case SampleType::AREA_SAMPLING: return  tr2("区域采样");
	default: return  tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::SampleModeToString(SampleType sampleType, int sampleMode)
{
	switch (sampleType)
	{
	case SampleType::CELL_SAMPLING:
	{
		switch (static_cast<CellSample>(sampleMode))
		{
		case CellSample::RANDOM: return  tr2("随机采样");
		case CellSample::SYSTEMATICAL_STAGGERED: return  tr2("交错采样");
		case CellSample::S_SHAPED: return  tr2("S形采样");
		case CellSample::PLUM_BLOSSOM: return  tr2("梅花形采样");
		default:
			break;
		}
	}
	case SampleType::POINT_SAMPLING: 
	{
		switch (static_cast<PointSample>(sampleMode))
		{
		case PointSample::UNIFORM_GRID: return  tr2("均匀网格");
		case PointSample::OFFSET_GRID: return  tr2("偏移网格");
		case PointSample::STRATIFIED_GRID: return  tr2("层状网格");
		case PointSample::RECTANGULAR_GRID: return  tr2("矩形网格");
		default:
			break;
		}
	}
	case SampleType::AREA_SAMPLING:
	{
		switch (static_cast<AreaSample>(sampleMode))
		{
		case AreaSample::AREA: return  tr2("区域采样");
		}
	}

	default: return  tr2("未知");
	}
}

QString AgriSoilAnalysisPredictUtil::GenerateUuid()
{
	const QByteArray &ba = QUuid::createUuid().toRfc4122();
	QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
	return uid.toString(QUuid::WithoutBraces);
}

bool AgriSoilAnalysisPredictUtil::CalcPolygonCenterPoint(const std::vector<QPointF>& zonePoints, QPointF& point)
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

bool AgriSoilAnalysisPredictUtil::JudgePointInZone_OddEven(const QPointF& point, const std::vector<QPointF>& zonePoints)
{
	QPolygonF qPolygon = QPolygonF(QVector<QPointF>::fromStdVector(zonePoints));
	return qPolygon.containsPoint(point, Qt::OddEvenFill);  // 使用奇偶规则
}

bool AgriSoilAnalysisPredictUtil::JudgePointInZone_Ray(const QPointF& point, const std::vector<QPointF>& zonePoints)
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

bool AgriSoilAnalysisPredictUtil::JudgePointInZone_Complex(const QPointF& point, const std::vector<QPointF>& zonePoints)
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

std::vector<QPointF> AgriSoilAnalysisPredictUtil::GenerateRandomPointsInPolygon(const std::vector<QPointF>& polygon, int n, SamplingMode mode /*= SamplingMode::Random*/)
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
	std::uniform_real_distribution<qreal> noise(-5.0, 5.0);  // 用于 S 型和对角线模式的偏移

	if (SamplingMode::PlumBlossom == mode)
	{
		qreal centerX = (maxX + minX) / 2.0;
		qreal centerY = (maxY + minY) / 2.0;
		qreal offset = std::min((maxX - minX), (maxY - minY)) / 2 * 0.8; // 梅花分布的偏移量

		points.emplace_back(QPointF(centerX - offset, centerY + offset));
		points.emplace_back(QPointF(centerX + offset, centerY + offset));
		points.emplace_back(QPointF(centerX, centerY));
		points.emplace_back(QPointF(centerX - offset, centerY - offset));
		points.emplace_back(QPointF(centerX + offset, centerY - offset));
	}
	else
	{
		while (points.size() < static_cast<size_t>(n)) {
			QPointF randomPoint;

			switch (mode) {
			case SamplingMode::Random:
				randomPoint = QPointF(distX(gen), distY(gen));
				break;

			case SamplingMode::Diagonal:
			{
				qreal centerX = (minX + maxX) / 2;  // 经度中心
				qreal centerY = (minY + maxY) / 2;  // 纬度中心
				qreal C = centerY - centerX;        // 计算 C，使 y = x + C 在区域中心

				qreal amplitude = (maxY - minY) * 0.2;  // 振幅 (纬度范围的 20%)
				qreal offsetMax = (maxX - minX) * 0.05; // 交错偏移量 (5% 的经度范围)

				qreal x = distX(gen);  // 随机选取经度

				qreal yOffset = amplitude * std::sin((x - minX) / (maxX - minX) * M_PI); // 波动
				qreal xOffset = (gen() % 2 == 0) ? offsetMax : -offsetMax; // 交错偏移

				qreal y = x + C + yOffset; // y 轴扰动
				x += xOffset; // x 轴左右交错

				randomPoint = QPointF(x, y);
			}
			break;

			case SamplingMode::SShape:
			{
				qreal t = static_cast<qreal>(points.size()) / n;  // 归一化参数 [0,1]
				qreal y = minY + (maxY - minY) * t;  // 沿 X 轴均匀分布
				qreal amplitude = (maxX - minX) * 0.4; // 40% 的高度作为振幅，避免超出边界
				qreal offsetX = (maxX + minX) / 2.0;     // S 形围绕中间位置
				qreal x = offsetX + amplitude * std::sin(t * M_PI * 2); // S 形曲线

				randomPoint = QPointF(x, y);
			}
			default:
				break;
			}

			// 确保点位于多边形内
			if (qPolygon.containsPoint(randomPoint, Qt::OddEvenFill)) {
				points.push_back(randomPoint);
			}
		}
	}

	return points;
}

// 近似转换为米的比例（适用于小区域）
const double EARTH_RADIUS = 6378137.0; // 地球半径（单位：米）

bool AgriSoilAnalysisPredictUtil::CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area)
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

double AgriSoilAnalysisPredictUtil::metersToLatitudeDegrees(double meters/* = 1.0*/)
{
	return (meters / (PI * R_POLAR)) * 180.0;
}

double AgriSoilAnalysisPredictUtil::metersToLongitudeDegrees(double latitude, double meters/* = 1.0*/)
{
	double latRad = latitude * PI / 180.0;  // 转换为弧度
	double radiusAtLatitude = R_EQUATOR * cos(latRad);
	return (meters / (PI * radiusAtLatitude)) * 180.0;
}

SoilFertility AgriSoilAnalysisPredictUtil::evaluateFertility(int nitrogen, int phosphorus, int potassium)
{
	if (nitrogen < 150 && phosphorus < 15 && potassium < 60) {
		return SoilFertility::Extremely_Low;
	}
	else if ((nitrogen >= 150 && nitrogen < 200) && phosphorus < 25 && (potassium >= 60 && potassium < 130)) {
		return SoilFertility::Low;
	}
	else if ((nitrogen >= 200 && nitrogen < 250) && (phosphorus >= 25 && phosphorus < 40) && (potassium >= 130 && potassium < 200)) {
		return SoilFertility::Medium;
	}
	else if ((nitrogen >= 250 && nitrogen < 300) && (phosphorus >= 40 && phosphorus < 45) && (potassium >= 200 && potassium < 230)) {
		return SoilFertility::High;
	}
	else if (nitrogen > 300 && phosphorus > 45 && potassium > 230) {
		return SoilFertility::Extremely_High;
	}
	else {
		return SoilFertility::Undefined;
	}
}

SoilFertility AgriSoilAnalysisPredictUtil::evaluateFertility_SingleN(int nitrogen)
{
	if (nitrogen < 150) {
		return SoilFertility::Extremely_Low;
	}
	else if ((nitrogen >= 150 && nitrogen < 200)) {
		return SoilFertility::Low;
	}
	else if ((nitrogen >= 200 && nitrogen < 250)) {
		return SoilFertility::Medium;
	}
	else if ((nitrogen >= 250 && nitrogen < 300)) {
		return SoilFertility::High;
	}
	else if (nitrogen > 300) {
		return SoilFertility::Extremely_High;
	}
	else {
		return SoilFertility::Undefined;
	}
}

SoilFertility AgriSoilAnalysisPredictUtil::evaluateFertility_SingleP(int phosphorus)
{
	if (phosphorus < 15) {
		return SoilFertility::Extremely_Low;
	}
	else if (phosphorus < 25) {
		return SoilFertility::Low;
	}
	else if (phosphorus >= 25 && phosphorus < 40) {
		return SoilFertility::Medium;
	}
	else if (phosphorus >= 40 && phosphorus < 45) {
		return SoilFertility::High;
	}
	else if (phosphorus > 45) {
		return SoilFertility::Extremely_High;
	}
	else {
		return SoilFertility::Undefined;
	}
}

SoilFertility AgriSoilAnalysisPredictUtil::evaluateFertility_SingleK(int potassium)
{
	if (potassium < 60) {
		return SoilFertility::Extremely_Low;
	}
	else if (potassium >= 60 && potassium < 130) {
		return SoilFertility::Low;
	}
	else if (potassium >= 130 && potassium < 200) {
		return SoilFertility::Medium;
	}
	else if (potassium >= 200 && potassium < 230) {
		return SoilFertility::High;
	}
	else if (potassium > 230) {
		return SoilFertility::Extremely_High;
	}
	else {
		return SoilFertility::Undefined;
	}
}

QString AgriSoilAnalysisPredictUtil::fertilityToString(SoilFertility fertility)
{
	switch (fertility) {
	case SoilFertility::Extremely_Low: return tr2("极低");
	case SoilFertility::Low: return tr2("低");
	case SoilFertility::Medium: return tr2("中");
	case SoilFertility::High: return tr2("高");
	case SoilFertility::Extremely_High: return tr2("极高");
	default: return tr2("无法确定");
	}
}

// **转换 Qt 的 QPolygonF 到 Clipper 多边形**
Paths convertQPolygonFToClipper(const QPolygonF& polygon) {
	Paths clipperPaths(1);
	for (const QPointF& point : polygon) {
		clipperPaths[0].push_back(IntPoint(point.x() * 10e7, point.y() * 10e7)); // Clipper 需要整数坐标，放大10e7倍
	}
	return clipperPaths;
}

// **计算交集**
QPolygonF AgriSoilAnalysisPredictUtil::calculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2)
{
	QPolygonF intersectionPolygon;

#if 1

	intersectionPolygon = polygon1.intersected(polygon2);
	if (!intersectionPolygon.isEmpty()) {
		// qDebug() << "交集区域点数：" << intersectionPolygon.size();
	}
	else {
		// qDebug() << "无交集";
	}

#else

	// 转换 QPolygonF 到 Clipper
	Paths subject1 = convertQPolygonFToClipper(polygon1);
	Paths subject2 = convertQPolygonFToClipper(polygon2);

	// 计算交集
	Clipper clipper;
	clipper.AddPaths(subject1, ptSubject, true);
	clipper.AddPaths(subject2, ptSubject, true);

	Paths solution;
	clipper.Execute(ctIntersection, solution, pftNonZero, pftNonZero);

	// 转换回 QPolygonF
	if (!solution.empty()) {
		for (const IntPoint& ip : solution[0]) {
			intersectionPolygon.append(QPointF(ip.X / 10e7, ip.Y / 10e7)); // 还原坐标
		}
	}

#endif

	return intersectionPolygon;
}