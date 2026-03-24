#include "UAVSelectDialog.h"
#include "ui_UAVSelectDialog.h"


#include "LY_UAVFlyControl_DJI_Dock.h"


#include "../djicloude/DJICloudeAPI.h"
#include "../djicloude/dji/sdk/dji_coludapi.h"
#include "../djicloude/dji/sdk/dji_coludapi.h"
#include "../djicloude/dji/sdk/dji_coludapimanager.h"
#include "../djicloude/dji/sdk/cloudapi/device/dji_dock.h"

UAVSelectDialog::UAVSelectDialog(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::UAVSelectDialog)
    , m_show(false)
    , m_InitWeaponInfoList(false)
    , m_VerticalLayout(nullptr)
    , m_FlowLayoutleftMargins(2)
    , m_FlowLayout(nullptr)
    , m_SelectCurrentWeaponComponent(nullptr)
    , m_SelectAllWeaponComponent(nullptr)
    , m_ButtonListWeaponComponent()
{
    ui->setupUi(this);

    m_ButtonListWeaponComponent.clear();

    m_VerticalLayout = new QVBoxLayout(nullptr);
    m_VerticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));

    m_FlowLayout = new QHBoxLayout;
    m_FlowLayout->setContentsMargins(m_FlowLayoutleftMargins, -1, -1, -1);

    m_InitWeaponInfoList = true;
    InitWeaponInfoList();


    DJICloudeAPI* pDJICloudeAPITemp= LY_UAVFlyControl_DJI_Dock::Get().getPDJICloudeAPI();
    if (pDJICloudeAPITemp != nullptr)
    {
        qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApi* pDjiColudApi = pDJICloudeAPITemp->pDjiCcoludApi();
        if (pDjiColudApi != nullptr)
        {
            qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApiManager* pDjiColudApiManager = pDjiColudApi->pDjiColudApiManager();
            if (pDjiColudApiManager != nullptr)
            {
                connect(pDjiColudApiManager, &qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApiManager::updateTopoOnlineDji, this, [this](const QString& gatewayS) {
                    
                    DJICloudeAPI* pDJICloudeAPITemp = LY_UAVFlyControl_DJI_Dock::Get().getPDJICloudeAPI();
                    if (pDJICloudeAPITemp == nullptr) {
                        return;
                    }

                    qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApi* pDjiColudApi = pDJICloudeAPITemp->pDjiCcoludApi();
                    if (pDjiColudApi == nullptr) {
                        return;
                    }

                    qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApiManager* pDjiColudApiManager = pDjiColudApi->pDjiColudApiManager();
                    if (pDjiColudApiManager == nullptr) {
                        return;
                    }

                    const QMap<QString, qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*>& djiGatewayDevice_MAP = pDjiColudApiManager->getDjiGatewayDevice_MAP();

                    const QMap<QString, qnzkna::DjiSdkApi::DjiCloudApi::DjiDevice*>& djiSubDevice_MAP = pDjiColudApiManager->getDjiSubDevice_MAP();

                    m_InitWeaponInfoList = true;
                    InitWeaponInfoList();

                    show();

                    });

                connect(pDjiColudApiManager, &qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApiManager::updateTopoOfflineDji, this, [this](const QString& gatewayS) {

                    DJICloudeAPI* pDJICloudeAPITemp = LY_UAVFlyControl_DJI_Dock::Get().getPDJICloudeAPI();
                    if (pDJICloudeAPITemp == nullptr) {
                        return;
                    }

                    qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApi* pDjiColudApi = pDJICloudeAPITemp->pDjiCcoludApi();
                    if (pDjiColudApi == nullptr) {
                        return;
                    }

                    qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApiManager* pDjiColudApiManager = pDjiColudApi->pDjiColudApiManager();
                    if (pDjiColudApiManager == nullptr) {
                        return;
                    }

                    m_InitWeaponInfoList = true;
                    InitWeaponInfoList();


                    });

            }
        }
    }
}

UAVSelectDialog::~UAVSelectDialog()
{
    delete ui;
}

