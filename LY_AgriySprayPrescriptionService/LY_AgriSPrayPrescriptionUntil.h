#ifndef LY_SPRAYPRESCRIPTIONUNTIL_H
#define LY_SPRAYPRESCRIPTIONUNTIL_H

#include <thread>
#include <condition_variable>
#include <functional>
#include <chrono>
#include <atomic>
#include <queue>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <functional>
#include <QPointF>
#include "DataManage/AgriFarm/AgriSprayPrescriptDecl.h"

#include "map3d/IEarth.h"
#include "map3d\IGraphics.h"

#include <osg/ComputeBoundsVisitor>
#include <osg/Depth>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/PolygonOffset>
#include <osg/PrimitiveSet>
#include <osgAnimation/BasicAnimationManager>>
#include <osgEarthAnnotation/AnnotationUtils>
#include <osg/LineWidth>
#include <osgFX/Scribe>
#include <osgFX/Outline>
#include <osg/LineWidth>
#include <osgManipulator/CommandManager>
#include <osgManipulator/Dragger>
#include <osg/NodeVisitor>
#include <osgGA/GUIEventHandler>
#include <osgUtil/LineSegmentIntersector>
#include <osg/Billboard>
#include <osgEarth/MapNode>
#include <osgEarth/ElevationQuery>

using namespace AgriSprayPrescriptionDecl;

#define M_PI (3.14159265358979323846)
// 常量
const double R = 6371000.0; // 地球平均半径(米)

namespace LY_AgriSPrayPrescriptionUntil
{
	QString vectorToString(const std::vector<int>& vec);
	// 将二维 int 向量转换为字符串，子向量用分号分隔
	QString vector2DToString(const std::vector<std::vector<int>>& vec2D);

	// 将一维 double 向量转换为逗号分隔的字符串
	QString vectorDoubleToString(const std::vector<double>& vec);
	// 将逗号分隔的字符串转换为一维 int 向量
	std::vector<int> stringToVector(const QString& str);
		// 将字符串转换为二维 int 向量
	std::vector<std::vector<int>> stringToVector2D(const QString& str);

	// 将逗号分隔的字符串转换为一维 double 向量
	std::vector<double> stringToVectorDouble(const QString& str);

	osgEarth::Symbology::Color hexToColor(const std::string& hexColor, float rgb_a = 1.0f);

	//获取经纬度
	double getElevation(double lon, double lat);

	// 将角度转换为弧度
	double toRadians(double degrees);
	// 将弧度转换为角度
	double toDegrees(double radians);

	// 计算目标点的经纬度和高程
	void calculateNewPosition(double lat0, double lon0, double h0, double distance,
		double azimuth, double elevation, double& lat1, double& lon1, double& h1);

	//绘制三角锥点面
	std::vector<IUserMesh2*> createDetectionZone(float angle_l, float angle_w, float dis_m);

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





class SimpleTimer {
public:
	SimpleTimer() : m_isActive(false), m_isPaused(false), m_interval(0) {}
	~SimpleTimer() { stop(); }

	// 设置定时器回调函数和时间间隔（毫秒）
	void start(int interval, std::function<void()> callback) {
		stop(); // 确保之前的定时器停止

		m_interval = interval;
		m_callback = callback;
		m_isActive = true;
		m_isPaused = false;

		m_thread = std::thread([this]() {
			while (m_isActive) {
				std::unique_lock<std::mutex> lock(m_mutex);
				// 等待间隔时间或激活通知
				if (m_cv.wait_for(lock,
					std::chrono::milliseconds(m_interval),
					[this] { return !m_isActive || !m_isPaused; })) {
					// 被唤醒：检查是否停止
					if (!m_isActive) break;

					// 如果是暂停状态则继续等待
					if (m_isPaused) {
						continue;
					}
				}

				// 执行回调（确保在解锁状态执行）
				lock.unlock();
				if (m_callback && m_isActive && !m_isPaused) {
					m_callback();
				}
			}
		});
	}

	// 停止定时器（线程安全）
	void stop() {
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (!m_isActive) return;
			m_isActive = false;
			m_isPaused = false;
		}
		m_cv.notify_all();
		if (m_thread.joinable()) {
			m_thread.join();
		}
	}

	// 暂停定时器
	void pause() {
		std::lock_guard<std::mutex> lock(m_mutex);
		if (m_isActive && !m_isPaused) {
			m_isPaused = true;
		}
	}

	// 继续定时器
	void resume() {
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_isActive && m_isPaused) {
				m_isPaused = false;
			}
		}
		m_cv.notify_all(); // 唤醒等待的线程
	}

	// 检查是否正在运行
	bool isActive() const { return m_isActive; }

	// 检查是否暂停
	bool isPaused() const { return m_isPaused; }

private:
	std::thread m_thread;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	std::atomic<bool> m_isActive;
	std::atomic<bool> m_isPaused;
	int m_interval;
	std::function<void()> m_callback;
};



#endif // LY_SPRAYPRESCRIPTIONUNTIL_H
