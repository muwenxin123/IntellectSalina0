#ifndef AGRIMACHINEMANAGERUTIL_H_
#define AGRIMACHINEMANAGERUTIL_H_

#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"

namespace AgriFarmManagerUtil
{
	const double SQM_PER_MU = 666.6667; // 1 亩 = 666.6667 平方米

	//盐田面积
	extern QString filed_area;

	// 解析枚举字符串查找对应值
	QString FetchValueByEnumId(const AgriEnumInfoPtrVec& vecEnums, const int& nEnumId);

	// 生成uuid
	QString GenerateUuid();

	/* 计算多边形区域面积 */
	bool CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area);

	/* 平方米转亩 */
	inline double sqmToMu(double sqm) { return sqm / SQM_PER_MU; }

	/* 亩转平方米 */
	inline double muToSqm(double mu) { return mu * SQM_PER_MU; }

	/* 计算多边形重心 */
	bool CalcPolygonCenterPoint(const std::vector<QPointF>& zonePoints, QPointF& point);

	/* 点是否在区域内(奇偶规则) */
	bool JudgePointInZone_OddEven(const QPointF& point, const std::vector<QPointF>& zonePoints);

}

#endif  // AgriFarmManagerUtil_H