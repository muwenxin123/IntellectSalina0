#include "camerawidget.h"
#include "ui_camerawidget.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QBitmap>
#include <QPixmap>
#include <QCoreApplication>
#include <QTextCodec>
#include "LY_AgriVideoPlayer.h"


#pragma execution_character_set("utf-8")
CameraWidget::CameraWidget(int videoID, QWidget *parent)
	:QWidget(parent)
	, m_videoID(videoID)
	, m_bDragging(false)
	, m_bResizing(false)
	, m_resizeMargin(8)
	, ui(new Ui::CameraWidget)
{


	ui->setupUi(this);
	this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	//this->setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
	this->setStyleSheet("CameraWidget{background-color: #162130};");
	//this->setStyleSheet(QString("CameraWidget{border-image: none;"
	//	  "border-image:url(\"../images/1k/black/ResourcePool/bg_notitle.png\");"
	//	"border:none}"));

	setMouseTracking(true);

	ui->label_2->setStyleSheet("QLabel{ border-image: url(:/images/1k/black/QPushButton/normal.png) 3 8 5 8px; border-width: 3 8 5 8px; }");
	ui->label_3->setStyleSheet("QLabel{ border-image: url(:/images/1k/black/QPushButton/normal.png) 3 8 5 8px; border-width: 3 8 5 8px; }");
	ui->label_4->setStyleSheet("QLabel{ border-image: url(:/images/1k/black/QPushButton/normal.png) 3 8 5 8px; border-width: 3 8 5 8px; }");
	ui->label_5->setStyleSheet("QLabel{ border-image: url(:/images/1k/black/QPushButton/normal.png) 3 8 5 8px; border-width: 3 8 5 8px; }");
	ui->pushButton->setMinimumWidth(30);
	ui->pushButton->setCheckable(false);
	ui->pushButton->setText(QString::fromUtf8("否"));
	connect(ui->pushButton, &QPushButton::clicked, this, &CameraWidget::isRecord);

	ui->closeBtn->setText(QString::fromUtf8("关闭"));
	connect(ui->modeCombox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIndexChanged(int)));
	connect(ui->closeBtn, &QPushButton::clicked, this, &CameraWidget::closeWidget);
	connect(ui->statusBtn, &QPushButton::clicked, this, &CameraWidget::on_status_click);
	ui->statusBtn->setCheckable(false);

	ui->statusBtn->setText(QString::fromUtf8("关闭"));
}
void CameraWidget::isRecord()
{

	QPushButton *button = qobject_cast<QPushButton*>(sender());
	if (button == nullptr)
	{
		return;
	}
	QWidget *widget = qobject_cast<QWidget*>(button->parent());
	std::map<int, CameraWidget *> cameraWidgetMap = LY_AgriVideoPlayer::Get().GetVideoPlayerMap();
	if (widget == nullptr || cameraWidgetMap.empty())
	{
		return;
	}
	int videoID = 0;
	for (const auto& pair : cameraWidgetMap) {
		if (pair.second == widget) {
			videoID = pair.first;
			break;
		}
	}
	auto it = std::find_if(VideoInfoVec.begin(), VideoInfoVec.end(),
		[&videoID](const VideoInfoData& data) {
		return data.id == videoID;
	});

	if (it != VideoInfoVec.end()) {
		it->isOpen = ui->statusBtn->isCheckable();
	}
	VideoThread *temp_thread = nullptr;
	auto ptr = m_videoThreadMap.find(videoID);
	if (ptr != m_videoThreadMap.end()) {
		temp_thread = ptr->second;
	}

	if (ui->pushButton->isCheckable())
	{
		ui->pushButton->setCheckable(false);
		ui->pushButton->setText(QString::fromUtf8("否"));
		if (temp_thread)
		{
			temp_thread->stopRecording(videoID);
		}

	}
	else
	{
		ui->pushButton->setCheckable(true);
		ui->pushButton->setText(QString::fromUtf8("是"));
		if (temp_thread)
		{
			temp_thread->setSegmentDuration(30);
			temp_thread->startRecording(videoID);
		}
	}
}
void CameraWidget::on_status_click()
{

	QPushButton *button = qobject_cast<QPushButton*>(sender());
	if (button == nullptr)
	{
		return;
	}
	QWidget *widget = qobject_cast<QWidget*>(button->parent());
	std::map<int, CameraWidget *> cameraWidgetMap = LY_AgriVideoPlayer::Get().GetVideoPlayerMap();
	if (widget == nullptr || cameraWidgetMap.empty())
	{
		return;
	}

	if (ui->statusBtn->isCheckable())
	{
		ui->statusBtn->setCheckable(false);
		ui->statusBtn->setText(QString::fromUtf8("关闭"));
	}
	else
	{
		ui->statusBtn->setCheckable(true);
		ui->statusBtn->setText(QString::fromUtf8("打开"));
	}

	int videoID = 0;
	for (const auto& pair : cameraWidgetMap) {
		if (pair.second == widget) {
			videoID = pair.first;
			break;
		}
	}
	auto it = std::find_if(VideoInfoVec.begin(), VideoInfoVec.end(),
		[&videoID](const VideoInfoData& data) {
		return data.id == videoID;
	});

	if (it != VideoInfoVec.end()) {
		it->isOpen = ui->statusBtn->isCheckable();
	}

	auto ptr = m_videoThreadMap.find(videoID);
	if (ptr != m_videoThreadMap.end()) {
		VideoThread* util = ptr->second;
		if (util != nullptr)
		{
			util->setVideoInfo(*it);
		}
	}


}
void CameraWidget::closeWidget()
{
	this->close();
}
void CameraWidget::showEvent(QShowEvent *event)
{
	for (const auto &infoData : VideoInfoVec)
	{
		if (infoData.id == m_videoID)
		{
			VideoInfoData info;
			info.url = infoData.url;
			info.id = infoData.id;
			if (m_videoThreadMap.empty() == true)
			{
				setVideoInfo(info);
			}
			
			break;
		}
	}

	//setUpdatesEnabled(false);
}
void CameraWidget::onComboBoxIndexChanged(int index) {

	VideoInfoData videoInfo = ui->modeCombox->currentData().value<VideoInfoData>();

	QString videoID = videoInfo.id;

	if (VideoInfoVec.size() == 0)
	{
		return;
	}
	auto it = std::find_if(VideoInfoVec.begin(), VideoInfoVec.end(),
		[&videoID](const VideoInfoData& data) {
		return data.id == videoID;
	});

	if (it == VideoInfoVec.end()) {
		// 未找到满足条件的元素
		std::cout << "未找到匹配 videoID 的元素" << std::endl;
	}
	else {
		// 找到了满足条件的元素
		if (index >= 0)
		{
			it->model_type = index;
		}

		it->id = videoInfo.id;
		it->url = videoInfo.url;
		qDebug() << "当前选中索引:" << it->model_type << it->id << it->isOpen;

		auto ptr = m_videoThreadMap.find(it->id);
		if (ptr != m_videoThreadMap.end()) {
			VideoThread* util = ptr->second;
			if (util != nullptr)
			{
				util->setVideoInfo(*it);
			}
		}
	}
}

