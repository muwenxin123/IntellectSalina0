#include "CDlgCodeSet_Load_Edit.h"
#include "ui_CDlgCodeSet_Load_Edit.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "LyMessageBox.h"
#include "lY_TaskManage.h"
#include <QMenu>

CDlgCodeSet_Load_Edit::CDlgCodeSet_Load_Edit(QWidget *parent) :
    LyDialog(tr("Planning Instruction Set Editing"), parent),
    ui(new Ui::CDlgCodeSet_Load_Edit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(700, 500);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();

    InitData();
    CreateTableMenu();
    TabWidgetConnet();
    CreateListHeading();
}

CDlgCodeSet_Load_Edit::~CDlgCodeSet_Load_Edit()
{
    delete ui;
    pChild = nullptr;
}

void CDlgCodeSet_Load_Edit::InitData()
{
    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_3_currentIndexChanged(int)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_2_currentIndexChanged(int)));

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
}

void CDlgCodeSet_Load_Edit::TabWidgetConnet()
{
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(customContextMenuRequested(const QPoint)));
}

void CDlgCodeSet_Load_Edit::CreateTableMenu()
{
    m_TableMenu = new QMenu(this);
    m_MenuAdd = m_TableMenu->addAction(tr("Add"));
    m_MenuEdit = m_TableMenu->addAction(tr("Edit"));
    m_MenuRemove = m_TableMenu->addAction(tr("Delete"));

    connect(m_MenuAdd, SIGNAL(triggered()), this, SLOT(on_Menu_Add()));
    connect(m_MenuEdit, SIGNAL(triggered()), this, SLOT(on_Menu_Edit()));
    connect(m_MenuRemove, SIGNAL(triggered()), this, SLOT(on_Menu_Remove()));
}

