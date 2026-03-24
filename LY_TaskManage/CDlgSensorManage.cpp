#include "CDlgSensorManage.h"
#include "ui_CDlgSensorManage.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include <QMenu>
#include <QTreeWidgetItem>

CDlgSensorManage::CDlgSensorManage(QWidget *parent) :
    LyDialog(tr("Sensor Planning"), parent),
    ui(new Ui::CDlgSensorManage)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1200, 800);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    m_pAmmunitionManageService = ITaskManagerActivator::Get().getPAmmunitionManageService();
    if (m_pAmmunitionManageService == nullptr)
    {
        return;
    }

    CreateTree();
    CreateListHeading();
    CreateTableMenu();
    ControlConnet();

    UpdataWindowData();
    UpdateTaskCombobox();

    UpdateTreeData();

    localWeaponNameBindToId();
}

CDlgSensorManage::~CDlgSensorManage()
{
    delete ui;
}

void CDlgSensorManage::CreateTree()
{
    ui->treeWidget->setHeaderHidden(true);
}

void CDlgSensorManage::UpdateTreeData()
{
    ui->treeWidget->clear();

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    if (m_AcitonRelationShowVec.size() == 0)
    {
        return;
    }

    auto itr = m_AcitonRelationShowVec.begin();
    for (auto TaskData : itr->ReTaskVec)
    {

        QTreeWidgetItem *pItemTask = new QTreeWidgetItem(ui->treeWidget);
        CNeubulaMission stMission = m_pITaskManageService->GetMission(TaskData.strTaskID);

        pItemTask->setText(0, tr("Task: ") + QString::fromStdString(stMission.GetName()));
        pItemTask->setData(0, Qt::UserRole, QString::fromStdString(TaskData.strTaskID));

        CRelationMissionResourceVec MissionResourceVec = stMission.GetPlatInfoMap();
        CNeubulaConntionAmmoVec TaskAmmoVec = m_pAmmunitionManageService->GetAmmunitionConnByTaskID(stMission.GetID());
        CNeubulaConntionAmmoVec TempAmmoVec;
        QMap<std::string, int> tempMap;
        for (auto data : TaskAmmoVec)
        {
            auto itr = tempMap.find(data.GetPlatID());
            if (itr == tempMap.end())
            {
                tempMap[data.GetPlatID()] = 1;
                TempAmmoVec.push_back(data);
            }
        }
        for (auto PlatData : MissionResourceVec)
        {
            for (auto AmmoData : TempAmmoVec)
            {
                if (PlatData.GetID() == AmmoData.GetRelationID())
                {

                    QTreeWidgetItem *pItemPlatID = new QTreeWidgetItem(pItemTask);
                    auto PlatName = SystemSysIDAndName.find(AmmoData.GetPlatID());
                    if (PlatName != SystemSysIDAndName.end())
                    {
                        pItemPlatID->setText(0, tr2("平台: ") + QString::fromStdString(PlatName->second));
                        pItemPlatID->setData(0, Qt::UserRole, QString::fromStdString(AmmoData.GetPlatID()));
                    }
                    else
                    {
                        pItemPlatID->setText(0, tr2("平台: "));
                    }

                    QTreeWidgetItem *pItemPodName = new QTreeWidgetItem(pItemPlatID);
                    pItemPodName->setText(0, tr2("主吊舱: ") + QString::fromStdString(PlatData.GetPrimaryPoc()));
                    if (QString::fromStdString(PlatData.GetSecondPod()) != "" && QString::fromStdString(PlatData.GetSecondPod()) != tr2("无"))
                    {

                        QTreeWidgetItem *pItemSecondPodName = new QTreeWidgetItem(pItemPlatID);
                        pItemSecondPodName->setText(0, tr2("次吊舱: ") + QString::fromStdString(PlatData.GetSecondPod()));
                    }

                    QTreeWidgetItem *pItemAmmoName = new QTreeWidgetItem(pItemPlatID);
                    pItemAmmoName->setText(0, tr2("主挂载DY: ") + QString::fromStdString(PlatData.GetPriMountType()));
                    if (QString::fromStdString(PlatData.GetSecMountType()) != "" && QString::fromStdString(PlatData.GetSecMountType()) != tr2("无"))
                    {

                        QTreeWidgetItem *pItemSecondAmmoName = new QTreeWidgetItem(pItemPlatID);
                        pItemSecondAmmoName->setText(0, tr2("次挂载DY: ") + QString::fromStdString(PlatData.GetSecMountType()));
                    }
                }
            }
        }

    }

    ui->treeWidget->expandAll();
}

