#ifndef LY_CUSTOMSLIDER_H
#define LY_CUSTOMSLIDER_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <vector>
#include <algorithm>

class LY_CustomSlider : public QWidget
{
    Q_OBJECT
public:


	//TripleSlider(int sliderCount = 3, const std::vector<double>& initialPositions = {}, QWidget *parent = nullptr)
	//	: QWidget(parent), m_sliderCount(sliderCount), m_pressedSliderIndex(-1), m_minValue(1), m_maxValue(10) {
	//	setMinimumHeight(30);
	//	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    explicit LY_CustomSlider(QWidget *parent = nullptr);

	// 获取三个滑块的当前值
	std::vector<int> getValues() const;

	void setMaxValue(int max_value);

	// 获取区间值
	std::vector<std::pair<int, int>> getIntervals() const;

	void setupSlider(std::vector<int> initialPositions);

protected:
	void paintEvent(QPaintEvent *event);
	void mousePressEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

private:
	int m_sliderCount;
	std::vector<double> m_sliderPositions;
	int m_pressedSliderIndex;
	int m_minValue;
	int m_maxValue;

signals:

	// 自定义信号，当滑块值改变时发出
	void valuesChanged(const std::vector<int>& values);
	// 自定义信号，当区间值改变时发出
	void intervalsChanged(const std::vector<std::pair<int, int>>& intervals);

public slots:
};

#endif // LY_CUSTOMSLIDER_H