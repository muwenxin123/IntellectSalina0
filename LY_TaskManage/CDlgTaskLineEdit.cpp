#include "CDlgTaskLineEdit.h"

#include "ITaskManagerActivator.h"
#include "lY_TaskManage.h"
#include <QGeoCoordinate>
#include "QRegExpExample.h"

CDlgTaskLineEdit::CDlgTaskLineEdit(QWidget *parent):
    LyDialog(tr("Route Editing"), parent),
    ui(new Ui::CDlgTaskLineEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(1200, 800);

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

    ui->pBCreateOrder->setEnabled(false);
    ui->lineEdit_subTime->setValidator(new QRegExpValidator(allNumber));

    initFormPlane();
    initForm();
    ui->lineEdit_dis->setText("20");

    connect(ui->pBSetScreenCenter, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBSetScreenCenter);
    connect(ui->pBDelPoint, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBDelPoint);
    connect(ui->pBInsertPoint, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBInsertPoint);
    connect(ui->pBAddPoint, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBAddPoint);
    connect(ui->pBClearPoint, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBClearPoint);
    connect(ui->pBCreateOrder, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBCreatOrder);
    connect(ui->pBSaveInfo, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBSaveInfo);
    connect(ui->pBDragPoint, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBDragPoint);
    connect(ui->pBEnsure, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBEnsure);
    connect(ui->pBShowOrClose, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_clicked_pBShowOrClose);

    connect(ui->pushButton_StartForm, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_startFormPlane_clicked);
    connect(ui->pushButton_GenForm, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_genFormPlane_clicked);
    connect(ui->pushButton_retainPoints, &QPushButton::clicked, this, &CDlgTaskLineEdit::on_isRetainPoints_clicked);
    connect(ui->comboBoxMissionName, &QComboBox::currentTextChanged, this, &CDlgTaskLineEdit::comboBoxCurrentIndexChanged);

}

CDlgTaskLineEdit::~CDlgTaskLineEdit()
{
    delete ui;
}

void CDlgTaskLineEdit::initMissionData()
{

    if (m_pITaskManageService == NULL)
    {
        return;
    }
    int oldIndex = ui->comboBoxMissionName->currentIndex();
    m_MissionVec.clear();
    ui->comboBoxMissionName->clear();
    m_MissionVec = m_pITaskManageService->GetActivatingMissions();

    selectedActionID = m_pITaskManageService->GetActivatingActionID();
    this->setWindowTitle(QString::fromStdString(m_pITaskManageService->GetNebulaAction(selectedActionID).GetName()));

    for (auto mission : m_MissionVec)
    {
        ui->comboBoxMissionName->addItem(QString::fromStdString(mission.GetID()));
    }
    ui->lineEditMissionName->setText("");

    int comBoxIDIndex = ui->comboBoxMissionName->currentIndex();
    if (oldIndex >= 0 && oldIndex < m_MissionVec.size())
    {
        comBoxIDIndex = oldIndex;
        ui->comboBoxMissionName->setCurrentIndex(oldIndex);
    }
    if (comBoxIDIndex >= 0 && comBoxIDIndex < m_MissionVec.size())
    {
        ui->lineEditMissionName->setText(QString::fromStdString(m_MissionVec[comBoxIDIndex].GetName()));
        ui->lineEditMissionName->setEnabled(false);
    }

}

