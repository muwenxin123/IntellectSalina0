#include "LY_MarkManageService.h"
#include "LY_MarkManageServiceInterface.h"
#include <IFunctionInterface.h>
#include "lyUI/LyDockManager.h"
#include <QCString.h>
#include "IMarkManageImpl.h"
#include "IMarkManageServiceActivator.h"
#include "CombatZoneEditDialog.h"
#include "lyUI/LyWidget.h"
#include "lyUI/LyMessageBox.h"
#include <QGeoCoordinate>
#include <QMouseEvent>
#include <QSize>
#include <QPainter>
#include <QDirIterator>

#include <osgEarth/MapNode>

#include <map3d/IEarth.h>
#include <map3d/EarthEvent.h>
#include <map3d/IEarth.h>
#include <map3d/ITrack.h>
#include <map3d/IGraphics.h>

#include "drawAble_drawFire.h"

#define  LINE_LENGTH	3

qnzkna::framework::IRegistry           *LY_MarkManageService::s_IRegistryImpl = nullptr;

CGetHeight LY_MarkManageService::s_CGetHeight("../data/dem/90");

inline bool operator < (const QSize &l, const QSize &r)
{
    return l.width() < r.width() || (l.width() == r.width() && l.height() < r.height());
}

typedef QPair<int, QSize> IntSize;

bool IsBlank(const QImage &img)
{
    const QSize &sz = img.size();
    for (int i = sz.height() - 1; i >= 0; i--)
    {
        const uchar *p = img.constScanLine(i);
        for (int j = sz.width() * img.depth() / 8 - 1; j >= 0; j--)
        {
            if (*(p + j))
            {
                return false;
            }
        }
    }
    return true;
}

class QMilitaryImageCache
{
public:
    QMilitaryImageCache() {};
    ~QMilitaryImageCache() {};

    inline static QMilitaryImageCache &get()
    {
        static QMilitaryImageCache ic;
        return ic;
    }

    QImage getImage(int nID, const QSize &size)
    {
        IntSize is(nID, size);
        auto it = mImg.find(is);
        if (it != mImg.end())
        {
            return it->second;
        }
        else
        {
            QImage img(size, QImage::Format_RGBA8888);
            QPainter p(&img);
            img.fill(QColor(0, 0, 0, 0));
            p.setRenderHint(QPainter::Antialiasing);

            bool bBlank = true;
            QRect rect(0, 0, size.width(), size.height());
			const QImage img2;
            bBlank = IsBlank(img2);
            if (!bBlank)
            {
                p.drawImage(0, 0, img2);
            }

            if (bBlank)
            {
                QTextOption opt(Qt::AlignCenter);
                opt.setWrapMode(QTextOption::WordWrap);
                p.setBackgroundMode(Qt::TransparentMode);
                p.drawEllipse(rect);
                p.drawText(rect, QString::asprintf("%d", nID), opt);
            }
            mImg.insert(std::make_pair(is, img));
            return img;
        }
    }

private:
    std::map<IntSize, QImage>  mImg;
};

LY_MarkManageService::LY_MarkManageService()
{
    IMarkManageServiceActivator::Get();
    m_pProj = nullptr;
    m_pDlgMarkList = nullptr;
    m_pDlgMarkEdit = nullptr;
    m_pDlgMarkAssign = nullptr;
    m_pDlgMarkImage = nullptr;
    m_pDlgMarkPicture = nullptr;
    m_pDlgMarkVideo = nullptr;
	m_pDlgZoneList = nullptr;

    m_mapModel.clear();
    m_mapModelPath.clear();
    m_mapLabel.clear();
    m_mapLabelPath.clear();

    int colorIndex = 0;
    m_nColor[0] = 0xffff00;
    m_nColor[colorIndex++] = 0xffff00;
    m_nColor[colorIndex++] = 0x00ff00;
    m_nColor[colorIndex++] = 0x008000;
    m_nColor[colorIndex++] = 0x0000ff;
    m_nColor[colorIndex++] = 0x800000;
    m_nColor[colorIndex++] = 0xff0000;

    CreateMenu();
}

LY_MarkManageService::~LY_MarkManageService()
{
    QString strPath = QObject::tr(m_strDataDir);
    strPath += QLatin1String("/temp");
    clearDir(strPath);
}

void LY_MarkManageService::Startup()
{
    IFunctionInterface &ifi = LY_MarkManageServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            s_IRegistryImpl = pIRegistry;
            IMarkManageServiceActivator::Get().start(pIRegistry);
        }
    }

    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->registerService(qnzkna::framework::ServiceReference("Ly.IMarkManageService", &(qnzkna::MarkManage::IMarkManageImpl::Get())));
    }
}

