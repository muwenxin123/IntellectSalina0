#include "CDlgTimeSynergy.h"
#include "ui_CDlgTimeSynergy.h"
#include "QCString.h"

CDlgTimeSynergy::CDlgTimeSynergy(QWidget *parent) :
    LyDialog(tr2("时间协同"), parent),
    ui(new Ui::CDlgTimeSynergy)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 400);

    m_Icon01.load(":/images/unfilled.png");
    m_Icon02.load(":/images/filled.png");
    m_Icon03.load(":/images/filling.png");

    for (int i = 0; i < ui->toolBox->count(); i++)
    {
        if (i == 3)
        {
            ui->toolBox->setItemIcon(i, m_Icon03);
        }
        else
        {
            ui->toolBox->setItemIcon(i, m_Icon02);
        }
    }
    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_currentIndexChanged(const QString)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));

    ui->toolBox->setCurrentIndex(8);
}

CDlgTimeSynergy::~CDlgTimeSynergy()
{
    delete ui;
}

void CDlgTimeSynergy::SetLineItemText(int index, const QString &text)
{
    ui->toolBox->setItemText(index, text);
}

void CDlgTimeSynergy::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->toolBox->setItemText(7, arg1);
}

void CDlgTimeSynergy::on_pushButton_clicked()
{
    this->hide();
}

void CDlgTimeSynergy::on_pushButton_2_clicked()
{
    this->hide();
}