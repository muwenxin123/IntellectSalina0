#include "UAVFlyManage_DJITaskManageDialog.h"
#include "ui_UAVFlyManage_DJITaskManageDialog.h"

#include <QMouseEvent>
#include <QPainter>

UAVFlyManage_DJITaskManageDialog::UAVFlyManage_DJITaskManageDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_DJITaskManageDialog)
{
    ui->setupUi(this);
}

UAVFlyManage_DJITaskManageDialog::~UAVFlyManage_DJITaskManageDialog()
{
    delete ui;
}

void UAVFlyManage_DJITaskManageDialog::changeEvent(QEvent *e)
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

void UAVFlyManage_DJITaskManageDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    // this->m_TaskManageLineEditStates = TaskManageLineEditStates::TaskManageLinePickNone;

    // if(QGCApplication::createVehicleMissionLineFromTaskEnabled()){
    //     InitTaskListInfo();
    // }
}

void UAVFlyManage_DJITaskManageDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    // stopMissionPlan();
}

void UAVFlyManage_DJITaskManageDialog::on_tabWidget_UAVFlyManage_DJITaskManage_Vehicles_customContextMenuRequested(const QPoint &pos)
{

}

void UAVFlyManage_DJITaskManageDialog::on_comboBox_UAVFlyManage_DJITaskManage_Task_currentTextChanged(const QString &arg1)
{

}


void UAVFlyManage_DJITaskManageDialog::on_comboBox_UAVFlyManage_DJITaskManage_Vehicles_currentTextChanged(const QString &arg1)
{

}


void UAVFlyManage_DJITaskManageDialog::on_pushButton_UAVFlyManage_DJITaskManage_VehiclesHideAll_clicked()
{

}


void UAVFlyManage_DJITaskManageDialog::on_pushButton_UAVFlyManage_DJITaskManage_VehiclesRemoveAll_clicked()
{

}


void UAVFlyManage_DJITaskManageDialog::on_pushButton_UAVFlyManage_DJITaskManage_UploadAllWayPoints_clicked()
{

}


void UAVFlyManage_DJITaskManageDialog::on_pushButton_UAVFlyManage_DJITaskManage_DownloadAllWayPoints_clicked()
{

}


void UAVFlyManage_DJITaskManageDialog::on_pushButton_UAVFlyManage_DJITaskManage_SaveAllPoint_clicked()
{

}


void UAVFlyManage_DJITaskManageDialog::on_pushButton_UAVFlyManage_DJITaskManage_Vehicles_Save_clicked()
{

}


void UAVFlyManage_DJITaskManageDialog::on_pushButton_UAVFlyManage_DJITaskManage_Vehicles_Open_clicked()
{

}

