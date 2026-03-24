#include "CDlgAgroTaskManage.h"
#include "ui_CDlgAgroTaskManage.h"
#include "QCString.h"
#include "IAgroTaskManagerActivator.h"
#include "QDateTime"
#include "QPushButton"
#include "LyMessageBox.h"
#include "QSqlQuery"
#include "CDlgAgroTaskManageEdit.h"

using namespace qnzkna::AgroTask;

CDlgAgroTaskManage::CDlgAgroTaskManage(QWidget *parent) :
	LyDialog(tr2("智慧农业任务管理"), parent),
	ui(new Ui::CDlgAgroTaskManage)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(900, 500);

	m_pIAgroTaskService = IAgroTaskManagerActivator::Get().getPIAgroTaskService();

	initTableData();

	connect(ui->pushButton, &QPushButton::clicked, this, &CDlgAgroTaskManage::on_pushButton_clicked);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &CDlgAgroTaskManage::on_pushButton_2_clicked);
	connect(ui->pushButton_5, &QPushButton::clicked, this, &CDlgAgroTaskManage::on_pushButton_5_clicked);
	connect(ui->pushButton_6, &QPushButton::clicked, this, &CDlgAgroTaskManage::on_pushButton_6_clicked);
	connect(ui->pushButton_4, &QPushButton::clicked, this, &CDlgAgroTaskManage::on_pushButton_4_clicked);
	connect(ui->pushButton_3, &QPushButton::clicked, this, &CDlgAgroTaskManage::on_pushButton_3_clicked);

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

CDlgAgroTaskManage::~CDlgAgroTaskManage()
{
    delete ui;
}

void CDlgAgroTaskManage::initTableData()
{
	ui->tableWidget->setRowCount(0);
	auto infoVec = m_pIAgroTaskService->getFarmlandAttributeInfoVec();
	int num = infoVec.size();
	ui->tableWidget->setRowCount(num);
	for (int i = 0; i < num; i++) {
		auto item0 = new QTableWidgetItem(QString::number(infoVec[i].nNo));
		item0->setTextAlignment(Qt::AlignCenter);

		auto item1 = new QTableWidgetItem(QString::fromLocal8Bit(infoVec[i].cName));
		item1->setTextAlignment(Qt::AlignCenter);

		auto item2 = new QTableWidgetItem(QString::fromLocal8Bit(infoVec[i].cSeedNo));
		item2->setTextAlignment(Qt::AlignCenter);

		QString item3Text{ "" };
		item3Text += m_pIAgroTaskService->getSeedStateRefer(infoVec[i].nPlanState);
		auto tempText = m_pIAgroTaskService->getSeedStateRefer(infoVec[i].nPlanState, infoVec[i].nPlanSubState);
		if (tempText != "") {
			item3Text += " - ";
			item3Text += tempText;
		}
		auto item3 = new QTableWidgetItem(item3Text);
		item3->setTextAlignment(Qt::AlignCenter);

		QDateTime startTime = QDateTime::fromSecsSinceEpoch(infoVec[i].llPlanDate);
		QString str1 = startTime.toString("yyyy/MM/dd hh:mm");
		auto item4 = new QTableWidgetItem(str1);
		item4->setToolTip(str1);
		item4->setTextAlignment(Qt::AlignCenter);

		auto item5 = new QTableWidgetItem(QString::fromLocal8Bit(infoVec[i].cEquipment));
		item5->setTextAlignment(Qt::AlignCenter);

		auto item6 = new QTableWidgetItem(QString::fromLocal8Bit(infoVec[i].remarks));
		item6->setTextAlignment(Qt::AlignCenter);

		ui->tableWidget->setItem(i, 0, item0);
		ui->tableWidget->setItem(i, 1, item1);
		ui->tableWidget->setItem(i, 2, item2);
		ui->tableWidget->setItem(i, 3, item3);
		ui->tableWidget->setItem(i, 4, item4);
		ui->tableWidget->setItem(i, 5, item5);
		ui->tableWidget->setItem(i, 6, item6);

	}
}

