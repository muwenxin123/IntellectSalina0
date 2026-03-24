#ifndef CDLGTASKPLATMATCH__H
#define CDLGTASKPLATMATCH__H

#include <QWidget>
#include <time.h>
#include <stdio.h>
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"

#include "LyDialog.h"
#include <map>
#include <string>

#include "ui_CDlgTaskPlatMatch.h"
#include "MissionPlatType.h"
#include "CDlgSetPlat.h"
#include "ThreadSavetoSql.h"

class CDlgSetPlat;

namespace Ui
{
class CDlgTaskPlatMatch;
}

class CDlgTaskPlatMatch : public QWidget, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT

public:
    explicit CDlgTaskPlatMatch(QWidget *parent = nullptr);
    ~CDlgTaskPlatMatch();

    void UpdataWindowData();

    void UpdataPlatList();

    void defaultPlatMatch();

    void localWeaponNameBindToId();

    std::vector<std::string> WeaponIDToNames(const std::vector<std::string> names);

    std::string PlatTypeEnumToStr(short int t);

    void saveToSql();

    void saveOneToSql(std::string platTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName);

    void saveMissionPlatToSql(missionPlatType *platType);

    std::vector<std::string> getAllCtrlers();

    CDlgSetPlat *getDlgSetPlat();
    QTableWidget *getTabWidget();

    virtual void Update(qnzkna::TaskManage::INebulaTaskManageService *sub);

    map<string, map<string, string>> getMissionWeaponPlats();

    std::vector<missionPlatType *> *getMissionPlatTypes()
    {
        return &missionPlats;
    }

    void removeMissionIDweaponID(string missionID, string weaponID, string platName);

    void addMissionIDWeaponID(string missionID, string weaponID, string platName, int platIndex);

    void updateMissionIDWeaponID(string missionID, string weaponID, string platName, int platIndex);

    void stackMissions();
    time_t StringToTime_t(std::string strTime);

    void defaultPlatMatch_MissionTime();

    void deleteMissionIDWeapoinName(string MissionID, int weaponTypeInt, string weaponName);
    void addMissionIDWeapoinName(string MissionID, int weaponTypeInt, string weaponName);

    std::map<std::string, std::map<int, std::vector<std::string>>> getMissionWeaponTypeAndNames()
    {
        return missionWeaponTypeAndNames_usabled;
    }

    void updateMissionTypeOldToNewName(string missionID, int weaponTypeInt, string weaponTypeName, int platIndex, string oldName, string newName);

    void swapMissionTypeOldToNewName(string missionID, string weaponTypeName, int OldIndex, string oldName, int NewIndex, string newName);

    void swapDiffMissionTypeOldToNewName(string missionID, int weaponTypeInt, string weaponTypeName, int OldIndex, string oldName, string newName);

    void updatePlatShow(int missionPlatIndex, vector<string> platNames);

    void updateCtrler(string missionID, string weaponTypeName, int oldIndex, string newName);

    void updateCtrlShow(int missionPlatIndex, vector<string> ctrlNames);

private:

    void showEvent(QShowEvent *event)
    {
        UpdataWindowData();
    }

public slots :

    void on_pBMatch_clicked();

    void on_setMatch_clicked();
    void on_setMatch_clickedItem(QTableWidgetItem *item);

    void on_MatchCheck_clicked();

    void on_pBMatchCancel_clicked();

    void updateMatched();

    void updateMatchedIndex(int SelectedRow);

    void tableWidget_cellClicked();

    void setCurMission(int curIndex);

private:
    Ui::CDlgTaskPlatMatch *ui;

    CNeubulaMissionVec			m_MissionVec;
    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;

    qnzkna::WeaponManage::WeaponSystemMap					 weaponSystemMap;

    CDlgSetPlat			*DlgSetPlat = nullptr;

    std::map<std::string, std::map<std::string, std::string>> missionWeaponPlatNames;
    std::vector<missionPlatType *> missionPlats;
    std::map<std::string, std::string> SystemSysNameAndID;
    std::map<std::string, std::string> SystemSysIDAndName;

    std::map<short int, std::vector<std::string>> WeaponTypeNames;

    std::map<std::string, std::vector<std::string>>  interMissions;
    std::map<std::string, std::map<std::string, std::string>> missionWeaponNameInterMIDs_used;
    std::map<std::string, std::map<int, std::vector<std::string>>> missionWeaponTypeAndNames_usabled;

    bool                                                     m_bPressEvect = true;
    int                                                      currActionID;
    int                                                      m_nActionID = 0;
};

#endif
