#include "UAVNavigationArmShotAutoAiming.h"

#include <float.h>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>

#include "LyMessageBox.h"

#include "IXyhcActivator.h"
#include "lyXyhc.h"

#include "Protocol/DY_ZC_Control/DY_ZC_Control.h"

#include <QGeoCoordinate>

LyMessageBoxCruise::LyMessageBoxCruise(const std::string &strComponentID, const std::string &strComponentName, UAVNavigationArmShotAutoAiming *pUAVNavigationArmShotAutoAiming, QWidget *parent)
    : LyDialog("", parent)
    , m_pUAVNavigationArmShotAutoAiming(pUAVNavigationArmShotAutoAiming)
    , m_strComponentID(strComponentID)
    , m_strComponentName(strComponentName)
    , m_LongitudeUAV(0)
    , m_LatitudeUAV(0)
    , m_AltitudeUAV(0)
    , m_AltitudeRelUAV(0)
    , m_LongitudeMark(0)
    , m_LatitudeMark(0)
    , m_AltitudeMark(0)
    , m_LongitudeFlyGoto(0)
    , m_LatitudeFlyGoto(0)
    , m_AltitudeFlyGoto(20.0)
    , m_AltitudeFlyGoto_Mark_Difference(300.0)
    , m_VelocityFlyGoto(10)
    , m_RadiusFlyGoto(25)
    , m_TimeFlyGoto(3000)
    , m_ModeFlyGoto(0)
{
    setWindowTitle(QStringLiteral("%1(%2):%3").arg(tr(m_strComponentName.c_str())).arg(QString::fromStdString(m_strComponentID)).arg(tr("瞄准")));
    QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);

    QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sizePolicy1.setHorizontalStretch(0);
    sizePolicy1.setVerticalStretch(0);

    QSizePolicy sizePolicy2(QSizePolicy::Ignored, QSizePolicy::Ignored);
    sizePolicy2.setHorizontalStretch(0);
    sizePolicy2.setVerticalStretch(0);

    widget_FlyGoto = new QWidget(centralWidget());
    widget_FlyGoto->setObjectName(QLatin1String("widget_FlyGoto"));
    sizePolicy.setHeightForWidth(widget_FlyGoto->sizePolicy().hasHeightForWidth());
    widget_FlyGoto->setSizePolicy(sizePolicy);
    widget_FlyGoto->setMinimumSize(QSize(2, 2));
    verticalLayout = new QVBoxLayout(widget_FlyGoto);
    verticalLayout->setSpacing(0);
    verticalLayout->setObjectName(QLatin1String("verticalLayout"));
    verticalLayout->setContentsMargins(0, 0, 0, 0);
    groupBox_FlyGoto = new QGroupBox(widget_FlyGoto);
    groupBox_FlyGoto->setObjectName(QLatin1String("groupBox_FlyGoto"));
    sizePolicy.setHeightForWidth(groupBox_FlyGoto->sizePolicy().hasHeightForWidth());
    groupBox_FlyGoto->setSizePolicy(sizePolicy);
    groupBox_FlyGoto->setMinimumSize(QSize(2, 2));
    gridLayout_4 = new QGridLayout(groupBox_FlyGoto);
    gridLayout_4->setObjectName(QLatin1String("gridLayout_4"));
    label_FlyGoto_Longitude = new QLabel(groupBox_FlyGoto);
    label_FlyGoto_Longitude->setObjectName(QLatin1String("label_FlyGoto_Longitude"));
    sizePolicy.setHeightForWidth(label_FlyGoto_Longitude->sizePolicy().hasHeightForWidth());
    label_FlyGoto_Longitude->setSizePolicy(sizePolicy);
    label_FlyGoto_Longitude->setMinimumSize(QSize(2, 2));

    gridLayout_4->addWidget(label_FlyGoto_Longitude, 0, 0, 1, 1);

    doubleSpinBox_FlyGoto_Longitude = new QDoubleSpinBox(groupBox_FlyGoto);
    doubleSpinBox_FlyGoto_Longitude->setObjectName(QLatin1String("doubleSpinBox_FlyGoto_Longitude"));
    QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Minimum);
    sizePolicy3.setHorizontalStretch(0);
    sizePolicy3.setVerticalStretch(0);
    sizePolicy3.setHeightForWidth(doubleSpinBox_FlyGoto_Longitude->sizePolicy().hasHeightForWidth());
    doubleSpinBox_FlyGoto_Longitude->setSizePolicy(sizePolicy3);
    doubleSpinBox_FlyGoto_Longitude->setMinimumSize(QSize(2, 2));
    doubleSpinBox_FlyGoto_Longitude->setDecimals(7);
    doubleSpinBox_FlyGoto_Longitude->setMinimum(-180.000000000000000);
    doubleSpinBox_FlyGoto_Longitude->setMaximum(180.000000000000000);
    doubleSpinBox_FlyGoto_Longitude->setSingleStep(0.000001000000000);

    gridLayout_4->addWidget(doubleSpinBox_FlyGoto_Longitude, 0, 1, 1, 2);

    label_FlyGoto_Latitude = new QLabel(groupBox_FlyGoto);
    label_FlyGoto_Latitude->setObjectName(QLatin1String("label_FlyGoto_Latitude"));
    sizePolicy.setHeightForWidth(label_FlyGoto_Latitude->sizePolicy().hasHeightForWidth());
    label_FlyGoto_Latitude->setSizePolicy(sizePolicy);
    label_FlyGoto_Latitude->setMinimumSize(QSize(2, 2));

    gridLayout_4->addWidget(label_FlyGoto_Latitude, 1, 0, 1, 1);

    doubleSpinBox_FlyGoto_Latitude = new QDoubleSpinBox(groupBox_FlyGoto);
    doubleSpinBox_FlyGoto_Latitude->setObjectName(QLatin1String("doubleSpinBox_FlyGoto_Latitude"));
    sizePolicy3.setHeightForWidth(doubleSpinBox_FlyGoto_Latitude->sizePolicy().hasHeightForWidth());
    doubleSpinBox_FlyGoto_Latitude->setSizePolicy(sizePolicy3);
    doubleSpinBox_FlyGoto_Latitude->setMinimumSize(QSize(2, 2));
    doubleSpinBox_FlyGoto_Latitude->setDecimals(7);
    doubleSpinBox_FlyGoto_Latitude->setMinimum(-90.000000000000000);
    doubleSpinBox_FlyGoto_Latitude->setMaximum(90.000000000000000);
    doubleSpinBox_FlyGoto_Latitude->setSingleStep(0.000001000000000);

    gridLayout_4->addWidget(doubleSpinBox_FlyGoto_Latitude, 1, 1, 1, 2);

    label_FlyGoto_Altitude = new QLabel(groupBox_FlyGoto);
    label_FlyGoto_Altitude->setObjectName(QLatin1String("label_FlyGoto_Altitude"));
    sizePolicy.setHeightForWidth(label_FlyGoto_Altitude->sizePolicy().hasHeightForWidth());
    label_FlyGoto_Altitude->setSizePolicy(sizePolicy);
    label_FlyGoto_Altitude->setMinimumSize(QSize(2, 2));

    gridLayout_4->addWidget(label_FlyGoto_Altitude, 2, 0, 1, 1);

    doubleSpinBox_FlyGoto_Altitude = new QDoubleSpinBox(groupBox_FlyGoto);
    doubleSpinBox_FlyGoto_Altitude->setObjectName(QLatin1String("doubleSpinBox_FlyGoto_Altitude"));
    sizePolicy3.setHeightForWidth(doubleSpinBox_FlyGoto_Altitude->sizePolicy().hasHeightForWidth());
    doubleSpinBox_FlyGoto_Altitude->setSizePolicy(sizePolicy3);
    doubleSpinBox_FlyGoto_Altitude->setMinimumSize(QSize(2, 2));
    doubleSpinBox_FlyGoto_Altitude->setMinimum(-100000.000000000000000);
    doubleSpinBox_FlyGoto_Altitude->setMaximum(100000.000000000000000);

    gridLayout_4->addWidget(doubleSpinBox_FlyGoto_Altitude, 2, 1, 1, 2);

    gridLayout_3 = new QGridLayout();
    gridLayout_3->setObjectName(QLatin1String("gridLayout_3"));
    label_FlyGoto_Velocity = new QLabel(groupBox_FlyGoto);
    label_FlyGoto_Velocity->setObjectName(QLatin1String("label_FlyGoto_Velocity"));
    sizePolicy.setHeightForWidth(label_FlyGoto_Velocity->sizePolicy().hasHeightForWidth());
    label_FlyGoto_Velocity->setSizePolicy(sizePolicy);
    label_FlyGoto_Velocity->setMinimumSize(QSize(2, 2));

    gridLayout_3->addWidget(label_FlyGoto_Velocity, 0, 0, 1, 1);

    spinBox_FlyGoto_Velocity = new QSpinBox(groupBox_FlyGoto);
    spinBox_FlyGoto_Velocity->setObjectName(QLatin1String("spinBox_FlyGoto_Velocity"));
    sizePolicy3.setHeightForWidth(spinBox_FlyGoto_Velocity->sizePolicy().hasHeightForWidth());
    spinBox_FlyGoto_Velocity->setSizePolicy(sizePolicy3);
    spinBox_FlyGoto_Velocity->setMinimumSize(QSize(2, 2));
    spinBox_FlyGoto_Velocity->setMaximum(100000);

    gridLayout_3->addWidget(spinBox_FlyGoto_Velocity, 0, 1, 1, 1);

    label_FlyGoto_Radius = new QLabel(groupBox_FlyGoto);
    label_FlyGoto_Radius->setObjectName(QLatin1String("label_FlyGoto_Radius"));
    sizePolicy.setHeightForWidth(label_FlyGoto_Radius->sizePolicy().hasHeightForWidth());
    label_FlyGoto_Radius->setSizePolicy(sizePolicy);
    label_FlyGoto_Radius->setMinimumSize(QSize(2, 2));

    gridLayout_3->addWidget(label_FlyGoto_Radius, 0, 2, 1, 1);

    spinBox_FlyGoto_Radius = new QSpinBox(groupBox_FlyGoto);
    spinBox_FlyGoto_Radius->setObjectName(QLatin1String("spinBox_FlyGoto_Radius"));
    sizePolicy3.setHeightForWidth(spinBox_FlyGoto_Radius->sizePolicy().hasHeightForWidth());
    spinBox_FlyGoto_Radius->setSizePolicy(sizePolicy3);
    spinBox_FlyGoto_Radius->setMinimumSize(QSize(2, 2));
    spinBox_FlyGoto_Radius->setMaximum(100000);

    gridLayout_3->addWidget(spinBox_FlyGoto_Radius, 0, 3, 1, 1);

    label_FlyGoto_Time = new QLabel(groupBox_FlyGoto);
    label_FlyGoto_Time->setObjectName(QLatin1String("label_FlyGoto_Time"));
    sizePolicy.setHeightForWidth(label_FlyGoto_Time->sizePolicy().hasHeightForWidth());
    label_FlyGoto_Time->setSizePolicy(sizePolicy);
    label_FlyGoto_Time->setMinimumSize(QSize(2, 2));

    gridLayout_3->addWidget(label_FlyGoto_Time, 1, 0, 1, 1);

    spinBox_FlyGoto_Time = new QSpinBox(groupBox_FlyGoto);
    spinBox_FlyGoto_Time->setObjectName(QLatin1String("spinBox_FlyGoto_Time"));
    sizePolicy3.setHeightForWidth(spinBox_FlyGoto_Time->sizePolicy().hasHeightForWidth());
    spinBox_FlyGoto_Time->setSizePolicy(sizePolicy3);
    spinBox_FlyGoto_Time->setMinimumSize(QSize(2, 2));
    spinBox_FlyGoto_Time->setMaximum(100000);

    gridLayout_3->addWidget(spinBox_FlyGoto_Time, 1, 1, 1, 1);

    label_FlyGoto_Mode = new QLabel(groupBox_FlyGoto);
    label_FlyGoto_Mode->setObjectName(QLatin1String("label_FlyGoto_Mode"));
    sizePolicy.setHeightForWidth(label_FlyGoto_Mode->sizePolicy().hasHeightForWidth());
    label_FlyGoto_Mode->setSizePolicy(sizePolicy);
    label_FlyGoto_Mode->setMinimumSize(QSize(2, 2));

    gridLayout_3->addWidget(label_FlyGoto_Mode, 1, 2, 1, 1);

    comboBox_FlyGoto_Mode = new QComboBox(groupBox_FlyGoto);
    comboBox_FlyGoto_Mode->setObjectName(QLatin1String("comboBox_FlyGoto_Mode"));
    QSizePolicy sizePolicy4(QSizePolicy::Expanding, QSizePolicy::Ignored);
    sizePolicy4.setHorizontalStretch(0);
    sizePolicy4.setVerticalStretch(0);
    sizePolicy4.setHeightForWidth(comboBox_FlyGoto_Mode->sizePolicy().hasHeightForWidth());
    comboBox_FlyGoto_Mode->setSizePolicy(sizePolicy4);
    comboBox_FlyGoto_Mode->setMinimumSize(QSize(2, 2));

    gridLayout_3->addWidget(comboBox_FlyGoto_Mode, 1, 3, 1, 1);

    gridLayout_4->addLayout(gridLayout_3, 3, 0, 1, 3);

    pushButton_FlyGoto_Cancel = new QPushButton(groupBox_FlyGoto);
    pushButton_FlyGoto_Cancel->setObjectName(QLatin1String("pushButton_FlyGoto_Cancel"));
    sizePolicy1.setHeightForWidth(pushButton_FlyGoto_Cancel->sizePolicy().hasHeightForWidth());
    pushButton_FlyGoto_Cancel->setSizePolicy(sizePolicy1);
    pushButton_FlyGoto_Cancel->setMinimumSize(QSize(2, 2));

    gridLayout_4->addWidget(pushButton_FlyGoto_Cancel, 4, 1, 1, 1);

    pushButton_FlyGoto_OK = new QPushButton(groupBox_FlyGoto);
    pushButton_FlyGoto_OK->setObjectName(QLatin1String("pushButton_FlyGoto_OK"));
    sizePolicy1.setHeightForWidth(pushButton_FlyGoto_OK->sizePolicy().hasHeightForWidth());
    pushButton_FlyGoto_OK->setSizePolicy(sizePolicy1);
    pushButton_FlyGoto_OK->setMinimumSize(QSize(2, 2));

    gridLayout_4->addWidget(pushButton_FlyGoto_OK, 4, 2, 1, 1);

    {
        QGroupBox *targetPositionGroupBox = new QGroupBox(widget_FlyGoto);
        targetPositionGroupBox->setObjectName(QLatin1String("groupBox_MarkPositionGroupBox"));
        sizePolicy.setHeightForWidth(targetPositionGroupBox->sizePolicy().hasHeightForWidth());
        targetPositionGroupBox->setSizePolicy(sizePolicy);
        targetPositionGroupBox->setMinimumSize(QSize(2, 2));
        QGridLayout *targetPositionGridLayout = new QGridLayout(targetPositionGroupBox);
        targetPositionGridLayout->setObjectName(QLatin1String("gridLayout_MarkPositionGridLayout"));
        label_MarkPosition = new QLabel(targetPositionGroupBox);
        label_MarkPosition->setObjectName(QLatin1String("label_MarkPositio"));
        sizePolicy.setHeightForWidth(label_MarkPosition->sizePolicy().hasHeightForWidth());
        label_MarkPosition->setSizePolicy(sizePolicy);
        label_MarkPosition->setMinimumSize(QSize(2, 2));
        targetPositionGridLayout->addWidget(label_MarkPosition, 0, 0, 1, 1);
        verticalLayout->addWidget(targetPositionGroupBox);

        targetPositionGroupBox->setTitle(tr("Calculate the drop point"));
    }
    verticalLayout->addWidget(groupBox_FlyGoto);

    QVBoxLayout *verticalLayout_2 = new QVBoxLayout(centralWidget());
    verticalLayout_2->setObjectName(QLatin1String("verticalLayout_2"));
    verticalLayout_2->addWidget(widget_FlyGoto);

    groupBox_FlyGoto->setTitle(tr("Direct missile drop point"));
    label_FlyGoto_Longitude->setText(QApplication::translate("UAVFlyManageDialog", "Longitude", nullptr));
    label_FlyGoto_Latitude->setText(QApplication::translate("UAVFlyManageDialog", "Latitude", nullptr));
    label_FlyGoto_Altitude->setText(QApplication::translate("UAVFlyManageDialog", "Altitude", nullptr));
    label_FlyGoto_Altitude->setText(tr("height(R)"));
    label_FlyGoto_Velocity->setText(QApplication::translate("UAVFlyManageDialog", "Speed", nullptr));
    label_FlyGoto_Radius->setText(QApplication::translate("UAVFlyManageDialog", "Radius", nullptr));
    label_FlyGoto_Time->setText(QApplication::translate("UAVFlyManageDialog", "Time", nullptr));
    label_FlyGoto_Mode->setText(QApplication::translate("UAVFlyManageDialog", "Mode", nullptr));
    pushButton_FlyGoto_Cancel->setText(tr("cancel"));
    pushButton_FlyGoto_OK->setText(tr("confirm"));

    QSettings settings("../data/conf/UAVNavigationArmShotAutoAimingConfig.ini", QSettings::IniFormat);
    settings.setIniCodec(QTextCodec::codecForName("utf-8"));
    settings.beginGroup("LY_UAVNAVIGATIONARMSHOT_AUTOAIMING_CRUISE_SETTINGS");

    if (settings.contains("UAVNavigationArmShotAutoAiming_FlyGoto_Speed") && settings.value("UAVNavigationArmShotAutoAiming_FlyGoto_Speed").canConvert(QVariant::Type::Int))
    {
        m_VelocityFlyGoto    = settings.value("UAVNavigationArmShotAutoAiming_FlyGoto_Speed", "0").toInt();
        if (m_VelocityFlyGoto <= 0)
        {
            m_VelocityFlyGoto = 10;
        }
    }

    if (settings.contains("UAVNavigationArmShotAutoAiming_FlyGoto_Time") && settings.value("UAVNavigationArmShotAutoAiming_FlyGoto_Time").canConvert(QVariant::Type::Int))
    {
        m_TimeFlyGoto    = settings.value("UAVNavigationArmShotAutoAiming_FlyGoto_Time", "0").toInt();
        if (m_TimeFlyGoto <= 0)
        {
            m_TimeFlyGoto = 3000;
        }
    }

    if (settings.contains("UAVNavigationArmShotAutoAiming_FlyGoto_Altitude") && settings.value("UAVNavigationArmShotAutoAiming_FlyGoto_Altitude").canConvert(QVariant::Type::Double))
    {
        m_AltitudeFlyGoto    = settings.value("UAVNavigationArmShotAutoAiming_FlyGoto_Altitude", "0").toDouble();
        if (m_AltitudeFlyGoto <= 0)
        {
            m_AltitudeFlyGoto = 20.0;
        }
    }

    if (settings.contains("UAVNavigationArmShotAutoAiming_FlyGoto_Altitude_Mark_Difference") && settings.value("UAVNavigationArmShotAutoAiming_FlyGoto_Altitude_Mark_Difference").canConvert(QVariant::Type::Double))
    {
        m_AltitudeFlyGoto_Mark_Difference    = settings.value("UAVNavigationArmShotAutoAiming_FlyGoto_Altitude_Mark_Difference", "0").toDouble();
        if (m_AltitudeFlyGoto_Mark_Difference <= 0)
        {
            m_AltitudeFlyGoto_Mark_Difference = 300.0;
        }

    }

    settings.setValue("UAVNavigationArmShotAutoAiming_FlyGoto_Speed", m_VelocityFlyGoto);
    settings.setValue("UAVNavigationArmShotAutoAiming_FlyGoto_Time", m_TimeFlyGoto);
    settings.setValue("UAVNavigationArmShotAutoAiming_FlyGoto_Altitude", m_AltitudeFlyGoto);
    settings.setValue("UAVNavigationArmShotAutoAiming_FlyGoto_Altitude_Mark_Difference", m_AltitudeFlyGoto_Mark_Difference);

    settings.endGroup();

    doubleSpinBox_FlyGoto_Altitude->setValue(m_AltitudeFlyGoto);
    spinBox_FlyGoto_Velocity->setValue(m_VelocityFlyGoto);
    spinBox_FlyGoto_Radius->setValue(m_RadiusFlyGoto);
    spinBox_FlyGoto_Time->setValue(m_TimeFlyGoto);

    comboBox_FlyGoto_Mode->addItem(tr("Position", "UAVFlyManageDialog FlyGoto Mode"), 0x00);
    comboBox_FlyGoto_Mode->addItem(tr("Hover", "UAVFlyManageDialog FlyGoto Mode"),    0x01);
    comboBox_FlyGoto_Mode->setCurrentIndex(0);

    connect(doubleSpinBox_FlyGoto_Altitude, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value)
    {
        calAimingPositionFromUAVAndMarkPosition(true);
    });

    connect(pushButton_FlyGoto_Cancel, &QPushButton::clicked, [this]()
    {
        this->close();
    });

    connect(pushButton_FlyGoto_OK, &QPushButton::clicked, [this]()
    {
        if (m_pUAVNavigationArmShotAutoAiming != nullptr)
        {

            if (doubleSpinBox_FlyGoto_Longitude == nullptr
                    || doubleSpinBox_FlyGoto_Latitude  == nullptr
                    || doubleSpinBox_FlyGoto_Altitude  == nullptr
               )
            {
                return ;
            }

            m_LongitudeFlyGoto = doubleSpinBox_FlyGoto_Longitude->value();
            m_LatitudeFlyGoto  = doubleSpinBox_FlyGoto_Latitude->value();
            m_AltitudeFlyGoto  = doubleSpinBox_FlyGoto_Altitude->value();
            m_VelocityFlyGoto  = spinBox_FlyGoto_Velocity->value();
            m_RadiusFlyGoto    = spinBox_FlyGoto_Radius->value();
            m_TimeFlyGoto      = spinBox_FlyGoto_Time->value();

            m_ModeFlyGoto      = 0x00;
            if (comboBox_FlyGoto_Mode->count() > 0)
            {
                m_ModeFlyGoto  = comboBox_FlyGoto_Mode->currentData().toInt();
            }

            const QGeoCoordinate flyGotoPoint(m_LatitudeFlyGoto, m_LongitudeFlyGoto, m_AltitudeFlyGoto);
            if (!flyGotoPoint.isValid())
            {
                LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position is invalid , Instruction not send!"), QMessageBox::Yes);
                return ;
            }

            if (m_AltitudeFlyGoto < 2)
            {
                LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position altitude must be greater than 2 , Instruction not send!"), QMessageBox::Yes);
                return ;
            }

            if (m_VelocityFlyGoto < 0)
            {
                LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position speed must be greater than or equal 0 , Instruction not send!"), QMessageBox::Yes);
                return ;
            }

            if (m_RadiusFlyGoto < 0)
            {
                LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position radius must be greater than or equal 0 , Instruction not send!"), QMessageBox::Yes);
                return ;
            }

            if (m_TimeFlyGoto < 0)
            {
                LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position radius must be greater than or equal 0 , Instruction not send!"), QMessageBox::Yes);
                return ;
            }

            if ((m_ModeFlyGoto != 0x00) && (m_ModeFlyGoto != 0x01))
            {
                LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position mode must be equal 0 or 1, Instruction not send!"), QMessageBox::Yes);
                return ;
            }

            m_pUAVNavigationArmShotAutoAiming->FlyGoto_FlyGotoPoint(m_LongitudeFlyGoto,
                                                                    m_LatitudeFlyGoto,
                                                                    m_AltitudeFlyGoto,
                                                                    m_VelocityFlyGoto,
                                                                    m_RadiusFlyGoto,
                                                                    m_TimeFlyGoto,
                                                                    m_ModeFlyGoto,
                                                                    m_strComponentID
                                                                   );
        }
        this->close();
    });
}

