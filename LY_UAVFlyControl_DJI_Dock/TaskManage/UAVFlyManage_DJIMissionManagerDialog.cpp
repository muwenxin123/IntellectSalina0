#include "UAVFlyManage_DJIMissionManagerDialog.h"
#include "ui_UAVFlyManage_DJIMissionManagerDialog.h"

UAVFlyManage_DJIMissionManagerDialog::UAVFlyManage_DJIMissionManagerDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_DJIMissionManagerDialog)
{
    ui->setupUi(this);
}

UAVFlyManage_DJIMissionManagerDialog::~UAVFlyManage_DJIMissionManagerDialog()
{
    delete ui;
}
