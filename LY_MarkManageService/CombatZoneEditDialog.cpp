#include "CombatZoneEditDialog.h"
#include "ui_CombatZoneEditDialog.h"
#include <QDebug>

#include "QCString.h"
#include "IMarkManageImpl.h"
#include "lyUI/LyMessageBox.h"
#include "LY_MarkManageService.h"
#include "QRegExpExample.h"

CombatZoneEditDialog::CombatZoneEditDialog(QWidget *parent) :
    LyDialog(tr2("编辑区域"), parent),
    ui(new Ui::CombatZoneEditDialog)
{

    ui->setupUi(centralWidget());
    this->setResizable(true);

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));

    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));

    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));

    connect(ui->comboBox_CombatZone_Type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_CombatZone_Type_currentIndexChanged(int)));

    connect(ui->comboBox_Mark, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(on_comboBox_Mark_currentTextChanged(const QString &)));

    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(on_pushButton_4_clicked()));

    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(on_pushButton_5_clicked()));

    QStringList list01, list02;

    list01 << tr2("未知") << tr2("标记区域") << tr2("打击区域") << tr2("危险区域") << tr2("安全区域") << tr2("行动区域");
    list02 << tr2("多边形") << tr2("圆柱") << tr2("扇形");

    ui->comboBox_CombatZone_Attitude->addItems(list01);
    ui->comboBox_CombatZone_Type->addItems(list02);
    m_PickPointStates = PICK_AS_POINT_NORMAL;

    ui->lineEdit_CombatZone_ID->setValidator(new QRegExpValidator(regNumStrUnderLine, this));
    ui->lineEdit_CombatZone_CreateTime_2->setValidator(new QRegExpValidator(regDouble, this));
    ui->lineEdit_34->setValidator(new QRegExpValidator(regDouble, this));
    ui->lineEdit_35->setValidator(new QRegExpValidator(regDouble, this));
    ui->lineEdit_31->setValidator(new QRegExpValidator(regLongitude, this));
    ui->lineEdit_32->setValidator(new QRegExpValidator(regLatitude, this));
    ui->lineEdit_33->setValidator(new QRegExpValidator(regDouble, this));
    ui->leX->setValidator(new QRegExpValidator(regLongitude, this));
    ui->leY->setValidator(new QRegExpValidator(regLatitude, this));
    ui->leHeight->setValidator(new QRegExpValidator(regDouble, this));
    ui->leRadius->setValidator(new QRegExpValidator(regDouble, this));
    ui->leStartAngle->setValidator(new QRegExpValidator(regAngle360, this));
    ui->leEndAngle->setValidator(new QRegExpValidator(regAngle360, this));

    CreateListHeading();
}

void CombatZoneEditDialog::show_GroupBox_CombatZone(const int type)
{
    if (ui == nullptr || ui->comboBox_CombatZone_Type == nullptr)
    {
        return ;
    }

    m_ZonePointVec.clear();

    switch (type)
    {
    case 0:
    {
        ui->groupBox_CombatZone_Cubic->setVisible(true);
        ui->groupBox_CombatZone_Cylinder->setVisible(false);
        ui->groupBox_CombatZone_Sector->setVisible(false);
    }
    break;
    case 1:
    {
        ui->groupBox_CombatZone_Cubic->setVisible(false);
        ui->groupBox_CombatZone_Cylinder->setVisible(true);
        ui->groupBox_CombatZone_Sector->setVisible(false);
    }
    break;
    case 2:
    {
        ui->groupBox_CombatZone_Cubic->setVisible(false);
        ui->groupBox_CombatZone_Cylinder->setVisible(false);
        ui->groupBox_CombatZone_Sector->setVisible(true);
    }
    break;
    default:
        break;
    }
}

CombatZoneEditDialog::~CombatZoneEditDialog()
{
    delete ui;
}

void CombatZoneEditDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CombatZoneEditDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    UpdataMarkCombobx();
    if (m_nType == 1)
    {
        show_GroupBox_CombatZone(ui->comboBox_CombatZone_Type->currentIndex());
    }

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

