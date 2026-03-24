#include "CDlgPlannedSynergy.h"
#include "ui_CDlgPlannedSynergy.h"
#include "QCString.h"

CDlgPlannedSynergy::CDlgPlannedSynergy(QWidget *parent) :
	LyDialog(QObject::tr("PlannedSynergy"), parent),
	ui(new Ui::CDlgPlannedSynergy)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(800, 400);

    m_Icon01.load(":/images/unfilled.png");
    m_Icon02.load(":/images/filled.png");
	m_Icon03.load(":/images/filling.png");

	for (int i = 0; i < ui->toolBox->count(); i++)
	{
		if (i == 2 || i == 7 )
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

CDlgPlannedSynergy::~CDlgPlannedSynergy()
{
    delete ui;
}

void CDlgPlannedSynergy::SetLineItemText(int index, const QString &text)
{
    ui->toolBox->setItemText(index, text);
}

void CDlgPlannedSynergy::on_comboBox_currentIndexChanged(const QString &arg1)
{
	ui->toolBox->setItemText(7, arg1);
}

void CDlgPlannedSynergy::on_pushButton_2_clicked()
{
	this->hide();

}

void CDlgPlannedSynergy::on_pushButton_clicked()
{
	this->hide();
}
