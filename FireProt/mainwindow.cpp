#include "mainwindow.h"
#include "MenuBar.h"
#include <QKeyEvent>
#include <QDebug>
#include <QAction>
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"
#include "LyFloatDockWrapperWidget.h"
#include "HeaderWidget.h"
#include "MenuData.h"
#include "ViewerWidget.h"
#include "qmapwidget.h"
#include "MapIconWidget.h"
#include "StyleTestWidget.h"
#include "LyMessageBox.h"
#include "Viewer2D.h"
#include <MenuData.h>
#include "RBCornerWidget.h"
#include "SplashScreen.h"
#include "LyWindow.h"
#include <PartMap.h>
#include <QAction>
#include <QDebug>
#include <QKeyEvent>
#include "LyWindow.h"

#include "LyResourceWidget2.h"
#include "LyDetailWidget.h"
#include "TreeChartWidget.h"
#include "Login.h"
#include "map3d\IEarth.h"

extern QString			g_strSeatPath;

const QStringList& selectiveForceTroops =
{
    QT_TRANSLATE_NOOP("MainWindow", "Shangdong airport"),
    QT_TRANSLATE_NOOP("MainWindow", "Yunnan airport"),
    QT_TRANSLATE_NOOP("MainWindow", "Shijiazhuang airport"),
};

const QString g_strQssPath = ":/qss/%1/%2/";

MainWindow::MainWindow(QWidget *parent)
    : DoubleScreenWidget(true, parent)
    , m_pHeaderWidget(new HeaderWidget(this, this))
    , m_pTroopGroup(NULL)
    , m_pMenuBar(NULL)
    , m_pStyleMenu(NULL)
    , m_pTroopsMenu(NULL)
    , m_pLanguagesMenu(NULL)
    , m_pDockMenu(NULL)
    , m_pBattlefieldEnvDockWgt(NULL)
    , m_pCornerWidget(NULL)
    , m_pFloatMapWidget(nullptr)
    , m_strStyleName("black")
    , m_strMonitorResolution("1k")
    , m_strLanguageName("zh_CN")
{
    m_pHeaderWidget->setWindowTitle(tr2("智慧巡检与管控系统"));
    setWindowTitle(tr2("智慧巡检与管控系统"));

    initUi();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimer()));
    m_timer.start(200);
}

MainWindow::~MainWindow()
{
    if (QGuiApplication::screens().count() >= 2)
    {
        //setMapFullScreen(false);
		setMapFullScreen(false);
		saveLayout("自定义");
    }
	else
	{
		saveLayout("单屏");
	}
    m_pViewer->m_LyCore.Shutdown();
}

void MainWindow::reSizeFullAllScreen()
{

    bool   isDoubleScreen             = false;
    bool   isReSizeFullAllScreen      = false;
    bool   isReSizeFullAllScreenPoint = false;
    bool   isReSizeFullAllScreenSize  = false;
    QPoint reSizeFullAllScreenPoint;
    QSize  reSizeFullAllScreenSize;
    {
        QString screenSetConfFile = "../data/conf/ScreenSet.ini";
        if(!QFileInfo::exists(screenSetConfFile)){
            screenSetConfFile = "../data/conf/ly.ini";
        }
        QSettings settings(screenSetConfFile, QSettings::IniFormat);
        settings.beginGroup("ScreenSet");
        if (settings.contains("DoubleScreen") && settings.contains("FullAllScreen") && settings.contains("FullAllScreenSize") && settings.contains("FullAllScreenPoint"))
        {

            QVariant doubleScreenVariant = settings.value("DoubleScreen", "false");
            if (doubleScreenVariant.isValid() && doubleScreenVariant.canConvert(QMetaType::Bool))
            {
                isDoubleScreen = doubleScreenVariant.toBool();
            }

            QVariant fullAllScreenVariant = settings.value("FullAllScreen", "false");
            if (fullAllScreenVariant.isValid() && fullAllScreenVariant.canConvert(QMetaType::Bool))
            {
                 isReSizeFullAllScreen = fullAllScreenVariant.toBool();
             }

             QVariant fullAllScreenPointVariant = settings.value("FullAllScreenPoint", "@Point(0 0)");
            if (fullAllScreenPointVariant.isValid() && fullAllScreenPointVariant.canConvert(QMetaType::QPoint))
            {
                 reSizeFullAllScreenPoint = fullAllScreenPointVariant.toPoint();
                 isReSizeFullAllScreenPoint = true;
             }

             QVariant fullAllScreenSizeVariant = settings.value("FullAllScreenSize", "@Size(0 0)");
            if (fullAllScreenSizeVariant.isValid() && fullAllScreenSizeVariant.canConvert(QMetaType::QSize))
            {
                 reSizeFullAllScreenSize = fullAllScreenSizeVariant.toSize();
                 isReSizeFullAllScreenSize = true;
             }
        }
        settings.endGroup();
    }

    if(!isDoubleScreen
            && isReSizeFullAllScreen
            && isReSizeFullAllScreenPoint
            && isReSizeFullAllScreenSize
            && reSizeFullAllScreenSize.isValid()
            && !reSizeFullAllScreenSize.isNull()
            && (reSizeFullAllScreenSize.width() > 0  && reSizeFullAllScreenSize.height() > 0)
       )
    {

    }
}

