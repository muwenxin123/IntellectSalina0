#include "QCString.h"
#include <QDateTime>

#include "AgriMachineInfoTable.h"
#include "ui_AgriMachineInfoTable.h"

AgriMachineInfoTable::AgriMachineInfoTable(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgriMachineInfoTable)
{
	ui->setupUi(this);

	InitTable();
}

AgriMachineInfoTable::~AgriMachineInfoTable()
{
	delete ui;
}

void AgriMachineInfoTable::InitTable()
{
	ui->tableWidget->setColumnCount(8);
	ui->tableWidget->setHorizontalHeaderLabels({
		tr2("序号"),
		tr2("农机"),
		tr2("数量"),
		tr2("动力机械"),
		tr2("作业机械"),
		tr2("装配状态"),
		tr2("创建时间"),
		tr2("修改时间")
	});
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->tableWidget->verticalHeader()->setVisible(false);

	// 禁用表格的编辑功能
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	//resizeColumnsToContents();
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第0列固定
	ui->tableWidget->setColumnWidth(0, 30);
	ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
	ui->tableWidget->setColumnWidth(2, 50);
}

void AgriMachineInfoTable::UpdateTableData(AgriMachineInfoPtrVec machineList)
{
	ui->tableWidget->clearContents();
	ui->tableWidget->setRowCount(0);

	for (int row = 0; row < machineList.size(); ++row)
	{
		AddOneRowTotable(machineList[row]);
	}

	// 居中显示
	int rows = ui->tableWidget->rowCount();
	int cols = ui->tableWidget->columnCount();
	for (int r = 0; r < rows; ++r) {
		for (int c = 0; c < cols; ++c) {
			if (QTableWidgetItem* item = ui->tableWidget->item(r, c)) {
				item->setTextAlignment(Qt::AlignCenter);
			}
		}
	}
}

void AgriMachineInfoTable::AddOneRowTotable(AgriMachineInfoPtr machineInfo)
{
	int rowCount = ui->tableWidget->rowCount();
	ui->tableWidget->insertRow(rowCount);

	auto userDefItem = new QTableWidgetItem(QString::number(rowCount + 1));
	userDefItem->setTextAlignment(Qt::AlignCenter);
	userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(machineInfo.get())));
	ui->tableWidget->setItem(rowCount, 0, userDefItem);
	ui->tableWidget->setItem(rowCount, 1, new QTableWidgetItem(QString::fromStdString(machineInfo->name)));
	ui->tableWidget->setItem(rowCount, 2, new QTableWidgetItem(QString::number(machineInfo->count)));
	ui->tableWidget->setItem(rowCount, 3, new QTableWidgetItem(QString::fromStdString(machineInfo->power_machine ? machineInfo->power_machine->name : "")));
	ui->tableWidget->setItem(rowCount, 4, new QTableWidgetItem(QString::fromStdString(machineInfo->work_machine ? machineInfo->work_machine->name : "")));
	ui->tableWidget->setItem(rowCount, 5, new QTableWidgetItem(machineInfo->assembled ? u8"已装配" : u8"未装配"));
	ui->tableWidget->setItem(rowCount, 6, new QTableWidgetItem(QDateTime::fromString(QString::fromStdString(machineInfo->create_time), Qt::ISODateWithMs).toString("yyyy-MM-dd HH:mm:ss")));
	ui->tableWidget->setItem(rowCount, 7, new QTableWidgetItem(QDateTime::fromString(QString::fromStdString(machineInfo->modify_time), Qt::ISODateWithMs).toString("yyyy-MM-dd HH:mm:ss")));
} 


//void AgriSoilTestingAnalysisWidget::UpdateOneRowTotable(AgriMachineInfoPtr machineInfo)
//{
//	int rowCount = ui->tableWidget->rowCount();
//	for (int row = 0; row < rowCount; ++row) {
//		if (QTableWidgetItem *item = ui->tableWidget->item(row, 0)) {
//			AgriSoilTestingDataset dataset = item->data(Qt::UserRole).value<AgriSoilTestingDataset>();
//			if (dataset.guid == soilTestingDataset->guid) {
//
//				auto userDefItem = new QTableWidgetItem(QString::number(row + 1));
//				userDefItem->setTextAlignment(Qt::AlignCenter);
//				userDefItem->setData(Qt::UserRole, QVariant::fromValue(*(soilTestingDataset.get())));
//				ui->tableWidget->setItem(row, 0, userDefItem);
//				ui->tableWidget->setItem(row, 2, new QTableWidgetItem(m_pFieldInfo->name));
//				ui->tableWidget->setItem(row, 3, new QTableWidgetItem(soilTestingDataset->name));
//				ui->tableWidget->setItem(row, 4, new QTableWidgetItem(AgriSoilAnalysisPredictUtil::SampleTypeToString(static_cast<SampleType>(soilTestingDataset->sample_type))));
//				ui->tableWidget->setItem(row, 5, new QTableWidgetItem(AgriSoilAnalysisPredictUtil::SampleModeToString(static_cast<SampleType>(soilTestingDataset->sample_type), soilTestingDataset->sample_mode)));
//				ui->tableWidget->setItem(row, 6, new QTableWidgetItem(soilTestingDataset->create_time));
//				ui->tableWidget->setItem(row, 7, new QTableWidgetItem(soilTestingDataset->update_time));
//				ui->tableWidget->setItem(row, 8, new QTableWidgetItem(QString::number(soilTestingDataset->grid_size)));
//				ui->tableWidget->setItem(row, 9, new QTableWidgetItem(QString::number(soilTestingDataset->grid_rotation)));
//			}
//		}
//	}
//}