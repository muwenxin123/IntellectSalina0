#include "CDlgMissionPlatformLoadList.h"
#include "ui_CDlgMissionPlatformLoadList.h"
#include "ITaskManagerActivator.h"
#include "QCString.h"
#include "lY_TaskManage.h"
#include "LyMessageBox.h"

CDlgMissionPlatformLoadList::CDlgMissionPlatformLoadList(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgMissionPlatformLoadList)
{
    ui->setupUi(this);
    this->resize(1400, 800);
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
    m_timer = new QTimer(this);
    m_timer->start(1000);
    connect(m_timer, &QTimer::timeout, this, &CDlgMissionPlatformLoadList::on_timeOut);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &CDlgMissionPlatformLoadList::on_treeWidget_customContextMenuRequested);
    connect(ui->tableWidget, &QTableWidget::customContextMenuRequested, this, &CDlgMissionPlatformLoadList::on_treeWidget_customContextMenuRequested);
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &CDlgMissionPlatformLoadList::on_tableWidget_cellDoubleClicked);
}

CDlgMissionPlatformLoadList::~CDlgMissionPlatformLoadList()
{
    if (m_timer != nullptr)
    {
        m_timer->stop();
    }
    delete ui;
}

void CDlgMissionPlatformLoadList::Init()
{
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

    m_NeubulaActionMissionVec = m_pITaskManageService->GetActivatingMissions();
    m_pAmmunitionManageService->LoadAmmoMarkDB();
    m_pAmmunitionManageService->LoadAmmoDB();
    ui->tabWidget->setCurrentWidget(ui->tab);
    ui->treeWidget->clear();
    QStringList header;
    header << tr("Task Number") << tr("Task Name") << tr("Mark Number") << tr("Mark Name") << tr("Ammunition Type") << tr("Number of Ammunition") << tr("Ammunition Number");
    ui->treeWidget->setHeaderLabels(header);
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);
    header.clear();
    header << tr("No.") << tr("Mark Number") << tr("Mark Name") << tr("Ammunition Number") << tr("Task Number");
    ui->tableWidget->clear();
    int rowCount = 0;
    for (auto TaskInfo : m_NeubulaActionMissionVec)
    {
        auto Mark = TaskInfo.GetMarkIDList();
        rowCount += Mark.size();
    }
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
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    CreateTableWidget();
    CreateTreeWidget();
    CreateTreeeMenu();
}

void CDlgMissionPlatformLoadList::CreateTreeWidget()
{
    if (ITaskManagerActivator::Get().getPIMarkManageService() == nullptr || m_pAmmunitionManageService == nullptr) {
        return;
    }
    for (auto TaskInfo : m_NeubulaActionMissionVec)
    {
        QTreeWidgetItem *root1 = new QTreeWidgetItem(ui->treeWidget);
        root1->setText(0, tr2(TaskInfo.GetID().c_str()));
        root1->setText(1, tr2(TaskInfo.GetName().c_str()));
        auto Mark = TaskInfo.GetMarkIDList();
        for (auto data : Mark)
        {
            QTreeWidgetItem *root2 = new QTreeWidgetItem(root1);
            root2->setText(2, QString::fromStdString(data.c_str()));

            SystemMarkInfo tmpTar = ITaskManagerActivator::Get().getPIMarkManageService()->GetMark(data);
            root2->setText(3, tr2(tmpTar.GetName().c_str()));
            CNeubulaAmmoConntionMarkVec TaskMarkAmmoVec = m_pAmmunitionManageService->GetTaskIDByInfo(TaskInfo.GetID());
            std::vector<std::string> strMarkAmmoVec = m_pAmmunitionManageService->GetMarkIDByAmmoMode(TaskMarkAmmoVec, data);
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
            if (!mAmmoMode.empty())
            {
                for (auto it = mAmmoMode.begin(); it != mAmmoMode.end(); ++it)
                {
                    QTreeWidgetItem *root3 = new QTreeWidgetItem(root2);
                    root3->setText(4, tr2(it.key().c_str()));
                    root3->setText(5, QString::number(it.value()));
                    std::vector<std::string> AmmoID;
                    std::string strAmmoID;
                    for (auto AmmoID : strMarkAmmoVec)
                    {
                        CNeubulaAmmunitionInfo AmmoInfo = m_pAmmunitionManageService->GetAmmoByID(AmmoID);
                        if (AmmoInfo.GetAmmoMode() == it.key())
                        {
                            strAmmoID += AmmoID;
                            strAmmoID += ";";
                        }
                    }
                    root3->setText(6, tr2(strAmmoID.c_str()));
                }
            }
        }
    }
}

