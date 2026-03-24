#include "UAVCurrentShotAiming.h"

#include <QVBoxLayout>
#include <QPainter>
#include <QDebug>
#include <QTimer>
#include <QMouseEvent>

#include "QCString.h"

UAVCurrentShotAiming::UAVCurrentShotAiming(QWidget *parent)
    : QWidget(parent)
    , m_show(true)
    , m_TitleLabel()
    , m_YawLabel()
    , m_DistLabel()
    , m_Yaw()
    , m_Dist()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(&m_TitleLabel);
    layout->addWidget(&m_YawLabel);
    layout->addWidget(&m_DistLabel);
    layout->addStretch();
    setLayout(layout);

    m_TitleLabel.setAlignment(Qt::AlignCenter);
    m_TitleLabel.setText(tr("<b>Current aiming:</b>"));

    m_YawLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_YawLabel.setText(tr2("azimuth(бу):"));

    m_DistLabel.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    m_DistLabel.setText(tr("distance(m):"));
}

UAVCurrentShotAiming::~UAVCurrentShotAiming()
{

}

void UAVCurrentShotAiming::updateTitleAndYawAndDist(const std::string title, const double yaw, const double dist, bool stop)
{
    if (qAbs(m_Yaw - yaw) > 0.000001 || qAbs(m_Dist - dist) > 0.000001 || m_Title != title)
    {
        m_Title = title;
        m_Yaw   = yaw;
        m_Dist  = dist;
        m_Stop  = stop;
        if (m_show)
        {
            update();

            if (m_Stop)
            {
                m_TitleLabel.setText(tr("<b>Current aiming:</b>"));
                m_YawLabel.setText(tr2("azimuth(бу):"));
                m_DistLabel.setText(tr("distance(m):"));
            }
            else
            {
                m_TitleLabel.setText(tr("<b>Current aiming:</b>") + QString::fromStdString(title));
                m_YawLabel.setText(tr2("azimuth(бу):")  + QString::number(m_Yaw));
                m_DistLabel.setText(tr("distance(m):") + QString::number(m_Dist));
            }
        }
    }
}

void UAVCurrentShotAiming::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        break;
    default:
        break;
    }
}

void UAVCurrentShotAiming::paintEvent(QPaintEvent *event)
{
    const int space = 30;

    QRect rectTemp  = this->rect();
    rectTemp.setTop(m_DistLabel.pos().ry() + m_DistLabel.height());
    rectTemp.adjust(space, space, -space, -space);

    const QRect &rect   = rectTemp;
    const int x         = rect.x();
    const int y         = rect.y();
    const int width     = rect.width();
    const int height    = rect.height();
    const QPoint center = rect.center();
    const int radius    = qMin(rect.width(), rect.height()) / 2;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen = painter.pen();
    pen.setColor(Qt::yellow);
    pen.setStyle(Qt::SolidLine);
    pen.setWidth(2);
    painter.setPen(pen);

    painter.drawLine(center.x() - radius, center.y(), center.x() + radius, center.y());
    painter.drawLine(center.x(), center.y()  - radius, center.x(), center.y() + radius);

    painter.setPen(pen);
    painter.drawEllipse(center, radius, radius);

    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);
    painter.drawEllipse(center, radius / 2, radius / 2);

    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    pen.setColor(Qt::green);
    painter.setPen(pen);
    painter.setBrush(QBrush(Qt::yellow));
    painter.drawEllipse(center, 6, 6);

    painter.setBrush(Qt::NoBrush);
    pen.setColor(Qt::yellow);
    painter.setPen(pen);
    painter.setFont(QFont("Arial", 11));
    painter.drawText(center.x() + radius / 2 - 35, center.y() - 5, "20m");
    painter.drawText(center.x() + radius     - 35, center.y() - 5, "40m");

    painter.drawText(center.x()          - 4,  center.y() - radius - 5,  "0");
    painter.drawText(center.x() + radius + 2,  center.y()          + 5,  "90");
    painter.drawText(center.x()          - 12, center.y() + radius + 15, "180");
    painter.drawText(center.x() - radius - 26, center.y()          + 5,  "270");

    if (!m_Stop)
    {
        const int distSum = 40;
        const double targetDist = m_Dist * radius / distSum;

        QPointF targetPos(center.x() - targetDist, center.y());

        QLineF line(center, targetPos);
        line.setAngle(- (m_Yaw - 90));

        pen.setColor(Qt::red);
        pen.setWidth(2);
        painter.setPen(pen);
        painter.setBrush(QBrush(Qt::red));
        painter.drawEllipse(line.p2(), 3, 3);

        pen.setStyle(Qt::DashLine);
        pen.setColor(Qt::lightGray);
        painter.setPen(pen);
        painter.drawLine(line);
    }
}

void UAVCurrentShotAiming::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    m_show = true;
}

void UAVCurrentShotAiming::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    m_show = false;
}
