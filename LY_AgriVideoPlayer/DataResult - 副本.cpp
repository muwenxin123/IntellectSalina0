#include "DataResult.h"
#include "ui_DataResult.h"
#include <QCString.h>
#include <QTimer>
#include <QDebug>
DataResult::DataResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataResult)
	, m_imageLabel(new QLabel(this))
{
    ui->setupUi(this);
	initTable();
	connect(ui->pushButton_1, &QPushButton::clicked, this, &DataResult::switchTo1);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &DataResult::switchTo2);
	connect(ui->pushButton_3, &QPushButton::clicked, this, &DataResult::switchTo3);
	connect(ui->pushButton_4, &QPushButton::clicked, this, &DataResult::switchTo4);
	connect(ui->pushButton_5, &QPushButton::clicked, this, &DataResult::switchTo5);

	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &DataResult::onDetectionDataReceived);
	m_timer->start(1000);
}

DataResult::~DataResult()
{
    delete ui;
}
void DataResult::initTable() {
	QStringList headerlabels;
	headerlabels << tr2("时间") << tr2("识别图像");

	ui->tableWidget->setColumnCount(headerlabels.size());
	ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget->verticalHeader()->setHidden(true);
	ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget->setAlternatingRowColors(true);
	ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	//设置列数
	ui->tableWidget->setRowCount(0);

	//设置表格属性
	ui->tableWidget->horizontalHeader()->setStretchLastSection(true);

	ui->tableWidget_2->setColumnCount(headerlabels.size());
	ui->tableWidget_2->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget_2->verticalHeader()->setHidden(true);
	ui->tableWidget_2->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget_2->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget_2->setAlternatingRowColors(true);
	ui->tableWidget_2->setContextMenuPolicy(Qt::CustomContextMenu);
	//设置列数
	ui->tableWidget_2->setRowCount(0);

	//设置表格属性
	ui->tableWidget_2->horizontalHeader()->setStretchLastSection(true);

	ui->tableWidget_3->setColumnCount(headerlabels.size());
	ui->tableWidget_3->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget_3->verticalHeader()->setHidden(true);
	ui->tableWidget_3->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget_3->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget_3->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget_3->setAlternatingRowColors(true);
	ui->tableWidget_3->setContextMenuPolicy(Qt::CustomContextMenu);
	//设置列数
	ui->tableWidget_3->setRowCount(0);

	//设置表格属性
	ui->tableWidget_3->horizontalHeader()->setStretchLastSection(true);

	ui->tableWidget_4->setColumnCount(headerlabels.size());
	ui->tableWidget_4->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget_4->verticalHeader()->setHidden(true);
	ui->tableWidget_4->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget_4->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget_4->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget_4->setAlternatingRowColors(true);
	ui->tableWidget_4->setContextMenuPolicy(Qt::CustomContextMenu);
	//设置列数
	ui->tableWidget_4->setRowCount(0);

	//设置表格属性
	ui->tableWidget_4->horizontalHeader()->setStretchLastSection(true);
}

void DataResult::switchTo1() {
	ui->stackedWidget->setCurrentIndex(0);
}
void DataResult::switchTo2() {
	ui->stackedWidget->setCurrentIndex(1);
}
void DataResult::switchTo3() {
	ui->stackedWidget->setCurrentIndex(2);
}
void DataResult::switchTo4() {
	ui->stackedWidget->setCurrentIndex(3);
}
void DataResult::switchTo5() {
	ui->stackedWidget->setCurrentIndex(4);
}


