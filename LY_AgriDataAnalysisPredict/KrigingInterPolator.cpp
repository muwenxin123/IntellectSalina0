#include <algorithm>
#include <cmath>
#include <QString>
#include <QVector>
#include <map3d/IEarth.h>

#include <osg/BlendFunc>
#include <osg/Depth>
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

#include <PtInPloy.h>
#include <AcmGeometry.h>

//#include "osgTools/DTB_BillboardManager.h"
#include "AgriDataAnalysisPredictUtil.h"
#include "CellularAutomata.h"
#include "KrigingInterPolator.h"

#include <QGeoCoordinate>
#include <osgEarth\GeoMath>
#include <osg/PolygonOffset>
#include <osg/CoordinateSystemNode>


using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;

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

QColor HeatMap3DUtil::osgColorToQColor(const osgEarth::Symbology::Color& osgColor)
{
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

//////////////////////////////////////////////////////////////////////////////////////////////


// 全局变量（存储标牌 UI）
osg::ref_ptr<osgWidget::Box> infoBox;
osg::ref_ptr<osgWidget::Label> titleLabel;
osg::ref_ptr<osgWidget::Label> contentLabel;
osg::ref_ptr<osgWidget::WindowManager> wm;

// 全局变量（存储当前选中的 FeatureNode 和 LabelNode）
osg::ref_ptr<FeatureNode> selectedFeatureNode = nullptr;

// 鼠标点击事件处理器
class PickHandler : public osgGA::GUIEventHandler {
public:
	explicit PickHandler(osg::ref_ptr<MapNode> mapNode) : _mapNode(mapNode) {}

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa) override {
		if (ea.getEventType() == osgGA::GUIEventAdapter::PUSH &&
			ea.getButton() == osgGA::GUIEventAdapter::LEFT_MOUSE_BUTTON) {

			osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
			if (!view) return false;

			osgUtil::LineSegmentIntersector::Intersections intersections;
			if (view->computeIntersections(ea.getX(), ea.getY(), intersections)) {
				FeatureNode* nearestNode = nullptr;
				osgUtil::LineSegmentIntersector::Intersection a = *intersections.begin();
				osg::NodePath nodePath = a.nodePath;
				if (nodePath.size())
				{
					nearestNode = dynamic_cast<FeatureNode*>(nodePath[nodePath.size() - 1]);
				}

				if (nearestNode)
				{
					highlightFeature(nearestNode);  // 高亮 FeatureNode
				}

				//// 检查是否点击到了 FeatureNode
				//for (auto it = nodePath.rbegin(); it != nodePath.rend(); ++it) {
				//	if (auto* featureNode = dynamic_cast<FeatureNode*>(*it)) {
				//		highlightFeature(featureNode);  // 高亮 FeatureNode
				//		updateInfoBox("Feature 选中", hit.getWorldIntersectPoint());
				//		return true;
				//	}
				//}
			}
		}
		return false;
	}

private:
	osg::ref_ptr<MapNode> _mapNode;
	Style originalStyle;  // 存储原始样式

						  // 高亮 FeatureNode
	void highlightFeature(osg::ref_ptr<FeatureNode> featureNode) {
		if (selectedFeatureNode == featureNode) return;  // 已选中的不重复处理

		// 恢复之前的 FeatureNode 样式
		if (selectedFeatureNode) {
			selectedFeatureNode->setStyle(originalStyle);
		}

		// 记录当前 FeatureNode 并保存原始样式
		selectedFeatureNode = featureNode;
		originalStyle = featureNode->getStyle();

		// 设置高亮样式
		Style highlightStyle = originalStyle;
		highlightStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color::Yellow;  // 设为黄色高亮
		featureNode->setStyle(highlightStyle);
	}

	// 更新标牌内容
	void updateInfoBox(const std::string& title, const osg::Vec3d& worldPos) {
		titleLabel->setLabel(title);
		contentLabel->setLabel("经度: " + std::to_string(worldPos.x()) + "\n"
			"纬度: " + std::to_string(worldPos.y()));

		// 设置 Box 位置（示例：根据屏幕空间位置调整）
		infoBox->setPosition(100, 300, 1000);
		infoBox->show();
	}
};

