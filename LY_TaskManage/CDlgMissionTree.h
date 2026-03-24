#ifndef CDLGMISSIONTREE_H
#define CDLGMISSIONTREE_H

#include <QWidget>
#include "LyMessageBox.h"
#include "QMenu"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/NebulaTask/NebulaAmmunition.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IMarkManageService.h"
#include "ITaskManagerActivator.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"

namespace Ui
{
class CDlgMissionTree;
}

class CDlgMissionTree : public QWidget, public qnzkna::TaskManage::INebulaTaskManageObserver
{
    Q_OBJECT

public:
    explicit CDlgMissionTree(QWidget *parent = nullptr);
    ~CDlgMissionTree();
public:
    void CreateTree();
    void UpdateTreeData();
    void UpdateWindowData();

    int GetSelectedTreeLevel();

    void CreateMenu();

    virtual void Update(qnzkna::TaskManage::INebulaTaskManageService *sub);

    void localWeaponNameBindToId();

private slots:

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_Menu_Action_Edit();

    void on_Menu_Action_Remove();

    void on_Menu_Task_Add();

    void on_Menu_Task_Edit();

    void on_Menu_Task_Remove();

    void on_Menu_PlatForm_Add();

    void on_Menu_PlatForm_Edit();

    void on_Menu_PlatForm_Remove();

    void on_Menu_Load_Add();

    void on_Menu_Load_Edit();

    void on_Menu_Load_Remove();

    void QTreeWidgetItemClicked();
public:
    std::map<std::string, std::string>                      SystemSysIDAndName;
private:
    Ui::CDlgMissionTree *ui;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::MarkManage::IMarkManageService			*m_pIMarkManageService = nullptr;
    qnzkna::AmmunitionManage::IAmmunitionManageService     *m_pAmmunitionManageService = nullptr;

    CRelationActionTaskVec									m_RelationActionTaskVec;
    CNebulaAcitonRelationVec								m_AcitonRelationShowVec;

    qnzkna::WeaponManage::WeaponSystemMap					weaponSystemMap;

    std::vector<std::string>                                PlatIDVec;

    QMenu													*m_TreeMenu_Action;
    QMenu													*m_TreeMenu_Task;
    QMenu													*m_TreeMenu_PlatForm;
    QMenu													*m_TreeMenu_Load;

    bool                                                     m_bPressEvect = true;
    int                                                      currActionID;
    int                                                      m_nActionID;

};

#endif
