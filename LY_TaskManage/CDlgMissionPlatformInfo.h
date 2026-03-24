#ifndef CDLGMISSIONPLATFORMINFO_H
#define CDLGMISSIONPLATFORMINFO_H

#include <QWidget>
#include"LyDialog.h"
#include "DataManage/NebulaTask/NebulaMission.h"
#include "DataManage/INebulaTaskManageService.h"
#include "DataManage/IMarkManageService.h"
#include "DataManage/Common/TypeDefine_Node.h"
#include "SystemConfig/IConfigProjectControlService.h"
#include "DataManage/IWeaponManageService.h"
#include "DataManage/ICommandUnitManageService.h"
namespace Ui
{
class CDlgMissionPlatformInfo;
}
typedef struct PlatfromInfo
{
    QString platType;
    QString PodTypeMain;
    QString PodTypeOrder;
    QString MountWQMain;
    int MountWQMainNum;
    QString MountWQOrder;
    int MountWQOrderNum;
} PlatfromInfo;

class CDlgMissionPlatformInfo : public LyDialog
{
    Q_OBJECT

public:
    explicit CDlgMissionPlatformInfo(QWidget *parent = nullptr);
    ~CDlgMissionPlatformInfo();

    void Init(QString platType, int currentRow);

    void showEvent(QShowEvent *event);

private:
    qnzkna::TaskManage::INebulaTaskManageService							*m_pITaskManageService = nullptr;
private slots:
    void on_pbtn_Ok_clicked();

    void on_pbtn_Cancel_clicked();

    void on_currentIndexChangedMountMain(const QString &text);
    void on_currentIndexChangedMountOrder(const QString &text);
    void on_currentIndexChangedWQMain(const QString &text);
    void on_currentIndexChangedWQOrder(const QString &text);

private:
    Ui::CDlgMissionPlatformInfo *ui;
    PlatfromInfo platfrominfo;
    int currentRow;
    CRelationActionResourceVec SensorResourceVec;
    CRelationActionResourceVec AmmoResourceVec;
    CRelationMissionAmmoVec                                        m_CRelationMissionAmmoVec;
};

#endif
