#include "GddVedioContainer.h"
#include "ui_GddVedioContainer.h"

#include <QtMath>
#include <QPainter>
#include<QStyleOption>
#include<QResizeEvent>
#include<QMouseEvent>


GddVedioContainer::GddVedioContainer(QWidget *parent) :
    QWidget(parent)
   , ui(new Ui::GddVedioContainer)
{
    ui->setupUi(this);
}

GddVedioContainer::~GddVedioContainer()
{
    delete ui;
}

void GddVedioContainer::setData(WS::DetectionData& data)
{
	m_data = data;
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
}

void GddVedioContainer::paintRect(QPainter& p)
{
	if (!m_data.metadata.polygons.empty()) {
		p.save();
		QPen pen(QColor(0, 255, 0, 255));
		pen.setWidth(1);
		pen.setStyle(Qt::SolidLine);
		p.setPen(pen);

		for (auto& polygon : m_data.metadata.polygons)  // 变量名改为 polygon
		{
			double widthScale = (width() - 4) * 1.0 / m_data.metadata.width;
			double heightScale = (height() - 4) * 1.0 / m_data.metadata.height;

			// 多边形需要绘制多个点，不是矩形
			QPolygonF qpolygon;
			for (const auto& point : polygon.points) {
				double x = point[0] * widthScale;
				double y = point[1] * heightScale;
				qpolygon << QPointF(x, y);
			}

			// 绘制多边形
			p.drawPolygon(qpolygon);

			// 如果需要绘制标签
			if (!polygon.label.empty()) {
				p.save();
				QFont font = qApp->font();
				font.setPixelSize(8);
				font.setWeight(1);
				p.setFont(font);

				QPen textPen(QColor(245, 0, 0));
				textPen.setWidth(2);
				p.setPen(textPen);

				QString text = QString::fromStdString(polygon.label);
				QFontMetrics fontMetrics(font);
				int textWidth = fontMetrics.horizontalAdvance(text);
				int textHeight = fontMetrics.height();

				// 使用多边形的第一个点作为标签位置
				if (!polygon.points.empty()) {
					double labelX = polygon.points[0][0] * widthScale;
					double labelY = polygon.points[0][1] * heightScale;
					QRectF curRect(labelX, labelY, textWidth, textHeight);
					p.drawText(curRect, Qt::AlignCenter, text);
				}
				p.restore();
			}
		}
		p.restore();
	}
}
