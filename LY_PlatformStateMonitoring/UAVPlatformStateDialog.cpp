#include "UAVPlatformStateDialog.h"
#include "ui_UAVPlatformStateDialog.h"

#include "UAVPlanTeleInformation.h"

UAVPlatformStateDialog::UAVPlatformStateDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVPlatformStateDialog),
    m_show(true)
{
    ui->setupUi(this);

    ui->lineEdit_Yaw->hide();
    ui->label->hide();

    m_PalNone.setColor(QPalette::Window, Qt::transparent);
    m_PalRed.setColor(QPalette::Window, Qt::red);
}

UAVPlatformStateDialog::~UAVPlatformStateDialog()
{
    delete ui;
}

void UAVPlatformStateDialog::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    switch (event->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UAVPlatformStateDialog::showEvent(QShowEvent *event)
{
    m_show = true;
}

void UAVPlatformStateDialog::hideEvent(QHideEvent *event)
{
    m_show = false;
}

void UAVPlatformStateDialog::updateTelemetryInformation(const QString& strWeaponComponentName, const QString &strWeaponComponentID, const QString &strWeaponComponentcontrolMode, const telemetryInformation& telemetryInfo)
{
    if (!m_show)
    {
        return ;
    }

    ui->widget_UAVPlaneAttitude->updateTelemetryInformation(strWeaponComponentName, strWeaponComponentID, telemetryInfo);
    ui->label_WeaponComponentID->setText(strWeaponComponentName);
    ui->label_51->setText(strWeaponComponentcontrolMode);
    ui->label_50->setText(QString(EFlyPhaseToString(EFlyPhase(telemetryInfo.flyPeriod))));
    ui->label_52->setText(QString::number(telemetryInfo.flyTime));
    ui->label_49->setText(QString::number(telemetryInfo.flyDistance));

    ui->label_24->setText(QString(ELockStateToString(telemetryInfo.lockStatus)));
    ui->label_25->setText(QString::number(telemetryInfo.angleofNose, 'f', 2));
    ui->label_26->setText(QString::number(telemetryInfo.fkTemperature));
    ui->label_27->setText(QString::number(telemetryInfo.communicationLostTime));
    if(telemetryInfo.communicationLostTime >= 50 ){

        ui->label_27->setAutoFillBackground(true);
        ui->label_27->setPalette(m_PalRed);
    }else{

        ui->label_27->setAutoFillBackground(true);
        ui->label_27->setPalette(m_PalNone);
    }

    if((telemetryInfo._status & NebulaUAVState_INS)){
        ui->label_35->setText(UAV_NORMAL_STATE_STR);

        ui->label_35->setAutoFillBackground(true);
        ui->label_35->setPalette(m_PalNone);
    }else{
        ui->label_35->setText(UAV_WARING_STATE_STR);

        ui->label_35->setAutoFillBackground(true);
        ui->label_35->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_Emergency)){
        ui->label_36->setText(UAV_NORMAL_STATE_STR);

        ui->label_36->setAutoFillBackground(true);
        ui->label_36->setPalette(m_PalNone);
    }else{
        ui->label_36->setText(UAV_WARING_STATE_STR);

        ui->label_36->setAutoFillBackground(true);
        ui->label_36->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_OverBorder)){
        ui->label_32->setText(UAV_NORMAL_STATE_STR);

        ui->label_32->setAutoFillBackground(true);
        ui->label_32->setPalette(m_PalNone);
    }else{
        ui->label_32->setText(UAV_WARING_STATE_STR);

        ui->label_32->setAutoFillBackground(true);
        ui->label_32->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_GPS)){
        ui->label_28->setText(UAV_NORMAL_STATE_STR);

        ui->label_28->setAutoFillBackground(true);
        ui->label_28->setPalette(m_PalNone);
    }else{
        ui->label_28->setText(UAV_WARING_STATE_STR);

        ui->label_28->setAutoFillBackground(true);
        ui->label_28->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_Sensor)){
        ui->label_34->setText(UAV_NORMAL_STATE_STR);

        ui->label_34->setAutoFillBackground(true);
        ui->label_34->setPalette(m_PalNone);
    }else{
        ui->label_34->setText(UAV_WARING_STATE_STR);

        ui->label_34->setAutoFillBackground(true);
        ui->label_34->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_Power)){
        ui->label_33->setText(UAV_NORMAL_STATE_STR);

        ui->label_33->setAutoFillBackground(true);
        ui->label_33->setPalette(m_PalNone);
    }else{
        ui->label_33->setText(UAV_WARING_STATE_STR);

        ui->label_33->setAutoFillBackground(true);
        ui->label_33->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_MC)){
        ui->label_30->setText(UAV_NORMAL_STATE_STR);

        ui->label_30->setAutoFillBackground(true);
        ui->label_30->setPalette(m_PalNone);
    }else{
        ui->label_30->setText(UAV_WARING_STATE_STR);

        ui->label_30->setAutoFillBackground(true);
        ui->label_30->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_RC)){
        ui->label_29->setText(UAV_NORMAL_STATE_STR);

        ui->label_29->setAutoFillBackground(true);
        ui->label_29->setPalette(m_PalNone);
    }else{
        ui->label_29->setText(UAV_WARING_STATE_STR);

        ui->label_29->setAutoFillBackground(true);
        ui->label_29->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_RotationRate)){
        ui->label_37->setText(UAV_NORMAL_STATE_STR);

        ui->label_37->setAutoFillBackground(true);
        ui->label_37->setPalette(m_PalNone);
    }else{
        ui->label_37->setText(UAV_WARING_STATE_STR);

        ui->label_37->setAutoFillBackground(true);
        ui->label_37->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_Ultrasound)){
        ui->label_38->setText(UAV_NORMAL_STATE_STR);

        ui->label_38->setAutoFillBackground(true);
        ui->label_38->setPalette(m_PalNone);
    }else{
        ui->label_38->setText(UAV_WARING_STATE_STR);

        ui->label_38->setAutoFillBackground(true);
        ui->label_38->setPalette(m_PalRed);
    }

    if((telemetryInfo._status & NebulaUAVState_SatelliteDirection)){
        ui->label_39->setText(UAV_NORMAL_STATE_STR);

        ui->label_39->setAutoFillBackground(true);
        ui->label_39->setPalette(m_PalNone);
    }else{
        ui->label_39->setText(UAV_WARING_STATE_STR);

        ui->label_39->setAutoFillBackground(true);
        ui->label_39->setPalette(m_PalRed);
    }

    ui->label_40->setText(QString(EDiffStateToString(EDiffState(telemetryInfo.differenceStatus))));
    ui->label_41->setText(QString::number(telemetryInfo.fkvoltage,'f',1));
    ui->label_44->setText(QString::number(telemetryInfo.powervoltage,'f',1));

    QString cstrTxt;
    cstrTxt.sprintf("%.0f%%", telemetryInfo._throttle * 10);
    ui->label_42->setText(cstrTxt);

    ui->lineEdit_GPS_Time->setText(QString(QString::number(telemetryInfo._year) + "-" + QString::number(telemetryInfo._month) + "-" + QString::number(telemetryInfo._day) + " " + QString::number(telemetryInfo._hour) + ":" + QString::number(telemetryInfo._min) + ":" + QString::number(telemetryInfo._sec)));
    ui->lineEditLng->setText(QString::number(telemetryInfo.position.lng, 'f', 6));
    ui->lineEdit_LandHeight->setText(QString::number(telemetryInfo.relativeOriginHeigth));
    ui->lineEdit_Lat->setText(QString::number(telemetryInfo.position.lat, 'f', 6));
    ui->lineEdit_LandSpeed->setText(QString::number(telemetryInfo.groundVelocity, 'f', 2));
    ui->lineEdit_GpsHeight->setText(QString::number(telemetryInfo.gpsHeigth * 10.f, 'f', 2));
    ui->lineEdit_Hdop->setText(QString::number(telemetryInfo.levelPrecision, 'f', 2));
    ui->lineEdit_LoseStar->setText(QString::number(telemetryInfo.gpsLostTime));
    ui->lineEdit_Vdop->setText(QString::number(telemetryInfo.verticalPrecision, 'f', 2));
    ui->lineEdit_StarCount->setText(QString::number(telemetryInfo.gpsNumber));
    ui->lineEdit_Sdop->setText(QString::number(telemetryInfo.velocityPrecision, 'f', 2));
    ui->lineEdit_RollTurn->setText(QString::number(telemetryInfo.angleofRoll, 'f', 2));
    ui->lineEdit_Pitch->setText(QString::number(telemetryInfo.angleofPitch, 'f', 2));
    ui->lineEdit_Yaw->setText(QString::number(telemetryInfo.angleofRudder, 'f', 2));
    ui->lineEdit_FlyPoint->setText(QString::number(telemetryInfo.angleofNose, 'f', 2));
}

void UAVPlatformStateDialog::updateAttitude()
{
    ui->widget_UAVPlaneAttitude->updateAttitude();
}
