#include "CDlgForecastTendency.h"
#include "ui_CDlgForecastTendency.h"
#include "QCString.h"
#include "CDlgPlay.h"
#include "lY_TaskManage.h"
#include <QMutex>
#include "DataManage/SystemInfo/SystemInternalInterface.h"
#include "lY_TaskManageInterface.h"
QMutex TelemetryMutex;
CDlgForecastTendency::CDlgForecastTendency(QWidget *parent) :
	LyDialog(tr2("提示"), parent),
    ui(new Ui::CDlgForecastTendency)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	connect(ui->pushButton, &QPushButton::clicked, this, &CDlgForecastTendency::on_pushButton_clicked);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &CDlgForecastTendency::on_pushButton2_clicked);
	connect(ui->pushButton_3, &QPushButton::clicked, this, &CDlgForecastTendency::on_pushButton3_clicked);
}

CDlgForecastTendency::~CDlgForecastTendency()
{
    delete ui;
}
void CDlgForecastTendency::Init(std::vector<std::string> adjustUavTelemetry)
{
	if (adjustUavTelemetry.size() < 2)
		return;
	m_ZoneRuleStu = LY_TaskManage::Get().GetDlgPlay()->m_BaseRuleStu;
	m_adjustUavTelemetry = adjustUavTelemetry;
	LY_TaskManage::Get().GetDlgPlay()->m_VecAdjustUAVTelemetrySpeed.clear();
	m_AdjustUAVMap = LY_TaskManage::Get().GetDlgPlay()->m_SystemSysNameAndID;
	std::string Name1 = m_AdjustUAVMap[adjustUavTelemetry[0]];
	std::string Name2 = m_AdjustUAVMap[adjustUavTelemetry[1]];
	QString str = Name1.c_str();
	str += ",";
	str += Name2.c_str();
	str += tr2("飞机航线冲突，选择需要切换定点模式的飞机");
	ui->pushButton->setVisible(true);
	ui->pushButton_2->setVisible(true);
	ui->label->setText(str);
	ui->pushButton->setText(Name1.c_str());
	ui->pushButton_2->setText(Name2.c_str());

	ui->pushButton_3->setVisible(false);

	QMap<QString, QString> telemetryMap = LY_TaskManage::Get().GetDlgPlay()->m_VecAdjustUAVTelemetrySpeed;
	QList<QString> valueList = telemetryMap.values();
	if (valueList.contains(adjustUavTelemetry[0].c_str()))
		ui->pushButton->setEnabled(false);
	else
		ui->pushButton->setEnabled(true);
	if (valueList.contains(adjustUavTelemetry[1].c_str()))
		ui->pushButton_2->setEnabled(false);
	else
		ui->pushButton_2->setEnabled(true);
	this->setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
}
void CDlgForecastTendency::Init(std::vector<std::string> adjustUavTelemetry, double dHight)
{
	QString str = "";
	str += tr2("四旋翼无人机: ");
	str += tr2(adjustUavTelemetry[0].c_str()) + ",";
	str += tr2(adjustUavTelemetry[1].c_str()) + tr2(" 触发空中碰撞告警，\n冲突消减规则：平台上升 / 下降(高度差 ");
	str += QString::number(dHight) + tr2("米)后恢复航点飞行");
	ui->label->setText(str);
	ui->pushButton->setVisible(false);
	ui->pushButton_2->setVisible(false);
	ui->pushButton_3->setVisible(true);
	this->setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
}
void CDlgForecastTendency::on_pushButton_clicked()
{
	TelemetryMutex.lock();
	LY_TaskManage::Get().GetDlgPlay()->m_VecAdjustUAVTelemetrySpeed[m_adjustUavTelemetry[1].c_str()] = m_adjustUavTelemetry[0].c_str();
	std::vector<SpeedControl>& uavControlVec = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	for (auto& uavcon : uavControlVec)
	{
		if (m_AdjustUAVMap[uavcon.UAVName.c_str()].c_str() == ui->pushButton->text())
		{
			uavcon.speed = 0;
			LY_TaskManage::Get().GetDlgPlay()->m_UAVStatus.nSubductionEnd = 1;
			LY_TaskManage::Get().GetDlgPlay()->m_UAVStatus.nQuadrate = 1;
		}
	}

	TelemetryMutex.unlock();
	close();
}

void CDlgForecastTendency::on_pushButton2_clicked()
{
	TelemetryMutex.lock();

	LY_TaskManage::Get().GetDlgPlay()->m_VecAdjustUAVTelemetrySpeed[m_adjustUavTelemetry[0].c_str()] = m_adjustUavTelemetry[1].c_str();
	std::vector<SpeedControl>& uavControlVec = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	for (auto& uavcon : uavControlVec)
	{
		if (m_AdjustUAVMap[uavcon.UAVName.c_str()].c_str() == ui->pushButton_2->text())
		{
			uavcon.speed = 0;
			LY_TaskManage::Get().GetDlgPlay()->m_UAVStatus.nSubductionEnd = 1;
			LY_TaskManage::Get().GetDlgPlay()->m_UAVStatus.nQuadrate = 1;
		}
	}
	TelemetryMutex.unlock();
	close();
}

void CDlgForecastTendency::on_pushButton3_clicked()
{
	close();
}

void CDlgForecastTendency::SetWaitDecideTime()
{

}