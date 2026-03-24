#include "CDlgTaskSchedule.h"
#include "ui_CDlgTaskSchedule.h"
#include "QCString.h"
#include "QPainter"
#include "IAgroTaskManagerActivator.h"
#include "LY_AgroTaskManage.h"

using namespace qnzkna::AgroTask;

CDlgTaskSchedule::CDlgTaskSchedule(QWidget *parent) :
	LyDialog(tr2("任务进度"), parent),
	ui(new Ui::CDlgTaskSchedule)
{
	ui->setupUi(centralWidget());
	//this->setResizable(true);
	this->resize(1440, 900);

	m_pIAgroTaskService = IAgroTaskManagerActivator::Get().getPIAgroTaskService();

	setMaxable(true);

	InitCtrl();

	ui->checkBox->setChecked(true);
	connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
	connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_2_currentIndexChanged(int)));
	connect(ui->checkBox, SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_stateChanged(int)));
	connect(ui->checkBox_2, SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_2_stateChanged(int)));
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
	connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_pushButton_5_clicked()));
	connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));
	connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));

	//ui->pushButton->hide();
	ui->checkBox_2->hide();
}

CDlgTaskSchedule::~CDlgTaskSchedule()
{
    delete ui;
}

void CDlgTaskSchedule::InitCtrl()
{
	if (m_pIAgroTaskService == nullptr)
	{
		return;
	}

	ui->comboBox_2->clear();
	QStringList strShowTypeList;
	strShowTypeList.push_back(tr2("播种时间 为时间起点(T0)"));
	strShowTypeList.push_back(tr2("年/月/日 格式"));
	ui->comboBox_2->addItems(strShowTypeList);

}

void CDlgTaskSchedule::UpdateNongTianCombobox()
{
	ui->comboBox->clear();

	m_FarmInfoVec.clear();
	m_FarmInfoVec = m_pIAgroTaskService->getFarmlandAttributeInfoVec();
	if (m_FarmInfoVec.size() == 0)
	{
		ui->comboBox->addItem(tr2("-"));
	}
	else
	{
		int nNo = 0;
		for (auto itr : m_FarmInfoVec)
		{
			QString strTmp = QString::number(itr.nNo) + tr(":") + QString::fromLocal8Bit(itr.cName);
			ui->comboBox->insertItem(nNo, strTmp);
			ui->comboBox->setItemData(nNo,QString::number(itr.nNo));
			nNo++;
		}
	}
}

void CDlgTaskSchedule::UpdateDrawFrame(const int& nComboboxCurrentIndex)
{
	if (ui->DrawWidget == nullptr)
	{
		return;
	}

	int nNongtianID = ui->comboBox->itemData(nComboboxCurrentIndex).toInt();
	FarmlandAttributeInfo  stFarmInfo = m_pIAgroTaskService->getFarmlandAttributeInfo(nNongtianID);
	QString strSeedID = QString::fromLocal8Bit(stFarmInfo.cSeedNo);
	CAgroRefDataInfoVec stInfoVec = m_pIAgroTaskService->getAgroRefDataInfo(strSeedID);
	for (auto& fieldInfo : m_vecFieldList)
	{
		// 匹配激活农场
		if (QString::fromLocal8Bit(stFarmInfo.cName) == fieldInfo.area_id)
		{
			m_FieldInfo = fieldInfo;
		}
	}
	ui->DrawWidget->SetFarmlandAttributeInfo(stFarmInfo);
	ui->DrawWidget->SetAgroRefDataInfoVec(stInfoVec);
	ui->DrawWidget->SetAgriFieldInfo(m_FieldInfo);
	ui->DrawWidget->ArrangeData();
	ui->DrawWidget->update();
}

void CDlgTaskSchedule::on_comboBox_currentIndexChanged(const int &arg1)
{
	if (arg1 == -1)
	{
		return;
	}
	UpdateDrawFrame(arg1);
}

void CDlgTaskSchedule::on_comboBox_2_currentIndexChanged(const int &arg1)
{
	if (ui->DrawWidget != nullptr)
	{
		ui->DrawWidget->SetTimeShowType(arg1);
	}
}

void CDlgTaskSchedule::on_checkBox_2_stateChanged(int arg1)
{
	ui->DrawWidget->SetShowMouseTipsFlag(arg1);
}

void CDlgTaskSchedule::on_checkBox_stateChanged(int arg1)
{
	ui->DrawWidget->SetShowDetailFlag(arg1);
	ui->DrawWidget->update();
}

void CDlgTaskSchedule::on_pushButton_clicked()
{
	ui->DrawWidget->SetScale(0);
	ui->DrawWidget->update();
}

void CDlgTaskSchedule::on_pushButton_2_clicked()
{
	this->hide();
}

void CDlgTaskSchedule::on_pushButton_3_clicked()
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

void CDlgTaskSchedule::on_pushButton_4_clicked()
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

void CDlgTaskSchedule::on_pushButton_5_clicked()
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

void CDlgTaskSchedule::showEvent(QShowEvent *event)
{
	ui->comboBox->disconnect();

	//更新盐田信息
	m_vecFieldList.clear();
	IAgroTaskManagerActivator::Get().GetPIAgriFarmManagerService()->GetFieldInfoList(m_vecFieldList);

	UpdateNongTianCombobox();
	connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));

	if (ui->comboBox->currentIndex() == -1)
	{
		return;
	}

	UpdateDrawFrame(ui->comboBox->currentIndex());

	if (ui->DrawWidget != nullptr)
	{
		ui->DrawWidget->SetShowDetailFlag(ui->checkBox->isChecked());
		ui->DrawWidget->SetShowMouseTipsFlag(ui->checkBox_2->isChecked());
		if (ui->comboBox_2->currentIndex() != -1)
		{
			ui->DrawWidget->SetTimeShowType(ui->comboBox_2->currentIndex());
		}
		ui->DrawWidget->CalaDrawData();
	}
}