QString MainWindow::SelectiveForceTroops(int type)
{
    if (type >= selectiveForceTroops.size())
    {
        return QStringLiteral("");
    }
    return tr(selectiveForceTroops[type].toStdString().c_str());
}

QString SelectiveForceTroops(int type)
{
    if (type >= selectiveForceTroops.size())
    {
        return QStringLiteral("");
    }
    return QCoreApplication::translate("MainWindow", selectiveForceTroops[type].toStdString().c_str());
}

void MainWindow::keyPressEvent(QKeyEvent * event)
{
    if (event->key() == Qt::Key_F5)
    {
        setQss();
    }
    DoubleScreenWidget::keyPressEvent(event);
}

void MainWindow::showEvent(QShowEvent * event)
{
    DoubleScreenWidget::showEvent(event);

    static bool isFirstShowResource = true;
    if (isFirstShowResource)
    {
        initResourceInfo();
        isFirstShowResource = false;
    }

    reSizeFullAllScreen();
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    Q_UNUSED(result);

    if (eventType == "windows_generic_MSG" && QGuiApplication::screens().count() >= 2)
    {
#ifdef Q_OS_WIN32
        MSG *msg = static_cast<MSG*>(message);
        if (msg->message == WM_POWERBROADCAST && msg->wParam == PBT_APMRESUMEAUTOMATIC)
        {
            setMapFullScreen(true);
            qDebug() << "qApp Wake up----------------";
        }
#endif
    }

    return false;
}

void MainWindow::initUi()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 11, 0)
    setWindowFlag(Qt::FramelessWindowHint);
