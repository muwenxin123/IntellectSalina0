#include "CDlgMissionPlanningOverview.h"
#include "ui_CDlgMissionPlanningOverview.h"
#include <QPainter>

#define		LINE_LENGTH		3

CDlgMissionPlanningOverview::CDlgMissionPlanningOverview(QWidget *parent) :
    LyDialog(tr2("任务规划总览"), parent),
    ui(new Ui::CDlgMissionPlanningOverview)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(900, 600);

    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    m_pAmmunitionManageService = ITaskManagerActivator::Get().getPAmmunitionManageService();
    if (m_pAmmunitionManageService == nullptr)
    {
        return;
    }
    m_timer = new QTimer(this);
    m_timer->start(10000);

    localWeaponNameBindToId();

    int colorIndex = 0;
    m_nColor[colorIndex] = 0xffff00;
    m_nColor[colorIndex++] = 0xffff00;
    m_nColor[colorIndex++] = 0xffa500;
    m_nColor[colorIndex++] = 0x008000;
    m_nColor[colorIndex++] = 0x808080;
    m_nColor[colorIndex++] = 0x000000;

    m_nColor[colorIndex++] = 0x800000;
    m_nColor[colorIndex++] = 0x808000;
    m_nColor[colorIndex++] = 0x00ff00;
    m_nColor[colorIndex++] = 0x00ffff;
    m_nColor[colorIndex++] = 0x008080;
    m_nColor[colorIndex++] = 0x0000ff;
    m_nColor[colorIndex++] = 0x000080;
    m_nColor[colorIndex++] = 0xff00ff;
    m_nColor[colorIndex++] = 0x800080;
    m_nColor[colorIndex++] = 0x404040;
    m_nColor[colorIndex++] = 0xc000c0;
    m_nColor[colorIndex++] = 0x00ffff;
    m_nColor[colorIndex++] = 0xaa5500;
    m_nColor[colorIndex] = 0xffffff;
    m_nColor[colorIndex++] = 0xc0c0c0;

    m_IconImage.load(tr2(":/images/marker.png"));
    m_IconImageSelected.load(tr2(":/images/marker_sel.png"));

    Init();

    connect(m_timer, &QTimer::timeout, this, &CDlgMissionPlanningOverview::on_timeOut);
    connect(ui->treeWidget, &QTreeWidget::customContextMenuRequested, this, &CDlgMissionPlanningOverview::on_treeWidget_customContextMenuRequested);

}

CDlgMissionPlanningOverview::~CDlgMissionPlanningOverview()
{
    if (m_timer != nullptr)
    {
        m_timer->stop();
    }
    delete ui;
}

void CDlgMissionPlanningOverview::Init()
{
    m_pITaskManageService = ITaskManagerActivator::Get().getPTaskManageService();
    if (m_pITaskManageService == nullptr)
    {
        return;
    }
    m_pAmmunitionManageService = ITaskManagerActivator::Get().getPAmmunitionManageService();
    if (m_pAmmunitionManageService == nullptr)
    {
        return;
    }
    m_pITaskManageService->LoadMissionRelation_PlatRoute();
    currActionID = m_pITaskManageService->GetActivatingActionID();
    currAction = m_pITaskManageService->GetNebulaAction(currActionID);
    m_NeubulaActionMissionVec = m_pITaskManageService->GetActivatingMissions();
    m_pAmmunitionManageService->LoadAmmoMarkDB();
    m_pAmmunitionManageService->LoadAmmoDB();
    ui->treeWidget->clear();
    QStringList header;
    header << tr2("任务编号") << tr2("任务名称") <<
           tr2("平台名称") << tr2("DY编号") <<
           tr2("DY类型") << tr2("目标编号") <<
           tr2("航线编号") << tr2("吊舱指令") <<
           tr2("DY指令")
           ;
    ui->treeWidget->setHeaderLabels(header);
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->treeWidget->header()->setDefaultAlignment(Qt::AlignCenter);

    int rowCount = 0;
    for (auto TaskInfo : m_NeubulaActionMissionVec)
    {
        auto Mark = TaskInfo.GetMarkIDList();
        rowCount += Mark.size();
    }

    if (ITaskManagerActivator::Get().getPIMarkManageService() != nullptr) {
        Areas = ITaskManagerActivator::Get().getPIMarkManageService()->GeAirApaceByID(currAction.GetAreaID());
    }

    InitMarkTypeSvgNames();

    CreateTreeWidget();
    CreateTreeeMenu();
}

