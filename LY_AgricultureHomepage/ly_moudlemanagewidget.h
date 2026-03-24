#ifndef LY_MOUDLEMANAGEWIDGET_H
#define LY_MOUDLEMANAGEWIDGET_H

#include <QWidget>
#include <QTimer>
#include "ly_mointorviewmodel.h"
#include "ly_cameralayoutwidget.h"
#include "DataManage/AgriFarm/AgriHomePageDecl.h"
#include "LY_AgricultureHomepageActivator.h"

namespace Ui {
	class LY_MoudleManageWidget;
}

class LY_MoudleManageWidget : public QWidget
{
	Q_OBJECT

protected:

	void paintEvent(QPaintEvent *event);

	
public:
	explicit LY_MoudleManageWidget(QWidget *parent = nullptr);
	~LY_MoudleManageWidget();
	void updateData();
	void draw3dText();
	bool DrawDynamic(QPainter &painter, IProj *pProj);

private slots:

	void clickDeviceMonitor();
	void clickMeteorMonitor();
	void clickSoilMonitor();
	void clickWormMonitor();
	void clickVideoMonitor();

	void  closeWidget();
	void showWidget(SensorListModel &model, int column);
	void handleDoubleClickTreeItem(SensorListModel &model, int column);
	void showWeather(QString &tem, QString &hum);
	void showParentWidget(QString str, int value);
	
	void  setSoilData(Soil s);

	void getSoilSensorDataType(QVector <SoilSensorData> vec);
	void on_soil_typeChanged(const int &);
	void getSoilHistoryList(SoilHistoryData data);

private:
	Ui::LY_MoudleManageWidget *ui;
	//LY_CameraLayoutWidget *m_cameraLayoutWidget;
	CameraWidget *m_cameraWidget;
	
	SoilSensorData m_data;
	std::string soidData = "";
	SensorListModel m_sensorListModel;
	bool m_isClickSoil = false;

	QString th = "";
	QString pe = "";
	QString k = "";
	QString np = "";
};

#endif // LY_MOUDLEMANAGEWIDGET_H