void LyMessageBoxCruise::setUAVAndMarkPosition(const double longitudeUAV, const double latitudeUAV, const double altitudeUAV, const double altitudeRelUAV, const double longitudeMark, const double latitudeMark, const double altitudeMark)
{
    m_LongitudeUAV    = longitudeUAV;
    m_LatitudeUAV     = latitudeUAV;
    m_AltitudeUAV     = altitudeUAV;
    m_AltitudeRelUAV  = altitudeRelUAV;
    m_LongitudeMark = longitudeMark;
    m_LatitudeMark  = latitudeMark;
    m_AltitudeMark  = altitudeMark;

    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *pIUAVRealTimeStateManageService = IXyhcActivator::Get().getPIUAVRealTimeStateManageService();
    if (pIUAVRealTimeStateManageService && doubleSpinBox_FlyGoto_Longitude)
    {
        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stelemetryInformation;
        bool result = pIUAVRealTimeStateManageService->GetUAVRealTimeState(m_strComponentID, stelemetryInformation);
        if (result)
        {
            m_LongitudeUAV    = stelemetryInformation.telemetryInfo.position.lng;
            m_LatitudeUAV     = stelemetryInformation.telemetryInfo.position.lat;
            m_AltitudeUAV     = stelemetryInformation.telemetryInfo.gpsHeigth * 10.0;
            m_AltitudeRelUAV  = stelemetryInformation.telemetryInfo.relativeOriginHeigth;

            const double gpsHeight              = altitudeMark + m_AltitudeFlyGoto_Mark_Difference;
            const double changeHeightDifference = gpsHeight - stelemetryInformation.telemetryInfo.gpsHeigth * 10.0;

            double relativeChangeHeight = 0;
            if (changeHeightDifference > 0)
            {

                relativeChangeHeight = std::fabs(changeHeightDifference);
            }
            else if (changeHeightDifference < 0)
            {

                relativeChangeHeight = 0;
            }
            else
            {

                relativeChangeHeight = 0;
            }

            m_LongitudeFlyGoto = longitudeMark;
            m_LatitudeFlyGoto  = latitudeMark;
            m_AltitudeFlyGoto  = stelemetryInformation.telemetryInfo.relativeOriginHeigth + relativeChangeHeight;

            doubleSpinBox_FlyGoto_Longitude->setValue(m_LongitudeFlyGoto);
            doubleSpinBox_FlyGoto_Latitude->setValue(m_LatitudeFlyGoto);
            doubleSpinBox_FlyGoto_Altitude->setValue(m_AltitudeFlyGoto);

            calAimingPositionFromUAVAndMarkPosition();

            return;
        }
    }

    m_LongitudeFlyGoto = 0;
    m_LatitudeFlyGoto  = 0;
    m_AltitudeFlyGoto  = 0;

    doubleSpinBox_FlyGoto_Longitude->setValue(m_LongitudeFlyGoto);
    doubleSpinBox_FlyGoto_Latitude->setValue(m_LatitudeFlyGoto);
    doubleSpinBox_FlyGoto_Altitude->setValue(m_AltitudeFlyGoto);

    calAimingPositionFromUAVAndMarkPosition();
}

