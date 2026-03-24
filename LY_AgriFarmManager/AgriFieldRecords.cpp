#include <QTabWidget>
#include <QTabBar>

#include "QCString.h"
#include "LyMessageBox.h"

#include "AgriFieldRecords.h"
#include "ui_AgriFieldRecords.h"

AgriFieldRecords::AgriFieldRecords(QWidget *parent)
	: QWidget(parent)
    , ui(new Ui::AgriFieldRecords)
{
	ui->setupUi(this);

	QObject::connect(ui->tabWidget->tabBar(), &QTabBar::tabBarClicked, this, &AgriFieldRecords::onTabBarClicked);

	Init();
}

AgriFieldRecords::~AgriFieldRecords()
{
    delete ui;
}

void AgriFieldRecords::Init()
{
	ui->tableWidget_PersonManage->clear();
	

	QString InfoData[2][3] = { {tr2("李"),tr2("组长"),tr2("是")},{tr2("王"),tr2("员工"),tr2("是")} };

	QStringList header;
	header << tr2("序号") << tr2("名字") << tr2("职位") << tr2("是否在岗");
	ui->tableWidget_PersonManage->setRowCount(2);
	ui->tableWidget_PersonManage->setColumnCount(header.size());
	ui->tableWidget_PersonManage->setHorizontalHeaderLabels(header);
	ui->tableWidget_PersonManage->horizontalHeader()->setStretchLastSection(true);
	ui->tableWidget_PersonManage->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget_PersonManage->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget_PersonManage->verticalHeader()->setVisible(false);
	ui->tableWidget_PersonManage->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
	ui->tableWidget_PersonManage->setSelectionBehavior(QAbstractItemView::SelectRows);
	for (int i = 0; i < 2; i++)
	{
		ui->tableWidget_PersonManage->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
		ui->tableWidget_PersonManage->setItem(i, 1, new QTableWidgetItem(InfoData[i][0]));
		ui->tableWidget_PersonManage->setItem(i, 2, new QTableWidgetItem(InfoData[i][1]));
		ui->tableWidget_PersonManage->setItem(i, 3, new QTableWidgetItem(InfoData[i][2]));
	}

}

void AgriFieldRecords::onTabBarClicked(int index)
{
	QWidget *clickedTabWidget = ui->tabWidget->widget(index);
	if (!clickedTabWidget)return;
}