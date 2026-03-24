#ifndef CDLGZONELIST_H
#define CDLGZONELIST_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QMutex>
#include "DataManage/Common/TypeDefine_Node.h"
#include "DataManage/Mark/Mark.h"
#include "Protocol/XygsZC/Bag_XygsZCDefines.h"
#include "QTableWidget"
#include "DataManage/Mark/ZoneInfo.h"
#include "DataManage/INebulaTaskManageService.h"
#include "Protocol/XygsTelegraph/TelegrpahHead.h"
#include "DataManage/IWeaponManageService.h"
#include "lyUI/LyWidget.h"
#include "CombatZoneEditDialog.h"

namespace Ui {
class CDlgZoneList;
}

class CDlgZoneList : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgZoneList(QWidget *parent = nullptr);
    ~CDlgZoneList();

private:
    Ui::CDlgZoneList*										ui;

    QMenu													m_TableMenuCombatZoneList;

    QStringList												m_headerlabels;

    CZoneInfoVec										    m_ZoneInfoVec;

    bool													m_bUpdateFlg;

    CombatZoneEditDialog*								    m_CombatZoneEditDialog;

    QMutex													m_MutexXygsZC_02Map;
    std::map<std::string, XygsZC_02>						m_XygsZC_02Map;
    QStringList												m_DBTargetType;

    int														m_nActionID;
    int														currActionID;
    bool													m_bPressEvect = true;

    std::map<std::string, std::string>						m_SystemSysNameAndID;

    qnzkna::WeaponManage::IWeaponManageService*				m_pIWeaponManageService = nullptr;

public:

    void TabWidgetConnet();
    void CreateTableMenu();

    void showCombatZoneEditDialog(int nType, CZoneInfo& stInfo);
    void CreateZoneEditDialog();
    CombatZoneEditDialog *GetCombatZoneDialog();
    CombatZoneEditDialog* GetZoneEditDialog();

    void UpdateTrackInfoToListHeading_CombatZone();
    void UpdataWindowData_Zone();
    void UpdateListData_CombatZone();

    CZoneInfoVec GetZoneInfoVec();

    QString GetZoneAttributeString(EZoneAttribute eType);
    QString GetZoneShapeString(EZoneShape eType);

    void localWeaponNameBindToId();

    void TableWidgetAddCheckBox(QTableWidget *tableWidget, int x, int y, QString text, Qt::CheckState CheckState);

private:

    void showEvent(QShowEvent *event);

private slots:

    void selectBox(int r, int c);

    void on_tableWidget_CombatZone_cellDoubleClicked(int row, int column);
};

#endif