#endif

    QSettings setting("../data/conf/styles.ini", QSettings::IniFormat);
    if (setting.contains("default"))
    {
        QString defaultStyle = setting.value("default").toString();
        if (defaultStyle.isEmpty())
        {
            defaultStyle = "black";
        }
        m_strStyleName = defaultStyle;
    }

    QDesktopWidget *pDesktop = QApplication::desktop();
    const QRect &rect = pDesktop->screenGeometry(QCursor::pos());
 //   if(rect.width() == 2560 && rect.height() == 1440)
	//{
	//	m_strMonitorResolution = "2k";
	//}
	//else
	//{
        m_strMonitorResolution = "1k";
    //}
    setQss();

    bool isDoubleScreen = QGuiApplication::screens().count() >= 2;

    {
        QString screenSetConfFile = "../data/conf/ScreenSet.ini";
        if (!QFileInfo::exists(screenSetConfFile))
        {
            screenSetConfFile = "../data/conf/ly.ini";
        }
        QSettings settings(screenSetConfFile, QSettings::IniFormat);
        settings.beginGroup("ScreenSet");
        if (settings.contains("DoubleScreen"))
        {
             QVariant doubleScreenVariant = settings.value("DoubleScreen", "false");
            if (doubleScreenVariant.isValid() && doubleScreenVariant.canConvert(QMetaType::Bool))
            {
                 isDoubleScreen = doubleScreenVariant.toBool();
             }
        }
        settings.endGroup();
    }

    SplashScreen splash(QStringLiteral("../data/images/splash/splash.png"));
    splash.init(m_strMonitorResolution);
    splash.show();
    QVBoxLayout * pMainVLayout = new QVBoxLayout(this);
    pMainVLayout->setMargin(0);
    pMainVLayout->setSpacing(1);
    pMainVLayout->addWidget(m_pHeaderWidget);
    QFrame *pHLine = new QFrame;
    pHLine->setFrameShape(QFrame::HLine);
    pMainVLayout->addWidget(pHLine);
    QGridLayout *gDockLayout = new QGridLayout;
    gDockLayout->setContentsMargins(8, 0, 8, 8);
    gDockLayout->addWidget(LyDockManager::instance()->wrapperWidget());
    pMainVLayout->addLayout(gDockLayout);

    m_pMenuBar = m_pHeaderWidget->menuBar();
    connect(m_pMenuBar, SIGNAL(triggered(QAction*)), this, SLOT(menuActionTriggered(QAction*)));
    splash.showMessage(tr("initialize Two-dimensional map ..."));
    MapMenuData mmdLeft, mmdRight;
    m_pMapWidget = new QMapWidget(this);
    m_pViewer = new CViewerWidget(mmdLeft, mmdRight, m_pMapWidget->m_pMap, m_pMapWidget->m_pLayerControl);
    m_pViewer->HideSplashScreen(this);
    m_pMapWidget->setWidget(m_pViewer->Get3D(), m_pViewer->m_p2D);
    splash.showMessage(tr("initialize Three-dimensional earth  ..."));

    splash.showMessage(tr("Initialization menu ..."));

    initMenubar(mmdLeft, mmdRight);
    splash.showMessage(tr("Initialization window layout ..."));

    loadLayout("自定义");

    LyDockWidget *pNewDockWgt = LyDockManager::instance()->dockWidget("MapWidget");
    if (!pNewDockWgt)
    {
        pNewDockWgt = LyDockManager::instance()->createDockWidget(LyDockDef::File, tr("Map"), "MapWidget");
        LyDockManager::instance()->embedFileDockWidget(pNewDockWgt);
    }
    pNewDockWgt->setCentralWidget(m_pMapWidget);
    pNewDockWgt->setWindowTitle(tr("Map"));
    pNewDockWgt->tabWidget()->tabBar()->hide();
    connect(m_pMapWidget->m_pMapIconWidget, SIGNAL(triggered(QAction*)), this, SLOT(menuActionTriggered(QAction*)));

	if (isDoubleScreen > 2)
	{
		pNewDockWgt = LyDockManager::instance()->dockWidget("LayerControl");
		if (!pNewDockWgt)
		{
			pNewDockWgt = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("地图图层"), "LayerControl");
			LyDockManager::instance()->embedToolDockWidget(pNewDockWgt, LyDockDef::Left);
		}
		pNewDockWgt->setCentralWidget(m_pMapWidget->m_pLayerControl);
		pNewDockWgt->setWindowTitle(tr2("地图图层"));
		
	}
	m_pBattlefieldEnvDockWgt = pNewDockWgt;

    loadStyles();

    if(m_strMonitorResolution == "2k")
    {
        gDockLayout->setContentsMargins(11, 0, 11, 11);
    }


    m_pViewer->SendMsg(IInterface::MSG_Create, 1000, (void *)this);
	m_pViewer->SendMsg(IInterface::MSG_Create, 1001, (void *)(this->m_pHeaderWidget->getPTroopLabel()));
	m_pViewer->SendMsg(IInterface::MSG_Create, 1002, (void *)(this->m_pHeaderWidget->getZzTimeLabel()));
	m_pViewer->SendMsg(IInterface::MSG_Create, 0, (void *)m_pMapWidget->m_pMap);
    m_pViewer->SendMsg(IInterface::MSG_Create, 1, (void *)m_pMapWidget->m_pLayerControl);
    m_pViewer->SendMsg(IInterface::MSG_Create, 2, (void *)pNewDockWgt);
    m_pViewer->SendMsg(IInterface::MSG_Create, 3, (void *)pNewDockWgt);
    m_pViewer->SendMsg(IInterface::MSG_Create, 5, (void *)m_pViewer->Get3D());
	m_pViewer->SendMsg(IInterface::MSG_Create, 6, (void *)m_pViewer->m_p2D);
	m_pViewer->SendMsg(IInterface::MSG_Create, 7, this);
	splash.showMessage(tr("Load plug-in ..."));

    initDockMenu();
	initLayoutMenu(tr("Custom"));
	initZoneManager();
	initViewVideo();

	initDJIControlMenu();

    splash.showMessage(tr("Load skin ..."));

    m_pViewer->m_p2D->reSetLayers(false);

    if(isDoubleScreen)
    {
        setMapFullScreen(true);
    }
    connect(m_pViewer->m_p2D, SIGNAL(signalMouseMove(const QPoint &)), this, SLOT(slotMouseMove(const QPoint &)));
    connect(m_pViewer->m_p2D, SIGNAL(signalMouseStateChanged(int)), this, SLOT(onMouseStateChanged(int)));
    m_pViewer->m_LyCore.activeRecv();

	splash.finish(&splash);
}

