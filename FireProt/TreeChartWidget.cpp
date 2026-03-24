#include "TreeChartWidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QStackedWidget>
#include <QPainter>
#include <QRadioButton>
#include <QHBoxLayout>
#include "LyTreeChart.h"

TreeChartWidget::TreeChartWidget(QWidget *parent)
	: QWidget(parent)
	, m_ypos(8)
	, m_width(50)
{
	m_pStackedWidget = new QStackedWidget(this);
	m_pChart1Btn = new QRadioButton(this);
	m_pChart1Btn->setObjectName("Chart1Btn");
	m_pChart1Btn->setText(tr("Organization"));
	m_pChart1Btn->setChecked(true);

	m_pChart2Btn = new QRadioButton(this);
	m_pChart2Btn->setObjectName("Chart2Btn");
	m_pChart2Btn->setText(tr("Plan"));

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(m_pStackedWidget);

	m_pStackedWidget->addWidget(LyTreeChart::getChart(1));
	m_pStackedWidget->addWidget(LyTreeChart::getChart(0));

	connect(m_pChart1Btn, &QRadioButton::clicked, this, [=]() {
		m_pStackedWidget->setCurrentIndex(0);
	});
	connect(m_pChart2Btn, &QRadioButton::clicked, this, [=]() {
		m_pStackedWidget->setCurrentIndex(1);
	});

	const QRect &rect = QApplication::desktop()->screenGeometry(QCursor::pos());
	if (rect.width() == 2560 && rect.height() == 1440){
		m_width = 60;
		m_ypos = 15;
	}

	m_pChart1Btn->setFixedWidth(m_width);
	m_pChart2Btn->setFixedWidth(m_width);
}

TreeChartWidget::~TreeChartWidget()
{
}

void TreeChartWidget::resizeEvent(QResizeEvent *event)
{
	m_pChart1Btn->move(width() - m_pChart1Btn->width() - m_pChart1Btn->width(), m_ypos);
	m_pChart2Btn->move(m_pChart1Btn->x() + m_pChart1Btn->width(), m_ypos);

	QWidget::resizeEvent(event);
}

void TreeChartWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);
	QStyleOption opt;
	opt.init(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);

	QWidget::paintEvent(event);
}