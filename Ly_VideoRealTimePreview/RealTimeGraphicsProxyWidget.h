#ifndef  REALTIMEGRAPHICSPROXYWIDGET_H
#define REALTIMEGRAPHICSPROXYWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsRotation>
#include <QGraphicsTextItem>
#include <QPropertyAnimation>
#include <QGraphicsProxyWidget>

class RealTimeGraphicsTextItem : public QGraphicsTextItem {
public:
	RealTimeGraphicsTextItem(const QString& text, QGraphicsItem* parent);
	virtual ~RealTimeGraphicsTextItem();

	void setBackgroundColor(const QColor& color);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)override;

private:
	QColor m_backgroundColor;
};


class RealTimeGraphicsProxyWidget : public QGraphicsProxyWidget {
    Q_OBJECT
public:
    explicit RealTimeGraphicsProxyWidget(const QString& text, QWidget *sourceWidget, QGraphicsScene* scence);
    virtual ~RealTimeGraphicsProxyWidget();

	// 设置代理窗口位置
	void setProxyViewPos(QPoint& pt);
	void setProxyViewPos(qreal ax, qreal ay);

	// 设置代理窗口大小
	void setProxyViewSize(QSize& size);
	void setProxyViewSize(QSizeF& size);

	// 开始拖拽
	void startDrag(int direction);

	// 停止拖拽
	void endDrag(QSize& recoverSize);

private:
	void updateTextPos();

private:
	int m_textWidth = 10;
	int m_textHeigth = 10;
	RealTimeGraphicsTextItem* m_pGraphicsTextItem = nullptr;
	QGraphicsScene* m_pGraphicsScene = nullptr;
	QGraphicsRotation* m_pRotation = nullptr;
	QPropertyAnimation* m_pPropertyAnimation = nullptr;
};

#endif // REALTIMEGRAPHICSPROXYWIDGET_H