void LyMessageBoxCruise::calAimingPositionFromUAVAndMarkPosition(bool userChange)
{
    m_LongitudeFlyGoto = doubleSpinBox_FlyGoto_Longitude->value();
    m_LatitudeFlyGoto  = doubleSpinBox_FlyGoto_Latitude->value();
    m_AltitudeFlyGoto  = doubleSpinBox_FlyGoto_Altitude->value();
    m_VelocityFlyGoto  = spinBox_FlyGoto_Velocity->value();
    m_RadiusFlyGoto    = spinBox_FlyGoto_Radius->value();
    m_TimeFlyGoto      = spinBox_FlyGoto_Time->value();

    qnzkna::UAVRealTimeStateManage::IUAVRealTimeStateManageService *pIUAVRealTimeStateManageService = IXyhcActivator::Get().getPIUAVRealTimeStateManageService();
    if (pIUAVRealTimeStateManageService && label_MarkPosition)
    {
        qnzkna::UAVRealTimeStateManage::RealTimeTelemetryInformation_t stelemetryInformation;
        bool result = pIUAVRealTimeStateManageService->GetUAVRealTimeState(m_strComponentID, stelemetryInformation);
        if (result)
        {
            m_LongitudeUAV    = stelemetryInformation.telemetryInfo.position.lng;
            m_LatitudeUAV     = stelemetryInformation.telemetryInfo.position.lat;
            m_AltitudeUAV     = stelemetryInformation.telemetryInfo.gpsHeigth * 10.0;
            m_AltitudeRelUAV  = stelemetryInformation.telemetryInfo.relativeOriginHeigth;

            const double &altitudeMark        = m_AltitudeMark;
            const double gpsHeight              = altitudeMark + m_AltitudeFlyGoto_Mark_Difference;
            const double changeHeightDifference = gpsHeight - stelemetryInformation.telemetryInfo.gpsHeigth * 10.0;

            double relativeChangeHeight = 0;
            if (changeHeightDifference > 0)
            {

                relativeChangeHeight = std::fabs(changeHeightDifference);
            }
            else if (changeHeightDifference < 0)
            {

                relativeChangeHeight = 0;
            }
            else
            {

                relativeChangeHeight = 0;
            }

            m_LongitudeFlyGoto = m_LongitudeMark;
            m_LatitudeFlyGoto  = m_LatitudeMark;

            if (!userChange)
            {
                m_AltitudeFlyGoto  = stelemetryInformation.telemetryInfo.relativeOriginHeigth + relativeChangeHeight;
                doubleSpinBox_FlyGoto_Altitude->setValue(m_AltitudeFlyGoto);
            }
            else
            {
                m_AltitudeFlyGoto  = doubleSpinBox_FlyGoto_Altitude->value();
            }

            doubleSpinBox_FlyGoto_Longitude->setValue(m_LongitudeFlyGoto);
            doubleSpinBox_FlyGoto_Latitude->setValue(m_LatitudeFlyGoto);

            double userRelativeChangeHeight = relativeChangeHeight;
            if (userChange)
            {
                userRelativeChangeHeight = m_AltitudeFlyGoto - stelemetryInformation.telemetryInfo.relativeOriginHeigth;
            }

            label_MarkPosition->setText(tr("\r\nDrone latitude and longitude height:") +
                                          QStringLiteral("%1 , %2, %3, R:%4\r\n").arg(m_LongitudeUAV, 0, 'f', 7).arg(m_LatitudeUAV, 0, 'f', 7).arg(m_AltitudeUAV, 0, 'f', 2).arg(m_AltitudeRelUAV, 0, 'f', 2) +
                                          tr("Mark latitude and longitude height:") +
                                          QStringLiteral("%1 , %2, %3\r\n").arg(m_LongitudeMark, 0, 'f', 7).arg(m_LatitudeMark, 0, 'f', 7).arg(m_AltitudeMark, 0, 'f', 2) +
                                          tr("High latitude and longitude for bomb dropping:") +
                                          QStringLiteral("%1 , %2, %3, R:%4\r\n").arg(m_LongitudeFlyGoto, 0, 'f', 7).arg(m_LatitudeFlyGoto, 0, 'f', 7).arg(m_AltitudeUAV + userRelativeChangeHeight, 0, 'f', 2).arg(m_AltitudeFlyGoto, 0, 'f', 2)
                                         );
            return;
        }
    }

    m_LongitudeFlyGoto = 0;
    m_LatitudeFlyGoto  = 0;
    m_AltitudeFlyGoto  = 0;
    if (label_MarkPosition != nullptr)
    {
        label_MarkPosition->setText(tr("\r\nMark latitude and longitude height:") +
                                      QStringLiteral("%1 , %2, %3").arg(m_LongitudeMark, 0, 'f', 7).arg(m_LatitudeMark, 0, 'f', 7).arg(m_AltitudeMark, 0, 'f', 2) +
                                      tr("High latitude and longitude for bomb dropping:") +
                                      QStringLiteral("%1 , %2, %3, R:%4\r\n").arg(m_LongitudeFlyGoto, 0, 'f', 7).arg(m_LatitudeFlyGoto, 0, 'f', 7).arg(m_AltitudeFlyGoto, 0, 'f', 2).arg(m_AltitudeFlyGoto, 0, 'f', 2)

                                     );
    }
}

UAVNavigationArmShotAutoAiming::UAVNavigationArmShotAutoAiming(QObject *parent)
    : QObject(parent)
    , m_CXYWeaponManage()
    , m_DistMarkCalculate(1000)
    , m_YawAngleDifference(0.5)
    , m_YawAngleDifferenceBeforeHeight(3)
    , m_HeightDifference(1.0)
    , m_DistDifference(1.0)
    , m_DistAutoAimingReday(3.0)
    , m_DistVehicleMark(1000.0)
    , m_HeightVehicleMark(100.0)
    , m_LowVehicleMark(20.0)
    , m_GetVehiclePitchStart(true)
    , m_MultiWeaponComponentMap()
    , m_pProj(nullptr)
    , m_VehiclePitchStart(0.0)
    , m_VehiclePitchStartNumber(0)
    , m_VehiclePitchStartList()
    , m_AutoAimingAdjustTimesMax(1000)
    , m_AutoAimingAdjustYawTimes(0)
    , m_AutoAimingAdjustHeightTimes(0)
    , m_AutoAimingAdjustDistTimes(0)
    , m_AutoAimingNoAdjustTimesMax(1000)
    , m_AutoAimingNoAdjustTimes(0)
    , m_AutoAimingTimerDuration(1000)
    , m_AutoAimingTimer(this)
    , m_StrComponentID("")
    , m_DY_ZC_ControlInfoData()
{

    connect(&m_AutoAimingTimer, &QTimer::timeout, this, [this]()
    {
        if (m_StrComponentID.empty())
        {
            return ;
        }

        AutoAiming(m_StrComponentID);
    });

    connect(this, &UAVNavigationArmShotAutoAiming::AutoAmimingSig, this, [this](const QString strComponentID, const QByteArray data)
    {
        m_DY_ZC_ControlInfoData = qnzkna::DY_ZC_Control::DY_ZC_ControlInfo();
        if (strComponentID.isEmpty())
        {
            return ;
        }

        if (data.length() != sizeof(qnzkna::DY_ZC_Control::DY_ZC_ControlInfo))
        {
            return ;
        }

        const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo *const DY_ZC_ControlInfoData = reinterpret_cast<const qnzkna::DY_ZC_Control::DY_ZC_ControlInfo *const>(data.constData());

        if (DY_ZC_ControlInfoData == nullptr)
        {
            return ;
        }

        if (strComponentID != QString::number(DY_ZC_ControlInfoData->componentID))
        {
            return ;
        }

        if (DY_ZC_ControlInfoData->oper == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_Oper_Enum::DY_ZC_ControlInfo_Oper_AutoAiming)
        {

            StopAutoAmiming();

            m_VehiclePitchStart       = 0;
            m_VehiclePitchStartNumber = 0;
            m_VehiclePitchStartList.clear();

            m_StrComponentID = "";
            if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_AutoAiming_Start)
            {
                if ((DY_ZC_ControlInfoData->bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Rocket)
                        || (DY_ZC_ControlInfoData->bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Cruise)
                   )
                {
                    m_StrComponentID        = strComponentID.toStdString();
                    m_DY_ZC_ControlInfoData = *DY_ZC_ControlInfoData;

                    StartAutoAmiming();
                }
                else
                {
                    StopAutoAmiming();
                    LyMessageBox::warning(nullptr, tr("Automatic aiming:"),
                                          tr("Automatic aiming:Equipment (UAV) ammunition type unknown, stop aiming!"), QMessageBox::Yes);
                }
            }
            else if (DY_ZC_ControlInfoData->operData == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_OperData_Enum::DY_ZC_ControlInfo_OperData_AutoAiming_Stop)
            {
                m_StrComponentID = strComponentID.toStdString();
                StopAutoAmiming();

                m_VehiclePitchStart       = 0;
                m_VehiclePitchStartNumber = 0;
                m_VehiclePitchStartList.clear();

                if (DY_ZC_ControlInfoData->bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Rocket)
                {

                    FlyMode_StabilizeMode(m_StrComponentID);
                }
                else if (DY_ZC_ControlInfoData->bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Cruise)
                {

                    FlyMode_StabilizeMode(m_StrComponentID);
                }
            }
            else
            {
                m_StrComponentID = strComponentID.toStdString();
                StopAutoAmiming();

                m_VehiclePitchStart       = 0;
                m_VehiclePitchStartNumber = 0;
                m_VehiclePitchStartList.clear();
            }
        }
    }, Qt::QueuedConnection);
}

