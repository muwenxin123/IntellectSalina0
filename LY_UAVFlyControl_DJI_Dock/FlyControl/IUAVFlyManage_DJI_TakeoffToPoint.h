#ifndef IUAVFLYMANAGE_DJI_TAKEOFFTOPOINT_H
#define IUAVFLYMANAGE_DJI_TAKEOFFTOPOINT_H

#include <QWidget>
#include "LyDialog.h"

#include "dji/sdk/cloudapi/wayline/dji_takeoff_to_point.h"

#include <QPainter>
#include "IProj.h"

namespace Ui {
class IUAVFlyManage_DJI_TakeoffToPoint;
}

class IUAVFlyManage_DJI_TakeoffToPoint : public LyDialog
{
    Q_OBJECT

public:
    explicit IUAVFlyManage_DJI_TakeoffToPoint(QWidget *parent = nullptr);
    ~IUAVFlyManage_DJI_TakeoffToPoint();

public:
	void Init(int nmode = 0);

	void setTargetPointLocation(const double& dlontitude,const double& dlatitude);

	void setSimulationPointLocation(const double& dlontitude, const double& dlatitude);

	void DrawTargetPointData(QPainter &painter, IProj *pProj);

private:
	void UpdateControlStatus(int bstatus = 0);

	QString generateRandomAlphaNumericString(int length = 16);

protected slots:
void on_pushButton_confirm();

void on_pushButton_cancal();

void on_checkBox_isopen_simulator();

void on_pushButton_map_selecting_point();

void on_pushButton_simulator_map_selectedpoint();

void on_pushButton_updateID();
private:
    Ui::IUAVFlyManage_DJI_TakeoffToPoint *ui;

public:
	

	bool b_map_selecting_point = false;

	bool b_simulation_selecting_point = false;

	int n_flyto_mode = 0;

	double d_flyto_lon;
	double d_flyto_lat;
};

#endif // IUAVFLYMANAGE_DJI_TAKEOFFTOPOINT_H
