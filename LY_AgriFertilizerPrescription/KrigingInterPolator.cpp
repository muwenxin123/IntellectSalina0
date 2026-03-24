#include "KrigingInterPolator.h"
#include <algorithm>
#include <cmath>

#include <map3d/IEarth.h>

#include <osgEarthAnnotation/ImageOverlay>
#include <osgEarthSymbology/Geometry>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/ImageLayer>
#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureListSource>
#include <osgEarthFeatures/FeatureModelGraph>
#include <osg/Texture2D>
#include <osgText/Text>
#include <osgWidget/Box>
#include <osgWidget/Label>
#include <osgWidget/Canvas>
#include <osgWidget/WindowManager>
#include <osgWidget/ViewerEventHandlers>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>

#include <osg/CoordinateSystemNode>

#include <QDebug>
#include <osg/Texture2D>
#include <PtInPloy.h>
#include <AcmGeometry.h>

#include <QGeoCoordinate>
#include <osgEarth\GeoMath>
#include <osg/PolygonOffset>


using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;

namespace HeatMap3DUtil {

	// 十六进制颜色转 osgEarth::Symbology::Color
	osgEarth::Symbology::Color hexToColor(const std::string& hexColor, float rgb_a = 1.0f);

	QPolygonF calculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2)
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
}



HeatMap3D::HeatMap3D()
{
	_mapNode = nullptr;
	_group = nullptr;
	_group_forD = nullptr;
}

HeatMap3D::HeatMap3D(osgEarth::MapNode * mapNode)
{
	_mapNode = nullptr;
	_group = nullptr;
	_group_forD = nullptr;
	
	_mapNode = mapNode;
	if (_group == nullptr)
	{
		_group = new osg::Group();
		mapNode->addChild(_group);
	}
	if (_group_forD == nullptr)
	{
		_group_forD = new osg::Group();
		mapNode->addChild(_group_forD);
	}
	/*lons = { 116.530001, 116.5311125, 116.52152545, 116.54, 116.5352545, 116.54 };
	lats = { 40.399002, 40.39585451, 40.375254, 40.395245525, 40.372545, 40.39 };
	values = { 4,	6,		8,		3,		4,	9 };
	points_bound.push_back(osg::Vec3d(116.52, 40.35, 0));
	points_bound.push_back(osg::Vec3d(116.55, 40.40, 0));
	points_bound.push_back(osg::Vec3d(116.53, 40.40, 0));
	points_bound.push_back(osg::Vec3d(116.52, 40.44, 0));*/
	//points_bound.push_back(osg::Vec3d(116.52, 40.35, 0));

}

HeatMap3D::~HeatMap3D()
{
}

void HeatMap3D::setMapNode(osgEarth::MapNode * mapNode)
{
	_mapNode = mapNode;
	if (_group == nullptr)
	{
		_group = new osg::Group();
		mapNode->addChild(_group);
	}
	if (_group_forD == nullptr)
	{
		_group_forD = new osg::Group();
		mapNode->addChild(_group_forD);
	}
	
}


void HeatMap3D::setBoundPoints(const std::vector<QPointF>& boundPoings)
{
	points_bound.clear();
	for (int i = 0; i < boundPoings.size(); i++)
	{
		points_bound.push_back(osg::Vec3d(boundPoings[i].x(), boundPoings[i].y(), 0.0));
	}
}

void HeatMap3D::setLonLatValue(const std::vector<double>& _lons, const std::vector<double> _lats, const std::vector<double> _values)
{
	lons.clear();
	lats.clear();
	values.clear();

	lons = _lons;
	lats = _lats;
	values = _values;

	/*std::copy(_lons.begin(), _lons.end(), lons.begin());
	std::copy(_lats.begin(), _lats.end(), lats.begin());
	std::copy(_values.begin(), _values.end(), values.begin());*/

}

void HeatMap3D::setPixelSpacing(double _spacing)
{
	spacing = _spacing;
}

void HeatMap3D::setEqualCount(int _equalCount)
{
	equalCount = _equalCount;
}

void HeatMap3D::setDisCount(int _disCount)
{
	disCount = _disCount;
}

