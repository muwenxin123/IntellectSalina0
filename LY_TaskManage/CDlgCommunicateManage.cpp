#include "CDlgCommunicateManage.h"
#include "ui_CDlgCommunicateManage.h"
#include "QCString.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include "BiIntervisibility.h"
#include <Intervisibility.h>
#include <QGeoCoordinate>
#include <QMenu>

#define HIGH_STEP				10

CDlgCommunicateManage::CDlgCommunicateManage(QWidget *parent) :
    LyDialog(tr("Communication Planning"), parent),
    ui(new Ui::CDlgCommunicateManage)
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
    localWeaponNameBindToId();
    UpdataWindowData();
    UpdateTaskCombobox();
    UpdateListData();
    UpdateTreeData();

    m_HeightPatch = "../data/dem/90";
    m_pCGetHeight = new CGetHeight(m_HeightPatch.toStdString().c_str());
}

CDlgCommunicateManage::~CDlgCommunicateManage()
{
    delete ui;
}

void CDlgCommunicateManage::CreateTree()
{
    ui->treeWidget->setHeaderHidden(true);
}

void CDlgCommunicateManage::UpdateTreeData()
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

        std::vector<std::string> PlatIDVec = m_pITaskManageService->readPlatFormMatch(stMission.GetID());
        std::vector<std::string> TempPlatIDVec;
        QMap<std::string, int> tempMap;
        for (auto data : PlatIDVec)
        {
            auto itr = tempMap.find(data);
            if (itr == tempMap.end())
            {
                tempMap[data] = 1;
                TempPlatIDVec.push_back(data);
            }
        }
        for (auto data : TempPlatIDVec)
        {
            QTreeWidgetItem *pItemPlatID = new QTreeWidgetItem(pItemTask);
            auto PlatName = SystemSysIDAndName.find(data);
            if (PlatName != SystemSysIDAndName.end())
            {
                pItemPlatID->setText(0, tr2("平台: ") + QString::fromStdString(PlatName->second));
                pItemPlatID->setData(0, Qt::UserRole, QString::fromStdString(data));
                QTreeWidgetItem *pItemRadioName = new QTreeWidgetItem(pItemPlatID);
                pItemRadioName->setText(0, tr2("电台: ") + tr2("海格电台"));
            }
            else
            {
                pItemPlatID->setText(0, tr2("平台: "));
            }
        }

    }

    ui->treeWidget->expandAll();
}

void CDlgCommunicateManage::CreateTableMenu()
{
    m_TreeMenu = new QMenu(this);
    QAction *TreeMenu_Add = m_TreeMenu->addAction(tr("Add"));
    QAction *TreeMenu_Remove = m_TreeMenu->addAction(tr("Delete"));
    connect(TreeMenu_Add, SIGNAL(triggered()), this, SLOT(on_Menu_Tree_Add()));
    connect(TreeMenu_Remove, SIGNAL(triggered()), this, SLOT(on_Menu_Tree_Remove()));
}

void CDlgCommunicateManage::ControlConnet()
{
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_tableWidget_customContextMenuRequested(const QPoint)));
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_treeWidget_customContextMenuRequested(const QPoint)));

    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));

    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(selectBox(int, int)));
}

