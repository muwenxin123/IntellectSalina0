#include "CDlgFireSynergy02.h"
#include "ui_CDlgFireSynergy02.h"
#include "QCString.h"

CDlgFireSynergy02::CDlgFireSynergy02(QWidget *parent) :
	LyDialog(QObject::tr("SynergyGuideControlMessage"), parent),
	ui(new Ui::CDlgFireSynergy02)
{
	ui->setupUi(centralWidget());
	this->setResizable(true);
	this->resize(800, 400);

	m_Icon01.load(":/images/unfilled.png");
	m_Icon02.load(":/images/filled.png");
	m_Icon03.load(":/images/filling.png");

	for (int i = 0; i < ui->toolBox->count(); i++)
	{
		if (i == 7 || i == 8 )
		{
			ui->toolBox->setItemIcon(i, m_Icon03);
		}
		else
		{
			ui->toolBox->setItemIcon(i, m_Icon02);
		}
	}

	connect(ui->comboBox, SIGNAL(currentTextChanged(const QString)), this, SLOT(on_comboBox_currentTextChanged(const QString)));
	connect(ui->lineEdit, SIGNAL(textChanged(const QString)), this, SLOT(on_lineEdit_textChanged(const QString)));
	connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
	connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
}

CDlgFireSynergy02::~CDlgFireSynergy02()
{
    delete ui;
}

void CDlgFireSynergy02::SetLineItemText(int index, const QString &text)
{
	ui->toolBox->setItemText(index, text);
}

void CDlgFireSynergy02::InitWindow()
{
	ui->comboBox->setCurrentIndex(0);
	ui->lineEdit->setText(QObject::tr("AssistAttack"));
	ui->toolBox->setCurrentIndex(8);
}

void CDlgFireSynergy02::on_lineEdit_textChanged(const QString &arg1)
{
	SetLineItemText(7, arg1);
}

void CDlgFireSynergy02::on_pushButton_clicked()
{
	this->hide();
}

void CDlgFireSynergy02::on_pushButton_2_clicked()
{
	this->hide();
}

void CDlgFireSynergy02::on_comboBox_currentTextChanged(const QString &arg1)
{
	SetLineItemText(8, arg1);
}
