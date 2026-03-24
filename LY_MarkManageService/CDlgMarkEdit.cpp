#include "CDlgMarkEdit.h"
#include "ui_CDlgMarkEdit.h"
#include "QCString.h"
#include "IMarkManageImpl.h"
#include "lyUI/LyMessageBox.h"
#include "LY_MarkManageService.h"

#include <time.h>
#include "QRegExpExample.h"
#include <QDateTime>

CDlgMarkEdit::CDlgMarkEdit(QWidget *parent) :
    LyDialog(tr("EditMark"), parent),
    ui(new Ui::CDlgMarkEdit)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(480, 740);

    InitData();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(on_pushButton_clicked()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(on_pushButton_2_clicked()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(on_pushButton_3_clicked()));

    connect(this, &CDlgMarkEdit::SetWindowInfoSig, this, &CDlgMarkEdit::SetWindowInfoSlot, Qt::QueuedConnection);

	ui->lineEdit->setValidator(new QRegExpValidator(regNumStrUnderLine));
    ui->lineEdit_4->setValidator(new QRegExpValidator(regNum));
    ui->lineEdit_5->setValidator(new QRegExpValidator(regNum));
    ui->lineEdit_7->setValidator(new QRegExpValidator(allNumber));
    ui->lineEdit_13->setValidator(new QRegExpValidator(allNumber));
    ui->lineEdit_14->setValidator(new QRegExpValidator(allNumber));
    ui->lineEdit_6->setValidator(new QRegExpValidator(regLongitude));
    ui->lineEdit_9->setValidator(new QRegExpValidator(regLatitude));
    ui->lineEdit_8->setValidator(new QRegExpValidator(allNumber));
    ui->lineEdit_11->setValidator(new QRegExpValidator(allNumber));
    ui->lineEdit_10->setValidator(new QRegExpValidator(regNum));
    ui->lineEdit_7->setValidator(new QRegExpValidator(regLongitude));

    this->hide();
}

CDlgMarkEdit::~CDlgMarkEdit()
{
    delete ui;
}