void CDlgMissionPlanningOverview::CreateTreeWidget()
{
    if (ITaskManagerActivator::Get().getPIMarkManageService() == nullptr || m_pAmmunitionManageService == nullptr || m_pITaskManageService == nullptr) {
        return;
    }
    MissionPlanInfoVec.clear();
    for (auto TaskInfo : m_NeubulaActionMissionVec)
    {
        MissionPlanInfo mplanInfo;

        CNeubulaConntionAmmoVec TaskPlatAmmoVec = m_pAmmunitionManageService->GetAmmunitionConnByTaskID(TaskInfo.GetID());
        CNeubulaAmmoConntionMarkVec TaskMarkAmmoVec = m_pAmmunitionManageService->GetTaskIDByInfo(TaskInfo.GetID());
        CNeubulaMissionLineVec LineVec = m_pITaskManageService->GetMissionLinesByTask(TaskInfo.GetID());
        if (TaskPlatAmmoVec.empty())
        {
            continue;
        }

        QTreeWidgetItem *root1 = new QTreeWidgetItem(ui->treeWidget);
        root1->setText(0, tr2(TaskInfo.GetID().c_str()));
        root1->setText(1, TaskInfo.GetName().c_str());
        auto Mark = TaskInfo.GetMarkIDList();

        mplanInfo.missionID = TaskInfo.GetID();

        int index = 0;
        for (auto plat : TaskPlatAmmoVec)
        {

            std::string plat_name = WeaponIDToName(plat.GetPlatID());
            QTreeWidgetItem *root2 = new QTreeWidgetItem(root1);
            root2->setText(2, QString::fromStdString(plat_name.c_str()));
            mplanInfo.platID = plat.GetPlatID();

            std::string ammo_id = plat.GetAmmoID();
            root2->setText(3, QString::fromStdString(ammo_id.c_str()));
            mplanInfo.AmmoID = ammo_id;

            CNeubulaAmmunitionInfo AmmoInfo = m_pAmmunitionManageService->GetAmmoByID(ammo_id);
            std::string Ammo_mode = AmmoInfo.GetAmmoMode();
            root2->setText(4, tr2(Ammo_mode.c_str()));

            std::string tar_id;
            for (auto tar : TaskMarkAmmoVec)
            {
                if (tar.GetAmmoMode() == ammo_id)
                {
                    tar_id = tar.GetMarkID();
                    break;
                }
            }

            if (!tar_id.empty())
            {

                SystemMarkInfo tmpTar = ITaskManagerActivator::Get().getPIMarkManageService()->GetMark(tar_id);
                root2->setText(5, QString::fromStdString(tmpTar.GetName().c_str()));

                CNebulaMissionRelation_PlatRouteVec PlatRoute = m_pITaskManageService->GetMissionIDByPlatRoute(TaskInfo.GetID());
                for (auto data : PlatRoute)
                {
                    std::string Tempplat_name = WeaponIDToName(data.GetPlatformID());
                    if (plat_name == Tempplat_name && tmpTar.GetID() == data.GetMarkID())
                    {
                        root2->setText(6, QString::number(data.GetRouteID()));
                    }
                }
                int mlineID = -1;
                if (!LineVec.empty() && LineVec.size() - 1 >= index)
                {
                    CNeubulaMissionLine mlineInfo = LineVec[index];
                    mlineID = mlineInfo.nID;
                }
                if (mlineID > 0)
                {
                    root2->setText(6, QString::number(mlineID));
                    mplanInfo.nlineID = mlineID;
                }
                mplanInfo.targetID = tmpTar.GetID();

                index++;
            }
            MissionPlanInfoVec.push_back(mplanInfo);

            root2->setText(7, tr2("航点:1;指令:吊舱 开始录像;航点:8;指令:吊舱 停止录像"));
            root2->setText(8, tr2("航点:1;指令:DY 解锁 保险1号DY;"));
        }
        ui->treeWidget->expandAll();
    }
}

