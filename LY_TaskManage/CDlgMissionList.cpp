#include "CDlgMissionList.h"
#include "ui_CDlgMissionList.h"

#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include "LyMessageBox.h"
#include "DataManage/NebulaTask/NebulaMissionAutoCommand.h"
#include "CDlgSetPlat.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "netheader.h"
#include "Protocol/XygsTelegraph/TelegrpahHead.h"
#include "lY_TaskManageInterface.h"

CDlgMissionList::CDlgMissionList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgMissionList),
	m_TableMenu(nullptr),
	m_MsgScheme(nullptr)
{
    ui->setupUi(this);

    UpdateTrackInfoToListHeading();
    CreateTableMenu();
    TabWidgetConnet();

    ui->tab_2->setVisible(false);
    ui->tab_3->setVisible(false);
    ui->tab_5->setVisible(false);
    ui->tab_6->setVisible(false);
    ui->tab_7->setVisible(false);
    ui->tab_8->setVisible(false);

    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    ui->tabWidget->setTabEnabled(3, false);
    ui->tabWidget->setTabEnabled(4, false);
    ui->tabWidget->setTabEnabled(5, false);
    ui->tabWidget->setTabEnabled(6, false);

    if(ui->tabWidget->tabBar() != nullptr){
        ui->tabWidget->tabBar()->setVisible(false);

        ui->tabWidget->tabBar()->setTabEnabled(1, false);
        ui->tabWidget->tabBar()->setTabEnabled(2, false);
        ui->tabWidget->tabBar()->setTabEnabled(3, false);
        ui->tabWidget->tabBar()->setTabEnabled(4, false);
        ui->tabWidget->tabBar()->setTabEnabled(5, false);
        ui->tabWidget->tabBar()->setTabEnabled(6, false);
    }
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabWidget_currentChanged(int)));

    m_bUpdateFlg = false;

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

    m_MsgScheme = new QTimer(this);
    connect(m_MsgScheme, &QTimer::timeout, this, &CDlgMissionList::timerOpenTask);
    m_MsgScheme->start(500);
}
void CDlgMissionList::timerOpenTask()
{
    if (b_openTask)
    {
        on_Menu_Add();
        b_openTask = false;
    }
}
CDlgMissionList::~CDlgMissionList()
{
    if (m_MsgScheme != nullptr)
    {
        m_MsgScheme->stop();
    }
    delete ui;
}

void CDlgMissionList::TabWidgetConnet()
{
    connect(ui->tableWidget_5, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(customContextMenuRequested(const QPoint)));
    connect(ui->tableWidget_5, &QTableWidget::cellClicked, this, &CDlgMissionList::QTableWidgetClicked);
    connect(ui->tableWidget_6, &QTableWidget::cellClicked, this, &CDlgMissionList::QTableWidgetClicked);
    connect(ui->tableWidget_7, &QTableWidget::cellClicked, this, &CDlgMissionList::QTableWidgetClicked);
    connect(ui->tableWidget_8, &QTableWidget::cellClicked, this, &CDlgMissionList::QTableWidgetClicked);
    connect(ui->tableWidget, &QTableWidget::cellClicked, this, &CDlgMissionList::QTableWidgetClicked);
    connect(ui->tableWidget_2, &QTableWidget::cellClicked, this, &CDlgMissionList::QTableWidgetClicked);
    connect(ui->tableWidget_3, &QTableWidget::cellClicked, this, &CDlgMissionList::QTableWidgetClicked);

}

