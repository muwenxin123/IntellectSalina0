#include "VideoHUDDialog.h"
#include "ui_VideoHUDDialog.h"

#include <QPainter>
#include <qstyleoption.h>
#include <QSettings>
#include <QTimer>
#include <QDebug>
#include <QtMath>

#include <DataManage/IUAVRealTimeStateManageService.h>
#include "LY_UAVNavigationArmShotAlone54.h"
#include "IUAVNavigationArmShotAlone54Activator.h"

#include "ly_ccip.h"

#define  MAX_CALCULATE_HEIGHT	200

double valTransfer(double angle)
{
    double headingAngle = angle;
    if (qFuzzyIsNull(std::fmod(std::abs(angle), 360)))
    {
        headingAngle = 0;
    }
    else if (angle >= 360)
    {
        headingAngle = std::fmod(std::abs(angle), 360);
    }
    else if (angle < 0)
    {
        headingAngle = 360 - std::fmod(std::abs(angle), 360);
    }
    return headingAngle;
}

VideoHUDDialog::VideoHUDDialog(const QString &currentComponentID, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::VideoHUDDialog)
    , m_currentComponentID(currentComponentID)
    , m_DY_ZC_HUD_ControlInfo_Show(true)
    , m_DY_ZC_HUD_ControlInfo_Update(false)
    , m_DY_ZC_HUD_ControlInfo_Times(0)
    , m_topDirection(0.00)
    , m_roll(0.00)
    , m_pitch(0.00)
    , m_height(0.00)
    , m_speed(0.00)
    , m_HPAHeight(0.00)
    , m_fSpeed(0)
    , m_fAngle(0.00)
    , m_skySpeed(0)
    , m_msec(0)
    , m_dMaxDistance(0)
{
    QSettings settings("./Data/conf/VideoHUDSettings.ini", QSettings::IniFormat);
    settings.beginGroup("VideoHUDSettings");
    if (settings.contains("VideoHUDShow"))
    {
        m_DY_ZC_HUD_ControlInfo_Show = settings.value("VideoHUDShow", "false").toBool();

        if (settings.contains("VideoHUDShow_" + currentComponentID))
        {
            m_DY_ZC_HUD_ControlInfo_Show = settings.value("VideoHUDShow_" + currentComponentID, "false").toBool();
        }
    }
    settings.endGroup();

    if (parent)
    {
    }

    ui->setupUi(this);
    setStyleSheet(QStringLiteral("VideoHUDDialog { background: transparent; }"));

    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_NoSystemBackground, true);

    setAttribute(Qt::WA_WState_WindowOpacitySet, true);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [this]()
    {
        qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *pIUAVRealTimeStateManageService = IUAVNavigationArmShotAlone54Activator::Get().getPIUAVRealTimeStateManageService();
        if (pIUAVRealTimeStateManageService == nullptr)
        {
            return ;
        }

        m_msec += 100;

        if (m_DY_ZC_HUD_ControlInfo_Times > 10)
        {
            m_DY_ZC_HUD_ControlInfo_Update = false;
            m_DY_ZC_HUD_ControlInfo_Times  = 0;
        }
        m_DY_ZC_HUD_ControlInfo_Times++;

        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stateInfo;
        std::string strFoucsUAVID = m_currentComponentID.toStdString();
        const bool getUAVRealTimeState = pIUAVRealTimeStateManageService->GetUAVRealTimeState(strFoucsUAVID, stateInfo);
        if (getUAVRealTimeState)
        {
            updateVideoHUDDialogTelemetryInformation(stateInfo.telemetryInfo);
            update();
        }
    });
    timer->start(100);

    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(0, 0, MAX_CALCULATE_HEIGHT, 0, MAX_CALCULATE_HEIGHT, 0, ccipdata_80mmlist);
    if (ccipdata_80mmlist.size() == 0)
    {
        return;
    }

    auto itr = ccipdata_80mmlist.rbegin();
    m_dMaxDistance = abs(itr->x);
}

VideoHUDDialog::~VideoHUDDialog()
{
    delete ui;
}

void VideoHUDDialog::updateVideoHUDDialogTelemetryInformation(const telemetryInformation &telemetryInfo)
{
    setUAVAttitude(telemetryInfo.angleofNose, telemetryInfo.angleofRoll, telemetryInfo.angleofPitch, telemetryInfo.relativeOriginHeigth, telemetryInfo.groundVelocity);
}

void VideoHUDDialog::updateDY_ZC_HUD_ControlInfo(const qnzkna::DY_ZC_HUD_Control::DY_ZC_HUD_ControlInfo &dy_ZC_HUD_ControlInfo)
{
    m_DY_ZC_HUD_ControlInfo         = dy_ZC_HUD_ControlInfo;
    m_DY_ZC_HUD_ControlInfo_Update  = true;
    m_DY_ZC_HUD_ControlInfo_Times   = 0;
}

bool VideoHUDDialog::eventFilter(QObject *watched, QEvent *event)
{
    return QWidget::eventFilter(watched, event);

}

void VideoHUDDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void VideoHUDDialog::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (m_DY_ZC_HUD_ControlInfo_Update == false || m_DY_ZC_HUD_ControlInfo.nVehicleMarkCal != 1)
    {
        return;
    }

    const QRect &rect = this->rect();
    const int x        = rect.x();
    int y        = rect.y();
    const int width    = rect.width();
    const int height   = rect.height();

    const double  rectMin = qMin(rect.width(), rect.height()) / 4.0;
    const double  distPerPixel = rectMin / 100.0;
    const double dist10 = distPerPixel * 10;
    const double dist20 = distPerPixel * 20;
    const double dist50 = distPerPixel * 50;
    const double dist100 = distPerPixel * 100;

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QPen(QColor(0, 255, 0), 2));
    const int top_x_Dist   = 70;
    const int top_x_left   = x + top_x_Dist;
    const int top_y_left   = this->rect().height() - dist100 * 2 - 15 - dist10 * 5;
    const int top_x_right  = x + width - top_x_Dist;
    const int top_v_num    = 19;
    const int top_v_height = 20;
    const float top_v_Dist = (top_x_right - top_x_left) * 1.0 / 2 / (top_v_num - 1);

    const int yaw = m_topDirection;
    int top_v_size1 = yaw / 10;
    int top_v_size2 = yaw % 10;
    int yaw1 = top_v_size1 * 10;
    const float move_length = top_v_size2 * top_v_Dist / 10.0;

    const int move_x = top_x_left - move_length;

    int num1 = -90 + yaw1;
    QFont fonttext;
    fonttext.setBold(true);
    fonttext.setPointSize(15);
    if (m_DY_ZC_HUD_ControlInfo_Show)
    {
        for (int i = 0; i < top_v_num; i++)
        {
            QPoint p1(move_x + i * top_v_Dist, top_y_left);
            QPoint p2(move_x + i * top_v_Dist, top_y_left + top_v_height - top_v_height / 2);
            painter.drawLine(p1, p2);
            if (num1 >= 360)
            {
                num1 = 0;
            }
            if (num1 >= 0)
            {
                painter.setFont(fonttext);
                if (num1 == 0)
                {
                    painter.drawText(p2.x() - 10, p2.y() + 15 - 2, tr("N"));
                }
                if (num1 == 90)
                {
                    painter.drawText(p2.x() - 6, p2.y() + 15 - 2, tr("E"));
                }
                else if (num1 == 180)
                {
                    painter.drawText(p2.x() - 6, p2.y() + 15 - 2, tr("S"));
                }
                if (num1 == 270)
                {
                    painter.drawText(p2.x() - 10, p2.y() + 15 - 2, tr("W"));
                }
            }
            else
            {
                painter.setFont(fonttext);
                int num2 = num1 + 360;
                if (num2 == 270)
                {
                    painter.drawText(p2.x() - 5 - 6, p2.y() + 15 - 2, tr("W"));
                }
            }

            num1 += 10;
        }
        const QPoint p1(top_x_left + (top_v_num - 1)* top_v_Dist / 2, top_y_left);
        QPoint center  = p1;
        QBrush brush(Qt::NoBrush);
        painter.setBrush(brush);
        painter.setPen(QPen(QColor(0, 255, 0), 3));
        QPolygon triangle;
        y = top_y_left - 15;
        triangle.setPoints(3, center.x(), y + 30, center.x() - 15, y + 45, center.x() + 15, y + 45);
        painter.drawPolygon(triangle);

        if (m_DY_ZC_HUD_ControlInfo_Update)
        {
            if (m_DY_ZC_HUD_ControlInfo.nVehicleMarkCal == 1)
            {
                if (abs(m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinYawDiff) < 100)
                {
                    QBrush brush(Qt::NoBrush);
                    painter.setBrush(brush);
                    painter.setPen(QPen(QColor(255, 255, 255), 3));
                    QPolygon triangle1;
                    int xStart = center.x() + m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinYawDiff / 10.0 * top_v_Dist;
                    triangle1.setPoints(3, xStart, y + 29, xStart - 10, y + 15, xStart + 10, y + 15);
                    painter.drawPolygon(triangle1);

                    fonttext.setPointSize(9);
                    painter.setFont(fonttext);
                    painter.setPen(QPen(QColor(255, 255, 255), 2));
                    painter.drawText(QPointF(xStart - 10 - 4, y + 10), QString::number(m_DY_ZC_HUD_ControlInfo.dVehicleMarkYaw, 'f', 2));
                }
            }
        }

        fonttext.setPointSize(10);
        painter.setFont(fonttext);
        painter.setPen(QPen(QColor(Qt::GlobalColor::white), 2));
        painter.drawText(QPointF(center.x() - 20, y + 60), QString::number(m_topDirection, 'f', 2));
        center = this->rect().center();
        center.setX(p1.x());

        int leftX = x + 10;
        int leftY = y + 50;
        QString Lab_Left;
        int num = 0;
        switch (num)
        {
        case 0:
            Lab_Left = QString::fromLocal8Bit("A/G");
            break;
        case 1:
            Lab_Left = QString::fromLocal8Bit("A/A");
            break;
        case 2:
            Lab_Left = QString::fromLocal8Bit("G/A");
            break;
        default:
            break;
        }

        painter.setPen(QPen(QColor(255, 255, 255)));
        QFont font;
        font.setBold(true);
        font.setPointSize(20);
        painter.setFont(font);
        const int rightX = (x + width) - 140;
        const int rightY = y + 100;
        const int HPAHeight = 300;
        const int sky_speed = 20;
        const int moveX = 100;
        const int moveY = 100;
        QString rocketBomb = "Rb01";
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor(255, 255, 255), 1));
        QRectF rectangleft(x + moveX + 5, y + height - 90, 40, 40);
        QRectF rectangright(x + width - moveX - 160 + 10, y + height - 90, 40, 40);
        y = rect.y();
        QFont font1;
        font1.setBold(true);
        font1.setPointSize(13);
        painter.setFont(font1);
        if (m_DY_ZC_HUD_ControlInfo_Update && m_DY_ZC_HUD_ControlInfo.nVehicleMarkCal == 1)
        {
            if (m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinDist3D < 5)
            {
                painter.setPen(QPen(QColor(Qt::GlobalColor::red), 1));
                painter.drawText(x + distPerPixel * 2, this->rect().height() - 40, ("RDY"));
            }
            else
            {
                painter.setPen(QPen(QColor(Qt::GlobalColor::gray), 1));
                painter.drawText(x + distPerPixel * 2, this->rect().height() - 40, ("UNRDY"));
            }
            painter.drawText(x + distPerPixel * 2, this->rect().height() - 22, ("L:") + QString::number(m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinDist3D, 'f', 2));
        }
        else
        {
            painter.drawText(x + width - 140, y + height - 100, ("UNRDY"));
            painter.drawText(x + width - 140, y + height - 60, ("L:"));
        }

        AMapPod54ControlStatus stInfo = IUAVNavigationArmShotAlone54Activator::Get().GetPod54ControlStatusInfoByUAVID(m_currentComponentID);
        QString strServoStatus = tr("Pod: ") + AMapPod54ControlStatus_ServoStatus_ToStr(stInfo.servoStatus);
        painter.drawText(x + distPerPixel * 2, this->rect().height() - 4, strServoStatus);
    }

    if (m_DY_ZC_HUD_ControlInfo_Update)
    {
        QPoint center = this->rect().center();
        if (m_DY_ZC_HUD_ControlInfo.nVehicleMarkCal == 1)
        {
            const QRect &rect    = this->rect();
            const QPoint  center1  = rect.center();
            const double  rectMin = qMin(rect.width(), rect.height()) / 4.0;
            const double  distPerPixel = rectMin / 100.0;

            painter.setPen(QPen(QColor(0, 100, 0), 2));
            painter.setBrush(Qt::NoBrush);

            center.setX(dist100 + dist20 * 2);
            center.setY(this->rect().height() - dist100 - 15 - distPerPixel * 2);

            if (m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinDist3D < 5)
            {
                painter.setPen(QPen(QColor(255, 0, 0), 1));
            }

            fonttext.setPointSize(9);
            painter.setFont(fonttext);

            painter.drawEllipse(QRectF(center.x() - dist10, center.y() - dist10, dist10 * 2, dist10 * 2));
            painter.drawText(center.x() + dist10 + 2, center.y(), tr("5"));

            painter.drawEllipse(QRectF(center.x() - dist20, center.y() - dist20, dist20 * 2, dist20 * 2));
            painter.drawText(center.x() + dist20 + 2, center.y(), tr("10"));

            painter.drawEllipse(QRectF(center.x() - dist50, center.y() - dist50, dist50 * 2, dist50 * 2));
            painter.drawText(center.x() + dist50 + 2, center.y(), tr("25"));

            painter.drawEllipse(QRectF(center.x() - dist100, center.y() - dist100, dist100 * 2, dist100 * 2));
            painter.drawText(center.x() + dist100 + 2, center.y(), tr("50m"));

            painter.drawLine(center.x()  - dist100, center.y(), center.x() - dist100 + dist100 * 2, center.y());
            painter.drawLine(center.x(), center.y() - dist100, center.x(), center.y() - dist100 + dist100 * 2);

            painter.drawPoint(center);
            const int botLeftX = center.x() + dist100 + distPerPixel * 20 + 2;
            const int botRightX = botLeftX + 20;
            const int botY = center.y() + dist100 - distPerPixel * 20;
            double dLaunchHeight = -1;
            constexpr int line_num = 15;
            if (m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinDist < 1000)
            {
                dLaunchHeight = CalLauchHeightNew();
            }

            int num = 0;
            painter.setPen(QPen(QColor(0, 255, 0), 2));
            painter.setFont(QFont());
            double value = m_height;
            int botLeftY = botY;
            if (value <= 300.00)
            {
                for (int i = 0; i < line_num; i++)
                {
                    painter.drawLine(botLeftX, botLeftY, botRightX, botLeftY);
                    painter.drawText(botLeftX + 30, botLeftY + 3 * (distPerPixel / 2), QString::number(num));
                    botLeftY -= 25 * (distPerPixel / 2);
                    num += 25;
                }
                QPen pen;
                pen.setColor(QColor(255, 255, 255));
                pen.setCapStyle(Qt::FlatCap);
                painter.setBrush(QBrush(QColor(0, 255, 0)));
                painter.setPen(pen);
                painter.drawRect(botLeftX, botY, 10, -value * (distPerPixel / 2));
                painter.drawText(botLeftX, botY + 25 * (distPerPixel / 2), QString::number(m_height, 'f', 2));

                if (dLaunchHeight != -1)
                {
                    pen.setColor(QColor(255, 255, 255));
                    pen.setWidth(2);
                    painter.setPen(pen);
                    painter.drawLine(botLeftX, botY - dLaunchHeight * (distPerPixel / 2), botRightX, botY - dLaunchHeight * (distPerPixel / 2));
                    painter.drawText(botLeftX, botY - (dLaunchHeight - 3) * (distPerPixel / 2), QString::number(dLaunchHeight, 'f', 2));

                }
            }
            else if (value > 350 && value <= 1000)
            {
                for (int i = 0; i <= 10; i++)
                {
                    painter.drawLine(botLeftX, botLeftY, botRightX, botLeftY);
                    painter.drawText(botLeftX - 30, botLeftY + 3, QString::number(num));
                    botLeftY -= 35 * (distPerPixel / 2);
                    num += 100;
                }
                QPen pen;
                pen.setColor(QColor(0, 255, 0));
                pen.setCapStyle(Qt::FlatCap);
                painter.setBrush(QBrush(QColor(0, 255, 0)));
                painter.setPen(pen);
                const int yTop = value / 1000 * 300;
                painter.drawRect(botLeftX, botY, 10, -yTop);
                painter.drawText(botLeftX - 15, botY + 25, QString::number(m_height, 'f', 2));
            }
            QString strT = tr("Distance from target: ");
            strT += tr("Ullage: ")   + QString::number(m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinDist3D, 'f', 2);
            strT += tr(", altitude: ") + QString::number(m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinHeightDiff, 'f', 2);
            strT += tr(", angle: ") + QString::number(m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinYawDiff, 'f', 2);

            painter.drawText(center.x() - distPerPixel * 75, center.y() + dist100 + 15, strT);

            m_DY_ZC_HUD_ControlInfo.m_dMarkX;
            m_DY_ZC_HUD_ControlInfo.m_dMarkY;
            m_DY_ZC_HUD_ControlInfo.m_dMarkH;
            m_DY_ZC_HUD_ControlInfo.m_dVehicleX;
            m_DY_ZC_HUD_ControlInfo.m_dVehicleY;
            m_DY_ZC_HUD_ControlInfo.m_dVehicleH;

            m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinX;
            m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinY;
            m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinZ;

            double dUAVToLaunch = sqrt(pow(m_DY_ZC_HUD_ControlInfo.m_dVehicleX - m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinX, 2) +
                                       pow(m_DY_ZC_HUD_ControlInfo.m_dVehicleY - m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinY, 2));

            double dUAVToMark = sqrt(pow(m_DY_ZC_HUD_ControlInfo.m_dVehicleX - m_DY_ZC_HUD_ControlInfo.m_dMarkX, 2) +
                                       pow(m_DY_ZC_HUD_ControlInfo.m_dVehicleY - m_DY_ZC_HUD_ControlInfo.m_dMarkY, 2));

            double dx = dUAVToLaunch * sin(abs(m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinYawDiff * M_PI / 180));

            double dy = dUAVToLaunch * cos(abs(m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinYawDiff * M_PI / 180)) - dUAVToMark;

            double dScreenX, dScreenY;
            if (m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinYawDiff > 0)
            {
                dScreenX = center.x() - distPerPixel * 2 * dx;
            }
            else
            {
                dScreenX = center.x() + distPerPixel * 2 * dx;
            }

            dScreenY = center.y() - dy * distPerPixel * 2;

            painter.setPen(QPen(QColor(255, 0, 0), 2, Qt::DotLine));
            painter.setBrush(Qt::NoBrush);
            painter.drawEllipse(QRectF(dScreenX - distPerPixel * 2 * 5, dScreenY - distPerPixel * 2 * 5, distPerPixel * 2 * 10, distPerPixel * 2 * 10));
            painter.drawPoint(dScreenX, dScreenY);
        }
    }
}