void MainWindow::loadStyles()
{
    QSettings setting((m_pViewer->m_LyCore.GetConfDir() + "/styles.ini").data(), QSettings::IniFormat);
    QStringList keys = setting.allKeys();
    QStringList styles;
    QString defaultStyle = setting.value("default").toString();
    if(defaultStyle.isEmpty())
    {
        defaultStyle = "black";
    }
    static QActionGroup *styleGroup = new QActionGroup(this);
    QMenu * styleMenu = m_pHeaderWidget->addSetMenu(tr("Skin change"), "style");
    m_pStyleMenu = styleMenu;
    styleMenu->installEventFilter(this);
    connect(styleMenu, SIGNAL(triggered(QAction*)), this, SLOT(changeStyle(QAction*)));
    foreach(QString key, keys)
    {
        if (key != "default")
        {
            QString value = setting.value(key).toString();

            QFile file(QString(g_strQssPath).arg(m_strMonitorResolution).arg(value) + value + ".css");
            if (file.open(QIODevice::ReadOnly))
            {
                QString qss = tr2(file.readAll());
                file.close();
                QStringList splits = qss.split("styleName");
                if (splits.size() >= 2)
                {

                    QAction * styleAction = styleMenu->addAction(splits[1]);
                    styleAction->setData(value);
                    styleAction->setCheckable(true);
                    styleAction->setChecked(defaultStyle == value);
                    styleGroup->addAction(styleAction);
                }
                else
                {
                    QAction * styleAction = styleMenu->addAction(key);
                    styleAction->setData(value);
                    styleAction->setCheckable(true);
                    styleAction->setChecked(defaultStyle == value);
                    styleGroup->addAction(styleAction);
                }
            }
        }
    }
    m_strStyleName = defaultStyle;

    QString strQssPath = QString(g_strQssPath).arg(m_strMonitorResolution).arg(m_strStyleName);
    QFile qssFile(strQssPath + m_strStyleName + ".css");
    if(qssFile.open(QIODevice::ReadOnly))
    {
        qApp->setStyleSheet(tr2(qssFile.readAll()));
        qssFile.close();
    }
}

void MainWindow::loadTroops()
{
    QMenu * troopsMenu = m_pHeaderWidget->addSetMenu(tr("Selective force"), "troops");
    m_pTroopsMenu = troopsMenu;
    troopsMenu->installEventFilter(this);
    connect(troopsMenu, SIGNAL(triggered(QAction*)), this, SLOT(selectTroops(QAction*)));

    m_pTroopGroup = new QActionGroup(this);
    const int size = selectiveForceTroops.size();
    for (int i = 0; i < size; i++)
    {
        QAction * troopAction = troopsMenu->addAction(SelectiveForceTroops(i));
        m_pTroopGroup->addAction(troopAction);
        troopAction->setCheckable(true);
    }

    if (selectiveForceTroops.size() > 0 && m_pTroopGroup->actions().size() > 0)
    {
        m_pHeaderWidget->setTroopName(SelectiveForceTroops(0));
        m_pTroopGroup->actions()[0]->setChecked(true);
    }
}

void MainWindow::initMenubar(const MapMenuData &mmdLeft, const MapMenuData &mmdRight)
{
    int nIndex = 0;
    MapMenuData::const_iterator it;
    QList<MenuData>::const_iterator it2;
    for(it = mmdLeft.begin(); it != mmdLeft.end(); it++)
    {
        nIndex = m_pMenuBar->addMenu(it->first.strText);
        for(it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            QString iconName = "common";
            QStringList iconNames = it2->strIcon.split("/");
            if(iconNames.size() > 0)
            {
                iconName = iconNames.last();
            }
            QAction *pAction = m_pMenuBar->addAction(nIndex, it2->strText, iconName);
            pAction->setData(QVariant::fromValue(*it2));
        }
    }
    for(it = mmdRight.begin(); it != mmdRight.end(); it++)
    {
        nIndex = m_pMenuBar->addMenu(it->first.strText);
        for(it2 = it->second.begin(); it2 != it->second.end(); it2++)
        {
            QString iconName = "common";
            QStringList iconNames = it2->strIcon.split("/");
            if(iconNames.size() > 0)
            {
                iconName = iconNames.last();
            }
            QAction *pAction = m_pMenuBar->addAction(nIndex, it2->strText, iconName);
            pAction->setData(QVariant::fromValue(*it2));
        }
    }
}

void MainWindow::initDJIControlMenu()
{
// 	if (m_pHeaderWidget != nullptr)
// 	{
// 		QAction* djiControlMenu = m_pHeaderWidget->addSetAction(tr2("大疆控制"));
// 		connect(djiControlMenu, &QAction::triggered, [this](bool)
// 		{
// 			if (m_pViewer != nullptr)
// 			{
// 				m_pViewer->SendMsg(IInterface::MSG_Command, 51009, nullptr);
// 			}
// 		});
// 
// 	}
}


