#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QWidget>
#include <QTimer>
#include "ZModuleOperator.h"
#include "DoubleScreenWidget.h"

class RBCornerWidget;
class HeaderWidget;
class MenuBar;
class CViewerWidget;
class QMapWidget;
class LyDockWidget;
class LyFloatDockWrapperWidget;

class MainWindow : public DoubleScreenWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = NULL);
    ~MainWindow();

    void reSizeFullAllScreen();
    HeaderWidget *getPHeaderWidget() const;
    QString SelectiveForceTroops(int type);
	LyDockWidget* getMapDockWidget() {
		return m_pBattlefieldEnvDockWgt;
	}
signals:
      void languageChanged(const QString language);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void showEvent(QShowEvent *event) override;
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:

    void initUi();

    void loadStyles();

    void loadTroops();

    void initLanguagesMenu();
    void updateTranslateUi();

	void initDockMenu();
	void initLayoutMenu(const QString &name);

    void initMenubar(const MapMenuData & mmdLeft, const MapMenuData & mmdRight);

    void initResourceInfo();

	void initZoneManager();
	void initViewVideo();

	void initDJIControlMenu();
private slots:

    void setQss();

    void saveLayout(const char *name);

    bool loadLayout(const char *name);

    void setMapFullScreen(bool bFullScreen);

    void menuActionTriggered(QAction * action);
    void slotTimer();
    void slotMouseMove(const QPoint &pt);

    void changeStyle(QAction *style);

    void selectTroops(QAction *troop);
    void onMouseStateChanged(int);

private:
    HeaderWidget    *m_pHeaderWidget;
    QActionGroup    *m_pTroopGroup;
    MenuBar         *m_pMenuBar;
    QMenu           *m_pStyleMenu;
    QMenu           *m_pTroopsMenu;
    QMenu           *m_pLanguagesMenu;
    QMenu           *m_pDockMenu;
	LyDockWidget    *m_pBattlefieldEnvDockWgt;
    CViewerWidget   *m_pViewer;
    RBCornerWidget  *m_pCornerWidget;
    QMapWidget      *m_pMapWidget;
	LyFloatDockWrapperWidget  *m_pFloatMapWidget;
    QTimer          m_timer;
    QString         m_strStyleName;
    QString         m_strMonitorResolution;
    QString         m_strLanguageName;


};

#endif
