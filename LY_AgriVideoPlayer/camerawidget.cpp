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
    ,ui(new Ui::CameraWidget)
{


    ui->setupUi(this);
	this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	//this->setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明
	this->setStyleSheet("CameraWidget{background-color: #162130};");
	//this->setStyleSheet(QString("CameraWidget{border-image: none;"
	//	  "border-image:url(\"../images/1k/black/ResourcePool/bg_notitle.png\");"
	//	"border:none}"));

	//setMouseTracking(true);
	ui->groupBox->hide();
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

	//如果是gdd，不启用此视频
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
// 	auto it = std::find_if(VideoInfoVec.begin(), VideoInfoVec.end(),
// 		[this](const VideoInfoData& data) {
// 		return data.id == m_videoID;
// 	});
// 
// 	if (it != VideoInfoVec.end()) {
// 		it->isOpen = ui->statusBtn->isChecked();
// 	}
// 
// 	auto ptr = m_videoThreadMap.find(m_videoID);  
// 	if (ptr != m_videoThreadMap.end()) {
// 		VideoThread* util = ptr->second;
// 		if (util != nullptr)
// 		{
// 			util->setVideoInfo(*it);
// 		}
// 	}
}
void CameraWidget::closeWidget()
{
	this->close();
}
void CameraWidget::showEvent(QShowEvent *event)
{
	
	
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
		if (it != VideoInfoVec.end()) {
			it->isOpen = true;
		}
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


void CameraWidget::setModelType(int index)
{
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
		if (it != VideoInfoVec.end()) {
			it->isOpen = true;
		}
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
	//m_image = img;
	//this->update();
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
    //decoderThread->setUavID(QString::number(videoInfo.id));
    decoderThread->setUavID(QString::number(m_videoID));

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
bool CameraWidget::isInResizeArea(const QPoint &pos)
{
	return false;
}
void CameraWidget::setResizeCursor(const QPoint &pos)
{
	
}

void CameraWidget::setDataSafely(const WS::DetectionData& data)
{
	QMutexLocker locker(&m_dataMutex);

	// 复制数据（使用深拷贝或移动语义）
	WS::DetectionData newData;
	newData.image = data.image.copy();  // 关键：深拷贝图像

										// 复制其他字段
	newData.metadata = data.metadata;
	newData.bboxes = data.bboxes;
	newData.polygons = data.polygons;

	// 在GUI线程中更新
	QMetaObject::invokeMethod(this, [this, newData]() {
		{
			QMutexLocker locker(&m_dataMutex);
			m_data = std::move(newData);
		}
		update();  // 在主线程中调用update
	}, Qt::QueuedConnection);
}

void CameraWidget::setData(const WS::DetectionData& data)
{
	//m_data = data;
	qDebug() << "🔄 setData被调用";
	qDebug() << "  图像尺寸:" << m_data.image.size();
	qDebug() << "  元数据尺寸:" << m_data.metadata.width << "x" << m_data.metadata.height;
	qDebug() << "  bboxes数量:" << m_data.bboxes.size();
	qDebug() << "  polygons数量:" << m_data.polygons.size();

	//update();
	setDataSafely(data);
}

void CameraWidget::paintEvent(QPaintEvent *event)
{
	qDebug() << "paintEvent called in thread:" << QThread::currentThreadId();
	qDebug() << "Widget valid:" << isVisible() << "geometry:" << geometry();

	if (m_data.image.isNull()) {
		qDebug() << "Image is null!";
		return;
	}
	QPainter painter(this);
	QSize s1 = m_data.image.size();
	if (s1.width() <= 0)
		return;
	ui->label->setGeometry(0, 0, width(), height());
	//QSize s2 = ui->label->size();
	QImage img = m_data.image.scaled(ui->label->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

	int x = ui->label->geometry().x();
	int y = ui->label->geometry().y();
	painter.drawImage(QPoint(x, y), img);

// 
// 	if (m_data.image.isNull())
// 		return;
// 
// 	QPainter p(this);
// 	QRect rc = rect();
// 
// 	{
// 		p.save();
// 		QPen pen(QColor(0, 255, 255, 250));
// 		pen.setWidth(6);
// 		pen.setStyle(Qt::SolidLine);
// 		p.setPen(pen);
// 		QRect boderRect = rect().adjusted(1, 1, -1, -1);
// 		p.drawRect(boderRect);
// 		p.restore();
// 	}
// 
// 	if (!m_data.image.isNull())
// 	{
// 		QRect boderRect = rect().adjusted(2, 2, -2, -2);
// 		p.drawImage(boderRect, m_data.image);
// 	}
// 
// 	paintRect(p);
}

void CameraWidget::paintRect(QPainter& p)
{
	qDebug() << "=== paintRect ===";
	qDebug() << "bboxes:" << m_data.bboxes.size();
	qDebug() << "polygons:" << m_data.polygons.size();

	// 计算缩放比例
	int baseWidth = m_data.metadata.width > 0 ? m_data.metadata.width : m_data.image.width();
	int baseHeight = m_data.metadata.height > 0 ? m_data.metadata.height : m_data.image.height();

	if (baseWidth == 0 || baseHeight == 0) return;

	double widthScale = (width() - 4) * 1.0 / baseWidth;
	double heightScale = (height() - 4) * 1.0 / baseHeight;

	// 1. 绘制检测框
	if (!m_data.bboxes.empty()) {
		p.save();
		for (const auto& bbox : m_data.bboxes) {
			// 设置颜色
			QColor color(bbox.color[0], bbox.color[1], bbox.color[2], 255);
			QPen pen(color);
			pen.setWidth(3);
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

	qDebug() << "=== paintRect 完成 ===";
}