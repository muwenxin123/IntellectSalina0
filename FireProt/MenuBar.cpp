#include "MenuBar.h"
#include <QLabel>
#include <QStyle>
#include "LyIconToolButton.h"

MenuBar::MenuBar(int maxVisibleMenuCount, QWidget *parent)
	: QWidget(parent)
	, m_iMaxVisibleMenuCount(maxVisibleMenuCount)
	, m_strLineColor("#fbb341")
{
	if (m_iMaxVisibleMenuCount < 5)
	{
		m_iMaxVisibleMenuCount = 5;
	}

	if (m_iMaxVisibleMenuCount > 15)
	{
		m_iMaxVisibleMenuCount = 15;
	}

	setObjectName("MenuBar");
	initUi();
}

MenuBar::~MenuBar()
{
}

int MenuBar::addMenu(const QString & menu)
{
	int index = m_rootMenuTBtns.size();

	QToolButton * pToolBtn = new QToolButton;
	pToolBtn->setProperty("type", "RootMenuTBtn");
	pToolBtn->setText(menu);
	pToolBtn->setCheckable(true);
	pToolBtn->hide();

	if (index >= 1 && index < m_iMaxVisibleMenuCount)
	{
		QFrame *pVLine = new QFrame(this);
		pVLine->setFrameShape(QFrame::VLine);
		m_pHRootMenuLayout->addWidget(pVLine);
	}
	else if (index == 0)
	{
		pToolBtn->setChecked(true);
	}

	m_pHRootMenuLayout->addWidget(pToolBtn);

	m_rootMenuTBtns.append(pToolBtn);
	m_pRootButtonGroup->addButton(pToolBtn, index);

	QWidget * pChildBarWidget = new QWidget;
	QHBoxLayout * hLayout = new QHBoxLayout(pChildBarWidget);
	hLayout->setMargin(0);
	QHBoxLayout * hBtnsLayout = new QHBoxLayout;
	hBtnsLayout->setContentsMargins(550, 20, 0, 0);
	hBtnsLayout->setSpacing(40);
	hLayout->addLayout(hBtnsLayout);
	hLayout->addStretch();

	int id = m_pChildBarStackedWidget->addWidget(pChildBarWidget);
	m_pChildMenuHLayouts[index] = hBtnsLayout;

	if (index >= m_iMaxVisibleMenuCount - 1)
	{
		QAction * action = m_pMoreMenu->addAction(m_rootMenuTBtns[index]->text());
		action->setProperty("index", index);
		action->setCheckable(true);
		m_pMoreMenuActionGroup->addAction(action);

		if (index > m_iMaxVisibleMenuCount - 1)
		{
			m_rootMenuTBtns[index]->setVisible(m_rootMenuTBtns[index]->isChecked());
		}
		action->setChecked(m_rootMenuTBtns[index]->isVisible());

		connect(action, SIGNAL(triggered(bool)), m_rootMenuTBtns[index], SLOT(setChecked(bool)));
		connect(m_rootMenuTBtns[index], SIGNAL(clicked(bool)), action, SLOT(setChecked(bool)));
		m_pMoreTBtn->show();
	}

	return index;
}

void MenuBar::addAction(int menuId, QAction * action)
{
	if (!m_pChildMenuHLayouts.contains(menuId) || action == NULL)
	{
		return;
	}

	LyIconToolButton * pToolBtn = new LyIconToolButton(action);
	pToolBtn->setDefaultAction(action);
	pToolBtn->updateIcon();

	m_pChildQButtonGroup->addButton(pToolBtn, buttonNum++);

	if (m_pChildMenuHLayouts[menuId])
	{
		m_pChildMenuHLayouts[menuId]->addWidget(pToolBtn);
	}

	m_pActionGroup->addAction(action);
}

QAction * MenuBar::addAction(int menuId, const QString & text, const QString & iconName)
{
	QAction * pAction = new QAction(text, this);
	pAction->setProperty("iconName", iconName);
	addAction(menuId, pAction);
	return pAction;
}

void MenuBar::setLineColor(const QString & colorName)
{
	m_strLineColor = colorName;
	rootMenuChanged(m_pRootButtonGroup->checkedButton());
}

