#include "CDlgScanSynergy02.h"
#include "ui_CDlgScanSynergy02.h"
#include "QCString.h"

CDlgScanSynergy02::CDlgScanSynergy02(QWidget *parent) :
    LyDialog(tr2("引导交接"), parent),
    ui(new Ui::CDlgScanSynergy02)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 400);

    m_Icon01.load(":/images/unfilled.png");
    m_Icon02.load(":/images/filled.png");
    m_Icon03.load(":/images/filling.png");

    for (int i = 0; i < ui->toolBox->count(); i++)
    {
        if (i == 6 || i == 7 || i == 8)
        {
            ui->toolBox->setItemIcon(i, m_Icon03);
        }
        else
        {
            ui->toolBox->setItemIcon(i, m_Icon02);
        }
    }

    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_currentIndexChanged(const QString)));
    connect(ui->lineEdit_2, SIGNAL(textChanged(const QString)), this, SLOT(on_lineEdit_2_textChanged(const QString)));
    connect(ui->lineEdit, SIGNAL(textChanged(const QString)), this, SLOT(on_lineEdit_textChanged(const QString)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));

}

CDlgScanSynergy02::~CDlgScanSynergy02()
{
    delete ui;
}

void CDlgScanSynergy02::SetLineItemText(int index, const QString &text)
{
    ui->toolBox->setItemText(index, text);
}

void CDlgScanSynergy02::InitWindow()
{
    ui->comboBox->setCurrentIndex(0);
    ui->lineEdit->setText("0");
    ui->lineEdit_2->setText("0");
    ui->toolBox->setCurrentIndex(8);
}

void CDlgScanSynergy02::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->toolBox->setItemText(6, arg1);
}

void CDlgScanSynergy02::on_lineEdit_textChanged(const QString &arg1)
{
    int dtemp = arg1.toDouble();
    ui->toolBox->setItemText(7, QString(QString::number(dtemp) + QObject::tr("次")));
}

void CDlgScanSynergy02::on_lineEdit_2_textChanged(const QString &arg1)
{
    double dtemp = arg1.toDouble();
    ui->toolBox->setItemText(8, QString(QString::number(dtemp, 'f', 2) + "%"));
}

void CDlgScanSynergy02::on_pushButton_clicked()
{
    this->hide();
}

void CDlgScanSynergy02::on_pushButton_2_clicked()
{
    this->hide();
}
