#include "QGridIconWidget.h"
#include <QAction>
#include <QMenu>
#include <QToolButton>
#include "MenuData.h"
#include "QMapOpHandler.h"

QGridIconWidget::QGridIconWidget(QWidget *parent)
	: QWidget(parent), m_Grid(this)
{
	QMapOpHandler moh;
	moh.loadConfig("../data/conf/MainUI.cfg");
	setData(moh.getMenuData());
}

QGridIconWidget::~QGridIconWidget()
{
}

void QGridIconWidget::setData(const QList<MenuData> &data)
{
	m_Grid.setMargin(0);
	m_Grid.setSpacing(3);

	for (int i = 0; i < 10; i++)
	{
		int row = i;
		int col = 0;
		if (i < data.size())
		{
			QToolButton* pToolBtn = new QToolButton(this);
			MenuData *pMD = new MenuData;
			*pMD = data[i];
			pToolBtn->setObjectName("HalfBG");
			pToolBtn->setUserData(0, (QObjectUserData*)pMD);
			pToolBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			pToolBtn->setIcon(QIcon(data[i].strIcon));

			pToolBtn->setIconSize(QSize(32, 32));
			pToolBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
			QObject::connect(pToolBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
			m_Grid.addWidget(pToolBtn, row, col);
		}
		else
		{
			QToolButton* pToolBtn = new QToolButton(this);
			pToolBtn->setObjectName("HalfBG");
			pToolBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
			pToolBtn->setMinimumHeight(32);
			m_Grid.addWidget(pToolBtn, row, col);

			if (i == 9)
			{
				pToolBtn->setCheckable(true);
				connect(pToolBtn, SIGNAL(clicked(bool)), this, SIGNAL(fullScreen(bool)));
			}
		}
	}
}

void QGridIconWidget::buttonClicked(bool)
{
	QToolButton *pButton = dynamic_cast<QToolButton*>(sender());
	if (pButton != NULL)
	{
		QMenu *pMenu = pButton->menu();
		if (pMenu != NULL)
		{
			pMenu->popup(mapToGlobal(pButton->pos()));
		}
		else
		{
			MenuData *pMenuData = (MenuData*)pButton->userData(0);
			QAction action;
			action.setData(QVariant::fromValue(*pMenuData));
			emit triggered(&action);
		}
	}
}
