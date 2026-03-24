#include <QDebug>
#include <QMetaObject>

#include <osg/Depth>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/PolygonOffset>
#include <osg/CoordinateSystemNode>

#include "AgriDataAnalysisDBHandler.h"
#include "LY_AgriDataAnalysisPredict.h"
#include "IAgriDataAnalysisPredictServiceImpl.h"
#include "AgriPestAndDiseaseDistributeWidget.h"

#include "legend/AgriPatrolLegend.h"



struct qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImplPrivate
{
	IAgriDataAnalysisPredictServiceImplPrivate() {};
	~IAgriDataAnalysisPredictServiceImplPrivate() {};

	AgriPatrolLegend* _PatrolLegend = nullptr;
	std::vector<osg::ref_ptr<osg::Geode>> _GeodeVec;
};

qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::IAgriDataAnalysisPredictServiceImpl()
	:_p(new IAgriDataAnalysisPredictServiceImplPrivate())
{
}

bool qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::getActiveFieldBounds(std::vector<QPointF> & vBoundPoings) const
{
	QMutexLocker locker(&m_mutex);

	qnzkna::ZoneManage::CZoneBaseVec zoneBaseVec = IAgriDataAnalysisPredictActivator::Get().getPIZoneManageService()->GetAllZoneBase();

	for (const auto &itrZone : zoneBaseVec)
	{
		QString area_id = u8"彭州盐田1";
		if (0 == area_id.compare(QString::fromLocal8Bit(itrZone.strID)))
		{
			for (auto zone : itrZone.stZonePointVec)
			{
				QPointF pointF;
				pointF.setX(zone.dLon);
				pointF.setY(zone.dLat);
				vBoundPoings.push_back(pointF);
			}
			return true;
		}
	}
	return false;
}

bool qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::generatePestDistributeMap() const
{
	AgriPestAndDiseaseAnalysisWidget* pWidget = LY_AgriDataAnalysisPredict::Get().GetAgriPestAnalysisWidget();

	QMetaObject::invokeMethod(pWidget, [=]() {

		QMutexLocker locker(&m_mutex);

		if (pWidget)
		{
			LY_AgriDataAnalysisPredict::Get().ReCreateDBHandler();
			AgrisprayComboBox* pComboBox = pWidget->getAgrisprayComboBox();
			if (pComboBox)
			{
				AgriFieldInfo fieldInfo;
				fieldInfo.id = u8"47658e87-d320-45bb-b0b3-9607c3ba52d2";
				fieldInfo.farm_id = u8"d49e2a82-2bc4-4345-99e2-bafd57b4c3d8";
				fieldInfo.area_id = u8"彭州盐田1";
				fieldInfo.name = u8"高标准盐田1";
				fieldInfo.category = u8"耕地";
				fieldInfo.description;
				fieldInfo.crop = 1;
				fieldInfo.crop_variety = 1;
				fieldInfo.plant_standard = 1001;
				fieldInfo.sow_model = 1001;
				pComboBox->setActiveFieldInfo(fieldInfo);
			}

			AgriPestAndDiseaseDistributeWidget* pDisWidget = pWidget->getPestAndDiseaseDataCollectWidget();
			if (pDisWidget)
			{
				pDisWidget->cleanUp();

				AgriPestDataset dataset;
				dataset.guid = u8"c859aaa4-731e-4602-aacb-1f7dd6c09b33";
				dataset.name = u8"P_01";
				dataset.field_id = u8"47658e87-d320-45bb-b0b3-9607c3ba52d2";
				dataset.crop_type = 3;
				dataset.collect_type = 1;
				dataset.begin_time;
				dataset.end_time;
				dataset.pest_disease = 1;
				dataset.pest_category = 1;
				pDisWidget->setCurrentDataset(std::make_shared<AgriPestDataset>(dataset));
				pDisWidget->on_btn_generate_clicked();
			}
		}

	}, Qt::QueuedConnection);

	return false;
}

void qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::cleanupMap() const
{
	AgriPestAndDiseaseAnalysisWidget* pWidget = LY_AgriDataAnalysisPredict::Get().GetAgriPestAnalysisWidget();
	QMetaObject::invokeMethod(pWidget, [=]() {

		QMutexLocker locker(&m_mutex);

		if (pWidget){
			AgriPestAndDiseaseDistributeWidget* pDisWidget = pWidget->getPestAndDiseaseDataCollectWidget();
			if (pDisWidget){
				pDisWidget->cleanUp();
			}
		}
	}, Qt::QueuedConnection);
}

bool qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::generateMutilspectralMap() const
{
	AgriPestAndDiseaseAnalysisWidget* pWidget = LY_AgriDataAnalysisPredict::Get().GetAgriPestAnalysisWidget();
	QMetaObject::invokeMethod(pWidget, [=]() {

		QMutexLocker locker(&m_mutex);

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
		for (const auto& geode : _p->_GeodeVec) {
			if (geode) {
				mapNode->removeChild(geode);
			}
		}
		_p->_GeodeVec.clear();

		// 设置浮空高度（以米为单位）
		double relativeHeight = 123.0/*513.0*/;

		// 加载影像 (AGRI_SENTINEL_IMAGE_PATH_PREFIX+)
		std::string tifPath =  "sentinel_image/GNDVI.png";
		osg::ref_ptr<osg::Image> image = osgDB::readImageFile(tifPath);
		if (!image)
		{
		std::cerr << "无法加载影像文件: " << tifPath << std::endl;
		return 1;
		}

		// 手动影像的地理范围（以 WGS84 度为单位）：
		double lon_min = 104.08904939999999;
		double lat_min = 30.946691699999999;
		double lon_max = 104.09006599999999;
		double lat_max = 30.947580200000001;

		// 创建 SRS（WGS84 地理坐标系）
		osgEarth::SpatialReference* srs = osgEarth::SpatialReference::get("wgs84");

		// 将四角地理坐标转换为地心坐标（ECEF）
		osg::Vec3d ll, lr, ur, ul;
		srs->getEllipsoid()->convertLatLongHeightToXYZ(
		osg::DegreesToRadians(lat_min), osg::DegreesToRadians(lon_min), relativeHeight, ll.x(), ll.y(), ll.z());
		srs->getEllipsoid()->convertLatLongHeightToXYZ(
		osg::DegreesToRadians(lat_min), osg::DegreesToRadians(lon_max), relativeHeight, lr.x(), lr.y(), lr.z());
		srs->getEllipsoid()->convertLatLongHeightToXYZ(
		osg::DegreesToRadians(lat_max), osg::DegreesToRadians(lon_max), relativeHeight, ur.x(), ur.y(), ur.z());
		srs->getEllipsoid()->convertLatLongHeightToXYZ(
		osg::DegreesToRadians(lat_max), osg::DegreesToRadians(lon_min), relativeHeight, ul.x(), ul.y(), ul.z());

		// 构建纹理四边形
		osg::ref_ptr<osg::Geometry> quad = new osg::Geometry();
		osg::ref_ptr<osg::Vec3Array> verts = new osg::Vec3Array();
		verts->push_back(ll);
		verts->push_back(lr);
		verts->push_back(ur);
		verts->push_back(ul);
		quad->setVertexArray(verts);

		osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array();
		texcoords->push_back(osg::Vec2(0.0f, 0.0f));
		texcoords->push_back(osg::Vec2(1.0f, 0.0f));
		texcoords->push_back(osg::Vec2(1.0f, 1.0f));
		texcoords->push_back(osg::Vec2(0.0f, 1.0f));
		quad->setTexCoordArray(0, texcoords);

		osg::ref_ptr<osg::DrawElementsUInt> indices = new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
		indices->push_back(0);
		indices->push_back(1);
		indices->push_back(2);
		indices->push_back(3);
		quad->addPrimitiveSet(indices);

		// 应用纹理
		osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D(image);
		texture->setResizeNonPowerOfTwoHint(false);

		osg::StateSet* stateSet = quad->getOrCreateStateSet();
		
		// 启用并绑定纹理贴图
		stateSet->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);

		// 关闭光照效果
		stateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

		// 添加到 Geode
		osg::ref_ptr<osg::Geode> geode = new osg::Geode();
		geode->addDrawable(quad);
		osg::StateSet* geodeStateSet = geode->getOrCreateStateSet();
		geodeStateSet->setRenderBinDetails(10, "RenderBin");
		//启用深度测试
		geodeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
		// 设置渲染状态
		geodeStateSet->setRenderBinDetails(300, "RenderBin"); // 在模型下方一层
		geodeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
		// 启用混合（透明效果）
		geodeStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
		geodeStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		geodeStateSet->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		//设置深度偏移（确保在上方）
		geodeStateSet->setAttribute(new osg::PolygonOffset(-1.0f, -1.0f));
		//禁用深度写入（确保不会相互遮挡）
		geodeStateSet->setAttributeAndModes(new osg::Depth(osg::Depth::ALWAYS));
		// 禁用深度测试（确保始终可见）
		geodeStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
		geodeStateSet->setAttributeAndModes(new osg::PolygonOffset(1.0f, 1.0f), osg::StateAttribute::ON);

		// 三维图层展示
		mapNode->addChild(geode);

		_p->_GeodeVec.emplace_back(geode);

	}, Qt::QueuedConnection);

	return true;
}

void qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::cleanupMutilspectralMap() const
{
	AgriPestAndDiseaseAnalysisWidget* pWidget = LY_AgriDataAnalysisPredict::Get().GetAgriPestAnalysisWidget();
	QMetaObject::invokeMethod(pWidget, [=]() {

		QMutexLocker locker(&m_mutex);

		osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
		for (const auto& geode : _p->_GeodeVec) {
			if (geode) {
				mapNode->removeChild(geode);
			}
		}
		_p->_GeodeVec.clear();

	}, Qt::QueuedConnection);
}

bool qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::showTaskInfotLegend() const
{
	AgriPestAndDiseaseAnalysisWidget* pWidget = LY_AgriDataAnalysisPredict::Get().GetAgriPestAnalysisWidget();
	QMetaObject::invokeMethod(pWidget, [=]() {

		QMutexLocker locker(&m_mutex);

		if (_p->_PatrolLegend)
		{
			delete _p->_PatrolLegend;
			_p->_PatrolLegend = nullptr;
		}

		_p->_PatrolLegend = new AgriPatrolLegend(pWidget);
		_p->_PatrolLegend->resize(QSize(485, 1177));
		_p->_PatrolLegend->move(2780, -800);
		_p->_PatrolLegend->UpdatTaskInfoLegend();
		_p->_PatrolLegend->show();

	}, Qt::QueuedConnection);

	return true;
}

bool qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::showResultLegend() const
{
	AgriPestAndDiseaseAnalysisWidget* pWidget = LY_AgriDataAnalysisPredict::Get().GetAgriPestAnalysisWidget();
	QMetaObject::invokeMethod(pWidget, [=]() {

		QMutexLocker locker(&m_mutex);

		if (_p->_PatrolLegend) {
			delete _p->_PatrolLegend;
			_p->_PatrolLegend = nullptr;
		}

		_p->_PatrolLegend = new AgriPatrolLegend(pWidget);
		_p->_PatrolLegend->resize(QSize(485, 1177));
		_p->_PatrolLegend->move(2780, -1170);
		_p->_PatrolLegend->UpdateShowLegend();
		_p->_PatrolLegend->show();

	}, Qt::QueuedConnection);

	return true;
}

void qnzkna::AgriDataAnalysisPredictService::IAgriDataAnalysisPredictServiceImpl::hideResultLegend() const
{
	AgriPestAndDiseaseAnalysisWidget* pWidget = LY_AgriDataAnalysisPredict::Get().GetAgriPestAnalysisWidget();
	QMetaObject::invokeMethod(pWidget, [=]() {

		QMutexLocker locker(&m_mutex);

		if (_p->_PatrolLegend) {
			_p->_PatrolLegend->hide();
		}

	}, Qt::QueuedConnection);
}