void CDlgMissionList::CreateTableMenu()
{
    if (m_TableMenu == nullptr)
    {
        m_TableMenu = new QMenu(this);
        m_MenuAdd = m_TableMenu->addAction(tr2("添加"));
        m_MenuEdit = m_TableMenu->addAction(tr2("修改"));
        m_MenuRemove = m_TableMenu->addAction(tr2("删除"));
        m_MenuUpdate = m_TableMenu->addAction(tr2("更新"));
        m_MenuSaveAs = m_TableMenu->addAction(tr2("另存"));
        m_MenuEditState = m_TableMenu->addAction(tr2("任务执行状态"));
        m_MenuMissionAllocation = m_TableMenu->addAction(tr2("任务分配"));
        m_MenuDistribute = m_TableMenu->addAction(tr2("下发"));

        m_StateMenu = new QMenu();
        m_MenuUnexecuted = m_StateMenu->addAction(tr2("任务未执行"));
        m_MenuExecuting = m_StateMenu->addAction(tr2("任务正常进行中"));
        m_MenuComplete = m_StateMenu->addAction(tr2("任务完成"));
        m_MenuCancellation = m_StateMenu->addAction(tr2("任务取消"));
        m_MenuAdvance = m_StateMenu->addAction(tr2("任务提前"));
        m_MenuDelay = m_StateMenu->addAction(tr2("任务滞后"));
        m_MenuFail = m_StateMenu->addAction(tr2("任务失败"));
        m_MenuEditState->setMenu(m_StateMenu);

        m_DistributeMenu = new QMenu();
        qnzkna::WeaponManage::MultiWeaponComponentMap pads;
        ITaskManagerActivator::Get().getISystemMaintenanceService()->GetWeaponComponents(qnzkna::WeaponManage::WeaponComponent_Type_HandheldPad, &pads);
        auto padsItor = pads.begin();
        while (padsItor != pads.end())
        {
            QAction *strIDAction = m_DistributeMenu->addAction(QString::fromStdString(padsItor->second.getComponentName()));
            auto strID = padsItor->second.getComponentID();
            connect(strIDAction, &QAction::triggered, this, [this, strID]()
            {
                char pBuf[1024] = { 0 };
                int pLength = 0;
                int currentRow = ui->tableWidget_5->currentRow();

                if (currentRow > m_MissionVec.size())
                {
                    LyMessageBox::information(NULL, tr("tip"), tr("Data error, send failed!"));
                    return;
                }

                NetHeader *pHead = (NetHeader *)pBuf;
                TeleMsg_FB_03 *pMsgTele = (TeleMsg_FB_03 *)(pBuf + sizeof(NetHeader));
                pHead->ucType = NETHEADER_UCTYPE_TELEGRAPH_CMD;
                pHead->ucSubType = NETHEADER_UCTYPE_TELEGRAPH_CMD_UP_TO_SERVER;
                pHead->unLength = sizeof(NetHeader) + sizeof(TeleMsg_FB_03);
                pHead->unNodeID = QString::fromStdString(strID).toInt();
                pHead->unNodeType = 3;
                pLength = pHead->unLength;
                pMsgTele->head.MainCode = 0xFB;
                pMsgTele->head.SubCode = 0x03;
                pMsgTele->head.MsgVersion = 1;
                pMsgTele->head.RecvId = 10003;
                pMsgTele->head.SendId = 0x00;
                pMsgTele->head.PassId = 0x00;
                pMsgTele->head.MsgTime = QDateTime::currentMSecsSinceEpoch();
                strncpy(pMsgTele->TaskID, m_MissionVec[currentRow].GetID().c_str(), 24);
                pMsgTele->taskType = m_MissionVec[currentRow].GetDBMissionType();

                auto targets = m_MissionVec[currentRow].GetMarkIDList();
                int len = min((int)targets.size(), 5);
                for (int i = 0; i < len; i++)
                {
                    SystemMarkInfo tmpTar = ITaskManagerActivator::Get().getPIMarkManageService()->GetMark(targets[i]);
                    strncpy(pMsgTele->stMarkInfo[i].cMarkID, tmpTar.GetID().c_str(), 24);
                    strncpy(pMsgTele->stMarkInfo[i].cName, tmpTar.GetName().c_str(), 24);
                    pMsgTele->stMarkInfo[i].cType = tmpTar.GetType();
                    pMsgTele->stMarkInfo[i].TaskHeight = tmpTar.GetHeight() * 1e2;
                    pMsgTele->stMarkInfo[i].TaskLng = tmpTar.GetLon() * 1e6;
                    pMsgTele->stMarkInfo[i].TaskLat = tmpTar.GetLat() * 1e6;
                }

                QString strTime;
                strTime = QString::fromStdString(m_MissionVec[currentRow].GetStartTime().c_str());
                QDateTime dataTime = QDateTime::fromString(strTime, "yyyy/M/d h:mm");
                pMsgTele->StartTime = dataTime.toSecsSinceEpoch();
                strTime = QString::fromStdString(m_MissionVec[currentRow].GetFinishTime().c_str());
                dataTime = QDateTime::fromString(strTime, "yyyy/M/d h:mm");
                pMsgTele->FinishTime = dataTime.toSecsSinceEpoch();
                strncpy(pMsgTele->cRequire, m_MissionVec[currentRow].GetrDetectDemand().c_str(), 32);
                strncpy(pMsgTele->cSource, m_MissionVec[currentRow].GetDBSource().c_str(), 32);
                strncpy(pMsgTele->cMark, m_MissionVec[currentRow].GetRemark().c_str(), 32);
                LY_TaskManageInterface::Get().SendData(pBuf, pLength);
            });
            padsItor++;
        }
        m_MenuDistribute->setMenu(m_DistributeMenu);

        connect(m_MenuAdd, SIGNAL(triggered()), this, SLOT(on_Menu_Add()));
        connect(m_MenuEdit, SIGNAL(triggered()), this, SLOT(on_Menu_Edit()));
        connect(m_MenuRemove, SIGNAL(triggered()), this, SLOT(on_Menu_Remove()));
        connect(m_MenuUpdate, SIGNAL(triggered()), this, SLOT(on_Menu_Update()));
        connect(m_MenuSaveAs, SIGNAL(triggered()), this, SLOT(on_Menu_SaveAs()));
        connect(m_MenuUnexecuted, SIGNAL(triggered()), this, SLOT(on_Menu_Unexecuted()));
        connect(m_MenuComplete, SIGNAL(triggered()), this, SLOT(on_Menu_Complete()));
        connect(m_MenuExecuting, SIGNAL(triggered()), this, SLOT(on_Menu_Executing()));
        connect(m_MenuCancellation, SIGNAL(triggered()), this, SLOT(on_Menu_Cancellation()));
        connect(m_MenuAdvance, SIGNAL(triggered()), this, SLOT(on_Menu_Advance()));
        connect(m_MenuDelay, SIGNAL(triggered()), this, SLOT(on_Menu_Delay()));
        connect(m_MenuFail, SIGNAL(triggered()), this, SLOT(on_Menu_Fail()));
        connect(m_MenuMissionAllocation, SIGNAL(triggered()), this, SLOT(on_Menu_MissionAllocation()));
        connect(m_MenuDistribute, &QAction::triggered, this, &CDlgMissionList::on_Menu_Distribute);
    }

    if (m_MenuEdit && m_MenuComplete)
    {
        m_MenuComplete->setEnabled(true);
        m_MenuUnexecuted->setEnabled(true);
        m_MenuExecuting->setEnabled(true);
        m_MenuComplete->setEnabled(true);
        m_MenuEdit->setEnabled(true);
        m_MenuRemove->setEnabled(true);
        m_MenuUpdate->setEnabled(true);
        m_MenuSaveAs->setEnabled(true);
        m_MenuEditState->setEnabled(true);
        m_MenuMissionAllocation->setEnabled(true);
    }
}

