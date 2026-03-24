#ifndef AGRIFIELDIDENTIFYDIALOG_H_
#define AGRIFIELDIDENTIFYDIALOG_H_
 
#include <QWidget>
#include <Protocol/XygsJL/sharedFile.h>

#include "LyDialog.h"
#include "IProj.h"
#include "AgriFieldIdentifyCalculate.h"

namespace Ui
{
	class AgriFieldIdentifyDialog;
}

#include <vector>
#include <mutex>

template<typename T>
class ThreadSafeVector {
public:
	void push_back(const T& value) {
		std::lock_guard<std::mutex> lock(mtx_);
		vec_.push_back(value);
	}

	void emplace_back(const T& value) {
		std::lock_guard<std::mutex> lock(mtx_);
		vec_.emplace_back(value);
	}

	bool get(size_t index, T& out) {
		std::lock_guard<std::mutex> lock(mtx_);
		if (index < vec_.size()) {
			out = vec_[index];
			return true;
		}
		return false;
	}

	size_t size() {
		std::lock_guard<std::mutex> lock(mtx_);
		return vec_.size();
	}

	void clear() {
		std::lock_guard<std::mutex> lock(mtx_);
		vec_.clear();
	}

	// 复制一份数据后遍历，避免死锁和竞态
	void forEach(std::function<void(const T&)> func) {
		std::vector<T> copy;
		{
			std::lock_guard<std::mutex> lock(mtx_);
			copy = vec_;  // 拷贝一份
		}
		for (const auto& item : copy) {
			func(item);  // 遍历使用
		}
	}

private:
	std::vector<T> vec_;
	std::mutex mtx_;
};

class AgriFieldIdentifyDialog : public LyDialog
{
	Q_OBJECT

public:
	explicit AgriFieldIdentifyDialog(QWidget *parent = nullptr);
	~AgriFieldIdentifyDialog();

	inline bool isPositionPick() { return m_bPick; };
	void updateTargetPoint(const QPointF & point);
	void getSelectArea(QVector<QPointF> & areaPoints);
	void drawDynamic(QPainter &painter, IProj *pProj);

protected:
	virtual void changeEvent(QEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual void hideEvent(QHideEvent *event);

private:
	void InitData();
	void InitAreaPointsTable();
	void AddOneRowToTable(QPointF point);

public slots :
	void on_btn_select_area_clicked();
	void handleAreaIdentifyFinish();
	void handleMapShowOrHide(int state);

private:
	Ui::AgriFieldIdentifyDialog						*ui;
	bool											m_show;
	bool											m_bPick = false;
	bool											m_bMapShow = false;
	ThreadSafeVector<QPointF>						m_areaPoints;
	std::vector<FeatureInfo_shp>					m_areaList;
};
#endif  // AgriFieldIdentifyDialog_H