#include "IconToolButton.h"
#include <QApplication>
#include <QFile>

IconToolButton::IconToolButton(const QString &text, const QString &iconName, QWidget *parent)
    : QToolButton(parent)
    , m_strIconName(iconName)
    , m_strState("normal")
    , m_pDefaultAction(NULL)
    , m_bHasIcon(true)
    , m_bHasHover(true)
{
    setText(text);
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    updateIcon();
}

IconToolButton::IconToolButton(QAction *action, QWidget *parent)
    : QToolButton(parent)
    , m_strIconName(action->property("iconName").toString())
    , m_strState("normal")
    , m_pDefaultAction(action)
    , m_bHasIcon(true)
    , m_bHasHover(false)
{
    setText(action->text());
    setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    updateIcon();
}

IconToolButton::~IconToolButton()
{
}

void IconToolButton::updateIcon()
{
    QString iconFileName = m_strIconPath.arg(m_strIconName).arg(m_strState);
    QPixmap iconPixmap(iconFileName);

    if (!iconPixmap.isNull())
    {
        setIconSize(iconPixmap.size());

        if (m_pDefaultAction)
        {
            m_pDefaultAction->setIcon(QIcon(iconPixmap));
        }
        else
        {
            setIcon(QIcon(iconPixmap));
        }
    }
}

void IconToolButton::setIconPath(QString iconPath)
{
    m_strIconPath = iconPath;
    updateIcon();
}

bool IconToolButton::event(QEvent *event)
{
    if (!m_bHasIcon)
    {

        return QToolButton::event(event);
    }

    m_strState = "normal";
    bool updateIcon = false;

    switch (event->type())
    {
    case QEvent::HoverEnter:
    {
        m_strState = "hover";
        updateIcon = true;
    }
    break;
    case QEvent::HoverLeave:
    {
        updateIcon = true;
    }
    break;
    case QEvent::MouseButtonPress:
    {
        if (!isCheckable())
        {
            m_strState = "pressed";
        }

        updateIcon = true;
    }
    break;
    case QEvent::MouseButtonRelease:
    {
        updateIcon = true;
    }
    break;
    case QEvent::EnabledChange:
    {
        if (!isEnabled())
        {
            m_strState = "disabled";
        }

        updateIcon = true;
    }
    break;
    default:
        break;
    }

    if (updateIcon)
    {
        if (isEnabled() && isChecked())
        {
            m_strState = "checked";
        }

        this->updateIcon();
    }

    return QToolButton::event(event);
}
