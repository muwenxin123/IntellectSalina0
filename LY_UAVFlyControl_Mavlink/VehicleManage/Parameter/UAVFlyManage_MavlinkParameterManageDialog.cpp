#include "UAVFlyManage_MavlinkParameterManageDialog.h"
#include "ui_UAVFlyManage_MavlinkParameterManageDialog.h"

#include "UAVFlyManage_MavlinkParameterManageEditDialog.h"

#include "lyUI/LyMessageBox.h"

#include "QGCApplication.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "MissionItem.h"

#define UAVFLYMANAGE_MAVLINKPARAMETERMANAGEEDITDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID "VehicleWeaponId_VehicleID"

UAVFlyManage_MavlinkParameterManageDialog::UAVFlyManage_MavlinkParameterManageDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UAVFlyManage_MavlinkParameterManageDialog)
{
    ui->setupUi(this);

    InitWeaponInfoList();
    if(QGCApplication::multiVehicleManager()){
        connect(QGCApplication::multiVehicleManager(), &MultiVehicleManager::vehicleAdded, this, [this](){
            InitWeaponInfoList();
        });

        connect(QGCApplication::multiVehicleManager(), &MultiVehicleManager::vehicleRemoved, this, [this](){
            InitWeaponInfoList();
        });
    }
}

UAVFlyManage_MavlinkParameterManageDialog::~UAVFlyManage_MavlinkParameterManageDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkParameterManageDialog::changeEvent(QEvent *e)
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

void UAVFlyManage_MavlinkParameterManageDialog::InitWeaponInfoList()
{

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles){
        return ;
    }

    m_UAVFlyManage_MavlinkParameterManageEditDialogList.clear();
    ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->clear();

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->count();
    for(int i = 0; i < tabWidgetCount; i++){
        QWidget *widget = ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->widget(i);
        ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->removeTab(0);
        if(widget){
            widget->deleteLater();
        }
        qDebug() << __FILE__ << __LINE__ << i << tabWidgetCount;
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

        const auto vehicleID = vehicle->id();
        const auto vehicleWeaponId = vehicle->weaponId();

        const QString& strVehicleName = QGCApplication::vehicleName(vehicleWeaponId, vehicleID);
        QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);

        ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->addItem(strVehicleName, userData);

        UAVFlyManage_MavlinkParameterManageEditDialog* pUAVFlyManage_MavlinkParameterManageEditDialog
                = new UAVFlyManage_MavlinkParameterManageEditDialog(
                    ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles,
                    vehicle,
                    vehicleID);

        pUAVFlyManage_MavlinkParameterManageEditDialog->setProperty(UAVFLYMANAGE_MAVLINKPARAMETERMANAGEEDITDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID, userData);

        ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->addTab(
                    pUAVFlyManage_MavlinkParameterManageEditDialog,
                    strVehicleName);
        m_UAVFlyManage_MavlinkParameterManageEditDialogList.append(pUAVFlyManage_MavlinkParameterManageEditDialog);
    }
}

void UAVFlyManage_MavlinkParameterManageDialog::on_tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles_currentChanged(int index)
{
    if(index < 0){
        return ;
    }

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles){
        return ;
    }

    const auto& tabWidgeCurrentIndex = ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->currentIndex();
    if(tabWidgeCurrentIndex < 0 || tabWidgeCurrentIndex >= ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->count()){
        return ;
    }

    ui->label_UAVFlyManage_MavlinkParameterManage_VehiclesInfo->setText("");

    auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkParameterManageEditDialog*>(ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->currentWidget());
    if(!tabWidgeCurrentWidget){
        return ;
    }

    const QString& strVehicleWeaponId_VehicleID = tabWidgeCurrentWidget->property(UAVFLYMANAGE_MAVLINKPARAMETERMANAGEEDITDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID).toString();
    if(strVehicleWeaponId_VehicleID.isEmpty()){
        return ;
    }

    tabWidgeCurrentWidget->setSelected(true);
    ui->label_UAVFlyManage_MavlinkParameterManage_VehiclesInfo->setText(tabWidgeCurrentWidget->getVehicleInfo());

    const auto& tabWidgetCurrentText = ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->tabText(tabWidgeCurrentIndex);
    const auto& curIndex = ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->findText(tabWidgetCurrentText);
    if(curIndex < 0 || curIndex >= ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->count()){
        return ;
    }

    const auto& curDataIndex = ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->findData(strVehicleWeaponId_VehicleID);
    if(curDataIndex < 0 || curDataIndex >= ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->count()){
        return ;
    }

    if(curDataIndex != curIndex){
        LyMessageBox::warning(nullptr, tr("Tip"), tr("Parameter Manage : Vehicle id and name of tab and combox do not match, the configuration is error!"), QMessageBox::Yes);
        return ;
    }

    ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->setCurrentIndex(curIndex);
}

void UAVFlyManage_MavlinkParameterManageDialog::on_comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles_currentTextChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles){
        return ;
    }

    if(!ui->label_UAVFlyManage_MavlinkParameterManage_VehiclesInfo){
        return ;
    }

    ui->label_UAVFlyManage_MavlinkParameterManage_VehiclesInfo->setText("");

    const auto& curText = ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->currentText();
    if(curText.isEmpty()){
        return ;
    }

    const auto& curData = ui->comboBox_UAVFlyManage_MavlinkParameterManage_Vehicles->currentData().toString();
    if(curData.isEmpty()){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->count();
    if(tabWidgetCount <= 0){
        return ;
    }

    bool findTabWidget = false;
    for(int i = 0 ; i < tabWidgetCount; i++){
        const auto& tabWidgetText = ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->tabText(i);
        auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkParameterManageEditDialog*>(ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->widget(i));
        if(tabWidgeCurrentWidget && tabWidgetText == curText){
            const QString& strVehicleWeaponId_VehicleID = tabWidgeCurrentWidget->property(UAVFLYMANAGE_MAVLINKPARAMETERMANAGEEDITDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID).toString();
            if(!strVehicleWeaponId_VehicleID.isEmpty() && curData == strVehicleWeaponId_VehicleID){
                ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->setCurrentIndex(i);

                auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkParameterManageEditDialog*>(ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->currentWidget());
                if(tabWidgeCurrentWidget){
                    tabWidgeCurrentWidget->setSelected(true);
                    ui->label_UAVFlyManage_MavlinkParameterManage_VehiclesInfo->setText(tabWidgeCurrentWidget->getVehicleInfo());
                }
                findTabWidget = true;
                break ;
            }
        }
    }

    if(!findTabWidget){
        LyMessageBox::warning(nullptr, tr("Tip"), tr("Parameter Manage : Vehicle id and name of combox and tab do not match, the configuration is error!"), QMessageBox::Yes);
    }
}

void UAVFlyManage_MavlinkParameterManageDialog::on_pushButton_UAVFlyManage_MavlinkParameterManage_RefreshAllVehicleParameters_clicked()
{

    if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Tip"), tr("Are you refresh all vehicle parameters?"), QMessageBox::Yes | QMessageBox::No)) {
        if(!ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles){
            return ;
        }

        if(!ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles){
            return ;
        }

        const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->count();
        for(int i = 0 ; i < tabWidgetCount; i++){
            UAVFlyManage_MavlinkParameterManageEditDialog *widget = qobject_cast<UAVFlyManage_MavlinkParameterManageEditDialog*>(ui->tabWidget_UAVFlyManage_MavlinkParameterManage_Vehicles->widget(i));
            if(widget){
                widget->refreshAllParameters();
            }
        }
    }
}
