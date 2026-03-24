#include "LY_AgriFertiliResultWidget.h"
#include "ui_LY_AgriFertiliResultWidget.h"
#include <QLabel>
#include "QCString.h"
#include <QVBoxLayout>
#include <QDebug>

LY_AgriFertiliResultWidget::LY_AgriFertiliResultWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_AgriFertiliResultWidget)
{

	ui->setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
	//setAttribute(Qt::WA_TranslucentBackground);
	setAttribute(Qt::WA_ShowWithoutActivating);

	this->setStyleSheet("background-color: #162130};");
	ui->widget->setObjectName("parentWidget");
	ui->widget->setStyleSheet("QWidget#parentWidget {background-image: url(:/images/AgriFarm/biaopaiBgk.png)};");

	 QLabel *m_titleLabel = new QLabel(this);
	 m_titleLabel->setText(tr2("施肥结果"));
	m_titleLabel->setStyleSheet("font-size: 30px;");
	m_titleLabel->move(96, 25);
	m_titleLabel->setFixedSize(135, 32);

	ui->label->setWordWrap(true);
	ui->label_2->setWordWrap(true);
	ui->label_3->setWordWrap(true);
	ui->label_4->setWordWrap(true);

	ui->label->setStyleSheet("font-size: 21px;");
	ui->label_2->setStyleSheet("font-size: 20px;");
	ui->label_3->setStyleSheet("font-size: 20px;");
	ui->label_4->setStyleSheet("font-size: 20px;");

}
void LY_AgriFertiliResultWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		QApplication::setOverrideCursor(Qt::OpenHandCursor);
		m_dragPosition = event->globalPos() - frameGeometry().topLeft();
		m_dragging = true;
	}
}

void LY_AgriFertiliResultWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (m_dragging && (event->buttons() & Qt::LeftButton)) {
		QApplication::setOverrideCursor(Qt::ClosedHandCursor);
		move(event->globalPos() - m_dragPosition);
	}
}

void LY_AgriFertiliResultWidget::mouseReleaseEvent(QMouseEvent *event)
{
	Q_UNUSED(event);
	QApplication::setOverrideCursor(Qt::ArrowCursor);
	m_dragging = false;
}
LY_AgriFertiliResultWidget::~LY_AgriFertiliResultWidget()
{
    delete ui;
}