void LY_MarkManageService::Shutdown()
{
    if (s_IRegistryImpl != nullptr)
    {
        s_IRegistryImpl->unregisterService(qnzkna::framework::ServiceReference("Ly.IMarkManageService", &(qnzkna::MarkManage::IMarkManageImpl::Get())));
    }

    IFunctionInterface &ifi = LY_MarkManageServiceInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            IMarkManageServiceActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_MarkManageService::Reset()
{

}

bool LY_MarkManageService::DrawGraph(QPainter &painter, IProj *pProj)
{
    return false;
}

bool LY_MarkManageService::DrawDynamic(QPainter &painter, IProj *pProj)
{
    DrawMarkSelected(painter, pProj);
    DrawMovingMark(painter, pProj);

    if (m_pDlgZoneList == nullptr)
    {
        return false;
    }
    CZoneInfoVec infVec = m_pDlgZoneList->GetZoneInfoVec();

    if (infVec.size() == 0)
    {
        return false;
    }

    for (auto Airinfo : infVec)
    {
        if (Airinfo.GetShowStates() == true)
        {
            if (Airinfo.GetShape() == EZoneShape_Circle)
            {
                ZonePointVec stPointVec = Airinfo.GetPoints();
                for (auto stpoint : stPointVec)
                {

                    QPoint pt = pProj->Jwd2Screen(stpoint.dLon, stpoint.dLat);
                    double viewLength2 = pProj->Map2ScreenDistance(stpoint.dRadius * 2);
                    const QRectF rectangle(pt.x() - viewLength2 / 2, pt.y() - viewLength2 / 2, viewLength2, viewLength2);
                    painter.setPen(m_nColor[Airinfo.GetAttributeENum(Airinfo.GetAttribute())]);
                    QColor viewColor(m_nColor[Airinfo.GetAttributeENum(Airinfo.GetAttribute())]);
                    if (Airinfo.GetAttribute() != EZoneAttribute_Action)
                    {
                        viewColor.setAlpha(50);

                    }
                    else
                    {
                        viewColor.setAlpha(0);
                    }
                    painter.setBrush(QBrush(viewColor));
                    painter.drawEllipse(rectangle);
                }
            }
            else if (Airinfo.GetShape() == EZoneShape_Polygon)
            {

                ZonePointVec stPointVec = Airinfo.GetPoints();
                QPolygon polygon;
                for (auto stpoint : stPointVec)
                {
                    QPoint pt = pProj->Jwd2Screen(stpoint.dLon, stpoint.dLat);
                    polygon << pt;
                }
                painter.setPen(m_nColor[Airinfo.GetAttributeENum(Airinfo.GetAttribute())]);
                QColor viewColor(m_nColor[Airinfo.GetAttributeENum(Airinfo.GetAttribute())]);
                if (Airinfo.GetAttribute() != EZoneAttribute_Action)
                {
                    viewColor.setAlpha(50);

                }
                else
                {
                    viewColor.setAlpha(0);
                }
                painter.setBrush(QBrush(viewColor));
                painter.drawPolygon(polygon);
            }
            else if (Airinfo.GetShape() == EZoneShape_Sector)
            {

                ZonePointVec stPointVec = Airinfo.GetPoints();
                QPolygon polygon;
                for (auto stpoint : stPointVec)
                {
                    QVector<QPointF> tmpPoint = getCircleQVec(QPointF(stpoint.dLon, stpoint.dLat), stpoint.dRadius, stpoint.dStartAngle, stpoint.dEndAngle, 1);
                    for (auto tmp : tmpPoint)
                    {
                        QPoint pt = pProj->Jwd2Screen(tmp.x(), tmp.y());
                        polygon << pt;
                    }
                    int count = tmpPoint.size();
                    painter.setPen(Qt::red);
                    QColor viewColor(Qt::red);
                    if (Airinfo.GetAttribute() != EZoneAttribute_Action)
                    {
                        viewColor.setAlpha(50);
                    }
                    else
                    {
                        viewColor.setAlpha(0);
                    }
                    painter.setBrush(QBrush(viewColor));
                    painter.drawPolygon(polygon);
                }
            }
        }
    }

    if (m_pDlgZoneList != nullptr)
    {
        if (m_pDlgZoneList->GetCombatZoneDialog() != nullptr)
        {
            if (!m_pDlgZoneList->GetCombatZoneDialog()->isHidden())
            {
                ZoneWindowPointVec wndPointMap;
                EZoneShape eShape;
                if (m_pDlgZoneList->GetZoneEditDialog()->GetWindowPointMap(eShape, wndPointMap))
                {
                    if (eShape == EZoneShape_Circle)
                    {
                        for (auto itr : wndPointMap)
                        {

                            QPoint pt = pProj->Jwd2Screen(itr.dLon, itr.dLat);
                            unsigned int nColor = 0xff0000;
                            painter.setPen(nColor);
                            QColor viewColor(nColor);
                            painter.setBrush(QBrush(viewColor));
                            painter.drawEllipse(pt, 3, 3);

                            double viewLength2 = pProj->Map2ScreenDistance(itr.dRadius * 2);
                            const QRectF rectangle(pt.x() - viewLength2 / 2, pt.y() - viewLength2 / 2, viewLength2, viewLength2);
                            painter.setBrush(Qt::NoBrush);
                            painter.drawEllipse(rectangle);
                        }
                    }
                    else if (eShape == EZoneShape_Polygon)
                    {

                        QPolygon polygon;
                        int nNo = 1;
                        unsigned int nColor = 0xff0000;
                        for (auto stpoint : wndPointMap)
                        {
                            QPoint pt = pProj->Jwd2Screen(stpoint.dLon, stpoint.dLat);
                            polygon << pt;

                            QPen penText(QColor(255, 255, 255));
                            painter.setPen(penText);
                            QPoint ptTmp(pt.rx() - 4, pt.ry() - 5);
                            painter.drawText(ptTmp, QString::number(nNo));
                            nNo++;

                            painter.setPen(nColor);
                            QColor greeb = nColor;
                            painter.setBrush(greeb);
                            const int rx = 3;
                            painter.drawEllipse(pt, rx, rx);
                        }
                        painter.setPen(nColor);
                        painter.setBrush(Qt::NoBrush);
                        painter.drawPolygon(polygon);
                    }
                    else if (eShape == EZoneShape_Sector)
                    {
                        QPolygon polygon;
                        int nNo = 1;
                        unsigned int nColor = 0xff0000;
                        for (auto stpoint : wndPointMap)
                        {
                            QVector<QPointF> tmpPoint = getCircleQVec(QPointF(stpoint.dLon, stpoint.dLat), stpoint.dRadius, stpoint.dStartAngle, stpoint.dEndAngle, 1000);
                            for (auto tmp : tmpPoint)
                            {
                                QPoint pt = pProj->Jwd2Screen(tmp.x(), tmp.y());
                                polygon << pt;

                            }
                            painter.setPen(nColor);
                            painter.setBrush(Qt::NoBrush);
                            painter.drawPolygon(polygon);
                        }
                    }
                }
            }
        }
    }

#if 0

    std::vector<double> pdLon;
    std::vector<double> pdLat;
    std::vector<double> pdHeight;

    pdLon.push_back(109.3183632);
    pdLat.push_back(24.5530731);
    pdHeight.push_back(200);

    pdLon.push_back(109.3284909);
    pdLat.push_back(24.5556891);
    pdHeight.push_back(200);

    pdLon.push_back(109.3424677);
    pdLat.push_back(24.5471275);
    pdHeight.push_back(200);

    pdLon.push_back(109.3370699);
    pdLat.push_back(24.5338393);
    pdHeight.push_back(200);

    double pM = 10;
    double pZ = 1000;

    CCreateShape cs;
    SHPObject *pShp = cs.CreatePolygon(pdLon.data(), pdLat.data(), 4, 100, true, nullptr, pdLat.data());
    if (pShp != nullptr)
    {
        IEarth *pEarth = IEarth::GetEarth();
        IUserMesh1 *pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
        if (pUM != nullptr)
        {
            QColor colorBombTrack(0x008000);
            colorBombTrack.setAlpha(4);
            QColor colorTemp;
            colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());

            pUM->SetColor(colorTemp.rgba());

            double dZ = 0;
            SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &pZ);
            IUserMesh1 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh1(pShp1);

            pUserMesh->SetColor(colorTemp.rgba());

        }
    }

#endif
    return false;
}

bool LY_MarkManageService::OnMouseButtonDown(QMouseEvent *e)
{
    MarkMouseButtonDown(e);

    if (m_pDlgZoneList == nullptr)
    {
        return false;
    }
    CZoneInfoVec infVec = m_pDlgZoneList->GetZoneInfoVec();

    if (infVec.size() == 0)
    {
        return false;
    }

    {
        if (m_pDlgZoneList->GetCombatZoneDialog() != nullptr)
        {
            if (!m_pDlgZoneList->GetCombatZoneDialog()->isHidden() && m_pDlgZoneList->GetZoneEditDialog()->GetPickPointStates() == PICK_AS_POINT_EDIT)
            {
                ZoneWindowPointVec wndPointMap;
                EZoneShape eShape;
                if (m_pDlgZoneList->GetZoneEditDialog()->GetWindowPointMap(eShape, wndPointMap))
                {
                    if (eShape == EZoneShape_Polygon)
                    {
                        QPoint pt = e->pos();
                        if (JudgePoint(pt, m_nSelectedID))
                        {
                            m_bMouseMove = true;
                            LY_MarkManageServiceInterface::Get().Refresh(1);
                        }
                    }
                }
            }
        }
    }
    return false;
}

bool LY_MarkManageService::OnMouseButtonRelease(QMouseEvent *e)
{
    if (m_pDlgZoneList->GetZoneEditDialog()->GetPickPointStates() == PICK_AS_POINT_NORMAL)
    {
        return false;
    }
    if (m_bMouseMove == true)
    {
        m_bMouseMove = false;
    }
    return false;
}

