#ifndef QGRIDICONWIDGET_H
#define QGRIDICONWIDGET_H

#include <QWidget>
#include <QGridLayout>

typedef struct struMenuData MenuData;

class QGridIconWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QGridIconWidget(QWidget *parent = nullptr);
	virtual ~QGridIconWidget();
	void setData(const QList<MenuData> &data);

signals:
	void triggered(QAction *action);
	void fullScreen(bool bFullScreen);

private slots :
	void buttonClicked(bool);

private:
	QGridLayout	m_Grid;
};

#endif