#include "CDlgMissionTree.h"
#include "ui_CDlgMissionTree.h"
#include "ITaskManagerActivator.h"
#include "QCString.h"
#include "lY_TaskManage.h"
#include "LyMessageBox.h"
CDlgMissionTree::CDlgMissionTree(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgMissionTree)
{
    ui->setupUi(this);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_pIMarkManageService = ITaskManagerActivator::Get().getPIMarkManageService();
    if (m_pIMarkManageService == nullptr)
    {
        return;
    }

    m_pAmmunitionManageService = ITaskManagerActivator::Get().getPAmmunitionManageService();
    if (m_pAmmunitionManageService == nullptr)
    {
        return;
    }

    localWeaponNameBindToId();
    UpdateWindowData();
    CreateTree();
    UpdateTreeData();

    CreateMenu();

}

CDlgMissionTree::~CDlgMissionTree()
{
    delete ui;
}

void CDlgMissionTree::CreateTree()
{
    ui->treeWidget->setHeaderHidden(true);
}

void CDlgMissionTree::UpdateTreeData()
{
    m_nActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
    ui->treeWidget->clear();

    if (m_pITaskManageService == nullptr || m_pAmmunitionManageService == nullptr)
    {
        return;
    }

    int nTaskID = 1;
    if (m_AcitonRelationShowVec.size() == 0)
    {
        QTreeWidgetItem *pRootItem = new QTreeWidgetItem(ui->treeWidget);
        CNeubulaAction stAction = m_pITaskManageService->GetNebulaAction(m_nActionID);
        pRootItem->setText(0, tr("Action: ") + QString::fromStdString(stAction.GetName()));
        pRootItem->setData(0, Qt::UserRole, QString::number(stAction.GetID()));
        QString str = pRootItem->data(0, Qt::UserRole).toString();

        QTreeWidgetItem *pChildTanskItem = new QTreeWidgetItem(pRootItem);
        pChildTanskItem->setText(0, tr("Task Number: "));

        QTreeWidgetItem  *pChildTaskInfoItem = new QTreeWidgetItem(pChildTanskItem);
        pChildTaskInfoItem->setText(0, tr("Basic Information"));
        QTreeWidgetItem  *pChildTaskTypeItem = new QTreeWidgetItem(pChildTaskInfoItem);
        pChildTaskTypeItem->setText(0, tr("Task Type: "));
        QTreeWidgetItem  *pChildTaskNameItem = new QTreeWidgetItem(pChildTaskInfoItem);
        pChildTaskNameItem->setText(0, tr("Task Name "));
        QTreeWidgetItem  *pChildTaskArmyItem = new QTreeWidgetItem(pChildTaskInfoItem);
        pChildTaskArmyItem->setText(0, tr("Task Force: "));
        QTreeWidgetItem  *pChildTakeoffPosItem = new QTreeWidgetItem(pChildTaskInfoItem);
        pChildTakeoffPosItem->setText(0, tr("Departure Position: "));
        QTreeWidgetItem  *pChildTaskoffTimeItem = new QTreeWidgetItem(pChildTaskInfoItem);
        pChildTaskoffTimeItem->setText(0, tr("Departure Time: "));
        QTreeWidgetItem  *pChildLandingPosItem = new QTreeWidgetItem(pChildTaskInfoItem);
        pChildLandingPosItem->setText(0, tr("Landing Position: "));
        QTreeWidgetItem  *pChildLandingTimeItem = new QTreeWidgetItem(pChildTaskInfoItem);
        pChildLandingTimeItem->setText(0, tr("Landing Time: "));

        QTreeWidgetItem  *PChildTaskPlatItem = new QTreeWidgetItem(pChildTanskItem);
        PChildTaskPlatItem->setText(0, tr("Task Platform"));
        QTreeWidgetItem  *PChildTaskPlatNumItem = new QTreeWidgetItem(PChildTaskPlatItem);
        PChildTaskPlatNumItem->setText(0, tr("Task Platform: "));
        QTreeWidgetItem  *PChildPlatTypeItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
        PChildPlatTypeItem->setText(0, tr("Platform Type: "));
        QTreeWidgetItem  *PChildTaskMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
        PChildTaskMountItem->setText(0, tr("Task Mount: "));
        QTreeWidgetItem  *PChildMountDYNumItem = new QTreeWidgetItem(PChildTaskMountItem);
        PChildMountDYNumItem->setText(0, tr("Number of Ammunition Types: "));

        QTreeWidgetItem  *PChildTaskMarkItem = new QTreeWidgetItem(pChildTanskItem);
        PChildTaskMarkItem->setText(0, tr("Task Mark"));
        QTreeWidgetItem  *PChildTaskMarkBHItem = new QTreeWidgetItem(PChildTaskMarkItem);
        PChildTaskMarkBHItem->setText(0, tr("Task Mark: "));
        QTreeWidgetItem  *PChildTaskMarkInfoItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
        PChildTaskMarkInfoItem->setText(0, tr("Basic Information: "));
        QTreeWidgetItem  *PChildTaskMarkDamageItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
        PChildTaskMarkDamageItem->setText(0, tr("Damage Requirement: "));
        QTreeWidgetItem  *PChildTaskMarkDYNumItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
        PChildTaskMarkDYNumItem->setText(0, tr("Number of Ammunition Types Used: "));

        QTreeWidgetItem  *PChildTaskAllParameterItem = new QTreeWidgetItem(pChildTanskItem);
        PChildTaskAllParameterItem->setText(0, tr("Planning Parameters"));
        QTreeWidgetItem  *PChildTaskParameterItem = new QTreeWidgetItem(PChildTaskAllParameterItem);
        PChildTaskParameterItem->setText(0, tr("Task Parameters"));
        QTreeWidgetItem  *PChildControllerNameItem = new QTreeWidgetItem(PChildTaskParameterItem);
        PChildControllerNameItem->setText(0, tr("Name of Controller: "));
        QTreeWidgetItem  *PChildCommanderItem = new QTreeWidgetItem(PChildTaskParameterItem);
        PChildCommanderItem->setText(0, tr("Name of Commander: "));

        QTreeWidgetItem  *PChildPlatParameterItem = new QTreeWidgetItem(PChildTaskAllParameterItem);
        PChildPlatParameterItem->setText(0, tr("Platform Parameters: "));
        QTreeWidgetItem  *PChildPlatCodeIDItem = new QTreeWidgetItem(PChildPlatParameterItem);
        PChildPlatCodeIDItem->setText(0, tr("Platform Number: "));
        QTreeWidgetItem  *PChildMountDYCodeIDItem = new QTreeWidgetItem(PChildPlatParameterItem);
        PChildMountDYCodeIDItem->setText(0, tr("Serial Number of Mount Ammo:"));
        QTreeWidgetItem  *PChildRouteNumeItem = new QTreeWidgetItem(PChildPlatParameterItem);
        PChildRouteNumeItem->setText(0, tr("Route Name: "));

        QTreeWidgetItem  *PChildMarkParameterItem = new QTreeWidgetItem(PChildTaskAllParameterItem);
        PChildMarkParameterItem->setText(0, tr("Mark Parameters: "));
        QTreeWidgetItem  *PChildMarkCodeIDItem = new QTreeWidgetItem(PChildMarkParameterItem);
        PChildMarkCodeIDItem->setText(0, tr("Mark Encoding Sequence Number: "));
        QTreeWidgetItem  *PChildDYCodeIDItem = new QTreeWidgetItem(PChildMarkParameterItem);
        PChildDYCodeIDItem->setText(0, tr("Use the Ammo Serial Number: "));
        QTreeWidgetItem  *PChildTartgetRouteNameItem = new QTreeWidgetItem(PChildMarkParameterItem);
        PChildTartgetRouteNameItem->setText(0, tr("Route Name: "));
    }
    for (auto ActionData : m_AcitonRelationShowVec)
    {
        QTreeWidgetItem *pRootItem = new QTreeWidgetItem(ui->treeWidget);
        CNeubulaAction stAction = m_pITaskManageService->GetNebulaAction(ActionData.nActionID);
        pRootItem->setText(0, tr("Action: ") + QString::fromStdString(stAction.GetName()));
        pRootItem->setData(0, Qt::UserRole, QString::number(stAction.GetID()));

        QString str = pRootItem->data(0, Qt::UserRole).toString();

        for (auto TaskData : ActionData.ReTaskVec)
        {

            QTreeWidgetItem *pChildItemTask = new QTreeWidgetItem(pRootItem);
            CNeubulaMission stMission = m_pITaskManageService->GetMission(TaskData.strTaskID);
            CNeubulaAmmoConntionMarkVec MarkIDAmmoVec = m_pAmmunitionManageService->GetTaskIDByInfo(stMission.GetID());
            QString MissionType;
            switch (stMission.GetType())
            {
            case 0:
                MissionType = tr("Unknown");
                break;
            case 1:
                MissionType = tr("Strike");
                break;
            case 2:
                MissionType = tr("Investigetion");
                break;
            case 3:
                MissionType = tr("Protection");
                break;
            case 4:
                MissionType = tr("Surveillance");
                break;
            case 5:
                MissionType = tr("Rescue");
                break;
            case 6:
                MissionType = tr("Transport");
                break;
            default:
                break;
            }
            PlatIDVec = m_pITaskManageService->readPlatFormMatch(stMission.GetID());
            pChildItemTask->setText(0, (MissionType + tr("Task: ") + QString::fromStdString(stMission.GetID())));
            pChildItemTask->setData(0, Qt::UserRole, QString::fromStdString(TaskData.strTaskID));
            QTreeWidgetItem  *pChildTaskInfoItem = new QTreeWidgetItem(pChildItemTask);
            pChildTaskInfoItem->setText(0, tr("Basic Information"));
            QTreeWidgetItem  *pChildTaskTypeItem = new QTreeWidgetItem(pChildTaskInfoItem);
            pChildTaskTypeItem->setText(0, tr("Task Type: ") + MissionType);
            QTreeWidgetItem  *pChildTaskNameItem = new QTreeWidgetItem(pChildTaskInfoItem);
            pChildTaskNameItem->setText(0, tr("Task Name: ") + tr(stMission.GetName().c_str()));
            QTreeWidgetItem  *pChildTaskArmyItem = new QTreeWidgetItem(pChildTaskInfoItem);
            pChildTaskArmyItem->setText(0, tr("Task Force: ") + tr(stMission.GetBelong().c_str()));
            QTreeWidgetItem  *pChildTakeoffPosItem = new QTreeWidgetItem(pChildTaskInfoItem);
            pChildTakeoffPosItem->setText(0, tr("Departure Position: ") + tr("Longitude of Origin--") + QString::number(stMission.GetStartLon()) + tr(",")
                                          + tr("Latitude of Origin--") + QString::number(stMission.GetStartLat()) + tr(",") + tr("Altitude of Origin--") + QString::number(stMission.GetStartAlt()));
            QTreeWidgetItem  *pChildTaskoffTimeItem = new QTreeWidgetItem(pChildTaskInfoItem);
            pChildTaskoffTimeItem->setText(0, tr("Start Time: ") + tr(stMission.GetStartTime().c_str()));
            QTreeWidgetItem  *pChildLandingPosItem = new QTreeWidgetItem(pChildTaskInfoItem);
            pChildLandingPosItem->setText(0, tr("Landing Position: "));
            QTreeWidgetItem  *pChildLandingTimeItem = new QTreeWidgetItem(pChildTaskInfoItem);
            pChildLandingTimeItem->setText(0, tr("Finish Time: ") + tr(stMission.GetFinishTime().c_str()));

            QTreeWidgetItem  *PChildTaskPlatItem = new QTreeWidgetItem(pChildItemTask);
            PChildTaskPlatItem->setText(0, tr("Task Platform"));
            if (stMission.GetPlatInfoMap().size() == 0)
            {
                QTreeWidgetItem  *PChildTaskPlatNumItem = new QTreeWidgetItem(PChildTaskPlatItem);

                PChildTaskPlatNumItem->setText(0, tr2(": "));
                QTreeWidgetItem  *PChildPlatTypeItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                PChildPlatTypeItem->setText(0, tr("Platform Type: "));
                QTreeWidgetItem  *PChildTaskMainMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                PChildTaskMainMountItem->setText(0, tr("Task Main Mount: "));
                QTreeWidgetItem  *PChildMainMountDYNumItem = new QTreeWidgetItem(PChildTaskMainMountItem);
                PChildMainMountDYNumItem->setText(0, tr("Number of Ammunition Types: "));
                QTreeWidgetItem  *PChildTaskOrderMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                PChildTaskOrderMountItem->setText(0, tr("Task Secondary Mount: "));
                QTreeWidgetItem  *PChildOrderMountDYNumItem = new QTreeWidgetItem(PChildTaskOrderMountItem);
                PChildOrderMountDYNumItem->setText(0, tr("Number of Ammunition Types: "));
            }
            for (auto data : stMission.GetPlatInfoMap())
            {
                QTreeWidgetItem  *PChildTaskPlatNumItem = new QTreeWidgetItem(PChildTaskPlatItem);

                std::string strPlatID;
                auto itr = PlatIDVec.begin();
                for (; itr != PlatIDVec.end(); itr++)
                {
                    if (tr(data.GetResourceName().c_str()) == tr(m_pITaskManageService->readPlatFormMatchZBXH(*itr, stMission.GetID()).c_str()))
                    {
                        strPlatID = *itr;
                        PlatIDVec.erase(itr);
                        break;
                    }
                }
                auto PlatName = SystemSysIDAndName.find(strPlatID);
                if (PlatName != SystemSysIDAndName.end())
                {
                    PChildTaskPlatNumItem->setText(0, tr(data.GetResourceName().c_str()) + tr("/") + tr(PlatName->second.c_str()));
                }
                else
                {
                    PChildTaskPlatNumItem->setText(0, tr(data.GetResourceName().c_str()) + tr("/"));
                }

                QTreeWidgetItem  *PChildPlatTypeItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                PChildPlatTypeItem->setText(0, tr("Platform Type: ") + tr(data.GetResourceName().c_str()));
                if (tr(data.GetPriMountType().c_str()) != tr(""))
                {
                    QTreeWidgetItem  *PChildTaskMainMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                    PChildTaskMainMountItem->setText(0, tr("Task Main Mount: ") + tr(data.GetPriMountType().c_str()));
                    QTreeWidgetItem  *PChildMountDYNumItem = new QTreeWidgetItem(PChildTaskMainMountItem);
                    PChildMountDYNumItem->setText(0, tr("Number of Ammunition Types: ") + QString::number(data.GetPriMountNum()));
                    if (tr(data.GetSecMountType().c_str()) != tr(""))
                    {
                        QTreeWidgetItem  *PChildTaskOrderMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                        PChildTaskOrderMountItem->setText(0, tr("Task Secondary Mount: ") + tr(data.GetSecMountType().c_str()));
                        QTreeWidgetItem  *PChildOrderMountDYNumItem = new QTreeWidgetItem(PChildTaskOrderMountItem);
                        PChildOrderMountDYNumItem->setText(0, tr("Number of Ammunition Types: ") + QString::number(data.GetSecMountNum()));
                    }
                }
                else
                {
                    QTreeWidgetItem  *PChildTaskMainMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                    PChildTaskMainMountItem->setText(0, tr("Task Main Mount: "));
                    QTreeWidgetItem  *PChildMainMountDYNumItem = new QTreeWidgetItem(PChildTaskMainMountItem);
                    PChildMainMountDYNumItem->setText(0, tr("Number of Ammunition Types:"));
                    QTreeWidgetItem  *PChildTaskOrderMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                    PChildTaskOrderMountItem->setText(0, tr("Task Secondary Mount: "));
                    QTreeWidgetItem  *PChildOrderMountDYNumItem = new QTreeWidgetItem(PChildTaskOrderMountItem);
                    PChildOrderMountDYNumItem->setText(0, tr("Number of Ammunition Types: "));
                }
            }

            QTreeWidgetItem  *PChildTaskMarkItem = new QTreeWidgetItem(pChildItemTask);
            PChildTaskMarkItem->setText(0, tr("Task Mark"));
            if (stMission.GetMarkIDList().size() == 0)
            {
                QTreeWidgetItem  *PChildTaskMarkBHItem = new QTreeWidgetItem(PChildTaskMarkItem);
                PChildTaskMarkBHItem->setText(0, tr("Mark Number: "));
                QTreeWidgetItem  *PChildTaskMarkInfoItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                PChildTaskMarkInfoItem->setText(0, tr("Basic Information: "));
                QTreeWidgetItem  *PChildTaskMarkDamageItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                PChildTaskMarkDamageItem->setText(0, tr("Damage Requirement: "));
                QTreeWidgetItem  *PChildTaskMarkAmmoModeItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                PChildTaskMarkAmmoModeItem->setText(0, tr("Type of Ammunition used: "));
                QTreeWidgetItem  *PChildTaskMarkAmmoNumItem = new QTreeWidgetItem(PChildTaskMarkAmmoModeItem);
                PChildTaskMarkAmmoNumItem->setText(0, tr("Number of Ammunition used: "));
            }
            else
            {
                for (auto data : stMission.GetMarkIDList())
                {
                    QTreeWidgetItem  *PChildTaskMarkBHItem = new QTreeWidgetItem(PChildTaskMarkItem);
                    PChildTaskMarkBHItem->setText(0, tr("Mark Number: ") + tr(data.c_str()));
                    QTreeWidgetItem  *PChildTaskMarkInfoItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                    PChildTaskMarkInfoItem->setText(0, tr("Basic Information"));

                    SystemMarkInfo tmpTar = ITaskManagerActivator::Get().getPIMarkManageService()->GetMark(data);
                    QTreeWidgetItem  *PChildMarkIDItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkIDItem->setText(0, tr("Mark ID: ") + tr(tmpTar.GetID().c_str()));
                    QTreeWidgetItem  *PChildMarkNameItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkNameItem->setText(0, tr("Mark Name: ") + tr(tmpTar.GetName().c_str()));
                    QTreeWidgetItem  *PChildMarkTyprItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkTyprItem->setText(0, tr("Mark Type: ") + tr(ESystemMarkTypeToString(tmpTar.GetType())));
                    QTreeWidgetItem  *PChildMarkAttributeItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkAttributeItem->setText(0, tr("Mark Property: ") + tr(ESystemMarkAttributeToString(tmpTar.GetAttribute())));
                    QTreeWidgetItem  *PChildMarkMaterialItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkMaterialItem->setText(0, tr("Mark Material: ") + tr(ESystemMarkMaterialToString(tmpTar.GetMaterial())));
                    QTreeWidgetItem  *PChildMarkSizeItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkSizeItem->setText(0, tr("Mark Size: ") + QString::number(tmpTar.GetSizeX()) + tr("X") + QString::number(tmpTar.GetSizeY()) + tr("X") + QString::number(tmpTar.GetSizeZ()));
                    QTreeWidgetItem  *PChildMarkPosItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkPosItem->setText(0, tr("Mark Position: ") + tr("Longitude-") + QString::number(tmpTar.GetLon()) + tr("Latitude-") + QString::number(tmpTar.GetLat()) + tr("¸ß¶È-") + QString::number(tmpTar.GetHeight()));
                    QTreeWidgetItem  *PChildMarkSpeedItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkSpeedItem->setText(0, tr("Mark Speed: ") + QString::number(tmpTar.GetSpeed()));
                    QTreeWidgetItem  *PChildMarkPitchItem = new QTreeWidgetItem(PChildTaskMarkInfoItem);
                    PChildMarkPitchItem->setText(0, tr("Mark Heading: ") + QString::number(tmpTar.GetPitch()));
                    QTreeWidgetItem  *PChildTaskMarkDamageItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                    int targetDamage = stMission.GetMarkDamage(data);
                    QString strMarkDamage = LY_TaskManage::Get().GetDlgMissionList()->GetHurtDegreeString(stMission.GetHurtDegreeENum(targetDamage));
                    PChildTaskMarkDamageItem->setText(0, tr("Damage Requirement: ") + strMarkDamage);
                    std::vector<std::string> strMarkAmmoVec = m_pAmmunitionManageService->GetMarkIDByAmmoMode(MarkIDAmmoVec, data);
                    QMap<std::string, int> mAmmoMode;
                    for (auto AmmoID : strMarkAmmoVec)
                    {
                        CNeubulaAmmunitionInfo AmmoInfo = m_pAmmunitionManageService->GetAmmoByID(AmmoID);
                        auto it = mAmmoMode.find(AmmoInfo.GetAmmoMode());
                        if (it != mAmmoMode.end())
                        {
                            mAmmoMode[AmmoInfo.GetAmmoMode()] += 1;
                        }
                        else
                        {
                            mAmmoMode.insert(AmmoInfo.GetAmmoMode(), 1);
                        }
                    }
                    if (mAmmoMode.empty())
                    {
                        QTreeWidgetItem  *PChildTaskMarkAmmoModeItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                        PChildTaskMarkAmmoModeItem->setText(0, tr("Type of Ammunition used: "));
                        QTreeWidgetItem  *PChildTaskMarkAmmoNumItem = new QTreeWidgetItem(PChildTaskMarkAmmoModeItem);
                        PChildTaskMarkAmmoNumItem->setText(0, tr("Number of Ammunition used: "));
                    }
                    else
                    {
                        for (auto it = mAmmoMode.begin(); it != mAmmoMode.end(); ++it)
                        {
                            QTreeWidgetItem  *PChildTaskMarkAmmoModeItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                            PChildTaskMarkAmmoModeItem->setText(0, tr("Type of Ammunition used: ") + tr(it.key().c_str()));
                            QTreeWidgetItem  *PChildTaskMarkAmmoNumItem = new QTreeWidgetItem(PChildTaskMarkAmmoModeItem);
                            PChildTaskMarkAmmoNumItem->setText(0, tr("Number of Ammunition used: ") + QString::number(it.value()));
                        }
                    }
                }
            }
            QTreeWidgetItem  *PChildTaskAllParameterItem = new QTreeWidgetItem(pChildItemTask);
            PChildTaskAllParameterItem->setText(0, tr("Planning Parameters"));

            QTreeWidgetItem  *PChildPlatParameterItem = new QTreeWidgetItem(PChildTaskAllParameterItem);
            PChildPlatParameterItem->setText(0, tr("Platform Parameters"));
            PlatIDVec = m_pITaskManageService->readPlatFormMatch(stMission.GetID());
            std::string strPlatID;
            for (auto data : stMission.GetPlatInfoMap())
            {
                QTreeWidgetItem  *PChildTaskPlatNumItem = new QTreeWidgetItem(PChildPlatParameterItem);
                auto itr = PlatIDVec.begin();
                for (; itr != PlatIDVec.end(); itr++)
                {
                    if (tr(data.GetResourceName().c_str()) == tr(m_pITaskManageService->readPlatFormMatchZBXH(*itr, stMission.GetID()).c_str()))
                    {
                        strPlatID = *itr;
                        PlatIDVec.erase(itr);
                        break;
                    }
                }
                auto PlatName = SystemSysIDAndName.find(strPlatID);
                if (PlatName != SystemSysIDAndName.end())
                {
                    PChildTaskPlatNumItem->setText(0, tr(data.GetResourceName().c_str()) + tr("/") + tr(PlatName->second.c_str()));
                }
                else
                {
                    PChildTaskPlatNumItem->setText(0, tr(data.GetResourceName().c_str()) + tr("/"));
                }
                QTreeWidgetItem  *PChildControllerNameItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                if (strPlatID != "")
                {
                    PChildControllerNameItem->setText(0, tr("Name of Controller: ") + tr(m_pITaskManageService->readPlatFormMatchKZRY(strPlatID, stMission.GetID()).c_str()));
                }
                else
                {
                    PChildControllerNameItem->setText(0, tr("Name of Controller: "));
                }
                QTreeWidgetItem  *PChildCommanderItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                PChildCommanderItem->setText(0, tr("Name of Commander: "));
                QTreeWidgetItem  *PChildPlatCodeIDItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                PChildPlatCodeIDItem->setText(0, tr("Platform Number: ") + tr(strPlatID.c_str()));
                int num;
                if (data.GetPriMountNum() != 0)
                {
                    for (int i = 0; i < data.GetPriMountNum(); i++)
                    {
                        QTreeWidgetItem  *PChildTaskMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                        PChildTaskMountItem->setText(0, tr("Serial Number of Mount Ammo: ") + QString::number(i + 1));
                        num = i + 1;
                    }
                    for (int i = 0; i < data.GetSecMountNum(); i++)
                    {
                        QTreeWidgetItem  *PChildTaskMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                        PChildTaskMountItem->setText(0, tr("Serial Number of Mount Ammo: "));
                    }
                }
                else
                {
                    QTreeWidgetItem  *PChildTaskMountItem = new QTreeWidgetItem(PChildTaskPlatNumItem);
                    PChildTaskMountItem->setText(0, tr("Serial Number of Mount Ammo: "));
                }
                for (auto PlatFormdata : TaskData.PlatFormVec)
                {

                    QTreeWidgetItem *pChildItemLine = new QTreeWidgetItem(PChildTaskPlatNumItem);
                    pChildItemLine->setText(0, tr("Route: ") + QString::fromStdString(PlatFormdata.strLineID));
                    pChildItemLine->setData(0, Qt::UserRole, QString::fromStdString(PlatFormdata.strLineID));

                }
            }

            QTreeWidgetItem  *PChildMarkParameterItem = new QTreeWidgetItem(PChildTaskAllParameterItem);
            PChildMarkParameterItem->setText(0, tr("Mark Parameters"));

            if (stMission.GetMarkIDList().size() == 0)
            {
                QTreeWidgetItem  *PChildTaskMarkBHItem = new QTreeWidgetItem(PChildMarkParameterItem);
                PChildTaskMarkBHItem->setText(0, tr(":"));
                QTreeWidgetItem  *PChildTaskMarkInfoItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                PChildTaskMarkInfoItem->setText(0, tr("Platform Number:"));
                QTreeWidgetItem  *PChildTaskMarkDamageItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                PChildTaskMarkDamageItem->setText(0, tr("Serial Number of Mount Ammo: "));
                QTreeWidgetItem  *PChildTaskMarkDYNumItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                PChildTaskMarkDYNumItem->setText(0, tr("Route Name: "));
            }
            else
            {
                for (auto data : stMission.GetMarkIDList())
                {
                    QTreeWidgetItem  *PChildTaskMarkBHItem = new QTreeWidgetItem(PChildMarkParameterItem);
                    PChildTaskMarkBHItem->setText(0, tr("Mark Number--") + tr(data.c_str()));
                    QTreeWidgetItem  *PChildTaskMarkInfoItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                    PChildTaskMarkInfoItem->setText(0, tr("Platform Number:"));
                    std::vector<std::string> strMarkAmmoVec = m_pAmmunitionManageService->GetMarkIDByAmmoMode(MarkIDAmmoVec, data);
                    if (strMarkAmmoVec.empty())
                    {
                        QTreeWidgetItem  *PChildTaskMarkDamageItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                        PChildTaskMarkDamageItem->setText(0, tr("Serial Number of Mount Ammo: "));
                    }
                    for (auto Ammo : strMarkAmmoVec)
                    {
                        QTreeWidgetItem  *PChildTaskMarkDamageItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                        PChildTaskMarkDamageItem->setText(0, tr("Serial Number of Mount Ammo: ") + tr(Ammo.c_str()));
                    }
                    QTreeWidgetItem  *PChildTaskMarkDYNumItem = new QTreeWidgetItem(PChildTaskMarkBHItem);
                    PChildTaskMarkDYNumItem->setText(0, tr("Route Name:"));
                }
            }
        }
        ui->treeWidget->setCurrentItem(pRootItem);
        pRootItem->setExpanded(true);
    }

}