void VideoHUDDialog::showEvent(QShowEvent *event)
{
}

void VideoHUDDialog::hideEvent(QHideEvent *event)
{
}
void VideoHUDDialog::chartDirectionPoor(QPainter &painter, const QPoint &center, const int moveX)
{
    painter.setPen(QPen(QColor(0, 255, 0), 2));
    int leftX        = center.x() - 100;
    const int rightX = center.x() + 100;
    const int height = center.y() - 200;
    painter.drawLine(leftX, height, rightX, height);
    const int num = 5;
    const int avg = (rightX - leftX) / (num - 1);
    const int lineY = 20;
    for (int i = 0; i < num; i++)
    {
        if (i == 0)
        {
            painter.drawLine(leftX, height - lineY, leftX, height + lineY);
        }
        else
        {
            painter.drawLine(leftX, height - lineY, leftX, height);
        }
        leftX += avg;
    }
    painter.setBrush(QBrush(QColor(0, 255, 0)));
    painter.setPen(QPen(QColor(0, 255, 0), 2));
    QPolygon triangle;
    triangle.setPoints(3, center.x() + moveX, center.y() - 198, center.x() + moveX - 10, center.y() - 180, center.x() + moveX + 10, center.y() - 180);
    painter.drawPolygon(triangle);
}

