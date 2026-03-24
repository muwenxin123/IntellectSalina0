#ifndef CUSTOMBRUSHSTYLECOMBOBOX_H
#define CUSTOMBRUSHSTYLECOMBOBOX_H

#include <QComboBox>

class  CustomBrushStyleComboBox : public QComboBox
{
    Q_OBJECT

public:
    CustomBrushStyleComboBox(QWidget *parent = nullptr);

    Qt::BrushStyle brushStyle() const;

    void setBrushStyle(Qt::BrushStyle style);

protected:
    QIcon iconForBrush(Qt::BrushStyle style);

};

#endif
