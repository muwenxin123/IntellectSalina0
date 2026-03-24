#include "UAVPlaneAttitude.h"
#include "ui_UAVPlaneAttitude.h"

#include <QtMath>

UAVPlaneAttitude::UAVPlaneAttitude(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVPlaneAttitude)
    , m_strWeaponComponentName("")
    , m_strWeaponComponentID("")
    , m_Pitch(0)
    , m_Roll(0)
    , m_Yaw(0)
    , m_Height(0.0)
    , m_Speed(0.0)
    , m_show(true)
{
    ui->setupUi(this);
}

UAVPlaneAttitude::~UAVPlaneAttitude()
{
    delete ui;
}

void UAVPlaneAttitude::updateTelemetryInformation(const QString &strWeaponComponentName, const QString &strWeaponComponentID, const telemetryInformation &telemetryInfo)
{
    m_strWeaponComponentName = strWeaponComponentName;
    m_strWeaponComponentID   = strWeaponComponentName;

    setAttitude(telemetryInfo.angleofPitch, telemetryInfo.angleofRoll, telemetryInfo.angleofNose, telemetryInfo.relativeOriginHeigth, telemetryInfo.groundVelocity);
}

void UAVPlaneAttitude::updateAttitude()
{
    if (!m_show)
    {
        return ;
    }
    update();
}

void UAVPlaneAttitude::setAttitude(float pitch, float roll, float yaw, double height, float speed)
{
    m_Pitch = pitch;

    m_Roll = roll;

    m_Yaw = yaw;
    if (m_Yaw < 0.0)
    {
        m_Yaw += 360.0;
    }

    m_Height = height;
    m_Speed  = speed;
}

void UAVPlaneAttitude::pitch_angle(QPainter &painter)
{
    const QPoint  center = rect().center();
    const QRect &rect   = this->rect();
    const int width      = rect.width();
    const int height     = rect.height();
    const int x          = rect.x();
    const int y          = rect.y();

    const int top_x = (x + width) / 2  - width / 2;
    const int top_y = (y + height) / 2 - 80 - height / 2;

    const int center_y = (y + height) / 2 - height / 2;

    const int bottom_y = (y + height) / 2 + 80 - height / 2;

    const int keDu_left = 40;
    const qreal left_v_Dist = (bottom_y - top_y) * 1.0 / (keDu_left - 1);
    int sum = 20;

    int pitch = qRound(m_Pitch * (center_y - top_y) / 20.0f);

    painter.save();

    painter.translate(center);
    painter.rotate(-m_Roll);

    const int wh = qMax(width, height) * 2;

    painter.fillRect(-wh, -wh + pitch,  wh * 2, wh, QColor(61, 89, 171));

    painter.fillRect(-wh,    0 + pitch,  wh * 2, wh, QColor(115, 74, 18));

    painter.setPen(QPen(QColor(0, 255, 0), 2));
    for (int i = 1; i <= keDu_left; i++)
    {
        if (i == 1)
        {
            const QPointF L1(top_x - 20, top_y + left_v_Dist * i + pitch);
            const QPointF L2(top_x + 20, top_y + left_v_Dist * i + pitch);
            painter.drawLine(L1, L2);
            painter.drawText(QPointF(L1.x() - 20, L2.y() + 5), QString::number(sum));
            painter.drawText(QPointF(L1.x() + 45, L2.y() + 5), QString::number(sum));
            sum -= 10;
        }
        if (i % 10 == 0)
        {
            const QPointF L1(top_x - 20, top_y + left_v_Dist * i + pitch);
            const QPointF L2(top_x + 20, top_y + left_v_Dist * i + pitch);
            painter.drawLine(L1, L2);
            if (sum > 0)
            {
                painter.drawText(QPointF(L1.x() - 20, L2.y() + 5), QString::number(sum));
                painter.drawText(QPointF(L1.x() + 45, L2.y() + 5), QString::number(sum));
                sum -= 10;
            }
            else if (sum == 0)
            {
                painter.drawText(QPointF(L1.x() - 10, L2.y() + 5), QString::number(sum));
                painter.drawText(QPointF(L1.x() + 50, L2.y() + 5), QString::number(sum));
                sum -= 10;
            }
            else
            {
                painter.drawText(QPointF(L1.x() - 25, L2.y() + 5), QString::number(sum));
                painter.drawText(QPointF(L1.x() + 45, L2.y() + 5), QString::number(sum));
                sum -= 10;
            }
        }
        else if (i % 5 == 0 && i % 10 != 0)
        {
            const QPointF L1(top_x - 15, top_y + left_v_Dist * i + pitch);
            const QPointF L2(top_x + 15, top_y + left_v_Dist * i + pitch);
            painter.drawLine(L1, L2);
        }
        else
        {
            const QPointF L1(top_x - 10, top_y + left_v_Dist * i + pitch);
            const QPointF L2(top_x + 10, top_y + left_v_Dist * i + pitch);
            painter.drawLine(L1, L2);
        }

    }

    painter.restore();

    const int top_Pen_Width  = 2;
    painter.setPen(QPen(QColor(Qt::GlobalColor::yellow), top_Pen_Width));
    painter.drawLine(center.x(), center.y() - 30, center.x(), center.y() - 10);
    painter.drawLine(center.x() - 45, center.y(), center.x()  - 30,  center.y());
    painter.drawLine(center.x() + 30,  center.y(), center.x() + 45, center.y());

    painter.drawText(QPointF(center.x() - 25.0, center.y() + 5), QString::number(m_Pitch, 'f', 2));
}