void CDlgMarkEdit::SetWindowInfoSlot(const XygsZC_Mark_01 xygsZC_Mark_01)
{
    const std::string &strCurrentDateTime = QStringLiteral("%1 %2 %3").arg(xygsZC_Mark_01.Z).arg(xygsZC_Mark_01.P).arg(QDateTime::fromMSecsSinceEpoch(xygsZC_Mark_01.targetTime).toString("HH:mm:ss zzz")).toStdString();
    SystemMarkInfo systemMarkInfo = qnzkna::MarkManage::IMarkManageImpl::Get().GetMark(strCurrentDateTime);
    if (systemMarkInfo.GetID().empty())
    {
        SetDataType(3);
        SystemMarkInfo data;

        data.SetID(strCurrentDateTime);
        data.SetName(strCurrentDateTime);
        data.SetLon(xygsZC_Mark_01.target_lon);
        data.SetLat(xygsZC_Mark_01.target_lat);
        data.SetHeight(xygsZC_Mark_01.target_alt);
        data.SetType((ESystemMarkType)(xygsZC_Mark_01.targetType));
        if (data.GetID() == m_SystemMarkInfo.GetID())
        {
            m_SystemMarkInfo = data;
            ui->lineEdit_6->setText(QString::number(data.GetLon(), 'f', 7));
            ui->lineEdit_9->setText(QString::number(data.GetLat(), 'f', 7));
            ui->lineEdit_8->setText(QString::number(data.GetHeight(), 'f', 2));
            ui->lineEdit_10->setText(QString::number(data.GetSpeed(), 'f', 2));
            ui->lineEdit_11->setText(QString::number(data.GetPitch(), 'f', 2));

            QString vehicleSensorDirections("");
            vehicleSensorDirections += QStringLiteral("T:") + QDateTime::fromMSecsSinceEpoch(xygsZC_Mark_01.targetTime).toString("yyyy MM dd HH:mm:ss zzz");

            {
                vehicleSensorDirections += QStringLiteral("\nV P:") + QString::number(xygsZC_Mark_01.vehiclePitch, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",R:") + QString::number(xygsZC_Mark_01.vehicleRoll, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",Y:") + QString::number(xygsZC_Mark_01.vehicleYaw, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",Lng:") + QString::number(xygsZC_Mark_01.vehicleLon, 'f', 7);
                vehicleSensorDirections += QStringLiteral(",Lat:") + QString::number(xygsZC_Mark_01.vehicleLat, 'f', 7);
                vehicleSensorDirections += QStringLiteral(",Alt:") + QString::number(xygsZC_Mark_01.vehicleAlt, 'f', 2);
            }

            {
                vehicleSensorDirections += QStringLiteral("\nS P:") + QString::number(xygsZC_Mark_01.sensorPitch, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",R:") + QString::number(xygsZC_Mark_01.sensorRoll, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",Y:") + QString::number(xygsZC_Mark_01.sensorYaw, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",Dist:") + QString::number(xygsZC_Mark_01.laserDistance, 'f', 2);
            }

            ui->textEdit->setText(vehicleSensorDirections);
        }
        else
        {
            QString vehicleSensorDirections("");
            vehicleSensorDirections += QStringLiteral("T:") + QDateTime::fromMSecsSinceEpoch(xygsZC_Mark_01.targetTime).toString("yyyy MM dd HH:mm:ss zzz");

            {
                vehicleSensorDirections += QStringLiteral("\nV P:") + QString::number(xygsZC_Mark_01.vehiclePitch, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",R:") + QString::number(xygsZC_Mark_01.vehicleRoll, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",Y:") + QString::number(xygsZC_Mark_01.vehicleYaw, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",Lng:") + QString::number(xygsZC_Mark_01.vehicleLon, 'f', 7);
                vehicleSensorDirections += QStringLiteral(",Lat:") + QString::number(xygsZC_Mark_01.vehicleLat, 'f', 7);
                vehicleSensorDirections += QStringLiteral(",Alt:") + QString::number(xygsZC_Mark_01.vehicleAlt, 'f', 2);
            }

            {
                vehicleSensorDirections += QStringLiteral("\nS P:") + QString::number(xygsZC_Mark_01.sensorPitch, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",R:") + QString::number(xygsZC_Mark_01.sensorRoll, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",Y:") + QString::number(xygsZC_Mark_01.sensorYaw, 'f', 2);
                vehicleSensorDirections += QStringLiteral(",Dist:") + QString::number(xygsZC_Mark_01.laserDistance, 'f', 2);
            }

            data.SetDirections(vehicleSensorDirections.toStdString());

            SetWindowInfo(data);
        }

        if (this->isHidden())
        {
            show();
        }
    }
    else
    {
        const QByteArray &ba = QUuid::createUuid().toRfc4122();
        systemMarkInfo.SetUuid(ba.data());
        systemMarkInfo.SetLon(xygsZC_Mark_01.target_lon);
        systemMarkInfo.SetLat(xygsZC_Mark_01.target_lat);
        systemMarkInfo.SetHeight(xygsZC_Mark_01.target_alt);

        QString vehicleSensorDirections("");
        vehicleSensorDirections += QStringLiteral("T:") + QDateTime::fromMSecsSinceEpoch(xygsZC_Mark_01.targetTime).toString("yyyy MM dd HH:mm:ss zzz");

        {
            vehicleSensorDirections += QStringLiteral("\nV P:") + QString::number(xygsZC_Mark_01.vehiclePitch, 'f', 2);
            vehicleSensorDirections += QStringLiteral(",R:") + QString::number(xygsZC_Mark_01.vehicleRoll, 'f', 2);
            vehicleSensorDirections += QStringLiteral(",Y:") + QString::number(xygsZC_Mark_01.vehicleYaw, 'f', 2);
            vehicleSensorDirections += QStringLiteral(",Lng:") + QString::number(xygsZC_Mark_01.vehicleLon, 'f', 7);
            vehicleSensorDirections += QStringLiteral(",Lat:") + QString::number(xygsZC_Mark_01.vehicleLat, 'f', 7);
            vehicleSensorDirections += QStringLiteral(",Alt:") + QString::number(xygsZC_Mark_01.vehicleAlt, 'f', 2);
        }

        {
            vehicleSensorDirections += QStringLiteral("\nS P:") + QString::number(xygsZC_Mark_01.sensorPitch, 'f', 2);
            vehicleSensorDirections += QStringLiteral(",R:") + QString::number(xygsZC_Mark_01.sensorRoll, 'f', 2);
            vehicleSensorDirections += QStringLiteral(",Y:") + QString::number(xygsZC_Mark_01.sensorYaw, 'f', 2);
            vehicleSensorDirections += QStringLiteral(",Dist:") + QString::number(xygsZC_Mark_01.laserDistance, 'f', 2);
        }

        systemMarkInfo.SetDirections(vehicleSensorDirections.toStdString());

        qnzkna::MarkManage::IMarkManageImpl::Get().SetMovingTaregt(systemMarkInfo);
    }
}

