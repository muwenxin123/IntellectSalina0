#include "CDlgMissionLineList.h"
#include "ui_CDlgMissionLineList.h"
#include "ITaskManagerActivator.h"
#include "QCString.h"
#include "lY_TaskManage.h"
#include "CreateShape.h"
#include "GLES3\gl3.h"

CDlgMissionLineList::CDlgMissionLineList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgMissionLineList)
{
    ui->setupUi(this);

    UpdateTrackInfoToListHeading();
    CreateTableMenu();
    TabWidgetConnet();

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_pIWeaponManageService = ITaskManagerActivator::Get().getPWeaponManageService();
    if (m_pIWeaponManageService == nullptr)
    {
        return;
    }

    UpdataWindowData();
    UpdateListData();

    localWeaponNameBindToId();

    connect(ui->tableWidget, &QTableWidget::doubleClicked, this, &CDlgMissionLineList::doubleClicked_line);
}

CDlgMissionLineList::~CDlgMissionLineList()
{
    delete ui;
}

void CDlgMissionLineList::TabWidgetConnet()
{
    connect(ui->tableWidget, SIGNAL(customContextMenuRequested(const QPoint)), this, SLOT(customContextMenuRequested(const QPoint)));
    connect(ui->tableWidget, SIGNAL(cellClicked(int, int)), this, SLOT(selectBox(int, int)));

}

void CDlgMissionLineList::CreateTableMenu()
{
    m_TableMenu = new QMenu(this);
    m_MenuAdd = m_TableMenu->addAction(tr("Add"));
    m_MenuEdit = m_TableMenu->addAction(tr("Edit"));
    m_MenuRemove = m_TableMenu->addAction(tr("Delete"));
    m_MenuUpdate = m_TableMenu->addAction(tr("Update"));
    m_MenuHideSelectMissionAll = m_TableMenu->addAction(tr("Hide Belonging Tasks"));
    m_MenuShowSelectMissionAll = m_TableMenu->addAction(tr("Display Belonging Tasks"));
    m_MenuHideAll = m_TableMenu->addAction(tr("Hide All"));
    m_MenuShowAll = m_TableMenu->addAction(tr("Display All"));

    connect(m_MenuAdd, SIGNAL(triggered()), this, SLOT(on_Menu_Add()));
    connect(m_MenuEdit, SIGNAL(triggered()), this, SLOT(on_Menu_Edit()));
    connect(m_MenuRemove, SIGNAL(triggered()), this, SLOT(on_Menu_Remove()));
    connect(m_MenuUpdate, SIGNAL(triggered()), this, SLOT(on_Menu_Update()));
    connect(m_MenuHideSelectMissionAll, SIGNAL(triggered()), this, SLOT(on_Menu_HideSelectMissionAll()));
    connect(m_MenuShowSelectMissionAll, SIGNAL(triggered()), this, SLOT(on_Menu_ShowSelectMissionAll()));
    connect(m_MenuHideAll, SIGNAL(triggered()), this, SLOT(on_Menu_HideAll()));
    connect(m_MenuShowAll, SIGNAL(triggered()), this, SLOT(on_Menu_ShowAll()));
}

void CDlgMissionLineList::UpdateTrackInfoToListHeading()
{
    QStringList headerlabels;
    headerlabels << tr(" No. ") << tr(" Visible and Hidden ") << tr(" Route Number ") << tr(" Route Name ")  << tr(" Task ID ") << tr(" Platform ");
    headerlabels << tr(" Route Type ") << tr(" Protocol Type ") << tr(" Destinations Number ");

    ui->tableWidget->setColumnCount(headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels);

    for (int i = 0; i < headerlabels.size(); i++)
    {
        if (i == 0 || i == 1)
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
        else
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Interactive);
        }
    }

    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
}

void CDlgMissionLineList::UpdataWindowData()
{
    QMutexLocker locker(&m_Mutex);

    m_MissionLineVec.clear();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    m_MissionLineVec = m_pITaskManageService->GetAllMissionLine();

    ui->tableWidget->clear();
    UpdateTrackInfoToListHeading();
    UpdateShowData();
    UpdateListData();
    if (LY_TaskManage::Get().GetDlgTaskLineEdit() != NULL)
    {
        LY_TaskManage::Get().GetDlgTaskLineEdit()->update();
    }
}

