#include "camerawidget.h"
#include "ui_camerawidget.h"
#include <QPainter>
#include <QDebug>
#include <QCloseEvent>
#include "LY_AgriVideoPlayer.h"

#pragma execution_character_set("utf-8")

CameraWidget::CameraWidget(int videoID, QWidget *parent)
	: QWidget(parent)
	, m_videoID(videoID)
	, ui(new Ui::CameraWidget)
	, m_hoveredBox(nullptr)
{
	ui->setupUi(this);

	// 设置窗口属性
	this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	this->setAttribute(Qt::WA_TranslucentBackground, true);
	this->setStyleSheet(QString("CameraWidget{border-image: none;"
		"border-image:url(\"../images/1k/black/ResourcePool/bg_notitle.png\");"
		"border:none}"));

	setMouseTracking(true);
	setFixedHeight(800);
	// 如果是gdd，不启用此视频
	if (m_videoID != 110505)
	{
		for (const auto &infoData : VideoInfoVec)
		{
			if (infoData.id == m_videoID)
			{
				VideoInfoData info;
				info.url = infoData.url;
				info.id = infoData.id;
				setVideoInfo(info);
				break;
			}
		}
	}

	qDebug() << "CameraWidget创建完成，ID:" << m_videoID;
}

CameraWidget::~CameraWidget()
{
	stopPlay();

	// 停止所有线程
	for (auto& pair : m_videoThreadMap) {
		VideoThread *thread = pair.second;
		if (thread != nullptr) {
			thread->stop();
			thread->wait(1000);
			delete thread;
		}
	}
	m_videoThreadMap.clear();

	delete ui;
}

void CameraWidget::setVideoInfo(VideoInfoData videoInfo)
{
	VideoThread *decoderThread = new VideoThread(this);
	decoderThread->setUavID(QString::number(m_videoID));

	connect(decoderThread, &VideoThread::sendOneFrame,
		this, &CameraWidget::getOneFrame, Qt::QueuedConnection);

	m_videoThreadMap[videoInfo.id] = decoderThread;
	decoderThread->startPlay(videoInfo);
}

void CameraWidget::getOneFrame(QImage img)
{
	// 可以在这里处理接收到的帧，如果需要的话
	// m_image = img;
	// update();
}

void CameraWidget::setData(const WS::DetectionData& data)
{
	QMutexLocker locker(&m_dataMutex);

	// 深拷贝数据
	WS::DetectionData newData;
	newData.image = data.image.copy();
	newData.metadata = data.metadata;
	newData.bboxes = data.bboxes;
	newData.polygons = data.polygons;

	// 在GUI线程中更新
	QMetaObject::invokeMethod(this, [this, newData]() {
		{
			QMutexLocker locker(&m_dataMutex);
			m_data = newData;
			// 更新框信息列表
			updateBoxesFromData();
		}
		update();
	}, Qt::QueuedConnection);
}

void CameraWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	if (m_data.image.isNull()) {
		return;
	}

	QPainter painter(this);

	// 设置label的大小与窗口相同
	ui->label->setGeometry(0, 0, width(), height());

	// 缩放图像以适应label
	QImage img = m_data.image.scaled(ui->label->size(),
		Qt::IgnoreAspectRatio,
		Qt::SmoothTransformation);

	// 绘制图像
	painter.drawImage(ui->label->geometry().topLeft(), img);

	// 绘制检测框
	paintRect(painter);
}

void CameraWidget::paintRect(QPainter& p)
{
	if (m_data.bboxes.empty()) return;

	// 计算缩放比例
	int baseWidth = m_data.metadata.width > 0 ? m_data.metadata.width : m_data.image.width();
	int baseHeight = m_data.metadata.height > 0 ? m_data.metadata.height : m_data.image.height();

	if (baseWidth == 0 || baseHeight == 0) return;

	double widthScale = width() * 1.0 / baseWidth;
	double heightScale = height() * 1.0 / baseHeight;

	p.save();

	for (const auto& bbox : m_data.bboxes) {
		// 设置颜色
		QColor color(bbox.color[0], bbox.color[1], bbox.color[2], 255);
		QPen pen(color);
		//pen.setWidth(3);
		// ⭐ 判断是否是悬停的框（通过比较指针或trackId）
		bool isHovered = false;
		for (const auto& boxInfo : m_currentBoxes) {
			if (&boxInfo.bbox == &bbox || boxInfo.trackId == bbox.trackId) {
				if (&boxInfo == m_hoveredBox) {
					isHovered = true;
				}
				break;
			}
		}

		if (isHovered) {
			pen.setWidth(5);
			pen.setColor(Qt::yellow);
		}
		else {
			pen.setWidth(3);
		}

		p.setPen(pen);

		// 计算坐标
		const auto& box = bbox.bbox;
		double x = box[0] * baseWidth * widthScale;
		double y = box[1] * baseHeight * heightScale;
		double w = box[2] * baseWidth * widthScale;
		double h = box[3] * baseHeight * heightScale;

		// 绘制矩形框
		p.drawRect(QRectF(x, y, w, h));
	}

	p.restore();
}

