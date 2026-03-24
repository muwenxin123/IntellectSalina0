#include "RealTimeGraphicsView.h"

#include <QPainter>
#include<QStyleOption>
#include<QResizeEvent>
#include<QMouseEvent>
#include <chrono>
#include <QDebug>

RealTimeGraphicsView::RealTimeGraphicsView(QWidget *parent) :
	QGraphicsView(parent)
{
	setAttribute(Qt::WA_TranslucentBackground);
	setStyleSheet("background:transparent");
	setFrameShape(QFrame::NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	//setViewportUpdateMode(QGraphicsView::NoViewportUpdate);
	setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	setAttribute(Qt::WA_PaintOnScreen);
	setDragMode(DragMode::NoDrag);
}

RealTimeGraphicsView::~RealTimeGraphicsView()
{
}

// ÊÂ¼þ¹ýÂËÆ÷
bool RealTimeGraphicsView::eventFilter(QObject *object, QEvent *event)
{
	return QGraphicsView::eventFilter(object, event);
}

void RealTimeGraphicsView::showEvent(QShowEvent *event)
{
	QGraphicsView::showEvent(event);
}

void RealTimeGraphicsView::drawBackground(QPainter *painter, const QRectF &rect)
{
}

void RealTimeGraphicsView::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();
	RealTimeGraphicsScene* pScene = GetScene();
	if (nullptr != pScene)
		pScene->AdjustScenceSize(size);
	QGraphicsView::resizeEvent(event);

	QTimer::singleShot(1, [&]() {
		centerOn(size.width()/2, size.height()/2);
	});
}

void RealTimeGraphicsView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && m_dragFlag == false)
	{
		m_dragFlag = true;
		m_dragStartPos = event->pos();
		setCursor(Qt::ClosedHandCursor);

		RealTimeGraphicsScene* pScene = GetScene();
		if (nullptr != pScene)
			pScene->StartDrag();

		event->accept();
		return;
	}
	QGraphicsView::mousePressEvent(event);
}

void RealTimeGraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && m_dragFlag == true)
	{
		m_dragFlag = false;
		m_dragStartPos = QPoint();

		RealTimeGraphicsScene* pScene = GetScene();
		if (nullptr != pScene)
			pScene->StopDrag();

		setCursor(Qt::ArrowCursor);
		event->accept();
		return;
	}
	QGraphicsView::mouseReleaseEvent(event);
}

void RealTimeGraphicsView::mouseMoveEvent(QMouseEvent *event)
{
	if (m_dragFlag)
	{
		QPointF delta = mapToScene(m_dragStartPos) - mapToScene(event->pos());
		QPointF center = mapToScene(viewport()->rect().center());
		QPointF newCenter = center + delta;

		QRectF sceneRectTmp = this->sceneRect();
		newCenter.setX(qMax(sceneRectTmp.left() + viewport()->width() / 2, qMin(newCenter.x(), sceneRectTmp.right() - viewport()->width() / 2)));
		newCenter.setY(qMax(sceneRectTmp.top() + viewport()->height() / 2, qMin(newCenter.y(), sceneRectTmp.bottom() - viewport()->height() / 2)));
		
		setUpdatesEnabled(false);
		centerOn(newCenter);
		setUpdatesEnabled(true);

		m_dragStartPos = event->pos();
		event->accept();

		auto scene = GetScene();
		scene->ResetViewVisible();
		return;
	}

	QGraphicsView::mouseMoveEvent(event);
}

RealTimeGraphicsScene* RealTimeGraphicsView::GetScene()
{
	return dynamic_cast<RealTimeGraphicsScene*>(scene());
}
