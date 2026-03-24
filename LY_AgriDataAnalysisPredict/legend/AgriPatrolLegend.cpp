#include <QDebug>
#include <QLabel>
#include <QHeaderView>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QVBoxLayout>

#include "AgriPatrolLegend.h"
#include "ui_AgriPatrolLegend.h"


AgriPatrolLegend::AgriPatrolLegend(QWidget *parent /*= nullptr*/):
	QWidget(parent),
	ui(new Ui::AgriPatrolLegend)
{ 
	ui->setupUi(this);

	Init();
}

AgriPatrolLegend::~AgriPatrolLegend()
{
	delete ui;
}

void AgriPatrolLegend::UpdatTaskInfoLegend()
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

	{
		QVBoxLayout * topLayout = new QVBoxLayout();
		topLayout->setContentsMargins(0, 20, 0, 0); // 左、上、右、下

		QLabel *label = new QLabel(this);

		QLabel *titleLabel = new QLabel(this);
		titleLabel->setStyleSheet("font-size: 28px;");
		titleLabel->setText(tr2("无人机多光谱巡检任务"));
		titleLabel->move(96, 25);
		titleLabel->setFixedSize(300, 32);

		QString htmlText = QString::fromLocal8Bit(R"(
			<ul style='font-size:20px;'>
			  <li><b>作物长势监测：</b>基于NDVI等植被指数，评估当前玉米生长状况，辅助识别弱苗区域与生长不均匀地块。</li>
			  <li><b>病虫害与胁迫预警：</b>利用光谱数据识别早期病害（如锈病、大斑病）和虫害（如玉米螟），结合历史数据趋势预测风险，生成科学防治建议。</li>
			  <li><b>产量与品质预估：</b>建立作物生长模型，评估玉米生物量、成熟度，提前预估各地块产量及品质水平。</li>
			  <li><b>精准农事指导：</b>结合长势与病虫害情况，提供变量施肥和精准喷药建议，提升资源利用效率。</li>
			</ul>
			)");

		label->setText(htmlText);
		label->setWordWrap(true);     // 自动换行
		label->adjustSize();          // 自适应大小
		label->setTextFormat(Qt::RichText);  // 启用 HTML 格式
		label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
		label->setStyleSheet("font-size: 20px; padding: 2px;");

		QVBoxLayout *layout = new QVBoxLayout();
		layout->addWidget(label);
		layout->setContentsMargins(0, 0, 0, 0);

		topLayout->addLayout(layout);

		ui->groupBox->setTitle("");
		ui->groupBox->setStyleSheet(QString("background-color: transparent;"));
		ui->groupBox->setLayout(topLayout);
	}
}

void AgriPatrolLegend::UpdateShowLegend()
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
	//cleanLayout(ui->groupBox_2);

	{
		QVBoxLayout * topLayout = new QVBoxLayout();
		topLayout->setContentsMargins(0, 20, 0, 0); // 左、上、右、下
		QVector<QVector<QString>> growthList = m_growthListMap["20250416T051208"];

		QTableWidget *table = new QTableWidget(this);
		QLabel *label = new QLabel(this);

		QLabel *titleLabel = new QLabel(this);
		titleLabel->setStyleSheet("font-size: 28px;");
		titleLabel->setText(tr2("无人机多光谱巡检结果"));
		titleLabel->move(96, 25);
		titleLabel->setFixedSize(300, 32);

		InitGrowthDisplayLegend(table);
		UpdateGrowthLegendTable(table, growthList);
		UpdateGrowthLegendLabel(label);
		QLabel *sugLabel = new QLabel(this);
		UpdateGrowthSuggestLabel(sugLabel);

		//QLabel *tableTitle = new QLabel(tr2("表：") + tr2(imageInfo.data.toStdString().c_str()), this);
		//tableTitle->setAlignment(Qt::AlignCenter); // 或 Qt::AlignCenter 居中对齐
		//tableTitle->setStyleSheet("font-weight: bold; font-size: 12px;");

		QVBoxLayout *layout = new QVBoxLayout();
		//layout->addWidget(tableTitle);
		layout->addWidget(label);
		layout->addWidget(table);
		layout->addWidget(sugLabel);
		layout->setContentsMargins(0, 0, 0, 0);

		topLayout->addLayout(layout);

		//"color: black;"                  // 设置字体颜色
		//	"font-weight: bold;"            // 字体加粗
		//	"font-size: 14px;"              // 字体大小
		//	"border: 1px solid gray;"       // 设置边框
		//	"padding: 4px;"
		table->horizontalHeader()->setStyleSheet( 
			"QHeaderView::section {"
			"background-color: rgba(25, 163, 137, 77);"   // 设置背景颜色
			"}"
		);

		ui->groupBox->setTitle("");
		ui->groupBox->setStyleSheet(QString("background-color: transparent;"));
		ui->groupBox->setLayout(topLayout);
	}

	/*
	{
		QTableWidget *table = new QTableWidget(this);
		QLabel *label = new QLabel(this);

		InitYieldDisplayLegend(table);
		UpdateYieldLegendTable(table, m_yieldList);
		UpdateYieldLegendLabel(label);

		table->horizontalHeader()->setStyleSheet(
			"QHeaderView::section {"
			"background-color: rgba(25, 163, 137, 77);"   // 设置背景颜色
			"}"
		);

		QVBoxLayout *layout = new QVBoxLayout();
		layout->addWidget(label);
		layout->addWidget(table);
		layout->setContentsMargins(0, 20, 0, 0); // 左、上、右、下

		label->setStyleSheet(QString("background-color: transparent;"));
		ui->groupBox_2->setStyleSheet(QString("background-color: transparent;"));
		
		ui->groupBox_2->setLayout(layout);
	}
	*/
}

