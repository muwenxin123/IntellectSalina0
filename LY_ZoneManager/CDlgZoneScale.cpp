#include "CDlgZoneScale.h"
#include "ui_CDlgZoneScale.h"
#include "QCString.h"
#include "IZoneManagerActivator.h"
#include "QSettings"

CDlgZoneScale::CDlgZoneScale(QWidget *parent) :
	LyDialog(tr2("区域显示缩放设置"), parent),
	ui(new Ui::CDlgZoneScale)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(430, 130);

	m_pIZoneManageService = IZoneManagerActivator::Get().getPIZoneManageService();

	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
	connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));

	InitCtrl();
}

CDlgZoneScale::~CDlgZoneScale()
{
    delete ui;
}

void CDlgZoneScale::InitCtrl()
{
	QStringList strList;
	strList << tr2("航路");
	ui->comboBox->addItems(strList);

	ui->doubleSpinBox->setValue(1);
	ui->doubleSpinBox_2->setValue(1);
	ui->spinBox->setValue(-1);
}

void CDlgZoneScale::LoadIni()
{
	QSettings settings("../data/conf/ZoneShowScale.ini", QSettings::IniFormat);
	settings.setIniCodec(QTextCodec::codecForName("utf-8"));
	settings.beginGroup("ZONE_SCALE_SETTING");

	double d1(0), d2(0);
	int n1(0);
	if (settings.contains("WidthScale") && settings.value("WidthScale").canConvert(QVariant::Type::Double))
	{
		d1 = settings.value("WidthScale", "").toDouble();
	}
	if (settings.contains("HeightScale") && settings.value("HeightScale").canConvert(QVariant::Type::Double))
	{
		d2 = settings.value("HeightScale", "").toDouble();
	}
	if (settings.contains("Diaphaneity") && settings.value("Diaphaneity").canConvert(QVariant::Type::Int))
	{
		n1 = settings.value("Diaphaneity", "").toInt();
	}
	settings.endGroup();

	ui->doubleSpinBox->setValue(d1);
	ui->doubleSpinBox_2->setValue(d2);
	ui->spinBox->setValue(n1);

}

void CDlgZoneScale::showEvent(QShowEvent *event)
{
	LoadIni();
}

void CDlgZoneScale::on_pushButton_clicked()
{
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}
	double d1 = ui->doubleSpinBox->value();
	double d2 = ui->doubleSpinBox_2->value();
	int n1 = ui->spinBox->value();

	m_pIZoneManageService->SetZoneShowScale(qnzkna::ZoneManage::CZoneBase::LINE, d1, d2, n1);
}

void CDlgZoneScale::on_pushButton_2_clicked()
{
	ui->doubleSpinBox->setValue(1);
	ui->doubleSpinBox_2->setValue(1);
	ui->spinBox->setValue(-1);

	if (m_pIZoneManageService == nullptr)
	{
		return;
	}
	m_pIZoneManageService->SetZoneShowScaleDefault(qnzkna::ZoneManage::CZoneBase::LINE);
}

void CDlgZoneScale::on_pushButton_3_clicked()
{
	double d1 = ui->doubleSpinBox->value();
	double d2 = ui->doubleSpinBox_2->value();
	int n1 = ui->spinBox->value();

	QSettings settings("../data/conf/ZoneShowScale.ini", QSettings::IniFormat);
	settings.beginGroup("ZONE_SCALE_SETTING");
	settings.setValue("WidthScale", d1);
	settings.setValue("HeightScale", d2);
	settings.setValue("Diaphaneity", n1);
	settings.endGroup();
}