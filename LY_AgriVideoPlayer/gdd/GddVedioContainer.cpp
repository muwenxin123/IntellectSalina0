#include "GddVedioContainer.h"
#include "ui_GddVedioContainer.h"

#include <QCString.h>
#include <QtMath>
#include <QPainter>
#include<QStyleOption>
#include<QResizeEvent>
#include<QMouseEvent>
#include "QDebug"
#include "../LY_AgriVideoPlayer.h"

GddVedioContainer::GddVedioContainer(QWidget *parent) :
	LyDialog(tr2("GDD视频窗口"),parent)
   , ui(new Ui::GddVedioContainer)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(500, 800);
}

GddVedioContainer::~GddVedioContainer()
{
    delete ui;
}

void GddVedioContainer::setData(const WS::DetectionData& data)
{
	m_data = data;
	qDebug() << "🔄 setData被调用";
	qDebug() << "  图像尺寸:" << m_data.image.size();
	qDebug() << "  元数据尺寸:" << m_data.metadata.width << "x" << m_data.metadata.height;
	qDebug() << "  bboxes数量:" << m_data.bboxes.size();
	qDebug() << "  polygons数量:" << m_data.polygons.size();

	update();
}

void GddVedioContainer::paintEvent(QPaintEvent *event)
{
	if (m_data.image.isNull())
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

	if (!m_data.image.isNull())
	{
		QRect boderRect = rect().adjusted(2, 2, -2, -2);
		p.drawImage(boderRect, m_data.image);
	}

	paintRect(p);
	//QPixmap pixmap = this->grab();
	//WS::EventData eventData;
	//eventData.recimage = pixmap;
	//LY_AgriVideoPlayer::Get().updateDataResult(eventData);
	
}

//void GddVedioContainer::paintRect(QPainter& p)
//{
//	if (!m_data.metadata.polygons.empty()) {
//		p.save();
//		QPen pen(QColor(0, 255, 0, 255));
//		pen.setWidth(1);
//		pen.setStyle(Qt::SolidLine);
//		p.setPen(pen);
//
//		for (auto& polygon : m_data.metadata.polygons)  // 变量名改为 polygon
//		{
//			double widthScale = (width() - 4) * 1.0 / m_data.metadata.width;
//			double heightScale = (height() - 4) * 1.0 / m_data.metadata.height;
//
//			// 多边形需要绘制多个点，不是矩形
//			QPolygonF qpolygon;
//			for (const auto& point : polygon.points) {
//				double x = point[0] * widthScale;
//				double y = point[1] * heightScale;
//				qpolygon << QPointF(x, y);
//			}
//
//			// 绘制多边形
//			p.drawPolygon(qpolygon);
//
//			// 如果需要绘制标签
//			if (!polygon.label.empty()) {
//				p.save();
//				QFont font = qApp->font();
//				font.setPixelSize(8);
//				font.setWeight(1);
//				p.setFont(font);
//
//				QPen textPen(QColor(245, 0, 0));
//				textPen.setWidth(2);
//				p.setPen(textPen);
//
//				QString text = QString::fromStdString(polygon.label);
//				QFontMetrics fontMetrics(font);
//				int textWidth = fontMetrics.horizontalAdvance(text);
//				int textHeight = fontMetrics.height();
//
//				// 使用多边形的第一个点作为标签位置
//				if (!polygon.points.empty()) {
//					double labelX = polygon.points[0][0] * widthScale;
//					double labelY = polygon.points[0][1] * heightScale;
//					QRectF curRect(labelX, labelY, textWidth, textHeight);
//					p.drawText(curRect, Qt::AlignCenter, text);
//				}
//				p.restore();
//			}
//		}
//		p.restore();
//	}
//}

void GddVedioContainer::paintRect(QPainter& p)
{
	qDebug() << "=== paintRect ===";
	qDebug() << "bboxes:" << m_data.bboxes.size();
	qDebug() << "polygons:" << m_data.polygons.size();

	// 计算缩放比例
	int baseWidth = m_data.metadata.width > 0 ? m_data.metadata.width : m_data.image.width();
	int baseHeight = m_data.metadata.height > 0 ? m_data.metadata.height : m_data.image.height();

	if (baseWidth == 0 || baseHeight == 0) return;

	double widthScale = (width() - 4) * 1.0 / baseWidth;
	double heightScale = (height() - 4) * 1.0 / baseHeight;

	// 1. 绘制检测框
	if (!m_data.bboxes.empty()) {
		p.save();
		for (const auto& bbox : m_data.bboxes) {
			// 设置颜色
			QColor color(bbox.color[0], bbox.color[1], bbox.color[2], 255);
			QPen pen(color);
			pen.setWidth(3);
			p.setPen(pen);

			// 计算坐标
			const auto& box = bbox.bbox;
			double x = box[0] * baseWidth * widthScale;
			double y = box[1] * baseHeight * heightScale;
			double w = box[2] * baseWidth * widthScale;
			double h = box[3] * baseHeight * heightScale;

			// 绘制矩形框
			p.drawRect(QRectF(x, y, w, h));
		}
		p.restore();
	}

	//// 2. 绘制多边形
	//if (!m_data.polygons.empty()) {
	//	p.save();
	//	QPen pen(QColor(0, 255, 0, 255));
	//	pen.setWidth(2);
	//	p.setPen(pen);

	//	for (const auto& polygon : m_data.polygons) {
	//		QPolygonF qpolygon;
	//		for (const auto& point : polygon.points) {
	//			double x = point[0] * baseWidth * widthScale;
	//			double y = point[1] * baseHeight * heightScale;
	//			qpolygon << QPointF(x, y);
	//		}
	//		p.drawPolygon(qpolygon);
	//	}
	//	p.restore();
	//}

	//// 3. 绘制测试框
	//p.save();
	//QPen testPen(Qt::red);
	//testPen.setWidth(3);
	//p.setPen(testPen);
	//p.drawRect(1200, 200, 150, 50);
	//p.drawText(1200, 200, tr2("测试框"));
	//p.restore();

	qDebug() << "=== paintRect 完成 ===";
}