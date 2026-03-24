#include "CDlgActionList.h"
#include "ui_CDlgActionList.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include "LyMessageBox.h"
#include "CDlgMessageBox.h"

CDlgActionList::CDlgActionList(QWidget *parent) :
    LyDialog(tr("Action Setup"), parent),
    ui(new Ui::CDlgActionList)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1100, 700);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    connect(ui->pushButton_2, &QPushButton::clicked, this, &CDlgActionList::on_pbtnCancel_clicked);

    connect(ui->btnAdd, &QPushButton::clicked, this, &CDlgActionList::on_Menu_Add);
    connect(ui->btnEdit, &QPushButton::clicked, this, &CDlgActionList::on_btnEdit_clicked);
    connect(ui->btnDele, &QPushButton::clicked, this, &CDlgActionList::on_btnDele_clicked);
    connect(ui->btnUpdata, &QPushButton::clicked, this, &CDlgActionList::on_Menu_Update);

    CreateTableMenu();
    TabWidgetConnet();
    CreateListHeading();
    UpdataWindowData();
    UpdateListData();

    m_Timer = new QTimer(this);
    connect(m_Timer, &QTimer::timeout, this, &CDlgActionList::timerUpdateAction);
    m_Timer->start(3000);
}

CDlgActionList::~CDlgActionList()
{
    m_Timer->stop();
    delete ui;
}

void CDlgActionList::TabWidgetConnet()
{
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(selectBox(int, int)));
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(customContextMenuRequested(const QPoint)));
    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tbw_cellDoubleClicked(int, int)));
}

void CDlgActionList::CreateTableMenu()
{
    m_TableMenu = new QMenu(this);
    m_MenuAdd = m_TableMenu->addAction(tr("Add"));
    m_MenuEdit = m_TableMenu->addAction(tr("Edit"));
    m_MenuRemove = m_TableMenu->addAction(tr("Delete"));
    m_MenuUpdate = m_TableMenu->addAction(tr("Update"));

    connect(m_MenuAdd, SIGNAL(triggered()), this, SLOT(on_Menu_Add()));
    connect(m_MenuEdit, SIGNAL(triggered()), this, SLOT(on_Menu_Edit()));
    connect(m_MenuRemove, SIGNAL(triggered()), this, SLOT(on_Menu_Remove()));
    connect(m_MenuUpdate, SIGNAL(triggered()), this, SLOT(on_Menu_Update()));
}

void CDlgActionList::CreateListHeading()
{
    QStringList headerlabels;
    headerlabels << tr("No.") << tr("Activate") << tr("Action Number") << tr("Action Name") << tr("Action Code") << tr("Action Purpose");
    headerlabels << tr("Action Area") << tr("Action Objective") << tr("Resource Information") << tr("Environmental Information") << tr("Start Time") << tr("End Time") << tr("Remark");

    ui->tableWidget->setColumnCount(headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
    for (int i = 0; i < headerlabels.size(); i++)
    {
        if (i == 11 || i == 7)
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
        }
        else
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
    }

    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void CDlgActionList::UpdataWindowData()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    m_ActionVec.clear();
    m_ActionVec = m_pITaskManageService->GetAllNebulaAction();
}

