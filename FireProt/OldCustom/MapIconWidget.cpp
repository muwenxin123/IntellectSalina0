#include "MapIconWidget.h"
#include "MenuData.h"
#include "QMapOpHandler.h"
#include <QAction>
#include <QEvent>
#include <QMenu>
#include <QPainter>
#include <QStyleOption>
#include <QToolButton>

MapIconWidget::MapIconWidget(QWidget *parent)
    : QWidget(parent)
    , m_vLayout(this)
{
    QMapOpHandler moh;
    moh.loadConfig("../data/conf/MainUI.cfg");
    setData(moh.getMenuData());
}

MapIconWidget::~MapIconWidget()
{
}

void MapIconWidget::setData(const QList<MenuData> &data)
{
    m_vLayout.setMargin(0);
    m_vLayout.setSpacing(6);
    m_p3DTBtn.clear();
    for (int i = 0; i < data.size(); i++)
    {
        QToolButton *pToolBtn = new QToolButton(this);
        MenuData *pMD = new MenuData;
        *pMD = data[i];
        pToolBtn->setObjectName("MapTBtn");
        pToolBtn->setUserData(0, (QObjectUserData *)pMD);
        pToolBtn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        pToolBtn->setProperty("iconName", pMD->strIcon);
        pToolBtn->setToolTip(pMD->strText);
        pToolBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
        pToolBtn->setCheckable(pMD->stage > 0 ? false : true);
        if (pMD->stage < 0)
        {
            m_btgn.append(pToolBtn);
        }
        m_vLayout.addWidget(pToolBtn);
        m_btns.append(pToolBtn);
        pToolBtn->setProperty("index", i);
        if (pMD->showOn3d)
        {
            m_p3DTBtn.append(pToolBtn);
        }
        QObject::connect(pToolBtn, SIGNAL(clicked(bool)), this, SLOT(buttonClicked(bool)));
    }
    m_vLayout.addStretch();
}

void MapIconWidget::reset()
{
    for (auto &i : m_btgn)
    {
        if (i->isChecked())
        {
            i->click();
        }
    }
}

void MapIconWidget::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MapIconWidget::setIconPath(const QString &iconPath)
{
    m_strIconPath = iconPath;
    for (auto btn : m_btns)
    {
        QString icon = btn->property("iconName").toString();
        btn->setStyleSheet(QStringLiteral("QToolButton { image: url(%1) }").arg(m_strIconPath.arg(icon)));
    }
}

void MapIconWidget::buttonClicked(bool bChecked)
{
    QToolButton *pButton = qobject_cast<QToolButton *>(sender());
    if (pButton != nullptr)
    {
        QMenu *pMenu = pButton->menu();
        if (pMenu != nullptr)
        {
            pMenu->popup(mapToGlobal(pButton->pos()));
        }
        else
        {
            MenuData *pMenuData = (MenuData *)pButton->userData(0);
            if (bChecked && m_btgn.contains(pButton))
            {
                for (auto &i : m_btgn)
                {
                    if (i->isChecked() && i != pButton)
                    {
                        i->setChecked(false);
                    }
                }
            }

            QAction action(this);
            action.setCheckable(true);
            action.setChecked(bChecked);
            if (bChecked || !m_btgn.contains(pButton))
            {
                action.setData(QVariant::fromValue(*pMenuData));
            }
            else
            {
                static MenuData md;
                if (md.strAction.isEmpty())
                {
                    md.strAction = QLatin1String("ResetMapTool()");
                }
                action.setData(QVariant::fromValue(md));
            }
            emit triggered(&action);
        }
    }
}