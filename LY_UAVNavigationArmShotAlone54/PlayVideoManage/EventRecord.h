#ifndef EVENTRECORD_H
#define EVENTRECORD_H

#include <QWidget>
#include <QTableWidget>
#include "yolo_v2_class.hpp"
#include "videoconfig.h"
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
	void AddtableWidget2(std::vector<bbox_t> a,const char* str);
//public slots:
//	void handleData(const char* str);
private:
    Ui::EventRecord *ui;

	void initTable();

};

#endif // EVENTRECORD_H
