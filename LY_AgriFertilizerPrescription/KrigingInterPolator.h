/*
根据一个边界和一些坐标点的value值生成边界范围内的热力图，并贴到边界区域上，
边界的值需要排列好， 
使用：
1、初始化，调用默认构造或 HeatMap3D(osgEarth::MapNode* mapNode);构造，
2、设置：
	2.1 若调用默认构造，必须调用  setMapNode(osgEarth::MapNode* mapNode); 函数
	2.2 设置边界： setBoundPoints(const std::vector<QPointF>& boundPoings);
	2.3 设置坐标点及值： setLonLatValue(const std::vector<double>& _lons, const std::vector<double> _lats, const std::vector<double> _values);
	注意： 默认像素点是 0.97m， 即经纬度的 秒单位的 1/32″，可通过  setPixelSpacing(double _spacing);修改

3、计算生成：
	调用  startCreateHeatMap();开始生成，
	可通过  setImageShowOrHide(bool isShow); 设置生成的图是否显示， （true: 显示 false 隐藏）


*/

#pragma once

#include <iostream>
#include <vector>

#include <QPointF>
#include <osgEarthSymbology/Color>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthSymbology/Style>
#include <QMap>
#include <QPainter>
#include <QRectF>
#include "IProj.h"
#include "LY_AgriFertilizerPrescriptionActivator.h"
#include <QPointF>


namespace osg {
	class HeightField;
	class Group;
	class Image;
	class Vec3d;
}

namespace osgEarth {
	class MapNode;
	
}

// 简单的克里金插值算法实现
class KrigingInterpolator {
public:
	KrigingInterpolator(const std::vector<double>& x, const std::vector<double>& y, const std::vector<double>& z)
		: x_(x), y_(y), z_(z) {}

	double interpolate(double x, double y) {
		double sumWeights = 0.0;
		double sumWeightedValues = 0.0;

		for (size_t i = 0; i < x_.size() && i < y_.size() && i < z_.size(); ++i) {
			double distance = std::sqrt(std::pow(x - x_[i], 2) + std::pow(y - y_[i], 2));
			double weight = 1.0 / (distance + 1e-9); // 简单反距离加权
			sumWeights += weight;
			sumWeightedValues += weight * z_[i];
		}

		return sumWeightedValues / sumWeights;
	}
private:
	std::vector<double> x_;
	std::vector<double> y_;
	std::vector<double> z_;
};

class HeatMap3D
{
public:
	HeatMap3D();
	HeatMap3D(osgEarth::MapNode* mapNode);
	~HeatMap3D();

	void updateColors(const std::vector<osgEarth::Symbology::Color>& colors);
	//每个等级对应的方格信息
	std::map<double, osgEarth::Polygon*>mapInfoMap;
	//帅选相邻元素
	std::vector<std::vector<int>> groupConsecutiveNumbers(const std::vector<int>& nums, bool isLeft);
	//统计每个等级的个数
	QMap<double, int> keyCounts;

	std::vector<AgriSoilTestingFertilityPoint> gridVec;
	void setOrigins(const std::vector<double> origins);

	//std::map<QPoint, AgriSoilTestingFertilityPoint> soilFertilityMap;
	void cleanMap();
	void setMapNode(osgEarth::MapNode* mapNode);
	//设置边界
	void setBoundPoints(const std::vector<QPointF>& boundPoings);
	//设置内点值
	void setLonLatValue(const std::vector<double>& _lons, const std::vector<double> _lats, const std::vector<double> _values);
	//设置像素值
	void setPixelSpacing(double _spacing);
	//设置与位置点同值的层数
	void setEqualCount(int _equalCount);
	//设置逐步消减的层数
	void setDisCount(int _disCount);
	//设置图片显隐
	void setImageShowOrHide(bool isShow);
	//开始执行
	void startCreateHeatMap_forDisPosition(double gridAngle);

	//绘制2D
	void create2DGridDrawing( QPainter& painter, IProj *pProj);
	void setSpace(double space);
	//设置图片显隐
	void setImageShowOrHide_forDisPosition(bool isShow);
	//////////处方  下界

	double spaceLon = 0.00001141; // 度°(degree)
	double spaceLat = 0.00000899;

	double new_spaceLon = 0.00001141; // 度°(degree)
	double new_spaceLat = 0.00000899;

private:
	
	// 创建高度场
	osg::HeightField* createHeightField(const std::vector<std::vector<double>>& grid, double minX, double minY, double spacing);

	// 逐层递减赋值，并处理相交区域相加 //equalCount:相等的范围，disCount：递减至0的范围 (虫害图)
	void assignValuesWithLayers(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats, 
		const std::vector<double>& values, int equalCount, int disCount, double minLon, double minLat, double spacing, double angel, bool isDispose = false);

	//处方图
	std::vector<std::vector<double>>  drawPestSprayPrescriptionMap(std::vector<std::vector<double>>& grid, double kH, double kW);


	void initStyles_forDisPosition();

	void HeatMap3D::createGridGeometry(std::vector<std::vector<double>>& grid, double minLon, double minLat, double spacing
		, std::vector<osgEarth::Features::FeatureList>& featureLists , double angel);



