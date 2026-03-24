#include "IResourceCarryingPlanDialog.h"
#include "ui_IResourceCarryingPlanDialog.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "ISAddCarryingPlanResDialog.h"

IResourceCarryingPlanDialog::IResourceCarryingPlanDialog(QWidget *parent) :
    LyDialog(tr2("携行规划"), parent),
    ui(new Ui::IResourceCarryingPlanDialog)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1200, 1000);
    m_currentWidget = nullptr;
    m_pAddCarryingPlanResDialog = new ISAddCarryingPlanResDialog;
    m_pAddCarryingPlanResDialog->hide();

    connect(ui->pbtnSave, &QPushButton::clicked, this, &IResourceCarryingPlanDialog::on_pbtnSave_clicked);
    connect(ui->pbtnClose, &QPushButton::clicked, this, &IResourceCarryingPlanDialog::on_pbtnClose_clicked);
    QList<QTableWidget *> children = this->findChildren<QTableWidget *>();
    for (auto menuUi : children)
    {
        menuUi->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(menuUi, &QTableWidget::customContextMenuRequested, this, &IResourceCarryingPlanDialog::on_AllPlanTable_customContextMenuRequested);
    }
    Init();
}

IResourceCarryingPlanDialog::~IResourceCarryingPlanDialog()
{
    delete ui;
}

void IResourceCarryingPlanDialog::Init()
{

    CResourceCarryingVec allResVec = ITaskManagerActivator::Get().getPTaskManageService()->GetResourceCarryingPlanInfo();
    vector<pair<int, CResourceCarrying>> CarryingInfoVec;
    for (auto data : allResVec)
    {
        pair<int, CResourceCarrying> carryInfo;
        carryInfo.first = data.GetResourceType();
        carryInfo.second = data;
        CarryingInfoVec.push_back(carryInfo);
    }

    m_CarryingPlatInfoVec.clear();
    vector<int> existKey;
    for (auto info : CarryingInfoVec)
    {
        int key = info.first;
        if (::find(existKey.begin(), existKey.end(), key) != existKey.end())
        {
            continue;
        }
        existKey.push_back(key);
        vector<CResourceCarrying> data;
        for (auto carry : CarryingInfoVec)
        {
            if (info.first == carry.first)
            {
                data.push_back(carry.second);
            }
        }
        m_CarryingPlatInfoVec[key] = data;
    }

    CreateCarryingTable(m_CarryingPlatInfoVec);
}

void IResourceCarryingPlanDialog::CreateCarryingTable(map<int, CResourceCarryingVec> carryingInfo)
{

    QList<QTableWidget *> children = this->findChildren<QTableWidget *>();
    int num = 0;
    for (QObject *child : children)
    {
        QTableWidget *tableWidget = qobject_cast<QTableWidget *>(child);
        if (!tableWidget)
        {
            continue;
        }
        QStringList headerlabels;
        headerlabels << tr2("No.") << tr2("资源名称") << tr2("资源数量") << tr2("备用/剩余数量") << tr2("备注");
        tableWidget->clear();
        tableWidget->setColumnCount(headerlabels.size());
        tableWidget->setHorizontalHeaderLabels(headerlabels);
        tableWidget->verticalHeader()->setHidden(true);
        tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
        tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
        tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        tableWidget->setFocusPolicy(Qt::StrongFocus);
        tableWidget->setAlternatingRowColors(true);
        tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
        tableWidget->horizontalHeader()->setStretchLastSection(true);
        auto dataVec = carryingInfo[num + 1];
        tableWidget->setRowCount(dataVec.size());
        for (int i = 0; i < dataVec.size(); i++)
        {
            tableWidget->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
            tableWidget->setItem(i, 1, new QTableWidgetItem(dataVec[i].GetResourceName().c_str()));
            tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(dataVec[i].GetResourceNum())));
            tableWidget->setItem(i, 3, new QTableWidgetItem(QString::number(dataVec[i].GetRemainNum())));
            tableWidget->setItem(i, 4, new QTableWidgetItem(dataVec[i].GetRemark().c_str()));
        }
        ++num;
    }
}

