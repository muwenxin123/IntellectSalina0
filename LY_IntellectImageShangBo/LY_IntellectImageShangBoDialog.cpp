#include "LY_IntellectImageShangBoDialog.h"
#include "QCString.h"
LY_IntellectImageShangBoDialog::LY_IntellectImageShangBoDialog(QWidget *parent)
	: LyDialog(tr2("上博智象平台"), parent),
	ui(new Ui::LY_IntellectImageShangBoDialog)
{
	ui->setupUi(this);
	connect(ui->pbtnUp, &QPushButton::clicked, this, &LY_IntellectImageShangBoDialog::on_pbtnUp_clicked);
	connect(ui->pbtnRet, &QPushButton::clicked, this, &LY_IntellectImageShangBoDialog::on_pbtnRet_clicked);
	connect(ui->pbtndown, &QPushButton::clicked, this, &LY_IntellectImageShangBoDialog::on_pbtndown_clicked);
	connect(ui->pbtnLeft, &QPushButton::clicked, this, &LY_IntellectImageShangBoDialog::on_pbtnLeft_clicked);
	connect(ui->pbtnstop, &QPushButton::clicked, this, &LY_IntellectImageShangBoDialog::on_pbtnstop_clicked);
	connect(ui->pbtncentre, &QPushButton::clicked, this, &LY_IntellectImageShangBoDialog::on_pbtncentre_clicked);
	connect(ui->pbtnoffdyname, &QPushButton::clicked, this, &LY_IntellectImageShangBoDialog::on_pbtnoffdyname_clicked);
}

LY_IntellectImageShangBoDialog::~LY_IntellectImageShangBoDialog()
{
}

void LY_IntellectImageShangBoDialog::Init()
{
}

void LY_IntellectImageShangBoDialog::on_pbtnRet_clicked()
{
}

void LY_IntellectImageShangBoDialog::on_pbtndown_clicked()
{
}

void LY_IntellectImageShangBoDialog::on_pbtnLeft_clicked()
{
}

void LY_IntellectImageShangBoDialog::on_pbtnstop_clicked()
{
}

void LY_IntellectImageShangBoDialog::on_pbtncentre_clicked()
{
}

void LY_IntellectImageShangBoDialog::on_pbtnoffdyname_clicked()
{
}

void LY_IntellectImageShangBoDialog::on_pbtnUp_clicked()
{

}
