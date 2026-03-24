#include "QColorMenu.h"
#include <QGridLayout>
#include <QToolButton>
#include <QWidgetAction>

QColor GetColor(char colorIndex);

QColorMenu::QColorMenu(int nIdBegin, QWidget *parent): QMenu(parent)
{
    QWidgetAction *pAction = new QWidgetAction(this);
    QWidget *pWidget = new QWidget(this);
    QGridLayout *gLayout = new QGridLayout(pWidget);
    gLayout->setMargin(0);
    gLayout->setSpacing(0);

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            QPixmap pix(26, 26);
            int nIndex = i * 4 + j;
            pix.fill(GetColor(nIndex));

            QToolButton *btn = new QToolButton(pWidget);

            btn->setFixedSize(26, 26);
            QAction *act = new QAction(QIcon(pix), QString(), btn);

            act->setData(nIdBegin + nIndex);
            bool b = connect(act, &QAction::triggered, this, [&](bool checked)
            {
                QAction *pA = qobject_cast<QAction *>(sender());
                emit QMenu::triggered(pA);
            });

            btn->setDefaultAction(act);
            gLayout->addWidget(btn, i, j);
        }
    }

    pAction->setDefaultWidget(pWidget);
    addAction(pAction);
}
