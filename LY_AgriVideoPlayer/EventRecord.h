#ifndef EVENTRECORD_H
#define EVENTRECORD_H

#include <QWidget>
#include <QTableWidget>
#include "yolo_v2_class.hpp"
#include "videoconfig.h"
#include "gdd/wsProtocol.h"

#include <QReadLocker>
namespace Ui {
class EventRecord;
}

class EventRecord : public QWidget
{
    Q_OBJECT

public:
    explicit EventRecord(QWidget *parent = nullptr);
    ~EventRecord();

	void addTableRow(const QDate &date, const QTime &time, const QString &status, const QString &category, const QString &location);
	void AddtableWidget(std::vector<bbox_t> a, const VideoInfoData videoInfo);
	//void setEventData(const WS::EventData& data);
	void AddtableWidget2(std::vector<bbox_t> a,const char* str);
	void setfeedback(const WS::DetectionData& data);
	
	void setEventData(const WS::EventData& data);
public slots:


//public slots:
//	void handleData(const char* str);
private:
    Ui::EventRecord *ui;

	void initTable();
	// 添加新的方法用于处理检测数据
	void addDetectionEvent(const WS::DetectionData& detectionData);

	public slots:
	// 新增槽函数，用于接收检测数据
	void onDetectionDataReceived();

	// 辅助方法：检查是否有检测目标
	bool hasDetectionTargets(const WS::EventData& data);


	// 辅助方法：生成事件内容
	QString generateEventContent(const WS::EventData& eventData);

	QString EventRecord::generateDetailedEventContent(const WS::EventData& eventData);


private:
	std::vector<WS::EventData> eventdatavec;
	QTimer* m_timer;
	mutable QReadWriteLock m_readLock;

};

#endif // EVENTRECORD_H
