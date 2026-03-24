#ifndef LY_MOINTERADDWIDGET_H
#define LY_MOINTERADDWIDGET_H

#include <QWidget>
#include "LyDialog.h"
#include "ly_mointorviewmodel.h"
#include "DataManage/AgriFarm/AgriHomePageDecl.h"

using namespace std;
namespace Ui {
class LY_MointerAddWidget;
}



class LY_MointerAddWidget : public LyDialog
{
    Q_OBJECT

public:
    explicit LY_MointerAddWidget(QWidget *parent = nullptr);
    ~LY_MointerAddWidget();

	
	void setType(int type);
	void setSensorType(SensorType type);
	void setData(SensorListModel& listModel);


signals:

	void addSensor(SensorListModel &model);
	void updateSensor(SensorListModel &model);

	public slots :

     void sureAction();
     void cancleAction();
	 void getScreenPoint();
     void on_isShow_stateChanged(int arg1);
     void on_isFieldOfView_stateChanged(int arg1);
     void on_isStatus_stateChanged(int arg1);
	 void on_elevation_slider_valueChanged(int value);
	 void on_depression_slider_valueChanged(int value);



private:
	//MonitManageType m_type;
    Ui::LY_MointerAddWidget *ui;
	LY_MointorViewModel *m_viewModel;
	SensorType m_sensorType;
	int m_type; //1 添加  2 编辑

	bool                                         isShow = true;//显示隐藏
	bool                                         isStatus = true;//状态隐藏
	bool                                         isMap = true;//视场隐藏
	
	SensorListModel   currentModel;

};

#endif // LY_MOINTERADDWIDGET_H
