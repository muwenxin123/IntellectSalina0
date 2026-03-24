#include "RealTimeGraphicsProxyWidget.h"

#include <QList>
#include <QPainter>
#include <QApplication>
#include <QGraphicsTransform>

#include "RealTimeGraphicsScene.h"

RealTimeGraphicsTextItem::RealTimeGraphicsTextItem(const QString& text, QGraphicsItem* parent)
	: QGraphicsTextItem(text, parent), m_backgroundColor(0,0,0, 255)
{

}

RealTimeGraphicsTextItem::~RealTimeGraphicsTextItem()
{

}

void RealTimeGraphicsTextItem::setBackgroundColor(const QColor& color)
{
	m_backgroundColor = color;
	update();
}

void RealTimeGraphicsTextItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	painter->save();
	painter->setBrush(m_backgroundColor);
	painter->setPen(Qt::NoPen);
	painter->drawRect(boundingRect());
	painter->restore();
	QGraphicsTextItem::paint(painter, option, widget);
}

RealTimeGraphicsProxyWidget::RealTimeGraphicsProxyWidget(const QString& text,  QWidget *sourceWidget, QGraphicsScene* scence)
	: QGraphicsProxyWidget(), m_pGraphicsScene(scence)
{
	setWidget(sourceWidget);

	// 设置特效文字
	QFont font = qApp->font();
	font.setPixelSize(12);
	QFontMetrics fontMetrics(font);
	m_textWidth = fontMetrics.width(text, text.size());
	m_textHeigth = fontMetrics.height();
	m_pGraphicsTextItem = new RealTimeGraphicsTextItem(text, nullptr);
	m_pGraphicsTextItem->setFont(font);
	m_pGraphicsTextItem->setDefaultTextColor(QColor(240, 240, 240));
	m_pGraphicsTextItem->setBackgroundColor({ 0,0,0,100 });
	m_pGraphicsScene->addItem(m_pGraphicsTextItem);
	m_pGraphicsTextItem->hide();

	m_pRotation = new QGraphicsRotation(this);

	QList<QGraphicsTransform*> transformList;
	transformList << m_pRotation;
	setTransformations(transformList);
}

RealTimeGraphicsProxyWidget::~RealTimeGraphicsProxyWidget()
{
	if (nullptr != m_pPropertyAnimation)
	{
		m_pPropertyAnimation->stop();
		delete m_pPropertyAnimation;
	}

	if (nullptr != m_pGraphicsTextItem)
	{
		m_pGraphicsTextItem->hide();
		m_pGraphicsScene->removeItem(m_pGraphicsTextItem);
		delete m_pGraphicsTextItem;
		m_pGraphicsTextItem = nullptr;
	}
}

void RealTimeGraphicsProxyWidget::setProxyViewPos(QPoint& pt)
{
	setPos(pt);
	updateTextPos();
}

void RealTimeGraphicsProxyWidget::setProxyViewPos(qreal ax, qreal ay)
{
	setPos(ax, ay);
	updateTextPos();
}

void RealTimeGraphicsProxyWidget::setProxyViewSize(QSize& size)
{
	setMinimumSize(size);
	setMaximumSize(size);
	updateTextPos();
}

void RealTimeGraphicsProxyWidget::setProxyViewSize(QSizeF& size)
{
	setMinimumSize(size);
	setMaximumSize(size);
	updateTextPos();
}

void RealTimeGraphicsProxyWidget::startDrag(int direction)
{
	{
		// 大小变化
		QSizeF size = this->size();
		size.setWidth(size.width() - 40);
		size.setHeight(size.height() - 40);
		this->setProxyViewSize(size);
	}

	{
		// 动画设置
		if (nullptr != m_pPropertyAnimation)
		{
			m_pPropertyAnimation->stop();
			delete m_pPropertyAnimation;
		}

		int degreeValue = 0;
		switch (direction)
		{
		case RealTimeGraphicsScene::EDirectType::Vertical:
			m_pRotation->setAxis(Qt::XAxis);
			degreeValue = -30;
			break;
		case RealTimeGraphicsScene::EDirectType::Horizontal:
		default:
			m_pRotation->setAxis(Qt::YAxis);
			degreeValue = 30;
			break;
		}

		m_pRotation->setOrigin(QVector3D(size().width() / 2.0, size().height() / 2.0, 0));
		m_pPropertyAnimation = new QPropertyAnimation(m_pRotation, "angle");
		connect(m_pPropertyAnimation, &QPropertyAnimation::destroyed, [&]() {
			m_pPropertyAnimation = nullptr;
		});
		m_pPropertyAnimation->setDuration(300);
		m_pPropertyAnimation->setEndValue(degreeValue);
		m_pPropertyAnimation->start(QAbstractAnimation::DeleteWhenStopped);
		m_pGraphicsTextItem->show();
	}
}

void RealTimeGraphicsProxyWidget::endDrag(QSize& recoverSize)
{
	{
		this->setProxyViewSize(recoverSize);
	}
	{
		if (nullptr != m_pPropertyAnimation)
		{
			m_pPropertyAnimation->stop();
			delete m_pPropertyAnimation;
		}
		m_pRotation->setOrigin(QVector3D(size().width() / 2.0, size().height() / 2.0, 0));
		m_pPropertyAnimation = new QPropertyAnimation(m_pRotation, "angle");
		connect(m_pPropertyAnimation, &QPropertyAnimation::destroyed, [&]() {
			m_pPropertyAnimation = nullptr;
		});
		m_pPropertyAnimation->setDuration(300);
		m_pPropertyAnimation->setEndValue(0);
		m_pPropertyAnimation->start(QAbstractAnimation::DeleteWhenStopped);
		m_pGraphicsTextItem->hide();
	}
}

void RealTimeGraphicsProxyWidget::updateTextPos()
{
	auto rect = this->sceneBoundingRect();
	int posX = rect.left() + (rect.width() - font().weight()) / 2.0;
	m_pGraphicsTextItem->setPos(posX, rect.bottom());
}
