#ifndef UAVPLANREALTIMEALITITUDEDIALOG_H
#define UAVPLANREALTIMEALITITUDEDIALOG_H

#include <QWidget>
#include <QMap>
#include <QVector>
#include <QComboBox>
#include <qwt/qwt_plot_zoomer.h>
#include <qwt/qwt_plot_canvas.h>
#include "Protocol/XygsJL/sharedFile.h"

#include "qwt/qwt_plot.h"
#include "qwt/qwt_plot_curve.h"
#include "qwt/qwt_plot_tradingcurve.h"

namespace Ui
{
class UAVPlanRealTimeAlititudeDialog;
}

class UAVPlanRealTimeAlititudeDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UAVPlanRealTimeAlititudeDialog(QWidget *parent = nullptr);
    ~UAVPlanRealTimeAlititudeDialog();

    void updateTelemetryInformation(const QString &strWeaponComponentName, const QString &strWeaponComponentID, const telemetryInformation &telemetryInfo);
    void updateRealTimeAlititude();

private:
    void lineChart();
    void tuXingSize(QwtPlot *plot);

public slots:
    void onZoomer(const QRectF &rect);

protected:

    virtual void changeEvent(QEvent *event);
    virtual void showEvent(QShowEvent *event);
    virtual void hideEvent(QHideEvent *event);

private:
    Ui::UAVPlanRealTimeAlititudeDialog *ui;
    QString                             m_strWeaponComponentID;
    QString                             m_strWeaponComponentName;
    QVector<QPointF>                    m_seriesDataVector;
    QwtPlot                             *m_plot;
    QwtPlotCurve                       *m_curve;
    QwtPlotZoomer                      *m_zoomer;
    double                              m_plotAxisyLeftMax;
    bool                                m_show;
};

#endif
