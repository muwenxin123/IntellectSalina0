#include "HeaderWidget.h"
#include "MenuBar.h"
#include <QFile>
#include <QPainter>
#include <QStyleOption>
#include <QApplication>
#include "LyIconToolButton.h"
#include <QCString.h>
#include "MenuData.h"
#include "LyWindow2.h"
#include "LyDataManager.h"
#include "LyTreeComboBox.h"

extern QString SelectiveForceTroops(int type);
extern const QStringList &selectiveForceTroops;

HeaderWidget::HeaderWidget(QWidget *mainWindow, QWidget *parent)
    : QWidget(parent)
    , m_pMainWindow(mainWindow)
    , m_pTitleLabel(new QLabel(this))
    , m_pTroopIconLabel(new QLabel(this))
    , m_pTroopLabel(new QLabel((selectiveForceTroops.size() > 0 ? SelectiveForceTroops(0) : ""), this))
    , m_pMinTBtn(new LyIconToolButton(tr("Minimize"), "minimize", this))
    , m_pCloseTBtn(new LyIconToolButton(tr("Close"), "close", this))
    , m_pSetTBtn(new LyIconToolButton(tr("Setup"), "setup", this))
    , m_pZzDateLabel(new QLabel(this))
    , m_pZzTimeLabel(new QLabel(this))
    , m_pDateLabel(new QLabel(this))
    , m_pTimeLabel(new QLabel(this))
    , m_pMenuBar(new MenuBar(8, this))
    , m_pSetMenu(new QMenu(this))
{
    initUi();

    m_pTroopLabel->setText(tr2("  智 慧 农 业"));
	m_pTroopLabel->hide();
}

HeaderWidget::~HeaderWidget()
{
}

void HeaderWidget::setWindowTitle(const QString &title)
{
    m_pTitleLabel->setText(title);
}

void HeaderWidget::setDateTime(const QDateTime &dateTime)
{
    m_pDateLabel->setText(dateTime.date().toString(tr2("yyyy-MM-dd")));
    m_pTimeLabel->setText(dateTime.time().toString("hh:mm:ss"));
}

void HeaderWidget::setDateText(const QString &date)
{
    m_pDateLabel->setText(date);
}

void HeaderWidget::setTimeText(const QString &time)
{
    m_pTimeLabel->setText(time);
}

void HeaderWidget::setTroopName(const QString &name)
{
    m_pTroopLabel->setText(name);
    m_pTroopLabel->setText(tr2("  智 慧 农 业"));
}

QAction *HeaderWidget::addSetAction(const QString &text, const QString &iconName)
{
    QAction *action = m_pSetMenu->addAction(text);

    if (!iconName.isEmpty())
    {
        action->setProperty("iconName", iconName);
    }

    m_setActions.append(action);
    return action;
}

QMenu *HeaderWidget::addSetMenu(const QString &text, const QString &iconName)
{
    QMenu *menu = m_pSetMenu->addMenu(text);

    if (!iconName.isEmpty())
    {
        menu->setProperty("iconName", iconName);
    }

    m_setMenus.append(menu);
    return menu;
}

void HeaderWidget::setIconPath(const QString &iconPath)
{
    m_strIconPath = iconPath;

    for (int i = 0; i < m_setActions.size(); ++i)
    {
        QAction *action = m_setActions[i];

        if (action)
        {
            QString iconName = action->property("iconName").toString();
            QIcon icon;
            icon.addPixmap(QPixmap(m_strIconPath.arg(iconName).arg("normal")), QIcon::Normal, QIcon::On);
            icon.addPixmap(QPixmap(m_strIconPath.arg(iconName).arg("hover")), QIcon::Active, QIcon::On);
            action->setIcon(icon);
        }
    }

    for (int i = 0; i < m_setMenus.size(); ++i)
    {
        QMenu *menu = m_setMenus[i];

        if (menu)
        {
            QString iconName = menu->property("iconName").toString();
            QIcon icon;
            icon.addPixmap(QPixmap(m_strIconPath.arg(iconName).arg("normal")), QIcon::Normal, QIcon::On);
            icon.addPixmap(QPixmap(m_strIconPath.arg(iconName).arg("hover")), QIcon::Active, QIcon::On);
            menu->setIcon(icon);
        }
    }
}

void HeaderWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QStyleOption option;
    option.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);

    QWidget::paintEvent(event);
}

bool HeaderWidget::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::MouseButtonDblClick && obj == m_pTroopLabel)
    {
        QPoint offset;
        if (LyWindow2::instance()->is2K())
        {
            offset = QPoint(3, 1);
        }
        else
        {
            offset = QPoint(3, -7);
        }
        m_treeCBox->setFixedWidth(m_pTroopIconLabel->width() + m_pTroopLabel->width());
        m_treeCBox->show();
        m_treeCBox->move(m_pTroopIconLabel->pos() + offset);
        m_treeCBox->hide();
        if (!m_treeCBox->isEmpty())
        {
            m_treeCBox->setCurrentText(m_pTroopLabel->text());
            m_treeCBox->showPopup();
        }
    }

    return QWidget::eventFilter(obj, e);
}