void CDlgMissionPlanningOverview::CreateTreeeMenu()
{
    m_TreeMenuRight = new QMenu(this);
    QAction *TaskMenu_Edit = m_TreeMenuRight->addAction(tr2("编辑"));
    connect(TaskMenu_Edit, SIGNAL(triggered()), this, SLOT(on_Menu_MarkAmmo_Edit()));
}

void CDlgMissionPlanningOverview::localWeaponNameBindToId()
{
    WeaponTypeNames.clear();
    if (ITaskManagerActivator::Get().getPWeaponManageService() == nullptr) {
        return;
    }

    ITaskManagerActivator::Get().getPWeaponManageService()->GetWeaponSystemMap(weaponSystemMap);
    for (auto sysItr : weaponSystemMap)
    {
        auto weaponComPonentMap = sysItr.second.GetWeaponComponentMapPtr();
        for (auto weapon : *weaponComPonentMap)
        {
            SystemSysNameAndID[weapon.second.getComponentName()] = weapon.second.getComponentID();
            SystemSysIDAndName[weapon.second.getComponentID()] = weapon.second.getComponentName();

            short int tmpInt = weapon.second.getPWeaponModel()->getComponentType();
            auto tmpItor = WeaponTypeNames.find(tmpInt);
            if (tmpItor != WeaponTypeNames.end())
            {
                tmpItor->second.push_back(weapon.second.getComponentName());
            }
            else
            {
                std::vector<std::string> tmpNames;
                tmpNames.clear();
                tmpNames.push_back(weapon.second.getComponentName());
                WeaponTypeNames[tmpInt] = tmpNames;
            }
        }
    }
}

std::string CDlgMissionPlanningOverview::WeaponIDToName(const std::string nID)
{
    std::string temp;
    auto tmpItor = SystemSysIDAndName.find(nID);
    if (tmpItor != SystemSysIDAndName.end())
    {
        temp = tmpItor->second;
    }
    return temp;
}

void CDlgMissionPlanningOverview::InitMarkTypeSvgNames()
{
    QString dirQStr = "../data/images/svg/";
    QString NoQStr = "NoKnoe.svg";
    QString build = "build.svg";
    QString car = "car.svg";
    QString man = "man.svg";
    QString uav = "uav.svg";

    targetTypeSvgNames.clear();
    targetTypeSvgNames.emplace_back(dirQStr + NoQStr);
    targetTypeSvgNames.emplace_back(dirQStr + build);
    targetTypeSvgNames.emplace_back(dirQStr + car);
    targetTypeSvgNames.emplace_back(dirQStr + man);
    targetTypeSvgNames.emplace_back(dirQStr + uav);
}

std::vector<QPolygonF> CDlgMissionPlanningOverview::unionPolygon(std::vector<QPolygonF> &tmpQPolygonVec)
{
    std::vector<QPolygonF> returnVec;
    defUnionPolygon(tmpQPolygonVec, returnVec);
    return returnVec;
}

void CDlgMissionPlanningOverview::defUnionPolygon(std::vector<QPolygonF> &beforeVec, std::vector<QPolygonF> &unitedPolys)
{
    if (beforeVec.empty())
    {
        return;
    }
    QPolygonF firstPoly = beforeVec[0];
    auto PolyItor = beforeVec.begin();
    PolyItor++;

    std::vector<QPolygonF> nextBeforeVecc;
    nextBeforeVecc.clear();
    while (PolyItor != beforeVec.end())
    {
        if (firstPoly.intersects(*PolyItor))
        {
            firstPoly = firstPoly.united(*PolyItor);
        }
        else
        {
            nextBeforeVecc.emplace_back(*PolyItor);
        }
        PolyItor++;
    }
    unitedPolys.emplace_back(firstPoly);
    defUnionPolygon(nextBeforeVecc, unitedPolys);
}

void CDlgMissionPlanningOverview::drawMissionArea(QPainter &painter, IProj *pProj)
{
    auto pointList = Areas.GetPoints();

    if (pointList.size() > 0)
    {
        QVector<QPoint> points;
        points.clear();
        for (int pointIndex = 0; pointIndex < pointList.size(); pointIndex++)
        {
            QPoint point1 = pProj->Jwd2Screen(pointList[pointIndex].dLon,
                                              pointList[pointIndex].dLat);
            points.push_back(point1);
        }
        QPolygon poly(points);

        QColor colorTemp(0, 255, 0, 60);
        QPen pen(colorTemp, 1);
        painter.setPen(pen);
        painter.setBrush(QBrush(colorTemp));
        painter.drawPolygon(poly, Qt::WindingFill);
    }
}