	/* 计算多边形交集 */
	QPolygonF calculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2);
	// 地球的半径（单位：米）
	const double R_EQUATOR = 6378137.0;  // 赤道半径
	const double R_POLAR = 6356752.3;    // 极半径
	const double PI = 3.14159265358979323846;

	// 计算纬度方向上的 1m 对应的度数
	double metersToLatitudeDegrees(double meters = 1.0);

	// 计算经度方向上的 1m 对应的度数（取决于纬度） 纬度（度°）
	double metersToLongitudeDegrees(double latitude, double meters = 1.0);

private:
	osgEarth::MapNode*	_mapNode;
	osg::Group*			_group;


	std::vector<std::vector<double>> m_grid;
	double m_minLat = 0.00;
	double m_minLon = 0.00;

	//经度、纬度、数据值
	std::vector<double> lons;
	std::vector<double> lats;
	std::vector<double> values; //等级
	std::vector<double> _origins;

	std::vector<osg::Vec3d> points_bound;

	double spacing = 1.0  * (1.0 / 110000.0);		// m
	//double spaceLon = 0.00001141; // 度°(degree)
	//double spaceLat = 0.00000899;

	//处方图像素大小
	//double new_space = 4.0 * (1.0 / 110000.0);
	double new_space = 4.0 * (1.0 / 110000.0);
	//double new_spaceLon = 0.00001141; // 度°(degree)
	//double new_spaceLat = 0.00000899;
	//分布图像素大小
	//double spacing = 2.0 * (1.0 / 110000.0);		

	double m_old_space = 1.0;

	int equalCount = 0;

	int disCount = 0;

	osg::Group*			_group_forD;


	std::vector<osgEarth::Symbology::Color>  colors_Sym_forD = {
		osgEarth::Symbology::Color::Green,
		osgEarth::Symbology::Color::Gray,
		//osgEarth::Symbology::Color::Green,
		osgEarth::Symbology::Color::Black,
		osgEarth::Symbology::Color::Orange,
		osgEarth::Symbology::Color::Red,
		osgEarth::Symbology::Color::Navy,
		osgEarth::Symbology::Color::Blue,
		osgEarth::Symbology::Color::Purple,
		osgEarth::Symbology::Color::Fuchsia,
		osgEarth::Symbology::Color::Teal,
	};
	std::vector<osgEarth::Symbology::Style*> styles_forD;
	//////////处方  下界


	QColor osgColorToQColor(const osgEarth::Symbology::Color& osgColor) {
		// 提取 RGBA 分量（范围 0.0~1.0）
		float r = osgColor.r();
		float g = osgColor.g();
		float b = osgColor.b();
		float a = osgColor.a();

		// 映射到 0~255 并构造 QColor
		return QColor(
			static_cast<int>(r * 255.0f + 0.5f),  // 四舍五入
			static_cast<int>(g * 255.0f + 0.5f),
			static_cast<int>(b * 255.0f + 0.5f),
			static_cast<int>(a * 255.0f + 0.5f)
		);
	}

	// 获取两点在世界坐标系中的X / Y差值
		osg::Vec2d getWorldXYDistance(
			double lon1, double lat1,  // 点1经纬度
			double lon2, double lat2,  // 点2经纬度
			osgEarth::MapNode* mapNode)
	{
		// 转为世界坐标
		osgEarth::GeoPoint p1(mapNode->getMap()->getSRS(), lon1, lat1);
		osgEarth::GeoPoint p2(mapNode->getMap()->getSRS(), lon2, lat2);

		osg::Vec3d world1, world2;
		p1.toWorld(world1);
		p2.toWorld(world2);

		// 返回X/Y方向的距离（米）
		return osg::Vec2d(
			world2.x() - world1.x(),  // X方向差值（经度方向）
			world2.y() - world1.y()   // Y方向差值（纬度方向）
		);
	}
		osg::Vec2d worldToScreenDistance(
			const osg::Vec2d& worldDist,
			const osg::Vec3d& refWorldPos,  // 参考点世界坐标（通常取两点中点）
			osg::Camera* camera)
		{
			// 获取视口和投影参数
			osg::Viewport* vp = camera->getViewport();
			double fovy, aspect, znear, zfar;
			camera->getProjectionMatrixAsPerspective(fovy, aspect, znear, zfar);
			fovy = osg::DegreesToRadians(fovy);

			// 计算参考点到相机的距离
			osg::Vec3d eye = camera->getViewMatrix().getTrans();
			double distance = (refWorldPos - eye).length();

			// 计算像素/米比例（分别处理X/Y方向）
			double pixelsPerMeter = vp->height() / (2 * distance * tan(fovy / 2));
			return osg::Vec2d(
				worldDist.x() * pixelsPerMeter * aspect,  // X方向像素距离
				worldDist.y() * pixelsPerMeter           // Y方向像素距离
			);
		}
		osg::Vec2d geoToScreenXYDistance(
			double lon1, double lat1,
			double lon2, double lat2,
			osgEarth::MapNode* mapNode,
			osg::Camera* camera)
		{
			// 1. 获取世界坐标XY差值
			osg::Vec2d worldXY = getWorldXYDistance(lon1, lat1, lon2, lat2, mapNode);

			// 2. 获取参考点（中点）世界坐标
			osgEarth::GeoPoint midPoint(
				mapNode->getMap()->getSRS(),
				(lon1 + lon2) / 2,
				(lat1 + lat2) / 2);
			osg::Vec3d midWorldPos;
			midPoint.toWorld(midWorldPos);

			// 3. 转换为屏幕像素距离
			return worldToScreenDistance(worldXY, midWorldPos, camera);
		}
};