void CDlgTaskLineEdit::initWidgetLines()
{
    havingRoutes.clear();
    disconnect(ui->tabWidget, &QTabWidget::currentChanged, this, &CDlgTaskLineEdit::tabWidgetChanged);

    clearTabWidget();
    int newLineCount = 0;
    if (ui->comboBoxMissionName->count() > 0)
    {
        int curMissionIndex = ui->comboBoxMissionName->currentIndex();
        CNeubulaMission curMission = m_MissionVec[curMissionIndex];

        CRelationMissionResourceVec platS =  m_pITaskManageService->GetRelationMissionPtResource(curMission.GetID());

        int platNum = 0;
        for (auto platType : platS)
        {
            platNum += platType.GetResourceNum();
        }

        CNeubulaMissionLineVec usedLineVec = m_pITaskManageService->GetMissionLinesByTask(curMission.GetID());

        QString windowTitle01 = "HX";

        int lineIndex = 0;
        for (int i = 0; i < platS.size(); i++)
        {
            for (int j = 0; j < platS[i].GetResourceNum(); j++)
            {
                WidgetLineEdit *widgetLine = new WidgetLineEdit;
                CNeubulaMissionLine curLine;

                if (lineIndex < usedLineVec.size())
                {
                    curLine = usedLineVec[lineIndex];
                }
                else
                {

                    int newNLineID = m_pITaskManageService->GetMissionLineMaxID() + newLineCount;
                    newLineCount++;
                    curLine.nID = newNLineID;

                    QString lineStrName = tr("Route") + QString::number(newNLineID);

                    strcpy(curLine.strName, lineStrName.toLocal8Bit().data());
                    strcpy(curLine.strMissionID, QString::fromStdString(curMission.GetID()).toLocal8Bit().data());

                }
                QString windowTitle = windowTitle01 + QString::number(curLine.nID);
                havingRoutes.push_back(curLine.nID);
                widgetLine->setWindowName(windowTitle);
                widgetLine->setPlatType(QString::fromStdString(platS[i].GetResourceName()));

                widgetLine->setTaskLine(curLine);
                widgetLine->setPlatName(QString::fromStdString(curLine.nPlatId));
                widgetLine->setLineName(tr2(curLine.strName));

				ui->tabWidget->addTab(widgetLine, windowTitle);
				lineIndex++;

            }
        }
    }
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &CDlgTaskLineEdit::tabWidgetChanged);
    if (ui->tabWidget->count() > 0)
    {
        tabWidgetChanged(0);
    }
}

void CDlgTaskLineEdit::update()
{

    initMissionData();
    initWidgetLines();
    tabWidgetChanged(ui->tabWidget->currentIndex());

}

void CDlgTaskLineEdit::clearTabWidget()
{
    widgetLines.clear();
    ui->tabWidget->clear();
}

void CDlgTaskLineEdit::AddPointInLine(double lon, double lat, double alt)
{
    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    QTableWidget *tableWid = curWidget->getTableWidget();
    QList<QTableWidgetItem *> selectedItems = tableWid->selectedItems();
    int PointIndex;
    if (AddPointType == 0)
    {
        PointIndex = tableWid->rowCount();
    }
    else
    {
        if (selectedItems.empty())
        {
            LyMessageBox::warning(this, tr("tip"), tr("Waypoint not selected!"), QMessageBox::Ok);
            return;
        }
        PointIndex = selectedItems.at(0)->row();
    }

    curWidget->addPointN(PointIndex, lon, lat, alt);
    if (isForingPlane)
    {
        formPoints.push_back(QPointF(lon, lat));
    }
}

void CDlgTaskLineEdit::setComBoxCurText(QString curText)
{
    int comBoxCount = ui->comboBoxMissionName->count();
    for (int i = 0; i < comBoxCount; i++)
    {
        if (ui->comboBoxMissionName->itemText(i) == curText)
        {
            if (i == ui->comboBoxMissionName->currentIndex())
            {
                comboBoxCurrentIndexChanged();
            }
            else
            {
                ui->comboBoxMissionName->setCurrentIndex(i);
            }

            break;
        }
    }
}

void CDlgTaskLineEdit::setTabWidgetCurLine(int nLineID)
{
    int tabCount = ui->tabWidget->count();
    for (int i = 0; i < tabCount; i++)
    {
        WidgetLineEdit *tmpWidget = (WidgetLineEdit *)(ui->tabWidget->widget(i));
        QString tmpWidName = tmpWidget->getWindowName();
        QStringList LineIDList = tmpWidName.split("HX");
        if (LineIDList.at(1).toInt() == nLineID)
        {
            if (i == ui->tabWidget->currentIndex())
            {
                tabWidgetChanged(i);
            }
            else
            {
                ui->tabWidget->setCurrentIndex(i);
            }

            break;
        }
    }
}

