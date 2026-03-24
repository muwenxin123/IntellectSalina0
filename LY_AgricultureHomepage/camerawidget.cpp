#include "camerawidget.h"
#include "ui_camerawidget.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>
#include <QBitmap>
#include <QPixmap>
#include <QCoreApplication>
#include "LY_AgricultureHomepage.h"


#pragma execution_character_set("utf-8")
CameraWidget::CameraWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CameraWidget)
	
{

    ui->setupUi(this);
	//this->setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	//this->setAttribute(Qt::WA_TranslucentBackground, true);//设置窗口背景透明

	setMouseTracking(true);

	//connect(&m_timer, &QTimer::timeout, this, [this]() {

	//	startPlay(m_currentUrl);
	//});
	//m_timer.start(3000); // 定时器每隔1000毫秒触发一次timeout信号
	
}
void CameraWidget::getOneFrame(QImage img)
{
	//qDebug() << "main---" << QThread::currentThread();
	//mat_image = matImage;
	m_image = img;
	
	this->update();
}
void CameraWidget::startHarmManagerPlay(QString playUrl)
{

	VideoDecoderThread *decoderThread1 = new VideoDecoderThread();
	decoderThread1->setCheckInterval(10); // 每10秒检测一次
	decoderThread1->setRetryPolicy(5, 3); // 最多重试5次，间隔3秒
	decoderThread1->switchStream(playUrl);
	connect(decoderThread1, SIGNAL(frameReady(QImage)), this, SLOT(getOneFrame(QImage)));
	decoderThread1->start();

	m_decoderThread.push_back(decoderThread1);
}
void CameraWidget::startPlay( QString playUrl)
{
	m_currentUrl = playUrl;
	if (decoderThread == nullptr)
	{
		decoderThread = new VideoDecoderThread();
	}
	decoderThread->setCheckInterval(10); // 每10秒检测一次
	decoderThread->setRetryPolicy(5, 3); // 最多重试5次，间隔3秒
	decoderThread->switchStream(playUrl);
	connect(decoderThread, SIGNAL(frameReady(QImage)), this, SLOT(getOneFrame(QImage)));
	decoderThread->start();

	//connect(decoderThread, &VideoDecoderThread::errorOccurred, this, [=](QString & messgae) {
	//	qDebug() << "play error:" << messgae;
	//});
}

void CameraWidget::closeEvent(QCloseEvent *event)
{
	this->stopPlay();
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


void CameraWidget::stopAllPLayer()
{



}
void CameraWidget::stopPlay()
{
	for (auto &player : m_decoderThread)
	{
		if (player) {
			player->stop();
		}
	}
}
CameraWidget::~CameraWidget()
{
	stopPlay();


    delete ui;
}
