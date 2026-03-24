#include "CDlgDebug.h"
#include "ui_CDlgDebug.h"
#include "LY_AgroTaskManage.h"

CDlgDebug::CDlgDebug(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgDebug)
{
    ui->setupUi(this);
}

CDlgDebug::~CDlgDebug()
{
    delete ui;
}

void CDlgDebug::on_pushButton_clicked()
{
	if (LY_AgroTaskManage::Get().getPDlgTaskSchedule()->isVisible() == true)
	{
		LY_AgroTaskManage::Get().getPDlgTaskSchedule()->hide();
	}
	else
	{
		LY_AgroTaskManage::Get().getPDlgTaskSchedule()->show();
		LY_AgroTaskManage::Get().getPDlgTaskSchedule()->raise();
	}
}

void CDlgDebug::on_pushButton_2_clicked()
{
	if (LY_AgroTaskManage::Get().getPDlgTaskEventRecord()->isVisible() == true)
	{
		LY_AgroTaskManage::Get().getPDlgTaskEventRecord()->hide();
	}
	else
	{
		LY_AgroTaskManage::Get().getPDlgTaskEventRecord()->show();
		LY_AgroTaskManage::Get().getPDlgTaskEventRecord()->raise();
	}
}

void CDlgDebug::on_pushButton_3_clicked()
{
	if (LY_AgroTaskManage::Get().getPDlgAgroTaskManage()->isVisible() == true)
	{
		LY_AgroTaskManage::Get().getPDlgAgroTaskManage()->hide();
	}
	else
	{
		LY_AgroTaskManage::Get().getPDlgAgroTaskManage()->show();
		LY_AgroTaskManage::Get().getPDlgAgroTaskManage()->raise();
	}
}

void CDlgDebug::on_pushButton_4_clicked()
{
	if (LY_AgroTaskManage::Get().getPDlgCropGrowStageWidget()->isVisible() == true)
	{
		LY_AgroTaskManage::Get().getPDlgCropGrowStageWidget()->hide();
	}
	else
	{
		LY_AgroTaskManage::Get().getPDlgCropGrowStageWidget()->show();
		LY_AgroTaskManage::Get().getPDlgCropGrowStageWidget()->raise();
	}
}
