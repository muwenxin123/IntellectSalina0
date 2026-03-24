#include "CDlgAgroTaskManageEdit.h"
#include "ui_CDlgAgroTaskManageEdit.h"
#include "QCString.h"
#include "QDateTime"
#include "IAgroTaskManagerActivator.h"
#include <algorithm>
#include "QTableWidget"

using namespace qnzkna::AgroTask;

CDlgAgroTaskManageEdit::CDlgAgroTaskManageEdit(QWidget *parent) :
	LyDialog(tr2("智慧农业任务创建"), parent),
    ui(new Ui::CDlgAgroTaskManageEdit)
{
    ui->setupUi(centralWidget());

	m_pIAgroTaskService = IAgroTaskManagerActivator::Get().getPIAgroTaskService();
	m_pIZoneManageService = IAgroTaskManagerActivator::Get().getIZoneManageService();

	initUI();

	connect(ui->pushButton, &QPushButton::clicked, this, &CDlgAgroTaskManageEdit::on_pushButton_clicked);
	connect(ui->tableWidget, &QTableWidget::itemDoubleClicked, this, &CDlgAgroTaskManageEdit::on_tablewidget_doubleclicked);
	connect(ui->tableWidget_2, &QTableWidget::itemDoubleClicked, this, &CDlgAgroTaskManageEdit::on_tablewidget_2_doubleclicked);
}

CDlgAgroTaskManageEdit::~CDlgAgroTaskManageEdit()
{
    delete ui;
}

void CDlgAgroTaskManageEdit::on_pushButton_clicked()
{
	FarmlandAttributeInfo info;
	auto infoVec = m_pIAgroTaskService->getFarmlandAttributeInfoVec();
	int maxNo = 0;
	for (const auto &info : infoVec) {
		maxNo = std::max(maxNo, info.nNo);
	}
	info.nNo = ++maxNo;

	QString nongtian = ui->comboBox->currentText();
	strcpy(info.cName, nongtian.toLocal8Bit().data());
	QString zhongzi = ui->comboBox_2->currentText();
	strcpy(info.cSeedNo, zhongzi.toLocal8Bit().data());

	QDateTime starTime = QDateTime::fromString(ui->dateTimeEdit->text(), "yyyy/MM/dd hh:mm");
	info.llPlanDate = starTime.toSecsSinceEpoch();
	QString remarks = ui->textEdit->toPlainText();
	strcpy(info.remarks, remarks.toLocal8Bit().data());

	int rowC = ui->tableWidget->rowCount();
	//获取左侧表格信息拼接成一个字符串
	for (int i = 0;i < rowC;i++) {

	}

	emit signalFarmlandAttributeInfo(info);

	this->close();
}

void CDlgAgroTaskManageEdit::on_tablewidget_doubleclicked()
{
	int curR = ui->tableWidget->currentRow();
	ui->tableWidget->removeRow(curR);
	int rowC = ui->tableWidget->rowCount();
	for (int i = curR;i < rowC;i++) {
		ui->tableWidget->item(i, 0)->setText(QString::number(i + 1));
	}
}

void CDlgAgroTaskManageEdit::on_tablewidget_2_doubleclicked()
{
	int curR = ui->tableWidget_2->currentRow();
	int rowC = ui->tableWidget->rowCount();
	ui->tableWidget->setRowCount(rowC + 1);

	QTableWidgetItem* item0 = new QTableWidgetItem(QString::number(rowC + 1));
	item0->setTextAlignment(Qt::AlignCenter);
	ui->tableWidget->setItem(rowC, 0, item0);

	auto text1 = ui->tableWidget_2->item(curR, 1)->text();
	QTableWidgetItem* item1 = new QTableWidgetItem(text1);
	item1->setTextAlignment(Qt::AlignCenter);
	ui->tableWidget->setItem(rowC, 1, item1);

	auto text2 = ui->tableWidget_2->item(curR, 2)->text();
	QTableWidgetItem* item2 = new QTableWidgetItem(text2);
	item2->setTextAlignment(Qt::AlignCenter);
	ui->tableWidget->setItem(rowC, 2, item2);
}

void CDlgAgroTaskManageEdit::initUI()
{
	auto allZoneVec = m_pIZoneManageService->GetAllZoneBase();
	for (const auto &zone : allZoneVec) {
		auto name = QString::fromLocal8Bit(zone.strID);
		if (name.contains(tr2("盐田"))) {
			ui->comboBox->addItem(name);
		}
	}

	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->setCornerButtonEnabled(false);
	ui->tableWidget->setFocusPolicy(Qt::NoFocus);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	ui->tableWidget_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget_2->setCornerButtonEnabled(false);
	ui->tableWidget_2->setFocusPolicy(Qt::NoFocus);
	ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);

	//初始化设备列表

}
