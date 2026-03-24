#ifndef RBCORNERWIDGET_H
#define RBCORNERWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "LyTabWidget.h"

class QTabWidget;
class QLabel;

class RBCornerWidget : public QWidget
{
	Q_OBJECT

public:
	RBCornerWidget(QWidget *parent = NULL);
	~RBCornerWidget();

    void setTroopName(const QString &name);

    void updateTranslateUi();

public slots:
	void onPre();
	void onNext();
	void onMenuChange(QAction *action);

protected:
	bool eventFilter(QObject *watched, QEvent *event);

private:
	void initial();
	void initMenu();
	Q_SLOT void onPopup();
	Q_SLOT void onClose();

private:
	QPushButton * m_pPopBtn;
	QPushButton * m_pCloseBtn;
	QWidget * m_pParentWidget;

    QTabWidget  * m_pTabWidget;
	QPushButton * m_pPreBtn;
	QPushButton * m_pNextBtn;
	QLabel * m_pCurLabel;
	QPushButton * m_pMenuBtn;
	QMenu * m_pMenu;
};

#endif
