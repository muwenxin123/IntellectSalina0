#ifndef FireSpreadWidget_H
#define FireSpreadWidget_H

#include <QWidget>
#include "IFireSpreadService.h"
#include "QQueue"
#include "QPoint"
#include "QList"
#include "IMarkManageService.h"
#include "INebulaTaskManageService.h"
#include "Mark\Mark.h"
#include "IZoneManageService.h"

namespace Ui {
class FireSpreadWidget;
}

class FireSpreadWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FireSpreadWidget(QWidget *parent = nullptr);
    ~FireSpreadWidget();

public slots:
	void on_pb_forecast_clicked();
	void on_pb_clear_clicked();

public:
	QQueue<QVector<QPointF>> outQueueFireSpread;

private:
    Ui::FireSpreadWidget *ui;

	qnzkna::FireSpread::IFireSpreadService *m_pIFireSpreadService;
	qnzkna::MarkManage::IMarkManageService *m_pIMarkManageService;
	qnzkna::TaskManage::INebulaTaskManageService *m_pINebulaTaskManageService;
	qnzkna::ZoneManage::IZoneManageService* m_pIZoneManageService;
	CSystemMarkInfoVec m_MarkInfoVec;
};

#endif
