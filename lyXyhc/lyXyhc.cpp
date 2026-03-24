#include "lyXyhc.h"
#include <lycore.h>
#if QTCORE_VERSION >= 0x050000
    #include <QtWidgets>
#else
    #include <QtGui>
#endif

#include "lyXyhcInterface.h"
#include <IFunctionInterface.h>
#include <CreateShape.h>
#include <ShapeClip.h>
#include <DrawShape.h>

#include <LyMessageBox.h>
#include <QDb.h>
#include "cmd.h"
#include "CmdTakeBackControl.h"
#include "LyDockManager.h"
#include "LyDockWidget.h"
#include "LyDockTabWidget.h"
#include "QColorMenu.h"
#include "trackList.h"

#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/IEarth.h>
#include <map3d/ITrack.h>
#include <map3d/IGraphics.h>
#include <QObject>

#include "IXyhcActivator.h"
#include "UAVCurrentShotAiming.h"

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

LyXyhc::LyXyhc()
{
    m_pCmd          = nullptr;
    m_pProj         = nullptr;
    m_pWnd          = nullptr;
    m_pLayerControl = nullptr;
    m_pTrack        = nullptr;
    m_pUAVNavigationArmShotAutoAiming = nullptr;
    m_pUAVNavigationArmShotUpdateCalBombLine = nullptr;
    m_pDlgPlannedSynergy = nullptr;
    m_pDlgZoneSynergy = nullptr;
    m_pDlgTimeSynergy = nullptr;
    m_pDlgInfoSynergy = nullptr;
    m_pDlgScanSynergy01 = nullptr;
    m_pDlgScanSynergy02 = nullptr;
    m_pDlgFireSynergy01 = nullptr;
    m_pDlgFireSynergy02 = nullptr;
    m_pUAVCurrentShotAiming = nullptr;
    m_pDlgSystemMessageDistribute = nullptr;

    m_slAttributeTitle << QObject::tr("Mark Type") << QObject::tr("Mark Model") << QObject::tr("Identification of Friend or Enemy")
                       << QObject::tr("Damaged state") << QObject::tr("Task type") << QObject::tr("Formation type") << QObject::tr("Power type") << QObject::tr("Remaining energy")
                       << QObject::tr("Designated pilot") << QObject::tr("Set goals") << QObject::tr("Touching the target");
    m_vslAttributeItem.push_back({ QObject::tr("Aircraft"), QObject::tr("Soldiers"), QObject::tr("Armored vehicle"), QObject::tr("Tank vehicle"),
                                   QObject::tr("Rocket launcher "), QObject::tr("Ships"), QObject::tr("Other"), QObject::tr("Unknown") });
    m_vslAttributeItem.push_back({ QObject::tr("Four rotor unmanned aerial vehicle"), QObject::tr("Six rotor unmanned aerial vehicle"), QObject::tr("DJI Phantom-4"),
                                   QObject::tr("Vertical fixed wing"), QObject::tr("Lingyun V-20"), QObject::tr("Sniper"), QObject::tr("Airborne individual soldier"), QObject::tr("Infantry"),
                                   QObject::tr("Artillery observation post"), QObject::tr("Specoal operations infantry"), QObject::tr("Communication individual"), QObject::tr("BMP-3"), QObject::tr("M2"), QObject::tr("T-15"),
                                   QObject::tr("Panther"), QObject::tr("M1A22"), QObject::tr("Leopard 2"), QObject::tr("T-99"), QObject::tr("T-96"), QObject::tr("T-90"), QObject::tr("SR-5"),
                                   QObject::tr("Pinaka"), QObject::tr("Astros"), QObject::tr("WS-15"), QObject::tr("Bobcat"), QObject::tr("BM-27"), QObject::tr("M270"), QObject::tr("WS-2D"),
                                   QObject::tr("Hovercraft"), QObject::tr("Civil ships"), QObject::tr("Landing craft"), QObject::tr("Gunboat") });
    m_vslAttributeItem.push_back({ QObject::tr("Enemy"), QObject::tr("Me"), QObject::tr("Friend"), QObject::tr("Neutral"), QObject::tr("Unknown"), QObject::tr("UnSpot") });
    m_vslAttributeItem.push_back({ QObject::tr("Normal"), QObject::tr("Personnel injury"), QObject::tr("Equipment damage"), QObject::tr("Personnel sacrifice"), QObject::tr("Equipment destroyed"), QObject::tr("Mechanical fault"), QObject::tr("Unknown") });
    m_vslAttributeItem.push_back({ QObject::tr("Investigation"), QObject::tr("Monitoring"), QObject::tr("Blockade"), QObject::tr("Conquering Difficulties"), QObject::tr("Armor piercing"),
                                   QObject::tr("Mark indication"), QObject::tr("Transport"), QObject::tr("Search for and rescue"), QObject::tr("Communications Relay") });
    m_vslAttributeItem.push_back({ QObject::tr("Single column team"), QObject::tr("Double column team"), QObject::tr("Single horizontal team"), QObject::tr("Double horizontal team"),
                                   QObject::tr("Trapezoidal team"), QObject::tr("Azimuth team"), QObject::tr("Herringbone team"), QObject::tr("Diamond team"), QObject::tr("Triangle team"), QObject::tr("Wedge"), QObject::tr("Warning screen"),
                                   QObject::tr("Single column formation"), QObject::tr("Compound formation"), QObject::tr("Multple formation team"), QObject::tr("Complex formation"), QObject::tr("Multiple Column"), QObject::tr("Multiple horizontal formation"), QObject::tr("Other"), QObject::tr("Undefined") });
    m_vslAttributeItem.push_back({ QObject::tr("Oil driven"), QObject::tr("Electric"), QObject::tr("Other"), QObject::tr("Unknown") });
    m_vslAttributeItem.push_back({ QObject::tr("20%"), QObject::tr("40%"), QObject::tr("60%"), QObject::tr("80%"), QObject::tr("100%") });
    m_vslAttributeItem.push_back({ QObject::tr("pilot1"), QObject::tr("pilot2"), QObject::tr("pilot3"), QObject::tr("pilot4"), QObject::tr("pilot5"), QObject::tr("pilot6"), QObject::tr("pilot7"), QObject::tr("pilot8"), QObject::tr("pilot9") });
    m_vslAttributeItem.push_back({ QObject::tr("1"), QObject::tr("2"), QObject::tr("3"), QObject::tr("4"), QObject::tr("5"), QObject::tr("6"), QObject::tr("7"), QObject::tr("8"), QObject::tr("9") });
    m_vslAttributeItem.push_back({ QObject::tr("1.1"), QObject::tr("1.2") });
}

