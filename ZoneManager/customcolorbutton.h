#pragma once

#ifndef CUSTOMCOLORBUTTON_H
#define CUSTOMCOLORBUTTON_H

#include <QPushButton>

class CustomColorButton: public QPushButton
{
    Q_OBJECT
public:
    CustomColorButton(QWidget *parent = nullptr);

    QColor color() const;
    void setColor(const QColor &color);
signals:
    void colorChanged(const QColor &color);
protected:
    void paintEvent(QPaintEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
private:
    QColor m_color;

    QIcon iconForBrush(QColor color);
};
#endif
