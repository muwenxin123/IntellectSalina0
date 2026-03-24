#ifndef LY_ADDMOINTTYPEWIDGET_H
#define LY_ADDMOINTTYPEWIDGET_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/AgriFarm/AgriHomePageDecl.h"
namespace Ui {
class Ly_AddMointTypeWidget;
}

class Ly_AddMointTypeWidget : public LyDialog
{
    Q_OBJECT

public:
    explicit Ly_AddMointTypeWidget(QWidget *parent = nullptr);
    ~Ly_AddMointTypeWidget();

signals:

	void getMointName(SensorType &type);

	private slots:

	void on_sensor_type_box(const int &index);
private:
    Ui::Ly_AddMointTypeWidget *ui;
	SensorType m_sensorType;
	QString sensor_name = nullptr;
	
};

#endif // LY_ADDMOINTTYPEWIDGET_H