void CDlgMarkEdit::InitData()
{
    QStringList list01, list02, list03, list04, list05, list06;
    list01 << tr2("未知") << tr2("飞机") << tr2("生物") << tr2("车辆")<<tr2("建筑") << tr2("碉堡") << tr2("工事") << tr2("工厂") << tr2("阵地") << tr2("仓库") << tr2("交通设施") << tr2("普通火");
    list02 << tr2("未知") << tr2("严重危险") << tr2("危险") << tr2("紧急") << tr2("一般") << tr2("普通");
    list03 << tr2("未知") << tr2("金属材料") << tr2("有机合成材料") << tr2("玻璃-陶瓷类") << tr2("钢筋混凝土") << tr2("砖石") << tr2("土方") << tr2("生物体") << tr2("未识别");
    list04 << tr2("未知 ") << tr2("打击") << tr2("监测") << tr2("防护");
    list05 << tr("1") << tr("2") << tr("3") << tr("4") << tr("5") << tr2("未知");
    list06 << tr("M1") << tr("M2") << tr("M3") << tr("M4") << tr("M5") << tr2("未知");

    ui->comboBox->addItems(list01);
    ui->comboBox_2->addItems(list02);
    ui->comboBox_3->addItems(list03);
    ui->comboBox_4->addItems(list04);
    ui->comboBox_5->addItems(list05);
    ui->comboBox_6->addItems(list06);

    ui->lineEdit_6->setText(tr("0.0"));
    ui->lineEdit_9->setText(tr("0.0"));
    ui->lineEdit_8->setText(tr("0.0"));
    ui->lineEdit_10->setText(tr("0.0"));
    ui->lineEdit_11->setText(tr("0.0"));
    ui->textEdit->setText(tr("-"));
    ui->lineEdit_7->setText(tr("0.0"));
    ui->lineEdit_13->setText(tr("0.0"));
    ui->lineEdit_14->setText(tr("0.0"));
    ui->lineEdit_3->setText(tr("-"));
    ui->lineEdit_4->setText(tr("-"));
    ui->lineEdit_5->setText(tr("-"));

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    m_DataType = 0;

    m_PickPointStates = PICK_POINT_NORMAL;
}

void CDlgMarkEdit::InitWindowCtrl()
{
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_6->setText(tr("0.0"));
    ui->lineEdit_9->setText(tr("0.0"));
    ui->lineEdit_8->setText(tr("0.0"));
    ui->lineEdit_10->setText(tr("0.0"));
    ui->lineEdit_11->setText(tr("0.0"));
    ui->textEdit->setText(tr("-"));
    ui->lineEdit_7->setText(tr("0.0"));
    ui->lineEdit_13->setText(tr("0.0"));
    ui->lineEdit_14->setText(tr("0.0"));
    ui->lineEdit_3->setText(tr("-"));
    ui->lineEdit_4->setText(tr("-"));
    ui->lineEdit_5->setText(tr("-"));

    ui->comboBox->setCurrentIndex(11);
    ui->comboBox_2->setCurrentIndex(1);
    ui->comboBox_3->setCurrentIndex(0);
	ui->comboBox_4->setCurrentIndex(0);
    ui->comboBox_5->setCurrentIndex(5);
    ui->comboBox_6->setCurrentIndex(8);
    InitTimeEdit();

}