void UAVNavigationArmShotAutoAiming::InitWeaponComponent()
{
    m_MultiWeaponComponentMap.clear();

    qnzkna::SystemMaintenance::ISystemMaintenanceService *pSystemMaintenanceService = IXyhcActivator::Get().getISystemMaintenanceService();
    if (!pSystemMaintenanceService)
    {
        return ;
    }

    pSystemMaintenanceService->GetWeaponComponents(&m_MultiWeaponComponentMap);
}

void UAVNavigationArmShotAutoAiming::setPProj(IProj *pProj)
{
    m_pProj = pProj;
}

void UAVNavigationArmShotAutoAiming::StartAutoAmiming()
{

    StopAutoAmiming();

    if (m_StrComponentID.empty())
    {
        return ;
    }

    const QString fileName = "../data/conf/AutoAiming.ini";
    if (QFileInfo::exists(fileName))
    {
        QSettings settings(fileName, QSettings::IniFormat);
        settings.beginGroup("AutoAimingSetting");
        if (settings.contains("AutoAimingTimerDuration"))
        {
            QVariant autoAimingTimerDurationVariant = settings.value("AutoAimingTimerDuration", "1000");
            if (autoAimingTimerDurationVariant.isValid() && autoAimingTimerDurationVariant.canConvert(QMetaType::Int))
            {
                bool ok = false;
                const int autoAimingTimerDuration = autoAimingTimerDurationVariant.toInt(&ok);
                if (ok && autoAimingTimerDuration >= 50 && autoAimingTimerDuration <= 5000)
                {
                    m_AutoAimingTimerDuration = autoAimingTimerDuration;
                }
            }
        }

        if (settings.contains("AutoAimingAdjustTimesMax"))
        {
            QVariant autoAimingAdjustTimesMaxVariant = settings.value("AutoAimingAdjustTimesMax", "1000");
            if (autoAimingAdjustTimesMaxVariant.isValid() && autoAimingAdjustTimesMaxVariant.canConvert(QMetaType::ULongLong))
            {
                bool ok = false;
                const unsigned long long int autoAimingAdjustTimesMax = autoAimingAdjustTimesMaxVariant.toULongLong(&ok);
                if (ok && autoAimingAdjustTimesMax >= 10 && autoAimingAdjustTimesMax <= 10000)
                {
                    m_AutoAimingAdjustTimesMax = autoAimingAdjustTimesMax;
                }
            }
        }

        if (settings.contains("AutoAimingNoAdjustTimesMax"))
        {
            QVariant autoAimingNoAdjustTimesMaxVariant = settings.value("AutoAimingNoAdjustTimesMax", "1000");
            if (autoAimingNoAdjustTimesMaxVariant.isValid() && autoAimingNoAdjustTimesMaxVariant.canConvert(QMetaType::ULongLong))
            {
                bool ok = false;
                const unsigned long long int autoAimingNoAdjustTimesMax = autoAimingNoAdjustTimesMaxVariant.toULongLong(&ok);
                if (ok && autoAimingNoAdjustTimesMax >= 10 && autoAimingNoAdjustTimesMax <= 10000)
                {
                    m_AutoAimingNoAdjustTimesMax = autoAimingNoAdjustTimesMax;
                }
            }
        }

        if (settings.contains("DistMarkCalculate"))
        {
            QVariant distMarkCalculateVariant = settings.value("DistMarkCalculate", "1000");
            if (distMarkCalculateVariant.isValid() && distMarkCalculateVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double distMarkCalculate = distMarkCalculateVariant.toDouble(&ok);
                if (ok && distMarkCalculate >= 1 && distMarkCalculate <= 2000)
                {
                    m_DistMarkCalculate = distMarkCalculate;
                }
            }
        }

        if (settings.contains("YawAngleDifference"))
        {
            QVariant yawAngleDifferenceVariant = settings.value("YawAngleDifference", "0.5");
            if (yawAngleDifferenceVariant.isValid() && yawAngleDifferenceVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double yawAngleDifference = yawAngleDifferenceVariant.toDouble(&ok);
                if (ok && yawAngleDifference >= 0.05 && yawAngleDifference <= 5)
                {
                    m_YawAngleDifference = yawAngleDifference;
                }
            }
        }

        if (settings.contains("YawAngleDifferenceBeforeHeight"))
        {
            QVariant yawAngleDifferenceBeforeHeightVariant = settings.value("YawAngleDifferenceBeforeHeight", "3");
            if (yawAngleDifferenceBeforeHeightVariant.isValid() && yawAngleDifferenceBeforeHeightVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double yawAngleDifferenceBeforeHeight = yawAngleDifferenceBeforeHeightVariant.toDouble(&ok);
                if (ok && yawAngleDifferenceBeforeHeight >= 0.05 && yawAngleDifferenceBeforeHeight <= 10)
                {
                    m_YawAngleDifferenceBeforeHeight = yawAngleDifferenceBeforeHeight;
                }
            }
        }

        if (settings.contains("HeightVehicleMark"))
        {
            QVariant heightVehicleMarkVariant = settings.value("HeightVehicleMark", "100");
            if (heightVehicleMarkVariant.isValid() && heightVehicleMarkVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double heightVehicleMark = heightVehicleMarkVariant.toDouble(&ok);
                if (ok && heightVehicleMark >= 0.1 && heightVehicleMark <= 500)
                {
                    m_HeightVehicleMark = heightVehicleMark;
                }
            }
        }

        if (settings.contains("LowVehicleMark"))
        {
            QVariant lowVehicleMarkVariant = settings.value("LowVehicleMark", "20");
            if (lowVehicleMarkVariant.isValid() && lowVehicleMarkVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double lowVehicleMark = lowVehicleMarkVariant.toDouble(&ok);
                if (ok && lowVehicleMark >= 0.1 && lowVehicleMark <= 100)
                {
                    m_LowVehicleMark = lowVehicleMark;
                }
            }
        }

        if (settings.contains("HeightDifference"))
        {
            QVariant heightDifferenceVariant = settings.value("HeightDifference", "1");
            if (heightDifferenceVariant.isValid() && heightDifferenceVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double heightDifference = heightDifferenceVariant.toDouble(&ok);
                if (ok && heightDifference >= 0.1 && heightDifference <= 500)
                {
                    m_HeightDifference = heightDifference;
                }
            }
        }

        if (settings.contains("DistVehicleMark"))
        {
            QVariant distVehicleMarkVariant = settings.value("DistVehicleMark", "1000");
            if (distVehicleMarkVariant.isValid() && distVehicleMarkVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double distVehicleMark = distVehicleMarkVariant.toDouble(&ok);
                if (ok && distVehicleMark >= 0.1 && distVehicleMark <= 2000)
                {
                    m_DistVehicleMark = distVehicleMark;
                }
            }
        }

        if (settings.contains("DistDifference"))
        {
            QVariant distDifferenceVariant = settings.value("DistDifference", "1");
            if (distDifferenceVariant.isValid() && distDifferenceVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double distDifference = distDifferenceVariant.toDouble(&ok);
                if (ok && distDifference >= 0.1 && distDifference <= 500)
                {
                    m_DistDifference = distDifference;
                }
            }
        }

        if (settings.contains("DistAutoAimingReday"))
        {
            QVariant distAutoAimingRedayVariant = settings.value("DistAutoAimingReday", "3");
            if (distAutoAimingRedayVariant.isValid() && distAutoAimingRedayVariant.canConvert(QMetaType::Double))
            {
                bool ok = false;
                const double distAutoAimingReday = distAutoAimingRedayVariant.toDouble(&ok);
                if (ok && distAutoAimingReday >= 0.1 && distAutoAimingReday <= 500)
                {
                    m_DistAutoAimingReday = distAutoAimingReday;
                }
            }
        }

        if (settings.contains("VehiclePitchStart"))
        {
            QVariant getVehiclePitchStartVariant = settings.value("VehiclePitchStart", "1");
            if (getVehiclePitchStartVariant.isValid() && getVehiclePitchStartVariant.canConvert(QMetaType::Bool))
            {
                m_GetVehiclePitchStart = getVehiclePitchStartVariant.toBool();
            }
        }

        settings.endGroup();
    }

    if (m_AutoAimingTimerDuration < 50 || m_AutoAimingTimerDuration > 5000)
    {
        m_AutoAimingTimerDuration = 1000;
    }

    if (m_AutoAimingAdjustTimesMax < 10 || m_AutoAimingAdjustTimesMax > 10000)
    {
        m_AutoAimingAdjustTimesMax = 1000;
    }

    if (m_AutoAimingNoAdjustTimesMax < 10 || m_AutoAimingNoAdjustTimesMax > 10000)
    {
        m_AutoAimingNoAdjustTimesMax = 1000;
    }

    if (m_DistMarkCalculate < 1 && m_DistMarkCalculate > 2000)
    {
        m_DistMarkCalculate = 1000;
    }

    if (m_YawAngleDifference < 0.05 || m_YawAngleDifference > 5)
    {
        m_YawAngleDifference = 0.5;
    }

    if (m_YawAngleDifferenceBeforeHeight < 0.05 || m_YawAngleDifferenceBeforeHeight > 10)
    {
        m_YawAngleDifferenceBeforeHeight = 3;
    }

    if (m_HeightVehicleMark < 0.1 || m_HeightVehicleMark > 500)
    {
        m_HeightVehicleMark = 100.0;
    }

    if (m_LowVehicleMark < 0.1 || m_LowVehicleMark > 100)
    {
        m_LowVehicleMark = 20.0;
    }

    if (m_HeightDifference < 0.1 || m_HeightDifference > 500)
    {
        m_HeightDifference = 1;
    }

    if (m_DistVehicleMark < 0.1 || m_DistVehicleMark > 2000)
    {
        m_DistVehicleMark = 1000.0;
    }

    if (m_DistDifference < 0.1 || m_DistDifference > 500)
    {
        m_DistDifference = 1;
    }

    if (m_DistAutoAimingReday < 0.1 || m_DistAutoAimingReday > 20)
    {
        m_DistAutoAimingReday = 3;
    }

    m_AutoAimingAdjustYawTimes    = 0;
    m_AutoAimingAdjustHeightTimes = 0;
    m_AutoAimingAdjustDistTimes   = 0;
    m_AutoAimingNoAdjustTimes     = 0;

    m_VehiclePitchStart  = 0.0;

    if (m_GetVehiclePitchStart)
    {
        bool ok = false;
        const unsigned long long componentID = QString::fromStdString(m_StrComponentID).toULongLong(&ok);
        if (ok)
        {
            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(componentID);
            if (track != nullptr)
            {
                m_VehiclePitchStart = track->Pitch;
            }
        }
    }

    if (!m_StrComponentID.empty())
    {
        IProj *pProj = m_pProj;
        if (pProj == nullptr)
        {
            return ;
        }

        bool ok = false;
        const unsigned long long componentID = QString::fromStdString(m_StrComponentID).toULongLong(&ok);
        if (!ok)
        {
            StopAutoAmiming();
            LyMessageBox::warning(nullptr, tr("Automatic aiming:"),
                                  tr("Automatic aiming:Equipment (UAV) ID error, stop aiming!"), QMessageBox::Yes);
            return ;
        }

        const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(componentID);
        if (track == nullptr)
        {
            StopAutoAmiming();
            LyMessageBox::warning(nullptr, tr("Automatic aiming:"),
                                  tr("Automatic aiming:The selected equipment (UAV) does not match the equipment (UAV) in the situation, stop aiming!"), QMessageBox::Yes);
            return ;
        }

        if (track->m_nMarkID == 0 || track->m_strMarkID[0] == '\0' || (track->m_nMarkAssign != 1 && track->m_nMarkAssign != 2))
        {
            StopAutoAmiming();
            LyMessageBox::warning(nullptr, tr("Automatic aiming:"),
                                  tr("Automatic aiming:Equipment (UAV) No target selected,please select a target, stop aiming!"), QMessageBox::Yes);
            return ;
        }

        const double dMarkX             = track->m_dMarkX;
        const double dMarkY             = track->m_dMarkY;
        const double dMarkH             = track->m_dMarkH;
        const QString unNodeNameSimple(QLatin1String(track->unNodeNameSimple));

        double dAltitudeVehicle           = track->m_nGaoDu;
        if (dAltitudeVehicle <= 0.000001 && dAltitudeVehicle >= -0.000001)
        {
            dAltitudeVehicle = dAltitudeVehicle + 0.0005;
        }

        const double dLongitudeVehicle    = track->m_Longitude;
        const double dLatitudeVehicle     = track->m_Latitude;
        const double dGaoDu               = track->m_nGaoDu;
        const double dGaoDuRel            = track->m_nGaoDuRel;
        const double dYawVehicle          = track->m_nHangXiang;
        const double dSpeedVehicle        = track->m_nSuDu;
        const double dRelAltitudeVehicle  = (dAltitudeVehicle - dMarkH) + 1;
        const double dPitchVehicle        = m_VehiclePitchStart;

        if (m_DY_ZC_ControlInfoData.bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Rocket)
        {

            double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
            pProj->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
            if (dDistanceVehicleMark >= m_DistMarkCalculate)
            {
                StopAutoAmiming();
                LyMessageBox::warning(nullptr, tr("Automatic aiming:") + QStringLiteral("%1(%2)").arg(unNodeNameSimple).arg(QString::fromStdString(m_StrComponentID)),
                                      tr("Automatic aiming:the distance from the target exceeds") + QStringLiteral("%1m").arg(m_DistMarkCalculate, 0, 'f', 2) + tr(",stop aiming!"), QMessageBox::Yes);
                return ;
            }

            if (dRelAltitudeVehicle >= m_HeightVehicleMark)
            {
                StopAutoAmiming();
                LyMessageBox::warning(nullptr, tr("Automatic aiming:") + QStringLiteral("%1(%2)").arg(unNodeNameSimple).arg(QString::fromStdString(m_StrComponentID)),
                                      tr("Automatic aiming:exceeds the target altitude") + QStringLiteral("%1m").arg(m_HeightVehicleMark, 0, 'f', 2) + tr(",stop aiming!"), QMessageBox::Yes);
                return ;
            }

            if (dRelAltitudeVehicle + m_LowVehicleMark < 0)
            {
                StopAutoAmiming();
                LyMessageBox::warning(nullptr, tr("Automatic aiming:") + QStringLiteral("%1(%2)").arg(unNodeNameSimple).arg(QString::fromStdString(m_StrComponentID)),
                                      tr("Automatic aiming:below the target altitude") + QStringLiteral("%1m").arg(dRelAltitudeVehicle, 0, 'f', 2) + tr(",stop aiming!"), QMessageBox::Yes);
                return ;
            }

            if (QMessageBox::Yes == LyMessageBox::warning(nullptr, tr("Confirm again Automatic aiming:") + QStringLiteral("%1(%2)").arg(unNodeNameSimple).arg(QString::fromStdString(m_StrComponentID)),
                                                          tr("Start Automatic aiming:Confirm again,are you sure you want to start automatic aiming?"), QMessageBox::Yes | QMessageBox::No))
            {

                m_AutoAimingTimer.start(m_AutoAimingTimerDuration);
                LyXyhc::Get().getTrackMgr().UpdateAutoAimingState(componentID, 1);

                return ;
            }

            StopAutoAmiming();
        }
        else if (m_DY_ZC_ControlInfoData.bombType == qnzkna::DY_ZC_Control::DY_ZC_ControlInfo_BombType_Enum::DY_ZC_ControlInfo_BombType_Cruise)
        {

            LyXyhc::Get().getTrackMgr().UpdateAutoAimingState(componentID, 0);

            LyMessageBoxCruise *lyMessageBoxCruise = new LyMessageBoxCruise(m_StrComponentID, std::string(track->unNodeNameSimple, sizeof(track->unNodeNameSimple) - 1), this);
            if (lyMessageBoxCruise != nullptr)
            {
                lyMessageBoxCruise->setUAVAndMarkPosition(dLongitudeVehicle, dLatitudeVehicle, dGaoDu, dGaoDuRel, dMarkX, dMarkY, dMarkH);
                lyMessageBoxCruise->setAttribute(Qt::WA_DeleteOnClose);
                lyMessageBoxCruise->setResizable(true);

                lyMessageBoxCruise->exec();
            }

            LyXyhc::Get().getTrackMgr().UpdateAutoAimingState(componentID, 1);
            StopAutoAmiming();
            return ;
        }
    }

    StopAutoAmiming();
    LyMessageBox::warning(nullptr, tr("Automatic aiming:"),
                          tr("Automatic aiming:Equipment (UAV) No target selected,please select a target, stop aiming!"), QMessageBox::Yes);
}

