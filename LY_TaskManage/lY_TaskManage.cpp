#include "lY_TaskManage.h"
#include "lY_TaskManageInterface.h"
#include <IFunctionInterface.h>
#include "IProj.h"
#include "lyUI/LyDockManager.h"
#include "CreateShape.h"

#include "BiIntervisibility.h"
#include "DrawShape.h"

#include "IResourceCarryingPlanDialog.h"
#include "ISAddCarryingPlanResDialog.h"
#include "CDlgForecastTendency.h"

#include <QSettings>

Q_DECLARE_METATYPE(qnzkna::framework::IRegistry *)

#define  LINE_LENGTH			3
#define  LINE_LENGTH_JUDGE		6
#define  DRAW_SCAN_HZ			2

static CGetHeight *s_pCGetHeight = nullptr;
CGetHeight LY_TaskManage::s_CGetHeight("../data/dem/90");

LY_TaskManage::LY_TaskManage()
{
    m_pProj        = nullptr;
    m_pWnd         = nullptr;
    m_pTroopLabel = nullptr;
    m_pZzTimeLabel = nullptr;

    m_FlightTaskManageEditLineElevationDir = "../data/dem/90";

    QSettings settings("../data/conf/FlightTaskManageSettings.ini", QSettings::IniFormat);
    settings.beginGroup("FlightTaskManageEditLineSettings");
    if (settings.contains("FlightTaskManageEditLineElevationDir"))
    {
        QString flightTaskManageEditLineElevationDirTemp = settings.value("FlightTaskManageEditLineElevationDir", "").toString();
        if (!flightTaskManageEditLineElevationDirTemp.isEmpty())
        {
            if (QDir(flightTaskManageEditLineElevationDirTemp).exists())
            {
                m_FlightTaskManageEditLineElevationDir = flightTaskManageEditLineElevationDirTemp;
            }
        }
    }

    if (settings.contains("FlightTaskManageEditLineElevation"))
    {
        m_FlightTaskManageEditLineElevation = settings.value("FlightTaskManageEditLineElevation", true).toBool();
    }
    settings.endGroup();

    s_pCGetHeight = new CGetHeight(m_FlightTaskManageEditLineElevationDir.toStdString().c_str());

    int colorIndex = 0;
    m_nColor[colorIndex]   = 0xffff00;
    m_nColor[colorIndex++] = 0xffff00;

    m_nColor[colorIndex++] = 0x000000;

    m_nColor[colorIndex++] = 0x800000;

    m_nColor[colorIndex++] = 0x00ff00;

    m_nColor[colorIndex++] = 0xffa500;
    m_nColor[colorIndex++] = 0x404040;
    m_nColor[colorIndex++] = 0xc000c0;
    m_nColor[colorIndex++] = 0x00ffff;
    m_nColor[colorIndex++] = 0xaa5500;
    m_nColor[colorIndex]   = 0xffffff;
    m_nColorNumber = colorIndex;

    m_IconImage.load(":/images/marker.png");
    m_IconImageSelected.load(":/images/marker_sel.png");
    m_CodeSetIconImage.load(":/images/support_pressed.png");
    m_MissionHomeIcon.load(":/images/mission_start.png");

    m_strIconPatch = "Temp_Line/marker48X48_3D.png";

    m_ScanTimer = new QTimer();
    m_ScanTimer->setInterval(1000 / DRAW_SCAN_HZ);
    QObject::connect(m_ScanTimer, &QTimer::timeout, &LY_TaskManage::on_ScanTimer);

    CreateMenu();
}

void LY_TaskManage::Startup()
{
    IFunctionInterface &ifi = LY_TaskManageInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            ITaskManagerActivator::Get().start(pIRegistry);
        }
    }
}

void LY_TaskManage::Shutdown()
{
    IFunctionInterface &ifi = LY_TaskManageInterface::Get().GetFunctionInterface();
    QVariant vt = ifi.Invoke("Ly.IRegistryImpl", 0);
    if (vt.canConvert<qnzkna::framework::IRegistry *>())
    {
        qnzkna::framework::IRegistry *pIRegistry = vt.value<qnzkna::framework::IRegistry *>();
        if (pIRegistry != nullptr)
        {
            ITaskManagerActivator::Get().stop(pIRegistry);
        }
    }
}

void LY_TaskManage::Reset()
{

}

bool LY_TaskManage::DrawGraph(QPainter &painter, IProj *pProj)
{
    return true;
}

bool LY_TaskManage::DrawDynamic(QPainter &painter, IProj *pProj)
{

	if (m_pDlgPlay != nullptr)
	{
		m_pDlgPlay->drawPoint(painter, pProj);
	}

    if (m_pDlgMissionLineList != nullptr)
    {
        DrawMissionLine(painter, pProj);
    }

    if (!m_pDlgSensorManage->isHidden())
    {
        DrawSensorView(painter, pProj);
    }
    else
    {
        if (m_ScanTimer->isActive() == true)
        {
            m_ScanTimer->stop();
            m_nTimer = 0;
        }
    }

    if (!m_pDlgFireManage->isHidden())
    {
        DrawFireView(painter, pProj);
    }

    if (!m_pDlgCommunicateManage->isHidden())
    {
        DrawCommunicateView(painter, pProj);
        DrawCommunicateCoverView(painter, pProj);
    }

    DrawLineOld(painter, pProj);

    if (m_pDlgMissionPlanningOverview->isVisible() && m_pDlgMissionPlanningOverview->b_isDraw)
    {

        if (m_pDlgMissionEdit != nullptr)
        {

            typedef std::vector<CNeubulaMission>            CNeubulaMissionVec;
            CNeubulaMissionVec MissionVec = ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingMissions();
            for (auto data : MissionVec)
            {
                double dLon = data.GetStartLon();
                double dLat = data.GetStartLat();
                QPoint ptPoint = pProj->Jwd2Screen(dLon, dLat);
                if (dLon != 0 && dLat != 0)
                {
                    DrawMissionStartIcon(painter, ptPoint, QString::fromStdString(data.GetID()));
                }
            }

            if (!m_pDlgMissionEdit->isHidden())
            {
                if (m_pDlgMissionEdit->GetPickPointStates() == PICK_POINT_EDIT)
                {
                    double dLon = m_pDlgMissionEdit->GetStartLon();
                    double dLat = m_pDlgMissionEdit->GetStartLat();
                    QPoint ptPoint1 = pProj->Jwd2Screen(dLon, dLat);
                    QColor greeb1 = 0x0000ff;
                    QPen pen1(greeb1);
                    pen1.setWidth(3);
                    painter.setPen(pen1);
                    painter.setBrush(greeb1);
                    painter.drawEllipse(ptPoint1, LINE_LENGTH, LINE_LENGTH);
                }
            }
        }

        m_pDlgMissionPlanningOverview->drawMissionArea(painter, pProj);

        m_pDlgMissionPlanningOverview->drawMarkInfoRange(painter, pProj);

        m_pDlgMissionPlanningOverview->drawPlanLine(painter, pProj);

    }
    if (m_pDlgCommunicateManage->isDraw)
    {
        if (bIntervisibility)
        {
            DrawMeasureIntervisibility(painter, pProj);
        }
        CDrawShape ds;
        CDrawShape::Style style;
        for (auto data : m_pShpVec)
        {
            if (data[0] != NULL)
            {
                style.clrPen = QColor(Qt::green);
                ds.Draw(data[0], painter, pProj, style);
            }
            if (data[1] != NULL)
            {
                style.clrPen = QColor(Qt::red);
                ds.Draw(data[1], painter, pProj, style);
            }
        }

    }

    return true;
}

bool LY_TaskManage::OnMouseButtonDown(QMouseEvent *e)
{
    if (LY_TaskManage::Get().GetDlgTaskLineEdit())
    {
        if (e->button() == Qt::LeftButton && LY_TaskManage::Get().GetDlgTaskLineEdit()->getIsAddPointStatus())
        {
            QPoint pt = e->pos();
            double dLon, dLat;
            m_pProj->Screen2Jwd(pt, dLon, dLat);
            LY_TaskManage::Get().GetDlgTaskLineEdit()->AddPointInLine(dLon, dLat);
        }
        if (e->button() == Qt::LeftButton && LY_TaskManage::Get().GetDlgTaskLineEdit()->getIsDragStatus())
        {
            MouseButtonDownMission(e);
        }
    }
    if (m_pDlgAddMaintainMark->bScreenPoint)
    {
        m_pDlgAddMaintainMark->bScreenPoint = false;
    }
    MouseButtonDownOld(e);

    return false;
}

bool LY_TaskManage::OnMouseButtonRelease(QMouseEvent *e)
{
    if (m_bMouseMove == true)
    {
        m_pDlgEditLine->UpdateTableConnect(true);
        m_bMouseMove = false;
        m_pDlgEditLine->ClearSelectedStates();
    }

    if (m_bMissionMouseMove == true)
    {
        m_bMissionMouseMove = false;
        m_nMissionSelectLineID = 0;
        m_nMissionSelectPointID = 0;
    }
    return false;
}

bool LY_TaskManage::OnMouseMove(QMouseEvent *e)
{
    MouseMoveOld(e);
    MouseMoveMission(e);

    MouseMoveMarkManage(e);
    return false;
}

