#include <algorithm>
#include <cmath>
#include <QString>
#include <QVector>
#include <QDebug>
#include <QPolygonF>
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
#include <Eigen/Dense>

#include <QGeoCoordinate>
#include <osgEarth\GeoMath>
#include <osg/PolygonOffset>
#include <osg/CoordinateSystemNode>

//#include "osgTools/DTB_BillboardManager.h"
#include "AgriSoilAnalysisPredictUtil.h"
#include "KrigingInterPolator.h"


using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Symbology;

using namespace Eigen;

#include <QVector>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrent>


// 存储结果结构体
struct GridResult {
	double centerLon;
	double centerLat;
	double value;
};

class GridWorker : public QRunnable
{
public:
	GridWorker(int startRow, int endRow, int cols,
		double minLat, double minLon, double m_step, double gridAngle,
		std::function<double(double, double)> estimateFunc,
		QVector<GridResult>& resultVec, QMutex* mutex)
		: startRow(startRow), endRow(endRow), cols(cols),
		minLat(minLat), minLon(minLon), m_step(m_step), gridAngle(gridAngle),
		estimateFunc(estimateFunc), resultVec(resultVec), mutex(mutex) {}

	void run() override
	{
		for (int x = startRow; x < endRow; ++x)
		{
			for (int y = 0; y < cols; ++y)
			{
				double v1LonR, v1LatR;
				double Tempdistance = sqrt(x * x + y * y) * m_step;
				double bearingRad = (x == 0)
					? gridAngle + osg::DegreesToRadians(90.0)
					: atan((y * 1.0) / (x * 1.0)) + gridAngle;

				osgEarth::GeoMath::destination(osg::DegreesToRadians(minLat), osg::DegreesToRadians(minLon),
					bearingRad, Tempdistance, v1LatR, v1LonR);
				osg::Vec3d v1(osg::RadiansToDegrees(v1LonR), osg::RadiansToDegrees(v1LatR), 0.0);

				double v2LonR, v2LatR, v3LonR, v3LatR, v4LonR, v4LatR;
				osgEarth::GeoMath::destination(v1LatR, v1LonR, gridAngle, m_step, v2LatR, v2LonR);
				osgEarth::GeoMath::destination(v1LatR, v1LonR, gridAngle + osg::DegreesToRadians(45.0), m_step * sqrt(2), v3LatR, v3LonR);
				osgEarth::GeoMath::destination(v1LatR, v1LonR, gridAngle + osg::DegreesToRadians(90.0), m_step, v4LatR, v4LonR);

				osg::Vec3d v3(osg::RadiansToDegrees(v3LonR), osg::RadiansToDegrees(v3LatR), 0.0);
				double centerLon = (v1.x() + v3.x()) / 2.0;
				double centerLat = (v1.y() + v3.y()) / 2.0;
				double value = estimateFunc(centerLon, centerLat);

				GridResult result;
				result.centerLon = centerLon;
				result.centerLat = centerLat;
				result.value = value;

				QMutexLocker locker(mutex);
				resultVec.append(result);
			}
		}
	}

private:
	int startRow, endRow, cols;
	double minLat, minLon, m_step, gridAngle;
	std::function<double(double, double)> estimateFunc;
	QVector<GridResult>& resultVec;
	QMutex* mutex;
};


class GridGeometryWorker : public QRunnable
{
public:
	GridGeometryWorker(int startRow, int endRow,
		const std::vector<std::vector<double>>& grid,
		double minLon, double minLat, float angle,
		QMutex* mutex,
		std::function<void(std::vector<std::vector<double>>&, double, double
			, std::vector<osgEarth::Features::FeatureList>&, std::vector<osgEarth::Annotation::LabelNode*>&
			, float angle, int, int)> gridGeometryFunc,
		std::vector<osgEarth::Features::FeatureList>* sharedFeatureLists,
		std::vector<osgEarth::Annotation::LabelNode*>* sharedLabelList)
		: _startRow(startRow), _endRow(endRow), _grid(grid),
		_minLon(minLon), _minLat(minLat), _angle(angle), _gridGeometryFunc(gridGeometryFunc),
		_mutex(mutex), _sharedFeatureLists(sharedFeatureLists), _sharedLabelList(sharedLabelList)
	{}

