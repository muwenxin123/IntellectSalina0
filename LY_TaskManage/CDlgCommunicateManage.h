#ifndef CDLGCOMMUNICATEMANAGE_H
#define CDLGCOMMUNICATEMANAGE_H

#include <LyDialog.h>
#include <QMutex>
#include <QWidget>
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/WeaponSystem/WeaponSystem.h"
#include "DataManage/INebulaAmmunitionManageService.h"
#include "shapefil.h"
#include "GetHeight.h"

class QMenu;

struct ShapLine
{
    SHPObject	*pShapLine[2] ;
    ShapLine()
    {
        pShapLine[0] = nullptr;
        pShapLine[1] = nullptr;
    }
};

typedef std::vector<ShapLine> ShapLineVec;

struct CoverPoint
{
    double dLon;
    double dLat;
    double dAlt;
    CoverPoint()
    {
        dLon = 0;
        dLat = 0;
        dAlt = 0;
    }
};

typedef std::vector<CoverPoint> CoverPointVec;

struct CoverArea
{
    int				nLineID;
    int				nPointID;
    int				nRadius;
    double			dLon;
    double			dLat;
    double			dAlt;
    bool			bShow;
    CoverPointVec	m_CoverPointVec;
    CoverArea()
    {
        nLineID = 0;
        nPointID = 0;
        nRadius = 0;
        dLon = 0;
        dLat = 0;
        dAlt = 0;
        bShow = false;
    }
};
typedef std::vector<CoverArea> CoverAreaVec;

namespace Ui
{
class CDlgCommunicateManage;
}
class QTreeWidgetItem;

class CDlgCommunicateManage : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgCommunicateManage(QWidget *parent = nullptr);
    ~CDlgCommunicateManage();

public:

    void CreateTree();
    void UpdateTreeData();
    void CreateTableMenu();
    void ControlConnet();

    void CreateListHeading();
    void UpdateListData(std::string strTaskID = "", std::string strPlatformID = "");
    void UpdataWindowData();
    void UpdateTaskCombobox();

    int GetSelectedTreeLevel(QMap<int, QString> &TreeDataMap);
    int GetSelectedTreeLevel();
    void GetTaskInfo(QString &strTaskID, QString &strPlatformID, QString &strLoadID);
    void ClearShapLineVec();
    ShapLineVec GetShapLineVec();
    CoverAreaVec GetCoverAreaVec();

    void localWeaponNameBindToId();

    void showEvent(QShowEvent *event);

    void closeEvent(QCloseEvent *event);

private slots:

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

    void on_comboBox_currentIndexChanged(int index);

    void on_Menu_Tree_Add();

    void on_Menu_Tree_Remove();

    void on_treeWidget_itemClicked(QTreeWidgetItem *item, int column);

    void selectBox(int r, int c);
public:
    bool isDraw = false;
    std::vector<std::vector<SHPObject *>> m_pShpVec;
    SHPObject *m_pShp[2];
private:
    Ui::CDlgCommunicateManage *ui;

    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::AmmunitionManage::IAmmunitionManageService     *m_pAmmunitionManageService = nullptr;

    CRelationActionTaskVec									m_RelationActionTaskVec;
    CNebulaAcitonRelationVec								m_AcitonRelationShowVec;
    CCodeSet_Load_DB_Vec									m_CodeSet_Load_DB_Vec;

    QMenu													*m_TreeMenu;

    int														m_nTreeLevel = 0;
    QString													m_TaskID;
    QString													m_PlatformID;
    QString													m_LoadID;
    ShapLineVec												m_ShapLineVec;

    CGetHeight												*m_pCGetHeight = nullptr;
    QString													m_HeightPatch;

    CoverAreaVec											m_CoverAreaVec;
    QMutex													m_MutexCoverArea;
    qnzkna::WeaponManage::WeaponSystemMap					weaponSystemMap;
    std::map<std::string, std::string>                      SystemSysIDAndName;
    QLabel *pic;

};

#endif
