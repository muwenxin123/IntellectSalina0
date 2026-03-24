#ifndef IAGRISOILTESTINGANALYSISSERVICEIMPL_h__
#define IAGRISOILTESTINGANALYSISSERVICEIMPL_h__

#include <QMutex>

#include "DataManage/IAgriSoilTestingAnalysisService.h"
#include "DataManage/AgriSoilTestingAnalysis/AgriSoilTestingAnalysisDecl.h"

namespace qnzkna
{
	namespace AgriSoilTestingAnalysisService
	{
		class IAgriSoilTestingAnalysisServiceImpl : public IAgriSoilTestingAnalysisService
		{
		public:
			IAgriSoilTestingAnalysisServiceImpl();
			virtual ~IAgriSoilTestingAnalysisServiceImpl(void) {}

			/* 测土数据集 */
			virtual AgriSoilTestingDatasetPtrVec GetSoilTestingDatasetList(const QStringList& fieldList) const;

			/* 测土采样点 */
			virtual AgriSoilTestingCollectionPtrVec GetSoilTestingCollectionList(const QString& sDatasetId) const;
			virtual AgriSoilTestingCollectionPtrVec GetSoilTestingCollectionPagedData(const QString& sDatasetId, int nPage, int nPageSize) const;
			virtual int GetSoilTestingCollectionCount(const QString& sDatasetId) const;

			/* 分布图 */
			virtual AgriSoilFertilityDistributeMapPtrVec GetSoilTestingDistributeMap(const QString& sDatasetId) const;

			/* 肥力分布图点数据 */
			virtual QPair<bool, QString> GetSoilFertilityDistribute(const AgriSoilFertilityDistributeMapPtr inDistributeMap, AgriSoilTestingFertilityPointPtrVec & outSoilFertilityPointVec, std::vector<QPointF> boundPoings = std::vector<QPointF>()) override;

			/* 生成分布图 */
			virtual bool generateSoilTestingMap() const;

			/* 清理分布图 */
			virtual void cleanupMap() const;
		private:
			mutable QMutex                       m_mutex;
		};
	}
}
#endif // IAGRISOILTESTINGANALYSISSERVICEIMPL_h__