void CDlgMissionLineList::UpdateShowData()
{
    m_MissionLineShowVec.clear();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }

    auto m_Missions = m_pITaskManageService->GetActivatingMissions();

    for (int i = 0; i < m_Missions.size(); i++)
    {
        auto newLines = m_pITaskManageService->GetMissionLinesByTask(m_Missions[i].GetID());
        for (int j = 0; j < newLines.size(); j++)
        {
            m_MissionLineShowVec.push_back(newLines[j]);
        }
    }

}

void CDlgMissionLineList::UpdateListData()
{
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(m_MissionLineShowVec.size());

    m_nActionID = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID();
    if (m_MissionLineShowVec.size() == 0)
    {
        return;
    }

    int nNo(0), intMissionLineNo(0);
    QString strMissionIDTemp = "";
    for (auto data : m_MissionLineShowVec)
    {
        QString strNo, strShow, strID, strName, strTask, strPlatform, strType, strProtocol, strPointCount;
        strNo = QString::number(nNo + 1);
        strID = "HX" + QString::number(data.nID);
        strName = tr2(data.strName);
		strShow = tr("Display");
        strTask = tr2(data.strMissionID);

        strType = GetLineTypeStr(data.cType);
        strProtocol = GetLineProtocolStr(data.cProtocolType);
        if (strMissionIDTemp == strTask)
        {
            intMissionLineNo++;
        }
        else
        {
            strMissionIDTemp = strTask;
            intMissionLineNo = 0;
        }
        std::string stdstrPlatform = LineMatchPlatform(data.strMissionID, data.nID, intMissionLineNo);
        strPlatform = GetPlatNameByID(stdstrPlatform);

        if (strTask == "")
        {
            strTask = "-";
        }

        if (strPlatform == "")
        {
            strPlatform = "-";
        }

        if (strType == "")
        {
            strType = "-";
        }

        if (strProtocol == "")
        {
            strProtocol = "-";
        }

        strPointCount = QString::number(data.LinePointVec.size());

        QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
        QTableWidgetItem *item1 = new QTableWidgetItem(strShow);
        if (data.cShowFlag == true)
        {
            item1->setCheckState(Qt::Checked);
        }
        else
        {
            item1->setCheckState(Qt::Unchecked);
        }
        QTableWidgetItem *item2 = new QTableWidgetItem(strID);
        QTableWidgetItem *item3 = new QTableWidgetItem(strName);
        QTableWidgetItem *item4 = new QTableWidgetItem(strTask);
        QTableWidgetItem *item5 = new QTableWidgetItem(strPlatform);
        QTableWidgetItem *item6 = new QTableWidgetItem(strType);
        QTableWidgetItem *item7 = new QTableWidgetItem(strProtocol);
        QTableWidgetItem *item8 = new QTableWidgetItem(strPointCount);

        ui->tableWidget->setItem(nNo, 0, item0);
        ui->tableWidget->setItem(nNo, 1, item1);
        ui->tableWidget->setItem(nNo, 2, item2);
        ui->tableWidget->setItem(nNo, 3, item3);
        ui->tableWidget->setItem(nNo, 4, item4);
        ui->tableWidget->setItem(nNo, 5, item5);
        ui->tableWidget->setItem(nNo, 6, item6);
        ui->tableWidget->setItem(nNo, 7, item7);
        ui->tableWidget->setItem(nNo, 8, item8);

        ui->tableWidget->item(nNo, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        ui->tableWidget->item(nNo, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 5)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 6)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 7)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        ui->tableWidget->item(nNo, 8)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

        nNo++;
    }

	Draw3DLinesWeaponID();
}

bool CDlgMissionLineList::GetShowInfoByLineID(const int nLineID, QString &strTaskID, QString &strPlatformID)
{
    if (m_pITaskManageService == nullptr)
    {
        return false;
    }
    CNebulaAcitonRelationVec RelationVec = m_pITaskManageService->GetActivatingShowData();
    for (auto data : RelationVec)
    {
        for (auto dataTask : data.ReTaskVec)
        {
            for (auto dataPlatform : dataTask.PlatFormVec)
            {
                if (QString::fromStdString(dataPlatform.strLineID).toInt() == nLineID)
                {
                    strTaskID = QString::fromStdString(dataTask.strTaskID);
                    strPlatformID = QString::fromStdString(dataPlatform.strPlatformID);
                    return true;
                }
            }
        }
    }
    return false;
}