void LyXyhc::Startup()
{
    IFunctionInterface &ifi = LyXyhcInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IXyhcActivator::Get().start(pIRegistry);
        }
    }
}

void LyXyhc::Shutdown()
{
    IFunctionInterface &ifi = LyXyhcInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IXyhcActivator::Get().stop(pIRegistry);
        }
    }
}

void LyXyhc::Reset()
{

}

bool LyXyhc::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LyXyhc::DrawDynamic(QPainter &painter, IProj *pProj)
{

    m_TrackMgr.Draw(painter, pProj);
    return true;
}

bool LyXyhc::OnMouseButtonDown(QMouseEvent *e)
{
    bool bReturn = false;
    m_MouseButton = e->button();
    switch (e->button())
    {
    case Qt::LeftButton:
    {
        m_ptPre = e->pos();
        m_pTrack = m_TrackMgr.SearchTrack(m_ptPre);
    }
    break;
    default:
        break;
    }
    return bReturn;
}

bool LyXyhc::OnMouseButtonUp(QMouseEvent *e)
{
    m_pTrack = nullptr;
    m_MouseButton = Qt::NoButton;
    switch (e->button())
    {
    case Qt::RightButton:
    {
        m_pTrack = m_TrackMgr.SearchTrack(e->pos());
        if (m_pTrack != nullptr)
        {
            ContextMenu(e->globalPos());
        }
    }
    break;
    default:
        break;
    }
    return false;
}

