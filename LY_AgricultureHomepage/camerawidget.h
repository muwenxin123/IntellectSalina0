#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include "ffmpeguntil.h"
#include <QThread>
#include <QImage>
#include  <QPushButton>
#include <QVector>
#include <opencv2/opencv.hpp>
#include "videodecoderthread.h"
#include <QTimer>

#define EDGE_MARGIN 8

namespace Ui {
class CameraWidget;
}

class CameraWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CameraWidget(QWidget *parent = nullptr);
    ~CameraWidget();

	void startPlay( QString playUrl);
	void stopAllPLayer();
	void stopPlay();
	void startHarmManagerPlay(QString playUrl);

	QImage       m_image;

protected:
	void paintEvent(QPaintEvent *e);
	void closeEvent(QCloseEvent *e);


signals:
	void dealingMessage();
private slots:
	void getOneFrame(QImage img);

	

private:
    Ui::CameraWidget *ui;

	FFmpegUntil  *ffmpengUntil = nullptr;
	QThread     *m_playThread = nullptr;
	QTimer m_timer;
	QString m_currentUrl = "";
	VideoDecoderThread *decoderThread = nullptr;
	
	std::vector<VideoDecoderThread *> m_decoderThread;

};

#endif // CAMERAWIDGET_H