void UAVSelectDialog::InitWeaponInfoList()
{
    if(!m_show){
        return ;
    }

    if(!m_InitWeaponInfoList){
        return ;
    }

    if(!m_VerticalLayout || !m_FlowLayout){
        return ;
    }


    DJICloudeAPI* pDJICloudeAPITemp = LY_UAVFlyControl_DJI_Dock::Get().getPDJICloudeAPI();
    if(!pDJICloudeAPITemp){
        return ;
    }

    qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApi* pDjiColudApi = pDJICloudeAPITemp->pDjiCcoludApi();
    if (pDjiColudApi == nullptr) {
        return;
    }

    qnzkna::DjiSdkApi::DjiCloudApi::DjiColudApiManager* pDjiColudApiManager = pDjiColudApi->pDjiColudApiManager();
    if (pDjiColudApiManager == nullptr) {
        return;
    }

    m_ButtonListWeaponComponent.clear();


    const QMap<QString, qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*>&   djiGatewayDevice_MAP = pDjiColudApiManager->getDjiGatewayDevice_MAP();

    const QMap<QString, qnzkna::DjiSdkApi::DjiCloudApi::DjiDevice*>& djiSubDevice_MAP = pDjiColudApiManager->getDjiSubDevice_MAP();

    if (djiGatewayDevice_MAP.size() <= 0) {
        return;
    }

    const auto& vehiclesList = djiGatewayDevice_MAP.values();
    if(vehiclesList.size() <= 0){
        return ;
    }

    QList<qnzkna::DjiSdkApi::DjiCloudApi::DjiDock*> vehiclesListTemp(vehiclesList);

    //std::sort(vehiclesListTemp.begin(), vehiclesListTemp.end(),
    //          [](const auto* vehicle1, const auto* vehicle2) {
    //    return (vehicle1->weaponId() < vehicle2->weaponId());
    //} );

    QLayoutItem *child;
    while ((child = m_FlowLayout->takeAt(0)) != nullptr) {
        m_FlowLayout->removeWidget(child->widget());
        child->widget()->hide();
        child->widget()->setParent(nullptr);
        child->widget()->deleteLater();
        delete child;
    }

    if(m_SelectAllWeaponComponent == nullptr){
        m_SelectAllWeaponComponent = new UAVPushButton(0, 0, nullptr, QT_TRANSLATE_NOOP("UAVSelectDialog", tr("\xe9\x80\x89\xe4\xb8\xad\xe6\x89\x80\xe6\x9c\x89\xe6\x97\xa0\xe4\xba\xba\xe6\x9c\xba")), "ALL", "0x00", this);
        if(m_SelectAllWeaponComponent == nullptr){
            return;
        }

        connect(m_SelectAllWeaponComponent, &UAVPushButton::clicked, this, &UAVSelectDialog::SelectAllWeaponComponent);
        m_SelectAllWeaponComponent->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum));
        m_SelectAllWeaponComponent->setMinimumSize(QSize(40, 2));
        m_SelectAllWeaponComponent->setCheckable(true);
        if(m_SelectAllWeaponComponent != nullptr){
            m_VerticalLayout->addWidget(m_SelectAllWeaponComponent);
        }
        m_VerticalLayout->addLayout(m_FlowLayout);
    }

    const auto vehiclesCount = vehiclesListTemp.count();
    for (int i = 0; i < vehiclesCount; i++) {
        const qnzkna::DjiSdkApi::DjiCloudApi::DjiDock* vehicle = (vehiclesListTemp.at(i));
        if(!vehicle){
            continue ;
        }

        const auto vehicleWeaponId = vehicle->weaponId();
        const auto vehicleID       = vehicle->id();

        const QString& strVehicleName = vehicle->getGatewaySn();// QGCApplication::vehicleName(vehicleWeaponId, vehicleID);
        UAVPushButton* weaponComponentButton = new UAVPushButton(vehicleWeaponId, vehicleID, vehicle, strVehicleName, QString(vehicleWeaponId), "0x00", this);
        if (weaponComponentButton == nullptr) {
            continue;
        }
        weaponComponentButton->hide();
        connect(weaponComponentButton, &UAVPushButton::clicked, this, &UAVSelectDialog::SelectSingleWeaponComponent);

        weaponComponentButton->setSizePolicy(QSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding));
        weaponComponentButton->setMinimumSize(QSize(40, 2));
        weaponComponentButton->setCheckable(true);

        if (weaponComponentButton != nullptr) {
            m_ButtonListWeaponComponent.insert(weaponComponentButton, vehicleID);
            m_FlowLayout->addWidget(weaponComponentButton);
        }

        weaponComponentButton->show();

        qDebug() << __FILE__ << __LINE__ << vehiclesCount << " " << vehicleID << " " << vehicleWeaponId << " " << i;

    }

    m_VerticalLayout->setStretch(0, 1);
    int stretch = m_ButtonListWeaponComponent.size() / 2;
    if(stretch <= 0){
        stretch = 1;
    }
    m_VerticalLayout->setStretch(1, stretch);

    ui->scrollAreaWidgetContents->setLayout(m_VerticalLayout);

    m_InitWeaponInfoList = false;

    if(m_SelectAllWeaponComponent != nullptr){
        m_SelectAllWeaponComponent->setVisible(false);
        m_SelectAllWeaponComponent->setEnabled(false);
        m_SelectAllWeaponComponent->hide();
    }
}

void UAVSelectDialog::SelectAllWeaponComponent()
{
    m_SelectCurrentWeaponComponent = nullptr;
    for (UAVPushButton *item : m_ButtonListWeaponComponent.keys()) {
        item->setChecked(false);
    }
    m_SelectCurrentWeaponComponent = m_SelectAllWeaponComponent;
}

void UAVSelectDialog::SelectSingleWeaponComponent()
{
    m_SelectCurrentWeaponComponent = nullptr;
    if(m_SelectAllWeaponComponent && (m_SelectAllWeaponComponent->isChecked())){
        m_SelectAllWeaponComponent->setChecked(false);
    }

    for (UAVPushButton *item : m_ButtonListWeaponComponent.keys()) {
        if(item->isChecked()){
            m_SelectCurrentWeaponComponent = item;
            break;
        }
    }
}

void UAVSelectDialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)

}

void UAVSelectDialog::showEvent(QShowEvent *event)
{
    Q_UNUSED(event)

    m_show = true;

    if(m_InitWeaponInfoList){
        InitWeaponInfoList();
    }
}

void UAVSelectDialog::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)

    m_show = false;
}

const QMap<UAVPushButton *, QString> UAVSelectDialog::getButtonListWeaponComponent() const
{
    return m_ButtonListWeaponComponent;
}

const UAVPushButton* UAVSelectDialog::getSelectAllWeaponComponent() const
{
    return m_SelectAllWeaponComponent;
}

const UAVPushButton* UAVSelectDialog::getSelectCurrentWeaponComponent() const
{
    return m_SelectCurrentWeaponComponent;
}
