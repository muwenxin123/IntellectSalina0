#include "CButton_setPlat.h"
#include <QMouseEvent>

CButton_setPlat::CButton_setPlat(QWidget *parent)
    : QPushButton(parent)
{
    platWidget = NULL;
}

CButton_setPlat::CButton_setPlat(std::string typeName, int index, std::string nameStr, std::vector<std::string> names)
{
    platTypeName = typeName;
    platIndex = index;
    platName = nameStr;
    platNames = names;
    this->setText(QString::fromStdString(nameStr));
    connect(this, &CButton_setPlat::buttonDoubleClicked, this, &CButton_setPlat::on_push_doubleClicked);
    platWidget = NULL;
}

void CButton_setPlat::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->buttons() == Qt::LeftButton)
    {
        emit(buttonDoubleClicked());
    }
}

CButton_setPlat::~CButton_setPlat()
{
}

void CButton_setPlat::on_push_doubleClicked()
{
    if (platWidget == NULL)
    {
        platWidget = new CDlgSetPlat();
        connect(platWidget, SIGNAL(sendCurrentText(std::string)), this, SLOT(setPlatName(std::string)));
        platWidget->setPlatTypeName(platTypeName);
        platWidget->setPlatIndex(platIndex);
        platWidget->setNames(platNames);
    }
    platWidget->show();
}

void CButton_setPlat::setPlatName(std::string str)
{
    platName = str;
    this->setText(QString::fromStdString(str));
    if (true)
    {
        emit(buttonTextChanged(str));
    }
}
