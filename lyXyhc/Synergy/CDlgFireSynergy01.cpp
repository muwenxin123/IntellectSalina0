#include "CDlgFireSynergy01.h"
#include "ui_CDlgFireSynergy01.h"
#include "QCString.h"

CDlgFireSynergy01::CDlgFireSynergy01(QWidget *parent) :
    LyDialog(QObject::tr("SynergyTrackMarkMessage"), parent),
    ui(new Ui::CDlgFireSynergy01)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 400);

    m_Icon01.load(":/images/unfilled.png");
    m_Icon02.load(":/images/filled.png");
    m_Icon03.load(":/images/filling.png");

    for (int i = 0; i < ui->toolBox->count(); i++)
    {
        if (i == 1 || i == 2 || i == 3)
        {
            ui->toolBox->setItemIcon(i, m_Icon03);
        }
        else
        {
            ui->toolBox->setItemIcon(i, m_Icon02);
        }
    }

    connect(ui->comboBox, SIGNAL(currentTextChanged(const QString)), this, SLOT(on_comboBox_currentTextChanged(const QString)));
    connect(ui->lineEdit_2, SIGNAL(textChanged(const QString)), this, SLOT(on_lineEdit_2_textChanged(const QString)));
    connect(ui->lineEdit, SIGNAL(textChanged(const QString)), this, SLOT(on_lineEdit_textChanged(const QString)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
}

CDlgFireSynergy01::~CDlgFireSynergy01()
{
    delete ui;
}

void CDlgFireSynergy01::SetLineItemText(int index, const QString &text)
{
    ui->toolBox->setItemText(index, text);
}

void CDlgFireSynergy01::InitWindow()
{
    ui->lineEdit_2->setText("-");
    ui->comboBox->setCurrentIndex(0);
    ui->lineEdit->setText("100");
    ui->toolBox->setCurrentIndex(8);
}

void CDlgFireSynergy01::on_lineEdit_2_textChanged(const QString &arg1)
{
    SetLineItemText(1, arg1);
}

void CDlgFireSynergy01::on_lineEdit_textChanged(const QString &arg1)
{
    double dtemp = arg1.toDouble();
    ui->toolBox->setItemText(3, QString(QString::number(dtemp, 'f', 2) + "%"));

}

void CDlgFireSynergy01::on_comboBox_currentTextChanged(const QString &arg1)
{
    SetLineItemText(2, arg1);
}

void CDlgFireSynergy01::on_pushButton_clicked()
{
    this->hide();
}

void CDlgFireSynergy01::on_pushButton_2_clicked()
{
    this->hide();
}
