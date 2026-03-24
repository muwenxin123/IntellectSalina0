#ifndef CDLGACTIONCOMPENDIUM_H
#define CDLGACTIONCOMPENDIUM_H

#include <QWidget>
#include "DataManage/NebulaTask/NebulaTask.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/INebulaTaskManageService.h"
#include "LyDialog.h"
#include "CDlgOpenFile.h"
#include "CDlgEditLine.h"
#include "CDlgMissionList.h"

typedef std::vector<CNebulaTask *>					NebulaTaskPtrVec, *LPNebulaTaskPtrVec;
typedef NebulaTaskPtrVec::iterator					NebulaTaskPtrVecItr;

typedef std::vector<int>							WndListSelectNum, *LPWndListSelectNum;
typedef WndListSelectNum::iterator					WndListSelectNumItr;

namespace Ui
{
class CDlgActionCompendium;
}

class ITaskManagerActivator;
class CDlgActionCompendium : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgActionCompendium(QWidget *parent = nullptr);
    ~CDlgActionCompendium();

    void SetDlgOpenFile(CDlgOpenFile *pDlgOpenFile);
    void SetDlgEditLine(CDlgEditLine *pDlgEditLine);
    void SetDlgMissionList(CDlgMissionList *pDlgList);

    CDlgOpenFile				*m_pDlgOpenFile = nullptr;
    CDlgEditLine				*m_pDlgEditLine = nullptr;
    CDlgMissionList			*m_pDlgMissionList = nullptr;

private:
    Ui::CDlgActionCompendium	*ui;

protected:

    virtual void changeEvent(QEvent *event);

public:
    void UpdateTaskInfoToWnd();
    void ClearWndTaskInfo();
    void UpdateTaksListdate();
    void DeleteTaskListdate(int nlistselect);

    void UpdatePlatformToWnd(int nlistselect);
    void ClearWndPlatformData();
    void UpdatePlatformListDate();

    void UpdateMarkListToWnd(int nlistselect);
    void UpdateMarkListdate();
    void ClearMarkListdata();

    void UpdateAreaListToWnd(int nlistselect);
    void UpdateAreaListdate();
    void ClearAreaListdate();

    void UpdateEidtCtrl(int nlistselect);
    void ClearEidtText();
    void SetEditStates(bool bstates);
    void ClearListData();
    void SetListStates(bool bstates);

    void CreateTableHeaders();

private slots:

    void openpushButton();

    void openpushButtonByMission(QString &strFileName);

    void SaveXML();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_5_clicked();

    void on_tableWidget_itemChanged(QTableWidgetItem *item);

    void on_tableWidget_cellClicked(int row, int column);

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

private:
    int						m_Flag_btn;
    QPoint					m_pscreen;

    int						m_nListSelectHitrow;

    NebulaTaskPtrVec		m_vecNebulaTaskPtr;
    NebulaTaskPlatformMap	m_mapNebulaTaskPlatform;
    NebulaMarkInfoList	m_listNebulaMarkInfo;
    NebulaRegionInfoList	m_listNebulaRegionInfoList;
    WndListSelectNum		m_vecWndListSelectNum;

    int						m_nTabselect;
    bool					m_bAddstates;
    bool					m_bEditstates;
    NebulaTaskType			m_NebulaTaskType;
    QString					m_FileName_Opened;
    qnzkna::TaskManage::INebulaTaskManageService *m_pITaskManageService = nullptr;
    qnzkna::WeaponManage::IWeaponManageService *m_pIWeaponManageService = nullptr;
};

#endif
