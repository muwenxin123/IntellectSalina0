#ifndef MOUSEINFOWIDGET_H
#define MOUSEINFOWIDGET_H

#include <QLabel>

class MouseInfoWidget : public QLabel
{
	Q_OBJECT

public:
	MouseInfoWidget(QWidget *parent = 0);
	~MouseInfoWidget();

	Q_SLOT void updateLocationInfo(const QPoint & pos);
};

#endif