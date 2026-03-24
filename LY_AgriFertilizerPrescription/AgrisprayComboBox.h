#ifndef AgrisprayComboBox_H
#define AgrisprayComboBox_H

#include <QWidget>
#include "IProj.h"
#include "Zone/ZoneManagerInfo.h"
#include "QComboBox"
#include "QString"
#include "QPainter"
#include "LY_AgriFertilizerPrescriptionActivator.h"

#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>
#include <functional>

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

class AgrisprayComboBox : public QComboBox
{
	Q_OBJECT
public:
	explicit AgrisprayComboBox(QWidget *parent = nullptr);
	~AgrisprayComboBox();

public:
	void initData();
	void zoneHighLight(QPainter &painter, IProj *pProj);
	void clearZoneHighLight();
	void selectedActiveFarm();
	void switchActiveFarm();
	QString getFieldId() const;

	public slots:
	void slotCurrentTextChange(const QString& strText);

signals:
	void sigCurrentTextChange(const QString& strText);
	void setupField(QString &text, QString &file_id, QString &id);

private:
	void setScreenCenter(double &dLon, double &dLat);

private:
	AgriFieldInfoModelVec m_vecFieldList;
	QString m_currentFarm;
	ThreadSafeMap<qnzkna::ZoneManage::CZoneBase> m_zoneDrawMap;
	//qnzkna::ZoneManage::CZoneBase m_zoneDrawMap;
};




#endif // AgrisprayComboBox_H
