#pragma once

#include <QPushButton>
#include "string.h"
#include "QCString.h"
#include "CDlgSetPlat.h"

class CButton_setPlat : public QPushButton
{
    Q_OBJECT

public:
    CButton_setPlat(QWidget *parent);
    ~CButton_setPlat();

    CButton_setPlat(std::string typeName, int index, std::string nameStr, std::vector<std::string> names);

    void setPlatTypeName(std::string str)
    {
        platTypeName = str;
    }
    std::string getPlatTypeName()
    {
        return platTypeName;
    }

    void setPlatIndex(int index)
    {
        platIndex = index;
    }
    int getPlatIndex()
    {
        return platIndex;
    }

    std::string getPlatName()
    {
        return platName;
    }

    void setNames(std::vector<std::string> names)
    {
        platNames = names;
    }
    std::vector<std::string> getNames()
    {
        return platNames;
    }

public slots :
    void on_push_doubleClicked();
    void setPlatName(std::string str);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void buttonDoubleClicked();
    void buttonTextChanged(std::string);
private:
    std::string platTypeName;
    int platIndex;
    std::string platName;
    std::vector<std::string> platNames;
    CDlgSetPlat *platWidget;
};
