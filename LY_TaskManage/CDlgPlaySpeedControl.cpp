#include "CDlgPlaySpeedControl.h"
#include "ui_CDlgPlaySpeedControl.h"
#include <QCString.h>
#include <QMutex>
#include "lY_TaskManage.h"
#include <QtPositioning/QGeoCoordinate>
QMutex MutexAmount;
CDlgPlaySpeedControl::CDlgPlaySpeedControl(QWidget *parent) :
	LyDialog(tr2("飞行控制"), parent),
	ui(new Ui::CDlgPlaySpeedControl)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	connect(ui->tableWidget, &QTableWidget::currentCellChanged, this, &CDlgPlaySpeedControl::on_tableWidget_currentCellChanged);
	connect(ui->pbtnAddSpeed, &QPushButton::clicked, this, &CDlgPlaySpeedControl::on_pbtnAddSpeed_clicked);
	connect(ui->pbtndecreas, &QPushButton::clicked, this, &CDlgPlaySpeedControl::on_pbtndecreas_clicked);
	connect(ui->pbtnAddLine, &QPushButton::clicked, this, &CDlgPlaySpeedControl::on_pbtnAddLine_clicked);
	connect(ui->pbtndecreasLine, &QPushButton::clicked, this, &CDlgPlaySpeedControl::on_pbtndecreasLine_clicked);
	connect(ui->pbtnUp, &QPushButton::clicked, this, &CDlgPlaySpeedControl::on_pbtnUp_clicked);
	connect(ui->pbtndown, &QPushButton::clicked, this, &CDlgPlaySpeedControl::on_pbtndown_clicked);
	connect(ui->pbtnRecover, &QPushButton::clicked, this, &CDlgPlaySpeedControl::on_pbtnRecover_clicked);
	connect(ui->radioButtCheck, &QPushButton::clicked, this, &CDlgPlaySpeedControl::on_radioButtCheck_clicked);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

CDlgPlaySpeedControl::~CDlgPlaySpeedControl()
{
    delete ui;
}

void CDlgPlaySpeedControl::Init(std::vector<SpeedControl> UAVSpeedControl)
{
	ui->tableWidget->clear();
	QStringList header;
	m_AdjustUAVMap = LY_TaskManage::Get().GetDlgPlay()->m_SystemSysNameAndID;
	header << tr2("名称") << tr2("速率") << tr2("偏移量(DD)");
	ui->tableWidget->setColumnCount(header.size());
	ui->tableWidget->setHorizontalHeaderLabels(header);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	ui->tableWidget->setRowCount(UAVSpeedControl.size());
	ui->tableWidget->setFocusPolicy(Qt::StrongFocus);
	int row = 0;
	for (auto uav : UAVSpeedControl)
	{
		ui->tableWidget->setItem(row, 0, new QTableWidgetItem(tr2(m_AdjustUAVMap[uav.UAVName].c_str())));
		ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(uav.speed)));
		ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(uav.amount)));
		++row;
	}

	QIntValidator* intValidator = new QIntValidator(this);
	intValidator->setRange(0, 100000);
	ui->leAmount->setValidator(intValidator);
	b_CheckRouteSafety = ui->radioButtCheck->isChecked();
}
void CDlgPlaySpeedControl::UpdataTableWidget(std::vector<SpeedControl> UAVSpeedControl)
{
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(UAVSpeedControl.size());
	int row = 0;
	for (auto uav : UAVSpeedControl)
	{
		ui->tableWidget->setItem(row, 0, new QTableWidgetItem(tr2(uav.UAVName.c_str())));
		ui->tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(uav.speed)));
		ui->tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(uav.amount)));
		++row;
	}
}
void CDlgPlaySpeedControl::on_pbtnAddSpeed_clicked()
{
	QString text = ui->leUAVName->text();
	std::vector<SpeedControl>& uavControlVec = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	for (auto& scp : uavControlVec)
	{
		if(text != scp.UAVName.c_str())
			continue;
		if (scp.speed == 0)
			scp.speed = 1;
		else
		{
			MutexAmount.lock();
			scp.speed *= 2;
			MutexAmount.unlock();
			ui->lespeed->setText(QString::number(scp.speed));
			break;
		}
	}

	UpdataTableWidget(uavControlVec);
}
void CDlgPlaySpeedControl::on_pbtndecreas_clicked()
{
	QString text = ui->leUAVName->text();
	std::vector<SpeedControl>& uavControlVec = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	for (auto& scp : uavControlVec)
	{
		if (text != scp.UAVName.c_str())
			continue;
		MutexAmount.lock();
		scp.speed /= 2;
		MutexAmount.unlock();
		ui->lespeed->setText(QString::number(scp.speed));
		break;
	}
	UpdataTableWidget(uavControlVec);
}
void CDlgPlaySpeedControl::on_tableWidget_currentCellChanged()
{
	if (nullptr == ui->tableWidget->currentItem())
		return;
	int row = ui->tableWidget->currentRow();
	QString currentItem = ui->tableWidget->item(row, 0)->text();
	ui->leUAVName->setText(currentItem);
	QString currentItemValue = ui->tableWidget->item(row, 1)->text();
	ui->lespeed->setText(currentItemValue);
	QString currentItemAmount = ui->tableWidget->item(row, 2)->text();
	ui->leAmount->setText(currentItemAmount);
}

