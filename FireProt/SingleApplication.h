#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QObject>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

class QWidget;
class QLocalServer;

class SingleApplication : public QApplication
{
	Q_OBJECT
public:
	SingleApplication(int &argc, char **argv);
	bool isRunning();
    void setWidget(QWidget *pWidget) { m_pWidget = pWidget; }

    void SetLanguage(const QString language);
    void SetLanguage(const int languageID);

    const QLocale& getLocale() const;

private slots:

    void newLocalConnection();

private:

	void initLocalConnection();

	void newLocalServer();

private:
	bool			m_bRunning;
	QWidget			*m_pWidget;
	QLocalServer	*m_pLocalServer;
	QString			m_strServerName;
    QTranslator     m_Translator;
    QLocale         m_locale;
};

#endif
