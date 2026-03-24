#ifndef LY_VIDEOREALTIMEPREVIEWWIDGET_H
#define LY_VIDEOREALTIMEPREVIEWWIDGET_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include "Ly_VideoRealTimePreview.h"

namespace Ui {
class Ly_VideoRealTimePreviewWidget;
}

class MapEventWatcher : public QWidget
{
public:
	friend class Ly_VideoRealTimePreviewWidget;
	MapEventWatcher(Ly_VideoRealTimePreviewWidget* pWgt, QWidget* pParent);
	~MapEventWatcher();

	// 事件过滤器
	bool eventFilter(QObject *object, QEvent *event)override;

private:
	Ly_VideoRealTimePreviewWidget* m_pWgt;
};


class VedioDetailShowWgt;
class Ly_VideoRealTimeContainterWgt;
class Ly_VideoRealTimePreviewWidget : public QWidget
{
    Q_OBJECT
public:
	friend class MapEventWatcher;
	explicit Ly_VideoRealTimePreviewWidget(QWidget *parent = nullptr);
    ~Ly_VideoRealTimePreviewWidget();

	// 设置父窗口
	void setMapWidget(QWidget* pParent);

	// 设置窗口显影依赖(巨恶心，地图窗口隐藏后不发送窗口隐藏消息，所以需要找一个依赖窗口)
	void  setVisiableDependsWgt(QWidget* pParent);

	// 配置发生改变
	void ConfigureChanged();

	// 初始化
	void Init();

	// 设置视图位置
	void setViewLocation(const VideoRealTime::EViewLocation& location);

	// 重设大小
	void ResizeIt(QSize& size);

	// 移动
	void MoveIt(QRect& rect);

	// 焦距改变
	void FocalZoomChanged(int imageZoom);

	// 姿态能改变
	void OberverCameraAttributeChanged(long long identity,
		double yawValue, double pitchValue, double rollValue);
	
	// 调整详细试图的位置
	void AdjustDetaiView();

public slots:
	// 地图更新
	void UpdateMap();

private:
	// 绘制
	void paintEvent(QPaintEvent *event)override;

	// 事件过滤器
	bool eventFilter(QObject *object, QEvent *event)override;

	// 显示
	void showEvent(QShowEvent *event)override;

	// 获取视图大小
	QSize GetViewSize(const QSize& parentSize);

	// 获取视图位置
	QPoint GetViewPos(const QRect& rect);

	// 获取视图相对位置
	QPoint GetViewRelatePos(const QRect& rect);

	// 获取详细显示视图大小
	QSize GetDetailViewSize(const QSize& parentSize);

	// 获取详细显示视图位置
	QPoint GetDetailViewPos(const QRect& rect);

	// 获取详细显示视图相对位置
	QPoint GetDetailViewRelatePos(const QRect& rect);

	// 尝试显示
	void CheckShow();

	// 根据父窗口重新设置
	void ResetByParent();

private slots:
	void onVedioCountChanged();

	void onDeleteLater(QObject* pObejct);

	void onEntityDoubleClicked(long long identity);

	// 定时器事件到达
	void onTimeArrive();

private:
    Ui::Ly_VideoRealTimePreviewWidget *ui;
	Ly_VideoRealTimeContainterWgt* m_pContainer = nullptr;

	QWidget* m_pParentWidget = nullptr;											// 2维 3维视图
	MapEventWatcher* m_wathcer = nullptr;										// 观察2三维试图缩放
	QWidget* m_pDependentView = nullptr;										// 框架主视图

	bool m_showFlag = false;													// 配置显影标识
	bool m_dependentViewShowFlag = false;										// 框架视图显示表示
	bool m_isMiniSize = false;													// 是否最小化
	VideoRealTime::EViewLocation m_locationEnum = VideoRealTime::ViewBottom;	// 窗口位置
	double m_detailOccupy = 0.25;
	QTimer m_timer;																// 定时器轮询更新
	
	VedioDetailShowWgt* m_pVedioDetailShow = nullptr;							// 代理显示试图
};

#endif // LY_VIDEOREALTIMEPREVIEWWIDGET_H