void CDlgMissionList::UpdateTrackInfoToListHeading()
{
    for (int i = 0 ; i < ui->tabWidget->count(); i++)
    {
        QWidget *p1 = ui->tabWidget->widget(i);
        if (p1->layout()->count() > 0)
        {
            QLayoutItem *it = p1->layout()->itemAt(0);
            QTableWidget *pTabWidget = qobject_cast<QTableWidget *>(it->widget());

            if (pTabWidget == NULL)
            {
                continue;
            }

            QStringList headerlabels;

			headerlabels << tr("  No.  ") << tr("  Task Number  ") << tr("  Task Execution Status  ") << tr("  Task Name  ") << tr("  Task Force  ") << tr("  Task Type  ");
			headerlabels << tr("  Task Target  ") << tr("  Task Platform  ");

			headerlabels << tr(" Task Requirement ") << tr(" The latest effective time of investigation information ");
			headerlabels << tr(" Start Time ") << tr(" Destroy Time ") << tr(" End Time ") << tr(" Remark ");

            pTabWidget->setColumnCount(headerlabels.size());
            pTabWidget->setHorizontalHeaderLabels(headerlabels);
            for (int i = 0; i < headerlabels.size(); i++)
            {

                pTabWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);

            }

            pTabWidget->verticalHeader()->setHidden(true);
            pTabWidget->setSelectionBehavior(QTableWidget::SelectRows);
            pTabWidget->setSelectionMode(QAbstractItemView::SingleSelection);
            pTabWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
            pTabWidget->setAlternatingRowColors(true);
            pTabWidget->setContextMenuPolicy(Qt::CustomContextMenu);
            pTabWidget->setSortingEnabled(true);
            pTabWidget->sortItems(0);
        }
    }
}

void CDlgMissionList::UpdataWindowData()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_MissionVec.clear();

    m_MissionVec = m_pITaskManageService->GetActivatingMissions();
    m_nActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
}

void CDlgMissionList::UpdateListData()
{
    UpdataWindowData();
    UpdateListData_All();
    UpdateListData_Attack();
    UpdateListData_Dectect();
    UpdateListData_Protect();
    UpdateListData_Overlook();
    UpdateListData_Help();
    UpdateListData_Transport();
    UpdateListData_Plat();
}

void CDlgMissionList::UpdataListCtrlData(QTableWidget *pQTable, CNeubulaMission &stMission, int &nNo)
{
    QString strNo, strID, strState, strName, strBelong, strType, strDetectDemand, strStartTime, strAttackTime, strFinishTime, strRemark, strVaildTime;
    strNo = tr("%1").arg(nNo + 1, 2, 10, QLatin1Char('0'));
    strID = QString::fromStdString(stMission.GetID());
    strState = GetHurtExeccutingState(stMission.GetState());
    strName = QString::fromStdString(stMission.GetName());
    strBelong = QString::fromStdString(stMission.GetBelong());
    strType = GetTypeString(stMission.GetType());
    QString strMissionMark, strMissionPlat;
    for (auto MissionMark : stMission.GetMarkIDList())
    {
        if (stMission.GetMarkIDList().size() == 1)
        {
            strMissionMark = QString::fromStdString(MissionMark);
        }
        else
        {
            strMissionMark += QString::fromStdString(MissionMark) + tr(";");
        }
    }
    for (auto MissionPlat : stMission.GetPlatInfoMap())
    {
        if (stMission.GetPlatInfoMap().size() == 1)
        {
            strMissionPlat = QString::fromStdString(MissionPlat.GetResourceName()) + tr("(") + QString::number(MissionPlat.GetResourceNum()) + tr(")");
        }
        else
        {
            strMissionPlat += QString::fromStdString(MissionPlat.GetResourceName()) + tr("(") + QString::number(MissionPlat.GetResourceNum()) + tr(")") + tr(";");
        }
    }

    strDetectDemand = QString::fromStdString(stMission.GetrDetectDemand());
    strVaildTime = QString::fromStdString(stMission.GetValidTime());
    if (stMission.GetType() != ENeubulaMissionType_Detect)
    {
        strVaildTime = "-";
    }
    strStartTime = QString::fromStdString(stMission.GetStartTime());
    strAttackTime = QString::fromStdString(stMission.GetAttackTime());
    strFinishTime = QString::fromStdString(stMission.GetFinishTime());
    strRemark = QString::fromStdString(stMission.GetRemark());

    QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
    QTableWidgetItem *item1 = new QTableWidgetItem(strID);
    QTableWidgetItem *item2 = new QTableWidgetItem(strState);
    switch (stMission.GetState())
    {
    case ENeubulaMissionExeccutingState_Unexecuted:
        break;
    case ENeubulaMissionExeccutingState_Complete:
        item2->setTextColor(Qt::red);
        break;
    case ENeubulaMissionExeccutingState_Executing:
        item2->setTextColor(Qt::green);
        break;
    default:
        break;
    }
    QTableWidgetItem *item3 = new QTableWidgetItem(strName);
    QTableWidgetItem *item4 = new QTableWidgetItem(strBelong);
    QTableWidgetItem *item5 = new QTableWidgetItem(strType);
    QTableWidgetItem *item6 = new QTableWidgetItem(strMissionMark);
    QTableWidgetItem *item7 = new QTableWidgetItem(strMissionPlat);
    QTableWidgetItem *item12 = new QTableWidgetItem(strDetectDemand);
    QTableWidgetItem *item15 = new QTableWidgetItem(strStartTime);
    QTableWidgetItem *item16 = new QTableWidgetItem(strAttackTime);
    QTableWidgetItem *item17 = new QTableWidgetItem(strFinishTime);
    QTableWidgetItem *item18 = new QTableWidgetItem(strRemark);
    QTableWidgetItem *item19 = new QTableWidgetItem(strVaildTime);

    pQTable->setItem(nNo, 0, item0);
    pQTable->setItem(nNo, 1, item1);
    pQTable->setItem(nNo, 2, item2);
    pQTable->setItem(nNo, 3, item3);
    pQTable->setItem(nNo, 4, item4);
    pQTable->setItem(nNo, 5, item5);
    pQTable->setItem(nNo, 6, item6);
    pQTable->setItem(nNo, 7, item7);
    pQTable->setItem(nNo, 8, item12);
    pQTable->setItem(nNo, 9, item19);
    pQTable->setItem(nNo, 10, item15);
    pQTable->setItem(nNo, 11, item16);
    pQTable->setItem(nNo, 12, item17);
    pQTable->setItem(nNo, 13, item18);

    pQTable->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 8)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 11)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 12)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    pQTable->item(nNo, 13)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void CDlgMissionList::UpdateListData_All()
{
    ui->tableWidget_5->clearContents();
    ui->tableWidget_5->setRowCount(m_MissionVec.size());
    ui->tableWidget_5->sortItems(0);

    if (m_MissionVec.size() == 0)
    {
        return;
    }

    int nNo = 0;
    for (auto data : m_MissionVec)
    {

        UpdataListCtrlData(ui->tableWidget_5, data, nNo);
        nNo++;
    }
}

