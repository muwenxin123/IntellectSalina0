#include "ImageDetailDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QMessageBox>

ImageDetailDialog::ImageDetailDialog(QWidget *parent)
	: QDialog(parent)
	, m_totalBoxes(0)
{
	setWindowTitle(tr2("图片详情"));;
	resize(1000, 700);
	setupUI();
}

ImageDetailDialog::~ImageDetailDialog()
{
}

// ⭐ 解析类别名：从 "coco.pt:car" 提取 "car"
QString ImageDetailDialog::parseClassName(const std::string& label)
{
	QString labelStr = QString::fromStdString(label);
	if (labelStr.contains(":")) {
		QStringList parts = labelStr.split(":");
		return parts.size() > 1 ? parts[1].trimmed() : parts[0].trimmed();
	}
	return labelStr.trimmed();
}

void ImageDetailDialog::setupUI()
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);

	// 主分割器
	m_mainSplitter = new QSplitter(Qt::Horizontal);

	// 左侧：图片
	QWidget* leftWidget = new QWidget();
	QVBoxLayout* leftLayout = new QVBoxLayout(leftWidget);
	leftLayout->setContentsMargins(0, 0, 0, 0);

	m_imageLabel = new QLabel();
	m_imageLabel->setAlignment(Qt::AlignCenter);
	m_imageLabel->setStyleSheet(
		"QLabel {"
		"   background-color: #2b2b2b;"
		"   border: 1px solid #444;"
		"}"
	);
	m_imageLabel->setMinimumSize(400, 400);
	m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	leftLayout->addWidget(m_imageLabel);
	m_mainSplitter->addWidget(leftWidget);

	// 右侧：详细信息
	QWidget* rightWidget = new QWidget();
	QVBoxLayout* rightLayout = new QVBoxLayout(rightWidget);
	rightLayout->setSpacing(10);

	// 信息标签
	m_infoLabel = new QLabel();
	m_infoLabel->setStyleSheet(
		"QLabel {"
		"   color: #2196F3;"
		"   font-size: 12px;"
		"   background-color: rgba(33, 150, 243, 0.1);"
		"   padding: 8px;"
		"   border-radius: 4px;"
		"}"
	);
	rightLayout->addWidget(m_infoLabel);

	// 详情表格
	m_detailTable = new QTableWidget();
	m_detailTable->setColumnCount(4);
	QStringList headers; 
	headers << (tr2("类别")) << (tr2("数量")) << (tr2("平均置信度")) << (tr2("跟踪ID"));
	m_detailTable->setHorizontalHeaderLabels(headers);
	m_detailTable->horizontalHeader()->setStretchLastSection(true);
	m_detailTable->verticalHeader()->setVisible(false);
	m_detailTable->setEditTriggers(QTableWidget::NoEditTriggers);
	m_detailTable->setAlternatingRowColors(true);
	rightLayout->addWidget(m_detailTable, 1);

	// 按钮
	QHBoxLayout* btnLayout = new QHBoxLayout();
	btnLayout->addStretch();

	m_exportBtn = new QPushButton(tr2("导出图片"));
	m_annotateBtn = new QPushButton(tr2("标注信息"));
	m_closeBtn = new QPushButton(tr2("关闭"));

	m_exportBtn->setFixedSize(100, 30);
	m_annotateBtn->setFixedSize(100, 30);
	m_closeBtn->setFixedSize(100, 30);

	btnLayout->addWidget(m_exportBtn);
	btnLayout->addWidget(m_annotateBtn);
	btnLayout->addWidget(m_closeBtn);
	rightLayout->addLayout(btnLayout);

	m_mainSplitter->addWidget(rightWidget);
	m_mainSplitter->setStretchFactor(0, 6);
	m_mainSplitter->setStretchFactor(1, 4);

	mainLayout->addWidget(m_mainSplitter);

	// 信号连接
	connect(m_closeBtn, &QPushButton::clicked, this, &QDialog::accept);
	connect(m_exportBtn, &QPushButton::clicked, [this]() {
		if (m_currentImage.isNull()) return;
		QString fileName = QFileDialog::getSaveFileName(this, (tr2("保存图片")),
			QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".png",
			(tr2("图片 (*.png *.jpg)")));
		if (!fileName.isEmpty()) {
			m_currentImage.save(fileName);
			QMessageBox::information(this, (tr2("成功")), (tr2("图片已保存")));
		}
	});
	connect(m_annotateBtn, &QPushButton::clicked, [this]() {
		QMessageBox::information(this, (tr2("提示")), (tr2("标注功能开发中")));
	});
}

