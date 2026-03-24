#include "CDlgZoneRuleManage.h"
#include "ui_CDlgZoneRuleManage.h"
#include "QCString.h"
#include "IZoneManagerActivator.h"
#include "LyMessageBox.h"
#include "LY_ZoneManager.h"

CDlgZoneRuleManage::CDlgZoneRuleManage(QWidget *parent) :
	LyDialog(tr2("区域规则管理"), parent),
	ui(new Ui::CDlgZoneRuleManage)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(630, 400);

	m_pIZoneManageService = IZoneManagerActivator::Get().getPIZoneManageService();

	InitData();
	CreateListHeader();
	UpdatelistData();

	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
	connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
	connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));
	connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(on_pushButton_7_clicked()));
	connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(on_pushButton_8_clicked()));
	connect(ui->tableWidget, &QTableWidget::doubleClicked, this, &CDlgZoneRuleManage::on_tableWidget_doubleclicked);

	ui->tabWidget->setCurrentIndex(0);
}

CDlgZoneRuleManage::~CDlgZoneRuleManage()
{
    delete ui;
}

void CDlgZoneRuleManage::InitData()
{
	QStringList strComList;

	strComList.clear();
	strComList << tr2("放弃计划") << tr2("边界融合");
	ui->comboBox_3->addItems(strComList);

	strComList.clear();
	strComList << tr2("区域优先") << tr2("航道避让");
	ui->comboBox_4->addItems(strComList);

	strComList.clear();
	strComList << tr2("驳回申请") << tr2("延迟起飞时间") << tr2("最优路线切换");
	ui->comboBox_5->addItems(strComList);

	ui->spinBox_3->setValue(100);
	ui->spinBox_4->setValue(10);
}

void CDlgZoneRuleManage::UpdateRuleData()
{
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	qnzkna::ZoneManage::CZoneBaseRule stRule;
	m_pIZoneManageService->GetZoneRule(stRule);

	ui->spinBox->setValue(stRule.nZoneSafeDis);
	ui->spinBox_2->setValue(stRule.nZoneDangerDis);
	ui->spinBox_5->setValue(stRule.nUAVSafeDis);
	ui->spinBox_6->setValue(stRule.nWaitTime);
	ui->spinBox_7->setValue(stRule.nWaitTime);
	ui->spinBox_8->setValue(stRule.nAltDiff);

}

void CDlgZoneRuleManage::CreateListHeader()
{
	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("规则类型") << tr2("规则描述");
	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	for (int i = 0; i < headerlabels.size(); i++)
	{
		if (i == 0)
		{
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
		}
		else if ( i == 2)
		{
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
		}
		else
		{
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
		}
	}
}

void CDlgZoneRuleManage::UpdatelistData()
{
	QStringList strList1, strList2;

	strList1.push_back(tr2("禁飞区"));
	strList2.push_back(tr2("管理区;医院;学校;大厦;"));

	strList1.push_back(tr2("限飞区"));
	strList2.push_back(tr2("管理区;医院;学校;大厦;"));

	strList1.push_back(tr2("危险区"));
	strList2.push_back(tr2("气流区;高压线;"));

	strList1.push_back(tr2("起降区"));
	strList2.push_back(tr2("充电点;起降场;"));

	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(strList1.size());
	ui->tableWidget->setSortingEnabled(false);

	QString strNo, strName, strType, strProperty;
	for (int i = 0;i<strList1.size() ; i++)
	{
		strNo = QString::number(i).sprintf("%02d", i + 1);
		strName = strList1[i];
		strType = strList2[i];
		QTableWidgetItem* item0 = new QTableWidgetItem(strNo);
		QTableWidgetItem* item1 = new QTableWidgetItem(strName);
		QTableWidgetItem* item2 = new QTableWidgetItem(strType);

		ui->tableWidget->setItem(i, 0, item0);
		ui->tableWidget->setItem(i, 1, item1);
		ui->tableWidget->setItem(i, 2, item2);

		ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	}
}

void CDlgZoneRuleManage::UpdateTablewidget(int row, std::vector<QString>selectRule)
{
	QString ruleInfo;
	for (auto data:selectRule)
	{
		ruleInfo += data;
		ruleInfo += ";";
	}
	ui->tableWidget->item(row, 2)->setText(ruleInfo);
}

void CDlgZoneRuleManage::showEvent(QShowEvent *event)
{
	UpdateRuleData();
}

void CDlgZoneRuleManage::on_pushButton_clicked()
{
	int n1(0), n2(0),n3(0);
	n1 = ui->spinBox->value();
	n2 = ui->spinBox_2->value();
	n3 = ui->spinBox_5->value();

	qnzkna::ZoneManage::CZoneBaseRule stRule;
	m_pIZoneManageService->GetZoneRule(stRule);

	stRule.nZoneSafeDis = n1;
	stRule.nZoneDangerDis = n2;
	stRule.nUAVSafeDis = n3;

	bool success = m_pIZoneManageService->SetZoneRule(stRule);
	if (success == false)
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("区域安全距离设置失败。"));
	}
	else
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("区域安全距离设置成功。"));
	}
}

void CDlgZoneRuleManage::on_pushButton_2_clicked()
{
	UpdateRuleData();
}

void CDlgZoneRuleManage::on_pushButton_3_clicked()
{
	int n1(0);
	n1 = ui->spinBox_6->value();

	qnzkna::ZoneManage::CZoneBaseRule stRule;
	m_pIZoneManageService->GetZoneRule(stRule);

	stRule.nWaitTime = n1;

	bool success = m_pIZoneManageService->SetZoneRule(stRule);
	if (success == false)
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("接近区域处置规则设置失败。"));
	}
	else
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("接近区域处置规则设置成功。"));
	}
}

void CDlgZoneRuleManage::on_pushButton_4_clicked()
{
	UpdateRuleData();
}

void CDlgZoneRuleManage::on_pushButton_7_clicked()
{
	int n1(0), n2(0);
	n1 = ui->spinBox_7->value();
	n2 = ui->spinBox_8->value();

	qnzkna::ZoneManage::CZoneBaseRule stRule;
	m_pIZoneManageService->GetZoneRule(stRule);

	stRule.nWaitTime = n1;
	stRule.nAltDiff = n2;

	bool success = m_pIZoneManageService->SetZoneRule(stRule);
	if (success == false)
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("飞行碰撞处置规则设置失败。"));
	}
	else
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("飞行碰撞处置规则设置成功。"));
	}
}

void CDlgZoneRuleManage::on_pushButton_8_clicked()
{
	UpdateRuleData();
}

void CDlgZoneRuleManage::on_tableWidget_doubleclicked()
{
	int row = ui->tableWidget->currentRow();
	if (row<0)
	{
		return;
	}
	QString rule= ui->tableWidget->item(row, 1)->text();
	QString selectRuleInfo = ui->tableWidget->item(row, 2)->text();
	QStringList selectRuleList = selectRuleInfo.split(";");
	selectRuleList.pop_back();
	if (LY_ZoneManager::Get().GetPZoneTypeMatchDialog() == nullptr)
	{
		return;
	}

	if (LY_ZoneManager::Get().GetPZoneTypeMatchDialog()->isHidden())
	{
		LY_ZoneManager::Get().GetPZoneTypeMatchDialog()->Init(row, rule, selectRuleList);
		LY_ZoneManager::Get().GetPZoneTypeMatchDialog()->show();
	}
	else
	{
		LY_ZoneManager::Get().GetPZoneTypeMatchDialog()->hide();
	}
}