WidgetLineEdit *CDlgTaskLineEdit::getTabWidgetLine(int nLineID)
{
    int tabCount = ui->tabWidget->count();
    for (int i = 0; i < tabCount; i++)
    {
        WidgetLineEdit *tmpWidget = (WidgetLineEdit *)(ui->tabWidget->widget(i));
        if (tmpWidget->getTaskLine().nID == nLineID)
        {
            return tmpWidget;
        }
    }
    return NULL;
}

void CDlgTaskLineEdit::updateLinePointLL(int nLineID, int nPointID, double dLon, double dLat)
{
    int tabCount = ui->tabWidget->count();
    for (int tabIndex = 0; tabIndex < tabCount; tabIndex++)
    {
        WidgetLineEdit *tmpWid = (WidgetLineEdit *)(ui->tabWidget->widget(tabIndex));
        if ((tmpWid->getTaskLine()).nID == nLineID)
        {
            tmpWid->updateLinePointLonLat(nPointID, dLon, dLat);
            return;
        }

    }
}

void CDlgTaskLineEdit::showWarmSelectedPoint()
{
    LyMessageBox::warning(this, tr("tip"), tr("The selected waypoint doesn't belong to the current task, please choose again!"), QMessageBox::Ok);
    return;
}

WidgetLineEdit *CDlgTaskLineEdit::getCurWidget()
{
    return (WidgetLineEdit *)(ui->tabWidget->currentWidget());
}

void CDlgTaskLineEdit::SanveMisssionPlatHXRelation()
{
    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    std::string sMissionID = ui->comboBoxMissionName->currentText().toStdString();
    std::string sMarkID = curWidget->GetMarkIDString();
    std::string sPlatType = curWidget->GetPlatTypeString();
    QString strRoute = ui->tabWidget->tabText(ui->tabWidget->currentIndex());
    QStringList tempsStringList = strRoute.split("HX");
    std::string sPlatID;
    CNeubulaAmmoConntionMarkVec tempAmmoConntionMarkVec = m_pAmmunitionManageService->GetTaskIDByInfo(sMissionID);
    std::vector<std::string> AmmoIDVec = m_pAmmunitionManageService->GetMarkIDByAmmoMode(tempAmmoConntionMarkVec, sMarkID);
    if (AmmoIDVec.empty())
    {
        LyMessageBox::warning(this, tr("tip"), tr("There is no ammo matching for the selected target."), QMessageBox::Ok);
        return;
    }
    m_pITaskManageService->RemoveRowMissionRelation_PlatRoute(sMissionID, tempsStringList[1].toInt());
    m_pITaskManageService->LoadMissionRelation_PlatRoute();
    QMap<std::string, std::string> tempData;
    CNeubulaConntionAmmoVec platConntionAmmo = m_pAmmunitionManageService->GetAmmunitionConnByTaskID(sMissionID);
    CRelationMissionResourceVec MissionResource = m_pITaskManageService->GetRelationMissionPtResource(sMissionID);
    for (auto data : AmmoIDVec)
    {
        for (auto data1 : platConntionAmmo)
        {
            if (data == data1.GetAmmoID())
            {
                for (auto data2 : MissionResource)
                {
                    if (data2.GetID() == data1.GetRelationID())
                    {
                        QPair<int, std::string>temppair(data1.GetRelationID(), data2.GetResourceName());
                        tempData.insert(data1.GetPlatID(), data2.GetResourceName());
                        break;
                    }
                }
                break;
            }
        }
    }
    CNebulaMissionRelation_PlatRouteVec tempMissionRelation_PlatRoute = m_pITaskManageService->GetMissionIDByPlatRoute(sMissionID);
    std::vector<std::string> PlatIDVec;
    for (auto data : tempMissionRelation_PlatRoute)
    {
        PlatIDVec.push_back(data.GetPlatformID());
    }
    for (auto data : PlatIDVec)
    {
        auto it = tempData.find(data);
        if (it != tempData.end())
        {
            tempData.erase(it);
        }
    }
    auto itr = tempData.begin();
    for (; itr != tempData.end(); ++itr)
    {
        if (itr.value() == sPlatType)
        {
            sPlatID = itr.key();
        }
    }
    if (sPlatID != "")
    {
        CNebulaMissionRelation_PlatRoute temp;
        temp.SetPlatformID(sPlatID);
        temp.SetRouteID(tempsStringList[1].toInt());
        temp.SetTaskID(sMissionID);
        temp.SetMarkID(sMarkID);
        m_pITaskManageService->AddMissionRelation_PlatRoute(temp);
    }

}

