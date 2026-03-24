#include <QDebug>
#include <QLabel>
#include <QHeaderView>

#include "LyMessageBox.h"
#include "AgriSatelliteDataAnalysis.h"
#include "ui_AgriSatelliteDataAnalysis.h"
#include "LY_AgriMultispectralSensor.h"
#include "AgriMultispectralSensorUtil.h"


AgriSatelliteDataAnalysis::AgriSatelliteDataAnalysis(QWidget *parent /*= nullptr*/):
	QWidget(parent),
	ui(new Ui::AgriSatelliteDataAnalysis)
{ 
	ui->setupUi(this);

	Init();
}

AgriSatelliteDataAnalysis::~AgriSatelliteDataAnalysis()
{
	delete ui;
}

void AgriSatelliteDataAnalysis::UpdateShowLegend(std::vector<AgriSatelliteDataTree::ImageInfo>& imageVec)
{
	auto cleanLayout = [&](QGroupBox * groupBox) {
		if (groupBox) {
			QLayout *oldLayout = groupBox->layout();
			if (oldLayout) {
				QLayoutItem *item;
				while ((item = oldLayout->takeAt(0)) != nullptr) {
					if (item->widget()) {
						delete item->widget();
					}
					delete item;
				}
				delete oldLayout;
			}
		}
	};

	cleanLayout(ui->groupBox);
	cleanLayout(ui->groupBox_2);

	QVBoxLayout * topLayout = new QVBoxLayout();
	topLayout->setContentsMargins(0, 20, 0, 0); // 左、上、右、下

	bool isShow = false;
	for (const auto &imageInfo : imageVec)
	{
		if (imageInfo.category == "YIELD")
		{
			isShow = true;
		}

		if (imageInfo.category != "GROWTH")
		{
			continue;
		}

		if (m_growthListMap.find(imageInfo.data.toStdString()) != m_growthListMap.end()) {

			QVector<QVector<QString>> growthList = m_growthListMap[imageInfo.data.toStdString()];

			QTableWidget *table = new QTableWidget(this);
			QLabel *label = new QLabel(this);

			InitGrowthDisplayLegend(table);
			UpdateGrowthLegendTable(table, growthList);
			UpdateGrowthLegendLabel(label);

			//QLabel *tableTitle = new QLabel(tr2("表：") + tr2(imageInfo.data.toStdString().c_str()), this);
			//tableTitle->setAlignment(Qt::AlignCenter); // 或 Qt::AlignCenter 居中对齐
			//tableTitle->setStyleSheet("font-weight: bold; font-size: 12px;");

			QVBoxLayout *layout = new QVBoxLayout(); 
			//layout->addWidget(tableTitle);
			layout->addWidget(table);
			layout->addWidget(label);

			layout->setContentsMargins(0, 0, 0, 0);

			topLayout->addLayout(layout);
		}
	}

	ui->groupBox->setLayout(topLayout);  

	if (isShow) {

		QTableWidget *table = new QTableWidget(this);
		QLabel *label = new QLabel(this);

		InitYieldDisplayLegend(table);
		UpdateYieldLegendTable(table, m_yieldList);
		UpdateYieldLegendLabel(label);

		QVBoxLayout *layout = new QVBoxLayout();
		layout->addWidget(label);
		layout->addWidget(table);

		layout->setContentsMargins(0, 20, 0, 0); // 左、上、右、下

		ui->groupBox_2->setLayout(layout);
	}
}

void AgriSatelliteDataAnalysis::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
}

void AgriSatelliteDataAnalysis::showEvent(QShowEvent *event)
{
}

void AgriSatelliteDataAnalysis::hideEvent(QHideEvent *event)
{
}

void AgriSatelliteDataAnalysis::Init()
{
}

void AgriSatelliteDataAnalysis::InitGrowthDisplayLegend(QTableWidget *table)
{
	// 创建图例表格
	table->setColumnCount(4);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setVisible(false);

	// 禁用表格的编辑功能
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	table->setSelectionBehavior(QAbstractItemView::SelectRows);

	QStringList headers = { tr2("颜色") , tr2("长势"), tr2("占地面积 (亩)"), tr2("百分比 (%)") };
	table->setHorizontalHeaderLabels(headers);

	table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // 第0列固定
	table->setColumnWidth(0, 60);
	//table->setColumnWidth(1, 120);  
	//table->setColumnWidth(2, 115);

	//table->resizeRowsToContents();

	//table->setFixedHeight(150);
}

void AgriSatelliteDataAnalysis::InitYieldDisplayLegend(QTableWidget *table)
{
	// 创建图例表格
	table->setColumnCount(4);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	table->verticalHeader()->setVisible(false); 

	// 禁用表格的编辑功能
	table->setEditTriggers(QAbstractItemView::NoEditTriggers);

	// 设置 QTableWidget 按行选择
	table->setSelectionBehavior(QAbstractItemView::SelectRows);

	QStringList headers = { tr2("颜色"), tr2("产量范围 (kg/mu)"), tr2("占地面积 (亩)"), tr2("百分比 (%)") };
	table->setHorizontalHeaderLabels(headers);

	table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed); // 第0列固定
	table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed); 
	table->setColumnWidth(0, 60);
	table->setColumnWidth(2, 80);
	table->setColumnWidth(3, 60);

	//table->setFixedHeight(200);
}

