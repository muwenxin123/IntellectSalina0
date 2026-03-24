#include "UAVFlyManage_MavlinkMainDialog.h"
#include "ui_UAVFlyManage_MavlinkMainDialog.h"

#include <QDebug>

#include "QGCApplication.h"

#include "ApplicationSetting/UAVFlyManage_MavlinkApplicationSetting.h"
#include "VehicleManage/UAVFlyManage_MavlinkVehicleManageDialog.h"

UAVFlyManage_MavlinkMainDialog::UAVFlyManage_MavlinkMainDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkMainDialog)
    , m_pUAVFlyManage_MavlinkApplicationSetting(nullptr)
    , m_pUAVFlyManage_MavlinkVehicleManageDialog(nullptr)
{
    ui->setupUi(this);

    ui->listWidget_UAVFlyManage_MavlinkMainDialog->setStyleSheet(QString("QListView::item {border-width: 1px;padding: 1px; height: 50px;}"));
    ui->listWidget_UAVFlyManage_MavlinkMainDialog->style()->unpolish(ui->listWidget_UAVFlyManage_MavlinkMainDialog);
    ui->listWidget_UAVFlyManage_MavlinkMainDialog->style()->polish(ui->listWidget_UAVFlyManage_MavlinkMainDialog);

    ui->listWidget_UAVFlyManage_MavlinkMainDialog->clear();

    if(QGCApplication::qgcApplicationSettingDialogShow()){

       m_pUAVFlyManage_MavlinkApplicationSetting = new UAVFlyManage_MavlinkApplicationSetting(this);

       m_pUAVFlyManage_MavlinkApplicationSetting->setVisible(false);
       m_pUAVFlyManage_MavlinkApplicationSetting->hide();

       QListWidgetItem* listWidgetItem = new QListWidgetItem(tr("Setting"));
       listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<QWidget*>(m_pUAVFlyManage_MavlinkApplicationSetting)));
       ui->listWidget_UAVFlyManage_MavlinkMainDialog->addItem(listWidgetItem);
    }

    if(QGCApplication::qgcApplicationVehicleManageDialogShow()){

       m_pUAVFlyManage_MavlinkVehicleManageDialog = new UAVFlyManage_MavlinkVehicleManageDialog(this);

       m_pUAVFlyManage_MavlinkVehicleManageDialog->setVisible(false);
       m_pUAVFlyManage_MavlinkVehicleManageDialog->hide();

       QListWidgetItem* listWidgetItem = new QListWidgetItem(tr("Vehicles"));
       listWidgetItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<QWidget*>(m_pUAVFlyManage_MavlinkVehicleManageDialog)));
       ui->listWidget_UAVFlyManage_MavlinkMainDialog->addItem(listWidgetItem);
    }
}

UAVFlyManage_MavlinkMainDialog::~UAVFlyManage_MavlinkMainDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkMainDialog::DrawDynamic(QPainter &painter, IProj *pProj)
{
    if(m_pUAVFlyManage_MavlinkVehicleManageDialog){
        m_pUAVFlyManage_MavlinkVehicleManageDialog->DrawDynamic(painter, pProj);
    }
}

void UAVFlyManage_MavlinkMainDialog::OnMouseButtonDown(QMouseEvent *e, IProj *pProj)
{

}

void UAVFlyManage_MavlinkMainDialog::OnMouseButtonRelease(QMouseEvent *e, IProj *pProj)
{

}

void UAVFlyManage_MavlinkMainDialog::OnMouseDoubleClick(QMouseEvent *e, IProj *pProj)
{
    if(m_pUAVFlyManage_MavlinkVehicleManageDialog){
        m_pUAVFlyManage_MavlinkVehicleManageDialog->OnMouseDoubleClick(e, pProj);
    }
}

void UAVFlyManage_MavlinkMainDialog::OnMouseMove(QMouseEvent *e, IProj *pProj)
{

}

void UAVFlyManage_MavlinkMainDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UAVFlyManage_MavlinkMainDialog::showEvent(QShowEvent *event)
{

}

void UAVFlyManage_MavlinkMainDialog::hideEvent(QHideEvent *event)
{

}

void UAVFlyManage_MavlinkMainDialog::on_listWidget_UAVFlyManage_MavlinkMainDialog_itemDoubleClicked(QListWidgetItem *item)
{
    if(!item){
        return ;
    }

    if(!ui || !ui->verticalLayout_UAVFlyManage_MavlinkMainDialog){
        return ;
    }

    if(!ui || !ui->listWidget_UAVFlyManage_MavlinkMainDialog){
        return ;
    }

    auto* horizontalLayout = ui->verticalLayout_UAVFlyManage_MavlinkMainDialog;
    if(!horizontalLayout){
        return ;
    }

    auto* listWidget = ui->listWidget_UAVFlyManage_MavlinkMainDialog;
    if(!listWidget){
        return ;
    }

    auto* currentItem = listWidget->currentItem();
    if(!currentItem){
        return ;
    }

    const QVariant& userData = currentItem->data(Qt::UserRole);
    if(!userData.isValid()){
        return ;
    }

    QWidget* widget = qobject_cast<QWidget*>(qvariant_cast<QObject *>(userData));
    if (!widget) {
        qWarning() << "AutoPilotPlugin::_recalcSetupComplete Incorrectly typed VehicleComponent";
        return ;
    }

    QLayoutItem *child;
    while ((child = horizontalLayout->takeAt(0)) != nullptr) {
        if(child->widget()){
            child->widget()->setVisible(false);
            child->widget()->hide();
        }

    }
    horizontalLayout->addWidget(widget);
    widget->setVisible(true);
    widget->show();
}