void UAVNavigationArmShotAutoAiming::StopAutoAmiming() const
{
    if (m_AutoAimingTimer.isActive())
    {
        m_AutoAimingTimer.stop();
    }

    bool ok = false;
    const unsigned long long componentID = QString::fromStdString(m_StrComponentID).toULongLong(&ok);
    if (ok)
    {
        LyXyhc::Get().getTrackMgr().UpdateAutoAimingState(componentID, 2);
    }

    m_VehiclePitchStart       = 0;
    m_VehiclePitchStartNumber = 0;
    m_VehiclePitchStartList.clear();

    m_AutoAimingAdjustYawTimes    = m_AutoAimingAdjustTimesMax;
    m_AutoAimingAdjustHeightTimes = m_AutoAimingAdjustTimesMax;
    m_AutoAimingAdjustDistTimes   = m_AutoAimingAdjustTimesMax;
    m_AutoAimingNoAdjustTimes     = m_AutoAimingAdjustTimesMax;
}

double UAVNavigationArmShotAutoAiming::HeadAngle(const double angle) const
{

    double headingAngle = angle;
    if (qFuzzyIsNull(std::fmod(std::abs(angle), 360)))
    {
        headingAngle = 0;
    }
    else if (angle >= 360)
    {
        headingAngle = std::fmod(std::abs(angle), 360);
    }
    else if (angle < 0)
    {
        headingAngle = 360 - std::fmod(std::abs(angle), 360);
    }
    return headingAngle;
}