bool LY_MarkManageService::OnMouseMove(QMouseEvent *e)
{
    if (m_bMouseMove == false)
    {
        return false;
    }

    if (m_pDlgZoneList->GetZoneEditDialog()->GetPickPointStates() == PICK_AS_POINT_NORMAL)
    {
        return false;
    }

    QPoint pt = e->pos();
    double dLongitude, Latitude;
    m_pProj->Screen2Jwd(pt, dLongitude, Latitude);
    if (m_pDlgZoneList != nullptr)
    {
        if (m_pDlgZoneList->GetCombatZoneDialog() != nullptr)
        {
            if (!m_pDlgZoneList->GetCombatZoneDialog()->isHidden())
            {
                double dHeight(0), dRadius(0);
                m_pDlgZoneList->GetZoneEditDialog()->SetWindowPoint(m_nSelectedID, dLongitude, Latitude, dHeight, dRadius);
            }
        }
    }

    LY_MarkManageServiceInterface::Get().Refresh(1);
    return false;
}

bool LY_MarkManageService::OnMouseDoubleClick(QMouseEvent *e)
{
    if (m_pDlgMarkEdit)
    {
        if (m_pDlgMarkEdit->GetPickPointStates() == PICK_POINT_EDIT)
        {
            QPoint pt = e->pos();
            double dLon(0), dLat(0), dHeight(0);
            m_pProj->Screen2Jwd(pt, dLon, dLat);
            m_pDlgMarkEdit->SetPostion(dLon, dLat, dHeight);
        }
    }

    if (m_pDlgZoneList && m_pDlgZoneList->GetZoneEditDialog())
    {
        if (m_pDlgZoneList->GetZoneEditDialog()->GetPickPointStates() == PICK_POINT_EDIT)
        {
            QPoint pt = e->pos();
            double dLon(0), dLat(0), dHeight(0);
            m_pProj->Screen2Jwd(pt, dLon, dLat);
            m_pDlgZoneList->GetZoneEditDialog()->SetPostion(dLon, dLat, dHeight);
        }
    }
    return false;
}

bool LY_MarkManageService::OnCommand(int nID)
{
	switch (nID)
	{
	case 900501:
	{
        if (m_pDlgMarkList != nullptr)
		{
			double dLon(0), dLat(0), dAlt(0);
            qnzkna::MarkManage::IMarkManageImpl::Get().GetThreatenSequencePosition(dLon,dLat,dAlt);
            m_pDlgMarkList->SetSearchZoneCenterPoint(dLon,dLat,dAlt);
		}
	}
	break;
	case 900502:
	{
        if (m_pDlgMarkList != nullptr)
		{
			double dLon(0), dLat(0), dAlt(0);
            qnzkna::MarkManage::IMarkManageImpl::Get().GetThreatenSequenceTakeOffpoint(dLon, dLat, dAlt);
            m_pDlgMarkList->SetTakeOffPoint(dLon, dLat, dAlt);
		}
	}
	break;
	default:
		break;
	}
    return false;
}

bool LY_MarkManageService::OnCreate(int wParam, QTabWidget *pTab)
{
    switch (wParam)
    {
    case 0:
    {

        InitMapLayerDesc();
        InitModelPatch();
        qnzkna::MarkManage::IMarkManageImpl::Get().InitMapIcon();

    }
    break;
    case 1:
    {
        LyDockWidget *pDockMarkListDialog = LyDockManager::instance()->dockWidget("MarkList");
        if (pDockMarkListDialog == nullptr)
        {
            pDockMarkListDialog = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("目标信息"), "MarkList");
            LyDockManager::instance()->embedToolDockWidget(pDockMarkListDialog, LyDockDef::Right);
        }

        if (m_pDlgMarkList == nullptr)
        {
            m_pDlgMarkList = new CDlgMarkList;
            pDockMarkListDialog->setCentralWidget(m_pDlgMarkList);
            m_pDlgMarkList->setWindowTitle(tr2("目标信息"));
            LyDockWidget::connect(m_pDlgMarkList, &CDlgMarkList::windowTitleChanged, pDockMarkListDialog, &LyDockWidget::setWindowTitle);
            m_pDlgMarkList->setWindowTitle(tr2("目标信息"));

            if (IMarkManageServiceActivator::Get().getPTaskManageService() != NULL)
            {
                IMarkManageServiceActivator::Get().getPTaskManageService()->Attach(m_pDlgMarkList);
            }
        }

        LyDockWidget *pDocZoneListDialog = LyDockManager::instance()->dockWidget("ZoneList");
        if (pDocZoneListDialog == nullptr)
		{
            pDocZoneListDialog = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, tr2("区域列表"), "ZoneList");
            LyDockManager::instance()->embedToolDockWidget(pDocZoneListDialog, LyDockDef::Right);
		}
        if (m_pDlgZoneList == nullptr)
		{
            m_pDlgZoneList = new CDlgZoneList;
            pDocZoneListDialog->setCentralWidget(m_pDlgZoneList);
            m_pDlgZoneList->setWindowTitle(tr2("区域列表"));
            LyDockWidget::connect(m_pDlgZoneList, &CDlgMarkList::windowTitleChanged, pDocZoneListDialog, &LyDockWidget::setWindowTitle);
		}

        if (NULL == m_pDlgMarkEdit) {
            m_pDlgMarkEdit = new CDlgMarkEdit;

            m_pDlgMarkEdit->setWindowTitle(tr2("目标编辑"));
            m_pDlgMarkEdit->hide();
        }

        if (NULL == m_pDlgMarkImage) {
            m_pDlgMarkImage = new CDlgMarkImage;
            m_pDlgMarkImage->setWindowTitle(QObject::tr("MarkImage"));
            m_pDlgMarkImage->hide();
            qnzkna::MarkManage::IMarkManageImpl *pMarkManageImpl = &(qnzkna::MarkManage::IMarkManageImpl::Get());
            m_pDlgMarkImage->setMarkManageImpl(pMarkManageImpl);
        }

        if (NULL == m_pDlgMarkPicture)
        {
            m_pDlgMarkPicture = new CDlgMarkPicture;
            m_pDlgMarkPicture->setWindowTitle(QObject::tr("MarkImage"));
            m_pDlgMarkPicture->hide();
        }

        if (NULL == m_pDlgMarkVideo)
        {
            m_pDlgMarkVideo = new CDlgMarkVideo;
            m_pDlgMarkVideo->setWindowTitle(QObject::tr("MarkVideo"));
            m_pDlgMarkVideo->hide();
        }

        if (m_pDlgMarkMerge == nullptr)
        {
            m_pDlgMarkMerge = new CDlgMarkMerge;

            m_pDlgMarkMerge->setWindowTitle(("目标合并"));
            m_pDlgMarkMerge->hide();
        }
    }
    break;
    case 2:
    {

    }
    break;
    case 3:
    {

    }
    break;
    case 4:
        break;
    }
    return false;
}

bool LY_MarkManageService::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        LY_MarkManageServiceInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}

CDlgMarkEdit *LY_MarkManageService::GetDlgMark()
{
    return m_pDlgMarkEdit;
}

CDlgMarkList *LY_MarkManageService::GetDlgMarkList()
{
    return m_pDlgMarkList;
}

CDlgMarkAssign *LY_MarkManageService::GetDlgMarkAssign()
{
    return m_pDlgMarkAssign;
}

CDlgMarkImage *LY_MarkManageService::GetDlgMarkImage()
{
    return m_pDlgMarkImage;
}

