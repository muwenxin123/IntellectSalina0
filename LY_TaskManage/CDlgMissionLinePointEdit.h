#ifndef CDLGMISSIONLINEPOINTEDIT_H
#define CDLGMISSIONLINEPOINTEDIT_H

#include <QWidget>
#include "LyDialog.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMissionLine.h"
#include "CommonStruct.h"

namespace Ui
{
class CDlgMissionLinePointEdit;
}
class QMenu;

class CDlgMissionLinePointEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMissionLinePointEdit(QWidget *parent = nullptr);
    ~CDlgMissionLinePointEdit();

private:
    Ui::CDlgMissionLinePointEdit *ui;

public:
    void InitWindow();
    void SetWindowData(CNeubulaMissionLine stline);
    void SetWindowStates(int nType);

    void TabWidgetConnet();
    void CreateTableMenu();

    void CreateListHeading();
    void UpdateListData();
    void UpdateListData_XY54();
    void UpdatePoint_XY54(const CNeubulaMissionLinePoint stPoint, const int nNo);
    void UpdateLoaclDistance(const int nRow, const int nColum);

    int GetMaxPointNo();
    bool AddLinePointMouseDoubleClick(double dLon, double dLat, double dHeight);
    bool UpdateLinePointMouseMove(int nLineID, int nPointID, double dLon, double dLat);
    void UpdateListLinePoint(int nPointID, double dLon, double dLat);

    void UpdateCtrlData();
    void UpdateLineDistance();

    bool GetShowInfoByLineID(const int nLineID, QString &strTaskID, QString &strPlatformID);

    EeditStates GetLineEditStates();

    bool CheckLineEdit();
private:

    void showEvent(QShowEvent *event);

private slots:

    void on_comboBox_9_currentIndexChanged(int index);

    void on_pushButton_11_clicked();

    void on_pushButton_12_clicked();

    void on_pushButton_13_clicked();

    void on_pushButton_14_clicked();

    void on_pushButton_15_clicked();

    void on_pushButton_9_clicked();

    void on_pushButton_10_clicked();

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

    void OnStartuavroute();

    void OnStopuavroute();

    void OnRemovepoint();

    void OnInsetpoint();

    void OnRemoveallpoint();

    void OnEditCodeSet();

    void on_tableWidget_cellChanged(int row, int column);

    void on_pushButton_17_clicked();

    void on_pushButton_18_clicked();

private:
    CNeubulaMissionLine										m_MissionLine;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;

    QMenu													*m_TableMenu;

    int														m_nProtocolType = 0;

    int														m_nType = 0;

    EeditStates												m_EeditStates = PickNormal;
};

#endif
