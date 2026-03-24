#include "CDlgAgroSampleParaShow.h"
#include "ui_CDlgAgroSampleParaShow.h"
#include "QCString.h"

CDlgAgroSampleParaShow::CDlgAgroSampleParaShow(QWidget *parent) :
	LyDialog(tr2("¹æÔòÄÚÈİ"), parent),
	ui(new Ui::CDlgAgroSampleParaShow)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(450, 300);

	InitCtrl();
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
}

CDlgAgroSampleParaShow::~CDlgAgroSampleParaShow()
{
    delete ui;
}

void CDlgAgroSampleParaShow::SetStringName(const QString& strTmp)
{
	ui->label_2->setText(strTmp);
}

void CDlgAgroSampleParaShow::SetStringNYCS(const QString& strTmp)
{
	ui->textEdit->setText(strTmp);
}

void CDlgAgroSampleParaShow::InitCtrl()
{
	ui->label_2->setText("");
	ui->textEdit->setText("");
}

void CDlgAgroSampleParaShow::hideEvent(QHideEvent *event)
{
	InitCtrl();
}

void CDlgAgroSampleParaShow::on_pushButton_clicked()
{
	InitCtrl();
	this->hide();
}
