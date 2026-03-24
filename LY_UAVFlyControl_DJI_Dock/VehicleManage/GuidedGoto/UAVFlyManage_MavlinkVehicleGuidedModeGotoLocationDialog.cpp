#include "UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog.h"
#include "ui_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog.h"
#include "lyUI/LyWidget.h"
#include "lyUI/LyMessageBox.h"
#include "IProj.h"
#include "QGCApplication.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "FlyControl/UAVSelectDialog.h"
#include "LY_UAVFlyControl_Mavlink.h"
#include <QMouseEvent>

#define  LINE_LENGTH              3

UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog)
    , m_Show(false)
    , m_needOnMouseDoubleClickEvent(false)
    , m_MapGetPosition(false)
    , m_Longitude_GuidedMode_GotoLocation(0)
    , m_Latitude_GuidedMode_GotoLocation(0)
    , m_Altitude_GuidedMode_GotoLocation(0)
{
    ui->setupUi(this);

    InitWeaponInfoList();
    if(QGCApplication::multiVehicleManager()){
        connect(QGCApplication::multiVehicleManager(), &MultiVehicleManager::vehicleAdded, this, [this](Vehicle* vehicle){
            InitWeaponInfoListAdded(vehicle);
        });

        connect(QGCApplication::multiVehicleManager(), &MultiVehicleManager::vehicleRemoved, this, [this](Vehicle* vehicle){
            InitWeaponInfoListRemoved(vehicle);
        });
    }

    DisplayListVehicleInfo();
}

UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::~UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::changeEvent(QEvent *e)
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

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::showEvent(QShowEvent * )
{

}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::hideEvent(QHideEvent * )
{
    m_MapGetPosition = false;
    m_needOnMouseDoubleClickEvent = m_MapGetPosition;
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::InitWeaponInfoList()
{

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle){
        return ;
    }

    ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle->clear();

    if(!MavlinkApp()){
        return ;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        return ;
    }

    const auto vehiclesCount = vehiclesList->count();
    for (int i = 0; i < vehiclesCount; i++) {
        Vehicle * vehicle = vehiclesList->at(i);
        if(!vehicle){
            continue ;
        }

        const auto vehicleID = vehicle->id();
        const auto vehicleWeaponId = vehicle->weaponId();

        const QString& strVehicleName = QGCApplication::vehicleName(vehicleWeaponId, vehicleID);
        QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);

        ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle->addItem(strVehicleName, userData);
    }
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::InitWeaponInfoListAdded(Vehicle *vehicleAdded)
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle){
        return ;
    }

    if(!MavlinkApp()){
        return ;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        return ;
    }

    const auto vehiclesCount = vehiclesList->count();
    for (int i = 0; i < vehiclesCount; i++) {
        Vehicle * vehicle = vehiclesList->at(i);
        if(!vehicle){
            continue ;
        }

        if(vehicle != vehicleAdded){
            continue ;
        }

        const auto vehicleID = vehicle->id();
        const auto vehicleWeaponId = vehicle->weaponId();

        const QString& strVehicleName = QGCApplication::vehicleName(vehicleWeaponId, vehicleID);
        QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);

        ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle->insertItem(i, strVehicleName, userData);

    }
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::InitWeaponInfoListRemoved(Vehicle *vehicleRemoved)
{
    if(!ui || !vehicleRemoved){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle){
        return ;
    }

    for(int comboxIndex = 0; comboxIndex < ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle->count(); comboxIndex++){
        const auto vehicleID       = vehicleRemoved->id();
        const auto vehicleWeaponId = vehicleRemoved->weaponId();
        QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);
        if(ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle->itemData(comboxIndex).toString() == userData){
            ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle->removeItem(comboxIndex);
            break;
        }
    }
}

