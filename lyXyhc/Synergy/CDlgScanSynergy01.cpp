#include "CDlgScanSynergy01.h"
#include "ui_CDlgScanSynergy01.h"
#include "QCString.h"

CDlgScanSynergy01::CDlgScanSynergy01(QWidget *parent) :
    LyDialog(tr2("Ð­Í¬Ì½²â"), parent),
    ui(new Ui::CDlgScanSynergy01)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 400);

    m_Icon01.load(":/images/unfilled.png");
    m_Icon02.load(":/images/filled.png");
    m_Icon03.load(":/images/filling.png");

    for (int i = 0; i < ui->toolBox->count(); i++)
    {
        if (i == 4 || i == 5 || i == 6)
        {
            ui->toolBox->setItemIcon(i, m_Icon03);
        }
        else
        {
            ui->toolBox->setItemIcon(i, m_Icon02);
        }
    }

    connect(ui->lineEdit_3, SIGNAL(textChanged(const QString)), this, SLOT(on_lineEdit_3_textChanged(const QString)));
    connect(ui->lineEdit_2, SIGNAL(textChanged(const QString)), this, SLOT(on_lineEdit_2_textChanged(const QString)));
    connect(ui->lineEdit, SIGNAL(textChanged(const QString)), this, SLOT(on_lineEdit_textChanged(const QString)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
}

CDlgScanSynergy01::~CDlgScanSynergy01()
{
    delete ui;
}

void CDlgScanSynergy01::SetLineItemText(int index, const QString &text)
{
    ui->toolBox->setItemText(index, text);
}

void CDlgScanSynergy01::InitEdit()
{
    ui->lineEdit->setText("0");
    ui->lineEdit_2->setText("0");
    ui->lineEdit_3->setText("0");
    ui->toolBox->setCurrentIndex(6);
}

void CDlgScanSynergy01::on_lineEdit_3_textChanged(const QString &arg1)
{
    double dtemp = arg1.toDouble();
    ui->toolBox->setItemText(4, QString(QString::number(dtemp, 'f', 2) + QObject::tr("¡ã")));
}

void CDlgScanSynergy01::on_lineEdit_textChanged(const QString &arg1)
{
    double dtemp = arg1.toDouble();
    ui->toolBox->setItemText(5, QString(QString::number(dtemp, 'f', 2) + "%"));
}

void CDlgScanSynergy01::on_lineEdit_2_textChanged(const QString &arg1)
{
    double dtemp = arg1.toDouble();
    ui->toolBox->setItemText(6, QString(QString::number(dtemp, 'f', 2) + "%"));
}

void CDlgScanSynergy01::on_pushButton_clicked()
{
    this->hide();
}

void CDlgScanSynergy01::on_pushButton_2_clicked()
{
    this->hide();
}