void CDlgTaskLineEdit::initFormPlane()
{
    ui->comboBox->clear();
    ui->comboBox->addItem(tr("One word formation"));
    ui->comboBox->addItem(tr("Star formation"));
    ui->comboBox->addItem(tr("Arrow formation"));
    ui->comboBox->addItem(tr("Layered formation"));
}

void CDlgTaskLineEdit::initFormSelectPlanes(int curIndex)
{
    ui->comboBox_Plane->clear();
    for (int i = 0; i < havingRoutes.size(); i++)
    {
        if (i != curIndex)
        {
            ui->comboBox_Plane->AddItem(QString::number(havingRoutes[i]));
        }
    }
}

void CDlgTaskLineEdit::initForm()
{
    forms.clear();
    std::vector<QPointF> form1;
    form1.clear();
    form1.push_back(QPointF(-90.0, 1.0));
    form1.push_back(QPointF(90.0, 1.0));
    form1.push_back(QPointF(-90.0, 2.0));
    form1.push_back(QPointF(90.0, 2.0));
    form1.push_back(QPointF(-90.0, 3.0));
    form1.push_back(QPointF(90.0, 3.0));
    forms.push_back(form1);

    std::vector<QPointF> form2;
    form2.clear();
    form2.push_back(QPointF(-162.0, 1.73205));
    form2.push_back(QPointF(162.0, 1.73205));
    form2.push_back(QPointF(-126.0, 1.0));
    form2.push_back(QPointF(126.0, 1.0));

    forms.push_back(form2);

    std::vector<QPointF> form3;
    form3.clear();
    form3.push_back(QPointF(-135.0, 1.0));
    form3.push_back(QPointF(135.0, 1.0));
    form3.push_back(QPointF(-135.0, 2.0));
    form3.push_back(QPointF(135.0, 2.0));

    forms.push_back(form3);

    std::vector<QPointF> form4;
    form4.clear();
    form4.push_back(QPointF(-90.0, 1.0));
    form4.push_back(QPointF(90.0, 1.0));
    form4.push_back(QPointF(-135.0, 1.414213));
    form4.push_back(QPointF(135.0, 1.414213));
    forms.push_back(form4);

}

void CDlgTaskLineEdit::on_clicked_pBSetScreenCenter()
{
    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    QTableWidget *tableWid = curWidget->getTableWidget();
    QList<QTableWidgetItem *> selectedItems = tableWid->selectedItems();

    int PointIndex = tableWid->rowCount();
    if (PointIndex > 0)
    {
        LY_TaskManage::Get().SetScreenCenter(curWidget->getTaskLine().LinePointVec[0].dLon,
                                             curWidget->getTaskLine().LinePointVec[0].dLat);
    }
}

void CDlgTaskLineEdit::on_clicked_pBDelPoint()
{
    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    QTableWidget *tableWid = curWidget->getTableWidget();
    QList<QTableWidgetItem *> selectedItems = tableWid->selectedItems();
    if (selectedItems.empty())
    {
        return;
    }
    int selectedRow = selectedItems.at(0)->row();
    curWidget->delPointN(selectedRow);
}

void CDlgTaskLineEdit::on_clicked_pBInsertPoint()
{
    if (isDrag)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Do this after you have finished the drag operation."));
        return;
    }
    if (isAddPoint && AddPointType == 0)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Do this after you have finished the adding operation."));
        return;
    }
    isAddPoint = !isAddPoint;
    if (isAddPoint)
    {
        ui->pBInsertPoint->setText(tr("End the Insertion"));
        ui->pBAddPoint->setEnabled(false);
        ui->pBDragPoint->setEnabled(false);
    }
    else
    {
        ui->pBInsertPoint->setText(tr("Insert Waypoints"));
        ui->pBAddPoint->setEnabled(true);
        ui->pBDragPoint->setEnabled(true);
    }

    AddPointType = 1;

    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    QTableWidget *tableWid = curWidget->getTableWidget();
    QList<QTableWidgetItem *> selectedItems = tableWid->selectedItems();
    if (selectedItems.empty())
    {
        return;
    }
    int PointIndex = selectedItems.at(0)->row();
    if (PointIndex >= 0 && isAddPoint == true)
    {

    }

}