void IResourceCarryingPlanDialog::on_AllPlanTable_customContextMenuRequested(const QPoint &pos)
{
    QMenu *TableMenu = new QMenu(this);
    QAction *MenuAdd = TableMenu->addAction(tr2("添加"));
    QAction *MenuEdit = TableMenu->addAction(tr2("修改"));
    QAction *MenuRemove = TableMenu->addAction(tr2("删除"));

    connect(MenuAdd, SIGNAL(triggered()), this, SLOT(on_Menu_Add()));
    connect(MenuEdit, SIGNAL(triggered()), this, SLOT(on_Menu_Edit()));
    connect(MenuRemove, SIGNAL(triggered()), this, SLOT(on_Menu_Remove()));
    QWidget *widget = QApplication::widgetAt(pos);
    if (widget)
    {
        QTableWidget *tablewidget = qobject_cast<QTableWidget *>(sender());
        if (tablewidget)
        {
            m_currentWidget = tablewidget;
        }
    }

    TableMenu->exec(QCursor::pos());
}

void IResourceCarryingPlanDialog::on_Menu_Add()
{
    if (LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->isVisible())
    {
        LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->close();
    }
    else
    {

        if (!m_currentWidget)
        {
            return;
        }
        QList<QTableWidget *> children = this->findChildren<QTableWidget *>();
        int value = 0;
        for (value; value < children.size(); value++)
        {
            if (m_currentWidget != children[value])
            {
                continue;
            }
            ++value;
            break;
        }
        LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->Init(value, m_currentWidget);
        LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->show();
        m_currentWidget = nullptr;
    }
}

void IResourceCarryingPlanDialog::on_Menu_Edit()
{
    if (LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->isVisible())
    {
        LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->close();
    }
    else
    {

        if (!m_currentWidget)
        {
            LyMessageBox::information(this, tr2("提示"), tr2("使用鼠标左键点击需要修改的资源!"));
            return;
        }

        QList<QTableWidget *> children = this->findChildren<QTableWidget *>();
        int value = 0;
        for (value; value < children.size(); value++)
        {
            if (m_currentWidget != children[value])
            {
                continue;
            }
            ++value;
            break;
        }
        int row = m_currentWidget->currentRow();
        QString strName = m_currentWidget->item(row, 1)->text();
        int nNum = m_currentWidget->item(row, 2)->text().toInt();
        int nSurPlus = m_currentWidget->item(row, 3)->text().toInt();
        QString strRemark = m_currentWidget->item(row, 4)->text();
        LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->Init(value, m_currentWidget);
        LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->SetAmendInit(strName, nNum, nSurPlus, strRemark);
        LY_TaskManage::Get().GetISAddCarryingPlanResDialog()->show();
        m_currentWidget = nullptr;
    }
}

void IResourceCarryingPlanDialog::on_Menu_Remove()
{
    if (!m_currentWidget)
    {
        LyMessageBox::information(this, tr2("提示"), tr2("未选中资源!"));
        return;
    }
    QTableWidget *item = m_currentWidget;
    int row = item->currentRow();
    if (-1 == row)
    {
        return;
    }

    item->removeRow(row);
    for (int i = 0; i < item->rowCount(); i++)
    {
        item->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
    }
    m_currentWidget = nullptr;
}

void IResourceCarryingPlanDialog::on_pbtnSave_clicked()
{
    vector<CResourceCarrying> CarryingInfoVec;
    QList<QTableWidget *> children = this->findChildren<QTableWidget *>();
    int num = 1;
    for (QObject *child : children)
    {
        QTableWidget *tableWidget = qobject_cast<QTableWidget *>(child);
        if (!tableWidget)
        {
            continue;
        }
        int row = tableWidget->rowCount();
        for (int i = 0; i < row; i++)
        {
            CResourceCarrying info;
            info.SetResourceType(num);
            string strName = tableWidget->item(i, 1)->text().toStdString();
            info.SetResourceName(strName);
            info.SetResourceNum(tableWidget->item(i, 2)->text().toInt());
            info.SetRemainNum(tableWidget->item(i, 3)->text().toInt());
            string strRemark = tableWidget->item(i, 4)->text().toStdString();
            info.SetRemark(strRemark);
            CarryingInfoVec.push_back(info);
        }
        ++num;
    }

    bool bsucc = ITaskManagerActivator::Get().getPTaskManageService()->DeleteCarryingResource();
    if (!bsucc)
    {
        LyMessageBox::information(this, tr2("提示"), tr2("保存失败，原数据未成功删除!"));
        return;
    }
    num = 1;
    for (auto &info : CarryingInfoVec)
    {
        info.SetBH(num);
        ITaskManagerActivator::Get().getPTaskManageService()->AddCarryingResource(info);
        ++num;
    }
    LyMessageBox::information(this, tr2("提示"), tr2("保存完成!"));
}

void IResourceCarryingPlanDialog::on_pbtnClose_clicked()
{
    close();
}