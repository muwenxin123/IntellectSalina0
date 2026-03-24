#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>

#include <QThread> 
#include <QImage>
#include  <QPushButton>
#include <QLabel>
#include <QShowEvent>
#include <QComboBox>
#include <map>
#include "videothread.h"

#define EDGE_MARGIN 8



namespace Ui {
	class CameraWidget;
}

class CameraWidget : public QWidget
{
	Q_OBJECT

public:
	explicit CameraWidget(int videoID, QWidget *parent = nullptr);
	~CameraWidget();



	void stopPlay();
	void startPlay(VideoInfoData videoInfo);
	void pause(bool state);

protected:

	void closeEvent(QCloseEvent *e);
	void showEvent(QShowEvent *event);

	void paintEvent(QPaintEvent *e);
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;


private:
	void setVideoInfo(VideoInfoData videoInfo);


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

	QImage       m_image;

	VideoThread *decoderThread;

	//VideoThread *decoderThread = nullptr;
	int m_videoID = 0;

	std::map<int, VideoThread*> m_videoThreadMap;


private:
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

	bool m_bDragging;       // 是否正在拖动
	bool m_bResizing;       // 是否正在调整大小
	int m_resizeMargin;     // 边缘检测宽度（像素）

	QPoint m_startPos;              // 鼠标按下时的全局位置
	QRect m_originalGeometry;       // 鼠标按下时的窗口几何信息
	ResizeDirection m_resizeDirection = None; // 当前调整方向

};


#endif // CAMERAWIDGET_H
