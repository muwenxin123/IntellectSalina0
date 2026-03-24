#ifndef IUAVFLYMANAGE_DJI_WAYLINE_UPLOADDIALOG_H
#define IUAVFLYMANAGE_DJI_WAYLINE_UPLOADDIALOG_H

#include <QWidget>
#include "LyDialog.h"
#include "IUAVFlyControl_DJI_DockActivator.h"
#include "LY_UAVFlyControl_DJI_Dock.h"

#include "wpml_codec/DJI_UAVName2Type.h"

#include <QPainter>
#include "IProj.h"
#include "wpml_codec/wpml_codec.h"
#include <QTableWidgetItem>

#include <QTimer>
namespace Ui {
class IUAVFlyManage_DJI_WayLine_UploadDialog;
}

class IUAVFlyManage_DJI_WayLine_UploadDialog : public LyDialog
{
    Q_OBJECT

public:
    explicit IUAVFlyManage_DJI_WayLine_UploadDialog(QWidget *parent = nullptr);
    ~IUAVFlyManage_DJI_WayLine_UploadDialog();

	void Init();

	bool getWidgetEnable();

	protected slots:

	void on_pushButton_VehicleUpDownMissionWayPointAltitude();

	void on_doubleSpinBox_VehicleUpDownMissionWayPointSpeed();

	void on_tableWidget_currentItemChanged(QTableWidgetItem* item);

	void slots_uploadRoute();

	void slots_downloadRoute();

	void onTimerTimeout();

public:
	void DrawRouteLine(QPainter & painter, IProj * pProj);

	void AddRoutePoint(const double& dlon, const double& dlat);

	bool createWaylineStruct();
	
private:
    Ui::IUAVFlyManage_DJI_WayLine_UploadDialog *ui;

	PayloadEnum2Type										*m_payloadEnum2Type = nullptr;
	DroneEnum2Type											*m_droneEnum2Type = nullptr;

	std::string												savePath = "../data/wpmz/wayline/";

	wcs::WPMZData m_wpmzData;

	QTimer*														m_timer;

	bool														b_timer_success = false;

	double														d_phase;// 用于控制正弦周期-呼吸频率
	
};

#endif // IUAVFLYMANAGE_DJI_WAYLINE_UPLOADDIALOG_H
