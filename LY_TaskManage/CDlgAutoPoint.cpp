#include "CDlgAutoPoint.h"
#include "ui_CDlgAutoPoint.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "CoordinateTranfer.h"
#include "QGeoCoordinate"

CDlgAutoPoint::CDlgAutoPoint(QWidget *parent) :
    LyDialog(tr("Strike Waypoint Setting"), parent),
    ui(new Ui::CDlgAutoPoint)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(480, 740);

    ui->textEdit->setReadOnly(true);

    m_updatePositionAndMarkIDCal = false;
    m_isDlgAutoPointShow           = false;
    m_drawMarkLine               = false;

    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));

    InitChart();
}

CDlgAutoPoint::~CDlgAutoPoint()
{
    delete ui;
}

void CDlgAutoPoint::SetPosition(const double &dLon, const double &dLat)
{
    m_dLon = dLon;
    m_dLat = dLat;
}

void CDlgAutoPoint::SetMarkID(const std::string &strMarkID)
{
    m_strMarkID = strMarkID;
}

void CDlgAutoPoint::SetUpdatePositionAndMarkIDCal(bool updatePositionAndMarkIDCal)
{
    m_updatePositionAndMarkIDCal = updatePositionAndMarkIDCal;
}

bool CDlgAutoPoint::UpdatePositionAndMarkIDCal()
{
    return m_updatePositionAndMarkIDCal;
}

bool CDlgAutoPoint::isDlgAutoPointShow()
{
    return m_isDlgAutoPointShow;
}

bool CDlgAutoPoint::drawMarkLine()
{
    return m_drawMarkLine;
}

bool CDlgAutoPoint::JudeInMarkInfoSpace()
{

    m_isDlgAutoPointShow           = false;
    m_drawMarkLine               = false;

    if (m_lineSeries && m_lineSeries)
    {
        m_lineSeries->clear();
        m_lineSeriesMark->clear();
    }

    if (ui && ui->lineEdit)
    {
        ui->lineEdit->setText("");
        ui->lineEdit_2->setText("");
        ui->lineEdit_3->setText("");
        ui->lineEdit_4->setText("");
        ui->lineEdit_5->setText("");
        ui->textEdit->setText("");
    }

    if (m_strMarkID == "-")
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Didn't select a target?"), QMessageBox::Yes | QMessageBox::No);
        return false;
    }

    qnzkna::MarkManage::IMarkManageService *pIMarkManageService = nullptr;
    pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();
    if (pIMarkManageService == nullptr)
    {
        return false;
    }

    SystemMarkInfo stInfo = pIMarkManageService->GetMark(m_strMarkID);

    if (stInfo.GetID().empty())
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Does the selected target not exist in the target database?"), QMessageBox::Yes | QMessageBox::No);
        return false;
    }

    m_MarkInfo = stInfo;

    m_dDis = get_distance_from_lat_lon(m_dLat, m_dLon, m_MarkInfo.GetLat(), m_MarkInfo.GetLon());

    if (m_dDis > 1000)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("More than 1000 meters from the target?"), QMessageBox::Yes | QMessageBox::No);
        return false;
    }

    if (m_dDis <= 1000)
    {
        return true;
    }

    LyMessageBox::warning(this, tr("Tips"), tr("More than 1000 meters from the target?"), QMessageBox::Yes | QMessageBox::No);
    return false;
}