void UAVNavigationArmShotAutoAiming::AutoAiming(const std::string &strComponentID) const
{
    IProj *pProj = m_pProj;
    if (pProj == nullptr)
    {
        StopAutoAmiming();
        return ;
    }

    m_AutoAimingNoAdjustTimes++;
    if (m_AutoAimingNoAdjustTimes > m_AutoAimingNoAdjustTimesMax)
    {
        StopAutoAmiming();
        qDebug() << __LINE__ << ":" << tr2("停止瞄准， 调整超过最大次数1000：") << m_AutoAimingNoAdjustTimes;
        return ;
    }

    bool ok = false;
    const unsigned long long componentID = QString::fromStdString(strComponentID).toULongLong(&ok);
    if (!ok)
    {
        StopAutoAmiming();
        return ;
    }

    const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(componentID);
    if (track == nullptr)
    {
        StopAutoAmiming();
        return ;
    }

    if (track->m_nMarkID == 0 || track->m_strMarkID[0] == '\0')
    {
        StopAutoAmiming();
        return ;
    }

    if (m_GetVehiclePitchStart && m_VehiclePitchStartNumber < 5)
    {
        m_VehiclePitchStartNumber++;

        bool ok = false;
        const unsigned long long componentID = QString::fromStdString(m_StrComponentID).toULongLong(&ok);
        if (ok)
        {
            const QTrack *track = LyXyhc::Get().getTrackMgr().FindTrackNodeID(componentID);
            if (track != nullptr)
            {
                m_VehiclePitchStartList.push_back(track->Pitch);
            }
        }

        if (m_VehiclePitchStartList.size() > 0)
        {
            double sum_x  = 0.0;
            double sum_xx = 0.0;

            int num = m_VehiclePitchStartList.size();

            double average_x = 0.0;

            for (int i = 0; i < num; i++)
            {
                sum_x  += m_VehiclePitchStartList[i];
            }

            average_x = sum_x / num;
            m_VehiclePitchStart = average_x;

        }
        return ;
    }

    if (m_VehiclePitchStart < -10 || m_VehiclePitchStart > 10)
    {
        m_VehiclePitchStart = 0;
    }

    const double dMarkX             = track->m_dMarkX;
    const double dMarkY             = track->m_dMarkY;
    const double dMarkH             = track->m_dMarkH;

    double dAltitudeVehicle           = track->m_nGaoDu;
    if (dAltitudeVehicle <= 0.000001 && dAltitudeVehicle >= -0.000001)
    {
        dAltitudeVehicle = dAltitudeVehicle + 0.0005;
    }

    const double dLongitudeVehicle    = track->m_Longitude;
    const double dLatitudeVehicle     = track->m_Latitude;
    const double dYawVehicle          = track->m_nHangXiang;
    const double dSpeedVehicle        = track->m_nSuDu;
    const double dRelAltitudeVehicle  = (dAltitudeVehicle - dMarkH) + 1;
    const double dPitchVehicle        = m_VehiclePitchStart;

    if (std::abs(dRelAltitudeVehicle) >= m_HeightVehicleMark)
    {
        StopAutoAmiming();
        qDebug() << __LINE__ << ":" << tr2("停止瞄准， 距离目标高度超过100m：") << dRelAltitudeVehicle;
        return ;
    }

    double dDistanceVehicleMark = DBL_MAX, dAngleVehicleMark = DBL_MAX;
    pProj->L_A(dLongitudeVehicle, dLatitudeVehicle, dMarkX, dMarkY, dDistanceVehicleMark, dAngleVehicleMark);
    if (dDistanceVehicleMark > m_DistMarkCalculate)
    {
        StopAutoAmiming();
        qDebug() << __LINE__ << ":" << tr2("停止瞄准， 距离目标距离超过1000m：") << dDistanceVehicleMark;
        return ;
    }

    LyXyhc::Get().getTrackMgr().UpdateAutoAimingState(componentID, 1);

    {

        const double dYawAngleDifference = (HeadAngle(dYawVehicle) - HeadAngle(dAngleVehicleMark));
        if (dYawAngleDifference > m_YawAngleDifferenceBeforeHeight)
        {

            if (dYawAngleDifference <= 180)
            {
                m_AutoAimingNoAdjustTimes = 0;

                if (dYawAngleDifference / 2.0 > 5)
                {
                    FlyAction_TurnLeft(5, strComponentID);
                }
                else
                {
                    if (dYawAngleDifference >= 2.0)
                    {
                        FlyAction_TurnLeft(dYawAngleDifference / 2.0, strComponentID);
                    }
                    else
                    {
                        FlyAction_TurnLeft(dYawAngleDifference, strComponentID);
                    }
                }
            }
            else if (dYawAngleDifference > 180 && (360 - dYawAngleDifference < 180))
            {
                m_AutoAimingNoAdjustTimes = 0;
                if ((360 - dYawAngleDifference) / 2.0 > 5)
                {
                    FlyAction_TurnRight(5, strComponentID);
                }
                else
                {
                    if ((360 - dYawAngleDifference) / 2.0 >= 2.0)
                    {
                        FlyAction_TurnRight((360 - dYawAngleDifference) / 2.0, strComponentID);
                    }
                    else
                    {
                        FlyAction_TurnRight((360 - dYawAngleDifference), strComponentID);
                    }
                }
            }

            m_AutoAimingAdjustYawTimes++;
            if (m_AutoAimingAdjustYawTimes > m_AutoAimingAdjustTimesMax)
            {
                StopAutoAmiming();
            }
            return ;
        }
        else if (dYawAngleDifference < -m_YawAngleDifferenceBeforeHeight)
        {

            if (dYawAngleDifference >= -180)
            {
                m_AutoAimingNoAdjustTimes = 0;
                if (std::abs(dYawAngleDifference) / 2.0 > 5)
                {
                    FlyAction_TurnRight(5, strComponentID);
                }
                else
                {
                    if (std::abs(dYawAngleDifference) / 2.0 >= 2.0)
                    {
                        FlyAction_TurnRight(std::abs(dYawAngleDifference) / 2.0, strComponentID);
                    }
                    else
                    {
                        FlyAction_TurnRight(std::abs(dYawAngleDifference), strComponentID);
                    }
                }
            }
            else if (dYawAngleDifference < -180 && (360 + dYawAngleDifference < 180))
            {
                m_AutoAimingNoAdjustTimes = 0;
                if ((360 + dYawAngleDifference) / 2.0 > 5)
                {
                    FlyAction_TurnLeft(5, strComponentID);
                }
                else
                {
                    if ((360 + dYawAngleDifference) / 2.0 >= 2.0)
                    {
                        FlyAction_TurnLeft((360 + dYawAngleDifference) / 2.0, strComponentID);
                    }
                    else
                    {
                        FlyAction_TurnLeft((360 + dYawAngleDifference), strComponentID);
                    }
                }
            }

            m_AutoAimingAdjustYawTimes++;
            if (m_AutoAimingAdjustYawTimes > m_AutoAimingAdjustTimesMax)
            {
                StopAutoAmiming();
            }
            return ;
        }
        m_AutoAimingAdjustYawTimes = 0;
    }

#ifndef USING_LY_CCIP_80MM_DEFINE

#elif defined USING_LY_CCIP_80MM_DEFINE
    std::list<qnzkna::ly_ccip::ly_ccip_80mm::ccipdata_80mm> ccipdata_80mmlist;
    qnzkna::ly_ccip::ly_ccip_80mm::ccip_80mm_max_dist(dLongitudeVehicle, dLatitudeVehicle, dAltitudeVehicle, dSpeedVehicle, dRelAltitudeVehicle, dPitchVehicle, ccipdata_80mmlist);
    if (ccipdata_80mmlist.size() > 0)
    {
        double dDistance = DBL_MAX, dAngle = DBL_MAX, dMinDistance3D = DBL_MAX, dMinDistance = DBL_MAX, dMarkDistance = DBL_MAX;
        double dHeight = DBL_MAX, dMinHeight = DBL_MAX;
        double dJD = 0, dWD = 0;

        for (const auto ccipdata : ccipdata_80mmlist)
        {

            pProj->A_L(dLongitudeVehicle, dLatitudeVehicle, ccipdata.x, dYawVehicle, dJD, dWD);

            pProj->L_A(dJD, dWD, dMarkX, dMarkY, dDistance, dAngle);

            dHeight = (dAltitudeVehicle + ccipdata.zd) - dMarkH;
            dMarkDistance = ((dHeight * dHeight) + (dDistance * dDistance));
            if (dMarkDistance < dMinDistance3D)
            {
                dMinDistance3D = dMarkDistance;

                if (((dAngleVehicleMark - dYawVehicle) >= -90 && (dAngleVehicleMark - dYawVehicle) <= 90)
                        && (dDistanceVehicleMark > dDistance))
                {
                    dMinDistance   = dDistance;
                }
                else
                {
                    dMinDistance   = -dDistance;
                }
                dMinHeight     = dHeight;
            }
        }

        dMinDistance3D = std::sqrt(dMinDistance3D);

        LyXyhc::Get().getTrackMgr().UpdateAutoAimingState(componentID, 1);

        if (dMinDistance < m_DistAutoAimingReday)
        {
            StopAutoAmiming();
            qDebug() << __LINE__ << ":" << tr2("停止瞄准， 调整完毕，落点距离目标：") << dMinDistance;
            return ;
        }

        if (dMinDistance3D <= 1000 && (dMinHeight <= m_HeightVehicleMark && dMinHeight >= -m_HeightVehicleMark))
        {

            const double dHeightDifference = dMinHeight;
            if (dHeightDifference > m_HeightDifference)
            {
                m_AutoAimingNoAdjustTimes = 0;

                if (std::abs(dHeightDifference) / 2.0 > 5)
                {
                    FlyAction_GoDown(5, strComponentID);
                }
                else
                {
                    if (std::abs(dHeightDifference) / 2.0 >= 2.0)
                    {
                        FlyAction_GoDown(std::abs(dHeightDifference) / 2.0, strComponentID);
                    }
                    else
                    {
                        FlyAction_GoDown(std::abs(dHeightDifference), strComponentID);
                    }
                }

                m_AutoAimingAdjustHeightTimes++;
                if (m_AutoAimingAdjustHeightTimes > m_AutoAimingAdjustTimesMax)
                {
                    StopAutoAmiming();
                }
                return ;
            }
            else if (dHeightDifference < -m_HeightDifference && dHeightDifference > -(dAltitudeVehicle / 2.0))
            {
                m_AutoAimingNoAdjustTimes = 0;
                if (std::abs(dHeightDifference) / 2.0 > 5)
                {
                    FlyAction_GoUp(5, strComponentID);
                }
                else
                {
                    if (std::abs(dHeightDifference) / 2.0 >= 2.0)
                    {
                        FlyAction_GoUp(std::abs(dHeightDifference) / 2.0, strComponentID);
                    }
                    else
                    {
                        FlyAction_GoUp(std::abs(dHeightDifference), strComponentID);
                    }
                }

                m_AutoAimingAdjustHeightTimes++;
                if (m_AutoAimingAdjustHeightTimes > m_AutoAimingAdjustTimesMax)
                {
                    StopAutoAmiming();
                }
                return ;
            }

            m_AutoAimingAdjustHeightTimes = 0;
        }

        if (dMinDistance3D <= 1000
                && (dMinDistance <= m_DistVehicleMark && dMinDistance >= -m_DistVehicleMark)
                && ((dAngleVehicleMark - dYawVehicle) >= -90 && (dAngleVehicleMark - dYawVehicle) <= 90)
           )
        {

            const double dDistanceDifference = dMinDistance;
            if (dDistanceDifference > m_DistDifference)
            {
                m_AutoAimingNoAdjustTimes = 0;

                if (std::abs(dDistanceDifference) / 2.0 > 5)
                {
                    FlyAction_GoForward(5, strComponentID);
                }
                else
                {
                    if (std::abs(dDistanceDifference) / 2.0 >= 2.0)
                    {
                        FlyAction_GoForward(std::abs(dDistanceDifference) / 2.0, strComponentID);
                    }
                    else
                    {
                        FlyAction_GoForward(std::abs(dDistanceDifference), strComponentID);
                    }
                }

                m_AutoAimingAdjustDistTimes++;
                if (m_AutoAimingAdjustDistTimes > m_AutoAimingAdjustTimesMax)
                {
                    StopAutoAmiming();
                }
                return ;
            }
            else if (dDistanceDifference < -m_DistDifference)
            {
                m_AutoAimingNoAdjustTimes = 0;
                if (std::abs(dDistanceDifference) / 2.0 > 5)
                {
                    FlyAction_GoBack(5, strComponentID);
                }
                else
                {
                    if (std::abs(dDistanceDifference) / 2.0 >= 2.0)
                    {
                        FlyAction_GoBack(std::abs(dDistanceDifference) / 2.0, strComponentID);
                    }
                    else
                    {
                        FlyAction_GoBack(std::abs(dDistanceDifference), strComponentID);
                    }
                }

                m_AutoAimingAdjustDistTimes++;
                if (m_AutoAimingAdjustDistTimes > m_AutoAimingAdjustTimesMax)
                {
                    StopAutoAmiming();
                }
                return ;
            }

            m_AutoAimingAdjustDistTimes = 0;
        }
    }
#endif
    {

        const double dYawAngleDifference = (HeadAngle(dYawVehicle) - HeadAngle(dAngleVehicleMark));
        if (dYawAngleDifference > m_YawAngleDifference)
        {

            if (dYawAngleDifference <= 180)
            {
                m_AutoAimingNoAdjustTimes = 0;

                if (dYawAngleDifference / 2.0 > 5)
                {
                    FlyAction_TurnLeft(5, strComponentID);
                }
                else
                {
                    FlyAction_TurnLeft(dYawAngleDifference / 2.0, strComponentID);
                }
            }
            else if (dYawAngleDifference > 180 && (360 - dYawAngleDifference < 180))
            {
                m_AutoAimingNoAdjustTimes = 0;
                if ((360 - dYawAngleDifference) / 2.0 > 5)
                {
                    FlyAction_TurnRight(5, strComponentID);
                }
                else
                {
                    FlyAction_TurnRight((360 - dYawAngleDifference) / 2.0, strComponentID);
                }
            }

            m_AutoAimingAdjustYawTimes++;
            if (m_AutoAimingAdjustYawTimes > m_AutoAimingAdjustTimesMax)
            {
                StopAutoAmiming();
            }
            return ;
        }
        else if (dYawAngleDifference < -m_YawAngleDifference)
        {

            if (dYawAngleDifference >= -180)
            {
                m_AutoAimingNoAdjustTimes = 0;
                if (std::abs(dYawAngleDifference) / 2.0 > 5)
                {
                    FlyAction_TurnRight(5, strComponentID);
                }
                else
                {
                    FlyAction_TurnRight(std::abs(dYawAngleDifference) / 2.0, strComponentID);
                }
            }
            else if (dYawAngleDifference < -180 && (360 + dYawAngleDifference < 180))
            {
                m_AutoAimingNoAdjustTimes = 0;
                if ((360 + dYawAngleDifference) / 2.0 > 5)
                {
                    FlyAction_TurnLeft(5, strComponentID);
                }
                else
                {
                    FlyAction_TurnLeft((360 + dYawAngleDifference) / 2.0, strComponentID);
                }
            }

            m_AutoAimingAdjustYawTimes++;
            if (m_AutoAimingAdjustYawTimes > m_AutoAimingAdjustTimesMax)
            {
                StopAutoAmiming();
            }
            return ;
        }
        m_AutoAimingAdjustYawTimes = 0;
    }
}

void UAVNavigationArmShotAutoAiming::FlyMode_StabilizeMode(const std::string &strComponentID) const
{

    if (strComponentID.empty())
    {
        return ;
    }

    {

        SendFlyCmd(0xF1, 0, strComponentID);
    }

    qDebug() << __LINE__ << ":" << tr2("飞行模式 增稳") << strComponentID.c_str();

}

void UAVNavigationArmShotAutoAiming::CommOper_HoverStart(const std::string &strComponentID) const
{

    if (strComponentID.empty())
    {
        return ;
    }

    {

        SendFlyCmd(0x29, 0, strComponentID);
    }

}

void UAVNavigationArmShotAutoAiming::CommOper_HoverStop(const std::string &strComponentID) const
{

    if (strComponentID.empty())
    {
        return ;
    }

    {

        SendFlyCmd(0x03, 0, strComponentID);
    }

}

void UAVNavigationArmShotAutoAiming::FlyAction_GoForward(const double goForwardDist, const std::string &strComponentID) const
{

    if (strComponentID.empty() || goForwardDist <= 0)
    {
        return ;
    }

    const unsigned char uExtParam = static_cast<const unsigned char>(std::round(goForwardDist));
    SendFlyCmd(0x21, uExtParam, strComponentID);

    qDebug() << __LINE__ << ":" << tr2("飞行动作 前进") << strComponentID.c_str() << goForwardDist;

}

void UAVNavigationArmShotAutoAiming::FlyAction_TurnLeft(const double turnLeftAngle, const std::string &strComponentID) const
{

    if (strComponentID.empty() || turnLeftAngle <= 0)
    {
        return ;
    }

    const unsigned char uExtParam = static_cast<const unsigned char>(std::round(turnLeftAngle));
    SendFlyCmd(0x22, uExtParam, strComponentID);

    qDebug() << __LINE__ << ":" << tr2("飞行动作 左转") << strComponentID.c_str() << turnLeftAngle;

}

void UAVNavigationArmShotAutoAiming::FlyAction_TurnLeftStationStick(const double turnLeftAngle, const std::string &strComponentID) const
{

    if (strComponentID.empty())
    {
        return ;
    }

    double nYouMen  = 50;
    double nFangWei = 50;

    xygsJL_15 xygsJL_15_temp;
    memset(&xygsJL_15_temp, 0, sizeof(xygsJL_15_temp));
    xygsJL_15_temp.flags = 15;

    xygsJL_15_temp.groupIndex = 0;
    xygsJL_15_temp.planeIndex = 0;
    xygsJL_15_temp.param.aliStickValue = 2048;
    xygsJL_15_temp.param.eleStickValue = 2048;

    xygsJL_15_temp.param.thrStickValue = 4096.0 * nYouMen / 100.0;
    xygsJL_15_temp.param.rudStickValue = 4096.0 / 2.0 + 4096.0 / 2.0 / 100.0 * nFangWei;

}

