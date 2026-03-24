#include "CDlgMissionPlatformList.h"
#include "ui_CDlgMissionPlatformList.h"
#include "ITaskManagerActivator.h"
#include "QCString.h"
#include "lY_TaskManage.h"
#include "LyMessageBox.h"

CDlgMissionPlatformList::CDlgMissionPlatformList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgMissionPlatformList)
{
    ui->setupUi(this);
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
    m_pIWeaponManageService = ITaskManagerActivator::Get().getPWeaponManageService();
    if (m_pIWeaponManageService == nullptr)
    {
        return;
    }
    m_timer = new QTimer(this);
    m_timer->start(1000);
    connect(m_timer, &QTimer::timeout, this, &CDlgMissionPlatformList::on_timeOut);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &CDlgMissionPlatformList::on_tableWidget_customContextMenuRequested);
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &CDlgMissionPlatformList::on_tableWidget_cellDoubleClicked);
    Init();
}

CDlgMissionPlatformList::~CDlgMissionPlatformList()
{
    if (m_timer != nullptr)
    {
        m_timer->stop();
    }
    delete ui;
}

void CDlgMissionPlatformList::Init()
{
    m_NeubulaActionMissionVec = m_pITaskManageService->GetActivatingMissions();
    int rowCount = 0;
    for (auto data : m_NeubulaActionMissionVec)
    {
        std::vector<std::string> newVec;
        newVec = m_pITaskManageService->readPlatFormMatch(data.GetID());
        rowCount += newVec.size();
    }
    QStringList header;
    header << tr("No.") << tr("Platform Number")  << tr("Main Mount Ammunition Number") << tr("Secondary Mount Ammunition Number") << tr("Task Number");
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(rowCount);
    ui->tableWidget->setColumnCount(header.size());
    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tableWidget->setSortingEnabled(false);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    LY_TaskManage::Get().GetDlgMissionTree()->localWeaponNameBindToId();
    SystemSysIDAndName = LY_TaskManage::Get().GetDlgMissionTree()->SystemSysIDAndName;
    CreateTableWidget();
    CreateTableMenu();
}