void LyXyhc::ContextMenu(const QPoint &pt)
{
    QMenu menu(nullptr);
    menu.addAction(QObject::tr("Micro meter switch"))->setData(0);
    menu.addAction(QObject::tr("Point line trajectory"))->setData(1);
    menu.addAction(QObject::tr("Growth trajectory"))->setData(2);
    menu.addAction(QObject::tr("Shorten the flight path"))->setData(3);
    menu.addAction(QObject::tr("5-point trajectory"))->setData(4);

    QMenu *pMn = menu.addMenu(QObject::tr("Full track"));
    pMn->addAction(QObject::tr("Partial trajectory"))->setData(5);
    pMn->addAction(QObject::tr("Without time"))->setData(6);
    pMn->addAction(QObject::tr("Half time"))->setData(7);
    pMn->addAction(QObject::tr("Full time"))->setData(8);
    pMn->addAction(QObject::tr("Clear Track"))->setData(MapMenuCommandOper::MapMenuCommandOper_FullTrack_ClearTrackLine);
    pMn->addAction(QObject::tr("Reconnaissance field of view"))->setData(MapMenuCommandOper::MapMenuCommandOper_FullTrack_ScoutView);
    pMn->addAction(QObject::tr("Ballistic line"))->setData(MapMenuCommandOper::MapMenuCommandOper_FullTrack_BallisticLine);

    QColorMenu *pCM = new QColorMenu(9);
    menu.addMenu(pCM)->setText(QObject::tr("Track color adjustment"));
    bool b = QObject::connect(pCM, &QMenu::triggered, [&](QAction * pA)
    {
        m_TrackMgr.OnMenuAction(m_pTrack, pA);
        menu.setActiveAction(pA);
        menu.setDefaultAction(pA);
        menu.close();
    });

    IFunctionInterface &ifi = LyXyhcInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("xxx.xxxx.query_name", 0);

    vt = ifi.Invoke("tsbh.target.query_name", 0);
    if (vt.canConvert<QStringList>())
    {
        const QStringList &sl = vt.toStringList();
        if (sl.size())
        {
            QStringList &sl2 = m_vslAttributeItem[m_vslAttributeItem.size() - 2];
            sl2.clear();
            sl2 = sl;
        }
    }

    UpdateContextMenuMark();

    QStringList &sl2 = m_vslAttributeItem[m_vslAttributeItem.size() - 1];
    sl2.clear();
    for (auto it = m_TrackMgr.m_Track.begin(); it != m_TrackMgr.m_Track.end(); it++)
    {
        if (&it->second != m_pTrack && (it->second.getIFF() == 1 || it->second.getIFF() == 5))
        {
            sl2.push_back(QString::asprintf("%d.%d", ((it->first >> 8) & 0xFF), it->first & 0xFF));
        }
    }

    int nIndex = 25;
    bool bCmd = (1 == LyXyhcInterface::Get().GetSettings().value("User/Role").toInt());
    for (int i = 0; i < m_slAttributeTitle.size(); i++)
    {
		if (i == 9) {
			pMn = menu.addMenu(m_slAttributeTitle[i]);
		}

		for (int j = 0; j < m_vslAttributeItem[i].size(); j++)
		{
			if (i == 9) {
				pMn->addAction(m_vslAttributeItem[i][j])->setData(nIndex);
			}
			nIndex++;
		}
		if (m_slAttributeTitle.size() - i < 4)
		{
			pMn->setEnabled(bCmd);
		}
    }
    QAction *pAction = menu.addAction(QObject::tr("Cancel Strike"));
    pAction->setData(nIndex++);
    pAction->setEnabled(bCmd);

    pAction = menu.exec(pt);
    if (pAction != nullptr)
    {
        m_TrackMgr.OnMenuAction(m_pTrack, pAction);
    }
    m_pTrack = nullptr;
}

void LyXyhc::UpdateContextMenuMark()
{
    qDebug() << __FILE__ << __LINE__;
    if (IXyhcActivator::Get().getPIMarkManageService() != nullptr)
    {
        const auto *pIMarkManageService = IXyhcActivator::Get().getPIMarkManageService();
        if (pIMarkManageService != nullptr)
        {
            const auto &targetList = pIMarkManageService->GetMarkIDListFromDB_ActivatingAction();
            QStringList &sl2 = m_vslAttributeItem[m_vslAttributeItem.size() - 2];
            sl2.clear();
            for (const auto &targetID : targetList)
            {
                sl2 << (targetID);
            }
        }
    }
}

