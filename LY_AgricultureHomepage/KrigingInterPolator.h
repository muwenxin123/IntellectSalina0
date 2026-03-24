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

	void setMapNode(osgEarth::MapNode* mapNode);
	//void setLonLatValues();

	//开始执行
	void startCreateHeatMap();

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


	//////////处方 上界

	//开始执行
	void startCreateHeatMap_forDisPosition();

	//设置像素值
	void setPixelSpacing_forDisPosition(double _spacing);

	//设置与位置点同值的层数
	void setEqualCount_forDisPosition(int _equalCount);

	//设置逐步消减的层数
	void setDisCount_forDisPosition(int _disCount);

	//设置图片显隐
	void setImageShowOrHide_forDisPosition(bool isShow);

	//////////处方  下界

private:
	
	// 创建高度场
	osg::HeightField* createHeightField(const std::vector<std::vector<double>>& grid, double minX, double minY, double spacing);

	//将高度场转换为RGBA图像
	osg::Image* heightFieldToImage(osg::HeightField* hf, double minlon, double minlat, double spacing);


	// 逐层递减赋值，并处理相交区域相加 //equalCount:相等的范围，disCount：递减至0的范围
	void assignValuesWithLayers(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats, 
		const std::vector<double>& values, int equalCount, int disCount, double minLon, double minLat, double spacing, bool isDispose = false);

	//
	void initStyles();
	void initStyles_forDisPosition();

	void HeatMap3D::createGridGeometry(std::vector<std::vector<double>>& grid, double minLon, double minLat, double spacing
		, std::vector<osgEarth::Features::FeatureList>& featureLists);

private:
	osgEarth::MapNode*	_mapNode;
	osg::Group*			_group;


	//经度、纬度、数据值
	std::vector<double> lons;
	std::vector<double> lats;
	std::vector<double> values;

	std::vector<osg::Vec3d> points_bound;

	//像素值大小
	double spacing = 8.680555555555556e-6;		//0.97m

	int equalCount = 0;
	int disCount = 0;

	std::vector<osgEarth::Symbology::Color>  colors_Sym = {
		/*osgEarth::Symbology::Color::White,
		osgEarth::Symbology::Color::Gray,*/
		osgEarth::Symbology::Color::Yellow,
		osgEarth::Symbology::Color::Black,
		osgEarth::Symbology::Color::Orange,
		osgEarth::Symbology::Color::Red
	};
	std::vector<osgEarth::Symbology::Style*> styles;


	//////////处方  上界

	osg::Group*			_group_forD;
	//像素值大小
	double spacing_forD = 5.555555555555556e-4;		//0.97m

	int equalCount_forD = 0;
	int disCount_forD = 0;

	std::vector<osgEarth::Symbology::Color>  colors_Sym_forD = {
		//osgEarth::Symbology::Color::White,
		osgEarth::Symbology::Color::Gray,
		osgEarth::Symbology::Color::Yellow,
		osgEarth::Symbology::Color::Black,
		osgEarth::Symbology::Color::Orange,
		osgEarth::Symbology::Color::Red
	};
	std::vector<osgEarth::Symbology::Style*> styles_forD;
	//////////处方  下界

	

};
