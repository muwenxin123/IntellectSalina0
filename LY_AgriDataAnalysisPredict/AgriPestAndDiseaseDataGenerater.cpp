#include <random>
#include <QDateTime>
#include <QDebug>
#include <QRandomGenerator>

#include "AgriPestAndDiseaseDataGenerater.h"
#include "LY_AgriDataAnalysisPredict.h"


AgriPestAndDiseaseDataGenerater::AgriPestAndDiseaseDataGenerater(QObject *parent /*= nullptr*/)
{

}

AgriPestAndDiseaseDataGenerater::~AgriPestAndDiseaseDataGenerater()
{

}

bool AgriPestAndDiseaseDataGenerater::generatePestDataCollection(const QString& sDatasetId, const std::vector<QPointF>& vBoundPoings, int number)
{
	if (sDatasetId.isEmpty() || vBoundPoings.size() <=2 || number <=0)
	{
		return false;
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(1, 10);

	AgriPestCollectionPtrVec collectInfoVec;

	/* 区域内随机生成点测试 */
	std::vector<QPointF> vecRandomPonit = AgriDataAnalysisPredictUtil::GenerateRandomPointsInPolygon(vBoundPoings, number);

	if (vecRandomPonit.empty())
	{
		return false;
	}

	for (int i = 0; i < vecRandomPonit.size(); i++) {

		AgriPestCollectionPtr pCollect = std::make_shared<AgriPestCollection>();
		pCollect->guid = AgriDataAnalysisPredictUtil::GenerateUuid();
		pCollect->name = QString("CJ_%1").arg(i+1);
		pCollect->dataset_id = sDatasetId;
		pCollect->sensor_id = "";
		pCollect->collect_type = 1; // 默认手动添加
		pCollect->collect_time = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
		pCollect->lon = vecRandomPonit[i].x();
		pCollect->lat = vecRandomPonit[i].y();
		pCollect->alt = 0;
		pCollect->category = QRandomGenerator::global()->bounded(1, 5);
		pCollect->stage = QRandomGenerator::global()->bounded(1, 4);
		pCollect->count = QRandomGenerator::global()->bounded(1, 101);
		pCollect->range = QRandomGenerator::global()->bounded(1, 6);
		pCollect->level = static_cast<int>(determinePestLevel(pCollect));

		collectInfoVec.emplace_back(pCollect);
	}

	return LY_AgriDataAnalysisPredict::Get().GetpDbHandler()->BatchInsertPestCollection(collectInfoVec);
}

PestLevel AgriPestAndDiseaseDataGenerater::determinePestLevel(const AgriPestCollectionPtr& pestCollect)
{
	double score = 0;
	score += pestCollect->count / 20;	// 每10只虫害加1分
	score += (pestCollect->range / 2);	// 范围越大影响越大

	switch (static_cast<PestStage>(pestCollect->stage))
	{
	case PestStage::PEST_STAGE_1:
		score += 0;
		break;
	case PestStage::PEST_STAGE_2:
		score += 1;
		break;
	case PestStage::PEST_STAGE_3:
		score += 2;
		break;
	default:
		break;
	}

	if (score >= 10) return PestLevel::PEST_LEVEL_10;
	if (score >= 9) return PestLevel::PEST_LEVEL_9;
	if (score >= 8) return PestLevel::PEST_LEVEL_8;
	if (score >= 7) return PestLevel::PEST_LEVEL_7;
	if (score >= 6) return PestLevel::PEST_LEVEL_6;
	if (score >= 5) return PestLevel::PEST_LEVEL_5;
	if (score >= 4) return PestLevel::PEST_LEVEL_4;
	if (score >= 3) return PestLevel::PEST_LEVEL_3;
	if (score >= 2) return PestLevel::PEST_LEVEL_2;
	if (score >= 1) return PestLevel::PEST_LEVEL_1;
	return PestLevel::PEST_LEVEL_NONE;
}
