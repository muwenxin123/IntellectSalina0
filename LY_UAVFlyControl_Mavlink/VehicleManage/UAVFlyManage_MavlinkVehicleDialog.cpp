#include "UAVFlyManage_MavlinkVehicleDialog.h"
#include "ui_UAVFlyManage_MavlinkVehicleDialog.h"

#include "Controls/UAVFlyManage_MavlinkVehicleControlsDialog.h"
#include "StatusInfo/UAVFlyManage_MavlinkVehicleStatusInfoDialog.h"
#include "Parameter/UAVFlyManage_MavlinkParameterManageEditDialog.h"

#include "QGCApplication.h"
#include "Vehicle.h"

UAVFlyManage_MavlinkVehicleDialog::UAVFlyManage_MavlinkVehicleDialog(QWidget *parent, Vehicle *vehicle, int vehicleId)
    : QWidget(parent)
    , ui(new Ui::UAVFlyManage_MavlinkVehicleDialog)
    , m_Show(false)
    , m_vehicle(vehicle)
    , m_vehicleId(vehicleId)
    , m_Selected(false)
    , m_needOnMouseDoubleClickEvent(false)
    , m_pUAVFlyManage_MavlinkVehicleControlsDialog(nullptr)
    , m_pUAVFlyManage_MavlinkVehicleStatusInfoDialog(nullptr)
    , m_pUAVFlyManage_MavlinkParameterManageEditDialog(nullptr)
{
    ui->setupUi(this);

    InitVehicleInfoList();
}

UAVFlyManage_MavlinkVehicleDialog::~UAVFlyManage_MavlinkVehicleDialog()
{
    delete ui;
}

void UAVFlyManage_MavlinkVehicleDialog::changeEvent(QEvent *e)
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

void UAVFlyManage_MavlinkVehicleDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)
    m_Show = true;

}

void UAVFlyManage_MavlinkVehicleDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    m_Show                        = false;
    m_Selected                    = false;
    m_needOnMouseDoubleClickEvent = false;
}

Vehicle *UAVFlyManage_MavlinkVehicleDialog::getVehicle() const
{
    return m_vehicle;
}

QString UAVFlyManage_MavlinkVehicleDialog::getVehicleInfo() const
{
    if(getVehicle()){
        return (" " + getVehicle()->firmwareTypeString() + " : " + getVehicle()->vehicleTypeString());
    }
    return QString("No Vehicle!");
}

bool UAVFlyManage_MavlinkVehicleDialog::getSelected() const
{
    return m_Selected;
}

void UAVFlyManage_MavlinkVehicleDialog::setSelected(bool Selected)
{
    m_Selected = Selected;
}

void UAVFlyManage_MavlinkVehicleDialog::DrawDynamic(QPainter &painter, IProj *pProj)
{
    if(!m_needOnMouseDoubleClickEvent){
        return ;
    }

    if(m_pUAVFlyManage_MavlinkVehicleControlsDialog){
        m_pUAVFlyManage_MavlinkVehicleControlsDialog->DrawDynamic(painter, pProj);
    }
}

void UAVFlyManage_MavlinkVehicleDialog::OnMouseButtonDown(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleDialog::OnMouseButtonRelease(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleDialog::OnMouseDoubleClick(QMouseEvent *e, IProj *pProj)
{
    if(!m_needOnMouseDoubleClickEvent){
        return ;
    }

    if(m_pUAVFlyManage_MavlinkVehicleControlsDialog){
        m_pUAVFlyManage_MavlinkVehicleControlsDialog->OnMouseDoubleClick(e, pProj);
    }
}

void UAVFlyManage_MavlinkVehicleDialog::OnMouseMove(QMouseEvent *e, IProj *pProj)
{
    Q_UNUSED(e)
    Q_UNUSED(pProj)
}

void UAVFlyManage_MavlinkVehicleDialog::InitVehicleInfoList()
{

    if(!ui){
        return ;
    }

    if(!ui->tabWidget_UAVFlyManage_MavlinkVehicleDialog){
        return ;
    }

    const auto& tabWidgetCount = ui->tabWidget_UAVFlyManage_MavlinkVehicleDialog->count();
    for(int i = 0; i < tabWidgetCount; i++){
        QWidget *widget = ui->tabWidget_UAVFlyManage_MavlinkVehicleDialog->widget(i);
        ui->tabWidget_UAVFlyManage_MavlinkVehicleDialog->removeTab(0);
        if(widget){
            widget->deleteLater();
        }
        qDebug() << __FILE__ << __LINE__ << i << tabWidgetCount;
    }

    if(QGCApplication::qgcApplicationMavlinkVehicleControlsDialogShow()){
        if(m_pUAVFlyManage_MavlinkVehicleControlsDialog == nullptr){
            m_pUAVFlyManage_MavlinkVehicleControlsDialog = new UAVFlyManage_MavlinkVehicleControlsDialog(this, getVehicle(), getVehicle()->id());
            ui->tabWidget_UAVFlyManage_MavlinkVehicleDialog->addTab(
                        m_pUAVFlyManage_MavlinkVehicleControlsDialog,
                        tr("Controls"));
            connect(m_pUAVFlyManage_MavlinkVehicleControlsDialog, &UAVFlyManage_MavlinkVehicleControlsDialog::needOnMouseDoubleClickEventSig, this, [this](bool need){
                this->m_needOnMouseDoubleClickEvent = need;
                emit needOnMouseDoubleClickEventSig(this->m_needOnMouseDoubleClickEvent);
            });
        }
    }

    if(QGCApplication::qgcApplicationMavlinkVehicleStatusInfoDialogShow()){
        if(m_pUAVFlyManage_MavlinkVehicleStatusInfoDialog == nullptr){
            m_pUAVFlyManage_MavlinkVehicleStatusInfoDialog = new UAVFlyManage_MavlinkVehicleStatusInfoDialog(this, getVehicle(), getVehicle()->id());
            ui->tabWidget_UAVFlyManage_MavlinkVehicleDialog->addTab(
                        m_pUAVFlyManage_MavlinkVehicleStatusInfoDialog,
                        tr("StatusInfo"));
        }
    }

    if(QGCApplication::qgcApplicationMavlinkParameterManageEditDialogShow()){
        if(m_pUAVFlyManage_MavlinkParameterManageEditDialog == nullptr){
            m_pUAVFlyManage_MavlinkParameterManageEditDialog = new UAVFlyManage_MavlinkParameterManageEditDialog(this, getVehicle(), getVehicle()->id());
            ui->tabWidget_UAVFlyManage_MavlinkVehicleDialog->addTab(
                        m_pUAVFlyManage_MavlinkParameterManageEditDialog,
                        tr("Parameter"));
        }
    }
}