QString CDlgMissionLineList::GetLineTypeStr(int nLineType)
{
    QString strF = "";
    switch (nLineType)
    {
    case 1:
        strF = tr("Reconnaissance");
        break;
    case 2:
        strF = tr("Strike");
        break;
    default:
        break;
    }
    return strF;
}

QString CDlgMissionLineList::GetLineProtocolStr(int nLinePro)
{
    QString strF = "";
    switch (nLinePro)
    {
    case 1:
        strF = tr("XY54");
        break;
    case 2:
        strF = tr("XY");
        break;
    default:
        break;
    }
    return strF;
}

CNeubulaMissionLineVec CDlgMissionLineList::GetShowData()
{
    QMutexLocker locker(&m_Mutex);
    return m_MissionLineShowVec;
}

void CDlgMissionLineList::AddLinePoint(const int nLineID, const QString strName, const CNeubulaMissionLinePoint stPoint, const QString MissionID)
{
    QMutexLocker locker(&m_Mutex);
    bool bFind = false;
    for (auto &line : m_MissionLineShowVec)
    {
        if (line.nID == nLineID)
        {
            line.LinePointVec.push_back(stPoint);

            bFind = true;
            break;
        }
    }

    if (bFind == false)
    {
        CNeubulaMissionLine stLine;
        stLine.nID = nLineID;
        strncpy(stLine.strName, strName.toLocal8Bit(), MAX_LINE_LEN_1);
        strncpy(stLine.strMissionID, MissionID.toLocal8Bit(), MAX_LINE_LEN_1);
        stLine.cShowFlag = true;

        stLine.LinePointVec.push_back(stPoint);
        m_MissionLineShowVec.push_back(stLine);
        m_pITaskManageService->AddMissionLine(stLine);
    }
    UpdateListData();
}

void CDlgMissionLineList::insertLinePointN(const int nLineID, const QString strName, const CNeubulaMissionLinePoint stPoint,
                                           const int insertId, const QString MissionID)
{
    QMutexLocker locker(&m_Mutex);
    bool bFind = false;
    for (auto &line : m_MissionLineShowVec)
    {
        if (line.nID == nLineID)
        {

            auto newPointItor = line.LinePointVec.begin() + insertId;
            if (newPointItor >= line.LinePointVec.end())
            {
                line.LinePointVec.push_back(stPoint);
            }
            else
            {
                newPointItor = line.LinePointVec.insert(newPointItor, stPoint);
                newPointItor++;
                while (newPointItor != line.LinePointVec.end())
                {
                    newPointItor->nNo++;
                    newPointItor++;
                }
            }

            bFind = true;
            break;
        }
    }

    if (bFind == false)
    {
        CNeubulaMissionLine stLine;
        stLine.nID = nLineID;
        strncpy(stLine.strName, strName.toLocal8Bit(), MAX_LINE_LEN_1);
        strncpy(stLine.strMissionID, MissionID.toLocal8Bit(), MAX_LINE_LEN_1);
        stLine.cShowFlag = true;

        stLine.LinePointVec.push_back(stPoint);
        m_MissionLineShowVec.push_back(stLine);
        m_pITaskManageService->AddMissionLine(stLine);
    }
    UpdateListData();
}

void CDlgMissionLineList::RemoveLinePoint(const int nLineID, const int nPoint)
{
    QMutexLocker locker(&m_Mutex);

    for (auto &line : m_MissionLineShowVec)
    {

        if (line.nID == nLineID)
        {
            if (nPoint >= 0 && nPoint < line.LinePointVec.size())
            {
                CNeubulaMissionLinePointVec::iterator pointItor = line.LinePointVec.begin() + nPoint;
                if (pointItor != line.LinePointVec.end())
                {
                    pointItor = line.LinePointVec.erase(pointItor);

                }
                while (pointItor != line.LinePointVec.end())
                {
                    pointItor->nNo--;
                    pointItor++;
                }
            }
        }
    }
    UpdateListData();
}

