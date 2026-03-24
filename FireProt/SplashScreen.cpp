#include "SplashScreen.h"
#include <QPainter>
#include <QCString.h>

SplashScreen::SplashScreen(const QPixmap &pixmap)
	: QSplashScreen(pixmap)
	, m_textPos(45, 350)
	, m_iFontSize(14)
{
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::init(const QString & resolution)
{
    if (resolution == "1k") {
		m_textPos = QPoint(45, 360);
		m_iFontSize = 14;
    }else {
		m_textPos = QPoint(60, 470);
		m_iFontSize = 18;
	}
}

void SplashScreen::drawContents(QPainter * painter)
{

    painter->setPen(QColor("#6bb2f4"));
	QFont font(tr2("Î¢ÈíÑÅºÚ"));
	font.setPixelSize(m_iFontSize);
	painter->setFont(font);

    painter->drawText(rect(), message(), QTextOption(Qt::AlignCenter));
}
