#ifndef CDLGPLAYSPEEDCONTROL_H
#define CDLGPLAYSPEEDCONTROL_H

#include <QWidget>
#include "LyDialog.h"
#include "CDlgPlay.h"
namespace Ui {
class CDlgPlaySpeedControl;
}

class CDlgPlaySpeedControl : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgPlaySpeedControl(QWidget *parent = nullptr);
    ~CDlgPlaySpeedControl();

	void Init(std::vector<SpeedControl> UAVSpeedControl);

	void UpdataTableWidget(std::vector<SpeedControl> UAVSpeedControl);

public:
	bool b_CheckRouteSafety = false;
	std::map<std::string, std::string> m_AdjustUAVMap;
private slots:
void on_pbtnAddSpeed_clicked();
void on_pbtndecreas_clicked();
void on_pbtnAddLine_clicked();
void on_pbtndecreasLine_clicked();
void on_tableWidget_currentCellChanged();
void on_pbtnUp_clicked();
void on_pbtndown_clicked();
void on_pbtnRecover_clicked();
void on_radioButtCheck_clicked();
private:
    Ui::CDlgPlaySpeedControl *ui;
};

#endif