void ImageDetailDialog::setImage(const QImage& image)
{
	m_currentImage = image;
	if (image.isNull()) return;

	QSize labelSize = m_imageLabel->size();
	if (labelSize.width() > 0 && labelSize.height() > 0) {
		QPixmap pixmap = QPixmap::fromImage(image);
		pixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		m_imageLabel->setPixmap(pixmap);
	}
}

// ⭐ 核心：从 EventData 设置数据
void ImageDetailDialog::setEventData(const WS::EventData& eventData)
{
	// 保存检测框列表
	m_bboxes = QVector<WS::BBox>::fromStdVector(eventData.bboxes);

	// 保存元信息
	m_totalBoxes = eventData.totalBoxesInImage;
	m_videoId = QString::fromStdString(eventData.videoid);
	m_captureTime = eventData.time;

	// 计算统计详情
	calculateDetails();

	// 更新表格
	updateDetailTable();

	// 更新信息标签
	QString info = QString((tr2("时间: %1 | 视频流: %2 | 检测目标总数: %3")))
		.arg(m_captureTime.toString("hh:mm:ss"))
		.arg(m_videoId)
		.arg(m_totalBoxes);
	m_infoLabel->setText(info);
}

void ImageDetailDialog::calculateDetails()
{
	m_details.clear();
	QMap<QString, DetailItem> itemMap;

	// 遍历所有检测框进行统计
	for (const auto& bbox : m_bboxes) {
		// 从 label 解析类别名
		QString className = parseClassName(bbox.label);

		if (!itemMap.contains(className)) {
			DetailItem item;
			item.category = className;
			item.count = 0;
			item.avgConfidence = 0;
			itemMap[className] = item;
		}

		// 更新计数和置信度总和
		itemMap[className].count++;
		itemMap[className].avgConfidence += bbox.score;

		// 记录不重复的 trackId
		if (bbox.trackId > 0 && !itemMap[className].trackIds.contains(bbox.trackId)) {
			itemMap[className].trackIds.append(bbox.trackId);
		}
	}

	// 计算平均置信度并转换为列表
	for (auto it = itemMap.begin(); it != itemMap.end(); ++it) {
		DetailItem& item = it.value();
		if (item.count > 0) {
			item.avgConfidence /= item.count;
		}
		m_details.append(item);
	}
}

void ImageDetailDialog::updateDetailTable()
{
	m_detailTable->setRowCount(m_details.size());

	for (int i = 0; i < m_details.size(); i++) {
		const auto& item = m_details[i];

		// 类别
		QTableWidgetItem* categoryItem = new QTableWidgetItem(item.category);
		categoryItem->setTextAlignment(Qt::AlignCenter);
		m_detailTable->setItem(i, 0, categoryItem);

		// 数量
		QTableWidgetItem* countItem = new QTableWidgetItem(QString::number(item.count));
		countItem->setTextAlignment(Qt::AlignCenter);
		m_detailTable->setItem(i, 1, countItem);

		// 平均置信度
		QString confStr = QString("%1%").arg(int(item.avgConfidence * 100));
		QTableWidgetItem* confItem = new QTableWidgetItem(confStr);
		confItem->setTextAlignment(Qt::AlignCenter);
		m_detailTable->setItem(i, 2, confItem);

		// 跟踪ID
		QStringList idStrs;
		for (int id : item.trackIds) {
			idStrs << QString::number(id);
		}
		QTableWidgetItem* trackItem = new QTableWidgetItem(idStrs.join(", "));
		trackItem->setTextAlignment(Qt::AlignCenter);
		m_detailTable->setItem(i, 3, trackItem);
	}

	m_detailTable->resizeColumnsToContents();
}

void ImageDetailDialog::resizeEvent(QResizeEvent *event)
{
	QDialog::resizeEvent(event);

	if (!m_currentImage.isNull()) {
		QSize labelSize = m_imageLabel->size();
		if (labelSize.width() > 0 && labelSize.height() > 0) {
			QPixmap pixmap = QPixmap::fromImage(m_currentImage);
			pixmap = pixmap.scaled(labelSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			m_imageLabel->setPixmap(pixmap);
		}
	}
}