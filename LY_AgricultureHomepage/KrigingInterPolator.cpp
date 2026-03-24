#include "KrigingInterPolator.h"
#include <algorithm>
#include <cmath>

#include <osgEarthAnnotation/ImageOverlay>
#include <osgEarthSymbology/Geometry>
#include <osgEarth/Map>
#include <osgEarth/MapNode>
#include <osgEarth/ImageLayer>

#include <osgEarthFeatures/Feature>
#include <osgEarthFeatures/FeatureListSource>
#include <osgEarthFeatures/FeatureModelGraph>


#include <osg/Texture2D>





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

void HeatMap3D::startCreateHeatMap()
{
	while(_group->getNumChildren() > 0)
	{
		_group->removeChild(_group->getChild(0));
	}

	//创建克里金插值器
	//KrigingInterpolator kriging(lons, lats, values);
	double maxLon = *(std::max_element(lons.begin(), lons.end()));
	double minLon = *(std::min_element(lons.begin(), lons.end()));
	double maxLat = *(std::max_element(lats.begin(), lats.end()));
	double minLat = *(std::min_element(lats.begin(), lats.end()));

	for (int i = 0; i < points_bound.size(); i++)
	{
		maxLon = maxLon > points_bound[i].x() ? maxLon : points_bound[i].x();
		minLon = minLon < points_bound[i].x() ? minLon : points_bound[i].x();
		maxLat = maxLat > points_bound[i].y() ? maxLat : points_bound[i].y();
		minLat = minLat < points_bound[i].y() ? minLat : points_bound[i].y();
	}

	//行、列数
	size_t rows = static_cast<size_t>((maxLat - minLat) / spacing) + 1;
	size_t cols = static_cast<size_t>((maxLon - minLon) / spacing) + 1;

	std::vector<std::vector<double>> grid(rows, std::vector<double>(cols, 0.0));

	assignValuesWithLayers(grid, lons, lats, values, equalCount, disCount, minLon, minLat, spacing);

	initStyles();

	std::vector<osgEarth::Features::FeatureList> fLists;
	fLists.resize(colors_Sym.size());
	createGridGeometry(grid, minLon, minLat, spacing, fLists);
	for (int i = 0; i < fLists.size(); i++)
	{
		osg::ref_ptr<osgEarth::Annotation::FeatureNode> tempFeatureNode = new osgEarth::Annotation::FeatureNode(_mapNode, fLists[i], *styles[i]);
		_group->addChild(tempFeatureNode);
	}
	return;

	// 创建高度场
	osg::ref_ptr<osg::HeightField> hf = createHeightField(grid, minLon, minLat, spacing);
	
	//创建遮罩图
	osg::ref_ptr<osg::Image> image = heightFieldToImage(hf.get(), minLon, minLat, spacing);
	osg::ref_ptr<osgEarth::Annotation::ImageOverlay> imageOverlay = new osgEarth::Annotation::ImageOverlay(_mapNode/*, image.get()*/);
	//osg::Texture2D* texture2 = new osg::Texture2D(image.get());
	//texture2->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
	//texture2->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
	///*texture2->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	//texture2->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);*/
	//imageOverlay->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture2);
	imageOverlay->setImage(image);
	

	//osg::Texture2D* texture = imageOverlay.get()->getTexture();
	osg::StateSet* stateSet = imageOverlay->getOrCreateStateSet();
	osg::ref_ptr<osg::Texture2D> texture2 = new osg::Texture2D(image.get());
	texture2->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	texture2->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
	texture2->setWrap(osg::Texture2D::WRAP_S, osg::Texture2D::CLAMP_TO_EDGE);
	texture2->setWrap(osg::Texture2D::WRAP_T, osg::Texture2D::CLAMP_TO_EDGE);
	texture2->setUseHardwareMipMapGeneration(true);
	texture2->setMaxAnisotropy(1.0f);
	stateSet->setMode(GL_BLEND, osg::StateAttribute::OFF);		//禁用透明混合
	stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	stateSet->setTextureAttributeAndModes(0, texture2.get(), osg::StateAttribute::ON);

	imageOverlay->setBounds(osgEarth::Annotation::Bounds(minLon, minLat, maxLon, maxLat));
	_group->addChild(imageOverlay.get());
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

osg::Image * HeatMap3D::heightFieldToImage(osg::HeightField * hf, double minlon, double minlat, double spacing)
{
	int cols = hf->getNumColumns();
	int rows = hf->getNumRows();

	//创建图像
	osg::Image* image = new osg::Image();
	image->allocateImage(cols, rows, 1, GL_RGBA, GL_UNSIGNED_BYTE);

	osgEarth::Symbology::Geometry* geoMery = osgEarth::Symbology::Geometry::create(osgEarth::Symbology::Geometry::TYPE_POLYGON, &points_bound);
	osgEarth::Symbology::Polygon * polygon = dynamic_cast<osgEarth::Symbology::Polygon*>(geoMery);
	
	// 创建颜色数组（根据高度值映射颜色）
	//osg::Vec4Array* colors = new osg::Vec4Array();
	float minHeight = hf->getHeight(0, 0);
	float maxHeight = hf->getHeight(0, 0);
	for (unsigned int y = 0; y < hf->getNumRows(); ++y) {
		for (unsigned int x = 0; x < hf->getNumColumns(); ++x) {
			float height = hf->getHeight(x, y);
			if (height < minHeight) minHeight = height;
			if (height > maxHeight) maxHeight = height;
		}
	}

	unsigned char* data = image->data();
	for (int y = 0; y < rows; ++y)
	{
		for (int x = 0; x < cols; ++x)
		{
			//计算当前像素的索引
			int index = (y * cols + x) * 4;

			if(!(polygon->contains2D(minlon + x * spacing, minlat + y * spacing)))
			{
				data[index] = 0;
				data[index + 1] = 0;
				data[index + 2] = 0;
				data[index + 3] = 0;
				continue;
			}
			
			float height = hf->getHeight(x, y);

			int colorIndex = (int)((height - minHeight) / (maxHeight - minHeight) * (colors_Sym.size() - 1));
			//osgEarth::Symbology::Color* tempColor = &(colors_Sym[colorIndex]);
			data[index] = colors_Sym[colorIndex].r() * 255;
			data[index + 1] = colors_Sym[colorIndex].g() * 255;
			data[index + 2] = colors_Sym[colorIndex].b() * 255;
			data[index + 3] = colors_Sym[colorIndex].a() * 255;
			/*int normalizedHeight = (int)((height - minHeight) / (maxHeight - minHeight) * 255) % 256;
			//osg::Vec4 color = osg::Vec4(normalizedHeight, 1.0 - normalizedHeight, 0.0, 1.0); // 颜色映射

			//设置RGBA值
			data[index] = normalizedHeight;
			data[index + 1] = 255 - normalizedHeight;
			data[index + 2] = 0;
			data[index + 3] = height == 0 ? height : 255;*/

			
		}
	}
	return image;
}

void HeatMap3D::assignValuesWithLayers(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats,
	const std::vector<double>& values, int equalCount, int disCount, double minLon, double minLat, double spacing, bool isDispose)
{
	int layer = equalCount + disCount;
	int maxDis = sqrt(pow(layer, 2) + pow(layer, 2));
	for (size_t idx = 0; idx < lons.size() && idx < lats.size() && idx < values.size(); ++idx) {
		int y = (lons[idx] - minLon) / spacing;
		int x = (lats[idx] - minLat) / spacing;
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

void HeatMap3D::initStyles()
{
	styles.clear();
	for (int i = 0; i < colors_Sym.size(); i++)
	{
		osgEarth::Symbology::Style* geomStyle = new osgEarth::Symbology::Style;
		/*geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = osgEarth::Color::Black;
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0.0;*/
		
		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = colors_Sym[i];
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

		styles.push_back(geomStyle);
	}
}

void HeatMap3D::initStyles_forDisPosition()
{
	styles_forD.clear();
	for (int i = 0; i < colors_Sym_forD.size(); i++)
	{
		osgEarth::Symbology::Style* geomStyle = new osgEarth::Symbology::Style;
		/*geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = osgEarth::Color::Black;
		geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0.0;*/

		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = colors_Sym[i];
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

void HeatMap3D::createGridGeometry(std::vector<std::vector<double>>& grid, double minLon, double minLat, double spacing,
	std::vector<osgEarth::Features::FeatureList>& featureLists)
{
	osg::Vec3d origin(minLon, minLat, 0);
	
	osgEarth::Symbology::Geometry* geoMery = osgEarth::Symbology::Geometry::create(osgEarth::Symbology::Geometry::TYPE_POLYGON, &points_bound);
	osgEarth::Symbology::Polygon * polygon = dynamic_cast<osgEarth::Symbology::Polygon*>(geoMery);

	double minHeight = 0, maxHeight = values[0];

	for (int i = 1; i < values.size(); i++)
	{
		minHeight = minHeight < values[i] ? minHeight : values[i];
		maxHeight = maxHeight > values[i] ? maxHeight : values[i];
	}

	for (int x = 0; x < grid.size(); x++)
	{
		for (int y = 0; y < grid[0].size(); y++)
		{
			osg::Vec3d v1 = origin + osg::Vec3d(y * spacing, x * spacing, 0);
			if (!(polygon->contains2D(v1.x(), v1.y())))
			{
				continue;
			}
			osg::Vec3d v2 = v1 + osg::Vec3d(spacing, 0, 0);
			osg::Vec3d v3 = v2 + osg::Vec3d(0, spacing, 0);
			osg::Vec3d v4 = v1 + osg::Vec3d(0, spacing, 0);

			osgEarth::Polygon* polygon = new osgEarth::Polygon();
			polygon->push_back(v1);
			polygon->push_back(v2);
			polygon->push_back(v3);
			polygon->push_back(v4);

			osg::ref_ptr<osgEarth::Annotation::Feature> feature = new osgEarth::Annotation::Feature(polygon, _mapNode->getMapSRS()->getGeographicSRS());
			
			
			double height = grid[x][y];
			if (height > 0)
			{
				int a = 0; 
				int b = a;
			}
			//添加颜色
			int colorIndex = (int)((height - minHeight) / (maxHeight - minHeight) * (styles.size() - 1));
			if (colorIndex != 0)
			{
				int a = 0;
				int b = a;
			}
			//feature->style() = *(styles[colorIndex]);
			featureLists[colorIndex].push_back(feature);
			
		}
	}
	
}

void HeatMap3D::startCreateHeatMap_forDisPosition()
{
	while (_group_forD->getNumChildren() > 0)
	{
		_group_forD->removeChild(_group_forD->getChild(0));
	}

	//创建克里金插值器
	//KrigingInterpolator kriging(lons, lats, values);
	double maxLon = *(std::max_element(lons.begin(), lons.end()));
	double minLon = *(std::min_element(lons.begin(), lons.end()));
	double maxLat = *(std::max_element(lats.begin(), lats.end()));
	double minLat = *(std::min_element(lats.begin(), lats.end()));

	for (int i = 0; i < points_bound.size(); i++)
	{
		maxLon = maxLon > points_bound[i].x() ? maxLon : points_bound[i].x();
		minLon = minLon < points_bound[i].x() ? minLon : points_bound[i].x();
		maxLat = maxLat > points_bound[i].y() ? maxLat : points_bound[i].y();
		minLat = minLat < points_bound[i].y() ? minLat : points_bound[i].y();
	}

	//行、列数
	size_t rows = static_cast<size_t>((maxLat - minLat) / spacing_forD) + 1;
	size_t cols = static_cast<size_t>((maxLon - minLon) / spacing_forD) + 1;

	std::vector<std::vector<double>> grid(rows, std::vector<double>(cols, 0.0));

	assignValuesWithLayers(grid, lons, lats, values, equalCount_forD, disCount_forD, minLon, minLat, spacing_forD, true);

	initStyles_forDisPosition();

	std::vector<osgEarth::Features::FeatureList> fLists;
	fLists.resize(colors_Sym.size());
	createGridGeometry(grid, minLon, minLat, spacing_forD, fLists);
	for (int i = 0; i < fLists.size(); i++)
	{
		osg::ref_ptr<osgEarth::Annotation::FeatureNode> tempFeatureNode = new osgEarth::Annotation::FeatureNode(_mapNode, fLists[i], *styles_forD[i]);
		_group_forD->addChild(tempFeatureNode);
	}
}

void HeatMap3D::setPixelSpacing_forDisPosition(double _spacing)
{
	spacing_forD = _spacing;
}

void HeatMap3D::setEqualCount_forDisPosition(int _equalCount)
{
	equalCount_forD = _equalCount;
}

void HeatMap3D::setDisCount_forDisPosition(int _disCount)
{
	disCount_forD = _disCount;
}

void HeatMap3D::setImageShowOrHide_forDisPosition(bool isShow)
{
	if (_group_forD != nullptr)
	{
		_group_forD->setNodeMask(isShow ? 1 : 0);
	}
}