// 创建 UI 标牌
osg::ref_ptr<osgWidget::Box> createInfoBox() {
	osg::ref_ptr<osgWidget::Box> box = new osgWidget::Box("InfoBox", osgWidget::Box::VERTICAL);

	// 设置背景颜色 & 边框
	box->getBackground()->setColor(0.1f, 0.1f, 0.1f, 0.8f);
	box->getBackground()->setPadding(5);

	// 标题
	titleLabel = new osgWidget::Label("TitleLabel", "标题");
	titleLabel->setColor(1.0f, 1.0f, 0.0f, 1.0f);  // 黄色字体
	box->addWidget(titleLabel);

	// 详细内容
	contentLabel = new osgWidget::Label("ContentLabel", "信息");
	contentLabel->setColor(1.0f, 1.0f, 1.0f, 1.0f);  // 白色字体
	box->addWidget(contentLabel);

	return box;
}


/////////////////////////////////////////////////////////////////////////////////////////////////


HeatMap3D::HeatMap3D()
{
	_mapNode = nullptr;
	_group = nullptr;
}

HeatMap3D::HeatMap3D(osgEarth::MapNode * mapNode)
{
	_mapNode = nullptr;
	_group = nullptr;

	_mapNode = mapNode;
	if (_group == nullptr)
	{
		_group = new osg::Group();
		mapNode->addChild(_group);
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

void HeatMap3D::updateColors(const std::vector<osgEarth::Symbology::Color>& colors)
{
	colors_Sym = colors;
}

void HeatMap3D::getPixelStatistic(std::vector<int>& statistics)
{
	for (const auto& feature : fLists)
	{
		statistics.emplace_back((int)feature.size());
	}
}

void HeatMap3D::cleanUp()
{
	lons.clear();
	lats.clear();
	values.clear();

	if (_group)
	{
		_group->removeChildren(0, _group->getNumChildren());
	}

	_group = nullptr;
}

void HeatMap3D::setMapNode(osgEarth::MapNode * mapNode)
{
	_mapNode = mapNode;
	if (_group == nullptr)
	{
		_group = new osg::Group();
		mapNode->addChild(_group);
	}
}

void HeatMap3D::setViewer(osgViewer::Viewer* viewer)
{
	_viewer = viewer;
}

void HeatMap3D::startCreateHeatMap(bool isCoordinateShow /*= false*/, bool isGridShow /*= false*/, double gridAngle /*= 0.0*/)
{
	InitMap(gridAngle);

	assignValuesWithLayers(grid, lons, lats, values, equalCount, disCount, minLon, minLat, gridAngle);

	initStyles(isGridShow);

	//std::vector<osgEarth::Features::FeatureList> fLists;
	fLists.resize(colors_Sym.size());

	//std::vector<osgEarth::Annotation::LabelNode*> labelList;

	createGridGeometry(grid, minLon, minLat, fLists, labelList, gridAngle);

	for (int i = 0; i < fLists.size(); i++)
	{
		//osg::ref_ptr<osgEarth::Annotation::FeatureNode> tempFeatureNode = new osgEarth::Annotation::FeatureNode(_mapNode, fLists[i], *styles[i]);
		//_group->addChild(tempFeatureNode);
		for (auto& feature : fLists[i])
		{
			osg::ref_ptr<osgEarth::Annotation::FeatureNode> tempFeatureNode = new osgEarth::Annotation::FeatureNode(_mapNode, feature, *styles[i]);
			
			osg::StateSet* stateSet = tempFeatureNode->getOrCreateStateSet();
			// 设置深度偏移（确保在上方）
			stateSet->setAttribute(new osg::PolygonOffset(-1.0f, -1.0f));
			// 禁用深度写入（确保不会相互遮挡）
			stateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS));
			// 禁用深度测试（确保始终可见）
			stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			stateSet->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f), osg::StateAttribute::ON);
			// 设置渲染状态
			stateSet->setRenderBinDetails(200, "RenderBin"); // 在模型下方一层
			// 启用混合（透明效果）
			stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			stateSet->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

			_group->addChild(tempFeatureNode);
		}
	}

	for (int i = 0; i < labelList.size(); i++)
	{
		if (auto tmpLabelNode = labelList[i])
		{
			isCoordinateShow ? tmpLabelNode->setNodeMask(0xFFFFFFFF) : tmpLabelNode->setNodeMask(0x0);
			_group->addChild(tmpLabelNode);
		}
	}

	/*
	// 添加鼠标点击事件处理器
	_viewer->addEventHandler(new PickHandler(_mapNode));

	// 创建 OSG Widget 管理器
	wm = new osgWidget::WindowManager(_viewer, 1280, 720, 0);

	// 创建 UI 标牌
	infoBox = createInfoBox();
	wm->addChild(infoBox);
	*/

	//_viewer->realize();  // 确保窗口创建

	// 调整 UI 窗口大小（重要）
	//wm->resizeAllWindows();

	// 监听窗口大小变化
	//_viewer->addEventHandler(new osgWidget::ResizeHandler(wm));

	/*
	DTB_BillboardManager::instance()->setWinManager(wm.get());

	osg::Node *pos = _group->getChild(0);
	std::vector<std::string> detail({ "111" ,"222"});

	DTB_BillboardManager::instance()->addBoard("1", pos, "hello", detail);

	auto billboard = DTB_BillboardManager::instance()->getBoard("1");
	billboard->setPos(osg::Vec3(100,100,100));
	billboard->setDetailVisible(true);
	*/

	return;

	// 创建高度场
	osg::ref_ptr<osg::HeightField> hf = createHeightField(grid, minLon, minLat);

	//创建遮罩图
	osg::ref_ptr<osg::Image> image = heightFieldToImage(hf.get(), minLon, minLat);
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