void CDlgActionList::UpdateListData()
{
    if (m_ActionVec.size() == 0)
    {
        return;
    }

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(m_ActionVec.size());

    int nNo = 0;
    for (auto data : m_ActionVec)
    {
        QString strNo, strID, strName, strCode, strObject, strAreaID, strInfo, strStartTime, strFinishTime, strRemark, strIDList, strResource;
        strNo = QString::number(nNo + 1);
        strID = QString::number(data.GetID());
        strName = QString::fromStdString(data.GetName());
        strCode = QString::fromStdString(data.GetCode());
        strStartTime = QString::fromStdString(data.GetStartTime());
        strFinishTime = QString::fromStdString(data.GetFinishTime());
        strRemark = QString::fromStdString(data.GetRemark());
        strObject = QString::fromStdString(data.GetObject());
        strAreaID = QString::fromStdString(data.GetAreaID());
        strInfo = QString::fromStdString(data.GetInfo());
        for (auto itr : data.GetMarkIDList())
        {
            strIDList += QString::fromStdString(itr)  + ";";
        }
        if (data.GetPlatInfoVec().empty() && data.GetAmmoInfoVec().empty() && data.GetSensorInfoVec().empty())
        {
            strResource = "";
        }
        else
        {
            strResource = tr("(");
            for (auto plat_pair : data.GetPlatInfoVec())
            {
                strResource += QString::fromStdString(plat_pair.first);
                strResource += tr(":");
                strResource += QString::number(plat_pair.second);
                strResource += tr(",");
            }
            strResource += tr(")");

            strResource += tr("(");
            for (auto ammo_pair : data.GetAmmoInfoVec())
            {
                strResource += QString::fromStdString(ammo_pair.first);
                strResource += tr(":");
                strResource += QString::number(ammo_pair.second);
                strResource += tr(",");
            }
            strResource += tr(")");

            strResource += tr("(");
            for (auto sensor_pair : data.GetSensorInfoVec())
            {
                strResource += QString::fromStdString(sensor_pair.first);
                strResource += tr(":");
                strResource += QString::number(sensor_pair.second);
                strResource += tr(",");
            }
            strResource += tr(")");
        }

        QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
        QTableWidgetItem *item1 = new QTableWidgetItem();
        if (data.GetActivateStates() == true)
        {
            item1->setCheckState(Qt::Checked);
            int mid = data.GetID();
            activeAction = data;
            LY_TaskManage::Get().setTroopName(QString::fromStdString(data.GetName()));
        }
        else
        {
            item1->setCheckState(Qt::Unchecked);
        }
        QTableWidgetItem *item2 = new QTableWidgetItem(strID);
        QTableWidgetItem *item3 = new QTableWidgetItem(strName);
        QTableWidgetItem *item4 = new QTableWidgetItem(strCode);
        QTableWidgetItem *item5 = new QTableWidgetItem(strStartTime);
        QTableWidgetItem *item6 = new QTableWidgetItem(strFinishTime);
        QTableWidgetItem *item7 = new QTableWidgetItem(strRemark);
        QTableWidgetItem *item8 = new QTableWidgetItem(strObject);
        QTableWidgetItem *item9 = new QTableWidgetItem(strAreaID);
        QTableWidgetItem *item10 = new QTableWidgetItem(strInfo);
        QTableWidgetItem *item11 = new QTableWidgetItem(strIDList);

        QTableWidgetItem *item12 = new QTableWidgetItem(strResource);

        ui->tableWidget->setItem(nNo, 0, item0);
        ui->tableWidget->setItem(nNo, 1, item1);
        ui->tableWidget->setItem(nNo, 2, item2);
        ui->tableWidget->setItem(nNo, 3, item3);
        ui->tableWidget->setItem(nNo, 4, item4);
        ui->tableWidget->setItem(nNo, 10, item5);
        ui->tableWidget->setItem(nNo, 11, item6);
        ui->tableWidget->setItem(nNo, 12, item7);
        ui->tableWidget->setItem(nNo, 5, item8);
        ui->tableWidget->setItem(nNo, 6, item9);
        ui->tableWidget->setItem(nNo, 9, item10);
        ui->tableWidget->setItem(nNo, 7, item11);
        ui->tableWidget->setItem(nNo, 8, item12);

        ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 8)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 9)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 10)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 11)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        nNo++;
    }
}

CNeubulaActionVec CDlgActionList::GetActionVec()
{
    return m_ActionVec;
}

void CDlgActionList::SetActive(int r, int c)
{
    selectBox(r, c);
}

void CDlgActionList::showEvent(QShowEvent *e)
{

}

void CDlgActionList::on_pbtnCancel_clicked()
{
    close();
}

void CDlgActionList::on_Menu_Add()
{
    if (LY_TaskManage::Get().GetCDlgActionEdit() != nullptr)
    {
        if (LY_TaskManage::Get().GetCDlgActionEdit()->isVisible())
        {
            LY_TaskManage::Get().GetCDlgActionEdit()->close();
        }
        LY_TaskManage::Get().GetCDlgActionEdit()->Init();
        LY_TaskManage::Get().GetCDlgActionEdit()->isAddorEx = 0;
        LY_TaskManage::Get().GetCDlgActionEdit()->exec();
    }
}

void CDlgActionList::on_Menu_Edit()
{
    if (activeAction.GetID() != 0)
    {
        int row = ui->tableWidget->currentRow();
        std::string strID = tr3(ui->tableWidget->item(row, 2)->text());
        int ActiveID = atoi(strID.c_str());

        if (activeAction.GetID() != ActiveID)
        {
            LyMessageBox::information(this, tr("tip"), tr("Please activate the action before modifying!"));
            return;
        }
        if (LY_TaskManage::Get().GetCDlgActionEdit() != nullptr)
        {
            LY_TaskManage::Get().GetCDlgActionEdit()->Init();
            LY_TaskManage::Get().GetCDlgActionEdit()->UpdateCurrAction(activeAction);
            LY_TaskManage::Get().GetCDlgActionEdit()->isAddorEx = 1;
            LY_TaskManage::Get().GetCDlgActionEdit()->exec();
        }
    }
}