void CDlgMissionPlanningOverview::drawMarkInfoRange(QPainter &painter, IProj *pProj)
{
    long srceenScale = pProj->GetMapScale();
    if (srceenScale > 200000)
    {
        return;
    }

    QPen penGreen(Qt::green, 1);
    QPen penRed(Qt::red, 1);
    QPen pendBlue(Qt::blue, 1);

    tmpQPolygonSenserVec.clear();
    tmpQPolygonFireVec.clear();
    targetVitalDirPolys.clear();
    m_tmpPolygonCircle.clear();
    m_tmpPolyonPie.clear();
    QList<QTreeWidgetItem *> selectedItmes = ui->treeWidget->selectedItems();
    std::pair<QPointF, double> tmpCircle;
    if (!MissionPlanInfoVec.empty())
    {
        for (auto miPlan : MissionPlanInfoVec)
        {
            QString strid = "";
            for (QTreeWidgetItem *item : selectedItmes)
            {
                strid = item->text(0);
                break;
            }
            if (miPlan.missionID.c_str() != strid.toLocal8Bit())
            {
                continue;
            }
            if (!miPlan.targetID.empty())
            {
                SystemMarkInfo targetinfo = ITaskManagerActivator::Get().getPIMarkManageService()->GetMark(miPlan.targetID);
                double lon = targetinfo.GetLon();
                double lat = targetinfo.GetLat();
                QPointF pf = pProj->Jwd2Screen(lon, lat);

                double azimut = targetinfo.GetPitch();

                double vitalDirection = targetinfo.GetVitalDirection();
                vitalDirection += azimut;
                double vitalSize = targetinfo.GetVitalSize();

                double vitalDisScreen = pProj->Map2ScreenDistance(2000);
                VitalPostion pos;
                QPointF point(lon, lat);
                pos.targetPoint = point;
                pos.vitalDir = 360 - vitalSize;
                pos.vitalDisScreen = vitalDisScreen;
                pos.vitalSize = vitalDirection;
                m_tmpPolyonPie.emplace_back(pos);
                QPainterPath vitalPa;
                QRect rectVital(pf.x(), pf.y(), vitalDisScreen, vitalDisScreen);
                rectVital.moveCenter(QPoint(pf.x(), pf.y()));
                vitalPa.moveTo(rectVital.center());
                vitalPa.arcTo(rectVital, (90 - vitalDirection), -vitalSize);

                QPolygonF m_areaVital = vitalPa.toFillPolygon();
                targetVitalDirPolys[targetinfo.GetID()] = m_areaVital;

                painter.setPen(pendBlue);
                painter.setBrush(QColor(11, 11, 11, 0));
                painter.drawPolygon(m_areaVital);

                if (true)
                {

                    double dDefenceNorm = targetinfo.GetDefenceNorm() * 150;
                    double dScreenDefenceNorm = pProj->Map2ScreenDistance(dDefenceNorm);

                    double azimutRange = 22.5;
                    QPainterPath paFire;

                    QRect rectFire(pf.x(), pf.y(), dScreenDefenceNorm, dScreenDefenceNorm);
                    rectFire.moveCenter(QPoint(pf.x(), pf.y()));
                    paFire.moveTo(rectFire.center());
                    paFire.arcTo(rectFire, (90 - azimut), -(2 * azimutRange));

                    QPolygonF m_Area2 = paFire.toFillPolygon();
                    tmpQPolygonFireVec.push_back(m_Area2);
                }

                if (true)
                {

                    double dSenser = targetinfo.GetDefenceNorm() * 200;
                    double dScreenSenser = pProj->Map2ScreenDistance(dSenser);

                    QPainterPath paSecser;
                    paSecser.addEllipse(pf, dScreenSenser, dScreenSenser);

                    QPolygonF m_Area = paSecser.toFillPolygon();

                    tmpQPolygonSenserVec.push_back(m_Area);
                }

                ESystemMarkType targetTypeNum = targetinfo.GetType();

                QSvgRenderer targetSvg;
                int typeNum = targetTypeNum;
                if (typeNum > targetTypeSvgNames.size() || typeNum < 0)
                {
                    typeNum = 0;
                }
                targetSvg.load(targetTypeSvgNames[typeNum]);
                int len = 10;

                int Slong = pProj->Map2ScreenDistance(len);
                if (Slong == 0)
                {
                    Slong = 5;
                }
                painter.resetTransform();
                painter.translate(pf.x(), pf.y());
                double targetPitch = targetinfo.GetPitch();
                painter.rotate(targetPitch);

                QRect SvgRect(0, 0, Slong, Slong);
                SvgRect.moveCenter(QPoint(0, 0));

                targetSvg.render(&painter, SvgRect);
                painter.resetTransform();

            }
        }

        if (!tmpQPolygonSenserVec.empty())
        {
            std::vector<QPolygonF> unitedPolys = unionPolygon(tmpQPolygonSenserVec);
            for (auto poly : unitedPolys)
            {
                painter.setPen(penGreen);
                QColor colorTemp(255, 255, 0, 20);
                painter.setBrush(QBrush(colorTemp));
                painter.drawPolygon(poly);
            }
        }

        if (!tmpQPolygonFireVec.empty())
        {
            std::vector<QPolygonF> unitedPolys = unionPolygon(tmpQPolygonFireVec);
            for (auto poly : unitedPolys)
            {
                painter.setPen(penRed);
                QColor colorTemp(255, 0, 0, 20);
                painter.setBrush(QBrush(colorTemp));
                painter.drawPolygon(poly);
            }
        }
    }
}

