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

#include <QColor>
#include <QPointF>
#include <QPainter>

#include <osgEarthSymbology/Color>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthSymbology/Style>
#include <osgEarthAnnotation/LabelNode>
#include <osgViewer/Viewer>

#include "IProj.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"

using namespace AgriSoilTestingFertilizer;

namespace osg {
	class HeightField;
	class Group;
	class Image;
	class Vec3d;
}

namespace osgEarth {
	class MapNode;
}

namespace HeatMap3DUtil {

	// 十六进制颜色转 osgEarth::Symbology::Color
	osgEarth::Symbology::Color hexToColor(const std::string& hexColor, float rgb_a = 1.0f);

	QColor osgColorToQColor(const osgEarth::Symbology::Color& osgColor);
}

/* 克里金插值算法 */

class KrigingInterpolator 
{
public:
	struct Point {
		Point() {};
		Point(double dLon, double dLat, double dValue);

		double lon;
		double lat;
		double value;
	};

	// range、step 单位：米
	/* << range 取值范围推荐 >>
		|  平均距离倍数	|	    插值风格			|
		|	   0.5		|	保守、局部强影响		|
		|	   1.0		|		平衡				|
		|	   1.5		|	  更平滑插值			|
		---------------------------------------------
		<< neighbors 取值范围推荐 >>
		|	点数范围		|		推荐			|
		|	  < 500			|	所有点（不必局部）  |
		|	500 ~ 5,000		|		32 - 128		|
		|	> 10,000		|		6 - 64			|
		---------------------------------------------
	*/
	KrigingInterpolator(double step = 1.0, int neighbors = 32, double range = 1.0);

	void setSamples(const std::vector<Point>& samples);

	double estimate(double lon, double lat) const;

	void generateGrid(const std::vector<QPointF>& boundPoings, std::vector<double>& lons, std::vector<double>& lats,  std::vector<double>& values, double gridAngle);

	// 线性归一化剪裁，线性映射插值结果到 [min_val, max_val] 区间
	void rescaleValue(std::vector<double>& values, double min_val, double max_val);

private:
	double gaussianCov(double d) const;
	double distance(double lon1, double lat1, double lon2, double lat2) const;
	void autoTuneSill();
	void autoTuneRange(double scale = 1.0);

	std::vector<Point> m_samples;
	double m_range; // 高斯核距离参数（空间尺度参数）（表示一个点能影响 m_range 度经纬度内的区域）
	double m_sill;  // 协方差幅度（推荐：样本点值的方差 value）
	double m_step;  // 网格步长
	int	   m_neighbors; // 使用“邻近样本点”参与每个插值（局部 Kriging）, 优化执行效率（比如 32、64、128）
};


class HeatMap3D
{
public:
	HeatMap3D();
	HeatMap3D(osgEarth::MapNode* mapNode);
	~HeatMap3D();

	// 初始化边界
	void setBoundPoints(const std::vector<QPointF>& boundPoings);

	// 初始化内点值
	void setLonLatValue(const std::vector<double>& _lons, const std::vector<double> _lats, const std::vector<double> _values);

	// 初始化像素值(米)
	void setPixelSpacing(double _spacing);

	// 初始化与位置点同值的层数
	void setEqualCount(int _equalCount);

	// 初始化逐步消减的层数
	void setDisCount(int _disCount);

	// 更新颜色
	void updateColors(const std::vector<osgEarth::Symbology::Color>& colors);

	void setMapNode(osgEarth::MapNode* mapNode);

	void setViewer(osgViewer::Viewer* viewer);

public:
	//开始执行
	void startCreateHeatMap(bool isCoordinateShow = false, bool isGridShow = false, double gridAngle = 0.0);

	void create2DGridDrawing(QPainter& painter, IProj *pProj);

	void generateSamplesData(AgriSoilTestingCollectionPtrVec& soilTestingCollectVec);

public:
	// 获取统计信息
	void getPixelStatistic(std::vector<int>& statistics);

	// 设置图片显隐
	void setImageShowOrHide(bool isShow);

	// 设置坐标显隐
	void setCoordinateShowOrHide(bool isShow);

	// 设置编号显隐
	void setPointIdShowOrHide(bool isShow);

	// 设置网格线显隐
	void setGridLineShowOrHide(bool isShow);

	// 设置网格采样信息
	void setGridSampleInfo(SampleType sampleType = SampleType::CELL_SAMPLING, int sampleMode = 1);

	void cleanUp();

private:
	void InitMap(double gridAngle = 0.0);

	// 创建高度场
	osg::HeightField* createHeightField(const std::vector<std::vector<double>>& grid, double minX, double minY);

	//将高度场转换为RGBA图像
	osg::Image* heightFieldToImage(osg::HeightField* hf, double minlon, double minlat);

	// 逐层递减赋值，并处理相交区域相加 //equalCount:相等的范围，disCount：递减至0的范围
	void assignValuesWithLayers(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats,
		const std::vector<double>& values, int equalCount, int disCount, double minLon, double minLat, double angle = 0.0);

	void assignValuesWithLayers2(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats,
		const std::vector<double>& values, double minLon, double minLat, std::vector<std::vector<double>>& spread, int spreadLayer);

	// 初始化样式
	void initStyles(bool isGridShow = false);
	void initStyles_forDisPosition();

	void createGridGeometry(std::vector<std::vector<double>>& grid, double minLon, double minLat
		, std::vector<osgEarth::Features::FeatureList>& featureLists, std::vector<osgEarth::Annotation::LabelNode*>& labelList
		, float angle = 0.0, int _startRow = 0, int _endRow = 0);

	void createPointsGeometry(const std::vector<double>& lons, const std::vector<double>& lats,
		const std::vector<double>& values, std::vector<osgEarth::Features::FeatureList>& featureLists); // 新增：旋转中心纬度
private:
	osgEarth::MapNode*	_mapNode;
	osg::Group*			_group;
	osgViewer::Viewer*	_viewer;

	// 经度、纬度、数据值
	std::vector<double> lons;
	std::vector<double> lats;
	std::vector<double> values;

	// 边界
	std::vector<osg::Vec3d> points_bound;

	// 网格
	std::vector<std::vector<double>> grid;

	// 边界最值、行数、列数
	double maxLon = 0.0;
	double minLon = 0.0;
	double maxLat = 0.0;
	double minLat = 0.0;
	size_t rows = 0;
	size_t cols = 0;

	// 像素值大小
	double spacing = 1.0;		// m
	double spaceLon = 0.00001141; // 度°(degree)
	double spaceLat = 0.00000899;

	int equalCount = 0;
	int disCount = 0;

	// 网格线显隐
	bool gridLineShow = false;
	bool coordinateShow = false;
	bool pointIdShow = false;

	// 网格点类型
	SampleType _sampleType;
	int _sampleMode;

	std::vector<osgEarth::Symbology::Color>  colors_Sym = {
		HeatMap3DUtil::hexToColor("#30913C", 0.5f),
		osgEarth::Symbology::Color::Green,
		osgEarth::Symbology::Color::Yellow,
		osgEarth::Symbology::Color::Orange,
		osgEarth::Symbology::Color::Red,
		osgEarth::Symbology::Color::Purple,
		osgEarth::Symbology::Color::Black
	};

	std::vector<osgEarth::Symbology::Style*> styles;
	std::vector<osgEarth::Features::FeatureList> fLists;
	std::vector<osgEarth::Annotation::LabelNode*> labelList;

	AgriSoilTestingCollectionPtrVec m_soilTestingCollectionVec;
};
