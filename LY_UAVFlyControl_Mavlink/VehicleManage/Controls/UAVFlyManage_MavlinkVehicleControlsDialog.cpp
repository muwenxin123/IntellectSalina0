#include "UAVFlyManage_MavlinkVehicleControlsDialog.h"
#include "ui_UAVFlyManage_MavlinkVehicleControlsDialog.h"
#include "lyUI/LyWidget.h"
#include "lyUI/LyMessageBox.h"
#include "IProj.h"
#include "QGCApplication.h"
#include "MissionManager.h"
#include "Vehicle.h"
#include <QPainter>
#include <QMouseEvent>

#define  LINE_LENGTH              3

UAVFlyManage_MavlinkVehicleControlsDialog::UAVFlyManage_MavlinkVehicleControlsDialog(QWidget *parent, Vehicle *vehicle, int vehicleId)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkVehicleControlsDialog)
    , m_Show(false)
    , m_vehicle(vehicle)
    , m_vehicleId(vehicleId)
    , m_Selected(false)
    , m_needOnMouseDoubleClickEvent(false)
    , m_MapGetPosition(false)
    , m_Color(0)
    , m_Longitude_GuidedMode_GotoLocation(0)
    , m_Latitude_GuidedMode_GotoLocation(0)
    , m_Altitude_GuidedMode_GotoLocation(0)
{
    ui->setupUi(this);

    if(m_vehicle){
        m_Color = m_vehicle->color();
    }

    if(ui && ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint){
        ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint->installEventFilter(this);
    }

    InitVehicleControls();
}

UAVFlyManage_MavlinkVehicleControlsDialog::~UAVFlyManage_MavlinkVehicleControlsDialog()
{
    delete ui;
}

bool UAVFlyManage_MavlinkVehicleControlsDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::MouseButtonPress){
        if(ui && (watched == ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint)
                )
        {
            InitVehicleControlsWayPoints();
        }
    }

    return QWidget::eventFilter(watched, event);
}

void UAVFlyManage_MavlinkVehicleControlsDialog::changeEvent(QEvent *e)
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

void UAVFlyManage_MavlinkVehicleControlsDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    m_Show = true;
}

void UAVFlyManage_MavlinkVehicleControlsDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    m_Show     = false;
    m_Selected = false;

    m_MapGetPosition = false;
    m_needOnMouseDoubleClickEvent = m_MapGetPosition;
    ui->pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_MapGetPositions->setText(tr("Start Getting Position"));
}

