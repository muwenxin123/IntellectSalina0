#pragma once

#include <QWidget>
#include "LyDialog.h"
#include "QCString.h"
#include "ui_CDlgSetPlat.h"

namespace Ui
{
class CDlgSetPlat;
}

class CDlgSetPlat : public LyDialog
{
    Q_OBJECT

public:
    CDlgSetPlat(QWidget *parent = Q_NULLPTR);
    ~CDlgSetPlat();

    void setPlatTypeName(std::string str)
    {
        ui->platTypeName->setText(QString::fromStdString(str));
    }

    void setPlatIndex(int index)
    {
        ui->platIndex->setText(QString::number(index));
    }

    void setNames(std::vector<std::string> names);

public slots:
    void on_pushBOK_clicked();
    void on_pushBCancel_clicked();

signals:
    void sendCurrentIndex(int);
    void sendCurrentText(std::string);

private:
    Ui::CDlgSetPlat *ui;
};