void CDlgTaskLineEdit::on_clicked_pBAddPoint()
{
    if (isDrag)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Do this after you have finished the drag operation."));
        return;
    }
    if (isAddPoint && AddPointType == 1)
    {
        LyMessageBox::information(NULL, tr("tip"), tr("Do this after you have finished the adding operation."));
        return;
    }
    isAddPoint = !isAddPoint;
    if (isAddPoint)
    {
        ui->pBAddPoint->setText(tr("Finish Adding"));
        ui->pBInsertPoint->setEnabled(false);
        ui->pBDragPoint->setEnabled(false);
    }
    else
    {
        ui->pBAddPoint->setText(tr("Add Waypoints"));
        ui->pBInsertPoint->setEnabled(true);
        ui->pBDragPoint->setEnabled(true);
    }
    AddPointType = 0;

    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    QTableWidget *tableWid = curWidget->getTableWidget();
    QList<QTableWidgetItem *> selectedItems = tableWid->selectedItems();
    int PointIndex = tableWid->rowCount();
    if (PointIndex > 0 && isAddPoint == true)
    {

    }
}

void CDlgTaskLineEdit::on_clicked_pBClearPoint()
{
    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    curWidget->clearPoints();
}

void CDlgTaskLineEdit::on_clicked_pBCreatOrder()
{

}

void CDlgTaskLineEdit::on_clicked_pBSaveInfo()
{
    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    std::string curMissionID = m_MissionVec[ui->comboBoxMissionName->currentIndex()].GetID();

    if (curWidget->getIsNewLine())
    {
        int newLineID = m_pITaskManageService->GetMissionLineMaxID();
        curWidget->updateMissionLineID(newLineID);
        curWidget->setIsNewLine(false);
    }

    curWidget->getTaskLine();
    m_pITaskManageService->RemoveMissionLine(curWidget->getTaskLine().nID);
    bool isAdd = m_pITaskManageService->AddMissionLine(curWidget->getTaskLine());
    if (isAdd)
    {
        LyMessageBox::information(this, tr("tip"), tr("Route saved successfully!"), QMessageBox::Ok);
    }
    else
    {
        LyMessageBox::information(this, tr("tip"), tr("Route saved failed!"), QMessageBox::Ok);
        return;
    }
    SanveMisssionPlatHXRelation();

}

void CDlgTaskLineEdit::on_clicked_pBDragPoint()
{
    if (isAddPoint)
    {
        if (AddPointType == 0)
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Do this after you have finished the adding operation."));
            return;
        }
        else
        {
            LyMessageBox::information(NULL, tr("tip"), tr("Do this after you have finished the insertion operation."));
            return;
        }

    }
    isDrag = !isDrag;
    if (isDrag)
    {
        ui->pBDragPoint->setText(tr("End the Drag"));
        ui->pBInsertPoint->setEnabled(false);
        ui->pBAddPoint->setEnabled(false);
    }
    else
    {
        ui->pBDragPoint->setText(tr("Drag Waypoints"));
        ui->pBInsertPoint->setEnabled(true);
        ui->pBAddPoint->setEnabled(true);
    }

    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }
    QTableWidget *tableWid = curWidget->getTableWidget();
    QList<QTableWidgetItem *> selectedItems = tableWid->selectedItems();
    if (selectedItems.empty())
    {
        if (curWidget->getTaskLine().LinePointVec.size() > 0)
        {

        }
    }
    else
    {
        int PointIndex = selectedItems.at(0)->row();
        if (PointIndex >= 0 && isAddPoint == true)
        {

        }
    }

}

