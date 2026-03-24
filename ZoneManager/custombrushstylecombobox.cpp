#include "custombrushstylecombobox.h"

#include <QList>
#include <QPair>
#include <QCString.h>
#include <QBrush>
#include <QPainter>
#include <QPen>

CustomBrushStyleComboBox::CustomBrushStyleComboBox(QWidget *parent)
    : QComboBox(parent)
{
    QList < QPair<Qt::BrushStyle, QString> > styles;
    styles << qMakePair(Qt::SolidPattern, tr2("实线"))
           << qMakePair(Qt::NoBrush, tr2("没有画刷"))
           << qMakePair(Qt::HorPattern, tr2("水平"))
           << qMakePair(Qt::VerPattern, tr2("垂直"))
           << qMakePair(Qt::CrossPattern, tr2("十字"))
           << qMakePair(Qt::BDiagPattern, tr2("斜线"))
           << qMakePair(Qt::FDiagPattern, tr2("反斜线"))
           << qMakePair(Qt::DiagCrossPattern, tr2("X斜线"))
           << qMakePair(Qt::Dense1Pattern, tr2("密集 1"))
           << qMakePair(Qt::Dense2Pattern, tr2("密集 2"))
           << qMakePair(Qt::Dense3Pattern, tr2("密集 3"))
           << qMakePair(Qt::Dense4Pattern, tr2("密集 4"))
           << qMakePair(Qt::Dense5Pattern, tr2("密集 5"))
           << qMakePair(Qt::Dense6Pattern, tr2("密集 6"))
           << qMakePair(Qt::Dense7Pattern, tr2("密集 7"));

    setIconSize(QSize(32, 16));

    for (int i = 0; i < styles.count(); i++)
    {
        Qt::BrushStyle style = styles.at(i).first;
        QString name = styles.at(i).second;
        addItem(iconForBrush(style), name, QVariant(static_cast<int>(style)));
    }

    setCurrentIndex(1);

}

Qt::BrushStyle CustomBrushStyleComboBox::brushStyle() const
{
    return (Qt::BrushStyle) currentData().toInt();
}

void CustomBrushStyleComboBox::setBrushStyle(Qt::BrushStyle style)
{
    int idx = findData(QVariant(static_cast<int>(style)));
    setCurrentIndex(idx == -1 ? 0 : idx);
}

QIcon CustomBrushStyleComboBox::iconForBrush(Qt::BrushStyle style)
{
    QPixmap pix(iconSize());
    QPainter p;
    pix.fill(Qt::transparent);

    p.begin(&pix);
    QBrush brush(QColor(100, 100, 100), style);
    p.setBrush(brush);
    QPen pen(Qt::NoPen);
    p.setPen(pen);
    p.drawRect(QRect(QPoint(0, 0), iconSize()));
    p.end();

    return QIcon(pix);
}
