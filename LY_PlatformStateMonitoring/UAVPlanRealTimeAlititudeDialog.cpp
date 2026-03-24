#include "UAVPlanRealTimeAlititudeDialog.h"
#include "ui_UAVPlanRealTimeAlititudeDialog.h"
#include <QComboBox>
#include <QFile>
#include <qdatetime.h>
#include <QMap>
#include <qwt/qwt_date_scale_engine.h>
#include <qwt/qwt_date_scale_draw.h>
#include <qwt/qwt_plot_zoneitem.h>
#include <qwt/qwt_scale_draw.h>
#include <qwt/qwt_plot_grid.h>
#include <qwt/qwt_plot_curve.h>
#include <qwt/qwt_plot_canvas.h>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_symbol.h>
#include <qwt/qwt_date.h>

class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer(QWidget *canvas) :
        QwtPlotZoomer(canvas)
    {
        setRubberBandPen(QColor(Qt::darkGreen));
        setTrackerMode(QwtPlotPicker::AlwaysOn);
    }

protected:
    virtual QwtText trackerTextF(const QPointF &pos) const
    {
        const QDateTime dt = QDateTime::fromMSecsSinceEpoch(pos.x());

        QString s;
        s += QwtDate::toString(QDateTime::fromMSecsSinceEpoch(pos.x()),
                               "MM dd hh:mm:ss", QwtDate::FirstThursday)
             + " " + QString::number(pos.y());

        QwtText text(s);
        text.setColor(Qt::white);

        QColor c = rubberBandPen().color();
        text.setBorderPen(QPen(c));
        text.setBorderRadius(6);
        c.setAlpha(170);
        text.setBackgroundBrush(c);

        return text;
    }
};

class DateScaleDraw: public QwtDateScaleDraw
{
public:
    DateScaleDraw(Qt::TimeSpec timeSpec):
        QwtDateScaleDraw(timeSpec)
    {

        setDateFormat(QwtDate::Millisecond, "hh:mm:ss:zzz\nddd dd MMM");
        setDateFormat(QwtDate::Second, "hh:mm:ss\nddd dd MMM");
        setDateFormat(QwtDate::Minute, "hh:mm\nddd dd MMM");
        setDateFormat(QwtDate::Hour, "hh:mm\nddd dd MMM");
        setDateFormat(QwtDate::Day, "ddd dd MMM");
        setDateFormat(QwtDate::Week, "Www");
        setDateFormat(QwtDate::Month, "MMM");
    }
};

class ZoneItem: public QwtPlotZoneItem
{
public:
    ZoneItem(const QString &title)
    {
        setTitle(title);
        setZ(11);
        setOrientation(Qt::Vertical);
        setItemAttribute(QwtPlotItem::Legend, true);
    }

    void setColor(const QColor &color)
    {
        QColor c = color;

        c.setAlpha(100);
        setPen(c);

        c.setAlpha(20);
        setBrush(c);
    }

    void setInterval(const QDate &date1, const QDate &date2)
    {
        const QDateTime dt1(date1, QTime(), Qt::UTC);
        const QDateTime dt2(date2, QTime(), Qt::UTC);

        QwtPlotZoneItem::setInterval(QwtDate::toDouble(dt1),
                                     QwtDate::toDouble(dt2));
    }
};

class TimeScaleDraw : public QwtScaleDraw
{
public:
    TimeScaleDraw()
    {
    }
    virtual QwtText label(double v) const
    {

        QDateTime upTime = QDateTime::fromMSecsSinceEpoch(v);
        return upTime.toString(QObject::tr("yy-MM-dd hh:mm:ss"));

    }
};
UAVPlanRealTimeAlititudeDialog::UAVPlanRealTimeAlititudeDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVPlanRealTimeAlititudeDialog)
    , m_strWeaponComponentID("")
    , m_strWeaponComponentName("")
    , m_plot(nullptr)
    , m_curve(nullptr)
    , m_zoomer(nullptr)
    , m_plotAxisyLeftMax(60.0)
    , m_show(true)
{
    ui->setupUi(this);
    m_seriesDataVector.clear();
    m_plot = new QwtPlot(ui->widget_UAVPlanRealTimeAlititudeDialog);
    ui->widget_UAVPlanRealTimeAlititudeDialog->layout()->addWidget(m_plot);
    m_plot->setAutoReplot(true);
    m_plot->setTitle("");
    m_plot->setCanvasBackground(Qt::white);
    m_plot->show();

    lineChart();
    tuXingSize(m_plot);
}

UAVPlanRealTimeAlititudeDialog::~UAVPlanRealTimeAlititudeDialog()
{
    delete ui;
}

void UAVPlanRealTimeAlititudeDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UAVPlanRealTimeAlititudeDialog::showEvent(QShowEvent *event)
{
    m_show = true;
}

void UAVPlanRealTimeAlititudeDialog::hideEvent(QHideEvent *event)
{
    m_show = false;
}

void UAVPlanRealTimeAlititudeDialog::updateTelemetryInformation(const QString &strWeaponComponentName, const QString &strWeaponComponentID, const telemetryInformation &telemetryInfo)
{
    if (!m_show)
    {
        return ;
    }

    const QString dateTime =
        QStringLiteral("%1 %2 %3 %4:%5:%6")
        .arg(telemetryInfo._year,  2, 10, QLatin1Char('0'))
        .arg(telemetryInfo._month, 2, 10, QLatin1Char('0'))
        .arg(telemetryInfo._day,   2, 10, QLatin1Char('0'))
        .arg(telemetryInfo._hour,  2, 10, QLatin1Char('0'))
        .arg(telemetryInfo._min,   2, 10, QLatin1Char('0'))
        .arg(telemetryInfo._sec,   2, 10, QLatin1Char('0'));

    const QDateTime time = QDateTime::fromString(dateTime, QObject::tr("yy MM dd hh:mm:ss"));

    const double depthValue    = time.toMSecsSinceEpoch();
    const double sonicvelValue = telemetryInfo.position.heigth;

    if (m_strWeaponComponentID != strWeaponComponentID)
    {
        m_seriesDataVector.clear();
        m_strWeaponComponentID   = strWeaponComponentID;
        m_strWeaponComponentName = strWeaponComponentName;
        ui->label_WeaponComponentName->setText(strWeaponComponentName);
        if (m_plot != nullptr && m_curve != nullptr && m_seriesDataVector.size() > 0)
        {
            m_curve->setSamples(m_seriesDataVector);
        }
        m_plotAxisyLeftMax = 60;
        m_plot->setAxisScale(QwtPlot::xBottom, depthValue, depthValue + 1000);
    }

    if (sonicvelValue > m_plotAxisyLeftMax)
    {
        m_plotAxisyLeftMax = sonicvelValue + (sonicvelValue / 10) + 10;
        m_plot->setAxisScale(QwtPlot::yLeft, 0.0, m_plotAxisyLeftMax);
    }

    m_seriesDataVector.push_back(QPointF(depthValue, sonicvelValue));
}

void UAVPlanRealTimeAlititudeDialog::updateRealTimeAlititude()
{
    if (m_plot != nullptr && m_curve != nullptr && m_seriesDataVector.size() > 0)
    {
        m_curve->setSamples(m_seriesDataVector);
    }
}

void UAVPlanRealTimeAlititudeDialog::lineChart()
{

    m_plot->setAxisScale(QwtPlot::yLeft, 0.0, 60.0);
    m_plot->setAxisScaleDraw(QwtPlot::xBottom, new TimeScaleDraw());
    m_plot->setAxisTitle(QwtPlot::yLeft,   QObject::tr("Height"));
    m_plot->setAxisTitle(QwtPlot::xBottom, QObject::tr("Time"));

    QwtPlotGrid *grid = new QwtPlotGrid();
    grid->setPen(QColor("#FFD700"));
    grid->attach(m_plot);

    m_curve = new QwtPlotCurve();
    m_curve->setPen((Qt::blue), 2, Qt::SolidLine);
    if (m_seriesDataVector.size() > 0)
    {
        m_curve->setSamples(m_seriesDataVector);
    }

    m_curve->attach(m_plot);
}

void UAVPlanRealTimeAlititudeDialog::tuXingSize(QwtPlot *plot)
{
    m_zoomer = new Zoomer(plot->canvas());

    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect2, Qt::RightButton, Qt::ControlModifier);
    m_zoomer->setMousePattern(QwtEventPattern::MouseSelect3, Qt::RightButton);

    connect(m_zoomer, SIGNAL(zoomed(const QRectF &)), this, SLOT(onZoomer(const QRectF &)));
}

void UAVPlanRealTimeAlititudeDialog::onZoomer(const QRectF &rect)
{
    if (m_zoomer->zoomRectIndex() == 0)
    {
        if (m_seriesDataVector.size() > 0)
        {
            m_plot->setAxisScale(QwtPlot::yLeft, 0.0, m_plotAxisyLeftMax);
            m_plot->setAxisScale(QwtPlot::xBottom, m_seriesDataVector.first().rx(), m_seriesDataVector.back().rx());
        }
    }
}
