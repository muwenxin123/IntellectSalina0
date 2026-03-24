#include "CDlgTaskEventRecord.h"
#include "ui_CDlgTaskEventRecord.h"
#include "QCString.h"
#include "IAgroTaskManagerActivator.h"
#include "QDateTime"
#include "Zone\AgroTaskInfo.h"

CDlgTaskEventRecord::CDlgTaskEventRecord(QWidget *parent) :
	LyDialog(tr2("智慧农业事件记录"), parent),
	ui(new Ui::CDlgTaskEventRecord)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(900, 500);

	m_pIAgroTaskService = IAgroTaskManagerActivator::Get().getPIAgroTaskService();

	initTableData();

	connect(ui->pushButton, &QPushButton::clicked, this, &CDlgTaskEventRecord::on_pushButton_clicked);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &CDlgTaskEventRecord::on_pushButton_2_clicked);
	connect(ui->pushButton_3, &QPushButton::clicked, this, &CDlgTaskEventRecord::on_pushButton_3_clicked);

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

CDlgTaskEventRecord::~CDlgTaskEventRecord()
{
    delete ui;
}

void CDlgTaskEventRecord::initTableData()
{
	auto cmp = [](qnzkna::AgroTask::FarmEvevtInfo a1, qnzkna::AgroTask::FarmEvevtInfo a2) {
		if (a1.nOperDate > a2.nOperDate) return true;
		return false;
	};

	ui->tableWidget->setRowCount(0);
	auto infoVec = m_pIAgroTaskService->getFarmEvevtInfoVec();
	std::sort(infoVec.begin(), infoVec.end(), cmp);

	int num = infoVec.size();
	ui->tableWidget->setRowCount(num);
	for (int i = 0; i < num; i++) {
		auto info = m_pIAgroTaskService->getFarmlandAttributeInfo(infoVec[i].nNo);
		auto item0 = new QTableWidgetItem(QString::fromLocal8Bit(info.cName));
		item0->setTextAlignment(Qt::AlignCenter);

		QDateTime startTime = QDateTime::fromSecsSinceEpoch(infoVec[i].nOperDate);
		QString str1 = startTime.toString("yyyy/MM/dd hh:mm:ss");
		auto item1 = new QTableWidgetItem(str1);
		item1->setToolTip(str1);
		item1->setTextAlignment(Qt::AlignCenter);

		auto item2 = new QTableWidgetItem(QString::fromLocal8Bit(infoVec[i].cOperContent));
		item2->setTextAlignment(Qt::AlignCenter);

		auto item3 = new QTableWidgetItem(QString::fromLocal8Bit(infoVec[i].cRemarks));
		item3->setTextAlignment(Qt::AlignCenter);

		ui->tableWidget->setItem(i, 0, item0);
		ui->tableWidget->setItem(i, 1, item1);
		ui->tableWidget->setItem(i, 2, item2);
		ui->tableWidget->setItem(i, 3, item3);

	}
}

void CDlgTaskEventRecord::on_pushButton_clicked()
{
	QString curText = ui->lineEdit->text();
	int rowC = ui->tableWidget->rowCount();
	for (int i = 0; i < rowC; i++) {
		bool isHidden = true;
		for (int j = 0; j < 4; j++) {
			QString t_ij = ui->tableWidget->item(i, j)->text();
			if (t_ij.contains(curText)) {
				isHidden = false;
				break;
			}
		}
		ui->tableWidget->setRowHidden(i, isHidden);
	}
}

void CDlgTaskEventRecord::on_pushButton_2_clicked()
{
	int rowC = ui->tableWidget->rowCount();
	for (int i = 0; i < rowC; i++) {
		ui->tableWidget->setRowHidden(i, false);
	}

	ui->lineEdit->setText("");
}

void CDlgTaskEventRecord::on_pushButton_3_clicked()
{
	this->close();
}