void CDlgMissionList::UpdateListData_Attack()
{
    ui->tableWidget_6->clearContents();
    ui->tableWidget_6->setRowCount(0);
    ui->tableWidget_6->sortItems(0);

    int nNo = 0;
    for (auto data : m_MissionVec)
    {
        if (data.GetType() != ENeubulaMissionType_Attack)
        {
            continue;
        }
        ui->tableWidget_6->setRowCount(nNo + 1);
        UpdataListCtrlData(ui->tableWidget_6, data, nNo);
        nNo++;
        qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_CommonCommand st1;
        st1.nCodeID = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_StartStopContinueGrab;
        st1.para.ZC_CommonCommand_StartStopContinueGrab.interval = 1000;
    }
}

void CDlgMissionList::UpdateListData_Dectect()
{
    ui->tableWidget_7->clearContents();
    ui->tableWidget_7->setRowCount(0);
    ui->tableWidget_7->sortItems(0);

    int nNo = 0;
    for (auto data : m_MissionVec)
    {
        if (data.GetType() != ENeubulaMissionType_Detect)
        {
            continue;
        }
        ui->tableWidget_7->setRowCount(nNo + 1);
        UpdataListCtrlData(ui->tableWidget_7, data, nNo);
        nNo++;
    }
}

void CDlgMissionList::UpdateListData_Protect()
{
    ui->tableWidget_8->clearContents();
    ui->tableWidget_8->setRowCount(0);
    ui->tableWidget_8->sortItems(0);

    int nNo = 0;
    for (auto data : m_MissionVec)
    {
        if (data.GetType() != ENeubulaMissionType_Protect)
        {
            continue;
        }
        ui->tableWidget_8->setRowCount(nNo + 1);
        UpdataListCtrlData(ui->tableWidget_8, data, nNo);
        nNo++;
    }
}

void CDlgMissionList::UpdateListData_Overlook()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->sortItems(0);

    int nNo = 0;
    for (auto data : m_MissionVec)
    {
        if (data.GetType() != ENeubulaMissionType_Overlook)
        {
            continue;
        }
        ui->tableWidget->setRowCount(nNo + 1);
        UpdataListCtrlData(ui->tableWidget, data, nNo);
        nNo++;
    }
}

void CDlgMissionList::UpdateListData_Help()
{
    ui->tableWidget_2->clearContents();
    ui->tableWidget_2->setRowCount(0);
    ui->tableWidget_2->sortItems(0);

    int nNo = 0;
    for (auto data : m_MissionVec)
    {
        if (data.GetType() != ENeubulaMissionType_Help)
        {
            continue;
        }
        ui->tableWidget_2->setRowCount(nNo + 1);
        UpdataListCtrlData(ui->tableWidget_2, data, nNo);
        nNo++;
    }
}

void CDlgMissionList::UpdateListData_Transport()
{
    ui->tableWidget_3->clearContents();
    ui->tableWidget_3->setRowCount(0);
    ui->tableWidget_3->sortItems(0);

    int nNo = 0;
    for (auto data : m_MissionVec)
    {
        if (data.GetType() != ENeubulaMissionType_Transport)
        {
            continue;
        }
        ui->tableWidget_3->setRowCount(nNo + 1);
        UpdataListCtrlData(ui->tableWidget_3, data, nNo);
        nNo++;
    }
}

void CDlgMissionList::UpdateListData_Plat()
{
    int MissionCount = m_MissionVec.size();

    for (int i = 0; i < m_MissionVec.size(); i++)
    {
        auto Mission = m_MissionVec[i];

    }

}

void CDlgMissionList::SetTabEnable(bool bStates)
{

}

QString CDlgMissionList::GetTypeString(ENeubulaMissionType eType)
{
    switch (eType)
    {
    case ENeubulaMissionType_Unknow:
        return tr("Unknown");
    case ENeubulaMissionType_Attack:
        return tr("Strike");
    case ENeubulaMissionType_Detect:
        return tr("Investigetion");
    case ENeubulaMissionType_Protect:
        return tr("Protection");
    case ENeubulaMissionType_Overlook:
        return tr("Surveillance");
    case ENeubulaMissionType_Help:
        return tr("Rescue");
    case ENeubulaMissionType_Transport:
        return tr("Transport");
    case ENeubulaMissionType_DB:
        return tr("Single Pawn");
    default:
        return tr("Unknown");
    }
}