void CDlgMarkEdit::SetWindowInfo(SystemMarkInfo &stInfo)
{
    m_SystemMarkInfo = stInfo;
    ui->lineEdit->setText(QString::fromStdString(stInfo.GetID()));
    ui->lineEdit_2->setText(QString::fromStdString(stInfo.GetName()));
    ui->comboBox->setCurrentIndex(stInfo.GetTypeENum(stInfo.GetType()));
    ui->comboBox_2->setCurrentIndex(stInfo.GetAttributeENum(stInfo.GetAttribute()));
    ui->comboBox_3->setCurrentIndex(stInfo.GetMaterialENum(stInfo.GetMaterial()));
    ui->lineEdit_6->setText(QString::number(stInfo.GetLon(), 'f', 7));
    ui->lineEdit_9->setText(QString::number(stInfo.GetLat(), 'f', 7));
    ui->lineEdit_8->setText(QString::number(stInfo.GetHeight(), 'f', 2));
    ui->lineEdit_10->setText(QString::number(stInfo.GetSpeed(), 'f', 2));
    ui->lineEdit_11->setText(QString::number(stInfo.GetPitch(), 'f', 2));
    ui->textEdit->setText(QString::fromStdString(stInfo.GetDirections()));
    ui->comboBox_4->setCurrentIndex(stInfo.GetMissionTypeENum(stInfo.GetMissionType()));
    ui->comboBox_5->setCurrentIndex(stInfo.GetThreatenDegreeENum(stInfo.GetThreatenDegree()) - 1);
    ui->lineEdit_7->setText(QString::number(stInfo.GetSizeX(), 'f', 6));
    ui->lineEdit_13->setText(QString::number(stInfo.GetSizeY(), 'f', 6));
    ui->lineEdit_14->setText(QString::number(stInfo.GetSizeZ(), 'f', 6));
    ui->lineEdit_3->setText(QString::fromStdString(stInfo.GetDetectRequest()));

    InitTimeEdit();
    int nSel = ui->comboBox_6->findText(QString::fromStdString(stInfo.GetFindEqu()));
    if (nSel == -1)
    {
        ui->comboBox_6->setCurrentIndex(8);
    }
    else
    {
        ui->comboBox_6->setCurrentIndex(nSel);
    }
    if (m_DataType == 2)
    {

    }
}

void CDlgMarkEdit::ShowTop()
{
    this->show();
}

void CDlgMarkEdit::SetDataType(int nType)
{
    m_DataType = nType;
}

int CDlgMarkEdit::GetPickPointStates()
{
    return m_PickPointStates;
}

void CDlgMarkEdit::SetPostion(double &dLon, double &dLat, double &dHeight)
{
    ui->lineEdit_6->setText(QString::number(dLon, 'f', 7));
    ui->lineEdit_9->setText(QString::number(dLat, 'f', 7));

}

void CDlgMarkEdit::InitTimeEdit()
{
    QDateTime dt = QDateTime::currentDateTime();
    ui->lineEdit_12->setText(dt.toString("yyyy-MM-dd HH:mm:ss"));
}

void CDlgMarkEdit::UpdateMissionCombobox()
{

}