void VideoHUDDialog::charCrossHairLine(QPainter &painter, const QPoint &center)
{
    const int centerX = center.x();
    const int centerY = center.y();
    painter.setBrush(QBrush(QColor(0, 255, 0)));
    painter.setPen(QPen(QColor(0, 255, 0), 3));
    painter.drawEllipse(center, 8, 8);

    painter.drawLine(centerX - 55, centerY, centerX - 15, centerY);
    painter.drawLine(centerX + 15, centerY, centerX + 55, centerY);
    painter.drawLine(centerX, centerY - 15, centerX, centerY - 55);
    painter.drawLine(centerX, centerY + 15, centerX, centerY + 55);

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setColor(QColor(0, 0, 255));
    pen.setWidth(5);
    painter.setPen(pen);
    painter.resetTransform();
    painter.translate(center);
    painter.rotate(-m_roll);
    painter.drawLine(0 - 80, 0, 0 - 180, 0);
    painter.drawLine(0 + 80, 0, 0 + 180, 0);
    painter.resetTransform();

    painter.setPen(QPen(QColor(0, 255, 0), 2));
    const int pointX = centerX;
    const int pointY = centerY - 80;
    const QPoint p1(pointX, pointY);
    painter.drawEllipse(p1, 2, 2);
    painter.setBrush(QBrush());
    painter.drawEllipse(p1, 15, 15);

    painter.drawLine(pointX - 15, pointY, pointX - 70, pointY);
    painter.drawLine(pointX - 70, pointY, pointX - 70, pointY + 220);
    painter.drawLine(pointX - 70, pointY + 220, centerX, pointY + 220);

    painter.drawRect(centerX - 50, centerY + 150, 100, 30);
    painter.setBrush(QBrush(QColor(255, 0, 0)));
    painter.drawRect(centerX - 5, centerY + 160, 10, 10);
}

