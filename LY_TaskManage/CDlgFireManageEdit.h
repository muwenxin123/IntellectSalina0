#ifndef CDLGFIREMANAGEEDIT_H
#define CDLGFIREMANAGEEDIT_H

#include <QWidget>
#include "LyDialog.h"
#include "QMap"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/IMarkManageService.h"
#include "DataManage/NebulaTask/NebulaMissionAutoCommand.h"
#include "ly_ccip.h"
#include <QChartView>
#include <QValueAxis>
#include <QLineSeries>

QT_CHARTS_USE_NAMESPACE

using namespace qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage;

typedef std::vector<CCodeSet_Load_DB>	WindowCodeSetVec;
typedef WindowCodeSetVec::iterator		WindowCodeSetVecItr;
typedef QVector<QLineSeries *>			PQLineSeriesVec;

namespace Ui
{
class CDlgFireManageEdit;
}

class CDlgFireManageEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgFireManageEdit(QWidget *parent = nullptr);
    ~CDlgFireManageEdit();

private slots:

    void on_comboBox_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_comboBox_3_currentIndexChanged(int index);

    void on_comboBox_4_currentIndexChanged(int index);

    void on_comboBox_5_currentIndexChanged(int index);

    void on_pushButton_3_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_4_clicked();

    void on_listWidget_currentTextChanged(const QString &currentText);

private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

public:

    void InitControl();
    void UpdateLinePointCombobox();
    void InitLoadCombobox();

    void ConnectControl();

    void SetCodeSetData(const CCodeSet_Load_DB_Vec CodeSetVec);
    void SetTaskID(const std::string strID);
    void SetPlatformID(const std::string strID);
    void SetLoadID(const std::string strID);
    void SetLineID(const int nLineID);
    void SetPointID(const int nID);
    void SetMarkIDList(const QStringList strList);

    void UpdateListData();

    void UpdateListDataState(const QString strCodeName, const int strCodeID, const unsigned int nNo, const unsigned int nStates);

    void CalculateMark();

    void InitChart();
    void ShowChartCCIP(std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> &cciplist, double &dAlt);
    void ShowChartMark(QLineSeries *pSeries, double &dMarkDis);
    void UpdateChart();
    void UpdateCodeSetLoadCommonCommand(CodeSetLoad_CommonCommand &stCode, int &nCoidID, int &nCodePara);
    void ClearMarkSerious();
    double Get80mmFireRange(double dLon, double dLat, double dAlt);
private:
    Ui::CDlgFireManageEdit *ui;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::MarkManage::IMarkManageService			*m_pIMarkManageService = nullptr;

    QMap<QCheckBox *, QString>								m_pQCheckBoxMarkMap;

    CCodeSet_Load_DB_Vec									m_CodeSet_Load_DB_Vec;
    CCodeSet_Load_DB_Vec									m_CodeSet_Load_DB_Wnd_Vec;
    QStringList												m_MarkList;
    std::string												m_strTaskID;
    std::string												m_strPlatformID;
    std::string												m_strLoadID;
    int														m_LineID;
    int														m_PointID;

    QChart													*m_chart;

    QValueAxis												*m_axisX;
    QValueAxis												*m_axisY;

    QLineSeries											*m_lineSeries;
    QLineSeries											*m_lineSeriesMark;
    PQLineSeriesVec											m_pQLineSeriesVec;

    int														m_AXIS_MAX_X = 10;
    int														m_AXIS_MAX_Y = 10;

};

#endif