void CDlgMissionTree::UpdateWindowData()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    m_RelationActionTaskVec.clear();
    m_RelationActionTaskVec = m_pITaskManageService->GetAllRelationTask();

    m_AcitonRelationShowVec.clear();
    m_AcitonRelationShowVec = m_pITaskManageService->GetActivatingShowData();
}

int CDlgMissionTree::GetSelectedTreeLevel()
{
    QTreeWidgetItem *ItemChild = ui->treeWidget->currentItem();
    int nLevel = 0;
    while (ItemChild != nullptr)
    {
        QString str = ItemChild->text(0);
        QTreeWidgetItem *ItemParent = ItemChild->parent();
        ItemChild = ItemParent;
        nLevel++;
    }

    return nLevel;
}

void CDlgMissionTree::CreateMenu()
{
    m_TreeMenu_Action = new QMenu(this);
    QAction *ActionMenu_Edit = m_TreeMenu_Action->addAction(tr("Edit Action"));
    QAction *ActionMenu_Remove = m_TreeMenu_Action->addAction(tr("Delete Action"));
    connect(ActionMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_Action_Edit()));
    connect(ActionMenu_Remove, SIGNAL(triggered()), this, SLOT(on_Menu_Action_Remove()));

    m_TreeMenu_Task = new QMenu(this);
    QAction *TaskMenu_Add = m_TreeMenu_Task->addAction(tr("Add Task"));
    QAction *TaskMenu_Edit = m_TreeMenu_Task->addAction(tr("Edit Task"));
    QAction *TaskMenu_Remove = m_TreeMenu_Task->addAction(tr("Delete Task"));
    connect(TaskMenu_Add, SIGNAL(triggered()), this, SLOT(on_Menu_Task_Add()));
    connect(TaskMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_Task_Edit()));
    connect(TaskMenu_Remove, SIGNAL(triggered()), this, SLOT(on_Menu_Task_Remove()));

    m_TreeMenu_PlatForm = new QMenu(this);
    QAction *PlatFormMenu_Add = m_TreeMenu_PlatForm->addAction(tr("Add Platform"));
    QAction *PlatFormMenu_Edit = m_TreeMenu_PlatForm->addAction(tr("Edit Platform"));
    QAction *PlatFormMenu_Remove = m_TreeMenu_PlatForm->addAction(tr("Delete Platform"));
    connect(PlatFormMenu_Add, SIGNAL(triggered()), this, SLOT(on_Menu_PlatForm_Add()));
    connect(PlatFormMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_PlatForm_Edit()));
    connect(PlatFormMenu_Remove, SIGNAL(triggered()), this, SLOT(on_Menu_PlatForm_Remove()));

    m_TreeMenu_Load = new QMenu(this);
    QAction *LoadMenu_Add = m_TreeMenu_Load->addAction(tr("Add Mount"));
    QAction *LoadMenu_Edit = m_TreeMenu_Load->addAction(tr("Edit Mount"));
    QAction *LoadMenu_Remove = m_TreeMenu_Load->addAction(tr("Delete Mount"));
    connect(LoadMenu_Add, SIGNAL(triggered()), this, SLOT(on_Menu_Load_Add()));
    connect(LoadMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_Load_Edit()));
    connect(LoadMenu_Remove, SIGNAL(triggered()), this, SLOT(on_Menu_Load_Remove()));
}

