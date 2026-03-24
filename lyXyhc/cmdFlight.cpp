#include "cmdFlight.h"
#include "ui_cmdFlight.h"

CmdFlight::CmdFlight(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CmdFlight)
{
    ui->setupUi(this);
}

CmdFlight::~CmdFlight()
{
    delete ui;
}
