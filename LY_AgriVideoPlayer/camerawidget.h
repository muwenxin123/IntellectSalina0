#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QScreen>
#include <QThread> 
#include <QImage>
#include  <QPushButton>
#include <QLabel>
#include <QShowEvent>
#include <QComboBox>
#include <map>
#include "videothread.h"
#include "gdd/wsProtocol.h"  // 包含结构体定义
#define EDGE_MARGIN 8



namespace Ui {
class CameraWidget;
}

class CameraWidget : public QWidget
{
    Q_OBJECT
public slots:
	void setDataSafely(const WS::DetectionData& data);
public:
    explicit CameraWidget(int videoID , QWidget *parent = nullptr);
    ~CameraWidget();

	void stopPlay();
	void startPlay(VideoInfoData videoInfo);
	void pause(bool state);

	void setModelType(int index);

	void setData(const WS::DetectionData& data);

	inline int getVideoID() {
		return m_videoID;
	}
protected:

	void closeEvent(QCloseEvent *e);
	void showEvent(QShowEvent *event);
	void paintRect(QPainter& p);
	void paintEvent(QPaintEvent *e);


private:
	void setVideoInfo(VideoInfoData videoInfo);

private:
	QMutex m_dataMutex;
signals:
	void dealingMessage();

private slots:

	void getOneFrame(QImage img);
	void onComboBoxIndexChanged(int index);
	void closeWidget();
	void on_status_click();
	void isRecord();

private:
    Ui::CameraWidget *ui;
	//VideoThread *decoderThread = nullptr;
	int m_videoID = 0;

	std::map<int, VideoThread*> m_videoThreadMap;

	enum ResizeDirection {
		None,
		TopLeft, Top, TopRight,
		Left, Right,
		BottomLeft, Bottom, BottomRight
	};

	// 判断是否在可调整大小的区域
	bool isInResizeArea(const QPoint &pos);

	// 根据位置设置对应的光标
	void setResizeCursor(const QPoint &pos);

	WS::DetectionData m_data;
};


#endif // CAMERAWIDGET_H
