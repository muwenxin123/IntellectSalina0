#ifndef CDLGAUTOPOINT_H
#define CDLGAUTOPOINT_H

#include <QWidget>
#include "LyDialog.h"
#include "LyMessageBox.h"
#include "DataManage/Mark/ZoneInfo.h"
#include "DataManage/Mark/Mark.h"
#include "ly_ccip.h"
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>

QT_CHARTS_USE_NAMESPACE

namespace Ui
{
class CDlgAutoPoint;
}

class CDlgAutoPoint : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgAutoPoint(QWidget *parent = nullptr);
    ~CDlgAutoPoint();

private:
    Ui::CDlgAutoPoint *ui;

    bool                m_updatePositionAndMarkIDCal;
    bool                m_isDlgAutoPointShow;
    bool                m_drawMarkLine;
    double				m_dLon;
    double				m_dLat;
    std::string			m_strMarkID;
    SystemMarkInfo	m_MarkInfo;
    double				m_dDis;

    double				m_StartLon;
    double				m_StartLat;
    double				m_StartAlt;
    double				m_FinishLon;
    double				m_FinishLat;
    double				m_FinishAlt;
    double				m_MarkLon;
    double				m_MarkLat;
    double				m_MarkAlt;
    QChart *m_chart;

    QValueAxis *m_axisX, *m_axisY;
    QLineSeries *m_lineSeries;
    QLineSeries *m_lineSeriesMark;

    const int AXIS_MAX_X = 10, AXIS_MAX_Y = 10;

public:
    void SetPosition(const double &dLon, const double &dLat);
    void SetMarkID(const std::string &strMarkID);
    void CalFromPositionAndMarkID();
    void SetUpdatePositionAndMarkIDCal(bool updatePositionAndMarkIDCal);
    bool UpdatePositionAndMarkIDCal();
    bool isDlgAutoPointShow();
    bool drawMarkLine();
    bool JudeInMarkInfoSpace();
    bool JudeInZone();
    double MakeSureAngel(double dangle);
    void SetEditLinePoint(bool editLinePoint);
    void SetStartPoint(double dLon, double dLat, double dHeight);
    void GetStartPoint(double &dLon, double &dLat, double &dHeight);
    void SetMarkPoint(double dLon, double dLat, double dHeight);
    void GetMarkPoint(double &dLon, double &dLat, double &dHeight);
    void SetFinishPoint(double dLon, double dLat, double dHeight);
    void GetFinishPoint(double &dLon, double &dLat, double &dHeight);
    void InitChart();
    void ShowChart(std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> &cciplist, double &dAlt, double &dDis, double &dMarkDis);
protected:
    virtual void closeEvent(QCloseEvent *event);
    virtual void showEvent(QShowEvent *event);
private slots:

    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

signals:
    void SetPoint(double dLon, double dLat, double dHeigeth);
};

#endif
