#include "CDlgSetPlat.h"

CDlgSetPlat::CDlgSetPlat(QWidget *parent) :
    LyDialog(tr("SetPlat"), parent),
    ui(new Ui::CDlgSetPlat)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);

    this->setWindowTitle(tr("Set Platform"));
    connect(ui->PBOk, &QPushButton::clicked, this, &CDlgSetPlat::on_pushBOK_clicked);
    connect(ui->PBCancel, &QPushButton::clicked, this, &CDlgSetPlat::on_pushBCancel_clicked);
}

CDlgSetPlat::~CDlgSetPlat()
{
}

void CDlgSetPlat::setNames(std::vector<std::string> names)
{
    ui->comboBoxPlatName->clear();
    ui->comboBoxPlatName->addItem(QStringLiteral(""));
    for (int i = 0; i < names.size(); i++)
    {
        ui->comboBoxPlatName->addItem(QString::fromStdString(names[i]));
    }
    ui->comboBoxPlatName->setCurrentIndex(0);
}

void CDlgSetPlat::on_pushBOK_clicked()
{
    emit(sendCurrentIndex(ui->comboBoxPlatName->currentIndex()));
    emit(sendCurrentText(ui->comboBoxPlatName->currentText().toStdString()));
    this->close();
}
void CDlgSetPlat::on_pushBCancel_clicked()
{
    this->close();
}
