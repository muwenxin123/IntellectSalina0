#include "VedioSDLContainer.h"

#include <QFont>
#include <QtMath>
#include <QPainter>
#include <QStyleOption>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QApplication>


#define SDL_MAIN_HANDLED
extern "C"
{
#include "SDL2/SDL_config.h"
#include "SDL2/SDL.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavutil/pixfmt.h"
#include "libavutil/hwcontext.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
}

#include <QCString.h>
#include "Ly_VideoRealTimePreview_global.h"
#include "Ly_VideoRealTimePreviewInterface.h"

VedioSDLContainer::VedioSDLContainer(EntityVedioDataSPtr pEntityData, QWidget *parent) :
    QWidget(parent)
	,m_entityDataPtr(pEntityData)
{
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NativeWindow);
	setAttribute(Qt::WA_WState_Created);

	m_identity = m_entityDataPtr->GetIdentity();
	m_entityDataPtr->GetEntityName(m_weaponName);

	connect(this, &VedioSDLContainer::updateContainter, this, &VedioSDLContainer::onUpdateContainer, Qt::QueuedConnection);

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		qFatal("SDL INIT FAilded");
	}
}

VedioSDLContainer::~VedioSDLContainer()
{
	ReleaseSDL();
	SDL_Quit();
}

long long VedioSDLContainer::GetIdentity()
{
	return m_identity;
}

void VedioSDLContainer::createSDL()
{
	if (nullptr == m_sdlWindow)
	{
		m_sdlWindow = SDL_CreateWindowFrom((void*)this->winId());
		if (nullptr == m_sdlWindow)
		{
			qFatal("SDL_CreateWindowFrom error");
			return;
		}
		m_sdlRenderer = SDL_CreateRenderer(m_sdlWindow, -1, SDL_RENDERER_ACCELERATED);
		if (nullptr == m_sdlRenderer)
		{
			qFatal("SDL_CreateRenderer error");
			return;
		}
		//m_sdl_Texture = SDL_CreateTexture(m_sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, width(), height());
		//if (nullptr == m_sdl_Texture)
		//{
		//	qFatal("SDL_CreateTexture error");
		//	return;
		//}
	}
}

void VedioSDLContainer::ReleaseSDL()
{
	if (m_sdl_Texture)
	{
		SDL_DestroyTexture(m_sdl_Texture);
		m_sdl_Texture = nullptr;
	}

	if (m_sdlRenderer)
	{
		SDL_DestroyRenderer(m_sdlRenderer);
		m_sdlRenderer = nullptr;
	}

	if (m_sdlWindow)
	{
		SDL_DestroyWindow(m_sdlWindow);
		m_sdlWindow = nullptr;
	}
}

void VedioSDLContainer::resizeEvent(QResizeEvent* event)
{
	QWidget::resizeEvent(event);
	ReleaseSDL();
	createSDL();
	update();
}

QPaintEngine* VedioSDLContainer::paintEngine() const
{
	return QWidget::paintEngine();
}

void VedioSDLContainer::onUpdateContainer()
{
	auto pService = IVideoRealTimePreviewActivator::Get().getPISensorVedioService();
	if (nullptr != pService)
	{
		if (m_entityDataPtr->isChanged())
			update();
	}
}

void VedioSDLContainer::paintEvent(QPaintEvent *event)
{
	paint();
}

