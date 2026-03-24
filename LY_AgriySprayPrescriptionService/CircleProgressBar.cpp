#include "CircleProgressBar.h"
#include <QPainter>
#include <QPainterPath>
CircleProgressBar::CircleProgressBar(QWidget *parent) 
	: QWidget(parent),
	m_progress(0)  // 初始进度为 0
{
}

// 设置进度
void CircleProgressBar::setProgress(int progress)
{
	m_progress = qBound(0, progress, 100);  // 确保进度在 0-100 之间
	update();  // 触发重绘
}

// 重写绘制事件
void CircleProgressBar::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);  // 抗锯齿

	int width = this->width();
	int height = this->height();
	int size =60;  // 取宽高的最小值，确保是圆形
	int x = 10;    // 水平居中
	int y = 10;  // 垂直居中

									 // 绘制背景圆环
	QPen penBackground;
	penBackground.setColor(Qt::gray);          // 背景颜色
	penBackground.setWidth(10);                // 圆环宽度
	penBackground.setCapStyle(Qt::FlatCap);    // 端点样式
	painter.setPen(penBackground);
	painter.drawArc(x, y, size, size, 90 * 16, 360 * 16);  // 绘制背景圆环（360 度）

														   // 绘制进度圆环
	QPen penProgress;
	penProgress.setColor(QColor(0, 255, 191));            // 进度颜色
	penProgress.setWidth(10);                  // 圆环宽度
	penProgress.setCapStyle(Qt::FlatCap);      // 端点样式
	painter.setPen(penProgress);

	// 计算进度对应的角度
	int arcLength = static_cast<int>((m_progress / 100.0) * 360 * 16);  // 360 度对应 100%
	painter.drawArc(x, y, size, size, 90 * 16, -arcLength);  // 绘制进度圆环（逆时针绘制）

	// 绘制百分比文字
	painter.setPen(Qt::white);  // 文字颜色
	painter.setFont(QFont("", 14, QFont::Bold));  // 文字字体
	if (m_progress != 0)
	{
		QString text = QString("%1%").arg(m_progress);     // 百分比文字
		QRect textRect(x, y, size, size);
		painter.drawText(textRect, Qt::AlignCenter, text);  // 文字居中显示
	}
	else
	{
		QString text = QString("%1").arg(m_progress);     // 百分比文字
		QRect textRect(x, y, size, size);
		painter.drawText(textRect, Qt::AlignCenter, text);  // 文字居中显示
	}

}