CDlgPlannedSynergy *LyXyhc::getDlgPlannedSynergy()
{
    return m_pDlgPlannedSynergy;
}

CDlgZoneSynergy *LyXyhc::getPDlgZoneSynergy() const
{
    return m_pDlgZoneSynergy;
}

CDlgTimeSynergy *LyXyhc::getPDlgTimeSynergy() const
{
    return m_pDlgTimeSynergy;
}

CDlgSystemMessageDistribute *LyXyhc::getPDlgSystemMessageDistribute() const
{
    return m_pDlgSystemMessageDistribute;
}

CDlgInfoSynergy *LyXyhc::getPDlgInfoSynergy() const
{
    return m_pDlgInfoSynergy;
}

CDlgScanSynergy01 *LyXyhc::getPDlgScanSynergy01() const
{
    return m_pDlgScanSynergy01;
}

CDlgScanSynergy02 *LyXyhc::getPDlgScanSynergy02() const
{
    return m_pDlgScanSynergy02;
}

CDlgFireSynergy01 *LyXyhc::getPDlgFireSynergy01() const
{
    return m_pDlgFireSynergy01;
}

CDlgFireSynergy02 *LyXyhc::getPDlgFireSynergy02() const
{
    return m_pDlgFireSynergy02;
}

bool LyXyhc::ExecSql(const QString &strSql)
{
    try
    {
        QDb db(m_strDbIP.data(), m_strDbUser.data(), m_strDbPass.data(), m_strDbName.data(), m_strDbDriver.data());
        if (db.m_db.isOpen())
        {
            db.m_db.exec(strSql);
            return true;
        }
    }
    catch (const std::exception &)
    {

    }
    return false;
}

bool LyXyhc::GetDb(QDb &db)
{
    db.init(m_strDbIP.data(), m_strDbUser.data(), m_strDbPass.data(), m_strDbName.data(), m_strDbDriver.data());
    return true;
}

const QString &LyXyhc::GetPilotName(int nID) const
{
    for (int i = m_viPilot.size() - 1; i >= 0; i--)
    {
        if (nID == m_viPilot[i])
        {
            return m_vslAttributeItem[m_vslAttributeItem.size() - 3].at(i);
        }
    }
    static QString s = "";
    return s;
}

bool LyXyhc::OnMouseMove(QMouseEvent *e)
{
    switch (m_MouseButton)
    {
    case Qt::LeftButton:
    {
        if (m_pTrack != nullptr)
        {
            int x = m_pTrack->m_nOffsetX + (e->pos().x() - m_ptPre.x()) / OFFSET_RATE;
            int y = m_pTrack->m_nOffsetY + (e->pos().y() - m_ptPre.y()) / OFFSET_RATE;

            if (x <= 127 && x >= -128)
            {
                if (m_pTrack->m_nOffsetX != x)
                {
                    m_pTrack->m_nOffsetX = x;
                    m_ptPre.setX(e->pos().x());
                }
            }
            if (y <= 127 && y >= -128)
            {
                if (m_pTrack->m_nOffsetY != y)
                {
                    m_pTrack->m_nOffsetY = y;
                    m_ptPre.setY(e->pos().y());
                }
            }

            m_TrackMgr.SaveWeiBiaoViewShowConfigureINITrackOffset(m_pTrack);

        }
    }
    break;
    default:
        break;
    }
    return false;
}

bool LyXyhc::OnMouseDoubleClick(QMouseEvent *e)
{
    return false;
}

bool LyXyhc::OnCommand(int nID)
{
    switch (nID)
    {
    case 12001:
    {
        if (LyXyhcInterface::Get().GetSettings().value("User/Role").toInt() == 1)
        {
            CmdTakeBackControl dlg;
            dlg.exec();
        }
        else
        {
            LyMessageBox::information(m_pWnd, QObject::tr("Warning"), QObject::tr("Sorry! You are not authorized to use this feature! \n This function must be used as a commander!"));
        }
    }
    break;
    case 44777:
    {

    }
    return false;
    case 44778:
    {
    }
    break;
    default:
        break;
    }
    return false;
}

