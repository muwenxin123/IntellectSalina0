#include "custompenstylecombobox.h"

#include <QList>
#include <QPair>
#include <QCString.h>
#include <QPainter>
#include <QPen>

CustomPenStyleComboBox::CustomPenStyleComboBox(QWidget *parent)
    : QComboBox(parent)
{
    QList < QPair<Qt::PenStyle, QString> > styles;
    styles << qMakePair(Qt::NoPen, tr2("不显示画笔"))
           << qMakePair(Qt::SolidLine, tr2("实线"))
           << qMakePair(Qt::DashLine, tr2("短横虚线"))
           << qMakePair(Qt::DotLine, tr2("点虚线"))
           << qMakePair(Qt::DashDotLine, tr2("短横-点虚线"))
           << qMakePair(Qt::DashDotDotLine, tr2("短横-点-点虚线"));

    setIconSize(QSize(32, 12));

    for (int i = 0; i < styles.count(); i++)
    {
        Qt::PenStyle style = styles.at(i).first;
        QString name = styles.at(i).second;
        addItem(iconForPen(style), name, QVariant((int)style));
    }
}

Qt::PenStyle CustomPenStyleComboBox::penStyle() const
{
    return (Qt::PenStyle) currentData().toInt();
}

void CustomPenStyleComboBox::setPenStyle(Qt::PenStyle style)
{
    int idx = findData(QVariant((int)style));
    setCurrentIndex(idx == -1 ? 0 : idx);
}

QIcon CustomPenStyleComboBox::iconForPen(Qt::PenStyle style)
{
    QPixmap pix(iconSize());
    QPainter p;
    pix.fill(Qt::transparent);

    p.begin(&pix);
    QPen pen(style);
    pen.setWidth(2);
    p.setPen(pen);
    double mid = iconSize().height() / 2.0;
    p.drawLine(0, mid, iconSize().width(), mid);
    p.end();

    return QIcon(pix);
}