void HeatMap3D::setImageShowOrHide(bool isShow)
{
	if (_group != nullptr)
	{
		_group->setNodeMask(isShow ? 1 : 0);
	}
}

osg::HeightField * HeatMap3D::createHeightField(const std::vector<std::vector<double>>& grid, double minX, double minY, double spacing)
{
	osg::HeightField* hf = new osg::HeightField();
	size_t rows = grid.size();
	size_t cols = grid[0].size();
	hf->allocate(cols, rows);

	for (size_t i = 0; i < rows; ++i) {
		for (size_t j = 0; j < cols; ++j) {
			hf->setHeight(j, i, grid[i][j]);
		}
	}

	hf->setOrigin(osg::Vec3(minX, minY, 0.0));
	hf->setXInterval(spacing);
	hf->setYInterval(spacing);
	return hf;
}


std::vector<std::vector<double>>  HeatMap3D::drawPestSprayPrescriptionMap(std::vector<std::vector<double>>& grid, double old_space, double new_space)
{

	double H_in = grid.size();
	double W_in = grid[0].size();

	
	//double space_space_multiple_lon = spaceLon / new_spaceLon;
	//double space_space_multiple_lat = spaceLat / new_spaceLat;

	//int space_lon = m_old_space / space_space_multiple_lon;
	//int space_lat = m_old_space / space_space_multiple_lat;


	//double space_multiple = old_space / new_space;
	//int space_d = m_old_space / space_multiple;

	//int H_out = (int)(H_in * space_lat) + 1;
	//int W_out = (int)(W_in * space_lon) + 1;

	double space_multiple = old_space / new_space;
	int space_d = m_old_space / space_multiple;

	int H_out = (int)(H_in * space_multiple) + 1;
	int W_out = (int)(W_in * space_multiple) + 1;

	std::vector<std::vector<double>> output(H_out, std::vector<double>(W_out, 0));

	// 遍历每个输出位置
	for (int i_out = 0; i_out < H_out; ++i_out) {
		for (int j_out = 0; j_out < W_out; ++j_out) {
			int i_start = i_out * space_d;
			int j_start = j_out * space_d;

			float max_val = -INFINITY; // 初始化为最小浮点数

			 // 遍历池化窗口
			for (int i = i_start; i < i_start + space_d && i <grid.size(); ++i) {
				for (int j = j_start; j < j_start + space_d && j < grid[0].size(); ++j) {
					if (grid[i][j] > max_val) {
						max_val = grid[i][j];
					}
				}
			}
			output[i_out][j_out] = max_val;
		}
	}
	return output;
}

void HeatMap3D::setSpace(double space)
{
	new_space = space * (1.0 / 110000.0);
}
/*
diffuse_layer: disCount
equalCount: 0

grid_unit: spacing
*/
void HeatMap3D::assignValuesWithLayers(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats,
	const std::vector<double>& values, int equalCount, int disCount, double minLon, double minLat, double spacing, double angel, bool isDispose)
{
	int layer = equalCount + disCount;
	int maxDis = sqrt(pow(layer, 2) + pow(layer, 2));
	QGeoCoordinate minGeo(minLat, minLon);
	double disSpacing = spacing * 110000.0;

	for (size_t idx = 0; idx < lons.size() && idx < lats.size() && idx < values.size(); ++idx) {

		//int y = (lons[idx] - minLon) / spaceLon;
		//int x = (lats[idx] - minLat) / spaceLat;

		double tempLon = 0.0;
		double tempLat = 0.0;
		int y = 0;
		int x = 0;

		double realDistance = osgEarth::GeoMath::distance(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
			osg::DegreesToRadians(lats[idx]), osg::DegreesToRadians(lons[idx]));
		double realBearing = osgEarth::GeoMath::bearing(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
			osg::DegreesToRadians(lats[idx]), osg::DegreesToRadians(lons[idx]));
		double disBearingRad = realBearing - angel;
		y = int((sin(disBearingRad) * realDistance) / disSpacing);
		x = int((cos(disBearingRad) * realDistance) / disSpacing);

		double value = values[idx];

		{
			for (int i = x - layer; i <= x + layer; ++i) {
				for (int j = y - layer; j <= y + layer; ++j) {
					if (i >= 0 && i < grid.size() && j >= 0 && j < grid[0].size()) 
					{
						if (isDispose)	//处方图
						{
							if (abs(x - i) <= equalCount && abs(y - j) <= equalCount)
							{
								grid[i][j] = grid[i][j] > value ? grid[i][j] : value;
							}
						}
						else
						{
							if (abs(x - i) <= equalCount && abs(y - j) <= equalCount)
							{
								grid[i][j] = value;
							}
							else
							{
								int dis = sqrt(pow(y - j, 2) + pow(x - i, 2));
								double weight = 1.0 - static_cast<double>(dis) / static_cast<double>(maxDis);
								grid[i][j] += value * weight; // 相交区域相加

							}
						}
					}
				}
			}
		}
	}
}



