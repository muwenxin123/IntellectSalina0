#include "WaveformWidget.h"
#include <QPainter>
#include <QRandomGenerator>

// 构造函数
WaveformWidget::WaveformWidget(QWidget *parent)
	: QWidget(parent),
	m_fillColor(Qt::cyan)
{
	generateWaveform();  // 生成演示波形数据
}

// 设置波形数据（外部调用时可替换数据）
void WaveformWidget::setWaveformData(const QVector<qreal> &data)
{
	m_waveformData = data;
	update();  // 触发重绘
}

// 生成演示波形数据（模拟随机波形）
void WaveformWidget::generateWaveform()
{
	m_waveformData.clear();
	int sampleCount = 100;  // 采样点数量
	qreal baseline = height() / 2.0;  // 基准线（中线）
	qreal maxAmplitude = baseline * 0.8;  // 最大振幅
	QVector<qreal> data{ 4.7,5,5.2,5.1,5,4.5,4.8,5.1,5,5.2,5.1,5,4.5,4.8,5.1,5,5.2,5.1,5,4.5,4.8,5.1,5,5.2,5.1,5,4.5,4.8,5.1 };
	m_waveformData = data;
}

// 重写绘制事件
void WaveformWidget::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true); // 抗锯齿

	if (m_waveformData.isEmpty()) return;

	// 1. 数据映射：将原始数据适配到控件宽高
	// 1.1 计算数据范围
	qreal minVal = *std::min_element(m_waveformData.begin(), m_waveformData.end());
	qreal maxVal = *std::max_element(m_waveformData.begin(), m_waveformData.end());
	qreal dataRange = maxVal - minVal;
	if (dataRange < 1e-6) dataRange = 1; // 避免除零

										 // 1.2 定义波形垂直范围（基准线 + 振幅，让波形“悬浮”）
	qreal baselineY = height() * 0.5;   // 基准线（控件中下部，可调整）
	qreal amplitude = height() * 0.4;   // 波形振幅（控制波动范围，可调整）

										// 2. 构建波形路径（三次贝塞尔曲线）
	QPainterPath wavePath;
	int pointCount = m_waveformData.size();

	// 2.1 起点：从左侧底部平缓开始（可根据需求调整）
	wavePath.moveTo(0, baselineY);

	for (int i = 0; i < pointCount; ++i) {
		// 当前点坐标（X 均匀分布，Y 映射到控件高度）
		qreal x = static_cast<qreal>(i) * width() / (pointCount - 1);
		qreal yRatio = (m_waveformData.at(i) - minVal) / dataRange;
		qreal y = baselineY - amplitude / 2 + yRatio * amplitude;

		if (i == 0) {
			// 第一个点，直接移动到起点
			wavePath.moveTo(x, y);
		}
		else {
			// 计算控制点：用前后点的“中点趋势”让曲线平滑
			QPointF prevPoint = wavePath.currentPosition();
			QPointF currPoint(x, y);

			// 简单做法：取前后点的中点作为控制点（可优化为更复杂的算法）
			QPointF control1 = (prevPoint + currPoint) / 2;
			QPointF control2 = control1;

			// 三次贝塞尔曲线连接
			wavePath.cubicTo(control1, control2, currPoint);
		}
	}

	// 2.2 闭合路径：回到底部，形成填充区域
	wavePath.lineTo(width(), height());
	wavePath.lineTo(0, height());
	wavePath.closeSubpath();

	// 3. 填充渐变（模拟示例图的“底部渐变”效果）
	QLinearGradient gradient(0, 0, 0, height());
	gradient.setColorAt(0, QColor(0, 255, 191, 50));   // 顶部淡色
	gradient.setColorAt(1, QColor(0, 255, 191, 150));  // 底部深色
	painter.setBrush(gradient);
	painter.drawPath(wavePath);

	// 4. 绘制波形描边（突出线条，接近示例图的高亮效果）
	QPen wavePen(QColor(0, 255, 191));
	wavePen.setWidth(2);
	wavePen.setCapStyle(Qt::RoundCap);   // 圆角端点，更平滑
	painter.setPen(wavePen);
	painter.drawPath(wavePath);

}
