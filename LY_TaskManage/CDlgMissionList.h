#ifndef CDLGMISSIONLIST_H
#define CDLGMISSIONLIST_H

#include <QWidget>
#include <QMenu>

#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"
#include "DataManage/IMarkManageService.h"
#include "DataManage/NebulaTask/NebulaOperationalInformation.h"
#include "ITaskManagerActivator.h"
#include "QTableWidget"
#include "CButton_setPlat.h"

class QGroupBox;

namespace Ui
{
class CDlgMissionList;
}

class CDlgMissionList : public QWidget, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT

public:
    explicit CDlgMissionList(QWidget *parent = nullptr);
    ~CDlgMissionList();

    bool						b_openTask = false;

    QTimer *m_MsgScheme;
private:
    Ui::CDlgMissionList *ui;

    QMenu						*m_TableMenu;
    QMenu						*m_StateMenu;
    QMenu						*m_DistributeMenu;
    QAction						*m_MenuAdd;
    QAction						*m_MenuEdit;
    QAction						*m_MenuRemove;
    QAction						*m_MenuUpdate;
    QAction                     *m_MenuSaveAs;
    QAction                     *m_MenuEditState;
    QAction                     *m_MenuUnexecuted;
    QAction                     *m_MenuExecuting;
    QAction                     *m_MenuComplete;
    QAction                     *m_MenuCancellation;
    QAction                     *m_MenuAdvance;
    QAction                     *m_MenuDelay;
    QAction                     *m_MenuFail;
    QAction                     *m_MenuMissionAllocation;
    QAction                     *m_MenuDistribute;

    CNeubulaMissionVec			m_MissionVec;

    bool						m_bUpdateFlg;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::MarkManage::IMarkManageService			*m_pIMarkManageService = nullptr;

    int							m_old_CurrentTabNo = -1;
    int							m_old_CurrentTableRow = -1;

    std::map<std::string, std::vector<std::string>> SysNameAndType_Default;
    std::map<std::string, std::string> SysNameAndType;

    std::map<std::string, CButton_setPlat*> PlatName_Button;

    bool                                                     m_bPressEvect = true;
    int                                                      currActionID;
    int                                                      m_nActionID;
public:
    CNeubulaMissionLineVec                                   m_MissionLineVec;
    bool                                                     m_bIsSaveAs = false;
    std::string                                              m_sMissionName;

public:

    void TabWidgetConnet();
    void CreateTableMenu();

    void UpdateTrackInfoToListHeading();

    void UpdataWindowData();
    void UpdateListData();
    void UpdataListCtrlData(QTableWidget *pQTable, CNeubulaMission &stMarkInfo, int &nNo);
    void UpdateListData_All();
    void UpdateListData_Attack();
    void UpdateListData_Dectect();
    void UpdateListData_Protect();
    void UpdateListData_Overlook();
    void UpdateListData_Help();
    void UpdateListData_Transport();
    void UpdateListData_Plat();

    void SetTabEnable(bool bStates);

    QString GetTypeString(ENeubulaMissionType eType);
    QString GetHurtDegreeString(ENeubulaMissionHurtDegree eType);
    QString GetHurtExeccutingState(ENeubulaMissionExeccutingState eState);
    QString GetMarkState(EMarkState eState);

    virtual void Update(qnzkna::TaskManage::INebulaTaskManageService *sub);

    void clearGroupBoxPlat(QGroupBox *groupBoxPlat);

    void updateMenu();

private:

    void showEvent(QShowEvent *event);

signals:
    void openLocalFile(QString &strFileName);
    void ShowLine(QStringList &strLineIDlist, double &dLon, double &dLat, bool bAgainFlag);

public slots:

private slots:

    void on_Menu_Add();

    void on_Menu_Edit();

    void on_Menu_Remove();

    void on_Menu_Update();

    void on_Menu_SaveAs();

    void on_Menu_Unexecuted();

    void on_Menu_Complete();

    void on_Menu_Executing();

    void on_Menu_Cancellation();

    void on_Menu_Advance();

    void on_Menu_Delay();

    void on_Menu_Fail();

    void on_Menu_MissionAllocation();

    void on_Menu_Distribute();

    void customContextMenuRequested(const QPoint &pos);

    void tabWidget_currentChanged(int index);

    void QTableWidgetClicked();

    void timerOpenTask();
};

#endif
