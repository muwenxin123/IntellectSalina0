#include "UAVPlaneStatesDialog.h"
#include "ui_UAVPlaneStatesDialog.h"

#include "UAVPlanTeleInformation.h"

UAVPlaneStatesDialog::UAVPlaneStatesDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVPlaneStatesDialog),
    m_show(true)
{
    ui->setupUi(this);
    initData();
}

UAVPlaneStatesDialog::~UAVPlaneStatesDialog()
{
    delete ui;
}

void UAVPlaneStatesDialog::changeEvent(QEvent *event)
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

void UAVPlaneStatesDialog::showEvent(QShowEvent *event)
{
    m_show = true;
}

void UAVPlaneStatesDialog::hideEvent(QHideEvent *event)
{
    m_show = false;
}

void UAVPlaneStatesDialog::initData()
{
    ui->UAVPlaneStates_lat_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_lng_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_pressureHeight_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_GPSHeight_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_AirSpeed_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_MarkSpeed_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_LandSpeed_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_SkySpeed_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Pitch_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_rollMedian_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_PointLocation_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_MC_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH1_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH2_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH3_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH4_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH5_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH6_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH7_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH8_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH9_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_CH10_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_FlyVoltage_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_RPM_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_FlyTemperature_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_DifferentStatus_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_GPSStar_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_HDOP_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_LoseStarTime_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_NotNumberOfTurn_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_NumberOfTurn_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_INCH1_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_INCH2_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_INCH3_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_INCH4_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_INCH5_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_INCH6_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_INCH7_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_DistanceHeight_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_MarkPoint_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_MarkHeight_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_MarkDirection_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Status_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Control_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Aileron_IN_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Elevator_IN_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Throttle_IN_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Rudder_IN_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Aileron_OUT_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Elevator_OUT_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Throttle_OUT_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_Rudder_OUT_VALUE->setText(QStringLiteral("--"));
}

