#ifndef IFIRESPREADIMPL_H
#define IFIRESPREADIMPL_H

#include "DataManage/IFireSpreadService.h"
#include "CellularAutomata.h"

namespace qnzkna
{
	namespace FireSpread
	{
		class IFireSpreadImpl:
			public IFireSpreadService
		{
		public:

			IFireSpreadImpl(void);

			~IFireSpreadImpl(void);

			void getFireStateMapQueue();
			void setFireStateMapSize(double l, double w);
			void setInitFireInfo(double lon, double lat, double l, double w);
			void setSpreadTime(int time_sec);
			void setTimeStep(int time_sec);
			QQueue<QVector<QPointF>> spreadFire(const QVector<int> &weatherConditionVec);
			void DrawDynamic(QPainter *painter);
			void setFireSpreadQueue(QQueue<QVector<QPointF>> FireSpreadQueue);
			void clearFireSpreadQueue();

		private:

		private:
			CellularAutomata* m_CellularAutomata = nullptr;
			QQueue<QVector<QPointF>> m_FireSpreadQueue;

		};
	}
}

#endif
