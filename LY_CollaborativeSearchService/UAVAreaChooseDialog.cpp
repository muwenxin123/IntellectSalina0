#include "UAVAreaChooseDialog.h"
#include "ui_UAVAreaChooseDialog.h"
#include "ICollaborativeSearchActivator.h"
#include "QCString.h"
#include "LY_CollaborativeSearchService.h"

UAVAreaChooseDialog::UAVAreaChooseDialog(QWidget *parent) :
	LyDialog(tr2("区域选择"), parent),
    ui(new Ui::UAVAreaChooseDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	m_pIZoneManageService = ICollaborativeSearchActivator::Get().getPIZoneManageService();
	connect(ui->pbtn_OK, &QPushButton::clicked, this, &UAVAreaChooseDialog::on_pbtn_OK_clicked);
	connect(ui->pbtn_Cancel, &QPushButton::clicked, this, &UAVAreaChooseDialog::on_pbtn_Cancel_clicked);
}

UAVAreaChooseDialog::~UAVAreaChooseDialog()
{
    delete ui;
}

void UAVAreaChooseDialog::init()
{
	qnzkna::ZoneManage::CZoneBaseVec  zoneVec = ICollaborativeSearchActivator::Get().getPIZoneManageService()->GetAllZoneBase();
	ui->comboBox_takeoff->clear();
	ui->comboBox_in->clear();
	ui->comboBox_out->clear();

	ui->comboBox_takeoff->addItem("");
	ui->comboBox_in->addItem("");
	ui->comboBox_out->addItem("");
	for (auto item : zoneVec)
	{
		if (item.nProperty == qnzkna::ZoneManage::CZoneBase::ZoneProperty::QJ)
		{
			ui->comboBox_takeoff->addItem(tr2(item.strID));
		}
		if (item.nProperty == qnzkna::ZoneManage::CZoneBase::ZoneProperty::HL)
		{
			ui->comboBox_in->addItem(tr2(item.strID));
			ui->comboBox_out->addItem(tr2(item.strID));
		}
	}

	ui->comboBox_takeoff->setCurrentText(tr2("起降区"));
	ui->comboBox_in->setCurrentText(tr2("航路_进"));
	ui->comboBox_out->setCurrentText(tr2("航路_出"));
}

void UAVAreaChooseDialog::on_pbtn_OK_clicked()
{
	QString takeoffarea = ui->comboBox_takeoff->currentText();
    QString inLine = ui->comboBox_in->currentText();
	QString outLine = ui->comboBox_out->currentText();

	LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->strTakeOffArea = takeoffarea;
	LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->strEnterLine = inLine;
	LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->strOutLine = outLine;

	LY_CollaborativeSearchService::Get().m_uav_dispose_route_dialog->clearAll();
	close();

}

void UAVAreaChooseDialog::on_pbtn_Cancel_clicked()
{
    this->close();
}
