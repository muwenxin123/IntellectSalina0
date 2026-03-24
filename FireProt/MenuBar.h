#ifndef MENUBAR_H
#define MENUBAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QMenuBar>
#include <QFrame>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QButtonGroup>

class MenuBar : public QWidget
{
	Q_OBJECT
	Q_PROPERTY(QString lineColor READ lineColor WRITE setLineColor)

public:
	MenuBar(int maxVisibleMenuCount = 8, QWidget *parent = 0);
	~MenuBar();

	int addMenu(const QString &menu);

	void addAction(int menuId, QAction *action);

	QAction *addAction(int menuId, const QString &text, const QString &iconName = "common");

    QString lineColor() const
    {
        return m_strLineColor;
    }
	void setLineColor(const QString &colorName);

public slots:
	void slotButtonStyleShift(int index);

protected:
	void showEvent(QShowEvent *event);

signals:
	void triggered(QAction *action);

private:

	void initUi();

	Q_SLOT void rootMenuChanged(QAbstractButton *btn);
	Q_SLOT void moreMenuChanged(QAction *action);

private:
	QHBoxLayout * m_pHRootMenuLayout;
	QFrame * m_pHLine;
	QList<QToolButton*> m_rootMenuTBtns;
	QButtonGroup * m_pRootButtonGroup;
	QButtonGroup *m_pChildQButtonGroup = nullptr;
	QHash<int, QList<QToolButton*> > m_pChildMenuTBtns;
	QHash<int, QHBoxLayout*> m_pChildMenuHLayouts;
	QStackedWidget * m_pChildBarStackedWidget;
	QActionGroup * m_pActionGroup;

	QToolButton * m_pMoreTBtn;
	int m_iMaxVisibleMenuCount;
	QMenu * m_pMoreMenu;
	QActionGroup * m_pMoreMenuActionGroup;
	QString m_strLineColor;
	int buttonNum = 0;
};

#endif