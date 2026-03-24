#include "CDlgZoneSynergy.h"
#include "ui_CDlgZoneSynergy.h"
#include "QCString.h"

CDlgZoneSynergy::CDlgZoneSynergy(QWidget *parent) :
    LyDialog(QObject::tr("SpqceSynergy"), parent),
    ui(new Ui::CDlgZoneSynergy)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(800, 400);

    m_Icon01.load(":/images/unfilled.png");
    m_Icon02.load(":/images/filled.png");
    m_Icon03.load(":/images/filling.png");

    for (int i = 0; i < ui->toolBox->count(); i++)
    {
        if (i == 2 || i == 4 || i == 6)
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

CDlgZoneSynergy::~CDlgZoneSynergy()
{
    delete ui;
}

void CDlgZoneSynergy::SetLineItemText(int index, const QString &text)
{
    ui->toolBox->setItemText(index, text);
}

void CDlgZoneSynergy::on_comboBox_currentIndexChanged(const QString &arg1)
{
    ui->toolBox->setItemText(7, arg1);
}

void CDlgZoneSynergy::on_pushButton_clicked()
{
    this->hide();
}

void CDlgZoneSynergy::on_pushButton_2_clicked()
{
    this->hide();
}
