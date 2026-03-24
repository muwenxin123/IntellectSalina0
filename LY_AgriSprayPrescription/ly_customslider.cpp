#include "ly_customslider.h"

LY_CustomSlider::LY_CustomSlider(QWidget *parent) : QWidget(parent)
{
	setMinimumHeight(30);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	// 初始化三个滑块的位置，范围 1 到 10
	//m_sliderPositions = { 2.0, 5.0, 8.0 };
	m_pressedSliderIndex = -1;
	m_minValue = 1;
	m_maxValue = 10;
}
void LY_CustomSlider::setMaxValue(int max_value)
{
	m_maxValue = max_value;
}
void LY_CustomSlider::setupSlider(std::vector<int> initialPositions)
{
	 m_sliderCount = initialPositions.size();
	m_sliderPositions.resize(m_sliderCount);

	if (initialPositions.empty()) {
		// 如果没有提供初始位置，均匀分布滑块
		for (int i = 0; i < m_sliderCount; ++i) {
			m_sliderPositions[i] = m_minValue + (i + 1.0) * (m_maxValue - m_minValue) / (m_sliderCount + 1);
		}
	}
	else {
		// 使用提供的初始位置
		for (int i = 0; i < std::min(m_sliderCount, static_cast<int>(initialPositions.size())); ++i) {
			m_sliderPositions[i] = initialPositions[i];
		}
	}
}
std::vector<int> LY_CustomSlider::getValues() const {
	std::vector<int> values;
	for (double pos : m_sliderPositions) {
		values.push_back(static_cast<int>(pos));
	}
	return values;
}
std::vector<std::pair<int, int>> LY_CustomSlider::getIntervals() const
{
	std::vector<std::pair<int, int>> intervals;
	intervals.emplace_back(m_minValue, static_cast<int>(m_sliderPositions[0]));
	for (int i = 0; i < m_sliderCount - 1; ++i) {
		intervals.emplace_back(static_cast<int>(m_sliderPositions[i]), static_cast<int>(m_sliderPositions[i + 1]));
	}
	intervals.emplace_back(static_cast<int>(m_sliderPositions[m_sliderCount - 1]), m_maxValue);
	return intervals;
}
void LY_CustomSlider::paintEvent(QPaintEvent *event)  {
	Q_UNUSED(event);
	QPainter painter(this);

	// 绘制滑块轨道
	int trackHeight = 6;
	int trackY = (height() - trackHeight) / 2;
	QRectF trackRect(0, trackY, width(), trackHeight);
	painter.setBrush(Qt::lightGray);
	painter.setPen(Qt::NoPen);
	painter.drawRoundedRect(trackRect, 3, 3);

	// 绘制滑块
	int sliderWidth = 16;
	int sliderHeight = 20;
	for (double pos : m_sliderPositions) {
		double relativePos = (pos - m_minValue) / (m_maxValue - m_minValue);
		int sliderX = static_cast<int>(relativePos * (width() - sliderWidth));
		int sliderY = (height() - sliderHeight) / 2;
		QRectF sliderRect(sliderX, sliderY, sliderWidth, sliderHeight);
		painter.setBrush(Qt::blue);
		painter.setPen(Qt::NoPen);
		painter.drawRoundedRect(sliderRect, 3, 3);
	}
}
void LY_CustomSlider::mousePressEvent(QMouseEvent *event) {
	int sliderWidth = 16;
	m_pressedSliderIndex = -1;
	for (int i = 0; i < m_sliderCount; ++i) {
		double relativePos = (m_sliderPositions[i] - m_minValue) / (m_maxValue - m_minValue);
		int sliderX = static_cast<int>(relativePos * (width() - sliderWidth));
		int sliderY = (height() - 20) / 2;
		QRectF sliderRect(sliderX, sliderY, sliderWidth, 20);
		if (sliderRect.contains(event->pos())) {
			m_pressedSliderIndex = i;
			break;
		}
	}
}
void LY_CustomSlider::mouseMoveEvent(QMouseEvent *event)  {
	if (m_pressedSliderIndex != -1) {
		double relativePos = static_cast<double>(event->x()) / width();
		double newPos = m_minValue + relativePos * (m_maxValue - m_minValue);
		newPos = qBound(static_cast<double>(m_minValue), newPos, static_cast<double>(m_maxValue));

		// 确保滑块不重叠
		if (m_pressedSliderIndex > 0) {
			newPos = std::max(newPos, m_sliderPositions[m_pressedSliderIndex - 1] + 0.001);
		}
		if (m_pressedSliderIndex < m_sliderCount - 1) {
			newPos = std::min(newPos, m_sliderPositions[m_pressedSliderIndex + 1] - 0.001);
		}

		m_sliderPositions[m_pressedSliderIndex] = newPos;
		update();

		// 发出信号通知值已改变
		emit valuesChanged(getValues());
		emit intervalsChanged(getIntervals());
	}
}
void LY_CustomSlider::mouseReleaseEvent(QMouseEvent *event) {
	Q_UNUSED(event);
	m_pressedSliderIndex = -1;
}