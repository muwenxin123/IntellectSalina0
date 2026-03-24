#include "CDlgMessageBox.h"
#include "QCString.h"
#include<QPushButton>
CDlgMessageBox::CDlgMessageBox(QWidget *parent)
	: LyDialog(tr("tip"), parent),
	ui(new Ui::CDlgMessageBox)
{
	ui->setupUi(centralWidget());
	connect(ui->btnok, &QPushButton::clicked, this, &CDlgMessageBox::on_btnok_clicked);
}

CDlgMessageBox::~CDlgMessageBox()
{
}
void CDlgMessageBox::on_btnok_clicked()
{
	close();
}