#include "CDlgFireManage.h"
#include "ui_CDlgFireManage.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include <QMenu>

CDlgFireManage::CDlgFireManage(QWidget *parent) :
    LyDialog(tr("Fire Planning"), parent),
    ui(new Ui::CDlgFireManage)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1200, 800);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    CreateTree();
    CreateListHeading();
    CreateTableMenu();
    ControlConnet();

    UpdataWindowData();
    UpdateTaskCombobox();
    UpdateListData();
    UpdateTreeData();
}

CDlgFireManage::~CDlgFireManage()
{
    delete ui;
}

void CDlgFireManage::CreateTree()
{
    ui->treeWidget->setHeaderHidden(true);
}

void CDlgFireManage::UpdateTreeData()
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

        for (auto PlatFormdata : TaskData.PlatFormVec)
        {
            if (PlatFormdata.strPlatformID == "")
            {
                continue;
            }

            QTreeWidgetItem *pChildItemPlatform = new QTreeWidgetItem(pItemTask);
            CNebulaMissionPlatform stPlatform = m_pITaskManageService->GetMissionPlatform(PlatFormdata.strPlatformID);

			pChildItemPlatform->setText(0, tr("Platform: ") + QString::fromLocal8Bit(stPlatform.strMode));
			pChildItemPlatform->setData(0, Qt::UserRole, QString::fromLocal8Bit(stPlatform.strID));

            set<std::string> lineIDSet(PlatFormdata.strLoadIDVec.begin(), PlatFormdata.strLoadIDVec.end());
            PlatFormdata.strLoadIDVec.assign(lineIDSet.begin(), lineIDSet.end());

            for (auto loadData : PlatFormdata.strLoadIDVec)
            {
                if (loadData == "")
                {
                    continue;
                }

                CNebulaMissionPlatformLoad stPlatformLoad = m_pITaskManageService->GetMissionPlatformLoad(loadData);
                if (stPlatformLoad.eType != EMissionPlatformLoadType_DY)
                {
                    continue;
                }

                QTreeWidgetItem *pChildItemLoad = new QTreeWidgetItem(pChildItemPlatform);
                QString strTmp;
				strTmp = tr("Mount: ") + QString::fromLocal8Bit(stPlatformLoad.strMode);

                pChildItemLoad->setText(0, strTmp);
                pChildItemLoad->setData(0, Qt::UserRole, QString::fromStdString(loadData));
            }
        }
    }

    ui->treeWidget->expandAll();
}

void CDlgFireManage::CreateTableMenu()
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

void CDlgFireManage::ControlConnet()
{
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_tableWidget_customContextMenuRequested(const QPoint)));
    connect(ui->treeWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(on_treeWidget_customContextMenuRequested(const QPoint)));

    connect(ui->treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(on_treeWidget_itemClicked(QTreeWidgetItem *, int)));
}

void CDlgFireManage::CreateListHeading()
{
    QStringList headerlabels;
    headerlabels << tr(" No. ") << tr(" Execution Sequence ") << tr(" Waypoint Number ") << tr(" Order ") << tr(" Execution Time(ms) ") << tr(" Remark ");

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

void CDlgFireManage::UpdateListData(std::string strTaskID , std::string strPlatformID , std::string strLoadID )
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    m_CodeSet_Load_DB_Vec.clear();

    if (strPlatformID == "" || strTaskID == "" || strLoadID == "")
    {
        return;
    }

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_CodeSet_Load_DB_Vec = m_pITaskManageService->GetCodeSet_Load(strTaskID, strPlatformID);

    int nNo = 0;
    for (auto data : m_CodeSet_Load_DB_Vec)
    {
        if (0 == strcmp(data.GetLoadID().c_str(), strLoadID.c_str()) &&
                0 == strcmp(data.GetPlatformID().c_str(), strPlatformID.c_str()))
        {
            ui->tableWidget->setRowCount(nNo + 1);

            QString strNo, strSuit, strPointNo, strCodeName, strTime, strRemark;
            strNo = QString::number(nNo + 1);
            strSuit = QString::number(data.GetSuite());
            strPointNo = QString::number(data.GetPointID());
            qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = (qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID)data.GetCodeID();
            strCodeName = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ePara);
            strTime = "0";
            strRemark = QString::fromStdString(data.GetRemark());

            QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
            QTableWidgetItem *item1 = new QTableWidgetItem(strSuit);
            QTableWidgetItem *item2 = new QTableWidgetItem(strPointNo);
            QTableWidgetItem *item3 = new QTableWidgetItem(strCodeName);
            QTableWidgetItem *item4 = new QTableWidgetItem(strTime);
            QTableWidgetItem *item5 = new QTableWidgetItem(strRemark);

            ui->tableWidget->setItem(nNo, 0, item0);
            ui->tableWidget->setItem(nNo, 1, item1);
            ui->tableWidget->setItem(nNo, 2, item2);
            ui->tableWidget->setItem(nNo, 3, item3);
            ui->tableWidget->setItem(nNo, 4, item4);
            ui->tableWidget->setItem(nNo, 5, item5);

            ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

            nNo++;
        }
    }
}