void CDlgAgroTaskManage::on_pushButton_clicked()
{
	QString curText = ui->lineEdit->text();
	int rowC = ui->tableWidget->rowCount();
	for (int i = 0; i < rowC; i++) {
		bool isHidden = true;
		for (int j = 0; j < m_tableRowCount; j++) {
			QString t_ij = ui->tableWidget->item(i, j)->text();
			if (t_ij.contains(curText)) {
				isHidden = false;
				break;
			}
		}
		ui->tableWidget->setRowHidden(i, isHidden);
	}
}

void CDlgAgroTaskManage::on_pushButton_2_clicked()
{
	int rowC = ui->tableWidget->rowCount();
	for (int i = 0; i < rowC; i++) {
		ui->tableWidget->setRowHidden(i, false);
	}
}


void CDlgAgroTaskManage::on_pushButton_4_clicked()
{
	auto wCDlgAgroTaskManageEdit = new CDlgAgroTaskManageEdit;
	wCDlgAgroTaskManageEdit->raise();
	wCDlgAgroTaskManageEdit->show();

	connect(wCDlgAgroTaskManageEdit, &CDlgAgroTaskManageEdit::signalFarmlandAttributeInfo, this, &CDlgAgroTaskManage::slot_add_info);
}

void CDlgAgroTaskManage::slot_add_info(FarmlandAttributeInfo info)
{
	bool success = m_pIAgroTaskService->addFarmlandAttributeInfo(info);
	if (success) {
		int rowC = ui->tableWidget->rowCount();
		ui->tableWidget->setRowCount(rowC + 1);

		auto item0 = new QTableWidgetItem(QString::number(info.nNo));
		item0->setTextAlignment(Qt::AlignCenter);

		auto item1 = new QTableWidgetItem(QString::fromLocal8Bit(info.cName));
		item1->setTextAlignment(Qt::AlignCenter);

		auto item2 = new QTableWidgetItem(QString::fromLocal8Bit(info.cSeedNo));
		item2->setTextAlignment(Qt::AlignCenter);

		QString item3Text{ "" };
		item3Text += m_pIAgroTaskService->getSeedStateRefer(info.nPlanState);
		auto tempText = m_pIAgroTaskService->getSeedStateRefer(info.nPlanState, info.nPlanSubState);
		if (tempText != "") {
			item3Text += " - ";
			item3Text += tempText;
		}
		auto item3 = new QTableWidgetItem(item3Text);
		item3->setTextAlignment(Qt::AlignCenter);

		QDateTime startTime = QDateTime::fromSecsSinceEpoch(info.llPlanDate);
		QString str1 = startTime.toString("yyyy/MM/dd hh:mm");
		auto item4 = new QTableWidgetItem(str1);
		item4->setToolTip(str1);
		item4->setTextAlignment(Qt::AlignCenter);

		auto item5 = new QTableWidgetItem(QString::fromLocal8Bit(info.cEquipment));
		item5->setTextAlignment(Qt::AlignCenter);

		auto item6 = new QTableWidgetItem(QString::fromLocal8Bit(info.remarks));
		item6->setTextAlignment(Qt::AlignCenter);

		ui->tableWidget->setItem(rowC, 0, item0);
		ui->tableWidget->setItem(rowC, 1, item1);
		ui->tableWidget->setItem(rowC, 2, item2);
		ui->tableWidget->setItem(rowC, 3, item3);
		ui->tableWidget->setItem(rowC, 4, item4);
		ui->tableWidget->setItem(rowC, 5, item5);
		ui->tableWidget->setItem(rowC, 6, item6);
	}
}

void CDlgAgroTaskManage::on_pushButton_5_clicked()
{
	m_pIAgroTaskService->loadDataFromDB_CFarmlandAttributeInfoVec();
	initTableData();
	update();
}

void CDlgAgroTaskManage::on_pushButton_6_clicked()
{
	auto itemList = ui->tableWidget->selectedItems();

	if (itemList.size() == 0) {
		LyMessageBox::warning(this, tr2("提示"), tr2("请选中表格中一行再进行删除！"));
		return;
	}

	int curRow = itemList[0]->row();

	bool success = m_pIAgroTaskService->deleteFarmlandAttributeInfo(ui->tableWidget->item(curRow, 0)->text().toInt());

	if (success) {
		ui->tableWidget->removeRow(curRow);
	}

}

void CDlgAgroTaskManage::on_pushButton_3_clicked()
{
	this->close();
}