void CDlgMissionLineList::updateLineName(const int nLineID, const QString strName)
{
    bool isChanged = false;
    for (int i = 0; i < m_MissionLineShowVec.size(); i++)
    {
        if (nLineID == m_MissionLineShowVec[i].nID)
        {
            strcpy(m_MissionLineShowVec[i].strName, strName.toLocal8Bit().data());
            isChanged = true;
            break;
        }
    }
    if (isChanged)
    {
        UpdateListData();
    }
}

void CDlgMissionLineList::ClearLinePoints(const int nLineID)
{
    for (auto &line : m_MissionLineShowVec)
    {

        if (line.nID == nLineID)
        {
            line.LinePointVec.clear();
        }
    }
    UpdateListData();
}

bool CDlgMissionLineList::UpdateLinePointMouseMove(int nLineID, int nPointID, double dLon, double dLat)
{
    QMutexLocker locker(&m_Mutex);

    for (auto &line : m_MissionLineShowVec)
    {
        if (line.nID == nLineID)
        {
            for (auto &point : line.LinePointVec)
            {
                if (nPointID == point.nNo)
                {
                    point.dLon = dLon;
                    point.dLat = dLat;

                    break;
                }
            }
            break;
        }
    }

    return false;
}

void CDlgMissionLineList::Update(qnzkna::TaskManage::INebulaTaskManageService *sub)
{
    UpdataWindowData();

    if (LY_TaskManage::Get().GetDlgEditLine() != nullptr)
    {
        LY_TaskManage::Get().GetDlgEditLine()->UpdateDataFromActivatingActionIDChange();
    }
}

void CDlgMissionLineList::showEvent(QShowEvent *event)
{
    UpdataWindowData();

}

void CDlgMissionLineList::on_Menu_Add()
{
    if (LY_TaskManage::Get().GetDlgTaskLineEdit() == nullptr)
    {
        LY_TaskManage::Get().NewDlgTaskLineEdit();
    }
    else
    {
        LY_TaskManage::Get().GetDlgTaskLineEdit()->update();
    }
    if (LY_TaskManage::Get().GetDlgTaskLineEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgTaskLineEdit()->hide();
    }
    else
    {
        CNeubulaMissionLine stLine;
        int taskID = LY_TaskManage::Get().GetDlgTaskLineEdit()->getActionID();

        int activeActionId = m_pITaskManageService->GetActivatingActionID();

        if (taskID != activeActionId)
        {
            LY_TaskManage::Get().GetDlgTaskLineEdit()->update();
        }

        LY_TaskManage::Get().GetDlgTaskLineEdit()->setParent(this);
        LY_TaskManage::Get().GetDlgTaskLineEdit()->setWindowFlags(LY_TaskManage::Get().GetDlgTaskLineEdit()->windowFlags() | Qt::Dialog);
        LY_TaskManage::Get().GetDlgTaskLineEdit()->show();
        LY_TaskManage::Get().GetDlgTaskLineEdit()->raise();
    }
}

void CDlgMissionLineList::on_Menu_Edit()
{
    if (LY_TaskManage::Get().GetDlgTaskLineEdit() == nullptr)
    {
        LY_TaskManage::Get().NewDlgTaskLineEdit();
    }

    if (LY_TaskManage::Get().GetDlgTaskLineEdit()->isVisible())
    {
        LY_TaskManage::Get().GetDlgTaskLineEdit()->hide();
    }
    else
    {
        int taskID = LY_TaskManage::Get().GetDlgTaskLineEdit()->getActionID();
        int activeActionId = m_pITaskManageService->GetActivatingActionID();

        if (taskID != activeActionId)
        {
            LY_TaskManage::Get().GetDlgTaskLineEdit()->update();
        }
        CNeubulaMissionLine *stLine = NULL;
        int selectedRow = ui->tableWidget->currentRow();
        if (selectedRow >= 0 && selectedRow < m_MissionLineShowVec.size())
        {
            stLine = &m_MissionLineShowVec[selectedRow];

            LY_TaskManage::Get().GetDlgTaskLineEdit()->initMissionData();
            LY_TaskManage::Get().GetDlgTaskLineEdit()->setComBoxCurText(tr2(stLine->strMissionID));
            LY_TaskManage::Get().GetDlgTaskLineEdit()->setTabWidgetCurLine(stLine->nID);
        }

        LY_TaskManage::Get().GetDlgTaskLineEdit()->setParent(this);
        LY_TaskManage::Get().GetDlgTaskLineEdit()->setWindowFlags(LY_TaskManage::Get().GetDlgTaskLineEdit()->windowFlags() | Qt::Dialog);
        LY_TaskManage::Get().GetDlgTaskLineEdit()->show();
        LY_TaskManage::Get().GetDlgTaskLineEdit()->raise();
    }
}

