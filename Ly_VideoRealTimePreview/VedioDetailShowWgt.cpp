#include "VedioDetailShowWgt.h"

#include <QtMath>
#include <QPainter>
#include <QMetaObject>
#include <QStyleOption>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QApplication>

#include <QCString.h>
#include "EntityVedioData.h"
#include "Ly_VideoRealTimePreview_global.h"
#include "Ly_VideoRealTimePreviewInterface.h"

VedioDetailShowWgt::VedioDetailShowWgt(QWidget* dependView, QWidget *parent)
	: QWidget(parent)
	, m_dependView(dependView)
{
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
	m_dependView->installEventFilter(this);
}

VedioDetailShowWgt::~VedioDetailShowWgt()
{
	if (m_identity >= 0)
		reduceEntityShowFlag(m_identity);
	
	m_dependView->removeEventFilter(this);
}

void VedioDetailShowWgt::SetEntityIdentity(long long identity)
{
	// 如果已经显示则变为隐藏
	if (m_identity == identity)
	{
		reduceEntityShowFlag(m_identity);
		m_identity = -1;
		CheckShow();
		return;
	}

	auto pWEntity = EntityDataManage::Get().FindEntity(identity);
	auto pEntity = pWEntity.lock();
	if (pEntity)
	{
		reduceEntityShowFlag(m_identity);
		m_identity = identity;
		addEntityShowFlag(m_identity);
		pEntity->GetEntityName(m_weaponName);
		CheckShow();
	}
}

long long VedioDetailShowWgt::GetEntityIdentityID()
{
	return m_identity;
}

void VedioDetailShowWgt::onTimeOut()
{
	if (!isHidden())
	{
		QMetaObject::invokeMethod(this, "update", Qt::QueuedConnection);
	}
}

void VedioDetailShowWgt::focusOutEvent(QFocusEvent *event)
{
	return QWidget::focusOutEvent(event);	
}

bool VedioDetailShowWgt::eventFilter(QObject *object, QEvent *event)
{
	QWidget* pWgt = dynamic_cast<QWidget*>(object);
	if (pWgt == m_dependView && nullptr != pWgt)
	{
		int type = event->type();
		switch (type)
		{
		case QEvent::Show:
		case QEvent::ShowToParent:
		{
			m_isDenpendVisiable = true;
			QMetaObject::invokeMethod(this, "CheckShow", Qt::QueuedConnection);
		}
		break;
		case QEvent::Hide:
		case QEvent::HideToParent:
		{
			m_isDenpendVisiable = false;
			QMetaObject::invokeMethod(this, "CheckShow", Qt::QueuedConnection);
		}
		break;
		default:
			break;
		}
	}
	return QWidget::eventFilter(object, event);
}

void VedioDetailShowWgt::CheckShow()
{
	if (m_isDenpendVisiable && m_identity >= 0)
	{
		show();
		raise();
		setFocus();
		activateWindow();
	}
	else
	{
		hide();
	}
}

void VedioDetailShowWgt::addEntityShowFlag(long long identity)
{
	auto pWEntity = EntityDataManage::Get().FindEntity(identity);
	auto pEntity = pWEntity.lock();
	if (pEntity)
	{
		pEntity->addShowFlag();
	}
}

void VedioDetailShowWgt::reduceEntityShowFlag(long long identity)
{
	auto pWEntity = EntityDataManage::Get().FindEntity(identity);
	auto pEntity = pWEntity.lock();
	if (pEntity)
	{
		pEntity->reduceShowFlag();
	}
}

void VedioDetailShowWgt::paintEvent(QPaintEvent *event)
{
	auto pWEntity = EntityDataManage::Get().FindEntity(m_identity);
	auto pEntity = pWEntity.lock();
	if (!pEntity)
	{
		reduceEntityShowFlag(m_identity);
		m_identity = -1;
		QMetaObject::invokeMethod(this, "CheckShow", Qt::QueuedConnection);
		return;
	}

	QPixmap map;
	pEntity->GetPixMap(map);

	double distance = 0;
	pEntity->GetEntityDistance(distance);

	std::vector<XygsZC_Mark_01> targets;
	pEntity->GetTargetInfo(targets);

	std::vector<qnzkna::SensorVedio::FriendRectScope> friends;
	pEntity->GetFriendInfo(friends);

	double yaw = pEntity->GetEntityYaw();

	double cameraYaw = 0.0;
	double cameraPitch = 0.0;
	double cameraRoll = 0.0;
	pEntity->GetCameraInfo(cameraYaw, cameraPitch, cameraRoll);

	if (map.isNull())
		return;

	QPainter p(this);
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

	if (!map.isNull())
	{
		QRect boderRect = rect().adjusted(2, 2, -2, -2);
		p.drawPixmap(boderRect, map);
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

	if (m_friendShowFlag)
		paintFriends(friends, p);

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

void VedioDetailShowWgt::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::RightButton)
	{
		reduceEntityShowFlag(m_identity);
		m_identity = -1;
		QMetaObject::invokeMethod(this, "CheckShow", Qt::QueuedConnection);
	}
	return QWidget::mousePressEvent(event);
}

