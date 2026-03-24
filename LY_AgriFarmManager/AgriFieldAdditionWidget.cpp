#include <QTabWidget>
#include <QTabBar>

#include "QCString.h"
#include "LyMessageBox.h"

#include "AgriFieldAdditionWidget.h"
#include "ui_AgriFieldAdditionWidget.h"
#include "LY_AgriFarmManager.h"

AgriFieldAdditionWidget::AgriFieldAdditionWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::AgriFieldAdditionWidget)
{
	ui->setupUi(this);

	QObject::connect(ui->tabWidget->tabBar(), &QTabBar::tabBarClicked, this, &AgriFieldAdditionWidget::onTabBarClicked);
}

AgriFieldAdditionWidget::~AgriFieldAdditionWidget()
{
	delete ui;
}

void AgriFieldAdditionWidget::setCurrentField(const AgriFieldInfoPtr& fieldInfo)
{
	m_pFieldInfo = fieldInfo;

	QWidget *clickedTabWidget = ui->tabWidget->widget(0);
	if (clickedTabWidget)
	{
		handleSensorTabClick(clickedTabWidget);
	}
}

void AgriFieldAdditionWidget::createSensorTableHeader()
{
	// 创建传感器展示表
	if (!m_tableWidget)
	{
		m_tableWidget = new QTableWidget;
	}

	m_tableWidget->clear();
	m_tableWidget->setColumnCount(6); // Set the number of columns
	m_tableWidget->setHorizontalHeaderLabels({ tr2("名称"), tr2("ID"), tr2("类型"), tr2("型号"), tr2("高度"), tr2("位置") });

	// 禁用表格的编辑功能
	m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void AgriFieldAdditionWidget::onTabBarClicked(int index)
{
	QWidget *clickedTabWidget = ui->tabWidget->widget(index);
	if (!clickedTabWidget)return;

	switch (index)
	{
	case  0:
		handleSensorTabClick(clickedTabWidget);
		break;
	case  1:

		break;
	default:
		break;
	}
}

void AgriFieldAdditionWidget::handleSensorTabClick(QWidget* tabWidget)
{
	if (m_pFieldInfo)
	{
		auto monitorVec = LY_AgriFarmManager::Get().GetpDbHandler()->GetRelateSensorList(m_pFieldInfo->id);
		
		createSensorTableHeader();
		
		m_tableWidget->setRowCount(monitorVec.size());

		for (int i = 0; i < monitorVec.size(); ++i) {
			const auto& monitor = monitorVec[i];

			m_tableWidget->setItem(i, 0, new QTableWidgetItem(monitor->mointName));
			m_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(monitor->nID)));
			m_tableWidget->setItem(i, 2, new QTableWidgetItem(monitor->mointType));
			m_tableWidget->setItem(i, 3, new QTableWidgetItem(monitor->mointModenlName));
			m_tableWidget->setItem(i, 4, new QTableWidgetItem(QString::number(monitor->localHeight)));
			m_tableWidget->setItem(i, 5, new QTableWidgetItem(QString("(%1, %2)")
				.arg(QString::number(monitor->longitude, 'f', 7)).arg(QString::number(monitor->latitude, 'f', 7))));
		}

		// 删除已有布局
		//if (layout() != nullptr) {
		//	delete layout();  
		//}

		QVBoxLayout *layout = new QVBoxLayout(tabWidget);
		layout->addWidget(m_tableWidget);
		setLayout(layout);

		// 设置单元格内容后，调整列宽
		//m_tableWidget->resizeColumnsToContents();

		// 为地理位置列设置宽度为 300
		m_tableWidget->setColumnWidth(5, 150);
	}
}

void AgriFieldAdditionWidget::handleObstacleTabClick(QWidget* tabWidget)
{

}

void AgriFieldAdditionWidget::cleanUp()
{
	setCurrentField(std::make_shared<AgriFieldInfo>());

	m_tableWidget->clear();

	createSensorTableHeader();
}

void AgriFieldAdditionWidget::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
}

void AgriFieldAdditionWidget::showEvent(QShowEvent *event)
{

}

void AgriFieldAdditionWidget::hideEvent(QHideEvent *event)
{

}