void CDlgMissionLineList::on_Menu_Remove()
{
    int nCul = ui->tableWidget->currentRow();
    if (nCul == -1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Please select the route you want to delete!"));
        return;
    }

    if (nCul > m_MissionLineShowVec.size())
    {
        return;
    }

    QString strTmp = tr("Whether to delete the route:") + QString::number(nCul + 1);
    if (QMessageBox::StandardButton::Yes != LyMessageBox::question(this, tr("tip"), strTmp, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel))
    {
        return;
    }
    CNeubulaMissionLine *stLine = NULL;
    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0 && selectedRow < m_MissionLineShowVec.size())
    {
        stLine = &m_MissionLineShowVec[selectedRow];

        if (LY_TaskManage::Get().GetDlgTaskLineEdit() != nullptr)
        {
            if (LY_TaskManage::Get().GetDlgTaskLineEdit()->getComBoxCurText() == tr2(stLine->strMissionID))
            {
                WidgetLineEdit *deledLine = LY_TaskManage::Get().GetDlgTaskLineEdit()->getTabWidgetLine(stLine->nID);
                if (deledLine)
                {
                    deledLine->clearPoints();
                }
            }
        }
    }
    auto itr = m_MissionLineShowVec.begin();
    itr += nCul;

    m_pITaskManageService->RemoveMissionLine(itr->nID);

    int nActionID = m_pITaskManageService->GetActivatingActionID();
    m_pITaskManageService->RmoveRelationTaskLine(nActionID, "", QString::number(itr->nID).toStdString(), "");

    UpdataWindowData();

}

void CDlgMissionLineList::on_Menu_Update()
{
    UpdataWindowData();
    if (LY_TaskManage::Get().GetDlgTaskLineEdit() == NULL)
    {
        LY_TaskManage::Get().NewDlgTaskLineEdit();
    }
    if (LY_TaskManage::Get().GetDlgTaskLineEdit() != NULL)
    {
        LY_TaskManage::Get().GetDlgTaskLineEdit()->update();
    }

}

void CDlgMissionLineList::on_Menu_HideSelectMissionAll()
{
    if (ui == nullptr || ui->tableWidget == nullptr || m_pITaskManageService == nullptr)
    {
        return ;
    }

    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0 && selectedRow < ui->tableWidget->rowCount() && selectedRow < m_MissionLineShowVec.size())
    {
        const QString &strMissionID = ui->tableWidget->item(selectedRow, 4)->text();
        auto itr = m_MissionLineShowVec.begin();
        for (; itr != m_MissionLineShowVec.end(); itr++)
        {
            if (strMissionID == tr2(itr->strMissionID))
            {
                itr->cShowFlag = false;
                m_pITaskManageService->SetMissionLiseShowStates(itr->nID, false);
            }
        }

        for (int r = 0; r < ui->tableWidget->rowCount(); r++)
        {
            if (r >= 0 && r < m_MissionLineShowVec.size())
            {
                if (strMissionID == ui->tableWidget->item(r, 4)->text())
                {
                    ui->tableWidget->item(r, 1)->setCheckState(Qt::Unchecked);
                }
            }
        }
    }
}

