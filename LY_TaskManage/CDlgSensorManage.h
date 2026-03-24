#ifndef CDLGSENSORMANAGE_H
#define CDLGSENSORMANAGE_H

#include <LyDialog.h>
#include <QWidget>
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/INebulaAmmunitionManageService.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/NebulaTask/NebulaAmmunition.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"

namespace Ui
{
class CDlgSensorManage;
}
class QMenu;
class QTreeWidgetItem;

class CDlgSensorManage : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgSensorManage(QWidget *parent = nullptr);
    ~CDlgSensorManage();
public:

    void CreateTree();
    void UpdateTreeData();
    void CreateTableMenu();
    void ControlConnet();

    void CreateListHeading();

    void UpdataWindowData();
    void UpdateTaskCombobox();

    int GetSelectedTreeLevel(QMap<int, QString> &TreeDataMap);
    int GetSelectedTreeLevel();
    void GetTaskInfo(QString &strTaskID, QString &strPlatformID, QString &strLoadID);

    void localWeaponNameBindToId();

    void showEvent(QShowEvent *event);

    void UpdataListData(QString TaskID = "", QString Platfrom = "");

private slots:

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

    void on_comboBox_currentIndexChanged(int index);

    void on_Menu_Tree_Add();

    void on_Menu_Tree_Remove();

    void on_Menu_Table_Add();

    void on_Menu_Table_Edit();

    void on_Menu_Table_Remove();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::CDlgSensorManage *ui;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::AmmunitionManage::IAmmunitionManageService     *m_pAmmunitionManageService = nullptr;

    CRelationActionTaskVec									m_RelationActionTaskVec;
    CNebulaAcitonRelationVec								m_AcitonRelationShowVec;
    CCodeSet_Load_DB_Vec									m_CodeSet_Load_DB_Vec;

    QMenu													*m_TreeMenu;
    QMenu													*m_TableMenu;

    int														m_nTreeLevel = 0;
    QString													m_TaskID;
    QString													m_PlatformID;
    QString													m_LoadID;
    int                                                     m_nRoutePointID;
    int                                                     m_ExecutionOrser;
    qnzkna::WeaponManage::WeaponSystemMap					weaponSystemMap;
    std::map<std::string, std::string>                      SystemSysIDAndName;
};

#endif