void CameraWidget::getOneFrame(QImage img)
{
	//qDebug() << "main---" << QThread::currentThread();
	m_image = img;
	this->update();
}

void CameraWidget::setVideoInfo(VideoInfoData videoInfo)
{
	ui->label_5->setText(QString::number(videoInfo.id));
	for (int i = 0; i < ModelVec.size(); i++)
	{
		QString str = ModelVec.at(i);
		ui->modeCombox->addItem(str, QVariant::fromValue(videoInfo));
	}

	VideoThread *decoderThread = new VideoThread(this);

	connect(decoderThread, &VideoThread::sendOneFrame,
		this, &CameraWidget::getOneFrame, Qt::QueuedConnection);

	m_videoThreadMap[videoInfo.id] = decoderThread;

	//// 开始解码
	//decoderThread->start();
	decoderThread->startPlay(videoInfo);


}

void CameraWidget::closeEvent(QCloseEvent *event)
{
	this->stopPlay();
	// 停止所有线程
	for (auto& pair : m_videoThreadMap) {
		VideoThread *thread = pair.second;
		if (thread != nullptr)
		{
			thread->stop();
			thread->wait(1000);
			delete thread;
			thread = nullptr;
		}
	}
	m_videoThreadMap.clear();  // 清空 map
}

void CameraWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (m_image.size().width() <= 0)
		return;

	QImage img = m_image.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	int x = ui->label->geometry().x();
	int y = ui->label->geometry().y();
	painter.drawImage(QPoint(x, y), img);

}


void CameraWidget::startPlay(VideoInfoData videoInfo)
{
	//if (decoderThread)
	//{
	//	decoderThread->startPlay(videoInfo);
	//}

}
void CameraWidget::pause(bool state)
{
	//if (decoderThread)
	//{
	//	decoderThread->pause(state);
	//}

}
void CameraWidget::stopPlay()
{
	//if (decoderThread)
	//{
	//	decoderThread->stop();
	//}
}
CameraWidget::~CameraWidget()
{
	stopPlay();
	// 停止所有线程
	for (auto& pair : m_videoThreadMap) {
		VideoThread *thread = pair.second;
		if (thread != nullptr)
		{
			thread->stop();
			thread->wait(1000);
			delete thread;
			thread = nullptr;
		}
	}
	m_videoThreadMap.clear();  // 清空 map

	delete ui;
}
void CameraWidget::mousePressEvent(QMouseEvent *event)
{
	//if (event->button() == Qt::LeftButton) {
	//	QApplication::setOverrideCursor(Qt::OpenHandCursor);
	//	m_dragPosition = event->globalPos() - frameGeometry().topLeft();
	//	m_dragging = true;
	//}

	if (event->button() == Qt::LeftButton) {
		m_startPos = event->globalPos();
		m_originalGeometry = geometry();

		// 检查是否在调整大小的区域
		if (isInResizeArea(event->pos())) {
			m_bResizing = true;
			setResizeCursor(event->pos());
		}
		// 否则认为是拖动
		else {
			m_bDragging = true;
			setCursor(Qt::SizeAllCursor);
		}
	}
	QWidget::mousePressEvent(event);
}

