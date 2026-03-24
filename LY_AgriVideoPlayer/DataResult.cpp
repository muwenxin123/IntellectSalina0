#include "DataResult.h"
#include "ui_DataResult.h"
#include <QCString.h>
#include <QTimer>
#include <QDebug>
#include <QThread>
#include <QDialog>
#include <QScrollBar>
#include <QDebug>
DataResult::DataResult(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataResult)
	, m_imageLabel(new QLabel(this))
{
    ui->setupUi(this);
	initTable();
	initImageDialog();
	connect(ui->pushButton_1, &QPushButton::clicked, this, &DataResult::switchTo1);
	connect(ui->pushButton_2, &QPushButton::clicked, this, &DataResult::switchTo2);
	connect(ui->pushButton_3, &QPushButton::clicked, this, &DataResult::switchTo3);
	connect(ui->pushButton_4, &QPushButton::clicked, this, &DataResult::switchTo4);
	connect(ui->pushButton_5, &QPushButton::clicked, this, &DataResult::switchTo5);


	m_timer = new QTimer(this);
	connect(m_timer, &QTimer::timeout, this, &DataResult::onDetectionDataReceived);
	m_timer->start(1000);
}
void DataResult::initImageDialog()
{
	m_imageDialog = new QDialog(this);
	m_imageDialog->setWindowTitle(tr2("图片详情"));
	m_imageDialog->resize(1200, 900);

	QVBoxLayout* layout = new QVBoxLayout(m_imageDialog);
	layout->setContentsMargins(0, 0, 0, 0);
	//QScrollArea* scrollArea = new QScrollArea(m_imageDialog);
	//scrollArea->setWidgetResizable(true);

	m_imageDisplayLabel = new QLabel();
	m_imageDisplayLabel->setAlignment(Qt::AlignCenter);
	m_imageDisplayLabel->setStyleSheet("QLabel{ background-color:#2b2b2b;}");
	m_imageDisplayLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	//scrollArea->setWidget(m_imageDisplayLabel);
	//layout->addWidget(scrollArea);
	layout->addWidget(m_imageDisplayLabel, 1);  // 1 表示拉伸因子

	QPushButton* closeBtn = new QPushButton(tr2("关闭"), m_imageDialog);
	closeBtn->setFixedSize(100, 30);
	connect(closeBtn, &QPushButton::clicked, m_imageDialog, &QDialog::accept);
	layout->addWidget(closeBtn, 0, Qt::AlignCenter);
}

void DataResult::showImageDialog(const QImage& image, const int& info)
{
	QWriteLocker lk(&m_readLock);

	if (image.isNull()) {
		return;
	}
	
	if (image.width() <= 0 || image.height() <= 0) {
		return;
	}

	if (image.format() == QImage::Format_Invalid) {
		return;
	}

	if (!m_imageDialog) {
		initImageDialog();
	}
	
	if (!m_imageDialog || !m_imageDisplayLabel)
		return;

	QPixmap pixmap;
	try {
		pixmap = QPixmap::fromImage(image);
		if (pixmap.isNull()) {
			qDebug() << "❌ 转换失败";
			return;
		}
	}
	catch (...) {
		qDebug() << "❌ 转换异常";
		return;
	}

	m_imageDisplayLabel->setPixmap(pixmap);
	m_imageDisplayLabel->setScaledContents(true);

	m_imageDialog->resize(800, 600);
	if (!info ==NULL) {
		m_imageDialog->setWindowTitle(tr2("图片详情 识别总数- ") + QString::number(info));
	}

	m_imageDialog->show();
	m_imageDialog->raise();
}

DataResult::~DataResult()
{
	m_isShuttingDown = true;

	if (m_timer) {
		m_timer->stop();
		m_timer->deleteLater();
		m_timer = nullptr;
	}

	if (m_imageDialog) {
		m_imageDialog->close();  // 关闭对话框
		m_imageDialog->deleteLater();
		m_imageDialog = nullptr;
	}

	{
		QWriteLocker locker(&m_readLock);
		eventdatavec.clear();
	}

	disconnect();

	//QCoreApplication::processEvents();

    delete ui;
}

