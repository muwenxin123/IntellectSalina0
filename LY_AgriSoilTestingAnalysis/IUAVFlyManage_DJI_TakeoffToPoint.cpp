#include "IUAVFlyManage_DJI_TakeoffToPoint.h"
#include "ui_IUAVFlyManage_DJI_TakeoffToPoint.h"

IUAVFlyManage_DJI_TakeoffToPoint::IUAVFlyManage_DJI_TakeoffToPoint(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IUAVFlyManage_DJI_TakeoffToPoint)
{
    ui->setupUi(this);
}

IUAVFlyManage_DJI_TakeoffToPoint::~IUAVFlyManage_DJI_TakeoffToPoint()
{
    delete ui;
}
