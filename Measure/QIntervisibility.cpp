#include "QIntervisibility.h"
#include "ui_QIntervisibility.h"
#include <QCString.h>
#include <Intervisibility.h>
#include <BiIntervisibility.h>
#include "Measure.h"

QIntervisibility::QIntervisibility(bool bBiIntervisibility, QWidget *parent) :
    QDialog(parent),
    m_bBiIntervisibility(bBiIntervisibility),
    ui(new Ui::QIntervisibility)
{
    ui->setupUi(this);
    m_pShp[0] = NULL;
    m_pShp[1] = NULL;
}

QIntervisibility::~QIntervisibility()
{
    delete ui;
}

void QIntervisibility::setLonLat(const QLineF &line)
{
    char szT[32];
    const QString pT[6] = { QObject::tr("Observation point longitude"), QObject::tr("Observation point latitude"), QObject::tr("Observation point height"),
                            QObject::tr("Mark point longitude"), QObject::tr("Mark point latitude"), QObject::tr("Mark point height")
                          };
    double dT[6] = {line.x1(), line.y1(), 100, line.x2(), line.y2(), 100};
    QTableWidgetItem *pItem;
    ui->tableWidget->setRowCount(6);
    for (int i = 0; i < 6; i++)
    {
        pItem = new QTableWidgetItem(pT[i]);
        ui->tableWidget->setItem(i, 0, pItem);
        sprintf(szT, "%.6f", dT[i]);
        pItem = new QTableWidgetItem(szT);
        ui->tableWidget->setItem(i, 1, pItem);
    }
}

void QIntervisibility::on_pushButtonOk_clicked()
{
    setCursor(Qt::WaitCursor);
    double dT[6];
    for (int i = 0; i < 6; i++)
    {
        dT[i] = ui->tableWidget->item(i, 1)->text().toDouble();
    }

    std::vector<SHPObject *> v;
    if (m_bBiIntervisibility)
    {
        CBiIntervisibility iv(Measure::Get().GetDataDir() + "dem/90/");
        v = iv.CalIntervisibility(dT[0], dT[1], dT[3], dT[4], dT[2], dT[5]);
    }
    else
    {
        CIntervisibility iv(Measure::Get().GetDataDir() + "dem/90/");
        v = iv.CalIntervisibility(dT[0], dT[1], dT[3], dT[4], dT[2], dT[5]);
    }
    m_pShp[0] = v.front();
    m_pShp[1] = v.back();
    setCursor(Qt::ArrowCursor);
    accept();
}
