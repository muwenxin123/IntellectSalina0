#include "latlineedit.h"

#include <QRegExpValidator>

LatLineEdit::LatLineEdit(QWidget *parent): QLineEdit(parent)
{
    QRegExp coorlatValidate("([0-8]?\\d{1}\\.[0-5]\\d'[0-5]\\d\"|90\\.0{2}'0{2}\")");
    QValidator *latValidator = new QRegExpValidator(coorlatValidate, this);
    this->setValidator(latValidator);
    this->setInputMask("00.00'00\";0");
}
