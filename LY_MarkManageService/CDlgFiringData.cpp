#include "CDlgFiringData.h"
#include "ui_CDlgFiringData.h"

CDlgFiringData::CDlgFiringData(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CDlgFiringData)
{
    ui->setupUi(this);
}

CDlgFiringData::~CDlgFiringData()
{
    delete ui;
}