CDlgMarkPicture *LY_MarkManageService::GetDlgMarkPicture()
{
    return m_pDlgMarkPicture;
}

CDlgMarkVideo *LY_MarkManageService::GetDlgMarkVideo()
{
    return m_pDlgMarkVideo;
}

CDlgMarkMerge *LY_MarkManageService::getDlgMarkMerge()
{
    return m_pDlgMarkMerge;
}

CDlgZoneList* LY_MarkManageService::GetDlgZoneList()
{
	return m_pDlgZoneList;
}
void LY_MarkManageService::CreateMenu()
{
    m_MarkMenu = new QMenu();
    m_Menu_Picture = m_MarkMenu->addAction(QObject::tr("MarkImage"));
    m_Menu_Video = m_MarkMenu->addAction(QObject::tr("MarkVideo"));
    m_Menu_Task = m_MarkMenu->addAction(QObject::tr("SingleObjectiveTaskPlan"));
    m_Menu_Scheme = m_MarkMenu->addAction(QObject::tr("MarkStrikePlan"));
    m_Menu_MarkState = m_MarkMenu->addAction(QObject::tr("Current state of the Mark"));

    m_Mark_State = new QMenu();
    m_Menu_Destroy = m_Mark_State->addAction(QObject::tr("Complete des truction"));
    m_Menu_Pressing = m_Mark_State->addAction(QObject::tr("Complete suppression"));
    m_Menu_Attack = m_Mark_State->addAction(QObject::tr("Attacking "));
    m_Menu_Undestructed = m_Mark_State->addAction(QObject::tr("Incomplete desteuction"));
    m_Menu_Unsuppressed = m_Mark_State->addAction(QObject::tr("Incomplete suppression"));
    m_Menu_MarkState->setMenu(m_Mark_State);

    QObject::connect(m_Menu_Picture, &QAction::triggered, [this]()
    {
        on_Menu_Picture();
    });
    QObject::connect(m_Menu_Video, &QAction::triggered, [this]()
    {
        on_Menu_Video();
    });
    QObject::connect(m_Menu_Task, &QAction::triggered, [this]()
    {
        on_Menu_Task();
    });
    QObject::connect(m_Menu_Scheme, &QAction::triggered, [this] {on_Menu_Scheme();});

    QObject::connect(m_Menu_Destroy, &QAction::triggered, [this]()
    {
        on_Menu_Destroy();
    });
    QObject::connect(m_Menu_Pressing, &QAction::triggered, [this]()
    {
        on_Menu_Pressing();
    });
    QObject::connect(m_Menu_Attack, &QAction::triggered, [this]()
    {
        on_Menu_Attack();
    });
    QObject::connect(m_Menu_Undestructed, &QAction::triggered, [this] {on_Menu_Undestructed(); });
    QObject::connect(m_Menu_Unsuppressed, &QAction::triggered, [this] {on_Menu_Unsuppressed(); });
}
void LY_MarkManageService::on_Menu_Task()
{
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    props.put("TargetName", m_SelectedMarkID);
    IMarkManageServiceActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgOnlyMarkTaskSend", pBuf, nBufLen, props));
}

void LY_MarkManageService::on_Menu_Scheme()
{
    char pBuf[MAX_BUFFER_LEN] = { 0 };
    int nBufLen = 0;
    qnzkna::framework::MessageReferenceProps  props;
    props.put("TargetName", m_SelectedMarkID);
    IMarkManageServiceActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgOnlyMarkSchemeSend", pBuf, nBufLen, props));
}

void LY_MarkManageService::on_Menu_Destroy()
{
    Update_MarkState(1);
}

void LY_MarkManageService::on_Menu_Pressing()
{
    Update_MarkState(2);
}

void LY_MarkManageService::on_Menu_Attack()
{
    Update_MarkState(3);
}

void LY_MarkManageService::on_Menu_Undestructed()
{
    Update_MarkState(4);
}

void LY_MarkManageService::on_Menu_Unsuppressed()
{
    Update_MarkState(5);
}

void LY_MarkManageService::Update_MarkState(int Num)
{
    if (IMarkManageServiceActivator::Get().getPTaskManageService() == nullptr)
    {
        return;
    }
    int nActionID = IMarkManageServiceActivator::Get().getPTaskManageService()->GetActivatingActionID();

    CSystemMarkInfoVec m_MarkInfoVec;

    CRelationActionMarkVec m_RelationActionMarkVec = IMarkManageServiceActivator::Get().getPTaskManageService()->GetRelationMark(nActionID);

    for (auto data : m_RelationActionMarkVec)
    {
        SystemMarkInfo stinfo = qnzkna::MarkManage::IMarkManageImpl::Get().GetMark(data.GetMarkID());
        if (stinfo.GetID() != "")
        {
            m_MarkInfoVec.push_back(stinfo);
        }
    }
    QString strID = QString::fromStdString(m_SelectedMarkID);
    for (auto data : m_MarkInfoVec)
    {
        if (0 == QString::compare(strID, QString::fromStdString(data.GetID())))
        {
            data.SetState(data.GetStateENum(Num));
            qnzkna::MarkManage::IMarkManageImpl::Get().UpdateTaregtState(data);
            break;
        }
    }
    m_pDlgMarkList->on_Menu_Update();
}

void LY_MarkManageService::on_Menu_Picture()
{
    if (GetDlgMarkPicture() != nullptr)
    {
        std::vector<std::string> ImageList;
        bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().GetMarkImageNamesList(m_SelectedMarkID, &ImageList);
        if (ImageList.size() == 0)
        {
            LyMessageBox::information(nullptr, QObject::tr("Prompt"), QObject::tr("The current Mark has not taken any pictures!"));
            return;
        }
        if (GetDlgMarkPicture()->isVisible())
        {
            GetDlgMarkPicture()->hide();
        }
        GetDlgMarkPicture()->SetPicNameVec(ImageList);
        GetDlgMarkPicture()->show();
    }
}

void LY_MarkManageService::on_Menu_Video()
{
    if (GetDlgMarkVideo() != nullptr)
    {
        std::vector<std::string> ImageList;
        bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().GetMarkVideoNamesList(m_SelectedMarkID, &ImageList);
        if (ImageList.size() == 0)
        {
            LyMessageBox::information(nullptr, QObject::tr("Prompt"), QObject::tr("The current Mark has not captured a video!"));
            return;
        }
        if (GetDlgMarkVideo()->isVisible())
        {
            GetDlgMarkVideo()->hide();
        }
        GetDlgMarkVideo()->SetPicNameVec(ImageList);
        GetDlgMarkVideo()->show();
    }

}

bool LY_MarkManageService::JudgePoint(const QPoint &pt, int &nPoint)
{
    if (!m_pDlgZoneList->GetCombatZoneDialog()->isHidden())
    {
        ZoneWindowPointVec wndPointMap;
        EZoneShape eShape;
        if (m_pDlgZoneList->GetZoneEditDialog()->GetWindowPointMap(eShape, wndPointMap))
        {
            if (eShape == EZoneShape_Polygon)
            {
                int nNo = 0;
                for (auto stpoint : wndPointMap)
                {
                    QPoint ptPoint = m_pProj->Jwd2Screen(stpoint.dLon, stpoint.dLat);

                    if (MatchLinePoint(ptPoint, pt))
                    {
                        nPoint = nNo;
                        return true;
                    }
                    nNo++;
                }
            }
        }
    }

    return false;
}