void VedioDetailShowWgt::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_friendShowFlag = !m_friendShowFlag;
		//double lon = 0;
		//double lat = 0;
		//auto pWEntity = EntityDataManage::Get().FindEntity(m_identity);
		//auto pEntity = pWEntity.lock();
		//if (pEntity)
		//{
		//	pEntity->GetEntityPos(lon, lat);
		//	if (lon >= -180 && lat >= -90)
		//	{
		//		Ly_VideoRealTimePreviewInterface::Get().Script("SetMapScale(4000)");
		//		QString str = "CenterAt(" + QString::number(lon, 'f', 8)
		//			+ "," + QString::number(lat, 'f', 8) + ")";
		//		Ly_VideoRealTimePreviewInterface::Get().Script(str.toLocal8Bit());
		//	}
		//}
	}
	return QWidget::mouseDoubleClickEvent(event);
}

void VedioDetailShowWgt::showEvent(QShowEvent* event)
{
	QWidget::showEvent(event);
}

void VedioDetailShowWgt::paintTargets(std::vector<XygsZC_Mark_01>& targets, QPainter& p)
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

void VedioDetailShowWgt::paintFriends(std::vector<qnzkna::SensorVedio::FriendRectScope>& friends, QPainter& p)
{
	if (!friends.empty()) {
		p.save();
		QPen pen(QColor(255, 0, 0, 255));
		pen.setWidth(1);
		pen.setStyle(Qt::SolidLine);
		p.setPen(pen);

		int fontPix = 24;
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

				p.fillRect(QRectF(sumRect.x(), sumRect.y() + sumRect.height(), 10, sumRect.height() / 2), backgroundColor);
				p.restore();
			}
#endif
		}
		p.restore();
	}
}

QColor VedioDetailShowWgt::GetFontColor(int sideType)
{
	QColor color;
	switch (sideType)
	{
	case 1:
		color = {255,255,255, 160};
		break;
	case 0:
	default:
		color = {255,255,255, 160};
		break;
	}
	return color;
}

QColor VedioDetailShowWgt::GetBackgroundColor(int sideType)
{
	QColor color;
	switch (sideType)
	{
	case 1:
		color = {0,0,255,40};
		break;
	case 0:
	default:
		color = { 255,0,0,40 };
		break;
	}

	return color;
}

void VedioDetailShowWgt::drawGameText(QPainter& p, const QString& text, QFont& font, QRectF& rect, QColor& fontColor)
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

// 绘制指南针
void VedioDetailShowWgt::drawCompass(QPainter& painter, const QRect& rect, qreal yaw)
{
	painter.save(); // 保存当前painter状态

					// 设置抗锯齿
	painter.setRenderHint(QPainter::Antialiasing, true);

	// 计算中心点和半径
	QPointF center = rect.center();
	qreal radius = qMin(rect.width(), rect.height()) * 0.45;

	// 绘制外圆
	painter.setPen(QPen(Qt::black, 2));
	painter.setBrush(QColor{ 255,255,255,100});
	painter.drawEllipse(center, radius, radius);

	// 绘制方向标记
	drawDirectionMarkers(painter, center, radius, 0);

	// 绘制指向当前航向的箭头
	drawDirectionArrow(painter, center, radius, yaw);

	// 显示当前航向
	drawHeadingText(painter, center, radius, yaw);

	painter.restore(); // 恢复painter状态
}

// 绘制方位标签
void VedioDetailShowWgt::drawDirectionMarkers(QPainter& painter, const QPointF& center, qreal radius, qreal yaw)
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

		QRectF textRect(pos.x() - radius * 0.3 , pos.y() - radius * 0.3,
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

// 绘制方位箭头
void VedioDetailShowWgt::drawDirectionArrow(QPainter& painter, const QPointF& center, qreal radius, qreal yaw)
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

// 绘制方位文字
void VedioDetailShowWgt::drawHeadingText(QPainter& painter, const QPointF& center, qreal radius, qreal yaw)
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

// 计算圆中点
QPointF VedioDetailShowWgt::calculatePointOnCircle(const QPointF& center, qreal radius, qreal angleDegrees)
{
	qreal angleRadians = qDegreesToRadians(angleDegrees);
	return QPointF(
		center.x() + radius * qSin(angleRadians),
		center.y() - radius * qCos(angleRadians)
	);
}