void HeatMap3D::initStyles_forDisPosition()
{
	styles_forD.clear();
	for (int i = 0; i < colors_Sym_forD.size(); i++)
	{
		osgEarth::Symbology::Style* geomStyle = new osgEarth::Symbology::Style;
		//geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = osgEarth::Color::Black;
		//geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0.0;

		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = colors_Sym_forD[i];
		//geomStyle.get()->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;

		geomStyle->getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset().mutable_value().enabled() = true;

#if 1
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_NONE;
		//geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 513.0;	//相对地面高度
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 123.0;	//相对地面高度
		//geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalScale() = 150.0;	//相对地面高度
#else
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symboloy::gAltitudeSymbol::CLAMP_TO_TERRAIN;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 0.5;
#endif

		styles_forD.push_back(geomStyle);
	}
}
void HeatMap3D::setOrigins(const std::vector<double> origins)
{
	_origins = origins;
}
void HeatMap3D::createGridGeometry(std::vector<std::vector<double>>& grid, double minLon, double minLat, double spacing,
	std::vector<osgEarth::Features::FeatureList>& featureLists, double angel)
{

	QPolygonF polygonBounds;
	for (auto point : points_bound) {
		DPOINT polypt(point.x(), point.y());
		polygonBounds << QPointF(point.x(), point.y());
	}

	osg::Vec3d origin(minLon, minLat, 0);

	osgEarth::Symbology::Geometry* geoMery = osgEarth::Symbology::Geometry::create(osgEarth::Symbology::Geometry::TYPE_POLYGON, &points_bound);
	osgEarth::Symbology::Polygon * polygon = dynamic_cast<osgEarth::Symbology::Polygon*>(geoMery);

	double minHeight = 0, maxHeight = values[0];

	//airInstructionPoints.clear();

	for (int i = 1; i < values.size(); i++)
	{
		minHeight = minHeight < values[i] ? minHeight : values[i];
		maxHeight = maxHeight > values[i] ? maxHeight : values[i];
	}
	osg::EllipsoidModel* tempElisMode = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
	osgEarth::GeoPoint minlonlat(_mapNode->getMap()->getSRS(), minLon, minLat);
	double disSpacing = spacing * 110000.0;
	for (int x = 0; x < grid.size(); x++)
	{
		for (int y = 0; y < grid[0].size(); y++)
		{

			osg::Vec3d v1, v2, v3, v4;

			{
				double v1LonR, v1LatR;
				double Tempdistance = sqrt(x * x + y * y) * disSpacing;
				double bearingRad = 0.0;
				if (x == 0)
				{
					bearingRad = angel + osg::DegreesToRadians(90.0);
				}
				else
				{
					bearingRad = atan((y * 1.0) / (x * 1.0)) + angel;
				}
				osgEarth::GeoMath::destination(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
					bearingRad, Tempdistance, v1LatR, v1LonR);

				v1.set(osg::Vec3d(osg::RadiansToDegrees(v1LonR), osg::RadiansToDegrees(v1LatR), 0.0));
		/*		if (!(polygon ->contains2D(v1.x(), v1.y())))
				{
					continue;
				}*/
				double v2LonR, v2LatR, v3LonR, v3LatR, v4LonR, v4LatR;
				osgEarth::GeoMath::destination(v1LatR, v1LonR, angel, disSpacing, v2LatR, v2LonR);
				osgEarth::GeoMath::destination(v1LatR, v1LonR, angel + osg::DegreesToRadians(45.0), disSpacing * sqrt(2), v3LatR, v3LonR);
				osgEarth::GeoMath::destination(v1LatR, v1LonR, angel + osg::DegreesToRadians(90.0), disSpacing, v4LatR, v4LonR);

				v2.set(osg::Vec3d(osg::RadiansToDegrees(v2LonR), osg::RadiansToDegrees(v2LatR), 0.0));
				v3.set(osg::Vec3d(osg::RadiansToDegrees(v3LonR), osg::RadiansToDegrees(v3LatR), 0.0));
				v4.set(osg::Vec3d(osg::RadiansToDegrees(v4LonR), osg::RadiansToDegrees(v4LatR), 0.0));

			}
	

			osgEarth::Polygon* boundPolygon = new osgEarth::Polygon();
			//boundPolygon->push_back(v1);
			//boundPolygon->push_back(v2);
			//boundPolygon->push_back(v3);
			//boundPolygon->push_back(v4);

			// 区域交集处理
			QPolygonF polygonRect;
			polygonRect << QPointF(v1.x(), v1.y());
			polygonRect << QPointF(v2.x(), v2.y());
			polygonRect << QPointF(v3.x(), v3.y());
			polygonRect << QPointF(v4.x(), v4.y());

			QPolygonF intersection = HeatMap3DUtil::calculateIntersection(polygonBounds, polygonRect);
			if (intersection.isEmpty()) {
				continue;
			}

			for (const auto& pointF : intersection) {
				boundPolygon->push_back(osg::Vec3d(pointF.x(), pointF.y(), 0));
			}


			osg::ref_ptr<osgEarth::Annotation::Feature> feature = new osgEarth::Annotation::Feature(boundPolygon, _mapNode->getMapSRS()->getGeographicSRS());
			

			double height = grid[x][y];
			if (height < 0)
			{
				continue;
			}
			if (height > 0)
			{
				int a = 0; 
				int b = a;
			}
			//添加颜色
			int colorIndex = (int)((height - minHeight) / (maxHeight - minHeight) * (styles_forD.size() - 1));
			
			//double cenLon = (2.0 * y + 1.0) * new_spaceLon / 2.0 + minLon;
			//double cenLat = (2.0 * x + 1.0) * new_spaceLat / 2.0 + minLat;

			double CenLonR, CenLatR;
			double bearingRad = atan(((y * 1.0) + 0.5) / ((x * 1.0) + 0.5)) + angel;
			double Tempdistance1 = sqrt(((x * 1.0) + 0.5) * ((x * 1.0) + 0.5) + ((y * 1.0) + 0.5) * ((y * 1.0) + 0.5)) * disSpacing;
			//destination()函数内的单位都为弧度制，需要从度°转为弧度
			osgEarth::GeoMath::destination(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
				bearingRad, Tempdistance1, CenLatR, CenLonR);
			double centerLon = osg::RadiansToDegrees(CenLonR);
			double centerlat = osg::RadiansToDegrees(CenLatR);

			if (colorIndex != 0)
			{
				qDebug() << "*******===:" << colorIndex;
				int a = 0;
				int b = a;
				AgriSoilTestingFertilityPoint ptr;
				ptr.lon = centerLon;
				ptr.lat = centerlat;
				ptr.fertility = colorIndex;
				gridVec.push_back(ptr);
			}
			featureLists[colorIndex].push_back(feature);
			if (height > 0)
			{

				keyCounts[height]++;
			}
			
			mapInfoMap[height] = polygon;
		}
	}
	qDebug() << "*******===:" << keyCounts;
}