void UAVPlaneStatesDialog::updateTelemetryInformation(const QString &strWeaponComponentName, const QString &strWeaponComponentID, const telemetryInformation &telemetryInfo)
{
    if (!m_show)
    {
        return ;
    }
    ui->UAVPlaneStates_UAV_VALUE->setText(strWeaponComponentName);
    ui->UAVPlaneStates_lat_VALUE->setText(QString::number(telemetryInfo.position.lat, 'f', 7));
    ui->UAVPlaneStates_lng_VALUE->setText(QString::number(telemetryInfo.position.lng, 'f', 7));
    ui->UAVPlaneStates_pressureHeight_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_GPSHeight_VALUE->setText(QString::number(telemetryInfo.position.heigth, 'f', 2));
    ui->UAVPlaneStates_AirSpeed_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_MarkSpeed_VALUE->setText(QString::number(telemetryInfo.targetVelocity, 'f', 2));
    ui->UAVPlaneStates_LandSpeed_VALUE->setText(QString::number(telemetryInfo.groundVelocity, 'f', 2));
    ui->UAVPlaneStates_SkySpeed_VALUE->setText(QString::number(telemetryInfo.verticalVelocity, 'f', 2));
    ui->UAVPlaneStates_Pitch_VALUE->setText(QString::number(telemetryInfo.angleofPitch, 'f', 2));
    ui->UAVPlaneStates_rollMedian_VALUE->setText(QString::number(telemetryInfo.angleofRoll, 'f', 2));
    ui->UAVPlaneStates_PointLocation_VALUE->setText(QString::number(telemetryInfo.angleofNose, 'f', 2));

    if ((telemetryInfo._status & NebulaUAVState_MC))
    {
        ui->UAVPlaneStates_MC_VALUE->setText(UAV_NORMAL_STATE_STR);
    }
    else
    {
        ui->UAVPlaneStates_MC_VALUE->setText(UAV_WARING_STATE_STR);
    }

    ui->UAVPlaneStates_CH1_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal01));
    ui->UAVPlaneStates_CH2_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal02));
    ui->UAVPlaneStates_CH3_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal03));
    ui->UAVPlaneStates_CH4_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal04));
    ui->UAVPlaneStates_CH5_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal05));
    ui->UAVPlaneStates_CH6_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal06));
    ui->UAVPlaneStates_CH7_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal07));
    ui->UAVPlaneStates_CH8_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal08));
    ui->UAVPlaneStates_CH9_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal09));
    ui->UAVPlaneStates_CH10_VALUE->setText(QString::number(telemetryInfo._fkOutputChannal10));
    ui->UAVPlaneStates_FlyVoltage_VALUE->setText(QString::number(telemetryInfo.fkvoltage, 'f', 1));
    ui->UAVPlaneStates_RPM_VALUE->setText(QStringLiteral("--"));
    ui->UAVPlaneStates_FlyTemperature_VALUE->setText(QString::number(telemetryInfo.fkTemperature));
    ui->UAVPlaneStates_DifferentStatus_VALUE->setText(EDiffStateToString(EDiffState(telemetryInfo.differenceStatus)));
    ui->UAVPlaneStates_GPSStar_VALUE->setText(QString::number(telemetryInfo.gpsNumber));
    ui->UAVPlaneStates_HDOP_VALUE->setText(QString::number(telemetryInfo.levelPrecision, 'f', 2));
    ui->UAVPlaneStates_LoseStarTime_VALUE->setText(QString::number(telemetryInfo.gpsLostTime));
    ui->UAVPlaneStates_NotNumberOfTurn_VALUE->setText(QString::number(telemetryInfo.unFinishCircleNum));
    ui->UAVPlaneStates_NumberOfTurn_VALUE->setText(QString::number(telemetryInfo.finishCircleNum));
    ui->UAVPlaneStates_INCH1_VALUE->setText(QString::number(telemetryInfo.inputChannel01));
    ui->UAVPlaneStates_INCH2_VALUE->setText(QString::number(telemetryInfo.inputChannel02));
    ui->UAVPlaneStates_INCH3_VALUE->setText(QString::number(telemetryInfo.inputChannel03));
    ui->UAVPlaneStates_INCH4_VALUE->setText(QString::number(telemetryInfo.inputChannel04));
    ui->UAVPlaneStates_INCH5_VALUE->setText(QString::number(telemetryInfo.inputChannel05));
    ui->UAVPlaneStates_INCH6_VALUE->setText(QString::number(telemetryInfo.inputChannel06));
    ui->UAVPlaneStates_INCH7_VALUE->setText(QString::number(telemetryInfo.inputChannel07));
    ui->UAVPlaneStates_DistanceHeight_VALUE->setText(QString::number(telemetryInfo.relativeGroundHeigth, 'f', 2));
    ui->UAVPlaneStates_MarkPoint_VALUE->setText(QString::number(telemetryInfo.targetPoint));
    ui->UAVPlaneStates_MarkHeight_VALUE->setText(QString::number(telemetryInfo.targetHeigth, 'f', 1));
    ui->UAVPlaneStates_MarkDirection_VALUE->setText(QString::number(telemetryInfo.targetDirection, 'f', 1));

    ui->UAVPlaneStates_Status_VALUE->setText(QString::asprintf("%02X", telemetryInfo._status));

    ui->UAVPlaneStates_Control_VALUE->setText(QStringLiteral("--"));

    ui->UAVPlaneStates_Aileron_IN_VALUE->setText(QString::number(telemetryInfo.inputChannel01));
    ui->UAVPlaneStates_Elevator_IN_VALUE->setText(QString::number(telemetryInfo.inputChannel02));
    ui->UAVPlaneStates_Throttle_IN_VALUE->setText(QString::number(telemetryInfo.inputChannel03));
    ui->UAVPlaneStates_Rudder_IN_VALUE->setText(QString::number(telemetryInfo.inputChannel04));

    ui->UAVPlaneStates_Aileron_OUT_VALUE->setText(QString::number(telemetryInfo._aileron  / 10.0, 'f', 2));
    ui->UAVPlaneStates_Elevator_OUT_VALUE->setText(QString::number(telemetryInfo._elevator / 10.0, 'f', 2));
    ui->UAVPlaneStates_Throttle_OUT_VALUE->setText(QString::number(telemetryInfo._throttle / 10.0, 'f', 2));
    ui->UAVPlaneStates_Rudder_OUT_VALUE->setText(QString::number(telemetryInfo._rudder   / 10.0, 'f', 2));
}