void CDlgSensorManage::CreateTableMenu()
{
    m_TableMenu = new QMenu(this);
    QAction *TableMenu_Add = m_TableMenu->addAction(tr("Add"));
    QAction *TableMenu_Edit = m_TableMenu->addAction(tr("Edit"));
    QAction *TableMenu_Remove = m_TableMenu->addAction(tr("Delete"));
    connect(TableMenu_Add, SIGNAL(triggered()), this, SLOT(on_Menu_Table_Add()));
    connect(TableMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_Table_Edit()));
    connect(TableMenu_Remove, SIGNAL(triggered()), this, SLOT(on_Menu_Table_Remove()));

    m_TreeMenu = new QMenu(this);
    QAction *TreeMenu_Add = m_TreeMenu->addAction(tr("Add"));
    QAction *TreeMenu_Remove = m_TreeMenu->addAction(tr("Delete"));
    connect(TreeMenu_Add, SIGNAL(triggered()), this, SLOT(on_Menu_Tree_Add()));
    connect(TreeMenu_Remove, SIGNAL(triggered()), this, SLOT(on_Menu_Tree_Remove()));
}

void CDlgSensorManage::ControlConnet()
{
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_tableWidget_customContextMenuRequested(const QPoint)));

    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));
}

void CDlgSensorManage::CreateListHeading()
{
    QStringList headerlabels;
    headerlabels << tr2("No.") << tr2("执行顺序") << tr2("航点编号") << tr2("指令") << tr2("备注") << tr2("经度") << tr2("纬度");

    ui->tableWidget->setColumnCount(headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
    for (int i = 0; i < headerlabels.size(); i++)
    {
        ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }

    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void CDlgSensorManage::UpdataWindowData()
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

void CDlgSensorManage::UpdateTaskCombobox()
{
    ui->comboBox->clear();

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    if (m_AcitonRelationShowVec.size() == 0)
    {
        return;
    }

    auto itr = m_AcitonRelationShowVec.begin();
    int nNo = 0;
    for (auto data : itr->ReTaskVec)
    {
        CNeubulaMission stMission = m_pITaskManageService->GetMission(data.strTaskID);
        ui->comboBox->insertItem(nNo, QString::fromStdString(stMission.GetName()));
        ui->comboBox->setItemData(nNo, QString::fromStdString(stMission.GetID()));
        nNo++;
    }

}

int CDlgSensorManage::GetSelectedTreeLevel(QMap<int, QString> &TreeDataMap)
{
    QTreeWidgetItem *ItemChild = ui->treeWidget->currentItem();
    int nLevel = 0;
    while (ItemChild != nullptr)
    {
        QString strID = ItemChild->data(0, Qt::UserRole).toString();
        QTreeWidgetItem *ItemParent = ItemChild->parent();
        ItemChild = ItemParent;
        nLevel++;

        TreeDataMap.insert(nLevel, strID);
    }

    switch (nLevel)
    {
    case 1:
        m_TaskID = TreeDataMap.find(1).value();
        break;
    case 2:
        m_PlatformID = TreeDataMap.find(1).value();
        m_TaskID = TreeDataMap.find(2).value();
        break;
    case 3:
        m_PlatformID = TreeDataMap.find(2).value();
        m_TaskID = TreeDataMap.find(3).value();
        break;
    default:
        break;
    }

    return nLevel;
}

int CDlgSensorManage::GetSelectedTreeLevel()
{
    return m_nTreeLevel;
}

void CDlgSensorManage::GetTaskInfo(QString &strTaskID, QString &strPlatformID, QString &strLoadID)
{
    strTaskID = m_TaskID;
    strPlatformID = m_PlatformID;
    strLoadID = m_LoadID;
}

void CDlgSensorManage::localWeaponNameBindToId()
{
    if (ITaskManagerActivator::Get().getPWeaponManageService() == nullptr) {
        return;
    }
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

void CDlgSensorManage::showEvent(QShowEvent *event)
{
    UpdataWindowData();
    UpdateTaskCombobox();
    UpdataListData();
    UpdateTreeData();
}

void CDlgSensorManage::UpdataListData(QString TaskID, QString Platfrom )
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    m_CodeSet_Load_DB_Vec.clear();

    if (TaskID == "" || Platfrom == "")
    {
        return;
    }

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_CodeSet_Load_DB_Vec = m_pITaskManageService->GetCodeSet_Load(TaskID.toStdString(), Platfrom.toStdString());

    int nNo = 0;
    for (auto data : m_CodeSet_Load_DB_Vec)
    {
        if (0 == strcmp(data.GetPlatformID().c_str(), Platfrom.toStdString().c_str()))
        {
            ui->tableWidget->setRowCount(nNo + 1);

            QString strNo, strSuit, strPointNo, strCodeName, strTime, strRemark, strLon, strLat;
            strNo = QString::number(nNo + 1);
            strSuit = QString::number(data.GetSuite());
            strPointNo = QString::number(data.GetPointID());
            qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = (qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID)data.GetCodeID();
            strCodeName = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ePara);
            strTime = "0";
            strRemark = QString::fromStdString(data.GetRemark());
            strLon = QString::number(data.GetLon(), 'f', 8);
            strLat = QString::number(data.GetLat(), 'f', 8);

            QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
            QTableWidgetItem *item1 = new QTableWidgetItem(strSuit);
            QTableWidgetItem *item2 = new QTableWidgetItem(strPointNo);
            QTableWidgetItem *item3 = new QTableWidgetItem(strCodeName);

            QTableWidgetItem *item4 = new QTableWidgetItem(strRemark);
            QTableWidgetItem *item5 = new QTableWidgetItem(strLon);
            QTableWidgetItem *item6 = new QTableWidgetItem(strLat);

            ui->tableWidget->setItem(nNo, 0, item0);
            ui->tableWidget->setItem(nNo, 1, item1);
            ui->tableWidget->setItem(nNo, 2, item2);
            ui->tableWidget->setItem(nNo, 3, item3);
            ui->tableWidget->setItem(nNo, 4, item4);
            ui->tableWidget->setItem(nNo, 5, item5);
            ui->tableWidget->setItem(nNo, 6, item6);

            ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            nNo++;
        }
    }
}

void CDlgSensorManage::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    m_TreeMenu->exec(QCursor::pos());
}