Vehicle *UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::GetMainVehicle() const
{
    if(!ui){
        return nullptr;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle){
        return nullptr;
    }

    if(!MavlinkApp()){
        return nullptr;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        return nullptr;
    }

    Vehicle *currentVehicle = nullptr;
    const auto vehiclesCount = vehiclesList->count();
    for (int i = 0; i < vehiclesCount; i++) {
        Vehicle * vehicle = vehiclesList->at(i);
        if(!vehicle){
            continue ;
        }
        const auto vehicleID = vehicle->id();
        const auto vehicleWeaponId = vehicle->weaponId();

        const QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);

        if(userData == ui->comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle->currentData().toString()){
            currentVehicle = vehicle;
            break;
        }
    }

    return currentVehicle;
}

bool UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::GetListVehicle()
{
    m_Vehicle_GuidedMode_GotoLocationList.clear();

    if(!ui){
        return false;
    }

    if(!ui->label_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle_Tip){
        return false;
    }

    ui->label_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle_Tip->setText("");

    const UAVSelectDialog* pUAVSelectDialog = LY_UAVFlyControl_Mavlink::Get().getPUAVSelectDialog();
    if(!pUAVSelectDialog){

        return false;
    }

    const UAVPushButton* pSelectCurrentWeaponComponent = pUAVSelectDialog->getSelectCurrentWeaponComponent();
    if(!pSelectCurrentWeaponComponent){

        ui->label_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle_Tip->setText(tr("UAV not selected!"));
        return false;
    }

    const UAVPushButton* pSelectAllWeaponComponent = pUAVSelectDialog->getSelectAllWeaponComponent();
    if(!pSelectAllWeaponComponent){

        return false;
    }

    if(!MavlinkApp()){
        return false;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){

        return false;
    }

    const auto& listWeaponComponent = pUAVSelectDialog->getButtonListWeaponComponent();
    if(listWeaponComponent.size() <= 0){

        return false;
    }

    const auto vehiclesCount = vehiclesList->count();
    for (int i = 0; i < vehiclesCount; i++) {
        const Vehicle * vehicle = vehiclesList->at(i);
        if(!vehicle){

            return false;
        }

        const QMap<UAVPushButton *, unsigned long long int>::const_key_value_iterator lineListFind
                = std::find_if(listWeaponComponent.constKeyValueBegin(), listWeaponComponent.constKeyValueEnd(),
                               [&vehicle](const std::pair<UAVPushButton *, unsigned long long int>& node)
        {
            if(!node.first){
                return false;
            }

            if(node.first->getVehicle() == vehicle && node.first->getWeaponId() == vehicle->weaponId() && node.first->getVehicleID() == vehicle->id()){
                return true;
            }

            return false;
        }
        );

        if(lineListFind == listWeaponComponent.constKeyValueEnd()) {

            return false;
        }
    }

    QList<Vehicle*> vehicleListSelected;
    vehicleListSelected.clear();

    if (pSelectAllWeaponComponent == pSelectCurrentWeaponComponent && "ALL" == pSelectAllWeaponComponent->getStrComponentID())
    {
        if(!pSelectAllWeaponComponent->isChecked()){

            return false;
        }

        for (int i = 0; i < vehiclesCount; i++) {
            Vehicle * vehicle = vehiclesList->at(i);
            if(!vehicle){

                return false;
            }
            vehicleListSelected.append(vehicle);
        }
    }
    else
    {

        const auto vehiclesCount = vehiclesList->count();
        for (int i = 0; i < vehiclesCount; i++) {
            Vehicle * vehicle = vehiclesList->at(i);
            if(!vehicle){

                return false;
            }

            const QMap<UAVPushButton *, unsigned long long int>::const_key_value_iterator lineListFind
                    = std::find_if(listWeaponComponent.constKeyValueBegin(), listWeaponComponent.constKeyValueEnd(),
                                   [&vehicle](const std::pair<UAVPushButton *, unsigned long long int>& node)
            {
                if(!node.first){
                    return false;
                }

                if(node.first->getVehicle() == vehicle && node.first->getWeaponId() == vehicle->weaponId() && node.first->getVehicleID() == vehicle->id()){
                    return true;
                }

                return false;
            }
            );

            if(lineListFind == listWeaponComponent.constKeyValueEnd()) {

                return false;
            }

            if(lineListFind.base().key()->isChecked()){
                vehicleListSelected.append(vehicle);
            }
        }
    }

    Vehicle* mainVehicle = GetMainVehicle();

    if(!vehicleListSelected.contains(mainVehicle)){

        ui->label_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle_Tip->setText(tr("Main Vehicle is not contains!"));
        return false;
    }

    if(!mainVehicle->coordinate().isValid()){
        return false;
    }

    const QGeoCoordinate mainVheicle_Location(mainVehicle->coordinate().latitude(), mainVehicle->coordinate().longitude(), mainVehicle->altitudeRelativeNumber() );
    const QGeoCoordinate mainVheicle_guidedMode_GotoLocation(m_Latitude_GuidedMode_GotoLocation, m_Longitude_GuidedMode_GotoLocation, m_Altitude_GuidedMode_GotoLocation);

    QList<Vehicle_GuidedMode_GotoLocation> vehicle_GuidedMode_GotoLocationListTemp;
    vehicle_GuidedMode_GotoLocationListTemp.clear();

    const auto mainVheicle_dist    = mainVheicle_Location.distanceTo(mainVheicle_guidedMode_GotoLocation);
    const auto mainVheicle_azimuth = mainVheicle_Location.azimuthTo(mainVheicle_guidedMode_GotoLocation);
    for( auto* vehicle : vehicleListSelected){
        if(vehicle->coordinate().isValid()){
            const QGeoCoordinate vheicle_Location = vehicle->coordinate().atDistanceAndAzimuth(mainVheicle_dist, mainVheicle_azimuth);
            vehicle_GuidedMode_GotoLocationListTemp.append(Vehicle_GuidedMode_GotoLocation{vehicle, vheicle_Location.longitude(), vheicle_Location.latitude(), vehicle->altitudeRelativeNumber()  });
        }
    }

    m_Vehicle_GuidedMode_GotoLocationList = vehicle_GuidedMode_GotoLocationListTemp;

    if(ui && m_Vehicle_GuidedMode_GotoLocationList.size() == 0){
        ui->label_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle_Tip->setText(tr("no vehicle seleted!"));
    }

    return true;
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::DisplayListVehicleInfo()
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_ChangeAltitude_Altitude->sizePolicy().hasHeightForWidth());

    QLayoutItem *child = nullptr;
     while ((child = ui->verticalLayout_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_FlyGoto_List->takeAt(0)) != nullptr) {
         if(child->layout()){
             QLayoutItem *child2 = nullptr;
             while ((child2 = child->layout()->takeAt(0)) != nullptr) {
                 if(child2->widget()){
                     child2->widget()->setParent(nullptr);
                     child2->widget()->deleteLater();
                 }
                 delete child2;
             }
         }
         delete child;
     }

    for(const auto& vehicle_GuidedMode_GotoLocation : m_Vehicle_GuidedMode_GotoLocationList){
        if(!vehicle_GuidedMode_GotoLocation.vehicle){
            continue;
        }
        const unsigned long long int adder = (unsigned long long int)(vehicle_GuidedMode_GotoLocation.vehicle);
        const double longitude = vehicle_GuidedMode_GotoLocation.longitude_GuidedMode_GotoLocation;
        const double latitude  = vehicle_GuidedMode_GotoLocation.latitude_GuidedMode_GotoLocation;
        const double altitude  = vehicle_GuidedMode_GotoLocation.altitude_GuidedMode_GotoLocation;
        QHBoxLayout* horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout_%1").arg(adder));
        QLabel* label_GuidedMode_GotoLocation_Altitude_Name = new QLabel(this);
        label_GuidedMode_GotoLocation_Altitude_Name->setObjectName(QString::fromUtf8("label_GuidedMode_GotoLocation_Altitude_Name_%1").arg(adder));
        label_GuidedMode_GotoLocation_Altitude_Name->setText(vehicle_GuidedMode_GotoLocation.vehicle->weaponName() + (": "));

        horizontalLayout->addWidget(label_GuidedMode_GotoLocation_Altitude_Name);

        QLabel* label_GuidedMode_GotoLocation_Longitude = new QLabel(this);
        label_GuidedMode_GotoLocation_Longitude->setObjectName(QString::fromUtf8("label_GuidedMode_GotoLocation_Longitude_%1").arg(adder));
        label_GuidedMode_GotoLocation_Longitude->setText(tr("Longitude:"));

        horizontalLayout->addWidget(label_GuidedMode_GotoLocation_Longitude);

        QDoubleSpinBox* doubleSpinBox_GuidedMode_GotoLocation_Longitude = new QDoubleSpinBox(this);
        doubleSpinBox_GuidedMode_GotoLocation_Longitude->setObjectName(QString::fromUtf8("doubleSpinBox_GuidedMode_GotoLocation_Longitude_%1").arg(adder));
        sizePolicy.setHeightForWidth(doubleSpinBox_GuidedMode_GotoLocation_Longitude->sizePolicy().hasHeightForWidth());
        doubleSpinBox_GuidedMode_GotoLocation_Longitude->setSizePolicy(sizePolicy);
        doubleSpinBox_GuidedMode_GotoLocation_Longitude->setReadOnly(true);
        doubleSpinBox_GuidedMode_GotoLocation_Longitude->setDecimals(7);
        doubleSpinBox_GuidedMode_GotoLocation_Longitude->setMinimum(-360.000000000000000);
        doubleSpinBox_GuidedMode_GotoLocation_Longitude->setMaximum(360.000000000000000);
        doubleSpinBox_GuidedMode_GotoLocation_Longitude->setValue(longitude);

        horizontalLayout->addWidget(doubleSpinBox_GuidedMode_GotoLocation_Longitude);

        QLabel* label_GuidedMode_GotoLocation_Latitude = new QLabel(this);
        label_GuidedMode_GotoLocation_Latitude->setObjectName(QString::fromUtf8("label_GuidedMode_GotoLocation_Latitude_%1").arg(adder));
        label_GuidedMode_GotoLocation_Latitude->setText(tr("Latitude:"));

        horizontalLayout->addWidget(label_GuidedMode_GotoLocation_Latitude);

        QDoubleSpinBox* doubleSpinBox_GuidedMode_GotoLocation_Latitude = new QDoubleSpinBox(this);
        doubleSpinBox_GuidedMode_GotoLocation_Latitude->setObjectName(QString::fromUtf8("doubleSpinBox_GuidedMode_GotoLocation_Latitude_%1").arg(adder));
        sizePolicy.setHeightForWidth(doubleSpinBox_GuidedMode_GotoLocation_Latitude->sizePolicy().hasHeightForWidth());
        doubleSpinBox_GuidedMode_GotoLocation_Latitude->setSizePolicy(sizePolicy);
        doubleSpinBox_GuidedMode_GotoLocation_Latitude->setReadOnly(true);
        doubleSpinBox_GuidedMode_GotoLocation_Latitude->setDecimals(7);
        doubleSpinBox_GuidedMode_GotoLocation_Latitude->setMinimum(-360.000000000000000);
        doubleSpinBox_GuidedMode_GotoLocation_Latitude->setSingleStep(0.000000000000000);
        doubleSpinBox_GuidedMode_GotoLocation_Latitude->setValue(latitude);

        horizontalLayout->addWidget(doubleSpinBox_GuidedMode_GotoLocation_Latitude);

        QLabel* label_GuidedMode_GotoLocation_Altitude = new QLabel(this);
        label_GuidedMode_GotoLocation_Altitude->setObjectName(QString::fromUtf8("label_GuidedMode_GotoLocation_Altitude_%1").arg(adder));
        label_GuidedMode_GotoLocation_Altitude->setText(tr("Altitude:"));

        horizontalLayout->addWidget(label_GuidedMode_GotoLocation_Altitude);

        QDoubleSpinBox* doubleSpinBox_GuidedMode_GotoLocation_Altitude = new QDoubleSpinBox(this);
        doubleSpinBox_GuidedMode_GotoLocation_Altitude->setObjectName(QString::fromUtf8("doubleSpinBox_GuidedMode_GotoLocation_Altitude_%1").arg(adder));
        sizePolicy.setHeightForWidth(doubleSpinBox_GuidedMode_GotoLocation_Altitude->sizePolicy().hasHeightForWidth());
        doubleSpinBox_GuidedMode_GotoLocation_Altitude->setSizePolicy(sizePolicy);
        doubleSpinBox_GuidedMode_GotoLocation_Altitude->setReadOnly(true);
        doubleSpinBox_GuidedMode_GotoLocation_Altitude->setMinimum(-1000000.000000000000000);
        doubleSpinBox_GuidedMode_GotoLocation_Altitude->setMaximum(1000000.000000000000000);
        doubleSpinBox_GuidedMode_GotoLocation_Altitude->setValue(altitude);

        horizontalLayout->addWidget(doubleSpinBox_GuidedMode_GotoLocation_Altitude);
        ui->verticalLayout_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_FlyGoto_List->addLayout(horizontalLayout);
    }
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::SendFlyCmdFun(std::function<void (Vehicle* vehicle)>& vehicleFun)
{
    if(!vehicleFun){
        LyMessageBox::warning(nullptr, tr("Tips"), tr("vehicle fun is nullptr, Instruction not send?"), QMessageBox::Yes);
        return ;
    }

    const UAVSelectDialog* pUAVSelectDialog = LY_UAVFlyControl_Mavlink::Get().getPUAVSelectDialog();
    if(!pUAVSelectDialog){
        LyMessageBox::warning(nullptr, tr("Tips"), tr("UAVSelectDialog is nullptr, Instruction not send?"), QMessageBox::Yes);
        return ;
    }

    const UAVPushButton* pSelectCurrentWeaponComponent = pUAVSelectDialog->getSelectCurrentWeaponComponent();
    if(!pSelectCurrentWeaponComponent){
        LyMessageBox::warning(nullptr, tr("Tips"), tr("UAVPushButton SelectCurrentWeaponComponent is nullptr, UAV not selected, Instruction not send?"), QMessageBox::Yes);
        return ;
    }

    const UAVPushButton* pSelectAllWeaponComponent = pUAVSelectDialog->getSelectAllWeaponComponent();
    if(!pSelectAllWeaponComponent){

        LyMessageBox::warning(nullptr, tr("Tips"), tr("UAVPushButton SelectAllWeaponComponent is nullptr, ALL Button error, Instruction not send?"), QMessageBox::Yes);
        return ;
    }

    if(!MavlinkApp()){
        return ;
    }

    auto* vehiclesList = QGCApplication::vehicles();
    if(!vehiclesList){
        LyMessageBox::warning(nullptr, tr("Tips"), tr("VehiclesList is nullptr, VehiclesList error, Instruction not send?"), QMessageBox::Yes);
        return ;
    }

    const auto& listWeaponComponent = pUAVSelectDialog->getButtonListWeaponComponent();
    if(listWeaponComponent.size() <= 0){
        LyMessageBox::warning(nullptr, tr("Tips"), tr("WeaponComponentList size is zero, ListWeaponComponent error, Instruction not send?"), QMessageBox::Yes);
        return ;
    }

    const auto vehiclesCount = vehiclesList->count();
    for (int i = 0; i < vehiclesCount; i++) {
        const Vehicle * vehicle = vehiclesList->at(i);
        if(!vehicle){
            LyMessageBox::warning(nullptr, tr("Tips"), tr("Vehicle is nullptr, Vehicle error, Instruction not send?"), QMessageBox::Yes);
            return ;
        }

        const QMap<UAVPushButton *, unsigned long long int>::const_key_value_iterator lineListFind
                = std::find_if(listWeaponComponent.constKeyValueBegin(), listWeaponComponent.constKeyValueEnd(),
                               [&vehicle](const std::pair<UAVPushButton *, unsigned long long int>& node)
        {
            if(!node.first){
                return false;
            }

            if(node.first->getVehicle() == vehicle && node.first->getWeaponId() == vehicle->weaponId() && node.first->getVehicleID() == vehicle->id()){
                return true;
            }

            return false;
        }
        );

        if(lineListFind == listWeaponComponent.constKeyValueEnd()) {
            LyMessageBox::warning(nullptr, tr("Tips"), vehicle->weaponTitle() + tr("Vehicle is not equal UAVPushButton, Vehicle or ID error, Instruction not send?"), QMessageBox::Yes);
            return ;
        }
    }

    if (pSelectAllWeaponComponent == pSelectCurrentWeaponComponent && "ALL" == pSelectAllWeaponComponent->getStrComponentID())
    {
        if(!pSelectAllWeaponComponent->isChecked()){
            LyMessageBox::warning(nullptr, tr("Tips"), tr("UAVPushButton SelectAllWeaponComponent is ischecked false, Instruction not send?"), QMessageBox::Yes);
            return ;
        }

        for (int i = 0; i < vehiclesCount; i++) {
            Vehicle * vehicle = vehiclesList->at(i);
            if(!vehicle){
                LyMessageBox::warning(nullptr, tr("Tips"), tr("Vehicle is nullptr, Vehicle error, Instruction not send?"), QMessageBox::Yes);
                return ;
            }

            vehicleFun(vehicle);
        }
    }
    else
    {

        QList<Vehicle*> vehicleListSelected;
        vehicleListSelected.clear();
        const auto vehiclesCount = vehiclesList->count();
        for (int i = 0; i < vehiclesCount; i++) {
            Vehicle * vehicle = vehiclesList->at(i);
            if(!vehicle){
                LyMessageBox::warning(nullptr, tr("Tips"), tr("Vehicle is nullptr, Vehicle error, Instruction not send?"), QMessageBox::Yes);
                return ;
            }

            const QMap<UAVPushButton *, unsigned long long int>::const_key_value_iterator lineListFind
                    = std::find_if(listWeaponComponent.constKeyValueBegin(), listWeaponComponent.constKeyValueEnd(),
                                   [&vehicle](const std::pair<UAVPushButton *, unsigned long long int>& node)
            {
                if(!node.first){
                    return false;
                }

                if(node.first->getVehicle() == vehicle && node.first->getWeaponId() == vehicle->weaponId() && node.first->getVehicleID() == vehicle->id()){
                    return true;
                }

                return false;
            }
            );

            if(lineListFind == listWeaponComponent.constKeyValueEnd()) {
                LyMessageBox::warning(nullptr, tr("Tips"), vehicle->weaponTitle() + tr("Vehicle is not equal UAVPushButton, Vehicle or ID error, Instruction not send?"), QMessageBox::Yes);
                return ;
            }

            if(lineListFind.base().key()->isChecked()){
                vehicleListSelected.append(vehicle);
            }
        }

        for( auto* vehicle : vehicleListSelected){
            vehicleFun(vehicle);
        }
    }
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::DrawDynamic(QPainter &painter, IProj *pProj)
{
    if(!m_needOnMouseDoubleClickEvent){
        return ;
    }

    if(!pProj){
        return ;
    }

    for(const auto& vehicle_GuidedMode_GotoLocation : m_Vehicle_GuidedMode_GotoLocationList){
        if(!vehicle_GuidedMode_GotoLocation.vehicle){
            continue;
        }
        const QColor&  penColor = vehicle_GuidedMode_GotoLocation.vehicle->color();
        QPen penText(QColor(255,255,255));
        QPen pen(penColor);

        const QPoint& pos = pProj->Jwd2Screen(vehicle_GuidedMode_GotoLocation.longitude_GuidedMode_GotoLocation, vehicle_GuidedMode_GotoLocation.latitude_GuidedMode_GotoLocation);
        const int rx = (LINE_LENGTH + 0);
        painter.setPen(pen);
        painter.setBrush(QBrush(penColor));
        painter.drawEllipse(pos, rx, rx);

        if(vehicle_GuidedMode_GotoLocation.vehicle && vehicle_GuidedMode_GotoLocation.vehicle->coordinate().isValid()){
            const double Lat_X = vehicle_GuidedMode_GotoLocation.vehicle->coordinate().latitude();
            const double Lon_Y = vehicle_GuidedMode_GotoLocation.vehicle->coordinate().longitude();
            const double Alt_Z = vehicle_GuidedMode_GotoLocation.vehicle->coordinate().altitude();
            const QPoint& ptFirst = pProj->Jwd2Screen(Lon_Y, Lat_X);

            const QLine line(pos, ptFirst);
            const qreal arrowSize = 10;
            const double angle = std::atan2(-line.dy(), line.dx());

            const QPointF arrowP1 = line.p1() + QPointF(sin(angle + M_PI / 3) * arrowSize,        cos(angle + M_PI / 3) * arrowSize);
            const QPointF arrowP2 = line.p1() + QPointF(sin(angle + M_PI - M_PI / 3) * arrowSize, cos(angle + M_PI - M_PI / 3) * arrowSize);

            QPolygonF arrowHead;
            arrowHead.clear();
            arrowHead << line.p1() << arrowP1 << arrowP2;

            painter.drawLine(line);
            painter.drawPolygon(arrowHead);
        }
    }
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::OnMouseButtonDown(QMouseEvent *, IProj *)
{

}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::OnMouseButtonRelease(QMouseEvent *, IProj *)
{

}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::OnMouseDoubleClick(QMouseEvent *e, IProj *pProj)
{
    if(!m_needOnMouseDoubleClickEvent){
        return ;
    }

    if(!ui || !e || !pProj){
        return ;
    }

    if(!GetMainVehicle()){
        return ;
    }

    if(!ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_Longitude){
        return ;
    }

    double longitude = 0;
    double latitude  = 0;
    double altitude  = QGCApplication::guidedMinimumAltitude();

    if(GetMainVehicle()->coordinate().isValid()) {

        altitude  = GetMainVehicle()->altitudeRelativeNumber();

    }

    const QPoint& pos = e->pos();
    pProj->Screen2Jwd(pos, longitude, latitude);

    ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_Longitude->setValue(longitude);
    ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_Latitude->setValue(latitude);
    ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_Altitude->setValue(altitude);

    m_Longitude_GuidedMode_GotoLocation = longitude;
    m_Latitude_GuidedMode_GotoLocation  = latitude;
    m_Altitude_GuidedMode_GotoLocation  = altitude;

    GetListVehicle();

    DisplayListVehicleInfo();
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::OnMouseMove(QMouseEvent *, IProj *)
{

}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::on_comboBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_SelectVehicle_currentIndexChanged(const QString &arg1)
{

}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::on_pushButton_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_ChangeAltitude_Altitude_clicked()
{

    const auto altitude  = ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_ChangeAltitude_Altitude->value();
    const auto pause     = ui->checkBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_ChangeAltitude_Pause->isChecked();

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"),
                                                  QString(tr("Are you sure want to change altitude(altitude:%1)?"))
                                                  .arg(altitude, 0, 'f', 2), QMessageBox::Yes | QMessageBox::No)) {
        GetListVehicle();
        DisplayListVehicleInfo();

        for(const auto& vehicle_GuidedMode_GotoLocation : m_Vehicle_GuidedMode_GotoLocationList){
            if(!vehicle_GuidedMode_GotoLocation.vehicle){
                continue;
            }

            vehicle_GuidedMode_GotoLocation.vehicle->guidedModeChangeAltitude(altitude, pause);
        }

    }
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::on_pushButton_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_MapGetPositions_clicked()
{

    auto* mainVehicle = GetMainVehicle();
    if (mainVehicle) {

        if (mainVehicle->coordinate().isValid()) {
            ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_Longitude->setValue(mainVehicle->coordinate().longitude());
            ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_Latitude->setValue(mainVehicle->coordinate().latitude());
            ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_Altitude->setValue(mainVehicle->altitudeRelativeNumber()  );

            m_Longitude_GuidedMode_GotoLocation = mainVehicle->coordinate().longitude();
            m_Latitude_GuidedMode_GotoLocation  = mainVehicle->coordinate().latitude();

            m_Altitude_GuidedMode_GotoLocation  = mainVehicle->altitudeRelativeNumber();
        }
    }

    if(m_MapGetPosition){
        ui->pushButton_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_MapGetPositions->setText(tr("Start Getting Position"));
    }else{
        LyMessageBox::warning(nullptr, tr("Tip"), QString(tr("Double click map getting position from map.")), QMessageBox::Yes);
        ui->pushButton_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_MapGetPositions->setText(tr("Stop Getting Position"));
    }
    m_MapGetPosition = !m_MapGetPosition;

    m_needOnMouseDoubleClickEvent = m_MapGetPosition;

    emit needOnMouseDoubleClickEventSig(m_needOnMouseDoubleClickEvent);
}

void UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog::on_pushButton_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_FlyGoto_clicked()
{

    Vehicle* mainVehicle = GetMainVehicle();
    if(!mainVehicle){
        QGCApplication::showAppMessage(mainVehicle->weaponTitle() + QString(tr("mainVehicle is not isValid!")));
        return ;
    }
    GetListVehicle();
    DisplayListVehicleInfo();

    for(const auto& vehicle_GuidedMode_GotoLocation : m_Vehicle_GuidedMode_GotoLocationList){
        if(!vehicle_GuidedMode_GotoLocation.vehicle){
            continue;
        }

        const auto longitude = vehicle_GuidedMode_GotoLocation.longitude_GuidedMode_GotoLocation;
        const auto latitude  = vehicle_GuidedMode_GotoLocation.latitude_GuidedMode_GotoLocation;
        const auto altitude  = vehicle_GuidedMode_GotoLocation.altitude_GuidedMode_GotoLocation;
        QGeoCoordinate coordinate(latitude, longitude, altitude);

        if(!coordinate.isValid()){
            QGCApplication::showAppMessage(vehicle_GuidedMode_GotoLocation.vehicle->weaponTitle() + QString(tr("Position(longitude:%1, latitude:%2, altitude:%3) is not isValid!")).arg(longitude, 0, 'f', 7).arg(latitude, 0, 'f', 7).arg(altitude, 0, 'f', 2));
            return ;
        }
    }

    const auto changeAltitudeRelative = ui->checkBox_UAVFlyManage_MavlinkVehicleGuidedModeGotoLocationDialog_GuidedMode_GotoLocation_Altitude_ChangeAltitudeRelative->isChecked();

    for(const auto& vehicle_GuidedMode_GotoLocation : m_Vehicle_GuidedMode_GotoLocationList){
        if(!vehicle_GuidedMode_GotoLocation.vehicle){
            continue;
        }

        const auto longitude = vehicle_GuidedMode_GotoLocation.longitude_GuidedMode_GotoLocation;
        const auto latitude  = vehicle_GuidedMode_GotoLocation.latitude_GuidedMode_GotoLocation;
        const auto altitude  = vehicle_GuidedMode_GotoLocation.altitude_GuidedMode_GotoLocation;
        QGeoCoordinate coordinate(latitude, longitude, altitude);

        if(!coordinate.isValid()){
            QGCApplication::showAppMessage(vehicle_GuidedMode_GotoLocation.vehicle->weaponTitle() + QString(tr("Position(longitude:%1, latitude:%2, altitude:%3) is not isValid!")).arg(longitude, 0, 'f', 7).arg(latitude, 0, 'f', 7).arg(altitude, 0, 'f', 2));
            return ;
        }

        QGCApplication::showAppMessage(QString(tr("%1 fly goto Position(longitude:%2, latitude:%3, altitude:%4)")).arg(vehicle_GuidedMode_GotoLocation.vehicle->weaponName()).arg(coordinate.longitude(), 0, 'f', 7).arg(coordinate.latitude(), 0, 'f', 7).arg(coordinate.altitude(), 0, 'f', 2)
                                       , vehicle_GuidedMode_GotoLocation.vehicle->weaponTitle());
        if(changeAltitudeRelative){
            vehicle_GuidedMode_GotoLocation.vehicle->guidedModeGotoLocationChangeAltitude(coordinate);
        }else{
            vehicle_GuidedMode_GotoLocation.vehicle->guidedModeGotoLocation(coordinate);
        }
    }
}
