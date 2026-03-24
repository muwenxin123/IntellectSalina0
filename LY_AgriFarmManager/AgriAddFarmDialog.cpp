#include <QTimer>

#include "QCString.h"
#include "AgriAddFarmDialog.h"
#include "ui_AgriAddFarmDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "LY_AgriFarmManager.h"
#include "LyMessageBox.h"
#include "AgriFarmManagerUtil.h"

AgriAddFarmDialog::AgriAddFarmDialog(QWidget *parent) :
	LyDialog(tr2("添加农场"), parent),
    ui(new Ui::AgriAddFarmDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	connect(ui->btnOK, &QPushButton::clicked, this, &AgriAddFarmDialog::on_btnOK_clicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &AgriAddFarmDialog::on_btnCancel_clicked);
	connect(ui->btnPick, &QPushButton::clicked, this, &AgriAddFarmDialog::on_btnPick_clicked);
}

AgriAddFarmDialog::~AgriAddFarmDialog()
{
	if (m_farmInfo)
	{
		delete m_farmInfo;
		m_farmInfo = nullptr;
	}

    delete ui;
}

void AgriAddFarmDialog::Init()
{

}

void AgriAddFarmDialog::updateTargetPoint(const QPointF & point)
{
	m_targetPoint = point;

	ui->farm_pos->setText(tr2("%1,%2")
		.arg(QString::number(m_targetPoint.x(), 'f', 7))
		.arg(QString::number(m_targetPoint.y(), 'f', 7)));

	this->show();

	// 恢复默认光标样式
	QApplication::restoreOverrideCursor();

	m_bPick = false;
}


AgriFarmInfo* AgriAddFarmDialog::GetDialogInfo()
{
	return m_farmInfo;
}

void AgriAddFarmDialog::closeEvent(QCloseEvent* event)
{
	// 恢复默认光标样式
	QApplication::restoreOverrideCursor();

	m_bPick = false;
}

void AgriAddFarmDialog::on_btnOK_clicked()
{
	if (ui->farm_name->text().isEmpty())
	{
		LyMessageBox::warning(this, "错误", "农场名称不能为空！");
		return;
	}

	m_farmInfo = new AgriFarmInfo;
	m_farmInfo->id = AgriFarmManagerUtil::GenerateUuid();
	m_farmInfo->name = ui->farm_name->text();
	m_farmInfo->category = ui->farm_category->text();
	m_farmInfo->description = ui->farm_desc->toPlainText();
	m_farmInfo->lon = m_targetPoint.x();
	m_farmInfo->lat = m_targetPoint.y();
	
	bool isSuccess = LY_AgriFarmManager::Get().GetpDbHandler()->AddFarmInfo(*m_farmInfo);
	if (!isSuccess)
	{
		LyMessageBox::warning(this, "错误", "存储数据库失败！");
	}
	else
	{
		emit addFinish();
	}
}

void AgriAddFarmDialog::on_btnCancel_clicked()
{
	reject();

	this->close();
}

void AgriAddFarmDialog::on_btnPick_clicked()
{
	// 设置全局的自定义光标样式
	QApplication::setOverrideCursor(Qt::CrossCursor);

	m_bPick = true;

	this->hide();
}