void CDlgTaskLineEdit::tabWidgetChanged(int curIndex)
{
    int tabCount = ui->tabWidget->count();
    if (tabCount > curIndex && curIndex >= 0 && LY_TaskManage::Get().GetDlgMissionLineList())
    {
        initFormSelectPlanes(curIndex);
        LY_TaskManage::Get().GetDlgMissionLineList()->UpdateShowData();

        WidgetLineEdit *tmpWid = (WidgetLineEdit *)(ui->tabWidget->widget(curIndex));

        QString tmpWidName = tmpWid->getWindowName();
        QStringList LineIDList = tmpWidName.split("HX");

        CNeubulaMissionLine tmpLine = m_pITaskManageService->GetMissionLine(LineIDList.at(1).toInt());
        if (tmpLine.nID == 0)
        {
            tmpLine.nID = LineIDList.at(1).toInt();
        }

        if (strlen(tmpLine.strMissionID) == 0)
        {
            QString missionID = ui->comboBoxMissionName->currentText();
            QString linName = tr("HX") + QString::number(tmpLine.nID);

            strcpy(tmpLine.strName, linName.toLocal8Bit().data());
            strcpy(tmpLine.strMissionID, missionID.toLocal8Bit().data());
        }

        if (tmpLine.nID == 0 && tmpLine.LinePointVec.size() == 0)
        {

        }
        else
        {
            tmpWid->setTaskLine(tmpLine);
            auto missionShowLines = LY_TaskManage::Get().GetDlgMissionLineList()->getMissionShowLines();
            for (int showLineIndex = 0; showLineIndex < missionShowLines.size(); showLineIndex++)
            {
                if (missionShowLines[showLineIndex].nID == tmpLine.nID)
                {
                    if (missionShowLines[showLineIndex].cShowFlag == true)
                    {
                        ui->pBShowOrClose->setText(tr("Hidden Air Route"));
                    }
                    else
                    {
                        ui->pBShowOrClose->setText(tr("Display Air Route"));
                    }
                    break;
                }
            }
        }

    }

}

void CDlgTaskLineEdit::on_clicked_pBEnsure()
{
    this->close();
}

void CDlgTaskLineEdit::on_clicked_pBShowOrClose()
{
    WidgetLineEdit *tmpWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (tmpWidget == nullptr)
    {
        return;
    }
    int tmpLineID = tmpWidget->getTaskLine().nID;
    auto missionShowLines = LY_TaskManage::Get().GetDlgMissionLineList()->getMissionShowLines();

    if (ui->pBShowOrClose->text() == tr("Display Air Route"))
    {
        ui->pBShowOrClose->setText(tr("Hidden Air Route"));
    }
    else
    {
        ui->pBShowOrClose->setText(tr("Display Air Route"));
    }

    for (int showLineIndex = 0; showLineIndex < missionShowLines.size(); showLineIndex++)
    {
        if (missionShowLines[showLineIndex].nID == tmpLineID)
        {
            LY_TaskManage::Get().GetDlgMissionLineList()->selectBox(showLineIndex, 1);
            return;
        }
    }

}

void CDlgTaskLineEdit::on_startFormPlane_clicked()
{

    on_clicked_pBAddPoint();
    isForingPlane = !isForingPlane;
    if (isForingPlane)
    {
        formPoints.clear();
        ui->pushButton_StartForm->setText(tr("End formation"));
    }
    else
    {
        ui->pushButton_StartForm->setText(tr("Start formation"));
    }
}