void HeatMap3D::create2DGridDrawing(QPainter& painter, IProj *pProj)

{
	if (m_grid.size() <= 0 || points_bound.size() <= 0)
	{
		return;
	}
	if (m_grid[0].size() <= 0)
	{
		return;
	}

	// 计算最小/最大高度值
	double minHeight = 0, maxHeight = m_grid[0][0];
	for (const auto& row : m_grid) {
		for (double val : row) {
			if (val < 0) continue;
			minHeight = std::min(minHeight, val);
			maxHeight = std::max(maxHeight, val);
		}
	}

	// 准备边界多边形
	std::vector<DPOINT> polypts;
	for (auto point : points_bound) {
		DPOINT polypt;
		polypt.x = point.x();
		polypt.y = point.y();
		polypts.push_back(polypt);
	}

	double disSpacing = new_space * 110000.0;  // 网格间距对应的米数
	double gridAngle = osg::DegreesToRadians(10.0);

	for (int x = 0; x < m_grid.size(); x++) {
		for (int y = 0; y < m_grid[0].size(); y++) {


			// 计算网格中心点地理坐标
			double centerLon, centerLat;
			if (gridAngle == 0.0) {
				// 无旋转时的简化计算
				centerLon = m_minLon + new_space * (y + 0.5);
				centerLat = m_minLat + new_space * (x + 0.5);
			}
			else {

				double CenLonR, CenLatR;
				double bearingRad = atan(((y * 1.0) + 0.5) / ((x * 1.0) + 0.5)) + gridAngle;
				double Tempdistance1 = sqrt(((x * 1.0) + 0.5) * ((x * 1.0) + 0.5) + ((y * 1.0) + 0.5) * ((y * 1.0) + 0.5)) * disSpacing;
				//destination()函数内的单位都为弧度制，需要从度°转为弧度
				osgEarth::GeoMath::destination(osg::DegreesToRadians(m_minLat), osg::DegreesToRadians(m_minLon),
					bearingRad, Tempdistance1, CenLatR, CenLonR);
				centerLon = osg::RadiansToDegrees(CenLonR);
				centerLat = osg::RadiansToDegrees(CenLatR);
			}

			//检查点是否在边界内
			DPOINT centerPoint;
			centerPoint.x = centerLon;
			centerPoint.y = centerLat;
			if (!insidepolygon(polypts.size(), polypts.data(), centerPoint) == 0) {
				continue; // 点不在多边形内，跳过
			}

// 			double height = m_grid[x][y];
// 			if (height < 0)
// 			{
// 				continue;
// 			}
// 			if (height > 0)
// 			{
// 				int a = 0;
// 				int b = a;
// 			}
			// 计算颜色索引
			//int colorIndex = (int)((height - minHeight) / (maxHeight - minHeight) * (styles_forD.size() - 1));

			// 设置绘制样式
			QBrush brush;
			QColor color = osgColorToQColor(colors_Sym_forD[0]);
			brush.setColor(color);
			brush.setStyle(Qt::SolidPattern);
			painter.setBrush(brush);

			QPen pen;
			pen.setWidth(1);
			pen.setColor(QColor(255, 255, 255, 100));
			painter.setPen(pen);

			// 计算四个顶点（旋转后的网格单元）
			std::vector<QPointF> vertices;
			osg::Vec3d v1, v2, v3, v4;
			{
				double v1LonR, v1LatR;
				double Tempdistance = sqrt(x * x + y * y) * disSpacing;
				double bearingRad = 0.0;
				if (x == 0)
				{
					bearingRad = gridAngle + osg::DegreesToRadians(90.0);
				}
				else
				{
					bearingRad = atan((y * 1.0) / (x * 1.0)) + gridAngle;
				}
				osgEarth::GeoMath::destination(osg::DegreesToRadians(m_minLat), osg::DegreesToRadians(m_minLon),
					bearingRad, Tempdistance, v1LatR, v1LonR);

				v1.set(osg::Vec3d(osg::RadiansToDegrees(v1LonR), osg::RadiansToDegrees(v1LatR), 0.0));
				double v2LonR, v2LatR, v3LonR, v3LatR, v4LonR, v4LatR;
				osgEarth::GeoMath::destination(v1LatR, v1LonR, gridAngle, disSpacing, v2LatR, v2LonR);
				osgEarth::GeoMath::destination(v1LatR, v1LonR, gridAngle + osg::DegreesToRadians(45.0), disSpacing * sqrt(2), v3LatR, v3LonR);
				osgEarth::GeoMath::destination(v1LatR, v1LonR, gridAngle + osg::DegreesToRadians(90.0), disSpacing, v4LatR, v4LonR);

				v2.set(osg::Vec3d(osg::RadiansToDegrees(v2LonR), osg::RadiansToDegrees(v2LatR), 0.0));
				v3.set(osg::Vec3d(osg::RadiansToDegrees(v3LonR), osg::RadiansToDegrees(v3LatR), 0.0));
				v4.set(osg::Vec3d(osg::RadiansToDegrees(v4LonR), osg::RadiansToDegrees(v4LatR), 0.0));
			}
			QPoint screenPoint1 = pProj->Jwd2Screen(v1.x(), v1.y());
			QPoint screenPoint2 = pProj->Jwd2Screen(v2.x(), v2.y());
			QPoint screenPoint3 = pProj->Jwd2Screen(v3.x(), v3.y());
			QPoint screenPoint4 = pProj->Jwd2Screen(v4.x(), v4.y());
			vertices.push_back(QPointF(screenPoint1.x(), screenPoint1.y()));
			vertices.push_back(QPointF(screenPoint2.x(), screenPoint2.y()));
			vertices.push_back(QPointF(screenPoint3.x(), screenPoint3.y()));
			vertices.push_back(QPointF(screenPoint4.x(), screenPoint4.y()));


			// 绘制旋转后的网格单元
			QPolygonF polygon;
			polygon << vertices[0] << vertices[1] << vertices[2] << vertices[3];
			painter.drawPolygon(polygon);
		}
	}
}
std::vector<std::vector<int>> HeatMap3D::groupConsecutiveNumbers(const std::vector<int>& nums, bool isLeft) {
	std::vector<std::vector<int>> result;

	if (nums.empty()) return result;

	std::vector<int> currentGroup = { nums[0] };

	for (size_t i = 1; i < nums.size(); ++i) {
		if (isLeft)
		{
			if (nums[i] == nums[i - 1] + 1) {
				// 当前数字与前一个数字相邻，加入当前组
				currentGroup.push_back(nums[i]);
			}
			else {
				// 不相邻，将当前组加入结果，开始新组
				result.push_back(currentGroup);
				currentGroup = { nums[i] };
			}
		}
		else
		{
			if (nums[i ]  == nums[i - 1] - 1) {
				// 当前数字与前一个数字相邻，加入当前组
				currentGroup.push_back(nums[i]);
			}
			else {
				// 不相邻，将当前组加入结果，开始新组
				result.push_back(currentGroup);
				currentGroup = { nums[i] };
			}
		}
	
	}

	// 添加最后一组
	result.push_back(currentGroup);

	return result;
}