void UAVFlyManage_MavlinkVehicleControlsDialog::InitVehicleControls()
{
    if(!ui){
        return ;
    }

    if(!getVehicle()){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Mode){
        return ;
    }

    if(ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint){
        ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint->clear();
        MissionManager* missionManager = getVehicle()->missionManager();
        if(missionManager){
            QStringList itemList;
            itemList.clear();
            for(const auto* missionItem : missionManager->missionItems()){
                itemList.append(QString("%1").arg(missionItem->sequenceNumber()));
            }
            ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint->addItems(itemList);
        }
    }

    if(ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Mode){
        ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Mode->clear();
        ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Mode->addItems(getVehicle()->flightModes());
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::InitVehicleControlsWayPoints()
{
    if(!ui){
        return ;
    }

    if(!getVehicle()){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Mode){
        return ;
    }

    if(ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint){
        ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint->clear();
        MissionManager* missionManager = getVehicle()->missionManager();
        if(missionManager){
            QStringList itemList;
            itemList.clear();
            for(const auto* missionItem : missionManager->missionItems()){
                itemList.append(QString("%1").arg(missionItem->sequenceNumber()));
            }
            ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint->addItems(itemList);
        }
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::MavlinkVehicleConnect()
{

}

void UAVFlyManage_MavlinkVehicleControlsDialog::MavlinkVehicleDisconnect()
{

}

Vehicle *UAVFlyManage_MavlinkVehicleControlsDialog::getVehicle() const
{
    return m_vehicle;
}

QString UAVFlyManage_MavlinkVehicleControlsDialog::getVehicleInfo() const
{
    if(getVehicle()){
        return (" " + getVehicle()->firmwareTypeString() + " : " + getVehicle()->vehicleTypeString());
    }
    return QString("No Vehicle!");
}

bool UAVFlyManage_MavlinkVehicleControlsDialog::getSelected() const
{
    return m_Selected;
}

void UAVFlyManage_MavlinkVehicleControlsDialog::setSelected(bool Selected)
{
    m_Selected = Selected;
}

void UAVFlyManage_MavlinkVehicleControlsDialog::DrawDynamic(QPainter &painter, IProj *pProj)
{
    if(!m_needOnMouseDoubleClickEvent){
        return ;
    }

    if(!pProj){
        return ;
    }

    const QColor&  penColor = m_Color;
    QPen penText(QColor(255,255,255));
    QPen pen(penColor);

    const QPoint& pos = pProj->Jwd2Screen(m_Longitude_GuidedMode_GotoLocation, m_Latitude_GuidedMode_GotoLocation);
    const int rx = (LINE_LENGTH + 2);
    painter.setPen(pen);
    painter.setBrush(QBrush(penColor));
    painter.drawEllipse(pos, rx, rx);

    if(getVehicle() && getVehicle()->coordinate().isValid()){
        const double Lat_X = getVehicle()->coordinate().latitude();
        const double Lon_Y = getVehicle()->coordinate().longitude();
        const double Alt_Z = getVehicle()->coordinate().altitude();
        const QPoint& ptFirst = pProj->Jwd2Screen(Lon_Y, Lat_X);
        painter.drawLine(ptFirst, pos);
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::OnMouseButtonDown(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleControlsDialog::OnMouseButtonRelease(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleControlsDialog::OnMouseDoubleClick(QMouseEvent *e, IProj *pProj)
{
    if(!m_needOnMouseDoubleClickEvent){
        return ;
    }

    if(!ui || !e || !pProj){
        return ;
    }

    if(!getVehicle()){
        return ;
    }

    if(!ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Longitude){
        return ;
    }

    double longitude = 0;
    double latitude  = 0;
    double altitude  = QGCApplication::guidedMinimumAltitude();

    if(getVehicle()->coordinate().isValid()) {

        altitude  = getVehicle()->altitudeRelativeNumber();
    }

    const QPoint& pos = e->pos();
    pProj->Screen2Jwd(pos, longitude, latitude);

    ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Longitude->setValue(longitude);
    ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Latitude->setValue(latitude);
    ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Altitude->setValue(altitude);

    m_Longitude_GuidedMode_GotoLocation = longitude;
    m_Latitude_GuidedMode_GotoLocation  = latitude;
    m_Altitude_GuidedMode_GotoLocation  = altitude;
}

void UAVFlyManage_MavlinkVehicleControlsDialog::OnMouseMove(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_SetMode_clicked()
{
    if(!ui){
        return ;
    }

    if(!getVehicle()){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Mode){
        return ;
    }

    const QString& flightMode = ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Mode->currentText();
    if(flightMode.isEmpty()){
        return ;
    }

    if(getVehicle()->flightModes().contains(flightMode)){
        if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + QString(tr("Are you sure want to switch to %1 Mode?")).arg(flightMode), QMessageBox::Yes | QMessageBox::No)) {
            getVehicle()->setFlightMode(flightMode);
        }
    }else{
        LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + QString(tr("Vehicle is not support %1 Mode!")).arg(flightMode), QMessageBox::Yes | QMessageBox::No);
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_SetWaypoint_clicked()
{
    if(!ui){
        return ;
    }

    if(!getVehicle()){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint){
        return ;
    }

    const QString& waypointIndexText = ui->comboBox_UAVFlyManage_MavlinkVehicleControlsDialog_Waypoint->currentText();
    if(waypointIndexText.isEmpty()){
        return ;
    }

    bool ok = false;
    const int waypointIndex = waypointIndexText.toInt(&ok);
    if(!ok){
        return ;
    }

    MissionManager* missionManager = getVehicle()->missionManager();
    if(missionManager){
        if(waypointIndex >= 0 || waypointIndex < missionManager->missionItems().size()){
            if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + QString(tr("Are you sure want set current mission sequence : %1?")).arg(waypointIndex), QMessageBox::Yes | QMessageBox::No)) {
                getVehicle()->setCurrentMissionSequence(waypointIndex);
            }
        }else{
            LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + QString(tr("Vehicle is not support set current mission sequence : %1, mission items size : %2!")).arg(waypointIndex).arg(missionManager->missionItems().size()), QMessageBox::Yes | QMessageBox::No);
        }

    }else{
        LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + QString(tr("Vehicle is not support set current mission sequence : %1!")).arg(waypointIndex), QMessageBox::Yes | QMessageBox::No);
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Mission_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + tr("Are you sure want to switch to Mission Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        if(vehicle->flightModes().contains(vehicle->missionFlightMode())){
            vehicle->setFlightMode(vehicle->missionFlightMode());
        }else{
            LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Mission Mode!"), QMessageBox::Yes | QMessageBox::No);
        }
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Manual_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + tr("Are you sure want to switch to Manual Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        if(vehicle->flightModes().contains(vehicle->takeControlFlightMode())){
            vehicle->setFlightMode(vehicle->takeControlFlightMode());
        }else{
            LyMessageBox::warning(nullptr, tr("Tip"), vehicle->weaponTitle() + tr("Vehicle is not support Manual Mode!"), QMessageBox::Yes | QMessageBox::No);
        }
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_RTL_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + tr("Are you sure want to switch to RTL Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        vehicle->guidedModeRTL(false);
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Land_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + tr("Are you sure want to switch to Land Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        vehicle->guidedModeLand();
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Arm_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + tr("Are you sure want to switch to Arm Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        vehicle->setArmed(true, true);
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_Disarm_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() + tr("Are you sure want to switch to Disarm Mode?"), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        vehicle->setArmed(false, true);
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_EmergencyStop_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you sure want to switch to Emergency Stop?"), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        vehicle->emergencyStop();
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_ChangeAltitude_Altitude_clicked()
{
    if(!getVehicle()){
        return ;
    }

    const auto altitude  = ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_ChangeAltitude_Altitude->value();
    const auto pause     = ui->checkBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_ChangeAltitude_Pause->isChecked();

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() +
                                                  QString(tr("Are you sure want to change altitude(altitude:%1)?"))
                                                  .arg(altitude, 0, 'f', 2), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        vehicle->guidedModeChangeAltitude(altitude, pause);
    }
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_MapGetPositions_clicked()
{

    if (getVehicle()) {

        m_Color = getVehicle()->color();

        if (getVehicle()->coordinate().isValid()) {
            ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Longitude->setValue(getVehicle()->coordinate().longitude());
            ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Latitude->setValue(getVehicle()->coordinate().latitude());
            ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Altitude->setValue(getVehicle()->altitudeRelativeNumber()  );

            m_Longitude_GuidedMode_GotoLocation = getVehicle()->coordinate().longitude();
            m_Latitude_GuidedMode_GotoLocation  = getVehicle()->coordinate().latitude();

            m_Altitude_GuidedMode_GotoLocation  = getVehicle()->altitudeRelativeNumber();
        }
    }

    if(m_MapGetPosition){
        ui->pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_MapGetPositions->setText(tr("Start Getting Position"));
    }else{
        LyMessageBox::warning(nullptr, tr("Tip"), QString(tr("Double click map getting position from map.")), QMessageBox::Yes);
        ui->pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_MapGetPositions->setText(tr("Stop Getting Position"));
    }
    m_MapGetPosition = !m_MapGetPosition;

    m_needOnMouseDoubleClickEvent = m_MapGetPosition;

    emit needOnMouseDoubleClickEventSig(m_needOnMouseDoubleClickEvent);
}

void UAVFlyManage_MavlinkVehicleControlsDialog::on_pushButton_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_FlyGoto_clicked()
{

    if(!getVehicle()){
        return ;
    }

    const auto longitude = ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Longitude->value();
    const auto latitude  = ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Latitude->value();
    const auto altitude  = ui->doubleSpinBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Altitude->value();
    const auto changeAltitudeRelative = ui->checkBox_UAVFlyManage_MavlinkVehicleControlsDialog_GuidedMode_GotoLocation_Altitude_ChangeAltitudeRelative->isChecked();
    QGeoCoordinate coordinate(latitude, longitude, altitude);

    if(!coordinate.isValid()){
        QGCApplication::showAppMessage(getVehicle()->weaponTitle() + QString(tr("Position(longitude:%1, latitude:%2, altitude:%3) is not isValid!")).arg(longitude, 0, 'f', 7).arg(latitude, 0, 'f', 7).arg(altitude, 0, 'f', 2));
        return ;
    }

    if (coordinate.isValid() && QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), getVehicle()->weaponTitle() +
                                                  QString(tr("Are you sure want to fly goto Position(longitude:%1, latitude:%2, altitude:%3)?"))
                                                  .arg(coordinate.longitude(), 0, 'f', 7).arg(coordinate.latitude(), 0, 'f', 7).arg(coordinate.altitude(), 0, 'f', 2), QMessageBox::Yes | QMessageBox::No)) {
        Vehicle* vehicle = getVehicle();
        if(!vehicle){
            return ;
        }

        if(changeAltitudeRelative){
            vehicle->guidedModeGotoLocationChangeAltitude(coordinate);
        }else{
            vehicle->guidedModeGotoLocation(coordinate);
        }
    }
}