void VedioSDLContainer::paint()
{
	QPainter p(this);

	if (!PaintSDL(p))
		return;

	double distance = 0;
	m_entityDataPtr->GetEntityDistance(distance);

	std::vector<XygsZC_Mark_01> targets;
	m_entityDataPtr->GetTargetInfo(targets);

	std::vector<qnzkna::SensorVedio::FriendRectScope> friends;
	m_entityDataPtr->GetFriendInfo(friends);

	double yaw = m_entityDataPtr->GetEntityYaw();

	double cameraYaw = 0.0;
	double cameraPitch = 0.0;
	double cameraRoll = 0.0;
	m_entityDataPtr->GetCameraInfo(cameraYaw, cameraPitch, cameraRoll);

	QRect rc = rect();

	{
		p.save();
		QPen pen(QColor(0, 255, 255, 250));
		pen.setWidth(6);
		pen.setStyle(Qt::SolidLine);
		p.setPen(pen);
		QRect boderRect = rect().adjusted(1, 1, -1, -1);
		p.drawRect(boderRect);
		p.restore();
	}
	
	{
		p.save();
		// 设置字体
		QFont font = qApp->font();
		font.setPixelSize(12);
		font.setWeight(1);
		p.setFont(font);

		// 设置画笔
		QPen pen = p.pen();
		pen.setColor({ 245, 245, 220 });
		pen.setWidth(2);
		p.setPen(pen);
		QFontMetrics fontMetrics(font);
		int width = fontMetrics.width(m_weaponName, m_weaponName.size());
		int height = fontMetrics.height();

		int posX = (rect().width() - width) / 2;
		int posY = 2 + height / 2;
		QRect nameRect(posX, posY, width, height);
		p.drawText(nameRect, Qt::AlignCenter, m_weaponName);

		if (distance >= 0)
		{
			QString distanceStr = QString(QString::fromLocal8Bit("距离：%1 米")).arg(distance, 0, 'f', 3);
			width = fontMetrics.width(distanceStr, distanceStr.size());
			posX = (rect().width() - width) / 2;
			posY = 2 + fontMetrics.lineSpacing() + height / 2;
			QRect distanceRect(posX, posY, width, height);
			p.drawText(distanceRect, Qt::AlignCenter, distanceStr);
		}
		p.restore();
	}

	paintTargets(targets, p);
	//paintFriends(friends, p);

	{
		// 设置字体
		QFont font = qApp->font();
		font.setPixelSize(12);
		font.setWeight(1);

		QFontMetrics merics(font);
		int space = merics.height();

		int width = this->rect().width() / 6;
		int height = this->rect().height() / 6;
		int radius = qMin(width, height);
		QRect rect(this->rect().width() - radius - space, space,
			radius, radius);
		drawCompass(p, rect, yaw + cameraYaw);
	}
}

bool VedioSDLContainer::PaintSDL(QPainter& painter)
{
	AVFrame* pAvframe = nullptr;
	QByteArray array;
	m_entityDataPtr->GetSDLData(&pAvframe, array);
	if (nullptr != pAvframe)
	{
		if (nullptr == m_sdlRenderer)
		{
			av_frame_free(&pAvframe);
			return false;
		}

		Uint32 sdl_format = 0;
		switch (pAvframe->format)
		{
		case   AVPixelFormat::AV_PIX_FMT_NV12:
			sdl_format = SDL_PIXELFORMAT_NV12;
			break;
		case   AVPixelFormat::AV_PIX_FMT_YUV420P:
		default:
			sdl_format = SDL_PIXELFORMAT_IYUV;
			break;
		}
	
		if (m_sdl_Texture != nullptr)
		{
			SDL_DestroyTexture(m_sdl_Texture);
			m_sdl_Texture = nullptr;
		}

		m_sdl_Texture = SDL_CreateTexture(m_sdlRenderer,
			sdl_format,
			SDL_TEXTUREACCESS_STREAMING,
			pAvframe->width,
			pAvframe->height);

		if (sdl_format == SDL_PIXELFORMAT_IYUV)
		{
			SDL_UpdateYUVTexture(m_sdl_Texture, nullptr,
				pAvframe->data[0], pAvframe->linesize[0],
				pAvframe->data[1], pAvframe->linesize[1],
				pAvframe->data[2], pAvframe->linesize[2]);
		}
		else if (sdl_format == SDL_PIXELFORMAT_NV12)
		{
			//SDL_UpdateTexture(m_sdl_Texture, nullptr, buffer, p_frame_raw->width);
		}

		SDL_RenderClear(m_sdlRenderer);
		int window_w = 0;
		int window_h = 0;
		SDL_GetRendererOutputSize(m_sdlRenderer, &window_w, &window_h);
		SDL_Rect destRect = { 0,0, window_w, window_h };
		SDL_RenderCopy(m_sdlRenderer, m_sdl_Texture, nullptr, &destRect);
		SDL_RenderPresent(m_sdlRenderer);
		av_frame_free(&pAvframe);
		return true;
	}
	else if (!array.isEmpty())
	{
		QImage image;
		image.loadFromData(array, "JPG");
		if (!image.isNull())
		{
			QPixmap map = QPixmap::fromImage(image);
			QRect boderRect = rect().adjusted(2, 2, -2, -2);
			painter.drawPixmap(boderRect, map);
			return true;
		}
	}

	return false;
}