void CDlgTaskLineEdit::on_genFormPlane_clicked()
{

    int formIndex = ui->comboBox->currentIndex();
    QStringList planeList = ui->comboBox_Plane->GetSelItemsText();
    if (planeList.size() > 4)
    {
        LyMessageBox::information(this, tr("Warning"), tr("Choose too many slaves,do not exceed 4"));
        return;
    }
    int Dis = ui->lineEdit_dis->text().toUInt();
    std::map<int, char> routes;
    routes.clear();
    for (auto routeId : planeList)
    {
        routes[routeId.toInt()] = '1';
    }

    if (!isRetainPoints)
    {
        int tabCount = ui->tabWidget->count();
        for (int i = 0; i < tabCount; i++)
        {
            WidgetLineEdit *tmpWid = (WidgetLineEdit *)(ui->tabWidget->widget(i));
            auto routesItor = routes.find(tmpWid->getLineID());
            if (routesItor != routes.end())
            {
                tmpWid->clearPoints();
            }
        }
        WidgetLineEdit *curWid = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
        if (curWid != NULL)
        {
            curWid->clearPoints();
        }

        for (int i = 0; i < formPoints.size(); i++)
        {
            curWid->addPointN(i, formPoints[i].x(), formPoints[i].y());
        }
    }

    std::vector<QPointF> form;
    if (formIndex < forms.size())
    {
        form = forms[formIndex];
    }
    else
    {
        form = forms[0];
    }

    if (ui->lineEdit_subTime->text() == NULL || ui->lineEdit_subTime->text().toInt() == 0)
    {
        if (formPoints.size() == 1)
        {
            QGeoCoordinate form0(formPoints[0].y(), formPoints[0].x());
            int routeDic = 0;
            int formIndex = 0;
            auto lineRouteItor = routes.begin();
            while (lineRouteItor != routes.end())
            {
                int routeID = lineRouteItor->first;
                QGeoCoordinate tmpFormPos = form0.atDistanceAndAzimuth(form[formIndex].y() * Dis, form[formIndex].x() + routeDic);

                int tabCount = ui->tabWidget->count();
                for (int i = 0; i < tabCount; i++)
                {
                    WidgetLineEdit *tmpWid = (WidgetLineEdit *)(ui->tabWidget->widget(i));
                    if (tmpWid != NULL && tmpWid->getLineID() == routeID)
                    {
                        int lineSize = tmpWid->getTaskLine().LinePointVec.size();
                        tmpWid->addPointN(lineSize, tmpFormPos.longitude(), tmpFormPos.latitude());
                        break;
                    }
                }
                lineRouteItor++;
                formIndex++;
            }
        }
        else
        {
            for (int i = 0; i < formPoints.size(); i++)
            {
                QGeoCoordinate form0;
                QGeoCoordinate form0Next;
                if (i == formPoints.size() - 1)
                {
                    form0.setLongitude(formPoints[i - 1].x());
                    form0.setLatitude(formPoints[i - 1].y());
                    form0Next.setLongitude(formPoints[i].x());
                    form0Next.setLatitude(formPoints[i].y());
                }
                else
                {
                    form0.setLongitude(formPoints[i].x());
                    form0.setLatitude(formPoints[i].y());
                    form0Next.setLongitude(formPoints[i + 1].x());
                    form0Next.setLatitude(formPoints[i + 1].y());
                }

                int routeDic = form0.azimuthTo(form0Next);
                int formIndex = 0;
                auto lineRouteItor = routes.begin();
                while (lineRouteItor != routes.end())
                {
                    int routeID = lineRouteItor->first;
                    QGeoCoordinate tmpFormPos = form0.atDistanceAndAzimuth(form[formIndex].y() * Dis, form[formIndex].x() + routeDic);
                    QGeoCoordinate tmpFormPosNext = form0Next.atDistanceAndAzimuth(form[formIndex].y() * Dis, form[formIndex].x() + routeDic);
                    int tabCount = ui->tabWidget->count();
                    for (int i = 0; i < tabCount; i++)
                    {
                        WidgetLineEdit *tmpWid = (WidgetLineEdit *)(ui->tabWidget->widget(i));
                        if (tmpWid != NULL && tmpWid->getLineID() == routeID)
                        {
                            int lineSize = tmpWid->getTaskLine().LinePointVec.size();
                            tmpWid->addPointN(lineSize, tmpFormPos.longitude(), tmpFormPos.latitude());
                            tmpWid->addPointN(++lineSize, tmpFormPosNext.longitude(), tmpFormPosNext.latitude());
                            break;
                        }
                    }
                    lineRouteItor++;
                    formIndex++;
                }
            }
        }
    }
    else
    {
        int subTime = ui->lineEdit_subTime->text().toInt();
        if (formPoints.size() < 2)
        {
            LyMessageBox::information(this, tr2("提示"), tr2("选择的航点数量小于2"));
        }
        QGeoCoordinate form0;
        QGeoCoordinate form0Next;

        form0.setLongitude(formPoints[0].x());
        form0.setLatitude(formPoints[0].y());
        form0Next.setLongitude(formPoints[1].x());
        form0Next.setLatitude(formPoints[1].y());

        int routeDic = form0.azimuthTo(form0Next);
        int formIndex = 0;
        auto lineRouteItor = routes.begin();
        while (lineRouteItor != routes.end())
        {
            int routeID = lineRouteItor->first;
            QGeoCoordinate tmpFormPos = form0.atDistanceAndAzimuth(form[formIndex].y() * Dis, form[formIndex].x() + routeDic);
            int tabCount = ui->tabWidget->count();
            for (int i = 0; i < tabCount; i++)
            {
                WidgetLineEdit *tmpWid = (WidgetLineEdit *)(ui->tabWidget->widget(i));
                if (tmpWid != NULL && tmpWid->getLineID() == routeID)
                {
                    int lineSize = tmpWid->getTaskLine().LinePointVec.size();
                    tmpWid->addPointN(lineSize, tmpFormPos.longitude(), tmpFormPos.latitude());
                    tmpWid->setHoveringTime(lineSize, subTime * (formIndex + 1));

                    for (int i = 1; i < formPoints.size(); i++)
                    {
                        tmpWid->addPointN(++lineSize, formPoints[i].x(), formPoints[i].y());
                        tmpWid->setHoveringTime(lineSize, 0);
                    }

                    break;
                }
            }
            lineRouteItor++;
            formIndex++;
        }
    }

    WidgetLineEdit *curWidget = (WidgetLineEdit *)(ui->tabWidget->currentWidget());
    if (curWidget == NULL)
    {
        return;
    }

    if (curWidget->getIsNewLine())
    {
        int newLineID = m_pITaskManageService->GetMissionLineMaxID();
        curWidget->updateMissionLineID(newLineID);
        curWidget->setIsNewLine(false);
    }
    m_pITaskManageService->RemoveMissionLine(curWidget->getTaskLine().nID);
    m_pITaskManageService->AddMissionLine(curWidget->getTaskLine());

    int tabCount = ui->tabWidget->count();
    for (int i = 0; i < tabCount; i++)
    {
        WidgetLineEdit *tmpWid = (WidgetLineEdit *)(ui->tabWidget->widget(i));
        auto routesItor = routes.find(tmpWid->getLineID());
        if (routesItor != routes.end())
        {
            if (tmpWid->getIsNewLine())
            {
                int newLineID = m_pITaskManageService->GetMissionLineMaxID();
                tmpWid->updateMissionLineID(newLineID);
                tmpWid->setIsNewLine(false);
            }
            m_pITaskManageService->RemoveMissionLine(tmpWid->getTaskLine().nID);
            m_pITaskManageService->AddMissionLine(tmpWid->getTaskLine());
        }
    }

}