void MainWindow::initLanguagesMenu()
{
    QActionGroup* actionGroup = new QActionGroup(this);
    QSettings settings("../data/conf/Languages.ini", QSettings::IniFormat);
    settings.beginGroup("Language");
    const QString languageString = settings.value("language", "").toString();
    settings.endGroup();

	QMenu *languagesMenu = m_pHeaderWidget->addSetMenu(tr("Language"));
	m_pLanguagesMenu = languagesMenu;
	QList<QLocale::Language> languagesList;
	languagesList << QLocale::Language::Chinese;
	languagesList << QLocale::Language::English;

	foreach(const auto language, languagesList)
	{
		QAction *dockAction = languagesMenu->addAction(QLocale::languageToString(language), this, [this](bool checked)
		{
			if (!checked)
			{
				return;
			}

			QObject *sender = this->sender();
			if (sender == nullptr)
			{
				return;
			}

			QAction *actionSender = qobject_cast<QAction *>(sender);
			if (actionSender == nullptr)
			{
				return;
			}

			const QVariant &actionData = actionSender->data();
			if (!actionData.isValid())
			{
				return;
			}

			bool ok = false;
			const int data = actionData.toInt(&ok);
			if (!ok || data == 0)
			{
				return;
			}

			const QLocale::Language languages = static_cast<QLocale::Language>(data);
			const QString &languagesString = QLocale::languageToString(languages);

			QSettings settings("./Data/conf/Languages.ini", QSettings::IniFormat);
			settings.beginGroup("Language");
			settings.setValue("language", languagesString);
			settings.endGroup();

			qDebug() << "Switch to languages : " << languagesString;

			emit languageChanged(languagesString);

			m_strLanguageName = QLocale(languages).name();

			updateTranslateUi();
		});

		if (dockAction)
		{
			actionGroup->addAction(dockAction);
			dockAction->setCheckable(true);
			dockAction->setData(language);
			if (languageString == dockAction->text())
			{
				m_strLanguageName = QLocale(language).name();
				dockAction->setChecked(true);
			}
		}
	}

}

void MainWindow::updateTranslateUi()
{
    if (m_pHeaderWidget)
    {
        m_pHeaderWidget->setWindowTitle(tr(""));
        m_pHeaderWidget->updateTranslateUi(m_strMonitorResolution, m_strStyleName, m_strLanguageName);
    }

    if (m_pCornerWidget)
    {
        m_pCornerWidget->updateTranslateUi();
    }

    if (m_pStyleMenu)
    {
        m_pStyleMenu->setTitle(tr("Skin change"));

        QList<QAction*> troopActionAndSeparatorList = m_pStyleMenu->actions();
        for (int i = 0; i < troopActionAndSeparatorList.size(); i++)
        {
            if (!troopActionAndSeparatorList[i]->isSeparator())
            {
                if (troopActionAndSeparatorList[i]->data().toString() == "black")
                {
                    troopActionAndSeparatorList[i]->setText(tr("black", "Qss Skin Style"));
                }
                else if (troopActionAndSeparatorList[i]->data().toString() == "white")
                {
                    troopActionAndSeparatorList[i]->setText(tr("white", "Qss Skin Style"));
                }
            }
        }
    }

    if (m_pTroopsMenu)
    {
        m_pTroopsMenu->setTitle(tr("Selective force"));
    }

    if (m_pLanguagesMenu)
    {
        m_pLanguagesMenu->setTitle(tr("Language"));
    }

    if (m_pDockMenu)
    {
        m_pDockMenu->setTitle(tr("Dock Manage"));
    }

    if (m_pTroopGroup)
    {
        QList<QAction*>  troopActionAndSeparatorList = m_pTroopGroup->actions();
        QList<QAction*>  troopActionList;
        for (int i = 0; i < troopActionAndSeparatorList.size(); i++)
        {
            if (!troopActionAndSeparatorList[i]->isSeparator())
            {
                troopActionList.push_back(troopActionAndSeparatorList[i]);
            }
        }

        const int size = qMin(selectiveForceTroops.size(), troopActionList.size());
        for (int i = 0; i < size; i++)
        {
            QAction* troopAction = troopActionList[i];
            troopAction->setText(SelectiveForceTroops(i));
            if (troopAction->isChecked())
            {
                m_pHeaderWidget->setTroopName(SelectiveForceTroops(i));
                m_pCornerWidget->setTroopName(SelectiveForceTroops(i));
            }
        }
    }

    if (m_pBattlefieldEnvDockWgt)
    {
         m_pBattlefieldEnvDockWgt->setWindowTitle(tr("Battlefield Env"));
    }
}