bool LY_MarkManageService::MatchLinePoint(const QPoint &mp, const QPoint &lp)
{
    if (abs(mp.x() - lp.x()) <= LINE_LENGTH && abs(mp.y() - lp.y()) <= LINE_LENGTH)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LY_MarkManageService::JudgeMark(const QPoint &pt, std::string &strMarkID)
{
    CSystemMarkInfoVec MarkInfoVec;
    MarkInfoVec = qnzkna::MarkManage::IMarkManageImpl::Get().GetMarkList();

    if (MarkInfoVec.size() == 0)
    {
        return false;
    }

    for (auto Mark : MarkInfoVec)
    {
        QPoint ptPoint = m_pProj->Jwd2Screen(Mark.GetLon(), Mark.GetLat());
        if (MatchMarkPoint(ptPoint, pt))
        {
            strMarkID = Mark.GetID();
            return true;
        }
    }
    return false;
}

bool LY_MarkManageService::MatchMarkPoint(const QPoint &mp, const QPoint &lp)
{
    if (abs(mp.x() - lp.x()) <= 3 * LINE_LENGTH && abs(mp.y() - lp.y()) <= 3 * LINE_LENGTH)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LY_MarkManageService::MarkMouseButtonDown(QMouseEvent *e)
{

    QPoint pt1 = e->globalPos();
    QPoint pt2 = e->pos();
    if (JudgeMark(pt2, m_SelectedMarkID))
    {
    }
    else
    {
        m_SelectedMarkID = "";
    }
    if (e->button() == Qt::RightButton && m_SelectedMarkID != "")
    {

        m_MarkMenu->exec(pt1);
    }
    return false;
}

#define MARK_SELECT_LENGTH 18
void LY_MarkManageService::DrawMarkSelected(QPainter &painter, IProj *pProj)
{
    if (m_SelectedMarkID == "")
    {
        return;
    }

    SystemMarkInfo stInfo = qnzkna::MarkManage::IMarkManageImpl::Get().GetMark(m_SelectedMarkID);
    QPoint ptCenter = pProj->Jwd2Screen(stInfo.GetLon(), stInfo.GetLat());
    QPoint ptFirst, ptSceond;

    QPen pen(Qt::green);
    pen.setWidth(4);
    painter.setPen(pen);

    ptFirst.setX(ptCenter.rx() - MARK_SELECT_LENGTH);
    ptFirst.setY(ptCenter.ry() - MARK_SELECT_LENGTH);
    ptSceond.setX(ptCenter.rx() - MARK_SELECT_LENGTH / 2);
    ptSceond.setY(ptCenter.ry() - MARK_SELECT_LENGTH);
    painter.drawLine(ptFirst, ptSceond);
    ptFirst.setX(ptCenter.rx() - MARK_SELECT_LENGTH);
    ptFirst.setY(ptCenter.ry() - MARK_SELECT_LENGTH);
    ptSceond.setX(ptCenter.rx() - MARK_SELECT_LENGTH);
    ptSceond.setY(ptCenter.ry() - MARK_SELECT_LENGTH / 2);
    painter.drawLine(ptFirst, ptSceond);

    ptFirst.setX(ptCenter.rx() - MARK_SELECT_LENGTH);
    ptFirst.setY(ptCenter.ry() + MARK_SELECT_LENGTH);
    ptSceond.setX(ptCenter.rx() - MARK_SELECT_LENGTH / 2);
    ptSceond.setY(ptCenter.ry() + MARK_SELECT_LENGTH);
    painter.drawLine(ptFirst, ptSceond);
    ptFirst.setX(ptCenter.rx() - MARK_SELECT_LENGTH);
    ptFirst.setY(ptCenter.ry() + MARK_SELECT_LENGTH);
    ptSceond.setX(ptCenter.rx() - MARK_SELECT_LENGTH);
    ptSceond.setY(ptCenter.ry() + MARK_SELECT_LENGTH / 2);
    painter.drawLine(ptFirst, ptSceond);

    ptFirst.setX(ptCenter.rx() + MARK_SELECT_LENGTH);
    ptFirst.setY(ptCenter.ry() - MARK_SELECT_LENGTH);
    ptSceond.setX(ptCenter.rx() + MARK_SELECT_LENGTH / 2);
    ptSceond.setY(ptCenter.ry() - MARK_SELECT_LENGTH);
    painter.drawLine(ptFirst, ptSceond);
    ptFirst.setX(ptCenter.rx() + MARK_SELECT_LENGTH);
    ptFirst.setY(ptCenter.ry() - MARK_SELECT_LENGTH);
    ptSceond.setX(ptCenter.rx() + MARK_SELECT_LENGTH);
    ptSceond.setY(ptCenter.ry() - MARK_SELECT_LENGTH / 2);
    painter.drawLine(ptFirst, ptSceond);

    ptFirst.setX(ptCenter.rx() + MARK_SELECT_LENGTH);
    ptFirst.setY(ptCenter.ry() + MARK_SELECT_LENGTH);
    ptSceond.setX(ptCenter.rx() + MARK_SELECT_LENGTH / 2);
    ptSceond.setY(ptCenter.ry() + MARK_SELECT_LENGTH);
    painter.drawLine(ptFirst, ptSceond);
    ptFirst.setX(ptCenter.rx() + MARK_SELECT_LENGTH);
    ptFirst.setY(ptCenter.ry() + MARK_SELECT_LENGTH);
    ptSceond.setX(ptCenter.rx() + MARK_SELECT_LENGTH);
    ptSceond.setY(ptCenter.ry() + MARK_SELECT_LENGTH / 2);
    painter.drawLine(ptFirst, ptSceond);
}

void LY_MarkManageService::DrawMovingMark(QPainter &painter, IProj *pProj)
{
    if (IMarkManageServiceActivator::Get().getPTaskManageService() == nullptr)
    {
        return;
    }
    int nActionID = IMarkManageServiceActivator::Get().getPTaskManageService()->GetSelectedActivatingActionIDLocal();

    CSystemMarkInfoVec MarkInfoVec;

    CRelationActionMarkVec m_RelationActionMarkVec = IMarkManageServiceActivator::Get().getPTaskManageService()->GetRelationMark(nActionID);

    for (auto data : m_RelationActionMarkVec)
    {
        SystemMarkInfo stinfo = qnzkna::MarkManage::IMarkManageImpl::Get().GetMark(data.GetMarkID());
        if (stinfo.GetID() != "")
        {
            MarkInfoVec.push_back(stinfo);

            CSystemMarkInfoVec PosVec = qnzkna::MarkManage::IMarkManageImpl::Get().GetMarkPosList(data.GetMarkID());

            QPoint ptFirst;
            QPen pen(Qt::red);
            painter.setPen(pen);
            int nNo = 1;
            if (PosVec.size() > 1)
            {
                for (auto MovingMark : PosVec)
                {
                    double dLongitude = MovingMark.GetLon();
                    double dLatitude = MovingMark.GetLat();
                    int dHeight = MovingMark.GetHeight();

                    QPoint ptPoint = pProj->Jwd2Screen(dLongitude, dLatitude);

                    if (nNo == 1)
                    {
                        ptFirst = ptPoint;
                    }
                    else
                    {

                        painter.setPen(pen);
                        painter.drawLine(ptFirst, ptPoint);
                        ptFirst = ptPoint;
                    }

                    painter.setPen(pen);
                    painter.setBrush(Qt::red);

                    const int rx = 2;
                    painter.drawEllipse(ptPoint, rx, rx);

                    nNo++;
                }
            }
        }
    }
}

void LY_MarkManageService::SetScreenCenter(double &dLon, double &dLat)
{
    QString str = "CenterAt(" + QString::number(dLon, 'f', 8) + "," + QString::number(dLat, 'f', 8) + ")";
    LY_MarkManageServiceInterface::Get().Script(str.toLocal8Bit());

}

void LY_MarkManageService::InitMapLayerDesc()
{
    SystemMarkInfo stInfo;
    QString strID;
    ld.m_bVisual = 1;
    ld.m_color = QColor(255, 0, 0);
	ld.m_clrText = QColor(255, 0, 0);
    ld.m_clrFill = QColor(255, 0, 0);
	ld.m_strFilePath = "Point?field=id:integer&field=名称:string(20)&field=aaaa:integer&index=yes";

    ld.m_bBiaoZhu = 1;
	ld.m_strBiaoZhuField = "名称";
    ld.m_nBiaoZhuSize = 12;
    ld.m_strBiaoZhuFontName = "黑体";
    ld.m_nSymbolSize = 8;
	ld.m_nSymbolStyle = 6;

	ld.m_strSymbolFontName = "images/fire.png";

	ld.m_strRotationField = "aaaa";

	ld.m_strLayerName = GetMarkTypeString(ESystemMarkType_Fire).toLocal8Bit().data();
    ld.m_nSymbolIndex = GetMarkAttributeSymbolIndex(ESystemMarkAttribute_Me);

    ld.m_strCategoryName = GetMarkAttributeLayerString(ESystemMarkAttribute_Me).toLocal8Bit().data();
    LY_MarkManageServiceInterface::Get().AddMemLayer(ld);

    ld.m_strLayerName = GetMarkTypeString(ESystemMarkType_Fire).toLocal8Bit().data();
    ld.m_nSymbolIndex = GetMarkAttributeSymbolIndex(ESystemMarkAttribute_Enemy);
    ld.m_strCategoryName = GetMarkAttributeLayerString(ESystemMarkAttribute_Enemy).toLocal8Bit().data();
    LY_MarkManageServiceInterface::Get().AddMemLayer(ld);

    ld.m_strLayerName = GetMarkTypeString(ESystemMarkType_Fire).toLocal8Bit().data();
    ld.m_nSymbolIndex = GetMarkAttributeSymbolIndex(ESystemMarkAttribute_Unknow);

    ld.m_strCategoryName = GetMarkAttributeLayerString(ESystemMarkAttribute_Unknow).toLocal8Bit().data();
    LY_MarkManageServiceInterface::Get().AddMemLayer(ld);
}

void LY_MarkManageService::InitModelPatch()
{
    m_mapModelPath.clear();

    FILE *fp = fopen(m_strConfDir + "/MilitaryModel.txt", "r+t");
    if (fp != NULL)
    {
        while (!feof(fp))
        {
            int n[6];
            char szT[256];
            szT[0] = 0;
            if (fscanf(fp, "%d %d %d %d %d %d %s", &n[0], &n[1], &n[2], &n[3], &n[4], &n[5], szT) == 7)
            {
                if (strlen(szT))
                {
                    ModelInfo mi;
                    mi.scale = n[1];
                    mi.height = n[2];
                    mi.yaw = n[3];
                    mi.pitch = n[4];
                    mi.roll = n[5];
                    mi.path = szT;
                    m_mapModelPath[n[0]] = mi;
                }
            }
        }
        fclose(fp);
    }
}

int LY_MarkManageService::AddIcon(double &dLon, double &dLat, ESystemMarkType &eType, ESystemMarkAttribute eAttribute, VecStr &vs)
{
    double dX1 = 0, dY1 = 0;
    m_pProj->jwxy(dLon, dLat, &dX1, &dY1);
    int nID;
    CCreateShape cs;
    SHPObject *pShp = cs.CreatePoint(dX1, dY1);
    QCString str1 = GetMarkAttributeLayerString(eAttribute).toLocal8Bit().data();
    QCString str2 = GetMarkTypeString(eType).toLocal8Bit().data();
    QCString strID = str1 + "/" + str2;

    nID = LY_MarkManageServiceInterface::Get().UpdateMemLayer(strID, pShp, vs);
    SHPDestroyObject(pShp);
    return nID;
}

bool LY_MarkManageService::Add3DIcon(SystemMarkInfo &stInfo)
{
    static IEarth *spEarth = IEarth::GetEarth();
    static IGraphics *spGgraphics = NULL;
    if (spGgraphics == NULL && spEarth != NULL)
    {
        spGgraphics = spEarth->GetGraphics();
    }

    if (spGgraphics == NULL)
    {
        return false;
    }

    QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(stInfo.GetUuid(), 16));

	double dAlt = s_CGetHeight.GetHeight(stInfo.GetLon(), stInfo.GetLat())+ 15;
    if (m_mapLabel.find(uid) == m_mapLabel.end())
    {
        std::string strImg;
        auto it = m_mapLabelPath.find(uid);
        if (it == m_mapLabelPath.end())
        {
            QString strPath = QObject::tr(m_strDataDir);
			strPath += QLatin1String("images/fire.png");
			QImage img = QImage(strPath);
			strPath = QObject::tr(m_strDataDir);
            strPath += QLatin1String("/temp/");
            strPath += uid.toString(QUuid::WithoutBraces);
            strPath += QLatin1String(".png");
            QFile::remove(strPath);
            img.save(strPath);
            strImg = tr3(strPath).data();
            m_mapLabelPath[uid] = strImg;
        }
        else
        {
            strImg = it->second;
        }
		QString strLable = tr2("名称：")+QString::fromStdString(stInfo.GetName());
		ILabel *pLabel = spGgraphics->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(), stInfo.GetLon(), stInfo.GetLat(), dAlt, 36, 36, 0);
        m_mapLabel[uid] = pLabel;
    }

    if (m_mapModel.find(uid) == m_mapModel.end())
    {
		del3D_fire(stInfo.GetID());
		add3D_fire(stInfo.GetID(), stInfo.GetLon(), stInfo.GetLat(), dAlt,stInfo.GetSizeX());

    }

    return true;
}