void AgriSatelliteDataAnalysis::UpdateGrowthLegendLabel(QLabel *label)
{
	/*
	QString htmlText = QString::fromLocal8Bit(R"(
		<b>按照油菜花成熟期阶段来算，当前：</b>
		<ul style='margin-left:-20px;'>
		  <li>生物量可能的大概范围：<b>470-560 公斤/亩</b></li>
		  <li>LAI（叶面积指数）大概范围：<b>1.5-2.2</b></li>
		  <li>SPAD 大概范围：<b>20-30</b>，对应的叶绿素含量的变化范围大约在：<b>3ug-5.4ug/cm^3</b></li>
		</ul>
	)");

	ui->label_growth->setText(htmlText);
	ui->label_growth->setWordWrap(true);     // 自动换行
	ui->label_growth->adjustSize();          // 自适应大小
	ui->label_growth->setTextFormat(Qt::RichText);  // 启用 HTML 格式
	ui->label_growth->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	ui->label_growth->setStyleSheet("font-size: 12px; padding: 2px;");
	*/
}

void AgriSatelliteDataAnalysis::UpdateYieldLegendLabel(QLabel *label)
{
	QString htmlText = QString::fromLocal8Bit(R"(
		<b>产量：平均值为<b>140</b>公斤/亩，最小值<b>90</b>，最大值<b>182</b>公斤/亩</b>
	)");

	label->setText(htmlText);
	label->setWordWrap(true);     // 自动换行
	label->adjustSize();          // 自适应大小
	label->setTextFormat(Qt::RichText);  // 启用 HTML 格式
	label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	label->setStyleSheet("font-size: 12px; padding: 2px;");
	
}

void AgriSatelliteDataAnalysis::UpdateGrowthLegendTable(QTableWidget *table, const QVector<QVector<QString>> &growthList)
{
	table->setRowCount((int)growthList.size());

	for (int row = 0; row < growthList.size(); ++row)
	{
		auto& growth = growthList[row];

		QWidget *tmpWidget = new QWidget(); 
		QFrame *colorBox = new QFrame(tmpWidget);
		colorBox->setFixedSize(30, 30);
		colorBox->setFrameShape(QFrame::StyledPanel);
		colorBox->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(growth[0]));

		QVBoxLayout* vLayout = new QVBoxLayout(colorBox);
		colorBox->setLayout(vLayout);
		vLayout->setContentsMargins(0, 0, 0, 0);

		// 创建一个容器 widget 用于居中布局
		QWidget* container = new QWidget();
		QHBoxLayout* hLayout = new QHBoxLayout(container);
		hLayout->addStretch();
		hLayout->addWidget(colorBox);
		hLayout->addStretch();
		hLayout->setContentsMargins(0, 0, 0, 0);
		container->setLayout(hLayout);

		table->setCellWidget(row, 0, container);

		QTableWidgetItem* growthItem = new QTableWidgetItem(growth[1]);
		growthItem->setTextAlignment(Qt::AlignCenter);
		table->setItem(row, 1, growthItem);

		QTableWidgetItem* percentItem = new QTableWidgetItem(growth[2]);
		percentItem->setTextAlignment(Qt::AlignCenter);
		table->setItem(row, 2, percentItem);

		QTableWidgetItem* rangeItem = new QTableWidgetItem(growth[3]);
		rangeItem->setTextAlignment(Qt::AlignCenter);
		table->setItem(row, 3, rangeItem);
	}

	//ui->tableWidget->resizeColumnsToContents();
}

void AgriSatelliteDataAnalysis::UpdateYieldLegendTable(QTableWidget *table, const QVector<QVector<QString>> &yieldList)
{
	table->setRowCount((int)yieldList.size());

	for (int row = 0; row < yieldList.size(); ++row)
	{
		auto& yield = yieldList[row];

		QWidget *tmpWidget = new QWidget();
		QFrame *colorBox = new QFrame(tmpWidget);
		colorBox->setFixedSize(30, 30);
		colorBox->setFrameShape(QFrame::StyledPanel);
		colorBox->setStyleSheet(QString("background-color: %1; border: 1px solid black;").arg(yield[0]));

		QVBoxLayout* vLayout = new QVBoxLayout(colorBox);
		colorBox->setLayout(vLayout);
		vLayout->setContentsMargins(0, 0, 0, 0);

		// 创建一个容器 widget 用于居中布局
		QWidget* container = new QWidget();
		QHBoxLayout* hLayout = new QHBoxLayout(container);
		hLayout->addStretch();
		hLayout->addWidget(colorBox);
		hLayout->addStretch();
		hLayout->setContentsMargins(0, 0, 0, 0);
		container->setLayout(hLayout);

		table->setCellWidget(row, 0, container);

		QTableWidgetItem* yieldItem = new QTableWidgetItem(yield[1]);
		yieldItem->setTextAlignment(Qt::AlignCenter);
		table->setItem(row, 1, yieldItem);

		QTableWidgetItem* percentItem = new QTableWidgetItem(yield[2]);
		percentItem->setTextAlignment(Qt::AlignCenter);
		table->setItem(row, 2, percentItem);

		QTableWidgetItem* rangeItem = new QTableWidgetItem(yield[3]);
		rangeItem->setTextAlignment(Qt::AlignCenter);
		table->setItem(row, 3, rangeItem);
	}

	//ui->tableWidget->resizeColumnsToContents();
}