void VideoHUDDialog::chartHeightRule(QPainter &painter, const QRect &rect)
{
    const int x = rect.x();
    const int width = rect.width();
    const QPoint center = rect.center();

    constexpr int line_num = 15;
    const int botLeftX = x + width - 50;
    const int botRightX = x + width - 20;
    const int botY = center.y() + 175;
    double dLaunchHeight = -1;
    if (m_DY_ZC_HUD_ControlInfo.dMarkVehicleMinDist < 1000)
    {
        dLaunchHeight = CalLauchHeight();
    }

    int num = 0;
    painter.setFont(QFont());
    double value = m_height;
    int botLeftY = botY;
    if (value <= 350.00)
    {
        for (int i = 0; i < line_num; i++)
        {
            painter.drawLine(botLeftX, botLeftY, botRightX, botLeftY);
            painter.drawText(botLeftX - 30, botLeftY + 3, QString::number(num));
            botLeftY -= 25;
            num += 25;
        }
        QPen pen;
        pen.setColor(QColor(0, 255, 0));
        pen.setCapStyle(Qt::FlatCap);
        painter.setBrush(QBrush(QColor(0, 255, 0)));
        painter.setPen(pen);
        painter.drawRect(botLeftX, botY, 10, -value);
        painter.drawText(botLeftX - 15, botY + 25, QString::number(m_height, 'f', 2));

        if (dLaunchHeight != -1)
        {
            pen.setColor(QColor(0, 0, 255));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(botLeftX, botY - dLaunchHeight, botRightX, botY - dLaunchHeight);
            painter.drawText(botLeftX, botY - dLaunchHeight - 3, QString::number(dLaunchHeight, 'f', 2));

        }
    }
    else if (value > 350 && value <= 1000)
    {
        for (int i = 0; i <= 10; i++)
        {
            painter.drawLine(botLeftX, botLeftY, botRightX, botLeftY);
            painter.drawText(botLeftX - 30, botLeftY + 3, QString::number(num));
            botLeftY -= 35;
            num += 100;
        }
        QPen pen;
        pen.setColor(QColor(0, 255, 0));
        pen.setCapStyle(Qt::FlatCap);
        painter.setBrush(QBrush(QColor(0, 255, 0)));
        painter.setPen(pen);
        const int yTop = value / 1000 * 350;
        painter.drawRect(botLeftX, botY, 10, -yTop);
        painter.drawText(botLeftX - 15, botY + 25, QString::number(m_height, 'f', 2));
    }
}

void VideoHUDDialog::charLaunchAllowConditions(QPainter &painter, const QRect &rect)
{
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(0, 255, 0), 0));
    const int y           = rect.y();
    const int height      = rect.height();
    const QPoint center   = rect.center();
    const int StartX      = center.x() - 160;
    const int rectY       = y + height - 100;
    int moveX = StartX;

    QFont font;
    font.setPixelSize(15);
    font.setBold(true);
    painter.setFont(font);
    for (int i = 0; i < 6 ; i++)
    {
        painter.drawRect(moveX, rectY, 40, 80);
        if (i == 0)
        {
            painter.drawText(QRect(moveX, rectY, 40, 80), Qt::AlignCenter, tr("Aiming Bearing").split("", QString::SkipEmptyParts).join("\n"));
        }
        else if (i == 1)
        {
            painter.drawText(QRect(moveX, rectY, 40, 80), Qt::AlignCenter, tr("Distance").split("", QString::SkipEmptyParts).join("\n"));
        }
        else if (i == 2)
        {
            painter.drawText(QRect(moveX, rectY, 40, 80), Qt::AlignCenter, tr("Roll Over").split("", QString::SkipEmptyParts).join("\n"));
        }
        else if (i == 3)
        {
            painter.drawText(QRect(moveX, rectY, 40, 80), Qt::AlignCenter, tr("Altitude").split("", QString::SkipEmptyParts).join("\n"));
        }
        else if (i == 4)
        {
            painter.drawText(QRect(moveX, rectY, 40, 80), Qt::AlignCenter, tr("Velocity").split("", QString::SkipEmptyParts).join("\n"));
        }
        else if (i == 5)
        {
            painter.drawText(QRect(moveX, rectY, 40, 80), Qt::AlignCenter, tr("Ammunition Preparation").split("", QString::SkipEmptyParts).join("\n"));
        }
        moveX += 40;
    }
    painter.drawText(center.x() - 30, rectY - 40, tr("Pitching:") + QString::number(m_pitch,        'f', 2));
    painter.drawText(center.x() - 30, rectY - 20, tr("Azimuth:") + QString::number(m_topDirection, 'f', 2));
}