int LY_MarkManageService::EditIcon(double &dLon, double &dLat, ESystemMarkType &eType, ESystemMarkAttribute eAttribute, VecStr &vs)
{
    double dX1 = 0, dY1 = 0;
    m_pProj->jwxy(dLon, dLat, &dX1, &dY1);
    int nID;
    CCreateShape cs;
    SHPObject *pShp = cs.CreatePoint(dX1, dY1);
    QCString str1 = GetMarkAttributeLayerString(eAttribute).toLocal8Bit().data();
    QCString str2 = GetMarkTypeString(eType).toLocal8Bit().data();
    QCString strID = str1 + "/" + str2;

    nID = LY_MarkManageServiceInterface::Get().UpdateMemLayer(strID, pShp, vs);
    SHPDestroyObject(pShp);
    return nID;
}

void LY_MarkManageService::DeleteIcon(ESystemMarkType &eType, ESystemMarkAttribute eAttribute, int &nId)
{
    QCString str1 = GetMarkAttributeLayerString(eAttribute).toLocal8Bit().data();
    QCString str2 = GetMarkTypeString(eType).toLocal8Bit().data();
    QCString strID = str1 + "/" + str2;

    std::vector<int> vi;
    vi.push_back(nId);
    LY_MarkManageServiceInterface::Get().DeleteFeatures(strID, vi);
}