void CDlgMissionPlanningOverview::drawPlanLine(QPainter &painter, IProj *pProj)
{
    QList<QTreeWidgetItem *> selectedItmes = ui->treeWidget->selectedItems();

    if (!MissionPlanInfoVec.empty())
    {
        int colorIndex = 0;
        for (auto miPlan : MissionPlanInfoVec)
        {
            QString strid = "";
            for (QTreeWidgetItem *item : selectedItmes)
            {
                strid = item->text(0);
                break;
            }
            if (miPlan.missionID.c_str() != strid.toLocal8Bit())
            {
                continue;
            }
            std::string uav_name = WeaponIDToName(miPlan.platID);

            int nID = 1;
            QPen penText(QColor(255, 255, 255));
            if (colorIndex > sizeof(m_nColor))
            {
                colorIndex = 0;
            }
            QPen pen(m_nColor[colorIndex]);
            QColor greeb = m_nColor[colorIndex];
            QPoint ptFirst;
            CNeubulaMissionLine mline = ITaskManagerActivator::Get().getPTaskManageService()->GetMissionLine(miPlan.nlineID);
            for (auto linepoint : mline.LinePointVec)
            {
                int nNo = nID;
                double dLongitude = linepoint.dLon;
                double dLatitude = linepoint.dLat;
                int dHeight = linepoint.dAlt;

                QPoint ptPoint = pProj->Jwd2Screen(dLongitude, dLatitude);

                const QPixmap &m_pic = m_IconImage;
                QPointF pmt;
                pmt.setX(ptPoint.x() - 9);
                pmt.setY(ptPoint.y() - 21);
                painter.drawPixmap(pmt, m_pic);

                if (nNo == 1)
                {
                    ptFirst = ptPoint;

                    painter.setPen(penText);
                    QPoint ptTmp(ptPoint.rx() - 5, ptPoint.ry() - 25);
                    QString strTaskID, strPlatformID;

                    painter.drawText(ptTmp, tr2(uav_name.c_str()));

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

                nID++;
            }
            colorIndex++;
        }

    }
}

void CDlgMissionPlanningOverview::showEvent(QShowEvent *event)
{
    Init();
    b_isDraw = true;
}

void CDlgMissionPlanningOverview::hideEvent(QHideEvent *event)
{
    b_isDraw = false;
}

void CDlgMissionPlanningOverview::on_timeOut()
{
    if (b_UpdateAmmo)
    {
        Init();
        b_UpdateAmmo = false;
    }
}

void CDlgMissionPlanningOverview::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    m_TreeMenuRight->exec(QCursor::pos());
}
