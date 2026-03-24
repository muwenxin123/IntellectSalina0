#include "cmdTrack.h"
#include "ui_cmdTrack.h"

CmdTrack::CmdTrack(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CmdTrack)
{
    ui->setupUi(this);
}

CmdTrack::~CmdTrack()
{
    delete ui;
}