void LY_MarkManageService::Delete3DIcon(SystemMarkInfo &stInfo)
{
    IEarth *spEarth = IEarth::GetEarth();
    if (spEarth != NULL)
    {
        IGraphics *pGgraphics = spEarth->GetGraphics();
        if (pGgraphics != NULL)
        {
            QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(stInfo.GetUuid(), 16));

            auto it = m_mapLabel.find(uid);

            if (it != m_mapLabel.end())
            {
                pGgraphics->RemoveObject(it->second);
                m_mapLabel.erase(it);
            }

            auto itPatch = m_mapLabelPath.find(uid);
            if (itPatch != m_mapLabelPath.end())
            {
                m_mapLabelPath.erase(itPatch);
            }

			del3D_fire(stInfo.GetID());
        }
    }
}

void LY_MarkManageService::Clear3DZones()
{
    auto itr = m_3DZoneMesh2Vec.begin();
    for (; itr != m_3DZoneMesh2Vec.end(); itr++)
    {
        IEarth *pEarth = IEarth::GetEarth();
        pEarth->GetGraphics()->RemoveObject(*itr);
        *itr = nullptr;
        delete *itr;
    }
    m_3DZoneMesh2Vec.clear();

    auto itr1 = m_3DZoneMesh1Vec.begin();
    for (; itr1 != m_3DZoneMesh1Vec.end(); itr1++)
    {
        IEarth *pEarth = IEarth::GetEarth();
        pEarth->GetGraphics()->RemoveObject(*itr1);
        *itr1 = nullptr;
        delete *itr1;
    }
    m_3DZoneMesh1Vec.clear();

    auto itr2 = m_3DZoneMesh3Vec.begin();
    for (; itr2 != m_3DZoneMesh3Vec.end(); itr2++)
    {
        IEarth *pEarth = IEarth::GetEarth();
        pEarth->GetGraphics()->RemoveObject(*itr2);
        *itr2 = nullptr;
        delete *itr2;
    }
    m_3DZoneMesh3Vec.clear();
}

void LY_MarkManageService::Draw3DZones()
{

    if (m_pDlgZoneList == nullptr)
    {
        return ;
    }
    CZoneInfoVec infVec = m_pDlgZoneList->GetZoneInfoVec();

    if (infVec.size() == 0)
    {
        return ;
    }
    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth == nullptr)
    {
        return;
    }
    Clear3DZones();

    for (auto Airinfo : infVec)
    {
        if (Airinfo.GetShowStates() == true)
        {
            if (Airinfo.GetShape() == EZoneShape_Circle)
            {
                ZonePointVec stPointVec = Airinfo.GetPoints();
                for (auto stpoint : stPointVec)
                {
                    QColor colorBombTrack((m_nColor[Airinfo.GetAttributeENum(Airinfo.GetAttribute())]));
                    colorBombTrack.setAlpha(90);
                    QColor colorTemp;
                    colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());
                    createUserMesh2FromShp(pEarth->GetGraphics(), m_3DZoneMesh2Vec, colorTemp.rgba(), stpoint.dLon, stpoint.dLat, stpoint.dRadius, 0, stpoint.dHeight);
                }
            }
            else if (Airinfo.GetShape() == EZoneShape_Polygon)
            {
                QColor colorBombTrack((m_nColor[Airinfo.GetAttributeENum(Airinfo.GetAttribute())]));
                colorBombTrack.setAlpha(40);
                QColor colorTemp;
                colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());

                std::vector<double> pdLon;
                std::vector<double> pdLat;
                std::vector<double> pdHeight;

                ZonePointVec stPointVec = Airinfo.GetPoints();
                double dAlt = 0;
                for (auto stpoint : stPointVec)
                {
                    pdLon.push_back(stpoint.dLon);
                    pdLat.push_back(stpoint.dLat);
                    pdHeight.push_back(0);
                    dAlt = stpoint.dHeight;
                }
                createUserMesh1Polygon(pEarth->GetGraphics(), m_3DZoneMesh1Vec, colorTemp.rgba(), pdLon.data(), pdLat.data(), pdHeight.data(), stPointVec.size(), dAlt);
            }
            else if (Airinfo.GetShape() == EZoneShape_Sector)
            {
                QColor colorBombTrack((m_nColor[Airinfo.GetAttributeENum(Airinfo.GetAttribute())]));
                colorBombTrack.setAlpha(40);
                QColor colorTemp;
                colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());
                double dLon = 0;
                double dLat = 0;
                double dHeight = 0;
                double pdStart = 0;
                double pdEnd = 0;
                double pdRedis = 0;
                ZonePointVec stPointVec = Airinfo.GetPoints();
                double dAlt = 0;
                QVector<QPointF> tmpVitalPoly;
                for (auto stpoint : stPointVec)
                {
                    double pdLon = stpoint.dLon;
                    double pdLat = stpoint.dLat;
                    dAlt = stpoint.dHeight;
                    pdStart = stpoint.dStartAngle;
                    pdEnd = stpoint.dEndAngle;
                    pdRedis = stpoint.dRadius;
                }

                createUserMesh1Pie(pEarth->GetGraphics(), m_3DZoneMesh3Vec, colorTemp.rgba(), dLon, dLat, dHeight, pdRedis, pdStart, pdEnd);
            }
        }
    }
}

QString LY_MarkManageService::GetMarkTypeString(ESystemMarkType eType)
{
    switch (eType)
    {
    case ESystemMarkType_Unknow:
		return "";
    case ESystemMarkType_UAV:
		return QObject::tr("UAV");
    case ESystemMarkType_Person:
		return  QObject::tr("Person");
    case ESystemMarkType_Vehicle:
		return  QObject::tr("Vehicle");
    case ESystemMarkType_Building:
		return  QObject::tr("Building");
    case ESystemMarkType_Fire:
		return  QObject::tr("Fire");
    default:
        return "";
    }
}

QString LY_MarkManageService::GetMarkAttributeLayerString(ESystemMarkAttribute eAttribute)
{
    switch (eAttribute)
    {
    case ESystemMarkAttribute_Unknow:
        return QObject::tr("TaskMark/Other");
    case ESystemMarkAttribute_Enemy:
        return QObject::tr("TaskMark/Enemy");
    case ESystemMarkAttribute_Me:
        return QObject::tr("TaskMark/Me");
    case ESystemMarkAttribute_Friend:
        return QObject::tr("TaskMark/Other");
    case ESystemMarkAttribute_Neutral:
        return QObject::tr("TaskMark/Other");
    case ESystemMarkAttribute_Civil:
        return QObject::tr("TaskMark/Other");
	default:
        return QObject::tr("TaskMark/Other");
    }
}

QString LY_MarkManageService::GetMarkAttributeString(ESystemMarkAttribute eAttribute)
{
    switch (eAttribute)
    {
    case ESystemMarkAttribute_Unknow:
        return QObject::tr("Unknow");
    case ESystemMarkAttribute_Enemy:
        return QObject::tr("Enemy");
    case ESystemMarkAttribute_Me:
        return QObject::tr("Me");
    case ESystemMarkAttribute_Friend:
        return QObject::tr("Friend");
    case ESystemMarkAttribute_Neutral:
        return QObject::tr("Neutral");
    case ESystemMarkAttribute_Civil:
        return QObject::tr("Civil");
    default:
        return QObject::tr("Unknow");
    }
}

