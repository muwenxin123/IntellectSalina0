#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QSplashScreen>

class SplashScreen : public QSplashScreen
{
	Q_OBJECT

public:
	SplashScreen(const QPixmap &pixmap = QPixmap());
	~SplashScreen();

	void init(const QString &resolution);

protected:
	void drawContents(QPainter *painter);

private:
	QPoint m_textPos;
	int m_iFontSize;
};

#endif