void HeatMap3D::startCreateHeatMap2(SpreadInitInfo spreadInfo, bool isCoordinateShow /*= false*/, bool isGridShow /*= false*/)
{
	if (!spreadInfo.spreadtimeSec
		|| !spreadInfo.timeStepSec
		|| !spreadInfo.tempo
		|| !spreadInfo.shidu
		//|| !spreadInfo.fengxiang
		|| !spreadInfo.fengsu) {
		return;
	}

	InitMap();

	CellularAutomata * pAutomatu = new CellularAutomata();

	pAutomatu->setGridSize(spacing);

	pAutomatu->setInitFireInfo(116.1963, 40.2647, spacing, spacing);

	// 影响因素
	pAutomatu->setSpreadTime(spreadInfo.spreadtimeSec);
	pAutomatu->setTimeStep(spreadInfo.timeStepSec);
	QVector<int> weatherConditionVec;
	weatherConditionVec.push_back(spreadInfo.tempo / 10.0);
	weatherConditionVec.push_back(spreadInfo.shidu);
	weatherConditionVec.push_back(spreadInfo.fengxiang);
	weatherConditionVec.push_back(spreadInfo.fengsu);

	int spreadLayer = 0;
	std::vector<std::vector<double>> outDltVec;
	if (!pAutomatu->spreadFire(weatherConditionVec, outDltVec))
	{
		return;
	}
	spreadLayer = pAutomatu->spreadLayer();

	assignValuesWithLayers2(grid, lons, lats, values, minLon, minLat, outDltVec, spreadLayer);

	initStyles(isGridShow);

	fLists.resize(colors_Sym.size());

	createGridGeometry(grid, minLon, minLat, fLists, labelList);

	for (int i = 0; i < fLists.size(); i++)
	{
		for (auto& feature : fLists[i])
		{
			osg::ref_ptr<osgEarth::Annotation::FeatureNode> tempFeatureNode = new osgEarth::Annotation::FeatureNode(_mapNode, feature, *styles[i]);
			_group->addChild(tempFeatureNode);
		}
	}

	for (int i = 0; i < labelList.size(); i++)
	{
		if (auto tmpLabelNode = labelList[i])
		{
			isCoordinateShow ? tmpLabelNode->setNodeMask(0xFFFFFFFF) : tmpLabelNode->setNodeMask(0x0);
			_group->addChild(tmpLabelNode);
		}
	}
}

