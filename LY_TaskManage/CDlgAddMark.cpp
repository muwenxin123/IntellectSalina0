#include "CDlgAddMark.h"
#include "ui_CDlgAddMark.h"
#include "QCString.h"
#include "LyDialog.h"
#include "ITaskManagerActivator.h"
#include <QString>
#include "lY_TaskManage.h"
CDlgAddMark::CDlgAddMark(QWidget *parent) :
    LyDialog(tr("Mark List"), parent),
    ui(new Ui::CDlgAddMark)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1080, 500);
    this->raise();
    this->setWindowFlags(this->windowFlags()& Qt::WindowCloseButtonHint);
    Init();
}

CDlgAddMark::~CDlgAddMark()
{
    delete ui;
}

void CDlgAddMark::Init()
{
    ui->tbwMark->clear();
    targetInfoVec = GetMarkList();
    QStringList header;
    header << tr("NO.") << tr("Mark Name") << tr("Mark ID");
    ui->tbwMark->setRowCount(targetInfoVec.size());
    ui->tbwMark->setColumnCount(header.size());
    ui->tbwMark->setHorizontalHeaderLabels(header);
    ui->tbwMark->horizontalHeader()->setStretchLastSection(true);
    ui->tbwMark->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tbwMark->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tbwMark->verticalHeader()->setVisible(false);
    ui->tbwMark->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tbwMark->setSelectionBehavior(QAbstractItemView::SelectRows);
    for (int i = 0; i < targetInfoVec.size(); i++)
    {
        auto data = targetInfoVec[i];
        ui->tbwMark->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        ui->tbwMark->setItem(i, 1, new QTableWidgetItem(tr(data.GetName().c_str())));
        ui->tbwMark->setItem(i, 2, new QTableWidgetItem(tr(data.GetID().c_str())));
    }

    connect(ui->tbwMark, &QTableWidget::doubleClicked, this, &CDlgAddMark::on_tbwMark_doubleclicked);
    connect(ui->btnOk, &QPushButton::clicked, this, &CDlgAddMark::on_btnOk_clicked);
    connect(ui->btnCencel, &QPushButton::clicked, this, &CDlgAddMark::on_btnCencel_clicked);
}

std::vector<SystemMarkInfo> CDlgAddMark::GetMarkList()
{
    std::vector<SystemMarkInfo>tmp;
    tmp = ITaskManagerActivator::Get().getPIMarkManageService()->GetMarkList();
    return tmp;
}

void CDlgAddMark::on_btnOk_clicked()
{
    int row = ui->tbwMark->currentRow();
    QString strName = ui->tbwMark->item(row, 1)->text();
    QString strID = ui->tbwMark->item(row, 2)->text();
    LY_TaskManage::Get().GetCDlgActionEdit()->addMark(strName, strID);

}

void CDlgAddMark::on_btnCencel_clicked()
{
    this->close();
}

void CDlgAddMark::on_tbwMark_doubleclicked()
{
    int row = ui->tbwMark->currentRow();
    QString strName = ui->tbwMark->item(row, 1)->text();
    QString strID = ui->tbwMark->item(row, 2)->text();
    LY_TaskManage::Get().GetCDlgActionEdit()->addMark(strName, strID);

}