void DataResult::shutdown() {
	m_isShuttingDown = true;

	if (m_timer) {
		m_timer->stop();
		m_timer->disconnect();
		qDebug() <<"stop finish";
	}

	disconnect();
	if (m_imageDialog) {
		m_imageDialog->close();
	}

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

	ui->tableWidget_5->setColumnCount(headerlabels.size());
	ui->tableWidget_5->setHorizontalHeaderLabels(headerlabels);
	ui->tableWidget_5->verticalHeader()->setHidden(true);
	ui->tableWidget_5->setSelectionBehavior(QTableWidget::SelectRows);
	ui->tableWidget_5->setSelectionMode(QAbstractItemView::SingleSelection);
	ui->tableWidget_5->setEditTriggers(QAbstractItemView::NoEditTriggers);
	ui->tableWidget_5->setAlternatingRowColors(true);
	ui->tableWidget_5->setContextMenuPolicy(Qt::CustomContextMenu);
	//设置列数
	ui->tableWidget_5->setRowCount(0);

	//设置表格属性
	ui->tableWidget_5->horizontalHeader()->setStretchLastSection(true);
	//ui->pushButton_1;
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


//void DataResult::addImage(const WS::EventData& data)
//{
//	//// 获取当前页面的widget
//	//QScrollArea* scrollArea = qobject_cast<QScrollArea*>(ui->stackedWidget->currentWidget());
//	//if (!scrollArea) {
//	//	return;
//	//}
//
//	//// 获取内容widget和布局
//	//QWidget* contentWidget = scrollArea->widget();
//	//if (!contentWidget) {
//	//	return;
//	//}
//
//	//QGridLayout* layout = qobject_cast<QGridLayout*>(contentWidget->layout());
//	//if (!layout) {
//	//	// 如果没有布局，创建一个
//	//	layout = new QGridLayout(contentWidget);
//	//	layout->setSpacing(10);
//	//	layout->setAlignment(Qt::AlignTop);
//	//}
//
//	//// 计算新图片的位置
//	//int count = layout->count();
//	//int row = count / 3;    // 每行3个图片
//	//int col = count % 3;
//
//	//// 创建图片Label
//	//QLabel *imageLabel = new QLabel();
//	//QPixmap pixmap = QPixmap::fromImage(image);
//	//if (!pixmap.isNull()) {
//	//	pixmap = pixmap.scaled(200, 150, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//	//	imageLabel->setPixmap(pixmap);
//	//}
//	//imageLabel->setAlignment(Qt::AlignCenter);
//	//imageLabel->setStyleSheet("border: 1px solid gray; background-color: white;");
//
//	//// 添加到网格布局
//	//layout->addWidget(imageLabel, row, col);
//
//
//	QWriteLocker lk(&m_readLock);
//	// 添加到事件向量（在开头插入，保持最新数据在最前面）
//	eventdatavec.push_back(data);
//	if (eventdatavec.size() > 60)
//	{
//		eventdatavec.erase(eventdatavec.begin());
//	}
//
//}

void DataResult::addImage(const WS::EventData& data)
{
	// 线程安全检查
	if (QThread::currentThread() != this->thread()) {
		qDebug() << "DataResult::addImage 跨线程调用";

		// 复制数据
		WS::EventData dataCopy = data;

		QMetaObject::invokeMethod(this, [this, dataCopy]() {
			QWriteLocker lk(&m_readLock);
			eventdatavec.push_back(dataCopy);

			if (eventdatavec.size() > 60) {
				eventdatavec.erase(eventdatavec.begin());
			}
		}, Qt::QueuedConnection);
	}
	else {
		QWriteLocker lk(&m_readLock);
		eventdatavec.push_back(data);

		if (eventdatavec.size() > 60) {
			eventdatavec.erase(eventdatavec.begin());
		}
	}
}
//void DataResult::addRect(const QPainter& p) {
//	eventdatavec.push_back(p);
//}
//void DataResult::onDetectionDataReceived() {
//	QReadLocker lk(&m_readLock);
//	std::vector<WS::EventData> tempVec = eventdatavec;
//	lk.unlock();
//
//	// 1. 创建videoid到tableWidget的映射
//	QMap<QString, QTableWidget*> tableMap;
//	tableMap["v1"] = ui->tableWidget;
//	tableMap["v2"] = ui->tableWidget_2;
//	tableMap["v3"] = ui->tableWidget_3;
//	tableMap["v4"] = ui->tableWidget_4;
//	tableMap["av_stream"] = ui->tableWidget_5;
//
//	// 2. 先重置所有表格
//	for (auto* table : tableMap.values()) {
//		if (table) {
//			table->clearContents();
//			table->setRowCount(0);
//		}
//	}
//
//	// 3. 按videoid分组
//	QMap<QString, QList<const WS::EventData*>> groupedData;
//	for (const auto& event : tempVec) {
//		groupedData[QString::fromStdString(event.videoid)].append(&event);
//	}
//
//	// 4. 填充每个表格
//	for (auto it = tableMap.begin(); it != tableMap.end(); ++it) {
//		const QString& vid = it.key();
//		QTableWidget* table = it.value();
//
//		if (!groupedData.contains(vid) || !table) {
//			continue;
//		}
//
//		const auto& events = groupedData[vid];
//		table->setRowCount(events.size());
//
//		// 逆序填充：最新的数据在第0行
//		for (int i = 0; i < events.size(); i++) {
//			int row = events.size() - i - 1; // 逆序索引
//			const auto* event = events[i];
//
//			// 设置时间
//			table->setItem(row, 0,
//				new QTableWidgetItem(event->time.toString("hh:mm:ss")));
//
//			// 设置事件内容
//			if (!event->eventcontent.isEmpty() && table->columnCount() > 2) {
//				table->setItem(row, 2,
//					new QTableWidgetItem(event->eventcontent));
//			}
//
//			// 设置图像
//			QLabel *imageLabel = new QLabel();
//			QPixmap pixmap = QPixmap::fromImage(event->image);
//			if (!pixmap.isNull()) {
//				pixmap = pixmap.scaled(300, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
//				imageLabel->setPixmap(pixmap);
//				imageLabel->setAlignment(Qt::AlignCenter);
//			}
//
//			table->setRowHeight(row, 300);
//			table->setCellWidget(row, 1, imageLabel);
//		}
//	}
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
	tableMap["av_stream"] = ui->tableWidget_5;

	// 2. 按videoid分组
	QMap<QString, QList<const WS::EventData*>> groupedData;
	for (const auto& event : tempVec) {
		groupedData[QString::fromStdString(event.videoid)].append(&event);
	}

	// 3. 更新每个表格（限制最多10行）
	for (auto it = tableMap.begin(); it != tableMap.end(); ++it) {
		const QString& vid = it.key();
		QTableWidget* table = it.value();

		if (!groupedData.contains(vid) || !table) {
			// 如果没有这个视频流的新数据，跳过
			continue;
		}

		const auto& events = groupedData[vid];

		int scrollValue = 0;
		if (table->verticalScrollBar()) {
			scrollValue = table->verticalScrollBar()->value();

		}

		// 获取当前表格的行数
		int currentRows = table->rowCount();

		// 计算需要添加的新行数
		int newRows = events.size();

		// 计算总行数（当前 + 新数据）
		int totalRows = currentRows + newRows;

		if (totalRows > 100) {
			// 如果超过10行，需要删除最旧的行
			int rowsToRemove = totalRows - 100;

			// 删除最旧的行（从第0行开始删除）
			for (int i = 0; i < rowsToRemove; i++) {
				table->removeRow(0);
			}

			// 更新当前行数
			currentRows = table->rowCount();
		}

		// 添加新数据到表格末尾
		for (int i = 0; i < events.size(); i++) {
			int row = currentRows + i;
			if (row >= 100) break;  // 确保不超过10行

			const auto* event = events[i];

			// 如果行数不够，插入新行
			if (row >= table->rowCount()) {
				table->insertRow(row);
			}

			// 设置时间
			table->setItem(row, 0,
				new QTableWidgetItem(event->time.toString("hh:mm:ss")));

			// 设置事件内容
			if (!event->eventcontent.isEmpty() && table->columnCount() > 2) {
				table->setItem(row, 2,new QTableWidgetItem(event->eventcontent));
			}

			// 设置图像
			//QLabel *imageLabel = new QLabel();
			//QPixmap pixmap = QPixmap::fromImage(event->image);
			//if (!pixmap.isNull()) {
			//	pixmap = pixmap.scaled(300, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			//	imageLabel->setPixmap(pixmap);
			//	imageLabel->setAlignment(Qt::AlignCenter);
			//}
			//table->setRowHeight(row, 300);
			//table->setCellWidget(row, 1, imageLabel);


					//ClickableImageLabel* imageLabel = new ClickableImageLabel();
			//imageLabel->setOriginalImage(event->image);

			//QPixmap pixmap = QPixmap::fromImage(event->image);
			//if (!pixmap.isNull()) {
			//	QPixmap scaled = pixmap.scaled(300, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			//	imageLabel->setPixmap(scaled);
			//	imageLabel->setAlignment(Qt::AlignCenter);
			//	connect(imageLabel, &ClickableImageLabel::clicked, this, [this, event](){this->showImageDialog(event->image, event->eventcontent); },
			//Qt::QueuedConnection);
			//}
//弹窗式图片显示功能
			QImage imageCopy = event->image.copy(); 
			int totalBoxesInThisImage = event->totalBoxesInImage;
			ClickableImageLabel* imageLabel = new ClickableImageLabel();
			imageLabel->setOriginalImage(imageCopy);  // 保存原图用于弹窗

			QPixmap pixmap = QPixmap::fromImage(imageCopy);
			if (!pixmap.isNull()) {
				pixmap = pixmap.scaled(300, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				imageLabel->setPixmap(pixmap);
				imageLabel->setAlignment(Qt::AlignCenter);
				//lambda匿名函数，c++11引入，可以就地定义和使用
				//使用 lambda 直接调用 showImageDialog
				//connect(imageLabel, &ClickableImageLabel::clicked,
				//	[this, imageCopy, totalBoxesInThisImage]() {
				//	this->showImageDialog(imageCopy, totalBoxesInThisImage);
				//});
				/////////////////////////////////////////////////////////
				//connect(imageLabel, &ClickableImageLabel::clicked, this,  
				//	[this, imageCopy, totalBoxesInThisImage]() {
				//	if (!m_isShuttingDown) {  
				//		this->showImageDialog(imageCopy, totalBoxesInThisImage);
				//	}
				//}, Qt::UniqueConnection);
				// 在 DataResult.cpp 的 onDetectionDataReceived 中
				connect(imageLabel, &ClickableImageLabel::clicked, this,
					[this, imageCopy, eventData = *event]() {  // ⭐ 直接拷贝整个 EventData
					if (!m_isShuttingDown) {
						ImageDetailDialog* dialog = new ImageDetailDialog(this);
						dialog->setAttribute(Qt::WA_DeleteOnClose);
						dialog->setImage(imageCopy);
						dialog->setEventData(eventData);  // 使用拷贝的数据
						dialog->show();
					}
				}, Qt::UniqueConnection);
			}

			table->setRowHeight(row, 300);
			table->setCellWidget(row, 1, imageLabel);
		} 
		if (table->verticalScrollBar()) {
			// 保持用户位置不变
			table->verticalScrollBar()->setValue(scrollValue);
		}
	}

	// 清空临时数据（避免重复处理）
	{
		QWriteLocker lk(&m_readLock);
		eventdatavec.clear();
	}
}