void HeatMap3D::startCreateHeatMap_forDisPosition(double gridAngle)
{
	if (_group_forD == nullptr)
	{
		return;
	}
	while (_group_forD->getNumChildren() > 0)
	{
		_group_forD->removeChild(_group_forD->getChild(0));
	}
	if (lons.size() == 0 || lats.size() == 0 || points_bound.size() == 0)
	{
		return;
	}
	//创建克里金插值器
	//KrigingInterpolator kriging(lons, lats, values);
	double maxLon = *(std::max_element(lons.begin(), lons.end()));
	double minLon = *(std::min_element(lons.begin(), lons.end()));
	double maxLat = *(std::max_element(lats.begin(), lats.end()));
	double minLat = *(std::min_element(lats.begin(), lats.end()));



	//// 计算旋转中心
	//const double centerLon = (minLon + maxLon) / 2.0;
	//const double centerLat = (minLat + maxLat) / 2.0;

	for (int i = 0; i < points_bound.size(); i++)
	{
		maxLon = maxLon > points_bound[i].x() ? maxLon : points_bound[i].x();
		minLon = minLon < points_bound[i].x() ? minLon : points_bound[i].x();
		maxLat = maxLat > points_bound[i].y() ? maxLat : points_bound[i].y();
		minLat = minLat < points_bound[i].y() ? minLat : points_bound[i].y();
	}

	QGeoCoordinate cenGeo((maxLat + minLat) / 2, (maxLon + minLon) / 2);
	QGeoCoordinate x1Geo(minLat, minLon);
	QGeoCoordinate x2Geo(maxLat, minLon);
	QGeoCoordinate x3Geo(maxLat, maxLon);
	double angle_1To3 = x1Geo.azimuthTo(x3Geo);
	int dis = cenGeo.distanceTo(x1Geo) + cenGeo.distanceTo(x2Geo);

	QGeoCoordinate tempGeo = cenGeo.atDistanceAndAzimuth(dis, 180 + osg::RadiansToDegrees(gridAngle) + angle_1To3);

	/*size_t tempcols = static_cast<size_t>((maxLat - minLat) / spacing) + 2;
	size_t temprows = static_cast<size_t>((maxLon - minLon) / spacing) + 2;
	osg::Vec3d origin(minLon, minLat, 0);

	osg::Vec3d v2 = origin + osg::Vec3d(-(0 * spacing * sin(-gridAngle)) + (tempcols * spacing * cos(-gridAngle)),
	(0 * spacing * cos(-gridAngle)) + (tempcols * spacing * sin(-gridAngle)), 0);
	osg::Vec3d v3 = origin + osg::Vec3d(-(temprows * spacing * sin(-gridAngle)) + (tempcols * spacing * cos(-gridAngle)),
	(temprows * spacing * cos(-gridAngle)) + (tempcols * spacing * sin(-gridAngle)), 0);
	osg::Vec3d v4 = origin + osg::Vec3d(-(temprows * spacing * sin(-gridAngle)) + (0 * spacing * cos(-gridAngle)),
	(temprows * spacing * cos(-gridAngle)) + (0 * spacing * sin(-gridAngle)), 0);
	*/

	/*QGeoCoordinate x1Geo(minLat, minLon);
	QGeoCoordinate x2Geo(maxLat, minLon);
	QGeoCoordinate x3Geo(maxLat, maxLon);
	QGeoCoordinate x4Geo(minLat, maxLon);
	QGeoCoordinate x2GeoLU = x1Geo.atDistanceAndAzimuth(x1Geo.distanceTo(x2Geo),
	x1Geo.azimuthTo(x2Geo) + osg::RadiansToDegrees(gridAngle));
	QGeoCoordinate x3GeoLU = x1Geo.atDistanceAndAzimuth(x1Geo.distanceTo(x3Geo),
	x1Geo.azimuthTo(x3Geo) + osg::RadiansToDegrees(gridAngle));
	QGeoCoordinate x4GeoLU = x1Geo.atDistanceAndAzimuth(x1Geo.distanceTo(x4Geo),
	x1Geo.azimuthTo(x4Geo) + osg::RadiansToDegrees(gridAngle));*/

	/*double x2 = v2[0];
	double y2 = v2[1];
	double x3 = v3[0];;
	double y3 = v3[1];
	double x4 = v4[0];;
	double y4 = v4[1];*/
	/*double x2 = x2GeoLU.longitude();
	double y2 = x2GeoLU.latitude();
	double x3 = x3GeoLU.longitude();
	double y3 = x3GeoLU.latitude();
	double x4 = x4GeoLU.longitude();
	double y4 = x4GeoLU.latitude();
	double x2 = minLon + (maxLon - minLon) * cos(-gridAngle) + (maxLat - minLat) * sin(-gridAngle);
	double y2 = minLat - (maxLon - minLon) * sin(-gridAngle) + (maxLat - minLat) * cos(-gridAngle);
	double x3 = minLon + (maxLon - minLon) * cos(-gridAngle);
	double y3 = minLat - (maxLon - minLon) * sin(-gridAngle);
	double x4 = minLon + (maxLat - minLat) * sin(-gridAngle);
	double y4 = minLat + (maxLat - minLat) * cos(-gridAngle);*/

	/*minLon = std::min(minLon, x2);
	minLon = std::min(minLon, x3);
	minLon = std::min(minLon, x4);

	minLat = std::min(minLat, y2);
	minLat = std::min(minLat, y3);
	minLat = std::min(minLat, y4);

	maxLon = std::max(maxLon, x2);
	maxLon = std::max(maxLon, x3);
	maxLon = std::max(maxLon, x4);

	maxLat = std::max(maxLat, y2);
	maxLat = std::max(maxLat, y3);
	maxLat = std::max(maxLat, y4);*/


	//行、列数
	size_t rows = static_cast<size_t>((maxLat - minLat) / spacing) + 1;
	size_t cols = static_cast<size_t>((maxLon - minLon) / spacing) + 1;

	if (/*false*/gridAngle != 0.0)
	{
		rows *= 3;
		cols *= 3;
	}

	std::vector<std::vector<double>> grid(rows, std::vector<double>(cols, 0.0));

	if (/*false*/gridAngle != 0.0)
	{
		minLon = tempGeo.longitude();
		minLat = tempGeo.latitude();
	}

	assignValuesWithLayers(grid, lons, lats, values, equalCount, disCount, minLon, minLat, spacing, gridAngle, false);

	//m_grid = grid;

	std::vector<std::vector<double>> vec = drawPestSprayPrescriptionMap(grid, spacing, new_space);

	initStyles_forDisPosition();

	std::vector<osgEarth::Features::FeatureList> fLists;
	fLists.resize(colors_Sym_forD.size());
	createGridGeometry(vec, minLon, minLat, new_space, fLists, gridAngle);
	m_grid = vec;
	m_minLon = minLon;
	m_minLat = minLat;
	for (int i = 0; i < fLists.size(); i++)
	{
		osg::ref_ptr<osgEarth::Annotation::FeatureNode> tempFeatureNode = new osgEarth::Annotation::FeatureNode(_mapNode, fLists[i], *styles_forD[i]);
		osg::StateSet* stateSet = tempFeatureNode->getOrCreateStateSet();
		//启用深度测试
		stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		stateSet->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f), osg::StateAttribute::ON);
		_group_forD->addChild(tempFeatureNode);
	}

	//std::vector<osgEarth::Features::FeatureList> fListPoints;
	//fListPoints.resize(colors_Sym_forD.size());
	//createPointsGeometry(lons, lats, values, fListPoints);
	//for (int i = 0; i < fListPoints.size(); i++)
	//{
	//	osg::ref_ptr<osgEarth::Annotation::FeatureNode> tempFeatureNode = new osgEarth::Annotation::FeatureNode(_mapNode, fListPoints[i], *styles_forD[i]);
	//	_group_forD->addChild(tempFeatureNode);
	//}

	//_viewer->addEventHandler(new PickHandler(_mapNode));


}


