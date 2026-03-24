#include "CDlgAddMaintainMark.h"
#include "CDlgMarkMaintain.h"
#include "QCString.h"
#include "ITaskManagerActivator.h"
#include "LyMessageBox.h"
#include "lY_TaskManage.h"
#include "QRegExpExample.h"

CDlgAddMaintainMark::CDlgAddMaintainMark(QWidget *parent)
    : LyDialog(tr2("添加目标"), parent),
      ui(new Ui::CDlgAddMaintainMark)
{
    ui->setupUi(centralWidget());
    this->setResizable(true);
    this->resize(500, 600);
    connect(ui->btnGetPoint, &QPushButton::clicked, this, &CDlgAddMaintainMark::on_btnGetPoint_clicked);
    connect(ui->btnConfirm, &QPushButton::clicked, this, &CDlgAddMaintainMark::on_btnConfirm_clicked);
    connect(ui->btnCancel, &QPushButton::clicked, this, &CDlgAddMaintainMark::on_btnCancel_clicked);

    ui->lineEdit_15->setValidator(new QRegExpValidator(regScenarioID));
    ui->leHeight->setValidator(new QRegExpValidator(regNum));
    ui->leHeight2->setValidator(new QRegExpValidator(regNum));
    ui->leWide->setValidator(new QRegExpValidator(regNum));
    ui->leLon->setValidator(new QRegExpValidator(regLongitude));
    ui->leLat->setValidator(new QRegExpValidator(regLatitude));
    ui->leSpeed->setValidator(new QRegExpValidator(regNum));
    ui->leDirection->setValidator(new QRegExpValidator(regNum));

}

CDlgAddMaintainMark::~CDlgAddMaintainMark()
{
}

void CDlgAddMaintainMark::Init(int index, string strID)
{
    InitData();
    if (index)
    {
        SetMarkBaseNature(strID);
    }
    m_DataType = index;
}

void CDlgAddMaintainMark::InitData()
{
	QStringList list01, list02, list03, list04, list05, list06;
	list01 << tr2("未知") << tr2("无人机") << tr2("人员") << tr2("车辆") << tr2("建筑物");
	list02 << tr2("未知") << tr2("敌") << tr2("我") << tr2("友") << tr2("中立") << tr2("民航");
	list03 << tr2("不明") << tr2("金属材料") << tr2("有机合成材料") << tr2("玻璃-陶瓷类") << tr2("钢筋混凝土") << tr2("砖石") << tr2("土方") << tr2("生命体") << tr2("未识别");
	list04 << tr2("未分配") << tr2("打击任务") << tr2("侦察任务") << tr2("保护任务");
	list05 << tr2("1") << tr2("2") << tr2("3") << tr2("4") << tr2("5") << tr2("未指定");
	list06 << tr2("A1") << tr2("A2") << tr2("A3") << tr2("A4") << tr2("A5") << tr2("B1") << tr2("B2") << tr2("B3") << tr2("其他");

	ui->cbxType->addItems(list01);
    ui->cbxNature->addItems(list02);
    ui->cbxQuality->addItems(list03);
    ui->cbxTaskType->addItems(list04);
    ui->cbxThreaten->addItems(list05);
    ui->cbxEquip->addItems(list06);

    ui->lineEdit_15->setText("");
    ui->lineEdit_16->setText("");
    ui->cbxType->setCurrentIndex(0);
    ui->cbxNature->setCurrentIndex(0);
    ui->cbxTaskType->setCurrentIndex(0);
    ui->cbxQuality->setCurrentIndex(0);
    ui->leLength->setText(tr("0.0"));
    ui->leWide->setText(tr("0.0"));
    ui->leHeight->setText(tr("0.0"));
    ui->leLon->setText(tr("0.0"));
    ui->leLat->setText(tr("0.0"));
    ui->leTaskNum->setText(tr("-"));
    ui->leHeight2->setText(tr("0.0"));
    ui->leSpeed->setText(tr("0.0"));
    ui->leDirection->setText(tr("0.0"));
    ui->leHureRequire->setText(tr("-"));
    ui->tedDescribe->setText(tr("-"));
    ui->lineEdit_27->setText("");
    ui->cbxEquip->setCurrentIndex(0);
    ui->cbxThreaten->setCurrentIndex(0);
    ui->tedDescribe->setText("");
    m_DataType = 0;
}