QString CDlgMissionList::GetHurtDegreeString(ENeubulaMissionHurtDegree eType)
{
    switch (eType)
    {
    case ENeubulaMissionHurtDegree_None:
        return tr("-");
    case ENeubulaMissionHurtDegree_Destroy:
        return tr("Destroy");
    case ENeubulaMissionHurtDegree_Damage:
        return tr("Damage");
    case ENeubulaMissionHurtDegree_Stifle:
        return tr("Suppression");
    case ENeubulaMissionHurtDegree_Jam:
        return tr("Interference");
    case ENeubulaMissionHurtDegree_Break:
        return tr("Turn off");
    case ENeubulaMissionHurtDegree_Overlook:
        return tr("Surveillance");
    case ENeubulaMissionHurtDegree_Block:
        return tr("Block");
    case ENeubulaMissionHurtDegree_Quest:
        return tr("Search and Discovery");
    case ENeubulaMissionHurtDegree_Detect:
        return tr("Reconnaissance Mark Elements");
    case ENeubulaMissionHurtDegree_Protect:
        return tr("Protection");
    case ENeubulaMissionHurtDegree_Help:
        return tr("Rescue");
    case ENeubulaMissionHurtDegree_TransPort:
        return tr("Transport");
    default:
        return tr("-");
    }
}

QString CDlgMissionList::GetHurtExeccutingState(ENeubulaMissionExeccutingState eState)
{
    switch (eState)
    {
    case ENeubulaMissionExeccutingState_Unexecuted:
        return tr("Task not executed");
    case ENeubulaMissionExeccutingState_Complete:
        return tr("Task completed");
    case ENeubulaMissionExeccutingState_Executing:
        return tr("The task is proceeding normally");
    case ENeubulaMissionExeccutingState_Cancellation:
        return tr("Task cancellation");
    case ENeubulaMissionExeccutingState_Advance:
        return tr("Task advanced");
    case ENeubulaMissionExeccutingState_Delay:
        return tr("Task lag");
    case ENeubulaMissionExeccutingState_Fail:
        return tr("Task Fail");

    default:
        return tr("Task not executed");
    }
}

void CDlgMissionList::showEvent(QShowEvent *event)
{
    UpdataWindowData();
    UpdateListData();
    ui->tabWidget->setCurrentIndex(0);
}

void CDlgMissionList::on_Menu_Add()
{
    LY_TaskManage::Get().GetDlgMissionEdit()->SetDataType(1);
    int nTabIndex = ui->tabWidget->currentIndex();
    LY_TaskManage::Get().GetDlgMissionEdit()->InitWindowCtrl(nTabIndex);
    if (LY_TaskManage::Get().GetDlgMissionEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgMissionEdit()->hide();
    }
    LY_TaskManage::Get().GetDlgMissionEdit()->show();
    LY_TaskManage::Get().GetDlgMissionEdit()->raise();
}

void CDlgMissionList::on_Menu_Edit()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Select the task you want to edit!"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                LY_TaskManage::Get().GetDlgMissionEdit()->SetDataType(2);
                LY_TaskManage::Get().GetDlgMissionEdit()->SetWindowInfo(data);
                if (LY_TaskManage::Get().GetDlgMissionEdit()->isVisible())
                {
                    LY_TaskManage::Get().GetDlgMissionEdit()->hide();
                }
                LY_TaskManage::Get().GetDlgMissionEdit()->show();
                LY_TaskManage::Get().GetDlgMissionEdit()->raise();
                break;
            }
        }
    }
}

void CDlgMissionList::on_Menu_Remove()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Select the target you want to delete!"));
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        QString strTmp = tr("Please confirm whether to delete ( task number:") + strID + ")";
        if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
        {
            bool bresut = m_pITaskManageService->RmoveMission(strID.toStdString());
            if (bresut == true)
            {
                LyMessageBox::information(NULL, tr("tip"), tr("Delete successfully"));
                UpdataWindowData();
                UpdateListData();
                LY_TaskManage::Get().GetDlgMissionTree()->Update(m_pITaskManageService);
                LY_TaskManage::Get().GetDlgTaskLineEdit()->update();
            }
            else
            {
                LyMessageBox::information(NULL, tr("tip"), tr("Delete failed"));
            }
        }
    }
}

void CDlgMissionList::on_Menu_Update()
{
    m_pITaskManageService->RmoveAllMission();
    m_pITaskManageService->LoadData();
    UpdataWindowData();
    UpdateListData();
}

void CDlgMissionList::on_Menu_SaveAs()
{
    m_bIsSaveAs = true;
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Please select the task you want to save!"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                m_MissionLineVec = m_pITaskManageService->GetMissionLinesByTask(data.GetID());
                if (m_bIsSaveAs)
                {
                    data.SetID("");
                }
                LY_TaskManage::Get().GetDlgMissionEdit()->SetDataType(1);
                LY_TaskManage::Get().GetDlgMissionEdit()->SetWindowInfo(data);
                if (LY_TaskManage::Get().GetDlgMissionEdit()->isVisible())
                {
                    LY_TaskManage::Get().GetDlgMissionEdit()->hide();
                }
                LY_TaskManage::Get().GetDlgMissionEdit()->show();
                LY_TaskManage::Get().GetDlgMissionEdit()->raise();
                break;
            }
        }
    }
}