void HeatMap3D::create2DGridDrawing(QPainter& painter, IProj *pProj)
{
	if (grid.size() <= 0 && points_bound.size() < 3) {
		return;
	}

	if (grid[0].size() <= 0) {
		return;
	}

	std::vector<DPOINT> polypts;
	QPolygonF polygonBounds;

	for (auto point : points_bound) {
		DPOINT polypt(point.x(), point.y());
		polypts.push_back(polypt);
		polygonBounds << QPointF(point.x(), point.y());
	}

	double lon = minLon + spaceLon;
	double lat = minLat + spaceLat;

	QPoint p1 = pProj->Jwd2Screen(minLon, minLat);
	QPoint p2 = pProj->Jwd2Screen(lon, lat);

	double px = p2.x() - p1.x();
	double py = p2.y() - p1.y();

	QBrush brush;
	brush.setStyle(Qt::BrushStyle::SolidPattern);

	QPen pen;
	pen.setWidth(1);

	QColor qColor;

	// 网格绘制
	for (int x = 0; x < grid.size(); x++) {
		for (int y = 0; y < grid[0].size(); y++) {

			double height = grid[x][y];
			if (height < 0) continue;

			double lon = minLon + spaceLon * y;
			double lat = minLat + spaceLat * x;

			QPolygonF polygonRect;
			polygonRect << QPointF(lon, lat);
			polygonRect << QPointF(lon, lat + spaceLat);
			polygonRect << QPointF(lon + spaceLon, lat + spaceLat);
			polygonRect << QPointF(lon + spaceLon, lat);

			// 计算多边形的中心点
			double centerLon = minLon + (2 * y + 1) / 2.0 * spaceLon;
			double centerLat = minLat + (2 * x + 1) / 2.0 * spaceLat;

			// 区域交集处理
			QPolygonF intersection = AgriDataAnalysisPredictUtil::calculateIntersection(polygonBounds, polygonRect);

			// 绘制交集
			if (!intersection.isEmpty()) {

				QPolygonF intersectionScreen;
				for (const auto& pointF : intersection)
				{
					QPoint point = pProj->Jwd2Screen(pointF.x(), pointF.y());
					intersectionScreen << point;
				}

				int colorIndex = grid[x][y];
				if (colorIndex >= colors_Sym.size())
				{
					colorIndex = colors_Sym.size() - 1;
				}

				qColor = HeatMap3DUtil::osgColorToQColor(colors_Sym[colorIndex]);

				brush.setColor(qColor);
				pen.setColor(gridLineShow ? QColor(255, 255, 255, 100) : qColor);

				painter.setBrush(brush);
				painter.setPen(pen);

				painter.drawPolygon(intersectionScreen);
			}
			else
			{
				// 绘制边界外框线(暂无该需求)
			}
		}
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

void HeatMap3D::setCoordinateShowOrHide(bool isShow)
{
	if (_group != nullptr)
	{
		unsigned int count = _group->getNumChildren();
		for (int i = 0; i < count; i++)
		{
			osg::Node * childNode = _group->getChild(i);
			if (auto* labelNode = dynamic_cast<osgEarth::Annotation::LabelNode*>(childNode))
			{
				isShow ? labelNode->setNodeMask(0xFFFFFFFF) : labelNode->setNodeMask(0x0);
			}
		}
	}
}

void HeatMap3D::setGridLineShowOrHide(bool isShow)
{
	if (_group != nullptr)
	{
		unsigned int count = _group->getNumChildren();
		for (int i = 0; i < count; i++)
		{
			osg::Node * childNode = _group->getChild(i);
			if (auto* featureNode = dynamic_cast<FeatureNode*>(childNode))
			{
				Style originalStyle = featureNode->getStyle();

				if (isShow)
				{
					originalStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = HeatMap3DUtil::hexToColor("#FFFFFF", 0.45f);
					originalStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0.0;
				}
				else
				{
					originalStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = osgEarth::Color(0, 0, 0, 0);
				}
				featureNode->setStyle(originalStyle);
			}
		}
	}

	gridLineShow = isShow;
}

void HeatMap3D::InitMap(double gridAngle/* = 0.0*/)
{
	while (_group->getNumChildren() > 0)
	{
		_group->removeChild(_group->getChild(0));
	}

	maxLon = *(std::max_element(lons.begin(), lons.end()));
	minLon = *(std::min_element(lons.begin(), lons.end()));
	maxLat = *(std::max_element(lats.begin(), lats.end()));
	minLat = *(std::min_element(lats.begin(), lats.end()));

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

	// 栅格大小
	spaceLon = AgriDataAnalysisPredictUtil::metersToLongitudeDegrees((maxLat + maxLat) / 2, spacing);
	spaceLat = AgriDataAnalysisPredictUtil::metersToLatitudeDegrees(spacing);

	//行、列数
	rows = static_cast<size_t>((maxLat - minLat) / spaceLat) + 1;
	cols = static_cast<size_t>((maxLon - minLon) / spaceLon) + 1;

	if (gridAngle != 0.0)
	{
		rows *= 3;
		cols *= 3;
	}

	grid = std::vector<std::vector<double>>(rows, std::vector<double>(cols, 0.0));

	if (gridAngle != 0.0)
	{
		minLon = tempGeo.longitude();
		minLat = tempGeo.latitude();
	}
}

osg::HeightField * HeatMap3D::createHeightField(const std::vector<std::vector<double>>& grid, double minX, double minY)
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
	hf->setXInterval(spaceLon);
	hf->setYInterval(spaceLat);
	return hf;
}

osg::Image * HeatMap3D::heightFieldToImage(osg::HeightField * hf, double minlon, double minlat)
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

			if (!(polygon->contains2D(minlon + x * spaceLon, minlat + y * spaceLat)))
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

/*
diffuse_layer: disCount
equalCount: 0
*/
void HeatMap3D::assignValuesWithLayers(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats, const std::vector<double>& values, 
	int equalCount, int disCount, double minLon, double minLat, double angle /*= 0.0*/)
{
	int layer = equalCount + disCount;
	double maxDis = sqrt(pow(layer, 2) + pow(layer, 2));
	for (size_t idx = 0; idx < lons.size() && idx < lats.size() && idx < values.size(); ++idx) {

		//int y = (lons[idx] - minLon) / spaceLon;
		//int x = (lats[idx] - minLat) / spaceLat;

		int y = 0;
		int x = 0;
		double realDistance = osgEarth::GeoMath::distance(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
			osg::DegreesToRadians(lats[idx]), osg::DegreesToRadians(lons[idx]));
		double realBearing = osgEarth::GeoMath::bearing(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
			osg::DegreesToRadians(lats[idx]), osg::DegreesToRadians(lons[idx]));
		double disBearingRad = realBearing - angle;
		y = int((sin(disBearingRad) * realDistance) / spacing);
		x = int((cos(disBearingRad) * realDistance) / spacing);

		double value = values[idx];
		{
			for (int i = x - layer; i <= x + layer; ++i) {
				for (int j = y - layer; j <= y + layer; ++j) {
					if (i >= 0 && i < grid.size() && j >= 0 && j < grid[0].size())
					{
						if (abs(x - i) <= equalCount && abs(y - j) <= equalCount)
						{
							grid[i][j] = value;
						}
						else
						{
							double tmpCalcVal = value - abs(x - i) - abs(y - j);
							grid[i][j] = std::max(grid[i][j], tmpCalcVal);// 相交区域取最大
						}
					}
				}
			}
		}
	}
}

void HeatMap3D::assignValuesWithLayers2(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats,
	const std::vector<double>& values, double minLon, double minLat, std::vector<std::vector<double>>& spread, int spreadLayer)
{
	int rowCount = spread.size() / 2;
	int colCount = 0;
	if (rowCount) colCount = spread[0].size() / 2;
	if (!colCount || !colCount) return;

	for (size_t idx = 0; idx < lons.size() && idx < lats.size() && idx < values.size(); ++idx) {
		int y = (lons[idx] - minLon) / spaceLon;
		int x = (lats[idx] - minLat) / spaceLat;
		double value = values[idx];
		int dltLayer = (int)value - spreadLayer; // 层数差值
		{
			int dx = 0;
			for (int i = x - rowCount; i < x + rowCount; ++i, ++dx) {
				int dy = 0;
				for (int j = y - colCount; j < y + colCount; ++j, ++dy) {
					if (i >= 0 && i < grid.size() && j >= 0 && j < grid[0].size())
					{
						double tmpCalcVal = 0.0;
						if (dltLayer < 0) {
							// 过滤超出层级
							if (((double)spreadLayer - value) - spread[dx][dy] > FLT_EPSILON) continue;
							tmpCalcVal = spread[dx][dy] - std::abs(dltLayer);
						}
						else {
							// 匹配层级，0级缺失处理
							if (spread[dx][dy] > FLT_EPSILON) {
								tmpCalcVal = spread[dx][dy] + std::abs(dltLayer);
							}
							else {
								tmpCalcVal = spread[dx][dy];
							}

						}

						grid[i][j] = std::max(grid[i][j], tmpCalcVal);// 相交区域取最大
					}
				}
			}
		}
	}
}

void HeatMap3D::initStyles(bool isGridShow /*= false*/)
{
	styles.clear();
	for (int i = 0; i < colors_Sym.size(); i++)
	{
		osgEarth::Symbology::Style* geomStyle = new osgEarth::Symbology::Style;
		if (isGridShow)
		{
			geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = HeatMap3DUtil::hexToColor("#FFFFFF", 1.0f);
			geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 0.0;
		}
		else
		{
			geomStyle->getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = osgEarth::Color(0, 0, 0, 0);
		}

		//geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = colors_Sym[i];
		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = osgEarth::Color(colors_Sym[i].r(), colors_Sym[i].g(),colors_Sym[i].b(), 0.6f);
		//geomStyle.get()->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_GPU;
		geomStyle->getOrCreate<osgEarth::Symbology::RenderSymbol>()->depthOffset().mutable_value().enabled() = true;

		//绘制点
		//geomStyle->getOrCreate<osgEarth::PointSymbol>()->size() = 10.0f; // 点大小
		//geomStyle->getOrCreate<osgEarth::PointSymbol>()->fill()->color() = colors_Sym_forD[i];

#if 1
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_NONE;
		//geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 513.0;	//相对地面高度
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 123.0;	//相对地面高度
		//geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalScale() = 513.0;	//相对地面高度
#else
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symbology::AltitudeSymbol::CLAMP_TO_TERRAIN;
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 0.5;
#endif

		styles.push_back(geomStyle);
	}
}

void HeatMap3D::createGridGeometry(std::vector<std::vector<double>>& grid, double minLon, double minLat, 
	std::vector<osgEarth::Features::FeatureList>& featureLists, std::vector<osgEarth::Annotation::LabelNode*>& labelList, float angle /*= 0.0*/)
{
	QPolygonF polygonBounds;
	for (auto point : points_bound) {
		DPOINT polypt(point.x(), point.y());
		polygonBounds << QPointF(point.x(), point.y());
	}

	osg::Vec3d origin(minLon, minLat, 0);

	osgEarth::Symbology::Geometry* geoMery = osgEarth::Symbology::Geometry::create(osgEarth::Symbology::Geometry::TYPE_POLYGON, &points_bound);
	osgEarth::Symbology::Polygon * boundPolygon = dynamic_cast<osgEarth::Symbology::Polygon*>(geoMery);

	double minHeight = 0, maxHeight = values.size() ? values[0] : 0;

	for (int i = 1; i < values.size(); i++)
	{
		minHeight = minHeight < values[i] ? minHeight : values[i];
		maxHeight = maxHeight > values[i] ? maxHeight : values[i];
	}

	osg::EllipsoidModel* tempElisMode = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
	osgEarth::GeoPoint minlonlat(_mapNode->getMap()->getSRS(), minLon, minLat);

	for (int x = 0; x < grid.size(); x++)
	{
		for (int y = 0; y < grid[0].size(); y++)
		{
			osg::Vec3d v1, v2, v3, v4;
			{
				double v1LonR, v1LatR;
				double Tempdistance = sqrt(x * x + y * y) * spacing;
				double bearingRad = 0.0;
				if (x == 0)
				{
					bearingRad = angle + osg::DegreesToRadians(90.0);
				}
				else
				{
					bearingRad = atan((y * 1.0) / (x * 1.0)) + angle;
				}
				osgEarth::GeoMath::destination(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
					bearingRad, Tempdistance, v1LatR, v1LonR);

				v1.set(osg::Vec3d(osg::RadiansToDegrees(v1LonR), osg::RadiansToDegrees(v1LatR), 0.0));

				double v2LonR, v2LatR, v3LonR, v3LatR, v4LonR, v4LatR;
				osgEarth::GeoMath::destination(v1LatR, v1LonR, angle, spacing, v2LatR, v2LonR);
				osgEarth::GeoMath::destination(v1LatR, v1LonR, angle + osg::DegreesToRadians(45.0), spacing * sqrt(2), v3LatR, v3LonR);
				osgEarth::GeoMath::destination(v1LatR, v1LonR, angle + osg::DegreesToRadians(90.0), spacing, v4LatR, v4LonR);

				v2.set(osg::Vec3d(osg::RadiansToDegrees(v2LonR), osg::RadiansToDegrees(v2LatR), 0.0));
				v3.set(osg::Vec3d(osg::RadiansToDegrees(v3LonR), osg::RadiansToDegrees(v3LatR), 0.0));
				v4.set(osg::Vec3d(osg::RadiansToDegrees(v4LonR), osg::RadiansToDegrees(v4LatR), 0.0));
			}

			//osg::Vec3d v1 = origin + osg::Vec3d(y * spaceLon, x * spaceLat, 0);
			//osg::Vec3d v2 = v1 + osg::Vec3d(spaceLon, 0, 0);
			//osg::Vec3d v3 = v2 + osg::Vec3d(0, spaceLat, 0);
			//osg::Vec3d v4 = v1 + osg::Vec3d(0, spaceLat, 0);

			// 计算多边形的中心点
			//double centerLon = (v1.x() + v3.x()) / 2.0;
			//double centerLat = (v1.y() + v3.y()) / 2.0;

			osgEarth::Polygon* polygon = new osgEarth::Polygon();

			// 区域交集处理
			QPolygonF polygonRect;
			polygonRect << QPointF(v1.x(), v1.y());
			polygonRect << QPointF(v2.x(), v2.y());
			polygonRect << QPointF(v3.x(), v3.y());
			polygonRect << QPointF(v4.x(), v4.y());

			QPolygonF intersection = AgriDataAnalysisPredictUtil::calculateIntersection(polygonBounds, polygonRect);
			if (intersection.isEmpty()) {
				continue;
			}

			for (const auto& pointF : intersection) {
				polygon->push_back(osg::Vec3d(pointF.x(), pointF.y(), 0));
			}

			// 创建区域色块
			osg::ref_ptr<osgEarth::Annotation::Feature> feature = new osgEarth::Annotation::Feature(polygon, _mapNode->getMapSRS()->getGeographicSRS());

			double CenLonR, CenLatR;
			double bearingRad = atan(((y * 1.0) + 0.5) / ((x * 1.0) + 0.5)) + angle;
			double Tempdistance1 = sqrt(((x * 1.0) + 0.5) * ((x * 1.0) + 0.5) + ((y * 1.0) + 0.5) * ((y * 1.0) + 0.5)) * spacing;
			//destination()函数内的单位都为弧度制，需要从度°转为弧度
			osgEarth::GeoMath::destination(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
				bearingRad, Tempdistance1, CenLatR, CenLonR);

			// 计算多边形的中心点
			double centerLon = osg::RadiansToDegrees(CenLonR);
			double centerLat = osg::RadiansToDegrees(CenLatR);

			//添加颜色
			double colorIndex = grid[x][y];
			double singleGrid = sqrt(pow(1, 2) + pow(1, 2));
			if (colorIndex >= colors_Sym.size())
			{
				colorIndex = colors_Sym.size() - 1;
			}
			featureLists[colorIndex].push_back(feature);

			// 创建label
			osgEarth::Symbology::Style labelTextStyle;
			labelTextStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
			labelTextStyle.getOrCreate<TextSymbol>()->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
			labelTextStyle.getOrCreate<TextSymbol>()->fill()->color() = HeatMap3DUtil::hexToColor("#FFFFFF", 0.7f);

			GeoPoint centerPoint(_mapNode->getMapSRS()->getGeographicSRS(), centerLon, centerLat, 123.5, ALTMODE_ABSOLUTE);
			LabelNode* label = new LabelNode(_mapNode, centerPoint, QString("(%1,%2)").arg(x).arg(y).toStdString(), labelTextStyle);
			labelList.emplace_back(label);
		}
	}
}

void HeatMap3D::createPointsGeometry(const std::vector<double>& lons, const std::vector<double>& lats, const std::vector<double>& values, std::vector<osgEarth::Features::FeatureList>& featureLists)
{
	double minHeight = 0, maxHeight = values[0];

	for (int i = 1; i < values.size(); i++)
	{
		minHeight = minHeight < values[i] ? minHeight : values[i];
		maxHeight = maxHeight > values[i] ? maxHeight : values[i];
	}

	for (int idx = 0; idx < lons.size() && idx < lats.size() && idx < values.size(); idx++)
	{
		osgEarth::Symbology::Vec3dVector points;
		points.push_back(osg::Vec3d(lons[idx], lats[idx], 0.0));
		osgEarth::Geometry* pointGeometry = osgEarth::Geometry::create(osgEarth::Geometry::TYPE_POINTSET, &points);
		/*	osgEarth::Polygon* polygon = new osgEarth::Polygon();
		polygon->push_back(v1);
		polygon->push_back(v2);
		polygon->push_back(v3);
		polygon->push_back(v4);*/

		osg::ref_ptr<osgEarth::Annotation::Feature> feature = new osgEarth::Annotation::Feature(pointGeometry, _mapNode->getMapSRS()->getGeographicSRS());

		double height = values[idx];
		if (height < 0)
		{
			continue;
		}
		//添加颜色
		int colorIndex = (int)((height - minHeight) / (maxHeight - minHeight) * (styles.size() - 1));

		featureLists[colorIndex].push_back(feature);
	}
}
