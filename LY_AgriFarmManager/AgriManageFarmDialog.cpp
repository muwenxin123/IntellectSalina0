#include <QApplication>
#include <QHeaderView>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QSettings>

#include "QCString.h"
#include "AgriManageFarmDialog.h"
#include "ui_AgriManageFarmDialog.h"
#include "DataManage/AgriFarm/AgriFarmManagerDecl.h"
#include "LY_AgriFarmManager.h"
#include "LyMessageBox.h"

AgriManageFarmDialog::AgriManageFarmDialog(const AgriFarmInfoVec& FarmList, QWidget *parent /*= nullptr*/)
	:LyDialog(tr2("管理盐田"), parent),
    ui(new Ui::AgriManageFarmDialog)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);

	Init(FarmList);

	connect(ui->btnOK, &QPushButton::clicked, this, &AgriManageFarmDialog::on_btnOK_clicked);
	connect(ui->btnCancel, &QPushButton::clicked, this, &AgriManageFarmDialog::on_btnCancel_clicked);
}

AgriManageFarmDialog::~AgriManageFarmDialog()
{
    delete ui;
}

void AgriManageFarmDialog::Init(const AgriFarmInfoVec& FarmList)
{
	m_tableWidget = new QTableWidget(this);
	m_tableWidget->setColumnCount(6); // 1列Radio + 4列数据
	m_tableWidget->setHorizontalHeaderLabels({ tr2("当前农场"),"ID", tr2("名称"), tr2("类别"), tr2("描述"), tr2("经纬度") });
	m_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	m_tableWidget->verticalHeader()->setVisible(false);

	m_buttonGroup = new QButtonGroup(this);
	m_buttonGroup->setExclusive(true); // 确保单选

	m_tableWidget->setRowCount(FarmList.size());

	for (int row = 0; row < FarmList.size(); ++row) {
		QRadioButton* radioButton = new QRadioButton();
		m_buttonGroup->addButton(radioButton, row);

		QWidget* radioWidget = new QWidget();
		QHBoxLayout* radioLayout = new QHBoxLayout(radioWidget);
		radioLayout->addWidget(radioButton);
		radioLayout->setAlignment(Qt::AlignCenter);
		radioLayout->setContentsMargins(0, 0, 0, 0);
		m_tableWidget->setCellWidget(row, 0, radioWidget);

		m_tableWidget->setItem(row, 1, new QTableWidgetItem(FarmList[row].id));
		m_tableWidget->setItem(row, 2, new QTableWidgetItem(FarmList[row].name));
		m_tableWidget->setItem(row, 3, new QTableWidgetItem(FarmList[row].category));
		m_tableWidget->setItem(row, 4, new QTableWidgetItem(FarmList[row].description));
		m_tableWidget->setItem(row, 5, new QTableWidgetItem(QString("%1, %2").arg(FarmList[row].lon).arg(FarmList[row].lat)));
	}

	// 删除已有布局
	//if (layout() != nullptr) {
	//	delete layout();
	//}

	QVBoxLayout* layout = ui->verticalLayout;
	layout->addWidget(m_tableWidget);
	setLayout(layout);

	// 禁用表格的编辑功能
	m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

	connect(m_buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &AgriManageFarmDialog::onRadioButtonClicked);

	initSelectedRadioButton();
}

void AgriManageFarmDialog::closeEvent(QCloseEvent* event)
{

}

void AgriManageFarmDialog::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);

	QAction* action = new QAction(tr2("添加农场"));
	menu.addAction(action);
	connect(action, &QAction::triggered, this, [&]() {
		emit addFarm();
	});

	QAction* viewDetailsAction = new QAction(tr2("查看详情"), this);
	QAction* deleteAction = new QAction(tr2("删除"), this);

	// 绑定菜单项的动作
	connect(viewDetailsAction, &QAction::triggered, this, &AgriManageFarmDialog::viewDetails);
	connect(deleteAction, &QAction::triggered, this, &AgriManageFarmDialog::deleteRow);

	menu.addAction(viewDetailsAction);
	menu.addAction(deleteAction);

	menu.exec(event->globalPos());
}


void AgriManageFarmDialog::initSelectedRadioButton()
{
	QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
	ini.beginGroup("Agri");
	QString farmId = ini.value("FarmID", "").toString();
	if (!farmId.isEmpty())
	{
		// 遍历所有行，找到匹配的ID
		for (int row = 0; row < m_tableWidget->rowCount(); ++row) {
			QString rowId = m_tableWidget->item(row, 1)->text();
			if (rowId == farmId) {
				// 选中该行的RadioButton
				QRadioButton* radioButton = qobject_cast<QRadioButton*>(m_tableWidget->cellWidget(row, 0)->layout()->itemAt(0)->widget());
				if (radioButton) {
					radioButton->setChecked(true);
				}
				break;
			}
		}
	}
}

void AgriManageFarmDialog::on_btnOK_clicked()
{
	if (m_nChoiceRow != -1)
	{
		// 获取该行的详细信息
		QString id = m_tableWidget->item(m_nChoiceRow, 1)->text();
		QString name = m_tableWidget->item(m_nChoiceRow, 2)->text();

		if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定将农场切换为[%1]？").arg(name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			QSettings ini("../data/conf/ly.ini", QSettings::IniFormat);
			ini.beginGroup("Agri");
			ini.setValue("FarmID", id);

			// 农场切换
			emit switchFarm(id);
		}
	}
}

void AgriManageFarmDialog::on_btnCancel_clicked()
{
	reject();

	this->close();
}

void AgriManageFarmDialog::viewDetails()
{
	int row = m_tableWidget->currentRow();
	if (row >= 0) {
		QString id = m_tableWidget->item(row, 1)->text();
		QString name = m_tableWidget->item(row, 2)->text();
		QString category = m_tableWidget->item(row, 3)->text();
		QString description = m_tableWidget->item(row, 4)->text();
		QString location = m_tableWidget->item(row, 5)->text();

		QMessageBox::information(this, tr2("农场详情"),
			tr2("ID: %1\n名称: %2\n类别: %3\n描述: %4\n经纬度: %5")
			.arg(name).arg(category).arg(description).arg(location));
	}
}

void AgriManageFarmDialog::deleteRow()
{
	// 获取当前选中的行
	int row = m_tableWidget->currentRow();
	if (row >= 0) {
		// 获取该行的详细信息（比如获取该行的农场ID、名称等）
		QString id = m_tableWidget->item(row, 1)->text();
		QString name = m_tableWidget->item(row, 2)->text();
		QString category = m_tableWidget->item(row, 3)->text();
		QString description = m_tableWidget->item(row, 4)->text();
		QString location = m_tableWidget->item(row, 5)->text();

		if (LyMessageBox::question(nullptr, tr2("提示"), tr2("是否确定删除农场[%1]？").arg(name), QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
		{
			if (LY_AgriFarmManager::Get().GetpDbHandler()->DeleteFarmInfo(id))
			{
				m_tableWidget->removeRow(row); // 删除该行
			}
		}
	}
	else {
		// 如果没有选中任何行，显示提示
		LyMessageBox::warning(this, tr2("警告"), tr2("请先选择一行进行删除"));
	}
}

void AgriManageFarmDialog::onRadioButtonClicked(int id)
{
	// 获取点击的行
	m_nChoiceRow = id;
}