int LY_MarkManageService::GetMarkAttributeSymbolIndex(ESystemMarkAttribute eAttribute)
{
    switch (eAttribute)
    {
    case ESystemMarkAttribute_Unknow:
        return 3;
    case ESystemMarkAttribute_Enemy:
        return 1;
    case ESystemMarkAttribute_Me:
        return 2;
    case ESystemMarkAttribute_Friend:
        return 2;
    case ESystemMarkAttribute_Neutral:
        return 3;
    case ESystemMarkAttribute_Civil:
        return 3;
    default:
        return 3;
    }

}

QString LY_MarkManageService::GetMarkSymbolFontName(ESystemMarkType eType)
{
    switch (eType)
    {
    case ESystemMarkType_Unknow:
		return "";
    case ESystemMarkType_UAV:
		return ("13000");
    case ESystemMarkType_Person:
		return ("1200");
    case ESystemMarkType_Vehicle:
		return ("6800");
    case ESystemMarkType_Building:
		return ("22600");
    case ESystemMarkType_Fire:
		return ("54402");
    default:
        return "";
    }

}

void LY_MarkManageService::clearDir(const QString &path)
{
    QDir m_dir(path);
    if (m_dir.isEmpty())
    {
        qDebug() << "临时文件文件夹" << path << "为空";
        m_dir.removeRecursively();
        return;
    }

    QDirIterator DirsIterator(path, QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    while (DirsIterator.hasNext())
    {
        if (!m_dir.remove(DirsIterator.next()))
        {

        }
    }

}

void LY_MarkManageService::createUserMesh2FromShp(IGraphics *pGraphics, std::vector<IUserMesh2 *> &wndVec, ARGB ar, double dLng, double dLat, double dR, double dAltitude, double dM )
{
    CCreateShape cs;
    SHPObject *pShp = cs.CreateCircle(0, 0, dR, 60000);
    if (pShp == nullptr)
    {
        return;
    }
    IUserMesh2 *pUserMesh = pGraphics->CreateUserMesh2(pShp);

    pUserMesh->SetColor(ar);
    pUserMesh->SetPosition(dLng, dLat, dAltitude + dM);
    wndVec.push_back(pUserMesh);

    double dZ = 0;
    SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
    pUserMesh = pGraphics->CreateUserMesh2(pShp1);

    pUserMesh->SetColor(ar);
    pUserMesh->SetPosition(dLng, dLat, dAltitude);
    wndVec.push_back(pUserMesh);
}

void LY_MarkManageService::createUserMesh1Polygon(IGraphics *pGraphics, std::vector<IUserMesh1 *> &wndVec, ARGB ar, double *dLng, double *dLat, double *dAlt, int nCount, double dM)
{

    IEarth *pEarth = IEarth::GetEarth();

    CCreateShape cs;
    SHPObject *pShp = cs.CreatePolygon(dLng, dLat, nCount, 100, true, nullptr, dAlt);
    if (NULL == pShp)
    {
        return;
    }
    IUserMesh1 *pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
    pUM->SetColor(ar);
    wndVec.push_back(pUM);

    double dZ = 0;
    SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dM);
    IUserMesh1 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
    pUserMesh->SetColor(ar);
    wndVec.push_back(pUserMesh);
}

QVector<QPointF> LY_MarkManageService::getCircleQVec(const QPointF &center, int dis, double startAngle, double endAngle, int muti)
{
    QVector<QPointF> rePoss;
    rePoss.clear();

    QGeoCoordinate geoCenter(center.y(), center.x());
    if (fabs(endAngle - startAngle) != 360)
    {
        rePoss.push_back(center * muti);
    }

    double minAngle = startAngle;
    double maxAngle = startAngle;
    if (startAngle != endAngle)
    {
        minAngle = startAngle < endAngle ? startAngle : endAngle;
        maxAngle = startAngle > endAngle ? startAngle : endAngle;
    }

    minAngle = fmod(minAngle, 360);
    maxAngle = fmod(maxAngle, 360);

    for (double i = minAngle; i < maxAngle; i += 0.5)
    {
        QGeoCoordinate tmpGeo = geoCenter.atDistanceAndAzimuth(dis, i);
        rePoss.push_back(QPointF(tmpGeo.longitude() * muti, tmpGeo.latitude() * muti));
    }
    if (fabs(endAngle - startAngle) != 360)
    {
        rePoss.push_back(center * muti);
    }
    return rePoss;
}

void LY_MarkManageService::add3D_fire(std::string MarkID, double lon, double lat, int height, float size)
{
	if(size == 0)
	{
		size = 5.0;
	}
	osgEarth::MapNode* mapNode = (osgEarth::MapNode*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EarthMapNode));
	if (m_fire_MatrixTransformPtr == nullptr)
	{

		m_fire_MatrixTransformPtr = new osg::Group;
		mapNode->addChild(m_fire_MatrixTransformPtr);
	}
	osg::ref_ptr<osg::MatrixTransform> fireModel = new osg::MatrixTransform;
	fireModel->setName(MarkID);
	osg::Matrixd posMatrix;
	osg::EllipsoidModel* em = (osg::EllipsoidModel*)(IEarth::GetEarth()->GetPtr(IEarth::EPtrType_EllipsoidModel));
	double height_t = 0.0;
	mapNode->getTerrain()->getHeight(mapNode->getMapSRS(), lon, lat, &height_t);
	em->computeLocalToWorldTransformFromLatLongHeight(osg::inDegrees(lat), osg::inDegrees(lon), height, posMatrix);
	fireModel->setMatrix(posMatrix);
	createFireBall(fireModel.get(), size);
	m_fire_MatrixTransformPtr->addChild(fireModel.get());
}

void LY_MarkManageService::del3D_fire(std::string MarkID)
{
	if (m_fire_MatrixTransformPtr == nullptr)
	{
		return;
	}
	int childrenNum = m_fire_MatrixTransformPtr->getNumChildren();
	for (int index = 0; index < childrenNum; index++)
	{
		if (m_fire_MatrixTransformPtr->getChild(index)->getName() == MarkID)
		{
			m_fire_MatrixTransformPtr->removeChild(m_fire_MatrixTransformPtr->getChild(index));
			break;
		}
	}
}

void LY_MarkManageService::createUserMesh1Pie(IGraphics *pGraphics, std::vector<IUserMesh1 *> &wndVec, ARGB ar, double dLng, double dLat, double dAlt, double dRedis, double dStart, double dEnd)
{

    IEarth *pEarth = IEarth::GetEarth();

    CCreateShape cs;

    SHPObject *pShp = cs.CreateChord(dLng, dLat, dRedis, dStart, dEnd, 10000);
    if (NULL == pShp)
    {
        return;
    }
    IUserMesh1 *pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
    pUM->SetColor(ar);
    wndVec.push_back(pUM);

    double dZ = 0;
    SHPObject *pShp1 = cs.AddZM2SHPObject(true, pShp, &dZ, &dAlt);
    IUserMesh1 *pUserMesh = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
    pUserMesh->SetColor(ar);
    wndVec.push_back(pUserMesh);
}

