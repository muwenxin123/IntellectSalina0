#include <QFileDialog>
#include <QTabWidget>
#include <QTabBar>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QString>

#include "QCString.h"
#include "LyMessageBox.h"

#include "AgriAPSimMet.h"
#include "ui_AgriAPSimMet.h"

AgriAPSimMet::AgriAPSimMet(QWidget *parent)
	: QWidget(parent)
    , ui(new Ui::AgriAPSimMet)
{
	ui->setupUi(this);

	QTabBar *tabBar = ui->tabWidget->tabBar();
	QObject::connect(tabBar, &QTabBar::tabBarClicked, this, &AgriAPSimMet::onTabBarClicked);
}

AgriAPSimMet::~AgriAPSimMet()
{
    delete ui;
}

void AgriAPSimMet::on_BT_LoadData_clicked()
{
	// 打开文件选择对话框
	sFilePath = QFileDialog::getOpenFileName(this, tr2("请选择气象文件"), "../data/model/apsim/files", "Text Files (*.met);;All Files (*.*)", nullptr, QFileDialog::Option::ReadOnly);

	if (!sFilePath.isEmpty()) {
		QFile file(sFilePath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in(&file);
			ui->raw_data->setText(in.readAll()); // 读取文件内容并显示
			file.close();
		}
		else {
			LyMessageBox::warning(this, tr2("错误"), tr2("无法打开文件！"));
		}
	}

	ui->FL_Name->setText(sFilePath);
}

// qt tabwidget 连接不同tab页点击信号槽，使用qcustomplot库，第一个tab页点击展示

 bool AgriAPSimMet::parseMetFile(AgriMetDataVec& vecMetData)
{
	 QFile file(sFilePath);
	 if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		 LyMessageBox::warning(nullptr, tr2("警告"), tr2("未获取气象文件"), QMessageBox::Ok);
		 return false;
	 }

	 QTextStream in(&file);
	 bool dataSection = false;

	 while (!in.atEnd()) {
		 QString line = in.readLine().trimmed();

		 // 跳过注释、空行和括号内容行
		 if (line.isEmpty() || line.startsWith("!") || line.startsWith("[") || line.contains("(")) {
			 continue;
		 }

		 // 检查是否到达数据部分
		 if (!dataSection && line.contains("year", Qt::CaseInsensitive)) {
			 dataSection = true;
			 continue;
		 }

		 // 解析数据部分
		 if (dataSection) {
			 QStringList columns = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);
			 if (columns.size() < 7) {
				 continue; // 确保有足够的列
			 }

			 AgriMetData data;
			 data.site = columns[0].toStdString();
			 data.year = columns[1].toInt();
			 data.day = columns[2].toInt();
			 data.radn = columns[3].toDouble();
			 data.maxt = columns[4].toDouble();
			 data.mint = columns[5].toDouble();
			 data.rain = columns[6].toDouble();
			 //data.evap = columns[7].toDouble();

			 vecMetData.emplace_back(data);
		 }
	 }

	 file.close();

	 return true;
}


void AgriAPSimMet::onTabBarClicked(int index)
{
	switch (index)
	{
	case  1:
		handleRainfallChart();
		break;
	case  2:
		handleTempeartureChart();
		break;
	default:
		break;
	}
}

void AgriAPSimMet::handleRainfallChart()
{
	vecMetData.clear();
	if (!parseMetFile(vecMetData)) return;

	QCustomPlot *customPlot = ui->tab_2->findChild<QCustomPlot *>();
	if (!customPlot) {

		QVBoxLayout *layout = new QVBoxLayout(ui->tab_2);
		QCustomPlot *customPlot = new QCustomPlot();
		layout->addWidget(customPlot);

		QVector<double> days, rainfalls;
		for (const auto &data : vecMetData) {
			days.push_back(static_cast<double>(data.day));
			rainfalls.push_back(data.rain);
		}

		updatePlotStyle(customPlot);
		plotRainfall(customPlot, days, rainfalls);
	}
}

void AgriAPSimMet::handleTempeartureChart()
{
	vecMetData.clear();
	if (!parseMetFile(vecMetData)) return;

	QCustomPlot *customPlot = ui->tab_3->findChild<QCustomPlot *>();
	if (!customPlot) {

		QVBoxLayout *layout = new QVBoxLayout(ui->tab_3);
		QCustomPlot *customPlot = new QCustomPlot();
		layout->addWidget(customPlot);

		QVector<double> days, maxt, mint;
		for (const auto &data : vecMetData) {
			days.push_back(static_cast<double>(data.day));
			maxt.push_back(data.maxt);
			mint.push_back(data.mint);
		}

		updatePlotStyle(customPlot);
		plotTemperate(customPlot, days, maxt, mint);
	}
}

