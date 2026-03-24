#include "CDlgInfoSynergy.h"
#include "ui_CDlgInfoSynergy.h"
#include "QCString.h"

CDlgInfoSynergy::CDlgInfoSynergy(QWidget *parent) :
    LyDialog(QObject::tr("InfoSynergyAction"), parent),
    ui(new Ui::CDlgInfoSynergy)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 400);

    m_Icon01.load(":/images/unfilled.png");
    m_Icon02.load(":/images/filled.png");
    m_Icon03.load(":/images/filling.png");

    for (int i = 0; i < ui->toolBox->count(); i++)
    {
        if (i == 2 || i == 4 || i == 3)
        {
            ui->toolBox->setItemIcon(i, m_Icon03);
        }
        else
        {
            ui->toolBox->setItemIcon(i, m_Icon02);
        }
    }

    connect(ui->comboBox, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_currentIndexChanged(const QString)));
    connect(ui->comboBox_2, SIGNAL(currentIndexChanged(const QString)), this, SLOT(on_comboBox_2_currentIndexChanged(const QString)));
    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
}

CDlgInfoSynergy::~CDlgInfoSynergy()
{
    delete ui;
}

void CDlgInfoSynergy::SetLineItemText(int index, const QString &text)
{
    ui->toolBox->setItemText(index, text);
}

void CDlgInfoSynergy::InitCombobox()
{
    ui->comboBox_2->setCurrentIndex(0);
    ui->comboBox->setCurrentIndex(0);
    ui->toolBox->setCurrentIndex(6);
}

void CDlgInfoSynergy::on_comboBox_2_currentIndexChanged(const QString &arg1)
{
    ui->toolBox->setItemText(5, arg1);

}

void CDlgInfoSynergy::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->toolBox->setItemText(6, arg1);
}

void CDlgInfoSynergy::on_pushButton_clicked()
{
    this->hide();
}

void CDlgInfoSynergy::on_pushButton_2_clicked()
{
    this->hide();
}