void CDlgMissionList::on_Menu_Unexecuted()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Please select the task you want to edit!"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                data.SetState(data.GetStateENum(6));
                m_pITaskManageService->SetMission(data);
                break;
            }
        }

        QTableWidgetItem *pItem1 = pTabCtrl1->item(nHitrow, 1);
        CNeubulaOperationalInformation temp1;
        std::string TaskID1 = pItem1->text().toStdString();
        temp1.SetTaskID(TaskID1);
        temp1.SetInformationType(1);
        temp1.SetRemarks(tr2("任务:修改任务状态为任务未执行").toStdString());
        m_pITaskManageService->DeleteOperationalInformation(1, pItem1->text().toStdString());
        m_pITaskManageService->AddOperationalInformation(temp1);

        pItem1 = pTabCtrl1->item(nHitrow, 6);
        QString str1MarkID = pItem1->text();
        QStringList strList = str1MarkID.split(";");
        for (auto data : strList)
        {
            if (!data.isEmpty())
            {
                CNeubulaOperationalInformation temp2;
                temp2.SetTaskID(TaskID1);
                temp2.SetInformationType(2);
                temp2.SetResourceID(data.toStdString());
                SystemMarkInfo targetInfo = m_pIMarkManageService->GetMark(data.toStdString());
                QString strMarkState = GetMarkState(targetInfo.GetStats());
                temp2.SetRemarks(strMarkState.toStdString());
                m_pITaskManageService->DeleteOperationalInformation(2, TaskID1, data.toStdString());
                m_pITaskManageService->AddOperationalInformation(temp2);
            }
        }

    }
    on_Menu_Update();
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatAmmoList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatAmmoID", pBuf, nBufLen, props));
}

void CDlgMissionList::on_Menu_Complete()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Please select the task you want to edit!"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                data.SetState(data.GetStateENum(1));
                m_pITaskManageService->SetMission(data);
                break;
            }
        }

        QTableWidgetItem *pItem1 = pTabCtrl1->item(nHitrow, 1);
        CNeubulaOperationalInformation temp1;
        std::string TaskID1 = pItem1->text().toStdString();
        temp1.SetTaskID(TaskID1);
        temp1.SetInformationType(1);
        temp1.SetRemarks(tr2("任务:修改任务状态为任务完成").toStdString());
        m_pITaskManageService->DeleteOperationalInformation(1, pItem1->text().toStdString());
        m_pITaskManageService->AddOperationalInformation(temp1);

        pItem1 = pTabCtrl1->item(nHitrow, 6);
        QString str1MarkID = pItem1->text();
        QStringList strList = str1MarkID.split(";");
        for (auto data : strList)
        {
            if (!data.isEmpty())
            {
                CNeubulaOperationalInformation temp2;
                temp2.SetTaskID(TaskID1);
                temp2.SetInformationType(2);
                temp2.SetResourceID(data.toStdString());
                SystemMarkInfo targetInfo = m_pIMarkManageService->GetMark(data.toStdString());
                QString strMarkState = GetMarkState(targetInfo.GetStats());
                temp2.SetRemarks(strMarkState.toStdString());
                m_pITaskManageService->DeleteOperationalInformation(2, TaskID1, data.toStdString());
                m_pITaskManageService->AddOperationalInformation(temp2);
            }
        }
    }
    on_Menu_Update();
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatAmmoList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatAmmoID", pBuf, nBufLen, props));
}

void CDlgMissionList::on_Menu_Executing()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Please select the task you want to edit!"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                data.SetState(data.GetStateENum(3));
                m_pITaskManageService->SetMission(data);
                break;
            }
        }
        QTableWidgetItem *pItem1 = pTabCtrl1->item(nHitrow, 1);
        CNeubulaOperationalInformation temp1;
        std::string TaskID1 = pItem1->text().toStdString();
        temp1.SetTaskID(TaskID1);
        temp1.SetInformationType(1);
        temp1.SetRemarks(tr2("任务:修改任务状态为任务正在执行中").toStdString());
        m_pITaskManageService->DeleteOperationalInformation(1, pItem1->text().toStdString());
        m_pITaskManageService->AddOperationalInformation(temp1);

        pItem1 = pTabCtrl1->item(nHitrow, 6);
        QString str1MarkID = pItem1->text();
        QStringList strList = str1MarkID.split(";");
        for (auto data : strList)
        {
            if (!data.isEmpty())
            {
                CNeubulaOperationalInformation temp2;
                temp2.SetTaskID(TaskID1);
                temp2.SetInformationType(2);
                temp2.SetResourceID(data.toStdString());
                SystemMarkInfo targetInfo = m_pIMarkManageService->GetMark(data.toStdString());
                QString strMarkState = GetMarkState(targetInfo.GetStats());
                temp2.SetRemarks(strMarkState.toStdString());
                m_pITaskManageService->DeleteOperationalInformation(2, TaskID1, data.toStdString());
                m_pITaskManageService->AddOperationalInformation(temp2);
            }
        }
    }
    on_Menu_Update();
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatAmmoList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatAmmoID", pBuf, nBufLen, props));
}

void CDlgMissionList::on_Menu_MissionAllocation()
{
    if (LY_TaskManage::Get().m_pDlgMissionAllocation)
    {
        if (LY_TaskManage::Get().m_pDlgMissionAllocation->isVisible())
        {
            LY_TaskManage::Get().m_pDlgMissionAllocation->close();
        }
        else
        {

            int nHitrow = -1;
            int nTabIndex = ui->tabWidget->currentIndex();
            QWidget *p1 = ui->tabWidget->widget(nTabIndex);
            if (p1->layout()->count() > 0)
            {
                QLayoutItem *it = p1->layout()->itemAt(0);
                QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
                nHitrow = pTabCtrl1->currentRow();
                m_sMissionName = pTabCtrl1->item(nHitrow, 1)->text().toStdString();
            }

            LY_TaskManage::Get().m_pDlgMissionAllocation->show();
            LY_TaskManage::Get().m_pDlgMissionAllocation->Init();
        }

    }
}

void CDlgMissionList::on_Menu_Distribute()
{
}

