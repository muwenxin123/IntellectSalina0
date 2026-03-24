#include <QFileDialog>
#include <QTabWidget>
#include <QTabBar>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QString>

#include "QCString.h"
#include "LyMessageBox.h"

#include "AgriAPSimAnalysis.h"
#include "ui_AgriAPSimAnalysis.h"

AgriAPSimAnalysis::AgriAPSimAnalysis(QWidget *parent)
	: QWidget(parent)
    , ui(new Ui::AgriAPSimAnalysis)
{
	ui->setupUi(this);

	QObject::connect(ui->tabWidget->tabBar(), &QTabBar::tabBarClicked, this, &AgriAPSimAnalysis::onTabBarClicked);
}

AgriAPSimAnalysis::~AgriAPSimAnalysis()
{
    delete ui;
}

void AgriAPSimAnalysis::setOutputPath(const QString& filePath)
{
	sFilePath = filePath;
	if (!sFilePath.isEmpty()) {

		// 替换文件后缀名
		sFilePath.replace(".apsim", ".out");

		QFile file(sFilePath);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
			QTextStream in(&file);
			ui->textBrowser->setText(in.readAll()); // 读取文件内容并显示
			file.close();
		}
	}

	vecOutputData.clear();
	parseOutputFile(vecOutputData);
}

void AgriAPSimAnalysis::onTabBarClicked(int index)
{
	QWidget *clickedTabWidget = ui->tabWidget->widget(index);
	if (!clickedTabWidget)return;

	QCustomPlot *customPlot = clickedTabWidget->findChild<QCustomPlot *>();
	if (!customPlot) {

		QVBoxLayout *layout = new QVBoxLayout(clickedTabWidget);
		QCustomPlot *customPlot = new QCustomPlot();
		layout->addWidget(customPlot);

		QVector<double> dayIndexes;  // X轴（日期，转为索引值）
		QVector<double> biomass, yields, stages;

		for (int i = 0; i < vecOutputData.size(); ++i) {
			dayIndexes.append(i);  // 用索引代替日期
			biomass.append(vecOutputData[i].biomass);
			yields.append(vecOutputData[i].yield);
			stages.append(vecOutputData[i].stage);
		}

		// 设置X轴为日期
		QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
		for (int i = 0; i < vecOutputData.size(); ++i) {
			textTicker->addTick(i, QString::fromStdString(vecOutputData[i].date));
		}
		customPlot->xAxis->setTicker(textTicker);

		updatePlotStyle(customPlot);

		switch (index)
		{
		case  1:
			plotChart(customPlot, dayIndexes, stages, "Day", "Stage");
			break;
		case  2:
			plotChart(customPlot, dayIndexes, yields, "Day", "Yield(kg/ha)");
			break;
		case  3:
			plotChart(customPlot, dayIndexes, biomass, "Day", "Biomass(kg/ha)");
			break;
		default:
			break;
		}
	}
}

bool AgriAPSimAnalysis::parseOutputFile(AgriOutputDataVec& vecOutputData)
{
	QFile file(sFilePath);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return false;
	}

	QTextStream in(&file);
	QString line;

	while (!in.atEnd()) {
		line = in.readLine();
		if (line.contains("Date", Qt::CaseInsensitive)) break;
	}

	// 跳过单位行
	if (!in.atEnd()) in.readLine();

	// 读取数据行
	while (!in.atEnd()) {
		line = in.readLine();
		QStringList columns = line.simplified().split(" ");

		if (columns.size() >= 7) {
			AgriOutputData point;
			point.date = columns[0].toStdString();
			point.biomass = columns[1].toDouble();
			point.yield = columns[2].toDouble();
			point.stage = columns[6].toDouble();
			vecOutputData.emplace_back(point);
		}
	}

	file.close();
	return true;
}

void AgriAPSimAnalysis::plotChart(QCustomPlot *customPlot, const QVector<double>& xAxis, const QVector<double>& yAxis, const QString& xLabel, const QString& yLabel)
{
	QCPGraph *graphStage = new QCPGraph(customPlot->xAxis, customPlot->yAxis);
	graphStage->setData(xAxis, yAxis);
	graphStage->setPen(QPen(Qt::blue));
	graphStage->setLineStyle(QCPGraph::lsLine);
	graphStage->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCircle, Qt::blue, 5));

	// 初始化右键菜单
	customPlot->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(customPlot, &QWidget::customContextMenuRequested, this, &AgriAPSimAnalysis::showContextMenu);

	/* 轴刻度范围 */
	customPlot->xAxis->setRange(0, *std::max_element(xAxis.begin(), xAxis.end()));
	customPlot->yAxis->setRange(0, *std::max_element(yAxis.begin(), yAxis.end()));

	/* 轴标签 */
	customPlot->xAxis->setLabel(xLabel);
	customPlot->yAxis->setLabel(yLabel);

	// 设置初始 X 轴标签
	updateXAxisLabels(customPlot, vecOutputData);

	// 连接范围变化信号
	QObject::connect(customPlot->xAxis, QOverload<const QCPRange&>::of(&QCPAxis::rangeChanged),
		[=]() {
		updateXAxisLabels(customPlot, vecOutputData);
	});
}

// 右键菜单槽函数
void AgriAPSimAnalysis::showContextMenu(const QPoint &pos)
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

void AgriAPSimAnalysis::updatePlotStyle(QCustomPlot *customPlot)
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

// 动态更新 X 轴标签
void AgriAPSimAnalysis::updateXAxisLabels(QCustomPlot* customPlot, const AgriOutputDataVec& data)
{
	QSharedPointer<QCPAxisTickerText> ticker(new QCPAxisTickerText);

	// 获取当前范围
	double rangeStart = customPlot->xAxis->range().lower;
	double rangeEnd = customPlot->xAxis->range().upper;

	// 动态设置显示间隔
	int interval = static_cast<int>((rangeEnd - rangeStart) / 10.0);  // 根据范围调整密度
	if (interval < 1) interval = 1;

	for (int i = 0; i < data.size(); ++i) {
		if (i % interval == 0) {
			ticker->addTick(i + 1, QString::fromStdString(data[i].date));  // 索引值从 1 开始
		}
	}

	customPlot->xAxis->setTicker(ticker);
	customPlot->replot();
}