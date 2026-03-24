#include "CDlgMarkMaintain.h"

#include "QCString.h"
#include "CDlgAddMaintainMark.h"
#include "lY_TaskManage.h"
#include "LyMessageBox.h"
CDlgMarkMaintain::CDlgMarkMaintain(QWidget *parent)
    : LyDialog(tr2("目标管理"), parent),
      ui(new Ui::CDlgMarkMaintain)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(500, 600);
    connect(ui->btnAdd, &QPushButton::clicked, this, &CDlgMarkMaintain::on_btnAdd_clicked);
    connect(ui->btnEdit, &QPushButton::clicked, this, &CDlgMarkMaintain::on_btnEdit_clicked);
    connect(ui->btnDele, &QPushButton::clicked, this, &CDlgMarkMaintain::on_btnDele_clicked);
    connect(ui->tbwMark, &QTableWidget::customContextMenuRequested, this, &CDlgMarkMaintain::on_tbwMark_customContextMenuRequested);
    connect(ui->leSearch, &QLineEdit::textChanged, this, &CDlgMarkMaintain::on_leSearch_textChanged);
}

CDlgMarkMaintain::~CDlgMarkMaintain()
{
}

void CDlgMarkMaintain::Init()
{

    m_MarkListVec = ITaskManagerActivator::Get().getPIMarkManageService()->GetMarkList();
    if (m_MarkListVec.empty())
    {
        return;
    }

    ui->tbwMark->clear();
    QStringList list;
    list << tr2("序号") << tr2("目标名称") << tr2("目标ID");
    ui->tbwMark->setColumnCount(list.size());
    ui->tbwMark->setHorizontalHeaderLabels(list);
    ui->tbwMark->verticalHeader()->setHidden(true);
    ui->tbwMark->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwMark->setAlternatingRowColors(true);
    ui->tbwMark->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwMark->horizontalHeader()->setStretchLastSection(true);
    ui->tbwMark->setSelectionBehavior(QTableWidget::SelectRows);
    int number = 0;
    ui->tbwMark->setRowCount(m_MarkListVec.size());

    for (auto info : m_MarkListVec)
    {
        ui->tbwMark->setItem(number, 0, new QTableWidgetItem(QString::number(number + 1)));
        ui->tbwMark->setItem(number, 1, new	QTableWidgetItem(QString::fromStdString(info.GetName())));
        ui->tbwMark->setItem(number, 2, new QTableWidgetItem(QString::fromStdString(info.GetID())));
        ++number;
    }
    RecordInformation();
}

void CDlgMarkMaintain::CreateTableData(vector<int> row)
{
    ui->tbwMark->clear();
    QStringList list;
    list << tr2("序号") << tr2("目标名称") << tr2("目标ID");
    ui->tbwMark->setColumnCount(list.size());
    ui->tbwMark->setHorizontalHeaderLabels(list);
    ui->tbwMark->verticalHeader()->setHidden(true);
    ui->tbwMark->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwMark->setAlternatingRowColors(true);
    ui->tbwMark->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->tbwMark->horizontalHeader()->setStretchLastSection(true);
    ui->tbwMark->setSelectionBehavior(QTableWidget::SelectRows);
    int number = 0;
    ui->tbwMark->setRowCount(row.size());
    for (int i = 0; i < m_MarkListVec.size(); i++)
    {
        if (::find(row.begin(), row.end(), i) == row.end())
        {
            continue;
        }
        ui->tbwMark->setItem(number, 0, new QTableWidgetItem(QString::number(number + 1)));
        ui->tbwMark->setItem(number, 1, new	QTableWidgetItem(QString::fromStdString(m_MarkListVec[i].GetName())));
        ui->tbwMark->setItem(number, 2, new QTableWidgetItem(QString::fromStdString(m_MarkListVec[i].GetID())));
        ++number;
    }
}

void CDlgMarkMaintain::RecordInformation()
{
    int row = ui->tbwMark->rowCount();
    vector<pair<int, QString>> data;
    for (int i = 0; i < row; i++)
    {
        pair<int, QString> parInfo;
        parInfo.first = i;
        QString str = ui->tbwMark->item(i, 1)->text();
        parInfo.second = str;
        data.push_back(parInfo);
        str = ui->tbwMark->item(i, 2)->text();
        data.push_back(parInfo);
    }
    RecordDataVec = data;
}