void MenuBar::slotButtonStyleShift(int index)
{
	auto buttonList = m_pChildQButtonGroup->buttons();
	int nSize = buttonList.size();
	for (int i = 0; i < nSize; i++) {
		auto bt = buttonList.at(i);
		if (i == index) {
			bt->setStyleSheet("QToolButton{border: none; background: transparent; color: #9bf4fd; font-size: 12px; height: 28px; border-image: none;}");
		}
		else {
			bt->setStyleSheet("QToolButton{border: none; background: transparent; color: #dfeef3; font-size: 12px; height: 28px; border-image: none;}");
		}
	}
}

void MenuBar::showEvent(QShowEvent * event)
{
	QWidget::showEvent(event);
	rootMenuChanged(m_pRootButtonGroup->checkedButton());
}

void MenuBar::initUi()
{
	m_pHRootMenuLayout = new QHBoxLayout;
	m_pHRootMenuLayout->setContentsMargins(14, 0, 0, 0);
	m_pHRootMenuLayout->setSpacing(14);

	m_pMoreTBtn = new QToolButton(this);
	m_pMoreTBtn->setObjectName("MoreTBtn");
	m_pMoreTBtn->setPopupMode(QToolButton::InstantPopup);
	m_pMoreTBtn->hide();
	m_pMoreMenu = new QMenu(m_pMoreTBtn);
	m_pMoreTBtn->setMenu(m_pMoreMenu);
	connect(m_pMoreMenu, SIGNAL(triggered(QAction*)), this, SLOT(moreMenuChanged(QAction*)));
	m_pMoreMenuActionGroup = new QActionGroup(this);
	m_pMoreMenuActionGroup->setExclusive(true);

	QHBoxLayout * pRootLayout = new QHBoxLayout;
	pRootLayout->setMargin(0);
	pRootLayout->setSpacing(14);
	pRootLayout->addLayout(m_pHRootMenuLayout);
	pRootLayout->addWidget(m_pMoreTBtn);
	pRootLayout->addStretch();

	m_pChildBarStackedWidget = new QStackedWidget;
	m_pChildBarStackedWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

	m_pHLine = new QFrame(this);
	m_pHLine->setFrameShape(QFrame::HLine);
	m_pHLine->setObjectName("HLine");

	QVBoxLayout * pMainVLayout = new QVBoxLayout(this);
	pMainVLayout->setMargin(0);
	pMainVLayout->setSpacing(0);
	pMainVLayout->addLayout(pRootLayout);
	pMainVLayout->addWidget(m_pHLine);

	pMainVLayout->addWidget(m_pChildBarStackedWidget);
	pMainVLayout->addStretch();

	m_pRootButtonGroup = new QButtonGroup(this);
	connect(m_pRootButtonGroup, SIGNAL(buttonClicked(int)), m_pChildBarStackedWidget, SLOT(setCurrentIndex(int)));
	connect(m_pRootButtonGroup, SIGNAL(buttonClicked(QAbstractButton*)), this, SLOT(rootMenuChanged(QAbstractButton*)));

	m_pChildQButtonGroup = new QButtonGroup(this);
	connect(m_pChildQButtonGroup, SIGNAL(buttonClicked(int)), this, SLOT(slotButtonStyleShift(int)));

	m_pActionGroup = new QActionGroup(this);
	m_pActionGroup->setExclusive(false);
	bool flag = connect(m_pActionGroup, SIGNAL(triggered(QAction *)), this, SIGNAL(triggered(QAction *)));
}

void MenuBar::rootMenuChanged(QAbstractButton * btn)
{
	if (btn == NULL) return;

	int btnPos = btn->pos().x() + btn->width() / 2;
	qreal lineWidth = m_pHLine->width();
	int linePos = m_pHLine->pos().x();

	qreal stop = (btnPos - linePos) / lineWidth;
	m_pHLine->setStyleSheet(QString("background: qlineargradient(x1: 0, y1: 0, x2: 1, y2: 0, stop: 0 transparent, stop: %1 %2, stop: 1 transparent);").arg(stop).arg(m_strLineColor));
}

void MenuBar::moreMenuChanged(QAction * action)
{
	int index = action->property("index").toInt();

	if (index < 0 || index >= m_rootMenuTBtns.size()) return;

	for (int i = m_iMaxVisibleMenuCount - 1; i < m_rootMenuTBtns.size(); ++i)
	{
		m_rootMenuTBtns[i]->hide();
	}

	m_rootMenuTBtns[index]->show();
	rootMenuChanged(m_rootMenuTBtns[index]);
	m_pChildBarStackedWidget->setCurrentIndex(index);
}
