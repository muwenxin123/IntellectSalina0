#ifndef CDLMISSIONALLOCATION_H
#define CDLMISSIONALLOCATION_H

#include <QWidget>
#include"LyDialog.h"
#include "LyMessageBox.h"
#include "mutiComboBox.h"
#include "ThreadSavetoSql.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include "CDlgSetPlat.h"
#include "ui_CDlgMissionAllocation.h"

namespace Ui
{
class CDlgMissionAllocation;
}
class CDlgMissionAllocation : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMissionAllocation(QWidget *parent = nullptr);
    ~CDlgMissionAllocation();

    void Init();

    void defaultPlatMatch_MissionTime();

    void localWeaponNameBindToId();

    std::string PlatTypeEnumToStr(short int t);

    std::vector<std::string> getAllCtrlers();

    std::vector<std::string> WeaponIDToNames(const std::vector<std::string> names);

    void deleteMissionIDWeapoinName(string MissionID, int weaponTypeInt, string weaponName);

    void stackMissions();
    time_t StringToTime_t(std::string strTime);

    void updateMissionTypeOldToNewName(string missionID, int weaponTypeInt, string weaponTypeName, int platIndex, string oldName, string newName);
    void addMissionIDWeapoinName(string MissionID, int weaponTypeInt, string weaponName);

    void initComBoBox(QStringList strList);
    void setComBoBoxIndex(int Index);
    int  getComCruIndex();
    void initMissionName(QString strName);
    QComboBox *getMissionComBox();

    void setPlatType(missionPlatType _platType);
    missionPlatType getPlatType();
    void updateDialog();

    void setConBoxCurIndexNewName(int curIndex, string newName);

    std::vector<std::string> getPlatNames();
    std::vector<std::string> getCtrlers()
    {
        return platType.getControlers();
    }
    std::string getMissionName()
    {
        return platType.getMissionName();
    }
    std::string getMissionID()
    {
        return platType.getMissionID();
    }

    void setIsSetReTrue(bool _isSetRe)
    {
        isSetRe = _isSetRe;
    }

    void updateItemsColor();

    void saveOneToSql(std::string platTypeName, std::string weaponName, std::string MissionID, std::string CtrlerName);

    void saveMissionPlatToSql(missionPlatType *platType);

    void updateCtrler(string missionID, string weaponTypeName, int oldIndex, string newName);

    void LoadDBToTable();
    void on_indicator_hidePopupedDJ();
    void on_indicator_hidePopuped2();
    void ConnectHidePopupedDJ();
    void ConnectHidePopupedDJ2();
    void DisConnectHidePopupedDJ();
    void SetPlatTypeSize();

    void ObtainAllData();

    void CreateComboboxTaskID();

    void CreateTableWidget();

    void ObtainCurrentTextData();

    void UpdatatbwMarkAmmoMatch();

    void ConnectHidePopuped();

    void DisConnectHidePopuped();

    void SaveData();

    void showEvent(QShowEvent *event);

    void closeEvent(QCloseEvent *event);

protected:
    virtual void resizeEvent(QResizeEvent *event)override;
public slots :

    void on_pbEnsure_clicked();
    void on_pbCancel_clicked();

    void checkIsRepetition(const QString &text);

    void ctrlChanged(const QString &text);

    void setCurMission(int curIndex);

    void on_cbxmode_currentTextChanged();
    void on_btnconfirm_clicked();
    void on_btnexit_clicked();
    void on_createcombo_changed();

    void on_pbtnOK_Clicked();

    void on_pbtnCancel_Clicked();

    void on_currentIndexChangedTaskID(const QString &text);

    void on_indicator_hidePopuped();

    void on_btnComplete_clicked();
    void on_btnCancel_1_clicked();
signals:

    void on_pbEnsure_isClicked();

public:
    CNeubulaMissionVec			m_MissionVec;

    qnzkna::WeaponManage::WeaponSystemMap					 weaponSystemMap;
    std::vector<missionPlatType *> missionPlats;
    std::map<std::string, std::string> SystemSysNameAndID;
    std::map<std::string, std::string> SystemSysIDAndName;
    std::map<short int, std::vector<std::string>> WeaponTypeNames;
    std::map<std::string, std::vector<std::string>>  interMissions;
    std::map<std::string, std::map<std::string, std::string>> missionWeaponNameInterMIDs_used;
    std::map<std::string, std::map<int, std::vector<std::string>>> missionWeaponTypeAndNames_usabled;

    missionPlatType platType;
    bool isSetRe = false;
    bool isSaveSuccess = false;

    CRelationMissionResourceVec m_pRelationMissionResourceVec;
    CRelationMissionResourceVec m_pRelationMissionResourceCurrVec;
    CRelationMissionResourceVec m_tmpread;
    CNeubulaConntionAmmoVec  m_pConntionAmmoVec;
    vector<std::pair<QString, QString>> m_pCurrPlatAmmoVec;
    CNeubulaMissionVec		 m_pMissionAllVec;
    CNeubulaAmmunitionInfoVec	m_pAllAmmoVec;
    NebulaResourcesVec			m_pAmmoType;
    string WeaponComponentName;

    QMap<int, QString>										m_mAmmoModeData;
    map<int, QMap<int, QString>>							m_mRowPriAmmoModeData;
    map<int, QMap<int, QString>>							m_mRowSedAmmoModeData;
    vector<string> ptpp;
    std::vector<std::pair<string, string>>idname;

    vector<QComboBox *> NewComboVec;

    vector<pair<int, QString>> m_AmmoCurrVec;
    vector<pair<string, map<std::string, int>>>	m_pMapAmmoCountVec;

    string	 strListCurrent = "";
    QStringList currList;
    QStringList currList2;

    std::string                        m_sCurrentTask;
    bool is_init = false;
    int loadindex = 0;

    std::vector<std::string>                                m_vTaskVec;
    CNeubulaConntionAmmoVec                                 m_cConntionAmmoVec;
    CNeubulaAmmoConntionMarkVec                           m_cAmmoConntionMarkVec;
    std::vector<std::string>                                m_vAmmoModeVec;
    QMap<std::string, std::vector<std::string>>              m_vMarkAmmoModeMap;
    CRelationMissionMarkVec                               m_taskMarkID;
    QMap<int, QString>										m_mAmmoModeDataDM;
    QStringList												m_indicators;
    bool                                                    IsHidePopuped = false;
private:
    Ui::CDlgMissionAllocation *ui;
    qnzkna::AmmunitionManage::IAmmunitionManageService     *m_pAmmunitionManageService = nullptr;
    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
};
#endif