	void run() override
	{
		std::vector<osgEarth::Features::FeatureList> localFeatures;
		std::vector<osgEarth::Annotation::LabelNode*> localLabels;

		_gridGeometryFunc(_grid, _minLon, _minLat, localFeatures, localLabels, _angle, _startRow, _endRow);

		// 合并结果（线程安全）
		QMutexLocker locker(_mutex);
		_sharedFeatureLists->insert(_sharedFeatureLists->end(), localFeatures.begin(), localFeatures.end());
		_sharedLabelList->insert(_sharedLabelList->end(), localLabels.begin(), localLabels.end());
	}

private:
	int _startRow, _endRow;
	std::vector<std::vector<double>> _grid;
	double _minLon, _minLat;
	float _angle;
	QMutex* _mutex;
	std::function<void(std::vector<std::vector<double>>&, double, double, 
		std::vector<osgEarth::Features::FeatureList>&, 
		std::vector<osgEarth::Annotation::LabelNode*>&, float angle, int, int)> _gridGeometryFunc;
	std::vector<osgEarth::Features::FeatureList>* _sharedFeatureLists;
	std::vector<osgEarth::Annotation::LabelNode*>* _sharedLabelList;
};


osgEarth::Symbology::Color HeatMap3DUtil::hexToColor(const std::string& hexColor, float rgb_a/* = 1.0f*/)
{
	unsigned int r, g, b;
	if (sscanf(hexColor.c_str(), "#%02X%02X%02X", &r, &g, &b) == 3)
	{
		return osgEarth::Symbology::Color(r / 255.0f, g / 255.0f, b / 255.0f, rgb_a); // 透明度默认为 1.0
	}
	return osgEarth::Symbology::Color(); // 失败返回默认颜色
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

////////////////////////////////////////////////////////////////////////////////////////////////

KrigingInterpolator::Point::Point(double dLon, double dLat, double dValue)
	: lon(dLon), lat(dLat), value(dValue)
{
}

KrigingInterpolator::KrigingInterpolator(double step, int neighbors, double range)
	: m_neighbors(neighbors), m_step(step)
{
	/*m_range = AgriSoilAnalysisPredictUtil::metersToLatitudeDegrees(range);*/
}

void KrigingInterpolator::setSamples(const std::vector<Point>& samples) 
{
	m_samples = samples;
}

double KrigingInterpolator::gaussianCov(double d) const 
{
	return m_sill * exp(-(d * d) / (m_range * m_range));
}

double KrigingInterpolator::distance(double lon1, double lat1, double lon2, double lat2) const 
{
	double dx = lon1 - lon2;
	double dy = lat1 - lat2;
	return sqrt(dx * dx + dy * dy);
}

void KrigingInterpolator::autoTuneSill()
{
	int n = m_samples.size();
	if (n < 2) return;

	double mean = 0.0;
	for (const auto& p : m_samples) {
		mean += p.value;
	}
	mean /= n;

	double var = 0.0;
	for (const auto& p : m_samples) {
		double diff = p.value - mean;
		var += diff * diff;
	}
	var /= (n - 1); // 无偏估计

	m_sill = var;
}

void KrigingInterpolator::autoTuneRange(double scale /*= 1.0*/)
{
	int n = m_samples.size();
	if (n < 2) return;

	double totalDist = 0.0;
	int count = 0;

	for (int i = 0; i < n; ++i) {
		for (int j = i + 1; j < n; ++j) {
			double d = distance(
				m_samples[i].lon, m_samples[i].lat,
				m_samples[j].lon, m_samples[j].lat
			);
			totalDist += d;
			count++;
		}
	}

	if (count > 0) {
		double avgDist = totalDist / count;
		m_range = avgDist * scale;
	}
}


double KrigingInterpolator::estimate(double lon, double lat) const
{
	int total = m_samples.size();
	if (total == 0) return 0.0;

	// 选出最近的 k 个样本点
	std::vector<std::pair<double, int>> dists;
	for (int i = 0; i < total; ++i) {
		double d = distance(lon, lat, m_samples[i].lon, m_samples[i].lat);
		dists.emplace_back(d, i);
	}

	sort(dists.begin(), dists.end());
	int k = std::min(m_neighbors, total);

	MatrixXd C(k, k);
	VectorXd c(k), z(k);

	for (int i = 0; i < k; ++i) {
		int idx_i = dists[i].second;
		z(i) = m_samples[idx_i].value;
		for (int j = 0; j < k; ++j) {
			int idx_j = dists[j].second;
			double d = distance(
				m_samples[idx_i].lon, m_samples[idx_i].lat,
				m_samples[idx_j].lon, m_samples[idx_j].lat
			);
			C(i, j) = gaussianCov(d);
		}
	}

	for (int i = 0; i < k; ++i) {
		int idx = dists[i].second;
		double d = distance(lon, lat, m_samples[idx].lon, m_samples[idx].lat);
		c(i) = gaussianCov(d);
	}

	VectorXd lambda = C.ldlt().solve(c);

	// return std::max(0.0, lambda.dot(z)); // 插值存在生成的值出现负值， 简单解决不会影响插值平滑性
	return lambda.dot(z);
}


void KrigingInterpolator::generateGrid(const std::vector<QPointF>& boundPoings, 
	std::vector<double>& lons, std::vector<double>& lats, std::vector<double>& values, 
	double gridAngle)
{
	autoTuneSill();
	autoTuneRange(0.5); 

	double maxLon = *(std::max_element(lons.begin(), lons.end()));
	double minLon = *(std::min_element(lons.begin(), lons.end()));
	double maxLat = *(std::max_element(lats.begin(), lats.end()));
	double minLat = *(std::min_element(lats.begin(), lats.end()));

	// 添加边界
	for (int i = 0; i < boundPoings.size(); i++)
	{
		maxLon = maxLon > boundPoings[i].x() ? maxLon : boundPoings[i].x();
		minLon = minLon < boundPoings[i].x() ? minLon : boundPoings[i].x();
		maxLat = maxLat > boundPoings[i].y() ? maxLat : boundPoings[i].y();
		minLat = minLat < boundPoings[i].y() ? minLat : boundPoings[i].y();
	}

	QGeoCoordinate cenGeo((maxLat + minLat) / 2, (maxLon + minLon) / 2);
	QGeoCoordinate x1Geo(minLat, minLon);
	QGeoCoordinate x2Geo(maxLat, minLon);
	QGeoCoordinate x3Geo(maxLat, maxLon);
	double angle_1To3 = x1Geo.azimuthTo(x3Geo);
	int dis = cenGeo.distanceTo(x1Geo) + cenGeo.distanceTo(x2Geo);

	QGeoCoordinate tempGeo = cenGeo.atDistanceAndAzimuth(dis, 180 + osg::RadiansToDegrees(gridAngle) + angle_1To3);
	if (gridAngle != 0.0)
	{
		minLon = tempGeo.longitude();
		minLat = tempGeo.latitude();
	}

	lons.clear();
	lats.clear();
	values.clear();

	// 栅格大小
	double spaceLon = AgriSoilAnalysisPredictUtil::metersToLongitudeDegrees((minLat + maxLat) / 2, m_step);
	double spaceLat = AgriSoilAnalysisPredictUtil::metersToLatitudeDegrees(m_step);

	//行、列数
	size_t rows = static_cast<size_t>((maxLat - minLat) / spaceLat) + 1;
	size_t cols = static_cast<size_t>((maxLon - minLon) / spaceLon) + 1;

	if (gridAngle != 0.0)
	{
		rows *= 3;
		cols *= 3;
	}

	int threadCount = QThread::idealThreadCount();
	QVector<GridResult> resultVec;
	QMutex mutex;
	QThreadPool pool;
	pool.setMaxThreadCount(threadCount);

	int rowsPerThread = rows / threadCount;
	int extraRows = rows % threadCount;

	auto estimateFunc = [&](double lon, double lat) -> double {
		return estimate(lon, lat); // estimate线程安全
	};

	// 启动线程池
	for (int i = 0; i < threadCount; ++i)
	{
		int startRow = i * rowsPerThread;
		int endRow = (i == threadCount - 1) ? rows : (startRow + rowsPerThread);

		GridWorker* worker = new GridWorker(startRow, endRow, cols, minLat, minLon, m_step, gridAngle,
			estimateFunc, resultVec, &mutex);
		pool.start(worker);
	}

	pool.waitForDone();

	for (const auto& r : resultVec)
	{
		lons.push_back(r.centerLon);
		lats.push_back(r.centerLat);
		values.push_back(r.value);
	}
}


void KrigingInterpolator::rescaleValue(std::vector<double>& values, double min_val, double max_val)
{
	if (values.empty()) return;

	// 查找原始最小值和最大值
	double vmin = *std::min_element(values.begin(), values.end());
	double vmax = *std::max_element(values.begin(), values.end());

	if (vmax - vmin < 1e-12) {
		std::cout << "[rescaleValue] Warning: Constant input values, setting all to min_val\n";
		for (double& v : values) v = min_val;
		return;
	}

	// 映射并裁剪
	for (double& v : values) {
		v = min_val + (v - vmin) / (vmax - vmin) * (max_val - min_val);
		v = std::min(max_val, std::max(min_val, v)); // 可选clip
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////


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

void HeatMap3D::startCreateHeatMap(bool isCoordinateShow /*= false*/, bool isGridShow /*= false*/, double gridAngle/* = 0.0*/)
{
	InitMap(gridAngle);

	assignValuesWithLayers(grid, lons, lats, values, equalCount, disCount, minLon, minLat, gridAngle);

	initStyles(isGridShow);

	fLists.clear();
	labelList.clear();

	fLists.resize(colors_Sym.size());

#if 0
	createGridGeometry(grid, minLon, minLat, fLists, labelList, gridAngle, 0, grid.size());
#else
	// 多线程计算大数据
	{
		int threadCount = QThread::idealThreadCount();
		int totalRows = static_cast<int>(grid.size());
		int rowsPerThread = totalRows / threadCount;
		int extraRows = totalRows % threadCount;

		QThreadPool pool;
		pool.setMaxThreadCount(threadCount);

		QMutex mutex;

		auto gridGeometryFunc = [&](
			std::vector<std::vector<double>>& grid, 
			double minLon, double minLat, 
			std::vector<osgEarth::Features::FeatureList>& featureLists, 
			std::vector<osgEarth::Annotation::LabelNode*>& labelList, float angle, int startRow, int endRow) -> void {
			createGridGeometry(grid, minLon, minLat, fLists, labelList, gridAngle, startRow, endRow);
		};

		for (int i = 0; i < threadCount; ++i)
		{
			int startRow = i * rowsPerThread;
			int endRow = (i == threadCount - 1) ? totalRows : (startRow + rowsPerThread);

			auto* worker = new GridGeometryWorker(startRow, endRow, grid, minLon, minLat, gridAngle,
				&mutex, gridGeometryFunc, &fLists, &labelList);
			pool.start(worker);
		}

		pool.waitForDone(); // 等待所有线程完成
	}
#endif

	for (int i = 0; i < fLists.size(); i++)
	{
		for (auto& feature : fLists[i])
		{
			osg::ref_ptr<osgEarth::Annotation::FeatureNode> tempFeatureNode = new osgEarth::Annotation::FeatureNode(_mapNode, feature, *styles[i]);
			osg::StateSet* stateSet = tempFeatureNode->getOrCreateStateSet();
			//启用深度测试
			stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
			// 设置渲染状态
			stateSet->setRenderBinDetails(200, "RenderBin"); // 在模型下方一层
			stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
			// 启用混合（透明效果）
			stateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
			stateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
			stateSet->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
			//设置深度偏移（确保在上方）
			stateSet->setAttribute(new osg::PolygonOffset(-1.0f, -1.0f));
			//禁用深度写入（确保不会相互遮挡）
			stateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS));
			// 禁用深度测试（确保始终可见）
			stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
			stateSet->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f), osg::StateAttribute::ON);
			_group->addChild(tempFeatureNode);
		}
	}

	for (int i = 0; i < labelList.size(); i++)
	{
		if (auto &tmpLabelNode = labelList[i])
		{
			//isCoordinateShow ? tmpLabelNode->setNodeMask(0xFFFFFFFF) : tmpLabelNode->setNodeMask(0x0); // TODO: 当前默认显示
			tmpLabelNode->setNodeMask(0xFFFFFFFF);
			_group->addChild(tmpLabelNode);
		}
	}
}