void DataResult::addImage(const WS::EventData& data)
{
	//// 获取当前页面的widget
	//QScrollArea* scrollArea = qobject_cast<QScrollArea*>(ui->stackedWidget->currentWidget());
	//if (!scrollArea) {
	//	return;
	//}

	//// 获取内容widget和布局
	//QWidget* contentWidget = scrollArea->widget();
	//if (!contentWidget) {
	//	return;
	//}

	//QGridLayout* layout = qobject_cast<QGridLayout*>(contentWidget->layout());
	//if (!layout) {
	//	// 如果没有布局，创建一个
	//	layout = new QGridLayout(contentWidget);
	//	layout->setSpacing(10);
	//	layout->setAlignment(Qt::AlignTop);
	//}

	//// 计算新图片的位置
	//int count = layout->count();
	//int row = count / 3;    // 每行3个图片
	//int col = count % 3;

	//// 创建图片Label
	//QLabel *imageLabel = new QLabel();
	//QPixmap pixmap = QPixmap::fromImage(image);
	//if (!pixmap.isNull()) {
	//	pixmap = pixmap.scaled(200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	//	imageLabel->setPixmap(pixmap);
	//}
	//imageLabel->setAlignment(Qt::AlignCenter);
	//imageLabel->setStyleSheet("border: 1px solid gray; background-color: white;");

	//// 添加到网格布局
	//layout->addWidget(imageLabel, row, col);


	//QReadLocker lk(&m_readLock);
	//// 添加到事件向量（在开头插入，保持最新数据在最前面）
	//eventdatavec.push_back(data);
	//if (eventdatavec.size() > 60)
	//{
	//	eventdatavec.erase(eventdatavec.begin());
	//}

	// 安全检查
	if (data.image.isNull()) {
		qDebug() << "addImage: Received null image, skipping";
		return;
	}

	if (data.videoid.empty()) {
		qDebug() << "addImage: Empty videoid, skipping";
		return;
	}

	QWriteLocker lk(&m_readLock);  // 改为写锁

								   // 限制存储数量
	if (eventdatavec.size() >= 600) {
		eventdatavec.erase(eventdatavec.begin());
	}

	// 深拷贝图像数据（重要！）
	//WS::EventData eventCopy = data;
	//eventCopy.image = data.image.copy();  // 创建图像副本

	eventdatavec.push_back(data);

	qDebug() << "Added image for videoid:" << QString::fromStdString(data.videoid)
		<< ", total events:" << eventdatavec.size();
}

//void DataResult::addRect(const QPainter& p) {
//	eventdatavec.push_back(p);
//}
void DataResult::onDetectionDataReceived() {
	QReadLocker lk(&m_readLock);
	std::vector<WS::EventData> tempVec = eventdatavec;
	lk.unlock();

	// 1. 创建videoid到tableWidget的映射
	QMap<QString, QTableWidget*> tableMap;
	tableMap["v1"] = ui->tableWidget;
	tableMap["v2"] = ui->tableWidget_2;
	tableMap["v3"] = ui->tableWidget_3;
	tableMap["v4"] = ui->tableWidget_4;

	// 2. 先重置所有表格
	for (auto* table : tableMap.values()) {
		if (table) {
			table->clearContents();
			table->setRowCount(0);
		}
	}

	// 3. 按videoid分组
	QMap<QString, QList<const WS::EventData*>> groupedData;
	for (const auto& event : tempVec) {
		groupedData[QString::fromStdString(event.videoid)].append(&event);
	}

	// 4. 填充每个表格
	for (auto it = tableMap.begin(); it != tableMap.end(); ++it) {
		const QString& vid = it.key();
		QTableWidget* table = it.value();

		if (!groupedData.contains(vid) || !table) {
			continue;
		}

		const auto& events = groupedData[vid];
		table->setRowCount(events.size());
		
		// 逆序填充：最新的数据在第0行
		for (int i = 0; i < events.size(); i++) {
			int row = events.size() - i - 1; // 逆序索引
			const auto* event = events[i];

			// 设置时间
			table->setItem(row, 0,
				new QTableWidgetItem(event->time.toString("hh:mm:ss")));

			// 设置事件内容
			if (!event->eventcontent.isEmpty() && table->columnCount() > 2) {
				table->setItem(row, 2,
					new QTableWidgetItem(event->eventcontent));
			}

			// 设置图像
			QLabel *imageLabel = new QLabel();
			QPixmap pixmap = QPixmap::fromImage(event->image);
			if (!pixmap.isNull()) {
				pixmap = pixmap.scaled(300, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				imageLabel->setPixmap(pixmap);
				imageLabel->setAlignment(Qt::AlignCenter);
			}

			table->setRowHeight(row, 300);
			table->setCellWidget(row, 1, imageLabel);
		}
	}
}