void CDlgFireManage::UpdataWindowData()
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

void CDlgFireManage::UpdateTaskCombobox()
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

int CDlgFireManage::GetSelectedTreeLevel(QMap<int, QString> &TreeDataMap)
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
        m_LoadID = TreeDataMap.find(1).value();
        m_PlatformID = TreeDataMap.find(2).value();
        m_TaskID = TreeDataMap.find(3).value();
        break;
    default:
        break;
    }

    return nLevel;
}

int CDlgFireManage::GetSelectedTreeLevel()
{
    return m_nTreeLevel;
}

void CDlgFireManage::GetTaskInfo(QString &strTaskID, QString &strPlatformID, QString &strLoadID)
{
    strTaskID = m_TaskID;
    strPlatformID = m_PlatformID;
    strLoadID = m_LoadID;
}

void CDlgFireManage::CalculateMark()
{
    m_MarkList.clear();

    CNebulaAcitonRelationVec RelationVec = m_pITaskManageService->GetActivatingShowData();

    for (auto action : RelationVec)
    {
        for (auto Task : action.ReTaskVec)
        {
            if (0 == strcmp(Task.strTaskID.c_str(), m_TaskID.toLocal8Bit()))
            {
                for (auto Platform : Task.PlatFormVec)
                {
                    if (0 == strcmp(Platform.strPlatformID.c_str(), m_PlatformID.toLocal8Bit()))
                    {
                        auto itr = Platform.strMarkIDvec.begin();
                        for (auto itr : Platform.strMarkIDvec)
                        {
                            m_MarkList.push_back(itr);
                        }

                        if (m_MarkList.size() > 0)
                        {

                            set<std::string> lineIDSet(m_MarkList.begin(), m_MarkList.end());
                            m_MarkList.assign(lineIDSet.begin(), lineIDSet.end());
                        }

                        break;
                    }
                }
                break;
            }
        }
    }
}

std::vector<std::string> CDlgFireManage::GetMarkList()
{
    return m_MarkList;
}

void CDlgFireManage::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    m_TreeMenu->exec(QCursor::pos());
}

void CDlgFireManage::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    m_TableMenu->exec(QCursor::pos());
}

void CDlgFireManage::on_comboBox_currentIndexChanged(int index)
{

}

void CDlgFireManage::on_Menu_Tree_Add()
{

}

void CDlgFireManage::on_Menu_Tree_Remove()
{

}

void CDlgFireManage::on_Menu_Table_Add()
{
    if (LY_TaskManage::Get().GetDlgFireManageEdit() == nullptr)
    {
        return;
    }

    if (LY_TaskManage::Get().GetDlgFireManageEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgFireManageEdit()->hide();
    }
    else
    {
        LY_TaskManage::Get().GetDlgFireManageEdit()->SetCodeSetData(m_CodeSet_Load_DB_Vec);
        LY_TaskManage::Get().GetDlgFireManageEdit()->SetPointID(1);
        LY_TaskManage::Get().GetDlgFireManageEdit()->show();
        LY_TaskManage::Get().GetDlgFireManageEdit()->raise();
    }
}

void CDlgFireManage::on_Menu_Table_Edit()
{
    if (LY_TaskManage::Get().GetDlgFireManageEdit() == nullptr)
    {
        return;
    }

    int nSel = ui->tableWidget->currentRow();
    if (nSel == -1)
    {
        LyMessageBox::information(nullptr, tr("tip"), tr("Please select edit entry"));
        return;
    }

    if (LY_TaskManage::Get().GetDlgFireManageEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgFireManageEdit()->hide();
    }
    else
    {
        LY_TaskManage::Get().GetDlgFireManageEdit()->SetCodeSetData(m_CodeSet_Load_DB_Vec);
        QTableWidgetItem *pItem = ui->tableWidget->item(nSel, 2);
        LY_TaskManage::Get().GetDlgFireManageEdit()->SetPointID(pItem->text().toInt());
        LY_TaskManage::Get().GetDlgFireManageEdit()->show();
        LY_TaskManage::Get().GetDlgFireManageEdit()->raise();
    }
}

void CDlgFireManage::on_Menu_Table_Remove()
{

}

void CDlgFireManage::on_treeWidget_itemClicked(QTreeWidgetItem *item, int column)
{
    QMap<int, QString> TreeDataMap;
    m_nTreeLevel = GetSelectedTreeLevel(TreeDataMap);
    if (m_nTreeLevel < 3)
    {
        UpdateListData();
        return;
    }

    UpdateListData(m_TaskID.toStdString(), m_PlatformID.toStdString(), m_LoadID.toStdString());
    CalculateMark();

    if (LY_TaskManage::Get().GetDlgFireManageEdit() != nullptr)
    {
        LY_TaskManage::Get().GetDlgFireManageEdit()->SetTaskID(m_TaskID.toStdString());
        LY_TaskManage::Get().GetDlgFireManageEdit()->SetPlatformID(m_PlatformID.toStdString());
        LY_TaskManage::Get().GetDlgFireManageEdit()->SetLoadID(m_LoadID.toStdString());
    }
}
