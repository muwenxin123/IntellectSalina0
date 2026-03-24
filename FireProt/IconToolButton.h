#ifndef ICONTOOLBUTTON_H
#define ICONTOOLBUTTON_H

#include <QToolButton>
#include <QEvent>
#include <QAction>

class IconToolButton : public QToolButton
{
    Q_OBJECT
	Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath)

public:
    IconToolButton(const QString &text, const QString &iconName, QWidget *parent = 0);
    IconToolButton(QAction *action, QWidget *parent = 0);
    ~IconToolButton();

    Q_SLOT void updateIcon();

    QString iconPath() const
    {
        return m_strIconPath;
    }
    void setIconPath(QString iconPath);

protected:
    bool event(QEvent *event);

private:
    QString m_strIconName;
    QString m_strStyleName;
    QAction *m_pDefaultAction;
    bool m_bHasIcon;
    bool m_bHasHover;
    QString m_strState;
    QString m_strIconPath;
};

#endif