void AgriAPSimMet::plotRainfall(QCustomPlot *customPlot, const QVector<double>& days, const QVector<double>& rainfall)
{
	QCPBars *rainBars = new QCPBars(customPlot->xAxis, customPlot->yAxis);
	rainBars->setWidth(0.6);
	rainBars->setData(days, rainfall);
	rainBars->setPen(QPen(Qt::blue));
	rainBars->setBrush(QColor(0, 0, 255, 150));

	/* 轴标签 */
	customPlot->xAxis->setLabel("Day of Year");
	customPlot->yAxis->setLabel("Rainfall (mm)");

	// 初始化右键菜单
	customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(customPlot, &QWidget::customContextMenuRequested, this, &AgriAPSimMet::showContextMenu);

	/* 轴刻度范围 */
	customPlot->xAxis->setRange(0, *std::max_element(days.begin(), days.end()));
	customPlot->yAxis->setRange(0, *std::max_element(rainfall.begin(), rainfall.end()));
}

void AgriAPSimMet::plotTemperate(QCustomPlot *customPlot, const QVector<double>& days, const QVector<double>& maxts, const QVector<double>& mint)
{
	// 添加第一条曲线：最高温
	customPlot->addGraph();
	customPlot->graph(0)->setData(days, maxts);
	customPlot->graph(0)->setPen(QPen(Qt::red)); // 设置曲线颜色为红色
	customPlot->graph(0)->setName("Max Temperature");

	// 添加第二条曲线：最低温
	customPlot->addGraph();
	customPlot->graph(1)->setData(days, mint);
	customPlot->graph(1)->setPen(QPen(Qt::blue)); // 设置曲线颜色为蓝色
	customPlot->graph(1)->setName("Min Temperature");

	// 设置坐标轴
	customPlot->xAxis->setLabel("Day");
	customPlot->yAxis->setLabel("Temperature(℃)");

	// 设置坐标范围
	customPlot->xAxis->setRange(0, *std::max_element(days.begin(), days.end()) + 1); // 横坐标范围
	double maxTemp = *std::max_element(maxts.begin(), maxts.end());
	double minTemp = *std::min_element(mint.begin(), mint.end());
	customPlot->yAxis->setRange(minTemp - 5, maxTemp + 5); // 纵坐标范围

	// 启用图例
	customPlot->legend->setVisible(true);
	customPlot->legend->setFont(QFont("Helvetica", 9));

	// 初始化右键菜单
	customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(customPlot, &QWidget::customContextMenuRequested, this, &AgriAPSimMet::showContextMenu);

	customPlot->replot();
}

// 右键菜单槽函数
void AgriAPSimMet::showContextMenu(const QPoint &pos)
{
	// 获取信号发送者
	QCustomPlot *customPlot = qobject_cast<QCustomPlot *>(sender());
	if (!customPlot) return;

	QMenu menu;

	// 添加菜单项
	menu.addAction(tr2("重置视图"), this, [customPlot]() {
		customPlot->rescaleAxes();
		customPlot->replot();
	});
	menu.addAction(tr2("保存为图片"), this, [customPlot]() {
		customPlot->savePng("plot.png");
	});

	// 显示菜单
	menu.exec(customPlot->mapToGlobal(pos));
}

void AgriAPSimMet::updatePlotStyle(QCustomPlot *customPlot)
{
	/* -------------------------------------------------- 轴线 -------------------------------------------------- */
	/* 轴线的画笔 */
	customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
	customPlot->yAxis->setBasePen(QPen(Qt::white, 1));
	/* 轴刻度线的画笔 */
	customPlot->xAxis->setBasePen(QPen(Qt::white, 1));
	customPlot->yAxis->setTickPen(QPen(Qt::white, 1));
	/* 轴子刻度线的画笔 */
	customPlot->xAxis->setSubTickPen(QPen(Qt::white, 1));
	customPlot->yAxis->setSubTickPen(QPen(Qt::white, 1));

	/* 轴刻度文字颜色 */
	customPlot->xAxis->setTickLabelColor(Qt::white);
	customPlot->yAxis->setTickLabelColor(Qt::white);
	/* 轴标签颜色 */
	customPlot->xAxis->setLabelColor(Qt::white);
	customPlot->yAxis->setLabelColor(Qt::white);


	/* -------------------------------------------------- 网格 -------------------------------------------------- */
	/* 网格线(对应刻度)画笔 */
	customPlot->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
	customPlot->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
	/* 子网格线(对应子刻度)画笔 */
	customPlot->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
	customPlot->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
	/* 显示子网格线 */
	customPlot->xAxis->grid()->setSubGridVisible(true);
	customPlot->yAxis->grid()->setSubGridVisible(true);
	/* 设置刻度为0时的网格线的画笔 */
	customPlot->xAxis->grid()->setZeroLinePen(QPen(Qt::red));
	customPlot->yAxis->grid()->setZeroLinePen(QPen(Qt::red));


	/* -------------------------------------------------- 区域 -------------------------------------------------- */
	// 设置绘图区域背景色
	customPlot->setBackground(Qt::transparent);


	/* -------------------------------------------------- 交互 -------------------------------------------------- */
	// 启用交互功能
	customPlot->setInteraction(QCP::iRangeDrag, true);  // 允许拖动图表进行滚动
	customPlot->setInteraction(QCP::iRangeZoom, true);  // 允许缩放图表
}
