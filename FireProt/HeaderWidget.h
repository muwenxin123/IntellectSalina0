#ifndef HEADERWIDGET_H
#define HEADERWIDGET_H

#include <QWidget>
#include <QToolButton>
#include <QLabel>
#include <QDateTime>
#include <QHBoxLayout>

class MenuBar;
class LyTreeComboBox;

class HeaderWidget : public QWidget
{
    Q_OBJECT
	Q_PROPERTY(QString iconPath READ iconPath WRITE setIconPath)

public:
    HeaderWidget(QWidget *mainWindow, QWidget *parent = NULL);
    ~HeaderWidget();

    void setWindowTitle(const QString &title);

    void setTranslationEnabled(bool bEnable = true) {   }

    void setDateTime(const QDateTime &dateTime);

    void setDateText(const QString &date);

    void setTimeText(const QString &time);

    void setTroopName(const QString &name);

    MenuBar *menuBar()
    {
        return m_pMenuBar;
    }

    QAction *addSetAction(const QString &text, const QString &iconName = QString());

    QMenu *addSetMenu(const QString &text, const QString &iconName = QString());

    QString iconPath() const
    {
        return m_strIconPath;
    }
    void setIconPath(const QString &iconPath);

    QToolButton *getPCloseTBtn() const
    {
        return m_pCloseTBtn;
    }

    void updateTranslateUi(const QString &strMonitorResolution, const QString &strStyleName, const QString &strLanguageName);

    QLabel *getPTroopLabel() const;
    inline QLabel *getZzTimeLabel() const
    {
        return m_pZzTimeLabel;
    };

protected:
    void paintEvent(QPaintEvent *event);
    bool eventFilter(QObject *obj, QEvent *e);

private:

    void initUi();

private:
    QWidget *m_pMainWindow;
    QLabel *m_pTitleLabel;
    QLabel *m_pTroopIconLabel;
    QLabel *m_pTroopLabel;
    QToolButton *m_pMinTBtn;
    QToolButton *m_pCloseTBtn;
    QToolButton	 *m_pStyleTBtn;
    QToolButton	 *m_pSetTBtn;
    QLabel *m_pZzDateLabel;
    QLabel *m_pZzTimeLabel;
    QLabel *m_pDateLabel;
    QLabel *m_pTimeLabel;
    MenuBar *m_pMenuBar;
    QMenu *m_pSetMenu;
    QString m_strIconPath;
    QVector<QAction *> m_setActions;
    QVector<QMenu *> m_setMenus;
    LyTreeComboBox *m_treeCBox;
};

#endif
