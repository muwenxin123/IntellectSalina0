#ifndef CDLGFIREMANAGE_H
#define CDLGFIREMANAGE_H

#include <LyDialog.h>
#include <QWidget>
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMission.h"

namespace Ui
{
class CDlgFireManage;
}
class QMenu;
class QTreeWidgetItem;

class CDlgFireManage : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgFireManage(QWidget *parent = nullptr);
    ~CDlgFireManage();
public:

    void CreateTree();
    void UpdateTreeData();
    void CreateTableMenu();
    void ControlConnet();

    void CreateListHeading();
    void UpdateListData(std::string strTaskID = "", std::string strPlatformID = "", std::string strLoadID = "");
    void UpdataWindowData();
    void UpdateTaskCombobox();

    int GetSelectedTreeLevel(QMap<int, QString> &TreeDataMap);
    int GetSelectedTreeLevel();
    void GetTaskInfo(QString &strTaskID, QString &strPlatformID, QString &strLoadID);
    void CalculateMark();

    std::vector<std::string> GetMarkList();
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
    Ui::CDlgFireManage *ui;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;

    CRelationActionTaskVec									m_RelationActionTaskVec;
    CNebulaAcitonRelationVec								m_AcitonRelationShowVec;
    CCodeSet_Load_DB_Vec									m_CodeSet_Load_DB_Vec;

    QMenu													*m_TreeMenu;
    QMenu													*m_TableMenu;

    int														m_nTreeLevel = 0;
    QString													m_TaskID;
    QString													m_PlatformID;
    QString													m_LoadID;

    std::vector<std::string>								m_MarkList;
};

#endif