void MainWindow::initResourceInfo()
{
#if 0
    QStandardItemModel *leftModel = new QStandardItemModel;
    leftModel->setHorizontalHeaderLabels(QStringList() << tr2("名称") << tr2("剩余") << tr2("总数量"));
    for (int i = 0; i < 20; ++i)
    {
        QStandardItem *item1 = new QStandardItem(tr2("飞机") + QString::number(i));
        QStandardItem *item2 = new QStandardItem(QString::number(2));
        QStandardItem *item3 = new QStandardItem(QString::number(10));
        item1->setData("id", Qt::UserRole);
        item1->setTextAlignment(Qt::AlignCenter);
        item2->setData("id", Qt::UserRole);
        item2->setTextAlignment(Qt::AlignCenter);
        item3->setData("id", Qt::UserRole);
        item3->setTextAlignment(Qt::AlignCenter);
        leftModel->setItem(i, 0, item1);
        leftModel->setItem(i, 1, item2);
        leftModel->setItem(i, 2, item3);
    }

    QStandardItemModel *rightModel = new QStandardItemModel;
    rightModel->setHorizontalHeaderLabels(QStringList() << tr2("名称") << tr2("剩余") << tr2("总数量"));
    for (int i = 0; i < 20; ++i)
    {
        QStandardItem *item1 = new QStandardItem(tr2("弹药") + QString::number(i));
        QStandardItem *item2 = new QStandardItem(QString::number(2));
        QStandardItem *item3 = new QStandardItem(QString::number(10));
        item1->setData("id", Qt::UserRole);
        item1->setTextAlignment(Qt::AlignCenter);
        item2->setData("id", Qt::UserRole);
        item2->setTextAlignment(Qt::AlignCenter);
        item3->setData("id", Qt::UserRole);
        item3->setTextAlignment(Qt::AlignCenter);
        rightModel->setItem(i, 0, item1);
        rightModel->setItem(i, 1, item2);
        rightModel->setItem(i, 2, item3);
    }

    QList<QString> names;
    QList<QStandardItemModel *> models;
    names << tr2("飞机使用情况") << tr2("弹药使用情况");
    models << leftModel << rightModel;
    QList<int> l1 = { 100, 40, 60 };
    QList<int> l2 = { 100, 40, 60 };
    QList<QList<int>> colWidths;
    colWidths << l1 << l2;

    LyResourceWidget2 *resourceWidget = new LyResourceWidget2();
    resourceWidget->setResourceInfo(names, models, colWidths);
    connect(resourceWidget, &LyResourceWidget2::itemClicked, this, [ = ](const QString & id, int index)
    {
        QList<StLyPlaneInfo> infos;
        for (int j = 0; j < 2; ++j)
        {
            StLyPlaneInfo detailInfo;
            detailInfo.airportName = QString(tr2("某某某机场")) + QString::number(j);
            QList<StLyTroopsInfo> troops;

            for (int k = 0; k < 5; ++k)
            {
                StLyTroopsInfo troop;
                troop.troopsName = QString(tr2("某某某部队")) + QString::number(k);
                troop.remain = 2;
                troop.total = 10;
                troops.append(troop);
            }

            detailInfo.troopsInfo = troops;
            infos.append(detailInfo);
        }

        if (index == 0)
        {

        }
        else
        {
            QStandardItemModel *rightModel = new QStandardItemModel;
            rightModel->setHorizontalHeaderLabels(QStringList() << tr2("名称") << tr2("剩余") << tr2("总数量"));
            for (int i = 0; i < 20; ++i)
            {
                QStandardItem *item1 = new QStandardItem(tr2("弹药") + QString::number(i));
                QStandardItem *item2 = new QStandardItem(QString::number(2));
                QStandardItem *item3 = new QStandardItem(QString::number(10));
                item1->setData("id", Qt::UserRole);
                item1->setTextAlignment(Qt::AlignCenter);
                item2->setData("id", Qt::UserRole);
                item2->setTextAlignment(Qt::AlignCenter);
                item3->setData("id", Qt::UserRole);
                item3->setTextAlignment(Qt::AlignCenter);
                rightModel->setItem(i, 0, item1);
                rightModel->setItem(i, 1, item2);
                rightModel->setItem(i, 2, item3);
            }
            QList<int> l1 = { 85, 30, 50 };

        }
    });

    connect(resourceWidget, &LyResourceWidget2::hideSig, LyWindow2::instance(), &LyWindow2::showDetailWidget);
#endif

}

void MainWindow::initZoneManager()
{
	QAction *ZoneManager = m_pHeaderWidget->addSetAction(tr2("区域设置"));
	connect(ZoneManager, &QAction::triggered, [this](bool)
	{
		if (m_pViewer != nullptr)
		{
			m_pViewer->SendMsg(IInterface::MSG_Create, 33385, nullptr);

		}
	});
	QAction *ZoneManager1 = m_pHeaderWidget->addSetAction(tr2("模拟播放"));
	connect(ZoneManager1, &QAction::triggered, [this](bool)
	{
		if (m_pViewer != nullptr)
		{
			m_pViewer->SendMsg(IInterface::MSG_Create, 51003, nullptr);

		}
	});
}

void MainWindow::initViewVideo()
{
	QAction *viewVideoCfg = m_pHeaderWidget->addSetAction(tr2("视频视图配置"));
	connect(viewVideoCfg, &QAction::triggered, [this](bool)
	{
		if (m_pViewer != nullptr)
		{
			m_pViewer->SendMsg(IInterface::MSG_Create, 50024, nullptr);

		}
	});
}

void MainWindow::initDockMenu()
{
    QList<LyDockWidget*> docks = LyDockManager::instance()->totalDockWidget();
    QMenu *dockMenu = m_pHeaderWidget->addSetMenu(tr("Dock Manage"));
    m_pDockMenu = dockMenu;
    foreach(LyDockWidget *dock, docks)
    {
        if (dock != NULL && dock->id() != "MapWidget")
        {
            QAction *dockAction = dockMenu->addAction(dock->windowTitle() + "-" + dock->id(), dock, SLOT(setVisibility(bool)));
            dockAction->setCheckable(true);
            dockAction->setChecked(dock->isVisible());
            connect(dock, SIGNAL(visiblityChanged(bool)), dockAction, SLOT(setChecked(bool)));
        }
    }
}

