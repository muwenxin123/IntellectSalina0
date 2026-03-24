#ifndef IAgriDataAnalysisPredictServiceImpl_h__
#define IAgriDataAnalysisPredictServiceImpl_h__

#include <QMutex>

#include "DataManage/IAgriDataAnalysisPredictService.h"

namespace qnzkna
{
	namespace AgriDataAnalysisPredictService
	{
		struct IAgriDataAnalysisPredictServiceImplPrivate;
		class IAgriDataAnalysisPredictServiceImpl : public IAgriDataAnalysisPredictService
		{
		public:
			IAgriDataAnalysisPredictServiceImpl();
			virtual ~IAgriDataAnalysisPredictServiceImpl(void) {}

			/* 查询激活田块边界 */
			virtual bool getActiveFieldBounds(std::vector<QPointF> & vBoundPoings) const;

			/* 生成分布图 */
			virtual bool generatePestDistributeMap() const;

			/* 清理分布图 */
			virtual void cleanupMap() const;

			/* 生成多光谱图 */
			virtual bool generateMutilspectralMap() const;

			/* 清理多光谱图 */
			virtual void cleanupMutilspectralMap() const;

			/* 展示任务信息 */
			virtual bool showTaskInfotLegend() const;

			/* 展示结果图例信息 */
			virtual bool showResultLegend() const;

			/* 隐藏结果图例信息 */
			virtual void hideResultLegend() const;

		private:
			mutable QMutex									m_mutex;
			IAgriDataAnalysisPredictServiceImplPrivate		* _p;
		};
	}
}
#endif // IAgriDataAnalysisPredictServiceImpl_h__
