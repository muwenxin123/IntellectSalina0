#ifndef VIDEOCONTAINER_H
#define VIDEOCONTAINER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QList>

#include "EntityVedioData.h"
#include  "IVideoRealTimePreviewActivator.h"

namespace Ui {
class VedioContainer;
}

class VedioContainer : public QWidget
{
    Q_OBJECT
public:	
    explicit VedioContainer(EntityVedioDataSPtr pEntityData, QWidget *parent = nullptr);
    ~VedioContainer();

	long long GetIdentity();

signals:
	void updateContainter();
	
	void entityDoubleClicked(long long identity);

public slots :
	void onUpdateContainer();

private:
	// 绘制
	void paintEvent(QPaintEvent *event)override;

	// 单机
	void mousePressEvent(QMouseEvent *event)override;

	// 双击
	void mouseDoubleClickEvent(QMouseEvent *event)override;

	bool event(QEvent* event)override;

	// 显示事件
	void showEvent(QShowEvent* event);

	// 显示事件
	void hideEvent(QHideEvent* event);

	// 绘制敌方
	void paintTargets(std::vector<XygsZC_Mark_01>& targets, QPainter& p);

	// 绘制我方
	void paintFriends(std::vector<qnzkna::SensorVedio::FriendRectScope>& friends, QPainter& p);

	// 获取字体颜色
	QColor GetFontColor(int sideType);

	// 获取背景颜色
	QColor GetBackgroundColor(int sideType);

	// 绘制游戏特效
	void drawGameText(QPainter& p, const QString& text, QFont& font, QRectF& rect, QColor& fontColor);

	// 绘制指南针
	void drawCompass(QPainter& painter, const QRect& rect, qreal yaw);

	// 绘制方位标签
	void drawDirectionMarkers(QPainter& painter, const QPointF& center, qreal radius, qreal yaw);

	// 绘制方位箭头
	void drawDirectionArrow(QPainter& painter, const QPointF& center, qreal radius, qreal yaw);

	// 绘制方位文字
	void drawHeadingText(QPainter& painter, const QPointF& center, qreal radius, qreal yaw);

	// 计算圆中点
	QPointF calculatePointOnCircle(const QPointF& center, qreal radius, qreal angleDegrees);

private:
    Ui::VedioContainer *ui;
	long long m_identity = 0;
	QString m_weaponName;
	EntityVedioDataSPtr m_entityDataPtr;
};

#endif // VedioContainer_H
