#include "CDlgZoneFileManage.h"
#include "ui_CDlgZoneFileManage.h"
#include "QCString.h"
#include "IZoneManagerActivator.h"
#include "LyMessageBox.h"
#include "LY_ZoneManager.h"

CDlgZoneFileManage::CDlgZoneFileManage(QWidget *parent) :
	LyDialog(tr2("区域方案管理"), parent),
	ui(new Ui::CDlgZoneFileManage)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(500, 400);

	m_pIZoneManageService = IZoneManagerActivator::Get().getPIZoneManageService();

	CreateListHeader();

	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
	connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
}

CDlgZoneFileManage::~CDlgZoneFileManage()
{
    delete ui;
}

void CDlgZoneFileManage::CreateListHeader()
{
	QStringList headerlabels;
	headerlabels << tr2("序号") << tr2("文件名称");
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
		else
		{
			ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
		}
	}
}

void CDlgZoneFileManage::UpdatelistData()
{
	ui->tableWidget->clearContents();

	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	QStringList strList = m_pIZoneManageService->GetZoneFileNames();

	ui->tableWidget->setRowCount(strList.size());
	ui->tableWidget->setSortingEnabled(false);

	int nNo = 0;
	QString strNo, strName;
	for (auto itr:strList)
	{
		strNo = QString::number(nNo).sprintf("%02d", nNo + 1);
		strName = itr;
		QTableWidgetItem* item0 = new QTableWidgetItem(strNo);
		QTableWidgetItem* item1 = new QTableWidgetItem(strName);

		ui->tableWidget->setItem(nNo, 0, item0);
		ui->tableWidget->setItem(nNo, 1, item1);

		ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		nNo++;
	}
}

void CDlgZoneFileManage::showEvent(QShowEvent *event)
{
	UpdatelistData();
}

void CDlgZoneFileManage::on_pushButton_2_clicked()
{
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	int nCurrentRow = ui->tableWidget->currentRow();
	if (nCurrentRow == -1)
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要删除的文件。"));
		return;
	}

	QString strName = ui->tableWidget->item(nCurrentRow, 1)->text();

	qnzkna::ZoneManage::CZoneBaseVec FileZoneVec;
	bool success = m_pIZoneManageService->OpenZoneFile(strName, FileZoneVec);
	if (success == false || FileZoneVec.size() == 0)
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("导入失败。"));
		return;
	}

	if (ui->checkBox->isChecked() == true)
	{
		m_pIZoneManageService->RemoveZoneBase();
	}

	if (ui->checkBox_2->isChecked() == true)
	{
		for (auto itr :FileZoneVec)
		{
			QString strID = QString::fromLocal8Bit(itr.strID);
			if (m_pIZoneManageService->IsZoneBaseIDExisted(strID))
			{
				m_pIZoneManageService->EditZoneBase(itr);
			}
			else
			{
				m_pIZoneManageService->AddZoneBase(itr);
			}
		}
	}
	else
	{
		for (auto itr : FileZoneVec)
		{
			QString strID = QString::fromLocal8Bit(itr.strID);
			if (!m_pIZoneManageService->IsZoneBaseIDExisted(strID))
			{
				m_pIZoneManageService->AddZoneBase(itr);
			}
		}
	}

	LyMessageBox::information(NULL, tr2("提示"), tr2("导入成功。"));
	UpdatelistData();
	if (LY_ZoneManager::Get().GetPZoneManageWidget()!=nullptr)
	{
		LY_ZoneManager::Get().GetPZoneManageWidget()->UpdateWindowData();
		LY_ZoneManager::Get().GetPZoneManageWidget()->UpdateTreeData();
	}
}

void CDlgZoneFileManage::on_pushButton_clicked()
{
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	QString strZoneFilePath = "../data/ZoneFile/";
	QString fileName = QFileDialog::getSaveFileName(this, tr("file"),strZoneFilePath,tr("XML Files (*.xml)"));
	bool success =  m_pIZoneManageService->SaveZoneFile(fileName);

	if (success == true)
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("保存成功。"));
		UpdatelistData();
	}
	else
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("保存失败。"));
	}
}

void CDlgZoneFileManage::on_pushButton_3_clicked()
{
	if (m_pIZoneManageService == nullptr)
	{
		return;
	}

	int nCurrentRow = ui->tableWidget->currentRow();
	if (nCurrentRow == -1)
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("请选择需要删除的文件。"));
		return;
	}

	QString strName = ui->tableWidget->item(nCurrentRow, 1)->text();

	bool success = m_pIZoneManageService->RemoveZoneFile(strName);

	if (success == true)
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("删除成功。"));
		UpdatelistData();
	}
	else
	{
		LyMessageBox::information(NULL, tr2("提示"), tr2("删除失败。"));
	}
}
