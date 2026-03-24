#pragma once
#ifndef CUSTOMPENSTYLE_H
#define CUSTOMPENSTYLE_H

#include <QComboBox>

class CustomPenStyleComboBox : public QComboBox
{
    Q_OBJECT

public:
    CustomPenStyleComboBox(QWidget *parent = nullptr);

    Qt::PenStyle penStyle() const;

    void setPenStyle(Qt::PenStyle style);

protected:
    QIcon iconForPen(Qt::PenStyle style);

};
#endif