void VideoHUDDialog::setUAVAttitude(float topDirection, float roll, float pitch, double height, double speed)
{
    m_topDirection  = topDirection;
    m_roll          = roll;
    m_pitch         = pitch;
    m_height        = height;
    m_speed         = speed;
}

double VideoHUDDialog::CalLauchHeight()
{
    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    double dDif = m_DY_ZC_HUD_ControlInfo.m_dVehicleAltitude - m_DY_ZC_HUD_ControlInfo.m_dMarkAltitude;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(m_DY_ZC_HUD_ControlInfo.m_dVehicleLongitude, m_DY_ZC_HUD_ControlInfo.m_dVehicleLatitude, dDif, 0, dDif, 0, ccipdata_80mmlist);
    if (ccipdata_80mmlist.size() == 0)
    {
        return -1;
    }
    auto itr = ccipdata_80mmlist.rbegin();
    double dDisNew = abs(itr->x);
    double dAdd = 0;
    double dUAVToMark = sqrt(pow(m_DY_ZC_HUD_ControlInfo.m_dVehicleX - m_DY_ZC_HUD_ControlInfo.m_dMarkX, 2) +
                               pow(m_DY_ZC_HUD_ControlInfo.m_dVehicleY - m_DY_ZC_HUD_ControlInfo.m_dMarkY, 2));

    if (dDisNew < dUAVToMark)
    {
        while (dDisNew < dUAVToMark)
        {
            dAdd = dAdd + 1;
            qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(m_DY_ZC_HUD_ControlInfo.m_dVehicleLongitude, m_DY_ZC_HUD_ControlInfo.m_dVehicleLatitude, dDif + dAdd, 0, dDif + dAdd, 0, ccipdata_80mmlist);
            if (ccipdata_80mmlist.size() == 0)
            {
                return -1;
            }
            auto itr1 = ccipdata_80mmlist.rbegin();
            dDisNew = abs(itr1->x);
            if (dDif + dAdd > 200)
            {
                return -1;
            }
        }
        return m_DY_ZC_HUD_ControlInfo.m_dVehicleAltitude + dAdd - 1;
    }
    else
    {
        while (dDisNew > dUAVToMark)
        {
            dAdd = dAdd - 1;
            qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(m_DY_ZC_HUD_ControlInfo.m_dVehicleLongitude, m_DY_ZC_HUD_ControlInfo.m_dVehicleLatitude, dDif + dAdd, 0, dDif + dAdd, 0, ccipdata_80mmlist);
            if (ccipdata_80mmlist.size() == 0)
            {
                return -1;
            }
            auto itr1 = ccipdata_80mmlist.rbegin();
            dDisNew = abs(itr1->x);
            if (dDif + dAdd < 0)
            {
                return -1;
            }
        }
        return m_DY_ZC_HUD_ControlInfo.m_dVehicleAltitude + dAdd + 1;
    }
}

double VideoHUDDialog::CalLauchHeightNew()
{
    double dDif = m_DY_ZC_HUD_ControlInfo.m_dVehicleAltitude - m_DY_ZC_HUD_ControlInfo.m_dMarkAltitude;

    if (abs(dDif) > MAX_CALCULATE_HEIGHT)
    {
        return -1;
    }

    double dUAVToMark = sqrt(pow(m_DY_ZC_HUD_ControlInfo.m_dVehicleX - m_DY_ZC_HUD_ControlInfo.m_dMarkX, 2) +
                               pow(m_DY_ZC_HUD_ControlInfo.m_dVehicleY - m_DY_ZC_HUD_ControlInfo.m_dMarkY, 2));

    if (dUAVToMark > m_dMaxDistance)
    {
        return -1;
    }

    double dDisNew = CalLauchDis(dDif);
    if (dDisNew == -1)
    {
        return -1;
    }

    int nSymbol = 0;
    double dHeight = 0;
    if (dDisNew < dUAVToMark)
    {
        dHeight = CalLauchHeightNewByErFen(dDif, MAX_CALCULATE_HEIGHT, dUAVToMark);
    }
    else
    {
        dHeight = CalLauchHeightNewByErFen(0, dDif, dUAVToMark);
    }
    return dHeight;
}

double VideoHUDDialog::CalLauchHeightNewByErFen(const double &dStartHeight, const double &dFinishHeight, const double &dDisUAVToTarget)
{
    double dHeight(0), dHeightNew(0);
    double dDis(0), dDisNew(0);
    double dStartHeightCal(dStartHeight), dFinishHeightCal(dFinishHeight);

    while (1)
    {
        dHeightNew = (dStartHeightCal + dFinishHeightCal) / 2.0;
        dDisNew = CalLauchDis(dHeightNew);
        if (dDisNew == -1)
        {
            return -1;
        }

        if (abs(dHeightNew - dHeight) < 1)
        {
            if (abs(dDisUAVToTarget - dDis) > abs(dDisUAVToTarget - dDisNew))
            {
                return dHeightNew;
            }
            else
            {
                return dHeight;
            }
        }
        else
        {
            dDis = dDisNew;
            dHeight = dHeightNew;
        }

        if (dDis < dDisUAVToTarget)
        {
            dStartHeightCal = dHeight;
        }
        else
        {
            dFinishHeightCal = dHeight;
        }
    }

}