bool LyXyhc::OnCreate(int wParam, QWidget *pWidget)
{
    switch (wParam)
    {
    case 0:
    {
        m_pWnd = pWidget;
        LyDockWidget *pDock = LyDockManager::instance()->dockWidget("LyXyhcTelemetry");
        if (pDock == nullptr)
        {
            pDock = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("Ò£²âÐÅÏ¢"), "LyXyhcTelemetry");
            LyDockManager::instance()->embedToolDockWidget(pDock, LyDockDef::Bottom);
        }

        m_TrackMgr.setPProj(m_pProj);

        if (m_TrackMgr.m_pTelemetryWidget)
        {
            pDock->setCentralWidget(m_TrackMgr.m_pTelemetryWidget);
        }
        else
        {
            pDock->setCentralWidget(m_TrackMgr.m_pTelemetry);
        }

        QString strIni = m_strConfDir;
        strIni += "/ly.ini";
        QFile file(strIni);
        if (file.exists())
        {
            QSettings ini(strIni, QSettings::IniFormat);
            ini.beginGroup("Database");
            m_strDbName = ini.value("DbName", "LY").toString().toLatin1().data();
            m_strDbIP = ini.value("SvrIP", "192.168.1.115").toString().toLatin1().data();
            m_strDbUser = ini.value("User", "root").toString().toLatin1().data();
            m_strDbPass = ini.value("Password", "123456").toString().toLatin1().data();
            m_strDbDriver = ini.value("Driver", "QMYSQL").toString().toLatin1().data();

            QDb db(m_strDbIP.data(), m_strDbUser.data(), m_strDbPass.data(), m_strDbName.data(), m_strDbDriver.data());
            QDbQuery qry = db.exec("SELECT id, name FROM user");
            QStringList &sl2 = m_vslAttributeItem[m_vslAttributeItem.size() - 3];
            sl2.clear();
            m_viPilot.clear();
            while (!qry.eof())
            {
                int nID;
                QString strT;
                qry >> nID >> strT;
                m_viPilot.push_back(nID);
                sl2 << strT;
            }
        }

    }
    break;
    case 1:
    {
        QTabWidget *pTab = qobject_cast<QTabWidget *>(pWidget);
         if(0 && pTab != nullptr)
        {
            m_pLayerControl = pTab;

            QTreeWidget *treeWidget = new QTreeWidget();
            treeWidget->setHeaderHidden(true);
            treeWidget->clear();
            pTab->addTab(treeWidget, QObject::tr("Track information"));

            QTreeWidget::connect(treeWidget, &QTreeWidget::itemChanged, [](QTreeWidgetItem * item, int column)
            {
                if (item == nullptr || column != 0)
                {
                    return ;
                }

                const QVariant &dataVariant = item->data(0, Qt::UserRole);

                if (!dataVariant.canConvert(QMetaType::VoidStar))
                {
                    return ;
                }

                void *pVoidFun = item->data(0, Qt::UserRole).value<void *>();
                if (pVoidFun == nullptr)
                {
                    return ;
                }

                std::function<void (bool show)> *fun = (std::function<void (bool show)> *)(pVoidFun);
                if (fun == nullptr)
                {
                    return ;
                }

                if (item->checkState(0) == Qt::Checked)
                {
                    fun->operator()(true);
                }
                else if (item->checkState(0) == Qt::Unchecked)
                {
                    fun->operator()(false);
                }
            });

            QTreeWidgetItem *pAirInfoItem = new QTreeWidgetItem(treeWidget);
            if (pAirInfoItem != nullptr)
            {
                pAirInfoItem->setText(0, QObject::tr("Air Situation Stratification"));
                pAirInfoItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
                pAirInfoItem->setCheckState(0, Qt::Unchecked);

                static std::function<void (bool show)> pAirInfoItem_Fun = [](bool show)
                {
                    qDebug() << __LINE__ << " : "
                             << QObject::tr("Air situation stratification")
                             << show
                             ;
                };

                pAirInfoItem->setData(0, Qt::UserRole, QVariant::fromValue<void *>(&pAirInfoItem_Fun));

                QTreeWidgetItem *pAirInfoAttributeItem = new QTreeWidgetItem(pAirInfoItem);
                if (pAirInfoAttributeItem)
                {
                    pAirInfoAttributeItem->setText(0, QObject::tr("Air Info Attribute"));
                    pAirInfoAttributeItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsAutoTristate);
                    pAirInfoAttributeItem->setCheckState(0, Qt::Unchecked);

                    static std::function<void (bool show)> pAirInfoAttributeItem_Fun = [](bool show)
                    {
                        qDebug() << __LINE__ << " : "
                                 << QObject::tr("Air Info Attribute")
                                 << show
                                 ;
                        for (auto &trackItem : LyXyhc::Get().getTrackMgr().m_Track)
                        {
                            trackItem.second.m_status.QuanHangJi = show;
                        }
                    };

                    pAirInfoAttributeItem->setData(0, Qt::UserRole, QVariant::fromValue<void *>(&pAirInfoAttributeItem_Fun));

                    static std::function<void (const unsigned char IFF_First, const bool show)> pFun_IFF_First([](const unsigned char IFF_First, const bool show)
                    {
                        qDebug() << __LINE__ << " : "
                                 << IFF_First
                                 << show
                                 ;

                        for (auto &trackItem : LyXyhc::Get().getTrackMgr().m_Track)
                        {
                            if (trackItem.second.getIFF() == IFF_First)
                            {
                                trackItem.second.m_status.QuanHangJi = show;
                            }
                        }
                    });

                    QTreeWidgetItem *pAirInfoAttributeEnemyItem = new QTreeWidgetItem(pAirInfoAttributeItem);
                    if (pAirInfoAttributeEnemyItem)
                    {
                        pAirInfoAttributeEnemyItem->setText(0, QObject::tr("Enemy"));
                        pAirInfoAttributeEnemyItem->setCheckState(0, Qt::Unchecked);

                        static std::function<void (bool show)> pAirInfoAttributeEnemyItem_Fun = [](bool show)
                        {
                            qDebug() << __LINE__ << " : "
                                     << QObject::tr("Enemy")
                                     << show
                                     ;

                            pFun_IFF_First(1, show);
                        };

                        pAirInfoAttributeEnemyItem->setData(0, Qt::UserRole, QVariant::fromValue<void *>(&pAirInfoAttributeEnemyItem_Fun));

                    }

                    QTreeWidgetItem *pAirInfoAttributeMySelfItem = new QTreeWidgetItem(pAirInfoAttributeItem);
                    if (pAirInfoAttributeMySelfItem)
                    {
                        pAirInfoAttributeMySelfItem->setText(0, QObject::tr("Me"));
                        pAirInfoAttributeMySelfItem->setCheckState(0, Qt::Unchecked);

                        static std::function<void (bool show)> pAirInfoAttributeMySelfItem_Fun = [](bool show)
                        {
                            qDebug() << __LINE__ << " : "
                                     << QObject::tr("Me")
                                     << show
                                     ;

                            pFun_IFF_First(2, show);
                        };

                        pAirInfoAttributeMySelfItem->setData(0, Qt::UserRole, QVariant::fromValue<void *>(&pAirInfoAttributeMySelfItem_Fun));
                    }

                    QTreeWidgetItem *pAirInfoAttributeFriendItem = new QTreeWidgetItem(pAirInfoAttributeItem);
                    if (pAirInfoAttributeFriendItem)
                    {
                        pAirInfoAttributeFriendItem->setText(0, QObject::tr("Friend"));
                        pAirInfoAttributeFriendItem->setCheckState(0, Qt::Unchecked);

                        static std::function<void (bool show)> pAirInfoAttributeFriendItem_Fun = [](bool show)
                        {
                            qDebug() << __LINE__ << " : "
                                     << QObject::tr("Friend")
                                     << show
                                     ;

                            pFun_IFF_First(3, show);
                        };

                        pAirInfoAttributeFriendItem->setData(0, Qt::UserRole, QVariant::fromValue<void *>(&pAirInfoAttributeFriendItem_Fun));
                    }

                    QTreeWidgetItem *pAirInfoAttributeNeutralityItem = new QTreeWidgetItem(pAirInfoAttributeItem);
                    if (pAirInfoAttributeNeutralityItem)
                    {
                        pAirInfoAttributeNeutralityItem->setText(0, QObject::tr("Neutral"));
                        pAirInfoAttributeNeutralityItem->setCheckState(0, Qt::Unchecked);

                        static std::function<void (bool show)> pAirInfoAttributeNeutralityItem_Fun = [](bool show)
                        {
                            qDebug() << __LINE__ << " : "
                                     << QObject::tr("Neutral")
                                     << show
                                     ;

                            pFun_IFF_First(4, show);
                        };

                        pAirInfoAttributeNeutralityItem->setData(0, Qt::UserRole, QVariant::fromValue<void *>(&pAirInfoAttributeNeutralityItem_Fun));
                    }

                    QTreeWidgetItem *pAirInfoAttributeUnKnownItem = new QTreeWidgetItem(pAirInfoAttributeItem);
                    if (pAirInfoAttributeUnKnownItem)
                    {
                        pAirInfoAttributeUnKnownItem->setText(0, QObject::tr("Unknown"));
                        pAirInfoAttributeUnKnownItem->setCheckState(0, Qt::Unchecked);

                        static std::function<void (bool show)> pAirInfoAttributeUnKnownItem_Fun = [](bool show)
                        {
                            qDebug() << __LINE__ << " : "
                                     << QObject::tr("Unknown")
                                     << show
                                     ;

                            pFun_IFF_First(5, show);
                        };

                        pAirInfoAttributeUnKnownItem->setData(0, Qt::UserRole, QVariant::fromValue<void *>(&pAirInfoAttributeUnKnownItem_Fun));
                    }

                    QTreeWidgetItem *pAirInfoAttributeOtherItem = new QTreeWidgetItem(pAirInfoAttributeItem);
                    if (pAirInfoAttributeOtherItem)
                    {
                        pAirInfoAttributeOtherItem->setText(0, QObject::tr("Other"));
                        pAirInfoAttributeOtherItem->setCheckState(0, Qt::Unchecked);

                        static std::function<void (bool show)> pAirInfoAttributeOtherItem_Fun = [](bool show)
                        {
                            qDebug() << __LINE__ << " : "
                                     << QObject::tr("Other")
                                     << show
                                     ;

                            pFun_IFF_First(6, show);
                        };

                        pAirInfoAttributeOtherItem->setData(0, Qt::UserRole, QVariant::fromValue<void *>(&pAirInfoAttributeOtherItem_Fun));
                    }

                    treeWidget->expandItem(pAirInfoAttributeItem);
                }

                treeWidget->expandItem(pAirInfoItem);
            }

        }
    }
    break;
    }
    return false;
}

