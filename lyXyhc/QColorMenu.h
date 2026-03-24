#ifndef QCOLORMENU_H
#define QCOLORMENU_H

#include <QMenu>

class QColorMenu : public QMenu
{
public:
    QColorMenu(int nIdBegin = 0, QWidget *parent = nullptr);
};

#endif