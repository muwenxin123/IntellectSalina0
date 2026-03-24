#include "QViewShed.h"
#include "ui_QViewShed.h"
#include "Measure.h"
#include <ViewShed.h>
#include <Cover.h>

QViewShed::QViewShed(QWidget *parent) :
    LyDialog(tr("ViewShed"), parent),
    ui(new Ui::QViewShed)
{
    ui->setupUi((QDialog *)centralWidget());
    m_pShp = NULL;
    connect(ui->pushButtonOk, SIGNAL(clicked()), this, SLOT(on_pushButtonOk_clicked()));
    connect(ui->pushButtonCancel, SIGNAL(clicked()), this, SLOT(rejected()));
}

QViewShed::~QViewShed()
{
    delete ui;
}

void QViewShed::setLonLat(double dLon, double dLat)
{
    ui->widgetLon->setValue(dLon);
    ui->widgetLat->setValue(dLat);
}

void QViewShed::on_pushButtonOk_clicked()
{
    double dLon = ui->widgetLon->value();
    double dLat = ui->widgetLat->value();

    CCover c(Measure::Get().GetDataDir() + "dem/90/");
    setCursor(Qt::WaitCursor);
    m_pShp = c.GetCover2(dLon, dLat,
                         ui->spinBoxHeight->value(),
                         ui->spinBoxMaxRadii->value() * 1000,
                         ui->spinBoxMarkHeight->value());
    setCursor(Qt::ArrowCursor);
    accept();
}
