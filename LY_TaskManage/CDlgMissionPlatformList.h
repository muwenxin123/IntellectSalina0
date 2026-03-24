#ifndef CDLGMISSIONPLATFORMLIST_H
#define CDLGMISSIONPLATFORMLIST_H

#include <QWidget>
#include "QMenu"
#include "DataManage/NebulaTask/NebulaMissionPlatform.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include "ITaskManagerActivator.h"

namespace Ui
{
class CDlgMissionPlatformList;
}

class CDlgMissionPlatformList : public QWidget
{
    Q_OBJECT

public:
    explicit CDlgMissionPlatformList(QWidget *parent = nullptr);
    ~CDlgMissionPlatformList();
public:

    void Init();

    void CreateTableWidget();

    void CreateTableMenu();

    std::string GetPlatPriAmmoID(const std::string TaskID, const std::string PlatID);

    std::string GetPlatSecondAmmoID(const std::string TaskID, const std::string PlatID);
public slots :

    void on_timeOut();

    void on_Menu_MarkAmmo_Edit();

    void on_tableWidget_customContextMenuRequested(const QPoint &pos);

    void on_tableWidget_cellDoubleClicked();

public:
    bool						                            b_UpdatePlatAmmo = false;
private:
    Ui::CDlgMissionPlatformList *ui;
    qnzkna::TaskManage::INebulaTaskManageService			*m_pITaskManageService = nullptr;
    qnzkna::AmmunitionManage::IAmmunitionManageService     *m_pAmmunitionManageService = nullptr;
    qnzkna::WeaponManage::IWeaponManageService				*m_pIWeaponManageService = nullptr;
    CNeubulaMissionVec                                      m_NeubulaActionMissionVec;
    std::map<std::string, std::string>                      SystemSysIDAndName;
    QTimer                                                  *m_timer = nullptr;
    QMenu													*m_MenuRight;

};

#endif