double VideoHUDDialog::CalLauchDis(const double &dHeight)
{
    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(m_DY_ZC_HUD_ControlInfo.m_dVehicleLongitude, m_DY_ZC_HUD_ControlInfo.m_dVehicleLatitude, dHeight, 0, dHeight, 0, ccipdata_80mmlist);
    if (ccipdata_80mmlist.size() == 0)
    {
        return -1;
    }
    auto itr = ccipdata_80mmlist.rbegin();
    return abs(itr->x);
}

int VideoHUDDialog::headDraw(double path, double depath, QPainter &painter)
{
    return 0;
}

bool flag4 = true, flag7 = true, flag9 = true;

void VideoHUDDialog::targetDraw(int turn, int height, int cx, int cy, int stpx, int stpy, int stpz, int grounddt, double head, double dehead, QPainter &painter)
{
    int eyedistance = 6;
    double pitchIFOV = 45, headIFOV = 45;
    double pitchag;
    int headag;
    int targpointx, targpointy;
    int border = 0;
    double gdistance = sqrt(pow((stpy - cy), 2) + pow((stpx - cx), 2));
    pitchag = atan(abs(stpz - height) / gdistance) * 180 / M_PI;
    if (height == stpz)
    {
        targpointy = 350;
    }
    else if (height < stpz)
    {
        if (pitchag >= pitchIFOV)
        {
            targpointy = 155;
            border = 1;
        }
        else
        {
            targpointy = 350 - eyedistance * tan(pitchag * M_PI / 180) * 350 / 6.0;
            if (targpointy < 155)
            {
                targpointy = 155;
                border = 1;
            }
        }
    }
    else if (height > stpz)
    {
        if (pitchag >= pitchIFOV)
        {
            targpointy = 545;
            border = 1;
        }
        else
        {
            targpointy = 350 + eyedistance * tan(pitchag * M_PI / 180) * 350 / 6.0;
            if (targpointy > 545)
            {
                targpointy = 545;
                border = 1;
            }
        }
    }
    headag = abs(static_cast<int>(head - dehead));
    if (headag > 180)
    {
        headag = 360 - headag;
    }
    if (0 == turn)
    {
        targpointx = 355;
    }
    else if (turn > 0)
    {
        if (headag >= headIFOV)
        {
            targpointx = 570;
            border = 1;
        }
        else
        {
            targpointx = 355 + eyedistance * tan(headag * M_PI / 180) * 350 / 6.0;
            if (targpointx > 570)
            {
                targpointx = 570;
                border = 1;
            }
        }
    }
    else if (turn < 0)
    {
        if (headag >= headIFOV)
        {
            targpointx = 140;
            border = 1;
        }
        else
        {
            targpointx = 355 - eyedistance * tan(headag * M_PI / 180) * 350 / 6.0;
            if (targpointx < 140)
            {
                targpointx = 140;
                border = 1;
            }
        }
    }
    int len = 8;
    double distance;
    QString str;
    int l = 30, d = 30;
    if (!flag7)
    {
        grounddt = 0;
    }
    if (flag7 || flag4)
    {
        distance = sqrt(pow(grounddt, 2) + pow(abs(static_cast<int>(height - stpz)), 2)) / 1000.0;
        if (headag)
        {
            str.sprintf("%d", headag);
            painter.drawText(QRect(targpointx - 10, targpointy - 25, targpointx + 10, targpointy - 10), Qt::AlignCenter, str);
        }
        if (static_cast<int>(10 * distance))
        {
            str.sprintf("%.1f", distance);
            painter.drawText(QRect(targpointx - 10, targpointy + 10, targpointx + 10, targpointy + 25), Qt::AlignCenter, str);
        }
        painter.drawLine(targpointx - len, targpointy - len, targpointx + len, targpointy - len);
        painter.drawLine(targpointx + len, targpointy - len, targpointx + len, targpointy + len);
        painter.drawLine(targpointx + len, targpointy + len, targpointx - len, targpointy + len);
        painter.drawLine(targpointx - len, targpointy + len, targpointx - len, targpointy - len);
    }
}

