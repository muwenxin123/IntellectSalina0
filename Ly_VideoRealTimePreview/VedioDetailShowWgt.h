#ifndef VEDIODETAILSHOWWGT_H
#define VEDIODETAILSHOWWGT_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QList>

#include  "IVideoRealTimePreviewActivator.h"

class VedioDetailShowWgt : public QWidget
{
    Q_OBJECT
public:
    explicit VedioDetailShowWgt(QWidget* dependView, QWidget *parent = nullptr);
    ~VedioDetailShowWgt();

	// 设置实体ID
	void SetEntityIdentity(long long identity);

	// 获取实体ID
	long long GetEntityIdentityID();

	// 定时处理
	void onTimeOut();

private:
	void focusOutEvent(QFocusEvent *event)override;

	// 事件过滤器
	bool eventFilter(QObject *object, QEvent *event)override;

private:
	// 绘制
	void paintEvent(QPaintEvent *event)override;

	void mousePressEvent(QMouseEvent *event);

	// 双击
	void mouseDoubleClickEvent(QMouseEvent *event)override;

	// 显示事件
	void showEvent(QShowEvent* event);

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

private slots:
	// 核对是否显示
	void CheckShow();

	void addEntityShowFlag(long long identity);
	void reduceEntityShowFlag(long long identity);

private:
	bool m_friendShowFlag = true;
	long long m_identity = -1;
	QString m_weaponName;
	QWidget* m_dependView = nullptr;
	bool m_isDenpendVisiable = false;
};

#endif // VEDIODETAILSHOWWGT_H