void CDlgMissionTree::Update(qnzkna::TaskManage::INebulaTaskManageService *sub)
{
    UpdateWindowData();
    CreateTree();
    UpdateTreeData();
    CreateMenu();
}

void CDlgMissionTree::localWeaponNameBindToId()
{
    ITaskManagerActivator::Get().getPWeaponManageService()->GetWeaponSystemMap(weaponSystemMap);
    for (auto sysItr : weaponSystemMap)
    {
        auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
        for (auto weapon : *weaponComPonentMap)
        {
            SystemSysIDAndName[weapon.second.getComponentID()] = weapon.second.getComponentName();
        }
    }

}

void CDlgMissionTree::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    int nLevel = GetSelectedTreeLevel();
    switch (nLevel)
    {
    case 1:
        m_TreeMenu_Action->exec(QCursor::pos());
        break;
    case 2:
        m_TreeMenu_Task->exec(QCursor::pos());
        break;
    case 3:
        m_TreeMenu_PlatForm->exec(QCursor::pos());
        break;
    case 4:
        m_TreeMenu_Load->exec(QCursor::pos());
        break;
    case 5:
        m_TreeMenu_Load->exec(QCursor::pos());
        break;
    default:
        break;
    }
}

void CDlgMissionTree::on_Menu_Action_Edit()
{

}