bool LY_TaskManage::OnMouseDoubleClick(QMouseEvent *e)
{
    if (e->button() == Qt::RightButton)
    {
        return false;
    }
    if (m_pDlgTaskLineEdit)
    {
        if (!m_pDlgTaskLineEdit->isHidden() && !m_pDlgAutoPoint->isHidden())
        {
            QPoint pt = e->pos();
            double dLon, dLat;
            m_pProj->Screen2Jwd(pt, dLon, dLat);

            if (m_pDlgAutoPoint != nullptr && m_pDlgAutoPoint->UpdatePositionAndMarkIDCal())
            {
                WidgetLineEdit *curWidgetLineEdit = m_pDlgTaskLineEdit->getCurWidget();
                std::string strMarkID = curWidgetLineEdit->GetMarkIDString();
                m_pDlgAutoPoint->SetPosition(dLon, dLat);
                m_pDlgAutoPoint->SetMarkID(strMarkID);
                m_pDlgAutoPoint->SetEditLinePoint(true);
                if (m_pDlgAutoPoint->JudeInMarkInfoSpace())

                {
                    m_pDlgAutoPoint->CalFromPositionAndMarkID();

                    return false;
                }

                return false;
            }
        }
    }

    if (m_pDlgEditLine)
    {
        if (m_pDlgEditLine->GetLineEditStates() == PickLinePoint)
        {
            m_pDlgEditLine->UpdateTableConnect(false);
            bool bReturn = false;
            QPoint pt = e->pos();
            double dLon, dLat;
            m_pProj->Screen2Jwd(pt, dLon, dLat);

            if (m_pDlgAutoPoint != nullptr && m_pDlgAutoPoint->UpdatePositionAndMarkIDCal())
            {
                std::string strMarkID = m_pDlgEditLine->GetMarkIDString();
                m_pDlgAutoPoint->SetPosition(dLon, dLat);
                m_pDlgAutoPoint->SetMarkID(strMarkID);
                m_pDlgAutoPoint->SetEditLinePoint(true);
                if (m_pDlgAutoPoint->JudeInMarkInfoSpace())

                {
                    m_pDlgAutoPoint->CalFromPositionAndMarkID();

                    return false;
                }

                return false;
            }
            m_pDlgEditLine->AddLinePointMouseDoubleClick(dLon, dLat, 0);
            m_pDlgEditLine->UpdateTableConnect(true);
            LY_TaskManageInterface::Get().Refresh(1);
        }
        else if (m_pDlgEditLine->GetLineEditStates() == PickSinglePoint)
        {
            m_pDlgEditLine->UpdateTableConnect(false);
            bool bReturn = false;
            QPoint pt = e->pos();
            double dLon, dLat;
            m_pProj->Screen2Jwd(pt, dLon, dLat);
            if (m_pDlgAutoPoint != nullptr && m_pDlgAutoPoint->UpdatePositionAndMarkIDCal())
            {
                std::string strMarkID = m_pDlgEditLine->GetMarkIDString();
                m_pDlgAutoPoint->SetPosition(dLon, dLat);
                m_pDlgAutoPoint->SetMarkID(strMarkID);
                m_pDlgAutoPoint->SetEditLinePoint(true);
                if (m_pDlgAutoPoint->JudeInMarkInfoSpace())

                {
                    m_pDlgAutoPoint->CalFromPositionAndMarkID();

                    return false;
                }

                return false;
            }
            m_pDlgEditLine->InsertLinePoint(dLon, dLat, 0);
            m_pDlgEditLine->UpdateTableConnect(true);
        }

    }

    if (m_pDlgMissionEdit != nullptr)
    {
        if (m_pDlgMissionEdit->GetPickPointStates() == PICK_POINT_EDIT)
        {
            QPoint pt = e->pos();
            double dLon(0), dLat(0), dHeight(0);
            m_pProj->Screen2Jwd(pt, dLon, dLat);
            dHeight = s_pCGetHeight->GetHeight(dLon, dLat);
            m_pDlgMissionEdit->SetPostion(dLon, dLat, dHeight);
        }
    }

    if (m_pDlgMissionLinePointEdit)
    {
        if (m_pDlgMissionLinePointEdit->GetLineEditStates() == PickLinePoint)
        {
            bool bReturn = false;
            QPoint pt = e->pos();
            double dLon, dLat;
            m_pProj->Screen2Jwd(pt, dLon, dLat);
            m_pDlgMissionLinePointEdit->AddLinePointMouseDoubleClick(dLon, dLat, 0);

        }
    }

    if (m_pDlgAddMaintainMark)
    {
        if (m_pDlgAddMaintainMark->bScreenPoint)
        {
            m_pDlgAddMaintainMark->bScreenPoint = false;
        }
    }
    return false;
}

bool LY_TaskManage::OnCommand(int nID)
{
    switch (nID)
    {

    case 50008:
    {

        if (m_pDlgEditLine->isVisible())
        {

            m_pDlgEditLine->hide();
        }
        else
        {

            m_pDlgEditLine->raiseShow();
        }
        break;
    }
    case 50001:
    {
        if (m_pDlgActionList && m_pDlgActionList->isVisible())
        {
            m_pDlgActionList->hide();
        }
        else if (m_pDlgActionList)
        {
            m_pDlgActionList->show();
            m_pDlgActionList->raise();
        }
        break;
    }

    case 50002:
    {
        if (m_pDlgMissionEdit && m_pDlgMissionEdit->isVisible())
        {
            m_pDlgMissionEdit->hide();
        }
        else if (m_pDlgMissionEdit)
        {
            m_pDlgMissionEdit->SetDataType(1);
            m_pDlgMissionEdit->InitWindowCtrl(0);

            m_pDlgMissionEdit->show();
            m_pDlgMissionEdit->raise();
        }
        break;
    }
    case 50003:
    {
        if (!m_pDlgTaskLineEdit)
        {
            NewDlgTaskLineEdit();
        }
        if (m_pDlgTaskLineEdit && m_pDlgTaskLineEdit->isVisible())
        {
            m_pDlgTaskLineEdit->hide();
        }
        else if (m_pDlgTaskLineEdit)
        {
            m_pDlgTaskLineEdit->setWindowFlags(m_pDlgTaskLineEdit->windowFlags() | Qt::Dialog);
            m_pDlgTaskLineEdit->show();
            m_pDlgTaskLineEdit->raise();

            if (ITaskManagerActivator::Get().getPTaskManageService()->GetActivatingActionID() != m_pDlgTaskLineEdit->getActionID())
            {
                m_pDlgTaskLineEdit->update();
            }
        }
        break;
    }
    case 50005:
    {
        if (m_pDlgSensorManage && m_pDlgSensorManage->isVisible())
        {
            m_pDlgSensorManage->hide();
        }
        else if (m_pDlgSensorManage)
        {
            m_pDlgSensorManage->show();
            m_pDlgSensorManage->raise();
        }
        break;
    }
    case 50006:
    {
        char pBuf[MAX_BUFFER_LEN] = { 0 };
        int nBufLen = 0;
        qnzkna::framework::MessageReferenceProps  props;
        std::string str = "1";
        props.put("MouseButtonDown", str);
        ITaskManagerActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgFirePlanSend", pBuf, nBufLen, props));

        break;
    }
    case 50004:
    {
        if (m_pDlgCommunicateManage && m_pDlgCommunicateManage->isVisible())
        {
            m_pDlgCommunicateManage->hide();
        }
        else if (m_pDlgCommunicateManage)
        {
            m_pDlgCommunicateManage->show();
            m_pDlgCommunicateManage->raise();
        }
        break;
    }
    case 50013:
    {
        if (m_pDlgMissionPlanningOverview && m_pDlgMissionPlanningOverview->isVisible())
        {
            m_pDlgMissionPlanningOverview->hide();
        }
        else if (m_pDlgMissionPlanningOverview)
        {
            m_pDlgMissionPlanningOverview->show();
            m_pDlgMissionPlanningOverview->raise();
        }
        break;

    }

    case 50014:
    {
        if (m_pResourceCarryingPlanDialog && m_pResourceCarryingPlanDialog->isVisible())
        {
            m_pResourceCarryingPlanDialog->hide();
        }
        else if (m_pResourceCarryingPlanDialog)
        {
            m_pResourceCarryingPlanDialog->show();
            m_pResourceCarryingPlanDialog->raise();
        }

        break;
    }
    case 57001:
    {
        if (m_pDlgMarkMaintain && m_pDlgMarkMaintain->isVisible())
        {
            m_pDlgMarkMaintain->hide();
        }
        else if (m_pDlgMarkMaintain)
        {
            m_pDlgMarkMaintain->Init();
            m_pDlgMarkMaintain->show();
            m_pDlgMarkMaintain->raise();
        }
        break;
    }
    default:
        break;
    }
    return false;
}