void CDlgMissionPlatformList::CreateTableWidget()
{
    int row = 0;
    for (auto TaskID : m_NeubulaActionMissionVec)
    {
        std::vector<std::string> PlatNameVec;
        for (auto plattype : TaskID.GetPlatInfoMap())
        {
            PlatNameVec.push_back(plattype.GetResourceName());
        }
        std::set<std::string> sTemp(PlatNameVec.begin(), PlatNameVec.end());
        PlatNameVec.assign(sTemp.begin(), sTemp.end());
        for (auto PlatTypeName : PlatNameVec)
        {
            std::vector<std::string> paltIDVec = m_pITaskManageService->readPlatFormMatch(PlatTypeName, TaskID.GetID());

            for (auto PlatID : paltIDVec)
            {
                auto PlatName = SystemSysIDAndName.find(PlatID);
                if (PlatName == SystemSysIDAndName.end())
                {
                    ui->tableWidget->setRowCount(ui->tableWidget->rowCount() - 1);
                    continue;
                }
                std::string strPlatPriAmmoID = GetPlatPriAmmoID(TaskID.GetID(), PlatID);
                std::string strPlatSecondAmmoID = GetPlatSecondAmmoID(TaskID.GetID(), PlatID);
                ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
                ui->tableWidget->setItem(row, 1, new QTableWidgetItem(tr2(PlatName->second.c_str())));
                ui->tableWidget->setItem(row, 2, new QTableWidgetItem(tr2(strPlatPriAmmoID.c_str())));
                ui->tableWidget->setItem(row, 3, new QTableWidgetItem(tr2(strPlatSecondAmmoID.c_str())));
                ui->tableWidget->setItem(row, 4, new QTableWidgetItem(tr2(TaskID.GetID().c_str())));
                ui->tableWidget->item(row, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                ui->tableWidget->item(row, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                ui->tableWidget->item(row, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                ui->tableWidget->item(row, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                ui->tableWidget->item(row, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
                ++row;
            }
        }
    }
    ui->tableWidget->setRowCount(row);
}

void CDlgMissionPlatformList::CreateTableMenu()
{
    m_MenuRight = new QMenu(this);
    QAction *TaskMenu_Edit = m_MenuRight->addAction(tr("Edit"));
    connect(TaskMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_MarkAmmo_Edit()));
}

std::string CDlgMissionPlatformList::GetPlatPriAmmoID(const std::string TaskID, const std::string PlatID)
{
    CNeubulaConntionAmmoVec TaskConntionAmmoVec = m_pAmmunitionManageService->GetAmmunitionConnByTaskID(TaskID);
    QMap<std::string, std::string> PlatAmmoID;
    for (auto data : TaskConntionAmmoVec)
    {
		const string &strPlatID = data.GetPlatID();
		const string &strType = data.GetType();
		const string &strAmmoID = data.GetAmmoID();
        if (strPlatID == PlatID && data.GetLoadMethod() == 0)
        {
            auto it = PlatAmmoID.find(strType);
            if (it != PlatAmmoID.end())
            {
                PlatAmmoID[strType] += ";" + strAmmoID;
            }
            else
            {
                PlatAmmoID.insert(strType, strAmmoID);
            }
        }
    }
    std::string strPlatAmmoID;
    for (auto it = PlatAmmoID.begin(); it != PlatAmmoID.end(); ++it)
    {
        strPlatAmmoID += it.key() + "(" + it.value() + ");";
    }
    size_t lastSemicolonIndex = strPlatAmmoID.find_last_of(';');
    if (lastSemicolonIndex != std::string::npos)
    {
        strPlatAmmoID.erase(lastSemicolonIndex, 1);
    }
    return strPlatAmmoID;
}

std::string CDlgMissionPlatformList::GetPlatSecondAmmoID(const std::string TaskID, const std::string PlatID)
{
    CNeubulaConntionAmmoVec TaskConntionAmmoVec = m_pAmmunitionManageService->GetAmmunitionConnByTaskID(TaskID);
    QMap<std::string, std::string> PlatAmmoID;
    for (auto data : TaskConntionAmmoVec)
    {
        const string &strPlatID = data.GetPlatID();
        const string &strType = data.GetType();
        const string &strAmmoID = data.GetAmmoID();
        if (strPlatID == PlatID && data.GetLoadMethod() == 1)
        {
            auto it = PlatAmmoID.find(strType);
            if (it != PlatAmmoID.end())
            {
                PlatAmmoID[strType] += ";" + strAmmoID;
            }
            else
            {
                PlatAmmoID.insert(strType, strAmmoID);
            }
        }
    }
    std::string strPlatAmmoID;
    for (auto it = PlatAmmoID.begin(); it != PlatAmmoID.end(); ++it)
    {
        strPlatAmmoID += it.key() + "(" + it.value() + ");";
    }
    size_t lastSemicolonIndex = strPlatAmmoID.find_last_of(';');
    if (lastSemicolonIndex != std::string::npos)
    {
        strPlatAmmoID.erase(lastSemicolonIndex, 1);
    }
    return strPlatAmmoID;
}

void CDlgMissionPlatformList::on_timeOut()
{
    if (b_UpdatePlatAmmo)
    {
        Init();
        b_UpdatePlatAmmo = false;
    }
}

void CDlgMissionPlatformList::on_Menu_MarkAmmo_Edit()
{
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    int row = ui->tableWidget->currentRow();
    if (row < 0)
    {
        return;
    }
    QString strTaskID = ui->tableWidget->item(row, 4)->text();
    props.put("TaskID", strTaskID.toStdString());
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgCDlgMissionPlatformList", pBuf, nBufLen, props));
}

void CDlgMissionPlatformList::on_tableWidget_customContextMenuRequested(const QPoint &pos)
{
    m_MenuRight->exec(QCursor::pos());
}

void CDlgMissionPlatformList::on_tableWidget_cellDoubleClicked()
{
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    int row = ui->tableWidget->currentRow();
    QString strTaskID = ui->tableWidget->item(row, 4)->text();
    props.put("TaskID", strTaskID.toStdString());
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgCDlgMissionPlatformList", pBuf, nBufLen, props));
}
