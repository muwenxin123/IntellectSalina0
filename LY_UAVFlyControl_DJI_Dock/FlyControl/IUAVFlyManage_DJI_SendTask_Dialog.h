#ifndef IUAVFLYMANAGE_DJI_SENDTASK_DIALOG_H
#define IUAVFLYMANAGE_DJI_SENDTASK_DIALOG_H

#include <QWidget>

#include "LyDialog.h"
#include "UAVFlyManage_DJI_DockDialog.h"
namespace Ui {
class IUAVFlyManage_DJI_SendTask_Dialog;
}

class IUAVFlyManage_DJI_SendTask_Dialog : public LyDialog
{
    Q_OBJECT

public:
    explicit IUAVFlyManage_DJI_SendTask_Dialog(QWidget *parent = nullptr);
    ~IUAVFlyManage_DJI_SendTask_Dialog();

	void Init();

	void setWaylineURLorFingerpoint(const QString& url, const QString& md5);

	void setMapSelectedPointEdit(const double& dlon, const double& dlat);

	void getListObjectsInBucket();

	double getReturnRouteHeight();
private:

	int getCurrentTaskTypeButton();

	int getCurrentWaylinePrecision();

	QString generateRandomAlphaNumericString(int length = 16);

private slots:
	void on_pushButton_DJI_flighttask_confirm();

	void on_pushButton_DJI_flighttask_cancel();

	void on_pushButton_routeline_file_load();

	void on_pushButton_DJITask_general();

	void on_pushButton_DJITask_leapfrog();

	void on_pushButton_DJITask_RunCondition();

	void on_pushButton__wayline_precision_GPS();

	void on_pushButton__wayline_precision__RTK();

	void on_pushButton_simulation_open();

	void on_pushButton_safecheck_open();

	void on_comboBox_lineEdit_DJI_flighttask_files();

	void on_pushButton_mapSelectPoint();

	void on_pushButton_updateName();

public:
	bool b_isMapPoint = false;
private:
    Ui::IUAVFlyManage_DJI_SendTask_Dialog *ui;

	qnzkna::DjiSdkApi::DjiCloudApi::DjiFlighttaskPrepare* m_DjiFlightTask;
};

#endif // IUAVFLYMANAGE_DJI_SENDTASK_DIALOG_H