void VideoHUDDialog::reticleDraw(int cenx, int ceny, int radius, double tarrange, double minrange, double roll, QPainter &painter)
{
    int ra = static_cast<int>(roll);
    if (ra < 0)
    {
        ra = 360 - abs(ra) % 360;
    }
    double ratheta;
    int r = radius - 10;
    if (ra < 90)
    {
        ratheta = ra * M_PI / 180;
        cenx = cenx + (ceny - 350) * sin(ratheta);
        ceny = ceny - (ceny - 350) * (1 - cos(ratheta));
    }
    else if (ra > 270)
    {
        ratheta = 2 * M_PI - ra * M_PI / 180;
        cenx = cenx - (ceny - 350) * sin(ratheta);
        ceny = ceny - (ceny - 350) * (1 - cos(ratheta));
    }
    double sweepang = 360 * tarrange / 12000;
    double startang = 360 - sweepang + 90;
    double theta = startang * M_PI / 180;
    double thetamin = (360 - 360 * minrange / 12000 + 90) * M_PI / 180;
    double startx = cenx + r * cos(theta);
    double starty = ceny - r * sin(theta);
    double trianx = cenx + (r - 3) * cos(thetamin);
    double triany = ceny - (r - 3) * sin(thetamin);
    painter.drawEllipse(cenx - radius, ceny - radius, 2 * radius, 2 * radius);
    painter.drawLine(cenx, ceny, cenx, ceny);
    painter.drawLine(cenx, ceny - radius, cenx, ceny - radius - 15);
    painter.drawLine(cenx, ceny + radius, cenx, ceny + radius + 15);
    painter.drawLine(cenx - radius, ceny, cenx - radius - 15, ceny);
    painter.drawLine(cenx + radius, ceny, cenx + radius + 15, ceny);
    painter.drawLine(startx, starty, startx - 10 * cos(theta), starty + 10 * sin(theta));
    painter.drawArc(cenx - r, ceny - r, 2 * r, 2 * r, startang, sweepang);
    double xl = trianx - 10 * cos(thetamin - M_PI / 6);
    double yl = triany + 10 * sin(thetamin - M_PI / 6);
    double xr = xl + 10 * cos(M_PI / 2 - thetamin);
    double yr = yl + 10 * sin(M_PI / 2 - thetamin);
    painter.drawLine(trianx, triany, xl, yl);
    painter.drawLine(xl, yl, xr, yr);
    painter.drawLine(trianx, triany, xr, yr);
    double sxline, syline, exline, eyline;
    double sxline2, syline2;
    double mrx, mry, mrx2, mry2, mrx3, mry3;
    int l = 350;
    if (ra < 90)
    {
        ratheta = ra * M_PI / 180;
        sxline = cenx + (r - 10) * sin(ratheta);
        syline = ceny - (r - 10) * cos(ratheta);
        exline = sxline + l * sin(ratheta);
        eyline = syline - l * cos(ratheta);
        sxline2 = cenx + radius * sin(ratheta);
        syline2 = ceny - radius * cos(ratheta);
        painter.drawLine(sxline, syline, exline, eyline);
        if ((tarrange - minrange) > 0)
        {
            if ((tarrange - minrange) > 3000)
            {
                mrx = sxline2 + 300 * sin(ratheta);
                mry = syline2 - 300 * cos(ratheta);
            }
            else
            {
                mrx = sxline2 + (tarrange - minrange) * 300 / 3000 * sin(ratheta);
                mry = syline2 - (tarrange - minrange) * 300 / 3000 * cos(ratheta);
            }
            mrx2 = mrx - 20;
            mry2 = mry;
            mrx3 = mrx2;
            mry3 = mry2 + 5;
            painter.drawLine(mrx, mry, mrx2, mry2);
            painter.drawLine(mrx2, mry2, mrx3, mry3);
            painter.drawLine(mrx, mry, mrx2 + 40, mry2);
            painter.drawLine(mrx2 + 40, mry2, mrx3 + 40, mry3);
        }
    }
    else if (ra > 270)
    {
        ratheta = 2 * M_PI - ra * M_PI / 180;
        sxline = cenx - (r - 10) * sin(ratheta);
        syline = ceny - (r - 10) * cos(ratheta);
        sxline2 = cenx - radius * sin(ratheta);
        syline2 = ceny - radius * cos(ratheta);
        exline = sxline - l * sin(ratheta);
        eyline = syline - l * cos(ratheta);
        painter.drawLine(sxline, syline, exline, eyline);
        if ((tarrange - minrange) > 0)
        {
            if ((tarrange - minrange) > 3000)
            {
                mrx = sxline2 - 300 * sin(ratheta);
                mry = syline2 - 300 * cos(ratheta);
            }
            else
            {
                mrx = sxline2 - (tarrange - minrange) * 300 / 3000 * sin(ratheta);
                mry = syline2 - (tarrange - minrange) * 300 / 3000 * cos(ratheta);
            }
            mrx2 = mrx - 20;
            mry2 = mry;
            mrx3 = mrx2;
            mry3 = mry2 + 5;
            painter.drawLine(mrx, mry, mrx2, mry2);
            painter.drawLine(mrx2, mry2, mrx3, mry3);
            painter.drawLine(mrx, mry, mrx2 + 40, mry2);
            painter.drawLine(mrx2 + 40, mry2, mrx3 + 40, mry3);
        }
    }
}

QString VideoHUDDialog::AMapPod54ControlStatus_ServoStatus_ToStr(const uint8_t servoStatus)
{
    switch (servoStatus)
    {
    case 0x01:
        return tr("Koad off");
    case 0x02:
        return tr("Munual");
    case 0x03:
        return tr("Cllect");
    case 0x04:
        return tr("Numerical Citation");
    case 0x05:
        return tr("Course Locking");
    case 0x06:
        return tr("Scan");
    case 0x07:
        return tr("Track Down");
    case 0x08:
        return tr("Vertical Look Down");
    case 0x09:
        return tr("Automatic gyroscope calibration");
    case 0x0A:
        return tr("Temperature gyroscope calibration");
    case 0x0B:
        return tr("Course Follow-up");
    case 0x0C:
        return tr("Centering");;
    case 0x0D:
        return tr("Manual gyroscope calibration");
    case 0x0E:
        return tr("Attitude Guidance");
    case 0x0F:
        return tr("Attitude Guidance");
    default:
        return tr("UnKnown");
    }
    return tr("UnKnown");
}