void HeaderWidget::initUi()
{
    m_pZzDateLabel->setObjectName("DateLabel");
    m_pZzTimeLabel->setObjectName("TimeLabel");
    m_pDateLabel->setObjectName("DateLabel");
    m_pTimeLabel->setObjectName("TimeLabel");
    m_pTimeLabel->setAlignment(Qt::AlignCenter);

    m_pTitleLabel->setObjectName("TitleLabel");
    m_pTitleLabel->setAlignment(Qt::AlignCenter);
    m_pTitleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_pTroopIconLabel->setObjectName("TroopIconLabel");
    m_pTroopLabel->setObjectName("TroopLabel");

    m_pSetTBtn->setObjectName("SetTBtn");
    m_pSetTBtn->setPopupMode(QToolButton::InstantPopup);
    m_pSetTBtn->setMenu(m_pSetMenu);
    m_pSetMenu->setObjectName("SetMenu");
    m_pSetMenu->setAttribute(Qt::WA_TranslucentBackground);
    m_pSetMenu->setWindowFlags(m_pSetMenu->windowFlags() | Qt::FramelessWindowHint);
    m_pMinTBtn->setObjectName("MinTBtn");
    m_pMinTBtn->setText(tr("Minimize"));
    connect(m_pMinTBtn, &QToolButton::clicked, m_pMainWindow, [ = ]()
    {
        m_pMainWindow->showMinimized();
#if 0
        if (QApplication::desktop()->screenCount() == 2)
        {
            FxWindow::instance()->showMinimized();
        }
#endif
    });
    MenuData md;
    md.strAction = "Quit()";
    QAction *pAction = new QAction(tr("Close"), m_pCloseTBtn);
    pAction->setData(QVariant::fromValue(md));

    m_pCloseTBtn->setDefaultAction(pAction);
    connect(m_pCloseTBtn, SIGNAL(triggered(QAction *)), m_pMainWindow, SLOT(menuActionTriggered(QAction *)));

    QHBoxLayout *pTroopLayout = new QHBoxLayout;
    pTroopLayout->setMargin(0);
    pTroopLayout->setSpacing(6);
    pTroopLayout->addWidget(m_pTroopIconLabel);
    pTroopLayout->addWidget(m_pTroopLabel, 1);

    QVBoxLayout *pTitleLayout = new QVBoxLayout;
    pTitleLayout->setMargin(0);
    pTitleLayout->setSpacing(0);
    pTitleLayout->addLayout(pTroopLayout);
    pTitleLayout->addWidget(m_pTitleLabel, 0, Qt::AlignCenter);
    pTitleLayout->addStretch();

    QHBoxLayout *pControlTBtnLayout = new QHBoxLayout;
    pControlTBtnLayout->setMargin(0);
    pControlTBtnLayout->setSpacing(15);
    pControlTBtnLayout->addStretch();
    pControlTBtnLayout->addWidget(m_pSetTBtn);
    pControlTBtnLayout->addWidget(m_pMinTBtn);
    pControlTBtnLayout->addWidget(m_pCloseTBtn);

    QHBoxLayout *pDataTimeLayout = new QHBoxLayout;
    pDataTimeLayout->setMargin(0);
    pDataTimeLayout->setSpacing(0);
    pDataTimeLayout->addStretch();
    pDataTimeLayout->addWidget(m_pZzDateLabel);
    pDataTimeLayout->addWidget(m_pZzTimeLabel);
    pDataTimeLayout->addWidget(m_pDateLabel);
    pDataTimeLayout->addWidget(m_pTimeLabel);

    QVBoxLayout *pRightLayout = new QVBoxLayout;
    pRightLayout->setMargin(0);
    pRightLayout->setSpacing(0);
    pRightLayout->addLayout(pDataTimeLayout);
    pRightLayout->addLayout(pControlTBtnLayout);
    pRightLayout->addStretch();

    QHBoxLayout *pHeaderLayout = new QHBoxLayout(this);
    pHeaderLayout->setContentsMargins(2, 0, 10, 0);
    pHeaderLayout->addLayout(pTitleLayout);
    pHeaderLayout->addSpacing(20);
    pHeaderLayout->addWidget(m_pMenuBar, 1);

    pHeaderLayout->addLayout(pRightLayout);

    m_pZzDateLabel->setText(tr2("当前时间"));
    m_pZzTimeLabel->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    m_pZzDateLabel->hide();
    m_pZzDateLabel->setVisible(false);

    m_pZzTimeLabel->hide();
    m_pZzTimeLabel->setVisible(false);

    m_pTitleLabel->hide();
    m_pTitleLabel->setVisible(false);

    m_pTroopIconLabel->hide();
    m_pTroopIconLabel->setVisible(false);

	//m_pMenuBar->hide();
	//m_pMenuBar->setVisible(false);

    m_pTroopLabel->setStyleSheet(QStringLiteral("font: 18pt \"Microsoft YaHei UI\";"));
}

QLabel *HeaderWidget::getPTroopLabel() const
{
    return m_pTroopLabel;
}

void HeaderWidget::updateTranslateUi(const QString &strMonitorResolution, const QString &strStyleName, const QString &strLanguageName)
{

    const QString titleLabelStyleSheet("QLabel#TitleLabel{ min-width: 265px; min-height: 20px; padding-top: 13px; padding-right: 8px;image: url(\"%1\"); image-position: right; }");
    const QString titleLabelImages = QStringLiteral(":/images/%1/%2/HeaderWidget/title_%3.png").arg(strMonitorResolution).arg(strStyleName).arg(strLanguageName);

    bool imagesExists = QFile::exists(titleLabelImages);
    if (imagesExists)
    {
        m_pTitleLabel->setStyleSheet(titleLabelStyleSheet.arg(titleLabelImages));
    }
    else
    {
        m_pTitleLabel->setStyleSheet(titleLabelStyleSheet.arg(":/images/1k/black/HeaderWidget/title.png"));
    }

    m_pMinTBtn->setText(tr("Minimize"));
    m_pCloseTBtn->setText(tr("Close"));
    m_pSetTBtn->setText(tr("Setup"));
}