void CDlgMissionLineList::on_Menu_ShowSelectMissionAll()
{
    if (ui == nullptr || ui->tableWidget == nullptr || m_pITaskManageService == nullptr)
    {
        return ;
    }

    int selectedRow = ui->tableWidget->currentRow();
    if (selectedRow >= 0 && selectedRow < ui->tableWidget->rowCount() && selectedRow < m_MissionLineShowVec.size())
    {
        const QString &strMissionID = ui->tableWidget->item(selectedRow, 4)->text();
        auto itr = m_MissionLineShowVec.begin();
        for (; itr != m_MissionLineShowVec.end(); itr++)
        {
            if (strMissionID == tr2(itr->strMissionID))
            {
                itr->cShowFlag = true;
                m_pITaskManageService->SetMissionLiseShowStates(itr->nID, true);
            }
        }

        for (int r = 0; r < ui->tableWidget->rowCount(); r++)
        {
            if (r >= 0 && r < m_MissionLineShowVec.size())
            {
                if (strMissionID == ui->tableWidget->item(r, 4)->text())
                {
                    ui->tableWidget->item(r, 1)->setCheckState(Qt::Checked);
                }
            }
        }
    }
}

void CDlgMissionLineList::on_Menu_HideAll()
{
    auto itr = m_MissionLineShowVec.begin();
    for (; itr != m_MissionLineShowVec.end(); itr++)
    {
        itr->cShowFlag = false;
        m_pITaskManageService->SetMissionLiseShowStates(itr->nID, false);
    }

    for (int r = 0; r < ui->tableWidget->rowCount(); r++)
    {
        ui->tableWidget->item(r, 1)->setCheckState(Qt::Unchecked);
    }
}

void CDlgMissionLineList::on_Menu_ShowAll()
{
    auto itr = m_MissionLineShowVec.begin();
    for (; itr != m_MissionLineShowVec.end(); itr++)
    {
        itr->cShowFlag = true;
        m_pITaskManageService->SetMissionLiseShowStates(itr->nID, true);
    }

    for (int r = 0; r < ui->tableWidget->rowCount(); r++)
    {
        ui->tableWidget->item(r, 1)->setCheckState(Qt::Checked);
    }
}

void CDlgMissionLineList::customContextMenuRequested(const QPoint &pos)
{
    m_TableMenu->exec(QCursor::pos());
}

void CDlgMissionLineList::selectBox(int r, int c)
{
	Clear3DlinesWeaponID();
    if (c != 1)
    {
        return;
    }

    auto itr = m_MissionLineShowVec.begin();
    int nNo = 0;
    if (r > m_MissionLineShowVec.size())
    {
        return;
    }
    itr += r;

    if (itr->cShowFlag == true)
    {
        ui->tableWidget->item(r, 1)->setCheckState(Qt::Unchecked);
    }
    else
    {
        ui->tableWidget->item(r, 1)->setCheckState(Qt::Checked);
    }
    if (Qt::Checked == ui->tableWidget->item(r, 1)->checkState())
    {
        if (itr->cShowFlag != true)
        {
            itr->cShowFlag = true;
            m_pITaskManageService->SetMissionLiseShowStates(itr->nID, true);
		}

    }
    else
    {
        if (itr->cShowFlag != false)
        {
            itr->cShowFlag = false;
            m_pITaskManageService->SetMissionLiseShowStates(itr->nID, false);
        }
    }

	Draw3DLinesWeaponID();
}

void CDlgMissionLineList::doubleClicked_line()
{
    auto selectedItems = ui->tableWidget->selectedItems();
    if (!selectedItems.empty())
    {
        int selectedRow = selectedItems.at(0)->row();
        if (m_MissionLineShowVec[selectedRow].LinePointVec.size() > 0)
        {
            LY_TaskManage::Get().SetScreenCenter(m_MissionLineShowVec[selectedRow].LinePointVec[0].dLon, m_MissionLineShowVec[selectedRow].LinePointVec[0].dLat);
        }
    }

}

void CDlgMissionLineList::tablewidget_cellClicked()
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

std::string CDlgMissionLineList::LineMatchPlatform(const std::string &strMissionID, const int &nLineID, const int &nNo)
{
    std::string strPlatID = "";
    CNeubulaMissionLineVec LineVec = m_pITaskManageService->GetMissionLinesByTask(strMissionID);

    if (LineVec.size() == 0)
    {
        return strPlatID;
    }

    std::vector<std::string> reStrs;
    reStrs = m_pITaskManageService->readPlatFormMatch(strMissionID);

    if (reStrs.size() == 0)
    {
        return strPlatID;
    }

    auto itrPlat = reStrs.begin();
    auto itrPlat2 = reStrs.begin();
    for (auto line : LineVec)
    {
        if (line.nID != nLineID)
        {
            continue;
        }

        WndRouteLineInfo *pPoint = new WndRouteLineInfo;
        pPoint->bFormState = false;

        if (nNo < reStrs.size())
        {

            itrPlat2 = itrPlat + nNo;
        }
        else
        {
            continue;
        }

        return *itrPlat2;
    }
    return strPlatID;
}

