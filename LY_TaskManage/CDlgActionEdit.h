#ifndef CDLGACTIONEDIT_H
#define CDLGACTIONEDIT_H

#include <QWidget>
#include <QString>
#include <string>
#include "LyDialog.h"

#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/Mark/ZoneInfo.h"
#include "DataManage/NebulaTask/NebulaMissionPlatform.h"
#include "DataManage/NebulaTask/NebulaMissionPlatformLoad.h"
#include "DataManage/Mark/Mark.h"
#include "DataManage/NebulaTask/NebulaResources.h"
#include "CDlgAddMark.h"
#include <QDateTime>

namespace Ui
{
class CDlgActionEdit;
}
class QTableWidgetItem;

class CDlgActionEdit : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgActionEdit(QWidget *parent = nullptr);
    ~CDlgActionEdit();

    void Init();

    void SplitPlatLoadByType();

    int GetMaxActionID();

    std::vector<SystemMarkInfo> GetTaegetInfoByIDList();
    std::vector<SystemMarkInfo> GetMarkList();

    std::vector<CZoneInfo> GetActionAreaInfoList();

    NebulaResourcesVec GetPlafFromInfoList();

    NebulaResourcesVec GetSensorInfoList();

    NebulaResourcesVec GetAmmoInfiList();

    bool IsMarkInRegion(SystemMarkInfo target);

    bool is_point_in_zone(const QPointF point, const std::vector<QPointF> ZonePoints);

    std::vector<QPointF> SetAreaToQPointF(ZonePointVec pointVec);

    void UpdateCurrAction(CNeubulaAction Action);

    void ReadResourceNumber();

    void closeEvent(QCloseEvent *e);

    void addMark(QString strName, QString strID);

    void sorttbwMark_2();

    void sorttbwMark();
public slots:
    bool on_pbtnOK_clicked();

    void on_pbtnCancel_clicked();

    void on_tbwArea_itemDoubleClicked(QTableWidgetItem *item);
    void on_tbwArea_2_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbwMark_itemDoubleClicked(QTableWidgetItem *item);
    void on_tbwMark_2_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbwPlatResource_itemDoubleClicked(QTableWidgetItem *item);
    void on_tbwPlatResource_2_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbwSensorResource_itemDoubleClicked(QTableWidgetItem *item);
    void on_tbwSensorResource_2_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbwAmmoResource_itemDoubleClicked(QTableWidgetItem *item);
    void on_tbwAmmoResource_2_itemDoubleClicked(QTableWidgetItem *item);

    void on_tbwOtherResource_itemDoubleClicked(QTableWidgetItem *item);
    void on_tbwOtherResource_2_itemDoubleClicked(QTableWidgetItem *item);

    void on_pbtnLeft_0_Clicked();
    void on_pbtnRight_0_Clicked();

    void on_pbtnRight_1_Clicked();
    void on_pbtnLeft_1_Clicked();

    void on_pbtnLeft_2_Clicked();
    void on_pbtnRight_2_Clicked();

    void on_pbtnLeft_3_Clicked();
    void on_pbtnRight_3_Clicked();

    void on_pbtnLeft_4_Clicked();
    void on_pbtnRight_4_Clicked();

    void on_pbtnLeft_5_Clicked();
    void on_pbtnRight_5_Clicked();

    void Update_tbwArea();
    void UpdateSeleAreaTab();
    void Update_tbwMark();
    void UpdateMark_2();

    void Update_tbwPlatResource();
    void Update_tbwSensorResource();
    void Update_tbwAmmoResource();
    void Update_tbwOtherResource();

    void GetTextFromLedtID();
    void GetTextFromLedtName();
    void GetTextFromLedtDH();
    void GetTextFromLedtAim();
    void GetTextFromLedtRemark();
    void GetTextFromLedtVisibility();

    void GetTextFromcbxSeason(const QString &text);
    void GetTextFromcbxDayNight(const QString &text);
    void GetTextFromcbxWeather(const QString &text);
    void GetTextFromcbxWindType(const QString &text);
    void GetTextFromcbxWindLevel(const QString &text);

    void GetTextFromDateTimeStart(const QDateTime &datetime);
    void GetTextFromDateTimeEnd(const QDateTime &datetime);

    void GetStateFromCbxActive(int state);

    void CreateAllTable();

    void on_btnAddMark_clicked();

    void on_btnAddTask_clicked();
public:
    CNeubulaAction ActionInfo;

    int ActionID;
    std::string ActionName;
    std::string ActionDH;
    std::string ActionStartTime;
    std::string ActionEndTime;
    std::string ActionRemark;
    bool isActive = false;
    std::string ActionAim;
    std::string ActionAreas;
    std::string EnvironmentInfo;

    std::string Season;
    std::string DayorNight;
    std::string Weather;
    std::string WindType;
    std::string WindLevel;
    std::string Visibility;
    int isAddorEx;
public:
    std::vector<std::string> m_MarkIDVec;

    std::vector<CNeubulaAction> CNeubulaActionVec;
    int MaxActionID = 0;

    std::vector<CZoneInfo> airspaceInfoVec;
    std::vector<CZoneInfo> selectedAreaVec;

    std::vector<SystemMarkInfo> targetInfoVec;
    std::vector<SystemMarkInfo> targetInRegionVec;
    std::vector<SystemMarkInfo> selectedMarkVec;

    CNebulaMissionPlatformVec platInfoVec;
    NebulaResourcesVec platResourceVec;
    std::vector<std::pair<NebulaResources, int>> selectedPlatVec;
    std::vector<std::pair<NebulaResources, int>> selectPlatInfoVec;

    NebulaResourcesVec SensorResourceVec;
    std::vector<std::pair<NebulaResources, int>> selectedSensorVec;
    std::vector<std::pair<NebulaResources, int>> selectSensorInfoVec;

    NebulaResourcesVec AmmoResourceVec;
    std::vector<std::pair<NebulaResources, int>> selectedAmmoVec;
    std::vector<std::pair<NebulaResources, int>> selectAmmoInfoVec;

    NebulaResourcesVec OtherResourceVec;
    std::vector<std::pair<NebulaResources, int>> selectedOtherVec;
    std::vector<std::pair<NebulaResources, int>> selectOtherInfoVec;

    CNebulaMissionPlatformLoadVec   AllLoadInfoVec;
    CNebulaMissionPlatformLoadVec   LoadPodVec;
    CNebulaMissionPlatformLoadVec   LoadAmmoVec;
    CNebulaMissionPlatformLoadVec   LoadCommVec;

private:
    Ui::CDlgActionEdit *ui;
};

#endif
