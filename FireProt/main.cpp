#include "mainwindow.h"
#include "SingleApplication.h"
#include <QTextCodec>
#include <QTranslator>
#include <QResource>
#include <QNetworkInterface>

#include "LY_UserManageService/QDlgUserLogin.h"
#include "LY_UserManageService/QDlgUserManage.h"
#include "LY_UserManageService/UserManageService.h"
#include "LY_UserManageService/UserManageServiceSql.h"

#include <QScreen>
#include "LyDockWidget.h"
int main(int argc, char *argv[])
{
    SingleApplication a(argc, argv);
    if (!a.isRunning())
    {
		qRegisterMetaType<LyDockWidget*>("LyDockWidget*");
		qRegisterMetaType<LyDockWidget*>("LyDockWidget*&");
        bool setExecutionStateContinuous = true;
        QSettings settingsSetExecutionState("../data/conf/ly.ini", QSettings::IniFormat);
        settingsSetExecutionState.beginGroup("SetExecutionState");
        if (settingsSetExecutionState.contains("SetExecutionStateContinuous"))
        {
            const QVariant setExecutionStateContinuousVariant = settingsSetExecutionState.value("SetExecutionStateContinuous", "true");
            if (setExecutionStateContinuousVariant.canConvert(QMetaType::Bool))
            {
                setExecutionStateContinuous = setExecutionStateContinuousVariant.toBool();
            }
        }
        settingsSetExecutionState.endGroup();

        if (setExecutionStateContinuous)
        {

            SetThreadExecutionState(ES_CONTINUOUS | ES_SYSTEM_REQUIRED | ES_DISPLAY_REQUIRED);

        }

        QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));
        QResource::registerResource("FireProt.rcc");

        QSettings settings("../data/conf/Languages.ini", QSettings::IniFormat);
        settings.beginGroup("Language");
        const QString languageString = settings.value("language", "").toString();
        settings.endGroup();
        a.SetLanguage(languageString);

        {
            QString strStyleName("black");
            QSettings setting("../data/conf/styles.ini", QSettings::IniFormat);
            if (setting.contains("default"))
            {
                QString defaultStyle = setting.value("default").toString();
                if (defaultStyle.isEmpty())
                {
                    defaultStyle = "black";
                }
                strStyleName = defaultStyle;
            }

            QString strMonitorResolution("1k");
            const QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
            if(screen && screen->size().width() == 2560 && screen->size().height() == 1440)
            {
                strMonitorResolution = "2k";
            }
            else
            {
                strMonitorResolution = "1k";
            }

            QString strQssPath = QStringLiteral(":/qss/%1/%2/").arg(strMonitorResolution).arg(strStyleName);
            QDir dir(strQssPath);
            QStringList cssNames = dir.entryList(QStringList() << "*.css");
            QFile qssFile;
            QString strQss;
            for(int i = 0; i < cssNames.size(); ++i)
            {
                qssFile.setFileName(strQssPath + cssNames[i]);
                if(qssFile.open(QIODevice::ReadOnly))
                {
                    strQss += tr2(qssFile.readAll());
                    qssFile.close();
                }
            }
            if(!strQss.isEmpty())
            {

                qApp->setStyleSheet(strQss);
            }
        }

        QDlgUserLogin dlgUserLogin;
        dlgUserLogin.exec();
        if(!dlgUserLogin.IsPwdCorrect())
        {
            return 0;
        }

        dlgUserLogin.close();

        QString userInfo = QString::number(dlgUserLogin.getUserInfo());
        a.setProperty("ly_userInfo_pilot", QString::fromStdString(userInfo.toStdString()));
        qDebug() << "ly_userInfo_pilot:" << a.property("ly_userInfo_pilot");

		QScreen* primaryScreen = QGuiApplication::primaryScreen();
		QSize screenSize = primaryScreen->size();
		a.setProperty("ly_screen_xy", screenSize);
		
        MainWindow w;
        a.setWidget(&w);
        w.showFullScreen();
		LyDockWidget* mapWidget = w.getMapDockWidget();
		a.setProperty("ly_map_lydialog", QVariant::fromValue(mapWidget));
        MainWindow::connect(&w, &MainWindow::languageChanged, &a, static_cast<void (SingleApplication::*)(const QString)>(&SingleApplication::SetLanguage));

#ifndef WIN32
        QRect rect = QGuiApplication::screens().at(0)->availableGeometry();
        printf("%d,%d\n", rect.width(), rect.height());
        w.resize(rect.width(), rect.height());
#endif

        const int result = a.exec();

        if (setExecutionStateContinuous)
        {

            SetThreadExecutionState(ES_CONTINUOUS);
        }

        return result;
    }
    return 0;
}