void CDlgAddMaintainMark::SetMarkBaseNature(string strID)
{
    auto MarkListVec = ITaskManagerActivator::Get().getPIMarkManageService()->GetMarkList();
    for (auto target : MarkListVec)
    {
        if (strID != target.GetID())
        {
            continue;
        }
        ui->lineEdit_15->setText(QString::fromStdString(target.GetID()));
        ui->lineEdit_16->setText(QString::fromStdString(target.GetName()));
        ui->cbxType->setCurrentIndex(static_cast<int>(target.GetType()));
        ui->cbxNature->setCurrentIndex(static_cast<int>(target.GetAttribute()));
        ui->cbxTaskType->setCurrentIndex(static_cast<int>(target.GetMissionType()));
        ui->cbxQuality->setCurrentIndex(static_cast<int>(target.GetMaterial()));
        ui->leLength->setText(QString::number(target.GetSizeX(), 'f', 4));
        ui->leWide->setText(QString::number(target.GetSizeY(), 'f', 4));
        ui->leHeight->setText(QString::number(target.GetSizeZ(), 'f', 4));
        ui->leLon->setText(QString::number(target.GetLon(), 'f', 6));
        ui->leLat->setText(QString::number(target.GetLat(), 'f', 6));
        ui->leHeight2->setText(QString::number(target.GetHeight(), 'f', 6));
        ui->leSpeed->setText(QString::number(target.GetSpeed(), 'f', 4));
        ui->leDirection->setText(QString::number(target.GetPitch(), 'f', 4));
        ui->lineEdit_27->setText(QString::fromStdString(target.GetFindTime()));
        ui->cbxEquip->setCurrentText(QString::fromStdString(target.GetFindEqu()));
        ui->leHureRequire->setText(QString::fromStdString(target.GetDetectRequest()));
        ui->cbxThreaten->setCurrentIndex(static_cast<int>(target.GetThreatenDegree()));
        ui->tedDescribe->setText(QString::fromStdString(target.GetDirections()));
        break;
    }
}

void CDlgAddMaintainMark::SetMarkLocation(double lon, double lat)
{
    ui->leLon->setText(QString::number(lon, 'f', 6));
    ui->leLat->setText(QString::number(lat, 'f', 6));
}

void CDlgAddMaintainMark::on_btnGetPoint_clicked()
{
    bScreenPoint = !bScreenPoint;
}

void CDlgAddMaintainMark::on_btnConfirm_clicked()
{
    if (ui->lineEdit_15->text() == "")
    {
        LyMessageBox::information(NULL, tr2("提示"), tr2("目标编码不能为空,请重新输入"));
        return;
    }
    SystemMarkInfo data;
    data.SetID(ui->lineEdit_15->text().toStdString());
    data.SetName(ui->lineEdit_16->text().toStdString());
    data.SetType(data.GetTypeENum(ui->cbxType->currentIndex()));
    data.SetAttribute(data.GetAttributeENum(ui->cbxNature->currentIndex()));
    data.SetMissionType(data.GetMissionTypeENum(ui->cbxTaskType->currentIndex()));
    data.SetMaterial(data.GetMaterialENum(ui->cbxQuality->currentIndex()));
    data.SetLon(ui->leLon->text().toDouble());
    data.SetLat(ui->leLat->text().toDouble());
    data.SetHeight(ui->leHeight2->text().toDouble());
    data.SetSpeed(ui->leSpeed->text().toDouble());
    data.SetPitch(ui->leDirection->text().toDouble());
    data.SetDirections(ui->tedDescribe->toPlainText().toStdString());
    data.SetThreatenDegree(data.GetThreatenDegreeENum(ui->cbxThreaten->currentIndex() + 1));
    data.SetFindTime(ui->lineEdit_27->text().toStdString());
    data.SetFindEqu(ui->cbxEquip->currentText().toStdString());
    data.SetSize(ui->leLength->text().toDouble(), ui->leWide->text().toDouble(), ui->leHeight->text().toDouble());
    data.SetDetectRequest(ui->leHureRequire->text().toStdString());
    bool bsucc = ITaskManagerActivator::Get().getPIMarkManageService()->SetTaregt(data);
    if (bsucc)
    {
        close();
    }
    else
    {
        LyMessageBox::information(NULL, tr2("提示"), tr2("添加目标失败！"));
        return;
    }
    LY_TaskManage::Get().GetDlgMaintainMark()->Init();
}

void CDlgAddMaintainMark::on_btnCancel_clicked()
{
    close();
}