void VedioSDLContainer::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		emit entityDoubleClicked(m_identity);
	}
	return QWidget::mousePressEvent(event);
}

void VedioSDLContainer::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		double lon = 0;
		double lat = 0;
		m_entityDataPtr->GetEntityPos(lon, lat);
		if (lon >= -180 && lat >= -90)
		{
			Ly_VideoRealTimePreviewInterface::Get().Script("SetMapScale(4000)");
			QString str = "CenterAt(" + QString::number(lon, 'f', 8)
				+ "," + QString::number(lat, 'f', 8) + ")";
			Ly_VideoRealTimePreviewInterface::Get().Script(str.toLocal8Bit());
		}
	}
	return QWidget::mouseDoubleClickEvent(event);
}

bool VedioSDLContainer::event(QEvent* event)
{
	switch (event->type())
	{
	case QEvent::Show:
	case QEvent::ShowToParent:
	{
		if (nullptr != m_entityDataPtr)
			m_entityDataPtr->addShowFlag();
	}
	break;
	case QEvent::Hide:
	case QEvent::HideToParent:
	{
		if (nullptr != m_entityDataPtr)
			m_entityDataPtr->reduceShowFlag();
	}
	break;
	default:
		break;
	}

	return QWidget::event(event);
}

void VedioSDLContainer::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
}

void VedioSDLContainer::hideEvent(QHideEvent* event)
{
	QWidget::hideEvent(event);
}

void VedioSDLContainer::paintTargets(std::vector<XygsZC_Mark_01>& targets, QPainter& p)
{
	if (!targets.empty()) {
		p.save();
		QPen pen(QColor(0, 255, 0, 255));
		pen.setWidth(1);
		pen.setStyle(Qt::SolidLine);
		p.setPen(pen);

		for (auto& target : targets)
		{
			double widthScale = (width() - 4) * 1.0 / target.videoWidth;
			double heightScale = (height() - 4) * 1.0 / target.videoHeight;
			double dx = target.targetRectX * widthScale;
			double dy = target.targetRectY * heightScale;

			QRectF rect(dx, dy, target.targetRectWidth *widthScale, target.targetRectHeight *heightScale);
			p.drawRect(rect);

			{
				p.save();
				// 设置字体
				QFont font = qApp->font();
				font.setPixelSize(8);
				font.setWeight(1);
				p.setFont(font);

				// 设置画笔
				QPen pen = p.pen();
				pen.setColor({ 245, 0, 0 });
				pen.setWidth(2);
				p.setPen(pen);

				QString text = QString(QString::fromLocal8Bit("目标： %1）").arg(target.targetID));
				QFontMetrics fontMetrics(font);
				int width = fontMetrics.width(text, text.size());
				int height = fontMetrics.height();
				QRectF curRect(dx, dy, width, height);
				p.drawText(curRect, Qt::AlignCenter, text);
				p.restore();
			}
		}
		p.restore();
	}
}