void CombatZoneEditDialog::InitWindowCtrl()
{
    ui->lineEdit_CombatZone_ID->setText("");
    ui->lineEdit_CombatZone_Name->setText("");

    ui->textEdit_CombatZone_Describe->setText("");

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
    ui->lineEdit_CombatZone_CreateTime_2->setText("");

    ui->lineEdit_34->setText("");
    ui->lineEdit_35->setText("");
    ui->lineEdit_31->setText("");
    ui->lineEdit_32->setText("");
    ui->lineEdit_33->setText("");

    ui->leX->setText("");
    ui->leY->setText("");
    ui->leHeight->setText("");
    ui->leRadius->setText("");
    ui->leStartAngle->setText("");
    ui->leEndAngle->setText("");

    ui->comboBox_CombatZone_Attitude->setCurrentIndex(0);
    ui->comboBox_CombatZone_Type->setCurrentIndex(0);
}

void CombatZoneEditDialog::SetWindowInfo(CZoneInfo &stInfo)
{
    InitWindowCtrl();
    if (m_nType == 1)
    {
        ui->lineEdit_CombatZone_ID->setEnabled(true);
        return;
    }

    m_ZoneInfo = stInfo;

    ui->lineEdit_CombatZone_ID->setText(QString::fromStdString(stInfo.GetID()));
    ui->lineEdit_CombatZone_Name->setText(QString::fromStdString(stInfo.GetName()));

    QString strTime;
    if (stInfo.GetCreateTime() == "")
    {
        ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    }
    else
    {
        strTime = QString::fromStdString(stInfo.GetCreateTime());
        QDateTime dataTime = QDateTime::fromString(strTime, "yyyy/M/d h:mm");
        ui->dateTimeEdit->setDateTime(dataTime);
    }

    ui->textEdit_CombatZone_Describe->setText(QString::fromStdString(stInfo.GetDescribe()));

    ui->comboBox_CombatZone_Attitude->setCurrentIndex(stInfo.GetAttributeENum(stInfo.GetAttribute()));
    ui->comboBox_CombatZone_Type->setCurrentIndex(stInfo.GetShapeENum(stInfo.GetShape()));

    if (stInfo.GetShape() == EZoneShape_Circle)
    {
        if (stInfo.GetPoints().size() > 1)
        {
            return;
        }
        for (auto point : stInfo.GetPoints())
        {
            ui->lineEdit_34->setText(QString::number(point.dRadius, 'f', 2));
            ui->lineEdit_35->setText(QString::number(point.dHeight, 'f', 2));
            ui->lineEdit_31->setText(QString::number(point.dLon, 'f', 7));
            ui->lineEdit_32->setText(QString::number(point.dLat, 'f', 7));
            ui->lineEdit_33->setText(QString::number(point.dHeight, 'f', 2));

            m_ZonePointVec.clear();
            AddWindowPoint(point.dLon, point.dLat, point.dHeight, point.dRadius);
        }
    }
    else if (stInfo.GetShape() == EZoneShape_Polygon)
    {
        m_ZonePointVec.clear();
        for (auto Point : stInfo.GetPoints())
        {

            AddWindowPoint(Point.dLon, Point.dLat, Point.dHeight, Point.dHeight);
        }

        UpdateListData(stInfo.GetPoints());
    }
    else if (stInfo.GetShape() == EZoneShape_Sector)
    {
        if (stInfo.GetPoints().size() > 0)
        {
            ui->leX->setText(QString::number(stInfo.GetPoints().front().dLon, 'f', 7));
            ui->leY->setText(QString::number(stInfo.GetPoints().front().dLat, 'f', 7));
            ui->leHeight->setText(QString::number(stInfo.GetPoints().front().dHeight, 'f', 2));
            ui->leRadius->setText(QString::number(stInfo.GetPoints().front().dRadius, 'f', 2));
            ui->leStartAngle->setText(QString::number(stInfo.GetPoints().front().dStartAngle, 'f', 2));
            ui->leEndAngle->setText(QString::number(stInfo.GetPoints().front().dEndAngle, 'f', 2));
        }
    }

    ui->lineEdit_CombatZone_ID->setEnabled(false);
}

void CombatZoneEditDialog::SetDataType(int nType)
{
    m_nType = nType;
}

void CombatZoneEditDialog::UpdataMarkCombobx()
{
    ui->comboBox_Mark->clear();

    ui->lineEdit_31->setEnabled(true);
    ui->lineEdit_32->setEnabled(true);

    CSystemMarkInfoVec infoVec;
    infoVec = qnzkna::MarkManage::IMarkManageImpl::Get().GetMarkList();

    QStringList strList;
    strList.push_back(tr("-"));
    for (auto data : infoVec)
    {
        strList.push_back(QString::fromStdString(data.GetID()));
    }
    ui->comboBox_Mark->addItems(strList);

    if (m_nType == 2)
    {
        QString strID1 = QString::fromStdString(m_ZoneInfo.GetMarkID());

        int nIndex01 = ui->comboBox_Mark->findText(strID1);

        if (nIndex01 != -1)
        {
            ui->comboBox_Mark->setCurrentIndex(nIndex01);
        }
    }
}

