#ifndef AGRIDATAANALYSISPREDICTUTIL_H_
#define AGRIDATAANALYSISPREDICTUTIL_H_

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <QPointF>
 
#include "AgriDataAnalysisPredictDecl.h"

const double SQM_PER_MU = 666.6667; // 1 亩 = 666.6667 平方米

namespace AgriDataAnalysisPredictUtil
{
	/* 枚举字符串转换 */

	QString CropTypeToString(CropType cropType);

	QString DataCollectTypeToString(DataCollectType collectType);

	QString PestDiseaseToString(PestDisease pestDisease);

	QString PestLevelToString(PestLevel level);

	QString PestCategoryToString(PestCategory category);

	QString PestStageToString(PestStage stage);

	QString GridSizeToString(GridSize size);

	QString DiffusionLayerToString(DiffusionLayer layer);

	DiffusionLayer GetDiffusionLayer(int layer);

	QString GeneTypeToString(GenerateType geneType);

	QString WindDirectToString(WindDirection windDirect);

	/* 生成uuid */
	QString GenerateUuid(); 

	/* 计算多边形重心 */
	bool CalcPolygonCenterPoint(const std::vector<QPointF>& zonePoints, QPointF& point);

	/* 点是否在区域内(奇偶规则) */
	bool JudgePointInZone_OddEven(const QPointF& point, const std::vector<QPointF>& zonePoints);
	/* 点是否在区域内(射线法) */
	bool JudgePointInZone_Ray(const QPointF& point, const std::vector<QPointF>& zonePoints);
	/* 点是否在区域内(复杂区域：自交多边形) */
	bool JudgePointInZone_Complex(const QPointF& point, const std::vector<QPointF>& zonePoints);

	/* 指定多边形内生成n个随机点 */
	std::vector<QPointF> GenerateRandomPointsInPolygon(const std::vector<QPointF>& polygon, int n);

	/* 计算多边形区域面积 */
	bool CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area);
	
	/* 平方米转亩 */
	inline double sqmToMu(double sqm) { return sqm / SQM_PER_MU; }

	/* 亩转平方米 */
	inline double muToSqm(double mu) { return mu * SQM_PER_MU; }

	// 地球的半径（单位：米）
	const double R_EQUATOR = 6378137.0;  // 赤道半径
	const double R_POLAR = 6356752.3;    // 极半径
	const double PI = 3.14159265358979323846;

	// 计算纬度方向上的 1m 对应的度数
	double metersToLatitudeDegrees(double meters = 1.0);

	// 计算经度方向上的 1m 对应的度数（取决于纬度） 纬度（度°）
	double metersToLongitudeDegrees(double latitude, double meters = 1.0);

	/* 计算多边形交集 */
	QPolygonF calculateIntersection(const QPolygonF& polygon1, const QPolygonF& polygon2);
}

template <typename T>
class ThreadSafeMap
{
public:
	void insert(const std::string& id, const T& data)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		data_[id] = data;
	}

	bool query(const std::string& id, T& result)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto it = data_.find(id);
		if (it != data_.end()) {
			result = it->second;
			return true;
		}
		return false;
	}

	bool remove(const std::string& id)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		auto it = data_.find(id);
		if (it != data_.end()) {
			data_.erase(it);
			return true;
		}
		return false;
	}

	void iterate_update(std::function<void(const std::string&, T&)> callback)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (auto& pair : data_) {
			callback(pair.first, pair.second);
		}
	}

	void iterate_query(std::function<void(const std::string&, const T&)> callback)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		for (const auto& pair : data_) {
			callback(pair.first, pair.second);
		}
	}

	void clear()
	{
		std::lock_guard<std::mutex> lock(mutex_);
		data_.clear();
	}

private:
	std::map<std::string, T> data_;
	std::mutex mutex_;
};

#endif  // AgriDataAnalysisPredictUtil_H