void CDlgMissionList::customContextMenuRequested(const QPoint &pos)
{
    CreateTableMenu();
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {

            m_MenuEdit->setEnabled(false);
            m_MenuRemove->setEnabled(false);
            m_MenuUpdate->setEnabled(false);
            m_MenuSaveAs->setEnabled(false);
            m_MenuEditState->setEnabled(false);
            m_MenuMissionAllocation->setEnabled(false);
            m_MenuDistribute->setEnabled(false);
        }
        else
        {
            QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
            QString strID = pItem->text();

            for (auto data : m_MissionVec)
            {
                if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
                {
                    switch (data.GetState())
                    {
                    case ENeubulaMissionExeccutingState_Unexecuted:
                        m_MenuUnexecuted->setEnabled(false);
                        break;
                    case ENeubulaMissionExeccutingState_Complete:
                        m_MenuEdit->setEnabled(false);
                        m_MenuComplete->setEnabled(false);
                        break;
                    case ENeubulaMissionExeccutingState_Executing:
                        m_MenuEdit->setEnabled(false);
                        m_MenuExecuting->setEnabled(false);
                        break;
                    default:
                        break;
                    }

                    break;
                }
            }

            QTableWidgetItem *pItem_5 = pTabCtrl1->item(nHitrow, 5);
            QString text_5 = pItem_5->text();
            if (text_5 == tr("Single Pawn"))
            {
                m_MenuDistribute->setEnabled(true);
            }
            else
            {
                m_MenuDistribute->setEnabled(false);
            }
        }
    }
    m_TableMenu->exec(QCursor::pos());
}

void CDlgMissionList::tabWidget_currentChanged(int index)
{

}

void CDlgMissionList::Update(qnzkna::TaskManage::INebulaTaskManageService *sub)
{
    on_Menu_Update();
}

void CDlgMissionList::clearGroupBoxPlat(QGroupBox *groupBoxPlat)
{
    QVBoxLayout *allVLay = groupBoxPlat->findChild<QVBoxLayout *>();

    auto pushButtons = groupBoxPlat->findChildren<QPushButton *>();
    auto labels = groupBoxPlat->findChildren<QLabel *>();

    auto hBoxs = groupBoxPlat->findChildren<QHBoxLayout *>();
    auto vBoxs = groupBoxPlat->findChildren<QVBoxLayout *>();
    for (int i = 0; i < pushButtons.size(); i++)
    {
        auto elem = pushButtons[i];
        if (elem != NULL)
        {
            delete elem;
        }
    }
    for (int i = 0; i < labels.size(); i++)
    {
        auto elem = labels[i];
        if (elem != NULL)
        {
            delete elem;
        }
    }
    for (int i = 0; i < hBoxs.size(); i++)
    {
        auto elem = hBoxs[i];
        if (elem != NULL)
        {
            delete elem;
        }
    }
    for (int i = 0; i < vBoxs.size(); i++)
    {
        auto elem = vBoxs[i];
        if (elem != NULL)
        {
            delete elem;
        }
    }
    groupBoxPlat->update();
}

void CDlgMissionList::updateMenu()
{
    on_Menu_Update();
}

void CDlgMissionList::QTableWidgetClicked()
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

void CDlgMissionList::on_Menu_Cancellation()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("Tip"), tr("Please select the task that needs to be edited！"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                data.SetState(data.GetStateENum(2));
                m_pITaskManageService->SetMission(data);
                break;
            }
        }
        QTableWidgetItem *pItem1 = pTabCtrl1->item(nHitrow, 1);
        CNeubulaOperationalInformation temp1;
        std::string TaskID1 = pItem1->text().toStdString();
        temp1.SetTaskID(TaskID1);
        temp1.SetInformationType(1);
        temp1.SetRemarks(tr2("任务:修改任务状态为任务取消").toStdString());
        m_pITaskManageService->DeleteOperationalInformation(1, pItem1->text().toStdString());
        m_pITaskManageService->AddOperationalInformation(temp1);

        pItem1 = pTabCtrl1->item(nHitrow, 6);
        QString str1MarkID = pItem1->text();
        QStringList strList = str1MarkID.split(";");
        for (auto data : strList)
        {
            if (!data.isEmpty())
            {
                CNeubulaOperationalInformation temp2;
                temp2.SetTaskID(TaskID1);
                temp2.SetInformationType(2);
                temp2.SetResourceID(data.toStdString());
                SystemMarkInfo targetInfo = m_pIMarkManageService->GetMark(data.toStdString());
                QString strMarkState = GetMarkState(targetInfo.GetStats());
                temp2.SetRemarks(strMarkState.toStdString());
                m_pITaskManageService->DeleteOperationalInformation(2, TaskID1, data.toStdString());
                m_pITaskManageService->AddOperationalInformation(temp2);
            }
        }
    }
    on_Menu_Update();
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatAmmoList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatAmmoID", pBuf, nBufLen, props));
}

void CDlgMissionList::on_Menu_Advance()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("Tip"), tr("Please select the task that needs to be edited！"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                data.SetState(data.GetStateENum(4));
                m_pITaskManageService->SetMission(data);
                break;
            }
        }
        QTableWidgetItem *pItem1 = pTabCtrl1->item(nHitrow, 1);
        CNeubulaOperationalInformation temp1;
        std::string TaskID1 = pItem1->text().toStdString();
        temp1.SetTaskID(TaskID1);
        temp1.SetInformationType(1);
        temp1.SetRemarks(tr2("任务:修改任务状态为任务提前").toStdString());
        m_pITaskManageService->DeleteOperationalInformation(1, pItem1->text().toStdString());
        m_pITaskManageService->AddOperationalInformation(temp1);

        pItem1 = pTabCtrl1->item(nHitrow, 6);
        QString str1MarkID = pItem1->text();
        QStringList strList = str1MarkID.split(";");
        for (auto data : strList)
        {
            if (!data.isEmpty())
            {
                CNeubulaOperationalInformation temp2;
                temp2.SetTaskID(TaskID1);
                temp2.SetInformationType(2);
                temp2.SetResourceID(data.toStdString());
                SystemMarkInfo targetInfo = m_pIMarkManageService->GetMark(data.toStdString());
                QString strMarkState = GetMarkState(targetInfo.GetStats());
                temp2.SetRemarks(strMarkState.toStdString());
                m_pITaskManageService->DeleteOperationalInformation(2, TaskID1, data.toStdString());
                m_pITaskManageService->AddOperationalInformation(temp2);
            }
        }
    }
    on_Menu_Update();
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatAmmoList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatAmmoID", pBuf, nBufLen, props));
}