void AgriPatrolLegend::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
}

void AgriPatrolLegend::showEvent(QShowEvent *event)
{
}

void AgriPatrolLegend::hideEvent(QHideEvent *event)
{
}

void AgriPatrolLegend::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	//QPainter painter(this);
	//painter.setRenderHint(QPainter::Antialiasing);

	//QColor backgroundColor = QColor(10, 30, 80, 0); // 深蓝透明
	//painter.setBrush(backgroundColor);
	//painter.setPen(Qt::NoPen);
	//painter.drawRoundedRect(rect(), 5, 5); // 圆角矩形
}

void AgriPatrolLegend::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		QApplication::setOverrideCursor(Qt::OpenHandCursor);
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		m_dragging = true;
	}
}

void AgriPatrolLegend::mouseMoveEvent(QMouseEvent *event)
{
	if (m_dragging && (event->buttons() & Qt::LeftButton)) {
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		move(event->globalPos() - m_dragPosition);
	}
}

void AgriPatrolLegend::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	QApplication::setOverrideCursor(Qt::ArrowCursor);
	m_dragging = false;
}

void AgriPatrolLegend::Init()
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

	ui->widget->setObjectName("parentWidget"); 
	ui->widget->setStyleSheet("QWidget#parentWidget {background-image:url(:/images/Legend/topwindow.png)};");
}

void AgriPatrolLegend::InitGrowthDisplayLegend(QTableWidget *table)
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

void AgriPatrolLegend::InitYieldDisplayLegend(QTableWidget *table)
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

void AgriPatrolLegend::UpdateGrowthLegendLabel(QLabel *label)
{
	QString htmlText = QString::fromLocal8Bit(R"(
	  <p><b>基于多光谱 NDVI 数据分析，结合当前玉米生长阶段（<span style='color:#2E8B57;'>灌浆期</span>），长势评估如下：</b></p>
	  <ul style='margin-left:-20px; font-size:18px;'>
		<li>估算生物量范围：<b style='color:#1E90FF;'>450 - 600 公斤/亩</b>，处于正常偏高水平</li>
		<li>叶面积指数（LAI）：<b style='color:#1E90FF;'>1.5 - 3.0</b>，叶片覆盖良好，具备良好光合能力</li>
		<li>SPAD 值（叶绿素含量）：<b style='color:#1E90FF;'>20 - 35</b>，对应叶绿素浓度为 <b>3 - 5.5 μg/cm3</b></li>
		<li>冠层高度估算：<b style='color:#1E90FF;'>160 - 220 cm</b>，多数植株表现健壮、挺拔</li>
		<li>NDVI 值集中在：<b style='color:#1E90FF;'>0.65 - 0.85</b>，显示植被覆盖度和生长活力均较高</li>
		<li>植株长势均匀性：<b style='color:#1E90FF;'>良好</b>，95% 区域 NDVI 标准差低于 0.05</li>
	  </ul>
	)");

	QString htmlText1 = QString::fromLocal8Bit(R"(
	  <p><b>玉米进入灌浆期，检测显示NDVI偏低，生物量不足，建议补施肥料以促进粒重形成。</b></p>
	)");

	label->setText(htmlText);
	label->setWordWrap(true);     // 自动换行
	label->adjustSize();          // 自适应大小
	label->setTextFormat(Qt::RichText);  // 启用 HTML 格式
	label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	label->setStyleSheet("font-size: 18px; padding: 2px;");
}

void AgriPatrolLegend::UpdateGrowthSuggestLabel(QLabel *label)
{
	QString htmlText = QString::fromLocal8Bit(R"(
	  <p><b>玉米进入灌浆期，检测显示NDVI偏低，生物量不足，建议补施肥料以促进粒重形成。</b></p>
	)");

	label->setText(htmlText);
	label->setWordWrap(true);     // 自动换行
	label->adjustSize();          // 自适应大小
	label->setTextFormat(Qt::RichText);  // 启用 HTML 格式
	label->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	label->setStyleSheet("font-size: 18px; padding: 2px;");
}

void AgriPatrolLegend::UpdateYieldLegendLabel(QLabel *label)
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

void AgriPatrolLegend::UpdateGrowthLegendTable(QTableWidget *table, const QVector<QVector<QString>> &growthList)
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

void AgriPatrolLegend::UpdateYieldLegendTable(QTableWidget *table, const QVector<QVector<QString>> &yieldList)
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
