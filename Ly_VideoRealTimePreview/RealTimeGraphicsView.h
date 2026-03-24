#ifndef  REALTIMEGRAPHICSVIEW_H
#define REALTIMEGRAPHICSVIEW_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGraphicsView>

#include "Ly_VideoRealTimePreview.h"
#include "RealTimeGraphicsScene.h"

namespace Ui {
class Ly_VideoRealTimePreviewWidget;
}

namespace VideoRealTime
{
	enum EViewLocation;
};

class RealTimeGraphicsScene;
class RealTimeGraphicsView : public QGraphicsView {
    Q_OBJECT
public:

    explicit RealTimeGraphicsView(QWidget *parent = nullptr);
    virtual ~RealTimeGraphicsView();

private:
	// 事件过滤器
	bool eventFilter(QObject *object, QEvent *event)override;

	// 显示
	void showEvent(QShowEvent *event)override;

	//绘制背景
	void drawBackground(QPainter *painter, const QRectF &rect)override;

	// item大小重置
	void resizeEvent(QResizeEvent *event);

	// 拖拽
	void mousePressEvent(QMouseEvent *event)override;

	// 抬起
	void mouseReleaseEvent(QMouseEvent *event)override;

	// 移动
	void mouseMoveEvent(QMouseEvent *event)override;

	// 获取Scene
	RealTimeGraphicsScene* GetScene();

private:
   Ui::Ly_VideoRealTimePreviewWidget *ui;
   bool m_dragFlag = false;
   QPoint m_dragStartPos;
};

#endif // REALTIMEGRAPHICSVIEW_H
