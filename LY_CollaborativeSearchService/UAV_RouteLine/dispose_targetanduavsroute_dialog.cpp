#include "dispose_targetanduavsroute_dialog.h"
#include "ui_dispose_targetanduavsroute_dialog.h"

Dispose_targetAndUAVSRoute_Dialog::Dispose_targetAndUAVSRoute_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dispose_targetAndUAVSRoute_Dialog)
{
    ui->setupUi(this);
}

Dispose_targetAndUAVSRoute_Dialog::~Dispose_targetAndUAVSRoute_Dialog()
{
    delete ui;
}