int CombatZoneEditDialog::GetPickPointStates()
{
    return m_PickPointStates;
}

void CombatZoneEditDialog::SetPostion(double &dLon, double &dLat, double &dHeight)
{
    if (0 == QString::compare(ui->comboBox_CombatZone_Type->currentText(), tr2("圆柱")))
    {
        ui->lineEdit_31->setText(QString::number(dLon, 'f', 7));
        ui->lineEdit_32->setText(QString::number(dLat, 'f', 7));

        m_ZonePointVec.clear();
        double dRadius = ui->lineEdit_34->text().toDouble();
        if (dRadius <= 0)
        {
            dRadius = 0;
        }
        AddWindowPoint(dLon, dLat, dHeight, dRadius);
    }
    else if (0 == QString::compare(ui->comboBox_CombatZone_Type->currentText(), tr2("多边形")))
    {
        int nNo = 0;
        double dHeight = 0;

        nNo = m_ZonePointVec.size();
        ZonePoint stZonePoint;
        stZonePoint.dLon = dLon;
        stZonePoint.dLat = dLat;
        stZonePoint, dHeight = dHeight;
        UpdateListData(nNo, stZonePoint);

        AddWindowPoint(dLon, dLat, dHeight, dHeight);
    }
    else if (0 == QString::compare(ui->comboBox_CombatZone_Type->currentText(), tr2("扇形")))
    {
        ui->leX->setText(QString::number(dLon, 'f', 7));
        ui->leY->setText(QString::number(dLat, 'f', 7));
    }
}

void CombatZoneEditDialog::SetWindowPoint(int &nNo, double &dLon, double &dLat, double &dHeight, double &dRadius)
{
    QMutexLocker locker(&sMutex);

    if (nNo >= m_ZonePointVec.size())
    {
        return;
    }

    auto itr = m_ZonePointVec.begin();
    if (itr + nNo == m_ZonePointVec.end())
    {
        return;
    }
    itr += nNo;
    itr->dLon = dLon;
    itr->dLat = dLat;
    itr->dHeight = dHeight;
    itr->dRadius = dRadius;

    UpdateListData(nNo, *itr);
}

void CombatZoneEditDialog::AddWindowPoint(double &dLon, double &dLat, double &dHeight, double &dRadius)
{
    QMutexLocker locker(&sMutex);

    ZonePoint stPoint;
    stPoint.dLon = dLon;
    stPoint.dLat = dLat;
    stPoint.dHeight = dHeight;
    stPoint.dRadius = dRadius;

    m_ZonePointVec.push_back(stPoint);

}

void CombatZoneEditDialog::RemoveWindowPoint(int &nNo)
{
    QMutexLocker locker(&sMutex);

    auto itr = m_ZonePointVec.begin();
    if (itr + nNo == m_ZonePointVec.end())
    {
        return;
    }
    itr += nNo;
    m_ZonePointVec.erase(itr);
}

bool CombatZoneEditDialog::GetWindowPointMap(EZoneShape &eStates, ZoneWindowPointVec &wndMap)
{
    QMutexLocker locker(&sMutex);

    CZoneInfo stInfo;
    eStates = stInfo.GetShapeENum(ui->comboBox_CombatZone_Type->currentIndex());

    if (m_ZonePointVec.size() == 0)
    {
        return false;
    }
    else
    {
        wndMap = m_ZonePointVec;
        return true;
    }
}

void CombatZoneEditDialog::CreateListHeading()
{
    QStringList headerlabels;

    headerlabels << tr2("序号") << tr2("经度") << tr2("纬度") << tr2("高度");

    ui->tableWidget->setColumnCount(headerlabels.size());
    ui->tableWidget->setHorizontalHeaderLabels(headerlabels);
    ui->tableWidget->verticalHeader()->setHidden(true);
    ui->tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    for (int i = 0; i < headerlabels.size(); i++)
    {
        if (i == 0 || i == 3)
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
        else
        {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        }
    }
}

