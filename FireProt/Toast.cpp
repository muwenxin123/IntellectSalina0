#include "Toast.h"
#include <QPropertyAnimation>
#include <QScreen>
#include <QGuiApplication>
#include <QPainter>
#include <QTimer>
#include <QLabel>
#include <QBoxLayout>

Toast::Toast(QWidget *parent)
    : QWidget(parent)
{
	m_pLabel = new QLabel(this);
	m_pLabel->setMinimumHeight(52);
	m_pLabel->setStyleSheet("background-color:rgba(0,0,0,0.80);border-radius:26px;color:#FFFFFF;font-family:simhei;font-size:22px;padding-left:25px;padding-right:25px;");
	QHBoxLayout *pLayout = new QHBoxLayout(this);
	pLayout->addWidget(m_pLabel);

	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
}

Toast::~Toast()
{
}

void Toast::setText(const QString& text)
{
    m_pLabel->setText(text);
}

void Toast::showAnimation(int timeout )
{

    QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
    animation->setDuration(1000);
    animation->setStartValue(0);
    animation->setEndValue(1);
    animation->start();
    show();

    QTimer::singleShot(timeout, [&]
    {

        QPropertyAnimation *animation = new QPropertyAnimation(this, "windowOpacity");
        animation->setDuration(1000);
        animation->setStartValue(1);
        animation->setEndValue(0);
        animation->start();
        connect(animation, &QPropertyAnimation::finished, [&]
        {
            close();
            deleteLater();
        });
    });
}

void Toast::showTip(const QString& text, QWidget* parent )
{
    Toast* toast = new Toast(parent);
    toast->setWindowFlags(toast->windowFlags() | Qt::WindowStaysOnTopHint);
    toast->setText(text);
    toast->adjustSize();

    QScreen* pScreen = QGuiApplication::primaryScreen();
    toast->move((pScreen->size().width() - toast->width()) / 2, pScreen->size().height() * 7 / 10);
    toast->showAnimation();
}

void Toast::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    paint.begin(this);
    auto kBackgroundColor = QColor(255, 255, 255);
    kBackgroundColor.setAlpha(0.0 * 255);
    paint.setRenderHint(QPainter::Antialiasing, true);
    paint.setPen(Qt::NoPen);
    paint.setBrush(QBrush(kBackgroundColor, Qt::SolidPattern));
    paint.drawRect(0, 0, width(), height());
    paint.end();
}
