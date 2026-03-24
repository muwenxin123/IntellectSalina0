#ifndef CDLGMISSIONPLATFORMLOADLIST_H
#define CDLGMISSIONPLATFORMLOADLIST_H

#include <QWidget>
#include <QTimer>
#include "QMenu"
#include "DataManage/NebulaTask/NebulaMissionPlatform.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include "ITaskManagerActivator.h"

namespace Ui
{
class CDlgMissionPlatformLoadList;
}

class CDlgMissionPlatformLoadList : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgMissionPlatformLoadList(QWidget *parent = nullptr);
    ~CDlgMissionPlatformLoadList();
public:

    void Init();

    void CreateTreeWidget();

    void CreateTableWidget();

    std::string GetSelectedTreeLevel();

    void CreateTreeeMenu();

    void showEvent(QShowEvent *event);

public slots:

    void on_timeOut();

    void on_Menu_MarkAmmo_Edit();

    void on_treeWidget_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_cellDoubleClicked();

public:
    bool						                            b_UpdateAmmo = false;
private:
    Ui::CDlgMissionPlatformLoadList *ui;
    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::AmmunitionManage::IAmmunitionManageService     *m_pAmmunitionManageService = nullptr;
    CNeubulaMissionVec                                      m_NeubulaActionMissionVec;
    QTimer                                                  *m_timer = nullptr;
    QMenu													*m_TreeMenuRight;

};

#endif