bool LY_TaskManage::OnCreate(int wParam, QTabWidget *pTab)
{

    if (m_pDlgEditLine == NULL)
    {
        m_pDlgEditLine = new CDlgEditLine(pTab);

        m_pDlgEditLine->hide();
    }

    if (m_pDlgPlay == NULL)
    {
        m_pDlgPlay = new CDlgPlay(pTab);

        m_pDlgPlay->hide();
    }

    if (m_pDlgActionEdit == nullptr)
    {
        m_pDlgActionEdit = new CDlgActionEdit(pTab);
        m_pDlgActionEdit->hide();
    }

    if (m_pDlgMissionPlanningOverview == nullptr)
    {
        m_pDlgMissionPlanningOverview = new CDlgMissionPlanningOverview(pTab);
        m_pDlgMissionPlanningOverview->hide();
    }

    if (m_pResourceCarryingPlanDialog == nullptr)
    {
        m_pResourceCarryingPlanDialog = new IResourceCarryingPlanDialog(pTab);
        m_pResourceCarryingPlanDialog->hide();
    }
    if (m_pISAddCarryingPlanResDialog == nullptr)
    {
        m_pISAddCarryingPlanResDialog = new ISAddCarryingPlanResDialog(pTab);
        m_pISAddCarryingPlanResDialog->hide();
    }
	if (m_pDlgPlaySpeedControl == nullptr)
	{
		m_pDlgPlaySpeedControl = new CDlgPlaySpeedControl(pTab);
		m_pDlgPlaySpeedControl->hide();
	}
	if (m_pDlgForecastTendency == nullptr)
	{
		m_pDlgForecastTendency = new CDlgForecastTendency(pTab);
		m_pDlgForecastTendency->hide();
	}

    switch (wParam)
    {
    case 1000:
    {
        m_pWnd = pTab;
    }
    break;
    case 1001:
    {
        m_pTroopLabel = pTab;
    }
    break;
    case 1002:
    {
        m_pZzTimeLabel = (QLabel *)pTab;
    }
    break;
    case 0:
    {
        if (m_pDlgPlay != nullptr)
        {

        }

        if (m_pDlgActionList == nullptr)
        {
            m_pDlgActionList = new CDlgActionList(pTab);
            m_pDlgActionList->hide();
        }

        if (m_pDlgSensorManage == nullptr)
        {
            m_pDlgSensorManage = new CDlgSensorManage(pTab);
            m_pDlgSensorManage->hide();
        }

        if (m_pDlgSensorManageEdit == nullptr)
        {
            m_pDlgSensorManageEdit = new CDlgSensorManageEdit(pTab);
            m_pDlgSensorManageEdit->hide();
        }

        if (m_pDlgFireManage == nullptr)
        {
            m_pDlgFireManage = new CDlgFireManage(pTab);
            m_pDlgFireManage->hide();
        }

        if (m_pDlgFireManageEdit == nullptr)
        {
            m_pDlgFireManageEdit = new CDlgFireManageEdit(pTab);
            m_pDlgFireManageEdit->hide();
        }

        if (m_pDlgCommunicateManage == nullptr)
        {
            m_pDlgCommunicateManage = new CDlgCommunicateManage(pTab);
            m_pDlgCommunicateManage->hide();
        }

        if (m_pDlgMissionLinePointEdit == nullptr)
        {
            m_pDlgMissionLinePointEdit = new CDlgMissionLinePointEdit(pTab);
            m_pDlgMissionLinePointEdit->hide();
        }

        if (m_pDlgMarkSchemeList == nullptr)
        {
            m_pDlgMarkSchemeList = new CDlgMarkSchemeList(pTab);
            m_pDlgMarkSchemeList->hide();
        }

        if (m_pDlgMissionAllocation == nullptr)
        {
            m_pDlgMissionAllocation = new CDlgMissionAllocation(pTab);
            m_pDlgMissionAllocation->hide();
        }
        if (m_pDlgMarkMaintain == nullptr)
        {
            m_pDlgMarkMaintain = new CDlgMarkMaintain(pTab);
            m_pDlgMarkMaintain->hide();
        }

        if (m_pDlgAddMaintainMark == nullptr)
        {
            m_pDlgAddMaintainMark = new CDlgAddMaintainMark(pTab);
            m_pDlgAddMaintainMark->hide();
        }
    }
    break;
    case 1:
    {

        LyDockWidget *pDockMarkListDialog = LyDockManager::instance()->dockWidget("MissionList");
        if (pDockMarkListDialog == nullptr)
        {
            pDockMarkListDialog = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, QObject::tr("Task List"), "MissionList");
            LyDockManager::instance()->embedToolDockWidget(pDockMarkListDialog, LyDockDef::Right);
        }

        if (m_pDlgMissionList == nullptr)
        {
            m_pDlgMissionList = new CDlgMissionList;
            pDockMarkListDialog->setCentralWidget(m_pDlgMissionList);
            m_pDlgMissionList->setWindowTitle(QObject::tr("Task List"));
            LyDockWidget::connect(m_pDlgMissionList, &CDlgMissionList::windowTitleChanged, pDockMarkListDialog, &LyDockWidget::setWindowTitle);

            m_pDlgEditLine->SetDlgMissionList(m_pDlgMissionList);

            if (ITaskManagerActivator::Get().getPTaskManageService() != NULL)
            {
                ITaskManagerActivator::Get().getPTaskManageService()->Attach(m_pDlgMissionList);
            }
        }

        if (m_pDlgMissionEdit == NULL)
        {
            m_pDlgMissionEdit = new CDlgMissionEdit(pTab);
            m_pDlgMissionEdit->setWindowTitle(QObject::tr("Edit Task"));
            m_pDlgMissionEdit->hide();
        }

        if (m_pDlgMissionPlatformEdit == NULL)
        {
            m_pDlgMissionPlatformEdit = new CDlgMissionPlatformEdit;
            m_pDlgMissionPlatformEdit->setWindowTitle(QObject::tr("Edit Weapon Platform"));
            m_pDlgMissionPlatformEdit->hide();
        }

        if (m_pDlgMissionPlatformLoadEdit == nullptr)
        {
            m_pDlgMissionPlatformLoadEdit = new CDlgMissionPlatformLoadEdit;
            m_pDlgMissionPlatformLoadEdit->setWindowTitle(QObject::tr("Edit Ammo Mounts"));
            m_pDlgMissionPlatformLoadEdit->hide();
        }

        if (m_pDlgAutoPoint == nullptr)
        {
            m_pDlgAutoPoint = new CDlgAutoPoint;
            m_pDlgAutoPoint->setWindowTitle(QObject::tr("Combat Waypoing Setting"));
            m_pDlgAutoPoint->hide();
            QObject::connect(m_pDlgAutoPoint, &CDlgAutoPoint::SetPoint, [](double dLon, double dLat, double dHeigeth)
            {

                LY_TaskManage::Get().SetPoint(dLon, dLat, dHeigeth);
            });
        }

        if (m_pDlgCodeSet_Load_Edit == nullptr)
        {
            m_pDlgCodeSet_Load_Edit = new CDlgCodeSet_Load_Edit(pTab);
            m_pDlgCodeSet_Load_Edit->setWindowTitle(QObject::tr("Planning Instruction Set Editing"));
            m_pDlgCodeSet_Load_Edit->hide();
        }

        LyDockWidget *pDockLineListDialog = LyDockManager::instance()->dockWidget("MissionLineList");
        if (pDockLineListDialog == nullptr)
        {
            pDockLineListDialog = LyDockManager::instance()->createDockWidget(LyDockDef::Tool, QObject::tr("Mission Route List"), "MissionLineList");
            LyDockManager::instance()->embedToolDockWidget(pDockLineListDialog, LyDockDef::Right);
        }

        if (m_pDlgMissionLineList == nullptr)
        {
            m_pDlgMissionLineList = new CDlgMissionLineList;
            pDockLineListDialog->setCentralWidget(m_pDlgMissionLineList);
            m_pDlgMissionLineList->setWindowTitle(QObject::tr("Mission Route List"));
            LyDockWidget::connect(m_pDlgMissionLineList, &CDlgMissionPlatformLoadList::windowTitleChanged, pDockLineListDialog, &LyDockWidget::setWindowTitle);

            if (ITaskManagerActivator::Get().getPTaskManageService() != NULL)
            {
                ITaskManagerActivator::Get().getPTaskManageService()->Attach(m_pDlgMissionLineList);
            }
        }
        if (m_pDlgMissionPlatfromInfo == nullptr)
        {
            m_pDlgMissionPlatfromInfo = new CDlgMissionPlatformInfo;
            m_pDlgMissionPlatfromInfo->setWindowTitle(QObject::tr("Equipment Selection"));
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
	case 51003:
	{
		if (m_pDlgPlay && m_pDlgPlay->isVisible())
		{
			m_pDlgPlay->hide();

			m_pDlgPlay->setWindowFlags(m_pDlgPlay->windowFlags() & ~Qt::WindowStaysOnTopHint);
		}
		else if (m_pDlgPlay)
		{
			m_pDlgPlay->show();
			m_pDlgPlay->raise();

		}
		break;
	}
    }
    return false;
}

bool LY_TaskManage::OnTimer(int wParam)
{
    switch (wParam)
    {
    case 1:
        LY_TaskManageInterface::Get().Refresh(1);
        break;
    case 2:
        break;
    case 3:
        break;
    }
    return true;
}

void LY_TaskManage::setTroopName(const QString &name)
{
    if (m_pTroopLabel == nullptr)
    {
        return ;
    }

    QLabel *pTroopLabel = qobject_cast<QLabel *>(m_pTroopLabel);
    if (pTroopLabel == nullptr)
    {
        return ;
    }

}

QWidget *LY_TaskManage::getPWnd() const
{
    return m_pWnd;
}

void LY_TaskManage::DrawPointIcon(QPainter &painter, QPoint pt, bool Selected)
{
    const QPixmap &m_pic = m_IconImage;
    QPixmap &m_picSel = m_IconImageSelected;
    if (Selected == false)
    {
        QPointF pmt;
        pmt.setX(pt.x() - 9);
        pmt.setY(pt.y() - 21);
        painter.drawPixmap(pmt, m_pic);
    }
    else
    {
        QPointF pmt;
        int wi = m_picSel.width();
        int he = m_picSel.height();
        m_picSel = m_picSel.scaled(21, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        pmt.setX(pt.x() - 10);
        pmt.setY(pt.y() - 31);
        painter.drawPixmap(pmt, m_picSel);
    }
}

void LY_TaskManage::DrawCodeSetPointIcon(QPainter &painter, QPoint pt, bool Selected)
{
    const QPixmap &m_pic = m_CodeSetIconImage;
    QPointF pmt;
    pmt.setX(pt.x() - 13);
    pmt.setY(pt.y() - 21);
    painter.drawPixmap(pmt, m_pic);
}

void LY_TaskManage::DrawMissionStartIcon(QPainter &painter, QPoint pt, QString strName)
{
    QColor greeb1 = 0xff0000;
    QPen pen1(greeb1);
    pen1.setWidth(1);
    painter.setPen(pen1);
    painter.setBrush(greeb1);
    painter.drawEllipse(pt, LINE_LENGTH, LINE_LENGTH);

    const QPixmap &m_pic = m_MissionHomeIcon;
    QPointF pmt;
    pmt.setX(pt.x() - 14);
    pmt.setY(pt.y() - 34);
    painter.drawPixmap(pmt, m_pic);

    QPen penText(QColor(255, 255, 255));
    painter.setPen(penText);
    QRect textRect(pt.rx() - 100, pt.ry(), 200, 20);
    painter.drawText(textRect, Qt::AlignCenter, strName);
}

bool LY_TaskManage::JudegePointCodeSet(const std::string strUAVID, const int nPointID)
{
	if (m_pDlgCodeSet_Load_Tree == nullptr) {
		return false;
	}

	CodeSetLoadVec wndCodeSetLoadVec = m_pDlgCodeSet_Load_Tree->GetSortOutData();

    for (auto data : wndCodeSetLoadVec)
    {
        if (0 == strcmp(data.strPlatformID.c_str(), strUAVID.c_str()))
        {
            for (auto point : data.PointParameterVec)
            {
                if (nPointID == point.nPointID)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

bool LY_TaskManage::JudgePoint(QPoint pt, int &nLineID, int &nPoint)
{
    WndRouteLineInfoList  wndRouteLineInfoList = m_pDlgEditLine->GetWndRoutlineInfoList();

    if (wndRouteLineInfoList.size() == 0)
    {
        return false;
    }

    if (m_pDlgEditLine->GetLineEditStates() != PickLinePoint)
    {
        return false;
    }

    for (auto itr : wndRouteLineInfoList)
    {
        if (itr->bLineShowStates == true)
        {

            for (auto itrLine : itr->vecNebulaRoutePoint)
            {
                int nNo = itrLine.nPointID;
                double dLongitude = itrLine.stNebulaRoutePoint.GetLongitude();
                double dLatitude = itrLine.stNebulaRoutePoint.GetLatitude();
                int dHeight = itrLine.stNebulaRoutePoint.GetHeight();

                QPoint ptPoint = m_pProj->Jwd2Screen(dLongitude, dLatitude);

                if (MatchLinePoint(ptPoint, pt))
                {
                    nLineID = itr->nLineID;
                    nPoint = itrLine.nPointID;
                    return true;
                }
            }
        }
    }
    return false;
}

bool LY_TaskManage::JudgePointMisson(QPoint pt, int &nLineID, int &nPoint)
{
    CNeubulaMissionLineVec  LineInfoList = m_pDlgMissionLineList->GetShowData();

    if (LineInfoList.size() == 0)
    {
        return false;
    }

    for (auto itr : LineInfoList)
    {
        if (itr.cShowFlag == true)
        {

            for (auto itrLine : itr.LinePointVec)
            {
                double dLongitude = itrLine.dLon;
                double dLatitude = itrLine.dLat;
                int dHeight = itrLine.dAlt;

                QPoint ptPoint = m_pProj->Jwd2Screen(dLongitude, dLatitude);

                if (MatchLinePoint(ptPoint, pt))
                {
                    nLineID = itr.nID;
                    nPoint = itrLine.nNo;
                    return true;
                }
            }
        }
    }
    return false;
}

bool LY_TaskManage::MatchLinePoint(QPoint mp, QPoint lp)
{
    if (abs(mp.rx() - lp.rx()) <= LINE_LENGTH_JUDGE && abs(mp.ry() - lp.ry()) <= LINE_LENGTH_JUDGE)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool LY_TaskManage::MouseButtonDownOld(QMouseEvent *e)
{
    WndRouteLineInfoList  wndRouteLineInfoList = m_pDlgEditLine->GetWndRoutlineInfoList();

    if (wndRouteLineInfoList.size() == 0)
    {
        return false;
    }

    QPoint pt1 = e->globalPos();
    QPoint pt2 = e->pos();

    if (m_pDlgEditLine->GetLineEditStates() != PickLinePoint)
    {
        return false;
    }

    if (JudgePoint(pt2, m_nLineID, m_nPointID))
    {
        m_pDlgEditLine->UpdateTableConnect(false);
        m_bMouseMove = true;
        m_pDlgEditLine->SetSelectedStates(m_nLineID, m_nPointID);
        LY_TaskManageInterface::Get().Refresh(1);
    }
    return false;
}

bool LY_TaskManage::MouseButtonDownMission(QMouseEvent *e)
{
    CNeubulaMissionLineVec  LineInfoList = m_pDlgMissionLineList->GetShowData();

    if (LineInfoList.size() == 0)
    {
        return false;
    }

    QPoint pt1 = e->globalPos();
    QPoint pt2 = e->pos();

    if (JudgePointMisson(pt2, m_nMissionSelectLineID, m_nMissionSelectPointID))
    {
        if (m_pDlgTaskLineEdit->getTabWidgetLine(m_nMissionSelectLineID) == NULL)
        {
            m_pDlgTaskLineEdit->showWarmSelectedPoint();
            return false;
        }
        m_bMissionMouseMove = true;
        LY_TaskManageInterface::Get().Refresh(1);
    }
    return false;
}

bool LY_TaskManage::MouseMoveOld(QMouseEvent *e)
{
    if (m_bMouseMove == false)
    {
        return false;
    }

    QPoint pt = e->pos();
    double dLongitude, Latitude;
    m_pProj->Screen2Jwd(pt, dLongitude, Latitude);

    m_pDlgEditLine->UpdateLinePointMouseMove(m_nLineID, m_nPointID, dLongitude, Latitude);
    LY_TaskManageInterface::Get().Refresh(1);

    return false;
}

bool LY_TaskManage::MouseMoveMission(QMouseEvent *e)
{
    if (m_bMissionMouseMove == false)
    {
        return false;
    }

    QPoint pt = e->pos();
    double dLongitude, Latitude;
    m_pProj->Screen2Jwd(pt, dLongitude, Latitude);

    m_pDlgMissionLineList->UpdateLinePointMouseMove(m_nMissionSelectLineID, m_nMissionSelectPointID, dLongitude, Latitude);
    m_pDlgTaskLineEdit->updateLinePointLL(m_nMissionSelectLineID, m_nMissionSelectPointID, dLongitude, Latitude);

    LY_TaskManageInterface::Get().Refresh(1);
    return false;
}

bool LY_TaskManage::MouseMoveMarkManage(QMouseEvent *e)
{
    if (!m_pDlgAddMaintainMark->bScreenPoint)
    {
        return false;
    }
    QPoint pt = e->pos();
    double dLongitude, Latitude;
    m_pProj->Screen2Jwd(pt, dLongitude, Latitude);
    m_pDlgAddMaintainMark->SetMarkLocation(dLongitude, Latitude);
    return false;
}

void LY_TaskManage::SetScreenCenter(double &dLon, double &dLat)
{
    QString str = "CenterAt(" + QString::number(dLon, 'f', 8) + "," + QString::number(dLat, 'f', 8) + ")";
    LY_TaskManageInterface::Get().Script(str.toLocal8Bit());
}

void LY_TaskManage::Draw3DLinesWeaponID(const string &strWeaponID)
{
    qDebug() << "Draw3DLinesWeaponID:" << strWeaponID.c_str();
    const WndRouteLineInfoList wndRouteLineInfoList = m_pDlgEditLine->GetWndRoutlineInfoList();
    if (wndRouteLineInfoList.size() <= 0)
    {
        return;
    }

    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth == nullptr)
    {
        return;
    }

    Clear3DlinesWeaponID(strWeaponID);

    const std::map<std::string, UAV3D>::iterator iterUAV3D = m_UAV3DMap.find(strWeaponID);
    if (iterUAV3D != m_UAV3DMap.end())
    {
        return;
    }

    const std::vector<WndRouteLineInfo *>::const_iterator lineListFind = std::find_if(wndRouteLineInfoList.begin(), wndRouteLineInfoList.end(),
                                                                         [strWeaponID](const WndRouteLineInfo * itemWndRouteLineInfo)
    {
        if (itemWndRouteLineInfo->strWeaponID == strWeaponID)
        {
            return true;
        }
        return false;
    }
                                                                                     );
    if (lineListFind == wndRouteLineInfoList.end())
    {
        return ;
    }

    int colorIndex = wndRouteLineInfoList.size();
    if (lineListFind != wndRouteLineInfoList.end())
    {
        if ((*lineListFind)->bLineShowStates == false)
        {
            return ;
        }
        if (colorIndex >= sizeof(m_nColor) / sizeof(m_nColor[0]))
        {
            colorIndex = 0;
        }

        QColor colorBombTrack((*lineListFind)->nColor);
        colorBombTrack.setAlpha(255);
        QColor colorTemp;
        colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());

        IUserMesh1      *pUM = nullptr;

        UAV3D stUAV3D;
        stUAV3D.strID = (*lineListFind)->strWeaponID;
        stUAV3D.colorLine = colorTemp.rgba();

        WndRoutePoint stWndRoutePoint;

        short elevation = 0;
        std::vector<double> pdLon;
        std::vector<double> pdLat;
        std::vector<double> pdHeight;
        unsigned int PointIndex = 0;
        auto itrLine = (*lineListFind)->vecNebulaRoutePoint.begin();
        if (itrLine != (*lineListFind)->vecNebulaRoutePoint.end() && s_pCGetHeight != nullptr && m_FlightTaskManageEditLineElevation)
        {
            short int elevationTemp = s_pCGetHeight->GetHeight(itrLine->stNebulaRoutePoint.GetLongitude(), itrLine->stNebulaRoutePoint.GetLatitude());
            if (elevationTemp != -32768)
            {
                elevation = elevationTemp;
            }
        }
        for (; itrLine != (*lineListFind)->vecNebulaRoutePoint.end(); itrLine++)
        {
            pdLon.push_back(0);
            pdLat.push_back(0);
            pdHeight.push_back(0);

            const double dLongitude        = itrLine->stNebulaRoutePoint.GetLongitude();
            const double dLatitude         = itrLine->stNebulaRoutePoint.GetLatitude();
            const double dHeightElevation  = itrLine->stNebulaRoutePoint.GetHeight() + elevation;

            pdLon[PointIndex]              = dLongitude;
            pdLat[PointIndex]              = dLatitude;
            pdHeight[PointIndex]           = dHeightElevation;

            short int elevationTemp = 0;
            if (s_pCGetHeight != nullptr && m_FlightTaskManageEditLineElevation)
            {
                short int heightTemp = s_pCGetHeight->GetHeight(dLongitude, dLatitude);
                if (heightTemp == -32768)
                {
                    heightTemp = 0;
                }
                elevationTemp = heightTemp;
                if (elevationTemp > dHeightElevation)
                {
                    elevationTemp = dHeightElevation;
                }
            }

            double pdLon1[2]    = { dLongitude, dLongitude };
            double pdLat1[2]    = { dLatitude, dLatitude  };
            double pdHeight1[2] = { dHeightElevation * 1.0, elevationTemp * 1.0 };
            IUserMesh1	*pUM1 = nullptr;
            CCreateShape cs1;
            SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
            if (pShp1 != nullptr)
            {
                pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
                if (pUM1 != nullptr)
                {
                    pUM1->SetColor(colorTemp.rgba());
                    pUM1->SetState(IUserMesh::POINT, IUserMesh::USERMESH_LINE);
                    pUM1->SetState(IUserMesh::LINEWIDTH, 1);
                    stUAV3D.pMesh1Vec.push_back(pUM1);
                }
            }

            const QByteArray &ba = QUuid::createUuid().toRfc4122();
            QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
            QString strPathOld = QObject::tr(m_strDataDir) + m_strIconPatch;
            QFile currenFile(strPathOld);
            QString strPath = QObject::tr(m_strDataDir);
            strPath += QLatin1String("temp/");
            strPath += uid.toString(QUuid::WithoutBraces);
            strPath += QLatin1String(".png");
            currenFile.copy(strPath);
            std::string strImg = tr3(strPath).data();
            QString strLable;
            if (PointIndex == 0)
            {
                strLable = QString::fromStdString((*lineListFind)->strID);
            }
            else
            {
                strLable = QString::number(PointIndex + 1);
            }

            ILabel *pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(), dLongitude,
                                                                dLatitude, dHeightElevation, 24, 24, 0);
            if (pLabel != nullptr)
            {
                pLabel->SetColor(0xffffffff, 0xffffffff, 0xffffffff);
                pLabel->SetFontSize(15);
                pLabel->SetTagState(false);
                pLabel->SetText(strLable.toLocal8Bit());
                stUAV3D.pLabelVec.push_back(pLabel);
            }
            PointIndex++;
        }

        CCreateShape cs;

        SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), PointIndex, 100, false, nullptr, pdHeight.data());
        if (pShp != nullptr)
        {
            pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
            if (pUM != nullptr)
            {
                pUM->SetColor(colorTemp.rgba());
                pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
                pUM->SetState(IUserMesh::LINEWIDTH, 2);
                stUAV3D.pMesh1Vec.push_back(pUM);
            }
        }
        m_UAV3DMap.insert(std::make_pair(stUAV3D.strID, stUAV3D));

        pdLon.clear();
        pdLat.clear();
        pdHeight.clear();
    }
}

void LY_TaskManage::Clear3DlinesWeaponID(const string &strWeaponID)
{
    IEarth *pEarth = IEarth::GetEarth();

    const std::map<std::string, UAV3D>::iterator iterUAV3D = m_UAV3DMap.find(strWeaponID);
    if (iterUAV3D != m_UAV3DMap.end())
    {
        for (auto &pMes1 : iterUAV3D->second.pMesh1Vec)
        {
            pEarth->GetGraphics()->RemoveObject(pMes1);
            pMes1 = nullptr;
        }
        iterUAV3D->second.pMesh1Vec.clear();

        for (auto &pLabel : iterUAV3D->second.pLabelVec)
        {
            pEarth->GetGraphics()->RemoveObject(pLabel);
            pLabel = nullptr;
        }
        iterUAV3D->second.pLabelVec.clear();
        m_UAV3DMap.erase(iterUAV3D);
    }
}

void LY_TaskManage::Draw3DLines()
{
    WndRouteLineInfoList  wndRouteLineInfoList = m_pDlgEditLine->GetWndRoutlineInfoList();

    if (wndRouteLineInfoList.size() == 0)
    {
        return;
    }

    IEarth *pEarth = IEarth::GetEarth();
    if (pEarth == nullptr)
    {
        return;
    }

    Clear3Dlines();
    int colorIndex = 0;
    for (auto LineList : wndRouteLineInfoList)
    {
        if (LineList->bLineShowStates == false)
        {
            continue;
        }
        if (colorIndex >= sizeof(m_nColor) / sizeof(m_nColor[0]))
        {
            colorIndex = 0;
        }
        QColor colorBombTrack((LineList->nColor));
        colorBombTrack.setAlpha(255);
        QColor colorTemp;
        colorTemp.setRgb(colorBombTrack.blue(), colorBombTrack.green(), colorBombTrack.red(), colorBombTrack.alpha());

        IUserMesh1	*pUM = nullptr;

        UAV3D stUAV3D;
        stUAV3D.strID = LineList->strWeaponID;
        stUAV3D.colorLine = colorTemp.rgba();

        WndRoutePoint stWndRoutePoint;

        short elevation = 0;
        std::vector<double> pdLon;
        std::vector<double> pdLat;
        std::vector<double> pdHeight;
        unsigned int PointIndex = 0;
        auto itrLine = LineList->vecNebulaRoutePoint.begin();
        if (itrLine != (LineList)->vecNebulaRoutePoint.end() && s_pCGetHeight != nullptr && m_FlightTaskManageEditLineElevation)
        {
            short int elevationTemp = s_pCGetHeight->GetHeight(itrLine->stNebulaRoutePoint.GetLongitude(), itrLine->stNebulaRoutePoint.GetLatitude());
            if (elevationTemp != -32768)
            {
                elevation = elevationTemp;
            }
        }

        for (; itrLine != LineList->vecNebulaRoutePoint.end(); itrLine++)
        {
            pdLon.push_back(0);
            pdLat.push_back(0);
            pdHeight.push_back(0);

            const double dLongitude        = itrLine->stNebulaRoutePoint.GetLongitude();
            const double dLatitude         = itrLine->stNebulaRoutePoint.GetLatitude();
            const double dHeightElevation  = itrLine->stNebulaRoutePoint.GetHeight() + elevation;

            pdLon[PointIndex]    = dLongitude;
            pdLat[PointIndex]    = dLatitude;
            pdHeight[PointIndex] = dHeightElevation;

            short int elevationTemp = 0;
            if (s_pCGetHeight != nullptr && m_FlightTaskManageEditLineElevation)
            {
                short int heightTemp = s_pCGetHeight->GetHeight(dLongitude, dLatitude);
                if (heightTemp == -32768)
                {
                    heightTemp = 0;
                }
                elevationTemp = heightTemp;
                if (elevationTemp > dHeightElevation)
                {
                    elevationTemp = dHeightElevation;
                }
            }

            double pdLon1[2]    = { dLongitude, dLongitude };
            double pdLat1[2]    = { dLatitude, dLatitude };
            double pdHeight1[2] = { dHeightElevation * 1.0, elevationTemp * 1.0};
            IUserMesh1	*pUM1 = nullptr;
            CCreateShape cs1;
            SHPObject *pShp1 = cs1.CreatePolyline(pdLon1, pdLat1, 2, 100, false, nullptr, pdHeight1);
            if (pShp1 != nullptr)
            {
                pUM1 = pEarth->GetGraphics()->CreateUserMesh1(pShp1);
                if (pUM1 != nullptr)
                {
                    pUM1->SetColor(colorTemp.rgba());
                    pUM1->SetState(IUserMesh::POINT, IUserMesh::USERMESH_LINE);
                    pUM1->SetState(IUserMesh::LINEWIDTH, 1);
                    stUAV3D.pMesh1Vec.push_back(pUM1);
                }
            }

            const QByteArray &ba = QUuid::createUuid().toRfc4122();
            QUuid uid = QUuid::fromRfc4122(QByteArray::fromRawData(ba, 16));
            QString strPathOld = QObject::tr(m_strDataDir) + m_strIconPatch;
            QFile currenFile(strPathOld);
            QString strPath = QObject::tr(m_strDataDir);
            strPath += QLatin1String("temp/");
            strPath += uid.toString(QUuid::WithoutBraces);
            strPath += QLatin1String(".png");
            currenFile.copy(strPath);
            std::string strImg = tr3(strPath).data();
            QString strLable;
            if (PointIndex == 0)
            {
                strLable = QString::fromStdString(LineList->strID);
            }
            else
            {
                strLable = QString::number(PointIndex + 1);
            }

            ILabel *pLabel = pEarth->GetGraphics()->CreateLabel(strLable.toLocal8Bit(), strImg.c_str(), dLongitude,
                                                                dLatitude, dHeightElevation, 24, 24, 0);
            if (pLabel != nullptr)
            {
                pLabel->SetColor(0xffffffff, 0xffffffff, 0xffffffff);
                pLabel->SetFontSize(15);
                pLabel->SetTagState(true);
                pLabel->SetText(strLable.toLocal8Bit());
                stUAV3D.pLabelVec.push_back(pLabel);
            }

            PointIndex++;
        }

        CCreateShape cs;

        SHPObject *pShp = cs.CreatePolyline(pdLon.data(), pdLat.data(), PointIndex, 100, false, nullptr, pdHeight.data());
        if (pShp != nullptr)
        {
            pUM = pEarth->GetGraphics()->CreateUserMesh1(pShp);
            if (pUM != nullptr)
            {
                pUM->SetColor(colorTemp.rgba());
                pUM->SetState(IUserMesh::POLYGONMODE, IUserMesh::USERMESH_LINE);
                pUM->SetState(IUserMesh::LINEWIDTH, 2);
                stUAV3D.pMesh1Vec.push_back(pUM);
            }
        }

        colorIndex ++;
        m_UAV3DMap.insert(std::make_pair(stUAV3D.strID, stUAV3D));

        pdLon.clear();
        pdLat.clear();
        pdHeight.clear();
    }
}

void LY_TaskManage::Clear3Dlines()
{
    IEarth *pEarth = IEarth::GetEarth();
    for (auto stUAV3D : m_UAV3DMap)
    {
        for (auto &pMes1 : stUAV3D.second.pMesh1Vec)
        {
            pEarth->GetGraphics()->RemoveObject(pMes1);
            pMes1 = nullptr;
        }
        stUAV3D.second.pMesh1Vec.clear();

        for (auto &pLabel : stUAV3D.second.pLabelVec)
        {
            pEarth->GetGraphics()->RemoveObject(pLabel);
            pLabel = nullptr;
        }
        stUAV3D.second.pLabelVec.clear();
    }

    m_UAV3DMap.clear();
}

void LY_TaskManage::SetPoint(double dLon, double dLat, double dHeigeth)
{
    if (m_pDlgEditLine->GetLineEditStates() == PickLinePoint)
    {
        m_pDlgEditLine->AddLinePointMouseDoubleClick(dLon, dLat, dHeigeth);
        m_pDlgEditLine->UpdateTableConnect(true);
        LY_TaskManageInterface::Get().Refresh(1);
    }
    else if (m_pDlgEditLine->GetLineEditStates() == PickSinglePoint)
    {
        m_pDlgEditLine->InsertLinePoint(dLon, dLat, dHeigeth);
        m_pDlgEditLine->UpdateTableConnect(true);
    }
    else if (m_pDlgTaskLineEdit)
    {
        if (!m_pDlgTaskLineEdit->isHidden())
        {
            LY_TaskManage::Get().GetDlgTaskLineEdit()->AddPointInLine(dLon, dLat, dHeigeth);
        }
    }
}

void LY_TaskManage::CreateMenu()
{
    m_LineMenu = new QMenu();
    m_MenuCodeSetEdit = m_LineMenu->addAction(QObject::tr("Instruction Set Setup"));

    QObject::connect(m_MenuCodeSetEdit, &QAction::triggered, [this]()
    {
        on_Menu_CodeSet();
    });
}

void LY_TaskManage::on_Menu_CodeSet()
{
    CCodeSet_Load_DB stCodeSetDB;
    QString strTaskID = QObject::tr("ZC001");

    m_pDlgCodeSet_Load_Edit->SetShowData(stCodeSetDB);

    if (m_pDlgCodeSet_Load_Edit->isVisible())
    {
        m_pDlgCodeSet_Load_Edit->hide();
    }
    m_pDlgCodeSet_Load_Edit->show();
}

void LY_TaskManage::UpdateCodeSetSensor()
{
    m_pDlgCodeSet_Load_Tree->UpdateWindowData();
    m_pDlgCodeSet_Load_Tree->UpdateTreeData();

    m_pDlgSensorManage->UpdataWindowData();
    m_pDlgSensorManage->UpdateTaskCombobox();
    m_pDlgSensorManage->UpdataListData();
    m_pDlgSensorManage->UpdateTreeData();
}

void LY_TaskManage::UpdateCodeSetFire()
{
    m_pDlgCodeSet_Load_Tree->UpdateWindowData();
    m_pDlgCodeSet_Load_Tree->UpdateTreeData();

    m_pDlgFireManage->UpdataWindowData();
    m_pDlgFireManage->UpdateTaskCombobox();
    m_pDlgFireManage->UpdateListData();
    m_pDlgFireManage->UpdateTreeData();
}

void LY_TaskManage::ArrangeSensorCodeSet(SensorLoadStatsMap &tmpLoadMap, CCodeSet_Load_DB_Vec &tmpCodeSetVec)
{
    for (auto CodeSet : tmpCodeSetVec)
    {
        auto loaditr = tmpLoadMap.find(CodeSet.GetLoadID());
        if (loaditr == tmpLoadMap.end())
        {
            SensorLoadStats stload;
            tmpLoadMap.insert(std::make_pair(CodeSet.GetLoadID(), stload));
        }

        const auto &load = tmpLoadMap.find(CodeSet.GetLoadID());
        if (CodeSet.GetCodeID() == qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_PayLoadStartWorking)
        {
            if (load->second.PowerVec.size() == 0)
            {
                SensorLoadSSection stSection;
                stSection.nStartNo = CodeSet.GetPointID();
                load->second.PowerVec.push_back(stSection);
            }
            else
            {
                auto PowerPoint = load->second.PowerVec.begin();
                for (; PowerPoint != load->second.PowerVec.end(); PowerPoint++)
                {

                    if (PowerPoint->nStopNo == 0)
                    {
                        if (PowerPoint->nStartNo > CodeSet.GetPointID())
                        {
                            PowerPoint->nStartNo = CodeSet.GetPointID();
                        }
                        break;
                    }
                }
                if (PowerPoint == load->second.PowerVec.end())
                {
                    SensorLoadSSection stSection;
                    stSection.nStartNo = CodeSet.GetPointID();
                    load->second.PowerVec.push_back(stSection);
                }
            }
        }
        else if (CodeSet.GetCodeID() == qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_PayLoadStopWorking)
        {
            if (load->second.PowerVec.size() == 0)
            {
            }
            else
            {
                for (auto &PowerPoint : load->second.PowerVec)
                {

                    if (PowerPoint.nStartNo != 0)
                    {
                        if (PowerPoint.nStartNo <= CodeSet.GetPointID())
                        {
                            if (PowerPoint.nStopNo == 0)
                            {
                                PowerPoint.nStopNo = CodeSet.GetPointID();
                                break;
                            }
                        }

                    }
                }
            }
        }
        else if (CodeSet.GetCodeID() == qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalScanParameterSet)
        {
            int nPointNo = CodeSet.GetPointID();
            qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_CommonCommand st1 = *((qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_CommonCommand *)(CodeSet.GetParameter().c_str()));
            auto itr = load->second.ScanParaMap.find(nPointNo);
            if (itr == load->second.ScanParaMap.end())
            {
                SensorLoadSSection stSection;
                stSection.nStartNo = st1.para.ZC_CommonCommand_GimbalScanParameterSet.scannAngle;
                stSection.nStopNo = st1.para.ZC_CommonCommand_GimbalScanParameterSet.scannSpeed;
                load->second.ScanParaMap.insert(make_pair(nPointNo, stSection));
            }
            else
            {
                itr->second.nStartNo = st1.para.ZC_CommonCommand_GimbalScanParameterSet.scannAngle;
                itr->second.nStopNo = st1.para.ZC_CommonCommand_GimbalScanParameterSet.scannAngle;
            }
        }
        else if (CodeSet.GetCodeID() == qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalRunScan)
        {
            if (load->second.ScanVec.size() == 0)
            {
                SensorLoadSSection stSection;
                stSection.nStartNo = CodeSet.GetPointID();
                load->second.ScanVec.push_back(stSection);
            }
            else
            {
                auto PowerPoint = load->second.ScanVec.begin();
                for (; PowerPoint != load->second.ScanVec.end(); PowerPoint++)
                {

                    if (PowerPoint->nStopNo == 0)
                    {
                        if (PowerPoint->nStartNo > CodeSet.GetPointID())
                        {
                            PowerPoint->nStartNo = CodeSet.GetPointID();
                        }
                        break;
                    }
                }
                if (PowerPoint == load->second.ScanVec.end())
                {
                    SensorLoadSSection stSection;
                    stSection.nStartNo = CodeSet.GetPointID();
                    load->second.ScanVec.push_back(stSection);
                }
            }
        }
        else if (CodeSet.GetCodeID() == qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_ZC_CommonCommand_GimbalCenter)
        {
            if (load->second.ScanVec.size() == 0)
            {
            }
            else
            {
                for (auto &PowerPoint : load->second.ScanVec)
                {
                    if (PowerPoint.nStartNo != 0)
                    {
                        if (PowerPoint.nStartNo <= CodeSet.GetPointID())
                        {
                            PowerPoint.nStopNo = CodeSet.GetPointID();
                        }
                        break;
                    }
                }
            }
        }
    }
}

void LY_TaskManage::DrawSensorView(QPainter &painter, IProj *pProj)
{
    QMutexLocker locker(&m_MutexTimer);
    int nLevel = m_pDlgSensorManage->GetSelectedTreeLevel();
    if (nLevel < 2)
    {
        return;
    }

    static int snLength2 = 0;
    static int snC = 20;
    static int snD = 20;
    if (snLength2 == 0)
    {
        QString strIni = LY_TaskManage::Get().GetConfDir();
        strIni += "ly.ini";
        QFile file(strIni);
        if (file.exists())
        {
            QSettings ini(strIni, QSettings::IniFormat);
            ini.beginGroup("TrackMgr");
            snLength2 = ini.value("ViewLength2", "1000").toInt();
            snC = ini.value("ViewC", "20").toInt();
            snD = ini.value("ViewD", "20").toInt();
            ini.endGroup();
        }
    }

    QString strTaskID, strPlatformID, strLoadID;
    m_pDlgSensorManage->GetTaskInfo(strTaskID, strPlatformID, strLoadID);

    QColor color = 0xffff00;
    QBrush brush(color, Qt::SolidPattern);
    painter.setPen(color);
    painter.setBrush(brush);

    SensorLoadStatsMap	LoadMap;

    CCodeSet_Load_DB_Vec CodeSetVec = ITaskManagerActivator::Get().getPTaskManageService()->GetCodeSet_Load(strTaskID.toStdString(), strPlatformID.toStdString());
    ArrangeSensorCodeSet(LoadMap, CodeSetVec);

    int nLineID = ITaskManagerActivator::Get().getPTaskManageService()->GetMissionLineID(strTaskID.toStdString(), strPlatformID.toStdString());
    CNeubulaMissionLine stline = ITaskManagerActivator::Get().getPTaskManageService()->GetMissionLine(nLineID);
    int nPointNo = 1;
    double dbearing = 0;
    auto linepoint = stline.LinePointVec.begin();
    for (; linepoint != stline.LinePointVec.end(); linepoint++)
    {
        double dLon = linepoint->dLon;
        double dLat = linepoint->dLat;
        QPoint ptPoint = pProj->Jwd2Screen(dLon, dLat);

        if (linepoint + 1 != stline.LinePointVec.end())
        {
            QGeoCoordinate QPoint1, QPoint2;
            QPoint1.setLongitude(dLon);
            QPoint1.setLatitude(dLat);
            QPoint2.setLongitude((linepoint + 1)->dLon);
            QPoint2.setLatitude((linepoint + 1)->dLat);
            dbearing = QPoint1.azimuthTo(QPoint2);
        }

        int nColorIndex = 0;
        for (auto itrLoad : LoadMap)
        {
            bool bPower = false;
            bool bScan = false;
            for (auto itr : itrLoad.second.PowerVec)
            {
                if (nPointNo >= itr.nStartNo)
                {

                    if (nPointNo < itr.nStopNo || itr.nStopNo == 0)
                    {
                        bPower = true;
                        break;
                    }
                }
            }

            if (bPower == false)
            {
                continue;
            }

            int nSacnArea = 100;
            int nSacnSpeed = 5;
            for (auto itrPara : itrLoad.second.ScanParaMap)
            {
                if (itrPara.first <= nPointNo)
                {
                    nSacnArea = itrPara.second.nStartNo;
                    nSacnSpeed = itrPara.second.nStopNo;
                }
            }
            for (auto itrscan : itrLoad.second.ScanVec)
            {
                if (nPointNo >= itrscan.nStartNo)
                {
                    if (nPointNo < itrscan.nStopNo || itrscan.nStopNo == 0)
                    {
                        if (m_ScanTimer->isActive() == false)
                        {
                            m_ScanTimer->start();
                        }
                        DrawSensorViewEllipse(painter, m_pProj, ptPoint, snLength2, nSacnArea, dbearing - 90, m_nColor[nColorIndex], 20);
                        bScan = true;
                        break;
                    }
                }
            }
            if (bScan == false)
            {
                DrawSensorViewEllipse(painter, m_pProj, ptPoint, snLength2, snC, dbearing - 90, m_nColor[nColorIndex], 80);
            }
            else
            {
                nSacnSpeed = nSacnSpeed / DRAW_SCAN_HZ;
                double dbearingNew = dbearing - 90;
                double nTimeAll = 2.0 * nSacnArea / nSacnSpeed;
                double nTimeOne = nTimeAll / 4;
                int m_nTimerNew = m_nTimer % ((int)nTimeAll);

                if (m_nTimerNew > nTimeOne * 3)
                {
                    dbearingNew = dbearingNew + nSacnArea / 2.0 - nSacnSpeed * (m_nTimerNew - nTimeOne * 3);
                }
                else if (m_nTimerNew > nTimeOne)
                {
                    dbearingNew = dbearingNew - nSacnArea / 2.0 + nSacnSpeed * (m_nTimerNew - nTimeOne);
                }
                else
                {
                    dbearingNew = dbearingNew -  nSacnSpeed * m_nTimerNew;
                }
                DrawSensorViewEllipse(painter, m_pProj, ptPoint, snLength2, snC, dbearingNew, m_nColor[nColorIndex], 80);
            }

            nColorIndex++;
        }

        nPointNo++;

    }
}

void LY_TaskManage::DrawSensorViewEllipse(QPainter &painter, IProj *pProj, const QPoint StartPoint, const int nLength, const int nAreaAngel, const int nCurrentAngel, const QColor color, const int nOpenness)
{
    const double viewLength2 = pProj->Map2ScreenDistance(nLength / 2.0);
    const int    viewSensorYaw = nCurrentAngel + (nAreaAngel / 2);
    const int    startAngle = -viewSensorYaw * 16;
    const int    spanAngle = nAreaAngel * 16;
    const QRectF rectangle(StartPoint.x() - viewLength2 / 2, StartPoint.y() - viewLength2 / 2, viewLength2, viewLength2);

    QColor viewColor(color);
    viewColor.setAlpha(nOpenness);
    painter.setBrush(QBrush(viewColor));
    painter.setPen(viewColor);
    painter.drawPie(rectangle, startAngle, spanAngle);
    painter.setBrush(Qt::BrushStyle::NoBrush);
    painter.setPen(color);

    painter.drawEllipse(StartPoint.x() - 1, StartPoint.y() - 1, 3, 3);

}

void LY_TaskManage::DrawLineOld(QPainter &painter, IProj *pProj)
{
    if (m_pDlgEditLine != nullptr)
    {
        WndRouteLineInfoList wndRouteLineInfoList = m_pDlgEditLine->GetWndRoutlineInfoList();

        if (wndRouteLineInfoList.size() > 0)
        {

            for (auto itr : wndRouteLineInfoList)
            {

                if (itr->bLineShowStates == true && itr->vecNebulaRoutePoint.size() > 0)
                {

                    QPoint ptFirst;
                    QColor greeb = itr->nColor;

                    QPen pen(greeb);
                    QPen penText(QColor(255, 255, 255));
                    painter.setPen(pen);

                    for (auto itrLine : itr->vecNebulaRoutePoint)
                    {
                        int nNo = itrLine.nPointID;
                        double dLongitude = itrLine.stNebulaRoutePoint.GetLongitude();
                        double dLatitude = itrLine.stNebulaRoutePoint.GetLatitude();
                        int dHeight = itrLine.stNebulaRoutePoint.GetHeight();

                        QPoint ptPoint = pProj->Jwd2Screen(dLongitude, dLatitude);

                        int nFlag = JudegePointCodeSet(itr->strID, nNo);

                        if (nFlag == false)
                        {
                            DrawPointIcon(painter, ptPoint, itrLine.m_bSelected);
                        }
                        else
                        {
                            DrawCodeSetPointIcon(painter, ptPoint, itrLine.m_bSelected);
                        }

                        if (nNo == 1)
                        {
                            ptFirst = ptPoint;

                            painter.setPen(penText);
                            QPoint ptTmp(ptPoint.rx() - 5, ptPoint.ry() - 5);

                            painter.drawText(ptTmp, QString::fromStdString(itr->strWeaponName + ":" + itr->strID));
                        }
                        else
                        {

                            painter.setPen(penText);
                            QPoint ptTmp(ptPoint.rx() - ((nNo >= 10) ? 9 : 4), ptPoint.ry() - 5);
                            painter.drawText(ptTmp, QString::number(nNo));

                            painter.setPen(pen);
                            painter.drawLine(ptFirst, ptPoint);
                            ptFirst = ptPoint;
                        }

                        painter.setPen(pen);
                        painter.setBrush(greeb);
                        const int rx = (itrLine.m_bSelected ? (LINE_LENGTH + 1) : LINE_LENGTH);
                        painter.drawEllipse(ptPoint, rx, rx);

                    }
                }

            }
        }
    }

    if (m_pDlgAutoPoint != nullptr && m_pDlgAutoPoint->isDlgAutoPointShow() && m_pDlgAutoPoint->drawMarkLine())
    {
        double dStartLon, dStartLat, dStartAlt;
        double dFinishLon, dFinishLat, dFinishAlt;
        double dMarkLon, dMarkLat, dMarkAlt;

        m_pDlgAutoPoint->GetStartPoint(dStartLon, dStartLat, dStartAlt);
        m_pDlgAutoPoint->GetFinishPoint(dFinishLon, dFinishLat, dFinishAlt);
        m_pDlgAutoPoint->GetMarkPoint(dMarkLon, dMarkLat, dMarkAlt);

        unsigned int nColor = 0xff0000;

        QColor greeb = nColor;
        QPen pen(greeb);
        pen.setWidth(3);
        QVector<qreal>dashes;
        qreal space = 4;
        dashes << 3 << space << 3 << space;
        pen.setDashPattern(dashes);
        QPoint ptPoint1 = pProj->Jwd2Screen(dStartLon, dStartLat);
        QPoint ptPoint2 = pProj->Jwd2Screen(dMarkLon, dMarkLat);
        QPoint ptPoint3 = pProj->Jwd2Screen(dFinishLon, dFinishLat);
        painter.setPen(pen);
        painter.drawLine(ptPoint1, ptPoint2);

        painter.setBrush(greeb);
        painter.drawEllipse(ptPoint1, LINE_LENGTH, LINE_LENGTH);
        painter.drawEllipse(ptPoint2, LINE_LENGTH, LINE_LENGTH);
        QColor greeb1 = 0x0000ff;
        QPen pen1(greeb1);
        pen1.setWidth(3);
        painter.setPen(pen1);
        painter.setBrush(greeb1);
        painter.drawEllipse(ptPoint3, LINE_LENGTH, LINE_LENGTH);
        pen1.setDashPattern(dashes);
        painter.setPen(pen1);
        painter.drawLine(ptPoint3, ptPoint2);

        DrawPointIcon(painter, ptPoint1, false);
        DrawPointIcon(painter, ptPoint2, false);
        DrawPointIcon(painter, ptPoint3, false);

        QPen penText(QColor(255, 255, 255));
        painter.setPen(penText);
        QPoint ptTmp1(ptPoint1.rx() - 18, ptPoint1.ry() - 5);
        painter.drawText(ptTmp1, QObject::tr("UAV"));
        QPoint ptTmp2(ptPoint2.rx() - 18, ptPoint2.ry() - 5);
        painter.drawText(ptTmp2, QObject::tr("Mark Point"));
        QPoint ptTmp3(ptPoint3.rx() - 18, ptPoint3.ry() - 5);
        painter.drawText(ptTmp3, QObject::tr("Point of Impact"));

    }
}

void LY_TaskManage::DrawMissionLine(QPainter &painter, IProj *pProj)
{
    int colorIndex = 0;
    CNeubulaMissionLineVec templineVec = m_pDlgMissionLineList->GetShowData();
    for (auto line : templineVec)
    {
        if (line.cShowFlag == true)
        {
            if (colorIndex >= sizeof(m_nColor) / sizeof(m_nColor[0]))
            {
                colorIndex = 0;
            }
            QPoint ptFirst;
            QColor greeb = m_nColor[colorIndex];

            QPen pen(greeb);
            QPen penText(QColor(255, 255, 255));
            painter.setPen(pen);
            int nID = 1;

            for (auto linepoint : line.LinePointVec)
            {
                int nNo = nID;
                double dLongitude = linepoint.dLon;
                double dLatitude = linepoint.dLat;
                int dHeight = linepoint.dAlt;

                QPoint ptPoint = pProj->Jwd2Screen(dLongitude, dLatitude);
                if (ptPoint.x() >= 0 && ptPoint.y() >= 0 && ptPoint.x() < 7680 && ptPoint.y() < 4320)
                {

                    if (line.nID == m_nMissionSelectLineID &&
                            linepoint.nNo == m_nMissionSelectPointID)
                    {
                        DrawPointIcon(painter, ptPoint, true);
                    }
                    else
                    {
                        DrawPointIcon(painter, ptPoint, false);
                    }

                    if (nNo == 1)
                    {
                        ptFirst = ptPoint;

                        painter.setPen(penText);
                        QPoint ptTmp(ptPoint.rx() - 5, ptPoint.ry() - 5);
                        QString strTaskID, strPlatformID;
                        m_pDlgMissionLineList->GetShowInfoByLineID(line.nID, strTaskID, strPlatformID);
                        if (strPlatformID == "")
                        {

                            painter.drawText(ptTmp, QString::number(line.nID));
                        }
                        else
                        {
                            painter.drawText(ptTmp, strPlatformID);
                        }
                    }
                    else
                    {

                        painter.setPen(penText);
                        QPoint ptTmp(ptPoint.rx() - ((nNo >= 10) ? 9 : 4), ptPoint.ry() - 5);
                        painter.drawText(ptTmp, QString::number(nNo));

                        painter.setPen(pen);
                        painter.drawLine(ptFirst, ptPoint);
                        ptFirst = ptPoint;
                    }

                    painter.setPen(pen);
                    painter.setBrush(greeb);

                    const int rx = LINE_LENGTH;
                    painter.drawEllipse(ptPoint, rx, rx);
                }
                nID++;
            }
        }
        colorIndex++;
    }
}

void LY_TaskManage::on_ScanTimer()
{
    QMutexLocker locker(&(LY_TaskManage::Get().m_MutexTimer));

    LY_TaskManage::Get().m_nTimer++;
}

#define FIRE_MAX_ALT	200
#define FIRE_MIN_ALT	1
void LY_TaskManage::DrawFireView(QPainter &painter, IProj *pProj)
{
    int nLevel = m_pDlgFireManage->GetSelectedTreeLevel();
    if (nLevel < 2)
    {
        return;
    }

    static int snBombR = 0;
    static int snBombH = 0;
    static int snBombPrecision = 0;
    if (snBombR == 0)
    {
        QString strIni = LY_TaskManage::Get().GetConfDir();
        strIni += "ly.ini";
        QFile file(strIni);
        if (file.exists())
        {
            QSettings ini(strIni, QSettings::IniFormat);
            ini.beginGroup("TrackMgr");
            snBombR = ini.value("BombR", "20").toInt();
            snBombH = ini.value("BombH", "50").toInt();
            snBombPrecision = ini.value("BombPrecision", "30000").toInt();
        }
    }

    QString strTaskID, strPlatformID, strLoadID;
    m_pDlgFireManage->GetTaskInfo(strTaskID, strPlatformID, strLoadID);

    QColor color = 0xffff00;
    QBrush brush(color, Qt::SolidPattern);
    painter.setPen(color);
    painter.setBrush(brush);

    int nLineID = ITaskManagerActivator::Get().getPTaskManageService()->GetMissionLineID(strTaskID.toStdString(), strPlatformID.toStdString());
    CNeubulaMissionLine stline = ITaskManagerActivator::Get().getPTaskManageService()->GetMissionLine(nLineID);
    int nPointNo = 1;
    double dbearing = 0;
    auto linepoint = stline.LinePointVec.begin();
    for (; linepoint != stline.LinePointVec.end(); linepoint++)
    {
        double dLon = linepoint->dLon;
        double dLat = linepoint->dLat;
        double dAlt = linepoint->dAlt;
        QPoint ptPoint = pProj->Jwd2Screen(dLon, dLat);

        if (linepoint + 1 != stline.LinePointVec.end())
        {
            QGeoCoordinate QPoint1, QPoint2;
            QPoint1.setLongitude(dLon);
            QPoint1.setLatitude(dLat);
            QPoint2.setLongitude((linepoint + 1)->dLon);
            QPoint2.setLatitude((linepoint + 1)->dLat);
            dbearing = QPoint1.azimuthTo(QPoint2);
        }

        CCodeSet_Load_DB_Vec CodeSetVec = ITaskManagerActivator::Get().getPTaskManageService()->GetCodeSet_Load(strTaskID.toStdString(), strPlatformID.toStdString(), nPointNo);
        if (CodeSetVec.size() == 0)
        {
            nPointNo++;
            continue;
        }

        for (auto CodeSet : CodeSetVec)
        {
            if (CodeSet.GetCodeID() == qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Unlock_Insure)
            {

            }
            else if (CodeSet.GetCodeID() == qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Shot_Percussion)
            {
                double dDis = m_pDlgFireManageEdit->Get80mmFireRange(dLon, dLat, dAlt);

                unsigned int nColor = 0xff0000;
                QColor greeb = nColor;
                QPen pen(greeb);
                pen.setWidth(3);
                QVector<qreal>dashes;
                qreal space = 4;
                dashes << 3 << space << 3 << space;
                pen.setDashPattern(dashes);

                QGeoCoordinate QPoint1, QPoint2;
                QPoint1.setLongitude(dLon);
                QPoint1.setLatitude(dLat);
                QPoint2 = QPoint1.atDistanceAndAzimuth(dDis, dbearing);

                QPoint ptPoint3 = pProj->Jwd2Screen(QPoint2.longitude(), QPoint2.latitude());
                painter.setPen(pen);
                painter.drawLine(ptPoint, ptPoint3);

                painter.setBrush(greeb);
                painter.drawEllipse(ptPoint3, LINE_LENGTH, LINE_LENGTH);
                greeb.setAlpha(50);
                painter.setBrush(QBrush(greeb));
                double viewLength = pProj->Map2ScreenDistance(snBombR * 2);
                const QRectF rectangle(ptPoint3.x() - viewLength / 2, ptPoint3.y() - viewLength / 2, viewLength, viewLength);
                painter.drawEllipse(rectangle);

            }
            else if (CodeSet.GetCodeID() == qnzkna::NebulaMissionManage::NebulaMissionAutoCommandManage::CodeSetLoad_Parameter_CodeID::CodeSetLoad_Parameter_CodeID_DY_CommonCommand_Lock_Reset)
            {

            }
        }

        nPointNo++;
    }

    static int MarkDisMin = 0;
    static int MarkDisMax = 0;
    if (MarkDisMin == 0)
    {
        MarkDisMin = m_pDlgFireManageEdit->Get80mmFireRange(0, 0, FIRE_MIN_ALT);
        MarkDisMax = m_pDlgFireManageEdit->Get80mmFireRange(0, 0, FIRE_MAX_ALT);
    }

    std::vector<std::string> MarkVec = m_pDlgFireManage->GetMarkList();
    for (auto itr : MarkVec)
    {
        if (itr == "")
        {
            continue;
        }

        SystemMarkInfo stInfo = ITaskManagerActivator::Get().getPIMarkManageService()->GetMark(itr);
        QPoint ptPoint = pProj->Jwd2Screen(stInfo.GetLon(), stInfo.GetLat());

        QPainterPath BigCircle;
        double viewLength = pProj->Map2ScreenDistance(MarkDisMax * 2);
        BigCircle.addEllipse(ptPoint.x() - viewLength / 2, ptPoint.y() - viewLength / 2, viewLength, viewLength);

        QPainterPath SmallCircle;
        viewLength = pProj->Map2ScreenDistance(MarkDisMin * 2);
        BigCircle.addEllipse(ptPoint.x() - viewLength / 2, ptPoint.y() - viewLength / 2, viewLength, viewLength);

        unsigned int nColor = 0xff0000;
        QColor greeb = nColor;
        painter.setPen(nColor);
        QColor viewColor(nColor);
        viewColor.setAlpha(50);
        painter.setBrush(QBrush(viewColor));

        QPainterPath path = BigCircle - SmallCircle;
        painter.drawPath(path);
    }
}

void LY_TaskManage::DrawCommunicateView(QPainter &painter, IProj *pProj)
{
    int nLevel = m_pDlgCommunicateManage->GetSelectedTreeLevel();
    if (nLevel < 3)
    {
        return;
    }

    if (m_ShapLineVec.size() == 0)
    {
        return;
    }

    QMutexLocker locker(&m_MutexShapLine);

    CDrawShape ds;
    CDrawShape::Style style;
    for (auto itr : m_ShapLineVec)
    {
        if (itr.pShapLine[0] != NULL)
        {
            style.clrPen = QColor(Qt::green);
            ds.Draw(itr.pShapLine[0], painter, pProj, style);
        }
        if (itr.pShapLine[1] != NULL)
        {
            style.clrPen = QColor(Qt::red);
            ds.Draw(itr.pShapLine[1], painter, pProj, style);
        }
    }
}

void LY_TaskManage::DrawCommunicateCoverView(QPainter &painter, IProj *pProj)
{
    CoverAreaVec AreaVec = m_pDlgCommunicateManage->GetCoverAreaVec();

    if (AreaVec.size() == 0)
    {
        return;
    }

    for (auto area : AreaVec)
    {
        if (area.bShow == false)
        {
            continue;
        }

        CoverPointVec stPointVec = area.m_CoverPointVec;
        QPolygon polygon;
        for (auto stpoint : stPointVec)
        {
            QPoint pt = pProj->Jwd2Screen(stpoint.dLon, stpoint.dLat);
            polygon << pt;
        }
        painter.setPen(m_nColor[0]);
        QColor viewColor(m_nColor[0]);
        viewColor.setAlpha(50);
        painter.setBrush(QBrush(viewColor));
        painter.drawPolygon(polygon);
    }
}

void LY_TaskManage::UpdateShapLineVec()
{
    ShapLineVec lineVec = m_pDlgCommunicateManage->GetShapLineVec();
    if (lineVec.size() == 0)
    {
        return;
    }

    QMutexLocker locker(&m_MutexShapLine);

    for (auto data : m_ShapLineVec)
    {
        for (int k = 0; k < 2; k++)
        {
            if (bflag == true)
            {
                continue;
            }
            if (data.pShapLine[k] != NULL)
            {
                SHPDestroyObject(data.pShapLine[k]);
                data.pShapLine[k] = NULL;
            }
        }
    }
    m_ShapLineVec.clear();

    for (auto data : lineVec)
    {
        ShapLine stShapLine;
        for (int k = 0; k < 2; k++)
        {
            SHPObject *pShpTmp = data.pShapLine[k];
            if (pShpTmp == NULL)
            {
                continue;
            }
            for (int i = 0; i < pShpTmp->nVertices; i++)
            {
                m_pProj->jwxy(pShpTmp->padfX[i], pShpTmp->padfY[i], &pShpTmp->padfX[i], &pShpTmp->padfY[i]);
            }
            stShapLine.pShapLine[k] = SHPCreateObject(pShpTmp->nSHPType, -1, pShpTmp->nParts,
                                                      pShpTmp->panPartStart, pShpTmp->panPartType, pShpTmp->nVertices,
                                                      pShpTmp->padfX, pShpTmp->padfY, pShpTmp->padfZ,
                                                      pShpTmp->bMeasureIsUsed ? pShpTmp->padfM : NULL);
        }
        m_ShapLineVec.push_back(stShapLine);
    }
}

void LY_TaskManage::DrawMeasureIntervisibility(QPainter &painter, IProj *pProj)
{
    m_pShpVec = m_pDlgCommunicateManage->m_pShpVec;
    for (int i = 0; i < m_pShpVec.size(); i++)
    {
        for (int k = 0; k < 2; k++)
        {
            SHPObject *pShp = m_pShpVec[i][k];
            if (m_pShpVec[i][k] != NULL)
            {

                m_pShpVec[i][k] = NULL;
            }
            if (pShp == nullptr)
            {
                continue;
            }
            for (int j = 0; j < pShp->nVertices; j++)
            {
                m_pProj->jwxy(pShp->padfX[j], pShp->padfY[j], &pShp->padfX[j], &pShp->padfY[j]);
            }
            m_pShpVec[i][k] = SHPCreateObject(pShp->nSHPType, -1, pShp->nParts,
                                              pShp->panPartStart, pShp->panPartType, pShp->nVertices,
                                              pShp->padfX, pShp->padfY, pShp->padfZ,
                                              pShp->bMeasureIsUsed ? pShp->padfM : NULL);
            SHPDestroyObject(pShp);
        }
    }
    bIntervisibility = false;

}

CDlgAutoPoint *LY_TaskManage::GetDlgAutoPoint()
{
    return m_pDlgAutoPoint;
}

CDlgActionEdit *LY_TaskManage::GetCDlgActionEdit()
{
    return m_pDlgActionEdit;
}

CDlgActionList *LY_TaskManage::GetCDlgActionList()
{
    return m_pDlgActionList;
}

CDlgEditLine *LY_TaskManage::GetDlgEditLine()
{
    return m_pDlgEditLine;
}

CDlgMissionList *LY_TaskManage::GetDlgMissionList()
{
    return m_pDlgMissionList;
}

CDlgMissionEdit *LY_TaskManage::GetDlgMissionEdit()
{
    return m_pDlgMissionEdit;
}

CDlgMissionPlatformList *LY_TaskManage::GetDlgMissionPlatformList()
{
    return m_pDlgMissionPlatformList;
}

CDlgMissionPlatformEdit *LY_TaskManage::GetDlgMissionPlatformEdit()
{
    return m_pDlgMissionPlatformEdit;
}

CDlgMissionPlatformLoadList *LY_TaskManage::GetDlgMissionPlatformLoadList()
{
    return m_pDlgMissionPlatformLoadList;
}

CDlgMissionPlatformLoadEdit *LY_TaskManage::GetDlgMissionPlatformLoadEdit()
{
    return m_pDlgMissionPlatformLoadEdit;
}

CDlgPlay *LY_TaskManage::GetDlgPlay()
{
    return m_pDlgPlay;
}

CDlgCodeSet_Load_Tree *LY_TaskManage::getDlgCodeSet_Load_Tree() const
{
    return m_pDlgCodeSet_Load_Tree;
}

CDlgCodeSet_Load_Edit *LY_TaskManage::getDlgCodeSet_Load_Edit() const
{
    return m_pDlgCodeSet_Load_Edit;
}

CDlgMissionLineList *LY_TaskManage::GetDlgMissionLineList() const
{
    return m_pDlgMissionLineList;
}

CDlgMissionLinePointEdit *LY_TaskManage::GetDlgMissionLinePointEdit() const
{
    return m_pDlgMissionLinePointEdit;
}

CDlgTaskLineEdit *LY_TaskManage::GetDlgTaskLineEdit() const
{
    return m_pDlgTaskLineEdit;
}

void LY_TaskManage::NewDlgTaskLineEdit()
{
    m_pDlgTaskLineEdit = new CDlgTaskLineEdit();
    m_pDlgTaskLineEdit->hide();
}

CDlgSensorManageEdit *LY_TaskManage::GetDlgSensorManageEdit() const
{
    return m_pDlgSensorManageEdit;
}

CDlgFireManageEdit *LY_TaskManage::GetDlgFireManageEdit() const
{
    return m_pDlgFireManageEdit;
}
CDlgMissionTree *LY_TaskManage::GetDlgMissionTree() const
{
    return m_pDlgMissionTree;
}
CDlgMissionPlatformInfo *LY_TaskManage::GetDlgMissionPlatfromInfo()
{
    return m_pDlgMissionPlatfromInfo;
}

CDlgMarkSchemeList *LY_TaskManage::GetDlgMarkSchemeList() const
{
    return m_pDlgMarkSchemeList;
}

CDlgMissionPlanningOverview *LY_TaskManage::GetDlgMissionPlanningOverview() const
{
    return m_pDlgMissionPlanningOverview;
}

CDlgMissionAllocation *LY_TaskManage::GetDlgMissionAllocation() const
{
    return m_pDlgMissionAllocation;
}

CDlgAddMaintainMark *LY_TaskManage::GetDlgAddMaintainMark() const
{
    return 	m_pDlgAddMaintainMark;
}

CDlgMarkMaintain *LY_TaskManage::GetDlgMaintainMark() const
{
    return m_pDlgMarkMaintain;
}
IProj *LY_TaskManage::GetIProj() const
{
    return m_pProj;
}

WidgetLineEdit *LY_TaskManage::GetWidgetLineEdit() const
{
    return m_pWidgetLineEdit;
}

IResourceCarryingPlanDialog *LY_TaskManage::GetIResourceCarryingPlan() const
{
    return m_pResourceCarryingPlanDialog;
}

ISAddCarryingPlanResDialog *LY_TaskManage::GetISAddCarryingPlanResDialog() const
{
    return m_pISAddCarryingPlanResDialog;
}

CDlgPlaySpeedControl * LY_TaskManage::GetPlaySpeedControl() const
{
	return m_pDlgPlaySpeedControl;
}

CDlgForecastTendency * LY_TaskManage::GetForecastTendency() const
{
	return m_pDlgForecastTendency;
}
