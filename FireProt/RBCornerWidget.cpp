#include "RBCornerWidget.h"
#include <QCString.h>
#include <QTabWidget>
#include <QLabel>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>

extern QString SelectiveForceTroops(int type);
extern const QStringList& selectiveForceTroops;

RBCornerWidget::RBCornerWidget(QWidget *parent)
	: QWidget(parent)
	, m_pPopBtn(NULL)
	, m_pCloseBtn(NULL)
	, m_pParentWidget(parent)
    , m_pTabWidget(NULL)
    , m_pPreBtn(NULL)
	, m_pNextBtn(NULL)
	, m_pCurLabel(NULL)
	, m_pMenu(NULL)
	, m_pMenuBtn(NULL)
{
    this->resize(250, 120);
	initial();
	installEventFilter(this);
}

RBCornerWidget::~RBCornerWidget()
{
}

void RBCornerWidget::setTroopName(const QString &name)
{
    if(m_pCurLabel){
        m_pCurLabel->setText(name);
    }
}

void RBCornerWidget::onPre()
{

}

void RBCornerWidget::onNext()
{

}

void RBCornerWidget::onMenuChange(QAction *action)
{

}

bool RBCornerWidget::eventFilter(QObject * watched, QEvent * event)
{
	if (m_pParentWidget && watched == m_pParentWidget)
	{
		if (event->type() == QEvent::Resize)
		{
			m_pPopBtn->move(m_pParentWidget->width() - m_pPopBtn->width(), m_pParentWidget->height() - m_pPopBtn->height() - 50);
			move(m_pParentWidget->width() - width() - 2, m_pParentWidget->height() - height() - 2);
		}
	}
	else if (watched == this && event->type() == QEvent::Paint)
	{
		QPainter painter(this);
		QStyleOption opt;
		opt.init(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);
	}
	else if (watched == m_pMenu && event->type() == QEvent::Show)
	{
		QPoint menuBtnPos = mapToGlobal(m_pMenuBtn->pos());
		m_pMenu->move(menuBtnPos - QPoint(m_pMenu->width(), m_pMenu->height() - m_pMenuBtn->height()));
		return true;
	}

	return QWidget::eventFilter(watched, event);
}

void RBCornerWidget::initial()
{
	m_pPopBtn = new QPushButton(m_pParentWidget);
    m_pPopBtn->setText(tr("Fo\nrc\nes"));
	m_pPopBtn->setObjectName("ResourcePopBtn");
	m_pPopBtn->show();
	connect(m_pPopBtn, SIGNAL(clicked()), this, SLOT(onPopup()));
	m_pCloseBtn = new QPushButton(this);
	m_pCloseBtn->setObjectName("CloseBtn");
    m_pCloseBtn->setText(tr("Close"));
	connect(m_pCloseBtn, SIGNAL(clicked()), this, SLOT(onClose()));

    m_pTabWidget = new QTabWidget(this);
    m_pTabWidget->addTab(new QWidget(), tr("Airplane"));
    m_pTabWidget->addTab(new QWidget(), tr("Ammo"));

	m_pPreBtn = new QPushButton(this);
	m_pPreBtn->setObjectName("PreBtn");
    m_pPreBtn->setText(tr("Pre"));
	connect(m_pPreBtn, SIGNAL(clicked()), this, SLOT(onPre()));
	m_pNextBtn = new QPushButton(this);
	m_pNextBtn->setObjectName("NextBtn");
    m_pNextBtn->setText(tr("Next"));
	connect(m_pNextBtn, SIGNAL(clicked()), this, SLOT(onNext()));

	m_pCurLabel = new QLabel(this);
	m_pCurLabel->setObjectName("CurLabel");
    m_pCurLabel->setText(tr("Cur"));
	m_pCurLabel->setWordWrap(true);
	m_pCurLabel->setAlignment(Qt::AlignCenter);
    m_pCurLabel->setText((selectiveForceTroops.size() > 0 ? SelectiveForceTroops(0) : ""));

	m_pMenuBtn = new QPushButton(this);
	m_pMenuBtn->setObjectName("MenuBtn");
	initMenu();
	if (m_pMenu)
	{
		m_pMenuBtn->setMenu(m_pMenu);
	}

	QVBoxLayout * vLayout = new QVBoxLayout;
	vLayout->setMargin(0);
	vLayout->addWidget(m_pCloseBtn);
	vLayout->addWidget(m_pPreBtn);
	vLayout->addWidget(m_pCurLabel);
	vLayout->addWidget(m_pNextBtn);
	vLayout->addStretch();
	vLayout->addWidget(m_pMenuBtn);

	QHBoxLayout * hLayout = new QHBoxLayout(this);
	hLayout->setMargin(0);
	hLayout->setSpacing(0);
	hLayout->addLayout(vLayout);
	hLayout->addWidget(m_pTabWidget);

    this->setLayout(hLayout);
}

void RBCornerWidget::initMenu()
{
	m_pMenu = new QMenu(this);
	m_pMenu->setObjectName("Menu");
	m_pMenu->installEventFilter(this);
	m_pMenu->setWindowFlags(m_pMenu->windowFlags() | Qt::FramelessWindowHint);
	m_pMenu->setAttribute(Qt::WA_TranslucentBackground);
	connect(m_pMenu, SIGNAL(triggered(QAction*)), this, SLOT(onMenuChange(QAction*)));

    const int size = selectiveForceTroops.size();
    for(int i = 0; i < size; i++){
        m_pMenu->addAction(SelectiveForceTroops(i));
        m_pMenu->addSeparator();
    }
}

void RBCornerWidget::onPopup()
{
	m_pPopBtn->hide();
	show();
}

void RBCornerWidget::onClose()
{
	hide();
	m_pPopBtn->show();
}

void RBCornerWidget::updateTranslateUi()
{
    if(m_pCurLabel == nullptr || m_pPopBtn == nullptr || m_pTabWidget == nullptr){
        return ;
    }

    m_pPopBtn->setText(tr("Fo\nrc\nes"));

    m_pTabWidget->setTabText(0, tr("Airplane"));
    m_pTabWidget->setTabText(1, tr("Ammo"));

    if(m_pMenu){
        QList<QAction*>  troopActionAndSeparatorList = m_pMenu->actions();
        QList<QAction*>  troopActionList;
        for(int i = 0; i < troopActionAndSeparatorList.size(); i++){
            if(!troopActionAndSeparatorList[i]->isSeparator()){
                troopActionList.push_back(troopActionAndSeparatorList[i]);
            }
        }

        const int size = qMin(selectiveForceTroops.size(), troopActionList.size());
        for(int i = 0; i < size; i++){
            QAction* troopAction = troopActionList[i];
            troopAction->setText(SelectiveForceTroops(i));
        }
    }
}