void MainWindow::initLayoutMenu(const QString &name)
{
	static QActionGroup *pGroup = new QActionGroup(this);
	static QMenu *pMenu = NULL;
	if (pMenu != NULL)
	{
		pMenu->clear();
	}
	else
	{
		pMenu = m_pHeaderWidget->addSetMenu(tr("Interference Layout"), "layout");
		pMenu->installEventFilter(this);
		connect(pMenu, &QMenu::triggered, [this](QAction *pAction)
		{
			loadLayout(tr3(pAction->text()).data());
		});
	}

	QDir dir(m_pViewer->m_LyCore.GetConfDir());
	const QStringList &sl = dir.entryList(QStringList() << "layout*.xml");
	for (int i = 0; i < sl.size(); ++i)
	{
		if (sl[i].size() > 10)
		{
			const QString &strT = sl[i].mid(6, sl[i].size() - 10);
			QAction *pAction = pMenu->addAction(strT);
			pAction->setCheckable(true);
			pAction->setChecked(strT == name);
			pGroup->addAction(pAction);
		}
	}
}

void MainWindow::setQss()
{

    QString strQssPath = QString(g_strQssPath).arg(m_strMonitorResolution).arg(m_strStyleName);
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

        QSettings setting("../data/conf/styles.ini", QSettings::IniFormat);
        setting.setValue("default", m_strStyleName);

        qApp->setStyleSheet(strQss);
    }

    if (m_pHeaderWidget)
    {
        m_pHeaderWidget->updateTranslateUi(m_strMonitorResolution, m_strStyleName, m_strLanguageName);
    }

    if (m_pStyleMenu)
    {
        m_pStyleMenu->setTitle(tr("Skin change"));

        QList<QAction*> troopActionAndSeparatorList = m_pStyleMenu->actions();
        for (int i = 0; i < troopActionAndSeparatorList.size(); i++)
        {
            if (!troopActionAndSeparatorList[i]->isSeparator())
            {
                if (troopActionAndSeparatorList[i]->data().toString() == "black")
                {
                    troopActionAndSeparatorList[i]->setText(tr("black", "Qss Skin Style"));
                }
                else if (troopActionAndSeparatorList[i]->data().toString() == "white")
                {
                    troopActionAndSeparatorList[i]->setText(tr("white", "Qss Skin Style"));
                }
            }
        }
    }
}

void MainWindow::saveLayout(const char *name)
{
    QByteArray layout = LyDockManager::instance()->save();
	QStringList files =
	{
		tr2((m_pViewer->m_LyCore.GetConfDir() + "layout" + name + ".xml").data()),
		g_strSeatPath + "layout" + tr2(name) + ".xml"
	};

	for (const auto &f : files)
	{
		QFile file(f);
		if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
		{
			file.write(layout);
			file.close();
		}
	}
}

bool MainWindow::loadLayout(const char *name)
{
	QFile file(tr2((m_pViewer->m_LyCore.GetConfDir() + "layout" + name + ".xml").data()));
	if (file.open(QIODevice::ReadOnly))
    {
        QByteArray layout = file.readAll();
        file.close();
        return LyDockManager::instance()->load(layout);
    }
    return false;
}

void MainWindow::setMapFullScreen(bool bFullScreen)
{

    {
        QString screenSetConfFile = "../data/conf/ScreenSet.ini";
        if (!QFileInfo::exists(screenSetConfFile))
        {
            screenSetConfFile = "../data/conf/ly.ini";
        }
        bool isDoubleScreen = true;
        QSettings settings(screenSetConfFile, QSettings::IniFormat);
        settings.beginGroup("ScreenSet");
        if (settings.contains("DoubleScreen"))
        {
             QVariant doubleScreenVariant = settings.value("DoubleScreen", "true");
            if (doubleScreenVariant.isValid() && doubleScreenVariant.canConvert(QMetaType::Bool))
            {
                 isDoubleScreen = doubleScreenVariant.toBool();
             }
        }
        settings.endGroup();
        if (!isDoubleScreen)
        {

        }
    }

    LyDockWidget *pMapDockWidget = LyDockManager::instance()->dockWidget("MapWidget");
    if (!pMapDockWidget)
    {
        return;
    }

    if (bFullScreen)
    {
        if (m_pFloatMapWidget == nullptr)
        {
            m_pFloatMapWidget = (LyFloatDockWrapperWidget *)LyDockManager::instance()->createFloatingWidget(pMapDockWidget);
			m_pFloatMapWidget->setResizable(false);
			m_pFloatMapWidget->setDragable(false);
            m_pFloatMapWidget->setAttribute(Qt::WA_TranslucentBackground, false);
			m_pFloatMapWidget->setWindowFlags(m_pFloatMapWidget->windowFlags() | Qt::WindowDoesNotAcceptFocus);
        }

        QList<QScreen *> list = QGuiApplication::screens();
        if (list.count() > 1)
        {
            list.removeAll(QGuiApplication::primaryScreen());
        }

        m_pFloatMapWidget->setGeometry(list.first()->geometry());
        pMapDockWidget->setFullScreen(true);
    }
    else
    {
        LyDockManager::instance()->embedFileDockWidget(pMapDockWidget);

        if (m_pFloatMapWidget)
        {
            delete m_pFloatMapWidget;
            m_pFloatMapWidget = nullptr;
        }
    }

    m_pViewer->SendMsg(IInterface::MSG_Create, 10, (void *)m_pFloatMapWidget);
}