void UAVNavigationArmShotAutoAiming::FlyAction_TurnRight(const double turnRightAngle, const std::string &strComponentID) const
{

    if (strComponentID.empty() || turnRightAngle <= 0)
    {
        return ;
    }

    const unsigned char uExtParam = static_cast<const unsigned char>(std::round(turnRightAngle));
    SendFlyCmd(0x23, uExtParam, strComponentID);

    qDebug() << __LINE__ << ":" << tr2("飞行动作 右转") << strComponentID.c_str() << turnRightAngle;

}

void UAVNavigationArmShotAutoAiming::FlyAction_TurnRightStationStick(const double turnRightAngle, const std::string &strComponentID) const
{

    if (strComponentID.empty())
    {
        return ;
    }

    double nYouMen  = 50;
    double nFangWei = -50;
    xygsJL_15 xygsJL_15_temp;
    memset(&xygsJL_15_temp, 0, sizeof(xygsJL_15_temp));
    xygsJL_15_temp.flags = 15;

    xygsJL_15_temp.groupIndex = 0;
    xygsJL_15_temp.planeIndex = 0;
    xygsJL_15_temp.param.aliStickValue = 2048;
    xygsJL_15_temp.param.eleStickValue = 2048;

    xygsJL_15_temp.param.thrStickValue = 4096.0 * nYouMen / 100.0;
    xygsJL_15_temp.param.rudStickValue = 4096.0 / 2.0 + 4096.0 / 2.0 / 100.0 * nFangWei;

}

void UAVNavigationArmShotAutoAiming::FlyAction_GoBack(const double backDist, const std::string &strComponentID) const
{

    if (strComponentID.empty() || backDist <= 0)
    {
        return ;
    }

    const unsigned char uExtParam = static_cast<const unsigned char>(std::round(backDist));
    SendFlyCmd(0x20, uExtParam, strComponentID);

    qDebug() << __LINE__ << ":" << tr2("飞行动作 后退") << strComponentID.c_str() << backDist;

}

void UAVNavigationArmShotAutoAiming::FlyAction_GoUp(const double upDist, const std::string &strComponentID) const
{

    if (strComponentID.empty() || upDist <= 0)
    {
        return ;
    }

    const unsigned char uExtParam = static_cast<const unsigned char>(std::round(upDist));
    SendFlyCmd(0x26, uExtParam, strComponentID);

    qDebug() << __LINE__ << ":" << tr2("飞行动作 上升") << strComponentID.c_str() << upDist;

}

void UAVNavigationArmShotAutoAiming::FlyAction_ShiftLeft(const double shiftLeftDist, const std::string &strComponentID) const
{

    if (strComponentID.empty())
    {
        return ;
    }

    const unsigned char uExtParam = static_cast<const unsigned char>(shiftLeftDist);
    SendFlyCmd(0x24, uExtParam, strComponentID);

}

void UAVNavigationArmShotAutoAiming::FlyAction_ShiftRight(const double shiftRightDist, const std::string &strComponentID) const
{

    if (strComponentID.empty())
    {
        return ;
    }

    const unsigned char uExtParam = static_cast<const unsigned char>(shiftRightDist);
    SendFlyCmd(0x25, uExtParam, strComponentID);

}

void UAVNavigationArmShotAutoAiming::FlyAction_GoDown(const double downDist, const std::string &strComponentID) const
{

    if (strComponentID.empty() || downDist <= 0)
    {
        return ;
    }

    const unsigned char uExtParam = static_cast<const unsigned char>(std::round(downDist));
    SendFlyCmd(0x27, uExtParam, strComponentID);

    qDebug() << __LINE__ << ":" << tr2("飞行动作 下降") << strComponentID.c_str() << downDist;

}

void UAVNavigationArmShotAutoAiming::FlyGoto_FlyGotoPoint(const double longitudeFlyGoto,
                                                          const double latitudeFlyGoto,
                                                          const double altitudeFlyGoto,
                                                          const int velocityFlyGoto,
                                                          const int radiusFlyGoto,
                                                          const int timeFlyGoto,
                                                          const int modeFlyGoto,
                                                          const std::string &strComponentID)
{

    const QGeoCoordinate flyGotoPoint(latitudeFlyGoto, longitudeFlyGoto, altitudeFlyGoto);
    if (!flyGotoPoint.isValid())
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position is invalid , Instruction not send!"), QMessageBox::Yes);
        return ;
    }

    if (altitudeFlyGoto < 2)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position altitude must be greater than 2 , Instruction not send!"), QMessageBox::Yes);
        return ;
    }

    if (velocityFlyGoto < 0)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position speed must be greater than or equal 0 , Instruction not send!"), QMessageBox::Yes);
        return ;
    }

    if (radiusFlyGoto < 0)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position radius must be greater than or equal 0 , Instruction not send!"), QMessageBox::Yes);
        return ;
    }

    if (timeFlyGoto < 0)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position radius must be greater than or equal 0 , Instruction not send!"), QMessageBox::Yes);
        return ;
    }

    if ((modeFlyGoto != 0x00) && (modeFlyGoto != 0x01))
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Fly Goto position mode must be equal 0 or 1, Instruction not send!"), QMessageBox::Yes);
        return ;
    }

    qnzkna::SystemMaintenance::ISystemMaintenanceService   *pSystemMaintenanceService      = IXyhcActivator::Get().getISystemMaintenanceService();
    qnzkna::PositionManage::IPositionManageService         *pIPositionManageService        = IXyhcActivator::Get().getPIPositionManageService();
    qnzkna::SensorManage::ISensorManageService             *pISensorManageService          = IXyhcActivator::Get().getPISensorManageService();
    qnzkna::framework::IRegistry                           *pIRegistry                     = IXyhcActivator::Get().getIRegistryImpl();

    if (!pSystemMaintenanceService || !pIPositionManageService || !pISensorManageService || !pIRegistry)
    {
        QString waringText = (pSystemMaintenanceService == nullptr ? tr("ISystemMaintenanceService is nullptr") : "");
        waringText += (pIPositionManageService == nullptr ? tr("IPositionManageService is nullptr") : "");
        waringText += (pISensorManageService   == nullptr ? tr("ISensorManageService is nullptr") : "");
        waringText += (pIRegistry              == nullptr ? tr("pIRegistry is nullptr") : "");
        waringText += tr("Instruction not send");
        LyMessageBox::warning(nullptr, tr("Tips"), waringText, QMessageBox::Yes);
        return ;
    }

    if (!strComponentID.empty())
    {
        qnzkna::SystemMaintenance::ISystemMaintenanceService   *pSystemMaintenanceService      = IXyhcActivator::Get().getISystemMaintenanceService();
        qnzkna::PositionManage::IPositionManageService         *pIPositionManageService        = IXyhcActivator::Get().getPIPositionManageService();
        qnzkna::SensorManage::ISensorManageService             *pISensorManageService          = IXyhcActivator::Get().getPISensorManageService();
        qnzkna::framework::IRegistry                           *pIRegistry                     = IXyhcActivator::Get().getIRegistryImpl();

        if (!pSystemMaintenanceService || !pIPositionManageService || !pISensorManageService || !pIRegistry)
        {
            QString waringText = (pSystemMaintenanceService == nullptr ? tr("ISystemMaintenanceService is nullptr") : "");
            waringText += (pIPositionManageService == nullptr ? tr("IPositionManageService is nullptr") : "");
            waringText += (pISensorManageService   == nullptr ? tr("ISensorManageService is nullptr") : "");
            waringText += (pIRegistry              == nullptr ? tr("pIRegistry is nullptr") : "");
            waringText += tr("Instruction not send");
            LyMessageBox::warning(nullptr, tr("Tips"), waringText, QMessageBox::Yes);
            return ;
        }

        qnzkna::NodeManage::NODE_TYPE_UINT      nNodeType = NODE_TYPE_CMDUNIT;
        std::string                             strNodeId;
        if (!pSystemMaintenanceService->GetNodeInfo(&nNodeType, &strNodeId))
        {
            LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetNodeInfo is false"), QMessageBox::Yes);
            return ;
        }

        XYWeaponIDSetting xyWeaponIDSetting2;
        if (!m_CXYWeaponManage.GetXYWeaponIDSetting(strComponentID, xyWeaponIDSetting2))
        {
            LyMessageBox::warning(nullptr, tr("Tips"),  tr("XYWeaponManage GetXYWeaponIDSetting is false"), QMessageBox::Yes);
            return ;
        }

        qnzkna::WeaponManage::MultiWeaponComponentMap::const_iterator iter = m_MultiWeaponComponentMap.find(strComponentID);
        if (iter == m_MultiWeaponComponentMap.end())
        {
            LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetWeaponComponent is nullptr"), QMessageBox::Yes);
            return ;
        }

        if (strComponentID != iter->second.getComponentID())
        {

            LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(iter->second.getComponentID()) + tr(", SystemMaintenanceService UAV configure error, command not send!"), QMessageBox::Yes);
            return ;
        }
        const std::string &strComponentName = iter->second.getComponentName();
        const std::string &strProtocolType  = iter->second.getProtocolType();

        const XYWeaponIDSetting &xyWeaponIDSetting     = xyWeaponIDSetting2;
        if (strComponentID != xyWeaponIDSetting.strWeaponID)
        {

            LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(xyWeaponIDSetting.strWeaponID) + tr(", XYWeaponIDSetting UAV configure error, command not send!"), QMessageBox::Yes);
            return ;
        }

        xygsJL_07 xygsJL_07_bag;
        xygsJL_07_bag.flags = 7;
        xygsJL_07_bag.param.type             = 0x00;
        xygsJL_07_bag.param.position.lng     = longitudeFlyGoto;
        xygsJL_07_bag.param.position.lat     = latitudeFlyGoto;
        xygsJL_07_bag.param.position.heigth  = altitudeFlyGoto;
        xygsJL_07_bag.param.radius           = radiusFlyGoto;
        xygsJL_07_bag.param.time_laps        = timeFlyGoto;
        xygsJL_07_bag.param.direction        = 0x00;
        xygsJL_07_bag.param.mode             = modeFlyGoto;
        xygsJL_07_bag.param.velocity         = velocityFlyGoto;

        const int nXYUAVGroupID              = strtol(xyWeaponIDSetting.strXYWeaponGroupID.c_str(), nullptr, 16) ;
        const int nXYUAVID                   = strtol(xyWeaponIDSetting.strXYWeaponID.c_str(), nullptr, 16);

        xygsJL_07_bag.groupIndex             = nXYUAVGroupID;
        xygsJL_07_bag.planeIndex             = nXYUAVID;

        SendMessageBuffer((const char *)&xygsJL_07_bag, sizeof(xygsJL_07_bag), nNodeType, strComponentName, strNodeId, xyWeaponIDSetting.strChannelID, strComponentID, strProtocolType);
    }
}

void UAVNavigationArmShotAutoAiming::SendMessageBuffer(const char *const message, const unsigned int msglength, qnzkna::NodeManage::NODE_TYPE_UINT nNodeType, const std::string &strName, const std::string &strNodeId, const std::string &strChId, const std::string &strComponentID, const std::string &strProtocolType)
{
    Q_UNUSED(strName)

    if (message == nullptr || msglength <= 0)
    {
        return ;
    }

    if (strComponentID == "" || nNodeType <= 0)
    {
        return ;
    }

    if (IXyhcActivator::Get().getIRegistryImpl() == nullptr)
    {
        return ;
    }

    if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY) == strProtocolType || std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54) == strProtocolType)
    {

#ifdef LY_UAVFLYCONTROL_SERVER_CLINET_RECV_SEND
        if (LY_UAVFlyControl::Get().getNetServerClient())
        {
            LY_UAVFlyControl::Get().NetServerClientSend((const char *const)&bag, sizeof(bag), QString::fromStdString(strComponentID));
        }
        else
#endif
        {
            qnzkna::framework::MessageReferenceProps  props;
            props.put("SourceType",   std::to_string(nNodeType));
            props.put("SourceID",     strNodeId);
            props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
            props.put("TargetID",     strChId);
            props.put("WeaponID",     strComponentID);
            props.put("ProtocolType", strProtocolType);
            IXyhcActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", message, msglength, props));
        }
    }
    else if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK) == strProtocolType)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("PX4 MavLink Temporarily unsupported, Instruction not send"), QMessageBox::Yes);

    }
    else if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI) == strProtocolType)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("DJi Temporarily unsupported, Instruction not send!"), QMessageBox::Yes);

    }
    else
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Dronoe The protocol type is not set or Temporarily unsupported, Instruction not send?"), QMessageBox::Yes);
    }
}