void CDlgActionList::on_Menu_Remove()
{
    int ActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
    QString strTmp = tr("Determine whether to delete the current action : %1").arg(to_string(ActionID).c_str());
    if (QMessageBox::StandardButton::Yes != LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
        return;
    }
    ITaskManagerActivator::Get().getPTaskManageService()->RmoveNebulaAction(ActionID);
    if (m_pITaskManageService != nullptr)
    {
        UpdataWindowData();
        UpdateListData();
    }

}

void CDlgActionList::on_Menu_Update()
{
    if (m_pITaskManageService != nullptr)
    {
        UpdataWindowData();
        UpdateListData();
        int ActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
        m_pITaskManageService->SetActivatingActionID(ActionID);
    }

}
void CDlgActionList::on_btnEdit_clicked()
{
    if (activeAction.GetID() != 0)
    {
        if (LY_TaskManage::Get().GetCDlgActionEdit() != nullptr)
        {
            LY_TaskManage::Get().GetCDlgActionEdit()->Init();
            LY_TaskManage::Get().GetCDlgActionEdit()->UpdateCurrAction(activeAction);
            LY_TaskManage::Get().GetCDlgActionEdit()->isAddorEx = 1;
            LY_TaskManage::Get().GetCDlgActionEdit()->exec();
        }
    }
}
void CDlgActionList::on_btnDele_clicked()
{
    int ActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
    QString strTmp = tr("Determine whether to delete the current action : %1").arg(to_string(ActionID).c_str());
    if (QMessageBox::StandardButton::Yes != LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
        return;
    }
    ITaskManagerActivator::Get().getPTaskManageService()->RmoveNebulaAction(ActionID);
    if (m_pITaskManageService != nullptr)
    {
        UpdataWindowData();
        UpdateListData();
    }
}
CDlgMessageBox *CDlgMessageBox::single = nullptr;
void CDlgActionList::timerUpdateAction()
{

	if (ITaskManagerActivator::Get().getPTaskManageService() == nullptr)
	{
		return;
	}
	int ActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionIDFormDB();
	if (ActionID != -1 && ActionID != activeAction.GetID())
	{
		on_Menu_Update();
		CDlgMessageBox* s1 = CDlgMessageBox::GetInit();
		s1->show();

	}
}

void CDlgActionList::customContextMenuRequested(const QPoint &pos)
{
    m_TableMenu->exec(QCursor::pos());
}

void CDlgActionList::tbw_cellDoubleClicked(int r, int c)
{
    SetActive(r, 1);
    on_Menu_Edit();
}

void CDlgActionList::selectBox(int r, int c)
{
    if (c != 1)
    {
        return;
    }

    auto itr = m_ActionVec.begin();
    int nNo = 0;

    for (; itr != m_ActionVec.end(); itr++)
    {
        if (r == nNo)
        {
            itr->SetActivateStates(true);
            if (m_pITaskManageService != nullptr)
            {
                int nActionID = itr->GetID();
                m_pITaskManageService->SetActivatingActionID(nActionID);
                activeAction = *itr;
                int acID = activeAction.GetID();
                auto tarVec = m_pITaskManageService->GetRelationMark(acID);
                std::vector <std::string> tmpVec;
                for (auto tarID : tarVec)
                {
                    tmpVec.push_back(tarID.GetMarkID());
                }
                activeAction.SetMarkIDList(tmpVec);
                LY_TaskManage::Get().setTroopName(QString::fromStdString(itr->GetName()));
            }
        }
        else
        {
            itr->SetActivateStates(false);
        }
        nNo++;
    }

    ui->tableWidget->disconnect(this);

    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        if (i != r)
        {
            ui->tableWidget->item(i, 1)->setCheckState(Qt::Unchecked);
        }
        else
        {
            ui->tableWidget->item(i, 1)->setCheckState(Qt::Checked);
            ui->tableWidget->selectRow(r);
        }
    }
    LY_TaskManage::Get().GetCDlgActionEdit()->Init();
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(selectBox(int, int)));
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(customContextMenuRequested(const QPoint)));
    connect(ui->tableWidget, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(tbw_cellDoubleClicked(int, int)));
}
