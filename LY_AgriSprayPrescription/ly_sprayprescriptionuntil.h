#ifndef LY_SPRAYPRESCRIPTIONUNTIL_H
#define LY_SPRAYPRESCRIPTIONUNTIL_H

#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <QPointF>


#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"
using namespace AgriSprayPrescriptionDecl;

namespace LY_SprayPrescriptionUntil
{
    /* 枚举字符串转换 */

    QString CropTypeToString(AgriSprayPrescriptionDecl::CropType cropType);

    QString DataCollectTypeToString(AgriSprayPrescriptionDecl::DataCollectType collectType);

    QString PestDiseaseToString(PestDisease pestDisease);

    QString PestLevelToString(PestLevel level);

    QString PestCategoryToString(PestCategory category);

    QString PestStageToString(PestStage stage);

	QString WorkStatueToString(WorkType stage);

    /* 生成uuid */
    QString GenerateUuid();

    /* 点是否在区域内 */
    bool JudgePointInZone(const QPointF& point, const std::vector<QPointF>& zonePoints);

	/* 指定多边形内生成n个随机点 */
	std::vector<QPointF> GenerateRandomPointsInPolygon(const std::vector<QPointF>& polygon, int n);

	bool CalcPolygonArea(const std::vector<QPointF>& zonePoints, double& area);

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

#endif // LY_SPRAYPRESCRIPTIONUNTIL_H