void CDlgSensorManage::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{

    {
        m_TableMenu->exec(QCursor::pos());
    }
}

void CDlgSensorManage::on_comboBox_currentIndexChanged(int index)
{

}

void CDlgSensorManage::on_Menu_Tree_Add()
{

}

void CDlgSensorManage::on_Menu_Tree_Remove()
{

}

void CDlgSensorManage::on_Menu_Table_Add()
{
    if (LY_TaskManage::Get().GetDlgSensorManageEdit() == nullptr)
    {
        return;
    }

    if (LY_TaskManage::Get().GetDlgSensorManageEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgSensorManageEdit()->hide();
    }
    else
    {
        LY_TaskManage::Get().GetDlgSensorManageEdit()->SetCodeSetData(m_CodeSet_Load_DB_Vec);
        LY_TaskManage::Get().GetDlgSensorManageEdit()->SetPointID(1);
        LY_TaskManage::Get().GetDlgSensorManageEdit()->show();
        LY_TaskManage::Get().GetDlgSensorManageEdit()->raise();
    }
}

void CDlgSensorManage::on_Menu_Table_Edit()
{
    if (LY_TaskManage::Get().GetDlgSensorManageEdit() == nullptr)
    {
        return;
    }

    int nSel = ui->tableWidget->currentRow();
    if (nSel == -1)
    {
        LyMessageBox::information(nullptr, tr("tip"), tr("Please select edit entry"));
        return;
    }

    if (LY_TaskManage::Get().GetDlgSensorManageEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgSensorManageEdit()->hide();
    }
    else
    {
        LY_TaskManage::Get().GetDlgSensorManageEdit()->SetCodeSetData(m_CodeSet_Load_DB_Vec);
        QTableWidgetItem *pItem = ui->tableWidget->item(nSel, 2);
        LY_TaskManage::Get().GetDlgSensorManageEdit()->SetPointID(pItem->text().toInt());
        LY_TaskManage::Get().GetDlgSensorManageEdit()->show();
        LY_TaskManage::Get().GetDlgSensorManageEdit()->raise();
    }
}

void CDlgSensorManage::on_Menu_Table_Remove()
{
    int currentRow = ui->tableWidget->currentRow();
    m_nRoutePointID = ui->tableWidget->item(currentRow, 2)->text().toInt();
    m_ExecutionOrser = ui->tableWidget->item(currentRow, 1)->text().toInt();
    m_pITaskManageService->RmoveCodeSet_Load(m_TaskID.toStdString(), m_PlatformID.toStdString(), m_nRoutePointID, m_ExecutionOrser);
    UpdataListData(m_TaskID, m_PlatformID);
}

void CDlgSensorManage::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QMap<int, QString> TreeDataMap;
    m_nTreeLevel = GetSelectedTreeLevel(TreeDataMap);

    UpdataListData(m_TaskID, m_PlatformID);
    if (m_nTreeLevel > 1)
    {
        if (LY_TaskManage::Get().GetDlgSensorManageEdit() != nullptr)
        {
            LY_TaskManage::Get().GetDlgSensorManageEdit()->SetTaskID(m_TaskID.toStdString());
            LY_TaskManage::Get().GetDlgSensorManageEdit()->SetPlatformID(m_PlatformID.toStdString());

        }
    }

}