void VedioSDLContainer::paintFriends(std::vector<qnzkna::SensorVedio::FriendRectScope>& friends, QPainter& p)
{
	if (!friends.empty()) {
		p.save();
		QPen pen(QColor(255, 0, 0, 255));
		pen.setWidth(1);
		pen.setStyle(Qt::SolidLine);
		p.setPen(pen);

		int fontPix = 12;
		for (auto& friendData : friends)
		{
			double deepScale = qMin(1.0, friendData.deepScan);
			deepScale = qMax(0.4, deepScale);
#ifdef USE_BRIEF_CALAC
			double widthScale = (width() - 4) * 1.0 / friendData.videoWidth;
			double heightScale = (height() - 4) * 1.0 / friendData.videoHeight;
			double dx = friendData.rectX * widthScale;
			double dy = friendData.rectY * heightScale;
			//QRectF rect(dx, dy, friendData.rectWidth *widthScale, friendData.rectHeight *heightScale);
			//p.drawRect(rect);

			{
				// 设置字体
				QFont textFont = qApp->font();
				textFont.setPixelSize(fontPix * deepScale);
				textFont.setWeight(1);
				textFont.setItalic(true);
				textFont.setBold(true);

				QFontMetrics fontMetrics(textFont);
				double miniWidth = fontMetrics.width("1", 1);

				double curDx = dx + friendData.rectWidth *widthScale + miniWidth;
				double curDy = dy;

				QString nameText = QString(QString::fromLocal8Bit("%1").arg(QString::fromLocal8Bit(friendData.targetName)));
				QString heightText = QString(QString::fromLocal8Bit("高度：%1").arg(friendData.targetHeight, 0, 'f', 3));
				QString distanceText = QString(QString::fromLocal8Bit("距离：%1").arg(friendData.distance, 0, 'f', 3));

				int width = 0;
				width = fontMetrics.width(nameText, nameText.size());
				width = qMax(fontMetrics.width(heightText, heightText.size()), width);
				width = qMax(fontMetrics.width(distanceText, distanceText.size()), width);
				width = width + miniWidth;

				int height = fontMetrics.height();
				QRectF nameRect(curDx, curDy, width, height);

				curDy += fontMetrics.lineSpacing();
				QRectF heightRect(curDx, curDy, width, height);

				curDy += fontMetrics.lineSpacing();
				QRectF distanceRect(curDx, curDy, width, height);

				double sumHeight = 3 * fontMetrics.lineSpacing();
				QRectF sumRect(curDx - miniWidth, dy - fontMetrics.leading(), width + 2 * miniWidth, sumHeight + fontMetrics.leading());

				p.save();
				p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

				// 设置画笔
				QColor fontColor = GetFontColor(friendData.sideType);
				QColor backgroundColor = GetBackgroundColor(friendData.sideType);
				QPen pen = p.pen();
				pen.setColor(fontColor);
				pen.setWidth(2);
				p.setPen(pen);
				p.fillRect(sumRect, backgroundColor);

				drawGameText(p, nameText, textFont, nameRect, fontColor);
				drawGameText(p, heightText, textFont, heightRect, fontColor);
				drawGameText(p, distanceText, textFont, distanceRect, fontColor);
				p.fillRect(QRectF(sumRect.x(), sumRect.y() + sumRect.height(), 10, sumRect.height() / 2), backgroundColor);
				p.restore();
		}
#else
			QRectF curRect(0, 0, friendData.videoWidth, friendData.videoHeight);
			QRectF piRect(QPointF(friendData.topLeft.dx, friendData.topLeft.dy),
				QPointF(friendData.bottomRight.dx, friendData.bottomRight.dy));
			QRectF intersect = piRect.intersected(curRect);
			if (!intersect.isValid())
				continue;

			auto centerPos = intersect.topLeft();
			double widthScale = (width() - 4) * 1.0 / friendData.videoWidth;
			double heightScale = (height() - 4) * 1.0 / friendData.videoHeight;
			double dx = (centerPos.x()) * widthScale;
			double dy = (centerPos.y()) * heightScale;

			//QPolygon polygon;
			//polygon << QPoint(friendData.topLeft.dx * widthScale, friendData.topLeft.dy* widthScale)
			//	<< QPoint(friendData.topRight.dx* widthScale, friendData.topRight.dy* widthScale)
			//	<< QPoint(friendData.bottomRight.dx* widthScale, friendData.bottomRight.dy* widthScale)
			//	<< QPoint(friendData.bottomLeft.dx* widthScale, friendData.bottomLeft.dy* widthScale);
			//p.drawPolygon(polygon);

			{

				// 设置字体
				QFont textFont = qApp->font();
				textFont.setPixelSize(fontPix * deepScale);
				textFont.setWeight(1);
				textFont.setItalic(true);
				textFont.setBold(true);

				QFontMetrics fontMetrics(textFont);
				double miniWidth = fontMetrics.width("1", 1);
				QString nameText = QString(QString::fromLocal8Bit("%1").arg(QString::fromLocal8Bit(friendData.targetName)));
				QString heightText = QString(QString::fromLocal8Bit("高度：%1").arg(friendData.targetHeight, 0, 'f', 3));
				QString distanceText = QString(QString::fromLocal8Bit("距离：%1").arg(friendData.distance, 0, 'f', 3));

				int width = 0;

				width = fontMetrics.width(nameText, nameText.size());
				width = qMax(fontMetrics.width(heightText, heightText.size()), width);
				width = qMax(fontMetrics.width(distanceText, distanceText.size()), width);
				width = width + miniWidth;

				dy = dx;
				dy = dy - (fontMetrics.lineSpacing() * 3) * 3 / 4;
				double curDx = dx + miniWidth;
				double curDy = dy;

				int height = fontMetrics.height();
				QRectF nameRect(curDx, curDy, width, height);

				curDy += fontMetrics.lineSpacing();
				QRectF heightRect(curDx, curDy, width, height);

				curDy += fontMetrics.lineSpacing();
				QRectF distanceRect(curDx, curDy, width, height);

				double sumHeight = 3 * fontMetrics.lineSpacing();
				QRectF sumRect(curDx - miniWidth, dy - fontMetrics.leading(), width + 2 * miniWidth, sumHeight + fontMetrics.leading());

				p.save();
				p.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);

				// 设置画笔
				QColor fontColor = GetFontColor(friendData.sideType);
				QColor backgroundColor = GetBackgroundColor(friendData.sideType);
				QPen pen = p.pen();
				pen.setColor(fontColor);
				pen.setWidth(2);
				p.setPen(pen);
				p.fillRect(sumRect, backgroundColor);

				drawGameText(p, nameText, textFont, nameRect, fontColor);
				drawGameText(p, heightText, textFont, heightRect, fontColor);
				drawGameText(p, distanceText, textFont, distanceRect, fontColor);

				//p.fillRect(QRectF(sumRect.x(), sumRect.y() + sumRect.height(), 10, sumRect.height() / 2), backgroundColor);
				p.restore();
			}
#endif
		}
	p.restore();
	}
}