void CDlgPlaySpeedControl::on_pbtndecreasLine_clicked()
{
	if ("" == ui->leUAVName->text())
		return;
	QString currentItem = ui->leUAVName->text();

	double amount = ui->leAmount->text().toDouble();

	double& dlon = LY_TaskManage::Get().GetDlgPlay()->dLongitude;
	const double metersPerDegree = 111320.0;
	MutexAmount.lock();
	dlon += amount / metersPerDegree;
	MutexAmount.unlock();

	auto& SpeedControl = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	if (SpeedControl.empty())
		return;

	for (auto& data : SpeedControl)
	{
		if (m_AdjustUAVMap[data.UAVName].c_str() != currentItem)
			continue;
		data.amount = dlon;
		break;
	}

	UpdataTableWidget(SpeedControl);
}
void CDlgPlaySpeedControl::on_pbtnAddLine_clicked()
{
	if ("" == ui->leUAVName->text())
		return;
	QString currentItem = ui->leUAVName->text();

	double amount = ui->leAmount->text().toDouble();

	double& dlon = LY_TaskManage::Get().GetDlgPlay()->dLongitude;
	const double metersPerDegree = 111320.0;
	MutexAmount.lock();
	dlon += amount / metersPerDegree * -1;
	MutexAmount.unlock();

	auto& SpeedControl = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	if (SpeedControl.empty())
		return;

	for (auto& data : SpeedControl)
	{
		if (m_AdjustUAVMap[data.UAVName].c_str() != currentItem)
			continue;
		data.amount = -dlon;
		break;
	}

	UpdataTableWidget(SpeedControl);
}
void CDlgPlaySpeedControl::on_pbtnUp_clicked()
{

	if ("" == ui->leUAVName->text())
		return;
	QString currentItem = ui->leUAVName->text();

	double amount = ui->leAmount->text().toDouble();

	double& dlat = LY_TaskManage::Get().GetDlgPlay()->dLatitude;
	const double metersPerDegree = 111320.0;
	MutexAmount.lock();
	dlat += amount / metersPerDegree;
	MutexAmount.unlock();

	auto& SpeedControl = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	if (SpeedControl.empty())
		return;

	for (auto& data : SpeedControl)
	{
		if (m_AdjustUAVMap[data.UAVName].c_str() != currentItem)
			continue;
		data.amount = dlat;
		break;
	}

	UpdataTableWidget(SpeedControl);
}

void CDlgPlaySpeedControl::on_pbtndown_clicked()
{
	if ("" == ui->leUAVName->text())
		return;
	QString currentItem = ui->leUAVName->text();

	double amount = ui->leAmount->text().toDouble();

	double& dlat = LY_TaskManage::Get().GetDlgPlay()->dLatitude;
	const double metersPerDegree = 111320.0;
	MutexAmount.lock();
	dlat += amount / metersPerDegree * -1;
	MutexAmount.unlock();

	auto& SpeedControl = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	if (SpeedControl.empty())
		return;

	for (auto& data : SpeedControl)
	{
		if (m_AdjustUAVMap[data.UAVName].c_str() != currentItem)
			continue;
		data.amount = -dlat;
		break;
	}

	UpdataTableWidget(SpeedControl);
}

void CDlgPlaySpeedControl::on_pbtnRecover_clicked()
{

	if ("" == ui->leUAVName->text())
		return;
	QString currentItem = ui->leUAVName->text();

	double amount = ui->leAmount->text().toDouble();

	MutexAmount.lock();
	LY_TaskManage::Get().GetDlgPlay()->dLatitude = 0;
	LY_TaskManage::Get().GetDlgPlay()->dLongitude = 0;
	MutexAmount.unlock();

	auto& SpeedControl = LY_TaskManage::Get().GetDlgPlay()->m_PlatSpeedCol;
	if (SpeedControl.empty())
		return;

	for (auto& data : SpeedControl)
	{
		if (m_AdjustUAVMap[data.UAVName].c_str() != currentItem)
			continue;
		data.amount = 0;
		break;
	}

	UpdataTableWidget(SpeedControl);

}
void CDlgPlaySpeedControl::on_radioButtCheck_clicked()
{
	b_CheckRouteSafety = ui->radioButtCheck->isChecked();
}