bool CDlgAutoPoint::JudeInZone()
{
    if (m_strMarkID == "-")
    {
        return false;
    }

    qnzkna::MarkManage::IMarkManageService	*pIMarkManageService = nullptr;
    pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();
    if (pIMarkManageService == nullptr)
    {
        return false;
    }

    CZoneInfoVec stInfoVec;
    stInfoVec = pIMarkManageService->GeAirApaceByMarkID(m_strMarkID);

    if (stInfoVec.size() == 0)
    {
        return false;
    }

    for (auto info : stInfoVec)
    {
        if (info.GetAttribute() == EZoneAttribute_Attack && info.GetShape() == EZoneShape_Circle)
        {
            m_MarkInfo = pIMarkManageService->GetMark(info.GetMarkID());

            m_dDis = get_distance_from_lat_lon(m_dLat, m_dLon, m_MarkInfo.GetLat(), m_MarkInfo.GetLon());

            ZonePointVec stVec = info.GetPoints();
            if (stVec.size() == 0)
            {
                return false;
            }
            auto itr = stVec.begin();
            if (m_dDis <= itr->dRadius)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

double CDlgAutoPoint::MakeSureAngel(double dangle)
{
    double dAmgeel = dangle;
    if (dAmgeel < 0)
    {
        dAmgeel += 360;
    }
    return dAmgeel;
}

void CDlgAutoPoint::SetEditLinePoint(bool editLinePoint)
{
    if (ui && ui->pushButton)
    {
        ui->pushButton->setEnabled(editLinePoint);
    }
}

void CDlgAutoPoint::SetStartPoint(double dLon, double dLat, double dHeight)
{
    m_StartLon = dLon;
    m_StartLat = dLat;
    m_StartAlt = dHeight;
}

void CDlgAutoPoint::GetStartPoint(double &dLon, double &dLat, double &dHeight)
{
    dLon = m_StartLon;
    dLat = m_StartLat;
    dHeight = m_StartAlt;
}

void CDlgAutoPoint::SetMarkPoint(double dLon, double dLat, double dHeight)
{
    m_MarkLon = dLon;
    m_MarkLat = dLat;
    m_MarkAlt = dHeight;
}

void CDlgAutoPoint::GetMarkPoint(double &dLon, double &dLat, double &dHeight)
{
    dLon = m_MarkLon;
    dLat = m_MarkLat;
    dHeight = m_MarkAlt;
}

void CDlgAutoPoint::SetFinishPoint(double dLon, double dLat, double dHeight)
{
    m_FinishLon = dLon;
    m_FinishLat = dLat;
    m_FinishAlt = dHeight;

}

void CDlgAutoPoint::GetFinishPoint(double &dLon, double &dLat, double &dHeight)
{
    dLon = m_FinishLon;
    dLat = m_FinishLat;
    dHeight = m_FinishAlt;
}

void CDlgAutoPoint::InitChart()
{
    m_axisX = new QValueAxis();
    m_axisY = new QValueAxis();
    m_axisX->setTitleText(tr("Distance"));
    m_axisY->setTitleText(tr("Altitude"));
    m_axisX->setMin(0);
    m_axisY->setMin(0);
    m_axisX->setMax(AXIS_MAX_X);
    m_axisY->setMax(AXIS_MAX_Y);

    m_chart = new QChart();
    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_lineSeries = new QLineSeries();
    m_lineSeries->setPointsVisible(false);
    m_lineSeries->setName(tr("Trajectory Curve"));
    m_chart->addSeries(m_lineSeries);
    m_lineSeries->attachAxis(m_axisX);
    m_lineSeries->attachAxis(m_axisY);

    m_lineSeriesMark = new QLineSeries();
    m_lineSeriesMark->setPointsVisible(true);
    m_lineSeriesMark->setName(tr("Mark Position"));
    m_chart->addSeries(m_lineSeriesMark);
    m_lineSeriesMark->attachAxis(m_axisX);
    m_lineSeriesMark->attachAxis(m_axisY);

    m_chart->setTheme(QChart::ChartThemeBlueCerulean);

    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    ui->graphicsView->setChart(m_chart);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);

}

void CDlgAutoPoint::ShowChart(std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> &cciplist, double &dAlt, double &dDis, double &dMarkDis)
{

    m_lineSeries->clear();
    double dMaxX = dMarkDis, dMaxY = 0;
    for (auto ccip : cciplist)
    {
        double dx = ccip.x;
        double dy = dAlt + ccip.zd;
        m_lineSeries->append(QPointF(dx, dy));
        if (dMaxX < dx)
        {
            dMaxX = dx;
        }
        if (dMaxY < dy)
        {
            dMaxY = dy;
        }
    }

    m_lineSeriesMark->clear();
    m_lineSeriesMark->append(QPointF(dMarkDis, 0));

    m_chart->axisX()->setMax(dMaxX + 10);
    m_chart->axisY()->setMax(dMaxY + 1);

}

void CDlgAutoPoint::closeEvent(QCloseEvent *event)
{
    m_updatePositionAndMarkIDCal = false;
    m_isDlgAutoPointShow           = false;
    m_drawMarkLine               = false;

    m_strMarkID                  = "-";

}

void CDlgAutoPoint::showEvent(QShowEvent *event)
{

    m_isDlgAutoPointShow           = false;
    m_drawMarkLine               = false;

    m_strMarkID                  = "-";

    if (m_lineSeries && m_lineSeries)
    {
        m_lineSeries->clear();
        m_lineSeriesMark->clear();
    }

    if (ui && ui->lineEdit)
    {
        ui->lineEdit->setText("");
        ui->lineEdit_2->setText("");
        ui->lineEdit_3->setText("");
        ui->lineEdit_4->setText("");
        ui->lineEdit_5->setText("");
        ui->textEdit->setText("");
    }

}

void CDlgAutoPoint::CalFromPositionAndMarkID()
{

    m_isDlgAutoPointShow           = false;
    m_drawMarkLine               = false;

    if (m_lineSeries && m_lineSeries)
    {
        m_lineSeries->clear();
        m_lineSeriesMark->clear();
    }

    if (ui && ui->lineEdit)
    {
        ui->lineEdit->setText("");
        ui->lineEdit_2->setText("");
        ui->lineEdit_3->setText("");
        ui->lineEdit_4->setText("");
        ui->lineEdit_5->setText("");
        ui->textEdit->setText("");
    }

    if (!JudeInMarkInfoSpace())
    {
        return;
    }

    double nHeihtGps = m_MarkInfo.GetHeight();
    double nHeihtAboveDist = 1.0;
    double nHeihtAbove = 0;
    double dDis = m_dDis;
    double dDisLand = 0;
    if (m_dDis < 5)
    {
        nHeihtAboveDist = 0.001;
    }
    else if (m_dDis < 20)
    {
        nHeihtAboveDist = 0.01;
    }
    else if (m_dDis < 50)
    {
        nHeihtAboveDist = 0.1;
    }
    else if (m_dDis < 100)
    {
        nHeihtAboveDist = 0.5;
    }
    else
    {
        nHeihtAboveDist = 1.0;
    }

    double dBombFlyDis    = 0;
    double dBombFlyHeight = 0;
    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist_F;
    while (1)
    {
        std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
        qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(m_dLon, m_dLat, nHeihtAbove + nHeihtGps, 0, nHeihtAbove, 0, ccipdata_80mmlist);
        auto itr = ccipdata_80mmlist.rbegin();
        double dDisNew = abs(itr->x - m_dDis);

        dBombFlyDis    = abs(itr->x);
        dBombFlyHeight = abs(itr->zd);

        if (dDis >= dDisNew)
        {
            dDis = dDisNew;
            dDisLand = itr->x;
            ccipdata_80mmlist_F = ccipdata_80mmlist;
        }
        else
        {
            nHeihtAbove = nHeihtAbove - nHeihtAboveDist;
            break;
        }
        nHeihtAbove = nHeihtAbove + nHeihtAboveDist;
        if (nHeihtAbove >= 200)
        {
            ui->textEdit->setText(tr("Calculation failed, please try again, calculated height more than 200(m)"));
            return;
        }
    }

    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(m_dLon, m_dLat, nHeihtAbove, 0, nHeihtAbove, 0, ccipdata_80mmlist);
    if (ccipdata_80mmlist.size() <= 0)
    {
        ui->textEdit->setText(tr("Calculation failed, please try again, calculated height more than 200(m)"));
        return;
    }
    auto itr = ccipdata_80mmlist.rbegin();
    dDis = abs(itr->x - m_dDis);

    dDisLand = itr->x;
    ccipdata_80mmlist_F = ccipdata_80mmlist;

    dBombFlyDis    = abs(itr->x);
    dBombFlyHeight = abs(itr->zd);

    QGeoCoordinate QPoint1, QPoint2;
    QPoint1.setLongitude(m_dLon);
    QPoint1.setLatitude(m_dLat);
    QPoint2.setLongitude(m_MarkInfo.GetLon());
    QPoint2.setLatitude(m_MarkInfo.GetLat());
    double dbearing = QPoint1.azimuthTo(QPoint2);

    ui->lineEdit->setText(QString::number(m_dLon, 'f', 7));
    ui->lineEdit_2->setText(QString::number(m_dLat, 'f', 7));
    ui->lineEdit_3->setText(QString::number(nHeihtAbove, 'f', 2));
    ui->lineEdit_4->setText(QString::number(dbearing, 'f', 2));
    ui->lineEdit_5->setText("0.00");
    QString strTips;
    strTips = tr("Horizontal distance of UAV to target: ") + QString::number(m_dDis, 'f', 2) + "(m).\n";
    strTips += tr("Current altitude range of the bomb: ") + QString::number(dBombFlyDis, 'f', 2)  + tr("(m), height: ") + QString::number(dBombFlyHeight, 'f', 2) + "(m).\n";
    strTips += tr("Current altitude distance between muntion drop and target point: ") + QString::number(dDis, 'f', 2) + "(m).\n";
    strTips += tr("Mark high warp and weft: ") + QString::number(m_MarkInfo.GetLon(), 'f', 7) + tr(", ") + QString::number(m_MarkInfo.GetLat(), 'f', 7) + tr(", ") + QString::number(m_MarkInfo.GetHeight(), 'f', 2) + "(m).\n";
    strTips += tr("Calculate the warp and weft height: ") + QString::number(m_dLon, 'f', 7) + tr(", ") + QString::number(m_dLat, 'f', 7) + tr(", ") + QString::number(nHeihtAbove, 'f', 2) + "(m).\n";
    ui->textEdit->setText(strTips);

    QPoint2 = QPoint1.atDistanceAndAzimuth(dDisLand, dbearing);
    SetStartPoint(m_dLon, m_dLat, nHeihtAbove);
    SetFinishPoint(QPoint2.longitude(), QPoint2.latitude(), m_MarkInfo.GetHeight());
    SetMarkPoint(m_MarkInfo.GetLon(), m_MarkInfo.GetLat(), m_MarkInfo.GetHeight());

    ShowChart(ccipdata_80mmlist_F, nHeihtAbove, m_dDis, m_dDis);

    m_isDlgAutoPointShow = true;
    m_drawMarkLine     = true;
}

void CDlgAutoPoint::on_pushButton_3_clicked()
{

    m_isDlgAutoPointShow           = false;
    m_drawMarkLine               = false;

    bool ok = false;
    double dLon = ui->lineEdit->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Longitude error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    double dLat = ui->lineEdit_2->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Latitude error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    double dHeight = ui->lineEdit_3->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Altitude error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    double dAzimuth = ui->lineEdit_4->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Azimuth error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    double dPitch = ui->lineEdit_5->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Pitch error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    m_dDis = get_distance_from_lat_lon(dLat, dLon, m_MarkInfo.GetLat(), m_MarkInfo.GetLon());

    if (m_dDis > 1000)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("More than 1000 meters from the target?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(dLon, dLat, dHeight, 0, dHeight, dPitch, ccipdata_80mmlist);
    auto itr = ccipdata_80mmlist.rbegin();
    double dDisNew = abs(itr->x - m_dDis);
    double dBombFlyDis    = abs(itr->x);
    double dBombFlyHeight = abs(itr->zd);
    QString strTips;
    strTips = tr("Horizontal distance of UAV to target: ") + QString::number(m_dDis, 'f', 2) + "(m).\n";
    if (itr->x > m_dDis)
    {

        strTips += tr("Current altitude range of the bomb: ") + QString::number(dBombFlyDis, 'f', 2)  + tr("(m), height: ") + QString::number(dBombFlyHeight, 'f', 2) + "(m).\n";
        strTips += tr("Current altitude distance between muntion drop and target point: ") + QString::number(dDisNew, 'f', 2) + "(m).\n";
    }
    else
    {

        strTips += tr("Current altitude range of the bomb: ") + QString::number(dBombFlyDis, 'f', 2)  + tr("(m), height: ") + QString::number(dBombFlyHeight, 'f', 2) + "(m).\n";
        strTips += tr("Current altitude distance between muntion drop and target point: ") + QString::number(dDisNew, 'f', 2) + "(m).\n";
    }

    QGeoCoordinate QPoint1, QPoint2;
    QPoint1.setLongitude(dLon);
    QPoint1.setLatitude(dLat);
    QPoint2.setLongitude(m_MarkInfo.GetLon());
    QPoint2.setLatitude(m_MarkInfo.GetLat());
    double dbearing = QPoint1.azimuthTo(QPoint2);
    QPoint2 = QPoint1.atDistanceAndAzimuth(itr->x, dAzimuth);
    SetStartPoint(dLon, dLat, dHeight);
    SetFinishPoint(QPoint2.longitude(), QPoint2.latitude(), m_MarkInfo.GetHeight());
    SetMarkPoint(m_MarkInfo.GetLon(), m_MarkInfo.GetLat(), m_MarkInfo.GetHeight());

    strTips += tr("Mark high warp and weft: ") + QString::number(m_MarkInfo.GetLon(), 'f', 7) + tr(", ") + QString::number(m_MarkInfo.GetLat(), 'f', 7) + tr(", ") + QString::number(m_MarkInfo.GetHeight(), 'f', 2) + "(m).\n";
    strTips += tr("Calculate the warp and weft height: ") + QString::number(m_dLon, 'f', 7) + tr(", ") + QString::number(m_dLat, 'f', 7) + tr(", ") + QString::number(dHeight, 'f', 2) + "(m).\n";
    ui->textEdit->setText(strTips);

    ShowChart(ccipdata_80mmlist, dHeight, m_dDis, m_dDis);

    m_isDlgAutoPointShow           = true;
    m_drawMarkLine               = true;

    m_strMarkID                  = "-";
}

void CDlgAutoPoint::on_pushButton_clicked()
{
    m_isDlgAutoPointShow           = false;
    m_drawMarkLine               = false;

    bool ok = false;
    double dLon = ui->lineEdit->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Longitude error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    double dLat = ui->lineEdit_2->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Latitude error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    double dHeight = ui->lineEdit_3->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Altitude error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    double dAzimuth = ui->lineEdit_4->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Azimuth error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    double dPitch = ui->lineEdit_5->text().toDouble(&ok);
    if (!ok)
    {
        LyMessageBox::warning(this, tr("Tips"), tr("Pitch error?"), QMessageBox::Yes | QMessageBox::No);
        return ;
    }

    m_updatePositionAndMarkIDCal = false;
    m_isDlgAutoPointShow           = false;
    m_drawMarkLine               = false;

    emit SetPoint(dLon, dLat, dHeight);

    this->hide();
}

void CDlgAutoPoint::on_pushButton_2_clicked()
{
    m_updatePositionAndMarkIDCal = false;
    m_isDlgAutoPointShow           = true;
    m_drawMarkLine               = true;

    m_strMarkID                  = "-";

    this->hide();
}
