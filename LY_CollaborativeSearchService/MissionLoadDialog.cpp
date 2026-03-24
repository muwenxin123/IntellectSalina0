#include "MissionLoadDialog.h"
#include "ui_MissionLoadDialog.h"
#include "LY_CollaborativeSearchService.h"
#include "Zone/ZoneManagerInfo.h"
#include "RoutePlanningDialog.h"
#include "QCString.h"

MissionLoadDialog::MissionLoadDialog(QWidget *parent) :
	LyDialog(tr2("任务区域信息"), parent),
    ui(new Ui::MissionLoadDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	m_pIZoneManageService = ICollaborativeSearchActivator::Get().getPIZoneManageService();
	connect(ui->pbtn_OK, &QPushButton::clicked, this, &MissionLoadDialog::on_pbtn_OK_clicked);
	connect(ui->pbtn_Cancel, &QPushButton::clicked, this, &MissionLoadDialog::on_pbtn_Cancel_clicked);
}

MissionLoadDialog::~MissionLoadDialog()
{
    delete ui;
}

void MissionLoadDialog::init(QString missionName)
{
	ui->comboBox_2->clear();
	ui->comboBox_3->clear();
	ui->comboBox_4->clear();
	ui->comboBox_5->clear();
	qnzkna::ZoneManage::CZoneBaseVec  zoneVec = ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetAllZoneBase();
	ui->comboBox_2->addItem("");
	ui->comboBox_3->addItem("");
	ui->comboBox_4->addItem("");
	ui->comboBox_5->addItem("");
	for (auto item : zoneVec)
	{
		if (item.nProperty == qnzkna::ZoneManage::CZoneBase::ZoneProperty::QJ)
		{
			ui->comboBox_2->addItem(tr2(item.strID));
		}
		if (item.nProperty == qnzkna::ZoneManage::CZoneBase::ZoneProperty::HL)
		{
			ui->comboBox_3->addItem(tr2(item.strID));
			ui->comboBox_4->addItem(tr2(item.strID));
		}
		if (item.nProperty == qnzkna::ZoneManage::CZoneBase::ZoneProperty::ZY)
		{
			ui->comboBox_5->addItem(tr2(item.strID));
		}
	}

	ui->comboBox_2->setCurrentIndex(ui->comboBox_2->count() - 1);
	ui->comboBox_3->setCurrentIndex(ui->comboBox_3->count() - 1);
	ui->comboBox_4->setCurrentIndex(ui->comboBox_4->count() - 1);
	ui->comboBox_5->setCurrentIndex(ui->comboBox_5->count() - 1);
}

void MissionLoadDialog::on_pbtn_OK_clicked()
{
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_takeoffArea = ui->comboBox_2->currentText();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_enterLine = ui->comboBox_3->currentText();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_outLine = ui->comboBox_4->currentText();
	LY_CollaborativeSearchService::Get().m_RoutePlanningDialog->m_workArea = ui->comboBox_5->currentText();
	close();
}

void MissionLoadDialog::on_pbtn_Cancel_clicked()
{
    this->close();
}
