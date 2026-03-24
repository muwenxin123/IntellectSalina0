#include "cmd.h"
#include "ui_cmd.h"

Cmd::Cmd(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Cmd)
{
    ui->setupUi(this);
}

Cmd::~Cmd()
{
    delete ui;
}