void CameraWidget::mouseMoveEvent(QMouseEvent *event)
{
	//if (m_dragging && (event->buttons() & Qt::LeftButton)) {
	//	QApplication::setOverrideCursor(Qt::ClosedHandCursor);
	//	move(event->globalPos() - m_dragPosition);
	//}
	if (m_bDragging) {
		// 计算位移并移动窗口
		QPoint delta = event->globalPos() - m_startPos;
		move(m_originalGeometry.topLeft() + delta);
	}
	else if (m_bResizing) {
		// 根据鼠标位置计算新的大小和位置
		QRect newGeometry = m_originalGeometry;
		QPoint delta = event->globalPos() - m_startPos;

		switch (m_resizeDirection) {
		case TopLeft:
			newGeometry.adjust(delta.x(), delta.y(), 0, 0);
			break;
		case Top:
			newGeometry.adjust(0, delta.y(), 0, 0);
			break;
		case TopRight:
			newGeometry.adjust(0, delta.y(), delta.x(), 0);
			break;
		case Left:
			newGeometry.adjust(delta.x(), 0, 0, 0);
			break;
		case Right:
			newGeometry.adjust(0, 0, delta.x(), 0);
			break;
		case BottomLeft:
			newGeometry.adjust(delta.x(), 0, 0, delta.y());
			break;
		case Bottom:
			newGeometry.adjust(0, 0, 0, delta.y());
			break;
		case BottomRight:
			newGeometry.adjust(0, 0, delta.x(), delta.y());
			break;
		default:
			break;
		}

		// 应用新的大小（自动约束最小大小）
		setGeometry(newGeometry.normalized());
	}
	else {
		// 没有按下时，更新光标显示
		setResizeCursor(event->pos());
	}

	QWidget::mouseMoveEvent(event);
}

void CameraWidget::mouseReleaseEvent(QMouseEvent *event)
{
	//Q_UNUSED(event);
	//QApplication::setOverrideCursor(Qt::ArrowCursor);
	//m_dragging = false;
	if (event->button() == Qt::LeftButton) {
		m_bDragging = false;
		m_bResizing = false;
		unsetCursor(); // 恢复默认光标
	}
	QWidget::mouseReleaseEvent(event);
}
bool CameraWidget::isInResizeArea(const QPoint &pos)
{
	const int w = width();
	const int h = height();

	// 检查各个区域
	if (pos.x() <= m_resizeMargin) {
		if (pos.y() <= m_resizeMargin) {
			m_resizeDirection = TopLeft;
			return true;
		}
		else if (pos.y() >= h - m_resizeMargin) {
			m_resizeDirection = BottomLeft;
			return true;
		}
		else {
			m_resizeDirection = Left;
			return true;
		}
	}
	else if (pos.x() >= w - m_resizeMargin) {
		if (pos.y() <= m_resizeMargin) {
			m_resizeDirection = TopRight;
			return true;
		}
		else if (pos.y() >= h - m_resizeMargin) {
			m_resizeDirection = BottomRight;
			return true;
		}
		else {
			m_resizeDirection = Right;
			return true;
		}
	}
	else if (pos.y() <= m_resizeMargin) {
		m_resizeDirection = Top;
		return true;
	}
	else if (pos.y() >= h - m_resizeMargin) {
		m_resizeDirection = Bottom;
		return true;
	}

	m_resizeDirection = None;
	return false;
}
void CameraWidget::setResizeCursor(const QPoint &pos)
{
	if (isInResizeArea(pos)) {
		switch (m_resizeDirection) {
		case TopLeft:
		case BottomRight:
			setCursor(Qt::SizeFDiagCursor);
			break;
		case TopRight:
		case BottomLeft:
			setCursor(Qt::SizeBDiagCursor);
			break;
		case Top:
		case Bottom:
			setCursor(Qt::SizeVerCursor);
			break;
		case Left:
		case Right:
			setCursor(Qt::SizeHorCursor);
			break;
		default:
			unsetCursor();
		}
	}
	else {
		// 在中心区域显示移动光标
		setCursor(Qt::SizeAllCursor);
	}
}