void MainWindow::menuActionTriggered(QAction *pAction)
{
    const struMenuData &menuData = pAction->data().value<struMenuData>();
    if (menuData.strAction.leftRef(12).compare(QLatin1String("ShowHideDock")) == 0)
    {

    }
    else if (menuData.strAction.leftRef(10).compare(QLatin1String("FullScreen")) == 0)
    {
        setMapFullScreen(pAction->isChecked());
    }
	else if (menuData.nID == 1000)
	{
		class LayoutDialog : public LyDialog
		{
		public:
			QLineEdit m_edit;
			LayoutDialog():LyDialog(tr("Save Interference Layout"))
			{
				setCentralWidget(&m_edit);
				m_edit.setText(tr("Layout_1"));
				addBottomButton(QDialogButtonBox::Save);
				addBottomButton(QDialogButtonBox::Cancel);
			}
		};

		LayoutDialog dlg;
		connect(&dlg, &QDialog::accepted, [this, &dlg]()
		{
			saveLayout(tr3(dlg.m_edit.text()).data());
			initLayoutMenu(dlg.m_edit.text());
		});
		dlg.exec();
	}
	else
    {
        m_pViewer->menuAction(pAction);
        if (menuData.strAction.leftRef(6).compare(QLatin1String("Show3D")) == 0)
        {
            bool bChecked = pAction->isChecked();
            m_pMapWidget->switch23D(bChecked);

            if (!bChecked)
            {
                m_pViewer->m_p2D->setFocus();
            }
        }
    }
}

void MainWindow::slotTimer()
{
    static unsigned int siI = 0;
    siI++;
        m_pViewer->SendTimer(1);
        m_pViewer->update();
    if(siI % 5 == 0)
    {
        m_pViewer->SendTimer(2);
        m_pHeaderWidget->setDateTime(QDateTime::currentDateTime());

        m_pViewer->m_LyCore.activeRecv();
    }
    if(siI % 300 == 0)
    {
        m_pViewer->SendTimer(3);
    }
}

void MainWindow::slotMouseMove(const QPoint &pt)
{
    if (!m_pViewer)
    {
        return;
    }

    QStringList params;
    double dX, dY;
    m_pViewer->m_LyCore.m_pProj->Screen2Jwd(pt, dX, dY);

    char cNS = dY < 0 ? 'S' : 'N';
    char cEW = dX < 0 ? 'W' : 'E';
    char szBuf[32];
    int nX = fabs(dX) * 3600 + 0.5;
    int nY = fabs(dY) * 3600 + 0.5;
    sprintf(szBuf, "%c%03d°%02d＇%02d＂", cEW, nX / 3600, nX / 60 % 60, nX % 60);
    params.append(tr2(szBuf));

    sprintf(szBuf, "%c%02d°%02d＇%02d＂", cNS, nY / 3600, nY / 60 % 60, nY % 60);
    params.append(tr2(szBuf));

    int nAltitude = m_pViewer->m_LyCore.m_pProj->GetMapScale();
    sprintf(szBuf, "%s %d", m_pViewer->m_LyCore.m_pProj->Jwd2FangGe(dX, dY), nAltitude);
    params.append(tr2(szBuf));

    m_pMapWidget->setTips(params);
}

void MainWindow::onMouseStateChanged(int nMouseState)
{
    if (nMouseState == 0)
    {
        m_pMapWidget->m_pMapIconWidget->reset();
    }
}

void MainWindow::changeStyle(QAction * style)
{
    if (style == NULL)
    {
        return;
    }

    QString styleName = style->data().toString();
    m_strStyleName = styleName;
    setQss();
}

void MainWindow::selectTroops(QAction *troop)
{
    if (troop)
    {
        m_pHeaderWidget->setTroopName(troop->text());
    }
}
HeaderWidget *MainWindow::getPHeaderWidget() const
{
    return m_pHeaderWidget;
}
