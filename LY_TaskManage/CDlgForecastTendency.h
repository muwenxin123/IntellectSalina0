#ifndef CDLGFORECASTTENDENCY_H
#define CDLGFORECASTTENDENCY_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/IZoneManageService.h"
namespace Ui {
class CDlgForecastTendency;
}

class CDlgForecastTendency : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgForecastTendency(QWidget *parent = nullptr);
    ~CDlgForecastTendency();

	void Init(std::vector<std::string> adjustUavTelemetry);
	void Init(std::vector<std::string> adjustUavTelemetry,double dHight);

	std::vector<std::string> m_adjustUavTelemetry;
	std::map<std::string, std::string> m_AdjustUAVMap;
	qnzkna::ZoneManage::CZoneBaseRule					m_ZoneRuleStu;
public:
	void SetWaitDecideTime();
private slots:
	void on_pushButton_clicked();
	void on_pushButton2_clicked();
	void on_pushButton3_clicked();
private:
    Ui::CDlgForecastTendency *ui;
};

#endif
