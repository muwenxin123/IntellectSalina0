#ifndef GRADIENTPROGRESSBAR_H
#define GRADIENTPROGRESSBAR_H

#include <QWidget>
#include <QPropertyAnimation>
//滑块类型的进度条
class GradientProgressBar : public QWidget
{
    Q_OBJECT
public:
	explicit GradientProgressBar(QWidget *parent = nullptr);

	// 设置进度（0.0 ~ 1.0）
	void setProgress(double progress, bool animated = true);

	// 设置渐变颜色（可选）
	void setGradientColors(const QColor &startColor, const QColor &endColor);

	//设置进度条长度
	void setGradientSize(const double& lenth);

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	double		m_progress = 0.0;              // 当前进度（0~1）
	double     m_endprogress=10;//进度条最终值
	QColor m_startColor = QColor(0, 160, 133); // 渐变起始色
	QColor m_endColor = QColor(0, 255, 191);   // 渐变结束色
	QPropertyAnimation *m_animation = nullptr; // 进度动画
};
//虚线指示的进度条
class CustomProgressBarLine : public QWidget
{
	Q_OBJECT

public:
	explicit CustomProgressBarLine(QWidget *parent = nullptr);

	// 设置进度（0.0 ~ 1.0）
	void setProgress(qreal progress, bool animated = true);

	// 设置标签文本（如 "A1"）
	void setLabelText(const QString &text) { m_labelText = text; update(); }

	// 设置刻度最大值（如 60）
	void setMaxValue(int maxValue) { m_maxValue = maxValue; update(); }

	// 设置渐变颜色（可选）
	void setGradientColors(const QColor &startColor, const QColor &endColor);

	//设置进度条长度
	void setGradientSize(const double& lenth);

protected:
	void paintEvent(QPaintEvent *event) override;

private:
	qreal m_progress = 0.0;              // 当前进度（0~1）
	double m_endprogress = 100;
	QString m_labelText = "A1";          // 左侧标签文本
	int m_maxValue = 100;                 // 刻度最大值
	QColor m_startColor = QColor(0, 160, 133); // 渐变起始色
	QColor m_endColor = QColor(0, 255, 191);   // 渐变结束色
	QPropertyAnimation *m_animation = nullptr; // 进度动画
};



#endif // GRADIENTPROGRESSBAR_H