void CDlgTaskLineEdit::on_isRetainPoints_clicked()
{
    isRetainPoints = !isRetainPoints;
    if (isRetainPoints)
    {
        ui->pushButton_retainPoints->setText(tr("Do not retain existing waypoints"));
    }
    else
    {
        ui->pushButton_retainPoints->setText(tr("Keep existing waypoints"));
    }
}

void CDlgTaskLineEdit::comboBoxCurrentIndexChanged()
{

    ui->lineEditMissionName->setText("");
    int comBoxIDIndex = ui->comboBoxMissionName->currentIndex();
    if (comBoxIDIndex >= 0 && comBoxIDIndex < m_MissionVec.size())
    {
        ui->lineEditMissionName->setText(QString::fromStdString(m_MissionVec[comBoxIDIndex].GetName()));
        ui->lineEditMissionName->setEnabled(false);
    }
    initWidgetLines();
}

void CDlgTaskLineEdit::closeEvent(QCloseEvent *event)
{

    ui->pBAddPoint->setText(tr("Add Waypoints"));
    ui->pBAddPoint->setEnabled(true);

    ui->pBInsertPoint->setText(tr("Insert Waypoints"));
    ui->pBInsertPoint->setEnabled(true);

    isAddPoint = false;

    ui->pBDragPoint->setText(tr("Drag Waypoints"));
    ui->pBDragPoint->setEnabled(true);
    isDrag = false;

}