void UAVPlaneAttitude::rollMedian(QPainter &painter)
{
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(0, 255, 0), 5));

    const QRect &rect = this->rect();

    const int width    = rect.width();
    const int height   = rect.height();
    const QPoint center = rect.center();

    painter.setPen(QPen(QColor(255, 222, 173), 2));
    const qreal r1 = qMin(width, height) / 2.0 - 10;
    const qreal r2 = qMin(height, width)  / 2.0 - 20;

    painter.setBrush(QBrush(QColor(255, 215, 0)));
    painter.setPen(QPen(QColor(255, 215, 0), 2));
    QPolygon triangle1;
    painter.save();
    painter.translate(center);
    painter.rotate(45);
    for (int angleRotate = -45; angleRotate <= 45; angleRotate += 5)
    {
        QPointF p1(0, r1);

        qreal r3 = r2;
        if ((angleRotate % 10) == 0)
        {
            r3 = r3 - 10;

        }
        QPointF p2(0, r3);

        painter.drawLine(p1, p2);
        painter.rotate(-5);
    }
    painter.restore();

    painter.save();
    painter.translate(center);
    painter.rotate(-m_Roll);
    const int rr1 = r1;
    triangle1.setPoints(3, 0 - 15 + 0, 0 + rr1 - 45,
                        0 + 15 + 0, 0 + rr1 - 45,
                        0 + 0,      0 + rr1 - 35);
    painter.drawPolygon(triangle1);
    painter.drawText(QPointF(0 - 15 + 0, 0 + rr1 - 50), QString::number(m_Roll, 'f', 2));
    painter.restore();
}