void HeatMap3D::create2DGridDrawing(QPainter& painter, IProj *pProj)
{
	if (grid.size() <= 0 && points_bound.size() < 3){
		return;
	}

	if (grid[0].size() <= 0){
		return;
	}

	std::vector<DPOINT> polypts;
	QPolygonF polygonBounds;

	for (auto point : points_bound){
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
			QPolygonF intersection = AgriSoilAnalysisPredictUtil::calculateIntersection(polygonBounds, polygonRect);

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

	// 采集点绘制
	int pointSize = 30;
	QFont font("Arial", pointSize);
	painter.setFont(font);

	// 设置颜色（带透明度（0-255，128 代表 50% 透明度））
	QColor textColor(205, 220, 57);
	textColor.setAlpha(200);
	pen.setColor(textColor);
	painter.setPen(pen);

	// 获取文本尺寸
	QFontMetrics fm(font);
	int textHeight = fm.ascent();

	long screenSpace = pProj->Map2ScreenDistance(spacing);

	for (int i = 0; i < labelList.size(); i++)
	{
		if (auto tmpLabelNode = labelList[i])
		{
			osgEarth::GeoPoint geoPoint = tmpLabelNode->getPosition();
			std::string labelText = tmpLabelNode->text();
			QPoint point = pProj->Jwd2Screen(geoPoint.x(), geoPoint.y());

			if (pointIdShow)
			{
				painter.setRenderHint(QPainter::TextAntialiasing);

				QFont font("Arial", 12);
				painter.setFont(font);
				QRect rect(point.x() - screenSpace / 2, point.y() - screenSpace / 2, screenSpace, screenSpace);

				painter.drawRect(rect); 
				painter.drawText(rect, Qt::AlignCenter | Qt::TextWordWrap, tr2(labelText.c_str()));
			}
			else
			{
				painter.drawText(QPoint(point.x() - textHeight * 0.2, point.y() + textHeight * 0.6), "*");
			}
		}
	}
}

void HeatMap3D::generateSamplesData(AgriSoilTestingCollectionPtrVec& soilTestingCollectVec)
{
	soilTestingCollectVec.assign(m_soilTestingCollectionVec.begin(), m_soilTestingCollectionVec.end());
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

	coordinateShow = isShow;
}

void HeatMap3D::setPointIdShowOrHide(bool isShow)
{
	/* TODO：换标点展示，考虑多文本存储展示
	if (_group != nullptr)
	{
		unsigned int count = _group->getNumChildren();
		for (int i = 0; i < count; i++)
		{
			osg::Node * childNode = _group->getChild(i);
			if (auto* labelNode = dynamic_cast<osgEarth::Annotation::LabelNode*>(childNode))
			{
				Style originalStyle = labelNode->getStyle();

				if (isShow)
				{
					originalStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = HeatMap3DUtil::hexToColor("#FFFFFF", 0.3f);
					originalStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke()->width() = 1.0;
				}
				else
				{
					originalStyle.getOrCreate<osgEarth::Symbology::LineSymbol>()->stroke().mutable_value().color() = osgEarth::Color(0, 0, 0, 0);
				}
				labelNode->setStyle(originalStyle);
			}
		}
	}
	*/

	pointIdShow = isShow;
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

void HeatMap3D::setGridSampleInfo(SampleType sampleType /*= SampleType::CELL_SAMPLING*/, int sampleMode /*= 1*/)
{
	_sampleType = sampleType;
	_sampleMode = sampleMode;
}

void HeatMap3D::InitMap(double gridAngle/* = 0.0*/)
{
	if (points_bound.size() < 3)
	{
		qDebug() << "ERROR: bound points set error!";
		return;
	}

	while (_group->getNumChildren() > 0)
	{
		_group->removeChild(_group->getChild(0));
	}

	if (lons.size() && lons.size())
	{
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
	}
	else
	{
		maxLon = points_bound[0].x();
		minLon = points_bound[0].x();
		maxLat = points_bound[0].y();
		minLat = points_bound[0].y();

		for (int i = 1; i < points_bound.size(); i++)
		{
			maxLon = maxLon > points_bound[i].x() ? maxLon : points_bound[i].x();
			minLon = minLon < points_bound[i].x() ? minLon : points_bound[i].x();
			maxLat = maxLat > points_bound[i].y() ? maxLat : points_bound[i].y();
			minLat = minLat < points_bound[i].y() ? minLat : points_bound[i].y();
		}
	}

	QGeoCoordinate cenGeo((maxLat + minLat) / 2, (maxLon + minLon) / 2);
	QGeoCoordinate x1Geo(minLat, minLon);
	QGeoCoordinate x2Geo(maxLat, minLon);
	QGeoCoordinate x3Geo(maxLat, maxLon);
	double angle_1To3 = x1Geo.azimuthTo(x3Geo);
	int dis = cenGeo.distanceTo(x1Geo) + cenGeo.distanceTo(x2Geo);

	QGeoCoordinate tempGeo = cenGeo.atDistanceAndAzimuth(dis, 180 + osg::RadiansToDegrees(gridAngle) + angle_1To3);

	//行、列数
	rows = static_cast<size_t>((maxLat - minLat) / spaceLat) + 1;
	cols = static_cast<size_t>((maxLon - minLon) / spaceLon) + 1;

	if (gridAngle != 0.0)
	{
		rows *= 3;
		cols *= 3;
	}

	// 栅格大小
	spaceLon = AgriSoilAnalysisPredictUtil::metersToLongitudeDegrees((maxLat + maxLat) / 2, spacing);
	spaceLat = AgriSoilAnalysisPredictUtil::metersToLatitudeDegrees(spacing);

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
void HeatMap3D::assignValuesWithLayers(std::vector<std::vector<double>>& grid, const std::vector<double>& lons, const std::vector<double>& lats,
	const std::vector<double>& values, int equalCount, int disCount, double minLon, double minLat, double angle /*= 0.0*/)
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

	// 插入底色
	//colors_Sym.insert(colors_Sym.begin(), HeatMap3DUtil::hexToColor("#30913C", 0.5f));

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
		geomStyle->getOrCreate<osgEarth::Symbology::PolygonSymbol>()->fill()->color() = osgEarth::Color(colors_Sym[i].r(), colors_Sym[i].g(), colors_Sym[i].b(), 0.6f);
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
		//geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalScale() = 150.0;	//相对地面高度
#else
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->technique() = osgEarth::Symbology::AltitudeSymbol::TECHNIQUE_DRAPE; // 可选，也可以用 TECHNIQUE_NONE
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->clamping() = osgEarth::Symboloy::gAltitudeSymbol::CLAMP_NONE; // 不贴地
		geomStyle->getOrCreate<osgEarth::Symbology::AltitudeSymbol>()->verticalOffset() = 100;	//相对地面高度(单位：米（meters）)
#endif

		styles.emplace_back(geomStyle);
	}
}

void HeatMap3D::createGridGeometry(std::vector<std::vector<double>>& grid, double minLon, double minLat,
	std::vector<osgEarth::Features::FeatureList>& featureLists, std::vector<osgEarth::Annotation::LabelNode*>& labelList
	, float angle /*= 0.0*/, int _startRow/* = 0*/, int _endRow/* = 0*/)
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
	osgEarth::GeoPoint minlonlat(_mapNode->getMap()->getSRS(), minLon, minLat, 123.5, ALTMODE_ABSOLUTE);

	// 数据清理
	m_soilTestingCollectionVec.clear();

	for (int x = _startRow; x < _endRow/*grid.size()*/; x++)
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

			QPolygonF intersection = AgriSoilAnalysisPredictUtil::calculateIntersection(polygonBounds, polygonRect);
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
			int colorIndex = grid[x][y];
			if (colorIndex >= colors_Sym.size())
			{
				colorIndex = colors_Sym.size() - 1;
			}
			featureLists[colorIndex].push_back(feature);

			// 创建label
			Style labelTextStyle;
			labelTextStyle.getOrCreate<osgEarth::Symbology::TextSymbol>()->encoding() = osgEarth::Symbology::TextSymbol::ENCODING_UTF8;
			if (TextSymbol* textSymbol = labelTextStyle.getOrCreate<TextSymbol>())
			{
				textSymbol->alignment() = TextSymbol::ALIGN_CENTER_CENTER;
				textSymbol->fill()->color() = HeatMap3DUtil::hexToColor("#FFFFFF", 1.0f);
				textSymbol->size() = 30.0f;
			}

			// 采样类型
			switch (_sampleType)
			{
			case SampleType::CELL_SAMPLING:
			{
				std::vector<QPointF> vecRandomPonit;

				std::vector<QPointF> q_points_bound;
				q_points_bound.emplace_back(v1.x(), v1.y());
				q_points_bound.emplace_back(v2.x(), v2.y());
				q_points_bound.emplace_back(v3.x(), v3.y());
				q_points_bound.emplace_back(v4.x(), v4.y());

				// 单元采样
				CellSample pCellSample = static_cast<CellSample>(_sampleMode);
				switch (pCellSample)
				{
				case CellSample::RANDOM:
				{
					// 随机点
					vecRandomPonit = AgriSoilAnalysisPredictUtil::GenerateRandomPointsInPolygon(q_points_bound, 15, AgriSoilAnalysisPredictUtil::SamplingMode::Random);
				}
				break;
				case CellSample::SYSTEMATICAL_STAGGERED:
				{
					// 交错点
					vecRandomPonit = AgriSoilAnalysisPredictUtil::GenerateRandomPointsInPolygon(q_points_bound, 15, AgriSoilAnalysisPredictUtil::SamplingMode::Diagonal);
				}
				break;
				case CellSample::S_SHAPED:
				{
					// S点
					vecRandomPonit = AgriSoilAnalysisPredictUtil::GenerateRandomPointsInPolygon(q_points_bound, 15, AgriSoilAnalysisPredictUtil::SamplingMode::SShape);
				}
				break;
				case CellSample::PLUM_BLOSSOM:
				{
					// 梅花点
					vecRandomPonit = AgriSoilAnalysisPredictUtil::GenerateRandomPointsInPolygon(q_points_bound, 15, AgriSoilAnalysisPredictUtil::SamplingMode::PlumBlossom);
				}
				break;
				default:
					break;
				}
				
				for (int i = 0; i < vecRandomPonit.size(); i++) {
					const QPointF& pPoint = vecRandomPonit[i];

					// 过滤
					if (boundPolygon->contains2D(pPoint.x(), pPoint.y())){
						GeoPoint centerPoint(_mapNode->getMapSRS()->getGeographicSRS(), pPoint.x(), pPoint.y(), 123.5, ALTMODE_ABSOLUTE);
						if (LabelNode* label = new LabelNode(_mapNode, centerPoint, "*", labelTextStyle)) {
							labelList.emplace_back(label);
						}
					}
				}

				if (boundPolygon->contains2D(centerLon, centerLat)) {
				
					// 生成采样点空数据（网格中心点） TODO: 暂未考虑边界交点情况
					AgriSoilTestingCollectionPtr soilTestingCollect = std::make_shared<AgriSoilTestingCollection>();
					soilTestingCollect->guid = AgriSoilAnalysisPredictUtil::GenerateUuid().toStdString();
					soilTestingCollect->name = QString("CT_%1_%2").arg(x + 1).arg(y + 1).toStdString();
					soilTestingCollect->lon = centerLon;
					soilTestingCollect->lat = centerLat;
					soilTestingCollect->alt = 0;

					m_soilTestingCollectionVec.emplace_back(soilTestingCollect);
				}

				break;
			}
			case SampleType::POINT_SAMPLING: 
			{
				LabelNode* label = nullptr;
				GeoPoint calcPoint;

				// 点采样
				PointSample pPointSample = static_cast<PointSample>(_sampleMode);
				switch (pPointSample)
				{
				case PointSample::UNIFORM_GRID:
				{
					calcPoint = GeoPoint(_mapNode->getMapSRS()->getGeographicSRS(), v3.x(), v3.y(), 123.5, ALTMODE_ABSOLUTE);
				}
				break;
				case PointSample::OFFSET_GRID:
				{
					if ((x % 2 && (y + 1) % 2) || (y % 2 && (x + 1) % 2))
					{
						calcPoint = GeoPoint(_mapNode->getMapSRS()->getGeographicSRS(), v3.x(), v3.y(), 123.5, ALTMODE_ABSOLUTE);
					}
				}
				break;
				case PointSample::STRATIFIED_GRID:
				{
					calcPoint = GeoPoint(_mapNode->getMapSRS()->getGeographicSRS(), centerLon, centerLat, 123.5, ALTMODE_ABSOLUTE);
				}
				break;
				case PointSample::RECTANGULAR_GRID:
				{
					if (x % 2)
					{
						calcPoint = GeoPoint(_mapNode->getMapSRS()->getGeographicSRS(), v3.x(), v3.y(), 123.5, ALTMODE_ABSOLUTE);
					}
				}
				break;
				default:
					break;
				}

				// 过滤
				if (boundPolygon->contains2D(calcPoint.x(), calcPoint.y())) {
					std::string pointId = QString("CT_%1_%2").arg(x + 1).arg(y + 1).toStdString();

					if (LabelNode* label = new LabelNode(_mapNode, calcPoint, pointId, labelTextStyle)) {
						labelList.emplace_back(label);
					}

					// 生成采样点空数据（网格交点） TODO: 暂未考虑边界交点情况
					AgriSoilTestingCollectionPtr soilTestingCollect = std::make_shared<AgriSoilTestingCollection>();
					soilTestingCollect->guid = AgriSoilAnalysisPredictUtil::GenerateUuid().toStdString();
					soilTestingCollect->name = pointId;
					soilTestingCollect->lon = calcPoint.x();
					soilTestingCollect->lat = calcPoint.y();
					soilTestingCollect->alt = 0;

					m_soilTestingCollectionVec.emplace_back(soilTestingCollect);
				}

				break;
			}
			case SampleType::AREA_SAMPLING:
			{
				break;
			}
			default: break;
			}
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
