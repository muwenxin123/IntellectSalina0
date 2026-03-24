#include <QDebug>

#include "AgriSoilAnalysisDBHandler.h"
#include "LY_AgriSoilAnalysisPredict.h"
#include "IAgriSoilTestingAnalysisServiceImpl.h"
#include "AgriSoilTestingDistributeWidget.h"

qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::IAgriSoilTestingAnalysisServiceImpl()
{

}

AgriSoilTestingFertilizer::AgriSoilTestingDatasetPtrVec qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::GetSoilTestingDatasetList(const QStringList& fieldList) const
{
	QMutexLocker locker(&m_mutex);

	return LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingDatasetList(fieldList);
}

AgriSoilTestingFertilizer::AgriSoilTestingCollectionPtrVec qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::GetSoilTestingCollectionList(const QString& sDatasetId) const
{
	QMutexLocker locker(&m_mutex);

	return LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionList(sDatasetId);
}

AgriSoilTestingFertilizer::AgriSoilTestingCollectionPtrVec qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::GetSoilTestingCollectionPagedData(const QString& sDatasetId, int nPage, int nPageSize) const
{
	QMutexLocker locker(&m_mutex);

	return LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionPagedData(sDatasetId, nPage, nPageSize);
}

int qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::GetSoilTestingCollectionCount(const QString& sDatasetId) const
{
	QMutexLocker locker(&m_mutex);

	return LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingCollectionCount(sDatasetId);
}

AgriSoilFertilityDistributeMapPtrVec qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::GetSoilTestingDistributeMap(const QString& sDatasetId) const
{
	QMutexLocker locker(&m_mutex);

	return LY_AgriSoilAnalysisPredict::Get().GetpDbHandler()->GetSoilTestingDistributeMap(sDatasetId);
}

QPair<bool, QString> qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::GetSoilFertilityDistribute(const AgriSoilFertilityDistributeMapPtr inDistributeMap, AgriSoilTestingFertilityPointPtrVec & outSoilFertilityPointVec, std::vector<QPointF> boundPoings/* = std::vector<QPointF>()*/)
{
	QMutexLocker locker(&m_mutex);

	return AgriSoilTestingDistributeWidget::getSoilFertilityDistribute(inDistributeMap, outSoilFertilityPointVec, boundPoings);
}

bool qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::generateSoilTestingMap() const
{
	QMutexLocker locker(&m_mutex);

	AgriSoilTestingAnalysisWidget* pWidget = LY_AgriSoilAnalysisPredict::Get().GetAgriPestAnalysisWidget();

	QMetaObject::invokeMethod(pWidget, [=]() {

		if (pWidget)
		{
			AgriFieldTree* pFieldTree = pWidget->getAgriFieldTree();
			if (pFieldTree)
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
				pFieldTree->setActiveFieldInfo(fieldInfo);
			}

			AgriSoilTestingDistributeWidget* pDisWidget = pWidget->getPestAndDiseaseDataCollectWidget();
			if (pDisWidget)
			{
				pDisWidget->cleanUp();

				AgriSoilTestingDataset dataset;
				dataset.guid = u8"dd32ac08-a6d5-427c-b7cb-2ca8995f7e5d";
				dataset.name = u8"P_01";
				dataset.field_id = u8"47658e87-d320-45bb-b0b3-9607c3ba52d2";
				dataset.soil_type = 1;
				dataset.soil_color = 1;
				dataset.soil_texture = 1;
				dataset.n_type = 1;
				dataset.p_type = 1;
				dataset.k_type = 1;
				dataset.sample_type = 2;
				dataset.sample_mode = 3;
				dataset.create_time = u8"2025-07-10 13:53:37";
				dataset.update_time = u8"2025-07-17 16:43:26";
				dataset.grid_size = 20 ;
				dataset.grid_rotation = 0;
				dataset.valid_identify = 7;
				pDisWidget->setCurrentDataset(std::make_shared<AgriSoilTestingDataset>(dataset));
				pDisWidget->on_btn_generate_clicked();
			}
		}

	}, Qt::QueuedConnection);

	return false;
}

void qnzkna::AgriSoilTestingAnalysisService::IAgriSoilTestingAnalysisServiceImpl::cleanupMap() const
{
	QMutexLocker locker(&m_mutex);

	AgriSoilTestingAnalysisWidget* pWidget = LY_AgriSoilAnalysisPredict::Get().GetAgriPestAnalysisWidget();
	QMetaObject::invokeMethod(pWidget, [=]() {

		if (pWidget) {
			AgriSoilTestingDistributeWidget* pDisWidget = pWidget->getPestAndDiseaseDataCollectWidget();
			if (pDisWidget) {
				pDisWidget->cleanUp();
			}
		}
	}, Qt::QueuedConnection);
}
