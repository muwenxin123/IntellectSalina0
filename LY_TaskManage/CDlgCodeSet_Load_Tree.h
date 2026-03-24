#ifndef CDLGCODESET_LOAD_TREE_H
#define CDLGCODESET_LOAD_TREE_H

#include <QWidget>
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include "QMenu"
#include "QTreeWidget"

namespace Ui
{
class CDlgCodeSet_Load_Tree;
}

class CDlgCodeSet_Load_Tree : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgCodeSet_Load_Tree(QWidget *parent = nullptr);
    ~CDlgCodeSet_Load_Tree();

private:
    Ui::CDlgCodeSet_Load_Tree *ui;

private:
    qnzkna::TaskManage::INebulaTaskManageService			            *m_pITaskManageService;

    CCodeSet_Load_DB_Vec												m_CodeSet_Load_DB_Vec;
    CodeSetLoadVec														m_CodeSetLoadVec;

    QMenu																*m_TreeMenu;
    QAction																*m_MenuAdd;
    QAction																*m_MenuEdit;
    QAction																*m_MenuRemove;
    QAction																*m_MenuUpdate;

public:
    void CreateTree();
    void UpdateWindowData();
    void UpdateTreeData();
    void CreateTreeMenu();

    void AddCodeSetLoad(const CodeSetLoad stCodeSetLoad);
    void AddCodeSetLoad_Point(const std::string strTaskID, const std::string strPlatformID, const CodeSetLoad_Point stCodeSetLoad_Point);
    void AddCodeSetLoad_Parameter(const std::string strTaskID, const std::string strPlatformID, const int nPointID, const CodeSetLoad_Parameter stCodeSetLoad_Point);

    int GetSelectedTreeLevel();
    CCodeSet_Load_DB getSelectedInfo(const int nLevel);

    QString TransformNumberToCode(int nNo);
    QString TransformNumberToChinese(int nNo);

    CodeSetLoadVec GetSortOutData();
private slots:

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_Menu_Add();

    void on_Menu_Edit();

    void on_Menu_Remove();

    void on_Menu_Update();
};

#endif
