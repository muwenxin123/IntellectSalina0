#include "ISAddCarryingPlanResDialog.h"
#include "ui_ISAddCarryingPlanResDialog.h"
#include "QCString.h"
#include "lY_TaskManage.h"
#include "IResourceCarryingPlanDialog.h"
#include <QTableWidget>

ISAddCarryingPlanResDialog::ISAddCarryingPlanResDialog(QWidget *parent) :
    LyDialog(tr2("添加资源"), parent),
    ui(new Ui::ISAddCarryingPlanResDialog)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    connect(ui->pbtnOK, &QPushButton::clicked, this, &ISAddCarryingPlanResDialog::on_pbtnOK_clicked);
    connect(ui->pbtnCancel, &QPushButton::clicked, this, &ISAddCarryingPlanResDialog::on_pbtnCancel_clicked);
}

ISAddCarryingPlanResDialog::~ISAddCarryingPlanResDialog()
{

}

void ISAddCarryingPlanResDialog::Init(int value, QTableWidget *widget)
{

    QStringList list;
    list << tr2("平台") << tr2("传感器") << tr2("电台") << tr2("DY") << tr2("燃料")
         << tr2("电池") << tr2("飞机配件（个）") << tr2("工具（件）") << tr2("食物和水（天）") << tr2("车辆");
    ui->cbxResourceType->addItems(list);

    if (value && value < ui->cbxResourceType->count())
    {

        ui->cbxResourceType->setCurrentIndex(value - 1);
    }
    m_tableWidget = widget;
}
void ISAddCarryingPlanResDialog::SetAmendInit(QString strName, int nNum, int nSurPlus, QString strRemark)
{
    ui->ledtResouceName->setText(strName);
    ui->ledtResouceAmount->setText(QString::number(nNum));
    ui->ledtRemainAmount->setText(QString::number(nSurPlus));
    ui->ledtRemark->setText(strRemark);
}
void ISAddCarryingPlanResDialog::on_pbtnOK_clicked()
{
    if (!m_tableWidget)
    {
        return;
    }

    QString strReName = ui->ledtResouceName->text();
    int nReNum = ui->ledtResouceAmount->text().toInt();
    int nSurPlus = ui->ledtRemainAmount->text().toInt();
    QString strRemark = ui->ledtRemark->text();

    int row = m_tableWidget->rowCount();
    m_tableWidget->insertRow(row);
    m_tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number(row)));
    m_tableWidget->setItem(row, 1, new QTableWidgetItem(strReName));
    m_tableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(nReNum)));
    m_tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(nSurPlus)));
    m_tableWidget->setItem(row, 4, new QTableWidgetItem(strRemark));

}
void ISAddCarryingPlanResDialog::on_pbtnCancel_clicked()
{
    close();
}