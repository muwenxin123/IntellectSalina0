#include "UAVFlyManage_MavlinkVehicleStatusInfoDialog.h"
#include "ui_UAVFlyManage_MavlinkVehicleStatusInfoDialog.h"

#include "lyUI/LyMessageBox.h"

#include "QGCApplication.h"
#include "VehicleComponent.h"
#include "AutoPilotPlugin.h"
#include "Vehicle.h"
#include <QGroupBox>
#include <QLabel>

UAVFlyManage_MavlinkVehicleStatusInfoDialog::UAVFlyManage_MavlinkVehicleStatusInfoDialog(QWidget *parent, Vehicle *vehicle, int vehicleId)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkVehicleStatusInfoDialog)
    , m_Show(false)
    , m_vehicle(vehicle)
    , m_vehicleId(vehicleId)
    , m_refreshParameter(false)
    , m_Selected(false)
{
    ui->setupUi(this);

    MavlinkVehicleConnect();
}

UAVFlyManage_MavlinkVehicleStatusInfoDialog::~UAVFlyManage_MavlinkVehicleStatusInfoDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkVehicleStatusInfoDialog::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void UAVFlyManage_MavlinkVehicleStatusInfoDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_Show = true;

}

void UAVFlyManage_MavlinkVehicleStatusInfoDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    m_Show     = false;
    m_Selected = false;
}

void UAVFlyManage_MavlinkVehicleStatusInfoDialog::MavlinkVehicleConnect()
{
    if(!ui){
        return ;
    }

    if(!ui->scrollAreaWidgetContents){
        return ;
    }

    if(!ui->scrollAreaWidgetContents->layout()){
        return ;
    }

    if(!getVehicle()){
        return ;
    }

    if(!getVehicle()->autopilotPlugin()){
        return ;
    }

     const QVariantList& vehicleComponentList = getVehicle()->autopilotPlugin()->vehicleComponents();

     for(const auto& vehicleComponentVariant : vehicleComponentList){
        qDebug() << __FILE__ << __LINE__ <<  vehicleComponentVariant;
         VehicleComponent* vehicleComponent = static_cast<VehicleComponent*>(vehicleComponentVariant.value<VehicleComponent*>());
         if(vehicleComponent)
         {
             QGroupBox*   groupBox       = new QGroupBox(ui->scrollAreaWidgetContents);
             QVBoxLayout* verticalLayout = new QVBoxLayout(groupBox);

             groupBox->setTitle(vehicleComponent->name());

             QLabel* label = new QLabel(groupBox);

             verticalLayout->addWidget(label);

             QLabel* label_2 = new QLabel(groupBox);

             verticalLayout->addWidget(label_2);

             label->setText(vehicleComponent->description() + vehicleComponent->iconResource() + vehicleComponent->summaryQmlSource().toString());
             label_2->setText(vehicleComponent->setupCompleteChangedTriggerList().join(","));

             ui->scrollAreaWidgetContents->layout()->addWidget(groupBox);
         }
     }

    {
        QGroupBox*   groupBox       = new QGroupBox(ui->scrollAreaWidgetContents);
        QVBoxLayout* verticalLayout = new QVBoxLayout(groupBox);

        groupBox->setTitle(tr("Airframe"));

        QLabel* label_SystemID = new QLabel(groupBox);
        verticalLayout->addWidget(label_SystemID);
        label_SystemID->setText(QString(tr("System ID : %1")).arg(getVehicle()->id()));

        QLabel* label_AirframeType = new QLabel(groupBox);
        verticalLayout->addWidget(label_AirframeType);
        label_AirframeType->setText(QString(tr("Airframe type : %1")).arg(getVehicle()->firmwareTypeString()));

        QLabel* label_Vehicle = new QLabel(groupBox);
        verticalLayout->addWidget(label_Vehicle);
        label_Vehicle->setText(QString(tr("Vehicle : %1")).arg(getVehicle()->vehicleTypeString()));

        QLabel* label_FirmwareVersion = new QLabel(groupBox);
        verticalLayout->addWidget(label_FirmwareVersion);
        label_FirmwareVersion->setText(QString(tr("Firmware Version : %1.%2.%3.%4 , Git Revision : %5"))
                               .arg(getVehicle()->firmwareMajorVersion())
                               .arg(getVehicle()->firmwareMinorVersion())
                               .arg(getVehicle()->firmwarePatchVersion())
                               .arg(getVehicle()->firmwareVersionTypeString())
                               .arg(getVehicle()->gitHash())
                               );

        ui->scrollAreaWidgetContents->layout()->addWidget(groupBox);
    }

    for(int i = 0; i < 10; i++){

        QGroupBox*   groupBox       = new QGroupBox(ui->scrollAreaWidgetContents);
        QVBoxLayout* verticalLayout = new QVBoxLayout(groupBox);
        QLabel* label = new QLabel(groupBox);

        verticalLayout->addWidget(label);

        QLabel* label_2 = new QLabel(groupBox);

        verticalLayout->addWidget(label_2);

        groupBox->setTitle(tr("Airframe"));
        label->setText(tr("System ID:"));

        ui->scrollAreaWidgetContents->layout()->addWidget(groupBox);
    }

    if (m_vehicle && m_vehicle->throttlePct()) {

    }
}

void UAVFlyManage_MavlinkVehicleStatusInfoDialog::MavlinkVehicleDisconnect()
{

}

Vehicle *UAVFlyManage_MavlinkVehicleStatusInfoDialog::getVehicle() const
{
    return m_vehicle;
}

QString UAVFlyManage_MavlinkVehicleStatusInfoDialog::getVehicleInfo() const
{
    if(getVehicle()){
        return (" " + getVehicle()->firmwareTypeString() + " : " + getVehicle()->vehicleTypeString());
    }
    return QString("No Vehicle!");
}

bool UAVFlyManage_MavlinkVehicleStatusInfoDialog::getSelected() const
{
    return m_Selected;
}

void UAVFlyManage_MavlinkVehicleStatusInfoDialog::setSelected(bool Selected)
{
    m_Selected = Selected;
}