bool LyXyhc::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        m_TrackMgr.Tidy();
        LyXyhcInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}

bool LyXyhc::OnNetRecv(int nLength, char *lpData)
{
    return m_TrackMgr.Process(nLength, lpData);
}

void LyXyhc::OnEarthMessage(int wParam, void *lParam)
{
    pos3d *p3d = (pos3d *)lParam;
    switch (wParam)
    {
    case IEarthEventHandler::Earth_LBUTTONDOWN:
    {
    }
    break;
    case IEarthEventHandler::Earth_TRACK_CONTENT:
    {
        static IEarth *pEarth = IEarth::GetEarth();
        static ITrackManager *pTrackManager = nullptr;
        if (pEarth != nullptr && pEarth->IsVisible())
        {
            if (pTrackManager == nullptr)
            {
                pTrackManager = pEarth->GetTrackManager();
            }
        }

        if (pTrackManager != nullptr && p3d != nullptr && !p3d->strMsg.empty())
        {
            unsigned int ucZH = 0;
            unsigned int ucPH = 0;
            sscanf(p3d->strMsg.c_str(), "%u.%u", &ucZH, &ucPH);
            if ((ucZH == 0 || ucPH == 0) || (ucZH > 255 || ucPH > 255))
            {
                break;
            }
            unsigned char z = static_cast<unsigned char>(ucZH) ;
            unsigned char p = static_cast<unsigned char>(ucPH) ;

            m_pTrack = m_TrackMgr.FindTrack(z, p);
            if (m_pTrack != nullptr)
            {
                QPoint pt(p3d->x, p3d->y);
                ContextMenu(m_pWnd->mapToGlobal(pt));
            }

        }
    }
    break;
    }
}
