#ifndef VIDEOSDLCONTAINER_H
#define VIDEOSDLCONTAINER_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QList>

#include "EntityVedioData.h"
#include  "IVideoRealTimePreviewActivator.h"

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;
class VedioSDLContainer : public QWidget
{
    Q_OBJECT
public:	
    explicit VedioSDLContainer(EntityVedioDataSPtr pEntityData, QWidget *parent = nullptr);
    ~VedioSDLContainer();

	long long GetIdentity();

signals:
	void updateContainter();
	
	void entityDoubleClicked(long long identity);

private:
	void createSDL();
	void ReleaseSDL();

	void resizeEvent(QResizeEvent* event);

	QPaintEngine* paintEngine()const override;

public slots :
	void onUpdateContainer();

private:
	// 绘制
	void paintEvent(QPaintEvent *event)override;

	void paint();

	// 绘制SDL界面
	bool PaintSDL(QPainter& painter);

	// 单机
	void mousePressEvent(QMouseEvent *event)override;

	// 双击
	void mouseDoubleClickEvent(QMouseEvent *event)override;

	bool event(QEvent* event)override;

	// 显示事件
	void showEvent(QShowEvent* event)override;

	// 显示事件
	void hideEvent(QHideEvent* event)override;

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
	long long m_identity = 0;
	QString m_weaponName;
	EntityVedioDataSPtr m_entityDataPtr;
	SDL_Window* m_sdlWindow = nullptr;
	SDL_Renderer* m_sdlRenderer = nullptr;
	SDL_Texture * m_sdl_Texture = nullptr;
};

#endif // VIDEOSDLCONTAINER_H
