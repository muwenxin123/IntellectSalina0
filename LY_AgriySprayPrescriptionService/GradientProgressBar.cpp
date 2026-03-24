#include "GradientProgressBar.h"
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

GradientProgressBar::GradientProgressBar(QWidget *parent) 
	: QWidget(parent)
{
	// 初始化动画（进度变化时平滑过渡）
	m_animation = new QPropertyAnimation(this, "progress", this);
	m_animation->setDuration(500);       // 动画时长（毫秒）
	m_animation->setEasingCurve(QEasingCurve::OutQuad); // 缓动曲线
}

void GradientProgressBar::setProgress(double progress, bool animated /*= true*/)
{
	double num = progress / m_endprogress;
 	m_progress = num;
	update();

}

void GradientProgressBar::setGradientColors(const QColor &startColor, const QColor &endColor)
{
	m_startColor = startColor;
	m_endColor = endColor;
	update();
}

void GradientProgressBar::setGradientSize(const double& lenth)
{
	m_endprogress = lenth;
}

void GradientProgressBar::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	// ===== 1. 绘制背景（灰色轨道）=====
	QRect backgroundRect = this->rect().adjusted(1, 1, -1, -1); // 缩小小边框
	QPainterPath backgroundPath;
	backgroundPath.addRoundedRect(backgroundRect, 8, 8);       // 圆角半径 8px

	QColor backgroundColor = QColor(40, 50, 60); // 背景轨道颜色
	painter.setPen(Qt::NoPen);
	painter.setBrush(backgroundColor);
	painter.drawPath(backgroundPath);

	// ===== 2. 绘制进度条（渐变）=====
	if (m_progress > 0) {
		
		QRect progressRect = backgroundRect;
		progressRect.setWidth(static_cast<int>(progressRect.width() * m_progress));

		QPainterPath progressPath;
		progressPath.addRoundedRect(progressRect, 8, 8);

		// 线性渐变（从左到右）
		QLinearGradient gradient(0, 0, progressRect.width(), 0);
		gradient.setColorAt(0, m_startColor);
		gradient.setColorAt(1, m_endColor);

		painter.setBrush(gradient);
		painter.drawPath(progressPath);
	}

	// ===== 3. 绘制滑块（右侧小方块）=====
	if (m_progress > 0) {
		// 滑块位置：进度条右侧
		int sliderX = static_cast<int>(backgroundRect.left() + backgroundRect.width() * m_progress);
		int sliderSize = 40; // 滑块大小
		int sliderY = backgroundRect.center().y() - sliderSize / 2;

		QRect sliderRect(sliderX - sliderSize / 4, sliderY, sliderSize / 4, sliderSize);
		QPainterPath sliderPath;
		sliderPath.addRect(sliderRect);

		QColor sliderColor = Qt::white; // 滑块颜色与进度条一致
		painter.setBrush(sliderColor);
		painter.drawPath(sliderPath);
	}
}











/////////////
CustomProgressBarLine::CustomProgressBarLine(QWidget *parent)
	: QWidget(parent)
{
	// 初始化动画（进度变化时平滑过渡）
	m_animation = new QPropertyAnimation(this, "progress", this);
	m_animation->setDuration(500);       // 动画时长（毫秒）
	m_animation->setEasingCurve(QEasingCurve::OutQuad); // 缓动曲线
}

void CustomProgressBarLine::setProgress(qreal progress, bool animated)
{
	m_progress = qBound(0.0, progress, 1.0);
	update();
}

void CustomProgressBarLine::setGradientColors(const QColor &startColor, const QColor &endColor)
{
	m_startColor = startColor;
	m_endColor = endColor;
	update();
}

void CustomProgressBarLine::setGradientSize(const double& lenth)
{
	m_endprogress = lenth;
}

void CustomProgressBarLine::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	// ===== 1. 绘制背景（灰色轨道）=====
	QRect backgroundRect = this->rect().adjusted(40, 5, -20, -40); // 左侧留空给标签
	QPainterPath backgroundPath;
	backgroundPath.addRoundedRect(backgroundRect, 0, 0);           // 圆角半径 8px

	QColor backgroundColor = QColor(40, 50, 60); // 背景轨道颜色
	painter.setPen(Qt::NoPen);
	painter.setBrush(backgroundColor);
	painter.drawPath(backgroundPath);

	// ===== 2. 绘制进度条（绿色渐变）=====
	//if (m_progress > 0) {
	QRect progressRect = backgroundRect;
	progressRect.setWidth(static_cast<int>(progressRect.width() * 1));

	QPainterPath progressPath;
	progressPath.addRoundedRect(progressRect, 0, 0);

	// 线性渐变（从左到右）
	QLinearGradient gradient(0, 0, progressRect.width(), 0);
	gradient.setColorAt(0, m_startColor);
	gradient.setColorAt(1, m_endColor);

	painter.setBrush(gradient);
	painter.drawPath(progressPath);
	//}

	// ===== 3. 绘制左侧标签（如 "A1"）=====
	QFont labelFont = painter.font();
	labelFont.setPixelSize(20);
	labelFont.setBold(true);
	painter.setFont(labelFont);

	QRect labelRect(0, 0, 30, height()); // 左侧标签区域
	painter.setPen(QColor(255, 255, 255));
	painter.drawText(labelRect, Qt::AlignVCenter | Qt::AlignRight, m_labelText);

	// ===== 4. 绘制刻度（如 0, 10, 20...60）=====
	QFont scaleFont = painter.font();
	scaleFont.setPixelSize(12);
	painter.setFont(scaleFont);

	int scaleCount = m_maxValue / 10; // 刻度数量（每10一个刻度）
	for (int i = 0; i <= scaleCount; ++i) {
		int scaleValue = i * 10;
		qreal scaleX = backgroundRect.left() + backgroundRect.width() * (static_cast<qreal>(scaleValue) / m_maxValue);

		// 绘制刻度线
		painter.drawLine(scaleX, backgroundRect.bottom() + 5, scaleX, backgroundRect.bottom() + 10);
		// 绘制刻度文本
		QRect textRect(scaleX - 20, backgroundRect.bottom() + 12, 40, 20);
		painter.drawText(textRect, Qt::AlignCenter, QString::number(scaleValue));
	}

	// ===== 5. 绘制指示线（当前进度对应的虚线）和下方刻度文本 =====
	if (m_progress > 0) {
		qreal indicatorX = backgroundRect.left() + backgroundRect.width() * m_progress;

		// 绘制虚线指示线
		painter.setPen(QPen(Qt::white, 3, Qt::DashLine));
		painter.drawLine(indicatorX, backgroundRect.top() - 40, indicatorX, backgroundRect.bottom() + 20);

		// 计算当前进度对应的刻度值
		int currentValue = static_cast<int>(m_progress * m_maxValue);

		// 设置文本样式
		QFont indicatorFont = painter.font();
		indicatorFont.setPixelSize(18);
		indicatorFont.setBold(true);
		painter.setFont(indicatorFont);

		// 绘制文本（在指示线正下方，刻度线下方）
		// 计算文本位置：指示线X坐标居中，在刻度文本下方一点
		QRect textRect(indicatorX - 30, backgroundRect.bottom() + 22, 60, 20);
		painter.setPen(m_endColor); // 白色文本
		painter.drawText(textRect, Qt::AlignCenter, QString::number(currentValue));
	}

}