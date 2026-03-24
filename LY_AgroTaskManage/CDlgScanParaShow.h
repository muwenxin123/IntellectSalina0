#ifndef CDlgScanParaShow_H
#define CDlgScanParaShow_H

#include <QWidget>
#include "LyDialog.h"
#include "IAgroTaskService.h"
#include "Zone\AgroTaskInfo.h"
#include "IZoneManageService.h"
#include <vector>
#include "QPoint"
#include "IProj.h"

namespace Ui {
class CDlgScanParaShow;
}

struct PoiScan
{
	PoiScan() {};
	PoiScan(double dLon, double dLat, double dAlt = 0.0)
	{
		lon = dLon;
		lat = dLat;
		alt = dAlt;
	}
	double lon;
	double lat;
	double alt; //等级
	double distanceTo(const PoiScan &other) const;
};

class CDlgScanParaShow : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgScanParaShow(QWidget *parent = nullptr);
    ~CDlgScanParaShow();

	void set_name(const QString &name);
	void set_explanation(const QString &explanation);
	void path_plan(const QString &zoneName);

	//对外使用：路线规划返回航点集合
	std::vector<QPointF> getPathPlanPoiVec(const QString &zoneName, double spacing, int planWay);

	bool DrawDynamic(QPainter *painter, IProj *pProj);

public slots:
	void on_pushButton_clicked();
	void on_pushButton_2_clicked();
	void on_pushButton_3_clicked();
	void on_pushButton_4_clicked();

public:
	bool isShow_path = true;

private:
	void init_widget(const std::vector<double>& rx, const std::vector<double>& ry);
	
	/**
	边界x
	边界y
	障碍物
	起点
	每个格子的宽
	方向
	返回的x
	返回的y
	输入的方向
	*/
	void planning(std::vector<double>& ox, std::vector<double>& oy,
		std::vector<std::vector<QPointF>>foxy,
		std::vector<PoiScan> stZonePointVec,
		QPointF areaFarPos,
		double resolution,
		std::vector<double>& rx,
		std::vector<double>& ry,
		int planWay);

private:
    Ui::CDlgScanParaShow *ui;

	qnzkna::AgroTask::IAgroTaskService *m_pIAgroTaskService;
	qnzkna::ZoneManage::IZoneManageService *m_pIZoneManageService;

	std::vector<double> path_poix;
	std::vector<double> path_poiy;

	QString m_zoneName{ "" };
};

#endif // CDlgScanParaShow_H
