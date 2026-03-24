#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QImage>
#include <QMutex>
#include <map>
#include "videothread.h"
#include "gdd/wsProtocol.h"

#define EDGE_MARGIN 8


struct BoxInfo {
	QRectF rect;                    // 屏幕坐标的矩形区域
	WS::BBox bbox;                  // 直接存储原始的 BBox 对象
	QString className;              // 类别名称（从 label 解析）
	float score;                    // 置信度 (bbox.score)
	int trackId;                    // 跟踪ID (bbox.trackId)
	QString videoId;                // 视频ID (从 metadata.videoid)
	qint64 timestamp;               // 时间戳
};

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
	void setData(const WS::DetectionData& data);
	
	inline int getVideoID() const { return m_videoID; }
signals:
	void boxClicked(int videoId, const BoxInfo& boxInfo);  // 框被点击的信号

protected:
	void closeEvent(QCloseEvent *e) override;
	void paintEvent(QPaintEvent *e) override;
	// 注意：showEvent 如果不需要实现，就完全删除声明
	void mousePressEvent(QMouseEvent *event)override;

	private slots:
	// 只保留实际使用的槽函数
	void getOneFrame(QImage img);

	// 删除所有不存在的槽函数声明：
	// void on_status_click();
	// void onComboBoxIndexChanged(int index);
	// void closeWidget();
	// void isRecord();

private:
	void setVideoInfo(VideoInfoData videoInfo);
	void paintRect(QPainter& p);
	void updateBoxesFromData();  // 更新框信息列表
	const BoxInfo* findBoxAtPosition(const QPoint& pos) const;  // 查找点击位置所在的框
	void mouseMoveEvent(QMouseEvent *event);

private:
	Ui::CameraWidget *ui;
	int m_videoID = 0;
	std::map<int, VideoThread*> m_videoThreadMap;
	WS::DetectionData m_data;
	QMutex m_dataMutex;


	const BoxInfo* m_hoveredBox;      //悬停的框（注意要用指针）

	QVector<BoxInfo> m_currentBoxes;  // 存储当前帧的所有框信息
};

#endif // CAMERAWIDGET_H