void CDlgMarkMaintain::on_btnAdd_clicked()
{
    if (LY_TaskManage::Get().GetDlgAddMaintainMark()->isVisible())
    {
        LY_TaskManage::Get().GetDlgAddMaintainMark()->hide();
    }
    else
    {
        LY_TaskManage::Get().GetDlgAddMaintainMark()->Init(0);
        LY_TaskManage::Get().GetDlgAddMaintainMark()->raise() ;
        LY_TaskManage::Get().GetDlgAddMaintainMark()->show();
    }
}

void CDlgMarkMaintain::on_btnEdit_clicked()
{
    if (nullptr == ui->tbwMark->currentItem())
    {
        return;
    }
    int row = ui->tbwMark->currentRow();
    QString str = ui->tbwMark->item(row, 2)->text();
    if (LY_TaskManage::Get().GetDlgAddMaintainMark()->isVisible())
    {
        LY_TaskManage::Get().GetDlgAddMaintainMark()->hide();
    }
    else
    {
        LY_TaskManage::Get().GetDlgAddMaintainMark()->Init(1, str.toStdString());
        LY_TaskManage::Get().GetDlgAddMaintainMark()->raise();
        LY_TaskManage::Get().GetDlgAddMaintainMark()->show();
    }
}

void CDlgMarkMaintain::on_btnDele_clicked()
{
    if (nullptr == ui->tbwMark->currentItem())
    {
        return;
    }
    int row = ui->tbwMark->currentRow();
    QString str = tr2("是否删除ID为：");
    str += ui->tbwMark->item(row, 2)->text();
    str += tr2(" 的目标");
    if (QMessageBox::Ok != LyMessageBox::information(this, tr2("提示"), str, QMessageBox::Ok | QMessageBox::Cancel))
    {
        return;
    }
    string targetid = ui->tbwMark->currentItem()->text().toStdString();
    bool bsucc = ITaskManagerActivator::Get().getPIMarkManageService()->RmoveTartget(targetid);
    if (!bsucc)
    {
        LyMessageBox::information(this, tr2("提示"), tr2("删除失败！"));
        return;
    }
    Init();
}

void CDlgMarkMaintain::on_tbwMark_customContextMenuRequested(const QPoint &pos)
{
    QTableWidgetItem *curItem = ui->tbwMark->itemAt(pos);
    if (curItem == NULL)
    {
        m_twAmmoMenu = new QMenu;
        QAction *TableWMenu_Add = m_twAmmoMenu->addAction(tr2("添加"));
        QAction *TableWMenu_Edit = m_twAmmoMenu->addAction(tr2("修改"));
        QAction *TableWMenu_Remove = m_twAmmoMenu->addAction(tr2("删除"));
        connect(TableWMenu_Add, SIGNAL(triggered()), this, SLOT(on_Menu_Ammo_Add()));
        m_twAmmoMenu->exec(QCursor::pos());
    }
    else
    {
        m_twAmmoMenu = new QMenu;
        QAction *TableWMenu_Add = m_twAmmoMenu->addAction(tr2("添加"));
        QAction *TableWMenu_Edit = m_twAmmoMenu->addAction(tr2("修改"));
        QAction *TableWMenu_Remove = m_twAmmoMenu->addAction(tr2("删除"));
        connect(TableWMenu_Add, SIGNAL(triggered()), this, SLOT(on_Menu_Ammo_Add()));
        connect(TableWMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_Ammo_Edit()));
        connect(TableWMenu_Remove, SIGNAL(triggered()), this, SLOT(on_Menu_Ammo_Delete()));
        m_twAmmoMenu->exec(QCursor::pos());
    }
}

void CDlgMarkMaintain::on_Menu_Ammo_Add()
{
    on_btnAdd_clicked();
}

void CDlgMarkMaintain::on_Menu_Ammo_Edit()
{
    on_btnEdit_clicked();
}

void CDlgMarkMaintain::on_Menu_Ammo_Delete()
{
    on_btnDele_clicked();
}
#include <QDebug>
void CDlgMarkMaintain::on_leSearch_textChanged()
{
    QString str = ui->leSearch->text();
    qDebug() << str;
    vector<int> data;
    for (auto info : RecordDataVec)
    {
        QString recorddata = info.second;
        qDebug() << recorddata;
        if (nullptr == strstr(recorddata.toStdString().c_str(), str.toStdString().c_str()))
        {
            continue;
        }
        if (::find(data.begin(), data.end(), info.first) != data.end())
        {
            continue;
        }
        data.push_back(info.first);
    }
    if (data.empty())
    {
        Init();
    }
    else
    {
        CreateTableData(data);
    }
}