void CDlgMissionTree::on_Menu_Action_Remove()
{

}

void CDlgMissionTree::on_Menu_Task_Add()
{
    LY_TaskManage::Get().GetDlgMissionEdit()->SetDataType(1);
    LY_TaskManage::Get().GetDlgMissionEdit()->InitWindowCtrl(0);
    if (LY_TaskManage::Get().GetDlgMissionEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgMissionEdit()->hide();
    }
    LY_TaskManage::Get().GetDlgMissionEdit()->show();
}

void CDlgMissionTree::on_Menu_Task_Edit()
{

}

void CDlgMissionTree::on_Menu_Task_Remove()
{

}

void CDlgMissionTree::on_Menu_PlatForm_Add()
{

}

void CDlgMissionTree::on_Menu_PlatForm_Edit()
{

}

void CDlgMissionTree::on_Menu_PlatForm_Remove()
{

}

void CDlgMissionTree::on_Menu_Load_Add()
{

}

void CDlgMissionTree::on_Menu_Load_Edit()
{

}

void CDlgMissionTree::on_Menu_Load_Remove()
{

}

void CDlgMissionTree::QTreeWidgetItemClicked()
{
    CNeubulaActionVec ActionVec = ITaskManagerActivator::Get().getPTaskManageService()->GetAllNebulaAction();
    for (auto data : ActionVec)
    {
        if (data.GetActivateStates() == true)
        {
            if (data.GetID() != m_nActionID)
            {
                if (data.GetID() != currActionID)
                {
                    m_bPressEvect = true;
                }
                if (m_bPressEvect)
                {
                    QString strTmp = tr("Switch to the current active action?");
                    if (QMessageBox::StandardButton::Yes != LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No))
                    {
                        m_bPressEvect = false;
                        currActionID = data.GetID();
                    }
                    else
                    {
                        Update(ITaskManagerActivator::Get().getPTaskManageService());
                        m_nActionID = data.GetID();
                        ITaskManagerActivator::Get().getPTaskManageService()->SetActivatingActionID(m_nActionID);
                    }
                }
            }
            break;
        }
    }
}