void CameraWidget::startPlay(VideoInfoData videoInfo)
{
	auto it = m_videoThreadMap.find(videoInfo.id);
	if (it != m_videoThreadMap.end()) {
		it->second->startPlay(videoInfo);
	}
}

void CameraWidget::stopPlay()
{
	for (auto& pair : m_videoThreadMap) {
		if (pair.second) {
			pair.second->stop();
		}
	}
}

void CameraWidget::closeEvent(QCloseEvent *event)
{
	stopPlay();

	// 停止所有线程
	for (auto& pair : m_videoThreadMap) {
		VideoThread *thread = pair.second;
		if (thread != nullptr) {
			thread->stop();
			thread->wait(1000);
			delete thread;
		}
	}
	m_videoThreadMap.clear();

	event->accept();
}

//以下函数都被删除了
// void CameraWidget::on_status_click() { ... }
// void CameraWidget::onComboBoxIndexChanged(int) { ... }
// void CameraWidget::closeWidget() { ... }
// void CameraWidget::isRecord() { ... }
// void CameraWidget::showEvent(QShowEvent*) { ... }
// void CameraWidget::setDataSafely(...) { ... }

void CameraWidget::mouseMoveEvent(QMouseEvent *event)
{
	// 只保留悬停检测功能
	const BoxInfo* box = nullptr;
	for (int i = m_currentBoxes.size() - 1; i >= 0; i--) {
		if (m_currentBoxes[i].rect.contains(event->pos())) {
			box = &m_currentBoxes[i];
			break;
		}
	}

	if (box != m_hoveredBox) {
		m_hoveredBox = box;
		update();
	}

	QWidget::mouseMoveEvent(event);
}

const BoxInfo* CameraWidget::findBoxAtPosition(const QPoint& pos) const
{
	// 从后往前遍历，这样上面的框会先被检测到（如果有重叠）
	for (int i = m_currentBoxes.size() - 1; i >= 0; i--) {
		if (m_currentBoxes[i].rect.contains(pos)) {
			return &m_currentBoxes[i];
		}
	}
	return nullptr;
}


void CameraWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		const BoxInfo* box = findBoxAtPosition(event->pos());
		if (box) {
			qDebug() << "=================================";
			qDebug() << "视频" << m_videoID << "点击了检测框";
			qDebug() << "  类别:" << box->className;
			qDebug() << "  置信度:" << box->score;
			qDebug() << "  跟踪ID:" << box->trackId;
			qDebug() << "  标签:" << QString::fromStdString(box->bbox.label);
			qDebug() << "  组ID:" << QString::fromStdString(box->bbox.groupId);
			qDebug() << "  原始坐标:" << box->bbox.bbox[0] << box->bbox.bbox[1]
				<< box->bbox.bbox[2] << box->bbox.bbox[3];
			qDebug() << "=================================";

			// 发送信号
			emit boxClicked(m_videoID, *box);
			event->accept();
			return;
		}
	}

	QWidget::mousePressEvent(event);
}

void CameraWidget::updateBoxesFromData()
{
	m_currentBoxes.clear();

	if (m_data.image.isNull() || m_data.bboxes.empty()) return;

	// 计算缩放比例
	int baseWidth = m_data.metadata.width;
	int baseHeight = m_data.metadata.height;

	if (baseWidth == 0 || baseHeight == 0) return;

	double widthScale = width() * 1.0 / baseWidth; //宽度比例
	double heightScale = height() * 1.0 / baseHeight;

	// 遍历所有检测框
	for (const auto& bbox : m_data.bboxes) {
		BoxInfo info;

		// 保存原始 BBox 对象
		info.bbox = bbox;
		info.score = bbox.score;
		info.trackId = bbox.trackId;

		// 解析类别名称（从 label 中提取）
		QString labelStr = QString::fromStdString(bbox.label);
		if (labelStr.contains(":")) {
			QStringList parts = labelStr.split(":");
			info.className = parts.size() > 1 ? parts[1] : parts[0];
		}
		else {
			info.className = labelStr;
		}

		info.videoId = QString::fromStdString(m_data.metadata.videoid);
		info.timestamp = 0;  // 如果没有时间戳，设为0

							 // 计算屏幕坐标
		const auto& box = bbox.bbox;
		double x = box[0] * widthScale;
		double y = box[1] * heightScale;
		double w = box[2] * widthScale;
		double h = box[3] * heightScale;

		info.rect = QRectF(x, y, w, h);

		m_currentBoxes.append(info);
	}
}