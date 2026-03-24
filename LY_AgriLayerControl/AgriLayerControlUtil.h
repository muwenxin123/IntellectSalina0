#ifndef AGRILAYERCONTROLUTIL_H_
#define AGRILAYERCONTROLUTIL_H_
 
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <QPointF>

const double SQM_PER_MU = 666.6667; // 1 亩 = 666.6667 平方米

namespace AgriDataAnalysisPredictUtil
{
	/* 生成uuid */
	QString GenerateUuid();

	/* 计算多边形重心 */
	bool CalcPolygonCenterPoint(const std::vector<QPointF>& zonePoints, QPointF& point);

	/* 计算多边形区域面积 */
	bool CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area);

	/* 平方米转亩 */
	inline double sqmToMu(double sqm) { return sqm / SQM_PER_MU; }

	/* 亩转平方米 */
	inline double muToSqm(double mu) { return mu * SQM_PER_MU; }

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
#endif  // AgriLayerControlUtil_H