void CDlgMissionList::on_Menu_Delay()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("Tip"), tr("Please select the task that needs to be edited！"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                data.SetState(data.GetStateENum(5));
                m_pITaskManageService->SetMission(data);
                break;
            }
        }
        QTableWidgetItem *pItem1 = pTabCtrl1->item(nHitrow, 1);
        CNeubulaOperationalInformation temp1;
        std::string TaskID1 = pItem1->text().toStdString();
        temp1.SetTaskID(TaskID1);
        temp1.SetInformationType(1);
        temp1.SetRemarks(tr2("任务:修改任务状态为任务滞后").toStdString());
        m_pITaskManageService->DeleteOperationalInformation(1, pItem1->text().toStdString());
        m_pITaskManageService->AddOperationalInformation(temp1);

        pItem1 = pTabCtrl1->item(nHitrow, 6);
        QString str1MarkID = pItem1->text();
        QStringList strList = str1MarkID.split(";");
        for (auto data : strList)
        {
            if (!data.isEmpty())
            {
                CNeubulaOperationalInformation temp2;
                temp2.SetTaskID(TaskID1);
                temp2.SetInformationType(2);
                temp2.SetResourceID(data.toStdString());
                SystemMarkInfo targetInfo = m_pIMarkManageService->GetMark(data.toStdString());
                QString strMarkState = GetMarkState(targetInfo.GetStats());
                temp2.SetRemarks(strMarkState.toStdString());
                m_pITaskManageService->DeleteOperationalInformation(2, TaskID1, data.toStdString());
                m_pITaskManageService->AddOperationalInformation(temp2);
            }
        }
    }
    on_Menu_Update();
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatAmmoList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatAmmoID", pBuf, nBufLen, props));
}

QString CDlgMissionList::GetMarkState(EMarkState eState)
{
    switch (eState)
    {
    case EMarkState_DESTROY:
        return tr2("完成摧毁");
    case EMarkState_PRESSING:
        return tr2("完成压制");
    case EMarkState_ATTACK:
        return tr2("正在攻击");
    case EMarkState_UNDESTRUCTED:
        return tr2("未完成摧毁");
    case EMarkState_UNSUPPRESSED:
        return tr2("未完成压制");
    case EMarkState_Damage:
        return tr2("目标损伤");
    case EMarkState_Disapper:
        return tr2("目标消失");
    default:
        return tr2("完成摧毁");
    }
    return tr2("完成摧毁");
}

void CDlgMissionList::on_Menu_Fail()
{
    int nHitrow = -1;
    int nTabIndex = ui->tabWidget->currentIndex();
    QWidget *p1 = ui->tabWidget->widget(nTabIndex);
    if (p1->layout()->count() > 0)
    {
        QLayoutItem *it = p1->layout()->itemAt(0);
        QTableWidget *pTabCtrl1 = qobject_cast<QTableWidget *>(it->widget());
        nHitrow = pTabCtrl1->currentRow();

        if (nHitrow == -1)
        {
            LyMessageBox::information(NULL, tr("Tip"), tr("Please select the task that needs to be edited！"));
            return;
        }

        QTableWidgetItem *pItem = pTabCtrl1->item(nHitrow, 1);
        QString strID = pItem->text();

        for (auto data : m_MissionVec)
        {
            if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
            {
                data.SetState(data.GetStateENum(7));
                m_pITaskManageService->SetMission(data);
                break;
            }
        }
        QTableWidgetItem *pItem1 = pTabCtrl1->item(nHitrow, 1);
        CNeubulaOperationalInformation temp1;
        std::string TaskID1 = pItem1->text().toStdString();
        temp1.SetTaskID(TaskID1);
        temp1.SetInformationType(1);
        temp1.SetRemarks(tr2("任务:修改任务状态为任务失败").toStdString());
        m_pITaskManageService->DeleteOperationalInformation(1, pItem1->text().toStdString());
        m_pITaskManageService->AddOperationalInformation(temp1);

        pItem1 = pTabCtrl1->item(nHitrow, 6);
        QString str1MarkID = pItem1->text();
        QStringList strList = str1MarkID.split(";");
        for (auto data : strList)
        {
            if (!data.isEmpty())
            {
                CNeubulaOperationalInformation temp2;
                temp2.SetTaskID(TaskID1);
                temp2.SetInformationType(2);
                temp2.SetResourceID(data.toStdString());
                SystemMarkInfo targetInfo = m_pIMarkManageService->GetMark(data.toStdString());
                QString strMarkState = GetMarkState(targetInfo.GetStats());
                temp2.SetRemarks(strMarkState.toStdString());
                m_pITaskManageService->DeleteOperationalInformation(2, TaskID1, data.toStdString());
                m_pITaskManageService->AddOperationalInformation(temp2);
            }
        }
    }
    on_Menu_Update();
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    std::string str = "1";
    props.put("UpdatePlatAmmoList", str);
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgPlatAmmoID", pBuf, nBufLen, props));
}
