#include "lonlineedit.h"

#include <QRegExpValidator>
LonLineEdit::LonLineEdit(QWidget *parent ): QLineEdit(parent)
{
    QRegExp coorlonValidate("(0?\\d{1,2}\\.[0-5]\\d'[0-5]\\d\"|1[0-7]\\d{1}\\.[0-5]\\d'[0-5]\\d\"|180\\.0{2}'0{2}\")");
    QValidator *lonValidator = new QRegExpValidator(coorlonValidate, this);
    this->setValidator(lonValidator);
    this->setInputMask("000.00'00\";0");
}

