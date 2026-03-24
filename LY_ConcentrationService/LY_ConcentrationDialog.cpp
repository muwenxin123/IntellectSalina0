#include "LY_ConcentrationDialog.h"
#include "ui_LY_ConcentrationDialog.h"

LY_ConcentrationDialog::LY_ConcentrationDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LY_ConcentrationDialog)
{
    ui->setupUi(this);
}

LY_ConcentrationDialog::~LY_ConcentrationDialog()
{
    delete ui;
}