void CDlgCodeSet_Load_Edit::CreateListHeading()
{
    QStringList headerlabels;
    headerlabels << tr(" No. ") << tr(" Execution Sequence ") << tr(" Mount Device ") << tr(" Instruction Number ") << tr(" Instruction Parameter ") << tr(" Remark ");

    ui->tableWidget->setColumnCount(headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
    for (int i = 0; i < headerlabels.size(); i++)
    {
        if (i ==  3)
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
        else
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }

    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void CDlgCodeSet_Load_Edit::UpdataWindowData()
{

}

void CDlgCodeSet_Load_Edit::UpdateListData()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    QVariant  qv = ui->comboBox_3->itemData(ui->comboBox_3->currentIndex());
    QString strTaskID = qv.toString();

    qv = ui->comboBox->itemData(ui->comboBox->currentIndex());
    QString strPlatform = qv.toString();

    qv = ui->comboBox_2->itemData(ui->comboBox_2->currentIndex());
    QString strPointID = qv.toString();

    m_CodeSet_Load_DB_Vec.clear();
    m_CodeSet_Load_DB_Vec = m_pITaskManageService->GetCodeSet_Load(strTaskID.toStdString(), strPlatform.toStdString());

    int nNo = 0;
    for (auto data : m_CodeSet_Load_DB_Vec)
    {
        if (strPointID.toInt() == data.GetPointID())
        {
            ui->tableWidget->setRowCount(nNo + 1);

            QString strNo, strSuit, strLoadName, strCodeName, strCodeParameter, strRemark;
            strNo = QString::number(data.GetID());
            strSuit = QString::number(data.GetSuite());
            CNebulaMissionPlatformLoad stLoad = m_pITaskManageService->GetMissionPlatformLoad(data.GetLoadID());
            strLoadName = tr2(stLoad.strMode);
            qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID ePara = (qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID)data.GetCodeID();
            strCodeName = qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID_To_String(ePara);
            strCodeParameter = QString::fromStdString(data.GetParameter());
            strRemark = QString::fromStdString(data.GetRemark());

            QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
            QTableWidgetItem *item1 = new QTableWidgetItem(strSuit);
            QTableWidgetItem *item2 = new QTableWidgetItem(strLoadName);
            QTableWidgetItem *item3 = new QTableWidgetItem(strCodeName);
            QTableWidgetItem *item4 = new QTableWidgetItem(strCodeParameter);
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

    UpdateSelectedList();
}

void CDlgCodeSet_Load_Edit::UpdateSelectedList()
{
    int nNo = m_CodeSet_Load_DB.GetID();
    if (nNo == 0)
    {
        ui->tableWidget->setCurrentCell(-1, 0);
    }
    else
    {
        for (int i = 0 ; i < ui->tableWidget->rowCount(); i++)
        {
            QTableWidgetItem *pItem = ui->tableWidget->item(i, 0);
            QString strNo = pItem->text();
            if (strNo.compare(QString::number(nNo)) == 0)
            {
                ui->tableWidget->setCurrentCell(i, 0);
            }
        }
    }
}

void CDlgCodeSet_Load_Edit::UpdateTaskCombobox()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    ui->comboBox_3->disconnect();
    ui->comboBox_3->clear();

    CNeubulaMissionVec	m_MissionVec;
    m_MissionVec = m_pITaskManageService->GetAllMission();

    int nNo = 0;
    for (auto data : m_MissionVec)
    {
        ui->comboBox_3->insertItem(nNo, QString::fromStdString(data.GetName()));
        ui->comboBox_3->setItemData(nNo, QString::fromStdString(data.GetID()));
        nNo++;
    }

    CNeubulaMission stMission;
    stMission = m_pITaskManageService->GetMission(m_CodeSet_Load_DB.GetTaskID());
    int nCel = ui->comboBox_3->findText(QString::fromStdString(stMission.GetName()));
    if (nCel != -1)
    {
        ui->comboBox_3->setCurrentIndex(nCel);
    }

    connect(ui->comboBox_3, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_3_currentIndexChanged(int)));
}

void CDlgCodeSet_Load_Edit::UpdatePlatformCombobox()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    ui->comboBox->disconnect();
    ui->comboBox->clear();

    CNebulaMissionPlatformVec	m_MissionVec;
    m_MissionVec = m_pITaskManageService->GetAllMissionPlatform();

    int nNo = 0;
    for (auto data : m_MissionVec)
    {
        ui->comboBox->insertItem(nNo, tr2(data.strMode));
        ui->comboBox->setItemData(nNo, tr2(data.strID));
        nNo++;
    }

    CNebulaMissionPlatform stMission;
    stMission = m_pITaskManageService->GetMissionPlatform(m_CodeSet_Load_DB.GetPlatformID());
    int nCel = ui->comboBox->findText(tr2(stMission.strMode));
    if (nCel != -1)
    {
        ui->comboBox->setCurrentIndex(nCel);
    }

    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_currentIndexChanged(int)));
}

void CDlgCodeSet_Load_Edit::UpdateLineCombobox()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    ui->comboBox_2->disconnect();
    ui->comboBox_2->clear();

    for (int i = 0; i < 10; i++)
    {
        ui->comboBox_2->insertItem(i, QString::number(i + 1));
        ui->comboBox_2->setItemData(i, QString::number(i + 1));
    }

    int nCel = ui->comboBox_2->findText(QString::number(m_CodeSet_Load_DB.GetPointID()));
    if (nCel != -1)
    {
        ui->comboBox_2->setCurrentIndex(nCel);
    }

    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_2_currentIndexChanged(int)));

}

void CDlgCodeSet_Load_Edit::SetShowData(const CCodeSet_Load_DB stCodeSetDB)
{
    m_CodeSet_Load_DB = stCodeSetDB;
}

int CDlgCodeSet_Load_Edit::CreateCodeSetNo()
{
    CCodeSet_Load_DB_Vec wndData = m_pITaskManageService->GetAllCodeSet_Load();
    int nMax = 0;
    for (auto data : wndData)
    {
        if (data.GetID() > nMax)
        {
            nMax = data.GetID();
        }
    }
    return nMax + 1;
}

void CDlgCodeSet_Load_Edit::UpdateCodeSetWidgets()
{

    UpdateTaskCombobox();
    UpdatePlatformCombobox();
    UpdateLineCombobox();
    UpdateListData();

    LY_TaskManage::Get().getDlgCodeSet_Load_Tree()->UpdateWindowData();;
    LY_TaskManage::Get().getDlgCodeSet_Load_Tree()->UpdateTreeData();;
}

