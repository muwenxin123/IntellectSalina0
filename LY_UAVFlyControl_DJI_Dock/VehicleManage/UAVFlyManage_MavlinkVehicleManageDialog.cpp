#include "UAVFlyManage_MavlinkVehicleManageDialog.h"
#include "ui_UAVFlyManage_MavlinkVehicleManageDialog.h"

#include "lyUI/LyMessageBox.h"

#include "UAVFlyManage_MavlinkVehicleDialog.h"

#include "QGCApplication.h"
#include "MultiVehicleManager.h"
#include "Vehicle.h"
#include "MissionItem.h"

#define UAVFLYMANAGE_MAVLINKVEHICLEMANAGEDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID "VehicleWeaponId_VehicleID"

UAVFlyManage_MavlinkVehicleManageDialog::UAVFlyManage_MavlinkVehicleManageDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkVehicleManageDialog)
    , m_needOnMouseDoubleClickEvent(false)
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
}

UAVFlyManage_MavlinkVehicleManageDialog::~UAVFlyManage_MavlinkVehicleManageDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkVehicleManageDialog::DrawDynamic(QPainter &painter, IProj *pProj)
{
    if(!m_needOnMouseDoubleClickEvent){
        return ;
    }

    if(!ui){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkVehicleDialog*>(ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->currentWidget());
    if(tabWidgeCurrentWidget){
        tabWidgeCurrentWidget->DrawDynamic(painter, pProj);
    }
}

void UAVFlyManage_MavlinkVehicleManageDialog::OnMouseButtonDown(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleManageDialog::OnMouseButtonRelease(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleManageDialog::OnMouseDoubleClick(QMouseEvent *e, IProj *pProj)
{
    if(!m_needOnMouseDoubleClickEvent){
        return ;
    }

    if(!ui){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkVehicleDialog*>(ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->currentWidget());
    if(tabWidgeCurrentWidget){
        tabWidgeCurrentWidget->OnMouseDoubleClick(e, pProj);
    }
}

void UAVFlyManage_MavlinkVehicleManageDialog::OnMouseMove(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleManageDialog::changeEvent(QEvent *e)
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

void UAVFlyManage_MavlinkVehicleManageDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
}

void UAVFlyManage_MavlinkVehicleManageDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    m_needOnMouseDoubleClickEvent = false;
}

void UAVFlyManage_MavlinkVehicleManageDialog::InitWeaponInfoList()
{

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    m_UAVFlyManage_MavlinkVehicleDialogList.clear();
    ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->clear();

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->count();
    for(int i = 0; i < tabWidgetCount; i++){
        QWidget *widget = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->widget(i);
        ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->removeTab(0);
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

        ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->addItem(strVehicleName, userData);

        UAVFlyManage_MavlinkVehicleDialog* pUAVFlyManage_MavlinkVehicleDialog
                = new UAVFlyManage_MavlinkVehicleDialog(
                    ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles,
                    vehicle,
                    vehicleID);

        pUAVFlyManage_MavlinkVehicleDialog->setProperty(UAVFLYMANAGE_MAVLINKVEHICLEMANAGEDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID, userData);

        ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->addTab(
                    pUAVFlyManage_MavlinkVehicleDialog,
                    strVehicleName);
        m_UAVFlyManage_MavlinkVehicleDialogList.append(pUAVFlyManage_MavlinkVehicleDialog);

        connect(pUAVFlyManage_MavlinkVehicleDialog, &UAVFlyManage_MavlinkVehicleDialog::needOnMouseDoubleClickEventSig, this, [this](bool need){
            this->m_needOnMouseDoubleClickEvent = need;
        });
    }
}

void UAVFlyManage_MavlinkVehicleManageDialog::InitWeaponInfoListAdded(Vehicle *vehicleAdded)
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    if(!MavlinkApp()){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->count();
    for(int i = 0 ; i < tabWidgetCount; i++){
        UAVFlyManage_MavlinkVehicleDialog *widget = qobject_cast<UAVFlyManage_MavlinkVehicleDialog*>(ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->widget(i));
        if(widget && widget->getVehicle() == vehicleAdded){
            return ;
        }
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

        ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->insertItem(i, strVehicleName, userData);

        UAVFlyManage_MavlinkVehicleDialog* pUAVFlyManage_MavlinkVehicleDialog
                = new UAVFlyManage_MavlinkVehicleDialog(
                    ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles,
                    vehicle,
                    vehicleID);

        pUAVFlyManage_MavlinkVehicleDialog->setProperty(UAVFLYMANAGE_MAVLINKVEHICLEMANAGEDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID, userData);

        ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->insertTab(i,
                    pUAVFlyManage_MavlinkVehicleDialog,
                    strVehicleName);
        m_UAVFlyManage_MavlinkVehicleDialogList.insert(i, pUAVFlyManage_MavlinkVehicleDialog);

        connect(pUAVFlyManage_MavlinkVehicleDialog, &UAVFlyManage_MavlinkVehicleDialog::needOnMouseDoubleClickEventSig, this, [this](bool need){
            this->m_needOnMouseDoubleClickEvent = need;
        });
    }
}

void UAVFlyManage_MavlinkVehicleManageDialog::InitWeaponInfoListRemoved(Vehicle *vehicleRemoved)
{
    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->count();
    for(int i = 0 ; i < tabWidgetCount; i++){
        UAVFlyManage_MavlinkVehicleDialog *widget_Vehicles = qobject_cast<UAVFlyManage_MavlinkVehicleDialog*>(ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->widget(i));
        if(widget_Vehicles && widget_Vehicles->getVehicle() == vehicleRemoved){
            QWidget *widget = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->widget(i);
            ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->removeTab(i);
            if(widget){
                widget->deleteLater();
            }

            for(int comboxIndex = 0; comboxIndex < ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->count(); comboxIndex++){
                const auto vehicleID       = widget_Vehicles->getVehicle()->id();
                const auto vehicleWeaponId = widget_Vehicles->getVehicle()->weaponId();
                QString userData = QString("%1%2").arg(vehicleWeaponId).arg(vehicleID);
                if(ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->itemData(comboxIndex).toString() == userData){
                    ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->removeItem(comboxIndex);
                    break;
                }
            }

            m_UAVFlyManage_MavlinkVehicleDialogList.removeOne(widget_Vehicles);
            break;
        }
    }
}

void UAVFlyManage_MavlinkVehicleManageDialog::on_tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles_currentChanged(int index)
{
    if(index < 0){
        return ;
    }

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    const auto& tabWidgeCurrentIndex = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->currentIndex();
    if(tabWidgeCurrentIndex < 0 || tabWidgeCurrentIndex >= ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->count()){
        return ;
    }

    ui->label_UAVFlyManage_MavlinkVehicleManage_VehiclesInfo->setText("");

    auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkVehicleDialog*>(ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->currentWidget());
    if(!tabWidgeCurrentWidget){
        return ;
    }

    const QString& strVehicleWeaponId_VehicleID = tabWidgeCurrentWidget->property(UAVFLYMANAGE_MAVLINKVEHICLEMANAGEDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID).toString();
    if(strVehicleWeaponId_VehicleID.isEmpty()){
        return ;
    }

    tabWidgeCurrentWidget->setSelected(true);
    ui->label_UAVFlyManage_MavlinkVehicleManage_VehiclesInfo->setText(tabWidgeCurrentWidget->getVehicleInfo());

    const auto& tabWidgetCurrentText = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->tabText(tabWidgeCurrentIndex);
    const auto& curIndex = ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->findText(tabWidgetCurrentText);
    if(curIndex < 0 || curIndex >= ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->count()){
        return ;
    }

    const auto& curDataIndex = ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->findData(strVehicleWeaponId_VehicleID);
    if(curDataIndex < 0 || curDataIndex >= ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->count()){
        return ;
    }

    if(curDataIndex != curIndex){
        LyMessageBox::warning(nullptr, tr("Tip"), tr("Vehicle Manage : Vehicle id and name of tab and combox do not match, the configuration is error!"), QMessageBox::Yes);
        return ;
    }

    ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->setCurrentIndex(curIndex);
}

void UAVFlyManage_MavlinkVehicleManageDialog::on_comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)

    if(!ui){
        return ;
    }

    if(!ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles){
        return ;
    }

    if(!ui->label_UAVFlyManage_MavlinkVehicleManage_VehiclesInfo){
        return ;
    }

    ui->label_UAVFlyManage_MavlinkVehicleManage_VehiclesInfo->setText("");

    const auto& curText = ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->currentText();
    if(curText.isEmpty()){
        return ;
    }

    const auto& curData = ui->comboBox_UAVFlyManage_MavlinkVehicleManage_Vehicles->currentData().toString();
    if(curData.isEmpty()){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->count();
    if(tabWidgetCount <= 0){
        return ;
    }

    bool findTabWidget = false;
    for(int i = 0 ; i < tabWidgetCount; i++){
        const auto& tabWidgetText = ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->tabText(i);
        auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkVehicleDialog*>(ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->widget(i));
        if(tabWidgeCurrentWidget && tabWidgetText == curText){
            const QString& strVehicleWeaponId_VehicleID = tabWidgeCurrentWidget->property(UAVFLYMANAGE_MAVLINKVEHICLEMANAGEDIALOG_PROPERTY_VEHICLEWEAPONID_VEHICLEID).toString();
            if(!strVehicleWeaponId_VehicleID.isEmpty() && curData == strVehicleWeaponId_VehicleID){
                ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->setCurrentIndex(i);

                auto* tabWidgeCurrentWidget = qobject_cast<UAVFlyManage_MavlinkVehicleDialog*>(ui->tabWidget_UAVFlyManage_MavlinkVehicleManage_Vehicles->currentWidget());
                if(tabWidgeCurrentWidget){
                    tabWidgeCurrentWidget->setSelected(true);
                    ui->label_UAVFlyManage_MavlinkVehicleManage_VehiclesInfo->setText(tabWidgeCurrentWidget->getVehicleInfo());
                }
                findTabWidget = true;
                break ;
            }
        }
    }

    if(!findTabWidget){
        LyMessageBox::warning(nullptr, tr("Tip"), tr("Vehicle Manage : Vehicle id and name of combox and tab do not match, the configuration is error!"), QMessageBox::Yes);
    }
}