void CDlgMarkEdit::showEvent(QShowEvent *event)
{
    UpdateMissionCombobox();

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

void CDlgMarkEdit::closeEvent(QCloseEvent *event)
{

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

void CDlgMarkEdit::on_pushButton_clicked()
{
    QString strBTN = ui->pushButton->text();

    if (m_PickPointStates == PICK_POINT_NORMAL)
    {
        ui->pushButton_2->setEnabled(false);
        ui->pushButton_3->setEnabled(false);
        ui->pushButton->setText(tr2("结束取点"));
        m_PickPointStates = PICK_POINT_EDIT;
    }

    else if (m_PickPointStates == PICK_POINT_EDIT)
    {
        ui->pushButton_2->setEnabled(true);
        ui->pushButton_3->setEnabled(true);
        ui->pushButton->setText(tr2("屏幕取点"));
        m_PickPointStates = PICK_POINT_NORMAL;
    }
}

void CDlgMarkEdit::on_pushButton_2_clicked()
{
    if (ui->lineEdit->text() == "")
    {

        LyMessageBox::information(NULL, tr2("提示"), tr2("目标编码不能为空，请输入目标编码"));

		setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        return;
    }
    SystemMarkInfo stInfo;

    stInfo.SetID(ui->lineEdit->text().toStdString());
    stInfo.SetName(ui->lineEdit_2->text().toStdString());
    stInfo.SetType(stInfo.GetTypeENum(ui->comboBox->currentIndex()));
    stInfo.SetAttribute(stInfo.GetAttributeENum(ui->comboBox_2->currentIndex()));
    stInfo.SetMaterial(stInfo.GetMaterialENum(ui->comboBox_3->currentIndex()));
    stInfo.SetLon(ui->lineEdit_6->text().toDouble());
    stInfo.SetLat(ui->lineEdit_9->text().toDouble());
    stInfo.SetHeight(ui->lineEdit_8->text().toDouble());
    stInfo.SetSpeed(ui->lineEdit_10->text().toDouble());
    stInfo.SetPitch(ui->lineEdit_11->text().toDouble());
    stInfo.SetDirections(ui->textEdit->toPlainText().toStdString());
    stInfo.SetMissionType(stInfo.GetMissionTypeENum(ui->comboBox_4->currentIndex()));
    stInfo.SetThreatenDegree(stInfo.GetThreatenDegreeENum(ui->comboBox_5->currentIndex() + 1));
    stInfo.SetFindTime(ui->lineEdit_12->text().toStdString());
    stInfo.SetFindEqu(ui->comboBox_6->currentText().toStdString());
    stInfo.SetSize(ui->lineEdit_7->text().toDouble(), ui->lineEdit_13->text().toDouble(), ui->lineEdit_14->text().toDouble());
    stInfo.SetDetectRequest(ui->lineEdit_3->text().toStdString());

    if (m_DataType == 1)
    {
        bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().ISExisted(stInfo.GetID());
        if (bresut == true)
        {

            LyMessageBox::information(NULL, tr2("提示"), tr2("目标编码重复，请修改新的目标编码"));
            return;
        }

        bresut = qnzkna::MarkManage::IMarkManageImpl::Get().SetTaregt(stInfo);
        if (bresut == true)
        {

        }
        else
        {

            LyMessageBox::information(NULL, tr2("提示"), tr2("添加失败"));
        }
        m_DataType = 0;
    }

    else if (m_DataType == 3)
    {
        bool bresut = qnzkna::MarkManage::IMarkManageImpl::Get().ISExisted(stInfo.GetID());
        if (bresut == true)
        {

            LyMessageBox::information(NULL, tr2("提示"), tr2("目标编码重复，请修改新的目标编码"));
            return;
        }
        bresut = qnzkna::MarkManage::IMarkManageImpl::Get().SetMovingTaregt(stInfo);
        if (bresut == true)
        {

        }
        else
        {

            LyMessageBox::information(NULL, tr2("提示"), tr2("添加失败"));
        }
        m_DataType = 0;
    }

    else if (m_DataType == 2)
    {
        bool bresut = false;
        if (0 == strcmp(m_SystemMarkInfo.GetID().c_str(), stInfo.GetID().c_str()))
        {
            bresut = qnzkna::MarkManage::IMarkManageImpl::Get().SetTaregt(stInfo);
        }
        else
        {
            bresut = qnzkna::MarkManage::IMarkManageImpl::Get().SetTaregt(stInfo, m_SystemMarkInfo.GetID());
        }
        if (bresut == true)
        {

        }
        else
        {

            LyMessageBox::information(NULL, tr2("提示"), tr2("添加失败"));
        }
        m_DataType = 0;
    }
    this->hide();

    LY_MarkManageService::Get().GetDlgMarkList()->UpdateDataByDB();
}

void CDlgMarkEdit::on_pushButton_3_clicked()
{

	setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
    this->hide();
}