void CDlgMissionPlatformLoadList::CreateTableWidget()
{
    if (ITaskManagerActivator::Get().getPIMarkManageService() == nullptr || m_pAmmunitionManageService == nullptr){
        return;
    }
    int Row = 0;
    for (auto TaskInfo : m_NeubulaActionMissionVec)
    {
        auto Mark = TaskInfo.GetMarkIDList();
        for (auto data : Mark)
        {
            SystemMarkInfo tmpTar = ITaskManagerActivator::Get().getPIMarkManageService()->GetMark(data);
            const string &strMarkName = tmpTar.GetName();
            CNeubulaAmmoConntionMarkVec TaskMarkAmmoVec = m_pAmmunitionManageService->GetTaskIDByInfo(TaskInfo.GetID());
            std::vector<std::string> strMarkAmmoVec = m_pAmmunitionManageService->GetMarkIDByAmmoMode(TaskMarkAmmoVec, data);
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
            std::string strAmmoID;
            if (!mAmmoMode.empty())
            {
                for (auto it = mAmmoMode.begin(); it != mAmmoMode.end(); ++it)
                {
                    strAmmoID += it.key() + "(";
                    std::vector<std::string> AmmoID;
                    for (auto AmmoID : strMarkAmmoVec)
                    {
                        CNeubulaAmmunitionInfo AmmoInfo = m_pAmmunitionManageService->GetAmmoByID(AmmoID);
                        if (AmmoInfo.GetAmmoMode() == it.key())
                        {
                            strAmmoID += AmmoID;
                            strAmmoID += ";";
                        }
                    }
                    size_t lastSemicolonIndex = strAmmoID.find_last_of(';');
                    if (lastSemicolonIndex != std::string::npos)
                    {
                        strAmmoID.erase(lastSemicolonIndex, 1);
                    }
                    strAmmoID += ");";
                }
            }
            size_t lastSemicolonIndex = strAmmoID.find_last_of(';');
            if (lastSemicolonIndex != std::string::npos)
            {
                strAmmoID.erase(lastSemicolonIndex, 1);
            }
            ui->tableWidget->setItem(Row, 0, new QTableWidgetItem(QString::number(Row + 1)));
            ui->tableWidget->setItem(Row, 1, new QTableWidgetItem(QString::fromStdString(data)));
            ui->tableWidget->setItem(Row, 2, new QTableWidgetItem(tr2(strMarkName.c_str())));
            ui->tableWidget->setItem(Row, 3, new QTableWidgetItem(tr2(strAmmoID.c_str())));
            ui->tableWidget->setItem(Row, 4, new QTableWidgetItem(tr2(TaskInfo.GetID().c_str())));
            ui->tableWidget->item(Row, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(Row, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(Row, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(Row, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ui->tableWidget->item(Row, 4)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
            ++Row;
        }
    }
}

std::string CDlgMissionPlatformLoadList::GetSelectedTreeLevel()
{
    QTreeWidgetItem *ItemChild = ui->treeWidget->currentItem();
    std::string str;
    if (ItemChild->parent() == nullptr)
    {
        str = ItemChild->text(0).toStdString();
        return str;
    }
    while (ItemChild->parent() != nullptr)
    {
        QTreeWidgetItem *ItemParent = ItemChild->parent();
        ItemChild = ItemParent;
        str = ItemChild->text(0).toStdString();
    }
    return str;
}

void CDlgMissionPlatformLoadList::CreateTreeeMenu()
{
    m_TreeMenuRight = new QMenu(this);
    QAction *TaskMenu_Edit = m_TreeMenuRight->addAction(tr("Edit"));
    connect(TaskMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_MarkAmmo_Edit()));
}

void CDlgMissionPlatformLoadList::showEvent(QShowEvent *event)
{
    Init();
}

void CDlgMissionPlatformLoadList::on_timeOut()
{
    if (b_UpdateAmmo)
    {
        Init();
        b_UpdateAmmo = false;
    }
}

void CDlgMissionPlatformLoadList::on_Menu_MarkAmmo_Edit()
{
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    if (ui->tabWidget->currentWidget() == ui->tab)
    {
        int row = ui->tableWidget->currentRow();
        QString strTaskID = ui->tableWidget->item(row, 4)->text();
        props.put("TaskID", strTaskID.toStdString());
    }
    else
    {
        std::string strTaskID = GetSelectedTreeLevel();
        props.put("TaskID", strTaskID);
    }
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgCDlgMissionPlatformLoadList", pBuf, nBufLen, props));
}

void CDlgMissionPlatformLoadList::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    m_TreeMenuRight->exec(QCursor::pos());
}

void CDlgMissionPlatformLoadList::on_tableWidget_cellDoubleClicked()
{
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    int row = ui->tableWidget->currentRow();
    QString strTaskID = ui->tableWidget->item(row, 4)->text();
    props.put("TaskID", strTaskID.toStdString());
    ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgCDlgMissionPlatformLoadList", pBuf, nBufLen, props));
}