QColor VedioSDLContainer::GetFontColor(int sideType)
{
	QColor color;
	switch (sideType)
	{
	case 1:
		color = { 255, 255, 255, 160 };
		break;
	case 0:
	default:
		color = { 255,255,255, 160 };
		break;
	}
	return color;
}

QColor VedioSDLContainer::GetBackgroundColor(int sideType)
{
	QColor color;
	switch (sideType)
	{
	case 1:
		color = { 0,0,255,40 };
		break;
	case 0:
	default:
		color = { 255,0,0,40 };
		break;
	}

	return color;
}

void VedioSDLContainer::drawGameText(QPainter& p, const QString& text, QFont& font, QRectF& rect, QColor& fontColor)
{
	p.save();
	p.setFont(font);

	QLinearGradient strokeGradient(rect.topLeft(), rect.bottomRight());
	strokeGradient.setColorAt(0, fontColor);
	strokeGradient.setColorAt(1, fontColor);
	p.setPen(QPen(strokeGradient, 5));
	p.drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, text);
	p.restore();
}

void VedioSDLContainer::drawCompass(QPainter& painter, const QRect& rect, qreal yaw)
{
	painter.save(); // 保存当前painter状态

					// 设置抗锯齿
	painter.setRenderHint(QPainter::Antialiasing, true);

	// 计算中心点和半径
	QPointF center = rect.center();
	qreal radius = qMin(rect.width(), rect.height()) * 0.45;

	// 绘制外圆
	painter.setPen(QPen(Qt::black, 2));
	painter.setBrush(QColor{ 255,255,255,100 });
	painter.drawEllipse(center, radius, radius);

	// 绘制方向标记
	drawDirectionMarkers(painter, center, radius, 0);

	// 绘制指向当前航向的箭头
	drawDirectionArrow(painter, center, radius, yaw);

	// 显示当前航向
	drawHeadingText(painter, center, radius, yaw);

	painter.restore(); // 恢复painter状态
}