void CDlgMissionLineList::localWeaponNameBindToId()
{
    m_SystemSysNameAndID.clear();
    qnzkna::WeaponManage::WeaponSystemMap weaponSystemMap;
    m_pIWeaponManageService->GetWeaponSystemMap(weaponSystemMap);
    for (auto sysItr : weaponSystemMap)
    {
        if (sysItr.second.getSystemType() == qnzkna::WeaponManage::WeaponSystem_Type::WeaponSystem_Type_UAV)
        {
            auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
            for (auto weapon : *weaponComPonentMap)
            {
                m_SystemSysNameAndID.insert(std::make_pair(weapon.second.getComponentID(), weapon.second.getComponentName()));
            }
        }
    }
}

QString CDlgMissionLineList::GetPlatNameByID(const std::string strPlatID)
{
    QString  strName = "";
    auto LineIte = m_SystemSysNameAndID.find(strPlatID);
    if (LineIte != m_SystemSysNameAndID.end())
    {
        strName = QString::fromStdString(LineIte->second);
    }
    return strName;
}

void CDlgMissionLineList::updateMenu()
{
    on_Menu_Update();
}

void CDlgMissionLineList::Draw3DLinesWeaponID()
{

	IEarth *pEarth = IEarth::GetEarth();
	if (pEarth == nullptr || pEarth->GetGraphics() == nullptr) {
		return;
	}

	for (int index = 0; index < Path3DLines.size(); index++)
	{
		pEarth->GetGraphics()->RemoveObject(Path3DLines[index]);
	}
	Path3DLines.clear();

	QColor colorBombTrack(Qt::yellow);
	colorBombTrack.setAlpha(255);
	QColor colorTemp;
	colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());

	IUserMesh1      *pUM = nullptr;

	short elevation = 0;
	unsigned int PointIndex = 0;
	for (int j =0 ;j<m_MissionLineShowVec.size();j++)
	{
		if (ui->tableWidget->item(j, 1)->checkState())
		{
			std::vector<double> pdLon;
			std::vector<double> pdLat;
			std::vector<double> pdHeight;
			for (int k = 0; k < m_MissionLineShowVec[j].LinePointVec.size(); k++)
			{
				pdLon.push_back(m_MissionLineShowVec[j].LinePointVec[k].dLon);
				pdLat.push_back(m_MissionLineShowVec[j].LinePointVec[k].dLat);
				pdHeight.push_back(m_MissionLineShowVec[j].LinePointVec[k].dAlt);

				double pdLon1[2] = { pdLon[k], pdLon[k] };
				double pdLat1[2] = { pdLat[k], pdLat[k] };
				double pdHeight1[2] = { pdHeight[k] * 1.0,0 };

				IUserMesh1	*pUM1 = nullptr;
				CCreateShape cs1;
				SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
				if (pShp1 != nullptr)
				{
					pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
					if (pUM1 != nullptr)
					{
						pUM1->SetColor(colorTemp.rgba());
						pUM1->SetState(IUserMesh::LINESTIPPLE, 4);
						pUM1->SetState(IUserMesh::LINEWIDTH, 1);
						pUM1->SetState(GL_DEPTH_TEST, 1);
						Path3DLines.push_back(pUM1);
					}
				}
			}

			CCreateShape cs;
			SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), pdHeight.size(), 100, false, nullptr, pdHeight.data());
			if (pShp != nullptr)
			{
				pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
				if (pUM != nullptr)
				{
					pUM->SetColor(colorTemp.rgba());
					pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
					pUM->SetState(IUserMesh::LINEWIDTH, 2);
					Path3DLines.push_back(pUM);
				}
			}
		}
	}
}

void CDlgMissionLineList::Clear3DlinesWeaponID()
{
	IEarth *pEarth = IEarth::GetEarth();

	return;
}