void UAVPlaneAttitude::paintEvent(QPaintEvent *event)
{
    const QRect &rect = this->rect();
    const int x        = rect.x();
    const int y        = rect.y();
    const int width    = rect.width();
    const int height   = rect.height();
    const QPoint center = rect.center();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    pitch_angle(painter);

    const int yaw = m_Yaw;
    int top_v_size1 = yaw / 30;
    int top_v_size2 = yaw % 30;
    int yaw1 = top_v_size1 * 30;

    const int top_x_Dist   = 70;
    const int top_y_Dist   = 30;
    const int top_x_left   = x + top_x_Dist;
    const int top_y_left   = y + top_y_Dist;
    const int top_x_right  = x + width - top_x_Dist;

    const int top_v_num    = 7;
    const int top_v_height = 15;
    int num                = -90 + yaw1;
    const qreal top_v_Dist = (top_x_right - top_x_left) * 1.0 / (top_v_num - 1);

    int movex = (top_v_size2 / 30.0 * top_v_Dist);

    const int top_v_x      = top_x_left - movex;
    const int top_v_y      = top_y_left;

    painter.setPen(QPen(QColor(0, 255, 0), 2));
    QFont serifFont1("ËÎÌו", 10);
    painter.setFont(serifFont1);
    for (int i = 0; i < top_v_num; i++)
    {
        const QPointF p1(top_v_x + top_v_Dist * i, top_v_y);
        const QPointF p2(top_v_x + top_v_Dist * i, top_v_y + top_v_height);
        painter.drawLine(p1, p2);
        if (num > 360)
        {
            num = 0;
        }

        if (num >= 0)
        {
            painter.drawText(QPointF(p1.x() - 5, p1.y() - 5), QString::number(num));
        }
        else
        {
            painter.drawText(QPointF(p1.x() - 10, p1.y() - 5), QString::number(num + 360));
        }
        num += 30;
    }

    const int top_Pen_Width  = 2;
    const int top_h_x        = top_x_left;
    const int top_h_y        = top_v_y + top_v_height + top_Pen_Width * 2;
    painter.setPen(QPen(QColor(128, 128, 135), top_Pen_Width));
    painter.drawLine(top_h_x, top_h_y, top_x_right, top_h_y);

    painter.setBrush(QBrush(QColor(128, 128, 105)));
    painter.setPen(QPen(QColor(128, 128, 105), 2));
    QPolygon triangle;
    triangle.setPoints(3, center.x() - 30, y + 70, center.x() + 30, y + 70, center.x(), y + 50);
    painter.drawPolygon(triangle);

    painter.setPen(QPen(QColor(Qt::GlobalColor::yellow), top_Pen_Width));
    painter.drawText(QPointF(center.x() - 20, y + 65), QString::number(m_Yaw, 'f', 2));

    const int top_x_Alt   = x + 5;
    const int top_y_Alt   = y + 15;
    const int top_x_IAS   = x + width - 55;
    const int top_y_IAS   = y + 15;

    painter.setPen(QColor(Qt::GlobalColor::yellow));
    serifFont1.setWeight(QFont::Bold);
    painter.setFont(serifFont1);
    painter.drawText(QPoint(top_x_Alt,  top_y_Alt),   "Alt[m]");
    painter.drawText(QPoint(top_x_IAS,  top_y_IAS),   "IAS[m/s]");

    {

        int left_top_Height = 200;
        const int keDu_left    = 5;
        const int top_x         = x + 80;
        const int top_y         = y + 70;
        const int mid_x         = top_x;
        const int mid_y         = y + height / 2;
        const int bottom_x      = top_x;
        const int bottom_y      = (y + height) - 70;

        const qreal left_v_Dist = (bottom_y - top_y) * 1.0 / (keDu_left - 1);

        painter.setPen(QPen(QColor(0, 255, 0), 5));
        painter.drawLine(top_x, top_y, bottom_x, bottom_y);
        int l_sum = 200;
        int l_sum_step = 100;
        if (m_Height > l_sum)
        {
            if (m_Height < 1000)
            {
                l_sum = 1000;
                l_sum_step = 500;
            }
            else if (m_Height < 1000)
            {
                l_sum = 10000;
                l_sum_step = 5000;
            }
            else if (m_Height < 10000)
            {
                l_sum = 100000;
                l_sum_step = 50000;
            }
            else
            {
                l_sum = qCeil(m_Height / 1000) * 1000;
                l_sum_step = l_sum / 2;
            }
        }

        left_top_Height = l_sum;

        for (int i = 0; i < keDu_left; i++)
        {
            if (i < 2)
            {
                painter.setPen(QPen(QColor(0, 255, 0), 3));
                const QPointF L1(top_x - 10, top_y + left_v_Dist * i);
                const QPointF L2(top_x, top_y + left_v_Dist * i);
                painter.drawLine(L1, L2);
                painter.drawText(QPointF(L1.x() - 25 - 6, L2.y() + 5), QString::number(l_sum));
                l_sum -= l_sum_step;
            }
            else if (i == 2)
            {
                painter.setPen(QPen(QColor(0, 0, 0), 3));
                const QPointF L1(mid_x - 10, (y + height) / 2);
                const QPointF L2(mid_x, (y + height) / 2);
                painter.drawLine(L1, L2);
                painter.drawText(QPointF(L1.x() - 25 - 6, L2.y() + 5), QString::number(0.0));
                l_sum -= l_sum_step;
            }
            else
            {
                painter.setPen(QPen(QColor(255, 0, 0), 3));
                const QPointF L1(top_x - 10, top_y + left_v_Dist * i);
                const QPointF L2(top_x, top_y + left_v_Dist * i);
                painter.drawLine(L1, L2);
                painter.drawText(QPointF(L1.x() - 30 - 6, L2.y() + 5), QString::number(l_sum));
                l_sum -= l_sum_step;
            }
        }
        painter.setPen(QPen(QColor(255, 0, 0), 4));
        painter.drawLine(mid_x, mid_y, bottom_x, bottom_y);

        {
            int angleX = top_x + 5;
            int angleY = mid_y - m_Height * ((mid_y - 70.0) / left_top_Height);

            painter.setBrush(QBrush(QColor(255, 215, 0, 180)));
            painter.setPen(QPen(QColor(255, 215, 0, 180), 0));
            QPolygon triangle;
            triangle.setPoints(3, angleX, angleY, angleX + 10, angleY - 10, angleX + 10, angleY + 10);
            painter.drawPolygon(triangle);

            painter.setPen(QPen(QColor(Qt::GlobalColor::yellow), top_Pen_Width));
            painter.drawText(QPointF(angleX + 13, angleY + 5), QString::number(m_Height, 'f', 2));
        }

    }

    {
        const int  keDu_right   = 5;
        const int top_x         = (x + width) - 80;
        const int top_y         = y + 70;
        const int mid_x         = (x + width) - 80;
        const int mid_y         = (y + height) / 2;
        const int bottom_x      = (x + width) - 80;
        const int bottom_y      = (y + height) - 70;

        const qreal right_v_Dist = (bottom_y - top_y) * 1.0 / (keDu_right - 1);

        painter.setPen(QPen(QColor(0, 255, 0), 5));
        painter.drawLine(top_x, top_y, bottom_x, bottom_y);
        int r_sum = 20;
        for (int i = 0; i < keDu_right; i++)
        {
            if (i < 2)
            {
                painter.setPen(QPen(QColor(0, 255, 0), 3));
                const QPointF L1(top_x, top_y + right_v_Dist * i);
                const QPointF L2(top_x + 10, top_y + right_v_Dist * i);
                painter.drawLine(L1, L2);
                painter.drawText(QPointF(L1.x() + 15, L2.y() + 4), QString::number(r_sum));
                r_sum -= 10;
            }
            else if (i == 2)
            {
                painter.setPen(QPen(QColor(0, 0, 0), 3));
                const QPointF L1(mid_x, (y + height) / 2);
                const QPointF L2(mid_x + 10, (y + height) / 2);
                painter.drawLine(L1, L2);
                painter.drawText(QPointF(L1.x() + 15, L2.y() + 4), QString::number(0.0));
                r_sum -= 10;
            }
            else
            {
                painter.setPen(QPen(QColor(255, 0, 0), 3));
                const QPointF L1(top_x, top_y + right_v_Dist * i);
                const QPointF L2(top_x + 10, top_y + right_v_Dist * i);
                painter.drawLine(L1, L2);
                painter.drawText(QPointF(L1.x() + 15, L2.y() + 4), QString::number(r_sum));
                r_sum -= 10;
            }
        }
        painter.setPen(QPen(QColor(255, 0, 0), 4));
        painter.drawLine(mid_x, mid_y, bottom_x, bottom_y);

        {
            painter.setBrush(QBrush(QColor(255, 215, 0, 180)));
            painter.setPen(QPen(QColor(255, 215, 0, 180), 0));
            int angleX = top_x;
            int angleY = mid_y - (m_Speed * ((mid_y - 70.0) / 20.0));
            QPolygon triangle;
            triangle.setPoints(3, angleX - 5, angleY, angleX - 15, angleY - 10, angleX - 15, angleY + 10);
            painter.drawPolygon(triangle);

            painter.setPen(QPen(QColor(Qt::GlobalColor::yellow), top_Pen_Width));
            painter.drawText(QPointF(angleX - 55, angleY + 5), QString::number(m_Speed, 'f', 2));
        }
    }

    rollMedian(painter);
}

void UAVPlaneAttitude::showEvent(QShowEvent *event)
{
    m_show = true;
}

void UAVPlaneAttitude::hideEvent(QHideEvent *event)
{
    m_show = false;
}