void VedioSDLContainer::drawDirectionMarkers(QPainter& painter, const QPointF& center, qreal radius, qreal yaw)
{
	QStringList directions = { "N", "E", "S", "W" };

	double textScale = 0.3;
	painter.setPen(QPen(Qt::black, 1));
	QFont font = painter.font();
	font.setPointSizeF(radius * textScale);
	painter.setFont(font);

	// 绘制主方向标记(N,E,S,W)
	for (int i = 0; i < 4; ++i) {
		qreal angle = i * 90.0 - yaw;
		QPointF pos = calculatePointOnCircle(center, radius * 0.6, angle);

		QRectF textRect(pos.x() - radius * 0.3, pos.y() - radius * 0.3,
			radius * textScale * 2, radius * textScale * 2);

		painter.drawText(textRect, Qt::AlignCenter, directions[i]);

		// 绘制主方向刻度线
		QPointF start = calculatePointOnCircle(center, radius * 0.8, angle);
		QPointF end = calculatePointOnCircle(center, radius, angle);
		painter.drawLine(start, end);
	}

	// 绘制小刻度线(每30度)
	painter.setPen(QPen(Qt::black, 0.5));
	for (int i = 0; i < 12; ++i) {
		if (i % 3 == 0) continue; // 跳过主方向刻度

		qreal angle = i * 30.0 - yaw;
		QPointF start = calculatePointOnCircle(center, radius * 0.90, angle);
		QPointF end = calculatePointOnCircle(center, radius, angle);
		painter.drawLine(start, end);
	}
}

void VedioSDLContainer::drawDirectionArrow(QPainter& painter, const QPointF& center, qreal radius, qreal yaw)
{
	qreal arrowSize = radius * 0.3;

	QPainterPath path;

	// 箭头尖端(指向当前航向)
	QPointF tip = calculatePointOnCircle(center, radius * 0.7, yaw);

	// 箭头尾部两点
	QPointF tail1 = calculatePointOnCircle(center, radius * 0.4, yaw + 150);
	QPointF tail2 = calculatePointOnCircle(center, radius * 0.4, yaw - 150);

	path.moveTo(tip);
	path.lineTo(tail1);
	path.lineTo(tail2);
	path.closeSubpath();

	// 绘制箭头
	painter.setPen(QPen(Qt::red, 1.5));
	painter.setBrush(QColor(255, 0, 0, 100)); // 半透明红色填充
	painter.drawPath(path);

	// 绘制箭头中心线
	painter.setPen(QPen(Qt::white, 1.5));
	painter.drawLine(center, tip);

	// 绘制中心固定点
	painter.setPen(Qt::NoPen);
	painter.setBrush(Qt::black);
	painter.drawEllipse(center, radius * 0.05, radius * 0.05);
}

void VedioSDLContainer::drawHeadingText(QPainter& painter, const QPointF& center, qreal radius, qreal yaw)
{
	QFont font = painter.font();
	font.setPointSizeF(radius * 0.2);
	font.setBold(true);
	painter.setFont(font);
	painter.setPen(QPen(Qt::black, 1));

	QString headingText = QString(tr2("%1°")).arg(qRound(fmod(yaw + 360.0, 360.0)), 3, 10, QLatin1Char('0'));
	QRectF textRect(center.x() - radius*0.3, center.y(),
		radius*0.8, radius*0.3);

	painter.drawText(textRect, Qt::AlignCenter, headingText);
}

QPointF VedioSDLContainer::calculatePointOnCircle(const QPointF& center, qreal radius, qreal angleDegrees)
{
	qreal angleRadians = qDegreesToRadians(angleDegrees);
	return QPointF(
		center.x() + radius * qSin(angleRadians),
		center.y() - radius * qCos(angleRadians)
	);
}