void CDlgCommunicateManage::CreateListHeading()
{
    QStringList headerlabels;
    headerlabels << tr(" Waypoint Number ") << tr(" Signal Visibility ") << tr(" Visibility Range ") << tr2("备注");

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

void CDlgCommunicateManage::UpdateListData(std::string strTaskID, std::string strPlatformID)
{
    QPixmap pix_R(":/TaskManage/state_red.png");
    QPixmap pix_G(":/TaskManage/state_blue.png");
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    m_pShpVec.clear();
    isDraw = true;
    LY_TaskManage::Get().bIntervisibility = true;
    if (strPlatformID == "" || strTaskID == "")
    {
        return;
    }

    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    CNebulaMissionRelation_PlatRouteVec MissionRelation_PlatRouteVec = m_pITaskManageService->GetMissionIDByPlatRoute(strTaskID);
    int routeID;
    for (auto data : MissionRelation_PlatRouteVec)
    {
        if (strPlatformID == data.GetPlatformID())
        {
            routeID = data.GetRouteID();
            break;
        }
    }
    if (routeID == 0)
    {
        LyMessageBox::information(this, tr2("提示"), tr2("当前平台无航线信息！"));
        return;
    }
    CNeubulaMissionLinePointVec MissionLinePointVec = m_pITaskManageService->getMissionLinePointsFornLineID(routeID);
    if (MissionLinePointVec.size() == 0)
    {
        LyMessageBox::information(this, tr2("提示"), tr2("当前平台所匹配的航线无航点信息！"));
        return;
    }
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(MissionLinePointVec.size());
    for (int i = 0; i < MissionLinePointVec.size(); i++)
    {
        int pointID = i + 1;
        int nIsIntervisibility = 0;
        double distance = 0;
        if (i == 0)
        {
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(pointID)));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(nIsIntervisibility)));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(distance, 'f', 4)));
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString("")));
            pic = new QLabel();
            pic->setPixmap(pix_R);
            pic->setAlignment(Qt::AlignCenter);
            ui->tableWidget->setCellWidget(i, 1, pic);

        }
        else
        {
            double dT[6];
            dT[0] = MissionLinePointVec[i - 1].dLon;
            dT[1] = MissionLinePointVec[i - 1].dLat;
            dT[2] = MissionLinePointVec[i - 1].dAlt;
            dT[3] = MissionLinePointVec[i].dLon;
            dT[4] = MissionLinePointVec[i].dLat;
            dT[5] = MissionLinePointVec[i].dAlt;

            std::vector<SHPObject *> v;
            CIntervisibility iv(LY_TaskManage::Get().GetDataDir() + "dem/90/");
            v = iv.CalIntervisibility(dT[0], dT[1], dT[3], dT[4], dT[2], dT[5]);
            m_pShpVec.push_back(v);
            m_pShp[0] = v.front();
            m_pShp[1] = v.back();
            if (m_pShp[1]->nVertices >= 1)
            {
                nIsIntervisibility = 0;
            }
            else
            {
                nIsIntervisibility = 1;
            }
            QGeoCoordinate coord1(MissionLinePointVec[i - 1].dLat, MissionLinePointVec[i - 1].dLon);
            QGeoCoordinate coord2(MissionLinePointVec[i].dLat, MissionLinePointVec[i].dLon);
            distance = coord2.distanceTo(coord1);
            ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(pointID)));
            ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::number(nIsIntervisibility)));
            ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(distance, 'f', 4)));
            ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString("")));
            if (nIsIntervisibility == 1)
            {
                pic = new QLabel();
                pic->setPixmap(pix_G);
                pic->setAlignment(Qt::AlignCenter);
                ui->tableWidget->setCellWidget(i, 1, pic);
            }
            else
            {
                pic = new QLabel();
                pic->setPixmap(pix_R);
                pic->setAlignment(Qt::AlignCenter);
                ui->tableWidget->setCellWidget(i, 1, pic);
            }

        }
        ui->tableWidget->item(i, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(i, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    }

}

void CDlgCommunicateManage::UpdataWindowData()
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

void CDlgCommunicateManage::UpdateTaskCombobox()
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

int CDlgCommunicateManage::GetSelectedTreeLevel(QMap<int, QString> &TreeDataMap)
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

int CDlgCommunicateManage::GetSelectedTreeLevel()
{
    return m_nTreeLevel;
}

void CDlgCommunicateManage::GetTaskInfo(QString &strTaskID, QString &strPlatformID, QString &strLoadID)
{
    strTaskID = m_TaskID;
    strPlatformID = m_PlatformID;
    strLoadID = m_LoadID;
}

void CDlgCommunicateManage::ClearShapLineVec()
{

}

ShapLineVec CDlgCommunicateManage::GetShapLineVec()
{
    return m_ShapLineVec;
}

CoverAreaVec CDlgCommunicateManage::GetCoverAreaVec()
{
    QMutexLocker locker(&m_MutexCoverArea);
    return m_CoverAreaVec;
}

void CDlgCommunicateManage::localWeaponNameBindToId()
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

void CDlgCommunicateManage::showEvent(QShowEvent *event)
{
    UpdataWindowData();
    UpdateTaskCombobox();
    UpdateListData();
    UpdateTreeData();
}

void CDlgCommunicateManage::closeEvent(QCloseEvent *event)
{
    isDraw = false;
}

void CDlgCommunicateManage::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{

}

void CDlgCommunicateManage::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{

}

void CDlgCommunicateManage::on_comboBox_currentIndexChanged(int index)
{

}

void CDlgCommunicateManage::on_Menu_Tree_Add()
{

}

void CDlgCommunicateManage::on_Menu_Tree_Remove()
{

}

void CDlgCommunicateManage::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QMap<int, QString> TreeDataMap;
    m_nTreeLevel = GetSelectedTreeLevel(TreeDataMap);
    if (m_nTreeLevel > 1)
    {
        UpdateListData(m_TaskID.toStdString(), m_PlatformID.toStdString());
    }

}

void CDlgCommunicateManage::selectBox(int r, int c)
{

}
