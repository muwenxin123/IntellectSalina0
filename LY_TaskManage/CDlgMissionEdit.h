#ifndef CDLGMISSIONEDIT_H
#define CDLGMISSIONEDIT_H

#include <QWidget>
#include <QMenu>
#include "LyDialog.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IMarkManageService.h"
#include "DataManage/Common/TypeDefine_Node.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/ICommandUnitManageService.h"
#include "CDlgMissionPlatformInfo.h"
#include "QGeoCoordinate"
#include "MissionLinePoint.h"
#include "Protocol/XygsTelegraph/TelegrpahHead.h"
#define PICK_POINT_NORMAL       1
#define PICK_POINT_EDIT         2

namespace Ui
{
class CDlgMissionEdit;
}
class QTableWidgetItem;

class CDlgMissionEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMissionEdit(QWidget *parent = nullptr);
    ~CDlgMissionEdit();

private:
    int																		m_DataType;

    CNeubulaMission															m_stMission;

    std::string                                                             m_ShcemeMarkID;
    std::string                                                             m_ShcemeDamageRequest;

    Ui::CDlgMissionEdit														*ui;

    qnzkna::TaskManage::INebulaTaskManageService							*m_pITaskManageService = nullptr;
    qnzkna::MarkManage::IMarkManageService							*m_pIMarkManageService = nullptr;
    qnzkna::SystemConfig::IConfigProjectControlService						*m_pIConfigProjectControlService = nullptr;
    qnzkna::WeaponManage::IWeaponManageService								*m_pIWeaponManageService = nullptr;
    qnzkna::CCommandUnitManage::ICommandUnitManageService					*m_pICommandUnitManageService = nullptr;

    qnzkna::NodeManage::CNodeRelationInfoPtrVector							m_vecNodeInfo;
    CRelationActionResourceVec                                              m_CRelationActionResourceVec;

    int																		m_PickPointStates;

    QGeoCoordinate startPoint;
    std::string strMarkId;
    std::string strLineName;

    CNeubulaMissionLinePointVec												m_RouteLineVec;
    CNeubulaMissionLineVec                                                  m_MissionLineVec;
    QMenu													               *m_mSelectedAmmoTableMenu;
    bool                                                                    m_bIdAmmoEmpty = false;
public:
    CRelationMissionAmmoVec                                                 m_CRelationMissionAmmoVec;
    std::string                                                             m_sSelectMarkID;
    std::string                                                             m_sSchemeID;
    QMap<std::string, std::string>                                          m_mMarkAndSchemeID;
public:
    void InitData();
    void InitWindowCtrl(int nTabIndex);
    void SetWindowInfo(CNeubulaMission &stInfo);
    void SetMStMission();
    void ShowTop();
    void SetDataType(int nType);
    void InitTimeEdit();

    void InitNodeInfoVec();
    void InitNodeInfoVecNew();

    qnzkna::NodeManage::CNodeRelationInfoPtrVector GetNodeInfoVec();
    int GetPickPointStates();
    double GetStartLon();
    double GetStartLat();
    void SetPostion(double &dLon, double &dLat, double &dHeight);

    void CreateTableHead();

    bool JudeMissionMark(const std::string strID);
    bool JudeMissionPlat(const std::string strName, const int nNumTotal, int &nNumLave);

    void UpdateMarkBackup();
    void UpdateMarkSelect();

    void UpdatePlatData();

    void UpdateEquBackup();
    void UpdateEquSelect();

    void UpdateEquSelect(PlatfromInfo platfrominfo);
    void UpdateEquBackup(int currentRow);
    void UpdateEquBackup1();
    void UpdateEquSelect1();

    void UpdataAmmoBackup();
    void UpdateAmmoSelect();
    void AddQRegExp();

    void SortPlatType();

    int CountcurrentWQNum(QString text);

    QStringList CreatTableHeadEquipInfo();
    void SaveDataToSturct();

    void AddPlatButton(QString platTyp, int currentRow);

    void SchemeUpdateMark(const std::string strMarkID, const std::string strDamageRequest);

    void AcceptOnlyMarkInfo(const char *msgInfo, const std::string targetID, const std::string strlineName,
                              QGeoCoordinate startpoint);

    void AcceptOnlyLineInfo(const char *msgInfo, const string &strsize);

    void SaveMissionAmmo();

    void CreatetbwSelectedAmmoMenu();

    CRelationMissionAmmoVec GettbwSelectedAmmoData();

    void SchemeGenerateTask();
private:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);

signals:
    void MsgOnlyMark();
    void DlgMissionEditShow();
private slots:
    void sendOnlySlots();

    void on_valueChanged(int value);

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_pushButton_6_clicked();

    void on_pushButton_7_clicked();

    void on_pushButton_8_clicked();

    void on_pushButton_9_clicked();

    void on_tableWidget_2_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_4_itemDoubleClicked(QTableWidgetItem *item);

    void on_tableWidget_3_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbwAlternateAmmo_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbwSelectedAmmo_itemDoubleClicked(QTableWidgetItem *item);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_Menu_MarkSchemeID_select();

    void on_combobox_currentIndexChanged(int value);

    void on_Start_DataTimeChanged();

    void on_End_DataTimeChanged();
};

#endif