void CDlgCodeSet_Load_Edit::showEvent(QShowEvent *event)
{
    UpdateTaskCombobox();
    UpdatePlatformCombobox();
    UpdateLineCombobox();

    UpdateListData();
}

void CDlgCodeSet_Load_Edit::on_Menu_Add()
{
    if (pChild == nullptr)
    {
        pChild = new CDlgCodeSet_Load_Edit_Child(this);
        connect(pChild, &CDlgCodeSet_Load_Edit_Child::PushOkButton, this, &CDlgCodeSet_Load_Edit::EditFinish);
    }

    CCodeSet_Load_DB stLoadDB;
    pChild->SetEditData(stLoadDB);

    if (pChild->isVisible())
    {
        pChild->hide();
    }
    pChild->show();
    pChild->raise();

}

void CDlgCodeSet_Load_Edit::on_Menu_Edit()
{
    if (pChild == nullptr)
    {
        pChild = new CDlgCodeSet_Load_Edit_Child(this);
        connect(pChild, &CDlgCodeSet_Load_Edit_Child::PushOkButton, this, &CDlgCodeSet_Load_Edit::EditFinish);
    }

    CCodeSet_Load_DB stLoadDB;

    int nCulRpw = ui->tableWidget->currentRow();
    if (nCulRpw == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the command you want to edit"));
        return;
    }

    QTableWidgetItem *pItem = ui->tableWidget->item(ui->tableWidget->currentRow(), 0);
    QString str = pItem->text();
    stLoadDB = m_pITaskManageService->GetCodeSet_Load(pItem->text().toInt());
    pChild->SetEditData(stLoadDB);

    if (pChild->isVisible())
    {
        pChild->hide();
    }
    pChild->show();
    pChild->raise();

}

void CDlgCodeSet_Load_Edit::on_Menu_Remove()
{
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    int nCulRpw = ui->tableWidget->currentRow();
    if (nCulRpw == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the command you want to delete"));
        return;
    }

    QTableWidgetItem *pItem = ui->tableWidget->item(ui->tableWidget->currentRow(), 0);
    QString str = pItem->text();

    QString strTmp = tr("Please confirm whether to delete the serial number: ") + str + tr("?");
    if (QMessageBox::StandardButton::Yes == LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
        bool bresut = m_pITaskManageService->RmoveCodeSet_Load(str.toInt());
        if (bresut == true)
        {
        }
        else
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Delete failed"));
            return;
        }
    }
    UpdateCodeSetWidgets();
}

void CDlgCodeSet_Load_Edit::customContextMenuRequested(const QPoint &pos)
{
    m_TableMenu->exec(QCursor::pos());
}

void CDlgCodeSet_Load_Edit::on_comboBox_3_currentIndexChanged(int index)
{
    UpdateListData();
}

void CDlgCodeSet_Load_Edit::on_comboBox_currentIndexChanged(int index)
{
    UpdateListData();
}

void CDlgCodeSet_Load_Edit::on_comboBox_2_currentIndexChanged(int index)
{
    UpdateListData();
}

void CDlgCodeSet_Load_Edit::on_pushButton_clicked()
{
    this->hide();
}

void CDlgCodeSet_Load_Edit::on_pushButton_2_clicked()
{
    this->hide();
}

void CDlgCodeSet_Load_Edit::EditFinish()
{
    if (pChild == nullptr)
    {
        return;
    }

    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    CCodeSet_Load_DB stNew = pChild->GetEditData();

    if (stNew.GetID() == 0)
    {
        stNew.SetID(CreateCodeSetNo());

        stNew.SetActionID(m_pITaskManageService->GetActivatingActionID());
        QVariant  qv = ui->comboBox_3->itemData(ui->comboBox_3->currentIndex());
        QString strTaskID = qv.toString();

        qv = ui->comboBox->itemData(ui->comboBox->currentIndex());
        QString strPlatform = qv.toString();

        qv = ui->comboBox_2->itemData(ui->comboBox_2->currentIndex());
        QString strPointID = qv.toString();

        stNew.SetTaskID(strTaskID.toStdString());
        stNew.SetPlatformID(strPlatform.toStdString());
        stNew.SetPointID(strPointID.toInt());

    }
    m_pITaskManageService->SetCodeSet_Load(stNew);

    UpdateCodeSetWidgets();
}
