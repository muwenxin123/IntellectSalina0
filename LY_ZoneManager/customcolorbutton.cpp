#include "customcolorbutton.h"

#include <QColor>
#include <QPainter>
#include <QColorDialog>

CustomColorButton::CustomColorButton(QWidget *parent ): QPushButton(parent)
{
    setAutoFillBackground(true);
}

QColor CustomColorButton::color() const
{
    return m_color;
}

void CustomColorButton::setColor(const QColor &color)
{
    m_color = color;
    update();
}

void CustomColorButton::paintEvent(QPaintEvent *e)
{
    QPixmap pixmap = iconForBrush(m_color).pixmap(size(), QIcon::Normal, QIcon::On);
    QPainter painter;
    painter.begin(this);
    int w = this->width();
    int h = this->height();
    painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
    painter.end();
}

void CustomColorButton::mouseReleaseEvent(QMouseEvent *e)
{
    QColor color = QColorDialog::getColor();
    if (color.isValid())
    {
        m_color = color;
        update();
        emit colorChanged(m_color);
        QPushButton::mouseReleaseEvent(e);
    }
}

QIcon CustomColorButton::iconForBrush(QColor color)
{
    QSize s = size();
    QPixmap pix(size());
    QPainter p;
    pix.fill(Qt::transparent);

    p.begin(&pix);
    QBrush brush(color, Qt::SolidPattern);
    p.setBrush(brush);
    QPen pen(Qt::NoPen);
    p.setPen(pen);
    p.drawRect(QRect(QPoint(0, 0), size()));
    p.end();

    return QIcon(pix);
}