void UAVNavigationArmShotAutoAiming::SendFlyCmd(unsigned char nCmd, unsigned char nExtParam, const std::string &strComponentID) const
{
    qnzkna::SystemMaintenance::ISystemMaintenanceService   *pSystemMaintenanceService      = IXyhcActivator::Get().getISystemMaintenanceService();
    qnzkna::PositionManage::IPositionManageService         *pIPositionManageService        = IXyhcActivator::Get().getPIPositionManageService();
    qnzkna::SensorManage::ISensorManageService             *pISensorManageService          = IXyhcActivator::Get().getPISensorManageService();
    qnzkna::framework::IRegistry                           *pIRegistry                     = IXyhcActivator::Get().getIRegistryImpl();

    if (!pSystemMaintenanceService || !pIPositionManageService || !pISensorManageService || !pIRegistry)
    {
        QString waringText = (pSystemMaintenanceService == nullptr ? tr("ISystemMaintenanceService is nullptr") : "");
        waringText += (pIPositionManageService == nullptr ? tr("IPositionManageService is nullptr") : "");
        waringText += (pISensorManageService   == nullptr ? tr("ISensorManageService is nullptr") : "");
        waringText += (pIRegistry              == nullptr ? tr("pIRegistry is nullptr") : "");
        waringText += tr("Instruction not send");
        LyMessageBox::warning(nullptr, tr("Tips"), waringText, QMessageBox::Yes);
        return ;
    }

    qnzkna::NodeManage::NODE_TYPE_UINT      nNodeType = NODE_TYPE_CMDUNIT;
    std::string                             strNodeId;
    if (!pSystemMaintenanceService->GetNodeInfo(&nNodeType, &strNodeId))
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetNodeInfo is false"), QMessageBox::Yes);
        return ;
    }

    XYWeaponIDSetting xyWeaponIDSetting2;
    if (!m_CXYWeaponManage.GetXYWeaponIDSetting(strComponentID, xyWeaponIDSetting2))
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("XYWeaponManage GetXYWeaponIDSetting is false"), QMessageBox::Yes);
        return ;
    }

    qnzkna::WeaponManage::MultiWeaponComponentMap::const_iterator iter = m_MultiWeaponComponentMap.find(strComponentID);
    if (iter == m_MultiWeaponComponentMap.end())
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetWeaponComponent is nullptr"), QMessageBox::Yes);
        return ;
    }

    if (strComponentID != iter->second.getComponentID())
    {

        LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(iter->second.getComponentID()) + tr(", SystemMaintenanceService UAV configure error, command not send!"), QMessageBox::Yes);
        return ;
    }
    const std::string &strComponentName = iter->second.getComponentName();
    const std::string &strProtocolType  = iter->second.getProtocolType();

    const XYWeaponIDSetting &xyWeaponIDSetting     = xyWeaponIDSetting2;
    if (strComponentID != xyWeaponIDSetting.strWeaponID)
    {

        LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(xyWeaponIDSetting.strWeaponID) + tr(", XYWeaponIDSetting UAV configure error, command not send!"), QMessageBox::Yes);
        return ;
    }

    xygsJL_17 bag;
    bag.flags = 17;
    bag.param.comandMain = 0xF3;
    bag.param.comandSub  = nCmd;
    bag.param.comandExt  = nExtParam;

    const int nXYUAVGroupID = strtol(xyWeaponIDSetting.strXYWeaponGroupID.c_str(), nullptr, 16) ;
    const int nXYUAVID      = strtol(xyWeaponIDSetting.strXYWeaponID.c_str(), nullptr, 16);

    bag.groupIndex = nXYUAVGroupID;
    bag.planeIndex = nXYUAVID;
    SendFlyCmdMsg(bag, nNodeType, strComponentName, strNodeId, xyWeaponIDSetting.strChannelID, strComponentID, strProtocolType);
}

void UAVNavigationArmShotAutoAiming::SendFlyCmdStationStick(const xygsJL_15 &bag, const std::string &strComponentID) const
{
    qnzkna::SystemMaintenance::ISystemMaintenanceService   *pSystemMaintenanceService      = IXyhcActivator::Get().getISystemMaintenanceService();
    qnzkna::PositionManage::IPositionManageService         *pIPositionManageService        = IXyhcActivator::Get().getPIPositionManageService();
    qnzkna::SensorManage::ISensorManageService             *pISensorManageService          = IXyhcActivator::Get().getPISensorManageService();
    qnzkna::framework::IRegistry                           *pIRegistry                     = IXyhcActivator::Get().getIRegistryImpl();

    if (!pSystemMaintenanceService || !pIPositionManageService || !pISensorManageService || !pIRegistry)
    {
        QString waringText = (pSystemMaintenanceService == nullptr ? tr("ISystemMaintenanceService is nullptr") : "");
        waringText += (pIPositionManageService == nullptr ? tr("IPositionManageService is nullptr") : "");
        waringText += (pISensorManageService   == nullptr ? tr("ISensorManageService is nullptr") : "");
        waringText += (pIRegistry              == nullptr ? tr("pIRegistry is nullptr") : "");
        waringText += tr("Instruction not send");
        LyMessageBox::warning(nullptr, tr("Tips"), waringText, QMessageBox::Yes);
        return ;
    }

    qnzkna::NodeManage::NODE_TYPE_UINT      nNodeType = NODE_TYPE_CMDUNIT;
    std::string                             strNodeId;
    if (!pSystemMaintenanceService->GetNodeInfo(&nNodeType, &strNodeId))
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetNodeInfo is false"), QMessageBox::Yes);
        return ;
    }

    XYWeaponIDSetting xyWeaponIDSetting2;
    if (!m_CXYWeaponManage.GetXYWeaponIDSetting(strComponentID, xyWeaponIDSetting2))
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("XYWeaponManage GetXYWeaponIDSetting is false"), QMessageBox::Yes);
        return ;
    }

    qnzkna::WeaponManage::MultiWeaponComponentMap::const_iterator iter = m_MultiWeaponComponentMap.find(strComponentID);
    if (iter == m_MultiWeaponComponentMap.end())
    {
        LyMessageBox::warning(nullptr, tr("Tips"),  tr("SystemMaintenanceService GetWeaponComponent is nullptr"), QMessageBox::Yes);
        return ;
    }

    if (strComponentID != iter->second.getComponentID())
    {

        LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(iter->second.getComponentID()) + tr(", SystemMaintenanceService UAV configure error, command not send!"), QMessageBox::Yes);
        return ;
    }
    const std::string &strComponentName = iter->second.getComponentName();
    const std::string &strProtocolType  = iter->second.getProtocolType();

    const XYWeaponIDSetting &xyWeaponIDSetting     = xyWeaponIDSetting2;
    if (strComponentID != xyWeaponIDSetting.strWeaponID)
    {

        LyMessageBox::warning(nullptr, tr("Tips"), QString::fromStdString(strComponentID) + " != " + QString::fromStdString(xyWeaponIDSetting.strWeaponID) + tr(", XYWeaponIDSetting UAV configure error, command not send!"), QMessageBox::Yes);
        return ;
    }

    xygsJL_15 bag15;
    bag15       = bag;
    bag15.flags = 15;

    const int nXYUAVGroupID = strtol(xyWeaponIDSetting.strXYWeaponGroupID.c_str(), nullptr, 16) ;
    const int nXYUAVID      = strtol(xyWeaponIDSetting.strXYWeaponID.c_str(), nullptr, 16);

    bag15.groupIndex = nXYUAVGroupID;
    bag15.planeIndex = nXYUAVID;
    SendFlyCmdStationStickMsg(bag15, nNodeType, strComponentName, strNodeId, xyWeaponIDSetting.strChannelID, strComponentID, strProtocolType);
}

void UAVNavigationArmShotAutoAiming::SendFlyCmdMsg(const xygsJL_17 &bag, qnzkna::NodeManage::NODE_TYPE_UINT nNodeType, const std::string &strName, const std::string &strNodeId, const std::string &strChId, const std::string &strComponentID, const std::string &strProtocolType) const
{
    Q_UNUSED(strName)

    if (strComponentID == "" || nNodeType <= 0)
    {
        return ;
    }

    if (IXyhcActivator::Get().getIRegistryImpl() == nullptr)
    {
        return ;
    }

    if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY) == strProtocolType || std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54) == strProtocolType)
    {

#ifdef LY_UAVFLYCONTROL_SERVER_CLINET_RECV_SEND
        if (LY_UAVFlyControl::Get().getNetServerClient())
        {
            LY_UAVFlyControl::Get().NetServerClientSend((const char *const)&bag, sizeof(bag), QString::fromStdString(strComponentID));
        }
        else
#endif
        {
            qnzkna::framework::MessageReferenceProps  props;
            props.put("SourceType",   std::to_string(nNodeType));
            props.put("SourceID",     strNodeId);
            props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
            props.put("TargetID",     strChId);
            props.put("WeaponID",     strComponentID);
            props.put("ProtocolType", strProtocolType);
            IXyhcActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", (const char *)&bag, sizeof(bag), props));
        }
    }
    else if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK) == strProtocolType)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("PX4 MavLink Temporarily unsupported, Instruction not send"), QMessageBox::Yes);
    }
    else if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI) == strProtocolType)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("DJi Temporarily unsupported, Instruction not send!"), QMessageBox::Yes);
    }
    else
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Dronoe The protocol type is not set or Temporarily unsupported, Instruction not send?"), QMessageBox::Yes);
    }
}

void UAVNavigationArmShotAutoAiming::SendFlyCmdStationStickMsg(const xygsJL_15 &bag, NODE_TYPE_UINT nNodeType, const std::string &strName, const std::string &strNodeId, const std::string &strChId, const std::string &strComponentID, const std::string &strProtocolType) const
{
    Q_UNUSED(strName)

    if (strComponentID == "" || nNodeType <= 0)
    {
        return ;
    }

    if (IXyhcActivator::Get().getIRegistryImpl() == nullptr)
    {
        return ;
    }

    if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY) == strProtocolType || std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_XY_54) == strProtocolType)
    {

#ifdef LY_UAVFLYCONTROL_SERVER_CLINET_RECV_SEND
        if (LY_UAVFlyControl::Get().getNetServerClient())
        {
            LY_UAVFlyControl::Get().NetServerClientSend((const char *const)&bag, sizeof(bag), QString::fromStdString(strComponentID));
        }
        else
#endif
        {
            qnzkna::framework::MessageReferenceProps  props;
            props.put("SourceType",   std::to_string(nNodeType));
            props.put("SourceID",     strNodeId);
            props.put("TargetType",   std::to_string(NODE_TYPE_WEAPONCOMPONENT));
            props.put("TargetID",     strChId);
            props.put("WeaponID",     strComponentID);
            props.put("ProtocolType", strProtocolType);
            IXyhcActivator::Get().getIRegistryImpl()->messageNotify(new qnzkna::framework::MessageReference("MsgUAVFlyControlSend", (const char *)&bag, sizeof(bag), props));
        }
    }
    else if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_MAVLINK) == strProtocolType)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("PX4 MavLink Temporarily unsupported, Instruction not send"), QMessageBox::Yes);
    }
    else if (std::string(WEAPONSYSTEM_VEHICLE_PROTOCOLTYPE_DJI) == strProtocolType)
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("DJi Temporarily unsupported, Instruction not send!"), QMessageBox::Yes);
    }
    else
    {
        LyMessageBox::warning(nullptr, tr("Tips"), tr("Dronoe The protocol type is not set or Temporarily unsupported, Instruction not send?"), QMessageBox::Yes);
    }
}