void HeatMap3D::cleanMap()
{
	lons.clear();
	lats.clear();
	values.clear();

	m_grid.clear();
	mapInfoMap.clear();
	gridVec.clear();
	keyCounts.clear();


	if (_group_forD)
	{
		while (_group_forD->getNumChildren() > 0)
		{
			_group_forD->removeChild(_group_forD->getChild(0));
		}
	}

	_group_forD = nullptr;
}

void HeatMap3D::setImageShowOrHide_forDisPosition(bool isShow)
{
	if (_group_forD != nullptr)
	{
		_group_forD->setNodeMask(isShow ? 1 : 0);
	}
}
void HeatMap3D::updateColors(const std::vector<osgEarth::Symbology::Color>& colors)
{
	colors_Sym_forD = colors;
}
osgEarth::Symbology::Color HeatMap3DUtil::hexToColor(const std::string& hexColor, float rgb_a/* = 1.0f*/)
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


// 计算纬度方向上的 1m 对应的度数
double HeatMap3D::metersToLatitudeDegrees(double meters /*= 1.0*/)
{
	return (meters / (PI * R_POLAR)) * 180.0;
}

// 计算经度方向上的 1m 对应的度数（取决于纬度） 纬度（度°）
double HeatMap3D::metersToLongitudeDegrees(double latitude, double meters /*= 1.0*/)
{
	double latRad = latitude * PI / 180.0;  // 转换为弧度
	double radiusAtLatitude = R_EQUATOR * cos(latRad);
	return (meters / (PI * radiusAtLatitude)) * 180.0;
}


// **计算交集**
QPolygonF HeatMap3D::calculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2)
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