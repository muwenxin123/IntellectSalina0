#include "RoutePlaningReportsDialog.h"
#include "ui_RoutePlaningReportsDialog.h"

RoutePlaningReportsDialog::RoutePlaningReportsDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RoutePlaningReportsDialog)
{
    ui->setupUi(this);
}

RoutePlaningReportsDialog::~RoutePlaningReportsDialog()
{
    delete ui;
}