void CombatZoneEditDialog::UpdateListData(const ZonePointVec &PointVec)
{
    if (PointVec.size() == 0)
    {
        return;
    }

    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(PointVec.size());

    int nCount = 0;
    for (auto point : PointVec)
    {
        UpdateListData(nCount, point);
        nCount++;
    }
}

void CombatZoneEditDialog::UpdateListData(int &nCount, const ZonePoint &stAirPoint)
{
    QString strNo, strLon, strLat, strHeight;
    if (nCount < 0)
    {
        return;
    }

    if (nCount == ui->tableWidget->rowCount())
    {
        ui->tableWidget->setRowCount(nCount + 1);
    }

    strNo = QString::number(nCount + 1);
    strLon = QString::number(stAirPoint.dLon, 'f', 7);
    strLat = QString::number(stAirPoint.dLat, 'f', 7);
    strHeight = QString::number(stAirPoint.dHeight, 'f', 2);

    QTableWidgetItem *item0 = new QTableWidgetItem(strNo);
    QTableWidgetItem *item1 = new QTableWidgetItem(strLon);
    QTableWidgetItem *item2 = new QTableWidgetItem(strLat);
    QTableWidgetItem *item3 = new QTableWidgetItem(strHeight);

    ui->tableWidget->setItem(nCount, 0, item0);
    ui->tableWidget->setItem(nCount, 1, item1);
    ui->tableWidget->setItem(nCount, 2, item2);
    ui->tableWidget->setItem(nCount, 3, item3);

    ui->tableWidget->item(nCount, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nCount, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nCount, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    ui->tableWidget->item(nCount, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void CombatZoneEditDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

	setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
}

void CombatZoneEditDialog::on_pushButton_clicked()
{
    QString strBTN = ui->pushButton->text();

    if (m_PickPointStates == PICK_POINT_NORMAL)
    {
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->pushButton->setText(tr2("结束取点"));
        m_PickPointStates = PICK_AS_POINT_EDIT;
    }

    else if (m_PickPointStates == PICK_POINT_EDIT)
    {
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton->setText(tr2("屏幕取点"));
        m_PickPointStates = PICK_AS_POINT_NORMAL;
    }
}

void CombatZoneEditDialog::on_pushButton_2_clicked()
{
    CZoneInfo stInfo;
    stInfo.SetID(ui->lineEdit_CombatZone_ID->text().toStdString());
    stInfo.SetName(ui->lineEdit_CombatZone_Name->text().toStdString());
    stInfo.SetCreateTime(ui->dateTimeEdit->text().toStdString());
    stInfo.SetDescribe(ui->textEdit_CombatZone_Describe->toPlainText().toStdString());
    stInfo.SetMarkID(ui->comboBox_Mark->currentText().toStdString());
    stInfo.SetAttribute(stInfo.GetAttributeENum(ui->comboBox_CombatZone_Attitude->currentIndex()));
    stInfo.SetShape(stInfo.GetShapeENum(ui->comboBox_CombatZone_Type->currentIndex()));

    if (stInfo.GetShape() == EZoneShape_Circle)
    {
        if (ui->lineEdit_34->text() == "" ||
                ui->lineEdit_35->text() == "" ||
                ui->lineEdit_31->text() == "" ||
                ui->lineEdit_32->text() == "" ||
                ui->lineEdit_33->text() == "")
        {

            LyMessageBox::information(this, tr2("提示"), tr2("请输入区域点"));
            return;
        }

        ZonePointVec stPointVec;
        ZonePoint stPoint;

        stPoint.dLon = ui->lineEdit_31->text().toDouble();
        stPoint.dLat = ui->lineEdit_32->text().toDouble();
        stPoint.dHeight = ui->lineEdit_35->text().toDouble();
        stPoint.dRadius = ui->lineEdit_34->text().toDouble();
        stPointVec.push_back(stPoint);
        stInfo.SetPoints(stPointVec);
    }
    else if (stInfo.GetShape() == EZoneShape_Polygon)
    {
        ZonePointVec stPointVec;

        for (auto itr : m_ZonePointVec)
        {
            stPointVec.push_back(itr);
        }
        stInfo.SetPoints(stPointVec);
    }
    else if (stInfo.GetShape() == EZoneShape_Sector)
    {
        if (ui->leX->text() == "" ||
                ui->leY->text() == "" ||
                ui->leHeight->text() == "" ||
                ui->leRadius->text() == "" ||
                ui->leStartAngle->text() == "" ||
                ui->leEndAngle->text() == "")
        {

            LyMessageBox::information(this, tr2("提示"), tr2("请输入区域点"));
            return;
        }
        ZonePoint stPoint;
        ZonePointVec stPointVec;
        stPoint.dLon = ui->leX->text().toDouble();
        stPoint.dLat = ui->leY->text().toDouble();
        stPoint.dHeight = ui->leHeight->text().toDouble();
        stPoint.dRadius = ui->leRadius->text().toDouble();
        stPoint.dStartAngle = ui->leStartAngle->text().toDouble();
        stPoint.dEndAngle = ui->leEndAngle->text().toDouble();
        stPointVec.push_back(stPoint);
        stInfo.SetPoints(stPointVec);
    }
    bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().SetZone(stInfo);
    if (bresut == true)
    {

    }
    else
    {

        LyMessageBox::information(this, tr2("提示"), tr2("修改失败"));
        return;
    }

    hide();

    LY_MarkManageService::Get().GetDlgZoneList()->UpdataWindowData_Zone();
    LY_MarkManageService::Get().GetDlgZoneList()->UpdateListData_CombatZone();
}

void CombatZoneEditDialog::on_pushButton_3_clicked()
{

    hide();
}

void CombatZoneEditDialog::on_comboBox_CombatZone_Type_currentIndexChanged(int index)
{
    show_GroupBox_CombatZone(index);
}

void CombatZoneEditDialog::on_comboBox_Mark_currentTextChanged(const QString &arg1)
{
    if (ui->comboBox_CombatZone_Type->currentText().compare(tr2("圆柱")) == 0)
    {
        ui->lineEdit_31->setEnabled(false);
        ui->lineEdit_32->setEnabled(false);

        std::string strID = arg1.toStdString();

        if (strID == "-" || strID == "")
        {
            ui->lineEdit_31->setEnabled(true);
            ui->lineEdit_32->setEnabled(true);
            return;
        }

        SystemMarkInfo stInfo;
        stInfo = qnzkna::MarkManage::IMarkManageImpl::Get().GetMark(strID);
        ui->lineEdit_31->setText(QString::number(stInfo.GetLon(), 'f', 7));
        ui->lineEdit_32->setText(QString::number(stInfo.GetLat(), 'f', 7));
    }
    else
    {
        ui->lineEdit_31->setEnabled(true);
        ui->lineEdit_32->setEnabled(true);
    }

}

void CombatZoneEditDialog::on_pushButton_4_clicked()
{
    if (0 == QString::compare(ui->comboBox_CombatZone_Type->currentText(), tr2("圆柱")))
    {
        ui->lineEdit_34->setText("");
        ui->lineEdit_35->setText("");
        ui->lineEdit_31->setText("");
        ui->lineEdit_32->setText("");
        ui->lineEdit_33->setText("");
        m_ZonePointVec.clear();

    }
    else if (0 == QString::compare(ui->comboBox_CombatZone_Type->currentText(), tr2("多边形")))
    {

        int nSel = ui->tableWidget->currentRow();
        if (nSel == -1)
        {

            LyMessageBox::information(this, tr2("提示"), tr2("请选择需要删除的点"));
            return;
        }

        RemoveWindowPoint(nSel);
        UpdateListData(m_ZonePointVec);
    }
    else if (0 == QString::compare(ui->comboBox_CombatZone_Type->currentText(), tr2("扇形")))
    {
        ui->leX->setText("");
        ui->leY->setText("");
        ui->leHeight->setText("");
        ui->leRadius->setText("");
        ui->leStartAngle->setText("");
        ui->leEndAngle->setText("");
    }
    else
    {
        LyMessageBox::information(this, tr2("提示"), tr2("未知图形"));
        return;
    }
}

void CombatZoneEditDialog::on_pushButton_5_clicked()
{
    QString strHeight = ui->lineEdit_CombatZone_CreateTime_2->text();
    double dHeight = strHeight.toDouble();
    if (dHeight <= 0)
    {

        LyMessageBox::information(this, tr2("提示"), tr2("请输入高度"));
        return;
    }

    QMutexLocker locker(&sMutex);
    for (auto &itr : m_ZonePointVec)
    {
        itr.dHeight = dHeight;
    }

    UpdateListData(m_ZonePointVec);
}
