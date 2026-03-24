#pragma once

#include <QWidget>

class QStackedWidget;
class QRadioButton;

class TreeChartWidget : public QWidget
{
	Q_OBJECT

public:
	TreeChartWidget(QWidget *parent = nullptr);
	~TreeChartWidget();

protected:
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	QStackedWidget *m_pStackedWidget;
	QRadioButton *m_pChart1Btn;
	QRadioButton *m_pChart2Btn;
	int m_ypos;
	int m_width;
};
